/***********************************************************
*
*
*  ICallBack.h
*
*  Create by muxd
*
*  2017/09/04
***********************************************************/
#ifndef _I_CALLBACK_H_
#define _I_CALLBACK_H_
#include "json/value.h"

class ICallBack
{
public:
    virtual ~ICallBack(){};

    virtual void handleCallBack( Json::Value& Ret ) = 0;
};

#endif //_I_CALLBACK_H_
