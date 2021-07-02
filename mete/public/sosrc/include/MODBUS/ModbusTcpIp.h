
#pragma once

#ifndef MODBUSTCPIP_H
#define MODBUSTCPIP_H

#include "ADU.h"
#include <string.h>


#ifdef _WIN32
    #ifndef WINDOWS_PLATFORM
        #define WINDOWS_PLATFORM
    #endif
#endif

#ifdef _WIN64
    #ifndef WINDOWS_PLATFORM
        #define WINDOWS_PLATFORM
    #endif
#endif




/*定义下面的宏则表示在
  createRl_P接口中给T_snnCfg
  和T_ptclFunDsc反之则在协议
  之外赋值
*/
//#define INTER_CREATERL_VAL




/*****************************************MODBUS*******************************/

typedef struct
{

    u_char      T0: 2;  //T0定时器用于处理连接建立或关闭操作超时
    u_char      T1: 2;  //T1定时器用于处理会话中请求的应答帧超时
    u_char      T2: 2;  //T2定时器用于处理单向传输时的接收帧超时
    u_char      T3: 2;  //T3定时器用于监视有效连接的空闲时间
} timerType;


// 接收端数据缓存区--通道（组）描述符的一个域
union revBuf
{
    struct
    {
        unsigned short  APDU_Lng;   // 数据长度
        unsigned char*  APDU_Data_p;// 数据头指针
    } APDU_Buf;

    struct
    {
        unsigned short  ASDU_Lng;
        unsigned char*  ASDU_Data_p;
    } ASDU_Buf;
};


typedef struct
{
    u_short fTypeL: 8;               //表示传输状态
    u_short fTypeH: 8;               //数据帧类型
} frameType;


typedef struct
{
    u_int errType: 16;               //表示传输状态
    u_int motion: 16;                //数据帧类型
} sandMotion;


typedef struct
{
    u_short bit0 : 1; //第0位（0）：设置定时器，非零表
    //            示需要框架对定时器
    //            进行设置，否则不需要设置
    u_short bit1 : 1; //第1位（1）：设置流量控制，非零
    //            表示需要框架对发送操作控
    //            制流量，否则不需要设置
    u_short bit2 : 1; //第2位（2）：是否需要发送端协作，是的
    //           话，需要构造传输控制数据
    u_short bit3 : 1; //第3位（3）：是否向应用层发送
    u_short pad  : 4;
    u_short fType: 8; //帧类型
} ctrl_Value;        //


// 传输控制信息队列的数据单元结构
typedef struct
{
    /* 请求协调进入的传输控制状态：
    暂停发送、恢复发送、启动发送
    （初始状态）、停止发送（删除
    或停止通道（组）前）、工作状态*/
    u_char  tranCtrlState;
    u_char  ctrlType;       // 控制帧类型
    u_char  frmLng;         // 控制帧帧长
    char*   ctrlFrm_p;      // 帧数据指针
    void*   tmStamp_p;      // 协调请求时标
} transCtrlFrmDsc;



#ifdef  WINDOWS_PLATFORM
#include "prtclIntrfc1Mdbs.h"
#else //linux
#ifdef __cplusplus
extern "C" {
#include "prtclIntrfc1Mdbs.h"
#endif //__cplusplus

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // WINDOWS_PLATFORM




#endif // MODBUSTCPIP_H