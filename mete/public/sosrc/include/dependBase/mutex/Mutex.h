/***********************************************************
*
*
*  Mutex.h
*
*  Create by muxd
*
*  2017/07/12
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
    CMutex& operator = ( const CMutex& obj );

private:
    CMutex(CMutex& oj);

private:
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

#endif //_CMUTEX_H_

