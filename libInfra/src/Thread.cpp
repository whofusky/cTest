/***********************************************************
 *
 * @file    Thread.cpp
 *
 * @brief   Thread源文件
 *
 * @author  fu.sky
 *
 * @date    2021-07-04
 *
 * @version V10.010.000
 *
 ***********************************************************/


#include <stdio.h>
#include "Thread.h"
#include "WriteLog.h"
#include "Version.h"
#include "errno.h"
#include "string.h"

__attribute__((constructor)) static void my_thread_init()
{
    PrintToStdout( LOGINITINFO, 
            "%s,CompileTime:[%s %s]", 
            LIB_INFRA_VERSION, __DATE__,__TIME__);
}

CThread::CThread()
{
    mThreadFD = 0;
    mIsAlive = false;
}  

CThread::~CThread()
{
}   

unsigned char CThread::startThread()
{
    if ( pthread_create( &mThreadFD,
                         NULL,
                         starFunc,
                         ( void* )this ) != 0 )
    {
        PrintToStdout( LOGERROR, 
                "pthread_create error, errno=[%d],errMsg=[%s]!",
               errno, strerror(errno) ); 
        return 0;
    }
    
    return 1;
}    


unsigned char CThread::stopThread()
{
    if ( mIsAlive )
    {
        pthread_cancel( mThreadFD );
        pthread_join( mThreadFD, NULL ); 
    }
    
    mIsAlive = false;   

    return 1;
}    


bool  CThread::isAlive( void )
{
    return mIsAlive;
}    

bool  CThread::setAlive( bool alive )
{
    mIsAlive = alive;
    return true;
}

void* CThread::starFunc( void* arg )
{
   CThread* tThreadPtr = (CThread*) arg;
   tThreadPtr->setAlive( true );
   tThreadPtr->threadHandler();
   
   return NULL; 
}    
