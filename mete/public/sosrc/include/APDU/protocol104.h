
#pragma once

#ifndef PROTOCOL104_H
#define PROTOCOL104_H
#include"APDU104.h"
#include <string.h>




#ifdef _WIN32
    #ifndef WINDOWS_PLATFORM
        #define WINDOWS_PLATFORM
    #endif
#endif

#ifdef WIN64
    #ifndef WINDOWS_PLATFORM
        #define WINDOWS_PLATFORM
    #endif
#endif




/*��������ĺ����ʾ��
  createRl_P�ӿ��и�T_snnCfg
  ��T_ptclFunDsc��֮����Э��
  ֮�⸳ֵ
*/
//#define INTER_CREATERL_VAL




//typedef struct
//{
//  u_char*         ASDU_p;         //ASDUָ��
//  u_short         ASDU_lng;       //ASDU����
//  u_short         chnlNo;         //ͨ�����飩��
//  //snnDsc*           snnDsc_p;   //������ƹ���KEY
//  u_char          ssnRlMode;      //�Ự��������ģʽ
//  u_char          ssnAct;         //������Ʒ�ʽ
//  u_char          ssnStepNo;      //������ƽ׶κ�
//}qASDUSndReqUnit;




typedef struct
{

    u_char      T0: 2; //T0��ʱ�����ڴ������ӽ�����رղ�����ʱ
    u_char      T1: 2; //T1��ʱ�����ڴ����Ự�������Ӧ��֡��ʱ
    u_char      T2: 2; //T2��ʱ�����ڴ���������ʱ�Ľ���֡��ʱ
    u_char      T3: 2; //T3��ʱ�����ڼ�����Ч���ӵĿ���ʱ��
} timerType;


// ���ն����ݻ�����--ͨ�����飩��������һ����
union revBuf
{
    struct
    {
        unsigned short  APDU_Lng;       // ���ݳ���
        unsigned char*  APDU_Data_p;    // ����ͷָ��
    } APDU_Buf;

    struct
    {
        unsigned short  ASDU_Lng;
        unsigned char*  ASDU_Data_p;
    } ASDU_Buf;
};


typedef struct
{
    u_short fTypeL: 8; // �ײ���ղ�APDU������������
    u_short fTypeH: 8; //����֡����
} frameType;


typedef struct
{
u_int errType:
    16; //��׼�����ͻ�������֮ǰ����鷢��״̬������״̬�ı���
    u_int motion: 16; //ͨ��ִ����
} sandMotion;


typedef struct
{
u_short bit0 :
    1; //��0λ��0�������ö�ʱ���������ʾ��Ҫ��ܶԶ�ʱ���������ã�������Ҫ����
u_short bit1 :
    1; //��1λ��1���������������ƣ������ʾ��Ҫ��ܶԷ��Ͳ�������������������Ҫ����
u_short bit2 :
    1; //��2λ��2�����Ƿ���Ҫ���Ͷ�Э�����ǵĻ�����Ҫ���촫���������
    u_short bit3 : 1; //��3λ��3�����Ƿ���Ӧ�ò㷢��
    u_short pad  : 4;
    u_short fType: 8; //֡����

} ctrl_Value;


// ���������Ϣ���е����ݵ�Ԫ�ṹ
typedef struct
{
    // ����Э������Ĵ������״̬����ͣ���͡��ָ����͡��������ͣ���ʼ״̬����ֹͣ���ͣ�ɾ����ֹͣͨ�����飩ǰ��������״̬
    u_char  tranCtrlState;
    u_char  ctrlType;       // ����֡����
    u_char  frmLng;         // ����֡֡��
    char*   ctrlFrm_p;      // ֡����ָ��
    void*   tmStamp_p;      // Э������ʱ��
} transCtrlFrmDsc;



#ifdef WINDOWS_PLATFORM
#include "prtclIntrfc104.h"
#else  //linux 

#ifdef __cplusplus
extern "C" {
#include "prtclIntrfc104.h"
#endif //__cplusplus

#ifdef __cplusplus
}
#endif//__cplusplus
#endif//WINDOWS_PLATFORM




#endif //  PROTOCOL104_H