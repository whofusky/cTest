/***********************************************************
 *
 * @file    MessageQueue.h
 *
 * @brief   MessageQueueͷ�ļ�
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

	//��Ϣ�������
	bool enqueueMessage( Message& msg, long delayMillis );

	Message dequeueMessage();

private:
	std::list<Message> mQueue; //list�����Ƕ��̰߳�ȫ��
	CMutex             mMutex;

};




#endif//_MESSAGEQUEUE_H_
