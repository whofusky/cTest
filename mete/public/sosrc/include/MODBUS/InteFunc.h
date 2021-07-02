
// InteFunc.h ����ModbusTcpIp���ڲ�ʹ�õ�һЩ����
#pragma once

#ifndef INTEFUNC_H
#define INTEFUNC_H

#include "ADU.h"
#include <string.h>


#define MC_MAXFC_NUMS    43


//���幦�����Ӧ���ֵַռ�õĳ���
typedef struct
{
    u_char byteLng;/*�ֽڳ���*/
    u_char BitLng; /*λ����*/
} T_MDPNTLNG;

/*Э���Ӧ�̶��Ĺ������Ӧ���ֵַռ�õĳ��ȣ�
(���ñ������ķ�ʽ����g_FCMDBUSLNG�±�Ϊfunction code ��ֵ)
*/
extern const T_MDPNTLNG  g_FCMDBUSLNG[];


/*********************************************************�ڲ�ʹ�ú����ǽӿ�****/
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
                            ONE_STEP* targetStep_p
                          );

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
void addNode ( ONE_STEP* step_p,
               ONE_STEP* tail_step_p
             );

/*******************************************************************************
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
 *    20180411145002��T_snnCfg�ṹ��仯�����ڲ��ӿ���ʵ����;������ע���ڲ��߼�
*******************************************************************************/
int setOneFrmCfg ( u_char actChnlID,
                   u_char stepNum,
                   u_char frmNum,
                   ELE_TYPE* eleVal_p,
                   T_snnCfg* snnCfg_p
                 );

/*******************************************************************************
  *���ܣ�
  *   ��������һ֡��ֵ
  *
  * ���룺
  *     nxtFrmRlStep_p����һ֡��Ӧ�Ĺ���
  *     nxtFrmStep_p  ����ǰ��
  *     nxtFrmStepNo_p: ��һ֡�Ĳ���
  *     funDsc_p      ���Ự����������
  *     ssnPar_p      �����������˵��
  *�����
  *     nxtFrmStep_p  ����һ֡��Ӧ�Ĳ�
  *���أ�
  *     0    �ɹ�
  *     ��ֵ �ӿ��쳣
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
  *���ܣ�
  *    modbus���ղ�У�鹦��
  *
  * ���룺
  *     nowRuleStep_p :��ǰ������
  *     lastRuleStep_p:���򲽵����һ��
  *     stepNo        :��ǰ����
  *     funDsc_p      :�Ự����������
  *     mbap_p        :��Ҫ��֤���ݵĶ�ӦC_MBAP����
  *�����
  *     ��
  *���أ�
  *     0    �ɹ�
  *     <0   ����
  *�޸���ʷ
  *
*******************************************************************************/
int chkRecData (  ONE_STEP* nowRuleStep_p,
                  ONE_STEP* lastRuleStep_p,
                  const u_char& stepNo,
                  T_ptclFunDsc* funDsc_p,
                  C_ModbusADU* mbap_p
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
int getChkNxtStepNo ( FUN_RL* Fun_p,
                      ONE_STEP* stepRl_p,
                      T_ptclFunDsc* funDsc_p,
                      T_stnAcsFlagM* stnAcsFlag_p,
                      u_int* nxtStepNo_p
                    );

/*******************************************************************************
  *���ܣ�
  *    �Ự����֡�������պ��T_snnPara��ֵ
  * ���룺
  *     stepRl_p  �����պ�ĵ�ǰ��
  *     funDsc_p  ���Ự����������
  *     mbap_p    ��C_MBAP����ָ��
  *     stnAcsFlag���洢����ȡ��ʶ
  *�����
  *     ssnPar_p��  ���������˵��
  *���أ�
  *     0 �ɹ�
  *    <0 ʧ��
  *�޸���ʷ
  *
*******************************************************************************/
int asgmtSnnPar ( ONE_STEP* stepRl_p,
                  T_ptclFunDsc* funDsc_p,
                  C_ModbusADU* mbap_p,
                  T_stnAcsFlagM* stnAcsFlag,
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
*******************************************************************************/
int getNxtFrmSnnStatus ( FUN_RL* Fun_p,
                         ONE_STEP* stepRl_p,
                         T_ptclFunDsc* funDsc_p,
                         u_int* nxtStepStat_p,
                         u_char* nxtStepNo_p
                       );

/*******************************************************************************
 *���ܣ���preAddData_p�������һ��one_p�ڵ�
 * ��1�����preAddData_p���м���������Ч�ڵ����one_p�滻��Щ��Ч�ڵ�
 * ��2�����preAddData_p���м���������Ч�ڵ㣬����preAddData_pĩβ���one_p�ڵ�ֵ
 *
 * ���룺
 *      preAddData_p����Ҫ��ֵ�����ݻ�����
 *      one_p       ����Ҫ��ӵĽڵ�

 *�����
 *      preAddData_p���ӽڵ������ݻ�����
 *���أ�
 *      0  �ɹ�
 *      <0 �ӿ��쳣
 *        -1:Դ�����߼�����
 *        -2:�Ѿ�����������ֵ���ƣ�������add������
 *�޸���ʷ
 *
*******************************************************************************/
int addSearData ( void* preAddData_p,
                  void* one_p
                );

/*******************************************************************************
 *���ܣ���preAddData_p��ɾ��one_p�ڵ�
 *         ��1����preAddData_p�в���one_p��Ӧ�Ľڵ�
 *         ��2���ɹ��ҵ�ƥ��ڵ��ֻ��ӦpreAddData_p�ж�Ӧ�ڵ�ֵΪ0(��Ч��״̬
 *
 * ���룺
 *      preAddData_p����Ҫ���ҵ����ݻ�����
 *      one_p       ����Ҫɾ���Ľڵ�

 *�����
 *      preAddData_p��ɾ���ڵ������ݻ�����
 *���أ�
 *      0  �ɹ�
 *      <0 �ӿ��쳣
 *        -1:Դ�����߼�����
 *        -2:Դ�����߼�����
 *        -3:preAddData_p ��û������
 *        -4:preAddData_p��û��ƥ�䵽����
 *�޸���ʷ
 *
*******************************************************************************/
int delSearData ( void* preAddData_p,
                  void* one_p
                );

/*******************************************************************************
*���ܣ���data_p�е����ݰ��ֽڽ��д�С�˷�ת��
*      (������ĺ�REVERS_FLAGΪ0ʱת��������ת����
*    1������Ǵ�˾����˺�����ΪС��
*    2�������С�˾����˺�����Ϊ���
*
*���룺data_p ����Ҫ���з�ת������
*      dataLng�����ݴ�С
*
*�����data_p ��ת�������
*
*����
*    0  �ɹ�
*    <0 ����
*
*******************************************************************************/
int reversEndian ( u_char* data_p,
                   int dataLng
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
  *    ����ELE_TYPE.lngֵ��Ϊ��Ϊ�����ĳ��Ȱ�ELE_TYPE.p_Mem�����ݿ�����dst_p
  *
  * ���룺
  *     dst_p      ����Ҫ��������Ŀ���ַ
  *     eleType_p  : ����Դ�ṹ
  *�����
  *     dst_p      ����Ҫ��������Ŀ���ַ
  *���أ�
  *     void
  *�޸���ʷ
  *
*******************************************************************************/
void*& memcpyEleType ( void* dst_p,
                       const ELE_TYPE* eleType_p
                     );



#endif//INTEFUNC_H