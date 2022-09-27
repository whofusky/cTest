/***********************************************************
 *
 * @file    MessageQueue.h
 *
 * @brief   MessageQueue头文件
 *
 * @author  
 *
 * @date    2021-07-04
 *
 * @version V10.010.000
 *
 ***********************************************************/

#ifndef _MESSAGEQUEUE_H_
#define _MESSAGEQUEUE_H_

#include <list>
#include <pthread.h>
#include "Message.h"
#include "Mutex.h"



// use pipe  or list
class MessageQueue
{
public:

	#define MESSAGE_QUEUE_SIZE 100

	MessageQueue();
    ~MessageQueue();

	//消息进入队列
	bool enqueueMessage( Message& msg, long delayMillis );

	Message dequeueMessage();

private:
	std::list<Message> mQueue; //list本身不是多线程安全的
	CMutex             mMutex;

};




#endif//_MESSAGEQUEUE_H_
