
/*
˵����
    ��ͷ�ļ�����һЩ�ڲ����õĽӿں���
    ��ͷ�ļ���protocol104.h�з������@20180207111656
*/

#pragma once

#ifndef INTERNALFUNCTION_H
#define INTERNALFUNCTION_H

#include"APDU104.h"
#include <string.h>
#include<stdio.h>




//����TID��Ϣ����
typedef struct
{
    u_short tidQdsLng: 4;  /*ֵ+qds�ĳ���*/
    u_short tidBitLng: 6;  /*ֵ����:��λΪλ�������ֽ�*/
    u_short qdsLng: 1;     /*qds����*/
    u_short tmStmpLng: 5; /*ʱ�곤��*/
} T_TIDVALLNG;

#define MC_MAXTID_IDS   141    /*Э���ﶨ���tid����*/

/*Э���Ӧ�̶���tid������Ϣ��
(���ñ������ķ�ʽ����g_TID104�±�Ϊtid��ֵ)
*/
extern const T_TIDVALLNG  g_TID104[];



//tid��س��Ƚṹ
typedef struct
{
    T_TIDVALLNG cfgLng;    //tid���õĳ���
    u_char      tidValLng; //tidֵ�ĳ���(���ֽ�Ϊ��λ,����һ���ֽڵ���һ���ֽ�)
    /*ֵ��QDS�Ƿ������һ���:
    ��QDS������ռ�ֽ�(�磺��Ʒ�������ʵĵ�����Ϣ)
      1:������һ��
      0:û����
    */
    u_char      qdsMixFlag;
} T_tidObjLng;



/********************************�ڲ�ʹ�ú����ǽӿ�*****************************/
/*******************************************************************************
  *���ܣ�������ONE_STEPĩβ����һ���ڵ�
  * ���룺
  *      one_step_p ������ͷָ��
  *      tail_step_p: Ҫ��ӵĽڵ�ָ��
  *�����
  *      one_step_p ����ӽڵ�ָ��֮�������ͷָ��
  *���أ�
  *      void
  *�޸���ʷ��
  *      20170221092324 �޸�ԭ�����߼�����
  *      20170314084211 ���step_pΪnull�����
*******************************************************************************/
void addNode ( ONE_STEP* step_p, ONE_STEP* tail_step_p );


/*******************************************************************************
  *���ܣ�
  *      ����startStep_p�в���ΪtargetStep_p->stepNo�Ĳ�
  *���룺
  *      startStep_p ������
  *      targetStep_p��Ҫ���ҵĲ�
  *�����
  *      ��
  *���أ�
  *      ONE_STEP*
  *              1���쳣������� NULL
  *              2�������������startStep_p�в���ΪtargetStep_p->stepNo�Ĳ�;
  *�޸���ʷ
  *
*******************************************************************************/
ONE_STEP* displayNextNode ( ONE_STEP* startStep_p,
                            const ONE_STEP* targetStep_p
                          );


/*******************************************************************************
  *���ܣ���װASDU
  * ���룺
  *      apdu_p      ��APDU104ָ��
  *      tid_p       : ���ͱ�ʶ
  *      vsq_p       : �ɱ�ṹ���޶���
  *      cot_p       : ASDU_COT1_VAL���ʹ���ԭ��
  *      cot2_p      : ASDU_COT2_VAL���ʹ���ԭ��
  *      addr_p      : ASDU������ַ
  *      bodyAddr_p  : ��Ϣ���ַ
  *      bodySet_p   : ��ϢԪ�ؼ�
  *      bodySetLng  : ��ϢԪ�ؼ�����
  *      tsp_p       : ��Ϣ��ʱ��
  *      bodyObjNum  : ��Ϣ�������
  *�����
  *     apdu_p       : ��ֵ���APDU104
  *���أ�
  *    =0 �ӿ�ִ�гɹ�
  *    <0 �ӿ�ִ���쳣
  *�޸���ʷ
  *
*******************************************************************************/
int packASDU ( APDU104* apdu_p,
               ASDU_TID_VAL* tid_p,
               ASDU_VSQ_VAL* vsq_p,
               ASDU_COT1_VAL* cot_p,
               ASDU_COT2_VAL* cot2_p,
               ASDU_ADDR_VAL* addr_p,
               u_char* bodyAddr_p,
               u_char* bodySet_p,
               u_short bodySetLng,
               void* tsp_p,
               u_char bodyObjNum = 1
             );


/*******************************************************************************
  *���ܣ�
  *     ��װONE_STEP�г�nextStep_p֮���ֵ
  * ���룺
  *      step_p         ��ONE_STEP�ռ�ָ��
  *      stepNo         : Ҫ��װ��ֵ ����
  *      reqPDU_FCode   : Ҫ��װ��ֵ ����֡ʹ�õ�֡��ʽ����
  *      reqPDU_Num     : Ҫ��װ��ֵ ����֡��PDU������
  *      reqAPDU_Obj_p  : Ҫ��װ��ֵ ����֡APDU����ָ���
  *      respPDU_FCode  : Ҫ��װ��ֵ ��Ӧ֡ʹ�õ�֡��ʽ����
  *      respPDU_Num    : Ҫ��װ��ֵ ��ӦPDU����
  *      respAPDU_Obj_p : Ҫ��װ��ֵ ��Ӧ֡APDU����ָ���
  *      sVerifyBlock_p : Ҫ��װ��ֵ ר�ò���֤��
  *�����
  *      step_p         : ��ֵ���ONE_STEPָ��
  *���أ�
  *      void
  *�޸���ʷ
  *
*******************************************************************************/
void setONE_STEP ( ONE_STEP* step_p,
                   u_char stepNo,
                   u_char reqPDU_FCode,
                   u_char reqPDU_Num,
                   u_char* reqAPDU_Obj_p,
                   u_char respPDU_FCode,
                   u_char respPDU_Num,
                   u_char* respAPDU_Obj_p,
                   vrfItem* sVerifyBlock_p
                 );


/******************************************************************************
*���ܣ�
*    �Ự����֡�������պ��T_snnPara��ֵ
* ���룺
*       stepRl_p  �����պ�ĵ�ǰ��
*       funDsc_p  ���Ự����������
*       apdu104_p ��APDU104����ָ��
*     stnAcsFlag���洢����ȡ��ʶ
*�����
*       ssnPar_p��  ���������˵��
*���أ�
*     0 �ɹ�
*    <0 ʧ��
*�޸���ʷ
*
*******************************************************************************/
int asgmtSnnPar ( const ONE_STEP* stepRl_p,
                  const T_ptclFunDsc* funDsc_p,
                  APDU104* apdu104_p,
                  T_stnAcsFlag* stnAcsFlag,
                  T_snnPara* ssnPar_p
                );


/*******************************************************************************
  *���ܣ�
  *    ר�ò���֤��У��
  *
  * ���룺
  *     iVerifyBlock_p  ��ר�ò���֤��
  *     apdu104_p       ��APDU104����
  *�����
  *     iVerifyBlock_p->chkRslt_p������״̬��//λ0 = 1 ��ʾ�����ֵ���ճɹ���
                                   //λ 1 = 1ʱ����Сֵ���ճɹ�
  *���أ�
  *     0  �ɹ�
  *     -5  �޶�����ʵ��Ԫ�ظ�������
  *     -21 ���ֵ����ʧ��
  *     -22 ��Сֵ����ʧ��
  *     -23 �������С����ֵ����
  *�޸���ʷ
  *
*******************************************************************************/
int chkVerifyBlock ( const vrfItem* iVerifyBlock_p,
                     APDU104* apdu104_p
                   );


/*******************************************************************************
*���ܣ�
*   ��������һ֡��ֵ
*
* ���룺
*       nxtFrmRlStep_p����һ֡��Ӧ�Ĺ���
*       nxtFrmStep_p  ����ǰ��
*       nxtFrmStepNo_p: ��һ֡�Ĳ���
*       funDsc_p      ���Ự����������
*       ssnPar_p      �����������˵��
*�����
*       nxtFrmStep_p  ����һ֡��Ӧ�Ĳ�
*���أ�
*     0 �ɹ�
*    -1 ������ģʽ����
*    -2 ��֧�ֿɱ��޶�ֵSQ=0�����
*    ������ֵΪ�²㺯������
*�޸���ʷ
*
*******************************************************************************/
int asgmtNxtFrmPar ( ONE_STEP* nxtFrmRlStep_p,
                     ONE_STEP* nxtFrmStep_p,
                     u_char* nxtFrmStepNo_p,
                     T_ptclFunDsc* funDsc_p,
                     T_snnPara* ssnPar_p
                   );


/*******************************************************************************
 *���ܣ������һ֡�Ự״̬,�������һ֡����
 * ���룺
 *      Fun_p        ��������
 *      stepRl_p     ����ǰ֡��Ӧ�Ĳ�
 *      funDsc_p     ���Ự����������
 *�����
 *      nxtStepStat_p����һ֡�Ự״̬
 *      nxtStepNo_p  ����һ֡����
 *���أ�
 *      >0 �Ự״̬
 *      <0 �ӿ��쳣
 *�޸���ʷ
 *
********************************************************************************/
int getNxtFrmSnnStatus ( FUN_RL* Fun_p,
                         ONE_STEP* stepRl_p,
                         T_ptclFunDsc* funDsc_p,
                         u_int* nxtStepStat_p,
                         u_char* nxtStepNo_p
                       );


/*******************************************************************************
 *���ܣ����ղ�����һ֡����,�洢�����ر�ʶ
 * ���룺
 *      Fun_p        ��������
 *      stepRl_p     ����ǰ֡��Ӧ�Ĳ�
 *      funDsc_p     ���Ự����������
 *�����
 *      stnAcsFlag_p ���洢�����ر�ʶ
 *      nxtStepNo_p  ����һ֡����
 *���أ�
 *      0  �ɹ�
 *      <0 �ӿ��쳣
 *�޸���ʷ
 *
*******************************************************************************/
int getChkNxtStepNo ( const FUN_RL* Fun_p,
                      const ONE_STEP* stepRl_p,
                      const T_ptclFunDsc* funDsc_p,
                      T_stnAcsFlag* stnAcsFlag_p,
                      u_int* nxtStepNo_p
                    );


/******************************************************************************
 *���ܣ�����eleVal_p��ֵ��snnCfg_p��ֵ��snnCfg_p.p_Mem�ڽӿ�������new�ռ䣩
 * ���룺
 *      actChnlID     ����������ʶ
 *      stepNum       ������stepNum����ֵ
 *      frmNum        ������frmNum֡��ֵ
 *      eleVal_p      ��ELE_TYPE [11]���ͱ�����ȡֵ����
 *      T_snnCfg      ����Ҫ��ֵ�Ľṹ

 *�����
 *      T_snnCfg      ����ֵ��Ľṹ
 *���أ�
 *      0  �ɹ�
 *      <0 �ӿ��쳣
 *�޸���ʷ
 *    20180223151731��T_snnCfg�ṹ��仯�����ڲ��ӿ���ʵ����;������ע���ڲ��߼�
*******************************************************************************/
int setOneFrmCfg ( u_char actChnlID,
                   u_char stepNum,
                   u_char frmNum,
                   ELE_TYPE* eleVal_p,
                   T_snnCfg* snnCfg_p
                 );


/*******************************************************************************
  *���ܣ�
  *   ��nxtStep��U֡�Ự��������
  *
  * ���룺
  *     srvNo         ���Ự��
  *     stepRl_p      ����ǰ��
  *     funDsc_p      ���Ự����������
  *     ssnPar_p      �����������˵��
  *�����
  *     nxtFrmStep_p  ����һ֡��Ӧ�Ĳ�
  *     nxtFrmStepNo  : ��һ֡�Ĳ���
  *     nxtStepStat_p : ��һ֡�Ự״̬
  *���أ�
  *     0    �ɹ�
  *     ��ֵ �ӿ��쳣
  *�޸���ʷ
  *
*******************************************************************************/
int asgmtNxtUPar ( u_char srvNo,
                   ONE_STEP* stepRl_p,
                   T_ptclFunDsc* funDsc_p,
                   T_snnPara* ssnPar_p,
                   u_char& nxtFrmStepNo,
                   u_int* nxtStepStat_p
                 );


/*******************************************************************************
  *���ܣ�
  *    У�����ݵ�Ԫ��ʶ��
  *
  * ���룺
  *     nowRuleStep_p :��ǰ������
  *     lastRuleStep_p:���򲽵����һ��
  *     stepNo        :��ǰ����
  *     funDsc_p     :�Ự����������
  *     apdu104_p    :��Ҫ��֤���ݵĶ�ӦAPDU104����
  *�����
  *     ��
  *���أ�
  *     0    �ɹ�
  *     <0   ����
  *�޸���ʷ
  *
*******************************************************************************/
int chkAsduDataID ( const ONE_STEP* nowRuleStep_p,
                    const ONE_STEP* lastRuleStep_p,
                    const u_char& stepNo,
                    const T_ptclFunDsc* funDsc_p,
                    APDU104* apdu104_p
                  );


/*******************************************************************************
  *���ܣ�
  *    ������θ�FUN_RL�ṹ��ֵ
  *
  * ���룺
  *     srvNo      �����ܺ�
  *     usingMode  : ģʽ��
  *     PDU_FCode  : ֡ʹ�õ�֡��ʽ����
  *     stnSnnCfg_p: �Ự����֡������Ϣ
  *�����
  *     fun_p     �� Ĭ�ϲ�����
  *���أ�
  *     0    �ɹ�
  *     <0   ����
  *�޸���ʷ
  *
*******************************************************************************/
void setFunRl ( const u_char& srvNo,
                const u_char& usingMode,
                const u_char& PDU_FCode,
                T_snnCfg*& snnCfg_p,
                FUN_RL*& fun_p
              );

/*******************************************************************************
  *���ܣ�
  *    ����tid��ֵ��Ӧ���õĳ��ȣ����ݹ�Լ�涨�õ���
  *
  *
  * ���룺
  *     TID      ��tid��ֵ
  *     flag
  *        0:tidֵ�ĳ��ȣ����ֽ�Ϊ��λ��
  *        1:tidֵ�ĳ��ȣ�ΪλΪ��λ��
  *        2:Ʒ�������ĳ��ȣ����ֽ�Ϊ��λ��
  *        3:ʱ��ĳ��ȣ����ֽ�Ϊ��λ��
  *        4:�ж�ֵ��QDS�Ƿ������һ���:��QDS������ռ�ֽ�
  *        5:tidֵ+QDS�ĳ���
  *�����
  *     ��
  *
  *���أ�
  *     tid��������Ӧֵ�ĳ���
  *�޸���ʷ
  *
*******************************************************************************/
u_char getTidOneLng ( const u_short& TID,
                      const u_char& flag = 0
                    );


/*******************************************************************************
  *���ܣ�(�˽ӿ���ʱδ�õ�)
  *    ����tid��ֵ��Ӧ���õĳ��ȣ����ݹ�Լ�涨�õ���
  *
  *
  * ���룺
  *     TID      ��tid��ֵ
  *
  *�����
  *     tidObjLng_p tid��صĳ���
  *
  *���أ�
  *     0   �ɹ�
  *     ��0 ʧ��
  *�޸���ʷ
  *
*******************************************************************************/
int getTidObjLng ( const u_short& TID,
                   T_tidObjLng* tidObjLng_p
                 );

/*******************************************************************************
  *���ܣ�
  *    ȡ�����е�ĳЩλ��Ϊֵ���
  *
  *
  * ���룺
  *     val_p      :�����������,��Ҫȡֵ�ĵ�ַ
  *     sizeV      :val_p���������͵Ĵ�С
  *     bgBit      :ȡֵ�Ŀ�ʼλ(��1��ʼ)
  *     edBit      :ȡֵ�Ľ���λ
  *
  *�����
  *     val_p
  *
  *���أ�
  *     0   �ɹ�
  *     ��0 ʧ��
  *ע��:
  *    1.val_p����������ֻ֧�� u_char,u_short,u_int
  *    1.�����ɹ��󽫸���ԭ��������ֵval_p
  *
*******************************************************************************/
int getBitsVal ( void* val_p,
                 const u_char& sizeV,
                 const u_char& bgBit,
                 const u_char& edBit
               );

/*******************************************************************************
  *���ܣ�
  *    ֡���ݸ�T_snnPara��ֵ
  *
  *
  * ���룺
  *     tTidObjLng     :tid��ӦЭ�����ó���
  *     usrData_index  :��֡�����ڲ��п�ʼ��
  *     tUsrData_p     :��Ĵ�ȡ����
  *     apdu104_p      :APDU104��Ӧ
  *     stnAcsFlag     :���ݴ�ȡ���ƽṹ
  *
  *�����
  *     tUsrData_p     :��Ĵ�ȡ����
  *     apdu104_p      :APDU104��Ӧ
  *     stnAcsFlag     :���ݴ�ȡ���ƽṹ
  *
  *���أ�
  *     0   �ɹ�
  *     ��0 ʧ��
  *ע��:
  *    
*******************************************************************************/
int  setPntSnnpar ( const T_tidObjLng& tTidObjLng,
                    const u_short& usrData_index,
                    T_snnUsrData* tUsrData_p,
                    APDU104* apdu104_p,
                    T_stnAcsFlag* stnAcsFlag
                  );

/*******************************************************************************
  *���ܣ�
  *    ���ղ�ȡ�ñ�ʶreqFlag���Ƿ����������Ӧ��
  *
  *
  * ���룺
  *     funDsc_p     :T_ptclFunDsc�ṹ����
  *
  *�����
  *     ��
  *
  *���أ�
  *     0   ����
  *     1   ��Ӧ
  *     ��0 ʧ��
  *ע��:
  *
*******************************************************************************/
inline char getRcvReqFlag( const T_ptclFunDsc* funDsc_p );

/*******************************************************************************
  *���ܣ�
  *    ���Ͳ�ȡ�ñ�ʶreqFlag���Ƿ����������Ӧ��
  *
  *
  * ���룺
  *     funDsc_p     :T_ptclFunDsc�ṹ����
  *
  *�����
  *     ��
  *
  *���أ�
  *     0   ����
  *     1   ��Ӧ
  *     ��0 ʧ��
  *ע��:
  *
*******************************************************************************/
inline char getSndReqFlag ( const T_ptclFunDsc* funDsc_p );

#endif//INTERNALFUNCTION_H