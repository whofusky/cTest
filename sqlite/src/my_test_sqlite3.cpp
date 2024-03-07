/***********************************************************
 *
 * @file    my_test_sqlite3.c
 *
 * @brief   my_test_sqlite3.c源文件
 *
 * @author  
 *
 * @date    2023-07-14
 *
 * @version V10.010.000
 *
 ***********************************************************/

#include "my_test_sqlite3.h"


void testprint_sqlite_codemsg()
{
    //SQLITE_BUSY, SQLITE_DONE, SQLITE_ROW, SQLITE_ERROR, or SQLITE_MISUSE
    b_write_log(_DEBUG,"SQLITE_BUSY:[%d][%s]",SQLITE_BUSY,sqlite3_errstr(SQLITE_BUSY));
    b_write_log(_DEBUG,"SQLITE_DONE:[%d][%s]",SQLITE_DONE,sqlite3_errstr(SQLITE_DONE));
    b_write_log(_DEBUG,"SQLITE_ROW:[%d][%s]",SQLITE_ROW,sqlite3_errstr(SQLITE_ROW));
    b_write_log(_DEBUG,"SQLITE_ERROR:[%d][%s]",SQLITE_ERROR,sqlite3_errstr(SQLITE_ERROR));
    b_write_log(_DEBUG,"SQLITE_MISUSE:[%d][%s]",SQLITE_MISUSE,sqlite3_errstr(SQLITE_MISUSE));
    return;
}



/*********************测试函数区域 begine**********************************************/


//测试函数1: 测试创建表及insert 语句, 主要是为了测试 
// C_MutexSqlite3::drop_table 和 C_MutexSqlite3::exe_prt_sql 接口
int test_func1()
{
    int retCode = SQLITE_OK;

    retCode = C_MutexSqlite3::open_v2("test.db");
    if(retCode != SQLITE_OK) return retCode;

    
    C_MutexSqlite3::drop_table("episodes");
    retCode = C_MutexSqlite3::exe_prt_sql("create table episodes(id int, name text)");
    if(retCode != SQLITE_OK) return retCode;
    retCode = C_MutexSqlite3::exe_prt_sql("insert into episodes values (10, 'The Dinner Party')");
    if(retCode != SQLITE_OK) return retCode;
    
    C_MutexSqlite3::close_db();
    return 0;
}



// sqlite3_exec 回调测试函数
//注意: 此回调函数返回值影响sqlite3_exec的执行
//      返回0 则不影响sqlite3_exec
//      返回非0 则sqlite3_exec将终止
//  
typedef struct{
    int line;
    const char* tips;
}T_testData;

int test_exe_callback(void* data, int ncols, char** values, char** headers)
{
    int i;
    T_testData* tData = (T_testData*) data;
    b_write_log(_INFO, "[%s]第[%d]行数据", tData->tips, tData->line );
    for(i=0; i < ncols; i++) {
        b_write_log(_INFO, "col[%d]:[%s=%s]", i, headers[i], values[i]);
    }
    tData->line++;
    return 0;
}

//Test the callback function of sqlite3_exec
int test_sqlite_exe_callback()
{
    int retCode = SQLITE_OK;

    T_testData tzData={0,"test data"};

    retCode = C_MutexSqlite3::open_v2("test.db");
    if(retCode != SQLITE_OK) return retCode;

    
    C_MutexSqlite3::drop_table(  "episodes");
    retCode = C_MutexSqlite3::exe_prt_sql("create table episodes(id int, name text)");
    if(retCode != SQLITE_OK) {
        c_write_log(_ERROR,"C_MutexSqlite3::exe_prt_sql return[%d]", retCode);
        return retCode;
    }
    //retCode = C_MutexSqlite3::exe_prt_sql("insert into episodes values (10, 'The Dinner Party')");
    //if(retCode != SQLITE_OK) {
    //    c_write_log(_ERROR,"C_MutexSqlite3::exe_prt_sql return[%d]", retCode);
    //    return retCode;
    //}

    const char* sql = "insert into episodes (id, name) values (10,'Mackinaw Peaches');"
          "insert into episodes (id, name) values (11,'fu sky');"
          "select * from episodes;";

    //const char* sql = "select count(*) as a  from episodes;";
    retCode = C_MutexSqlite3::sqlexec( sql, test_exe_callback, &tzData, NULL);

    b_write_log(_DEBUG,"C_MutexSqlite3::sqlexec return[%d][%s]",retCode, sqlite3_errstr(retCode) );
    
    C_MutexSqlite3::close_db();
    return retCode;
}


//生成episodes的测试数据
int test_gen_episodes_data()
{
    int retCode = 0;
    C_MutexSqlite3::drop_table(  "episodes");

    //retCode = C_MutexSqlite3::exe_prt_sql("create table episodes(id int, name text);"
    //        );
    //if (retCode != SQLITE_OK )  return retCode;

    retCode = C_MutexSqlite3::exe_prt_sql("create table episodes(id int, name text);"
            "insert into episodes values (10, 'The Dinner Party');"
            "insert into episodes (id, name) values (11,'Mackinaw Peaches')"
            );


    //retCode = C_MutexSqlite3::exe_prt_sql("create table episodes(id int, name text);"
    //        "insert into episodes values (10, 'The Dinner Party');"
    //        );

    return retCode;
}



//测试C_MutexSqlite3::get_prtsql_data (即:sqlite3_get_table)的返回结果及结果构成
int test_get_printsql_data()
{
    int retCode = SQLITE_OK;

    //打开数据库文件(文件不存在则自动创建)
    retCode = C_MutexSqlite3::open_v2("test.db");
    if(retCode != SQLITE_OK) return retCode;

    //先生成episodes表的测试数据
    retCode = test_gen_episodes_data();
    if(retCode != SQLITE_OK) return retCode;

    T_sqlRcdRet retData ={ 0, 0, NULL };
    //retCode = C_MutexSqlite3::get_prtsql_data( &retData,  
    //        "select * from episodes;"
    //        );
    const char* tsql = "select * from episodes;";
    retCode = C_MutexSqlite3::get_table( tsql, 
            &retData.result, &retData.rows, &retData.cols,
            NULL );
    if(retCode != SQLITE_OK){ 
        sqlite3_free_table(retData.result); //释放空间
        return retCode;
    }

    C_MutexSqlite3::prtSqlRcdRet( &retData );

    sqlite3_free_table(retData.result); //释放空间
    C_MutexSqlite3::close_db();
    return 0;
}


/*********************测试函数区域 end**********************************************/




