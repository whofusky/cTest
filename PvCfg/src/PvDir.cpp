/****************************************
 * 
 *    PvDir.cpp 
 * 
 *    Create by fushikai
 * 
 *    2022-09-25
 * 
 ****************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include "PvDir.h"




#define PV_HOSTNAME_ENV     "FSHOSTNAME" //配置主机名的环境变量

#define PV_DIR_MAX_LEN       1024        //路径的最大长度
#define PV_FILENAME_MAX      100         //文件名的最大长度
#define PV_ALLFILENAME_MAX   1124        //路径+文件名的最大长度
#define PV_SUFFIX_LEN        40
#define PV_ID1PATH_LEN       500

#define PV_DIR_LEVEL_NUM  6              //常用路径中文件夹的个数
#define PV_ID1_LEVEL      3              //id1在路径中的第几个文件夹
#define PV_ID2_LEVEL      4              //id2在路径中的第几个文件夹


#define assignRet(ret,value)\
    {\
        if ( ret != NULL ) \
        {\
            *ret = value;\
        }\
    }



static int pgsInitRet          = 0;
static pthread_mutex_t logLock = PTHREAD_MUTEX_INITIALIZER;

static char  pgsVerFileSuffix_a[PV_SUFFIX_LEN] = "_ver.ini";         //版本文件名后辍
static char  pgsCfgFileSuffix_a[PV_SUFFIX_LEN] = "_config.ini";      //配置文件名后辍
static char  pgsStrNull_a[]       = "";

static char  pgsBaseDir_a[PV_DIR_MAX_LEN]   = {0};   //程序运行路径
static char  pgsPathFile_a[PV_ALLFILENAME_MAX] = {0};   //带路径的程序名
static char  pgsVerFile_a[PV_ALLFILENAME_MAX]  = {0};   //带路径的版本文件名
static char  pgsCfgFile_a[PV_ALLFILENAME_MAX]  = {0};   //带路径的配置文件名
static char  pgsID1[PV_FILENAME_MAX] = {0};               //一级id名
static char  pgsID2[PV_FILENAME_MAX] = {0};               //二级id名
static char  pgsExe[PV_FILENAME_MAX] = {0};               //不带路径的程序名




//用于存储路径中的头几个文件夹名
static char pgsCmDirName_aa[PV_DIR_LEVEL_NUM][PV_FILENAME_MAX] ={
    "","","","","",""
};

//定义公共路径中固定部分 并 存储公共路径的最终值
static char pgsPubDir_aa[PV_DIR_COUNT][PV_DIR_MAX_LEN] = {
    "pubfile/app-send-p1",    /*PV_DIR_APPSENDP1,  */
    "pubfile/app-send-p2",    /*PV_DIR_APPSENDP2,    */
    "pubfile/app-send-s",     /*PV_DIR_APPSENDS,     */
    "pubfile/app-send-m",     /*PV_DIR_APPSENDM,     */
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
    "pubfile/temp/warn",      /*PV_DIR_WARFORIMS,    */
    "pubfile/temp",           /*PV_DIR_TEMP,         */
    "pubfile/export",         /*PV_DIR_EXPORT,       */
    "pubfile/dscsql-in",      /*PV_DIR_DSCSQLIN,     */
    "pubfile/dscsql-out",     /*PV_DIR_DSCSQLOUT,    */
    "pubfile/appback"         /*PV_DIR_APPBACK,      */
}; 


//定义公共配置文件名固定部分 并 存储公共配置文件名的绝对路径值
static char pgsPubCfgFile_aa[PV_PUBFILE_COUNT][PV_ALLFILENAME_MAX] = {
    "pub_config.ini",         /*PV_PUBCFG_FILE=0,     pub_config.ini*/
    "TB_FRM_SVR_SOFTINFO.csv" /*PV_APP_LIST,          applist.txt*/
};


//存储程序运行相关值
static char pgsAppRunPar_aa[RUNPAR_COUNT][PV_ALLFILENAME_MAX] = {
    "",        /*RUNPAR_ID1=0,  */
    "",        /*RUNPAR_ID2,    */
    "",        /*RUNPAR_PATH,   */
    "",        /*RUNPAR_EXE,    */
    "",        /*RUNPAR_PATHEXE,*/
    "",        /*RUNPAR_CFGFILE,*/
    "",        /*RUNPAR_VERFILE, */
    "PVF-P1"   /*RUNPAR_HOSTNAME, */
};




//// Dsc: 将字符串中的小写字母都转换成大写
//// 注: 此函数暂时没有用到，先注释
//static void strToUpper(char* src_p)
//{
//    for ( int i=0; src_p[i] != '\0'; i++)
//    {
//        if ( src_p[i] >= 'a' && src_p[i] <= 'z' )
//        {
//            src_p[i] = src_p[i] ^ 0x20;
//        }
//    }
//    return;
//}


// Dsc: 将字符串中的大写字母都转换成小写
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


// Dsc: 获取当前运行程序的路径值及根据路径值可推断出的一些其他值
static int getAppPath()
{

    int tLen = PV_ALLFILENAME_MAX -1;

    int r = readlink( "/proc/self/exe", pgsPathFile_a, tLen );
    if ( r < 0 || r >= tLen )
    {
        printf( "readlink fail,errno[%d],errMsg[%s]", errno, strerror(errno) );

        return PVDIR_GETEXEPATH_ERR;
    }

    //带路径值的可执行程序
    pgsPathFile_a[r] = '\0';
    int i            = 0;
    int tFindFlag    = 0;
    for ( i = r; i>=0; i-- )
    {
        if ( pgsPathFile_a[i] == '/' )
        {
            //pgsPathFile_a[i] = '\0';
            strncpy(pgsBaseDir_a,pgsPathFile_a,i);
            pgsBaseDir_a[i] = '\0';
            tLen            = r - i;
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
    for ( int j = 1; j < i && slashNum < PV_DIR_LEVEL_NUM; j++ )
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

    if ( slashNum < PV_ID1_LEVEL ) 
    {
        // "/xxxxx/vpf20/id1/pubfile"
        //程序至少需要运行在id1下面
        return PVDIR_APPRUNDIR_ERR;
    }

    //从程序运行路径各级文件夹名中取id1和id2的值
    strcpy( pgsID1, pgsCmDirName_aa[PV_ID1_LEVEL-1] );
    strcpy( pgsID2, pgsCmDirName_aa[PV_ID2_LEVEL-1] );

    //拼程序运行路径下的版本文件和配置文件名(带路径)
    char tmpStr[PV_FILENAME_MAX-PV_SUFFIX_LEN] = {0};
    strcpy( tmpStr, pgsExe );
    strToLower( tmpStr );
    snprintf( pgsVerFile_a, PV_ALLFILENAME_MAX-1,
            "%s/%s%s", pgsBaseDir_a, tmpStr, pgsVerFileSuffix_a );
    snprintf( pgsCfgFile_a, PV_ALLFILENAME_MAX-1,
            "%s/%s%s", pgsBaseDir_a, tmpStr, pgsCfgFileSuffix_a );

    //获取id所在的全路径值
    char tmpIdPath[PV_ID1PATH_LEN] = {0};
    for ( int k = 0; k < PV_ID1_LEVEL; k++ )
    {
        strcat( tmpIdPath, "/" );
        strcat( tmpIdPath, pgsCmDirName_aa[k] );
    }

    //拼装/xxxx/pvfs20/id1/pubfile下的路径值
    char tmpPath[PV_SUFFIX_LEN] = {0};
    for ( int k = 0; k < PV_DIR_COUNT; k++ )
    {
        strcpy( tmpPath, pgsPubDir_aa[k] );
        snprintf(  pgsPubDir_aa[k], PV_DIR_MAX_LEN,
                "%s/%s", tmpIdPath, tmpPath );

    }

    //拼装/xxxx/pvfs20/id1/pubfile/cfg下file 的全路径值
    for ( int k = 0; k < PV_PUBFILE_COUNT; k++ )
    {
        strcpy( tmpPath, pgsPubCfgFile_aa[k] );
        snprintf(  pgsPubCfgFile_aa[k], PV_ALLFILENAME_MAX,
                "%s/%s", pgsPubDir_aa[PV_DIR_CFG], tmpPath );
    }


    //拼程序相关
    snprintf( pgsAppRunPar_aa[RUNPAR_ID1], PV_ALLFILENAME_MAX,
            "%s", pgsID1 );
    snprintf( pgsAppRunPar_aa[RUNPAR_ID2], PV_ALLFILENAME_MAX,
            "%s", pgsID2 );
    snprintf( pgsAppRunPar_aa[RUNPAR_PATH], PV_ALLFILENAME_MAX,
            "%s", pgsBaseDir_a);
    snprintf( pgsAppRunPar_aa[RUNPAR_EXE], PV_ALLFILENAME_MAX,
            "%s", pgsExe );
    snprintf( pgsAppRunPar_aa[RUNPAR_PATHEXE], PV_ALLFILENAME_MAX,
            "%s", pgsPathFile_a );
    snprintf( pgsAppRunPar_aa[RUNPAR_CFGFILE], PV_ALLFILENAME_MAX,
            "%s", pgsCfgFile_a );
    snprintf( pgsAppRunPar_aa[RUNPAR_VERFILE], PV_ALLFILENAME_MAX,
            "%s", pgsVerFile_a );

    //获取环境变量FSHOSTNAME的值
    char* tmpStr_p = getenv( PV_HOSTNAME_ENV );
    if ( tmpStr_p == NULL )
    {
        return PVDIR_GET_ENVVAR_ERR;
    }
    snprintf( pgsAppRunPar_aa[RUNPAR_HOSTNAME], PV_ALLFILENAME_MAX,
            "%s", tmpStr_p );
        

    return PVDIR_SUCCESS;
}




static CPvDir* mInstance = NULL;


const char* CPvDir::_errorNames[ PVDIR_ERROR_COUNT + 1 ]={
    "PVDIR_SUCCESS",
    "PVDIR_GETEXEPATH_ERR",
    "PVDIR_APPRUNDIR_ERR",
    "PVDIR_TYPE_UNKNOWN_ERR",
    "PVDIR_GET_ENVVAR_ERR",
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
    assignRet( ret, pgsInitRet );

    if ( iType >= PV_DIR_COUNT || iType < 0 )
    {
        assignRet( ret, PVDIR_TYPE_UNKNOWN_ERR );
        return tmp_p;
    }

    tmp_p = pgsPubDir_aa[ iType ];
    return tmp_p;
}


const char* CPvDir::getFile( int iType, int* ret  )
{
    const char* tmp_p = pgsStrNull_a;
    assignRet( ret, pgsInitRet );

    if ( iType >= PV_PUBFILE_COUNT  || iType < 0 )    
    {
        assignRet( ret, PVDIR_TYPE_UNKNOWN_ERR );
        return tmp_p;
    }

    tmp_p = pgsPubCfgFile_aa[ iType ];
    return tmp_p;
}


const char* CPvDir::getPar( int iType, int* ret  )
{
    const char* tmp_p = pgsStrNull_a;
    assignRet( ret, pgsInitRet );

    if ( iType >= RUNPAR_COUNT  || iType < 0 )
    {
        assignRet( ret, PVDIR_TYPE_UNKNOWN_ERR );
        return tmp_p;
    }

    tmp_p = pgsAppRunPar_aa[ iType ];
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


