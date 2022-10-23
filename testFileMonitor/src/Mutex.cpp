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
//#include "WriteLog.h"

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
    //PrintToStdout( LOGERROR, 
    printf("%s:%s:%d:ERROR:[CMutex(CMutex& obj) format is Error!]\n", 
            __FILE__,__FUNCTION__,__LINE__ );    
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
    //PrintToStdout( LOGERROR, 
    printf("%s:%s:%d:ERROR:[CMutexGuard() format is Error!]\n",
         __FILE__,__FUNCTION__,__LINE__ );  
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
