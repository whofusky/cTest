#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "WriteLog.h"

#include "InfraBase.h"
#include "CustomOutLog.h"
#include "my_test_sqlite3.h"

int str_int(const char*)
{
    return 0;
}


int main(int argc, char **argv) {

    
    //sqlite3 *db = NULL;
    int retCode = SQLITE_OK;

    //打印sqlite返回码与返回信息
    //testprint_sqlite_codemsg();

    //retCode = test_func1();
    //c_write_log(_DEBUG,"test_func1 return[%d]",retCode);

    //retCode = test_sqlite_exe_callback();
    //c_write_log(_DEBUG,"test_sqlite_exe_callback return[%d]",retCode);

    //retCode = test_get_printsql_data();
    //c_write_log(_DEBUG,"test_get_printsql_data return[%d]",retCode);

    C_MutexSqlite3::test_sqlite_exe_callback();


    return 0;
}
