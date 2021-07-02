//˫�������(C)������(B)���ܵ����ȼ�(P)����ͷ�ļ�
//
//˵����1.����������ȼ��Ķ�����ɣ����ڸ����еĵ�Ԫ�ṹ��ͬ��ÿ��ӵ���Լ����¼�/�źš�
//      2.ÿ��������Ӻͳ��Ӳ����������һ����У���Ӻ��͸���ӵ�е��¼�/�źš�
//      3.�����ڽ��յ��¼�/�ź�������У��¼�/�ź���ʹ���ֹ���λ���ƣ����ڳ��Ӻ��������
//        ���о���ʱ�ŻḴλ��
//      4.���ڶ��������ݣ���������������������¼�/�ź�������λ��Ϊ��ֹƵ�ʿ�ѭִ�г��Ӳ�
//        ������ǰ��sleep 280ms(�궨��)���ڴ��ڼ䲻��Ӱ����Ӳ�����
//		5.���Ӳ��������ַ�ʽ��ѡ����ȴ����ӣ��¼��������ӡ����������е�1���¼�������Ӧ��
//		  �޵�1���¼�ʱ��Ӧ��2���¼������¼���������ʱ��1�����ȼ���ߡ������¼�ͬʱ����ʱ��
//        δ��Ӧ���¼�����ʧ��
//      6.��ʹ�������ȼ����ԶԵ�Ϊ����Ӧ�ø��ֻҪ��֤����Ψһ�ԣ����Դ���Ϊ���ӵ��Ⱥ�
//        ˳�򼴿ɡ�
//
//��λ���׷����Ϸ��ܹ���(�Ϻ�)���޹�˾
//��д�ˣ�Liu ambition
//��дʱ�䣺2016��12��
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
/*				    ��ʹ�õ��Զ����������� 	                */
/************************************************************/
//
#define LIG_MAX_SIZE		16
#define LGG_MAX_SIZE		1024
#define QB_MAX_VOL			20
#define QB_MAX_SIZE			100

// �ȴ����ݺͽ����¼��Ķ����顣��1��=0x1����2��=0x2�������е�����һ��=0x3
#define FST_GROUP	0x1
#define SCD_GROUP	0x2
#define BOTH_GROUP  0x3

//��ͷ����
typedef struct
{
	u_char	usedBlkNum;			//used q-unit number
	u_char	pshQIndx;			//push into null-pos of Q  , from 0 ...
	u_char	popQIndx;			//pop off full-pos of Q , from 0 ...
	u_char*	qCnxt_p;			//affacted pop-q state contxt ptr
	char*	qBody_p;			//q-body ptr
} qHd;
//
//���ڶ��зǿ�״̬����
typedef union		
{
	//���֧��16�����ȼ��Ķ����飬ʵ�ʶ���������������grpQNumȷ��(<=16)
	u_short	ltGrpQStMk;			//С��
	
	//���֧��1024�����ȼ��Ķ����飬ʵ�ʶ���������������grpQNumȷ��(<=1024)
	u_short lgGrpQStMk[64];		//����
}grpQStMk;
//
//����������
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
//��������ṹ
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
/*		     ˫����������������ܵ����ȼ�������             */
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
