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
//#include "WriteLog.h"

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
        //PrintToStdout( LOGERROR,"sendMessage loop is NULL" );
        printf("%s:%s:%d:ERROR:[sendMessage loop is NULL]\n",
             __FILE__,__FUNCTION__,__LINE__ );
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
        //PrintToStdout( LOGERROR,"sendMessageDelayed loop is NULL" );
        printf("%s:%s:%d:ERROR:[sendMessageDelayed loop is NULL]\n",
             __FILE__,__FUNCTION__,__LINE__ );
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
