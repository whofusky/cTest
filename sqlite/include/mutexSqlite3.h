/****************************************
 * 
 *    mutexSqlite3.h
 * 
 *    Create by fushikai
 * 
 *    2023-08-31
 * 
 ****************************************/
#ifndef _CMUTEX_SQLITE3_H_
#define _CMUTEX_SQLITE3_H_

#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"
#include "CustomOutLog.h"




// Less than 0 is the return value of the custom function
// If it is greater than or equal to 0, the sqlite3 interface returns the value
//
#define  MY_SQLITE_ERR_IN_NULL   -1     /*The function input variable is NULL*/
#define  MY_SQLITE_ERR_ALLOC     -2     /*Allocate space returns NULL*/




//The structure returned by sqlite3_get_table
typedef struct{
    /*The number of rows of data (excluding titles) in the result set, 0: title row; 1-rows is data rows; when there is no data, rows is 0*/
    int rows;         
    int cols;         /*Number of columns in result set*/
    /*Result set: The value of row i and column j is result[i*cols+j]; it should be released using sqlite3_free_table(result)*/
    char **result;      
}T_sqlRcdRet;
//Note: When returning a data result set: that is, when rows>0; there are so many string members in the result (rows+1)*cols,
//     Line 0 is the title field: data starts from line 1; therefore, the last field value should be: result[rows*cols +(cols-1)]




////sqlite3_exec callback function pointer
//typedef int (*sqlite3_callback)(void* data,int ncols,char** values, char** headers);
//  This function is called once for each row of data
//   data:      The 4th parameter of sqlite3_exec or the 4th parameter of sqlexec da
//   ncols      Indicates how many columns of data currently exist
//   values[i]  Represents the value of column i 0=< i < ncols
//   headers[i] Indicates the field name of column i in the current query table 0=< i < ncols
//
//Note: The return value of this callback function affects the execution of sqlite3_exec
//      Returning 0 does not affect sqlite3_exec
//      If non-zero is returned, sqlite3_exec will terminate.







class C_MutexSqlite3
{
    public:


    //Open sqlite3 file
    // return: SQLITE_OK success
    //         <0  Parameter error
    //         >0  sqlite3 returns error
    static int open_v2( 
            const char *filename = NULL,   /* Database filename (UTF-8) */ 
            int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,              /* Flags */ 
            const char *zVfs = NULL        /* Name of VFS module to use */
    );

    //call: sqlite3_close(sqlite3*),close SQLite db handle
    static void close_db();




    /*to call sqlite3_exec*/
    //Execute non-query sql statements
    //        sqlexec(db, sql, NULL, NULL, NULL)
    //Execute the sql statement of the query class (requires custom implementation of the callback function fu)
    //        sqlexec(db, sql, fu, da, NULL)
    //  (For the description of the callback function, see the sqlite3_callback comment in the previous part of this file)
    //errmsg Generally NULL because error return information can be obtained through sqlexec return code
    static int sqlexec(
      const char *sql,        /* SQL to be evaluated */
      sqlite3_callback fu,    /* Callback function */
      void * da,              /* 1st argument to callback */
      char **errmsg           /* Error msg written here */
    );




    //Execute non-query variable-length sql statements
    //    sqlite3_exec(db, tmpSql, NULL, NULL, NULL);
    // return: SQLITE_OK success
    //         <0  Parameter error
    //         >0  sqlite3 returns error
    static int exe_prt_sql( const char* sql, ...);




    //call sqlite3_get_table
    // T_sqlRcdRet *da
    //
    //  get_table( db, zSql, &(da->result), &(da->rows), &(da->cols), NULL)
    //
    // sqlite3_free_table(da->result)
    // 
    static int get_table(
      const char *zSql,     /* SQL to be evaluated */
      char ***pazResult,    /* Results of the query */
      int *pnRow,           /* Number of result rows written here */
      int *pnColumn,        /* Number of result columns written here */
      char **pzErrmsg       /* Error msg written here */
    );




    //Execute query type sql statement and return the results
    //  sqlite3_get_table
    // return: SQLITE_OK success
    //         <0  Parameter error
    //         >0  sqlite3 returns error
    // Note: retData->result needs to call sqlite3_free_table(retData->result) after use
    static int get_prtsql_data(T_sqlRcdRet *retData, const char* sql, ...);




    //Print T_sqlRcdRet data content
    static void prtSqlRcdRet( T_sqlRcdRet * p);




    /* Drops a table if it exists. */
    // return: SQLITE_OK success
    //         <0  Parameter error
    //         >0  sqlite3 returns error
    static int drop_table( const char* name );

    //Get the count value of count(*) sql
    static int get_count(const char *sql, int &oNum);

    //Determine whether table table_nam exists
    static bool exist_table(const char *table_name);



    //Execute sql statements that meet the conditions in the same transaction
    //  Execute count_sql to determine whether there is data. If there is no data, do not continue to execute other statements.
    //If there is data：
    //  (1) When oCount is not NULL, return count_sql query results
    //  (2) When query_sql and retData are not NULL, return query_sql query results to retData
    //  (3) Execute the modify_sql statement when modify_sql is not NULL 
    static int ExeIfExist(int *oCount,
            const char *count_sql,
            const char *query_sql,
            T_sqlRcdRet *retData,
            const char *modify_sql
            );


    //Test the callback function of sqlite3_exec
    static void test_sqlite_exe_callback();



    private:
        C_MutexSqlite3();
        ~C_MutexSqlite3();

};


#endif//_CMUTEX_SQLITE3_H_
