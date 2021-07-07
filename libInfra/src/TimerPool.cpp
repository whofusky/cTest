/********************************************************
*TimerPool.cpp
*
*
*2018/03/16
*
*********************************************************/
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/select.h>

#include "timer/TimerPool.h"
#include "log/Log.h"

CTimerPool* CTimerPool::mInstance = NULL;


CTimerPool::CTimerPool()
{
	mIsStart = false;
}



CTimerPool::~CTimerPool()
{
	
}



bool CTimerPool::startTimerPool( void )
{
	if( mIsStart )
	{
		return true;
	}

	startThread();
	mIsStart = True;

	return true;
}

bool CTimerPool::stopTimerPool( void )
{
	if( !mIsStart )
	{
		return true;
	}

	stopThread();
	mIsStart = false;

	return true;
}


bool CTimerPool::enableTimer( int second, 
	int microsecond, 
	int repet, 
	sp<ITimerHandler> obj,
	TIMER_HANDLER func
	)
{
	CMutexGuard autoGuard( mLock );
	std::vector<TIMER_CB_NODE>::iterator it;

	for( it = mCB.begin();
		 it != mCB.end();
		 it++
		)
	{
		if( it->mCB_Obj == obj )
		{
			return true;
		}
	}

	TIMER_CB_NODE tCB_Node;
	tCB_Node.mCB_Obj = obj;
	tCB_Node.mCB_Func = func;
	tCB_Node.mInternalSec = second;
	tCB_Node.mInternalMic = microsecond;
	tCB_Node.mRepetNum = repet;
	tCB_Node.mTickNum = 0;
	tCB_Node.mValid = true;

	mCB.push_back( tCB_Node );

	return true;
}

bool CTimerPool::disableTimer( ITimerHandler* obj, TIMER_HANDLER func )
{
	CMutexGuard autoGuard( mLock );
	std::vector<TIMER_CB_NODE>::iterator it;

	for( it = mCB.begin();
		it != mCB.end();
		it++
		)
	{
		if( it->mCB_Obj == obj )
		{
			mCB.erase( it );
			return true;
		}
	}

	return true;

}

void CTimerPool::threadHandler()
{
	while( isAlive() )
	{
		struct timeval tmpVal;
		tmpVal.tv_sec = 1;
		tmpVal.tv_usec = 0;
		if( !mIsStart )
		{
			usleep(10);
			continue;
		}

		select ( 0, NULL, NULL, NULL, &tmpVal );

		CMutexGuard autoGuard( mLock );

		std::vector<TIMER_CB_NODE>::iterator it;

		for( it = mCB.begin();
			it != mCB.end();
			)
		{
			if( it->mValid == false )
			{
				continue;
			}
			
			it->mTickNum++;
			if( it->mTickNum >= it->mInternalSec )
			{
				if( it->mRepetNum < 0 )
				{
					it->mCB_Obj->onTimerTick();
					it->mTickNum = 0;
					it++;
				}
				else if( it->mRepetNum > 0 )
				{
					it->mRepetNum--;
					it->mTickNum = 0;
					it->mCB_Obj->onTimerTick();
					it++;
				}
				else
				{
					it->mValid = false;
					it = mCB.erase( it );
				}
			}
			else
			{
				it++;
			}
		}

	}

}


CTimerPool* CTimerPool::Initialize()
{
	if( mInstance == NULL )
	{
		mInstance = new CTimerPool;
	}
	return mInstance;
}


