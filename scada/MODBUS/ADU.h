
#pragma once

#ifndef ADU_H
#define ADU_H

#include "PDU.h"




/*****************************************MODBUS*******************************/


//Slave Address 通用PDU地址域
typedef u_char ADU_ADDR_VAL;

//Error check 通用PDU差错校验
typedef u_short ADU_ECHK_VAL;

//类型标识符（ASDU_TID）的取值类型定义
typedef UI8 ASDU_TID_VAL;



//MBAP报文头 7字节
typedef struct
{
    u_short trans_id; //Transaction Identifier
    u_short  pctl_id; //Protocol Identifier
    u_short   length; //Length:Number of following bytes
    u_char   unit_id; //Unit Identifier
} ADU_MBAP_VAL;



//modbus tcp 报文标识
typedef struct
{
    u_char  func_id; //Function Code
    u_char  unit_id; //Unit Identifier
    u_short trans_id;//Transaction Identifier
} T_mbsID;



#pragma pack(1)
//MBAP报文头 7字节
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

        //通用PDU地址域
        ADU_ADDR_VAL*   getADU_ADDR_VAL ( u_short& lng_p );
        int setADU_ADDR_VAL ( ADU_ADDR_VAL* p_Mem );

        //通用PDU差错校验
        ADU_ECHK_VAL*   getADU_ECHK_VAL ( u_short& lng_p );
        int setADU_ECHK_VAL ( ADU_ECHK_VAL* p_Mem );

        //MBAP报文头
        ADU_MBAP_VAL*  getADU_MBAPH_VAL ( u_short& lng_p );
        int setADU_MBAPH_VAL ( ADU_MBAP_VAL* p_Mem );

};





#endif//ADU_H 