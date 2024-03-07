#include <stdio.h>
#include <string.h>
#include "WriteLog.h"


#include <unistd.h>
#include <string>
#include <stdlib.h>
#include <sys/syscall.h>
#include <uchardet/uchardet.h>
#include "json/json.h"



#include "InfraBase.h"
#include "CustomOutLog.h"



int main(int argc, char **argv) {

    c_write_log(_INFO,"argc=[%d]", argc);

    if ( argc != 2 )
    {
        c_write_log(_ERROR,"please input like:%s <file_name>", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    c_write_log(_INFO,"filename=[%s]",filename);
    FILE *file = fopen(filename, "rb");
    if (!file) {
        c_write_log(_ERROR,"Could not open file[%s]", filename);
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char *buffer = (unsigned char *)malloc(file_size);
    if (!buffer) {
        c_write_log(_ERROR,"Memory allocation failed");
        fclose(file);
        return 1;
    }

    size_t bytes_read = fread(buffer, 1, file_size, file);
    fclose(file);

    c_write_log(_INFO,"read the entire file.bytes_read=[%ld],file_size=[%ld]",bytes_read,file_size);
    if (bytes_read != file_size) {
        c_write_log(_ERROR,"Failed to read the entire file.bytes_read=[%ld],file_size=[%ld]",bytes_read,file_size);
        free(buffer);
        return 1;
    }

    uchardet_t handle = uchardet_new();
    if (!handle) {
        c_write_log(_ERROR,"Failed to create uchardet handle.");
        free(buffer);
        return 1;
    }

    c_write_log(_INFO,"begine uchardet_handle_data");
    int retval = uchardet_handle_data(handle, (const char *)buffer, bytes_read);
    c_write_log(_INFO,"end uchardet_handle_data,retval=[%d]",retval);
    if (retval != 0)
    {
        c_write_log(_ERROR, "uchardet-tests: handle data error.");
        return 1;
    }
    c_write_log(_INFO,"begine uchardet_data_end");
    uchardet_data_end(handle);
    c_write_log(_INFO,"end uchardet_data_end");

    //const char *detected_encoding = strdup(uchardet_get_encoding(handle, 0));
    //const char *detected_encoding = uchardet_get_encoding(handle, 0);
    const char *detected_encoding = uchardet_get_charset(handle);

    c_write_log(_INFO,"Detected encoding: [%s]", detected_encoding);

    uchardet_delete(handle);
    free(buffer);


    return 0;
}
