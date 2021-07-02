/***********************************************************
*
*
*  Looper.h
*
*  Create by muxd
*
*  2017/07/10
***********************************************************/
#ifndef _LOOPER_H_
#define _LOOPER_H_
#include <map>
#include <pthread.h>
#include "thread/MessageQueue.h"
#include "mutex/Mutex.h"

class Looper
{
public:
    Looper();
    ~Looper();

    static Looper* getLooper();

    void prepare();

    void incRef();

    void decRef();

    void Loop();

public:
    MessageQueue* mQueue; //消息队列的职能应该放在Looper
    
private:
    static std::map<pthread_t, Looper*> mLooperMap;
    static CMutex                       mLooperMutex;
    int  mRefNum;
};

#endif

