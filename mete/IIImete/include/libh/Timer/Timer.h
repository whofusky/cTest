/***********************************************************
*
*
*  Timer.h
*
*  Create by muxd
*
*  2017/07/08
**********************************************************/
#ifndef _C_TIMER_H_
#define _C_TIMER_H_

#include "base/Base.h"
#include "thread/Thread.h"

class CTimer: public CThread
{
public:
    CTimer();
    CTimer( bool autoRelease );
    virtual ~CTimer();

    bool setTimer( int second, int microsecond, int repet );

    bool startTimer( void );

    bool stopTimer( void );

    bool isStart();

private:
    void threadHandler();
    virtual void timerHandler() = 0;

private:
    bool mIsStart;
    int  mSecond;
    int  mMicroSecond;
    int  mRepet;
    bool mAutoRelease;
};

#endif //_C_TIMER_H_

