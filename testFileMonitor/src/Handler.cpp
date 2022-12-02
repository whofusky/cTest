/***********************************************************
 *
 * @file    Handler.h
 *
 * @brief   Handler源文件
 *
 * @author  
 *
 * @date    2021-07-03
 *
 * @version V10.010.000
 *
 ***********************************************************/

#include <stdio.h>

#include "Handler.h"
#include "Looper.h"
#include "CustomOutLog.h"

Handler::Handler()
{
	mRet = Json::Value::null;
}

Handler::~Handler()
{
	
}


void Handler::handleMessage( Message& msg )
{
	return;
}


bool Handler::sendMessage( Message& msg )
{
	Looper* loop = (Looper*) msg.mTargetLooper;

	if( loop == NULL )
	{
        b_write_log(_ERROR,"sendMessage loop is NULL");
		return false;
	}
	loop->mQueue->enqueueMessage( msg, 0);
	return true;
}


bool Handler::sendMessageDelayed( Message& msg, long delayMillis )
{
	Looper* loop = (Looper*) msg.mTargetLooper;

	if( loop == NULL )
	{
        b_write_log(_ERROR,"sendMessageDelayed loop is NULL");
		return false;
	}
	loop->mQueue->enqueueMessage( msg, delayMillis );
	return true;
}

void Handler::setCallBackRet( Json::Value& ret )
{
	mRet = ret;
}


Json::Value&  Handler::getCallBackRet()
{
	Json::Value& ret = mRet;
	return ret;
}
