/***********************************************************
 *
 * @file    Looper.h
 *
 * @brief   Looper头文件
 *
 * @author  
 *
 * @date    2021-07-04
 *
 * @version V10.010.000
 *
 ***********************************************************/

#ifndef _LOOPER_H_
#define _LOOPER_H_

#include <map>
#include <pthread.h>
#include "MessageQueue.h"
#include "Mutex.h"




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
private:
	int                                 mRefNum;

};




#endif//_LOOPER_H_
