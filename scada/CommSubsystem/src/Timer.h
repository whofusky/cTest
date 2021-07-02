//定时器头文件
//
//说明：1.支持定时通道（定时器组），内部分配定时通道号（定时通道ID），并返回给调用者，作为
//        用户级定时通道唯一标识，每组包含的定时器最大数量可定制，运行时不可修改。
//      2.所有定时器必须唯一属于特定定时通道，申请定时器必须在存在定时通道的情况下进行，按
//        在指定的定时通道内插入所申请的定时器。所有定时器采用组内唯一编号（ID）标识，由调
//		  用者根据用途或其它分类法为其编号，相同编号定时器不能申请到同一定时通道内。
//      3.定时通道和定时器的申请（添加）无线程安全保证，定时器控制和访问均提供线程安全保证。
//      4.定时器控制按状态机方式执行,一次性定时器到期触发后转为停止状态，周期性定时器触发后
//        再次进入启动状态：
//        (取消)未安装<-->安装-->启动-->暂停<-->继续
//				  |\			  |\      |     /
//				    \	 	        \     |    /
//                   \               \|  \|/ |/
//		              停                     止
//
//单位：兆方美迪风能工程(上海)有限公司
//编写人：赵金杰
//编写时间：2016年12月
//
#ifndef TIMER_H
#define TIMER_H

#include "FrameGlobal.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sysMgr.h"
/************************************************************/
/*				        平台相关部分    	                */
/************************************************************/
//

#ifdef WINDOWS_PLATFORM
#include <WinDef.h>
#include <WinBase.h>
#include <MMSystem.h>
#pragma comment(lib, "winmm.lib ")
#else
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>						//modify, by lzq 2016.12.23
#define CRITICAL_SECTION pthread_mutex_t		//modify, by lzq 2016.12.23
#endif
/************************************************************/
/*				        平台无关部分    	                */
/************************************************************/
//

#define MC_TICK_TIME			1000000		// 1S. clk src Accuracy lnx: uS; win: mS
#define MC_DEF_SOFT_TIME_MAX	5			// 定时通道定时器表个数默认值
#define MC_DEF_CHNL_MAX			1024		// 定时通道最大数
#define MC_ALLOC_CHNL_NUM		16			//可被1024整除的偶数，一次分配定时通道数，不够时追加按此数继续分配

#define MC_104_CHNL_TIME_MAX	4
#define MC_TRICK_TIME	1000
enum tmCtrlCode
{
    TM_UNSTALL	= 0x0,	//unstall state
    TM_START	= 0x1,	//start state
    TM_PAUSE	= 0x2,	//pause state
    TM_STOP		= 0x3,	//stop state

    TM_GO_ON = 0x4,		//goOn state
    TM_INSTALL = 0x5	//install state
};
class C_TmObj
{
private:

    clk			m_clkSrc;			// 时钟源结构
    tmr*		m_tmTrgrHd_p;		// 定时器驱动链表结构
    tmChnl*		m_tmChnlTblHd_p;	// 定时通道指针链表头
    u_short		m_tmChnlSlotTtlNum;	// 定时通道槽总数
    u_short		m_tmChnlSlotUsingNum;// 定时通道槽在用数
    //	u_short		m_tmChnlSlotIdleIdx;		// 首个定时通道空闲槽索引

public:
    // 软件定时器结构,目前暂不需要！
    CRITICAL_SECTION		tmKey;		// 互斥锁,modify, by lzq 2016.12.23
    unsigned int		m_clkSrcID;		// 时钟源ID
    unsigned int		m_tmrRes;		//
    unsigned int		m_accu;			// 时钟最小精度

public:
    // 创建和删除定时对象
    C_TmObj();
    ~C_TmObj();
    //	int initClk();
public:

    // 获取系统启动日历时间
#ifdef WINDOWS_PLATFORM
    int getSysStartTm(SYSTEMTIME &sysTm);
#else

#endif
    // 获取和驱动运行时间
    int getSysRunTm();
    //int drvRunTm();

    // 挂接或摘除软定时器（已启动的定时器链表，按剩余时间有小到大的顺序）
    int addTmr(u_short tmChnlID, tmCfg* tmrCfg_p);
    int delTmr(tmr* tmr_p);
    int delTmr(u_short tmChnlID, u_char tmrID);

    // 添加和删除定时通道
    int addTmChnl(u_char tmrNum, u_short* tmChnlID);
    int delTmChnlByID(u_short tmChnlID);
    int delTmChnlByPtr(tmChnl * tmChnl_p);

    // 注册和注销定时通道
    //int regLnkTmChnl(u_short tcID, u_short usIdx, FunType tmFun, u_char ucMode);

    // 启动、复位、停止和重载定时器，lnkID，通道号；stID，定时器号；ucMode=1,启动、重载定时器；ucMode=0,复位；ucMode=2,停止
    int ctlTmr(tmChnl* tmChnl_p, u_char tmrID, u_char ctlCd);

    //tick到时处理，加wrapper后用作linux和windows定时器到时处理回调函数
    void timeOut();
    int getTmChnlPara(u_short tmChnlID, u_char paraFlag);
    tmChnl* getTmChnlPtrByID(u_short tmChnlID);
    int getTmChnlIDByPtr(tmChnl* tmChnl_p);

    int getTmrPara(u_short tmChnlID, u_char tmrID, u_char paraFlag);
    char* getTmrName(u_short tmChnlID, u_char tmrID, char* name_p, int lng);
    u_char getTmrID(u_short tmChnlID, char* name_p);
    int tmrRunTm(u_short tmChnlID, u_char tmrID);

    //
    tmChnl* ctrAppTmChnl(u_short tmChnlID, tmCfg* tmrCfg_p);

    // 应用级定时器单独删除，因为并没有放到通道定时器链表中，modify by zhaojj，20170602
    // 
    int delAppTmChnl(u_short tmChnlID, tmChnl* tmChnlHdl_p);

};

#endif // !ifndef

