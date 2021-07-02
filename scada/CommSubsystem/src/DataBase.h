#pragma once
#ifndef H_DATA_BASE
#define H_DATA_BASE

#include "FrameGlobal.h"
#include "Timer.h"
#define DEF_BLK_LEN		50	//默认C_Storage每个数据块儿长度
#define DEF_MAX_BLK_NUM 20	//默认C_Storage数据块儿最大个数
#define LIB_NAME_104  "libAPDU.so"
#define LIB_NAME_106  "libMODBUS.so"
#ifdef WINDOWS_PLATFORM

#else
#define CRITICAL_SECTION pthread_mutex_t
#endif

class C_SsnCtnrObj // 会话容器
{
public:
	// 创建和删除会话容器
	C_SsnCtnrObj();
	~C_SsnCtnrObj();
public:
	// 接收和排除会话对象
	// FUN：接收会话对象
	// PARAM：
	//		1、uiPrio：优先级
	//		2、ssnCtrlBlk_p：会话控制块地址
	// RET：
	//		0，成功；其他，失败
	int aplyAdtSsnInst(u_int uiPrio, ssnCtlBlk *ssnCtrlBlk_p);

	// FUN：删除会话对象
	// PARAM：
	//		1、uiPrio：优先级
	//		2、ssnCtrlBlk_p：会话控制块地址
	// RET：
	//		0，成功；其他，失败
	int delSsnObj(u_int uiPrio, ssnCtlBlk *ssnCtrlBlk_p);

	// FUN：调度会话对象
	// PARAM:
	//			无
	// RET:
	//		非空，成功；NULL，失败
	ssnCtlBlk* schdSsnObj();

	// 派发会话对象
	// FUN：派发会话对象
	// PARAM：
	//		1、ssnHead_p：会话控制块链表头指针
	//		2、ucInit：主动性标识
	//		3、uiPrio：优先级
	// RET：
	//		0，成功；其他，失败
//	ssnCtlBlk* dispbSsnObj(ssnCtlBlk* ssnHead_p, u_char ucInit, u_int uiPrio);
	ssnCtlBlk* dispbSsnObj(ssnTbl* ssnTbl_p, T_ptclFunDsc maybeSsnSet[], u_char funNum, int* iIndex);

	// 设置和修改及获取执行权级
	// FUN：设置执行权级
	// PARAM：
	//		1、uiPrio：优先级
	// RET：
	//		0，成功；其他，失败
	int setPriority(u_int uiPrio);

	// FUN：获取执行权级
	// PARAM：
	// RET：
	//		执行权级
	int GetPriority();
private:
	u_int	m_uiPrio;	// 执行权级

	u_short	m_chPrioRdyBlk[MC_DEF_MAX_PRIO];	// 通道优先级就绪个数
	u_char  m_ssnRdyTab[MC_DEF_MAX_PRIO][128];	// 会话实例就绪表

	u_char	m_errFlag;				// 错误标记
	ssnCtlBlk*	m_errSsnBlk_p;		// 错误会话实例块，用于错误处理的会话实例块

	nsCPLock::CLock	m_lock;
};

class C_SsnObj //会话对象
{
public:
	// 构造和析构
	C_SsnObj();
	~C_SsnObj();
public:
    // FUN：获取会话号
    // PARAM：
    // RET：
    //		>=0，成功；其他，失败；
    int GetSsnNo();

    // 注册会话对象
    // 	FUN：注册此会话对象，注：asdu移到通道信息块中！！
    // 	PARAM：
    // 		1、ssnID：会话号
    // 		2、retSnnAddr_p：存放会话对象的地址
    // 	RET：
    // 		0，成功；负值，失败
    int regSsnObj(short ssnID, C_SsnObj* retSnnAddr_p);

    // 获取会话启动、驱动、验收函数注册地址
    // 	FUN：获取注册函数地址
    // 	PARAM：
    // 		1、ucMode：标识区分函数地址类型，启动函数=0，驱动函数=1，验收函数=2
    // 	RET：
    // 		0，成功；负值，失败
    // 	
    FunTypeOne getPtclSnnFun(u_char ucMode);

    // 注册会话结束用户回调函数
    // 	
    // 	FUN：为用户编程保留
    // 	PARAM：
    // 		1、dstFun：用户回调函数地址
    // 	RET：
    // 		0，成功；负值，失败
    // 	
    void CallBackForEnd(FunTypeOne dstFun);

    // 注册会话对象的ASDU存储体地址、设置和获取会话实例的ASDU长度
    // FUN：注册会话对象的ASDU存储体地址
    // PARAM：
    //		1、chnlInfoBlk_p：通道信息块
    //		2、asduAddr_p：ASDU存储体地址
    //		3、actFlag：主动性标记
    // RET：
    //		0，成功；负值，失败
    int regASDUaddr(chnlInfoBlk* chnlInfoBlk_p, APDU* asduAddr_p, u_char actFlag = MC_ACT_FLAG);

    // FUN：获取会话实例的ASDU长度
    // PARAM：
    //		1、ssnCB_p：会话实例地址
    // RET：
    //		0，成功；负值，失败
    int getASDUlen(ssnCtlBlk* ssnCB_p/*, u_char actFlag = MC_ACT_FLAG*/);

    // FUN：设置会话实例的ASDU长度
    // PARAM：
    //		1、ucASDUlen：ASDU长度
    //		2、ssnCB_p：会话实例地址
    //		3、actFlag：主动性标记
    // Ret：
    //		0，成功；负值，失败
    int setASDUlen(u_char ucASDUlen, ssnCtlBlk* ssnCB_p, u_char actFlag = MC_ACT_FLAG);

    // 执行注册函数
    // 	FUN：调用被注册的函数
    // 	PARAM:
    // 		1、ucMode：函数类型
    //				   启动函数=0，驱动函数=1，验收函数=2，会话结束用户回调函数=255
    // 		2、funParam_p：用于在调用被注册函数调用时，作为它的参数传入
    // 	RET:
    // 		0，成功；负值，失败
    int execRegFun(u_char ucMode, void *funParam_p);

    // 获取和设置主动会话实例结果信息报告
    // FUN：获取主动会话实例结构信息报告
    // PARAM：
    //		1、actSsnCtlBlk_p：主动会话实例地址
    // 	RET:
    // 		0，成功；负值，失败
    ssnInstRslt* getResultInfo(ssnCtlBlk* actSsnCtlBlk_p);

    // FUN：设置主动会话实例结构信息报告
    // PARAM：
    //		1、actSsnCtlBlk_p：主动会话实例地址
    //		2、rstInfo_p：返回信息报告的地址
    // 	RET:
    // 		0，成功；负值，失败
    int setResultInfo(ssnCtlBlk* actSsnCtlBlk_p, int rstInfo);

    // 获取和设置被动会话实例结果信息报告
    // FUN：获取被动会话实例结构信息报告
    // PARAM：
    //		1、chnlInfo_p：被动会话实例地址
    // 	RET:
    // 		0，成功；负值，失败
    int getPsvResInfo(chnlInfoBlk* chnlInfo_p);

    // FUN：设置被动会话实例结构信息报告
    // PARAM：
    //		1、dstChnlInfo_p：被动会话实例地址
    //		2、psvResInfo_p：被动会话信息报告地址
    // RET:
    // 		0，成功；负值，失败
    int setPsvResInfo(chnlInfoBlk* dstChnlInfo_p, int* psvResInfo_p);

    // 创建和删除主动会话实例
    // FUN：创建主动会话实例
    // PARAM：
    //		1、ssnCtlBlk_p：主动会话实例返回地址
    //		2、ssnStartPara_p：会话启动参数地址
    // RET:
    // 		0，成功；负值，失败
    int crtActiveSsn(ssnCtlBlk** ssnCtlBlk_p, T_snnPara* ssnStartPara_p);

    // FUN：删除主动会话实例
    // PARAM：
    //		1、ssnCtlBlk_p：主动会话实例地址
    // RET:
    // 		0，成功；负值，失败
    int delActiveSsn(ssnCtlBlk* ssnCtlBlk_p);

    // 创建和删除被动会话实例
    // FUN：创建被动会话实例
    // PARAM：
    //		1、ssnCtlBlk_p：被动会话实例返回地址
    // RET:
    // 		0，成功；负值，失败
    int crtPassiveSsn(ssnCtlBlk** ssnCtlBlk_p, T_snnPara* ssnStartPara_p);

    // FUN：删除被动会话实例
    // PARAM：
    //		1、ssnCtlBlk_p：被动会话实例地址
    // RET:
    // 		0，成功；负值，失败
    int delPassiveSsn(ssnCtlBlk* ssnCtlBlk_p);

    // 创建通道信息块到会话描述符中
    // FUN：创建通道信息块到会话描述符中
    // PARAM：
    //		1、chnlInfoB_p：通道信息块返回地址
    // RET:
    // 		0，成功；负值，失败
    int crtChnlInfo(chnlInfoBlk* &chnlInfoB_p);

    // FUN：删除通道信息块到会话描述符中
    // PARAM：
    //		1、chnlInfoB_p：通道信息块地址
    // RET:
    // 		0，成功；负值，失败
    int delChnlInfo(chnlInfoBlk* chnlInfoB_p);

    // FUN：获取通道信息块
    // PARAM：
    //		1、chnlID：通道号
    // RET：
    //		非空，成功；其他，失败；
    chnlInfoBlk* getChnlInfoBlk(u_short chnlID);//lzq

    // FUN：获取被动会话结果集
    // PARAM：
    //		1、chnlID：通道号
    // RET：
    //		非空，成功；其他，失败；
    ssnInstRslt* getPsvSsnRslt(u_short chnlID);//lzq

    // FUN：获取会话信息
    // PARAM：
    //		1、ssnCtlBlk_p：会话控制实例
    //		2、dstSsn_p：目标会话实例，用来存放获取到的会话实例控制信息
    // RET：
    //		0，成功；其他，失败；
    int getSsnPartInfo(ssnCtlBlk* ssnCtlBlk_p, ssnCtlBlk* dstSsn_p);

    // FUN：获取APDU地址
    // PARAM：
    //		1、actFlag：主动性标志
    //		2、chnlID：通道号
    // RET：
    //		非空，成功；其他，失败；
    ASDU* getAPDU(u_char actFlag, short chnlID);

    ASDU* getAPDU( u_char actFlag, chnlInfoBlk* chnlIBlk_p );

    // FUN：获取会话参数
    // PARAM：
    //		1、ssnCtlBlk_p：会话控制实例地址
    //		2、snnPar_p：会话参数地址
    // RET：
    //		非空，成功；其他，失败；
    int getSsnPara(ssnCtlBlk* ssnCtlBlk_p, T_snnPara* snnPar_p);

    // FUN：获取会话相关信息
    // PARAM：
    //		1、ssnCtlBlk_p：会话控制实例地址
    //		2、ssnInstInfo_p：会话相关信息存储地址
    // RET：
    //		0，成功；其他，失败；
    int getSsnInstInfo(ssnCtlBlk* ssnCtlBlk_p, T_ssnInstInfoSet* ssnInstInfo_p);
    //tAPDU1Obj_p = tSsnObj_p->getASDUObj(tChnlID, (u_char)tMaybeFunSet[tIndex].act_prio_chnlID);//method?

    // tSsnObj_p->getSsnPara(tSsnCtlBlk_p,tSsnPara_p);//method? get: tSsnCtlBlk_p->ssnPara_p;

    // tNxtStepNo_p = tSsnObj_p->getDrvStep(tSsnCtlBlk_p);//method?tNxtStepNo_p=0xFF no data load

    /*
    // 获取会话名
    void GetSnnName();

    // 获取会话优先级
    void GetPriority();

    // 获取会话对象指针
    void *GetObjPoint();
    */

public:
    ssnDsc		m_ssnDescript;		// 会话描述符
    ssnCtlBlk*	m_ssnCtrBlk_p;		// 会话实例控制块指针
    nsCPLock::CLock	m_lock;
};


class C_ChnlObj // 通道（组）对象
{
private: 
    chnlDsc		m_chnlDsc;	// 通道描述符
    u_char		m_ucState;	// 连接状态

public:
    // 生成和删除通道（组）
    C_ChnlObj(u_char &ucState, int iBufLen ,int ptclMdlNum);
    C_ChnlObj(int iBufLen , int ptclMdlNum);
    ~C_ChnlObj();
public:

    //************************************
    // FUN：	获取通道对象指针
    // PARAM：
    // RET：	非空，成功；其他，失败；
    //************************************
    C_ChnlObj* getObj();

    //************************************
    // FUN：	获取通道号
    // PARAM：
    // RET：	>=0，成功；其他，失败；
    //************************************
    int getChnlNo();

    //************************************
    // FUN：	设置通道号
    // PARAM：	1、chnlNO：通道号
    // RET：	0，成功；其他，失败；
    //************************************	
    int setChnlNo(short chnlNo);

    // 获取和设置通道名
    int getChnlName(char* chnlName_p, int iLen);
    int setChnlName(char* chnlName_p, int iLen);

    // 获取和设置支持会话集
    ssnSets* getSsnSet(short ssnNo, u_char actFlag = MC_ACT_FLAG);
    int setSsnSets(ssnSets *inSsnSets, u_short usNum);

    // 获取和设置传输控制层配置和实时参数值
    void* getTranCtrlCfg();
    int setTranCtrlCfg(void *tranCtrlCfg_p);

    // 实时参数指针
    void* getRealParam();
    int setRealParam(void *realParam_p);

    // 	// 获取和设置k、w门限值
    // 	int getKandW(u_short &usK, u_short &usW);
    // 	int setKandW(u_short usK, u_short usW);

    // 获取和设置传输控制层超时时间、定时资源时间通道指针和控制码
    int getOutTime(char tmCode);
    int setOutTime(char tmCode, short newValue);
    int getTimer(tmChnl*& tmChnl_p);
    int setTimer(tmChnl* tmChnl_p);
    int getCtrlCode(char tmCode, char &ctrlCode);
    int setCtrlCode(char tmCode, char ctrlCode);

    // 获取和设置角色
    int getRole();
    int setRole(short &role);

    // 获取和创建连接句柄
    void* getSockHdl();
    int setSockHdl(void* sockHdl_p);

    // 获取和设置本地网络地址
    int getLocNetAddr(addrItem &locAddr);
    int setLocNetAddr(addrItem &locAddr);

    // 获取和设置传输控制帧和ASDU描述符
    void* getTranCtrl();
    int setTranCtrl(char* tranCtrlBuf_p, int iLen);
    int getASDUDsc();
    int setASDUDsc();

    // 获取和锁定、解锁远程站
    C_StnObj* getStn();
    int lockStn(stnDsc* stnDsc_p, stnOrVew* stnOrVew_p);
    int unLockStn();

    // 获取和设置连接状态
    int getChnlState(char &cState);
    int setChnlState(char cState);

    // 加入和去除会话实例（保证被动会话的单实例特征，以及非空特性）
    int addSsn(ssnCtlBlk* ssnCtlBlk_p, u_int uiProi);
    int delSsn(ssnCtlBlk* ssnCtlBlk_p, u_int uiProi, bool bNoDel = false);

    // 获取会话实例表指针
    ssnTbl* getSsnTbl();

    // 获取被动会话实例控制块指针
    ssnCtlBlk* psvSsnCtlBlk(u_int uiProi);

    // 获取主动会话实例控制块指针、或者单实例会话控制块指针
    ssnCtlBlk* actSsnCtlBlk(u_int uiProi);

    // 
    ssnInstRslt* getActSsnRslt(u_short stnID);//lzq

    // 规约描述
    HMODULE* getPtclMdl();
    //公共通讯协议描述符
    HMODULE* getCommMdl();
    //set远程站对象指针
    int setStnObj(C_StnObj* stnObj);
    //get远程站对象指针
    C_StnObj* getStnObj();

    //set远程站回调函数
    int setRtnfun(void (*rtnFun)(void *));
    //get远程站回调用函数
    void* getRtnfun();

    // 接收端数据缓冲区指针 ...
    void* getRcvBufPtr();
    int setRcvBufPtr(void *rcvBufPtr[], char bufNum);

    //发送端数据缓冲指针
    void* getSendBuf();
    int setSendBuf(void *sendBuf[], char bufNum);

};

// 
struct addrRange			// ADDR
{
    u_short useNum;			// 实际应用点数
    u_short	begAddr;		// 起始地址，不可为零
    u_short	endAddr;		// 结束地址，可以为零
};
class C_DStorage
{
private:
    //	short		m_minThres;			// 最小阈值，间距小于该阈值则合并数据地址ADDR ！！外面配置合并！
    short		m_blkLen;			// 每个数据块长度
    short		m_blkNum;			// 数据块个数，默认最大20，可根据实际情况调整
    union usrData**	m_blkMem_pp;		// 数据块索引，<块号,块首地址>，为了快速定位数据块
    char		m_usrType;			//说明usrdata数据类型

    short		m_arLen;			// ADDR索引长度,m_addrIdx为数组,此变量为数组的长度
    addrRange*	m_addrIdx;			// 点表ADDR索引，<序号，addrRange>
    CRITICAL_SECTION 	memKey;		//mutex of G
public:
    // 初始化
    C_DStorage();
    C_DStorage(addrRange* cfgAddr_p, short cfgLen, u_char usrType);
    ~C_DStorage();

    // ucMask 取值掩码，默认255，全部取出，相应数值项对应掩码位数，位数为1则取出相应数据值，0则不需要
    int loadStnData(T_snnPara* ssnPara_p, u_short ucMask);

    // 策略：根据数据中addr，查找ADDR索引，确定数据在存储中的映射位置（块号、数据块中位置下标）
    //		 然后查找到数据块索引，定位数据映射位置，将相应数据存储进来
    // snnPara_p，数据结构中带有相关数据，及数据参数
    int storeStnData(T_snnPara* ssnPara_p, u_short ucMask);

    int getData(union usrData* blkMem_p , u_int addr);
};

// 
class C_StnObj // 远程站对象
{
public:
    // 生成和删除远程站
    C_StnObj();
    ~C_StnObj();

public:
    // 初始化远程站描述符中简易存储，注：此接口每次只能初始化一种数据，如包含两种及以上数据的话就需要调用多次
    int initDStorage(stnDsc* stnDsc_p, addrRange* cfgAddr_p, short cfgLen, u_short usrType);

    // 远程站重置
    int reSet();

    // 获取和设置远程站号
    int getStnID(stnDsc* stnDsc_p);
    int setStnID(stnDsc* stnDsc_p, short stnID);

    // 获取和设置连接号
    int getChnlID(stnDsc* stnDsc_p);
    int setChnlID(stnDsc* stnDsc_p, short chnlID);

    // 获取和设置远程站名
    int getStnName(stnDsc* stnDsc_p, char *stnName_p, int &iLen);
    int setStnName(stnDsc* stnDsc_p, char *stnName_p, int iLen);

    // 获取和设置远程站角色
    char getStnRole(stnDsc* stnDsc_p);
    int setStnRole(stnDsc* stnDsc_p, char stnRole);

    // 获取逻辑站指针
    int getLogicStn(short stnID, stnDsc* lgcStn_p);

    // 添加逻辑站结点
    int addLogicStn(stnDsc* dstStn_p, stnDsc* addStn_p);
    //	int delLogicStn(stnDsc* dstStn_p, stnDsc* delStn_p);

    // 登记、注销点表访问
    int loginInfoTbl(stnDsc* stnDsc_p, void* infoTbl_p);
    int logoffInfoTbl(stnDsc* stnDsc_p);

    // 设置和清除循环数据传输标识
    int setCleFlag(stnDsc* stnDsc_p);
    int delCleFlag(stnDsc* stnDsc_p);

    // 分配和释放跳变跟踪缓冲区
    int crtJumpFlw(stnDsc* stnDsc_p);
    int rlsJumpFlw(stnDsc* stnDsc_p);

    // 分配和释放存储映像脏标记跟踪索引数组
    int crtStrgMap(stnDsc* stnDsc_p);
    int rlsStrgMap(stnDsc* stnDsc_p);

    // 获取下一远程站指针
    int getNextStn(stnDsc* stnDsc_p, stnDsc* retStn_p);

    // 登记信息对象跳变回调例程
    int loginJumpFun(stnDsc* stnDsc_p, FunTypeOne jumpFun);

    // 获取信息对象跳变回调例程指针
    FunTypeOne getJumpFun(stnDsc* stnDsc_p);

    // 信息对象跳变记录例程
    int execRcdExpFun(stnDsc* stnDsc_p, void* exeParam_p);

    // 获取信息对象跳变记录例程指针
    FunTypeOne getRcdExpFun(stnDsc* stnDsc_p);

    // 获取和设置远程站支持的会话集信息
    int getSsnInfo();
    int setSsnInfo(void* ssnInfo_p);

    // 添加物理远程站对象（按照远程站号升序排列）
    int addPscStn(stnDsc* stnObj);

    // 删除远程站（逻辑or物理）
    int delStn(short stnID);

    // 得到远程站描述符指针
    stnDsc* getStnDsc(short stnID);
public:
    // 读取点表数据
    int loadStnData(T_snnPara* ssnPara_p, short stnID, u_char ucType, u_short ucMask , int iPtclID);

    // 存储点表数据
    int storeStnData(T_snnPara* ssnPara_p, short stnID, u_char ucType, u_short ucMask , int iPtclID);

    // 获取存储对象指针
    C_DStorage* getStrObj(short stnID);  
    C_DStorage* getStrObj(stnDsc* stnDsc_p);  

private:
    int			m_stnNum;
    stnDsc*		m_stnHead;

    nsCPLock::CLock	m_lock;
};

//static void ssnTmOutDftHdl(void* tmoutInfo_p);
extern C_TmObj tmFclty;
extern C_SsnMgr ssnMgr;
//void ssnTmOutDftHdl(void* tmoutInfo_p);
#endif
