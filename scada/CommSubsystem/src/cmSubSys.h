//�ֳ������ۺ�ͨ��ϵͳ--ͨ����ϵͳ����ͷ�ļ�
//������ϵͳ����ȫ�ֹ�������
//��˾���׷����Ϸ��ܹ���(�Ϻ�)���޹�˾
//��д�ˣ��Խ��
//��дʱ�䣺2016��12��

#ifndef CM_SUBSYS_H
#define CM_SUBSYS_H

#include "sysMgr.h"
#include "duGrp_BCPQ.h"
#include "FrameGlobal.h"
#include "DataBase.h"
#include "TransCtrlLayer.h"
#include "data.h"
#include "cfgManage.h"
#include "unitInfMem.h"
#include "countManage.h"
#include"common_comm.h"
#ifdef WINDOWS_PLATFORM //��WINDOWSƽ̨
#else
    #include<stddef.h>
    #include <pthread.h>
    #include <sys/syscall.h>
    #include <unistd.h>
    #include <stdlib.h>
#endif
//
// extern C_DuGrp_BCPQ  m_AsduSndQu;        // ��ASDU���ͺʹ�����������˫���У�����������У�
// extern C_DuGrp_BCPQ  m_AsduInfoQu;       // ASDU ��Ϣ���У�����������У�
//static snnTbl           m_snnTbl;           // �Ự��
//static nsCPLock::CLock  m_Lock;             //

#define SAVE_BUFF 0
#define LOAD_BUFF 1
typedef struct
{
    u_short  addrSign;//�Ĵ����ۼӴ�������
    u_short*  addrStart_p;//�Ĵ�����ʼ��ַ
    u_short  addrEnd; //�Ĵ���������ַ
    u_short*  funCode_p;//������
    u_short  accumNum;   //�ۼӴ���
    u_short*  accumValue_p;//���ݹ���������ʾ������仯������Ĵ�������ֶα�ʾ�Ĵ�������������Ƕ���Ȧ�����ʾbit��
    u_short  addrPos;//�Ĵ�����ǰλ�� ,��ֵ��ʼ��ʱӦ��Ĵ�����ʼ��ַһ��
} ModbusChnlAddrSign;

typedef struct
{
    u_short      funCode;//������
    u_short dataLng;       //����������
    u_short dataIdLng;       //����������
    T_snnUsrData* data_p;  //�˹���������ݻ�����
    DATA_ID* data_Id_p;//����ID��

} UsrData;
typedef struct
{
    u_short      funCodeNum; // �ֳ�MODBUSͨ�ŵ�ͨ����֧�ֵĹ��������
    UsrData* usrdata_p;
    DATAFORMAT_DATE7B1 dataTime;
    int chnlID; // ͨ����
    ModbusChnlAddrSign MCAS;
} BufData; // BufData chl0,chl1;
//

typedef struct
{
    nsCPLock::CLock    datalock;
    u_short            arLen;           // ADDR��������,addrIdx_pΪ����,�˱���Ϊ����ĳ���
    addrRange*         addrIdx_p;
    T_snnUsrData**     buf_pp;
    u_int              tmStmpLng;  //ʱ�곤��
} chnlDataBuf;

typedef struct
{
    u_char  chnlNo;
    u_char  ssnNo;
    u_short pNum;//��ַ�ĸ���
    T_snnUsrData* usrdataP[248];
} dataMsgP;
int getSnnParaMem ( T_snnPara*& snnPara_p );

//
int finishBlock ( ssnCtlBlk* ssnCtlBlk_p, T_TID trdID, u_short ssnID, u_short chnlNo, u_int errCd );

// �Ự����ӿ�
int reqSnn ( reqInfo* reqInfo_p, ssnReqCnt ssn, T_snnPara* snnStartPar_p, u_short timeout, PMSG& msg_p );
//u_char reqSnn(reqInfo* reqInfo_p, ssnDsc ssnID, snnInfo* snnInfo_p, u_short timeout);

// ��Լ�����Ŀռ��ͷŵ��������ռ�����
int resetPsvSsn ( ssnCtlBlk* ssnCtlBlk_p, u_short usSsnNo, u_short usChnlNo );

// �����Ự�����ɻỰʵ�����ƿ飬��Ӧ�ռ����뼰��Լ����Ӧ�����ʼ��������ӵ��Ự������
int crtSsn ( C_SsnObj*& tCurSsnObj_p, qSsnReqUint& tSsnReqBlk, C_ChnlObj*& tChnlObj_p,
             HMODULE*& tModule_p, ssnCtlBlk*& tSsnCtlBlk_p, u_int& tErrFlag, u_int actFlag = MC_ACT_FLAG );
//
//
void sysInit();//ģ��������ϵͳ
//
//static int finishBlock(u_short ssnNo,u_short chnlNo,u_char sucFlag);
//
void Et_sys_AppSnd();
//
void Et_sys_AppRec();

// // ϵͳ�߳�
// static void Tsys_inr(void* pParam);
//
//�û��߳�
void Tsys_Usr ( void* pParam ); //??????�û��߳������⣬������Ҫ�������÷�����ʹ֮��Ϊ����
//
void Test_NR ( void* pParam );

void Tsys_DtPross ( void* pParam );
//
void Et_sys_TakeErr ( void* pParam );

void Et_sys_DataPutStor ( void* pParam );

void Et_sys_DataOutStor ( void* pParam );

T_snnUsrData* findData ( u_int addr, chnlDataBuf*  datBuff );

//һ��д��������snnUsrData_p����������
int writeChnMPntObj ( const T_snnUsrData* snnUsrData_p,
                      const u_int& num,
                      chnlDataBuf*  datBuff_p
                    );
/*
    ����:��һ֡���ݵĵ�ֵת����������ֵ���빲���ڴ浥Ԫ
    ����:ssnPara_p һ֡���ݶ�Ӧ�Ľṹ
        :chnId    ͨ��id
    ���:unitInfData_p �����ڴ����
    ����:0   �ɹ�
         ��0 ʧ��
*/
int writeFrmPntToUMem ( const T_snnPara* ssnPara_p,   /*һ֡���ݶ�Ӧ�Ľṹ*/
                        const u_int& chnId,            /*ͨ����*/
                        C_unitInfData* unitInfData_p  /*�����ڴ����*/
                      );
int convPhyToSndChn ( C_unitInfData* unitInfData_p,  /*�����ڴ����*/
                      const u_int& getChnNum,        /*Ҫȡֵ��ͨ����*/
                      const u_int& writeChnNum,      /*Ҫд���ͨ����*/
                      const PHY_UNIT_TYPE* phyType_p,/*Ҫ������������phyType_p[]*/
                      const u_int& phyNum            /*Ҫ����������������*/
                    );
int setData(  T_snnUsrData* ssnPara_p, short stnID, u_char ucType);
int optData ( T_snnPara* ssnPara_p, short stnID, u_char ucType, u_char optType, u_char ssnNo );

int setSsnCfg ( T_snnCfg* ssnStnCnfg_p, u_short  addrStart, u_short  accumValue, int iPtclID );

/*
 *��ȡϵͳ��ǰʱ����ǰseconds��ʱ��
 * �����nowTime = ��ǰʱ��
 * �����beforeTime = ǰһʱ��ʱ��
 * ���룺nowTime - beforeTime ��ƫ���λ�룩
 */
 int getNAndBTime(DATEFORMAT_T& nowTime , DATEFORMAT_T& beforeTime ,int seconds);

/*
 *��ȡϵͳ��ǰʱ��
 */
int getSysTime ( DATAFORMAT_DATE7B1& sysTime , time_t& tTime);

#ifndef WINDOWS_PLATFORM
    /*******************************************************************************
    *���ܣ�����ת��
    * ���룺
    *      tChnlObj_p   �� ͨ������
    *      funDsc_p     ���Ự����������
    *      ssnPar_p     ���Ự��������Ϣ�������ò��ڵ���������Ϣ����
    *
    ********************************************************************************/
    int DataForward ( C_ChnlObj* tChnlObj_p, T_ptclFunDsc* funDsc_p, T_snnPara* ssnPar_p );

    //�Իػ�����������ã�ģ������ϵͳ
    int getDateID ( DATA_ID* dataId, int chnl, int funcId, int time );
#endif


//ϵͳ�˳�����Ӧ��Դ���л��մ���
    void sysClear();

//
// // Ӧ�÷����߳�
// static void Tsys_AppSnd(void* pParam);
//
// // ������ƴ����߳�
// static void Tsys_NetSnd(void* pParam);
//
// // Ӧ�ý����߳�
// static void Tsys_AppRec(void* pParam);
//
// // ������ƽ����߳���
// static void Tsys_NetRec(void* pParam);
//

#endif // ! CM_SUBSYS_H
