/***********************************************************
 *
 * @file    MessageQueue.cpp
 *
 * @brief   MessageQueue源文件
 *
 * @author  
 *
 * @date    2021-07-04
 *
 * @version V10.010.000
 *
 ***********************************************************/

#include <stdio.h>

#include "MessageQueue.h"
#include "CustomOutLog.h"

MessageQueue::MessageQueue()
{}

MessageQueue::~MessageQueue()
{}


bool MessageQueue::enqueueMessage( Message& msg, long delayMillis )
{
	CMutexGuard tGuard(mMutex);

	if( mQueue.size() >= MESSAGE_QUEUE_SIZE )
	{
        b_write_log(_ERROR,"enqueueMessage size is %lu>%d", 
                mQueue.size(), MESSAGE_QUEUE_SIZE );

		return false;
	}

	if( msg.isValid() )
	{
		time_t curTimeT;
		time( &curTimeT );
		msg.mWhen = curTimeT + delayMillis;

		mQueue.push_back( msg );
	}
	else
	{
		return false;
	}

	return true;
}


Message MessageQueue::dequeueMessage()
{
	Message msg;
	CMutexGuard tGuard( mMutex );
	if( !mQueue.empty() )
	{
		std::list<Message>::iterator it;
		for( it= mQueue.begin(); it != mQueue.end(); )
		{
			time_t curTimeT;
			time( &curTimeT );
			if( it->mWhen <= curTimeT )
			{
				msg = *it;
				it = mQueue.erase( it );
				break;
			}
			else
			{
				it++;
			}
		}
	}
	else
	{
		/*标记这个消息是一个无效的消息*/
		msg.setValid( false );
	}
	return msg;
}



