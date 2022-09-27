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
#include "IEventManager.h"
#include "Looper.h"
#include "Base.h"
#include "WriteLog.h"


IEventManager* IEventManager::mInstance = NULL;

IEventManager::IEventManagerHandler::IEventManagerHandler(
        IEventManager* eventManager )
{
    mEventManager = eventManager;
}


IEventManager::IEventManagerHandler::~IEventManagerHandler()
{

}

void IEventManager::IEventManagerHandler::handleMessage( Message msg )
{
    std::string tEvent( msg.mMetaStr );
    IEventHandler* tObj;
    HANDLER_FUNC tFunc;
    CMutexGuard autoGuard( mEventManager->mLock );

    std::map<std::string, CB_NODE*>::iterator it;
    it = mEventManager->mCB.find( tEvent );

    if( it == mEventManager->mCB.end() )
    {
        return;
    }

    CB_NODE* tNode = it->second;
    while( tNode != NULL )
    {

        tObj = tNode->CB_Obj;
        tFunc = tNode->CB_Func;
        EVENT_ACTION action = (EVENT_ACTION)msg.mArg1;
        tObj->onAppEvent( tEvent.c_str(), action, msg.mAttr );

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

unsigned char IEventManager::start()
{
    if( mHandler == NULL )
    {
        mHandler = new IEventManagerHandler( this );
    }
    else
    {
        return True;
    }
    startThread();

    PrintToStdout( LOGDEBUG, "IEventManager::start() Done" );

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
            CB_NODE* tCB_Node = (CB_NODE*) malloc( sizeof( CB_NODE ) );
            tCB_Node->CB_Obj  = obj;
            tCB_Node->CB_Func = func;
            tCB_Node->CB_Next = NULL;

            node->CB_Next     = tCB_Node;

            return True;
        }
    }

    CB_NODE* tCB_Node = (CB_NODE*) malloc( sizeof( CB_NODE ) );
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
            while( node != NULL && node->CB_Next != NULL )
            {
                if( node->CB_Obj == obj )
                {
                    if ( preNode == it->second )
                    {
                        it->second = node->CB_Next;
                    }
                    else
                    {
                        preNode->CB_Next = node->CB_Next;
                    }
                    free( node );
                    node = NULL;
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

unsigned char IEventManager::nofifyEvent(
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
    msg.setValid( True );
    msg.mTargetLooper = mLooper;

    mHandler->sendMessage( msg );

    return True;
}

IEventManager* IEventManager::Initialize()
{
    if( mInstance == NULL )
    {
        mInstance = new IEventManager;
    }

    return mInstance;
}

void IEventManager::threadHandler()
{
    mLooper = Looper::getLooper();
    mLooper->incRef();
    mLooper->prepare();
    mLooper->Loop();

    return;
}

