
#ifndef _C_TIMER_H_
#define _C_TIMER_H_

#include "Base.h"
#include "Thread.h"




class CTimer: public CThread
{
public:
	CTimer();
    CTimer( bool autoRelease );
    virtual ~CTimer();
    
    bool setTimer( int second, int microsecond, int repet );

	bool startTimer(void);

	bool stopTime(void);

	bool isStart(void);

private:
	void threadHandler();
	virtual void timerHandler() = 0;

private:
	bool mIsStart;
	int mSecond;
	int mMicroSecond;
	int mRepet;
	bool mAutoRelease;

};





#endif//_C_TIMER_H_
