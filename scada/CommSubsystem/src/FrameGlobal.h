#ifndef       H_FRAME_GLOBAL
#define H_FRAME_GLOBAL

#pragma once


//ƽ̨����
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


#ifdef WINDOWS_PLATFORM     //��WINDOWSƽ̨
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
/*                    �������������ض���                    */
/************************************************************/
typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned int    u_int;
typedef unsigned long   u_long;
typedef char*           T_ASDU_p;

/************************************************************/
/*                    �����궨��                                       */
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
    MC_SNNST_OK             = 0x00,     // �ɹ����
    MC_SNNST_ENQ_ERR        = 0x01,     // �Ự������������г���
    MC_SNNST_RES_TIMEOUT    = 0x02,     // �Ự��ʱ
    MC_SNNST_RES_FMT_ERR    = 0x03,     // ��Ӧ��ʽ��
    MC_SNNST_RES_DCHK_ERR   = 0x04,     // ��Ӧ��������δͨ��

    MC_SNNST_RUN        = 0x10,         // ����
    MC_SNNST_RDY        = 0x11,         // ����
    MC_SNNST_BLK        = 0x12,         // �ȴ�����
    //
    //  MC_SEND_WAIT_CHECK      = 0x11,         // ���͵ȴ�����
    //  MC_SEND_CNUE_EXEC       = 0x12,         // ��������ִ��
    //  MC_SEND_SSN_RETRY       = 0x13,         // ���ͻỰ����
    //  MC_SEND_SSN_FNSH        = 0x16,         // ���ͻỰ���
    //
    //  MC_NO_SEND_WAIT_CHECK       = 0x01,     // �����͵ȴ�����
    //  MC_NO_SEND_CNUE_EXEC        = 0x02,     // ����������ִ��
    //  MC_NO_SEND_SSN_STOP         = 0x04,     // �����ͻỰ��ֹ
    //  MC_NO_SEND_SSN_FNSH     = 0x06,         // �����ͻỰ���
};
enum StaValue
{
    MC_ACT_FLAG = 0,        // ������ʶֵ
    MC_PSV_FLAG = 1,        // ������ʶֵ
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
// �붨ʱ������״̬ͳһ�����޸ģ�
//#define TIMER_NOP   0       //��ʱ���ղ���������ԭ״̬������ͣ�ָ���
//#define TIMER_START 1       //��ʱ����������
//#define TIMER_STOP  2       //��ʱ����ֵ���������¸�λ
//#define TIMER_PAUSE 3       //��ʱ����ͣ����

#define DEF_ERROR_SSN_NO    0xFFFF

/************************************************************/
/*                       �������Ͷ���                       */
/************************************************************/
#pragma pack (1)
typedef struct
{
    u_char B_Confidence; //���ݿ��Ŷ�
    u_char B_Interval;   //���ݵ�ʱ����
    u_char B_Height;     //���ݵ����ڸ߶�
    u_char B_Source;     //������Դ
    u_char B_Kind;       //��������
    u_char B_Length;     //��������ֽڳ���
    u_char B_Amount;     //���ֽ�������Ϊ���ݼ�ʱ��Ч���������ݼ���Ԫ�صĸ���
    u_short  W_Encoding; //���������豸���
    u_char B_Type;       // ��������
    u_char B_Catalog;    //���ݾ���Ĵ���࣬�����ļ���sql����������ݼ�
} DATA_ID;
#pragma pack()
typedef struct
{
    u_int   ASDU_TID;
    u_int   InfoEleLng;         //��ϢԪ�ؼ���ÿ����ϢԪ�ص��ֽڳ���
    u_short ASDU_CMTBL [11];
} APDU_DSCR;

class C_StnObj;
class C_SsnObj;
class C_DStorage;

// �Ự����ṹ���壬32λλ��
struct snnResult
{
    // ...
};
// ��Ϣ���2��
struct Msg_SnnFnsh
{
    long msgTp;     // ��Ϣ���ͣ�=��long)tid, tidΪ�̵߳Ľ����ڱ�ʶ
    char msgTxt[sizeof ( struct snnResult )]; // ��Ϣ����Ϊ�Ự���
};


typedef ELE_TYPE ASDU_TID;      //���ͱ�ʶ
typedef ELE_TYPE ASDU_LNG;      //ASDU����
typedef ELE_TYPE ASDU_VSQ;      //�ɱ�ṹ�޶���
typedef ELE_TYPE ASDU_COT;      //����ԭ��
typedef ELE_TYPE ASDU_ADDR;     //ASDU������ַ
typedef ELE_TYPE BODY_TID;      //��Ϣ������
typedef ELE_TYPE BODY_ADDR;     //��Ϣ���ַ
typedef ELE_TYPE BODY_SET;      //��ϢԪ�ؼ�
typedef ELE_TYPE BODY_TSP;      //��Ϣ��ʱ��
typedef ELE_TYPE ASDU_TSP;      //ASDUʱ��

typedef u_char GEN_FRM[4];
typedef struct
{
    u_char startChar;//�����ַ���= SRTCHAR_F1_2_V�������ĵ�2.2.5����
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
/*                �����ڻ�������101��104��Լ���ж���               */
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


struct ssnSets              // �Ự���ṹ��
{
    u_short     ssnNo;      // �Ự��
    ptclStep*   stepDsc_p;  // ����
    u_char      setNum;     //ptclStep����
    u_char      actFlag;    // ������������
};

//�û�����и����ͨ��������Ϣ
struct baseInfo
{
    unsigned short pntEndian    : 1 ;//=0��С�ˣ�=1���
    unsigned short pntGrpEndian : 1 ;//=0��С�ˣ�=1���
    unsigned short pntType      : 4 ;//���ݱ�ʾ��ʽ������������С����˫��С�����ַ����ַ���
    unsigned short startBitNo   : 5 ;//��Ч��ʼλ��λ�ŷ�Χ0~31
    unsigned short pntBitWidth  : 5 ;//�˵����Чλ��=pntBitWidth+1
};

struct pntInfo//��12�ֽ�
{
    baseInfo pntBaseInfo      ; //2B
    unsigned short stnCmnAddr ; //2B
    unsigned short stnSubAddr ; //2B
    unsigned short pntAddr    ; //ȫ�ֵ�ַ=stnCmnAddr:stnSubAddr:pntAddr
    unsigned short scale_pre  ; //ϵ��������ֵ=pntֵ*cof,float cof = xml::pntDcrb.scale_pre
    unsigned short scale_pst  ; //
};

struct subSysAppDItemValDcrb
{
    unsigned char dItemID[DATA_ID_SIZE];
    unsigned char dTypeAndLng;
    unsigned short SMDItemStgPos;//ͨ������ϵͳSM�е�ƫ�ƶ�λsubSysAppDItemStg
    unsigned char strMaxLng;//�ַ�����󳤶ȣ�=0��ʾ��str���ͣ������к�'\0'
    pntInfo matchPnt;
};

enum DTYP_LNG
{
    SHORT_INT = 0x04,//4B����
    LONG_INT = 0x18,//8B����
    SGL_PRCN = 0x24,//����
    DB_PRCN = 0x38,//˫��
    DATE_TM = 0x47,//����Ϊ�������ʱ��
    ONE_CHAR = 0x51,//1���ַ�
    TWO_CHARS = 0x52,//2���ַ�
    THREE_CHARS = 0x53,//3���ַ�
    FOUR_CHARS = 0x54,
    FIVE_CHARS = 0x55,
    SIX_CHARS = 0x56,
    SEVEN_CHARS = 0x57,
    EIGHT_CHARS = 0x58,//8���ַ�
    STR_PNTR = 0x6F     //F��ʾ����--���ݻ������ͣ�=sizeof(void*)���ַ���������'\0'����
};


// typedef struct
// {
//  ssnCtlBlk*  ssnInst_p;      // �Ựʵ�����ƿ�
//  int*        ssnRslt_p;      // �Ự������������Ự����ʱʹ�ã��õ�ַ���浽�Ự��������ͨ�����飩��Ϣ����
// }ssnInstRslt;
typedef int ssnInstRslt;

typedef void ( *FunType ) ();
typedef void ( *FunTypeOne ) ( void* );

//
// timer
typedef struct
{
    u_char ID;          // ��ʱ����,��ʱ��ͨ���ڱ��
    char* name_p;       // ��ʱ������ascii string.
    u_char type;        // ���ڱ�ʶ
    u_int fstXpirTm;    // ��һ�ε�ʱʱ��, relative to start time
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
    unsigned int clkCntr;       // ʱ��Դ������
    FILETIME    locFileTm;      // ϵͳ�����ı����ļ�ʱ��
};
enum tmType
{
    TM_ONCE = 0,
    TM_PERIOD = 1
};
// ��ʵʱ��ʱ��1
struct tmr
{
    u_char ID;          // ��ʱ����,��ʱ��ͨ���ڱ��
    char* name_p;       // ��ʱ������ascii string.

    struct softTmChnl* tmChnl_p;    // ���Ӷ�ʱͨ��ָ��

    //=0, uninstall(from init and stop), =1, install(from addSoftTm);
    //=2, start(from install and stop), =5, stop(from start and goOn, =reset=pause+reload);
    //=3, pause(from start and goOn), =4, goOn(from pause);
    u_char wkState;
    u_char type;    // ���ڱ�ʶ. =1��3 period, =0��2 once
    u_int startTm;  // ����ʱ��relative to start up
    u_int xpirTm;   // ��ʱʱ��relative to start up

    tmr*    sortTrgrNxt_p;      // ��һ��ʱ��ָ�루��ʱ��������
    tmr*    sortTrgrPre_p;      // ��һ��ʱ��ָ�루��ʱ��������
    void*   tmFunHdlPara_p;     //�ص���������ָ��
};

// ��ʵʱ��ʱ��ͨ��
typedef struct softTmChnl
{
    u_short tmChnlID;           // ��ʱͨ����
    u_char state;
    u_char  tmrNum;             //tm num in chnl
    tmr* tmrTblHd_p;            //timer table head, =tmrTbl[tmrNum], idx =0: T0,...,=i: Ti, ...
    FunTypeOne* tmFunHdl_p;     //sTm expire handler. =tmFunHdl[tmrNum], idx =0: T0,...,=i: Ti, ...
} tmChnl;
// timer

// channel
// ͨ�����飩��Ϣ��
struct chnlInfoBlk
{
        chnlInfoBlk*    preBlk_p;           // ǰ����
        u_short         chnlNo;             // ͨ�����飩��
        u_char          ssnRlMode;          // �Ự��������ģʽ
        T_snnCfg*       ssnStnCnfg_p;       // �Ựվ���ã���Ự�������õ�
        int         psvSsnRprtInfo;         // �����Ự���������Ϣ��=ϵͳר�õ�ַ

        //ASDU*         actASDU_p;
        //ASDU*         psvASDU_p;
        APDU*           respAPDU_Obj_p;
        APDU*           reqAPDU_Obj_p;
        // asdu��س��ȣ��ڻỰ�������У�������ʹ��һ��

        FUN_RL*         funRl_p;            // ������ ע��Ŀǰʹ�ù�Լ�ⴴ���ù����Ժ�ʹ��������ϵͳ���ã�ֱ�Ӷ�ȡ��

        chnlInfoBlk*    postBlk_p;          // ��̿�
    public:
        chnlInfoBlk()
        {
            memset ( this, 0, sizeof ( chnlInfoBlk ) );
        }
};
// channel
// ssn
// �Ự�������ṹ
typedef struct
{
    u_char          ssnNo;              // �Ự��
    FunTypeOne      startSsnInst_p;     // ��Լ��Э����ṩ������ָ�����Ϳ⺯���ӿ�ȷ�Ϻ����
    FunTypeOne      driveSsnInst_p;     // ��Լ��Э����ṩ������ָ�����Ϳ⺯���ӿ�ȷ�Ϻ����
    FunTypeOne      chkSsnInst_p;       // ��Լ��Э����ṩ������ָ�����Ϳ⺯���ӿ�ȷ�Ϻ����
    FunTypeOne      usrCallback_p;      // �Ự����ʱ�û��ص������ӿ�
    //  ASDU*           ASDU_p;             // �Ựʵ��ʹ�õ�ASDUָ��
    chnlInfoBlk*    chnlInfoHead_p;     // ͨ�����飩��Ϣ������ͷָ��
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



// �Ựʵ�����ƿ�ṹ
struct ssnCtlBlk
{
    //�Ựʵ�����ƿ黥����(��ֹͬһʵ�����ƿ�ͬʱ�ڶ���߳�ͬʱ����)@add@20180710131457
    //nsCPLock::CLock ssnCtlLock;
    
    // ����Ϊ����ʱ�Ự������Ϣ����
    u_char      role;           // ��ɫ
    u_long      ssnNo;          // �Ự��
    u_short     chnlID;         // ͨ�����飩��
    T_TID       hostTID;        // �û���ϵͳ�߳�ID��linux�� ʹ�ý����ڵ��߳�ID
    u_short     ssnTmout;       // �Ự��ʱʱ��
    tmChnl*     ssnTmHead_p;    // ��ʱӦ��ͨ��
    u_char      retryNum;       // �Ựʧ�ܺ�ĳ��Դ�������ֵ0���Ự��������֮һ

    // ����Ϊ����ʱ��ܸ����������龳�ṩ����Ϣ����
    T_snnPara*      ssnPara_p;          // �Ự����ָ�룬Э����Ӧ�÷��Ϳ�ܼ��˫�򻺴棬������usrSsnData��usrSsnFileData����
    u_char          actFlg;             // �����Ա�ʶ��Ӧ��֡�����̴߳�����Ϊ������������ϵͳ������Ϊ����
    u_char          actASDU_lng;        // �Ựʵ����ASDU���� // ע���ݲ�ʹ�ã���Ŀǰ��apdu����ָ�룬������������ؼ�¼��by zhaojj��20170302��
    u_char          psvASDU_lng;        // �Ựʵ����ASDU����

    // ���´����Ựʵ�����̲�������Ϣ����
    chnlInfoBlk*    myChnlInfoBlk_p;    // ͨ������Ϣ��ָ��
    ssnDsc*         ssnDsc_p;           // �Ự������ָ��

    // ����Ϊʵʱ���Ʋ��֣����Ͳ����
    u_char          drvStepNo;      // ��ֵ0,���ղ෵�أ�����ָʾ���Ͳ�װ�����ݣ�0xFF��ʾ������
    ONE_STEP*       drvPrmtr_p;     // ������������ֵNULL(�Ự����) // ����ʵ��ͬʱ����ʼ�����ֶ�
    u_char          funCtlCd;       // ���ܿ�����
    u_short         checkStd;       // ���ձ�׼����ֵ0

    u_char          ptclCtlCd;      // Э�������--��ʶ I ��U ֡

    // ����Ϊʵʱ��Ϣ���֣����ղ����
    int             checkState;     // ����״̬�룺��ֵ0
    u_char          funCtlAck;      // ����ȷ��
    //  void*           stepCntxt_p;    // ������Ϣ

    // ����Ϊʵʱ��Ϣ���֣����Ͳ�ͽ��ղ������
    u_char          ssnTmrCtlCd;        // �Ự��ʱ��������
    u_char          rtRunStOrRslt;      // ʵʱ����״̬����ֵΪ��ɣ������Ự����ʱΪ��ŻỰ���������Ϣ

    // ����Ϊ�����Ự����ʱ�ĻỰ���������Ϣ���֣����Ͳ�ĻỰ�����鸺�𽫽����rtRunStOrRslt��д��õ�ַ
    ssnInstRslt     actSsnRprtInfo;     // �����Ự���������Ϣ��=&(snnInfo.ssnRslt)
    ssnCtlBlk*      nxtSsnCtlBlk_p;     // ��һʵ�����ƿ�ָ�루�Ự����ʹ�ã�
    ssnCtlBlk*      preSsnCtlBlk_p;     // ��һʵ�����ƿ�ָ�루�Ự����ʹ�ã�

    ssnCtlBlk*      nxtChnlSCBlk_p;     // ��һʵ�����ƿ�ָ�루ͨ������ʹ�ã�
};


//
// typedef union
// {
//  snnPar*         snnStartPar_p;      // �Ự������������Ҫ�޸�ASDU�洢ֵ�����Ǵ����µ�ASDU�ռ�ʱʹ��
//  ssnInstRslt     ssnRslt;            // �Ự�����
//  APDU*           ASDU_p;             // ʹ�õ�ASDUָ�룬����Ҫ�޸�ASDU�洢ֵʱʹ��
// }snnInfo;

// ���������ͣ�Ŀ����Ϊ�˼��ٴ��ε�����
typedef struct
{
    u_char  role;
    u_short chnlID;
    u_char  snnMaxRetryNum;         // �Ự������Դ�����������=0
    void*   infoObjchkStd_p;        // ��Ϣ�������ձ�׼����ʵ������
} reqInfo;

// move to other file
// �Ự����
// typedef struct
// {
//  u_long      ptcNo;          // Э���
//  void*       fHandle;        // �ļ������������
//  char*       snnDsc;         // �Ự���������ڴ洢Ϊÿ���Ự�����KEY����ֹ�ظ�����
//  u_char      prio;           // �Ự���ȼ�
//  C_SnnObj*   ssnObj_p;       // �Ự����ָ��
// }snnTblItem;

// �Ự����
typedef struct
{
    u_char      ptclNo;     // 104��101...
    u_char      snnDscNo;   // Э��ţ�վ�ٻ�
    u_char      prio;       // ���ȼ�
} snnAppCfg;

////�Ự�ؼ���
//���ڷ�ֹ�ظ�����Ự����
typedef struct
{
    u_char ptclNo;          //Э���
    u_char snnDscNo;     //Э���ڲ�Ψһ��ʶ�ĻỰ��
} ptclSnnKey;



// �Ự����
typedef struct
{
    u_char      usingFlg;       //���ñ�ʶ��=0�����У�=1������
    ptclSnnKey      ssnKey;
    HMODULE     fHdl;           // �ļ������������ָ��
    u_char      prio;           // �Ự���ȼ�
    C_SsnObj*   ssnObj_p;       // �Ự����ָ��
} snnTblItem;

// �Ự��
typedef struct
{
    u_int       ssnNum;         // ���ڴ��ڻỰ����
    snnTblItem* snnTblEntry_p;  // �������飬������=ssnNum
} snnTbl;
// move to other file

// ASDU����������е����ݵ�Ԫ�ṹ
typedef struct
{
    ASDU*       ASDU_p;         // ASDUָ��
    u_short     ASDU_lng;       // ASDU����
    u_short     chnlID;         // ͨ�����飩��
    ssnDsc*     snnDsc_p;       // ������ƹ���KEY
    u_char      ssnRlMode;      // �Ự��������ģʽ
    u_char      ssnAct;         // ������Ʒ�ʽ
    u_char      ssnStepNo;      // ������ƽ׶κ�
} qASDUSndReqUnit; // ע��20170228,��ʱ���ã�(by zhaojj)

// �Ự��������
struct qSsnReqUint
{
        u_char  role;           // ��ɫ
        T_TID   tID;            // �������߳�ID
        u_short chnlID;         // ͨ�����
        u_short ssnID;          // �Ự��
        T_snnPara*  snnPara_p;  // �Ự��������ָ��
		u_short  addrStart;//���ַ��ʼ��ַ��Modbus���ӣ�
		u_short  accumValue;//��ĸ��� ��Modbus���ӣ�
        u_short timeout;        // �Ự��ʱʱ��
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

// ���ն����ݻ�����--ͨ�����飩��������һ����
union revBuf
{
    struct
    {
        unsigned short  APDU_Lng;       // ���ݳ���
        unsigned char*  APDU_Data_p;    // ����ͷָ��
    } APDU_Buf;

    struct
    {
        unsigned short  ASDU_Lng;
        unsigned char*  ASDU_Data_p;
    } ASDU_Buf;
};

// �������֡
struct transCtrlF
{
    unsigned char       fLng;       // ֡��
    char                ctrlType;   // ����֡����
    unsigned char*      fData_p;    // ֡����ָ��
};

// ���������Ϣ���е����ݵ�Ԫ�ṹ
typedef struct
{
    // ����Э������Ĵ������״̬����ͣ���͡��ָ����͡��������ͣ���ʼ״̬����ֹͣ���ͣ�ɾ����ֹͣͨ�����飩ǰ��������״̬
    u_char  tranCtrlState;
    u_char  ctrlType;       // ����֡����
    u_char  frmLng;         // ����֡֡��
    char*   ctrlFrm_p;      // ֡����ָ��
    void*   tmStamp_p;      // Э������ʱ�� // ��ʱ����Ҫ��������20170411
} transCtrlFrmDsc;

// ���������Ϣ���е����ݵ�Ԫ�ṹ
typedef struct
{
    u_short         chanlID;    // ͨ�����飩��
    transCtrlFrmDsc ctrlFrmDsc; // �������֡���������ʱ*ctrlFrm_p��Ҫ���з���ռ��γɿ���֡����
} qTransCtrlInfoUnit;

// ASDU ��Ϣ���յ�Ԫ
typedef struct
{
    u_short chanlID;        // ͨ�����飩��
    APDU*   ASDU_p;         // ASDUָ��
    u_short ASDU_Lng;       // αASDU����,Ϊ�˿��ƻػ�������֡��ʱ�򳤶�Ϊ0
    u_short realData_lng;    //��ʵASDU����
    u_char  DU_type;        // ����
} qASDUInfoRcvUnit;

// ASDU ��Ϣ���������ṹ
typedef struct
{
    u_short infObjLng;
    u_char* infObjSet_p;
} ASDU_InfoObjDsc;

// ��ʱ��ʹ�ã���ɾ��
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

// �Ựʵ�����ͨ����
struct ssnItem
{
    u_char      ucRole;         // ��ɫ
    u_char      ucFlag;         // �Ự���ƿ����ͣ��������漸���ֶ�ֵ����Ч�ԣ�(����������Чλ��Ӧ����ֵ����Ӧλ)

    ssnCtlBlk*  ssnCtlBlk_p;    // �ǹ���Ựʵ�����ƿ�����ָ�� / �� / ����/��������ʵ���Ự���ƿ�
    u_short     usNum;          // �ǹ���Ựʵ�����ƿ�����������
    void*       actDataSwap_p;  // �ǹ���Ựʵ�����ݽ�������������

    ssnCtlBlk*  psvSsn_p;       // �����Ự���ƿ�ָ��
    void*       psvDataSwap_p;  // �����Ự���ݽ�����
};

// �Ựʵ����
struct ssnTbl
{
    ssnItem     ssnTblEntry[MC_DEF_MAX_PRIO];
};

class C_ChnlObj;

// ͨ�����飩����
struct chnlItem
{
    u_short     stnID;      // Զ��վ��
    // ...
    C_ChnlObj*  chnlObj;    // ͨ�����飩����ָ��
};

// ͨ�����飩�����
struct chnlTbl
{
    u_short chnlNum;            // ͨ�����飩����
    chnlItem*   chnlTblEntry_p; // ��������
};

typedef void ( *FunType ) ();

typedef void ( *FunTypeOne ) ( void* );

typedef struct lnkTmChnl lnkTmChnl;

// ��ʵʱ��ʱ��
struct softTimer
{
    u_short usID;           // ��ʱ����
    u_char  ucName;         // ��ʱ������

    lnkTmChnl*  lnkChnl_p;  // ���Ӷ�ʱͨ��ָ��

    u_char  ucStartFlag;    // ������־
    u_short usCircleFlag;   // ���ڱ�ʶ
    u_short usStartTime;    // ����ʱ��
    u_short usTotalTime;    // ��ʱʱ��

    softTimer*  stOrdNext_p;    // ��һ��ʱ��ָ�루��ʱ��������
    softTimer*  stOrdPre_p;     // ��һ��ʱ��ָ�루��ʱ��������
    softTimer*  stChnlNext_p;   // ��һ��ʱ��ָ�루��ʱͨ������
};

// ��ʵʱ��ʱ��
struct lnkTmChnl
{
    u_short usLnkID;        // ���Ӷ�ʱͨ����
    lnkTmChnl* ltcNext_p;   // ��һ���Ӷ�ʱͨ��ָ��
    softTimer* tmHead_p;    // ��ʱ��ָ���
    softTimer* stTail_p;    // βָ��

    void    ( *tmFunT0[MC_DEF_SOFT_TIME_NUM] ) (); // T0��ʱ�¼������������
    void    ( *tmFunT1 ) (); // T1��ʱ�¼������������
    void    ( *tmFunT2 ) (); // T2��ʱ�¼������������
    void    ( *tmFunT3 ) (); // T3��ʱ�¼������������
};

// Զ��վ����
typedef struct
{
    u_long      stnAddr;    //Զ��վͨ�ŵ�ַ��IP/���������豸�ţ������ֽ�˳��
    u_short     stnAppID;   //Զ��վӦ�ñ�ʶ����port/�����豸�����ʣ������ֽ�˳��
    u_short     stnID;      //Զ��վ��
    u_char      role;       //Զ��վ��ɫ
    u_short     localPort;  //���ض˿�
    u_long      localAddr;  //Զ��վͨ�ŵ�ַ��IP/���������豸�ţ������ֽ�˳��
} stnOrVew;

// ͨ�����飩����
typedef struct
{
    stnOrVew    stnOverView;    // վ����
    C_ChnlObj*  chnlObj_p;      // ͨ�����飩����ָ��
    u_short     chnlState;      // ͨ��״̬
} chnlTblItem;


struct addrItem
{
    u_int   uiAddr;     // ͨ�ŵ�ַ��ip��ַ���е�ַ��
    u_short usMask;     // ͨ�ű�ʶ���˿ڻ򴮿ڣ�
};

struct chnlDsc
{
    short   sChnlNo;            // ͨ����
    char*   chnlName_p;         // ͨ����ָ��
    char    cRole;              // ��ɫ
    void*   locOpr_p;           // ͨ���������

    ssnSets*    locSsnInfo_p;   // ����֧�ֵĻỰ����Ϣָ��
    short       numLocSsn;

    void*   tranCtrl_p;         // ����������ò���ָ��
    void*   realTranCtrl_p;     // �������ʵʱ����ָ��

    // ������Ʋ㳬ʱʱ���顢��ʱ��Դ�Ͷ�ʱ��������
    u_short usOutTmT0a[MC_DEF_SOFT_TIME_NUM];   // ��ʱʱ��t0~t3��ta
    char    tmCtrlCode[MC_DEF_SOFT_TIME_NUM];   // ��ʱ��������
    tmChnl* tmChnl_p;                           // ��ʱͨ��ָ��

    // ... // ������Ʋ�֡�ռ�������αASDU�ռ�����
    char* tranCtrlBuf_p;

    ssnTbl* ssnTbl_p;           // �Ựʵ����ָ��

    revBuf  revBuf_p[MC_DEF_BUFF_NUM];      // ���ն����ݻ���ָ��
    revBuf  sendBuf_p[MC_DEF_BUFF_NUM];     // ���Ͷ����ݻ���ָ��

    C_StnObj*   stnObj_p;       // Զ��վ����ָ��
    void*       rtnFun_p;       // �ص�����ָ��

    addrItem    localAddr;      // ����վ��ַ
    HMODULE     ptclMdl;        // ��Լ����
    HMODULE     commMdl;        //����ͨѶЭ��������
};

// Զ��վ������
struct stnDsc
{
        char    stnRole;        // Զ��վ��ɫ
        short   stnID;          // Զ��վ��
        char*   stnName_p;      // Զ��վ��ָ��
        short   chnlID;         // ͨ�����飩��

        void*   stnSsnInfo_p;   // Զ��վ֧�ֵĻỰ����Ϣ

        stnDsc* nxtLgcStn_p;        // �߼�վ����ָ��
        void*   infoTbl_p;          // ��Ϣ�������ָ��

        u_char  cleTranFlag;        // ���ں�ͻ�������ʶ

        void*   strgMap_p;          // �洢ӳ�����Ǹ�������
        void*   jumpFlw_p;          // ������ٻ���ָ��

        stnDsc* nxtStn_p;           // ��һԶ��վ����ָ��
        // ��ʱ����

        FunTypeOne  callBackFun;    // ��Ϣ��������ص�����ָ��
        FunTypeOne  rcdExpFun;      // ��Ϣ���������¼����ָ��

        C_DStorage* gStorageObj_p;  // ���״洢����ָ�루Ԫ�����ͣ�
        C_DStorage* fStorageObj_p;  // ���״洢����ָ�루�ļ����ͣ�
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
			gStorageObj_p = NULL;  // ���״洢����ָ�루Ԫ�����ͣ�
			fStorageObj_p = NULL;  // ���״洢����ָ�루�ļ����ͣ�
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
    u_char cstmFlag;        // ���Ʊ�ʶ
    void* tmObj_p;          // ��ʱ��Դ����ָ��
    u_char ucSoleFlag;      // ��ɫ��ʶ
    void* ctrlLocAddrTab_p; // ����վ��ڣ����ض˵�ַ��
    u_int ctrlUseChnl;      // ����ͨ�����飩��
    void* chargStnObj_p;    // Զ�̱���վ��������ͷָ��
    void* locAddrTbl_p;     // ģ�ⱻ��վ��ڣ����ض˵�ַ��
    u_int useChnl;          // ����ͨ�����飩��
    void* ctrlStnObj_p;     // Զ�̿���վ��������ͷָ��
    void* monitor_p;        // �������
};

struct chnlMgrTab
{
    u_int stnNum;           // Զ��վ��
    u_char stnAddr[16];     // Զ��վ��ַ
    u_int stnAppFlag;       // Զ��վӦ�ñ�ʶ
    u_char state;           // ״̬���
    void* channelObj_p;     // ͨ������ָ��
};
//////////////////////////////////////////////////////////////////////////
/*dll's fun**/
typedef u_char* ( *T_createAPDU ) ( APDU_DSCR* cstmInfo_p, u_char duType, u_char  allocFlag );
typedef FUN_RL* ( *T_createRl_P ) ( u_char srvNo, u_char usingMode, T_snnCfg* snnCfg_p, T_ptclFunDsc* ptclFunDsc );

////////Ӧ��֡�����߳�
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
/*                 ƽ̨��ز��֣���̬����                    */
/************************************************************/
//
#ifdef WINDOWS_PLATFORM         //windows 64λϵͳ
#include <WinNT.h>
#include <process.h>
#include <windows.h>

//static int            ssnFnshNfyID;       //�Ự���֪ͨ(msgID)��msgID for lnx, lzq,2016.12.22

#define MY_MSG      WM_USER+100
////////////////windows 64λ�õ��߳��ں˾������
////
//���ܣ�ͨ����ǰ�߳�ID�õ��̵߳��ں˾������α�����
//���룺��
//�������
//���أ�=��ǰ�߳��ں˾�����ɹ���=NULL��ʧ��
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
////////////////windows 64λ�Ự���֪ͨ����
////
//���ܣ���Ӧ��֡�����̵߳��ã��Ự����ʱ�ָ�������Ӧ�ã������Ự����ϵͳ�̣߳������Ự��
//���룺
//      ssnOwnerT���Ự�������߳��ں˾��
//�������
//���أ�=0
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
    } while ( tRtn > 1 ); //=1��resume right now!

    //  while(tRnt = ResumeThread(ssnOwnerT)>1);//=1��resume right now!
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
////////////////windows 64λ��ʼ���Ự���֪ͨʵ�庯��
////
//���ܣ����ûỰ���֪ͨʵ��ָ��Ϊ�գ�����һlinux��Ϣ����֪ͨ��ʽ��һ����
//���룺��
//�������
//���أ�=0
//
static int initSsnFnshNfyBody_WIN64()
{
    //ssnFnshNfyID = 0;
    return 0;
}

//ɾ����Ϣ����
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
#define getThread()     (getTrdHdl_WIN64())//�����̵߳��ں˾��
#define ssnWaitFnsh(trdID,dBuf_p,dLng,msg)  (ssnWaitFnsh_WIN64(trdID,dBuf_p,dLng,msg)) //windows 64λ�ȴ��Ự��ɺ�
#define ssnFnshNfy(ssnOwnerT,p1,p2) (ssnFnshNfy_WIN64(ssnOwnerT,p1,p2))//discard p1,p2
#define GetProcAddress(hModule, lpProcName) (GetProcAddress (hModule,lpProcName))


#else                   //linux 64λϵͳ
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>
//static int            ssnFnshNfyID;       //�Ự���֪ͨ(msgID)��msgID for lnx, lzq,2016.12.22
#define IPC_MSG_KEY     (key_t)0x1F2F3F4F   //for LNX

typedef struct
{
    long int msg_type;
    char* msg_p;            //use for chnlGID or timeout
} T_Msg;

//
//
////////////////Linux 64λ��ʼ���Ự���֪ͨʵ�庯��
////
//���ܣ����ûỰ���֪ͨʵ��ָ�룬������ȡ��IPC��Ϣ����
//���룺��
//�������
//���أ�>0����Ϣ����ID��=-1��ʧ��
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

//ɾ����Ϣ����
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
////////////////linux 64λ�ȴ��Ự��ɺ���
////
//���ܣ��������߳����õȴ��Լ��ķ���������Ự�����Ϣ��������Ựʱ�����˻Ự��ʱ����
//      ��ʱ����ʱ���ɳ�ʱ�����������Ϣ���з��ͳ�ʱ��Ϣ��T_Msg.msg�����Ӷ���ֹ�ûỰ
//      �ڴ˺��������ù��𣬳�Ϊ��ʬ�߳�
//���룺
//      trdID�������ߵ�posix tID
//      dLng��������Ҫ���յ����ݳ���
//�����
//      dBuf_p�����������ݻ�����ָ��
//���أ�>0���յ������ݳ��ȣ��ɹ���=-1��ʧ�ܣ�-2���ȴ���ʱ
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
////////////////linux 64λ�Ự���֪ͨ����
////
//���ܣ���Ӧ��֡�����̵߳��ã��Ự����ʱ�������Ự�����̷߳��ͻỰ���֪ͨ
//���룺
//      trdID��֪ͨĿ���̵߳�posix tID
//      dLng��Ҫ���͵���Ϣ���ݳ���
//�����
//      dBuf_p��Ҫ���͵����ݻ�����ָ��
//���أ�=0���ɹ���=-1��ʧ��
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
