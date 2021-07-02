#ifndef       H_FRAME_GLOBAL
#define H_FRAME_GLOBAL

#pragma once


//平台定义
#include "sysDefine.h"


#ifdef WINDOWS_PLATFORM
    #pragma warning(disable:4996)
    #pragma warning(disable:4244)
#endif

extern "C"
{
#include <stdio.h>
#include <stdlib.h>
};

//#include "CPShared/CPInt.h"
#include "CPShared/CPPub.h"
#include "CPShared/CPLock.h"
#include "CPShared/CPSocket.h"
#include "CPShared/CPLog.h"
// socket
#include <limits.h>


#ifdef WINDOWS_PLATFORM     //是WINDOWS平台
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
    #include "mswsock.h"
    typedef SOCKET sock_descriptor;
    typedef int socklen_t;
    #define sock_invalid INVALID_SOCKET
    #define sock_error SOCKET_ERROR
    #define CloseSocket(sock) closesocket(sock)
    typedef void ( PASCAL FAR* GETACCEPTEXSOCKADDRS ) ( PVOID, DWORD, DWORD, DWORD, LPSOCKADDR*, LPINT, LPSOCKADDR*, LPINT );

    #define crtLock(Key)        (InitializeCriticalSection(&Key))
    #define freeLock(Key)       (DeleteCriticalSection(&Key))
    #define lock(Key)           (EnterCriticalSection(&Key))
    #define unlock(Key)     (LeaveCriticalSection(&Key))
#else
    #include <unistd.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <sys/ioctl.h>
    #include <signal.h>
    #include <dlfcn.h>
    #include <sys/types.h>
    #include <sys/ipc.h>
    #include <sys/msg.h>
    #include"data.h"
    #include"time.h"
    typedef int sock_descriptor;
    #define sock_invalid (-1)
    #define sock_error (-1)
    #define CloseSocket(sock) close(sock)
    #define ioctlsocket ioctl
    #define crtLock(Key)        (pthread_mutex_init(&Key,NULL))
    #define freeLock(Key)       (pthread_mutex_destroy(&Key))
    #define lock(Key)           (pthread_mutex_lock(&Key))
    #define unlock(Key)     (pthread_mutex_unlock(&Key))
#endif


#include "ptcl/MACRO.h"
#include "ptcl/DataType.h"
//#include "comuSocketClient.h"
#include"cIface.h"
/************************************************************/
/*                    基本数据类型重定义                    */
/************************************************************/
typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned int    u_int;
typedef unsigned long   u_long;
typedef char*           T_ASDU_p;

/************************************************************/
/*                    基本宏定义                                       */
/************************************************************/
#ifdef WINDOWS_PLATFORM
    typedef HANDLE T_TID;
    #define VALIDTTID  (NULL)
#else
    typedef unsigned long int T_TID;
    typedef void* HMODULE;
    typedef void* HANDLE;
    typedef key_t* PMSG;
    typedef key_t   MSG;
    #define VALIDTTID  (0)
#endif

#define SYSELE_MASK_104         (0x07F8)
#define SYSELE_MASK_MODBUS      (0x0FFF)
#define MC_DEF_SOFT_TIME_NUM    5
#define MC_DEF_MAX_PRIO         8

#ifndef APDU
    #define APDU    u_char
#endif

#ifndef ASDU
    #define ASDU    u_char
#endif

#define MC_DEF_RPRT_INFO_LEN 4
#define MC_DEF_BUFF_NUM      2
#define MC_DEF_MAX_LEN       256
#define DATA_ID_SIZE         15

enum ssnState
{
    MC_SNNST_OK             = 0x00,     // 成功完成
    MC_SNNST_ENQ_ERR        = 0x01,     // 会话请求入请求队列出错
    MC_SNNST_RES_TIMEOUT    = 0x02,     // 会话超时
    MC_SNNST_RES_FMT_ERR    = 0x03,     // 响应格式错
    MC_SNNST_RES_DCHK_ERR   = 0x04,     // 响应数据验收未通过

    MC_SNNST_RUN        = 0x10,         // 运行
    MC_SNNST_RDY        = 0x11,         // 就绪
    MC_SNNST_BLK        = 0x12,         // 等待验收
    //
    //  MC_SEND_WAIT_CHECK      = 0x11,         // 发送等待验收
    //  MC_SEND_CNUE_EXEC       = 0x12,         // 发送连续执行
    //  MC_SEND_SSN_RETRY       = 0x13,         // 发送会话重试
    //  MC_SEND_SSN_FNSH        = 0x16,         // 发送会话完成
    //
    //  MC_NO_SEND_WAIT_CHECK       = 0x01,     // 不发送等待验收
    //  MC_NO_SEND_CNUE_EXEC        = 0x02,     // 不发送连续执行
    //  MC_NO_SEND_SSN_STOP         = 0x04,     // 不发送会话中止
    //  MC_NO_SEND_SSN_FNSH     = 0x06,         // 不发送会话完成
};
enum StaValue
{
    MC_ACT_FLAG = 0,        // 主动标识值
    MC_PSV_FLAG = 1,        // 被动标识值
    MC_A_P_FLAG = 2,
};

enum StnRole
{
    MC_CTL_STN = 0x01,
    MC_CTLED_STN = 0x02,
    MC_CPLX_STN = 0x03,
};
enum chnlStateBit
{
    MC_CHNL_BIT = ( 1 << 3 ),
};
// 与定时器控制状态统一，待修改！
//#define TIMER_NOP   0       //定时器空操作，继续原状态（或暂停恢复）
//#define TIMER_START 1       //定时器启动操作
//#define TIMER_STOP  2       //定时器定值操作，重新复位
//#define TIMER_PAUSE 3       //定时器暂停操作

#define DEF_ERROR_SSN_NO    0xFFFF

/************************************************************/
/*                       公共类型定义                       */
/************************************************************/
#pragma pack (1)
typedef struct
{
    u_char B_Confidence; //数据可信度
    u_char B_Interval;   //数据的时间间隔
    u_char B_Height;     //数据的所在高度
    u_char B_Source;     //数据来源
    u_char B_Kind;       //数据特征
    u_char B_Length;     //数据项的字节长度
    u_char B_Amount;     //此字节在数据为数据集时有效，代表数据集中元素的个数
    u_short  W_Encoding; //代表所属设备编号
    u_char B_Type;       // 数据类型
    u_char B_Catalog;    //数据具体的大分类，包括文件，sql，数据项，数据集
} DATA_ID;
#pragma pack()
typedef struct
{
    u_int   ASDU_TID;
    u_int   InfoEleLng;         //信息元素集中每个信息元素的字节长度
    u_short ASDU_CMTBL [11];
} APDU_DSCR;

class C_StnObj;
class C_SsnObj;
class C_DStorage;

// 会话结果结构定义，32位位域
struct snnResult
{
    // ...
};
// 消息体结2构
struct Msg_SnnFnsh
{
    long msgTp;     // 消息类型，=（long)tid, tid为线程的进程内标识
    char msgTxt[sizeof ( struct snnResult )]; // 消息数据为会话结果
};


typedef ELE_TYPE ASDU_TID;      //类型标识
typedef ELE_TYPE ASDU_LNG;      //ASDU长度
typedef ELE_TYPE ASDU_VSQ;      //可变结构限定词
typedef ELE_TYPE ASDU_COT;      //传送原因
typedef ELE_TYPE ASDU_ADDR;     //ASDU公共地址
typedef ELE_TYPE BODY_TID;      //信息体类型
typedef ELE_TYPE BODY_ADDR;     //信息体地址
typedef ELE_TYPE BODY_SET;      //信息元素集
typedef ELE_TYPE BODY_TSP;      //信息体时标
typedef ELE_TYPE ASDU_TSP;      //ASDU时标

typedef u_char GEN_FRM[4];
typedef struct
{
    u_char startChar;//启动字符：= SRTCHAR_F1_2_V，见本文的2.2.5部分
    u_char APDULng;//<= APDUMAX - 2
    GEN_FRM ctlField;
} APCI_104;

typedef struct
{
    u_int  byteL: 16;
    u_int byteH: 16;
} Bitint;


typedef struct aaaa
{
    int addr1: 8;
    int body1: 24;
    int addr2: 8;
    int body2: 24;
} T_tmpVal;

typedef struct
{
    u_char vsq_n: 7;
    u_char vsq_sq: 1;
} T_VSQ;

typedef struct
{
    u_char CAUSE: 6;
    u_char P_N: 1;
    u_char T: 1;
} ASDU_COT1_VAL;


/*********************************************************************/
/*                以下在基本规则、101和104规约库中定义               */
/****/
//
//#define UNDEDFINE_TID   0
//#define M_SP            1
//#define M_DP            3
////...
//#define F_DR            126
//#define OUTRANGE_TID    127
//
////#define time stamp type
//#define NO_TMSTAMP      0
//#define CP16_TM2a       2
//#define CP24_TM2a       3
//#define CP56_TM2a       7


union usrData
{
    snnPar_gInfoObj gData;
    snnPar_fInfoObj fData;
};


struct ssnSets              // 会话集结构体
{
    u_short     ssnNo;      // 会话号
    ptclStep*   stepDsc_p;  // 描述
    u_char      setNum;     //ptclStep个数
    u_char      actFlag;    // 主（被）动性
};

//用户点表中各点的通用描述信息
struct baseInfo
{
    unsigned short pntEndian    : 1 ;//=0，小端，=1大端
    unsigned short pntGrpEndian : 1 ;//=0，小端，=1大端
    unsigned short pntType      : 4 ;//数据表示方式；整数，单精小数，双精小数，字符，字符串
    unsigned short startBitNo   : 5 ;//有效起始位，位号范围0~31
    unsigned short pntBitWidth  : 5 ;//此点的有效位长=pntBitWidth+1
};

struct pntInfo//长12字节
{
    baseInfo pntBaseInfo      ; //2B
    unsigned short stnCmnAddr ; //2B
    unsigned short stnSubAddr ; //2B
    unsigned short pntAddr    ; //全局地址=stnCmnAddr:stnSubAddr:pntAddr
    unsigned short scale_pre  ; //系数，数据值=pnt值*cof,float cof = xml::pntDcrb.scale_pre
    unsigned short scale_pst  ; //
};

struct subSysAppDItemValDcrb
{
    unsigned char dItemID[DATA_ID_SIZE];
    unsigned char dTypeAndLng;
    unsigned short SMDItemStgPos;//通过在子系统SM中的偏移定位subSysAppDItemStg
    unsigned char strMaxLng;//字符串最大长度，=0表示非str类型，长度中含'\0'
    pntInfo matchPnt;
};

enum DTYP_LNG
{
    SHORT_INT = 0x04,//4B整型
    LONG_INT = 0x18,//8B整型
    SGL_PRCN = 0x24,//单精
    DB_PRCN = 0x38,//双精
    DATE_TM = 0x47,//精度为秒的日期时间
    ONE_CHAR = 0x51,//1个字符
    TWO_CHARS = 0x52,//2个字符
    THREE_CHARS = 0x53,//3个字符
    FOUR_CHARS = 0x54,
    FIVE_CHARS = 0x55,
    SIX_CHARS = 0x56,
    SEVEN_CHARS = 0x57,
    EIGHT_CHARS = 0x58,//8个字符
    STR_PNTR = 0x6F     //F表示长度--依据机器类型，=sizeof(void*)，字符串必须以'\0'结束
};


// typedef struct
// {
//  ssnCtlBlk*  ssnInst_p;      // 会话实例控制块
//  int*        ssnRslt_p;      // 会话结果集，主动会话返回时使用，该地址保存到会话描述符的通道（组）信息号内
// }ssnInstRslt;
typedef int ssnInstRslt;

typedef void ( *FunType ) ();
typedef void ( *FunTypeOne ) ( void* );

//
// timer
typedef struct
{
    u_char ID;          // 定时器号,定时器通道内编号
    char* name_p;       // 定时器名称ascii string.
    u_char type;        // 周期标识
    u_int fstXpirTm;    // 第一次到时时间, relative to start time
    FunTypeOne tmFunHdl;
    void* tmFunHdlPara_p;
} tmCfg;

#ifndef WINDOWS_PLATFORM
typedef struct _FILETIME
{
    u_long dwLowDateTime;
    u_long dwHighDateTime;
} FILETIME;
#endif


struct clk
{
    unsigned int clkCntr;       // 时钟源计数器
    FILETIME    locFileTm;      // 系统启动的本地文件时间
};
enum tmType
{
    TM_ONCE = 0,
    TM_PERIOD = 1
};
// 软实时定时器1
struct tmr
{
    u_char ID;          // 定时器号,定时器通道内编号
    char* name_p;       // 定时器名称ascii string.

    struct softTmChnl* tmChnl_p;    // 连接定时通道指针

    //=0, uninstall(from init and stop), =1, install(from addSoftTm);
    //=2, start(from install and stop), =5, stop(from start and goOn, =reset=pause+reload);
    //=3, pause(from start and goOn), =4, goOn(from pause);
    u_char wkState;
    u_char type;    // 周期标识. =1、3 period, =0、2 once
    u_int startTm;  // 启动时间relative to start up
    u_int xpirTm;   // 到时时间relative to start up

    tmr*    sortTrgrNxt_p;      // 下一定时器指针（定时驱动链表）
    tmr*    sortTrgrPre_p;      // 上一定时器指针（定时驱动链表）
    void*   tmFunHdlPara_p;     //回调函数参数指针
};

// 软实时定时器通道
typedef struct softTmChnl
{
    u_short tmChnlID;           // 定时通道号
    u_char state;
    u_char  tmrNum;             //tm num in chnl
    tmr* tmrTblHd_p;            //timer table head, =tmrTbl[tmrNum], idx =0: T0,...,=i: Ti, ...
    FunTypeOne* tmFunHdl_p;     //sTm expire handler. =tmFunHdl[tmrNum], idx =0: T0,...,=i: Ti, ...
} tmChnl;
// timer

// channel
// 通道（组）信息块
struct chnlInfoBlk
{
        chnlInfoBlk*    preBlk_p;           // 前趋块
        u_short         chnlNo;             // 通道（组）号
        u_char          ssnRlMode;          // 会话规则在用模式
        T_snnCfg*       ssnStnCnfg_p;       // 会话站配置，如会话规则配置等
        int         psvSsnRprtInfo;         // 被动会话结果报告信息，=系统专用地址

        //ASDU*         actASDU_p;
        //ASDU*         psvASDU_p;
        APDU*           respAPDU_Obj_p;
        APDU*           reqAPDU_Obj_p;
        // asdu相关长度，在会话描述符中，主被动使用一个

        FUN_RL*         funRl_p;            // 步规则 注：目前使用规约库创建该规则！以后使用配置子系统配置，直接读取。

        chnlInfoBlk*    postBlk_p;          // 后继块
    public:
        chnlInfoBlk()
        {
            memset ( this, 0, sizeof ( chnlInfoBlk ) );
        }
};
// channel
// ssn
// 会话描述符结构
typedef struct
{
    u_char          ssnNo;              // 会话号
    FunTypeOne      startSsnInst_p;     // 规约或协议库提供，函数指针类型库函数接口确认后更新
    FunTypeOne      driveSsnInst_p;     // 规约或协议库提供，函数指针类型库函数接口确认后更新
    FunTypeOne      chkSsnInst_p;       // 规约或协议库提供，函数指针类型库函数接口确认后更新
    FunTypeOne      usrCallback_p;      // 会话结束时用户回调函数接口
    //  ASDU*           ASDU_p;             // 会话实例使用的ASDU指针
    chnlInfoBlk*    chnlInfoHead_p;     // 通道（组）信息块链表头指针
} ssnDsc;


typedef struct
{
    u_char  actFlg;
    u_short chnlID;
    int     checkState;

    T_snnPara*  ssnPara_p;
    u_char      drvStepNo;

    ssnInstRslt*    actSsnRprtInfo_p;
    ONE_STEP*       drvPrmtr_p;
    u_short         ASDU_lng;
} T_ssnInstInfoSet;



// 会话实例控制块结构
struct ssnCtlBlk
{
    //会话实例控制块互斥锁(防止同一实例控制块同时在多个线程同时处理)@add@20180710131457
    //nsCPLock::CLock ssnCtlLock;
    
    // 以下为创建时会话请求信息部分
    u_char      role;           // 角色
    u_long      ssnNo;          // 会话号
    u_short     chnlID;         // 通道（组）号
    T_TID       hostTID;        // 用户或系统线程ID，linux中 使用进程内的线程ID
    u_short     ssnTmout;       // 会话超时时间
    tmChnl*     ssnTmHead_p;    // 定时应用通道
    u_char      retryNum;       // 会话失败后的尝试次数：初值0，会话启动参数之一

    // 以下为创建时框架根据上下文情境提供的信息部分
    T_snnPara*      ssnPara_p;          // 会话参数指针，协议与应用发送框架间的双向缓存，可以是usrSsnData或usrSsnFileData类型
    u_char          actFlg;             // 主动性标识，应用帧发送线程创建的为主动，配置子系统创建的为被动
    u_char          actASDU_lng;        // 会话实例的ASDU长度 // 注：暂不使用，因目前是apdu对象指针，对象里面有相关记录（by zhaojj，20170302）
    u_char          psvASDU_lng;        // 会话实例的ASDU长度

    // 以下创建会话实例过程产生的信息部分
    chnlInfoBlk*    myChnlInfoBlk_p;    // 通道组信息块指针
    ssnDsc*         ssnDsc_p;           // 会话描述符指针

    // 以下为实时控制部分，发送侧更新
    u_char          drvStepNo;      // 初值0,接收侧返回，用以指示发送侧装载数据，0xFF表示不加载
    ONE_STEP*       drvPrmtr_p;     // 驱动参数：初值NULL(会话规则) // 创建实例同时，初始化该字段
    u_char          funCtlCd;       // 功能控制码
    u_short         checkStd;       // 验收标准：初值0

    u_char          ptclCtlCd;      // 协议控制码--标识 I 、U 帧

    // 以下为实时信息部分，接收侧更新
    int             checkState;     // 验收状态码：初值0
    u_char          funCtlAck;      // 功能确认
    //  void*           stepCntxt_p;    // 步现信息

    // 以下为实时信息部分，发送侧和接收侧均更新
    u_char          ssnTmrCtlCd;        // 会话定时器控制码
    u_char          rtRunStOrRslt;      // 实时运行状态：初值为完成，主动会话结束时为存放会话结果报告信息

    // 以下为主动会话返回时的会话结果报告信息部分，发送侧的会话结束块负责将结果（rtRunStOrRslt）写入该地址
    ssnInstRslt     actSsnRprtInfo;     // 主动会话结果报告信息，=&(snnInfo.ssnRslt)
    ssnCtlBlk*      nxtSsnCtlBlk_p;     // 下一实例控制块指针（会话对象使用）
    ssnCtlBlk*      preSsnCtlBlk_p;     // 下一实例控制块指针（会话对象使用）

    ssnCtlBlk*      nxtChnlSCBlk_p;     // 下一实例控制块指针（通道链表使用）
};


//
// typedef union
// {
//  snnPar*         snnStartPar_p;      // 会话启动参数，需要修改ASDU存储值，或是创建新的ASDU空间时使用
//  ssnInstRslt     ssnRslt;            // 会话结果集
//  APDU*           ASDU_p;             // 使用的ASDU指针，不需要修改ASDU存储值时使用
// }snnInfo;

// 请求者类型，目的是为了减少传参的数量
typedef struct
{
    u_char  role;
    u_short chnlID;
    u_char  snnMaxRetryNum;         // 会话最大重试次数，不重试=0
    void*   infoObjchkStd_p;        // 信息对象验收标准，由实例保存
} reqInfo;

// move to other file
// 会话表项
// typedef struct
// {
//  u_long      ptcNo;          // 协议号
//  void*       fHandle;        // 文件句柄或描述符
//  char*       snnDsc;         // 会话描述，用于存储为每个会话分配的KEY，防止重复分配
//  u_char      prio;           // 会话优先级
//  C_SnnObj*   ssnObj_p;       // 会话对象指针
// }snnTblItem;

// 会话配置
typedef struct
{
    u_char      ptclNo;     // 104、101...
    u_char      snnDscNo;   // 协议号，站召唤
    u_char      prio;       // 优先级
} snnAppCfg;

////会话关键字
//用于防止重复分配会话表项
typedef struct
{
    u_char ptclNo;          //协议号
    u_char snnDscNo;     //协议内部唯一标识的会话号
} ptclSnnKey;



// 会话表项
typedef struct
{
    u_char      usingFlg;       //在用标识。=0，空闲；=1，在用
    ptclSnnKey      ssnKey;
    HMODULE     fHdl;           // 文件句柄或描述符指针
    u_char      prio;           // 会话优先级
    C_SsnObj*   ssnObj_p;       // 会话对象指针
} snnTblItem;

// 会话表
typedef struct
{
    u_int       ssnNum;         // 表内存在会话总数
    snnTblItem* snnTblEntry_p;  // 表项数组，表项数=ssnNum
} snnTbl;
// move to other file

// ASDU发送请求队列的数据单元结构
typedef struct
{
    ASDU*       ASDU_p;         // ASDU指针
    u_short     ASDU_lng;       // ASDU长度
    u_short     chnlID;         // 通道（组）号
    ssnDsc*     snnDsc_p;       // 传输控制规则KEY
    u_char      ssnRlMode;      // 会话规则在用模式
    u_char      ssnAct;         // 传输控制方式
    u_char      ssnStepNo;      // 传输控制阶段号
} qASDUSndReqUnit; // 注：20170228,暂时不用！(by zhaojj)

// 会话请求内容
struct qSsnReqUint
{
        u_char  role;           // 角色
        T_TID   tID;            // 请求者线程ID
        u_short chnlID;         // 通道组号
        u_short ssnID;          // 会话号
        T_snnPara*  snnPara_p;  // 会话启动参数指针
		u_short  addrStart;//点地址起始地址（Modbus增加）
		u_short  accumValue;//点的个数 （Modbus增加）
        u_short timeout;        // 会话超时时间
    public:
        qSsnReqUint ()
        {
            role = 0;
            tID = 0;
            chnlID = 0;
            ssnID = 0;
            timeout = 0;
			addrStart = 0;
			accumValue = 0;
        }

};

// 接收端数据缓存区--通道（组）描述符的一个域
union revBuf
{
    struct
    {
        unsigned short  APDU_Lng;       // 数据长度
        unsigned char*  APDU_Data_p;    // 数据头指针
    } APDU_Buf;

    struct
    {
        unsigned short  ASDU_Lng;
        unsigned char*  ASDU_Data_p;
    } ASDU_Buf;
};

// 传输控制帧
struct transCtrlF
{
    unsigned char       fLng;       // 帧长
    char                ctrlType;   // 控制帧类型
    unsigned char*      fData_p;    // 帧数据指针
};

// 传输控制信息队列的数据单元结构
typedef struct
{
    // 请求协调进入的传输控制状态：暂停发送、恢复发送、启动发送（初始状态）、停止发送（删除或停止通道（组）前）、工作状态
    u_char  tranCtrlState;
    u_char  ctrlType;       // 控制帧类型
    u_char  frmLng;         // 控制帧帧长
    char*   ctrlFrm_p;      // 帧数据指针
    void*   tmStamp_p;      // 协调请求时标 // 暂时不需要，待定！20170411
} transCtrlFrmDsc;

// 传输控制信息队列的数据单元结构
typedef struct
{
    u_short         chanlID;    // 通道（组）号
    transCtrlFrmDsc ctrlFrmDsc; // 传输控制帧描述，入队时*ctrlFrm_p域要另行分配空间形成控制帧副本
} qTransCtrlInfoUnit;

// ASDU 信息接收单元
typedef struct
{
    u_short chanlID;        // 通道（组）号
    APDU*   ASDU_p;         // ASDU指针
    u_short ASDU_Lng;       // 伪ASDU长度,为了控制回话，控制帧的时候长度为0
    u_short realData_lng;    //真实ASDU长度
    u_char  DU_type;        // 类型
} qASDUInfoRcvUnit;

// ASDU 信息对象描述结构
typedef struct
{
    u_short infObjLng;
    u_char* infObjSet_p;
} ASDU_InfoObjDsc;

// 暂时不使用！勿删！
// struct chkStd
// {
//  u_char  chkType;
//  u_int   chkDataPnt;
//  u_int   maxVal1;
//  u_int   minVal1;
//  u_int   maxVal2;
//  u_int   minVal2;
//  u_int   tmPara;
//  chkStd* next_p;
// };

// 会话实例表项（通道）
struct ssnItem
{
    u_char      ucRole;         // 角色
    u_char      ucFlag;         // 会话控制块类型（控制下面几个字段值的有效性）(主、被动有效位对应宏数值的相应位)

    ssnCtlBlk*  ssnCtlBlk_p;    // 非共享会话实例控制块链表指针 / 或 / （主/被动）单实例会话控制块
    u_short     usNum;          // 非共享会话实例控制块数（主动）
    void*       actDataSwap_p;  // 非共享会话实例数据交换区（主动）

    ssnCtlBlk*  psvSsn_p;       // 被动会话控制块指针
    void*       psvDataSwap_p;  // 被动会话数据交换区
};

// 会话实例表
struct ssnTbl
{
    ssnItem     ssnTblEntry[MC_DEF_MAX_PRIO];
};

class C_ChnlObj;

// 通道（组）表项
struct chnlItem
{
    u_short     stnID;      // 远程站号
    // ...
    C_ChnlObj*  chnlObj;    // 通道（组）对象指针
};

// 通道（组）管理表
struct chnlTbl
{
    u_short chnlNum;            // 通道（组）个数
    chnlItem*   chnlTblEntry_p; // 表项数组
};

typedef void ( *FunType ) ();

typedef void ( *FunTypeOne ) ( void* );

typedef struct lnkTmChnl lnkTmChnl;

// 软实时定时器
struct softTimer
{
    u_short usID;           // 定时器号
    u_char  ucName;         // 定时器名称

    lnkTmChnl*  lnkChnl_p;  // 连接定时通道指针

    u_char  ucStartFlag;    // 启动标志
    u_short usCircleFlag;   // 周期标识
    u_short usStartTime;    // 启动时间
    u_short usTotalTime;    // 到时时间

    softTimer*  stOrdNext_p;    // 下一定时器指针（定时驱动链表）
    softTimer*  stOrdPre_p;     // 上一定时器指针（定时驱动链表）
    softTimer*  stChnlNext_p;   // 下一定时器指针（定时通道链表）
};

// 软实时定时器
struct lnkTmChnl
{
    u_short usLnkID;        // 连接定时通道号
    lnkTmChnl* ltcNext_p;   // 下一连接定时通道指针
    softTimer* tmHead_p;    // 定时器指针表
    softTimer* stTail_p;    // 尾指针

    void    ( *tmFunT0[MC_DEF_SOFT_TIME_NUM] ) (); // T0到时事件处理例程入口
    void    ( *tmFunT1 ) (); // T1到时事件处理例程入口
    void    ( *tmFunT2 ) (); // T2到时事件处理例程入口
    void    ( *tmFunT3 ) (); // T3到时事件处理例程入口
};

// 远程站概览
typedef struct
{
    u_long      stnAddr;    //远程站通信地址：IP/串行总线设备号，主机字节顺序
    u_short     stnAppID;   //远程站应用标识符：port/串行设备波特率，主机字节顺序
    u_short     stnID;      //远程站号
    u_char      role;       //远程站角色
    u_short     localPort;  //本地端口
    u_long      localAddr;  //远程站通信地址：IP/串行总线设备号，主机字节顺序
} stnOrVew;

// 通道（组）表项
typedef struct
{
    stnOrVew    stnOverView;    // 站概览
    C_ChnlObj*  chnlObj_p;      // 通道（组）对象指针
    u_short     chnlState;      // 通道状态
} chnlTblItem;


struct addrItem
{
    u_int   uiAddr;     // 通信地址（ip地址或串行地址）
    u_short usMask;     // 通信标识（端口或串口）
};

struct chnlDsc
{
    short   sChnlNo;            // 通道号
    char*   chnlName_p;         // 通道名指针
    char    cRole;              // 角色
    void*   locOpr_p;           // 通道操作句柄

    ssnSets*    locSsnInfo_p;   // 本地支持的会话集信息指针
    short       numLocSsn;

    void*   tranCtrl_p;         // 传输控制配置参数指针
    void*   realTranCtrl_p;     // 传输控制实时参数指针

    // 传输控制层超时时间组、定时资源和定时器控制码
    u_short usOutTmT0a[MC_DEF_SOFT_TIME_NUM];   // 超时时间t0~t3、ta
    char    tmCtrlCode[MC_DEF_SOFT_TIME_NUM];   // 定时器控制码
    tmChnl* tmChnl_p;                           // 定时通道指针

    // ... // 传输控制层帧空间描述和伪ASDU空间描述
    char* tranCtrlBuf_p;

    ssnTbl* ssnTbl_p;           // 会话实例表指针

    revBuf  revBuf_p[MC_DEF_BUFF_NUM];      // 接收端数据缓冲指针
    revBuf  sendBuf_p[MC_DEF_BUFF_NUM];     // 发送端数据缓冲指针

    C_StnObj*   stnObj_p;       // 远程站对象指针
    void*       rtnFun_p;       // 回调函数指针

    addrItem    localAddr;      // 本地站地址
    HMODULE     ptclMdl;        // 规约描述
    HMODULE     commMdl;        //公共通讯协议描述符
};

// 远程站描述符
struct stnDsc
{
        char    stnRole;        // 远程站角色
        short   stnID;          // 远程站号
        char*   stnName_p;      // 远程站名指针
        short   chnlID;         // 通道（组）号

        void*   stnSsnInfo_p;   // 远程站支持的会话集信息

        stnDsc* nxtLgcStn_p;        // 逻辑站对象指针
        void*   infoTbl_p;          // 信息区（点表）指针

        u_char  cleTranFlag;        // 周期和突发传输标识

        void*   strgMap_p;          // 存储映像脏标记跟踪索引
        void*   jumpFlw_p;          // 跳变跟踪缓存指针

        stnDsc* nxtStn_p;           // 下一远程站对象指针
        // 暂时不用

        FunTypeOne  callBackFun;    // 信息对象跳变回调例程指针
        FunTypeOne  rcdExpFun;      // 信息对象跳变记录例程指针

        C_DStorage* gStorageObj_p;  // 简易存储对象指针（元素类型）
        C_DStorage* fStorageObj_p;  // 简易存储对象指针（文件类型）
    public:
        stnDsc ()
        {
            stnRole = stnID = chnlID = -1;
            stnName_p = NULL;

            stnSsnInfo_p = infoTbl_p = NULL;
            nxtLgcStn_p = nxtStn_p = NULL;

            cleTranFlag = 0;

            strgMap_p = jumpFlw_p = NULL;
            callBackFun = rcdExpFun = NULL;
			gStorageObj_p = NULL;  // 简易存储对象指针（元素类型）
			fStorageObj_p = NULL;  // 简易存储对象指针（文件类型）
        }
        ~stnDsc()
        {
            stnRole = stnID = chnlID = -1;

            if ( stnName_p != NULL )
            {
                delete stnName_p;
            }

            stnName_p = NULL;

            stnSsnInfo_p = infoTbl_p = NULL;
            nxtLgcStn_p = nxtStn_p = NULL;

            cleTranFlag = 0;

            strgMap_p = jumpFlw_p = NULL;
            callBackFun = rcdExpFun = NULL;
        }
        int copyStn ( stnDsc* srcStn_p )
        {
            stnRole = srcStn_p->stnRole;
            stnID = srcStn_p->stnID;
            chnlID = srcStn_p->chnlID;

            if ( srcStn_p->stnName_p )
            {
                int iLen = strlen ( srcStn_p->stnName_p );
                stnName_p = new char[iLen + 2];

                if ( stnName_p == NULL )
                {
                    return -20;
                }

                memcpy ( stnName_p, srcStn_p->stnName_p, iLen );
                stnName_p[iLen] = '\0';
            }
            else
            {
                stnName_p = NULL;
            }


            stnSsnInfo_p = srcStn_p->stnSsnInfo_p;
            infoTbl_p = srcStn_p->infoTbl_p;

            nxtLgcStn_p = srcStn_p->nxtLgcStn_p;
            nxtStn_p = srcStn_p->nxtStn_p;

            cleTranFlag = srcStn_p->cleTranFlag;

            strgMap_p = srcStn_p->strgMap_p;
            jumpFlw_p = srcStn_p->jumpFlw_p;

            callBackFun = srcStn_p->callBackFun;
            rcdExpFun = srcStn_p->rcdExpFun;

            return 0;
        }
};
struct soleInfo
{
    u_char cstmFlag;        // 定制标识
    void* tmObj_p;          // 定时资源对象指针
    u_char ucSoleFlag;      // 角色标识
    void* ctrlLocAddrTab_p; // 控制站入口：本地端地址表
    u_int ctrlUseChnl;      // 在用通道（组）数
    void* chargStnObj_p;    // 远程被控站对象链表头指针
    void* locAddrTbl_p;     // 模拟被控站入口：本地端地址表
    u_int useChnl;          // 在用通道（组）数
    void* ctrlStnObj_p;     // 远程控制站对象链表头指针
    void* monitor_p;        // 监听句柄
};

struct chnlMgrTab
{
    u_int stnNum;           // 远程站号
    u_char stnAddr[16];     // 远程站地址
    u_int stnAppFlag;       // 远程站应用标识
    u_char state;           // 状态标记
    void* channelObj_p;     // 通道对象指针
};
//////////////////////////////////////////////////////////////////////////
/*dll's fun**/
typedef u_char* ( *T_createAPDU ) ( APDU_DSCR* cstmInfo_p, u_char duType, u_char  allocFlag );
typedef FUN_RL* ( *T_createRl_P ) ( u_char srvNo, u_char usingMode, T_snnCfg* snnCfg_p, T_ptclFunDsc* ptclFunDsc );

////////应用帧发送线程
typedef int ( *T_loadPnt ) ( T_snnCfg* stnSnnCfg_p, u_char stepNo, T_snnPara* stnSnnPara_p );

typedef int ( *T_chk_ASDU ) ( u_char* ASDU_p, u_short ASDU_lng, u_int* TID, u_int* COT );

typedef int ( *T_getSsnSet ) ( u_int TID, u_int COT, T_ptclFunDsc* funDsc_p, u_char* funNum_p );

typedef int ( *T_getFunStep ) ( T_ptclFunDsc* funDsc_p, u_int TID, u_int COT, APDU* PDUObj_p, \
                                u_char* ASDU_p, u_short ASDUlng );

typedef int ( *T_chkFun ) ( FUN_RL* Fun_p, ONE_STEP* stepRl_p, T_ptclFunDsc* funDsc_p, T_snnPara* ssnPar_p, \
                            u_int* nxtStepNo_p );

typedef FUN_RL* ( *T_crt_FunRl ) ( u_char srvNo, u_char usingMode );

typedef u_int ( *T_bindToMem ) ( ONE_STEP* stepRl_p, T_ptclFunDsc* funDsc_p,/* u_char* PDUObj_p,*/\
                                 u_char* ASDU_p, u_short ASDUlng, u_char duType );

typedef u_char* ( *T_getASDU ) ( u_char* apduObj_p );

typedef  u_char* ( *T_getASDU_lng ) ( u_char* apduObj_p, u_short& apduLng, char duType );

typedef int ( *T_freeRl_P ) ( FUN_RL* pSpace, T_snnCfg* pSnnCfg, T_ptclFunDsc* ptclFunDsc );

typedef void ( *T_freeAPDU ) ( u_char* APDU_Obj_p );

typedef void ( *T_setAPDU_DSCR ) ( u_char* apduObj_p, APDU_DSCR apduDSCR );

typedef int ( *T_getFrameType ) ( u_char* data_p, u_short length );

typedef u_char* ( *T_getDU ) ( u_char* apduObj_p, u_short& apduLng, char duType );
//
typedef int ( *T_getPtclID ) ();

typedef int ( *T_getSaveCom ) ( T_ptclFunDsc* funDsc_p );


//////////////////////////////////////////////////////////////////////////

/************************************************************/
/*                 平台相关部分：静态函数                    */
/************************************************************/
//
#ifdef WINDOWS_PLATFORM         //windows 64位系统
#include <WinNT.h>
#include <process.h>
#include <windows.h>

//static int            ssnFnshNfyID;       //会话完成通知(msgID)，msgID for lnx, lzq,2016.12.22

#define MY_MSG      WM_USER+100
////////////////windows 64位得到线程内核句柄函数
////
//功能：通过当前线程ID得到线程的内核句柄（非伪句柄）
//输入：无
//输出：无
//返回：=当前线程内核句柄，成功；=NULL，失败
//
static HANDLE getTrdHdl_WIN64()
{
    HANDLE tPsdoPrsHdl, tPsdoTrdHdl;
    HANDLE tRealTrdHdl, *tRealTrdHdl_p;
    tRealTrdHdl_p = &tRealTrdHdl;

    int tRtn;
    tPsdoPrsHdl = GetCurrentProcess();
    tPsdoTrdHdl = GetCurrentThread();
    tRtn = DuplicateHandle ( tPsdoPrsHdl, tPsdoTrdHdl, tPsdoPrsHdl, tRealTrdHdl_p, 0, false, DUPLICATE_SAME_ACCESS );

    if ( !tRtn ) //fault!
    {
        return NULL;
    }
    else//sucs!
    {
        if ( tRealTrdHdl_p == NULL )
        {
            printf ( "getTrdHdl_WIN64()  !!\r\n" );
        }
        else
        {
            printf ( "no getTrdHdl_WIN64()  !!\r\n" );
        }

        return *tRealTrdHdl_p;
    }
}

//
//
////////////////windows 64位会话完成通知函数
////
//功能：由应用帧发送线程调用，会话返回时恢复调用者应用（主动会话）或系统线程（被动会话）
//输入：
//      ssnOwnerT：会话发起者线程内核句柄
//输出：无
//返回：=0
//
static int ssnFnshNfy_WIN64 ( T_TID ssnOwnerT, char* dBuf_p, int dLng ) //ssnOwnerT come from create thread
{
    int tRtn = 1;

    /*WORD tID = GetThreadId(ssnOwnerT);
    if ( !tID || !PostThreadMessage(tID, MY_MSG, (WPARAM)dBuf_p, 0) )
    {
    printf("post message failed, errno:%d \n", nsCPPub::LastError());
    }
    */
    do
    {
        tRtn = ResumeThread ( ssnOwnerT );
        //printf ( "ResumeThread %p \r\n", ssnOwnerT );
    } while ( tRtn > 1 ); //=1：resume right now!

    //  while(tRnt = ResumeThread(ssnOwnerT)>1);//=1：resume right now!
    while ( !tRtn )
    {
        nsCPPub::SleepMilliSecond ( 200 );
        tRtn = ResumeThread ( ssnOwnerT );
    }

    if ( tRtn <= 0 )
    {
        //g_TryLog.LPrintf ( DEBUG, "ResumeThread error! tRtn = %d \r\n", tRtn );
        return tRtn;
    }

    WORD tID = GetThreadId ( ssnOwnerT );

    if ( !tID || !PostThreadMessage ( tID, MY_MSG, ( WPARAM ) dBuf_p, 0 ) )
    {
        //g_TryLog.LPrintf ( DEBUG, "post message failed, errno:%d \n", nsCPPub::LastError() );
    }

    //printf("post %d %d tid=%d\n", *(int*)dBuf_p, *(int*)(dBuf_p+4), tID);
    return 0;
}

//
//
////////////////windows 64位初始化会话完成通知实体函数
////
//功能：设置会话完成通知实体指针为空，保持一linux消息队列通知方式的一致性
//输入：无
//输出：无
//返回：=0
//
static int initSsnFnshNfyBody_WIN64()
{
    //ssnFnshNfyID = 0;
    return 0;
}

//删除消息队列
static int rmSsnNfyBody_WIN64 ( int msqid )
{
    return 0;
}

//
static int ssnWaitFnsh_WIN64 ( T_TID trdID, char* dBuf_p, int dLng, MSG* msg )
{
    if ( trdID == NULL || msg == NULL )
    {
        return -1;
    }

    int iRet = SuspendThread ( trdID );

    if ( iRet < 0 )
    {
        printf ( "SuspendThread fail! tid=%p err=%d\r\n", trdID, nsCPPub::LastError() );
        return -2;
    }

    int iTry = 1000;

    while ( iTry && dBuf_p != NULL )
    {
        nsCPPub::SleepMilliSecond( 200 );
        iTry--;

        if ( GetMessage ( msg, 0, 0, 0 ) ) // get msg from message queue
        {
            if ( msg->message == MY_MSG )
            {
                //  dBuf_p = (char*)msg->wParam;
                memcpy ( dBuf_p, ( char* ) msg->wParam, dLng );
                //  printf("recv %d %d\n", *(int*)dBuf_p, *(int*)(dBuf_p+4));
                break;
            }
        }
    }

    CloseHandle ( trdID );
    //  printf("CloseHandle go on\r\n");
    //
    //  printf("recv ----\r\n");
    return iTry == 0 ? -1 : 0;
}

//
////////////////////////
#define initSsnAsynRsrc(msg)    (initSsnFnshNfyBody_WIN64())
#define rmSsnNfyBody(msqid)     (rmSsnNfyBody_WIN64(msqid))
#define getThread()     (getTrdHdl_WIN64())//返回线程的内核句柄
#define ssnWaitFnsh(trdID,dBuf_p,dLng,msg)  (ssnWaitFnsh_WIN64(trdID,dBuf_p,dLng,msg)) //windows 64位等待会话完成宏
#define ssnFnshNfy(ssnOwnerT,p1,p2) (ssnFnshNfy_WIN64(ssnOwnerT,p1,p2))//discard p1,p2
#define GetProcAddress(hModule, lpProcName) (GetProcAddress (hModule,lpProcName))


#else                   //linux 64位系统
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>
//static int            ssnFnshNfyID;       //会话完成通知(msgID)，msgID for lnx, lzq,2016.12.22
#define IPC_MSG_KEY     (key_t)0x1F2F3F4F   //for LNX

typedef struct
{
    long int msg_type;
    char* msg_p;            //use for chnlGID or timeout
} T_Msg;

//
//
////////////////Linux 64位初始化会话完成通知实体函数
////
//功能：设置会话完成通知实体指针，创建并取得IPC消息队列
//输入：无
//输出：无
//返回：>0，消息队列ID；=-1，失败
//
static int initSsnFnshNfyBody_LNX64 ( PMSG msg )
{
    int ssnFnshNfyID = 0;
    //ssnFnshNfyID = msgget ( *msg, 0666 | IPC_CREAT );
    ssnFnshNfyID = msgget ( IPC_PRIVATE, 0666  );

    if ( ssnFnshNfyID == -1 )
    {
        printf ( "%s/n", strerror ( errno ) );

    }

    return ssnFnshNfyID;
}

//删除消息队列
static int rmSsnNfyBody_LNX64 ( int msqid )
{
    int tRet = 0;
    //ssnFnshNfyID = msgget ( *msg, 0666 | IPC_CREAT );
    tRet = msgctl ( msqid,IPC_RMID, NULL );

    if ( tRet == -1 )
    {
        printf ( "%s/n", strerror ( errno ) );

    }

    return tRet;
}

//
//
////////////////linux 64位等待会话完成函数
////
//功能：调用者线程永久等待自己的发起的主动会话完成消息，若请求会话时设置了会话超时，当
//      超时发生时，由超时处理程序向消息队列发送超时消息（T_Msg.msg），从而防止该会话
//      在此函数内永久挂起，成为僵尸线程
//输入：
//      trdID：调用者的posix tID
//      dLng：调用者要接收的数据长度
//输出：
//      dBuf_p：调用者数据缓冲区指针
//返回：>0，收到的数据长度，成功；=-1，失败；-2，等待超时
//
static int ssnWaitFnsh_LNX64 ( T_TID trdID, char* dBuf_p, int dLng, MSG* msg, int ssn_type )
{
    int ssnFnshNfyID = ssn_type;
    /*
    if(ssn_type == 0)
    {
    ssnFnshNfyID = user_ssnFnshNfyID;
    }
    else
    {
    ssnFnshNfyID = err_ssnFnshNfyID;
    }
    */
    int tRtn = 0;
    //int sbmtLng = 0;
    T_Msg* tSsnFnshMsg;
    tSsnFnshMsg = (T_Msg*)calloc(1, sizeof(long) + dLng);
    if(!tSsnFnshMsg)
    	return -1;
    char* tRecMsg_p = (char*)tSsnFnshMsg+sizeof(long);
    //    tSsnFnshMsg->msg_p = (char*)tSsnFnshMsg + sizeof(long);
    //printf("wait trdID = %ld,ssnFnshNfyID = %d \r\n", trdID, user_ssnFnshNfyID);

    //tSsnFnshMsg.msg_p = dBuf_p;
	if (trdID < 0 )
	{
		trdID = 0 - trdID;
	}
	tRtn = msgrcv ( ssnFnshNfyID, ( void* )tSsnFnshMsg, dLng,  ( long int ) trdID, 0 );


    if ( tRtn > 0 )
    {
        memcpy ( dBuf_p, tRecMsg_p, dLng );
    }
	free(tSsnFnshMsg);
    //  if (tRtn != -1)
    //  {
    //      if (msgctl(*(unsigned long int*)trdID , IPC_RMID , 0)== -1)
    //      {
    //          fprintf(stderr , "msgctl(IPC_RMID) failed");
    //      }
    //  }
    return  tRtn;
}

//
//
////////////////linux 64位会话完成通知函数
////
//功能：由应用帧发送线程调用，会话返回时向主动会话请求线程发送会话完成通知
//输入：
//      trdID：通知目标线程的posix tID
//      dLng：要发送的消息数据长度
//输出：
//      dBuf_p：要发送的数据缓冲区指针
//返回：=0，成功；=-1，失败
//
static int ssnFnshNfy_LNX64 ( T_TID trdID, char* dBuf_p, int dLng, int ssn_type )
{
    int ssnFnshNfyID = ssn_type;
    /*
    if(ssn_type == 0)
    {
    ssnFnshNfyID = user_ssnFnshNfyID;
    }
    else
    {
    ssnFnshNfyID = err_ssnFnshNfyID;
    }
    printf("user_ssnFnshNfyID = %p\n", &user_ssnFnshNfyID);
    printf("err_ssnFnshNfyID = %p\n", &err_ssnFnshNfyID);
    */
    int tRtn;
    //int sbmtLng;
    T_Msg* tSsnFnshMsg;
    //long int t_id = GetThreadId(trdID);

	tSsnFnshMsg = (T_Msg*)calloc(1, sizeof(long) + dLng);
	 if(!tSsnFnshMsg)
	    	return -1;
	 char* tSndMsg_p = (char*)tSsnFnshMsg + sizeof(long);
	//tSsnFnshMsg->msg_p = (char*)tSsnFnshMsg + sizeof(long);
	tSsnFnshMsg->msg_type = ( long int )trdID;
	memcpy(tSndMsg_p, dBuf_p, dLng);
	if(ssnFnshNfyID < 0)
		ssnFnshNfyID = 0 - ssnFnshNfyID;
    tRtn = msgsnd ( ssnFnshNfyID, ( void* )tSsnFnshMsg, dLng, 0 );
	free(tSsnFnshMsg);
    //printf("tRtn = %d ,trdID = %ld,ssnFnshNfyID = %d \r\n",tRtn,trdID,ssnFnshNfyID);
    return tRtn;
}

////////////////////////
#define initSsnAsynRsrc(msg)    (initSsnFnshNfyBody_LNX64(msg))
#define rmSsnNfyBody(msqid)     (rmSsnNfyBody_LNX64(msqid))
//#define getThread()       (syscall(SYS_gettid))//??????????????????????????????????????????????????????ID??????posix ????????????ID??????????????????????????????????????????ID??????
#define getThread()     (pthread_self())//??????????????????????????????????????????????????????ID??????posix ????????????ID??????????????????????????????????????????ID??????

#define ssnWaitFnsh(trdID,dBuf_p,dLng,msg , ssn_type)   (ssnWaitFnsh_LNX64(trdID,dBuf_p,dLng , msg ,ssn_type))
#define ssnFnshNfy(trdID,dBuf_p,dLng , ssn_type)    (ssnFnshNfy_LNX64(trdID,dBuf_p,dLng , ssn_type))
#define GetProcAddress(hModule, lpProcName) (dlsym (hModule,lpProcName))
#endif



#endif  // ! H_FRAME_GLO
