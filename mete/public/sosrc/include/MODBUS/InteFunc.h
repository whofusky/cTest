
// InteFunc.h 定义ModbusTcpIp中内部使用的一些函数
#pragma once

#ifndef INTEFUNC_H
#define INTEFUNC_H

#include "ADU.h"
#include <string.h>


#define MC_MAXFC_NUMS    43


//定义功能码对应点地址值占用的长度
typedef struct
{
    u_char byteLng;/*字节长度*/
    u_char BitLng; /*位长度*/
} T_MDPNTLNG;

/*协议对应固定的功能码对应点地址值占用的长度：
(采用表驱动的方式，其g_FCMDBUSLNG下标为function code 的值)
*/
extern const T_MDPNTLNG  g_FCMDBUSLNG[];


/*********************************************************内部使用函数非接口****/
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
                            ONE_STEP* targetStep_p
                          );

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
void addNode ( ONE_STEP* step_p,
               ONE_STEP* tail_step_p
             );

/*******************************************************************************
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
 *    20180411145002因T_snnCfg结构体变化，此内部接口无实际用途，所以注释内部逻辑
*******************************************************************************/
int setOneFrmCfg ( u_char actChnlID,
                   u_char stepNum,
                   u_char frmNum,
                   ELE_TYPE* eleVal_p,
                   T_snnCfg* snnCfg_p
                 );

/*******************************************************************************
  *功能：
  *   给步的下一帧赋值
  *
  * 输入：
  *     nxtFrmRlStep_p：下一帧对应的规则步
  *     nxtFrmStep_p  ：当前步
  *     nxtFrmStepNo_p: 下一帧的步号
  *     funDsc_p      ：会话号与主动性
  *     ssnPar_p      ：详见此类型说明
  *输出：
  *     nxtFrmStep_p  ：下一帧对应的步
  *返回：
  *     0    成功
  *     负值 接口异常
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
  *功能：
  *    modbus接收侧校验功能
  *
  * 输入：
  *     nowRuleStep_p :当前步规则
  *     lastRuleStep_p:规则步的最后一步
  *     stepNo        :当前步号
  *     funDsc_p      :会话号与主动性
  *     mbap_p        :需要验证数据的对应C_MBAP对象
  *输出：
  *     无
  *返回：
  *     0    成功
  *     <0   错误
  *修改历史
  *
*******************************************************************************/
int chkRecData (  ONE_STEP* nowRuleStep_p,
                  ONE_STEP* lastRuleStep_p,
                  const u_char& stepNo,
                  T_ptclFunDsc* funDsc_p,
                  C_ModbusADU* mbap_p
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
int getChkNxtStepNo ( FUN_RL* Fun_p,
                      ONE_STEP* stepRl_p,
                      T_ptclFunDsc* funDsc_p,
                      T_stnAcsFlagM* stnAcsFlag_p,
                      u_int* nxtStepNo_p
                    );

/*******************************************************************************
  *功能：
  *    会话步中帧数据验收后给T_snnPara赋值
  * 输入：
  *     stepRl_p  ：验收后的当前步
  *     funDsc_p  ：会话号与主动性
  *     mbap_p    ：C_MBAP对象指针
  *     stnAcsFlag：存储、读取标识
  *输出：
  *     ssnPar_p：  详见此类型说明
  *返回：
  *     0 成功
  *    <0 失败
  *修改历史
  *
*******************************************************************************/
int asgmtSnnPar ( ONE_STEP* stepRl_p,
                  T_ptclFunDsc* funDsc_p,
                  C_ModbusADU* mbap_p,
                  T_stnAcsFlagM* stnAcsFlag,
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
*******************************************************************************/
int getNxtFrmSnnStatus ( FUN_RL* Fun_p,
                         ONE_STEP* stepRl_p,
                         T_ptclFunDsc* funDsc_p,
                         u_int* nxtStepStat_p,
                         u_char* nxtStepNo_p
                       );

/*******************************************************************************
 *功能：向preAddData_p的中添加一个one_p节点
 * （1）如果preAddData_p的中间区域有无效节点就用one_p替换这些无效节点
 * （2）如果preAddData_p的中间区域都是有效节点，则在preAddData_p末尾添加one_p节点值
 *
 * 输入：
 *      preAddData_p：需要加值的数据缓冲区
 *      one_p       ：将要添加的节点

 *输出：
 *      preAddData_p：加节点后的数据缓冲区
 *返回：
 *      0  成功
 *      <0 接口异常
 *        -1:源数据逻辑错误
 *        -2:已经超过最大添加值限制，不能再add数据了
 *修改历史
 *
*******************************************************************************/
int addSearData ( void* preAddData_p,
                  void* one_p
                );

/*******************************************************************************
 *功能：从preAddData_p中删除one_p节点
 *         （1）在preAddData_p中查找one_p对应的节点
 *         （2）成功找到匹配节点后只是应preAddData_p中对应节点值为0(无效）状态
 *
 * 输入：
 *      preAddData_p：需要查找的数据缓冲区
 *      one_p       ：将要删除的节点

 *输出：
 *      preAddData_p：删除节点后的数据缓冲区
 *返回：
 *      0  成功
 *      <0 接口异常
 *        -1:源数据逻辑错误
 *        -2:源数据逻辑错误
 *        -3:preAddData_p 中没有数据
 *        -4:preAddData_p中没有匹配到数据
 *修改历史
 *
*******************************************************************************/
int delSearData ( void* preAddData_p,
                  void* one_p
                );

/*******************************************************************************
*功能：对data_p中的数据按字节进行大小端反转：
*      (当定义的宏REVERS_FLAG为0时转换，否则不转换）
*    1、如果是大端经过此函数变为小端
*    2、如果是小端经过此函数变为大端
*
*输入：data_p ：需要进行反转的数据
*      dataLng：数据大小
*
*输出：data_p 反转后的数据
*
*返回
*    0  成功
*    <0 错误
*
*******************************************************************************/
int reversEndian ( u_char* data_p,
                   int dataLng
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
  *    根据ELE_TYPE.lng值作为作为拷贝的长度把ELE_TYPE.p_Mem的内容拷贝到dst_p
  *
  * 输入：
  *     dst_p      ：需要拷贝到的目标地址
  *     eleType_p  : 拷贝源结构
  *输出：
  *     dst_p      ：需要拷贝到的目标地址
  *返回：
  *     void
  *修改历史
  *
*******************************************************************************/
void*& memcpyEleType ( void* dst_p,
                       const ELE_TYPE* eleType_p
                     );



#endif//INTEFUNC_H