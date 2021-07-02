
/*
说明：
    此头文件定义一些内部调用的接口函数
    此头文件从protocol104.h中分离出来@20180207111656
*/

#pragma once

#ifndef INTERNALFUNCTION_H
#define INTERNALFUNCTION_H

#include"APDU104.h"
#include <string.h>
#include<stdio.h>




//定义TID信息对象
typedef struct
{
    u_short tidQdsLng: 4;  /*值+qds的长度*/
    u_short tidBitLng: 6;  /*值长度:单位为位而不是字节*/
    u_short qdsLng: 1;     /*qds长度*/
    u_short tmStmpLng: 5; /*时标长度*/
} T_TIDVALLNG;

#define MC_MAXTID_IDS   141    /*协议里定义的tid个数*/

/*协议对应固定的tid配置信息；
(采用表驱动的方式，其g_TID104下标为tid的值)
*/
extern const T_TIDVALLNG  g_TID104[];



//tid相关长度结构
typedef struct
{
    T_TIDVALLNG cfgLng;    //tid配置的长度
    u_char      tidValLng; //tid值的长度(以字节为单位,不足一个字节的算一个字节)
    /*值与QDS是否参杂在一起的:
    即QDS不单独占字节(如：带品质描述词的单点信息)
      1:参杂在一起
      0:没参杂
    */
    u_char      qdsMixFlag;
} T_tidObjLng;



/********************************内部使用函数非接口*****************************/
/*******************************************************************************
  *功能：在链表ONE_STEP末尾插入一个节点
  * 输入：
  *      one_step_p ：链表头指针
  *      tail_step_p: 要添加的节点指针
  *输出：
  *      one_step_p ：添加节点指针之后的链表头指针
  *返回：
  *      void
  *修改历史：
  *      20170221092324 修改原来的逻辑错误
  *      20170314084211 添加step_p为null的情况
*******************************************************************************/
void addNode ( ONE_STEP* step_p, ONE_STEP* tail_step_p );


/*******************************************************************************
  *功能：
  *      查找startStep_p中步号为targetStep_p->stepNo的步
  *输入：
  *      startStep_p ：规则步
  *      targetStep_p：要查找的步
  *输出：
  *      无
  *返回：
  *      ONE_STEP*
  *              1、异常情况返回 NULL
  *              2、正常情况返回startStep_p中步号为targetStep_p->stepNo的步;
  *修改历史
  *
*******************************************************************************/
ONE_STEP* displayNextNode ( ONE_STEP* startStep_p,
                            const ONE_STEP* targetStep_p
                          );


/*******************************************************************************
  *功能：组装ASDU
  * 输入：
  *      apdu_p      ：APDU104指针
  *      tid_p       : 类型标识
  *      vsq_p       : 可变结构体限定词
  *      cot_p       : ASDU_COT1_VAL类型传送原因
  *      cot2_p      : ASDU_COT2_VAL类型传送原因
  *      addr_p      : ASDU公共地址
  *      bodyAddr_p  : 信息体地址
  *      bodySet_p   : 信息元素集
  *      bodySetLng  : 信息元素集长度
  *      tsp_p       : 信息体时标
  *      bodyObjNum  : 信息对象个数
  *输出：
  *     apdu_p       : 赋值后的APDU104
  *返回：
  *    =0 接口执行成功
  *    <0 接口执行异常
  *修改历史
  *
*******************************************************************************/
int packASDU ( APDU104* apdu_p,
               ASDU_TID_VAL* tid_p,
               ASDU_VSQ_VAL* vsq_p,
               ASDU_COT1_VAL* cot_p,
               ASDU_COT2_VAL* cot2_p,
               ASDU_ADDR_VAL* addr_p,
               u_char* bodyAddr_p,
               u_char* bodySet_p,
               u_short bodySetLng,
               void* tsp_p,
               u_char bodyObjNum = 1
             );


/*******************************************************************************
  *功能：
  *     组装ONE_STEP中除nextStep_p之外的值
  * 输入：
  *      step_p         ：ONE_STEP空间指针
  *      stepNo         : 要组装的值 步号
  *      reqPDU_FCode   : 要组装的值 请求帧使用的帧格式编码
  *      reqPDU_Num     : 要组装的值 请求帧（PDU）个数
  *      reqAPDU_Obj_p  : 要组装的值 请求帧APDU对象指针表
  *      respPDU_FCode  : 要组装的值 响应帧使用的帧格式编码
  *      respPDU_Num    : 要组装的值 响应PDU个数
  *      respAPDU_Obj_p : 要组装的值 响应帧APDU对象指针表
  *      sVerifyBlock_p : 要组装的值 专用步验证块
  *输出：
  *      step_p         : 赋值后的ONE_STEP指针
  *返回：
  *      void
  *修改历史
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


/******************************************************************************
*功能：
*    会话步中帧数据验收后给T_snnPara赋值
* 输入：
*       stepRl_p  ：验收后的当前步
*       funDsc_p  ：会话号与主动性
*       apdu104_p ：APDU104对象指针
*     stnAcsFlag：存储、读取标识
*输出：
*       ssnPar_p：  详见此类型说明
*返回：
*     0 成功
*    <0 失败
*修改历史
*
*******************************************************************************/
int asgmtSnnPar ( const ONE_STEP* stepRl_p,
                  const T_ptclFunDsc* funDsc_p,
                  APDU104* apdu104_p,
                  T_stnAcsFlag* stnAcsFlag,
                  T_snnPara* ssnPar_p
                );


/*******************************************************************************
  *功能：
  *    专用步验证块校验
  *
  * 输入：
  *     iVerifyBlock_p  ：专用步验证块
  *     apdu104_p       ：APDU104对象
  *输出：
  *     iVerifyBlock_p->chkRslt_p：验收状态码//位0 = 1 表示按最大值验收成功，
                                   //位 1 = 1时按最小值验收成功
  *返回：
  *     0  成功
  *     -5  限定词与实际元素个数不等
  *     -21 最大值验收失败
  *     -22 最小值验收失败
  *     -23 按最大最小区间值验收
  *修改历史
  *
*******************************************************************************/
int chkVerifyBlock ( const vrfItem* iVerifyBlock_p,
                     APDU104* apdu104_p
                   );


/*******************************************************************************
*功能：
*   给步的下一帧赋值
*
* 输入：
*       nxtFrmRlStep_p：下一帧对应的规则步
*       nxtFrmStep_p  ：当前步
*       nxtFrmStepNo_p: 下一帧的步号
*       funDsc_p      ：会话号与主动性
*       ssnPar_p      ：详见此类型说明
*输出：
*       nxtFrmStep_p  ：下一帧对应的步
*返回：
*     0 成功
*    -1 主被动模式错误
*    -2 不支持可变限定值SQ=0的情况
*    其他负值为下层函数返回
*修改历史
*
*******************************************************************************/
int asgmtNxtFrmPar ( ONE_STEP* nxtFrmRlStep_p,
                     ONE_STEP* nxtFrmStep_p,
                     u_char* nxtFrmStepNo_p,
                     T_ptclFunDsc* funDsc_p,
                     T_snnPara* ssnPar_p
                   );


/*******************************************************************************
 *功能：获得下一帧会话状态,并输出下一帧步号
 * 输入：
 *      Fun_p        ：步规则
 *      stepRl_p     ：当前帧对应的步
 *      funDsc_p     ：会话号与主动性
 *输出：
 *      nxtStepStat_p：下一帧会话状态
 *      nxtStepNo_p  ：下一帧步号
 *返回：
 *      >0 会话状态
 *      <0 接口异常
 *修改历史
 *
********************************************************************************/
int getNxtFrmSnnStatus ( FUN_RL* Fun_p,
                         ONE_STEP* stepRl_p,
                         T_ptclFunDsc* funDsc_p,
                         u_int* nxtStepStat_p,
                         u_char* nxtStepNo_p
                       );


/*******************************************************************************
 *功能：接收侧获得下一帧步号,存储、加载标识
 * 输入：
 *      Fun_p        ：步规则
 *      stepRl_p     ：当前帧对应的步
 *      funDsc_p     ：会话号与主动性
 *输出：
 *      stnAcsFlag_p ：存储、加载标识
 *      nxtStepNo_p  ：下一帧步号
 *返回：
 *      0  成功
 *      <0 接口异常
 *修改历史
 *
*******************************************************************************/
int getChkNxtStepNo ( const FUN_RL* Fun_p,
                      const ONE_STEP* stepRl_p,
                      const T_ptclFunDsc* funDsc_p,
                      T_stnAcsFlag* stnAcsFlag_p,
                      u_int* nxtStepNo_p
                    );


/******************************************************************************
 *功能：根据eleVal_p的值给snnCfg_p赋值（snnCfg_p.p_Mem在接口中重新new空间）
 * 输入：
 *      actChnlID     ：主被动标识
 *      stepNum       ：给第stepNum步赋值
 *      frmNum        ：给第frmNum帧赋值
 *      eleVal_p      ：ELE_TYPE [11]类型变量，取值参数
 *      T_snnCfg      ：将要赋值的结构

 *输出：
 *      T_snnCfg      ：赋值后的结构
 *返回：
 *      0  成功
 *      <0 接口异常
 *修改历史
 *    20180223151731因T_snnCfg结构体变化，此内部接口无实际用途，所以注释内部逻辑
*******************************************************************************/
int setOneFrmCfg ( u_char actChnlID,
                   u_char stepNum,
                   u_char frmNum,
                   ELE_TYPE* eleVal_p,
                   T_snnCfg* snnCfg_p
                 );


/*******************************************************************************
  *功能：
  *   在nxtStep里U帧会话参数处理
  *
  * 输入：
  *     srvNo         ：会话号
  *     stepRl_p      ：当前步
  *     funDsc_p      ：会话号与主动性
  *     ssnPar_p      ：详见此类型说明
  *输出：
  *     nxtFrmStep_p  ：下一帧对应的步
  *     nxtFrmStepNo  : 下一帧的步号
  *     nxtStepStat_p : 下一帧会话状态
  *返回：
  *     0    成功
  *     负值 接口异常
  *修改历史
  *
*******************************************************************************/
int asgmtNxtUPar ( u_char srvNo,
                   ONE_STEP* stepRl_p,
                   T_ptclFunDsc* funDsc_p,
                   T_snnPara* ssnPar_p,
                   u_char& nxtFrmStepNo,
                   u_int* nxtStepStat_p
                 );


/*******************************************************************************
  *功能：
  *    校验数据单元标识符
  *
  * 输入：
  *     nowRuleStep_p :当前步规则
  *     lastRuleStep_p:规则步的最后一步
  *     stepNo        :当前步号
  *     funDsc_p     :会话号与主动性
  *     apdu104_p    :需要验证数据的对应APDU104对象
  *输出：
  *     无
  *返回：
  *     0    成功
  *     <0   错误
  *修改历史
  *
*******************************************************************************/
int chkAsduDataID ( const ONE_STEP* nowRuleStep_p,
                    const ONE_STEP* lastRuleStep_p,
                    const u_char& stepNo,
                    const T_ptclFunDsc* funDsc_p,
                    APDU104* apdu104_p
                  );


/*******************************************************************************
  *功能：
  *    根据入参给FUN_RL结构赋值
  *
  * 输入：
  *     srvNo      ：功能号
  *     usingMode  : 模式码
  *     PDU_FCode  : 帧使用的帧格式编码
  *     stnSnnCfg_p: 会话步、帧配置信息
  *输出：
  *     fun_p     ： 默认步规则
  *返回：
  *     0    成功
  *     <0   错误
  *修改历史
  *
*******************************************************************************/
void setFunRl ( const u_char& srvNo,
                const u_char& usingMode,
                const u_char& PDU_FCode,
                T_snnCfg*& snnCfg_p,
                FUN_RL*& fun_p
              );

/*******************************************************************************
  *功能：
  *    根据tid的值相应配置的长度（根据规约规定得到）
  *
  *
  * 输入：
  *     TID      ：tid的值
  *     flag
  *        0:tid值的长度（以字节为单位）
  *        1:tid值的长度（为位为单位）
  *        2:品质描述的长度（以字节为单位）
  *        3:时标的长度（以字节为单位）
  *        4:判断值与QDS是否参杂在一起的:即QDS不单独占字节
  *        5:tid值+QDS的长度
  *输出：
  *     无
  *
  *返回：
  *     tid所配置相应值的长度
  *修改历史
  *
*******************************************************************************/
u_char getTidOneLng ( const u_short& TID,
                      const u_char& flag = 0
                    );


/*******************************************************************************
  *功能：(此接口暂时未用到)
  *    根据tid的值相应配置的长度（根据规约规定得到）
  *
  *
  * 输入：
  *     TID      ：tid的值
  *
  *输出：
  *     tidObjLng_p tid相关的长度
  *
  *返回：
  *     0   成功
  *     非0 失败
  *修改历史
  *
*******************************************************************************/
int getTidObjLng ( const u_short& TID,
                   T_tidObjLng* tidObjLng_p
                 );

/*******************************************************************************
  *功能：
  *    取数据中的某些位作为值输出
  *
  *
  * 输入：
  *     val_p      :输入输出参数,需要取值的地址
  *     sizeV      :val_p中数据类型的大小
  *     bgBit      :取值的开始位(从1开始)
  *     edBit      :取值的结束位
  *
  *输出：
  *     val_p
  *
  *返回：
  *     0   成功
  *     非0 失败
  *注意:
  *    1.val_p的数据类型只支持 u_char,u_short,u_int
  *    1.函数成功后将覆盖原来的输入值val_p
  *
*******************************************************************************/
int getBitsVal ( void* val_p,
                 const u_char& sizeV,
                 const u_char& bgBit,
                 const u_char& edBit
               );

/*******************************************************************************
  *功能：
  *    帧数据给T_snnPara赋值
  *
  *
  * 输入：
  *     tTidObjLng     :tid对应协议配置长度
  *     usrData_index  :此帧数据在步中开始点
  *     tUsrData_p     :点的存取数据
  *     apdu104_p      :APDU104对应
  *     stnAcsFlag     :数据存取控制结构
  *
  *输出：
  *     tUsrData_p     :点的存取数据
  *     apdu104_p      :APDU104对应
  *     stnAcsFlag     :数据存取控制结构
  *
  *返回：
  *     0   成功
  *     非0 失败
  *注意:
  *    
*******************************************************************************/
int  setPntSnnpar ( const T_tidObjLng& tTidObjLng,
                    const u_short& usrData_index,
                    T_snnUsrData* tUsrData_p,
                    APDU104* apdu104_p,
                    T_stnAcsFlag* stnAcsFlag
                  );

/*******************************************************************************
  *功能：
  *    接收侧取得标识reqFlag（是否处理请求或响应）
  *
  *
  * 输入：
  *     funDsc_p     :T_ptclFunDsc结构变量
  *
  *输出：
  *     无
  *
  *返回：
  *     0   请求
  *     1   响应
  *     非0 失败
  *注意:
  *
*******************************************************************************/
inline char getRcvReqFlag( const T_ptclFunDsc* funDsc_p );

/*******************************************************************************
  *功能：
  *    发送侧取得标识reqFlag（是否处理请求或响应）
  *
  *
  * 输入：
  *     funDsc_p     :T_ptclFunDsc结构变量
  *
  *输出：
  *     无
  *
  *返回：
  *     0   请求
  *     1   响应
  *     非0 失败
  *注意:
  *
*******************************************************************************/
inline char getSndReqFlag ( const T_ptclFunDsc* funDsc_p );

#endif//INTERNALFUNCTION_H