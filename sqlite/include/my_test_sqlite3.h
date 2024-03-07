/****************************************
 * 
 *    my_test_sqlite3.h
 * 
 *    Create by fushikai
 * 
 *    2023-07-14
 * 
 ****************************************/
#ifndef MY_TEST_SQLITE3_H
#define MY_TEST_SQLITE3_H

#include "mutexSqlite3.h"







/**********************测试函数 begine**************************************/
/*test print sqlite return value's msg*/
void testprint_sqlite_codemsg();

//测试函数1
int test_func1();
int test_sqlite_exe_callback();

//测试get_printsql_data (即:sqlite3_get_table)的返回结果及结果构成
int test_get_printsql_data();

/**********************测试函数 end**************************************/



#endif //MY_TEST_SQLITE3_H
