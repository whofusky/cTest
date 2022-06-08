/***********************************************************
 *
 * @file    Message.h
 *
 * @brief   Message头文件
 *
 * @author  fu.sky
 *
 * @date    2021-07-04
 *
 * @version V10.010.000
 *
 ***********************************************************/

#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <string>
#include "json/json.h"
#include "ICallBack.h"

class Message
{
public:
    Message();
    //Message(Message& obj);
    ~Message();
    
    //void Lock( void );
    //void unLock( void );
    
    Message& operator = ( Message& obj );
    
    bool isValid();
    void setValid( bool flag );
    
public:
    int  mWhat;
    int  mArg1;
    int  mArg2;
    std::string mMetaStr;
    Json::Value mAttr;
    void* mTarget;
    void* mTargetLooper;
    ICallBack* mCallBack;
    
    long  mWhen;
    
private:
    bool mIsValid;

};



#endif//_MESSAGE_H_
