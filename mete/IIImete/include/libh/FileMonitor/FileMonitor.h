/***********************************************************
*
*
*  FileMonitor.h
*
*  Create by muxd
*
*  2017/07/08
**********************************************************/
#ifndef _FILE_MONITOR_H_
#define _FILE_MONITOR_H_
#include <string>
#include <map>
#include "base/Base.h"
#include "thread/Thread.h"
#include "mutex/Mutex.h"

#define EVENT_CREATE    0x01
#define EVENT_DELETE    0x02
#define EVENT_MOVE_TO   0x04
#define EVENT_MOVE_FROM 0x08

class FileMonitor: public CThread
{
public:
    typedef void (*NotifyHanlder)(const int event_mask);

public:
    FileMonitor();
    ~FileMonitor();

    bool startMonitor( const char* monitor_dir_path,
                     unsigned int evnt );

    bool stopMonitor( const char* monitor_dir_path );

private:
    std::map< int, std::string > mMonitorPath;
    std::map< int, UInt32 > mMonitorEvent;
    CMutex                     mMutex;
private:
    void threadHandler();
};

#endif //_FILE_MONITOR_H_

