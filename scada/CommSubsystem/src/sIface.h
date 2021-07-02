#pragma once
#include "cmuSkt.h"
#include "common_stru.h"

#define SUBSYSNUM	(16)
#define INVLD_CHNL (0)

#define TRANSTOKEN_SRV	MNGR_CMUTYP //manage packet
#define EXECFGCMD_SRV	CFGCMD_RSPD_CMUTYP //config and commond respond packet
#define EXEREQACS_SRV	REQACS_RSND_CMUTYP //require access respond packet
#define GETFIXTM_SRV	FIXTM_CMUTYP //fix time packet
#define GETANYTM_SRV	ANYTM_CMUTYP //any time packet

#ifndef  DCNTR_APP
#define DCNTR_APP		(0)
#endif

typedef int(*svr_T)(unsigned char* inOutData_p, int* inOutBufLng_p, int oprCd);
typedef int(*transToken_T)(unsigned char* inOutData_p, int* inOutBufLng_p, int oprCd);
typedef int(*exeCmdAndCfg_T)(unsigned char* inOutData_p, int* inOutBufLng_p, int oprCd);
typedef int(*exeReqAcs_T)(unsigned char* inOutData_p, int* inOutBufLng_p, int oprCd);
typedef int(*getFixTmData_T)(unsigned char* inOutData_p, int* inOutBufLng_p, int oprCd);
typedef int(*getAnyTmData_T)(unsigned char* inOutData_p, int* inOutBufLng_p, int oprCd);

class sIface
{
private:
//	int m_lSkt;//用于快速访问
//	int m_tmOut;
	int m_appSrcID;//源应用ID，通信数据的发送者应用;

	appChnl_T m_appChnl;//一个连接只响应一个会话，因此只有一个应用通道，该通道可与任意子系统通信
	appChnl_T m_smChnl;//共离内存通道，当远程使用自己的应用ID与服务端通信时，使用此通道
	appChnl_T m_anyTmChnl;//每个对象只有一个用于接收即时数据的通信通道，只与数据中心唯一应用通信
	appChnl_T m_fixTmChnl;//每个对象只有一个用于接收定时数据的通信通道，只与数据中心唯一应用通信
	unsigned char* m_appUsrData_p;
	int m_appUsrDataLng;
	svr_T m_svrTbl[7];

public:
	sIface(int appSrcID);
	~sIface();

	int regSvr(int srvNo, svr_T fun);

    int istlAppChnl(char* lIP, int lPrt, int lSkt);
    int istlSmChnl(char* lIP, int lPrt, int lSkt);
    int istlFixTmChnl(char* lIP, int lPrt, int lSkt);
    int istlAnyTmChnl(char* lIP, int lPrt, int lSkt);

	int ustlAppChnl();
	int ustlSmChnl();
	int ustlFixTmChnl();
	int ustlAnyTmChnl();

	appChnl_T* getAppChnl();
	appChnl_T* getSmChnl();
	appChnl_T* getFixTmChnl();
	appChnl_T* getAnyTmChnl();

    int setAppChnl(char* rIP, int rPrt, int rcvTmOut, int sndTmOut);
	int setSmChnl(char* rIP, int rPrt, int rcvTmOut, int sndTmOut);
	int setFixTmChnl(char* rIP, int rPrt, int rcvTmOut, int sndTmOut);
	int setAnyTmChnl(char* rIP, int rPrt, int rcvTmOut, int sndTmOut);

	int svrRspd(unsigned char* usrReqData_p, int usrReqDataLng);
   };

