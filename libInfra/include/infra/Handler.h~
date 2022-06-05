/***********************************************************
 *
 * @file    Handler.h
 *
 * @brief   Handler头文件
 *
 * @author  fu.sky
 *
 * @date    2021-07-03
 *
 * @version V10.010.000
 *
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
    virtual void handleMessage( Message msg);

	bool sendMessage( Message msg );

	bool sendMessageDelayed( Message msg, long delayMillis );

	void setCallBackRet( Json::value& ret );

	Json::value& getCallBackRet();

private:
	Json::value mRet;

}




#endif//_HANDLER_H_
