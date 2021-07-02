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

    //消息进入队列
    bool enqueueMessage( 
        Message msg, 
        long delayMillis );

    Message  dequeueMessage();

private:
    std::list<Message> mQueue; //list本身不是多线程安全的
    CMutex             mMutex;
};

#endif //_MESSAGE_QUEUE_H_

