/***********************************************************
 *
 * @file    TestFileMonitor.cpp
 *
 * @brief   TestFileMonitor源文件
 *
 * @author  fu.sky
 *
 * @date    2022-10-09_15:35:46
 *
 * @version V10.010.000
 *
 ***********************************************************/

#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include "TestFileMonitor.h"
#include "CustomOutLog.h"
#include "InfraBase.h"


static pthread_mutex_t  gsMutex = PTHREAD_MUTEX_INITIALIZER;

TestFileMonitor* TestFileMonitor::mInstance = NULL;
int TestFileMonitor::isRun = 0;


TestFileMonitor::TestFileHandler::TestFileHandler( TestFileMonitor* testFileM )
{
    mFileTestMonitor = testFileM;
}

TestFileMonitor::TestFileHandler::~TestFileHandler()
{

}

void TestFileMonitor::TestFileHandler::handleMessage( Message& msg )
{
    //c_write_log(_DEBUG,"evetAttr:[%s]",msg.mAttr.toStyledString().c_str() );

    std::string tpath  = msg.mAttr["Parameter"]["Dir"].asString();
    std::string tfname = msg.mAttr["Parameter"]["FileName"].asString();
    std::string tfop   = msg.mAttr["Parameter"]["Operation"].asString();

    c_write_log(_DEBUG,"[%s][%s][%s]", 
            tpath.c_str(), tfop.c_str(), tfname.c_str() );

    switch( msg.mWhat )
    {
        case MSG_STOCK_FILE:
            {
                c_write_log(_DEBUG,"MSG_STOCK_FILE");
            }
            break;
        case MSG_CREAT_FILE:
            {
                c_write_log(_DEBUG,"MSG_CREAT_FILE");
            }
            break;
        case MSG_MODIF_FILE:
            {
                c_write_log(_DEBUG,"MSG_MODIF_FILE");
            }
            break;
        case MSG_DELET_FILE:
            {
                c_write_log(_DEBUG,"MSG_DELET_FILE");
            }
            break;
        case MSG_MOVTO_FILE:
            {
                c_write_log(_DEBUG,"MSG_MOVTO_FILE");
            }
            break;
        case MSG_MOVFM_FILE:
            {
                c_write_log(_DEBUG,"MSG_MOVFM_FILE");
            }
            break;
        case MSG_CLOSER_FILE:
            {
                c_write_log(_DEBUG,"MSG_CLOSER_FILE");
            }
            break;
        default:
            c_write_log(_DEBUG,"MSG type not recognized");
            break;
    }

    return;
}


TestFileMonitor::TestFileMonitor()
{
    mHandler = NULL;
}


TestFileMonitor::~TestFileMonitor()
{
    if ( mHandler != NULL )
    {
        delete mHandler;
        mHandler = NULL;
    }
}


void TestFileMonitor::start( const int &pre_val )
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
        mHandler = new TestFileHandler( this );
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
            (IEventManager::HANDLER_FUNC) &TestFileMonitor::onAppEvent
            );

    startThread();

    c_write_log(_DEBUG,"TestFileMonitor::start() Done!");

    return;
}


void TestFileMonitor::stop()
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


void TestFileMonitor::onAppEvent( 
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
    c_write_log(_DEBUG,"eventAttr=[%s]!",eventAttr.toStyledString().c_str() );

    std::string tOpName = eventAttr["Parameter"]["Operation"].asString();

    c_write_log(_DEBUG,"topName=[%s]!",tOpName.c_str() );
    

    if ( tOpName == STORAGE_FILE_STOCK )
    {
        msg.mWhat = TestFileHandler::MSG_STOCK_FILE;
    }
    else if ( tOpName == STORAGE_FILE_ADDED )
    {
        msg.mWhat = TestFileHandler::MSG_CREAT_FILE;
    }
    else if ( tOpName == STORAGE_FILE_MODIFY )
    {
        msg.mWhat = TestFileHandler::MSG_MODIF_FILE;
    }
    else if ( tOpName == STORAGE_FILE_DELETED )
    {
        msg.mWhat = TestFileHandler::MSG_DELET_FILE;
    }
    else if ( tOpName == STORAGE_FILE_MOVEDTO )
    {
        msg.mWhat = TestFileHandler::MSG_MOVTO_FILE;
    }
    else if ( tOpName == STORAGE_FILE_MOVEDFROM )
    {
        msg.mWhat = TestFileHandler::MSG_MOVFM_FILE;
    }
    else if ( tOpName == STORAGE_FILE_CLOSEWRITE )
    {
        msg.mWhat = TestFileHandler::MSG_CLOSER_FILE;
    }
    else
    {
        c_write_log(_DEBUG,"topName=[%s], not recognized!",tOpName.c_str() );
        msg.mWhat = TestFileHandler::MSG_NULL;
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

void TestFileMonitor::threadHandler()
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

TestFileMonitor* TestFileMonitor::Instance()
{
    if ( mInstance == NULL )
    {
        pthread_mutex_lock( &gsMutex );
        if ( mInstance == NULL )
        {
            mInstance = new TestFileMonitor();
        }
        pthread_mutex_unlock( &gsMutex );
    }
    return mInstance;
}





