#pragma once

struct socketOptArg
{
	int optname;
	void *optval;
	socklen_t optlen;
	socketOptArg* nxt;
};



int setSocketOpt(int socket, socketOptArg* setOptLnk);
void closeComuSocket(char* why, int errNo, int socket);
int rcvFrame(int rcvSocket, unsigned char*buf, int size);
int sndFrame(int sndSocket, unsigned char*buf, int size);
void clrSktRcvBuf(int rcvSocket);
