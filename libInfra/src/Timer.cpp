/********************************************************
*Timer.cpp
*
*
*2018/03/16
*
*********************************************************/
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/select.h>

#include "Timer.h"
//#include "WriteLog.h"

CTimer::CTimer()
{
    mIsStart     = false;
	mSecond      = -1;
	mMicroSecond = -1;
	mRepet       = -1;
	mAutoRelease = false;
}

CTimer::CTimer( bool autoRelease)
{
    mIsStart     = false;
	mSecond      = -1;
	mMicroSecond = -1;
	mRepet       = -1;
	mAutoRelease = autoRelease;
}

CTimer::~CTimer()
{
	if( mIsStart )
	{
		stopThread();
	}
}



bool CTimer::setTimer( int second, int microsecond, int repet )
{
	mSecond      = second;
	mMicroSecond = microsecond;
	mRepet       = repet;

	return true;
}

bool CTimer::startTimer(void)
{
	if( mMicroSecond < 0 && mSecond < 0 )
	{
		return false;
	}
	if( mAutoRelease && mRepet < 0 )
	{
		return false;
	}
	startThread();
	mIsStart = true;

	return true;
}

bool CTimer::stopTime(void)
{
	stopThread();
	mIsStart = false;
	return true;
}

bool CTimer::isStart(void)
{
	return mIsStart;
}


void CTimer::threadHandler()
{
	while( isAlive() )
	{
		struct timeval tmpVal;
		tmpVal.tv_sec = mSecond;
		tmpVal.tv_usec = mMicroSecond;
		if( !mIsStart )
		{
			//usleep(10);
			PauseThreadSleep(0, 10);
			continue;
		}
		
		select ( 0, NULL, NULL, NULL, &tmpVal );
		if( mRepet < 0 )
		{
			timerHandler();
		}
		else if( mRepet > 0 )
		{
			mRepet--;
			timerHandler();
		}
		else
		{
			break;
		}
	}

	delete this;

}
