/***********************************************************
 *
 * @file    ELFmanager.h
 *
 * @brief   ELFmanager头文件
 *
 * @author  fu.sky
 *
 * @date    2022-11-11_13:32:38
 *
 * @version V10.010.000
 *
 ***********************************************************/

#ifndef _ELF_MANAGER_H_
#define _ELF_MANAGER_H_

#include <string>
#include <map>
#include <vector>
#include "Mutex.h"
#include "Thread.h"
#include "Timer.h"
#include "Handler.h"
#include "ELFcmd.h"
#include "Looper.h"





#define SAMEEXE_SPATH_PIDS 5  /*同程序名的在同一个路径下可运行的的pid最大个数*/
#define PID_STATU_LEN 20      /*取进程可读信息字符串最大长度*/








//程序配置的自启动状态
typedef enum  
{
    CFG_STA_INIT = 0,  /*初始化状态*/
    CFG_STA_STARTERR,  /*自启动失败(重启到一定次数后还未成功)*/
    CFG_STA_AUTOS,     /*自启动监视状态*/
    CFG_STA_STOPS,     /*停止自启动监视状态*/
    CFG_STA_UNKNOWN

}cfg_status;


//stdlogflag 标准输出日志是否保存
typedef enum
{
    std_log_save_off = 0,  /*不保存*/
    std_log_save_on        /*保存*/
}STD_LOG_FLAG;


//配置的程序自启动参数
typedef struct
{
    int cfgstat;           /*程序配置的自启动状态*/
    int stdlogflag;        /*程序是否把输出到标准输出的日志保存到特殊日志文件*/
    int cfginter;          /*配置的程序再次重启间隔时间(单位秒)*/
    std::string path;      /*程序路径*/
    std::string fname;     /*程序名*/
    std::string runprefix; /*程序运行需要的前缀:一般默认为空*/
    std::string runpar;    /*程序运行需要的参数:一般默认为空*/
}p_c_info;


//此处的结构是存储/proc/pid/status中信息
//  其中的信息展现形式为: human readable
//  例如:
//   进程状态也是带解读模式的如: S (sleeping)
//   其中内存信息是带单位的如: 184 kB
typedef struct
{
       char name[PID_STATU_LEN];   /*进程名*/
       char state[PID_STATU_LEN];  /*进程的状态*/
       int  ppid;                  /*进程的父进程*/
       char vmsize[PID_STATU_LEN]; /*进程现在正在占用的内存*/
       char vmpeak[PID_STATU_LEN]; /*当前进程运行过程中占用内存的峰值*/
       char vmrss[PID_STATU_LEN];  /*是程序现在使用的物理内存*/
       char vmhwm[PID_STATU_LEN];  /*是程序得到分配到物理内存的峰值*/
}p_s_info;


//被管理程序的运行的状态
typedef enum  
{
    RUN_STA_INIT = 0,  /*初始化状态*/
    RUN_STA_STARING,   /*正在启动中状态*/
    RUN_STA_STARERR,   /*启动失败状态*/
    RUN_STA_RUNING,    /*运行状态*/
    RUN_STA_STOPING,   /*正在停止中状态*/
    RUN_STA_DELETING,  /*正在停止并删除运行信息状态中*/
    RUN_STA_STOPRERR,  /*停止失败状态*/
    RUN_STA_STOP,      /*停止状态*/

}run_status;


//被管理程序的运行信息
typedef struct
{
    int  runstat;                    /*程序运行状态*/
    char stardate[9];                /*程序启动的日期:YYYYMMDD*/
    long timestamp;                  /*程序最近一次启动的时间戳*/
    int  startnum;                   /*程序在一天中重启的次数*/
    int  stopnum;                    /*对程序进行停止操作重试的次数*/
    int  pidnum;                     /*程序的进程数量*/
    int  pid[SAMEEXE_SPATH_PIDS];    /*程序pid值*/
    p_s_info pf[SAMEEXE_SPATH_PIDS]; /*pid对应的信息*/
}p_r_info;








/*************************************************************************
 *******          添加自启动(被管理)的程序
 ******* 
 *************************************************************************/
// 入参vector的每一个元素的完全格式如下:
// 
//  runname|cfgstat|stdlogflag|cfginter|runprefix|runpar
//
//  说明: runname是必须要有的,其他域如果没有可不带,多个域需要|分隔
//       缺少的域程序内容使用默认值
//       默认值为: runname|0|0|5||
//
//  其中和各部分的含义如下
//    runname           /*带绝对路径的程序名*/
//    cfgstat;          /*程序配置的状态*/
//    stdlogflag;       /*程序是否把输出到标准输出的日志保存到特殊日志文件*/
//    cfginter;         /*配置的程序再次重启间隔时间(单位秒)*/
//    runprefix;        /*程序运行需要的前缀:一般默认为空*/
//    runpar;           /*程序运行需要的参数:一般默认为空*/
int ELFM_addCfgInfo( std::vector<std::string> &cfgInfo );




/*************************************************************************
 *******          启动 或 停止 主程序(管理程序)接口 (其中包括定时器的启动)
 ******* 
 *************************************************************************/
//  timerFixSeconds 定时器的周期时间(单位秒)
//  starHold  自启动失败判断的阈值(0:不判断, 大于0:当自启动大于等于此次数时判断为失败)
//  stopHold  停止失败判断的阈值(0:不判断, 大于0:当停止程序次数大于等于此次数时判断为失败)
unsigned char ELFM_start( int timerFixSeconds = 2, int starHold = 0, int stopHold = 0, const int &pre_val = -1 );
unsigned char ELFM_stop();




/*************************************************************************
 *******          删除不再需要的被管理的程序
 ******* 
 *************************************************************************/
// 删除某个程序的配置和运行信息(同时结束此程序的运行,如果程序是运行状态)
//   此接口只是向ELFmanger主进程发消息进行处理，是否处理成功需要调用相关
//   查询接口进行查询；
//其中导致删除失败的一种常见原因是: 停止在运行的程序时停止不掉导致不能对
//   程序的信息进行删除
//
//  fullExeName: 带绝对路径的程序名
void ELFM_delExeInfo( const std::string &fullExeName );




/*************************************************************************
 *******          查询接口
 ******* 
 *************************************************************************/
//获取程序的的自启动配置信息
//  fullExeName: 带绝对路径的程序名
//  return:
//         -1 没有找到配置信息
//         0  成功
int ELFM_getPCInfo( const std::string &fullExeName, p_c_info &cinfo );


//获取程序的运行状态信息
//  fullExeName: 带绝对路径的程序名
//  return:
//         -1 没有找到配置信息
//         0  成功
int ELFM_getPRInfo( const std::string &fullExeName, p_r_info &rinfo );






/*************************************************************************
 *******          启动某个或某一类程序
 *******     启动程序的同时打开程序的自启动开关
 *******     此接口只是给管理程序发消息，被管理程序的具体状态需要调用查询接口
 *******     获得
 *************************************************************************/

//  fullExeName: 带绝对路径的程序名
//  return:
//         -1 没有找到配置信息
//         0  成功
int ELFM_startExe( const std::string &fullExeName );

//  prefix 带绝对路径的程序名的前缀
//  return:
//         -1 没有找到配置信息
//         0  成功
int ELFM_startExeByPrefix( const std::string &prefix );

//发送启动消息给所有配置的程序
int ELFM_startExeAll();







/*************************************************************************
 *******          停止某个或某一类程序
 *******     关闭程序的同时关闭程序的自启动开关 
 *******     此接口只是给管理程序发消息，被管理程序的具体状态需要调用查询接口
 *******     获得
 *************************************************************************/

//  fullExeName: 带绝对路径的程序名
//  return:
//         -1 没有找到配置信息
//         0  成功
int ELFM_stopExe( const std::string &fullExeName );

//  prefix 带绝对路径的程序名的前缀
//  return:
//         -1 没有找到配置信息
//         0  成功
int ELFM_stopExeByPrefix( const std::string &prefix );

//发送停止消息给所有配置的程序
int ELFM_stopExeAll();




/*************************************************************************
 *******打开程序自启动开关***
 *************************************************************************/

//  fullExeName: 带绝对路径的程序名
//  return:
//         -1 没有找到配置信息
//         0  成功
int ELFM_openAutoStart( const std::string &fullExeName );

//  prefix 带绝对路径的程序名的前缀
//  return:
//         -1 没有找到配置信息
//         0  成功
int ELFM_openPrefixAutoStart( const std::string &prefix );

//打开所有配置的自启动
int ELFM_openAllAutoStart();




/*************************************************************************
 *******关闭程序自启动开关***
 *************************************************************************/

//  fullExeName: 带绝对路径的程序名
//  return:
//         -1 没有找到配置信息
//         0  成功
int ELFM_closeAutoStart( const std::string &fullExeName );

//  prefix 带绝对路径的程序名的前缀
//  return:
//         -1 没有找到配置信息
//         0  成功
int ELFM_closePrefixAutoStart( const std::string &prefix );

//关闭所有配置的自启动
int ELFM_closeAllAutoStart();












class ELFmanager: public CThread
{

private:
    ELFmanager();
    ~ELFmanager();


private:
    static ELFmanager* mInstance;
    //static const char* _errorName[PVCFG_ERROR_COUNT+1];
    //

    //拆分固定格式入参到配置信息
    static void splitStrToCfg( 
            std::string &cfg, 
            std::string &fullExeName, 
            p_c_info *pcInfo );


public:
    static int isRun;
    static ELFmanager* Instance();
    //static const char* ErrorIdName(int errorId);

    //生成启动程序需要的字符串
    static void genRunStr( 
            const std::string &exePath, 
            const std::string &exeName, 
            const std::string &runprefix,
            const std::string &runpar,
            const int &stdlogflag, 
            std::string &outStr );
    // get human readable progress stauts info
    static int getHRPSInfo( int pid, p_s_info &retInfo );

    static void TrimString( std::string &instr );


public:

    /*************************************************************************
     *******添加监视程序的接口***
     *************************************************************************/

    // 添加需要被监视程序的配置信息
    // 每一个程序的配置信息格式如下:(至少有runname,其他域如果没有可不带,多个域
    // 需要|分隔,缺少的域程序内容使用默认值)
    //  runname|cfgstat|stdlogflag|cfginter|runprefix|runpar
    //   默认值为: runname|0|0|5||
    //  其中和各部分的含义如下
    //    runname          /*带绝对路径的程序名*/
    //    cfgstat;          /*程序配置的状态*/
    //    stdlogflag;       /*程序是否把输出到标准输出的日志保存到特殊日志文件*/
    //    cfginter;         /*配置的程序再次重启间隔时间(单位秒)*/
    //    runprefix;        /*程序运行需要的前缀:一般默认为空*/
    //    runpar;           /*程序运行需要的参数:一般默认为空*/
    int addCfgInfo( std::vector<std::string> &cfgInfo);

    // 删除某个程序的配置和运行信息(同时结束此程序的运行,如果程序是运行状态)
    //   此接口只是向ELFmanger主进程发消息进行处理，是否处理成功需要调用相关
    //   查询接口进行查询；
    //其中导致删除失败的一种常见原因是: 停止在运行的程序时停止不掉导致不能对
    //   程序的信息进行删除
    //
    //  fullExeName: 带绝对路径的程序名
    void delExeInfo( const std::string &fullExeName );




    //启动或停止主程序及定时器接口
    //timerFixSeconds 定时器的周期时间(单位秒)
    //starHold  自启动失败判断的阈值(0:不判断, 大于0:当自启动大于等于此次数时判断为失败)
    //stopHold  停止失败判断的阈值(0:不判断, 大于0:当停止程序次数大于等于此次数时判断为失败)
    unsigned char start( int timerFixSeconds = 2, int starHold = 0, int stopHold = 0, const int &pre_val = -1 );
    unsigned char stop();
    

    //获取程序的的配置信息
    //  fullExeName: 带绝对路径的程序名
    //  return:
    //         -1 没有找到配置信息
    //         0  成功
    int getPCInfo( const std::string &fullExeName, p_c_info &cinfo );



    //获取程序的运行信息
    //  fullExeName: 带绝对路径的程序名
    //  return:
    //         -1 没有找到配置信息
    //         0  成功
    int getPRInfo( const std::string &fullExeName, p_r_info &rinfo );



    /*************************************************************************
     ****启动监视程序接口:如果程序自启动开关是关闭的状态同时打开程序的自启动***
     *******  此接口返回0表示发消息成功,但程序实际的状态需要调用查询接口查询***
     *************************************************************************/

    //  fullExeName: 带绝对路径的程序名
    //  return:
    //         -1 没有找到配置信息
    //         0  成功
    int startExe( const std::string &fullExeName );

    //  prefix 带绝对路径的程序名的前缀
    //  return:
    //         -1 没有找到配置信息
    //         0  成功
    int startExeByPrefix( const std::string &prefix );

    //发送启动消息给所有配置的程序
    int startExeAll();



    /*************************************************************************
     *******关闭程序接口:如果程序自启动开关是打开的状态同时关闭程序的自启动***
     *******  此接口返回0表示发消息成功,但程序实际的状态需要调用查询接口查询***
     *************************************************************************/

    //  fullExeName: 带绝对路径的程序名
    //  return:
    //         -1 没有找到配置信息
    //         0  成功
    int stopExe( const std::string &fullExeName );

    //  prefix 带绝对路径的程序名的前缀
    //  return:
    //         -1 没有找到配置信息
    //         0  成功
    int stopExeByPrefix( const std::string &prefix );

    //发送停止消息给所有配置的程序
    int stopExeAll();



    /*************************************************************************
     *******打开程序自启动开关***
     *************************************************************************/

    //  fullExeName: 带绝对路径的程序名
    //  return:
    //         -1 没有找到配置信息
    //         0  成功
    int openAutoStart( const std::string &fullExeName );

    //  prefix 带绝对路径的程序名的前缀
    //  return:
    //         -1 没有找到配置信息
    //         0  成功
    int openPrefixAutoStart( const std::string &prefix );

    //打开所有配置的自启动
    int openAllAutoStart();


    /*************************************************************************
     *******关闭程序自启动开关***
     *************************************************************************/

    //  fullExeName: 带绝对路径的程序名
    //  return:
    //         -1 没有找到配置信息
    //         0  成功
    int closeAutoStart( const std::string &fullExeName );

    //  prefix 带绝对路径的程序名的前缀
    //  return:
    //         -1 没有找到配置信息
    //         0  成功
    int closePrefixAutoStart( const std::string &prefix );

    //关闭所有配置的自启动
    int closeAllAutoStart();




private:
    int mTmSec;    //定时器周期时间:秒
    int mTmMsec;   //定时器周期时间:微秒
    int mTmRepet;  //重复标识:<0 重复定时器; >0 定时器重复的次数

    class ELFTimer: public CTimer
    {
        public:
            ELFTimer( ELFmanager* ins, bool autoRelease);
            ~ELFTimer();

            static ELFTimer* getObj( ELFmanager* ins, bool autoRelease );
        private:
            virtual void timerHandler();
            ELFmanager* mInsELF;
    };


private:
    class ELFHandler: public Handler 
    {
        public:
            ELFHandler( ELFmanager* ins);
            ~ELFHandler();
            typedef enum elf_handler_msg
            {
                MSG_CHK_START,
                MSG_STAR_PRO,
                MSG_STOP_PRO,
                MSG_TMP_STOP,
                MSG_UPDATE_INFO
            }ELF_HANDLER_MSG;
            void handleMessage( Message& msg );
        private:
            ELFmanager* mInsELF;
    };

    void threadHandler();
    Handler*   mHandler;
    Looper*    mLooper;
    ELFTimer*  mTimer;


private:

    // 删除某个程序的配置数据
    //  fullExeName: 带绝对路径的程序名
    int delCfgInfo( const std::string &fullExeName );

    // 往mPRInfo中添加信息
    int setPRInfo( const std::string &fullExeName, p_r_info* rinfo = NULL );
    // 删除mPRInfo中信息
    int delPRInfo( const std::string &fullExeName );

    // 往mTmpProc中添加信息
    int setTmpProc( const std::string &fullExeName, const int &stat);
    // 删除mTmpProc中信息
    int delTmpProc( const std::string &fullExeName );

    //发送启动程序的消息
    void sendStartMsg( 
            const int msgWhat, 
            const std::string &metaStr, 
            const p_c_info *tpcinfo );

    //发送停止程序的消息
    void sendNoAttrMsg( 
            const int msgWhat, 
            const int mArg1,
            const std::string &metaStr );

    // flag: 1, 根据可执行程序全路径名设置某一个程序
    //       2, 根据可执行程序全路径名的前缀来设置所有匹配的程序
    //       3, 设置所有配置的程序
    int setAutoByFlag(const int flag, const int autoVal, const std::string &sstr="" );

    // flag: 1, 根据可执行程序全路径名执行某一个程序
    //       2, 根据可执行程序全路径名的前缀来启动所有匹配的程序
    //       3, 启动所有配置的程序
    int startExeByFlag(const int flag, const std::string &sstr="" );

    // flag: 1, 根据可执行程序全路径名停止某一个程序
    //       2, 根据可执行程序全路径名的前缀来停止所有匹配的程序
    //       3, 停止所有配置的程序
    int stopExeByFlag(const int flag, const std::string &sstr="" );

private:

    CMutex     mCLock;
    CMutex     mTLock;
    CMutex     mRLock;
    std::map< std::string, p_c_info* > mPCInfo; //配置信息
    std::map< std::string, int > mTmpProc;      //中间临时处理信息
    std::map< std::string, p_r_info* > mPRInfo; //运行信息
    int mStarThreshold;   //判定启动失败的阈值(0不判断,大于0才判断)
    int mStopThreshold;   //判定停止程序失败的阈值(0不判断,大于0才判断)

private:
    class Delector
    {
        public:
            ~Delector()
            {
                if ( ELFmanager::mInstance != NULL )
                {
                    delete ELFmanager::mInstance;
                }
            }
    };

    static Delector delector;

};








#endif//_ELF_MANAGER_H_
