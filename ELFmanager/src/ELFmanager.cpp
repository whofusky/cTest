/***********************************************************
 *
 * @file    ELFmanager.cpp
 *
 * @brief   ELFmanager源文件
 *
 * @author  fu.sky
 *
 * @date    2022-11-11_13:32:38
 *
 * @version V10.010.000
 *
 ***********************************************************/

#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <signal.h>

#include "ELFmanager.h"
#include "CustomOutLog.h"
#include "InfraBase.h"




//不同查询条件定义
#define  ACCORD_FULLNAME     1  /*根据配置中全路径文件名匹配*/
#define  ACCORD_PREFIXNAME   2  /*根据配置中全路径的前缀匹配*/
#define  ACCORD_ALLNAME      3  /*根据配置中所有的文件*/

//定义停止运行程序时是否删除运行信息
#define mArg1_HOLD_RUNINFO    0  /*保留运行信息*/
#define mArg1_DEL_RUNINFO     1  /*删除运行信息*/


//添加程序监视时，参数中域的序号
//  run_name|cfgstat|stdlogflag|cfginter|runprefix|runpar
enum init_pdomain_num
{
    INI_PDM_runname = 0,
    INI_PDM_cfgstat,
    INI_PDM_stdlogflag,
    INI_PDM_cfginter,
    INI_PDM_runprefix,
    INI_PDM_runpar,
    
    INIT_PDOMAIN_NUM
};




static pthread_mutex_t  gsMutex = PTHREAD_MUTEX_INITIALIZER;

int ELFmanager::isRun = 0;
ELFmanager* ELFmanager::mInstance = NULL;




ELFmanager::ELFTimer::ELFTimer(ELFmanager* ins, bool autoRelease): CTimer( autoRelease)
{
    mInsELF = ins;
}

ELFmanager::ELFTimer::~ELFTimer()
{

}


ELFmanager::ELFTimer* ELFmanager::ELFTimer::getObj(ELFmanager* ins, bool autoRelease)
{
    return new ELFTimer( ins, autoRelease);
}


void ELFmanager::ELFTimer::timerHandler()
{

    //c_write_log(_DEBUG,"fusktest:timerHandler begine");
    //tvcstop:记录正在停止中状态的程序,以便在自启动队列中状态为
    //  自启动状态不同时发启动消息
    std::vector<std::string> tvcstop;

    std::map< std::string, int >::iterator itT;

    CMutexGuard tGuard(mInsELF->mCLock);

    mInsELF->mTLock.Lock();
    for( itT = mInsELF->mTmpProc.begin();
            itT != mInsELF->mTmpProc.end();
            itT++
       )
    {
        int tsta  = ELFHandler::MSG_TMP_STOP;
        int mArg1 = mArg1_HOLD_RUNINFO;

        if ( RUN_STA_DELETING == itT->second )
        {
            mArg1 = mArg1_DEL_RUNINFO;
        }

        if ( RUN_STA_STOPING == itT->second )
        {
            //记录正在停止中状态的程序,以便在自启动队列中状态为
            //  自启动状态不同时发启动消息
            tvcstop.push_back( itT->first );
        }

        //sendNoAttrMsg( msgWhat, mArg1, metaStr )
        mInsELF->sendNoAttrMsg( tsta, mArg1, itT->first);

    }

    mInsELF->mTLock.unLock();

    std::map< std::string, p_c_info* >::iterator it;

    //mInsELF->mCLock.Lock();

    for( it = mInsELF->mPCInfo.begin();
            it != mInsELF->mPCInfo.end();
            it++
       )
    {

        //正在停止中的程序不发启动消息
        if ( std::find(tvcstop.begin(), tvcstop.end(), it->first ) != tvcstop.end() )
        {
            continue;
        }

        p_c_info* tpcinfo = it->second;
        if ( tpcinfo->cfgstat == CFG_STA_AUTOS )
        {
            //sendStartMsg( msgWhat, metaStr, p_c_info *)
            mInsELF->sendStartMsg( ELFHandler::MSG_CHK_START, it->first, tpcinfo ); 
        }
        else if ( tpcinfo->cfgstat == CFG_STA_STOPS ) 
        {
            //发消息更新运行消息

            int tsta  = ELFHandler::MSG_UPDATE_INFO;
            int mArg1 = mArg1_HOLD_RUNINFO;
            //sendNoAttrMsg( msgWhat, mArg1, metaStr )
            mInsELF->sendNoAttrMsg( tsta, mArg1, it->first);
        }

    }
    //mInsELF->mCLock.unLock();


    return;
}


ELFmanager::ELFHandler::ELFHandler( ELFmanager* ins )
{
    mInsELF = ins;
}

ELFmanager::ELFHandler::~ELFHandler()
{

}



void ELFmanager::ELFHandler::handleMessage( Message& msg )
{
    int msgType = msg.mWhat;

    switch( msgType )
    {
        case MSG_STAR_PRO: 
        case MSG_CHK_START: 
            {
                int stdlogflag        = msg.mAttr["stdlogflag"].asInt();  //标准输出日志标识
                int cfginter          = msg.mAttr["cfginter"].asInt();    //两次重启时间间隔
                std::string exeName   = msg.mAttr["fname"].asString();    //程序名(不带路径)
                std::string exePath   = msg.mAttr["path"].asString();     //路径运行绝对路径
                std::string runprefix = msg.mAttr["runprefix"].asString();//程序运行前缀
                std::string runpar    = msg.mAttr["runpar"].asString();   //程序运行参数

                if ( exeName.empty() ) return;
                if ( exePath.empty() ) return;

                std::string fullPname = exePath + "/" + exeName;

                //检查程序是否已经启动
                PNAME_RET_INFO tpidinfo;
                int ret = ELFcmd::getPidByFullName( fullPname, tpidinfo );
                if ( -1 == ret )
                {
                    c_write_log(_ERROR,"ELFcmd::getPidByFullName open /proc error");
                    return;
                }
                if ( 1 == ret )
                {
                    c_write_log(_ERROR,"The number of programs [%s] "
                            "running at the same time exceeds [%d]",
                            fullPname.c_str(), SAMEEXE_MAX_PIDS );
                }

                //同路径下同路径名的程序运行了多个,则把所有进程都结束，等待下次
                //定时器对此程序进行启动操作
                if ( tpidinfo.curnum > 1 )
                {
                    c_write_log(_ERROR,"The number of programs [%s] "
                            "running at the same time exceeds [%d],"
                            "and all programs are about to end!",
                            exeName.c_str(), 1 );
                    for( int i = 0; i < tpidinfo.curnum; i++ )
                    {
                        //kill ( tpidinfo.pt[i].pid, SIGKILL );
                        kill ( tpidinfo.pt[i].pid, SIGTERM );
                        c_write_log(_DEBUG,
                                "Send a SIGTERM signal to process[%d] to terminate",
                                tpidinfo.pt[i].pid );
                    }
                    return;
                }

                p_r_info truninfo;

                //获取程序的现有运行信息
                mInsELF->getPRInfo( fullPname, truninfo );

                //启动程序
                if ( 0 == tpidinfo.curnum )
                {
                    //自启动状态中如设置了启动次数限制且达到限制的则设置相应状态
                    //并退出
                    if ( msgType == MSG_CHK_START 
                            && mInsELF->mStarThreshold > 0
                            && truninfo.startnum >= mInsELF->mStarThreshold
                       )
                    {
                        //修改配置状态以便不发自动重启消息
                        mInsELF->setAutoByFlag( 
                                ACCORD_FULLNAME, 
                                CFG_STA_STARTERR, 
                                fullPname );
                        truninfo.runstat = RUN_STA_STARERR;
                        truninfo.stopnum = 0;
                        truninfo.pidnum = 0;
                        memset( truninfo.pid, 0, sizeof( truninfo.pid ) );
                        memset( truninfo.pf, 0, sizeof( truninfo.pf ) );
                        mInsELF->setPRInfo( fullPname, &truninfo );

                        c_write_log(_ERROR,
                                "start [%s] program failed,close auto restart!",
                                fullPname.c_str() );
                        return;
                    }


                    time_t curTimeT;
                    time( &curTimeT );
                    //判断两次重启是否满足最少时间间隔要求
                    if ( curTimeT - truninfo.timestamp < cfginter )
                    {
                        c_write_log(_DEBUG,
                                "The time interval between two "
                                "restarts of program [%s] is less than "
                                "[%d] seconds", 
                                fullPname.c_str(), cfginter );

                        return;
                    }
                    
                    std::string runStr;
                    genRunStr( exePath, exeName, runprefix, runpar, stdlogflag, 
                            runStr );

                    int timeout = 0;
                    int times = 0;
                    int killAllFlag = 1;

                    ret = ELFcmd::system_plus( 
                            runStr.c_str(), 
                            timeout, 
                            times,
                            killAllFlag );

                    c_write_log(_INFO,
                            "Call the interface system_plus startup program "
                            "[%s] and return [%d]", 
                            runStr.c_str(), ret );  

                    char stardate[9] = {0};
                    getFormatTime(stardate,9,"%Y%m%d");

                    truninfo.runstat = RUN_STA_STARING;

                    truninfo.timestamp = curTimeT;

                    //最多只记录一天中不断重启连续失败的次数
                    if ( strcmp(truninfo.stardate, stardate) != 0 )
                    {
                        truninfo.startnum = 1;
                        strcpy(truninfo.stardate, stardate);
                    }
                    else
                    {
                        truninfo.startnum++;
                    }
                    truninfo.stopnum = 0;
                    truninfo.pidnum = 0;
                    memset( truninfo.pid, 0, sizeof( truninfo.pid ) );
                    memset( truninfo.pf, 0, sizeof( truninfo.pf ) );

                }
                else //刷新程序运行状态信息
                {
                    truninfo.runstat = RUN_STA_RUNING;
                    truninfo.pidnum = 1;
                    truninfo.pid[0] = tpidinfo.pt[0].pid;
                    if ( truninfo.startnum != 0 ) truninfo.startnum = 0;

                    //获取当前进程人工可读模式的进程状态信息到truninfo.pf[0]
                    ret = getHRPSInfo( truninfo.pid[0], truninfo.pf[0] );
                }

                //将程序运行信息写入内存
                mInsELF->setPRInfo( fullPname, &truninfo );
            }
            break;

        case MSG_TMP_STOP: 
        case MSG_STOP_PRO: 
            {
                int mArg1             = msg.mArg1;
                std::string fullPname = msg.mMetaStr;

                std::string exeName;
                std::string exePath;
                ELFcmd::get_path_fname( fullPname, exePath, exeName );

                p_r_info truninfo;

                //获取程序的现有运行信息
                mInsELF->getPRInfo( fullPname, truninfo );

                //检查程序是否已经启动
                PNAME_RET_INFO tpidinfo;
                int ret = ELFcmd::getPidByFullName( fullPname, tpidinfo );
                if ( -1 == ret )
                {
                    c_write_log(_ERROR,"ELFcmd::getPidByFullName open /proc error");
                    return;
                }
                if ( -2 == ret ) //没有进程
                {
                    // 如果要求删除运行信息则进行删除
                    if ( mArg1_DEL_RUNINFO == mArg1 )
                    {
                        mInsELF->delTmpProc( fullPname );
                        mInsELF->delPRInfo( fullPname );
                        mInsELF->delCfgInfo( fullPname );
                        return;
                    }

                    truninfo.runstat = RUN_STA_STOP;
                    truninfo.pidnum  = 0;
                    truninfo.stopnum = 0;
                    memset( truninfo.pid, 0, sizeof( truninfo.pid ) );
                    memset( truninfo.pf, 0, sizeof( truninfo.pf ) );

                    if ( MSG_TMP_STOP == msgType )
                    {
                        // 如果临时状态信息队列有此程序则进行删除
                        mInsELF->delTmpProc( fullPname );
                    }

                }
                else //有进程
                {
                    if ( MSG_TMP_STOP == msgType 
                            && mInsELF->mStopThreshold > 0
                            && truninfo.stopnum >= mInsELF->mStopThreshold )
                    {
                        //停止进程还结束不掉则结束尝试
                        mInsELF->delTmpProc( fullPname );
                        c_write_log(_ERROR,"Stop [%s] program failed",
                                fullPname.c_str() );
                        truninfo.runstat = RUN_STA_STOPRERR;
                        //将程序运行信息写入内存
                        mInsELF->setPRInfo( fullPname, &truninfo );
                        return;
                    }

                    for( int i = 0; i < tpidinfo.curnum; i++ )
                    {
                        if ( truninfo.stopnum <= 1 )//前两次(0,1)正常SIGTERM
                        {
                            kill ( tpidinfo.pt[i].pid, SIGTERM );
                            c_write_log(_DEBUG, 
                                    "Send a SIGTERM signal to process[%d] "
                                    "to terminate",
                                    tpidinfo.pt[i].pid );
                        }
                        else//第3次开始用SIGKILL
                        {
                            kill ( tpidinfo.pt[i].pid, SIGKILL );
                            c_write_log(_DEBUG, 
                                    "Send a SIGKILL signal to process[%d] "
                                    "to terminate",
                                    tpidinfo.pt[i].pid );
                        }
                    }

                    truninfo.runstat = RUN_STA_STOPING;

                    if ( MSG_TMP_STOP != msgType )
                    {
                        int tstat = RUN_STA_STOPING;
                        if ( mArg1_DEL_RUNINFO == mArg1 )
                        {
                            tstat = RUN_STA_DELETING;
                        }
                        //将上面发送终止信息后程序记录mTmpProc以便下一定时器周期对其
                        // 进行确定是否已经终止程序
                        mInsELF->setTmpProc( fullPname, tstat );
                    }

                    truninfo.stopnum++;
                }

                //将程序运行信息写入内存
                mInsELF->setPRInfo( fullPname, &truninfo );
            }
            break;

        case MSG_UPDATE_INFO: 
            {
                std::string fullPname = msg.mMetaStr;

                std::string exeName;
                std::string exePath;
                ELFcmd::get_path_fname( fullPname, exePath, exeName );

                p_r_info truninfo;

                //获取程序的现有运行信息
                int ret = mInsELF->getPRInfo( fullPname, truninfo );
                if ( ret < 0 ) return;

                //检查程序是否已经启动
                PNAME_RET_INFO tpidinfo;
                ret = ELFcmd::getPidByFullName( fullPname, tpidinfo );
                if ( -1 == ret )
                {
                    c_write_log(_ERROR,"ELFcmd::getPidByFullName open /proc error");
                    return;
                }
                if ( -2 == ret ) //没有进程
                {

                    truninfo.runstat = RUN_STA_STOP;
                    truninfo.pidnum  = 0;
                    truninfo.stopnum = 0;
                    memset( truninfo.pid, 0, sizeof( truninfo.pid ) );
                    memset( truninfo.pf, 0, sizeof( truninfo.pf ) );
                }
                else //有进程 刷新运行数据
                {
                    //truninfo.runstat = RUN_STA_RUNING;
                    truninfo.pidnum = tpidinfo.curnum;
                    for( int i = 0; i < truninfo.pidnum; i++ )
                    {
                        truninfo.pid[i] = tpidinfo.pt[i].pid;

                        //获取当前进程人工可读模式的进程状态信息到truninfo.pf[i]
                        ret = getHRPSInfo( truninfo.pid[i], truninfo.pf[i] );
                    }
                }

                //将程序运行信息写入内存
                mInsELF->setPRInfo( fullPname, &truninfo );
            }
            break;

        default:
            break;

    }

    return;
}






ELFmanager::ELFmanager()
{
    mTmSec    = 5;
    mTmMsec   = 0;
    mTmRepet  = -1;
    mHandler  = NULL;
    mLooper   = NULL;
    mTimer    = NULL;
    mStarThreshold = 0;
    mStopThreshold = 0;

}


ELFmanager::~ELFmanager()
{

}


//拆分固定格式入参到配置信息(支持|的变化,如果域不足则用默认值)
//  run_name|cfgstat|stdlogflag|cfginter|runprefix|runpar
void ELFmanager::splitStrToCfg( std::string &cfg, 
        std::string &fullExeName, 
        p_c_info *pcInfo
        )
{
    fullExeName = "";


    if ( pcInfo != NULL )
    {
        pcInfo->cfgstat    = CFG_STA_INIT;
        pcInfo->stdlogflag = 0;
        pcInfo->cfginter   = 5;
        pcInfo->runprefix  = "";
        pcInfo->runpar     = "";
    }

    cfg.erase( 0, cfg.find_first_not_of(" ") );
    cfg.erase( cfg.find_last_not_of(" ") + 1 );
    if ( cfg.empty() ) return;

    std::string ts[INIT_PDOMAIN_NUM];
    std::string::size_type head = 0;
    std::string::size_type tail = cfg.find( "|", head );
    std::string::size_type rpos = cfg.rfind( "|");
    if ( rpos == cfg.npos )
    {
        fullExeName = cfg;
        TrimString( fullExeName );
        if ( pcInfo != NULL )
        {
            ELFcmd::get_path_fname( fullExeName, pcInfo->path, pcInfo->fname );
            TrimString( pcInfo->path );
            TrimString( pcInfo->fname );
        }
        return;
    }

    if ( NULL == pcInfo ) return;

    int i = 0;
    while ( tail != cfg.npos && i < INIT_PDOMAIN_NUM )
    {
        ts[i++] = cfg.substr( head, tail - head );
        head = tail + 1;
        if ( rpos == tail  ) break;
        tail = cfg.find( "|", head );
    }

    if ( i < INIT_PDOMAIN_NUM ) 
    {
        if ( head == cfg.size() )
        {
            ts[i++] = "";
        }
        else
        {
            ts[i++] = cfg.substr( head );
        }
    }

    if ( i > INI_PDM_runname )
    {
        fullExeName = ts[INI_PDM_runname];
        TrimString( fullExeName );
        if ( pcInfo != NULL )
        {
            ELFcmd::get_path_fname( fullExeName, pcInfo->path, pcInfo->fname );
            TrimString( pcInfo->path );
            TrimString( pcInfo->fname );
        }
    }

    int tval = 0;

    if( i > INI_PDM_cfgstat )
    {
        tval = atoi(ts[INI_PDM_cfgstat].c_str());
        if ( tval < CFG_STA_INIT || tval >= CFG_STA_UNKNOWN ) 
        {
            tval = CFG_STA_UNKNOWN;
        }
        pcInfo->cfgstat = tval;
    }

    if ( i > INI_PDM_stdlogflag )
    {
        tval = atoi(ts[INI_PDM_stdlogflag].c_str());
        if ( tval != 0 && tval != 1 ) tval = 0;
        pcInfo->stdlogflag = tval;
    }

    if ( i > INI_PDM_cfginter )
    {
        tval = atoi(ts[INI_PDM_cfginter].c_str());
        if ( tval > 0 ) pcInfo->cfginter = tval;
    }

    if ( i > INI_PDM_runprefix )
    {
        pcInfo->runprefix = ts[INI_PDM_runprefix];
        pcInfo->runpar = ts[INI_PDM_runpar];
        TrimString( pcInfo->runprefix );
        TrimString( pcInfo->runpar );
    }

    return;
}



// 添加需要被监视程序的配置信息
// 每一个程序的配置信息格式如下:(至少有run_name,其他域如果没有可不带,多个域
// 需要|分隔,缺少的域程序内容使用默认值)
//  run_name|cfgstat|stdlogflag|cfginter|runprefix|runpar
//   默认值为: run_name|0|0|5||
//  其中和各部分的含义如下
//    run_name          /*带绝对路径的程序名*/
//    cfgstat;          /*程序配置的状态*/
//    stdlogflag;       /*程序是否把输出到标准输出的日志保存到特殊日志文件*/
//    cfginter;         /*配置的程序再次重启间隔时间(单位秒)*/
//    runprefix;        /*程序运行需要的前缀:一般默认为空*/
//    runpar;           /*程序运行需要的参数:一般默认为空*/
int ELFmanager::addCfgInfo( std::vector<std::string> &cfgInfo)
{
    if ( cfgInfo.size() == 0 ) return 0;
    for ( std::string::size_type i = 0; i < cfgInfo.size(); i++ )
    {
        p_c_info* tpcinf_p = new p_c_info();
        if ( NULL == tpcinf_p  )
        {
            c_write_log(_ERROR," new p_c_info return NULL");
            return -1;
        }
        std::string tpath;
        splitStrToCfg( cfgInfo[i], tpath, tpcinf_p);

        mCLock.Lock();
        std::map< std::string, p_c_info* >::iterator it;
        it = mPCInfo.find ( tpath );
        if ( it == mPCInfo.end() )
        {
            mPCInfo.insert( make_pair( tpath, tpcinf_p )); 
        }
        else
        {
            p_c_info* toldinf_p = it->second;
            delete toldinf_p;
            it->second = tpcinf_p;
        }
        mCLock.unLock();
    }

    return 0;
}



// 删除某个程序的配置和运行信息(同时结束此程序的运行,如果程序是运行状态)
//   此接口只是向ELFmanger主进程发消息进行处理，是否处理成功需要调用相关
//   查询接口进行查询；
//其中导致删除失败的一种常见原因是: 停止在运行的程序时停止不掉导致不能对
//   程序的信息进行删除
//
//  fullExeName: 带绝对路径的程序名
void ELFmanager::delExeInfo( const std::string &fullExeName )
{
    if ( fullExeName.empty() ) return;

    mCLock.Lock();

    std::map< std::string, p_c_info* >::iterator it;
    it = mPCInfo.find ( fullExeName );

    if ( it == mPCInfo.end() ) 
    {
        mCLock.unLock();
        return;
    }

    p_c_info* tpcinfo_p = it->second;
    tpcinfo_p->cfgstat = CFG_STA_STOPS;
    delTmpProc( fullExeName );

    //sendNoAttrMsg( msgWhat, mArg1, metaStr )
    sendNoAttrMsg( ELFHandler::MSG_STOP_PRO, mArg1_DEL_RUNINFO, fullExeName );

    mCLock.unLock();

    return;
}



// 往mTmpProc中添加信息
int ELFmanager::setTmpProc( const std::string &fullExeName, const int &stat)
{
    if ( fullExeName.empty() ) return 0;

    mTLock.Lock();

    std::map< std::string, int >::iterator it;
    it = mTmpProc.find ( fullExeName );
    if ( it == mTmpProc.end() )
    {
        mTmpProc.insert( make_pair( fullExeName, stat )); 
    }
    else
    {
        if ( stat != it->second )
        {
            it->second = stat;
        }
    }

    mTLock.unLock();

    return 0;
}

// 删除mTmpProc中信息
int ELFmanager::delTmpProc( const std::string &fullExeName )
{
    mTLock.Lock();

    std::map< std::string, int >::iterator it;
    it = mTmpProc.find ( fullExeName );

    if ( it == mTmpProc.end() ) 
    {
        mTLock.unLock();
        return 0;
    }

    mTmpProc.erase(it);
    mTLock.unLock();
    return 0;
}


// 删除某个程序的配置信息
//  fullExeName: 带绝对路径的程序名
int ELFmanager::delCfgInfo( const std::string &fullExeName )
{
    if ( fullExeName.empty() ) return 0;

    mCLock.Lock();

    std::map< std::string, p_c_info* >::iterator it;
    it = mPCInfo.find ( fullExeName );

    if ( it == mPCInfo.end() ) 
    {
        mCLock.unLock();
        return 0;
    }

    p_c_info* tpcinfo_p = it->second;

    delete tpcinfo_p;

    mPCInfo.erase(it);

    mCLock.unLock();

    return 0;
}


int ELFmanager::setPRInfo( const std::string &fullExeName, p_r_info* rinfo )
{
    if ( fullExeName.empty() ) return 0;

    mRLock.Lock();

    std::map< std::string, p_r_info* >::iterator it;
    it = mPRInfo.find ( fullExeName );
    if ( it == mPRInfo.end() )
    {
        p_r_info* tprinfo_p = (p_r_info*) calloc(1,sizeof(p_r_info));
        if ( NULL == tprinfo_p )
        {
            c_write_log(_ERROR,"calloc p_r_info return NULL");
            mRLock.unLock();
            return -1;
        }
        if ( rinfo != NULL )
        {
            memcpy( tprinfo_p, rinfo, sizeof(p_r_info));
        }

        mPRInfo.insert( make_pair( fullExeName, tprinfo_p )); 
    }
    else
    {
        if ( rinfo != NULL )
        {
            memcpy( it->second, rinfo, sizeof(p_r_info) );
        }
    }

    mRLock.unLock();
    return 0;
}


int ELFmanager::delPRInfo( const std::string &fullExeName )
{
    mRLock.Lock();

    std::map< std::string, p_r_info* >::iterator it;
    it = mPRInfo.find ( fullExeName );

    if ( it == mPRInfo.end() ) 
    {
        mRLock.unLock();
        return 0;
    }

    p_r_info* tprinfo_p = it->second;
    free( tprinfo_p );

    mPRInfo.erase(it);

    mRLock.unLock();

    return 0;
}


//获取程序的配置信息
// -1 没有找到配置信息
// 0  成功
int ELFmanager::getPCInfo( const std::string &fullExeName, p_c_info &cinfo )
{
    mCLock.Lock();

    std::map< std::string, p_c_info* >::iterator it;
    it = mPCInfo.find ( fullExeName );

    if ( it == mPCInfo.end() ) 
    {
        cinfo.cfgstat    = 0;
        cinfo.stdlogflag = 0;
        cinfo.cfginter   = 0;
        cinfo.path       = "";
        cinfo.fname      = "";
        cinfo.runprefix  = "";
        cinfo.runpar     = "";

        mCLock.unLock();
        return -1;
    }

    cinfo.cfgstat    = it->second->cfgstat;
    cinfo.stdlogflag = it->second->stdlogflag;
    cinfo.cfginter   = it->second->cfginter;
    cinfo.path       = it->second->path;
    cinfo.fname      = it->second->fname;
    cinfo.runprefix  = it->second->runprefix;
    cinfo.runpar     = it->second->runpar;

    mCLock.unLock();

    return 0;
}


//获取程序的运行信息
//  fullExeName: 带绝对路径的程序名
//  return:
//         -1 没有找到配置信息
//         0  成功
int ELFmanager::getPRInfo( const std::string &fullExeName, p_r_info &rinfo )
{
    mRLock.Lock();

    std::map< std::string, p_r_info* >::iterator it;
    it = mPRInfo.find ( fullExeName );

    if ( it == mPRInfo.end() ) 
    {
        memset(&rinfo, 0, sizeof(p_r_info) );
        rinfo.runstat = RUN_STA_STOP;
        mRLock.unLock();
        return -1;
    }

    memcpy( &rinfo, it->second, sizeof(p_r_info) );

    mRLock.unLock();

    return 0;
}


unsigned char ELFmanager::start( 
        int timerFixSeconds,
        int starHold,
        int stopHold,
        const int &pre_val )
{
    if ( mHandler != NULL && mLooper != NULL && mTimer != NULL )
    {
        return True;
    }

    if ( pre_val != -1 )
    {
        while( 0 == pre_val )
        {
            PauseThreadSleep( 0, 10 );
        }
        c_write_log(_DEBUG,"Wait for the prepend to end!");
    }

    if( mHandler == NULL )
    {
        mHandler = new ELFHandler( this );
        if ( mHandler == NULL )
        {
            c_write_log(_ERROR,"new ELFHandler return NULL");
            return False;
        }
    }

    int needStarTmFlag = 0;
    if ( mTimer == NULL )
    {
        bool autoReleaseFlag = false;
        if ( mTmRepet > 0 )  autoReleaseFlag = true;
        mTimer = ELFTimer::getObj( this, autoReleaseFlag );
        if ( mTimer == NULL )
        {
            c_write_log(_ERROR,"ELFTimer::getObj return NULL");
            return False;
        }
        if ( timerFixSeconds > 0 && timerFixSeconds != mTmSec )
        {
            mTmSec = timerFixSeconds;
        }
        mTimer->setTimer( mTmSec, mTmMsec, mTmRepet );
        needStarTmFlag = 1;
        if ( starHold > 0 ) mStarThreshold = starHold;
        if ( stopHold > 0 ) mStopThreshold = stopHold;

    }

    if ( 0 == isRun )
    {
        int ret = startThread();
        if ( 0 == ret )
        {
            c_write_log(_ERROR,"startThread return error");
            return False;
        }
    }

    c_write_log(_DEBUG,"needStarTmFlag[%d],isRun[%d]!", needStarTmFlag, isRun);
    if ( 1 == needStarTmFlag )
    {
        while( 0 == isRun )
        {
            PauseThreadSleep( 0, 10 );
        }
        c_write_log(_DEBUG,"Wait for startThread to end!");
        mTimer->startTimer();

    }

    c_write_log(_DEBUG,"ELFmanager::start() Done");

    return True;
}


unsigned char ELFmanager::stop( )
{
    //注意Looper与线程释放的顺序，不要颠倒
    stopThread();
    if ( mLooper != NULL )
    {
        mLooper->decRef();
    }

    if ( mHandler != NULL )
    {
        delete mHandler;
        mHandler = NULL;
    }
    
    if ( mTimer != NULL )
    {
        mTimer->stopTime();
        delete mTimer;
        mTimer = NULL;
    }

    return True;
}



void ELFmanager::threadHandler()
{
    pid_t tid;
    tid = syscall(SYS_gettid);
    c_write_log(_INFO,"thread id[%d]", tid);

    mLooper = Looper::getLooper();
    if ( mLooper == NULL ) return;
    mLooper->incRef();
    mLooper->prepare();
    
    isRun = 1;

    mLooper->Loop();

    return;
}


ELFmanager* ELFmanager::Instance()
{
    if ( mInstance == NULL )
    {
        pthread_mutex_lock( &gsMutex );
        if ( mInstance == NULL )
        {
            mInstance = new ELFmanager();
        }
        pthread_mutex_unlock( &gsMutex );
    }

    return mInstance;
}

//生成启动程序需要的字符串
void ELFmanager::genRunStr( 
        const std::string &exePath, 
        const std::string &exeName, 
        const std::string &runprefix,
        const std::string &runpar,
        const int &stdlogflag, 
        std::string &outStr )
{
    outStr = "cd " + exePath + std::string(" && nohup");

    if ( ! runprefix.empty() ) outStr += " " + runprefix;

    outStr += " ./" + exeName;

    if ( ! runpar.empty() ) outStr += " " + runpar;
    
    std::string stdlog = " >/dev/null 2>&1 ";
    if ( std_log_save_on == stdlogflag )
    {
        std::string logdir = exePath + std::string("/ttylog/");
        ELFcmd::mkdirs( logdir );
        std::string logfile = logdir + exeName + std::string(".log");
        stdlog = " >>" + logfile + std::string(" 2>&1");
    }

    outStr += stdlog + std::string(" &");

    return ;
}


// get human readable progress stauts info
int ELFmanager::getHRPSInfo( int pid, p_s_info &retInfo )
{
    char tmpc[100] = {0};
    memset( &retInfo, 0, sizeof(p_s_info) );
    snprintf( tmpc,sizeof(tmpc),"%d",pid);

    std::string pfname = "/proc/";
    pfname += tmpc + std::string("/status");

    std::string strVal[8];
    int inoutnum = 7;

    // 取/proc/pid/status文件的内容
    //  str[0] Name   进程名
    //  str[1] State  进程的状态
    //  str[2] PPid   进程的父进程
    //  str[3] VmSize 进程现在正在占用的内存
    //  str[4] VmPeak 当前进程运行过程中占用内存的峰值
    //  str[5] VmRSS  是程序现在使用的物理内存
    //  str[6] VmHWM  是程序得到分配到物理内存的峰值
    int ret = ELFcmd::getPidStatus( pfname.c_str(), strVal, inoutnum );
    if ( ret != 0 )
    {
        c_write_log(_ERROR,"ELFcmd::getPidStatus return[%d]", ret);
        return ret;
    }
    snprintf( retInfo.name, sizeof(retInfo.name),
            "%s", strVal[0].c_str() );
    snprintf( retInfo.state, sizeof(retInfo.state),
            "%s", strVal[1].c_str() );
    retInfo.ppid = atoi( strVal[2].c_str() );
    snprintf( retInfo.vmsize, sizeof(retInfo.vmsize),
            "%s", strVal[3].c_str() );
    snprintf( retInfo.vmpeak, sizeof(retInfo.vmpeak),
            "%s", strVal[4].c_str() );
    snprintf( retInfo.vmrss, sizeof(retInfo.vmrss),
            "%s", strVal[5].c_str() );
    snprintf( retInfo.vmhwm, sizeof(retInfo.vmhwm),
            "%s", strVal[6].c_str() );

    return 0;
}


void ELFmanager::TrimString( std::string &instr )
{
    if( instr.empty() ) return;
    //去前后空格
    instr.erase( 0, instr.find_first_not_of(" ") );
    instr.erase( instr.find_last_not_of(" ") + 1 );
    return;
}


//打开fullExeName的自启动
// -1 没有找到配置的程序
// 0  成功
int ELFmanager::openAutoStart( const std::string &fullExeName )
{
    return setAutoByFlag( ACCORD_FULLNAME, CFG_STA_AUTOS, fullExeName );
}


//打开所有配置的自启动
int ELFmanager::openAllAutoStart()
{
    return setAutoByFlag( ACCORD_ALLNAME, CFG_STA_AUTOS );
}


//打开配置的所有程序中路径前缀是prefix的自启动
// -1 没有找到配置的程序
// 0  成功
int ELFmanager::openPrefixAutoStart( const std::string &prefix )
{
    return setAutoByFlag( ACCORD_PREFIXNAME, CFG_STA_AUTOS, prefix );
}


//关闭fullExeName的自启动
// -1 没有找到配置的程序
// 0  成功
int ELFmanager::closeAutoStart( const std::string &fullExeName )
{
    return setAutoByFlag( ACCORD_FULLNAME, CFG_STA_STOPS, fullExeName );
}


//关闭所有配置的自启动
int ELFmanager::closeAllAutoStart()
{
    return setAutoByFlag( ACCORD_ALLNAME, CFG_STA_STOPS );
}


//关闭配置的所有程序中路径前缀是prefix的自启动
// -1 没有找到配置的程序
// 0  成功
int ELFmanager::closePrefixAutoStart( const std::string &prefix )
{
    return setAutoByFlag( ACCORD_PREFIXNAME, CFG_STA_STOPS, prefix );
}




//发送启动fullExeName的消息(如果程序自启动开关是关闭的状态同时打开程序的自启动)
// -1 没有找到配置的程序
// 0  成功发送启动的消息，但程序是否启动需要查询程序的运行状态
int ELFmanager::startExe( const std::string &fullExeName )
{
    return startExeByFlag( ACCORD_PREFIXNAME, fullExeName );
}

//发送启动消息给配置的所有程序路径前缀是prefix的程序
// -1 没有找到配置的程序
// 0  成功发送消息
int ELFmanager::startExeByPrefix( const std::string &prefix )
{
    return startExeByFlag( ACCORD_PREFIXNAME, prefix );
}


//发送启动消息给所有配置的程序
int ELFmanager::startExeAll()
{
    return startExeByFlag( ACCORD_ALLNAME );
}




//  fullExeName: 带绝对路径的程序名
// -1 没有找到配置的程序
// 0  成功发送停止的消息，但程序是否停止需要查询程序的运行状态
int ELFmanager::stopExe( const std::string &fullExeName )
{
    return stopExeByFlag( ACCORD_FULLNAME, fullExeName);
}


// prefix 带绝对路径的程序名的前缀
// -1 没有找到配置的程序
// 0  成功发送消息
int ELFmanager::stopExeByPrefix( const std::string &prefix )
{
    return stopExeByFlag( ACCORD_PREFIXNAME, prefix);
}


//发送停止消息给所有配置的程序
int ELFmanager::stopExeAll()
{
    return stopExeByFlag( ACCORD_ALLNAME );
}




// flag: 1, 根据可执行程序全路径名设置某一个程序
//       2, 根据可执行程序全路径名的前缀来设置所有匹配的程序
//       3, 设置所有配置的程序
//       ACCORD_FULLNAME   1
//       ACCORD_PREFIXNAME 2
//       ACCORD_ALLNAME    3
int ELFmanager::setAutoByFlag( 
        const int flag, 
        const int autoVal, 
        const std::string &sstr )
{
    if ( flag != ACCORD_FULLNAME 
            && flag != ACCORD_PREFIXNAME 
            && flag != ACCORD_ALLNAME )
    {
        return -2;
    }

    if ( autoVal < CFG_STA_INIT ||  autoVal > CFG_STA_UNKNOWN )
    {
        return -3;
    }

    if ( ( ACCORD_FULLNAME == flag || ACCORD_PREFIXNAME == flag ) 
            && sstr.empty() 
       )
    {
        return 0;
    }

    int findFlag = 0;

    mCLock.Lock();

    std::map< std::string, p_c_info* >::iterator it;
    for( it = mPCInfo.begin();
            it != mPCInfo.end();
            it++
       )
    {
        if ( ACCORD_FULLNAME == flag && it->first != sstr )
        {
            continue;
        }
        else if ( ACCORD_PREFIXNAME == flag )
        {
            std::string::size_type len = sstr.size();
            if ( 0 != sstr.compare( 
                        0, len, 
                        it->first, 0, len )
               )
            {
                continue;
            }
        }

        p_c_info* tpcinfo = it->second;

        if ( tpcinfo->cfgstat != autoVal )
        {
            tpcinfo->cfgstat = autoVal;
        }

        findFlag = 1;
        
        //全路径名的只能有一匹配的,此处的break避免无谓的消耗
        if ( ACCORD_FULLNAME == flag ) break;
    }

    mCLock.unLock();

    //根据文件名或前缀查找时没有找到配置返回-1
    if ( (ACCORD_FULLNAME == flag || ACCORD_PREFIXNAME == flag ) 
            && 0 == findFlag 
       ) 
    {
        return -1;
    }

    return 0;
}




//发送启动程序的消息
void ELFmanager::sendStartMsg( 
        const int msgWhat, 
        const std::string &metaStr, 
        const p_c_info *tpcinfo )
{
    Json::Value tAttr;
    tAttr["stdlogflag"] = tpcinfo->stdlogflag;
    tAttr["cfginter"]   = tpcinfo->cfginter;
    tAttr["path"]       = tpcinfo->path;
    tAttr["fname"]      = tpcinfo->fname;
    tAttr["runprefix"]  = tpcinfo->runprefix;
    tAttr["runpar"]     = tpcinfo->runpar;

    Message msg;
    msg.mWhat         = msgWhat;
    msg.mMetaStr      = metaStr;
    msg.mAttr         = tAttr;
    msg.mTarget       = mHandler;
    msg.mTargetLooper = mLooper;
    msg.setValid( true );

    mHandler->sendMessage( msg );

    return;
}




// flag: 1, 根据可执行程序全路径名执行某一个程序
//       2, 根据可执行程序全路径名的前缀来启动所有匹配的程序
//       3, 启动所有配置的程序
//       ACCORD_FULLNAME   1
//       ACCORD_PREFIXNAME 2
//       ACCORD_ALLNAME    3
int ELFmanager::startExeByFlag(const int flag, const std::string &sstr )
{
    if ( flag != ACCORD_FULLNAME 
            && flag != ACCORD_PREFIXNAME 
            && flag != ACCORD_ALLNAME )
    {
        return -2;
    }

    if ( ( ACCORD_FULLNAME == flag || ACCORD_PREFIXNAME == flag ) 
            && sstr.empty() 
       )
    {
        return 0;
    }

    int findFlag = 0;

    mCLock.Lock();

    std::map< std::string, p_c_info* >::iterator it;
    for( it = mPCInfo.begin();
            it != mPCInfo.end();
            it++
       )
    {
        if ( ACCORD_FULLNAME == flag && it->first != sstr )
        {
            continue;
        }
        else if ( ACCORD_PREFIXNAME == flag )
        {
            std::string::size_type len = sstr.size();
            if ( 0 != sstr.compare( 
                        0, len, 
                        it->first, 0, len )
               )
            {
                continue;
            }
        }

        p_c_info* tpcinfo = it->second;

        //sendStartMsg( msgWhat, metaStr, p_c_info *)
        sendStartMsg( ELFHandler::MSG_STAR_PRO, it->first, tpcinfo ); 

        if ( tpcinfo->cfgstat != CFG_STA_AUTOS )
        {
            tpcinfo->cfgstat = CFG_STA_AUTOS;
        }
        delTmpProc( it->first );

        findFlag = 1;
        
        //全路径名的只能有一匹配的,此处的break避免无谓的消耗
        if ( ACCORD_FULLNAME == flag ) break;
    }

    mCLock.unLock();

    //根据文件名或前缀查找时没有找到配置返回-1
    if ( (ACCORD_FULLNAME == flag || ACCORD_PREFIXNAME == flag ) 
            && 0 == findFlag 
       ) 
    {
        return -1;
    }

    return 0;
}




//发送停止程序的消息
void ELFmanager::sendNoAttrMsg( 
        const int msgWhat, 
        const int mArg1, 
        const std::string &metaStr )
{
    Message msg;
    msg.mWhat         = msgWhat;
    msg.mArg1         = mArg1;
    msg.mMetaStr      = metaStr;
    msg.mTarget       = mHandler;
    msg.mTargetLooper = mLooper;
    msg.setValid( true );

    mHandler->sendMessage( msg );
    return;
}




// flag: 1, 根据可执行程序全路径名停止某一个程序
//       2, 根据可执行程序全路径名的前缀来停止所有匹配的程序
//       3, 停止所有配置的程序
//       ACCORD_FULLNAME   1
//       ACCORD_PREFIXNAME 2
//       ACCORD_ALLNAME    3
int ELFmanager::stopExeByFlag(const int flag, const std::string &sstr )
{
    if ( flag != ACCORD_FULLNAME 
            && flag != ACCORD_PREFIXNAME 
            && flag != ACCORD_ALLNAME )
    {
        return -2;
    }

    if ( ( ACCORD_FULLNAME == flag || ACCORD_PREFIXNAME == flag ) 
            && sstr.empty() 
       )
    {
        return 0;
    }

    int findFlag = 0;

    mCLock.Lock();

    std::map< std::string, p_c_info* >::iterator it;
    for( it = mPCInfo.begin();
            it != mPCInfo.end();
            it++
       )
    {
        if ( ACCORD_FULLNAME == flag && it->first != sstr )
        {
            continue;
        }
        else if ( ACCORD_PREFIXNAME == flag )
        {
            std::string::size_type len = sstr.size();
            if ( 0 != sstr.compare( 
                        0, len, 
                        it->first, 0, len )
               )
            {
                continue;
            }
        }

        p_c_info* tpcinfo = it->second;

        if ( tpcinfo->cfgstat != CFG_STA_STOPS )
        {
            tpcinfo->cfgstat = CFG_STA_STOPS;
        }

        //sendNoAttrMsg( msgWhat, mArg1, metaStr )
        sendNoAttrMsg( ELFHandler::MSG_STOP_PRO, mArg1_HOLD_RUNINFO, it->first ); 
        findFlag = 1;
        
        //全路径名的只能有一匹配的,此处的break避免无谓的消耗
        if ( ACCORD_FULLNAME == flag ) break;
    }

    mCLock.unLock();

    //根据文件名或前缀查找时没有找到配置返回-1
    if ( (ACCORD_FULLNAME == flag || ACCORD_PREFIXNAME == flag ) 
            && 0 == findFlag 
       ) 
    {
        return -1;
    }

    return 0;
}




int ELFM_addCfgInfo( std::vector<std::string> &cfgInfo )
{
    return ELFmanager::Instance()->addCfgInfo( cfgInfo );
}

void ELFM_delExeInfo( const std::string &fullExeName )
{
    return ELFmanager::Instance()->delExeInfo( fullExeName );
}

unsigned char ELFM_start( int timerFixSeconds, int starHold, int stopHold, const int &pre_val )
{
    return ELFmanager::Instance()->start(timerFixSeconds, starHold, stopHold, pre_val);
}

unsigned char ELFM_stop()
{
    return ELFmanager::Instance()->stop();
}

int ELFM_getPCInfo( const std::string &fullExeName, p_c_info &cinfo )
{
    return ELFmanager::Instance()->getPCInfo( fullExeName, cinfo );
}

int ELFM_getPRInfo( const std::string &fullExeName, p_r_info &rinfo )
{
    return ELFmanager::Instance()->getPRInfo( fullExeName, rinfo );
}

int ELFM_startExe( const std::string &fullExeName )
{
    return ELFmanager::Instance()->startExe( fullExeName );
}

int ELFM_startExeByPrefix( const std::string &prefix )
{
    return ELFmanager::Instance()->startExeByPrefix( prefix );
}

int ELFM_startExeAll()
{
    return ELFmanager::Instance()->startExeAll();
}

int ELFM_stopExe( const std::string &fullExeName )
{
    return ELFmanager::Instance()->stopExe( fullExeName );
}

int ELFM_stopExeByPrefix( const std::string &prefix )
{
    return ELFmanager::Instance()->stopExeByPrefix( prefix );
}

int ELFM_stopExeAll()
{
    return ELFmanager::Instance()->stopExeAll();
}

int ELFM_openAutoStart( const std::string &fullExeName )
{
    return ELFmanager::Instance()->openAutoStart( fullExeName );
}

int ELFM_openPrefixAutoStart( const std::string &prefix )
{
    return ELFmanager::Instance()->openPrefixAutoStart( prefix );
}

int ELFM_openAllAutoStart()
{
    return ELFmanager::Instance()->openAllAutoStart();
}

int ELFM_closeAutoStart( const std::string &fullExeName )
{
    return ELFmanager::Instance()->closeAutoStart( fullExeName );
}

int ELFM_closePrefixAutoStart( const std::string &prefix )
{
    return ELFmanager::Instance()->closePrefixAutoStart( prefix );
}

int ELFM_closeAllAutoStart()
{
    return ELFmanager::Instance()->closeAllAutoStart();
}













