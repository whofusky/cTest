/**********************************************************
* name:       mete_errHandling.h
*
* desc:      气象下载器的，差错处理
*
* date:      20170814095257
*
***********************************************************/

#ifndef METE_ERRHANDLING_H
#define METE_ERRHANDLING_H

#include<dirent.h>
#include "base/Base.h"
#include "thread/Thread.h"
#include "thread/Handler.h"
#include "thread/Looper.h"
#include "json/json.h"
#include "baseFunc.h"

#include "mete_datatype.h"

//#include "mete_init.h"
//#include "hlthRprtWrtXml.h"



class C_MErrHandling: public CThread
{
    private:
        C_MErrHandling();
        ~C_MErrHandling();

    private:
        void threadHandler(); //来自CThread的虚函数 启动线程时调用

    private:
        static C_MErrHandling* cInstance_p; //全局对象

        void init();//给不在构造函数里完成的变量进行初始化

        T_xmlStatDescItem retDesc;

    public:
        T_xmlStatDescItem* convertRetInfo( T_xmlStatDescItem* retDesc_p = NULL );

        char doingFlag;
        char oneceFlag;
        static C_MErrHandling* Instance(); //用于产生或获取全局实例对象

        void start(); //启动应用对象
        void stop() ; //停止并删除应用对象

        //用于解析信息并产生消息发送到C_MErrHandling的消息队列
        bool parseMsg ( const Json::Value& attr );

    private:
        int  prtDebugFlag;  //内部接口是否打印debug日志, 1 打印,0不打印
        char fullErrLog[MC_FULLN_LEN];   //错误日志名
        char fullOtherLog[MC_FULLN_LEN]; //其他程序日志名
        Json::Value otherAttr; //本地xml相关配置项对应的值

    private:
        class C_MErrHandlingHandler: public Handler
        {
            public:
                C_MErrHandlingHandler ( C_MErrHandling* errHandObj_p );
                ~C_MErrHandlingHandler();
                typedef enum file_healthrep_msg
                {
                    PARSE_ERRADD_ITEM,
                    PARSE_MSG_NULL
                } FILE_HEALTHREP_MSG;

                //来自Handler的虚函数，实现此函数实现取出消息后的处理逻辑
                void handleMessage ( Message msg );

            private:
                C_MErrHandling* cErrHandObj_p;

        };

    private:
        Handler*        cHandler_p; //C_MErrHandling 消息队列处理handler
        Looper*         cLooper_p ; //C_MErrHandling 自己消息队列处理looper


};


#endif // METE_ERRHANDLING_H
