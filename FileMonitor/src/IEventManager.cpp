/***********************************************************
 *
 * @file    IEventManager.cpp
 *
 * @brief   IEventManager源文件
 *
 * @author  
 *
 * @date    2022-09-13
 *
 * @version V10.010.000
 *
 ***********************************************************/

#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "InfraBase.h"
#include "IEventManager.h"
#include "CustomOutLog.h"


static pthread_mutex_t  gsMutex = PTHREAD_MUTEX_INITIALIZER;


IEventManager* IEventManager::mInstance = NULL;
int IEventManager::isRun = 0;

IEventManager::IEventManagerHandler::IEventManagerHandler(
        IEventManager* eventManager )
{
    mEventManager = eventManager;
}


IEventManager::IEventManagerHandler::~IEventManagerHandler()
{

}

void IEventManager::IEventManagerHandler::handleMessage( Message& msg )
{
    //HANDLER_FUNC tFunc; //此处注释是因为在后面的逻辑中暂时未调用

    //event string type: msg.mMetaStr
    //

    ////此处注释是因为可以直接用msg.mMetaStr
    //std::string tEvent( msg.mMetaStr ); 
    
    IEventHandler* tObj;
    CMutexGuard autoGuard( mEventManager->mLock );

    std::map<std::string, CB_NODE*>::iterator it;

    ////此处注释是因为可以直接用msg.mMetaStr
    //it = mEventManager->mCB.find( tEvent ); 

    it = mEventManager->mCB.find( msg.mMetaStr );

    if( it == mEventManager->mCB.end() )
    {
        return;
    }

    CB_NODE* tNode = it->second;
    while( tNode != NULL )
    {

        tObj  = tNode->CB_Obj;
        EVENT_ACTION action = (EVENT_ACTION)msg.mArg1;

        ////此处注释是因为在后面的逻辑中暂时未调用
        //tFunc = tNode->CB_Func;
        
        ////此处注释是因为可以直接用msg.mMetaStr
        //tObj->onAppEvent( tEvent.c_str(), action, msg.mAttr ); 

        tObj->onAppEvent( msg.mMetaStr.c_str(), action, msg.mAttr );

        tNode = tNode->CB_Next;
    }

    return;
}

IEventManager::IEventManager()
{
    mHandler = NULL;
}


IEventManager::~IEventManager()
{
    if( mHandler != NULL )
    {
        delete mHandler;
        mHandler = NULL;
    }
}

unsigned char IEventManager::start( const int &pre_val )
{
    if ( pre_val != -1 )
    {
        while( 0 == pre_val )
        {
            PauseThreadSleep( 0, 10 );
        }
        b_write_log(_DEBUG,"Wait for the prepend to end!");
    }

    if( mHandler == NULL )
    {
        mHandler = new IEventManagerHandler( this );
    }
    else
    {
        return True;
    }
    startThread();

    b_write_log(_DEBUG,"IEventManager::start() Done");

    return True;
}

unsigned char IEventManager::stop()
{
    //注意Looper与线程释放的顺序，不要颠倒
    stopThread();
    mLooper->decRef();
    return True;
}

unsigned char IEventManager::attachEventHandler(
        const char* event,
        IEventHandler* obj,
        HANDLER_FUNC func )
{
    std::string tEvent( event );
    std::map<std::string, CB_NODE*>::iterator it;
    CMutexGuard autoGuard( mLock );

    for( it = mCB.begin(); it != mCB.end(); it++ )
    {
        if( it->first == tEvent )
        {
            CB_NODE* node = it->second;
            while( node->CB_Next != NULL )
            {
                node = node->CB_Next;
            }
            CB_NODE* tCB_Node = (CB_NODE*) calloc( 1, sizeof( CB_NODE ) );
            tCB_Node->CB_Obj  = obj;
            tCB_Node->CB_Func = func;
            tCB_Node->CB_Next = NULL;

            node->CB_Next     = tCB_Node;

            return True;
        }
    }

    CB_NODE* tCB_Node = (CB_NODE*) calloc( 1, sizeof( CB_NODE ) );
    tCB_Node->CB_Obj  = obj;
    tCB_Node->CB_Func = func;
    tCB_Node->CB_Next = NULL;

    mCB.insert( make_pair( tEvent, tCB_Node ) );

    return True;
}

unsigned char IEventManager::detachEventHandler(
        const char* event,
        IEventManager::IEventHandler* obj )
{
    std::string tEvent( event );
    std::map<std::string, CB_NODE*>::iterator it;
    CMutexGuard autoGuard( mLock );

    for( it = mCB.begin(); it != mCB.end(); )
    {
        if( it->first == tEvent )
        {
            CB_NODE* node    = it->second;
            CB_NODE* preNode = it->second;
            while( node != NULL )
            {
                if( node->CB_Obj == obj )
                {
                    char vHeadFlag = 0;
                    if ( node == it->second )
                    {
                        it->second = node->CB_Next;
                        preNode    = node->CB_Next;
                        vHeadFlag  = 1;
                    }
                    else
                    {
                        preNode->CB_Next = node->CB_Next;
                    }

                    free( node );
                    if ( vHeadFlag == 1 )
                    {
                        node = preNode;
                    }
                    else if ( preNode != NULL )
                    {
                        node = preNode->CB_Next;
                    }
                    else
                    {
                        node = NULL;
                    }
                }
                else
                {
                    preNode = node;
                    node    = node->CB_Next;
                }
            } //end  while( node != NULL && node->CB_Next != NULL )

            if( it->second == NULL )
            {
                mCB.erase( it++ );
            }
            else
            {
                it++;
            }
        } //end if( it->first == tEvent )
        else
        {
            it++;
        }
    }
    return True;
}


unsigned char IEventManager::notifyEvent(
        const char* event,
        EVENT_ACTION action,
        Json::Value& evenAttr )
{
    Message msg;
    msg.mWhat    = IEventManagerHandler::EVENT_MANAGER_OUTER_MSG;
    msg.mArg1    = action;
    msg.mMetaStr = event;
    msg.mAttr    = evenAttr;
    msg.mTarget  = mHandler;
    msg.setValid( true );
    msg.mTargetLooper = mLooper;

    mHandler->sendMessage( msg );

    return True;
}

IEventManager* IEventManager::Initialize()
{
    if( mInstance == NULL )
    {
        pthread_mutex_lock( &gsMutex );
        if( mInstance == NULL )
        {
            mInstance = new IEventManager;
        }
        pthread_mutex_unlock( &gsMutex );
    }

    return mInstance;
}

void IEventManager::threadHandler()
{
    pid_t tid;
    tid = syscall(SYS_gettid);
    b_write_log(_INFO,"thread id[%d]", tid);

    mLooper = Looper::getLooper();
    mLooper->incRef();
    mLooper->prepare();

    isRun = 1;

    mLooper->Loop();

    return;
}

