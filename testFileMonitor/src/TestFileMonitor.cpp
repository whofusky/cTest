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

#include <libgen.h>
#include "TestFileMonitor.h"
#include <string.h>


TestFileMonitor* TestFileMonitor::mInstance = NULL;


TestFileMonitor::TestFileHandler::TestFileHandler( TestFileMonitor* testFileM )
{
    mFileTestMonitor = testFileM;
}

TestFileMonitor::TestFileHandler::~TestFileHandler()
{

}

void TestFileMonitor::TestFileHandler::handleMessage( Message& msg )
{
    printf("%s:%s:%d:DEBUG:[evetAttr:[%s]]\n",
            __FILE__,__FUNCTION__,__LINE__, 
            msg.mAttr.toStyledString().c_str() );

    switch( msg.mWhat )
    {
        case MSG_STOCK_FILE:
            {
                
                printf("%s:%s:%d:DEBUG:[MSG_STOCK_FILE [%s]]\n", 
                        __FILE__,__FUNCTION__,__LINE__,
                        msg.mAttr["Parameter"]["FileName"].asCString() );
            }
            break;
        case MSG_CREAT_FILE:
            {
                
                printf("%s:%s:%d:DEBUG:[MSG_CREAT_FILE [%s]]\n", 
                        __FILE__,__FUNCTION__,__LINE__,
                        msg.mAttr["Parameter"]["FileName"].asCString() );
            }
            break;
        case MSG_MODIF_FILE:
            {
                
                printf("%s:%s:%d:DEBUG:[MSG_MODIF_FILE [%s]]\n", 
                        __FILE__,__FUNCTION__,__LINE__,
                        msg.mAttr["Parameter"]["FileName"].asCString() );
            }
            break;
        case MSG_DELET_FILE:
            {
                
                printf("%s:%s:%d:DEBUG:[MSG_DELET_FILE [%s]]\n", 
                        __FILE__,__FUNCTION__,__LINE__,
                        msg.mAttr["Parameter"]["FileName"].asCString() );
            }
            break;
        case MSG_MOVTO_FILE:
            {
                
                printf("%s:%s:%d:DEBUG:[MSG_MOVTO_FILE [%s]]\n", 
                        __FILE__,__FUNCTION__,__LINE__,
                        msg.mAttr["Parameter"]["FileName"].asCString() );
            }
            break;
        case MSG_MOVFM_FILE:
            {
                
                printf("%s:%s:%d:DEBUG:[MSG_MOVFM_FILE [%s]]\n", 
                        __FILE__,__FUNCTION__,__LINE__,
                        msg.mAttr["Parameter"]["FileName"].asCString() );
            }
            break;
        default:
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


void TestFileMonitor::start()
{
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

    printf("%s:%s:%d:DEBUG:[TestFileMonitor::start() Done!]\n",
            __FILE__,__FUNCTION__,__LINE__);

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
    printf("%s:%s:%d:DEBUG:[eventAttr=[%s]!]\n", 
            __FILE__,__FUNCTION__,__LINE__,
            eventAttr.toStyledString().c_str() );

    std::string tOpName = eventAttr["Parameter"]["Operation"].asCString();

    if ( tOpName == STORAGE_FILE_STOCK )
    {
        printf("%s:%s:%d:DEBUG:[topName=[%s]!]\n",
                __FILE__,__FUNCTION__,__LINE__,STORAGE_FILE_STOCK );
        msg.mWhat = TestFileHandler::MSG_STOCK_FILE;
    }
    else if ( tOpName == STORAGE_FILE_ADDED )
    {
        printf("%s:%s:%d:DEBUG:[topName=[%s]!]\n",
                __FILE__,__FUNCTION__,__LINE__,STORAGE_FILE_ADDED );
        msg.mWhat = TestFileHandler::MSG_CREAT_FILE;
    }
    else if ( tOpName == STORAGE_FILE_MODIFY )
    {
        printf("%s:%s:%d:DEBUG:[topName=[%s]!]\n",
                __FILE__,__FUNCTION__,__LINE__,STORAGE_FILE_MODIFY );
        msg.mWhat = TestFileHandler::MSG_MODIF_FILE;
    }
    else if ( tOpName == STORAGE_FILE_DELETED )
    {
        printf("%s:%s:%d:DEBUG:[topName=[%s]!]\n",
                __FILE__,__FUNCTION__,__LINE__,STORAGE_FILE_DELETED );
        msg.mWhat = TestFileHandler::MSG_DELET_FILE;
    }
    else if ( tOpName == STORAGE_FILE_MOVEDTO )
    {
        printf("%s:%s:%d:DEBUG:[topName=[%s]!]\n",
                __FILE__,__FUNCTION__,__LINE__,STORAGE_FILE_MOVEDTO );
        msg.mWhat = TestFileHandler::MSG_MOVTO_FILE;
    }
    else if ( tOpName == STORAGE_FILE_MOVEDFROM )
    {
        printf("%s:%s:%d:DEBUG:[topName=[%s]!]\n",
                __FILE__,__FUNCTION__,__LINE__,STORAGE_FILE_MOVEDFROM );
        msg.mWhat = TestFileHandler::MSG_MOVFM_FILE;
    }
    else
    {
        printf("%s:%s:%d:DEBUG:[topName=[%s]!]\n",
                __FILE__,__FUNCTION__,__LINE__,"null" );
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
    Looper* me = Looper::getLooper();
    mLooper = me;
    mLooper->incRef();
    mLooper->prepare();
    mLooper->Loop();
}

TestFileMonitor* TestFileMonitor::Instance()
{
    if ( mInstance == NULL )
    {
        mInstance = new TestFileMonitor();
    }
    return mInstance;
}





