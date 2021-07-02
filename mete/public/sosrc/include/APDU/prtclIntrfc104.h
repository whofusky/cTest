/*
说明：
    此头文件只包含 protocol104.cpp 中导出的外部调用接口
    此头文件从protocol104.h中分离出来@20180207111656
*/

#pragma once

#ifndef PRTCLINTRFC104_H
#define PRTCLINTRFC104_H

//#include"APDU104.h"
//#include <string.h>




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




//该宏完成Dll项目内部使用__declspec(dllexport)导出
//在DLL项目外部使用时，用__declspec(dllimport)导入
#ifdef DLL_IMPLEMENT
    #define DLL_API extern"C" __declspec(dllexport)
#else
    #define DLL_API extern"C" __declspec(dllimport)
#endif




#ifdef WINDOWS_PLATFORM
    #define EXPDLL DLL_API
#else
    #define EXPDLL
#endif




/******************************************************************************/

/*功能：创建APDU对象
*输入：
*     cstmInfo_p ： APDU各数据长度配置
*输出：
*     duType     ： 根据duType设置ASDU域是否有效 0为APDU有效 ，1为APDU和ASDU都有效
*     allocFlag  ： allocFlag!=0时，分配APDU存储空间，用存储空间首地址（指针）设置
                    APDU成员
*返回值：
*       返回APDU对象地址
*/

EXPDLL u_char* createAPDU ( APDU_DSCR* cstmInfo_p,
                            u_char duType,
                            u_char  allocFlag
                          );


/*功能：释放APDU对象内存
*输入：
*       APDU_Obj_p ： APDU对象地址
*/
EXPDLL void freeAPDU ( u_char* APDU_Obj_p );


/*功能：获取协议号
*返回值：
*         协议号
*/
EXPDLL int getPtclID();


/*功能：创建步规则
* 输入：
*        srvNo     ：功能号
*        usingMode : 模式码
*输出：
*      stnSnnCfg_p: 默认会话步、帧配置信息
*      ptclFunDsc : 默认会话步对应的cot,tid配置
*返回：
*      FUN_RL     ： 默认步规则
*/
EXPDLL FUN_RL* createRl_P ( u_char srvNo,
                            u_char usingMode,
                            T_snnCfg* stnSnnCfg_p,
                            T_ptclFunDsc* ptclFunDsc
                          );


/*功能：获取通用加载点数据接口：通过会话的步配置信息，返回需要加载的数据点地址
*输入：
*        stnSnnCfg：站会话配置，会话的通道信息块中保留存储地址；
*        stepNo:步号
*输出：给定步号需要加载的点地址、时标和协议要求传输且由存储保存的ASDU其它类型元素值
*
*返回值：执行状态，=0成功，<0 失败
*/
EXPDLL int getGLoadPnt ( T_snnCfg* stnSnnCfg_p,
                         u_char stepNo,
                         T_snnPara* stnSnnPara_p
                       );


/******************************************************************************
 *功能 : 释放ONE_STEP对象空间
 *输入 : ONE_STEP对象指针
 *输出 : 无
 *返回 : 0 成功
 *修改历史
 *
*******************************************************************************/
EXPDLL int freeOneStep ( ONE_STEP* pSpace );


/******************************************************************************
 *功能 : 释放FUN_RL对象空间
 *输入 : FUN_RL对象指针
         T_snnCfg 对象指针
         T_ptclFunDsc 对象指针
 *输出 : 无
 *返回 : 0，成功
 *修改历史
 *
*******************************************************************************/
EXPDLL int  freeRl_P ( FUN_RL* pSpace,
                       T_snnCfg* pSnnCfg,
                       T_ptclFunDsc* ptclFunDsc
                     );


/******************************************************************************
  *功能：步驱动，设置起始帧对应的步
  *输入：
  *      srvNo            ：功能码
  *      usingMode        :模式码
  *      preStepRul_p     ：当前功能步，其中reqPDU_Num为当前功能步已完成的帧编号
  *      statrtStepStnD_p :会话步需要的数据信息（详见此类型说明）
  *输出：
  *      preStepRul_p     :设置完成起始帧后对应的步信息
  *返回：
  *      =0               :接口没有逻辑直接返回
  *      >0               :会话状态
  *      0xffff           :该功能不支持
  *      <0               :接口异常
  *修改历史
  *
******************************************************************************/
EXPDLL int startStep ( u_short ssnNo,
                       u_char usingNode,
                       ONE_STEP* preStepRul_p,
                       T_snnPara* statrtStepStnD_p
                     );


/******************************************************************************
 *功能：步驱动，设置下一帧对应的步
 * 输入：
 *      Fun_p        ：步规则
 *      stepRl_p     ：当前帧对应的步
 *      funDsc_p     ：会话号与主动性
 *      ssnPar_p     ：会话步需要的数据信息（详见此类型说明）
 *输出：
 *      stepRl_p     ：下一帧对应的步
 *      nxtStepStat_p：会话状态
 *返回：
 *      >0           ：调用nxtStep发的这一帧数据的下一帧对应的步号(如果已经是最后
                       一步最后一帧则此值为当前帧对应的步号）
 *      0xffff       ：该功能不支持
 *      <0           ：接口异常
 *修改历史
 *
*******************************************************************************/
EXPDLL int nxtStep ( FUN_RL* Fun_p,
                     ONE_STEP* stepRl_p,
                     T_ptclFunDsc* funDsc_p,
                     T_snnPara* ssnPar_p,
                     u_int* nxtStepStat_p
                   );


/******************************************************************************
  *功能：解ASDU
  *输入：
  *     APDU_p  ：APDU地址
  *     APDU_lng: APDU长度
  *输出：
  *     TID     ：类型标识符
  *     COT     ：传输原因
  *返回：
  *     0       ：正确
  *     <0      ：错误
  *修改历史：
  *
*******************************************************************************/
EXPDLL int chkASDU ( u_char* APDU_p,
                     u_short APDU_lng,
                     u_int* TID_p,
                     u_int* COT_p
                   );


/******************************************************************************
  *功能：获取可能的功能号
  *输入：
  *     TID：     类型标识符
  *     COT：     传输原因
  *输出：
  *     funDsc_p：会话号与数据的方向(控制与监视两个方向)
  *     funNum_p：功能码个数（即funDsc_p数组实际下标值）
  *返回：
  *     协议编号：如101、104
  *修改历史
  *
*******************************************************************************/
EXPDLL int getSsnSet ( u_int TID,
                       u_int COT,
                       T_ptclFunDsc* funDsc_p,
                       u_char* funNum_p
                     );


/******************************************************************************
  *功能：获取会话步号（并绑定新的ASDU对象)
  *输入：
  *     funDsc_p  :会话号与主动性
  *     TID       :类型标识符
  *     COT       :传输原因
  *     PDUObj_p  :APDU104对象
  *     ASDU_p    :ASDU地址
  *     ASDUlng   :ASDU长度
  *输出：
  *     无
  *返回：
  *     < 0       :系统异常
  *     0xffff    :该功能不支持
  *     > 0       :stepNo步号
  *修改历史:
  *
*******************************************************************************/
EXPDLL int getFunStep ( T_ptclFunDsc* funDsc_p,
                        u_int TID,
                        u_int COT,
                        u_char* PDUObj_p,
                        u_char* ASDU_p,
                        u_short ASDUlng
                      );


/******************************************************************************
  *功能：对会话步中的帧进行验收
  * 输入：
  *     Fun_p        :步规则
  *     stepRl_p     :当前步
  *     funDsc_p     :会话号与主动性
  *输出：
  *     ssnPar_p     :详见此类型说明
  *     nxtStepNo_p  :下一帧的步号
  *返回：
  *  0               :成功，
  *  0xffff          :该功能不支持
  *  负数            :失败
  *                -1 :当前会话步错误
  *                -2 :当前会话步中的帧数量错误
  *                -3 :传送原因错误
  *                -4 : 主被动性参数错误
  *                -5 :限定词与实际元素个数不等
  *               -21 :最大值验收失败
  *               -22 :最小值验收失败
  *               -23 :按最大最小区间值验收
  *               其他负值为下层函数返回
  *修改历史
  *
 ******************************************************************************/
EXPDLL int chkFun ( FUN_RL* Fun_p,
                    ONE_STEP* stepRl_p,
                    T_ptclFunDsc* funDsc_p,
                    T_snnPara* ssnPar_p,
                    u_int* nxtStepNo_p
                  );


/*功能：会话验收
 * 输入：
 *     stepRl_p：当前步
 *     funDsc_p：会话号与主动性
 *     ASDU_p  ：ASDU数据地址
 *     ASDUlng ：ASDU数据长度
 *     duType  ：ASDU指针是否有效
 *输出：
 *     ssnPar_p：  详见此类型说明
 *     nxtStepNo_p：下一帧的步号
 *返回：
 *  0表示绑定成功，-1 绑定失败
 */
EXPDLL int bindToMem ( ONE_STEP* stepRl_p,
                       T_ptclFunDsc* funDsc_p,
                       u_char* ASDU_p,
                       u_short ASDUlng,
                       u_char duType
                     );


/*功能：会话验收
 * 输入：
 *     apduObj_p：APDU对象指针
 *返回：
 *      ASDU数据指针
 */
EXPDLL u_char* getDU ( u_char* apduObj_p,
                       u_short& apduLng,
                       char duType
                     );


EXPDLL u_short getDULng ( u_char* apduObj_p );


EXPDLL void setAPDU_DSCR ( u_char* apduObj_p, APDU_DSCR apduDSCR );




/******************************传输控制*****************************************/

/*功能：获取报文头长度
 *返回值：
 *      报文头长度
 */
EXPDLL u_char getHeaderlng();


/*功能：组装APDU，根据ASDU数据信息和实时参数，可以构造出APCI。由APCI和ASDU组装成
 *APDU。在104规约中ASDU长度不为0，说明此帧为I帧。如果ASDU数据长度等于0，且ASDU数
 *据句柄不为NULL，说明此帧必定为U帧，由于S帧是由接收线程构造而来，它不会通过会话
 *的形式来直接控制。此函数并不修改实时参数，修改参数工作是由专门的函数
 *setTransCtrlParam()完成。
 *
 *输入：
 *      rtmParam   ：实时参数，包含了发送序号，接受序号 ， 以及K，W等值
 *输出：
 *      ASDUFrm_p   : ASDU/APDU数据句柄
 *      ASDUFrm_Lng : ASDU/APDU数据长度
 */
EXPDLL void packAPDU ( void* rtmParam_p,
                       void* cfgParam_p,
                       u_char* ASDUFrm_p,
                       u_short ASDUFrm_Lng,
                       u_char* APDU_buf_p,
                       u_char& APDU_bufLng
                     );


/*功能：检查并解析数据。首先分析数据是否含有一帧合法的104规约数据。若有合法数据，
 *就将APDU数据复制到通道接收缓冲指针指向的地址中。然后在进一步分析数据帧的类型。
 *根据数据帧的类型，构造相应帧或取出ASDU数据（数据均保持在源通道数据接收句柄上）
 *输入参数：
 *         revDataFrm : 通道上接收端数据缓冲区指针--ASDU
 *         rtmParam_p : 传输控制实时参数
 *         cfgParam   : 规约配置参数
 *返回值：
 *         1.高字节表示数据帧的类型，1表示传输控制帧，0表示ASDU帧（当低字节表示
 *           错误时，高字节为操作掩码）
 *         2.低字节表示传输状态(或错误)
 */
EXPDLL u_short chkAPDUData ( revBuf* revDataFrm,
                             void* rtmParam_p,
                             void* cfgParam_p
                           );

/*******************************************************************************
* 功能：检查底层发送的状态
* 描述：按照IEC104协议的标准，检查当前发送操作是否合理。它是通过通道实时参数中的当前
*      状态stage和即将发送数据的类型来判断当前操作是否合理。此操作并不更新状态，待数
*      据发送成功，再去更新到下一个状态。
* 接口参数：
*     1.role     通道角色，由角色决定了状态机的类型；
*     2.rtmParam 通道实时参数，记录了通道的当前状态stage；
*     3.data     通道数据；
*     4.length   数据长度；
* 返回值：0：发送准备就绪； >=1：发送操作非法,返回非法操作阶段号，含义如下
*   错误编码：int（高字节：通道的操作 低字节：错误类型
*******************************************************************************/
EXPDLL u_int CheckSendState ( short role,
                              void* rtmParam_p,
                              u_char* data_p,
                              u_short length
                            );


/*******************************************************************************
* 功能：检查底层接收的状态
* 描述：按照IEC104协议的标准，检查当前发送操作是否合理。它是通过通道实时参数中的当前
*      状态stage和即将发送数据的类型来判断当前操作是否合理。此操作并不更新状态，待数
*      据发送成功，再去更新到下一个状态。
* 接口参数：
*     1.role     通道角色，由角色决定了状态机的类型；
*     2.rtmParam 通道实时参数，记录了通道的当前状态stage；
*     3.data     通道数据；
*     4.length   数据长度；
* 返回值：0：发送准备就绪； >=1：发送操作非法,返回非法操作阶段号，含义如下
*   错误编码：int（高字节：通道的操作 低字节：错误类型
*******************************************************************************/
EXPDLL u_int CheckRecvState ( short role,
                              void* rtmParam_p,
                              u_char* data_p,
                              u_short length
                            );


/*******************************************************************************
* 功能：按照IEC104协议标准，更新底层发送的状态和实时参数
* 描述：本函数完成以下功能：
*     1.更新发送状态，它可以通过由通道的当前状态stage和发送数据，计算
*       出下一个状态；
*     2.更新流量控制实时参数，例如发送序号N(S)。
*     3.更新定时器的控制参数，比如若为I或U帧，重置T1定时器。
* 接口参数：
*     1.role        通道角色，由角色决定了状态机的类型；
*     2.rtmParam    通道实时参数，比如记录了通道的当前状态stage；
*     3.cfgParam    通道配置参数，比如用于流量控制的阀值K和W等；
*     4.tmrCtrlCode 定时器控制码
*     5.data        通道数据；
*     6.length      数据长度；
* 返回值：用于协调框架控制，返回值按位操作，含义如下：
*     低第0位 ：设置定时器，非零表示需要框架对定时器进行设置，否则不需要设置
*     低第1位 ：设置流量控制，非零表示需要框架对发送操作控制流量，否则不需要设置()
*     低第2位 ：是否需要发送端协作，是的话，需要构造传输控制数据(左)

*******************************************************************************/
EXPDLL u_short UpdateSendState ( short role,
                                 void* rtmParam_p,
                                 void* cfgParam_p,
                                 u_char* tmrCtrlCode_p,
                                 u_char* data_p,
                                 u_short length
                               );


/* 功能：获取帧类型
 *
 * 描述：
 *      解析data_p是哪一种帧
 * 参数：
 *      data_p为数据地址
 *      length为数据长度
 * 返回值：
 *     返回帧的类型
 */
EXPDLL int getFrameType ( u_char* data_p, u_short length );


/* 功能：
 *      处理接收的状态错误
 * 描述：
 *      根据error类型给出是否需要发送侧进行协调
 * 参数：
 *      error为错误类型，此错误来自chkAPDUData函数的返回值
 * 返回值：
 *      1表示需要协调，0表示不需要协调
 */
EXPDLL int HandleRecvStateError ( transCtrlFrmDsc* ctrlFrm,
                                  void* rtmParam_p,
                                  int error
                                );


/*功能：按照IEC104协议标准，更新底层接收端状态和实时参数
 *描述：本函数完成以下功能：
 *      1.更新发送状态，它可以通过由通道的当前状态stage和发送数据，计算出下一个状态
 *      2.更新流量控制实时参数,例如发送序号R（S）
 *      3.更新定时器的控制参数,比如：接收到I、S或U帧，重置T3定时器。
 *
 *      与发送端更新状态一样，数据帧的存储和帧类型识别都时通过ASDU数据长度来完成的。
 *接口参数：
 *      1.role        通道角色，由角色决定了状态机类型；
 *      2.rtmParam    通道实时参数，比如记录了通道的当前状态stage；
 *      3.cfgParam    通道配置参数，比如用于流量控制的阀值K和W等；
 *      4.tmrCtrlCode 定时器控制码；
 *      5.data        通道数据；
 *      6.length      数据长度；
 *返回值：用于协调框架控制，返回值按位操作，含义如下：
 *        低第1位（0）：设置定时器，非零表示需要框架对定时器进行设置，否则不需要设置
 *        低第2位（1）：设置流量控制，非零表示需要框架对发送操作控制流量，否则不需要
                       设置
 *        低第3位（2）：是否需要发送端协作，是的话，需要构造传输控制数据
 *        低9-16位：帧类型
 */
EXPDLL u_short UpdateRecvState ( short role,
                                 void* rtmParam_p,
                                 void* cfgParam_p,
                                 u_char* tmrCtrlCode_p,
                                 u_char* data_p,
                                 u_short length
                               );


/*******************************************************************************
* 功能：构造传输控制的应答帧数据(此函数在发送侧W值到达极限需回复S帧时要复位定时器)
* 描述：按照IEC104协议标准，本函数应当具有以下情况的功能：
*     1.如果接收数据为控制帧类型，构造相应的应答帧；比如接收到U帧的“激活帧”，需要
*       构造对应的“确认帧”；
*     2.如果接收数据为数据帧类型，且接收数量达到接收上限阀值W时，需要构造一个数据确
*       认帧，即S帧。
* 接口参数：
*     1.ctrlFrm   传输控制信息队列的数据单元结构；
*     2.rtmParam  实时参数；
*     3.frmType   帧类型；
* 返回值：
*     0为成功，1为不成功
*******************************************************************************/
EXPDLL int RespCtrlFrame ( transCtrlFrmDsc* ctrlFrm,
                           void* rtmParam_p,
                           int result
                         );


/*******************************************************************************
* 功能：构造传输控制的应答帧数据
* 描述：按照IEC104协议标准，本函数应当具有以下情况的功能：
*     1.如果接收数据为控制帧类型，构造相应的控制帧；比如接收到U帧的“激活帧”，需要
*       构造对应的“激活帧”；
*     2. 构造S帧
* 接口参数：
*     1.ctrlFrm   传输控制信息队列的数据单元结构；
*     2.rtmParam  实时参数；
*     3.result   帧类型；
* 返回值：
*     0为成功，1为不成功
*******************************************************************************/
EXPDLL int CtrlFrame ( transCtrlFrmDsc* ctrlFrm,
                       void* rtmParam_p,
                       int result
                     );


EXPDLL int APDUbindToMem ( u_char* apduObj_p,
                           u_char* APDU_p,
                           u_short ASDUlng,
                           u_char duType
                         );


/******************************************************************************
  *功能：从缓冲区获得APDU的存储长度
  * 输入：
  *      bufferAdd   ：缓冲区首地址
  *      bufferLng   : 缓冲区长度
  *输出：
  *      无
  *返回：
  *    >0 成功返回APDU长度
  *    －1 缓冲区长度错误
  *    －2 首地址不是启动字符
  *    <0  接口执行异常
  *修改历史
  *
*******************************************************************************/
EXPDLL int getApduLngFrmBuf ( char*  bufferAdd, int bufferLng );


/******************************************************************************
  *功能：定时器超时处理（关闭连接后需要状态转换，由应用层操作）
  * 输入：
  *      parameter_p   ：为保留参数暂时无用
  *返回：
  *   低第1位（位0）：至1为关闭定时器
  *   低第2位（位1）：至1为关闭链接
  *   低第3位（位2）：至1为发送S帧
  *   低第4位（位3）：至1为发送测试激活
  *修改历史
  *
*******************************************************************************/
EXPDLL int timerRingT0 ( void* parameter_p );


EXPDLL int timerRingT1 ( void* parameter_p );


EXPDLL int timerRingT2 ( void* parameter_p );


EXPDLL int timerRingT3 ( void* parameter_p );


/******************************************************************************
  *功能：返回定时器超时处理函数指针
  * 输入：
  *     定时器类型
  *返回：
  *   定时器函数指针
  *修改历史
  *
*******************************************************************************/
EXPDLL int timerRingAdd ( u_char timerId );


EXPDLL int getSaveCom ( T_ptclFunDsc* funDsc_p );



#endif //PRTCLINTRFC104_H