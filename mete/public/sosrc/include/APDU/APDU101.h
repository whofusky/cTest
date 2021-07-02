#ifndef APDU101_H
#define APDU101_H

#include "APDU1.h"




typedef struct
{
    u_short mScnd;
    u_char min: 6;
    u_char GEN: 1;
    u_char IV: 1;
} CP24TIME2a;                   //从毫秒到分的二进制时间2a


typedef UI16 CP16Time2a;        //毫秒二进制时间2a


typedef F16 NVA;                //归一化值


const u_short FBP = 0x55AA;     //固定测试字


typedef struct
{
    u_char DCS: 2;
    u_char IV: 6;
} DCO;                          //双命令


typedef struct
{
    u_char DCS: 2;
    u_char IV: 6;               //步调节命令

} RCO;


typedef struct
{
    u_char DIP: 1;
    u_char RES: 3;
    u_char BL: 1;
    u_char SB: 1;
    u_char NT: 1;
    u_char IV: 1;
} DIQ;                          //带品质描述词的双点信息


typedef struct
{
    u_char ES: 2;
    u_char RES: 1;
    u_char EI: 1;
    u_char BL: 1;
    u_char SB: 1;
    u_char NT: 1;
    u_char IV: 1;
} SEP;                          //继电保护设备单个事件


typedef struct
{
    u_char GS: 1;
    u_char SL1: 1;
    u_char SL2: 1;
    u_char SL3: 1;
    u_char SIE: 1;
    u_char SRD: 1;
    u_char RES: 2;
} SPE;                          //继电保护设备启动事件


typedef struct
{
    u_char GC: 1;
    u_char CL1: 1;
    u_char CL2: 1;
    u_char CL3: 1;
    u_char RES: 4;
} OCI;                          //继电保护设备输出电路信息


typedef F16 NVA;                //归一化值


typedef I16 SVA;                //标度化值


typedef struct
{
    u_char SCS: 1;
    u_char RES: 1;
    u_char QOC: 6;
} SCO;                          //单命令


typedef struct
{
    u_char SPI: 1;
    u_char RES: 3;
    u_char BL: 1;
    u_char SB: 1;
    u_char NT: 1;
    u_char IV: 1;
} SIQ;                          //带品质描述词的单点信息


typedef u_int BSI;              //二进制状态信息


typedef struct
{
    char value: 7;
    char TRS: 1;
} VTI;                          //带瞬变状态指示的值


typedef struct
{
    u_short ST;
    u_short CD;
} SCD;                          //状态和状态变位检出


typedef struct
{
    u_char OV: 1;
    u_char RES: 3;
    u_char BL: 1;
    u_char SB: 1;
    u_char NT: 1;
    u_char IV: 1;
} QDS;                          //品质描述词


typedef struct
{
    u_char RES: 3;
    u_char EI: 1;
    u_char BL: 1;
    u_char SB: 1;
    u_char NT: 1;
    u_char IV: 1;
} QDP;                          //继电保护设备事件的品质描述词


typedef struct
{
    int I32;
    u_char SQ: 5;
    u_char CY: 1;
    u_char CA: 1;
    u_char IV: 1;
} BCR;                          //二进制计数器读数


typedef struct
{
    u_char value: 7;
    u_char sign: 1;
} CQI;                          //初始化原因


typedef  UI8 QOI ;              //召唤限定词

typedef struct
{
    u_char RQT: 6;
    u_char FRZ: 2;
} QCC;                          //计数量召唤限定词


typedef struct
{
    u_char KPA: 6;
    u_char LPC: 1;
    u_char POP: 1;
} QPM;                          //测量值参数限定词


typedef UI8 QPA;                //参数激活限定词

typedef struct
{
    u_char QU: 7;
    u_char keep: 1;
} QOC;                          //命令限定词


typedef UI8 QRP;                //复位进程命令限定词

typedef struct
{
    u_char value: 7;
    u_char sign: 1;

} FRQ;                          //文件准备就绪限定词


typedef struct
{
    u_char value: 7;
    u_char sign: 1;

} SRQ;                          //节准备就绪限定词


typedef struct
{
    u_char value: 4;
    u_char sign: 4;

} SCQ;                          //选择和召唤限定词


typedef UI8 LSQ;                //最后的节和段的限定词


typedef struct
{
    u_char value: 4;
    u_char sign: 4;

} AFQ;                          //文件认可或节认可限定词


typedef I16 NOF;                //文件名称

typedef UI8 NOS;                //节名称

typedef u_int LOF;              //文件或节的长度

typedef UI8 LOS;                //段的长度

typedef UI8 CHS;                //校验和

typedef struct
{
    u_char STATUS: 5;
    u_char LFD: 1;
    u_char FOR: 1;
    u_char FA: 1;

} SOF;                          //文件的状态

typedef struct
{
    u_char QL: 5;
    u_char keep: 1;
} QOS;                          //设定命令限定词


typedef UI8 ASDU_TID_VAL;       //类型标识符（ASDU_TID）的取值类型定义


//可变结构限定词（ASDU_VSQ）
//的取值类型定义：用于ASDU_VSQ
//元素语义解析
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
} ASDU_COT1_VAL;                //传送原因（ASDU_COT）的取值类型定义


typedef struct
{
    ASDU_COT1_VAL cot_value;
    u_char sAddr;
} ASDU_COT2_VAL;                //传送原因（ASDU_COT）的取值类型定义


// 公共地址（ASDU_ADDR）
//的取值类型定义：用于对
//ASDU_ADDR元素语义解析
typedef union
{
    UI8 shortAddr;
    u_short longAddr;           //位宽16
} ASDU_ADDR_VAL;


#pragma pack(1)
//信息体地址（BODY_ADDR）
//的取值类型定义：用于BODY_ADDR
//元素语义解析。
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