/***********************************************************
 *
 * @file    IEventManager.h
 *
 * @brief   IEventManager头文件
 *
 * @author  
 *
 * @date    2022-08-03
 *
 * @version V10.010.000
 *
 ***********************************************************/

#ifndef _I_EVENTMANAGER_H_
#define _I_EVENTMANAGER_H_

#include <map>
#include "json/json.h"
#include "Thread.h"
#include "Handler.h"
#include "Looper.h"
#include "Mutex.h"
#include "IEventMeta.h"

class IEventManager: public CThread
{
private:
    IEventManager();
    ~IEventManager();
    
public:
    static IEventManager* Initialize();

public:
    //功能启动接口
    unsigned char start();
    
    //功能关闭接口
    unsigned char stop();

    typedef enum event_action
    {
        EVENT_START,
        EVENT_STOP,
        EVENT_BLINKED
    }EVENT_ACTION;

    //具体的事件处理虚接口类
    //
    class IEventHandler
    {
        public:
            virtual ~IEventHandler(){}

            virtual void onAppEvent( 
                    const char* event,
                    EVENT_ACTION action,
                    Json::Value& envetAttr 
                    ) = 0;
    };

    typedef void ( IEventHandler::*HANDLER_FUNC ) (
            EVENT_ACTION action,
            Json::Value& eventAttr 
            );

    //注册监视事件
    //
    unsigned char attachEventHandler( 
            const char* event,
            IEventManager::IEventHandler* obj,
            HANDLER_FUNC func 
            );

    //放弃对特定事件的监视
    //
    unsigned char detachEventHandler( 
            const char* event,
            IEventManager::IEventHandler* obj
            );

    //通告事件的发生
    //
    unsigned char notifyEvent( 
            const char* event,
            EVENT_ACTION action,
            Json::Value& eventAttr 
            );
    
private:

    //事件处理Handler,负责具体事件的处理
    //
    class IEventManagerHandler: public Handler
    {
        public:

            typedef enum event_manager_msg
            {
                EVENT_MANAGER_OUTER_MSG,
                EVENT_MANAGER_INNER_MSG
            }EVENT_MANAGER_MSG;

            IEventManagerHandler( IEventManager* parser );
            ~IEventManagerHandler();

            void handleMessage( Message& msg );

        private:
            IEventManager* mEventManager;
    };

    typedef struct cb_node
    {
        IEventHandler*  CB_Obj;
        HANDLER_FUNC    CB_Func;
        struct cb_node* CB_Next;
    }CB_NODE;

    void threadHandler();

    Handler* mHandler;
    Looper*  mLooper;
    std::map< std::string, CB_NODE* > mCB;
        
private:
        static IEventManager* mInstance;
        CMutex                mLock;

};


#endif//_I_EVENTMANAGER_H_
