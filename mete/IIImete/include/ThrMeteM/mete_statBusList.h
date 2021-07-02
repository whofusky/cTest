/**********************************************************
* name:       mete_statBusList.h
*
* desc:      气象下载器的，生成业务清单提供情况
*
* date:      20170731180033
*
***********************************************************/

#ifndef METE_STATBUSLIST_H
#define METE_STATBUSLIST_H

#include "base/Base.h"
#include "thread/Thread.h"
#include "thread/Handler.h"
#include "thread/Looper.h"
#include "baseFunc.h"


class C_MStatBList: public CThread
{
    private:
        C_MStatBList();
        ~C_MStatBList();

    private:
        static C_MStatBList* cInstance_p; //全局对象
        std::string  appName  ; //健康报告中被监视者名称
        Json::Value  healthVal; //健康报告中其他变量
        int      prtDebugFlag;  //内部接口是否打印debug日志, 1 打印,0不打印
        char fullOtherLog[MC_FULLN_LEN]; //debug日志名
    private:
        void threadHandler(); //来自CThread的虚函数 启动线程时调用
        void init();//给不在构造函数里完成的变量进行初始化
        void sendError ( const Json::Value&  errMsg ); //发送异常信息
    public:
        char doingFlag;
        char* staLisTrig_p;//业务清单提供情况 任务触发目录
        char* doStaPath_p; //业务清单提供情况生成目录
        char* doStaName_p; //业务清单提供情况文件名前缀


        static C_MStatBList* Instance(); //用于产生或获取全局实例对象
        void start(); //启动应用对象
        void stop() ; //停止并删除应用对象
        //用于解析信息并产生消息发送到C_MStatBList的消息队列
        bool parseFile ( const Json::Value& attr );

    private: //Handler 生成业务清单提供情况
        class C_MStatBListHandler: public Handler
        {
            public:
                C_MStatBListHandler ( C_MStatBList* syBList_p );
                ~C_MStatBListHandler();
                typedef enum file_statblist_msg
                {
                    PARSE_APPADD_FILE,
                    PARSE_MSG_NULL
                } FILE_STATBLIST_MSG;

                //来自Handler的虚函数，实现此函数实现取出消息后的处理逻辑
                void handleMessage ( Message msg );

            private:
                C_MStatBList* cStatBList_p;

        };

    private:
        Handler* cHandler_p; //C_MStatBList 消息队列处理handler
        Looper*  cLooper_p ; //C_MStatBList 自己消息队列处理looper


};


#endif // METE_STATBUSLIST_H
