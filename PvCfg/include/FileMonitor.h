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
#include <map>
#include "InfraBase.h"
#include "Thread.h"
#include "Mutex.h"

//监测文件事件类型定义
#define EVENT_CREATE     0x01
#define EVENT_DELETE     0x02
#define EVENT_MOVE_TO    0x04
#define EVENT_MOVE_FROM  0x08
#define EVENT_MODIFY     0x10
#define EVENT_STOCK      0x20    /*存量文件:启动进程之前已有的文件*/
#define EVENT_CLOSEWRITE 0x40


class FileMonitor: public CThread
{
    public:
        //功能启动接口
        unsigned char start( const int &pre_val = -1 );
        
        //功能关闭接口
        unsigned char stop();

    public:
        static int isRun;
        static FileMonitor* Instance();
    private:
        static FileMonitor* mInstance;

    typedef struct _stu_MONITOR_INFO{
        int wd;
        std::string MonitorPath;
        std::vector<std::string> MonitorFiles;
        std::vector<std::string> MonitorPrefix;
        std::vector<std::string> MonitorSuffix;
    }stu_MONITOR_INFO;

    //用于存储程序启动前监视目录，同一目录下的文件
    typedef struct fl_node
    {
        char fileName[100];
        long int fileMtime;
        struct fl_node* previous;
        struct fl_node* next;
    }MY_FL_NODE;

    public:
        typedef void (*NotifyHandler)(const int event_mask);

    private:
        FileMonitor();
        ~FileMonitor();
        bool rmAllWatch();

    public:

        //desc :
        //    启动对某个路径的监控功能
        //    可重复调用，完成同时对多个路径的监测
        //
        //param:
        //    pMonitorDir  : 监控的路径信息
        //    event        : 关注事件的类型
        //    monitorFiles : 关注的文件名(多个用|分隔) 
        //    monitorPrefix: 关注的文件名前缀(多个用|分隔) 
        //    monitorSuffix: 关注的文件名后缀(多个用|分隔) 
        bool addWatch( const char *pMonitorDir,
                unsigned int event,
                const char *monitorFiles = NULL,
                const char *monitorPrefix = NULL,
                const char *monitorSuffix = NULL );

        //desc :
        //    关闭对某个路径的监控功能
        //    可重复调用，完成对多个路径监控的关闭
        //
        //param:
        //    pMonitorDir : 监控的路径信息
        bool rmWatch( const char* pMonitorDir );

    private:
        int mMonitorFd;
        std::vector<stu_MONITOR_INFO> mVecMonitorInfo;
        CMutex   mMutex;

    private:
        void threadHandler();
        unsigned int SetEvent(unsigned int event);
        int getMonitorInfoByWd(int wd, stu_MONITOR_INFO & monitorInfo );
        int checkMonitorFile( std::string strMonitorFile, stu_MONITOR_INFO &monitorInfo);

    private:
        //用于存储程序启动前监视目录下的文件
        std::map< std::string, MY_FL_NODE* > mOldFMap;
        CMutex   mFlMutex;

        void freeFlNode( MY_FL_NODE** flNodes);
        void detachOldFiles( std::string &filePath, MY_FL_NODE** flNodes );
        void attachOldFiles( std::string &filePath, const char* fileName, long int tMtime );
        int readDirsFile( stu_MONITOR_INFO& stuInfo );
        void handStockFiles( stu_MONITOR_INFO& stuInfo, unsigned int& event );

        void spStrToVec(std::string &inS, const char* delimiter, std::vector<std::string> &vec);

private:
    class Delector
    {
        public:
            ~Delector()
            {
                if ( FileMonitor::mInstance != NULL )
                {
                    delete FileMonitor::mInstance;
                    FileMonitor::mInstance = NULL;
                }
            }
    };

    static Delector delector;

};

#endif//_I_FILEMONOTIR_H_

