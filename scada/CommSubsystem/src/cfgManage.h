//
// Created by 张赟亮 on 2018/1/29.
//
#ifndef CFG_MANAGE
#define CFG_MANAGE

#pragma once

#include <stdio.h>
#include <cstring>
#include "data.h"
#include "sysDefine.h"

#include "ptcl/MACRO.h"
//#include "ptcl/DataType.h"
//#include "FrameGlobal.h"
#include "DataBase.h"
#include"unitInfMem.h"
#include "CPShared/CPLog.h"
#include "IvBus.h"

#define NOT_SURE 0

/****************************************************************************************************************/
#define MCT_SSN_NUM      20      // 支持的会话个数
#define MC_DEF_STN_NUM   4       // 远程站默认数，注意要小于前面相应队列对象中队列数（MC_DEF_Q_NUM）
#define MC_IP_LEN        16      //ip地址长度
#define TIMER_PUT_DB_NUM 3       //入库时间间隔的个数如:1分钟 5分钟 15分钟
#define MC_TMSTMP_OFFSET 0       //计算入库本地时间戳的偏移量:如提前则为正,延后为负值
#define MC_TMSTMP_OUTOFFSET -60    //计算出库本地时间戳的偏移量:如提前则为正,延后为负值
#define MC_DIDBIN_LEN    64      //数据id导出二进制串最大长度

#define MC_TMPSTR_LEN   200     //配置文件里临时字符串长度

#define MC_DBBUF_MAX    1024*32*10

//解析xml报错定义
#define MC_ERROR_XMLPHY_DUP  -200 //xml中一个通道有重复的phyType值
#define MC_ERROR_CALLOC      -999       //calloc分配空间错误
#define MC_ERROR_INPAR       -898       //接口入参错误
#define MC_ERROR_XMLELE      -800       //xml关键结点或元素没有配置
#define MC_ERROR_XMLLOAD     -799       //加载xml文件错误
#define MC_ERROR_STRLEN      -798       //配置文件的字符串长度超过,需要扩展 MC_TMPSTR_LEN 的值

//xml配置文件路径定义
//#define CFG_FILE_PATH  "../cfg/unitMemInit.xml"  
#define CFG_FILE_PATH  "/zfmd/wpfs20/scada/unitMemInit.xml"
//系统日志文件路径定义
//#define SYS_LOG_PATH  "../trylog/"
#define SYS_LOG_PATH  "trylog/"


//定义调用vBus //调用则定义否则不定义
//#define RUN_VBUS_FLAG

#define VBUS_SCADA_APPID 0  //appid
#define VBUS_SCADA_HOSTID 0  //hostid



typedef struct
{
	char ipAddr[MC_IP_LEN];
	int  ipPort;
} T_ipVal;

typedef struct
{
	T_ipVal lcalDataInIp;
	T_ipVal rmtDataInIp; 
	T_ipVal lcalDataOutIp;
	T_ipVal rmtDataOutIp;
} T_lcalIps;

//可变结构限定词（ASDU_VSQ）的取值类型定义：用于ASDU_VSQ元素语义解析
typedef struct
{
    u_char N: 7;
    u_char SQ: 1;
} ASDU_VSQ_VAL;                 

#pragma pack(1)
typedef union
{
    u_char shortAddr;
    u_short shortlngAddr;
    struct HIACH_ADDR
    {
        u_short childAddr;
        u_char fatherAddr;
    } hiachAddr;
} BODY_ADDR_VAL;
#pragma pack()

//MBAP报文头 7字节
typedef struct
{
    u_short trans_id; //Transaction Identifier
    u_short  pctl_id; //Protocol Identifier
    u_short   length; //Length:Number of following bytes
    u_char   unit_id; //Unit Identifier
} ADU_MBAP_VAL;

typedef struct
{
    bool collMethods;//标示这个会话是否需要反复发送 true：需要反复发送，false：不需要反复发送
    u_short  cycTm;  //周期发送的时间频率
    ptclSnnKey ssn;
    //以下两项为modbus使用，104其他回话有可能使用
    u_short  addrStart;//点地址起始地址
    u_short  accumValue;//点的个数
} ssnReqCnt;

typedef struct
{
    int cntNum;
    reqInfo tReqInfo;
    ssnReqCnt* sRCnt_p;
} ssnReqInfo;

//通道相关配置
typedef struct
{
    u_char     stage;           //是否需要激活。=0：需要激活，=1：不需要激活
    char       putStagFlag;     //是否入库
    short      role;            // 本站角色
    short      stnRole ;        // 远程站角色
    char       szStnIp[16];     //远程站IP地址
    char       localszStnIp[16];//需要绑定的本地IP地址
    int        szStnPort;       //远程站端口号
    int        localPort;       //需要绑定胡本地IP地址
    int        ptclMdlNum;      //协议号
    int        bufLng;          //数据缓冲区的最大长度
    u_short    cfgLen;          //配置的点表个数,cfgAddr_p数组的长度
    u_char     usrType;         // 数据类型，0--元素集，1--文件
    APDU_DSCR  tmpApduDscr;
    addrRange* cfgAddr_p;       // 点表ADDR索引，<序号，addrRange>
    //T_initChnUnit inPar;
    u_int      phyTypeNum;      //物理量个数
    PHY_UNIT_TYPE*  phyType_p;  //物理量类型
    ssnReqInfo ssnRInfo;
    u_char     pntTmStmpLng;    //协议点的时标长度(一个通道设置一个最大值)
} chnlObjCfg;     





//using namespace tinyxml2;

class cfgManage
{
    public:
        cfgManage ( u_char Num );
        ~cfgManage();

    public:
        //日志类定义
        nsCPLog::CDailyLog tryLog;
        nsCPLog::CDailyLog tclLog;//传输控制日志
		 nsCPLog::CDailyLog didLog;//传输控制日志
    public:
        /*为每个会话创建会话步规则配置表
        * ssnNum:会话个数
        *
        * */
        void crtStmpCfg ( char snnNum );

        void crtFunDsc ( char ssnNum );
        /*
        * ssn :会话号
        * actFlag:会话的主动性，保留
        * infoType:会话的信息类型，通用信息对象或文件信息对象
        * cmnAddr://公共地址
        * stepNum:包含的会话总步数
        */
        void setCfg ( T_snnCfg* snnCfg, u_char ssnID, u_char actFlag, u_char infoType, u_short cmnAddr, u_char stepNum );

        /*
        * t_snnCfg:会话步数据信息
        * stnAcsFlag:
        *reqAPDUNum:请求帧个数
        * respAPDUNum:响应帧个数
        * */
        void setStepCfg ( T_ssnStepCfg* ssnStepCfg, u_short stnAcsFlag, u_char reqAPDUNum, u_char respAPDUNum );
        void setModbsStepCfg ( T_ssnStepCfg* ssnStepCfg, u_short stnAcsFlag, u_char reqAPDUNum, u_char respAPDUNum );

        /*
        *
        */
        void setData ( dataFrame* data, u_char objNum );

        void setEleVal ( ELE_TYPE* destEleVal, ELE_TYPE srcEleVal );

        void setFunDsc ( T_ptclFunDsc* funDsc, u_short ssnDscNo, u_short act_prio_chnlID,
                         u_char usingMode, u_char setNum );

        void setPtclStep ( ptclStep* stepDsc_p, u_short COT, u_short MaxLen, u_char TidNum );
        /*
        * 设置会话表
        */
        T_snnCfg* getSnnCfg ( u_char ssnid, int chnlId );

        T_ptclFunDsc* getFunDsc ( u_char ssnid, int chnlId );

        snnAppCfg*  getAppSnnCfg();

        chnlObjCfg* getchnlObjCfg();

        //XMLDocument* getCfgFile();

		T_lcalIps*   getDataBaseIP();

		T_initChnUnit* getChnUnit();
        // ssnReqInfo* getSsnRInfoCfg ( int chnlID );
        //ssnReqInfo* getSsnRInfoCfg ();
    private:

        int snnUseNum_p[MC_DEF_STN_NUM];   //snnUseNum_p[i]记录t_snnCfg[i]当前在用的会话数量

        T_snnCfg* t_snnCfg[MC_DEF_STN_NUM];//会话步数据信息表
        //XMLDocument docXml;
        u_char ssnNum;
        T_ptclFunDsc* ptclFunDsc[MC_DEF_STN_NUM];

        snnAppCfg ptclSnnCfg[MCT_SSN_NUM];// 协议相关配置，优先级、协议号、会话号

		T_initChnUnit* outChnUnit_p;
		T_lcalIps outLcalIps;
		chnlObjCfg *chnlCfg_p;
        // ssnReqInfo ssnRInfo[MC_DEF_STN_NUM];


        /*******************以上索引号需与通道号对应***********************/

	   /*
        *设置突发会话（高龙山被动站）
        * */
		void setAcqevtCfgGLS ( T_snnCfg* snnCfg, T_ptclFunDsc* funDsc, const int& index );
        /*
        *设置事件收集会话
        * */
        void setAcqevtCfg ( T_snnCfg* snnCfg, T_ptclFunDsc* funDsc,const int& index );
	   /*
        * 设置站召唤会话
        */
		void setStnInrgtnCfgGLS ( T_snnCfg* snnCfg, T_ptclFunDsc* funDsc,const int& index  );
        /*
        * 设置站召唤会话
        */
        void setStnInrgtnCfg ( T_snnCfg* snnCfg, T_ptclFunDsc* funDsc,const int& index  );
        /*
        *设置查询数据会话
        */
        void setQueDataCfg ( T_snnCfg* snnCfg, T_ptclFunDsc* funDsc,const int& index  );
        /*
        * 设置激活会话
        * */
        void setUstartdtCfg ( T_snnCfg* snnCfg, T_ptclFunDsc* funDsc,const int& index  );
        /*
        * 设置停止激活会话
        * */
        void setUstopdtCfg ( T_snnCfg* snnCfg, T_ptclFunDsc* funDsc,const int& index );

        /*
        * 设置测试会话
        * */
        void setTestfrCfg ( T_snnCfg* snnCfg, T_ptclFunDsc* funDsc,const int& index );
        /*
        * 设置建立通道会话
        * */
        void setNewholdCfg ( T_snnCfg* snnCfg, T_ptclFunDsc* funDsc,const int& index );
        /*
        * 设置关闭通道会话
        * */
        void setDelholdCfg ( T_snnCfg* snnCfg, T_ptclFunDsc* funDsc,const int& index );

        void setAppSnnCfg();

        void setchnlObjCfg();

        void setModbsQueDataCfg ( int ssnNo, T_snnCfg* snnCfg, T_ptclFunDsc* funDsc,const int& index ,u_short offset );

        void crtSsnRInfoCfg ( ssnReqInfo* ssnRInfo, int ssnRInfoNum );
        /*
        * 设置发起回话时需要的基本信息
        * chnlID ：通道号
        * ssn ： 回话号
        * ptclNo：协议号
        * ssnRINfoNum ：本通道需要发起的回话个数
        * addrStart：起始点地址
        * accumValue：点的个数
        * snnMaxRetryNum：最大回话重试次数
        * ssnRInfo：回话内容
        */
        void setSsnReqInfoCfg ( int chnlID, u_char ssnNo, u_char ptclNo,  int ssnRInfoIndex, u_short addrStart,
                                u_short accumValue, int snnMaxRetryNum, bool collMethods, ssnReqInfo* ssnRInfo );

        void setInParCfg ( T_initChnUnit* inPar, int chnlID );
        /*
        phyObjVal ： 物理量对应关系结构
        phyType ： 数据类型
        calcMethd ：  物理量值的算法
        phyValLng ： 物理量对应的值长度
        numOfAdd ： 对应点地址的个数
        pntAddr_p ：对应点地址的头
        dataId_p ：数据id地址
        */
        void setPhyObjValCfg ( T_phyValCfg*  phyObjVal,   PHY_UNIT_TYPE  phyType, PHY_CALC_METHD calcMethd,
                               u_char phyValLng,  u_int numOfAdd, PNT_ADDR_TYPE* pntAddr_p, void* dataId_p  );
};
















/*****************************************************************************
     以下为       傅世凯 合并解析xml配置文件的接口
******************************************************************************/






//释放T_initChnUnit[]
void freeNChnUnit ( const u_int& num,
                    T_initChnUnit** initChnUnit_pp
                  );
//释放chnlObjCfg[]
void freeNChnlObjCfg ( const u_int& num,
                       chnlObjCfg** freData_pp
                     );
//从xml文件获取应用配置
int getScadaAppCfgFxml ( const char* xmlFilePath_p,     /*xml配置文件(带路径)*/
                         u_int& outChnNumS,             /*出参:通道总数*/
                         chnlObjCfg** outChnlObjCfg_pp,/*出参:返回非共用内存的配置*/
                         T_initChnUnit** outChnUnit_pp,/*出参:返回共用内存的配置*/
                         T_lcalIps* outLcalIps_p       /*出参:返回本地ip*/
                       );




#endif //CFG_MANAGE
