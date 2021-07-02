/**********************************************************
* name:       mete_healthRep.h
*
* desc:      气象下载器的，生成健康报告
*
* date:      20170731191858
*
***********************************************************/

#ifndef METE_HEALTHREP_H
#define METE_HEALTHREP_H

#include<dirent.h>
#include "base/Base.h"
#include "thread/Thread.h"
#include "thread/Handler.h"
#include "thread/Looper.h"
#include "Timer/Timer.h"
#include "json/json.h"
#include "baseFunc.h"

//#include "funcLib.h"
//#include "mete_init.h"


class C_MHealthRep: public CThread
{
    private:
        C_MHealthRep();
        ~C_MHealthRep();

    private:
        void threadHandler(); //来自CThread的虚函数 启动线程时调用

    private:
        static C_MHealthRep* cInstance_p; //全局对象

        void init();//给不在构造函数里完成的变量进行初始化
        void sendError ( const Json::Value&  errMsg ); //发送异常信息

    public:
        char doStatFlag;
        static C_MHealthRep* Instance(); //用于产生或获取全局实例对象

        void start(); //启动应用对象
        void stop() ; //停止并删除应用对象

        //用于解析信息并产生消息发送到C_MHealthRep的消息队列
        bool parseMsg ( const Json::Value& attr );

    private:
        int      prtDebugFlag;  //内部接口是否打印debug日志, 1 打印,0不打印
        char fullOtherLog[MC_FULLN_LEN]; //debug日志名
        Json::Value otherAttr; //本地xml相关配置项对应的值

        //定时器控制变量
        int second   ; //秒
        int micSecond; //毫秒
        int repet    ; //重复标识:<0 定时器无限循环，>0定时器只运行repet次

    private:
        class C_MHealthRepHandler: public Handler
        {
            public:
                C_MHealthRepHandler ( C_MHealthRep* healthRep_p );
                ~C_MHealthRepHandler();
                typedef enum file_healthrep_msg
                {
                    PARSE_APPADD_ITEM,
                    PARSE_MERGE_TO_INTEG,
                    PARSE_MSG_NULL
                } FILE_HEALTHREP_MSG;

                //来自Handler的虚函数，实现此函数实现取出消息后的处理逻辑
                void handleMessage ( Message msg );

            private:
                C_MHealthRep* cHealthRep_p;

        };

    private:
        class C_MHealthTimer: public CTimer
        {
            public:
                C_MHealthTimer ( Json::Value&  tAttr,
                                 bool autoRelaseFlag,
                                 C_MHealthRep* healthRep_p
                               );
                ~C_MHealthTimer();

                static C_MHealthTimer* getObj ( Json::Value&  tiAttr,
                                                bool autoRelaseFlag,
                                                C_MHealthRep* healthRep_p
                                              );
            private:
                virtual void timerHandler();

                C_MHealthRep* cMHealthRep_p;
                Json::Value  tAttr;

        };

    private:
        Handler*        cHandler_p; //C_MHealthRep 消息队列处理handler
        Looper*         cLooper_p ; //C_MHealthRep 自己消息队列处理looper
        C_MHealthTimer* cMHTimer_p; //定时移动到上传ftp目录的timer


};


#endif // METE_HEALTHREP_H
