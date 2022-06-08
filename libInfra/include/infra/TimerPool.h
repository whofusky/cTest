
#ifndef _C_TIMERPOOL_H_
#define _C_TIMERPOOL_H_

#include <vector>
#include "Base.h"
#include "Thread.h"
#include "smartPointer.h"
#include "Mutex.h"




class CTimerPool: public CThread
{
public:
	CTimerPool();
    virtual ~CTimerPool();
    
public:
	static CTimerPool* Initialize();

	class ITimerHandler: public RefBase
	{
	public:
		virtual ~ITimerHandler() {}
		virtual void onTimerTick(void) = 0;
	};

	typedef void ( ITimerHandler::*TIMER_HANDLER)(void);

	bool startTimerPool(void);

	bool stopTimerPool(void);

	bool enableTimer( 
            int second, 
            int microsecond, 
            int repet, 
            sp<ITimerHandler> obj,
            TIMER_HANDLER func );

	bool disableTimer( 
            ITimerHandler* obj, 
            TIMER_HANDLER func );

private:
	void threadHandler();

private:
	typedef struct timer_cb_node
	{
		sp<ITimerHandler> mCB_Obj;
		TIMER_HANDLER     mCB_Func;
		int               mInternalSec;
		int               mInternalMic;
		int               mRepetNum;
		int               mTickNum;
		bool              mValid;
	}TIMER_CB_NODE;

	std::vector<TIMER_CB_NODE> mCB;
	static CTimerPool*         mInstance;

	bool   mIsStart;
	CMutex mLock;

};





#endif//_C_TIMERPOOL_H_
