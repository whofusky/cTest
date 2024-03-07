
#include <pthread.h>
#include <string.h>
#include "mutexSqlite3.h"



static pthread_mutex_t pgsLock = PTHREAD_MUTEX_INITIALIZER;

static sqlite3 *g_db = NULL;  //SQLite db handle



//Variable sql length example
//{
//    //char *sql = NULL;
//    //sql = sqlite3_mprintf("select count(*) from sqlite_master where name='%s'", name);
//    //if ( NULL == sql ){
//    //    b_write_log(_ERROR,
//    //            "sqlite3_mprintf(select count(*) from sqlite_master where name='%s',return NULL", name);
//    //    return MY_SQLITE_ERR_ALLOC;
//    //}
//    //sqlite3_free(sql);
//}




//Print T_sqlRcdRet data content
static void printSqlRcdRet(T_sqlRcdRet * p)
{
    if ( NULL == p ) return;

    if ( p->rows < 1 ) return;

    b_write_log(_INFO,"rows=[%d],cols=[%d]", p->rows, p->cols );

    char tmp_prt_buf[1024];
    int fixlen = 1024;
    int ret, blen;

    //Note that i below requires =rows to get the last row of data.
    // Line 0 is the title
    for( int i = 0; i<= p->rows; i++) {

        memset(tmp_prt_buf,0,fixlen);
        ret = 0; blen = 0;

        for( int j = 0; j< p->cols; j++) {
            if ( 0 == j ) {
                ret = snprintf(tmp_prt_buf + blen,fixlen - blen,
                        "[%s]",
                        p->result[i*p->cols + j]
                        );
            }
            else {
                ret = snprintf(tmp_prt_buf + blen,fixlen - blen,
                        ",[%s]",
                        p->result[i*p->cols + j]
                        );
            }

            if ( ret < 0 ) {
                b_write_log(_ERROR,"snprintf return[%d]",ret);
                return;
            }
            blen += ret;
            if ( blen >= fixlen ) {
                c_write_log(_ERROR,"max len [%d]",fixlen);
                break;
            }
        }
        if ( 0 == i ) {
            c_write_log(_INFO,"[head     ]:[%s]", tmp_prt_buf );
        } 
        else {
            c_write_log(_INFO,"[row:%5d]:[%s]", i,tmp_prt_buf );
        }
    }
    return;
}








////Execute non-query sql statements
//static int execute_print_sql(sqlite3 *db, const char* sql, ...)
//{
//    if ( NULL == db ) {
//        b_write_log(_ERROR,"execute_print_sql:input var db is NULL!");
//        return MY_SQLITE_ERR_IN_NULL;
//    }
//    if ( NULL == sql ) {
//        b_write_log(_ERROR,"execute_print_sql:input var sql is NULL!");
//        return MY_SQLITE_ERR_IN_NULL;
//    }
//
//	int retCode = SQLITE_OK;
//	char *tmpSql = NULL;
//
//	va_list ap;
//    va_start(ap, sql);
//    tmpSql = sqlite3_vmprintf(sql, ap);
//    va_end(ap);
//    if ( NULL == tmpSql ) {
//        b_write_log(_ERROR, "sqlite3_vmprintf return NULL");
//        return MY_SQLITE_ERR_ALLOC;
//    }
//
//    retCode = sqlite3_exec(db, tmpSql, NULL, NULL, NULL);
//
//    if(retCode != SQLITE_OK) {
//        b_write_log(_ERROR, "sqlite3_exec(%s) return[%d][%s]", 
//                tmpSql, retCode,
//                sqlite3_errstr(retCode) );
//    }
//
//    sqlite3_free(tmpSql);
//    return retCode;
//}




////Execute the sql statement of the query class and return the results
//static int get_printsql_data(T_sqlRcdRet *retData, sqlite3 *db, const char* sql, ...)
//{
//    if ( NULL == retData ) {
//        b_write_log(_ERROR,"input var retData is NULL!");
//        return MY_SQLITE_ERR_IN_NULL;
//    }
//    if ( NULL == db ) {
//        b_write_log(_ERROR,"input var db is NULL!");
//        return MY_SQLITE_ERR_IN_NULL;
//    }
//    if ( NULL == sql ) {
//        b_write_log(_ERROR,"input var sql is NULL!");
//        return MY_SQLITE_ERR_IN_NULL;
//    }
//
//
//	int retCode = SQLITE_OK;
//	char *tmpSql = NULL;
//
//	va_list ap;
//    va_start(ap, sql);
//    tmpSql = sqlite3_vmprintf(sql, ap);
//    va_end(ap);
//    if ( NULL == tmpSql ) {
//        b_write_log(_ERROR, "sqlite3_vmprintf return NULL");
//        return MY_SQLITE_ERR_ALLOC;
//    }
//
//    retCode = sqlite3_get_table(db, 
//            tmpSql, 
//            &(retData->result), 
//            &(retData->rows), 
//            &(retData->cols), 
//            NULL
//            );
//
//    if(retCode != SQLITE_OK) {
//        b_write_log(_ERROR, "sqlite3_get_table(%s) return[%d][%s]", 
//                tmpSql, retCode,
//                sqlite3_errstr(retCode) );
//    }
//
//    sqlite3_free(tmpSql);
//    return retCode;
//}




//static void testprint_sqlite_codemsg( int retCode )
//{
//    b_write_log(_DEBUG,"code:[%d],code_msg:[%s]",
//            retCode,sqlite3_errstr(retCode));
//
//    //SQLITE_BUSY, SQLITE_DONE, SQLITE_ROW, SQLITE_ERROR, or SQLITE_MISUSE
//    //b_write_log(_DEBUG,"SQLITE_BUSY:[%d][%s]",SQLITE_BUSY,sqlite3_errstr(SQLITE_BUSY));
//    //b_write_log(_DEBUG,"SQLITE_DONE:[%d][%s]",SQLITE_DONE,sqlite3_errstr(SQLITE_DONE));
//    //b_write_log(_DEBUG,"SQLITE_ROW:[%d][%s]",SQLITE_ROW,sqlite3_errstr(SQLITE_ROW));
//    //b_write_log(_DEBUG,"SQLITE_ERROR:[%d][%s]",SQLITE_ERROR,sqlite3_errstr(SQLITE_ERROR));
//    //b_write_log(_DEBUG,"SQLITE_MISUSE:[%d][%s]",SQLITE_MISUSE,sqlite3_errstr(SQLITE_MISUSE));
//    return;
//}




static inline int getSqlCount(sqlite3* db,  const char *sql, int &oNum)
{
    oNum = 0;
    sqlite3_stmt *stmt = NULL;
    int retCode = SQLITE_OK;
    retCode = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if(retCode != SQLITE_OK) {
        b_write_log(_ERROR,"sqlite3_prepare_v2(%s) return[%d][%s]",
                sql, retCode, sqlite3_errstr(retCode) );
        return retCode;
    }

    retCode = sqlite3_step(stmt);
    if ( retCode != SQLITE_ROW ) {
        b_write_log(_ERROR,"sqlite3_step return[%d][%s]",retCode,sqlite3_errstr(retCode));
        sqlite3_finalize(stmt);
        return retCode;
    }

    retCode = SQLITE_OK;

    oNum = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return retCode;
}




//Determine whether table table_name exists
static inline bool existTable(sqlite3* db,  const char *name)
{
    char sql[256] = {0};
    int fixlen    = 256;
    int ret = snprintf( sql, fixlen, "select count(*) from sqlite_master where name='%s'", name);
    if ( ret < 0 ) {
        b_write_log(_ERROR,
                "snprintf(select count(*) from sqlite_master where name='%s'),return [%d]", 
                name, ret );
        return false;
    }

    int exists = 0;
    getSqlCount( db, sql, exists);

    if ( exists > 0 ) return true;

    return false;
}




//Drop the table if it exists
// return: SQLITE_OK success
//         <0  Parameter error
//         >0  sqlite3 returns error
static inline int drop_exists_table(sqlite3* db, const char* name)
{
    if ( ! existTable(db, name) ) return 0;

    char sql[256] = {0};
    int fixlen    = 256;

    snprintf( sql, fixlen, "drop table %s", name);

    int retCode = sqlite3_exec(db, sql, NULL, NULL, NULL);
    if(retCode != SQLITE_OK) {
        b_write_log(_ERROR, "sqlite3_exec(%s) return[%d][%s]", 
                sql, retCode,
                sqlite3_errstr(retCode) );
    }

    return retCode;
}








C_MutexSqlite3::C_MutexSqlite3()
{

}

C_MutexSqlite3::~C_MutexSqlite3()
{

}








//Open sqlite3 file
// return: SQLITE_OK success
//         <0  Parameter error
//         >0  sqlite3 returns error
int C_MutexSqlite3::open_v2( 
  const char *filename,   /* Database filename (UTF-8) */
  int flags ,              /* Flags */
  const char *zVfs         /* Name of VFS module to use */
  )
{

    int retCode = SQLITE_OK;

    pthread_mutex_lock( &pgsLock );
        retCode = sqlite3_open_v2(filename, &g_db, flags, zVfs );
    pthread_mutex_unlock( &pgsLock );

    return retCode;
}




void C_MutexSqlite3::close_db()
{
    if ( g_db == NULL ) return;

    pthread_mutex_lock( &pgsLock );
        int retCode = sqlite3_close( g_db );
        c_write_log(_DEBUG,"sqlite3_close,retCode=[%d],retMsg=[%s]",
                retCode,sqlite3_errstr(retCode) );
        g_db = NULL;
    pthread_mutex_unlock( &pgsLock );

    return;
}




/*to call sqlite3_exec*/
int C_MutexSqlite3::sqlexec( 
        const char *sql,                           /* SQL to be evaluated */ 
        sqlite3_callback fu,                       /* Callback function */ 
        void * da,                                 /* 1st argument to callback */ 
        char **errmsg                              /* Error msg written here */
        )
{
    int ret = SQLITE_OK;
    pthread_mutex_lock( &pgsLock );
        ret = sqlite3_exec( g_db, sql, fu, da, errmsg);
    pthread_mutex_unlock( &pgsLock );
    return ret;
}




//Execute non-query variable-length sql statements
//    sqlite3_exec(db, tmpSql, NULL, NULL, NULL);
// return: SQLITE_OK success
//         <0  Parameter error
//         >0  sqlite3 returns error
int C_MutexSqlite3::exe_prt_sql( const char* sql, ...)
{
    if ( NULL == sql ) {
        b_write_log(_ERROR,"exe_prt_sql:input var sql is NULL!");
        return MY_SQLITE_ERR_IN_NULL;
    }

	int retCode = SQLITE_OK;
	char *tmpSql = NULL;

	va_list ap;
    va_start(ap, sql);
    tmpSql = sqlite3_vmprintf(sql, ap);
    va_end(ap);
    if ( NULL == tmpSql ) {
        b_write_log(_ERROR, "sqlite3_vmprintf return NULL");
        return MY_SQLITE_ERR_ALLOC;
    }

    pthread_mutex_lock( &pgsLock );

        retCode = sqlite3_exec(g_db, tmpSql, NULL, NULL, NULL);
        if(retCode != SQLITE_OK) {
            b_write_log(_ERROR, "sqlite3_exec(%s) return[%d][%s]", 
                    tmpSql, retCode,
                    sqlite3_errstr(retCode) );
        }

        sqlite3_free(tmpSql);

    pthread_mutex_unlock( &pgsLock );

    return retCode;
}




//call sqlite3_get_table
// T_sqlRcdRet *da
//
//  get_table( db, zSql, &(da->result), &(da->rows), &(da->cols), NULL)
//
// sqlite3_free_table(da->result)
// 
int C_MutexSqlite3::get_table( 
        const char *zSql,     /* SQL to be evaluated */ 
        char ***pazResult,    /* Results of the query */ 
        int *pnRow,           /* Number of result rows written here */ 
        int *pnColumn,        /* Number of result columns written here */ 
        char **pzErrmsg       /* Error msg written here */
        )
{
	int retCode = SQLITE_OK;

    pthread_mutex_lock( &pgsLock );
        retCode = sqlite3_get_table(g_db, zSql, pazResult, pnRow, pnColumn, pzErrmsg);
    pthread_mutex_unlock( &pgsLock );

    return retCode;
}




//Execute query type sql statement and return the results
//  sqlite3_get_table
// return: SQLITE_OK success
//         <0  Parameter error
//         >0  sqlite3 returns error
// Note: retData->result needs to call sqlite3_free_table(retData->result) after use
int C_MutexSqlite3::get_prtsql_data(T_sqlRcdRet *retData, const char* sql, ...)
{
    if ( NULL == retData ) {
        b_write_log(_ERROR,"input var retData is NULL!");
        return MY_SQLITE_ERR_IN_NULL;
    }
    if ( NULL == sql ) {
        b_write_log(_ERROR,"input var sql is NULL!");
        return MY_SQLITE_ERR_IN_NULL;
    }

	int retCode = SQLITE_OK;
	char *tmpSql = NULL;

	va_list ap;
    va_start(ap, sql);
    tmpSql = sqlite3_vmprintf(sql, ap);
    va_end(ap);
    if ( NULL == tmpSql ) {
        b_write_log(_ERROR, "sqlite3_vmprintf return NULL");
        return MY_SQLITE_ERR_ALLOC;
    }

    //SQLITE_API int sqlite3_get_table(
    //  sqlite3 *db,          /* An open database */
    //  const char *zSql,     /* SQL to be evaluated */
    //  char ***pazResult,    /* Results of the query */
    //  int *pnRow,           /* Number of result rows written here */
    //  int *pnColumn,        /* Number of result columns written here */
    //  char **pzErrmsg       /* Error msg written here */
    //);

    pthread_mutex_lock( &pgsLock );

        retCode = sqlite3_get_table(g_db, 
                tmpSql, 
                &(retData->result), 
                &(retData->rows), 
                &(retData->cols), 
                NULL
                );

        if(retCode != SQLITE_OK) {
            b_write_log(_ERROR, "sqlite3_get_table(%s) return[%d][%s]", 
                    tmpSql, retCode,
                    sqlite3_errstr(retCode) );
        }
        sqlite3_free(tmpSql);

    pthread_mutex_unlock( &pgsLock );

    return retCode;
}




//Print T_sqlRcdRet data content
void C_MutexSqlite3::prtSqlRcdRet( T_sqlRcdRet * p)
{
    return printSqlRcdRet( p );
}




/* Drops a table if it exists. */
// return: SQLITE_OK success
//         <0  Parameter error
//         >0  sqlite3 returns error
int C_MutexSqlite3::drop_table(const char* name)
{
    if ( NULL == name ) {
        b_write_log(_ERROR,"input var name is NULL!");
        return MY_SQLITE_ERR_IN_NULL;
    }

    int retCode =  SQLITE_OK;

    pthread_mutex_lock( &pgsLock );
        retCode = drop_exists_table( g_db, name );
    pthread_mutex_unlock( &pgsLock );

    return retCode;
}




//Get the count value of count(*) sql
int C_MutexSqlite3::get_count(const char *sql, int &oNum)
{
    if ( NULL == sql ) {
        b_write_log(_ERROR,"input var sql is NULL!");
        return MY_SQLITE_ERR_IN_NULL;
    }

    int retCode =  SQLITE_OK;

    pthread_mutex_lock( &pgsLock );
        retCode = getSqlCount( g_db, sql, oNum);
    pthread_mutex_unlock( &pgsLock );

    return retCode;
}




//Determine whether table table_nam exists
bool C_MutexSqlite3::exist_table(const char *table_name)
{
    bool bRet = true;
    pthread_mutex_lock( &pgsLock );
        bRet = existTable(g_db, table_name);
    pthread_mutex_unlock( &pgsLock );
    return bRet;
}




//Execute sql statements that meet the conditions in the same transaction
//  Execute count_sql to determine whether there is data. If there is no data, do not continue to execute other statements.
//If there is data：
//  (1) When oCount is not NULL, return count_sql query results
//  (2) When query_sql and retData are not NULL, return query_sql query results to retData
//  (3) Execute the modify_sql statement when modify_sql is not NULL 
int C_MutexSqlite3::ExeIfExist(int *oCount,
            const char *count_sql,
            const char *query_sql,
            T_sqlRcdRet *retData,
            const char *modify_sql
            )
{
    if ( count_sql == NULL ) return MY_SQLITE_ERR_IN_NULL;

    int retCode = SQLITE_OK;

    pthread_mutex_lock( &pgsLock );
        int tNum = 0;
        retCode = getSqlCount( g_db, count_sql, tNum);
        if ( oCount != NULL ) *oCount = tNum;

        if ( tNum < 1 ) {
            pthread_mutex_unlock( &pgsLock );
            return retCode;
        }

        //get query return data
        if ( query_sql != NULL && retData != NULL ) {
            retCode = sqlite3_get_table(g_db,
                    query_sql,
                    &(retData->result),
                    &(retData->rows),
                    &(retData->cols),
                    NULL
                    );
            if ( retCode != SQLITE_OK ) {
                b_write_log(_ERROR,
                        "sqlite3_get_table(%s) return[%d][%s]",
                        query_sql,retCode,sqlite3_errstr(retCode) );

                pthread_mutex_unlock( &pgsLock );
                return retCode;
            }
        }

        //modify tables
        if ( modify_sql != NULL ) {
            retCode = sqlite3_exec(g_db,modify_sql,NULL,NULL,NULL);
            if ( retCode != SQLITE_OK ) {
                b_write_log(_ERROR,
                        "sqlite3_exec(%s) return[%d][%s]",
                        modify_sql,retCode,sqlite3_errstr(retCode) );

                pthread_mutex_unlock( &pgsLock );
                return retCode;
            }
        }

    pthread_mutex_unlock( &pgsLock );
    return retCode;
}




/************************************************************/
/*              test area                                   */
/************************************************************/



// sqlite3_exec callback test function
//Note: The return value of this callback function affects the execution of sqlite3_exec
//      Returning 0 does not affect sqlite3_exec
//      If non-zero is returned, sqlite3_exec will terminate.

typedef struct{
    int line;
    const char* tips;
}T_testData;

static int test_exe_callback(void* data, int ncols, char** values, char** headers)
{
    T_testData* tData = (T_testData*) data;

    b_write_log(_INFO, "[%s]第[%d]行数据", tData->tips, tData->line );

    for(int i=0; i < ncols; i++) {
        b_write_log(_INFO, "col[%d]:[%s=%s]", i, headers[i], values[i]);
    }

    tData->line++;

    //      Returning 0 does not affect sqlite3_exec
    //      If non-zero is returned, sqlite3_exec will terminate.
    return 0;
}


//Test the callback function of sqlite3_exec
void C_MutexSqlite3::test_sqlite_exe_callback()
{
    int retCode = SQLITE_OK;

    T_testData tzData={0,"tips:[in exe callback]"};

    retCode = C_MutexSqlite3::open_v2("testcallback.db");
    if(retCode != SQLITE_OK) return ;

    
    C_MutexSqlite3::drop_table( "episodes");
    retCode = C_MutexSqlite3::exe_prt_sql("create table episodes(id INTEGER PRIMARY KEY, name text)");
    if(retCode != SQLITE_OK) {
        c_write_log(_ERROR,"C_MutexSqlite3::exe_prt_sql return[%d]", retCode);
        return ;
    }

    const char* sql = "insert into episodes (id, name) values (10,'Mackinaw Peaches');"
          "insert into episodes (id, name) values (11,'fu sky');"
          "insert into episodes (id, name) values (12,'fu sky1');"
          "select * from episodes;";

    retCode = C_MutexSqlite3::sqlexec(sql, test_exe_callback, &tzData, NULL);
    b_write_log(_DEBUG,"C_MutexSqlite3::sqlexec return[%d][%s]",retCode, sqlite3_errstr(retCode) );

    const char* sql1 = "select * from episodes";
    T_sqlRcdRet da;
    retCode = get_table(  sql1, &(da.result), &(da.rows), &(da.cols), NULL);
    b_write_log(_DEBUG,"C_MutexSqlite3::get_table return[%d][%s]",retCode, sqlite3_errstr(retCode) );
    prtSqlRcdRet( &da );
    sqlite3_free_table(da.result);
    
    C_MutexSqlite3::close_db();
    return ;
}





