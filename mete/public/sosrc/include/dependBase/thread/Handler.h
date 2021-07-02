/***********************************************************
*
*
*  Handler.h
*
*  Create by muxd
*
*  2017/07/10
***********************************************************/
#ifndef _HANDLER_H_
#define _HANDLER_H_
#include "json/value.h"
#include "Message.h"

class Handler
{
public:
    Handler();
    virtual ~Handler();

public:
    virtual void handleMessage( Message msg );

    bool sendMessage( Message msg );

    bool sendMessageDelayed( 
        Message msg, 
        long delayMillis );

    void setCallBackRet( Json::Value& ret );

    Json::Value& getCallBackRet();

private:
    Json::Value mRet;
};

#endif

