#ifndef APDU101_H
#define APDU101_H

#include "APDU1.h"




typedef struct
{
    u_short mScnd;
    u_char min: 6;
    u_char GEN: 1;
    u_char IV: 1;
} CP24TIME2a;                   //�Ӻ��뵽�ֵĶ�����ʱ��2a


typedef UI16 CP16Time2a;        //���������ʱ��2a


typedef F16 NVA;                //��һ��ֵ


const u_short FBP = 0x55AA;     //�̶�������


typedef struct
{
    u_char DCS: 2;
    u_char IV: 6;
} DCO;                          //˫����


typedef struct
{
    u_char DCS: 2;
    u_char IV: 6;               //����������

} RCO;


typedef struct
{
    u_char DIP: 1;
    u_char RES: 3;
    u_char BL: 1;
    u_char SB: 1;
    u_char NT: 1;
    u_char IV: 1;
} DIQ;                          //��Ʒ�������ʵ�˫����Ϣ


typedef struct
{
    u_char ES: 2;
    u_char RES: 1;
    u_char EI: 1;
    u_char BL: 1;
    u_char SB: 1;
    u_char NT: 1;
    u_char IV: 1;
} SEP;                          //�̵籣���豸�����¼�


typedef struct
{
    u_char GS: 1;
    u_char SL1: 1;
    u_char SL2: 1;
    u_char SL3: 1;
    u_char SIE: 1;
    u_char SRD: 1;
    u_char RES: 2;
} SPE;                          //�̵籣���豸�����¼�


typedef struct
{
    u_char GC: 1;
    u_char CL1: 1;
    u_char CL2: 1;
    u_char CL3: 1;
    u_char RES: 4;
} OCI;                          //�̵籣���豸�����·��Ϣ


typedef F16 NVA;                //��һ��ֵ


typedef I16 SVA;                //��Ȼ�ֵ


typedef struct
{
    u_char SCS: 1;
    u_char RES: 1;
    u_char QOC: 6;
} SCO;                          //������


typedef struct
{
    u_char SPI: 1;
    u_char RES: 3;
    u_char BL: 1;
    u_char SB: 1;
    u_char NT: 1;
    u_char IV: 1;
} SIQ;                          //��Ʒ�������ʵĵ�����Ϣ


typedef u_int BSI;              //������״̬��Ϣ


typedef struct
{
    char value: 7;
    char TRS: 1;
} VTI;                          //��˲��״ָ̬ʾ��ֵ


typedef struct
{
    u_short ST;
    u_short CD;
} SCD;                          //״̬��״̬��λ���


typedef struct
{
    u_char OV: 1;
    u_char RES: 3;
    u_char BL: 1;
    u_char SB: 1;
    u_char NT: 1;
    u_char IV: 1;
} QDS;                          //Ʒ��������


typedef struct
{
    u_char RES: 3;
    u_char EI: 1;
    u_char BL: 1;
    u_char SB: 1;
    u_char NT: 1;
    u_char IV: 1;
} QDP;                          //�̵籣���豸�¼���Ʒ��������


typedef struct
{
    int I32;
    u_char SQ: 5;
    u_char CY: 1;
    u_char CA: 1;
    u_char IV: 1;
} BCR;                          //�����Ƽ���������


typedef struct
{
    u_char value: 7;
    u_char sign: 1;
} CQI;                          //��ʼ��ԭ��


typedef  UI8 QOI ;              //�ٻ��޶���

typedef struct
{
    u_char RQT: 6;
    u_char FRZ: 2;
} QCC;                          //�������ٻ��޶���


typedef struct
{
    u_char KPA: 6;
    u_char LPC: 1;
    u_char POP: 1;
} QPM;                          //����ֵ�����޶���


typedef UI8 QPA;                //���������޶���

typedef struct
{
    u_char QU: 7;
    u_char keep: 1;
} QOC;                          //�����޶���


typedef UI8 QRP;                //��λ���������޶���

typedef struct
{
    u_char value: 7;
    u_char sign: 1;

} FRQ;                          //�ļ�׼�������޶���


typedef struct
{
    u_char value: 7;
    u_char sign: 1;

} SRQ;                          //��׼�������޶���


typedef struct
{
    u_char value: 4;
    u_char sign: 4;

} SCQ;                          //ѡ����ٻ��޶���


typedef UI8 LSQ;                //���ĽںͶε��޶���


typedef struct
{
    u_char value: 4;
    u_char sign: 4;

} AFQ;                          //�ļ��Ͽɻ���Ͽ��޶���


typedef I16 NOF;                //�ļ�����

typedef UI8 NOS;                //������

typedef u_int LOF;              //�ļ���ڵĳ���

typedef UI8 LOS;                //�εĳ���

typedef UI8 CHS;                //У���

typedef struct
{
    u_char STATUS: 5;
    u_char LFD: 1;
    u_char FOR: 1;
    u_char FA: 1;

} SOF;                          //�ļ���״̬

typedef struct
{
    u_char QL: 5;
    u_char keep: 1;
} QOS;                          //�趨�����޶���


typedef UI8 ASDU_TID_VAL;       //���ͱ�ʶ����ASDU_TID����ȡֵ���Ͷ���


//�ɱ�ṹ�޶��ʣ�ASDU_VSQ��
//��ȡֵ���Ͷ��壺����ASDU_VSQ
//Ԫ���������
typedef struct
{
    u_char N: 7;
    u_char SQ: 1;
} ASDU_VSQ_VAL;


typedef struct
{
    u_char CAUSE: 6;
    u_char P_N: 1;
    u_char T: 1;
} ASDU_COT1_VAL;                //����ԭ��ASDU_COT����ȡֵ���Ͷ���


typedef struct
{
    ASDU_COT1_VAL cot_value;
    u_char sAddr;
} ASDU_COT2_VAL;                //����ԭ��ASDU_COT����ȡֵ���Ͷ���


// ������ַ��ASDU_ADDR��
//��ȡֵ���Ͷ��壺���ڶ�
//ASDU_ADDRԪ���������
typedef union
{
    UI8 shortAddr;
    u_short longAddr;           //λ��16
} ASDU_ADDR_VAL;


#pragma pack(1)
//��Ϣ���ַ��BODY_ADDR��
//��ȡֵ���Ͷ��壺����BODY_ADDR
//Ԫ�����������
typedef union
{
    UI8 shortAddr;
    u_short shortlngAddr;
    struct HIACH_ADDR
    {
        u_short childAddr;
        UI8 fatherAddr;
    } hiachAddr;
} BODY_ADDR_VAL;
#pragma pack()




class APDU101 :  public APDU1
{
    private:

    public:
        APDU101 ( APDU_DSCR* cstmInfo, u_char duType, u_char  allocFlag );
        ~APDU101();

        ASDU_TID_VAL*   getASDU_TID_VAL ( u_short* lng_p );
        u_int           setASDU_TID_VAL ( ASDU_TID_VAL* p_Mem );

        ASDU_VSQ_VAL*   getASDU_VSQ_VAL ( u_short* lng_p );
        u_int           setASDU_VSQ_VAL ( ASDU_VSQ_VAL* p_Mem );

        void*           getASDU_COT_VAL ( u_short* lng_p );
        u_int           setASDU_COT_VAL ( ASDU_COT2_VAL* p_Mem );
        u_int           setASDU_COT_VAL ( ASDU_COT1_VAL* p_Mem );

        ASDU_ADDR_VAL*  getASDU_ADDR_VAL ( u_short* lng_p );
        u_int           setASDU_ADDR_VAL ( ASDU_ADDR_VAL* p_Mem );

        BODY_ADDR_VAL*  getBODY_ADDR_VAL ( u_short* lng_p, u_char index = 0 );
        u_int           setBODY_ADDR_VAL ( BODY_ADDR_VAL* p_Mem, u_char index = 0 );

        CP24TIME2a*     getBODY_TSP_VAL_CP24 ( u_short* lng_p, u_char index = 0 );

        CP56TIME2a*     getBODY_TSP_VAL_CP56 ( u_short* lng_p, u_char index = 0 );

        u_int           setBODY_TSP_VAL ( void* p_Mem, u_char index = 0 );

};




#endif //  APDU101_H