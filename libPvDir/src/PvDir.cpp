/****************************************
 * 
 *    PvDir.cpp 
 * 
 *    Create by fushikai
 * 
 *    2022-09-25
 * 
 ****************************************/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include "PvDir.h"


#define LIB_BASE_VERSION "[ libPvDir.a ] Version: V00.01.000"


#define CUR_STR_TIME_LEN  32 
#define BASE_DIR_MAX_LEN  1024
#define FILEANME_MAX      100
#define PATH_FILENAME_MAX 1124

#define CM_DIR_LEVEL_NUM  6       //常用路径中文件夹的个数
#define DIR_ID1_LEVEL     3       //id1在路径中的第几个文件夹
#define DIR_ID2_LEVEL     4       //id2在路径中的第几个文件夹



static char  pgsVerFileSuffix_a[] = "_ver.ini";         //版本文件名后辍
static char  pgsCfgFileSuffix_a[] = "_config.ini";      //配置文件名后辍
static char  pgsStrNull_a[]       = "";


static char  pgsBaseDir_a[BASE_DIR_MAX_LEN]   = {0};   //程序运行路径
static char  pgsPathFile_a[PATH_FILENAME_MAX] = {0};   //带路径的程序名
static char  pgsVerFile_a[PATH_FILENAME_MAX]  = {0};   //带路径的版本文件名
static char  pgsCfgFile_a[PATH_FILENAME_MAX]  = {0};   //带路径的配置文件名
static char  pgsID1[FILENAME_MAX] = {0};               //一级id名
static char  pgsID2[FILENAME_MAX] = {0};               //二级id名
static char  pgsExe[FILENAME_MAX] = {0};               //不带路径的程序名


static int pgsInitRet = 0;


static pthread_mutex_t logLock = PTHREAD_MUTEX_INITIALIZER;

static char  pgsCurStrTime_a[CUR_STR_TIME_LEN] = {0};

static char pgsCmDirName_aa[CM_DIR_LEVEL_NUM][FILEANME_MAX] ={
    "","","","","",""
};

static char pgsPubDir_aa[PV_DIR_COUNT][BASE_DIR_MAX_LEN] = {
    "pubfile/app-send-p1",    /*PV_DIR_APPSENDP1,  */
    "pubfile/app-send-p2",    /*PV_DIR_APPSENDP2,    */
    "pubfile/app-send-s",     /*PV_DIR_APPSENDS,     */
    "pubfile/eds-send-p1",    /*PV_DIR_EDSSENDP1,    */
    "pubfile/eds-send-p2",    /*PV_DIR_EDSSENDP2,    */
    "pubfile/eds-send-s",     /*PV_DIR_EDSSENDS,     */
    "pubfile/eds-receive-p1", /*PV_DIR_EDSRCVP1,     */
    "pubfile/eds-receive-p2", /*PV_DIR_EDSRCVP2,     */
    "pubfile/eds-receive-s",  /*PV_DIR_EDSRCVS,      */
    "pubfile/app-receive",    /*PV_DIR_APPRCV,       */
    "pubfile/app-send",       /*PV_DIR_APPSEND,      */
    "pubfile/eds-send",       /*PV_DIR_EDSSEND,      */
    "pubfile/eds-receive",    /*PV_DIR_EDSRCV,       */
    "pubfile/cfg",            /*PV_DIR_CFG,          */
    "pubfile/dsrc",           /*PV_DIR_DSRC,         */
    "pubfile/backup",         /*PV_DIR_BACKUP,       */
    "pubfile/temp",           /*PV_DIR_TEMP,         */
    "pubfile/export",         /*PV_DIR_EXPORT,       */
    "pubfile/dscsql-in",      /*PV_DIR_DSCSQLIN,     */
    "pubfile/dscsql-out",     /*PV_DIR_DSCSQLOUT,    */
    "pubfile/appback"         /*PV_DIR_APPBACK,      */
}; 


static char pgsPubCfgFile_aa[PV_PUBFILE_COUNT][PATH_FILENAME_MAX] = {
    "pub_config.ini", /*PV_PUBCFG_FILE=0,     pub_config.ini*/
    "applist.txt"     /*PV_APP_LIST,          applist.txt*/
};


static char pgsAppRunPar_aa[RUNPAR_COUNT][PATH_FILENAME_MAX] = {
    "",    /*RUNPAR_ID1=0,  */
    "",    /*RUNPAR_ID2,    */
    "",    /*RUNPAR_PATH,   */
    "",    /*RUNPAR_EXE,    */
    "",    /*RUNPAR_PATHEXE,*/
    "",    /*RUNPAR_CFGFILE,*/
    ""     /*RUNPAR_VERFILE, */
};


static CPvDir* mInstance = NULL;


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
static T_myCurStrTime            pgsCurStrTime = {0};


//__attribute__((constructor)) static void libBaseLog_version()
//{
//
//
//    PrintToStdout( LOGINITINFO, 
//            "%s,CompileTime:[%s %s]", 
//            LIB_BASE_VERSION, __DATE__, __TIME__ );
//
//    //PvDir( LOGINITINFO, 
//    //        "%s,CompileTime:[%s %s]", 
//    //        LIB_BASE_VERSION, __DATE__, __TIME__ );
//
//}




const int GetCurDateTime()
{
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

    return 0;
}




static void strToUpper(char* src_p)
{
    for ( int i=0; src_p[i] != '\0'; i++)
    {
        if ( src_p[i] >= 'a' && src_p[i] <= 'z' )
        {
            src_p[i] = src_p[i] ^ 0x20;
        }
    }
    return;
}


static void strToLower( char* src_p )
{
    for ( int i=0; src_p[i] != '\0'; i++)
    {
        if ( src_p[i] >= 'A' && src_p[i] <= 'Z' )
        {
            src_p[i] = src_p[i] ^ 0x20;
        }
    }
    return;
}


static int getAppPath()
{

    int tLen = PATH_FILENAME_MAX -1;

    int r = readlink( "/proc/self/exe", pgsPathFile_a, tLen );
    if ( r < 0 || r >= tLen )
    {
        printf( "readlink fail,errno[%d],errMsg[%s]",
                errno, strerror(errno) );

        return PVDIR_GETEXEPATH_ERR;
    }

    //带路径值的可执行程序
    pgsPathFile_a[r] = '\0';
    int i = 0;
    int tFindFlag = 0;
    for ( i = r; i>=0; i-- )
    {
        if ( pgsPathFile_a[i] == '/' )
        {
            //pgsPathFile_a[i] = '\0';
            strncpy(pgsBaseDir_a,pgsPathFile_a,i);
            pgsBaseDir_a[i] = '\0';
            tLen = r - i;
            strncpy( pgsExe, pgsPathFile_a+i+1, tLen );
            tFindFlag = 1;
            break;
        }
    }

    if ( tFindFlag == 0 )
    {
        return PVDIR_APPRUNDIR_ERR;
    }

    //从程序运行路径pgsBaseDir_a中拆分出路径中的文件夹名到pgsCmDirName_aa
    int preSlash = 0;
    int slashNum = 0;
    for ( int j = 1; j < i && slashNum < CM_DIR_LEVEL_NUM; j++ )
    {
        if ( pgsBaseDir_a[j] == '/' )
        {
            tLen = j - preSlash -1;
            strncpy(pgsCmDirName_aa[slashNum],
                    pgsBaseDir_a + preSlash + 1, 
                    tLen );
            pgsCmDirName_aa[slashNum][tLen] = '\0';
            preSlash = j;
            slashNum++;
        }
    }

    if ( slashNum < DIR_ID1_LEVEL ) 
    {
        // "/xxxxx/vpf20/id1/pubfile"
        //程序至少需要运行在id1下面
        return PVDIR_APPRUNDIR_ERR;
    }

    //从程序运行路径各级文件夹名中取id1和id2的值
    strcpy( pgsID1, pgsCmDirName_aa[DIR_ID1_LEVEL-1] );
    strcpy( pgsID2, pgsCmDirName_aa[DIR_ID2_LEVEL-1] );

    //拼程序运行路径下的版本文件和配置文件名(带路径)
    char tmpStr[FILENAME_MAX] = {0};
    strcpy( tmpStr, pgsExe );
    strToLower( tmpStr );
    snprintf( pgsVerFile_a, PATH_FILENAME_MAX-1,
            "%s/%s%s", pgsBaseDir_a, tmpStr, pgsVerFileSuffix_a );
    snprintf( pgsCfgFile_a, PATH_FILENAME_MAX-1,
            "%s/%s%s", pgsBaseDir_a, tmpStr, pgsCfgFileSuffix_a );

    //获取id所在的全路径值
    char tmpIdPath[PATH_FILENAME_MAX] = {0};
    for ( int k = 0; k < DIR_ID1_LEVEL; k++ )
    {
        strcat( tmpIdPath, "/" );
        strcat( tmpIdPath, pgsCmDirName_aa[k] );
    }

    //拼装/xxxx/pvf20/id1/pubfile下的路径值
    char tmpPath[PATH_FILENAME_MAX] = {0};
    for ( int k = 0; k < PV_DIR_COUNT; k++ )
    {
        strcpy( tmpPath, pgsPubDir_aa[k] );
        snprintf(  pgsPubDir_aa[k], BASE_DIR_MAX_LEN,
                "%s/%s", tmpIdPath, tmpPath );

    }

    //拼装/xxxx/pvf20/id1/pubfile/cfg下file 的全路径值
    for ( int k = 0; k < PV_PUBFILE_COUNT; k++ )
    {
        strcpy( tmpPath, pgsPubCfgFile_aa[k] );
        snprintf(  pgsPubCfgFile_aa[k], PATH_FILENAME_MAX,
                "%s/%s", pgsPubDir_aa[PV_DIR_CFG], tmpPath );
    }

    //拼程序相关
    snprintf( pgsAppRunPar_aa[RUNPAR_ID1], PATH_FILENAME_MAX,
            "%s", pgsID1 );
    snprintf( pgsAppRunPar_aa[RUNPAR_ID2], PATH_FILENAME_MAX,
            "%s", pgsID2 );
    snprintf( pgsAppRunPar_aa[RUNPAR_PATH], PATH_FILENAME_MAX,
            "%s", pgsBaseDir_a);
    snprintf( pgsAppRunPar_aa[RUNPAR_EXE], PATH_FILENAME_MAX,
            "%s", pgsExe );
    snprintf( pgsAppRunPar_aa[RUNPAR_PATHEXE], PATH_FILENAME_MAX,
            "%s", pgsPathFile_a );
    snprintf( pgsAppRunPar_aa[RUNPAR_CFGFILE], PATH_FILENAME_MAX,
            "%s", pgsCfgFile_a );
    snprintf( pgsAppRunPar_aa[RUNPAR_VERFILE], PATH_FILENAME_MAX,
            "%s", pgsVerFile_a );
        
    return PVDIR_SUCCESS;
}



const char* CPvDir::_errorNames[ PVDIR_ERROR_COUNT + 1 ]={
    "PVDIR_SUCCESS",
    "PVDIR_GETEXEPATH_ERR",
    "PVDIR_APPRUNDIR_ERR",
    "PVDIR_TYPE_UNKNOWN_ERR",
    ""
};


CPvDir::CPvDir()
{

}


CPvDir::~CPvDir()
{

}

/*static*/ const char* CPvDir::ErrorIDToName( int errorId )
{
    const char* errorName = _errorNames[PVDIR_ERROR_COUNT];
    if ( errorId < 0 || errorId > PVDIR_ERROR_COUNT )
    {
        return errorName;
    }
    errorName = _errorNames[ errorId ];
    return errorName;
}


CPvDir* CPvDir::Instance()
{
    if ( mInstance == NULL )
    {
        pthread_mutex_lock( &logLock );
        if ( mInstance == NULL )
        {
            mInstance = new CPvDir();
            pgsInitRet = getAppPath();
        }
        pthread_mutex_unlock( &logLock );

    }

    return mInstance;
}


const char* CPvDir::getDir( int iType, int* ret  )
{
    const char* tmp_p = pgsStrNull_a;
    if ( iType >= PV_DIR_COUNT || iType < 0 )
    {
        if ( ret != NULL ) *ret = PVDIR_TYPE_UNKNOWN_ERR;
        return tmp_p;
    }
    tmp_p = pgsPubDir_aa[ iType ];
    if ( ret != NULL )  *ret = pgsInitRet;
    return tmp_p;
}


const char* CPvDir::getFile( int iType, int* ret  )
{
    const char* tmp_p = pgsStrNull_a;
    if ( iType >= PV_PUBFILE_COUNT  || iType < 0 )    
    {
        if ( ret != NULL ) *ret = PVDIR_TYPE_UNKNOWN_ERR;
        return tmp_p;
    }
    tmp_p = pgsPubCfgFile_aa[ iType ];
    if ( ret != NULL )  *ret = pgsInitRet;
    return tmp_p;
}


const char* CPvDir::getPar( int iType, int* ret  )
{
    const char* tmp_p = pgsStrNull_a;
    if ( iType >= RUNPAR_COUNT  || iType < 0 )
    {
        if ( ret != NULL ) *ret = PVDIR_TYPE_UNKNOWN_ERR;
        return tmp_p;
    }
    tmp_p = pgsAppRunPar_aa[ iType ];
    if ( ret != NULL )  *ret = pgsInitRet;
    return tmp_p;
}


const char* GetPvPubDir( int iType, int* ret  )
{
    return CPvDir::Instance()->getDir( iType, ret );
}


const char* GetPvPubFile( int iType, int* ret )
{
    return CPvDir::Instance()->getFile( iType, ret );
}


const char* GetPvRunPar( int iType, int* ret  )
{
    return CPvDir::Instance()->getPar( iType, ret );
}


const char* PvStrError( int errorId )
{
    return CPvDir::ErrorIDToName( errorId );
}


