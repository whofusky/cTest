/***********************************************************
*
*
*  Message.h
*
*  Create by muxd
*
*  2017/07/10
***********************************************************/
#ifndef _MESSAGE_H_
#define _MESSAGE_H_
#include <string>
#include "base/Base.h"
#include "json/json.h"
#include "ICallBack.h"

class Message
{
public:
    Message();
    ~Message();

    Message& operator = ( Message& obj );

    bool isValid();

    void setValid( bool flag );

public:

    int         mWhat;
    int         mArg1;
    int         mArg2;
    std::string mMetaStr;
    Json::Value mAttr;
    void*       mTarget;
    void*       mTargetLooper;
    ICallBack*  mCallBack;

    long        mWhen;

private:

    bool  mIsValid;

};

#endif

