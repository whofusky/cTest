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
#include <sys/syscall.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "FileMonitor.h"
#include "json/json.h"
#include "IEventManager.h"
#include "CustomOutLog.h"




#define MONI_DATASIZE  8192   /* 1024*8 */




static pthread_mutex_t  gsMutex = PTHREAD_MUTEX_INITIALIZER;





FileMonitor* FileMonitor::mInstance = NULL;
int FileMonitor::isRun = 0;




//@brief:
// 释放MY_FL_NODE* 变量空间
void FileMonitor::freeFlNode( MY_FL_NODE** flNodes)
{
    
    MY_FL_NODE* tCurNodes = *flNodes;
    if ( tCurNodes == NULL )
    {
        return;
    }

    MY_FL_NODE* tNodes    = NULL;
    while ( tCurNodes != NULL && tCurNodes->next != NULL )
    {
        tNodes    = tCurNodes;
        tCurNodes = tCurNodes->next;
        free( tNodes );

    }
    if ( tCurNodes != NULL )
    {
        free( tCurNodes );
    }

    *flNodes = NULL;
    return;
}



//@brief:
// 将字符串inS按分隔符delimiter拆分到vec中去 
void FileMonitor::spStrToVec(std::string &inS, const char* delimiter, std::vector<std::string> &vec) 
{
    inS.erase( 0, inS.find_first_not_of(" ") );
    inS.erase( inS.find_last_not_of(" ") + 1 );
    if ( inS.empty() )
    {
        return;
    }
    
    std::string ts;
    std::string::size_type head = 0;
    std::string::size_type tail = inS.find( delimiter, head );
    while ( tail != inS.npos )
    {
        ts = inS.substr( head, tail - head );
        vec.push_back( ts );
        head = tail + 1;
        tail = inS.find( delimiter, head );
    }
    ts = inS.substr( head );
    vec.push_back( ts );

    return;
}




//@brief:
//  从std::map<std::string,MY_FL_NODE*> 取一个it
void FileMonitor::detachOldFiles( std::string &filePath, MY_FL_NODE** flNodes )
{
    CMutexGuard tMutexGuard( mFlMutex );
    std::map<std::string,MY_FL_NODE*>::iterator it;
    it= mOldFMap.find ( filePath );
    if( it == mOldFMap.end() )
    {
        return;
    }
    *flNodes = it->second;

    mOldFMap.erase( it );

    return;
}


//@brief:
//  将路径filePath下文件fileName 按文件修改时间升序放入std::map<std::string,MY_FL_NODE*>
void FileMonitor::attachOldFiles( 
        std::string &filePath, 
        const char* fileName, 
        long int tMtime 
        )
{
    CMutexGuard tMutexGuard( mFlMutex );
    std::map<std::string,MY_FL_NODE*>::iterator it;
    it= mOldFMap.find ( filePath );
    if( it == mOldFMap.end() )
    {
        MY_FL_NODE* tFlNode = (MY_FL_NODE*) calloc( 1,sizeof(MY_FL_NODE) );
        snprintf( tFlNode->fileName, sizeof(tFlNode->fileName),
                "%s", fileName);
        tFlNode->fileMtime = tMtime;
        tFlNode->next     = NULL;
        tFlNode->previous = NULL;
        mOldFMap.insert( make_pair(filePath,tFlNode) );
        return;
    }

    MY_FL_NODE* tFndNode  = it->second;
    MY_FL_NODE* tSameNode = NULL;

    //查看是否有同文件名的
    while ( tFndNode != NULL )
    {
        //有相同文件名只存一个值
        if ( strcmp( tFndNode->fileName, fileName ) == 0 )
        {
            //同一文件同一修改时间不需要操作
            if ( tFndNode->fileMtime == tMtime )
            {
                return;
            }

            tFndNode->fileMtime = tMtime;
            //已经是最后一个结点无须排序直接返回
            if ( tFndNode->next == NULL ) 
            {
                return;
            }

            //将更新时间的节点摘下来以便后面进行排序

            //如果要摘的结点是头结点
            if ( tFndNode == it->second )
            {
                it->second = tFndNode->next;
            }

            if ( tFndNode->previous != NULL ) 
            {
                tFndNode->previous->next = tFndNode->next;
                tFndNode->previous = NULL;
            }
            if ( tFndNode->next != NULL )
            {
                tFndNode->next->previous = tFndNode->previous;
                tFndNode->next = NULL;
            }
            tSameNode = tFndNode;

            break;
        }

        tFndNode = tFndNode->next;
    }
    
    if ( tSameNode == NULL )//在已有的节点中没有当前文件的结点
    {
        //生成当前文件的节点
        tSameNode = (MY_FL_NODE*) calloc( 1,sizeof(MY_FL_NODE) );
        snprintf( tSameNode->fileName, sizeof(tSameNode->fileName),
                "%s", fileName);
        tSameNode->fileMtime = tMtime;
        tSameNode->next     = NULL;
        tSameNode->previous = NULL;
    }

    //在已有的节点中按文件修改时间升序排序插入it->second
    tFndNode = it->second;
    MY_FL_NODE* tPreNode= tFndNode;
    while ( tFndNode != NULL && tFndNode->fileMtime <= tMtime )
    {
        tPreNode = tFndNode;
        tFndNode = tFndNode->next;
    }

    if ( tPreNode == it->second && tPreNode->fileMtime > tMtime )
    {
        //只有一个结点，且此结点的值小于当前要插入的值
        it->second = tSameNode;
        tSameNode->next    = tPreNode;
        tPreNode->previous = tSameNode;

    }
    else
    {
        tPreNode->next      = tSameNode;
        tSameNode->previous = tPreNode;
        if ( tFndNode != NULL )
        {
            tFndNode->previous = tSameNode;
            tSameNode->next    = tFndNode;
        }
    }

    return;
}


//@brief:
//  读取stuInfo.MonitorPath路径(但不查找子目录)下
//  符合stuInfo.MonitorFiles 或(stuInfo.MonitorPrefix 和 stuInfo.MonitorSuffix)
//  格式要求的文件按文件修改时间升序放入std::map<std::string,MY_FL_NODE*>
int FileMonitor::readDirsFile( stu_MONITOR_INFO& stuInfo )
{
    DIR*   dirStream_p    = NULL;
    struct dirent* dir_fp = NULL;

    dirStream_p = opendir( stuInfo.MonitorPath.c_str() );
    if ( dirStream_p == NULL )
    {
        return errno;
    }

    while ( ( dir_fp = readdir(dirStream_p) ) != NULL)
    {
        if( strcmp(dir_fp->d_name,".") == 0 
                || strcmp(dir_fp->d_name,"..")==0
          )    ///current dir OR parrent dir
        {
            continue;
        }

        std::string tPathFile = stuInfo.MonitorPath + "/" + std::string( dir_fp->d_name );
        struct stat statbuf;
        if ( lstat(tPathFile.c_str(), &statbuf ) == -1)
        {
            continue;
        }
        
        //if ( (statbuf.st_mode & S_IFMT) == S_IFLNK ) //symlink
        //if ( (statbuf.st_mode & S_IFMT) == S_IFDIR ) //directory
        if ( (statbuf.st_mode & S_IFMT) == S_IFREG ) //regular file
        {
            int iRet = checkMonitorFile( dir_fp->d_name, stuInfo );
            if ( iRet == 0 )
            {
                attachOldFiles( 
                        stuInfo.MonitorPath, 
                        dir_fp->d_name, 
                        statbuf.st_mtime 
                        );
            }
        }
    }

    closedir( dirStream_p );

    return 0;
}


//@brief:
//  判断是否对监视目录的存量文件(程序在对目录进行监视之前就有文件)进行处理
//  如下需要处理则给文件接收处理者发消息通知
void FileMonitor::handStockFiles( stu_MONITOR_INFO& stuInfo, unsigned int& event )
{
    //只有设置了EVENT_STOCK的event才进行处理
    if ( (event & EVENT_STOCK ) != EVENT_STOCK )
    {
        return;
    }

    int iRet = readDirsFile( stuInfo );
    if ( iRet != 0 )
    {
        return;
    }
    MY_FL_NODE* tFlNode = NULL;
    detachOldFiles( stuInfo.MonitorPath, &tFlNode );
    if ( tFlNode == NULL )
    {
        return;
    }
    MY_FL_NODE* tFL_p = tFlNode;
    while ( tFL_p != NULL )
    {
        Json::Value eventAttr;

        b_write_log(_DEBUG,"stock file[%s/%s]", 
            stuInfo.MonitorPath.c_str(), tFL_p->fileName );

        char curTime_a[64] = {0};
        getFormatTime( curTime_a, sizeof(curTime_a) );

        eventAttr["ID"] = 0;
        eventAttr["KeyWord"]                = STORAGE_EVENT;
        eventAttr["Parameter"]["FileName"]  = tFL_p->fileName;
        eventAttr["Parameter"]["Operation"] = STORAGE_FILE_STOCK;
        eventAttr["Parameter"]["Dir"]       = stuInfo.MonitorPath.c_str();
        eventAttr["Parameter"]["capTime"]   = curTime_a;

        IEventManager::Initialize()->notifyEvent(
                STORAGE_EVENT,
                IEventManager::EVENT_START,
                eventAttr );

        tFL_p = tFL_p->next;
    }
    freeFlNode( &tFlNode );

    return;
}


//@brief:
//   event define: man 7 inotify
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
    if ( EVENT_MODIFY & event )
    {
        flag |= IN_MODIFY;
    }
    if ( EVENT_CLOSEWRITE & event )
    {
        flag |= IN_CLOSE_WRITE;
    }
    if (flag == 0 )
    {
        b_write_log(_ERROR,"Bad event set, event is [%08x]", event );
    }

    return flag;
}


//@brief:
// 根据wd 取注册了的文件监视信息 
int FileMonitor::getMonitorInfoByWd(int wd, stu_MONITOR_INFO & monitorInfo )
{
    CMutexGuard tMutexGuard( mMutex );
    std::vector<stu_MONITOR_INFO>::iterator it;
    for ( it = mVecMonitorInfo.begin();
          it != mVecMonitorInfo.end();
          it++
        )
    {
        if ( it->wd == wd )
        {
            monitorInfo.wd = it->wd;
            monitorInfo.MonitorPath = it->MonitorPath;
            monitorInfo.MonitorFiles.assign( 
                    it->MonitorFiles.begin(), 
                    it->MonitorFiles.end() 
                    );
            monitorInfo.MonitorPrefix.assign( 
                    it->MonitorPrefix.begin(), 
                    it->MonitorPrefix.end() 
                    );
            monitorInfo.MonitorSuffix.assign( 
                    it->MonitorSuffix.begin(), 
                    it->MonitorSuffix.end() 
                    );

            return 0;
        }
    }
    return -1;
}


//@brief:
//  校验文件名strMonitorFile是否为需要的文件
int FileMonitor::checkMonitorFile( 
        std::string strMonitorFile, 
        stu_MONITOR_INFO &monitorInfo
        )
{
    //没有对文件名,文件前缀,文件后缀进行设置则任何文件都让其通过
    if ( monitorInfo.MonitorFiles.size() == 0
            && monitorInfo.MonitorPrefix.size() == 0
            && monitorInfo.MonitorSuffix.size() == 0
       )
    {
        return 0;
    }

    std::vector<std::string>::iterator it;

    for ( it = monitorInfo.MonitorFiles.begin();
          it != monitorInfo.MonitorFiles.end();
          it++
        )
    {
        if ( *it == strMonitorFile )
        {
            return 0;
        }
    }

    bool prefixFlag = true;
    bool suffixFlag = true;
    if ( monitorInfo.MonitorPrefix.size() > 0 ) prefixFlag = false; 

    if ( monitorInfo.MonitorSuffix.size() > 0 ) suffixFlag = false; 

    for ( it = monitorInfo.MonitorPrefix.begin();
          it != monitorInfo.MonitorPrefix.end();
          it++
        )
    {
        //std::string strPrefix = *it;
        if ( 0 == it->compare( 
                    0, 
                    it->size(), 
                    strMonitorFile, 
                    0, 
                    it->size() )
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

    for ( it = monitorInfo.MonitorSuffix.begin();
          it != monitorInfo.MonitorSuffix.end();
          it++
        )
    {
        //std::string strSuffix = *it;
        if ( strMonitorFile.size() >= it->size() )
        {
            if ( 0 == it->compare( 
                        0, 
                        it->size(), 
                        strMonitorFile,
                        strMonitorFile.size() - it->size(),
                        it->size() )
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
        b_write_log(_ERROR,"inotify_init1 fail,mMonitorFd=[%d],errno=[%d],errMsg=[%s]", 
                mMonitorFd, errno, strerror(errno) );
    }
}


FileMonitor::~FileMonitor()
{
    if ( mMonitorFd >= 0 )
    {
        close( mMonitorFd );
    }
}


unsigned char FileMonitor::start( const int &pre_val )
{
    if ( mMonitorFd < 0 )
    {
        b_write_log(_ERROR,"inotify_init1( IN_NONBLOCK ) return < %d",
               mMonitorFd );
        return False;
    }

    CMutexGuard tMutexGuard( mMutex );

    if ( pre_val != -1 )
    {
        while( 0 == pre_val )
        {
            PauseThreadSleep( 0, 10 );
        }
        b_write_log(_DEBUG,"Wait for the prepend to end!");
    }


    if ( !isAlive() )
    {
        startThread();
    }
    b_write_log(_DEBUG,"FileMonitor::start() Done");
    return True;
}



        
unsigned char FileMonitor::stop()
{

    rmAllWatch();

    CMutexGuard tMutexGuard( mMutex );

    if ( isAlive() )
    {
        stopThread();
    }

    return True;
}

//@brief:
//  添加对某个目录的监视
//param:
//    pMonitorDir  : 监控的路径信息
//    event        : 关注事件的类型
//    monitorFiles : 关注的文件名(多个用|分隔) 
//    monitorPrefix: 关注的文件名前缀(多个用|分隔) 
//    monitorSuffix: 关注的文件名后缀(多个用|分隔) 
bool FileMonitor::addWatch(
        const char *pMonitorDir,
        unsigned int event,
        const char *monitorFiles,
        const char *monitorPrefix,
        const char *monitorSuffix
        )
{
    if ( NULL == pMonitorDir )
    {
        b_write_log(_ERROR,"pMonitorDir is NULL" );
        return false;
    }

    if ( mMonitorFd < 0 )
    {
        b_write_log(_ERROR,"inotify_init1( IN_NONBLOCK ) return < %d",
               mMonitorFd );
        return false;
    }

    //如果开启过同路径的监控，先停止
    rmWatch( pMonitorDir );

    CMutexGuard tMutexGuard( mMutex );

    int wd = -1;

    stu_MONITOR_INFO stuMonitorInfo;
    stuMonitorInfo.wd            = wd;
    stuMonitorInfo.MonitorPath   = pMonitorDir;
    std::string ts;
    if ( monitorFiles != NULL )
    {
        b_write_log(_INFO,"[%s],monitorFiles=[%s]",pMonitorDir, monitorFiles );

        ts = monitorFiles;
        spStrToVec( ts, "|", stuMonitorInfo.MonitorFiles );
    }
    if ( monitorPrefix != NULL )
    {
        b_write_log(_INFO,"[%s],monitorPrefix=[%s]",pMonitorDir, monitorPrefix );

        ts = monitorPrefix;
        spStrToVec( ts, "|", stuMonitorInfo.MonitorPrefix );
    }
    if ( monitorSuffix != NULL )
    {
        b_write_log(_INFO,"[%s],monitorSuffix=[%s]",pMonitorDir,monitorSuffix );

        ts = monitorSuffix;
        spStrToVec( ts, "|", stuMonitorInfo.MonitorSuffix );
    }

    
    //处理程序启动之前监视目录已经有的(存量)文件
    handStockFiles( stuMonitorInfo, event );



    wd = inotify_add_watch( mMonitorFd, pMonitorDir, SetEvent( event ) );
    if ( wd < 0 )
    {
        b_write_log(_ERROR,"inotify_add_watch( %d, %s, %u) "
                "return[%d],errno=[%d,errMsg=[%s]",
                 mMonitorFd, pMonitorDir, event, 
                 wd, errno, strerror(errno) );

        return false;

    }

    b_write_log(_INFO,"inotify_add_watch [%s] return [%d]", 
            pMonitorDir, wd );




    stuMonitorInfo.wd            = wd;
    mVecMonitorInfo.push_back( stuMonitorInfo );

    if ( !isAlive() )
    {
        startThread();
    }


    return true;
}


bool FileMonitor::rmWatch( const char* pMonitorDir )
{
    CMutexGuard tMutexGuard( mMutex );
    
    std::vector<stu_MONITOR_INFO>::iterator it;
    for( it = mVecMonitorInfo.begin();
         it != mVecMonitorInfo.end();
         it++
       )
    {
        if ( it->MonitorPath == pMonitorDir )
        {
            int ret = inotify_rm_watch( mMonitorFd, it->wd );

            b_write_log(_INFO,"stop [%s] monitor return [%d]",pMonitorDir, ret);

            mVecMonitorInfo.erase( it );
            break;
        }
    }
    return true;
}

bool FileMonitor::rmAllWatch()
{
    CMutexGuard tMutexGuard( mMutex );

    std::vector<stu_MONITOR_INFO>::iterator it;
    for( it = mVecMonitorInfo.begin();
         it != mVecMonitorInfo.end();
       )
    {
        int ret = inotify_rm_watch( mMonitorFd, it->wd );

        b_write_log(_INFO,"stop [%s] monitor return [%d]",
                it->MonitorPath.c_str(), ret);

        mVecMonitorInfo.erase( it );
    }
    return true;
}


static int setJsonMonitorInfo( 
        inotify_event* event, 
        const char* monitorPath, 
        Json::Value &eventAttr 
        )
{

    b_write_log(_DEBUG,"event-mask:[%08x]",event->mask );

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
        case IN_MODIFY:
            {
                eventAttr["Parameter"]["Operation"] = STORAGE_FILE_MODIFY;
                break;
            }
        case IN_CLOSE_WRITE:
            {
                eventAttr["Parameter"]["Operation"] = STORAGE_FILE_CLOSEWRITE;
                break;
            }
        default:
            {
                b_write_log(_ERROR,"event-mask:[%08x] is not support", 
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
    char data[MONI_DATASIZE] = {0};

    pid_t tid;
    tid = syscall(SYS_gettid);
    b_write_log(_INFO,"thread id[%d]", tid);

    isRun = 1;

    while( isAlive() )
    {
        memset( data, 0, MONI_DATASIZE );
        num = read( mMonitorFd, data, MONI_DATASIZE );
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
                b_write_log(_ERROR,"wd=[%d] not fount monitor path!",event->wd);
            }
            else
            {
                iRet = checkMonitorFile( event->name, stuMonitorInfo );
                
                b_write_log(_DEBUG,"[%s] checkMonitorFile(%s) return[%d]",
                         stuMonitorInfo.MonitorPath.c_str() ,event->name,iRet);

                if ( iRet == 0 )
                {
                    Json::Value eventAttr;
                    iRet = setJsonMonitorInfo( event, 
                            stuMonitorInfo.MonitorPath.c_str(),
                            eventAttr );
                    if ( iRet == 0 )
                    {
                        //eventAttr.toStyledString().c_str() );

                        b_write_log(_DEBUG,"eventAttr:[%s]",
                                eventAttr.toStyledString().c_str() );

                        IEventManager::Initialize()->notifyEvent(
                                STORAGE_EVENT,
                                IEventManager::EVENT_START,
                                eventAttr );
                    }
                    else
                    {
                        b_write_log(_ERROR,"setJsonMonitorInfo return fail");
                    }
                }
            }

            pEvent += sizeof( struct inotify_event ) + event->len;

        }// end for


        PauseThreadSleep( 0, 10 );
    }

    return;
}



FileMonitor* FileMonitor::Instance()
{
    if ( mInstance == NULL )
    {
        pthread_mutex_lock( &gsMutex );
        if ( mInstance == NULL )
        {
            mInstance = new FileMonitor();
        }
        pthread_mutex_unlock( &gsMutex );
    }

    return mInstance;
}


