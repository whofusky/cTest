
#pragma once

#ifndef PDU_H
#define PDU_H

#include "MACRO.h"
#include "DataType.h"




typedef struct
{
    u_int   ASDU_TID;
    u_int   InfoEleLng;//��ϢԪ�ؼ���ÿ����ϢԪ�ص��ֽڳ���
    u_short ASDU_CMTBL [11];
} APDU_DSCR;


/*****************************************MODBUS*******************************/

typedef struct
{
    u_int   ADU_TID;
    u_int   InfoEleLng;     //PDU��������ÿ����ϢԪ�ص��ֽڳ���
    u_short ADU_CMTBL[ADU_CMTBL_NUM];
} ADU_DSCR;


//typedef struct{
//    u_char SYS_ELENum;      //����
//    ELE_TYPE* SYS_TYPE_p;   //δ֪Ԫ�ظ���������
//}DUT_INF;


//Function code PDU������
typedef u_char PDU_FC_VAL;


////���ͱ�ʶ����ASDU_TID����ȡֵ���Ͷ���
//typedef UI8 ASDU_TID_VAL;
//
//
////�ɱ�ṹ�޶��ʣ�ASDU_VSQ����ȡֵ���Ͷ��壺
////����ASDU_VSQԪ���������
//typedef struct{
//    u_char N: 7;
//    u_char SQ: 1;
//} ASDU_VSQ_VAL;
//
////����ԭ��ASDU_COT����ȡֵ���Ͷ���
//typedef struct{
//    u_char CAUSE: 6;
//    u_char P_N: 1;
//    u_char T: 1;
//} ASDU_COT1_VAL;
//
////����ԭ��ASDU_COT����ȡֵ���Ͷ���
//typedef struct{
//    ASDU_COT1_VAL cot_value;
//    u_char sAddr;
//} ASDU_COT2_VAL;
//
//// ������ַ��ASDU_ADDR����ȡֵ���Ͷ��壺��
////�ڶ�ASDU_ADDRԪ���������
//typedef union{
//    UI8 shortAddr;
//    u_short longAddr; //λ��16
//} ASDU_ADDR_VAL;
//
//
//
//
////��Ϣ���ַ��BODY_ADDR����ȡֵ���Ͷ��壺
////����BODY_ADDRԪ�����������
//#pragma pack(1)
//typedef union{
//    UI8 shortAddr;
//    u_short shortlngAddr;
//    struct HIACH_ADDR{
//        u_short childAddr;
//        UI8 fatherAddr;
//    }hiachAddr;
//} BODY_ADDR_VAL;
//#pragma pack()




//typedef ELE_TYPE ADU_TID; //���ͱ�ʶ
//typedef ELE_TYPE ASDU_LNG;    //ASDU����
//typedef ELE_TYPE ASDU_VSQ;    //�ɱ�ṹ�޶���
//typedef ELE_TYPE ASDU_COT;    //����ԭ��
//typedef ELE_TYPE ASDU_ADDR;   //ASDU������ַ
//typedef ELE_TYPE BODY_TID;    //��Ϣ������
//typedef ELE_TYPE BODY_ADDR;   //��Ϣ���ַ
//typedef ELE_TYPE BODY_SET;    //��ϢԪ�ؼ�
//typedef ELE_TYPE BODY_TSP;    //��Ϣ��ʱ��
//typedef ELE_TYPE ASDU_TSP;    //ASDUʱ��
//
//typedef DUT_INF ASDU_DUT;
//typedef DUT_INF ASDU_DUID;
//typedef DUT_INF INF_BODY;
//typedef DUT_INF APDU;

class C_ModbusPDU
{
    private:
        u_short DU_Ini_Lng;//ADU����ʱ���䳤��
        u_short DU_Lng;    //ADU��Ԫ��ʵ�����ݳ���
        u_char DUType;     //ָ������ռ����;��Ϊ
        //         0ʱADU��Ч��
        //         1ʱPDU��Ч��
        //         2ʱ���߶���Ч

    protected:
        u_char allocFlag;
        u_char* ADU_p;      //ADU�׵�ַ
        u_char* PDU_p;      //ASDU�׵�ַ
        ADU_DSCR DU_Info;   //ADU����֡�ṹ����

    public:
        /*
        ʹ�ö�����Ϣ��ʼ����ԱAPDU_Info������APDU�ܳ��ȣ�
            ����APDU_Lng��Ա �ͷŲ���ָ��ĵ�ַ�ռ�
        allocFlag!=0ʱ������ADU�洢�ռ䣬�ô洢�ռ��׵�
            ַ��ָ�룩����ADU��Ա���򲻷���APDU�洢�ռ䣬
            ��ԱAPDU=NULL
        */
        C_ModbusPDU ( ADU_DSCR* cstmInfo,
                      u_char duType,
                      u_char  allocFlag
                    );
        ~C_ModbusPDU();//�ͷŷ���ռ�

        ADU_DSCR* getDU_Info();

        //��ȡ������ADU_Info.ADU_CMTBL��δ���õ�
        //����Ԫ��ʱ������NULL
        u_char* getADU_ELEMENT ( u_short& lng_p,
                                 u_short emt
                               );

        //����ADU_Info.ADU_CMTBL��δ���õĻ�
        //��Ԫ��ʱ������-10
        int setADU_ELEMENT ( u_char* p_Mem,
                             u_short lng,
                             u_short emt
                           );

        //��ȡ�����ù�����
        PDU_FC_VAL* getPDU_FC_VAL ( u_short& lng_p );
        int setPDU_FC_VAL ( PDU_FC_VAL* p_Mem );


        //���ADU��ʵʱ����
        u_short getDU_Lng ();

        //���ADU���new�ռ�ĳ���
        u_short getDU_Ini_Lng ();

        //��ȡ��ǰ���д���Ļ���������
        u_char getDUType();

        //���APDU�洢�ռ���׵�ַָ��
        u_char* getDU ( char duType );

        //���ASDU�洢�ռ���׵�ַָ�벢��
        //���ռ䳤��
        u_char* getDU ( u_short& aduLng,
                        char duType
                      );

        //�ͷ�ADU_p��PDU_p
        u_int freeADU ( void );

        //��һ��洢�ռ�󶨵������ϣ����ó�Ա
        //ADU=memBlock�����ڽ���
        int bindToMem ( u_char* memBlock_p,
                        u_char duType,
                        u_short ADU_Lng
                      );

};




#endif // PDU_H