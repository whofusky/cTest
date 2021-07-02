#pragma once
#include "cmuSkt.h"
#include "common_stru.h"

#define SUBSYSNUM	(16)
#define INVLD_CHNL (0)

class cIface
{
private:
	int m_appSrcID;//ԴӦ��ID��ͨ�����ݵķ�����Ӧ��;

	//֧��ͬһԴӦ���Զ��߳���ʽ��ͬʱʹ�ò�ͬ��Ӧ��ͨ����SUBSYSNUM��Ŀ����ϵͳ����ͨ��
	//����һ���߳�����ʹ�ö��Ӧ��ͨ������������Ӧ��ͨ���ɶ���߳�ʹ��
	//������ΪĿ����ϵͳ�ţ�appTgtID��
	appChnl_T m_appChnl[SUBSYSNUM];//����������ϵͳͨ�ţ������Լ��������Լ�ʱʹ�ù����ڴ�
	appChnl_T m_anyTmChnl;//ÿ������ֻ��һ�����ڽ��ռ�ʱ���ݵ�ͨ��ͨ����ֻ����������ΨһӦ��ͨ��
	appChnl_T m_fixTmChnl;//ÿ������ֻ��һ�����ڽ��ն�ʱ���ݵ�ͨ��ͨ����ֻ����������ΨһӦ��ͨ��

public:
	cIface(int appSrcID);
	~cIface();

	//����appTgtIDΪͨ��������<0��Ч��
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

