
#include "funcLib.h"

#include "funcLibSo.h"
#include "pubExtern.h"
#include "mete_ftpDown.h"
#include "mete_healthRep.h"
#include "mete_monitor.h"
#include "mete_ftpPut.h"
#include "mete_statBusList.h"
#include "mete_synBusList.h"
#include "Log/Log.h"
#include "mete_errHandling.h"
#include "mete_init.h"

#define MC_MAX_PWDLEN 200 //程序运行当前工作目录的最大长度

//using namespace tinyxml2;

int end = 0;
int  prtMDebugFlag = 0;  //否打印main函数 debug日志, 1 打印,0不打印
char tMDebugLog[MC_FULLN_LEN]; //其他程序日志名


void signal_handler ( int sig )
{
    if ( 1 == prtMDebugFlag )
        writeLog ( tMDebugLog, "%s|%s|%d:catch th signal:[%d]------in------\n",
                   __FILE__, __func__, __LINE__, sig );


    //ftp下载线程
    C_MFtpDown::Instance()->stop();

    if ( 1 == prtMDebugFlag )
        writeLog ( tMDebugLog, "%s|%s|%d:----[%s] thread exit\n",
                   __FILE__, __func__, __LINE__, "C_MFtpDown" );

    sleep ( 1 );

    //业务清单合成处理线程
    C_MFsyBList::Instance()->stop();

    if ( 1 == prtMDebugFlag )
        writeLog ( tMDebugLog, "%s|%s|%d:----[%s] thread exit\n",
                   __FILE__, __func__, __LINE__, "C_MFsyBList" );

    sleep ( 1 );

    //业务清单提供情况线程
    C_MStatBList::Instance()->stop();

    if ( 1 == prtMDebugFlag )
        writeLog ( tMDebugLog, "%s|%s|%d:----[%s] thread exit\n",
                   __FILE__, __func__, __LINE__, "C_MStatBList" );

    sleep ( 1 );

    //健康报告
    C_MHealthRep::Instance()->stop();

    if ( 1 == prtMDebugFlag )
        writeLog ( tMDebugLog, "%s|%s|%d:----[%s] thread exit\n",
                   __FILE__, __func__, __LINE__, "C_MHealthRep" );

    sleep ( 1 );

    //ftp上传处理线程
    C_MPutFtp::Instance()->stop();

    if ( 1 == prtMDebugFlag )
        writeLog ( tMDebugLog, "%s|%s|%d:----[%s] thread exit\n",
                   __FILE__, __func__, __LINE__, "C_MPutFtp" );

    sleep ( 1 );

    //错误处理
    C_MErrHandling::Instance()->stop();

    if ( 1 == prtMDebugFlag )
        writeLog ( tMDebugLog, "%s|%s|%d:----[%s] thread exit\n",
                   __FILE__, __func__, __LINE__, "C_MErrHandling" );

    sleep ( 1 );

    //文件监视处理线程
    C_MMonitor::Instance()->stop();

    if ( 1 == prtMDebugFlag )
        writeLog ( tMDebugLog, "%s|%s|%d:----[%s] thread exit\n",
                   __FILE__, __func__, __LINE__, "C_MMonitor" );

    sleep ( 1 );

    //事件管理线程
    IEventManager::Initialize()->stop();

    if ( 1 == prtMDebugFlag )
        writeLog ( tMDebugLog, "%s|%s|%d:----[%s] thread exit\n",
                   __FILE__, __func__, __LINE__, "IEventManager" );

    sleep ( 1 );

    //初始化
    //C_MSysInit::Instance()->stop();
    C_MSysInit::Instance()->clear();

    if ( 1 == prtMDebugFlag )
        writeLog ( tMDebugLog, "%s|%s|%d:----[%s] thread exit\n",
                   __FILE__, __func__, __LINE__, "C_MSysInit" );

    sleep ( 1 );


    if ( 1 == prtMDebugFlag )
        writeLog ( tMDebugLog, "%s|%s|%d:catch th signal:[%d]------out------\n",
                   __FILE__, __func__, __LINE__, sig );

    end = 1;

}

int main ( void )
{
        int vRet = 0;
    //    int prtFlag = 0;



    //取环境变量得shell程序,进行程序多次运行限制
    char* pidNumSh_p = NULL;
    pidNumSh_p = getenv ( MC_GETPIDNUM );

    if ( pidNumSh_p == NULL )
    {
        printf ( "%s|%s|%d:  getenv [%s] return NULL error\n",
                 __FILE__, __func__, __LINE__, MC_GETPIDNUM );
        return -2;
    }

    int num = executeShell ( pidNumSh_p, "%s", "ThrMeteM" );

    if ( num > 1 )
    {
        printf ( "%s|%s|%d:[%s]\n",
                 __FILE__, __func__, __LINE__, "程序已经运行，不能重复运行!" );
        return 0;
    }

    char tErrMsg[ 512 + 1 ];
    memset ( tErrMsg, 0, sizeof( tErrMsg ) );
    //获得启动开始时间
    char vStartTime[30];
    memset ( vStartTime, 0, sizeof ( vStartTime ) );
    vRet = getTime ( 4, vStartTime, NULL, tErrMsg );
    if ( vRet != 0 )
    {
        printf ( "%s|%s|%d:  getTime return error, vRet=[%d] tErrMsg=[%s]\n",
                 __FILE__, __func__, __LINE__, vRet, tErrMsg );
        return -100;

    }

    //获取工作程序的工作目录
    char tCwdName[ MC_MAX_PWDLEN + 1 ];
    memset ( tCwdName, 0, sizeof ( tCwdName ) );
    char* cwdNmae_p = getcwd( tCwdName, MC_MAX_PWDLEN );
    if ( cwdNmae_p == NULL )
    {
        printf ( "%s|%s|%d:  getcwd MC_MAX_PWDLEN=[%d] return NULL errmsg=[%s]\n",
                 __FILE__, __func__, __LINE__, MC_MAX_PWDLEN, strerror(errno) );
        return -3;
        
    }

    //存储版本号的文件名
    char tVersionFile[ MC_MAX_PWDLEN + 1 ];
    memset ( tVersionFile, 0, sizeof ( tVersionFile ) );
    snprintf ( tVersionFile , sizeof( tVersionFile ), "%s/../version", tCwdName );


    //取写版本日志的shWriteLog.sh脚本路径，此路径与pidNumSh_p一致
    char tShDirName[ MC_MAX_PWDLEN + 1 ];
    memset ( tShDirName, 0, sizeof( tShDirName ) );
    vRet = sptDirName ( pidNumSh_p,
                        tShDirName,
                        NULL,
                        tErrMsg,
                        1
                      );
    if ( vRet != 0 )
    {
        printf ( "%s|%s|%d:  sptDirName [%s] return error,vRet=[%d], errmsg=[%s]\n",
                 __FILE__, __func__, __LINE__, pidNumSh_p, vRet, tErrMsg );
        return -4;

    }

    char tShName[ MC_MAX_PWDLEN + 1 ];
    memset ( tShName, 0, sizeof( tShName ) );
    snprintf( tShName, sizeof( tShName ), "%sshWriteLog.sh",tShDirName );
    //判断shWriteLog.sh是否存在
    vRet = fileExists ( tShName , tErrMsg );
    if ( vRet != 0 )
    {
        printf ( "%s|%s|%d:  fileExists [%s] return error,vRet=[%d], errmsg=[%s]\n",
                 __FILE__, __func__, __LINE__, tShName, vRet, tErrMsg );
        return -5;

    }
    //写程序版本号
    char tVerNo[ 100 + 1 ];
    memset ( tVerNo, 0, sizeof ( tVerNo ) );
    strcpy( tVerNo, "此软件版本号为: V10.01.000");
    char tVerCont[ 400 + 1 ];
    memset ( tVerCont, 0, sizeof ( tVerCont ) );
    snprintf( tVerCont, sizeof( tVerCont ), 
            "\n\t编译日期:%s\n\t编译时间:%s\n\t%s\n\t软件启动开始时间:%s\n\n",
            __DATE__, __TIME__, tVerNo, vStartTime );
    char tStr[1024];
    memset ( tStr, 0, sizeof ( tStr ) );
    snprintf( tStr, sizeof( tStr ),
            "%s %s 1 '%s'",tShName, tVersionFile, tVerCont );
   vRet = system_plus( tStr,
                        100, /*100秒之后将kill脚本*/ 
                        0,   /*超时后将尝试0次*/
                        1,  /*超时后将kill脚本及脚本派生的进程*/
                        tErrMsg,
                        sizeof( tErrMsg )
                    );
   if ( vRet < 0 || vRet == 127 )
   {
        printf ( "%s|%s|%d:  system_plus [%s] return error,vRet=[%d], errmsg=[%s]\n",
                 __FILE__, __func__, __LINE__, tStr, vRet, tErrMsg );

       return -6;
   }


    memset ( tMDebugLog, 0, sizeof ( tMDebugLog ) );

    OSAL_Syslog_Enable ( OSAL_SYS_LOG_NORMAL );
    OSAL_Syslog_Enable ( OSAL_SYS_LOG_WARNING );
    OSAL_Syslog_Enable ( OSAL_SYS_LOG_ERROR );

    //初始化 加载本地xml
    //C_MSysInit::Instance()->start();
    C_MSysInit::Instance()->init();

    //获取是否打印debug配置 和 日志文件配置
    prtMDebugFlag = C_MSysInit::Instance()->getPrtFlag();
    snprintf ( tMDebugLog, sizeof ( tMDebugLog ),
               "%s",
               C_MSysInit::Instance()->getMeteLog().fullOtherLog );

    //事件管理线程
    IEventManager::Initialize()->start();
    while ( IEventManager::Initialize()->mThreadFD == 0 )
    {
        sleepMilliSecond( 50 );
    }

    //错误处理线程
    C_MErrHandling::Instance()->start();
    while ( C_MErrHandling::Instance()->mThreadFD == 0 )
    {
        sleepMilliSecond( 50 );
    }

    //健康报告生成线程
    C_MHealthRep::Instance()->start();
    while ( C_MHealthRep::Instance()->mThreadFD == 0 )
    {
        sleepMilliSecond( 50 );
    }

    //ftp下载线程
    C_MFtpDown::Instance()->start();
    while ( C_MFtpDown::Instance()->mThreadFD == 0 )
    {
        sleepMilliSecond( 50 );
    }

    //业务清单合成处理线程
    C_MFsyBList::Instance()->start();
    while ( C_MFsyBList::Instance()->mThreadFD == 0 )
    {
        sleepMilliSecond( 50 );
    }

    //业务清单提供情况线程
    C_MStatBList::Instance()->start();
    while ( C_MStatBList::Instance()->mThreadFD == 0 )
    {
        sleepMilliSecond( 50 );
    }

    //ftp上传处理线程
    C_MPutFtp::Instance()->start();
    while ( C_MPutFtp::Instance()->mThreadFD == 0 )
    {
        sleepMilliSecond( 50 );
    }

    //文件监视处理线程
    C_MMonitor::Instance()->start();
    while ( C_MMonitor::Instance()->mThreadFD == 0 )
    {
        sleepMilliSecond( 50 );
    }

    signal ( SIGTSTP, signal_handler );
    signal ( SIGQUIT, signal_handler );
    signal ( SIGINT, signal_handler );
    signal ( SIGKILL, signal_handler );
    signal ( SIGTERM, signal_handler );

    while ( !end )
    {
        sleep ( 5 );
    }

    if ( 1 == prtMDebugFlag )
    {
        writeLog ( tMDebugLog, "%s|%s|%d:  okokok\n", __FILE__, __func__, __LINE__ );
    }

    return 0;


}
