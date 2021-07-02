/***********************************************************
*
*
*  Thread.h
*
*  Create by muxd
*
*  2017/07/08
***********************************************************/
#ifndef _CTHREAD_H_
#define _CTHREAD_H_

#include <pthread.h>
#include "base/Base.h"

class CThread
{
public:

    CThread();
    virtual ~CThread();

    U8 startThread();

    U8 stopThread( void );

    bool isAlive( void );

public:
    pthread_t     mThreadFD;
    bool          mIsAlive;

private:
    virtual void threadHandler() = 0;
    bool setAlive( bool alive );

private:
    static void* startFunc( void* arg );
};

#endif //_CTHREAD_H_

