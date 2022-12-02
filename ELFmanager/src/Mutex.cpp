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


#include <stdio.h>
#include "Mutex.h"
#include "CustomOutLog.h"

CMutex::CMutex()
{
    pthread_mutex_init( &mMutex, NULL );    
}


CMutex::~CMutex()
{
    pthread_mutex_destroy( &mMutex );    
}

CMutex::CMutex( CMutex& obj)
{
    b_write_log(_ERROR,"CMutex(CMutex& obj) format is Error!" );    
}


CMutex& CMutex::operator = ( CMutex& obj )
{
    this->mMutex = obj.mMutex;
    return *this;  
}    

void CMutex::Lock()
{
    pthread_mutex_lock( &mMutex );
}  

void CMutex::unLock()
{
    pthread_mutex_unlock( &mMutex );
}  

CMutexGuard::CMutexGuard()
{
    b_write_log(_ERROR,"CMutexGuard() format is Error!");  
}   

CMutexGuard::CMutexGuard( CMutex& mutex )
{
    mLock = mutex; 
    mLock.Lock(); 
} 

CMutexGuard::~CMutexGuard()
{
    mLock.unLock(); 
} 
