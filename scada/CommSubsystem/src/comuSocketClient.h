#pragma once


#include <sys/time.h>
#include <sys/socket.h>
#include "stdio.h"
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include "comuSocket.h"
#include "common_comm.h"
#include <string>
#include  "interPtcl.h"



enum
{
    PRESENTATION_SCADA,               //SCADA config
    PRESENTATION_SQL,                    //SQL
    PRESENTATION_TIMESEL,            // select
    PRESENTATION_UPFORCAST    //upload forcast


};
//int presentation_to_data(void *addr, int *structLen, rowData_t *rowData, DATEFORMAT_T *startTime, DATEFORMAT_T*endTime, unsigned int*step, DATEFORMAT_T*forcastTime);
//int data_to_presentation(void * addr, int len, unsigned char controlCode, DATEFORMAT_T startTime, DATEFORMAT_T endTime, unsigned int step, DATEFORMAT_T forcastTime, rowData_t* rowData);
//int activeResponse( int sockfd,pktDBuf* tRcvBuf,pktDBuf* sndBuf,int flagRepose,unsigned char controlCode);
//int receiveSelectResult( int sockfd,pktDBuf* tRcvBuf,pktDBuf* sndBuf,int flagRepose,unsigned char controlCode);
//int activeResponse( int srvSocket, unsigned char *result, int *resultLen,pktDBuf* tRcvBuf,pktDBuf* sndBuf,unsigned char controlCode);
// int dealResponseStr( unsigned char *result, int *resultLen,pktDBuf* rcvBuf);
//int receiveSrvResponse( int srvSocket,pktDBuf* rcvBuf,pktHdInfo tPktInfo,unsigned int *ReSndMsk);

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
    int serverPort;			//服务器PORT  本类是socket客户端
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
    int sockRequestAndResult(void *requestStr,int requestLen, unsigned char *result, int *resultLen,
                                unsigned char controlCode, int srcAppCode, int dstAppCode, int commType,
                                DATEFORMAT_T startTime, DATEFORMAT_T endTime, unsigned int step, DATEFORMAT_T forcastTime);

    int sockInitActive(void);

private:
    int sendRequest(pktDBuf* sndBuf,void *requestStr,int requestLen, int srcAppCode, int dstAppCode, int commType,unsigned char controlCode,int * flagRepose,
                                                    DATEFORMAT_T startTime, DATEFORMAT_T endTime, unsigned int step, DATEFORMAT_T forcastTime);


    int activeResponse( int srvSocket, unsigned char *result, int *resultLen,pktDBuf* tRcvBuf,pktDBuf* sndBuf,unsigned char controlCode);
    int dealResponseStr( unsigned char *result, int *resultLen,pktHdInfo pktInfo,pktDBuf* rcvBuf);
    int receiveSrvResponse( int srvSocket,pktDBuf* rcvBuf,pktHdInfo *tPktInfo,unsigned int *ReSndMsk);


    friend int setOnePacket(int srvSocket, pktDBuf* sndBuf, pktHdInfo* pktInfo);
    friend int rspdNAck(int srvSocket, unsigned int* rSndMsk, pktHdInfo* pktInfo, pktDBuf* sndBuf);
    friend int setOneFrame(int sndSocket, unsigned char*buf, int size);
    friend int getOnePacket(int srvSocket, int tmout, pktDBuf* rcvBuf, unsigned int* rSndMsk, pktHdInfo* pktInfo);
};
