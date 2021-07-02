#pragma once

#include <unistd.h>
#include <string.h>
#include "packet_comm.h"

#define	MAXINITSOCKRETRY	3

class sockComm
{
public:
    sockComm(int sock_timeout);

    virtual ~sockComm();

    void setSock(int sock);

    int timeOut;			//读写超时时间 单位秒 若传入0则默认2s
    int sockfd;

    //sock通信主动模式，从发送数据开始
    //sockfd		通信的socket
    //rowData		传入的原始数据长
    //返回值		0正常   负数是错误码
    int sockCommunicationActive(void *structSet, int structLen,
                                unsigned char controlCode, int srcAppCode, int dstAppCode, int commType,
                                DATEFORMAT_T startTime, DATEFORMAT_T endTime, unsigned int step, DATEFORMAT_T forcastTime);


    //sock通信被动模式，从接收数据开始
    //sockfd	通信socket
    //rowData	传出内容
    //返回值		0正常	负数是错误码
    int sockCommunicationPassive(void *structSet,  int * structLen,
                                 unsigned char *controlCode, int srcAppCode, int *dstAppCode, int *commType,
                                 DATEFORMAT_T* startTime, DATEFORMAT_T* endTime, unsigned int* step, DATEFORMAT_T*forcastTime);

    //销毁socket（客户端建联的socket，不是listen的socket）
    int sockCommEnd();
};


class sockCommActive:public sockComm
{
private:
	int serverPort;			//服务器PORT  本类是socket户端
    char serverIp[32];		//服务器IP
    int localPort;	       //local PORT
    char localIp[32];		//local  IP

public:
	//serIp		服务器IP
	//serPort	服务器PORT
	//_timeOut	超时时间 单位秒
    sockCommActive(char* serIp, int serPort, char* localIp, int localPort, const int _timeOut);

    //建立主动socket通信(客户端请求建联)
	//从connet开始，返回连接fd，即用于可通信
	int sockInitActive(void);

};

class sockCommPassive:public sockComm
{
private:
	int listenPort;				//监听PORT  本类是socket服务端
	char listenIp[32];			//监听IP
	int slisten;				//监听socket
    char clientIp[32];

public:

	//listen_port	监听端口号
	//listen_ip		监听IP	若传入“0.0.0.0”表示监听本机所有IP
    sockCommPassive(char* listen_ip, int listen_port, const int _timeOut);

	~sockCommPassive(void);

	//接收客户端建联请求
	//调用一次 得到一个通信通道（客户端连接）
	//返回值		正数是fd 正常	负数是错误码
	int sockAcceptPassive(void);

	//建立被动socket通信（服务端）
	//从listen开始，Accept后 方可通信
	//返回值		0正常	负数是错误码
	int sockInitPassive(void);

};


