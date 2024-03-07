#include <stdio.h>
#include <string.h>
#include "WriteLog.h"


#include <unistd.h>
#include <string>
#include <stdlib.h>
#include <sys/syscall.h>
#include "json/json.h"
#include "magic.h"



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
    magic_t magic_cookie;

    // 初始化 magic cookie
    //magic_cookie = magic_open(MAGIC_MIME_TYPE);
    //magic_cookie = magic_open(MAGIC_MIME_ENCODING);
    magic_cookie = magic_open(MAGIC_MIME);
    if (magic_cookie == NULL) {
        c_write_log(_ERROR, "Unable to initialize magic library.");
        return 1;
    }

    // 加载默认数据库
    if (magic_load(magic_cookie, NULL) != 0) {
        c_write_log(_ERROR,"Cannot load magic database: %s", magic_error(magic_cookie));
        magic_close(magic_cookie);
        return 1;
    }

    // 获取文件类型
    const char *file_type = magic_file(magic_cookie, filename);
    if (file_type == NULL) {
        c_write_log(_ERROR,"Failed to detect MIME type of file [%s],errmsg=[%s]", filename,magic_error(magic_cookie));
        magic_close(magic_cookie);
        return 1;
    }

    c_write_log(_INFO,"Detected MIME type: [%s]", file_type);

    // 关闭 magic cookie
    magic_close(magic_cookie);


    return 0;
}
