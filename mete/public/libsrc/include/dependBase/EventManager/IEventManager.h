/***********************************************************
*
*
*  IEventManager.h
*
*  Create by muxd
*
*  2017/07/10
***********************************************************/
#ifndef _I_EVENTMANAGER_H_
#define _I_EVENTMANAGER_H_
#include <map>
#include "base/Base.h"
#include "json/json.h"
#include "thread/Thread.h"
#include "thread/Handler.h"
#include "thread/Looper.h"

class IEventManager: public CThread
{
private:
    IEventManager();

    ~IEventManager();
public:
    static IEventManager* Initialize();

public:
    U8 start();

    U8 stop();

    typedef enum event_action
    {
        EVENT_START,
        EVENT_STOP,
        EVENT_BLINKED,
    }EVENT_ACTION;

    class IEventHandler
    {
    public:
        virtual ~IEventHandler() {}

        virtual void onAppEvent( const char* event,
                                 EVENT_ACTION action,
                                 Json::Value eventAttr ) = 0;

    };

    typedef void ( IEventHandler::*HANDLER_FUNC )(
        EVENT_ACTION action,
        Json::Value eventAttr );


    U8 attachEventHandler( const char* event,
                           IEventManager::IEventHandler* obj,
                           HANDLER_FUNC func );

    U8 detachEventHandler( const char* event,
                           IEventManager::IEventHandler* obj,
                           HANDLER_FUNC func );


    U8 notifyEvent(const char* event,
                   EVENT_ACTION action,
                   Json::Value eventAttr );
private:

    class IEventManagerHandler: public Handler
    {
    public:
        typedef enum event_manager_msg
        {
            EVENT_MAMANGER_OUTER_MSG,
            EVENT_MAMANGER_INNER_MSG
        } EVENT_MAMANGER_MSG;

        IEventManagerHandler( IEventManager* parser );
        ~IEventManagerHandler();

        void handleMessage( Message msg );

    private:
        IEventManager* mEventManager;
    };

    typedef struct cb_node
    {
        IEventHandler*  CB_Obj;
        HANDLER_FUNC    CB_Func;
        struct cb_node* CB_Next;
    } CB_NODE;

    void threadHandler();

    Handler*      mHandler;
    Looper*       mLooper;
    std::map< std::string, CB_NODE* > mCB;

private:

    static IEventManager* mInstance;
};

#endif //_I_EVENTMANAGER_H_

