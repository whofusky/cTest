

#ifndef SYSMGR_H
#define SYSMGR_H

#include "FrameGlobal.h"
#include <stdlib.h>
#include <string.h>

/************************************************************/
/*				         ƽ̨��ز���      	                */
/************************************************************/
//
#ifdef WINDOWS_PLATFORM
#include <windows.h>
#else
#include <pthread.h>
#define CRITICAL_SECTION pthread_mutex_t		//modify, by lzq 2016.12.23
//#define HMODULE (void*) 
#endif


/************************************************************/
/*				         ƽ̨�޹ز���      	                */
/************************************************************/
//

///////////////////////////////ssn manage
////////
#define MC_DEF_SSNTBLITEM_MAX		200
#define MC_ALLOC_SSNTBLITEM_NUM		50


class C_SsnMgr
{
private:
    //�Ự��ṹ
    u_short		ssnTblItemTtlNum;			// �Ự��������
    u_short		ssnTblItemUsingNum;			//�Ự����������
    snnTblItem*	snnTblHd_p;					//�Ự��ͷָ��
    CRITICAL_SECTION ssnTblkey;

public:
    C_SsnMgr();
    ~C_SsnMgr();

public:
    int aplSsnItem(snnAppCfg cfgPara, HMODULE fHdl);//return ssnID
    int gvBkSsnItem(u_short ssnID);//give back ssn item
    u_short getSsnID(ptclSnnKey snnKey);//using & ptcl+snnDscNo equ.
    C_SsnObj* getSsnObj(u_short ssnID);
    int regSsnObj(u_short ssnID, C_SsnObj* ssnObj_p);
    HMODULE getSsnFileHdl(u_short ssnID);
    int	getSsnPtclNo(u_short ssnID);
    int	getSsnDscNo(u_short ssnID);
    int	getSsnPrio(u_short ssnID);
};



///////////////////////////////channel manage
////////
#define MC_DEF_CHNLTBLITEM_MAX		1024
#define MC_ALLOC_CHNLTBLITEM_NUM	50
#define MC_CHNLST_MASK_REG		0x01//bit 0
#define MC_CHNLST_MASK_RDY		0x02//bit 1
#define MC_CHNLST_MASK_CNT		0x04//bit 2
#define MC_CHNLST_MASK_VALD		0x08//bit 3
#define MC_CHNLST_MASK_STARTUP	0x10//bit 4
#define MC_CHNLST_MASK_CHK		0x20//bit 5
#define MC_CHNLST_MASK_FAULT	0x40//bit 6
#define MC_CHNLST_MASK_DUNKW	0x80//bit 7

class C_ChnlMgr
{
private:
    u_short		chnlNum;				// ͨ�����飩����
    u_short		chnlTblItemTtlNum;		// ͨ�����飩��������
    u_short		chnlTblItemUsingNum;	//ͨ�����飩����������
    chnlTblItem*	chnlTblHd_p;		//ͨ�����飩��ͷָ��
    CRITICAL_SECTION chnlTblkey;

public:
    C_ChnlMgr();
    ~C_ChnlMgr();

public:
    int aplChnlItem();//return channel ID
    int gvBkChnlItem(u_short chnlID);//give back channel item

    int regChnlObj(u_short chnlID, C_ChnlObj* chnlObj_p);
    C_ChnlObj* getChnlObj(u_short chnlID);
    int regStnOrVew(u_short chnlID, stnOrVew* stnOrVew_p);
    stnOrVew* getStnOrVew(u_short chnlID);

    int setChnlState(u_short chnlID, u_short stateMsk);
    int clrChnlState(u_short chnlID, u_short stateMsk);
    int getChnlState(u_short chnlID);
};



#endif







