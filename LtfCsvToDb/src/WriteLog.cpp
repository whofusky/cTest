/****************************************
 * 
 *    WriteLog.cpp 
 * 
 *    Create by fushikai
 * 
 *    2021-06-19
 * 
 ****************************************/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>
#include <errno.h>
#include <cstdlib>
#include <sstream>
#include "WriteLog.h"

#define LIB_BASE_VERSION "[ libBaseLog.a ] Version: V00.01.000"

#define CUR_STR_TIME_LEN  32 
#define BASE_DIR_MAX_LEN  1024
#define LOG_DIR_1NAME "/LOG/"


typedef struct myCurStrTime
{
    int tm_year;
    int tm_mon;
    int tm_mday;
    int tm_hour;
    int tm_min;
    int tm_sec;
    unsigned long int tv_usec;

}T_myCurStrTime;


static char* getAppPath( char buf_a[], int n );
//static int ReadLogLevelFile();

static char defaultFlag                     = 1;
static std::string pgsLogPath               = "";
static std::string logNamePrefix            = "";

static FILE* pgsLogFp_p                     = NULL;
static char  pgsBaseDir_a[BASE_DIR_MAX_LEN] = {0};
static int   pgsLogLevel                    = LOGDEBUG;
static pthread_mutex_t logLock              = PTHREAD_MUTEX_INITIALIZER;

static char  pgsCurStrTime_a[CUR_STR_TIME_LEN] = {0};
static T_myCurStrTime            pgsCurStrTime = {0};

char LogLevelNames_aa[LOGEND][10] = {
    "EMERG:",
    "ALERT:",
    "CRIT:",
    "ERROR:",
    "WARNING:",
    "NOTICE:",
    "INFO:",
    "DEBUG:"
};




//__attribute__((constructor)) static void libBaseLog_version()
//{
//    getAppPath( pgsBaseDir_a, sizeof( pgsBaseDir_a ) );
//
//    ReadLogLevelFile();
//
//    PrintToStdout( LOGINITINFO, 
//            "%s,CompileTime:[%s %s]", 
//            LIB_BASE_VERSION, __DATE__, __TIME__ );
//
//    //WriteLog( LOGINITINFO, 
//    //        "%s,CompileTime:[%s %s]", 
//    //        LIB_BASE_VERSION, __DATE__, __TIME__ );
//
//}




const char* GetCurDateTime()
{
    //memset( pgsCurStrTime_a, 0, CUR_STR_TIME_LEN );
    struct timeval tv;
    time_t nowTimeSec;
    struct tm curTime;

    gettimeofday( &tv, NULL );

    //time_t tNow = time(NULL);
    nowTimeSec = tv.tv_sec;
    localtime_r( &nowTimeSec, &curTime );

    pgsCurStrTime.tm_year = curTime.tm_year+1900;
    pgsCurStrTime.tm_mon  = curTime.tm_mon+1;
    pgsCurStrTime.tm_mday = curTime.tm_mday;
    pgsCurStrTime.tm_hour = curTime.tm_hour;
    pgsCurStrTime.tm_min  = curTime.tm_min;
    pgsCurStrTime.tm_sec  = curTime.tm_sec;
    pgsCurStrTime.tv_usec = tv.tv_usec;

    snprintf( pgsCurStrTime_a, CUR_STR_TIME_LEN,
              "%04d-%02d-%02d_%02d:%02d:%02d.%lu",
              pgsCurStrTime.tm_year, pgsCurStrTime.tm_mon, 
              pgsCurStrTime.tm_mday, pgsCurStrTime.tm_hour, 
              pgsCurStrTime.tm_min, pgsCurStrTime.tm_sec,
              pgsCurStrTime.tv_usec
            );

    return pgsCurStrTime_a;
}

void _PrintToStdout( 
        const char* FILE_p, 
        int LINE, 
        int logLevel, 
        const char* format_p,
       ... 
        )
{
    if ( logLevel > pgsLogLevel && LOGINITINFO != logLevel )
    {
        return;
    }

    GetCurDateTime();

    char* tLogLevelNames_p = (char*)"";
    if ( LOGINITINFO != logLevel )
    {
        tLogLevelNames_p = LogLevelNames_aa[logLevel];
    }

    fprintf( stdout, "%s %s:%d[%s",
            pgsCurStrTime_a,
            basename(FILE_p), LINE, tLogLevelNames_p
           );

    va_list ap;
    va_start( ap, format_p );
    vfprintf( stdout, format_p, ap );
    va_end( ap );

    fprintf( stdout, "]\n" );

    fflush( stdout );

    return;
}


//static int ReadLogLevelFile()
//{
//    std::ostringstream oss;
//    oss<<pgsBaseDir_a<<"/LogLevel.ini";
//    std::string strPath = oss.str();
//
//    if ( access( strPath.c_str(), 0 ) == 0 )
//    {
//        char line_a[128] = {0};
//        FILE* fp = fopen( strPath.c_str(), "r" );
//        while ( NULL != fgets( line_a, 128, fp ) )
//        {
//            if( strncmp( line_a, "LogLevel", strlen("LogLevel") ) == 0 )
//            {
//                sscanf( line_a, "LogLevel = %d", &pgsLogLevel );
//                break;
//            }
//        }
//
//        fclose( fp );
//    }
//
//    if ( pgsLogLevel > LOGDEBUG || pgsLogLevel < LOGEMERG )
//    {
//        pgsLogLevel = LOGDEBUG;
//    }
//
//    return 0;
//}


static char* getAppPath( char buf_a[], int n )
{
    if ( n <=0 || buf_a == NULL )
    {
        return NULL;
    }

    int r = readlink( "/proc/self/exe", buf_a, n-1 );
    if ( r < 0 || r >= n-1 )
    {
        PrintToStdout( LOGERROR, 
                "readlink fail,errno[%d],errMsg[%s]",
                errno, strerror(errno) );

        return NULL;
    }

    buf_a[r] = '\0';
    for ( int i = r; i>=0; i-- )
    {
        if ( buf_a[i] == '/' )
        {
            buf_a[i] = '\0';
            break;
        }
    }

    return buf_a;
}


static int MakeDirs( std::string& strDir )
{
    int vRet = 0;

    vRet = access( strDir.c_str(), F_OK );
    if ( 0 == vRet )
    {
        return 0;
    }

    int         iIndex = 0;
    std::size_t iPos   = 0;
    while ( ( iPos = strDir.find('/', iIndex) ) != std::string::npos )
    {
        std::string strSub = strDir.substr( 0, iPos+1 );
        if ( !strSub.empty() )
        {
            vRet = access( strSub.c_str(), F_OK );
            if ( 0 != vRet )
            {
                vRet = mkdir( strSub.c_str(), 0755 );
                if ( 0 != vRet )
                {

                    PrintToStdout( LOGERROR, 
                            "mkdir dir[%s]fail,errno[%d],errMsg[%s]",
                            strSub.c_str(), errno, strerror(errno)
                            );

                    return -1;
                }
            }
        }
        iIndex = iPos + 1;
    }

    return 0;
}

void SetLogLevel( const int& iLevel )
{
    if ( iLevel > LOGDEBUG || iLevel < LOGEMERG )
    {
        pgsLogLevel = LOGDEBUG;
    }
    else
    {
        pgsLogLevel = iLevel;
    }
    return;
}

void WriteLogInit( 
        const int& iLevel,
        const char* dirName_p,      /*不设置时默认日志路径为程序运行路径下LOG/$YYYY/$MM*/
        const char* logPrefixName_p /*当设置dirName_p时才设置此值：日志名为logPrefixName_$YYYYMMDD.log*/
        )
{
    SetLogLevel( iLevel );

    if ( dirName_p != NULL )
    {
        defaultFlag = 0;
        pgsLogPath  = dirName_p;
        int tlen = strlen( dirName_p );
        if ( dirName_p[tlen-1] != '/' )
        {
            pgsLogPath = pgsLogPath + std::string("/");
        }

        if ( logPrefixName_p != NULL )
        {
            logNamePrefix = logPrefixName_p;
        }
    }

    return;
}


int MakeLogFilePath( std::string& strLogFile )
{
    int iErrNo = 0;

    char szDatePath_a[32] = {0};
    if ( defaultFlag == 1 )
    {
        snprintf( szDatePath_a, 
                  sizeof( szDatePath_a), 
                  "%04d/%02d/",
                  pgsCurStrTime.tm_year,pgsCurStrTime.tm_mon
                );

        pgsLogPath = pgsBaseDir_a + std::string(LOG_DIR_1NAME) 
                     + std::string(szDatePath_a);
    }

    iErrNo = MakeDirs( pgsLogPath );
    if ( 0 != iErrNo )
    {

        PrintToStdout( LOGERROR, 
                "mkdir[%s]fail,MakeDirs return[%d]",
                pgsLogPath.c_str(), iErrNo );

        return -1;
    }

    if ( defaultFlag == 1 )
    {
        //memset( szDatePath_a, 0, sizeof(szDatePath_a) );
        snprintf( szDatePath_a, sizeof(szDatePath_a), 
                  "%02d.log",
                  pgsCurStrTime.tm_mday
                );

        strLogFile = pgsLogPath + std::string(szDatePath_a);
    }
    else
    {
        snprintf( szDatePath_a, 
                  sizeof( szDatePath_a), 
                  "_%04d%02d%02d.log",
                  pgsCurStrTime.tm_year,pgsCurStrTime.tm_mon,
                  pgsCurStrTime.tm_mday
                );
        strLogFile = pgsLogPath + logNamePrefix + std::string(szDatePath_a);
    }

    return 0;
}




void _WriteLog( 
        const char* FILE_p, 
        int LINE, 
        int logLevel, 
        const char* format_p,
        ...
        )
{
    static std::string strStaticLogPath;

    int iRet = 0;
    std::string strLogFile;

    GetCurDateTime();

    if ( logLevel > pgsLogLevel && LOGINITINFO != logLevel )
    {
        return;
    }

    if ( 0 == strlen(pgsBaseDir_a) )
    {
        getAppPath( pgsBaseDir_a, sizeof(pgsBaseDir_a) );
    }

    iRet = MakeLogFilePath( strLogFile );
    if ( 0 != iRet )
    {
        return;
    }

    pthread_mutex_lock( &logLock );
    if ( 0 != strStaticLogPath.compare( strLogFile ) )
    {
        iRet = MakeLogFilePath( strLogFile );
        if ( 0 != iRet )
        {
            pthread_mutex_unlock( &logLock );
            return;
        }
        
        if ( 0 != strStaticLogPath.compare( strLogFile ) )
        {
            if ( NULL != pgsLogFp_p )
            {
                fclose( pgsLogFp_p );
                pgsLogFp_p = NULL;
            }

            std::ostringstream oss;
            std::string tmpString;

            pgsLogFp_p = fopen( strLogFile.c_str(), "a");
            if ( NULL == pgsLogFp_p )
            {
                PrintToStdout( LOGERROR, 
                        "fopen[%s]fail,errno[%d],errMsg[%s]",
                         strLogFile.c_str(), errno, strerror(errno)
                         );
            }
            else
            {
                strStaticLogPath = strLogFile;
                oss<<"path="<<strStaticLogPath<<",fd="<<fileno(pgsLogFp_p);
                tmpString = oss.str();
                //PrintToStdout( LOGDEBUG, 
                //       "path=%s,fd=%d",
                //       strStaticLogPath.c_str(), fileno(pgsLogFp_p) 
                //      );
            }
        }
    }

    if ( NULL != pgsLogFp_p )
    {
        char* tLogLevelNames_p = (char*)"";
        if ( LOGINITINFO != logLevel )
        {
            //tLogLevelNames_p = (char*)"";
            tLogLevelNames_p = LogLevelNames_aa[logLevel];
        }
        fprintf( pgsLogFp_p, "%02d:%02d:%02d.%lu %s:%d[%s",
                pgsCurStrTime.tm_hour,pgsCurStrTime.tm_min,
                pgsCurStrTime.tm_sec,pgsCurStrTime.tv_usec, 
                basename(FILE_p), LINE, tLogLevelNames_p
               );

        va_list ap;
        va_start( ap, format_p );
        vfprintf( pgsLogFp_p, format_p, ap );
        va_end( ap );

        fprintf ( pgsLogFp_p, "]\n" );

        fflush( pgsLogFp_p );
    }
    else
    {
        PrintToStdout( LOGERROR, "pgsLogFp_p==NULL" );
    }
    pthread_mutex_unlock( &logLock );

    return;
}

