#pragma once
#include "cmuSkt.h"
#include "common_stru.h"

#define SUBSYSNUM	(16)
#define INVLD_CHNL (0)

class cIface
{
private:
	int m_appSrcID;//源应用ID，通信数据的发送者应用;

	//支持同一源应用以多线程形式，同时使用不同的应用通道与SUBSYSNUM个目标子系统独立通信
	//允许一个线程依次使用多个应用通道，不允许多个应用通道由多个线程使用
	//索引号为目标子系统号（appTgtID）
	appChnl_T m_appChnl[SUBSYSNUM];//可与任意子系统通信，包括自己，当是自己时使用共享内存
	appChnl_T m_anyTmChnl;//每个对象只有一个用于接收即时数据的通信通道，只与数据中心唯一应用通信
	appChnl_T m_fixTmChnl;//每个对象只有一个用于接收定时数据的通信通道，只与数据中心唯一应用通信

public:
	cIface(int appSrcID);
	~cIface();

	//输入appTgtID为通信索引，<0无效）
    int istlAppChnl(int appTgtID, char* lIP, int lPrt);
    int istlFixTmChnl(char* lIP, int lPrt);
    int istlAnyTmChnl(char* lIP, int lPrt);

	int ustlAppChnl(int appTgtID);
	int ustlFixTmChnl();
	int ustlAnyTmChnl();
	appChnl_T* getAppChnl(int appTgtID);
	appChnl_T* getFixTmChnl();
	appChnl_T* getAnyTmChnl();

    int setAppChnl(int appTgtID, char* rIP, int rPrt, int rcvTmOut, int sndTmOut, int cnctFlg);
    int setFixTmChnl(char* rIP, int rPrt,  int rcvTmOut, int sndTmOut, int cnctFlg);
    int setAnyTmChnl(char* rIP, int rPrt,  int rcvTmOut, int sndTmOut, int cnctFlg);

	//trans-interface
    int SQLInsrtDBSsn(unsigned char* usrData, int usrdDataLng,  int rcvTmOut, int sndTmOut, int cnctFlg);
    int SQLSlctDBSsn(unsigned char* inOutUsrData, int* inOutUsrDataLng_p,  int rcvTmOut, int sndTmOut, int cnctFlg);
    int dItemInsrtDBSsn(unsigned char* usrData, int usrdDataLng, pDBADFD_T* reqPktDF_p, int rcvTmOut, int sndTmOut,int cnctFlg);
    int dItemSlctDBSsn(unsigned char* inOutUsrData, int* inOutUsrdDataLng_p, int bufferLength , pDBADFD_T* reqPktDF_p, int rcvTmOut, int sndTmOut,int cnctFlg);
};

