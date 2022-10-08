/***********************************************************
 *
 * @file    FileMonitor.h
 *
 * @brief   FileMonitor头文件
 *
 * @author  
 *
 * @date    2022-09-30
 *
 * @version V10.010.000
 *
 ***********************************************************/

#ifndef _I_FILEMONOTIR_H_
#define _I_FILEMONOTIR_H_

#include <string>
#include <vector>
#include "Base.h"
#include "Thread.h"
#include "Mutex.h"

#define EVENT_CREATE     0x01
#define EVENT_DELETE     0x02
#define EVENT_MOVE_TO    0x04
#define EVENT_MOVE_FROM  0x08


typedef struct _stu_MONITOR_INFO{
    int wd;
    std::string MonitorPath;
    std::vector<std::string> MonitorPrefix;
    std::vector<std::string> MonitorSuffix;
}stu_MONITOR_INFO;


class FileMonitor: public CThread
{
    public:
        typedef void (*NotifyHandler)(const int event_mask);

    public:
        FileMonitor();
        ~FileMonitor();

        //desc :
        //    启动对某个路径的监控功能
        //    可重复调用，完成同时对多个路径的监测
        //
        //param:
        //    pMonitorDir : 监控的路径信息
        //    event       : 关注事件的类型
        bool startMonitor( const char* pMonitorDir,
                unsigned int event,
                std::vector<std::string> &monitorPrefix,
                std::vector<std::string> &monitorSuffix );

        //desc :
        //    关闭对某个路径的监控功能
        //    可重复调用，完成对多个路径监控的关闭
        //
        //param:
        //    pMonitorDir : 监控的路径信息
        bool stopMonitor( const char* pMonitorDir );

    private:
        int mMonitorFd;
        std::vector<stu_MONITOR_INFO> mVecMonitorInfo;
        CMutex   mMutex;

    private:
        void threadHandler();
        unsigned int SetEvent(unsigned int event);
        int getMonitorInfoByWd(int wd, stu_MONITOR_INFO & monitorInfo );
        int checkMonitorFile( std::string strMonitorFile, stu_MONITOR_INFO &monitorInfo);


};

#endif//_I_FILEMONOTIR_H_

