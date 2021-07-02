//��ʱ��ͷ�ļ�
//
//˵����1.֧�ֶ�ʱͨ������ʱ���飩���ڲ����䶨ʱͨ���ţ���ʱͨ��ID���������ظ������ߣ���Ϊ
//        �û�����ʱͨ��Ψһ��ʶ��ÿ������Ķ�ʱ����������ɶ��ƣ�����ʱ�����޸ġ�
//      2.���ж�ʱ������Ψһ�����ض���ʱͨ�������붨ʱ�������ڴ��ڶ�ʱͨ��������½��У���
//        ��ָ���Ķ�ʱͨ���ڲ���������Ķ�ʱ�������ж�ʱ����������Ψһ��ţ�ID����ʶ���ɵ�
//		  ���߸�����;���������෨Ϊ���ţ���ͬ��Ŷ�ʱ���������뵽ͬһ��ʱͨ���ڡ�
//      3.��ʱͨ���Ͷ�ʱ�������루��ӣ����̰߳�ȫ��֤����ʱ�����ƺͷ��ʾ��ṩ�̰߳�ȫ��֤��
//      4.��ʱ�����ư�״̬����ʽִ��,һ���Զ�ʱ�����ڴ�����תΪֹͣ״̬�������Զ�ʱ��������
//        �ٴν�������״̬��
//        (ȡ��)δ��װ<-->��װ-->����-->��ͣ<-->����
//				  |\			  |\      |     /
//				    \	 	        \     |    /
//                   \               \|  \|/ |/
//		              ͣ                     ֹ
//
//��λ���׷����Ϸ��ܹ���(�Ϻ�)���޹�˾
//��д�ˣ��Խ��
//��дʱ�䣺2016��12��
//
#ifndef TIMER_H
#define TIMER_H

#include "FrameGlobal.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sysMgr.h"
/************************************************************/
/*				        ƽ̨��ز���    	                */
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
/*				        ƽ̨�޹ز���    	                */
/************************************************************/
//

#define MC_TICK_TIME			1000000		// 1S. clk src Accuracy lnx: uS; win: mS
#define MC_DEF_SOFT_TIME_MAX	5			// ��ʱͨ����ʱ�������Ĭ��ֵ
#define MC_DEF_CHNL_MAX			1024		// ��ʱͨ�������
#define MC_ALLOC_CHNL_NUM		16			//�ɱ�1024������ż����һ�η��䶨ʱͨ����������ʱ׷�Ӱ�������������

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

    clk			m_clkSrc;			// ʱ��Դ�ṹ
    tmr*		m_tmTrgrHd_p;		// ��ʱ����������ṹ
    tmChnl*		m_tmChnlTblHd_p;	// ��ʱͨ��ָ������ͷ
    u_short		m_tmChnlSlotTtlNum;	// ��ʱͨ��������
    u_short		m_tmChnlSlotUsingNum;// ��ʱͨ����������
    //	u_short		m_tmChnlSlotIdleIdx;		// �׸���ʱͨ�����в�����

public:
    // �����ʱ���ṹ,Ŀǰ�ݲ���Ҫ��
    CRITICAL_SECTION		tmKey;		// ������,modify, by lzq 2016.12.23
    unsigned int		m_clkSrcID;		// ʱ��ԴID
    unsigned int		m_tmrRes;		//
    unsigned int		m_accu;			// ʱ����С����

public:
    // ������ɾ����ʱ����
    C_TmObj();
    ~C_TmObj();
    //	int initClk();
public:

    // ��ȡϵͳ��������ʱ��
#ifdef WINDOWS_PLATFORM
    int getSysStartTm(SYSTEMTIME &sysTm);
#else

#endif
    // ��ȡ����������ʱ��
    int getSysRunTm();
    //int drvRunTm();

    // �ҽӻ�ժ����ʱ�����������Ķ�ʱ��������ʣ��ʱ����С�����˳��
    int addTmr(u_short tmChnlID, tmCfg* tmrCfg_p);
    int delTmr(tmr* tmr_p);
    int delTmr(u_short tmChnlID, u_char tmrID);

    // ��Ӻ�ɾ����ʱͨ��
    int addTmChnl(u_char tmrNum, u_short* tmChnlID);
    int delTmChnlByID(u_short tmChnlID);
    int delTmChnlByPtr(tmChnl * tmChnl_p);

    // ע���ע����ʱͨ��
    //int regLnkTmChnl(u_short tcID, u_short usIdx, FunType tmFun, u_char ucMode);

    // ��������λ��ֹͣ�����ض�ʱ����lnkID��ͨ���ţ�stID����ʱ���ţ�ucMode=1,���������ض�ʱ����ucMode=0,��λ��ucMode=2,ֹͣ
    int ctlTmr(tmChnl* tmChnl_p, u_char tmrID, u_char ctlCd);

    //tick��ʱ������wrapper������linux��windows��ʱ����ʱ����ص�����
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

    // Ӧ�ü���ʱ������ɾ������Ϊ��û�зŵ�ͨ����ʱ�������У�modify by zhaojj��20170602
    // 
    int delAppTmChnl(u_short tmChnlID, tmChnl* tmChnlHdl_p);

};

#endif // !ifndef

