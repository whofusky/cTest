/***********************************************************
 *
 * @file    FileReceiver.cpp
 *
 * @brief   FileReceiver源文件
 *
 * @author  fu.sky
 *
 * @date    2022-10-31_17:45:34
 *
 * @version V10.010.000
 *
 ***********************************************************/

#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <libgen.h>
#include "FileReceiver.h"
#include <string.h>
#include "PvCfg.h"
#include "CustomOutLog.h"
#include "InfraBase.h"

static pthread_mutex_t  gsMutex = PTHREAD_MUTEX_INITIALIZER;

FileReceiver* FileReceiver::mInstance = NULL;
int FileReceiver::isRun = 0;


FileReceiver::FileRcvHandler::FileRcvHandler( FileReceiver* inRcver )
{
    mRcver = inRcver;
}

FileReceiver::FileRcvHandler::~FileRcvHandler()
{

}

void FileReceiver::FileRcvHandler::handleMessage( Message& msg )
{
    //c_write_log(_DEBUG,"evetAttr:[%s]", msg.mAttr.toStyledString().c_str() );

    std::string tpath  = msg.mAttr["Parameter"]["Dir"].asString();
    std::string tfname = msg.mAttr["Parameter"]["FileName"].asString();
    std::string tfop   = msg.mAttr["Parameter"]["Operation"].asString();

    int ret = 0;

    c_write_log(_DEBUG,"[%s][%s][%s]]", 
            tpath.c_str(), tfop.c_str(), tfname.c_str() );

    switch( msg.mWhat )
    {
           
        case MSG_MOVTO_FILE:
        case MSG_CLOSER_FILE:
            {
                if ( PvCfg::mPubCfgPath ==  tpath 
                        && PvCfg::mPubCfgFile == tfname )
                {
                    //更新公共配置文件
                    ret = PvReLoadPubCfg();

                    c_write_log(_DEBUG,"PvReLoadPubCfg run ret=[%s]", 
                            PvCfgErrMsg(ret) );
                }
                else if ( PvCfg::mSftCfgPath ==  tpath 
                        && PvCfg::mSftCfgFile == tfname )
                {
                    //更新软件私有配置文件
                    ret = PvReLoadSftCfg();

                    c_write_log(_DEBUG,"PvReLoadSftCfg run ret=[%s]", 
                            PvCfgErrMsg(ret) );
                }
                
            }
            break;
        default:
            break;
    }

    return;
}


FileReceiver::FileReceiver()
{
    mHandler = NULL;
}


FileReceiver::~FileReceiver()
{
    if ( mHandler != NULL )
    {
        delete mHandler;
        mHandler = NULL;
    }
}


void FileReceiver::start( const int &pre_val )
{
    if ( pre_val != -1 )
    {
        while( 0 == pre_val )
        {
            PauseThreadSleep( 0, 10 );
        }
        c_write_log(_DEBUG,"Wait for the prepend to end!");
    }

    if ( mHandler == NULL )
    {
        mHandler = new FileRcvHandler( this );
    }
    else
    {
        return;
    }

    if ( isAlive() )
    {
        return;
    }

    IEventManager::Initialize()->attachEventHandler(
            STORAGE_EVENT,
            (IEventManager::IEventHandler*) this,
            (IEventManager::HANDLER_FUNC) &FileReceiver::onAppEvent
            );

    startThread();

    c_write_log(_DEBUG,"FileReceiver::start() Done!");

    return;
}


void FileReceiver::stop()
{
    if ( !isAlive() || mHandler == NULL )
    {
        return;
    }

    //Looper 与 线程释放顺序不能颠倒
    stopThread();
    mLooper->decRef();
    return;
}


void FileReceiver::onAppEvent( 
        const char* event,
        IEventManager::EVENT_ACTION action,
        Json::Value& eventAttr 
        )
{
    if ( strcmp( event, STORAGE_EVENT) != 0 )
    {
        return;
    }

    Message msg;
    
    //打印json值eventAttr (格式化后输出)
    c_write_log(_DEBUG,"eventAttr=[%s]!", eventAttr.toStyledString().c_str() );

    std::string tOpName = eventAttr["Parameter"]["Operation"].asString();

    c_write_log(_DEBUG,"topName=[%s]!",tOpName.c_str() );

    if ( tOpName == STORAGE_FILE_STOCK )
    {
        msg.mWhat = FileRcvHandler::MSG_STOCK_FILE;
    }
    else if ( tOpName == STORAGE_FILE_ADDED )
    {
        msg.mWhat = FileRcvHandler::MSG_CREAT_FILE;
    }
    else if ( tOpName == STORAGE_FILE_MODIFY )
    {
        msg.mWhat = FileRcvHandler::MSG_MODIF_FILE;
    }
    else if ( tOpName == STORAGE_FILE_DELETED )
    {
        msg.mWhat = FileRcvHandler::MSG_DELET_FILE;
    }
    else if ( tOpName == STORAGE_FILE_MOVEDTO )
    {
        msg.mWhat = FileRcvHandler::MSG_MOVTO_FILE;
    }
    else if ( tOpName == STORAGE_FILE_MOVEDFROM )
    {
        msg.mWhat = FileRcvHandler::MSG_MOVFM_FILE;
    }
    else if ( tOpName == STORAGE_FILE_CLOSEWRITE )
    {
        msg.mWhat = FileRcvHandler::MSG_CLOSER_FILE;
    }
    else
    {
        msg.mWhat = FileRcvHandler::MSG_NULL;
    }

    msg.mMetaStr = event;
    msg.mArg1    = action;
    msg.mAttr    = eventAttr;
    msg.setValid( true);
    msg.mTarget  = mHandler;
    msg.mTargetLooper = mLooper;

    mHandler->sendMessage( msg );

    return;
}

void FileReceiver::threadHandler()
{

    pid_t tid;
    tid = syscall(SYS_gettid);
    c_write_log(_INFO,"thread id[%d]", tid);


    Looper* me = Looper::getLooper();
    mLooper = me;
    mLooper->incRef();
    mLooper->prepare();

    isRun = 1;

    mLooper->Loop();
}

FileReceiver* FileReceiver::Instance()
{
    if ( mInstance == NULL )
    {
        pthread_mutex_lock( &gsMutex );
        if ( mInstance == NULL )
        {
            mInstance = new FileReceiver();
        }
        pthread_mutex_unlock( &gsMutex );
    }

    return mInstance;
}





