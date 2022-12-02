#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

#include "IEventManager.h"
#include "FileMonitor.h"
#include "FileReceiver.h"
#include "InfraBase.h"
#include "PvCfg.h"
#include "CustomOutLog.h"


int main()
{

    pid_t tid;
    tid = syscall(SYS_gettid);
    c_write_log(_INFO,"thread id[%d]", tid);

    int ret = 0;

    const char* sftCfgFile = GetPvRunPar( RUNPAR_CFGFILE );  //私有配置文件
    const char* pubCfgFile = GetPvPubFile( PV_PUBCFG_FILE ); //公共配置文件
    const char* sftVerFile = GetPvRunPar( RUNPAR_VERFILE );  //软件版本文件

    c_write_log(_DEBUG,"pubCfgFile[%s],sftCfgFile[%s],sftVerFile[%s]", 
            pubCfgFile, sftCfgFile, sftVerFile );

    //将当前程序版本号用minIni开源方法写入程序版本ini文件
    // ini_puts  return: 1:成功   0:失败
    ret = ini_puts( "cur_version", "ver_no", "v20.03.050", sftVerFile ); 
    if ( ret != 1 )
    {
        c_write_log(_ERROR,"ini_puts(%s)run ret=[%d]", sftVerFile, ret );
        return 1;
    }

    //加载配置文件 
    ret = PvCfgInitialize();
    if ( ret != PVCFG_SUCCESS )
    {
        c_write_log(_DEBUG,"PvCfgInitialize()run ret=[%s]", PvCfgErrMsg(ret) );

        return 1;

    }

    //启动事件管理线程
    IEventManager::Initialize()->start();

    //响应文件监视器的处理线程
    FileReceiver::Instance()->start( IEventManager::isRun );

    //启动文件监视器线程
    FileMonitor::Instance()->start( FileReceiver::isRun );

    //将公共配置文件添加到文件监视器中进行监视
    FileMonitor::Instance()->addWatch( 
            PvCfg::mPubCfgPath.c_str(),         /*路径*/
            (EVENT_MOVE_TO | EVENT_CLOSEWRITE), /*类型*/
            PvCfg::mPubCfgFile.c_str()          /*文件名*/
            );

    //将软件私有配置文件添加到文件监视器中进行监视
    FileMonitor::Instance()->addWatch( 
            PvCfg::mSftCfgPath.c_str(),
            (EVENT_MOVE_TO | EVENT_CLOSEWRITE),
            PvCfg::mSftCfgFile.c_str()
            );


    std::string tCfgVal;

    //取公共配置文件: 域[INFO] key:faultstate 的值
    ret = GetPvPubCfgVal( tCfgVal, "INFO", "faultstate" );

    c_write_log(_DEBUG,"GetPvPubCfgVal [INFO]->faultstates=[%s] ret=[%s]", 
            tCfgVal.c_str(), PvCfgErrMsg(ret) );

    //取私有配置文件: 域[ROOT] key:loglevel 的值
    ret = GetPvSftCfgVal( tCfgVal, "ROOT", "loglevel" );
    c_write_log(_DEBUG,"GetPvSftCfgVal [ROOT]->loglevel=[%s] ret=[%s]", 
            tCfgVal.c_str(), PvCfgErrMsg(ret) );


    while(1)
    {
        PauseThreadSleep( 10, 0 );

        ret = GetPvPubCfgVal( tCfgVal, "INFO", "faultstate" );
        c_write_log(_DEBUG,"GetPvPubCfgVal [INFO]->faultstates=[%s] ret=[%s]", 
                tCfgVal.c_str(), PvCfgErrMsg(ret) );

        ret = GetPvSftCfgVal( tCfgVal, "ROOT", "loglevel" );
        c_write_log(_DEBUG,"GetPvSftCfgVal [ROOT]->loglevel=[%s] ret=[%s]", 
                tCfgVal.c_str(), PvCfgErrMsg(ret) );
    }

    return 0;
}
