
#pragma once

#ifndef PDU_H
#define PDU_H

#include "MACRO.h"
#include "DataType.h"




typedef struct
{
    u_int   ASDU_TID;
    u_int   InfoEleLng;//信息元素集中每个信息元素的字节长度
    u_short ASDU_CMTBL [11];
} APDU_DSCR;


/*****************************************MODBUS*******************************/

typedef struct
{
    u_int   ADU_TID;
    u_int   InfoEleLng;     //PDU中数据域每个信息元素的字节长度
    u_short ADU_CMTBL[ADU_CMTBL_NUM];
} ADU_DSCR;


//typedef struct{
//    u_char SYS_ELENum;      //个数
//    ELE_TYPE* SYS_TYPE_p;   //未知元素个数的数组
//}DUT_INF;


//Function code PDU功能码
typedef u_char PDU_FC_VAL;


////类型标识符（ASDU_TID）的取值类型定义
//typedef UI8 ASDU_TID_VAL;
//
//
////可变结构限定词（ASDU_VSQ）的取值类型定义：
////用于ASDU_VSQ元素语义解析
//typedef struct{
//    u_char N: 7;
//    u_char SQ: 1;
//} ASDU_VSQ_VAL;
//
////传送原因（ASDU_COT）的取值类型定义
//typedef struct{
//    u_char CAUSE: 6;
//    u_char P_N: 1;
//    u_char T: 1;
//} ASDU_COT1_VAL;
//
////传送原因（ASDU_COT）的取值类型定义
//typedef struct{
//    ASDU_COT1_VAL cot_value;
//    u_char sAddr;
//} ASDU_COT2_VAL;
//
//// 公共地址（ASDU_ADDR）的取值类型定义：用
////于对ASDU_ADDR元素语义解析
//typedef union{
//    UI8 shortAddr;
//    u_short longAddr; //位宽16
//} ASDU_ADDR_VAL;
//
//
//
//
////信息体地址（BODY_ADDR）的取值类型定义：
////用于BODY_ADDR元素语义解析。
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




//typedef ELE_TYPE ADU_TID; //类型标识
//typedef ELE_TYPE ASDU_LNG;    //ASDU长度
//typedef ELE_TYPE ASDU_VSQ;    //可变结构限定词
//typedef ELE_TYPE ASDU_COT;    //传送原因
//typedef ELE_TYPE ASDU_ADDR;   //ASDU公共地址
//typedef ELE_TYPE BODY_TID;    //信息体类型
//typedef ELE_TYPE BODY_ADDR;   //信息体地址
//typedef ELE_TYPE BODY_SET;    //信息元素集
//typedef ELE_TYPE BODY_TSP;    //信息体时标
//typedef ELE_TYPE ASDU_TSP;    //ASDU时标
//
//typedef DUT_INF ASDU_DUT;
//typedef DUT_INF ASDU_DUID;
//typedef DUT_INF INF_BODY;
//typedef DUT_INF APDU;

class C_ModbusPDU
{
    private:
        u_short DU_Ini_Lng;//ADU创建时分配长度
        u_short DU_Lng;    //ADU单元的实际数据长度
        u_char DUType;     //指明分配空间的用途：为
        //         0时ADU有效，
        //         1时PDU有效，
        //         2时两者都有效

    protected:
        u_char allocFlag;
        u_char* ADU_p;      //ADU首地址
        u_char* PDU_p;      //ASDU首地址
        ADU_DSCR DU_Info;   //ADU数据帧结构描述

    public:
        /*
        使用定制信息初始化成员APDU_Info，计算APDU总长度，
            设置APDU_Lng成员 释放参数指向的地址空间
        allocFlag!=0时，分配ADU存储空间，用存储空间首地
            址（指针）设置ADU成员否则不分配APDU存储空间，
            成员APDU=NULL
        */
        C_ModbusPDU ( ADU_DSCR* cstmInfo,
                      u_char duType,
                      u_char  allocFlag
                    );
        ~C_ModbusPDU();//释放分配空间

        ADU_DSCR* getDU_Info();

        //获取或设置ADU_Info.ADU_CMTBL中未设置的
        //基本元素时，返回NULL
        u_char* getADU_ELEMENT ( u_short& lng_p,
                                 u_short emt
                               );

        //设置ADU_Info.ADU_CMTBL中未设置的基
        //本元素时，返回-10
        int setADU_ELEMENT ( u_char* p_Mem,
                             u_short lng,
                             u_short emt
                           );

        //获取或设置功能码
        PDU_FC_VAL* getPDU_FC_VAL ( u_short& lng_p );
        int setPDU_FC_VAL ( PDU_FC_VAL* p_Mem );


        //获得ADU的实时长度
        u_short getDU_Lng ();

        //获得ADU最初new空间的长度
        u_short getDU_Ini_Lng ();

        //获取当前类中储存的缓冲区类型
        u_char getDUType();

        //获得APDU存储空间的首地址指针
        u_char* getDU ( char duType );

        //获得ASDU存储空间的首地址指针并得
        //到空间长度
        u_char* getDU ( u_short& aduLng,
                        char duType
                      );

        //释放ADU_p或PDU_p
        u_int freeADU ( void );

        //将一块存储空间绑定到对象上，设置成员
        //ADU=memBlock，用于接收
        int bindToMem ( u_char* memBlock_p,
                        u_char duType,
                        u_short ADU_Lng
                      );

};




#endif // PDU_H