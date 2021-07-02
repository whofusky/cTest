#pragma once

#include <semaphore.h>
#include <netinet/in.h>
#include "cmuSkt.h"


#define MAX_SVRCNCT_NUM	20	//定义服务器允许接受的最大连接数
//服务器连接管理类
class cmuSvrCnctMgr
{
private:
	//host
	int m_subSysID;
	pthread_mutexattr_t tMutexAttr;
	pthread_mutex_t mutex;

	//监听
	struct sockaddr_in lsnSoketIn;
	int lsnSocket;

	//连接
	int maxAcptCnctNum;//最大接受的连接数
	int usingAcptCnctNum;//当前在用的接受连接数
	int usingComuTidNum;
	int comuTmOut;//数据通信超时时间，不允许动态修改，单位秒
	struct  sockaddr_in acptCnctStore[MAX_SVRCNCT_NUM];//保存接受连接通信地址的数组
	int comuSocket[MAX_SVRCNCT_NUM];
	pthread_t comuTidStore[MAX_SVRCNCT_NUM];//在特定接受连接上执行数据通信的线程id数组

public:
	cmuSvrCnctMgr(char* lsnIp, int lsnPort, int tmOut, int subSysID);//实参来自配置文件
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












