/***********************************************************
*
*
*  MessageQueue.h
*
*  Create by muxd
*
*  2017/07/10
***********************************************************/
#ifndef _MESSAGE_QUEUE_H_
#define _MESSAGE_QUEUE_H_
#include <list>
#include "thread/Message.h"
#include "mutex/Mutex.h"

// Use Pipe  or List
class MessageQueue
{
public:

    #define MESSAGE_QUEUE_SIZE 100

    MessageQueue();
    ~MessageQueue();

    //��Ϣ�������
    bool enqueueMessage( 
        Message msg, 
        long delayMillis );

    Message  dequeueMessage();

private:
    std::list<Message> mQueue; //list�����Ƕ��̰߳�ȫ��
    CMutex             mMutex;
};

#endif //_MESSAGE_QUEUE_H_

