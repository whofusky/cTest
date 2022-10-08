/***********************************************************
 *
 * @file    FileMonitor.cpp
 *
 * @brief   FileMonitor源文件
 *
 * @author  
 *
 * @date    2022-09-30
 *
 * @version V10.010.000
 *
 ***********************************************************/
#include <sys/inotify.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "FileMonitor.h"
#include "json/json.h"
#include "IEventManager.h"
#include "IEventMeta.h"
#include "WriteLog.h"


unsigned int FileMonitor::SetEvent( unsigned int event )
{
    unsigned int flag = 0;
    if ( EVENT_MOVE_TO & event )
    {
        flag |= IN_MOVED_TO;
    }
    if ( EVENT_MOVE_FROM & event )
    {
        flag |= IN_MOVED_FROM;
    }
    if ( EVENT_CREATE & event )
    {
        flag |= IN_CREATE;
    }
    if ( EVENT_DELETE & event )
    {
        flag |= IN_DELETE;
    }
    if (flag == 0 )
    {
        PrintToStdout( LOGERROR,"Bad event set, event is [%08x]", event );
    }

    return flag;
}


int FileMonitor::getMonitorInfoByWd(int wd, stu_MONITOR_INFO & monitorInfo )
{
    CMutexGuard tMutexGuard( mMutex );
    for ( std::vector<stu_MONITOR_INFO>::iterator it = mVecMonitorInfo.begin();
          it != mVecMonitorInfo.end();
          it++
        )
    {
        if ( it->wd == wd )
        {
            monitorInfo.wd = it->wd;
            monitorInfo.MonitorPath = it->MonitorPath;
            monitorInfo.MonitorPrefix.assign( it->MonitorPrefix.begin(), it->MonitorPrefix.end() );
            monitorInfo.MonitorSuffix.assign( it->MonitorSuffix.begin(), it->MonitorSuffix.end() );

            return 0;
        }
    }
    return -1;
}

int FileMonitor::checkMonitorFile( std::string strMonitorFile, stu_MONITOR_INFO &monitorInfo)
{
    bool prefixFlag = false;
    bool suffixFlag = false;

    for ( std::vector<std::string>::iterator it = monitorInfo.MonitorPrefix.begin();
          it != monitorInfo.MonitorPrefix.end();
          it++
        )
    {
        std::string strPrefix = *it;
        if ( 0 == strPrefix.compare( 0, strPrefix.size(), 
                                     strMonitorFile, 0, strMonitorFile.size()
                    )
           )
        {
            prefixFlag = true;
            break;
        }
    }

    if ( !prefixFlag )
    {
        return -1;
    }

    for ( std::vector<std::string>::iterator it = monitorInfo.MonitorSuffix.begin();
          it != monitorInfo.MonitorSuffix.end();
          it++
        )
    {
        std::string strSuffix = *it;
        if ( strMonitorFile.size() >= strSuffix.size() )
        {
            if ( 0 == strSuffix.compare( 0, strSuffix.size(), strMonitorFile,
                        strMonitorFile.size() - strSuffix.size(),
                        strSuffix.size()
                        )
               )
            {
                suffixFlag = true;
                break;
            }
        }
    }

    if ( !suffixFlag )
    {
        return -1;
    }

    return 0;
}

FileMonitor::FileMonitor()
{
    mMonitorFd = inotify_init1( IN_NONBLOCK );
    if ( mMonitorFd < 0  )
    {
        PrintToStdout( LOGERROR,
                "inotify_init1 fail,mMonitorFd=[%d],errno=[%d],errMsg=[%s]", 
                mMonitorFd, errno, strerror(errno) );
    }
}

FileMonitor::~FileMonitor()
{
    if ( isAlive() )
    {
        stopThread();
    }
}
        

bool FileMonitor::startMonitor(
        const char* pMonitorDir,
        unsigned int event,
        std::vector<std::string> & monitorPrefix,
        std::vector<std::string> & monitorSuffix
        )
{
    if ( mMonitorFd < 0 )
    {
        PrintToStdout( LOGERROR,"startMonitor [%s] fail,Fd=[%d]",
                pMonitorDir, mMonitorFd );
        return false;
    }

    //如果开启过同路径的监控，先停止
    stopMonitor( pMonitorDir );

    CMutexGuard tMutexGuard( mMutex );

    int wd = inotify_add_watch( mMonitorFd, pMonitorDir, SetEvent( event ) );
    if ( wd < 0 )
    {
        PrintToStdout( LOGERROR,"inotify_add_watch( %d, %s, %u) return[%d],errno=[%d,errMsg=[%s]",
                 mMonitorFd, pMonitorDir, event, wd, errno, strerror(errno) );
        return false;

    }

    PrintToStdout( LOGINFO,"start [%s] monitor return [%d]", pMonitorDir, wd );

    stu_MONITOR_INFO stuMonitorInfo;
    stuMonitorInfo.wd            = wd;
    stuMonitorInfo.MonitorPath   = pMonitorDir;
    stuMonitorInfo.MonitorPrefix = monitorPrefix;
    stuMonitorInfo.MonitorSuffix = monitorSuffix;
    mVecMonitorInfo.push_back( stuMonitorInfo );

    if ( !isAlive() )
    {
        startThread();
    }

    return true;
}


bool FileMonitor::stopMonitor( const char* pMonitorDir )
{
    CMutexGuard tMutexGuard( mMutex );

    for( std::vector<stu_MONITOR_INFO>::iterator it = mVecMonitorInfo.begin();
         it != mVecMonitorInfo.end();
         it++
       )
    {
        if ( it->MonitorPath == pMonitorDir )
        {
            int ret = inotify_rm_watch( mMonitorFd, it->wd );
            PrintToStdout( LOGINFO,"stop [%s] monitor return [%d]", 
                    pMonitorDir, ret );

            mVecMonitorInfo.erase( it );
            break;
        }
    }
    return true;
}

int setJsonMonitorInfo( inotify_event* event, 
        const char* monitorPath, 
        Json::Value &eventAttr 
        )
{

    PrintToStdout( LOGDEBUG,"event-mask:[%08x]", event->mask );

    switch( event->mask )
    {
        case IN_CREATE:
            {
                eventAttr["Parameter"]["Operation"] = STORAGE_FILE_ADDED;
                break;
            }
        case IN_DELETE:
            {
                eventAttr["Parameter"]["Operation"] = STORAGE_FILE_DELETED;
                break;
            }
        case IN_MOVED_TO:
            {
                eventAttr["Parameter"]["Operation"] = STORAGE_FILE_MOVEDTO;
                break;
            }
        case IN_MOVED_FROM:
            {
                eventAttr["Parameter"]["Operation"] = STORAGE_FILE_MOVEDFROM;
                break;
            }
        default:
            {
                PrintToStdout( LOGERROR, "event-mask:[%08x] is not support",
                        event->mask );
                return -1;
            }
    }

    char curTime_a[64] = {0};
    getFormatTime( curTime_a, sizeof(curTime_a) );
    eventAttr["ID"] = 0;
    eventAttr["KeyWord"] = STORAGE_EVENT;
    eventAttr["Parameter"]["FileName"] = event->name;
    eventAttr["Parameter"]["Dir"] = monitorPath;
    eventAttr["Parameter"]["capTime"] = curTime_a;

    return 0;
}

void FileMonitor::threadHandler()
{
    int iRet = 0;
    int num = 0;
    const int dataSize = 1024*8;
    char data[dataSize] = {0};

    while( isAlive() )
    {
        memset( data, 0, dataSize );
        num = read( mMonitorFd, data, dataSize );
        if( num <= 0 )
        {
            PauseThreadSleep( 0, 10 );
            continue;
        }

        for( char* pEvent = data; pEvent < data + num; )
        {
            stu_MONITOR_INFO stuMonitorInfo;
            inotify_event* event = (inotify_event*) pEvent;
            iRet = getMonitorInfoByWd( event->wd, stuMonitorInfo );
            if ( iRet != 0 )
            {
                PrintToStdout( LOGERROR, "wd=[%d] not fount monitor path!",
                        event->wd );
            }
            else
            {
                iRet = checkMonitorFile( event->name, stuMonitorInfo );
                if ( iRet == 0 )
                {
                    Json::Value eventAttr;
                    iRet = setJsonMonitorInfo( event, 
                            stuMonitorInfo.MonitorPath.c_str(),
                            eventAttr );
                    if ( iRet == 0 )
                    {
                        PrintToStdout( LOGDEBUG,"event:[%s]",eventAttr.toStyledString().c_str() );

                        IEventManager::Initialize()->notifyEvent(
                                STORAGE_EVENT,
                                IEventManager::EVENT_START,
                                eventAttr );
                    }
                    else
                    {
                        PrintToStdout( LOGERROR, "setJsonMonitorInfo return fail");
                    }
                }
            }

            pEvent += sizeof( struct inotify_event ) + event->len;
        }// end for


        PauseThreadSleep( 0, 10 );
    }
    return;
}


