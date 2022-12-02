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
#include <sys/syscall.h>
#include <sys/select.h>
#include "CustomOutLog.h"

#include "Timer.h"

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
    //b_write_log(_DEBUG,
    //"fusktest:mMicroSecond[%d],mSecond[%d],mAutoRelease[%d],mRepet[%d]",
    //        mMicroSecond,mSecond,mAutoRelease,mRepet);

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
    //b_write_log(_DEBUG,"fusktest:mIsStart[%d]",mIsStart);

	return true;
}

bool CTimer::stopTime(void)
{
	stopThread();
	mIsStart = false;
    //b_write_log(_DEBUG,"fusktest: stopTime");
	return true;
}

bool CTimer::isStart(void)
{
	return mIsStart;
}


void CTimer::threadHandler()
{
    pid_t tid;
    tid = syscall(SYS_gettid);
    b_write_log(_INFO,"thread id[%d], isAlive[%d],mIsStart[%d]", tid, isAlive(),mIsStart );

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
