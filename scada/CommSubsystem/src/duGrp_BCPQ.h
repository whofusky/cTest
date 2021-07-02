//双组带条件(C)和阻塞(B)功能的优先级(P)队列头文件
//
//说明：1.由两组带优先级的队列组成，组内各队列的单元结构相同，每组拥有自己的事件/信号。
//      2.每次数据入队和出队操作仅允许对一组进行，入队后发送该组拥有的事件/信号。
//      3.出队在接收到事件/信号量后进行，事件/信号量使用手工复位机制，仅在出队后该组所有
//        队列均空时才会复位。
//      4.组内队列有数据，但均不满足出队条件，事件/信号量不复位，为防止频率空循执行出队操
//        作返回前会sleep 280ms(宏定义)，在此期间不会影响入队操作。
//		5.出队操作有两种方式：选择组等待出队，事件引导出队。后者两组中第1组事件最先响应，
//		  无第1组事件时响应第2组事件，即事件引导出队时第1组优先级最高。两组事件同时到达时，
//        未响应的事件不丢失。
//      6.在使用中优先级可以对等为其它应用概念，只要保证它的唯一性，并以此作为出队的先后
//        顺序即可。
//
//单位：兆方美迪风能工程(上海)有限公司
//编写人：Liu ambition
//编写时间：2016年12月
//

#ifndef DUGRP_BCPQ_H
#define DUGRP_BCPQ_H

#include "sysDefine.h"

#include <stdlib.h>

#ifdef	WINDOWS_PLATFORM

#include <WinNT.h>
#include <WinBase.h>
#define T_EVT				HANDLE
#define T_EVTMEM			u_int		//window unuse
#else
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#define T_EVT				sem_t*
#define T_EVTMEM			sem_t
#define CRITICAL_SECTION pthread_mutex_t
#endif

/************************************************************/
/*				    类使用的自定义数据类型 	                */
/************************************************************/
//
#define LIG_MAX_SIZE		16
#define LGG_MAX_SIZE		1024
#define QB_MAX_VOL			20
#define QB_MAX_SIZE			100

// 等待数据和接收事件的队列组。第1组=0x1；第2组=0x2；两组中的任意一个=0x3
#define FST_GROUP	0x1
#define SCD_GROUP	0x2
#define BOTH_GROUP  0x3

//队头类型
typedef struct
{
	u_char	usedBlkNum;			//used q-unit number
	u_char	pshQIndx;			//push into null-pos of Q  , from 0 ...
	u_char	popQIndx;			//pop off full-pos of Q , from 0 ...
	u_char*	qCnxt_p;			//affacted pop-q state contxt ptr
	char*	qBody_p;			//q-body ptr
} qHd;
//
//组内队列非空状态类型
typedef union		
{
	//最多支持16个优先级的队列组，实际队列数由组描述的grpQNum确定(<=16)
	u_short	ltGrpQStMk;			//小组
	
	//最多支持1024个优先级的队列组，实际队列数由组描述的grpQNum确定(<=1024)
	u_short lgGrpQStMk[64];		//大组
}grpQStMk;
//
//队列组类型
typedef struct
{
	u_char				holdEvtFlag;		//hold=1,els=0
	u_char				grpQtype;		//type of G,=1,ltG;=2,lgG
	u_char				grpQULng;		//q-unit lng of G
	u_char				grpQVol;		//q-unit num in Q
	u_short				grpQNum;		//q-num in G
	u_short				qUsdUTCnt;		//q-unit used num total couter of G
	T_EVT				Ev_Nfy;			//envet notice of G
	CRITICAL_SECTION 	grpQKey;		//mutex of G
	grpQStMk			grpQStCd;		//all q state in G
	qHd*				grpQHd_p;		//G header ptr

	u_short				evtSnd;
	u_short				evtRev;

}grpQ;
//
//构造参数结构
typedef struct
{
	u_char				grpQtype;		//=1,ltG;=2,lgG
	u_char				grpQNum;		//q-num in G
	u_char				grpQULng;		//q-unit lng of G
	u_char				grpQVol;			//q-unit num in Q
}crtGrpQPar;
//
//
/************************************************************/
/*		     双组带条件和阻塞功能的优先级队列类             */
/************************************************************/

class C_DuGrp_BCPQ
{
private:
	u_char	qGrpNum;
	grpQ	fstGrpQ;
	grpQ 	scdGrpQ;
	T_EVT	evtMem[2];	//for Lnx, windows
	T_EVTMEM evt_Vlu[2];
public:
	C_DuGrp_BCPQ(crtGrpQPar fstGrpPar);
	C_DuGrp_BCPQ(crtGrpQPar fstGrpPar,crtGrpQPar scdGrpPar);
	~C_DuGrp_BCPQ();
public:
	int setCnxtQ(u_char grpId, u_short qIdx, void* cnxt_p);
	T_EVT *getEvtMem();
	grpQ* getFstGrpQ();
	grpQ* getScdGrpQ();
	int pshQ(u_char grpId, u_short qPrio, char *dBlk_p, u_char dBlkLng);
	int popQ(u_char grpId, char* dBlk_p, u_char* dBlkLng_p);
	int clrGQ(u_char grpId, u_short qPrio);
	int clrG(u_char grpId);
};

#endif
