//现场数据综合通信系统--通信子系统公共头文件
//包含子系统所有全局公共定义
//公司：兆方美迪风能工程(上海)有限公司
//编写人：赵金杰
//编写时间：2016年12月

#ifndef CM_SUBSYS_H
#define CM_SUBSYS_H

#include "sysMgr.h"
#include "duGrp_BCPQ.h"
#include "FrameGlobal.h"
#include "DataBase.h"
#include "TransCtrlLayer.h"
#include "data.h"
#include "cfgManage.h"
#include "unitInfMem.h"
#include "countManage.h"
#include"common_comm.h"
#ifdef WINDOWS_PLATFORM //是WINDOWS平台
#else
    #include<stddef.h>
    #include <pthread.h>
    #include <sys/syscall.h>
    #include <unistd.h>
    #include <stdlib.h>
#endif
//
// extern C_DuGrp_BCPQ  m_AsduSndQu;        // 带ASDU发送和传输控制请求的双队列（传输控制下行）
// extern C_DuGrp_BCPQ  m_AsduInfoQu;       // ASDU 信息队列（传输控制上行）
//static snnTbl           m_snnTbl;           // 会话表
//static nsCPLock::CLock  m_Lock;             //

#define SAVE_BUFF 0
#define LOAD_BUFF 1
typedef struct
{
    u_short  addrSign;//寄存器累加次数计数
    u_short*  addrStart_p;//寄存器起始地址
    u_short  addrEnd; //寄存器结束地址
    u_short*  funCode_p;//功能码
    u_short  accumNum;   //累加次数
    u_short*  accumValue_p;//根据功能码所表示的语义变化，如读寄存器则此字段表示寄存器个数，如果是读线圈，则表示bit数
    u_short  addrPos;//寄存器当前位置 ,此值初始化时应与寄存器起始地址一致
} ModbusChnlAddrSign;

typedef struct
{
    u_short      funCode;//功能码
    u_short dataLng;       //缓冲区长度
    u_short dataIdLng;       //缓冲区长度
    T_snnUsrData* data_p;  //此功能码的数据缓冲区
    DATA_ID* data_Id_p;//数据ID表

} UsrData;
typedef struct
{
    u_short      funCodeNum; // 现场MODBUS通信单通道所支持的功能码个数
    UsrData* usrdata_p;
    DATAFORMAT_DATE7B1 dataTime;
    int chnlID; // 通道号
    ModbusChnlAddrSign MCAS;
} BufData; // BufData chl0,chl1;
//

typedef struct
{
    nsCPLock::CLock    datalock;
    u_short            arLen;           // ADDR索引长度,addrIdx_p为数组,此变量为数组的长度
    addrRange*         addrIdx_p;
    T_snnUsrData**     buf_pp;
    u_int              tmStmpLng;  //时标长度
} chnlDataBuf;

typedef struct
{
    u_char  chnlNo;
    u_char  ssnNo;
    u_short pNum;//地址的个数
    T_snnUsrData* usrdataP[248];
} dataMsgP;
int getSnnParaMem ( T_snnPara*& snnPara_p );

//
int finishBlock ( ssnCtlBlk* ssnCtlBlk_p, T_TID trdID, u_short ssnID, u_short chnlNo, u_int errCd );

// 会话请求接口
int reqSnn ( reqInfo* reqInfo_p, ssnReqCnt ssn, T_snnPara* snnStartPar_p, u_short timeout, PMSG& msg_p );
//u_char reqSnn(reqInfo* reqInfo_p, ssnDsc ssnID, snnInfo* snnInfo_p, u_short timeout);

// 规约开创的空间释放掉，其他空间重置
int resetPsvSsn ( ssnCtlBlk* ssnCtlBlk_p, u_short usSsnNo, u_short usChnlNo );

// 创建会话，生成会话实例控制块，相应空间申请及规约库相应规则初始化，并添加到会话容器中
int crtSsn ( C_SsnObj*& tCurSsnObj_p, qSsnReqUint& tSsnReqBlk, C_ChnlObj*& tChnlObj_p,
             HMODULE*& tModule_p, ssnCtlBlk*& tSsnCtlBlk_p, u_int& tErrFlag, u_int actFlag = MC_ACT_FLAG );
//
//
void sysInit();//模仿配置子系统
//
//static int finishBlock(u_short ssnNo,u_short chnlNo,u_char sucFlag);
//
void Et_sys_AppSnd();
//
void Et_sys_AppRec();

// // 系统线程
// static void Tsys_inr(void* pParam);
//
//用户线程
void Tsys_Usr ( void* pParam ); //??????用户线程在类外，否则需要增加设置方法，使之成为数据
//
void Test_NR ( void* pParam );

void Tsys_DtPross ( void* pParam );
//
void Et_sys_TakeErr ( void* pParam );

void Et_sys_DataPutStor ( void* pParam );

void Et_sys_DataOutStor ( void* pParam );

T_snnUsrData* findData ( u_int addr, chnlDataBuf*  datBuff );

//一次写多个点对象snnUsrData_p到缓冲区中
int writeChnMPntObj ( const T_snnUsrData* snnUsrData_p,
                      const u_int& num,
                      chnlDataBuf*  datBuff_p
                    );
/*
    功能:把一帧数据的点值转换成物理量值存入共用内存单元
    输入:ssnPara_p 一帧数据对应的结构
        :chnId    通道id
    输出:unitInfData_p 共用内存对象
    返回:0   成功
         非0 失败
*/
int writeFrmPntToUMem ( const T_snnPara* ssnPara_p,   /*一帧数据对应的结构*/
                        const u_int& chnId,            /*通道号*/
                        C_unitInfData* unitInfData_p  /*共用内存对象*/
                      );
int convPhyToSndChn ( C_unitInfData* unitInfData_p,  /*共用内存对象*/
                      const u_int& getChnNum,        /*要取值的通道号*/
                      const u_int& writeChnNum,      /*要写入的通道号*/
                      const PHY_UNIT_TYPE* phyType_p,/*要操作的物理量phyType_p[]*/
                      const u_int& phyNum            /*要操作的物理量个数*/
                    );
int setData(  T_snnUsrData* ssnPara_p, short stnID, u_char ucType);
int optData ( T_snnPara* ssnPara_p, short stnID, u_char ucType, u_char optType, u_char ssnNo );

int setSsnCfg ( T_snnCfg* ssnStnCnfg_p, u_short  addrStart, u_short  accumValue, int iPtclID );

/*
 *获取系统当前时间与前seconds秒时间
 * 输出：nowTime = 当前时间
 * 输出：beforeTime = 前一时刻时间
 * 输入：nowTime - beforeTime 的偏差（单位秒）
 */
 int getNAndBTime(DATEFORMAT_T& nowTime , DATEFORMAT_T& beforeTime ,int seconds);

/*
 *获取系统当前时间
 */
int getSysTime ( DATAFORMAT_DATE7B1& sysTime , time_t& tTime);

#ifndef WINDOWS_PLATFORM
    /*******************************************************************************
    *功能：数据转发
    * 输入：
    *      tChnlObj_p   ： 通道对象
    *      funDsc_p     ：会话号与主动性
    *      ssnPar_p     ：会话步数据信息，包含该步内单向所有信息对象
    *
    ********************************************************************************/
    int DataForward ( C_ChnlObj* tChnlObj_p, T_ptclFunDsc* funDsc_p, T_snnPara* ssnPar_p );

    //对回话进行相关配置，模拟配置系统
    int getDateID ( DATA_ID* dataId, int chnl, int funcId, int time );
#endif


//系统退出对相应资源进行回收处理
    void sysClear();

//
// // 应用发送线程
// static void Tsys_AppSnd(void* pParam);
//
// // 网络控制传输线程
// static void Tsys_NetSnd(void* pParam);
//
// // 应用接收线程
// static void Tsys_AppRec(void* pParam);
//
// // 网络控制接收线程组
// static void Tsys_NetRec(void* pParam);
//

#endif // ! CM_SUBSYS_H
