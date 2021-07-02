/**********************************************************
* name:       mete_ftpDown.h
*
* desc:      气象下载器的，ftp下载
*
* date:      20170726173653
*
***********************************************************/

#ifndef METE_FTPDOWN_H
#define METE_FTPDOWN_H

#include "base/Base.h"
#include "thread/Thread.h"
#include "thread/Handler.h"
#include "thread/Looper.h"
#include "Timer/Timer.h"

#include "mete_datatype.h"



class C_MFtpDown: public CThread
{
    private:
        C_MFtpDown();
        ~C_MFtpDown();

    private:
        static C_MFtpDown* cInstance_p;//全局对象

    private:
        void threadHandler(); //来自CThread的虚函数 启动线程时调用
        void init();//给不在构造函数里完成的变量进行初始化
        void sendError ( const Json::Value&  errMsg ); //发送异常信息

    public:
        static C_MFtpDown* Instance();//用于产生或获取全局实例对象
        void start(); //启动应用对象
        void stop() ; //停止并删除应用对象

    private:
        int          prtDebugFlag;  //内部接口是否打印debug日志, 1 打印,0不打印
        char         fullOtherLog[MC_FULLN_LEN]; //其他程序日志名
        char*        getPidNumPath_p;
        T_ftpParas   ftpParas[MC_FTPSERNUM_MAX]; //加载ftp操作需要的配置变量
        T_doFtpSh    shNames[MC_FTPSERNUM_MAX];  //加载进行ftp操作需要的shell
        u_int        validFlag[MC_FTPSERNUM_MAX];//与ftpParas对应的是否有效flag
        int          numFtpSer;//配置的ftp server数量
        std::string  appName;  //健康报告中被监视者名称
        Json::Value  healthVal;//健康报告中其他变量

    private: //Handler  调用shell脚本下载文件
        char downingFlag;//是否正在下载标识;1:正在下载
        typedef enum mete_down_ftp_msg
        {
            METE_DOWN_MSG
        } METE_DOWN_FTP_MSG;
        class C_MFtpDownHandler: public Handler
        {
            public:
                C_MFtpDownHandler ( C_MFtpDown* ftpDown_p );
                ~C_MFtpDownHandler();

                //来自Handler的虚函数，实现此函数实现取出消息后的处理逻辑
                void handleMessage ( Message msg );

            private:
                C_MFtpDown* cFtpDown_p;

        };

    private: //CTimer 定时查询是否有下载进程在运行，如果没有则发消息启动脚本进行下载

        //定时器控制变量
        int second   ; //秒
        int micSecond; //毫秒
        int repet    ; //重复标识:<0 定时器无限循环，>0定时器只运行repet次

        class C_DownFtpTimer: public CTimer
        {
            public:
                C_DownFtpTimer ( C_MFtpDown* mFtpDown_p,
                                 C_MFtpDownHandler* handler_p,
                                 Message msg,
                                 bool autoRelaseFlag );
                ~C_DownFtpTimer();

            public:
                static C_DownFtpTimer* getObj (
                C_MFtpDown* mFtpDown_p,
                C_MFtpDownHandler* handler_p,
                Message msg,
                bool autoRelaseFlag
                );

            private:
                virtual void timerHandler();

                Message            cMsg;
                C_MFtpDownHandler* cHandler_p;
                C_MFtpDown*        cMFtpDown_p;
        };

    private:
        C_MFtpDownHandler* cHandler_p;//C_MFtpDown 消息队列处理handler
        Looper*            cLooper_p ;//C_MFtpDown 自己消息队列处理looper
        C_DownFtpTimer*    cTimer_p  ;//定时检查下载timer


};


#endif // METE_FTPDOWN_H
