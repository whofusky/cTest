
#pragma once

#ifndef ADU_H
#define ADU_H

#include "PDU.h"




/*****************************************MODBUS*******************************/


//Slave Address ͨ��PDU��ַ��
typedef u_char ADU_ADDR_VAL;

//Error check ͨ��PDU���У��
typedef u_short ADU_ECHK_VAL;

//���ͱ�ʶ����ASDU_TID����ȡֵ���Ͷ���
typedef UI8 ASDU_TID_VAL;



//MBAP����ͷ 7�ֽ�
typedef struct
{
    u_short trans_id; //Transaction Identifier
    u_short  pctl_id; //Protocol Identifier
    u_short   length; //Length:Number of following bytes
    u_char   unit_id; //Unit Identifier
} ADU_MBAP_VAL;



//modbus tcp ���ı�ʶ
typedef struct
{
    u_char  func_id; //Function Code
    u_char  unit_id; //Unit Identifier
    u_short trans_id;//Transaction Identifier
} T_mbsID;



#pragma pack(1)
//MBAP����ͷ 7�ֽ�
typedef struct
{
    u_short trans_id; //Transaction Identifier
    u_short  pctl_id; //Protocol Identifier
    u_short   length; //Length:Number of following bytes
    u_char   unit_id; //Unit Identifier
    u_char   func_id; //Function Code
    char     data[1]; //Variable Length data
} ADU_TCP_VAL;
#pragma pack()




class C_ModbusADU : public C_ModbusPDU
{
    private:

    public:
        C_ModbusADU ( ADU_DSCR* cstmInfo,
                      u_char duType,
                      u_char  allocFlag
                    );
        ~C_ModbusADU();

        //ͨ��PDU��ַ��
        ADU_ADDR_VAL*   getADU_ADDR_VAL ( u_short& lng_p );
        int setADU_ADDR_VAL ( ADU_ADDR_VAL* p_Mem );

        //ͨ��PDU���У��
        ADU_ECHK_VAL*   getADU_ECHK_VAL ( u_short& lng_p );
        int setADU_ECHK_VAL ( ADU_ECHK_VAL* p_Mem );

        //MBAP����ͷ
        ADU_MBAP_VAL*  getADU_MBAPH_VAL ( u_short& lng_p );
        int setADU_MBAPH_VAL ( ADU_MBAP_VAL* p_Mem );

};





#endif//ADU_H 