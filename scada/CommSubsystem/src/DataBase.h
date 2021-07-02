#pragma once
#ifndef H_DATA_BASE
#define H_DATA_BASE

#include "FrameGlobal.h"
#include "Timer.h"
#define DEF_BLK_LEN		50	//Ĭ��C_Storageÿ�����ݿ������
#define DEF_MAX_BLK_NUM 20	//Ĭ��C_Storage���ݿ��������
#define LIB_NAME_104  "libAPDU.so"
#define LIB_NAME_106  "libMODBUS.so"
#ifdef WINDOWS_PLATFORM

#else
#define CRITICAL_SECTION pthread_mutex_t
#endif

class C_SsnCtnrObj // �Ự����
{
public:
	// ������ɾ���Ự����
	C_SsnCtnrObj();
	~C_SsnCtnrObj();
public:
	// ���պ��ų��Ự����
	// FUN�����ջỰ����
	// PARAM��
	//		1��uiPrio�����ȼ�
	//		2��ssnCtrlBlk_p���Ự���ƿ��ַ
	// RET��
	//		0���ɹ���������ʧ��
	int aplyAdtSsnInst(u_int uiPrio, ssnCtlBlk *ssnCtrlBlk_p);

	// FUN��ɾ���Ự����
	// PARAM��
	//		1��uiPrio�����ȼ�
	//		2��ssnCtrlBlk_p���Ự���ƿ��ַ
	// RET��
	//		0���ɹ���������ʧ��
	int delSsnObj(u_int uiPrio, ssnCtlBlk *ssnCtrlBlk_p);

	// FUN�����ȻỰ����
	// PARAM:
	//			��
	// RET:
	//		�ǿգ��ɹ���NULL��ʧ��
	ssnCtlBlk* schdSsnObj();

	// �ɷ��Ự����
	// FUN���ɷ��Ự����
	// PARAM��
	//		1��ssnHead_p���Ự���ƿ�����ͷָ��
	//		2��ucInit�������Ա�ʶ
	//		3��uiPrio�����ȼ�
	// RET��
	//		0���ɹ���������ʧ��
//	ssnCtlBlk* dispbSsnObj(ssnCtlBlk* ssnHead_p, u_char ucInit, u_int uiPrio);
	ssnCtlBlk* dispbSsnObj(ssnTbl* ssnTbl_p, T_ptclFunDsc maybeSsnSet[], u_char funNum, int* iIndex);

	// ���ú��޸ļ���ȡִ��Ȩ��
	// FUN������ִ��Ȩ��
	// PARAM��
	//		1��uiPrio�����ȼ�
	// RET��
	//		0���ɹ���������ʧ��
	int setPriority(u_int uiPrio);

	// FUN����ȡִ��Ȩ��
	// PARAM��
	// RET��
	//		ִ��Ȩ��
	int GetPriority();
private:
	u_int	m_uiPrio;	// ִ��Ȩ��

	u_short	m_chPrioRdyBlk[MC_DEF_MAX_PRIO];	// ͨ�����ȼ���������
	u_char  m_ssnRdyTab[MC_DEF_MAX_PRIO][128];	// �Ựʵ��������

	u_char	m_errFlag;				// ������
	ssnCtlBlk*	m_errSsnBlk_p;		// ����Ựʵ���飬���ڴ�����ĻỰʵ����

	nsCPLock::CLock	m_lock;
};

class C_SsnObj //�Ự����
{
public:
	// ���������
	C_SsnObj();
	~C_SsnObj();
public:
    // FUN����ȡ�Ự��
    // PARAM��
    // RET��
    //		>=0���ɹ���������ʧ�ܣ�
    int GetSsnNo();

    // ע��Ự����
    // 	FUN��ע��˻Ự����ע��asdu�Ƶ�ͨ����Ϣ���У���
    // 	PARAM��
    // 		1��ssnID���Ự��
    // 		2��retSnnAddr_p����ŻỰ����ĵ�ַ
    // 	RET��
    // 		0���ɹ�����ֵ��ʧ��
    int regSsnObj(short ssnID, C_SsnObj* retSnnAddr_p);

    // ��ȡ�Ự���������������պ���ע���ַ
    // 	FUN����ȡע�ắ����ַ
    // 	PARAM��
    // 		1��ucMode����ʶ���ֺ�����ַ���ͣ���������=0����������=1�����պ���=2
    // 	RET��
    // 		0���ɹ�����ֵ��ʧ��
    // 	
    FunTypeOne getPtclSnnFun(u_char ucMode);

    // ע��Ự�����û��ص�����
    // 	
    // 	FUN��Ϊ�û���̱���
    // 	PARAM��
    // 		1��dstFun���û��ص�������ַ
    // 	RET��
    // 		0���ɹ�����ֵ��ʧ��
    // 	
    void CallBackForEnd(FunTypeOne dstFun);

    // ע��Ự�����ASDU�洢���ַ�����úͻ�ȡ�Ựʵ����ASDU����
    // FUN��ע��Ự�����ASDU�洢���ַ
    // PARAM��
    //		1��chnlInfoBlk_p��ͨ����Ϣ��
    //		2��asduAddr_p��ASDU�洢���ַ
    //		3��actFlag�������Ա��
    // RET��
    //		0���ɹ�����ֵ��ʧ��
    int regASDUaddr(chnlInfoBlk* chnlInfoBlk_p, APDU* asduAddr_p, u_char actFlag = MC_ACT_FLAG);

    // FUN����ȡ�Ựʵ����ASDU����
    // PARAM��
    //		1��ssnCB_p���Ựʵ����ַ
    // RET��
    //		0���ɹ�����ֵ��ʧ��
    int getASDUlen(ssnCtlBlk* ssnCB_p/*, u_char actFlag = MC_ACT_FLAG*/);

    // FUN�����ûỰʵ����ASDU����
    // PARAM��
    //		1��ucASDUlen��ASDU����
    //		2��ssnCB_p���Ựʵ����ַ
    //		3��actFlag�������Ա��
    // Ret��
    //		0���ɹ�����ֵ��ʧ��
    int setASDUlen(u_char ucASDUlen, ssnCtlBlk* ssnCB_p, u_char actFlag = MC_ACT_FLAG);

    // ִ��ע�ắ��
    // 	FUN�����ñ�ע��ĺ���
    // 	PARAM:
    // 		1��ucMode����������
    //				   ��������=0����������=1�����պ���=2���Ự�����û��ص�����=255
    // 		2��funParam_p�������ڵ��ñ�ע�ắ������ʱ����Ϊ���Ĳ�������
    // 	RET:
    // 		0���ɹ�����ֵ��ʧ��
    int execRegFun(u_char ucMode, void *funParam_p);

    // ��ȡ�����������Ựʵ�������Ϣ����
    // FUN����ȡ�����Ựʵ���ṹ��Ϣ����
    // PARAM��
    //		1��actSsnCtlBlk_p�������Ựʵ����ַ
    // 	RET:
    // 		0���ɹ�����ֵ��ʧ��
    ssnInstRslt* getResultInfo(ssnCtlBlk* actSsnCtlBlk_p);

    // FUN�����������Ựʵ���ṹ��Ϣ����
    // PARAM��
    //		1��actSsnCtlBlk_p�������Ựʵ����ַ
    //		2��rstInfo_p��������Ϣ����ĵ�ַ
    // 	RET:
    // 		0���ɹ�����ֵ��ʧ��
    int setResultInfo(ssnCtlBlk* actSsnCtlBlk_p, int rstInfo);

    // ��ȡ�����ñ����Ựʵ�������Ϣ����
    // FUN����ȡ�����Ựʵ���ṹ��Ϣ����
    // PARAM��
    //		1��chnlInfo_p�������Ựʵ����ַ
    // 	RET:
    // 		0���ɹ�����ֵ��ʧ��
    int getPsvResInfo(chnlInfoBlk* chnlInfo_p);

    // FUN�����ñ����Ựʵ���ṹ��Ϣ����
    // PARAM��
    //		1��dstChnlInfo_p�������Ựʵ����ַ
    //		2��psvResInfo_p�������Ự��Ϣ�����ַ
    // RET:
    // 		0���ɹ�����ֵ��ʧ��
    int setPsvResInfo(chnlInfoBlk* dstChnlInfo_p, int* psvResInfo_p);

    // ������ɾ�������Ựʵ��
    // FUN�����������Ựʵ��
    // PARAM��
    //		1��ssnCtlBlk_p�������Ựʵ�����ص�ַ
    //		2��ssnStartPara_p���Ự����������ַ
    // RET:
    // 		0���ɹ�����ֵ��ʧ��
    int crtActiveSsn(ssnCtlBlk** ssnCtlBlk_p, T_snnPara* ssnStartPara_p);

    // FUN��ɾ�������Ựʵ��
    // PARAM��
    //		1��ssnCtlBlk_p�������Ựʵ����ַ
    // RET:
    // 		0���ɹ�����ֵ��ʧ��
    int delActiveSsn(ssnCtlBlk* ssnCtlBlk_p);

    // ������ɾ�������Ựʵ��
    // FUN�����������Ựʵ��
    // PARAM��
    //		1��ssnCtlBlk_p�������Ựʵ�����ص�ַ
    // RET:
    // 		0���ɹ�����ֵ��ʧ��
    int crtPassiveSsn(ssnCtlBlk** ssnCtlBlk_p, T_snnPara* ssnStartPara_p);

    // FUN��ɾ�������Ựʵ��
    // PARAM��
    //		1��ssnCtlBlk_p�������Ựʵ����ַ
    // RET:
    // 		0���ɹ�����ֵ��ʧ��
    int delPassiveSsn(ssnCtlBlk* ssnCtlBlk_p);

    // ����ͨ����Ϣ�鵽�Ự��������
    // FUN������ͨ����Ϣ�鵽�Ự��������
    // PARAM��
    //		1��chnlInfoB_p��ͨ����Ϣ�鷵�ص�ַ
    // RET:
    // 		0���ɹ�����ֵ��ʧ��
    int crtChnlInfo(chnlInfoBlk* &chnlInfoB_p);

    // FUN��ɾ��ͨ����Ϣ�鵽�Ự��������
    // PARAM��
    //		1��chnlInfoB_p��ͨ����Ϣ���ַ
    // RET:
    // 		0���ɹ�����ֵ��ʧ��
    int delChnlInfo(chnlInfoBlk* chnlInfoB_p);

    // FUN����ȡͨ����Ϣ��
    // PARAM��
    //		1��chnlID��ͨ����
    // RET��
    //		�ǿգ��ɹ���������ʧ�ܣ�
    chnlInfoBlk* getChnlInfoBlk(u_short chnlID);//lzq

    // FUN����ȡ�����Ự�����
    // PARAM��
    //		1��chnlID��ͨ����
    // RET��
    //		�ǿգ��ɹ���������ʧ�ܣ�
    ssnInstRslt* getPsvSsnRslt(u_short chnlID);//lzq

    // FUN����ȡ�Ự��Ϣ
    // PARAM��
    //		1��ssnCtlBlk_p���Ự����ʵ��
    //		2��dstSsn_p��Ŀ��Ựʵ����������Ż�ȡ���ĻỰʵ��������Ϣ
    // RET��
    //		0���ɹ���������ʧ�ܣ�
    int getSsnPartInfo(ssnCtlBlk* ssnCtlBlk_p, ssnCtlBlk* dstSsn_p);

    // FUN����ȡAPDU��ַ
    // PARAM��
    //		1��actFlag�������Ա�־
    //		2��chnlID��ͨ����
    // RET��
    //		�ǿգ��ɹ���������ʧ�ܣ�
    ASDU* getAPDU(u_char actFlag, short chnlID);

    ASDU* getAPDU( u_char actFlag, chnlInfoBlk* chnlIBlk_p );

    // FUN����ȡ�Ự����
    // PARAM��
    //		1��ssnCtlBlk_p���Ự����ʵ����ַ
    //		2��snnPar_p���Ự������ַ
    // RET��
    //		�ǿգ��ɹ���������ʧ�ܣ�
    int getSsnPara(ssnCtlBlk* ssnCtlBlk_p, T_snnPara* snnPar_p);

    // FUN����ȡ�Ự�����Ϣ
    // PARAM��
    //		1��ssnCtlBlk_p���Ự����ʵ����ַ
    //		2��ssnInstInfo_p���Ự�����Ϣ�洢��ַ
    // RET��
    //		0���ɹ���������ʧ�ܣ�
    int getSsnInstInfo(ssnCtlBlk* ssnCtlBlk_p, T_ssnInstInfoSet* ssnInstInfo_p);
    //tAPDU1Obj_p = tSsnObj_p->getASDUObj(tChnlID, (u_char)tMaybeFunSet[tIndex].act_prio_chnlID);//method?

    // tSsnObj_p->getSsnPara(tSsnCtlBlk_p,tSsnPara_p);//method? get: tSsnCtlBlk_p->ssnPara_p;

    // tNxtStepNo_p = tSsnObj_p->getDrvStep(tSsnCtlBlk_p);//method?tNxtStepNo_p=0xFF no data load

    /*
    // ��ȡ�Ự��
    void GetSnnName();

    // ��ȡ�Ự���ȼ�
    void GetPriority();

    // ��ȡ�Ự����ָ��
    void *GetObjPoint();
    */

public:
    ssnDsc		m_ssnDescript;		// �Ự������
    ssnCtlBlk*	m_ssnCtrBlk_p;		// �Ựʵ�����ƿ�ָ��
    nsCPLock::CLock	m_lock;
};


class C_ChnlObj // ͨ�����飩����
{
private: 
    chnlDsc		m_chnlDsc;	// ͨ��������
    u_char		m_ucState;	// ����״̬

public:
    // ���ɺ�ɾ��ͨ�����飩
    C_ChnlObj(u_char &ucState, int iBufLen ,int ptclMdlNum);
    C_ChnlObj(int iBufLen , int ptclMdlNum);
    ~C_ChnlObj();
public:

    //************************************
    // FUN��	��ȡͨ������ָ��
    // PARAM��
    // RET��	�ǿգ��ɹ���������ʧ�ܣ�
    //************************************
    C_ChnlObj* getObj();

    //************************************
    // FUN��	��ȡͨ����
    // PARAM��
    // RET��	>=0���ɹ���������ʧ�ܣ�
    //************************************
    int getChnlNo();

    //************************************
    // FUN��	����ͨ����
    // PARAM��	1��chnlNO��ͨ����
    // RET��	0���ɹ���������ʧ�ܣ�
    //************************************	
    int setChnlNo(short chnlNo);

    // ��ȡ������ͨ����
    int getChnlName(char* chnlName_p, int iLen);
    int setChnlName(char* chnlName_p, int iLen);

    // ��ȡ������֧�ֻỰ��
    ssnSets* getSsnSet(short ssnNo, u_char actFlag = MC_ACT_FLAG);
    int setSsnSets(ssnSets *inSsnSets, u_short usNum);

    // ��ȡ�����ô�����Ʋ����ú�ʵʱ����ֵ
    void* getTranCtrlCfg();
    int setTranCtrlCfg(void *tranCtrlCfg_p);

    // ʵʱ����ָ��
    void* getRealParam();
    int setRealParam(void *realParam_p);

    // 	// ��ȡ������k��w����ֵ
    // 	int getKandW(u_short &usK, u_short &usW);
    // 	int setKandW(u_short usK, u_short usW);

    // ��ȡ�����ô�����Ʋ㳬ʱʱ�䡢��ʱ��Դʱ��ͨ��ָ��Ϳ�����
    int getOutTime(char tmCode);
    int setOutTime(char tmCode, short newValue);
    int getTimer(tmChnl*& tmChnl_p);
    int setTimer(tmChnl* tmChnl_p);
    int getCtrlCode(char tmCode, char &ctrlCode);
    int setCtrlCode(char tmCode, char ctrlCode);

    // ��ȡ�����ý�ɫ
    int getRole();
    int setRole(short &role);

    // ��ȡ�ʹ������Ӿ��
    void* getSockHdl();
    int setSockHdl(void* sockHdl_p);

    // ��ȡ�����ñ��������ַ
    int getLocNetAddr(addrItem &locAddr);
    int setLocNetAddr(addrItem &locAddr);

    // ��ȡ�����ô������֡��ASDU������
    void* getTranCtrl();
    int setTranCtrl(char* tranCtrlBuf_p, int iLen);
    int getASDUDsc();
    int setASDUDsc();

    // ��ȡ������������Զ��վ
    C_StnObj* getStn();
    int lockStn(stnDsc* stnDsc_p, stnOrVew* stnOrVew_p);
    int unLockStn();

    // ��ȡ����������״̬
    int getChnlState(char &cState);
    int setChnlState(char cState);

    // �����ȥ���Ựʵ������֤�����Ự�ĵ�ʵ���������Լ��ǿ����ԣ�
    int addSsn(ssnCtlBlk* ssnCtlBlk_p, u_int uiProi);
    int delSsn(ssnCtlBlk* ssnCtlBlk_p, u_int uiProi, bool bNoDel = false);

    // ��ȡ�Ựʵ����ָ��
    ssnTbl* getSsnTbl();

    // ��ȡ�����Ựʵ�����ƿ�ָ��
    ssnCtlBlk* psvSsnCtlBlk(u_int uiProi);

    // ��ȡ�����Ựʵ�����ƿ�ָ�롢���ߵ�ʵ���Ự���ƿ�ָ��
    ssnCtlBlk* actSsnCtlBlk(u_int uiProi);

    // 
    ssnInstRslt* getActSsnRslt(u_short stnID);//lzq

    // ��Լ����
    HMODULE* getPtclMdl();
    //����ͨѶЭ��������
    HMODULE* getCommMdl();
    //setԶ��վ����ָ��
    int setStnObj(C_StnObj* stnObj);
    //getԶ��վ����ָ��
    C_StnObj* getStnObj();

    //setԶ��վ�ص�����
    int setRtnfun(void (*rtnFun)(void *));
    //getԶ��վ�ص��ú���
    void* getRtnfun();

    // ���ն����ݻ�����ָ�� ...
    void* getRcvBufPtr();
    int setRcvBufPtr(void *rcvBufPtr[], char bufNum);

    //���Ͷ����ݻ���ָ��
    void* getSendBuf();
    int setSendBuf(void *sendBuf[], char bufNum);

};

// 
struct addrRange			// ADDR
{
    u_short useNum;			// ʵ��Ӧ�õ���
    u_short	begAddr;		// ��ʼ��ַ������Ϊ��
    u_short	endAddr;		// ������ַ������Ϊ��
};
class C_DStorage
{
private:
    //	short		m_minThres;			// ��С��ֵ�����С�ڸ���ֵ��ϲ����ݵ�ַADDR �����������úϲ���
    short		m_blkLen;			// ÿ�����ݿ鳤��
    short		m_blkNum;			// ���ݿ������Ĭ�����20���ɸ���ʵ���������
    union usrData**	m_blkMem_pp;		// ���ݿ�������<���,���׵�ַ>��Ϊ�˿��ٶ�λ���ݿ�
    char		m_usrType;			//˵��usrdata��������

    short		m_arLen;			// ADDR��������,m_addrIdxΪ����,�˱���Ϊ����ĳ���
    addrRange*	m_addrIdx;			// ���ADDR������<��ţ�addrRange>
    CRITICAL_SECTION 	memKey;		//mutex of G
public:
    // ��ʼ��
    C_DStorage();
    C_DStorage(addrRange* cfgAddr_p, short cfgLen, u_char usrType);
    ~C_DStorage();

    // ucMask ȡֵ���룬Ĭ��255��ȫ��ȡ������Ӧ��ֵ���Ӧ����λ����λ��Ϊ1��ȡ����Ӧ����ֵ��0����Ҫ
    int loadStnData(T_snnPara* ssnPara_p, u_short ucMask);

    // ���ԣ�����������addr������ADDR������ȷ�������ڴ洢�е�ӳ��λ�ã���š����ݿ���λ���±꣩
    //		 Ȼ����ҵ����ݿ���������λ����ӳ��λ�ã�����Ӧ���ݴ洢����
    // snnPara_p�����ݽṹ�д���������ݣ������ݲ���
    int storeStnData(T_snnPara* ssnPara_p, u_short ucMask);

    int getData(union usrData* blkMem_p , u_int addr);
};

// 
class C_StnObj // Զ��վ����
{
public:
    // ���ɺ�ɾ��Զ��վ
    C_StnObj();
    ~C_StnObj();

public:
    // ��ʼ��Զ��վ�������м��״洢��ע���˽ӿ�ÿ��ֻ�ܳ�ʼ��һ�����ݣ���������ּ��������ݵĻ�����Ҫ���ö��
    int initDStorage(stnDsc* stnDsc_p, addrRange* cfgAddr_p, short cfgLen, u_short usrType);

    // Զ��վ����
    int reSet();

    // ��ȡ������Զ��վ��
    int getStnID(stnDsc* stnDsc_p);
    int setStnID(stnDsc* stnDsc_p, short stnID);

    // ��ȡ���������Ӻ�
    int getChnlID(stnDsc* stnDsc_p);
    int setChnlID(stnDsc* stnDsc_p, short chnlID);

    // ��ȡ������Զ��վ��
    int getStnName(stnDsc* stnDsc_p, char *stnName_p, int &iLen);
    int setStnName(stnDsc* stnDsc_p, char *stnName_p, int iLen);

    // ��ȡ������Զ��վ��ɫ
    char getStnRole(stnDsc* stnDsc_p);
    int setStnRole(stnDsc* stnDsc_p, char stnRole);

    // ��ȡ�߼�վָ��
    int getLogicStn(short stnID, stnDsc* lgcStn_p);

    // ����߼�վ���
    int addLogicStn(stnDsc* dstStn_p, stnDsc* addStn_p);
    //	int delLogicStn(stnDsc* dstStn_p, stnDsc* delStn_p);

    // �Ǽǡ�ע��������
    int loginInfoTbl(stnDsc* stnDsc_p, void* infoTbl_p);
    int logoffInfoTbl(stnDsc* stnDsc_p);

    // ���ú����ѭ�����ݴ����ʶ
    int setCleFlag(stnDsc* stnDsc_p);
    int delCleFlag(stnDsc* stnDsc_p);

    // ������ͷ�������ٻ�����
    int crtJumpFlw(stnDsc* stnDsc_p);
    int rlsJumpFlw(stnDsc* stnDsc_p);

    // ������ͷŴ洢ӳ�����Ǹ�����������
    int crtStrgMap(stnDsc* stnDsc_p);
    int rlsStrgMap(stnDsc* stnDsc_p);

    // ��ȡ��һԶ��վָ��
    int getNextStn(stnDsc* stnDsc_p, stnDsc* retStn_p);

    // �Ǽ���Ϣ��������ص�����
    int loginJumpFun(stnDsc* stnDsc_p, FunTypeOne jumpFun);

    // ��ȡ��Ϣ��������ص�����ָ��
    FunTypeOne getJumpFun(stnDsc* stnDsc_p);

    // ��Ϣ���������¼����
    int execRcdExpFun(stnDsc* stnDsc_p, void* exeParam_p);

    // ��ȡ��Ϣ���������¼����ָ��
    FunTypeOne getRcdExpFun(stnDsc* stnDsc_p);

    // ��ȡ������Զ��վ֧�ֵĻỰ����Ϣ
    int getSsnInfo();
    int setSsnInfo(void* ssnInfo_p);

    // �������Զ��վ���󣨰���Զ��վ���������У�
    int addPscStn(stnDsc* stnObj);

    // ɾ��Զ��վ���߼�or����
    int delStn(short stnID);

    // �õ�Զ��վ������ָ��
    stnDsc* getStnDsc(short stnID);
public:
    // ��ȡ�������
    int loadStnData(T_snnPara* ssnPara_p, short stnID, u_char ucType, u_short ucMask , int iPtclID);

    // �洢�������
    int storeStnData(T_snnPara* ssnPara_p, short stnID, u_char ucType, u_short ucMask , int iPtclID);

    // ��ȡ�洢����ָ��
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
