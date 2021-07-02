/**********************************************************
* name:       mete_ftpPut.h
*
* desc:      气象下载器的，上传健康报告
*
* date:      20170731192249
*
***********************************************************/

#ifndef METE_FTPPUT_H
#define METE_FTPPUT_H

#include "base/Base.h"
#include "thread/Thread.h"
#include "thread/Handler.h"
#include "thread/Looper.h"

//#include "funcLib.h"
//#include "mete_init.h"
//#include "mete_healthRep.h"
#include "mete_datatype.h"


class C_MPutFtp: public CThread
{
    private:
        C_MPutFtp();
        ~C_MPutFtp();

    private:
        static C_MPutFtp* cInstance_p; //全局对象
    private:
        void threadHandler(); //来自CThread的虚函数 启动线程时调用
        void init();//给不在构造函数里完成的变量进行初始化
        void sendError ( const Json::Value&  errMsg ); //发送异常信息

    public:
        char doingFlag;
        static C_MPutFtp* Instance(); //用于产生或获取全局实例对象
        void start(); //启动应用对象
        void stop() ; //停止并删除应用对象
        //用于解析信息并产生消息发送到C_MPutFtp的消息队列
        bool parseFile ( const Json::Value& attr );

    private:
        int          prtDebugFlag;  //内部接口是否打印debug日志, 1 打印,0不打印
        char         fullOtherLog[MC_FULLN_LEN]; //其他程序日志名
        T_ftpParas   ftpParas[MC_FTPSERNUM_MAX]; //加载ftp操作需要的配置变量
        T_doFtpSh    shNames[MC_FTPSERNUM_MAX];  //加载进行ftp操作需要的shell
        u_int        validFlag[MC_FTPSERNUM_MAX];//与ftpParas对应的是否有效flag
        int          numFtpSer; //配置的ftp server数量
        std::string  appName;   //健康报告中被监视者名称
        Json::Value  healthVal; //健康报告中其他变量

    private://Handler 上传健康报告
        class C_MPutFtpHandler: public Handler
        {
            public:
                C_MPutFtpHandler ( C_MPutFtp* putFtp_p );
                ~C_MPutFtpHandler();
                typedef enum file_ftpput_msg
                {
                    PARSE_APPADD_FILE,
                    PARSE_MSG_NULL
                } FILE_FTPPUT_MSG;

                //来自Handler的虚函数，实现此函数实现取出消息后的处理逻辑
                void handleMessage ( Message msg );

            private:
                C_MPutFtp* cPutFtp_p;

        };

    private:
        Handler* cHandler_p; //C_MPutFtp 消息队列处理handler
        Looper*  cLooper_p ; //C_MPutFtp 自己消息队列处理looper


};


#endif // METE_FTPPUT_H
