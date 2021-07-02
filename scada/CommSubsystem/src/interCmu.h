#pragma once

#include <semaphore.h>
#include <netinet/in.h>
#include "cmuSkt.h"


#define MAX_SVRCNCT_NUM	20	//���������������ܵ����������
//���������ӹ�����
class cmuSvrCnctMgr
{
private:
	//host
	int m_subSysID;
	pthread_mutexattr_t tMutexAttr;
	pthread_mutex_t mutex;

	//����
	struct sockaddr_in lsnSoketIn;
	int lsnSocket;

	//����
	int maxAcptCnctNum;//�����ܵ�������
	int usingAcptCnctNum;//��ǰ���õĽ���������
	int usingComuTidNum;
	int comuTmOut;//����ͨ�ų�ʱʱ�䣬������̬�޸ģ���λ��
	struct  sockaddr_in acptCnctStore[MAX_SVRCNCT_NUM];//�����������ͨ�ŵ�ַ������
	int comuSocket[MAX_SVRCNCT_NUM];
	pthread_t comuTidStore[MAX_SVRCNCT_NUM];//���ض�����������ִ������ͨ�ŵ��߳�id����

public:
	cmuSvrCnctMgr(char* lsnIp, int lsnPort, int tmOut, int subSysID);//ʵ�����������ļ�
	~cmuSvrCnctMgr(void);

	int regAcptCnct(int socket, struct  sockaddr_in* socketAddrIn);
	int regComuTid(int pasvCnctRef, pthread_t comuTid);
	int logoutAcptCnct(int pasvCnctRef, int flag);
	void logoutALL();
	int getUsingCnctNum(int flag);
	int startSvrCnctMgr(void);

private:
	int setComuCnct(int comuSocket, socketOptArg* setOptLnk);
    int setSocketOpt(int socket, socketOptArg* setOptLnk);
};

void*  socketComuThread(void* arg);












