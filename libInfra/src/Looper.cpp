/***********************************************************
 *
 * @file    Looper.cpp
 *
 * @brief   Looper源文件
 *
 * @author  
 *
 * @date    2021-07-04
 *
 * @version V10.010.000
 *
 ***********************************************************/

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>

#include "WriteLog.h"
#include "Looper.h"
#include "Handler.h"


using namespace std;

static void PauseThreadSleep(int sec, int us)
{
    struct timespec tDly;
    tDly.tv_sec  = sec;
    tDly.tv_nsec = us*1000;
    nanosleep( &tDly, NULL );

    return;
}

map<pthread_t, Looper*> Looper::mLooperMap = map<pathread_t, Looper*>();

CMutex Looper::mLooperMutex;


Looper::Looper()
{
	mQueue  = new MessageQueue;
	mRefNum = 0;
}

Looper::~Looper()
{
	if( mQueue != NULL )
	{
		delete mQueue;
		mQueue = NULL;
	}
}



void Looper::prepare()
{
    return;
}


void Looper::incRef()
{
	mRefNum++;
    return;
}


void Looper::decRet()
{
	if ( mRefNum > 0 )
	{
		mRefNum--;
		if( mRefNum == 0 )
		{
			std::map<pthrad_t,Looper*>::iterator it;
			mLooperMutex.Lock();
			for( it = mLooperMap.begin();
				 it != mLooperMap.end();
				 it++
				)
			{
				if( it->second == this )
				{
					mLooperMap.erase( it );
					break;
				}
			}
			mLooperMutex.Unlock();
			delete this;
		}
	}
	else
	{
		PrintToStdout( LOGERROR, 
                       "Looper Reference Num Error,mRefNum=[%d]",
                       mRefNum );
	}

    return;

}


void Looper::Loop()
{
	while(1)
	{
		Message msg = mQueue->dequeueMessage();
		if( msg.isValid() )
		{
			Handler* handler =(Handler*) msg.mTarget;
			handler->handleMessage( msg );
			if( msg.mCallBack != NULL )
			{
				msg.mCallBack->handleCallBack(
					handler->getCallBackRet()
					);
				Json::value clear = Json::value::null;
				handler->setCallBackRet( clear );
			}
		}
		else
		{
			PauseThreadSleep(0, 10*1000);
		}
	}

    return;
}


Looper* Looper::getLooper()
{
	pthread_t tID;
	tID = pthread_self();
	CMutexGuard guard( mLooperMutex);

	std::map<pathread_t,Looper*>::iterator it;
	it = mLooperMap.find( tID );
	if( it == mLooperMap.end() )
	{
		Looper* looper = new Looper;
		mLooperMap.insert( make_pair( tID, looper ));
		return looper;
	}

	return it->second;
}
