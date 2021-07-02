#pragma once

#include <sys/socket.h>
#include <netinet/in.h>

#define INVLD_SKT (-1)
#define INVLD_TMOUT (-1)
#define SYS_RCVBUF	(0)//avoid sys_rcvbuf: socket_rcvbuf->[sys_rcvbuf]->usr 
#define SYS_SNDBUF	(0)//avoid sys_sndbuf: usr->[sys_sndbuf]->socket_sndbuf
#define INVLD_NETADDR ((unsigned short)(-1))


struct socketOptArg
{
	int optLvl;
	int optname;
	void *optval;
	socklen_t optlng;
	socketOptArg* nxt;
};

enum SKT_FLG
{
	SKT_LCL = 1,
	RNET_ADDR = 2,
	TM_RCV = 4,
	TM_SND = 8
};

class cmuSkt//通信而非监听socket
{
private:
	int m_lSkt;//本端通信socket
	struct sockaddr_in m_lCmuAddr;
	struct sockaddr_in m_rCmuAddr;

	int m_rcvTmOut;//本端接收超时时间，毫秒
	int m_sndTmOut;//本端发送超时时间，毫秒
	bool m_rcvTmVld;//本端接收超时时间生效标记：0未生效，1生效
	bool m_sndTmVld;//本端发送超时时间生效标记：0未生效，1生效
	bool m_sigInstl;//信号安装标记：0未安装，1已安装
	bool m_cnct;
public:
    cmuSkt(char* lIP, int lPrt);//(char* rIP, int rPrt, char* lIP, int lPrt, int rcvTmOut, int sndTmOut);
	~cmuSkt();
    int crtSkt(int rcvTmOut, int sndTmOut);

	int setRNetAddr(char* rIP, int rPrt);
	void setSktTm(int rcvTmOut, int sndTmOut);//设置本端socket的接收数据和发送数据超时时间
	void clrCmuPeers(SKT_FLG clrFlg);
	void isSktTmVld(bool* rcvTmVld, bool* sndTmVld);
	bool isInstlSig();
	bool isLNetAddr();
	bool isRNetAddr();
	bool isCnct();
	void getCmuPeers(int* lSkt, sockaddr_in* lCmuAddr, sockaddr_in* rCmuAddr);
    int setLSkt(int lSkt);
	void getSktTmOut(int* rcvTm, int* sndTm);

	int setSktOpts(socketOptArg* setOptLnk);
	int clrSktRcvBuf();
    int opnCmuSkt();
    void clsCmuSkt(char* why, int errNo);
	int rcvFrm(unsigned char* buf, int sz);
	int sndFrm(unsigned char* buf, int sz);
};


