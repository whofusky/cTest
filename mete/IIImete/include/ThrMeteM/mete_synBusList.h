/**********************************************************
* name:       mete_synBusList.h
*
* desc:      气象下载器的，合成业务清单
*
* date:      20170731164525
*
***********************************************************/

#ifndef METE_SYNBUSLIST_H
#define METE_SYNBUSLIST_H

#include "base/Base.h"
#include "thread/Thread.h"
#include "thread/Handler.h"
#include "thread/Looper.h"
#include "baseFunc.h"


class C_MFsyBList: public CThread
{
    private:
        C_MFsyBList();
        ~C_MFsyBList();

    private:
        static C_MFsyBList* cInstance_p; //全局对象
        std::string  appName;   //健康报告中被监视者名称
        Json::Value  healthVal; //健康报告中其他变量
        int      prtDebugFlag;  //内部接口是否打印debug日志, 1 打印,0不打印
        char fullOtherLog[MC_FULLN_LEN]; //debug日志名
    private:
        void threadHandler(); //来自CThread的虚函数 启动线程时调用
        void init();//给不在构造函数里完成的变量进行初始化
        void sendError ( const Json::Value&  errMsg ); //发送异常信息

    public:
        static C_MFsyBList* Instance(); //用于产生或获取全局实例对象
        void start(); //启动应用对象
        void stop();  //停止并删除应用对象
        //用于解析信息并产生消息发送到C_MFsyBList的消息队列
        bool parseFile ( const Json::Value& attr );

    private: //Handler 合成在用业务清单
        class C_MFsyBListHandler: public Handler
        {
            public:
                C_MFsyBListHandler ( C_MFsyBList* syBList_p );
                ~C_MFsyBListHandler();
                typedef enum file_synblist_msg
                {
                    PARSE_APPADD_FILE,
                    PARSE_MSG_NULL
                } FILE_SYBLIST_MSG;

                //来自Handler的虚函数，实现此函数实现取出消息后的处理逻辑
                void handleMessage ( Message msg );

            private:
                C_MFsyBList* cSyBList_p;

        };

    private:
        Handler* cHandler_p; //C_MFsyBList 消息队列处理handler
        Looper*  cLooper_p ; //C_MFsyBList 自己消息队列处理looper


};


#endif // METE_SYNBUSLIST_H
