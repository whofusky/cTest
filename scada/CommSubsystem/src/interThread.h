#pragma once
#include "interPtcl.h"

typedef struct
{
	int socket;
	int noticeRef;
	int comuTmOut;
}threadArg;

void*  socketComuThread(void* arg);
int reSendMissFrames(int srvSocket,unsigned int rSndMsk,pktHdInfo pktInfo, pktDBuf* sndBuf);


