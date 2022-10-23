
/***********************************************************
 *
 * @file    Mutex.h
 *
 * @brief   Mutex头文件
 *
 * @author  
 *
 * @date    2021-07-04
 *
 * @version V10.010.000
 *
 ***********************************************************/

#ifndef _CMUTEX_H_
#define _CMUTEX_H_

#include <pthread.h>

class CMutex
{
public:
    CMutex();
    ~CMutex();
    
    void Lock( void );
    void unLock( void );
    
    CMutex& operator = ( CMutex& obj );
    
private:
    CMutex( CMutex& obj );
    pthread_mutex_t mMutex;

};

class CMutexGuard
{

public:
    CMutexGuard( CMutex& );
    ~CMutexGuard();
private:
    CMutexGuard(); 
    CMutex mLock;   
        
};



#endif//_CMUTEX_H_
