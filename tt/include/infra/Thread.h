/***********************************************************
 *
 * @file    Thread.h
 *
 * @brief   Thread头文件
 *
 * @author  fu.sky
 *
 * @date    2021-07-04
 *
 * @version V10.010.000
 *
 ***********************************************************/

#ifndef _CTHREAD_H_
#define _CTHREAD_H_

#include <pthread.h>

class CThread
{
public:
    CThread();
    virtual ~CThread();
    
    unsigned char startThread( void );
    unsigned char stopThread( void );
    
    bool isAlive( void );

public:
    pthread_t mThreadFD;
    bool      mIsAlive;
    
private:
    virtual void threadHandler()=0;
    bool    setAlive( bool alive );
    
private:
    static void* starFunc( void* arg );            

};




#endif//_CTHREAD_H_
