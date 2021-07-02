/**********************************************************
* name:       mete_monitor.h
*
* desc:      气象下载器的，文件监视器
*
* date:      20170801153843
*
***********************************************************/

#ifndef METE_MONITOR_H
#define METE_MONITOR_H

#include "base/Base.h"
#include "thread/Thread.h"
#include "thread/Handler.h"
#include "thread/Looper.h"
#include "FileMonitor/FileMonitor.h"
#include "EventManager/IEventMeta.h"
#include "EventManager/IEventManager.h"
#include "baseFunc.h"


class C_MMonitor :  public CThread, public IEventManager::IEventHandler
{
    private:
        C_MMonitor();
        ~C_MMonitor();

    private:
        static C_MMonitor* cInstance_p; //全局对象
    private:
        void threadHandler(); //来自CThread的虚函数 启动线程时调用
        void init();//给不在构造函数里完成的变量进行初始化
        void sendError ( const Json::Value&  errMsg ); //发送异常信息
    public:
        static C_MMonitor* Instance(); //用于产生或获取全局实例对象
        //来自IEventManager::IEventHandler的虚函数
        //  IEventManager取出消息后调用此函数实现具体逻辑
        void onAppEvent ( const char* event,
                          IEventManager::EVENT_ACTION action,
                          Json::Value eventAttr
                        );
        void start(); //启动应用对象
        void stop() ; //停止并删除应用对象
        //用于解析信息并产生消息发送到C_MFsyBList的消息队列
        //bool parseMsg( const Json::Value attr );

    private:
        int      prtDebugFlag;  //内部接口是否打印debug日志, 1 打印,0不打印
        char fullOtherLog[MC_FULLN_LEN]; //debug日志名
        Json::Value   otherAttr;//本地xml相关配置项对应的值
        std::string  appName;  //健康报告中被监视者名称
        Json::Value  healthVal;//健康报告中其他变量
        //是否启用综合监视器标识(综合监视器即一个监视器监视多个目录):1启用,0不启用
        int monInteFlag;
        FileMonitor*  cMonitorDoB_p;    //业务清单合成文件源监视器
        FileMonitor*  cMonitorStaB_p;   //业务清单提供情况文件源监视器
        FileMonitor*  cMonitorHtI_p;    //健康报告完成文件源监视器 触发生成综合健康报告
        FileMonitor*  cMonitorUpFtp_p;  //ftp上传健康报告文件源监视器
        FileMonitor*  cMonitorInteg;    //综合监视器

    private://Handler 根据不同目录的用户给相应模块发消息
        class C_MMonitorHandler: public Handler
        {
            public:
                C_MMonitorHandler ( C_MMonitor* mMonitor_p );
                ~C_MMonitorHandler();

                typedef enum file_monitor_msg
                {
                    MSG_DOBULIST_FILE,
                    MSG_STATBULIST,
                    MSG_YESHPATH,
                    MSG_UPHEALTH,
                    MSG_NULL
                } FILE_MONITOR_MSG;

                //来自Handler的虚函数，实现此函数实现取出消息后的处理逻辑
                void handleMessage ( Message msg );

            private:
                C_MMonitor* cmMonitor_p;

        };

    private:
        Handler* cHandler_p; //C_MMonitor 消息队列处理handler
        Looper*  cLooper_p ; //C_MMonitor 自己消息队列处理looper


};


#endif // METE_MONITOR_H
