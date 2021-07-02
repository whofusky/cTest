/*
zhaojinjie, 2016.09.12
FUN: socketͨ�ţ���ƽ̨����ʵ��socket���������ա����͵���Ӧͨ�Ź���
*/
#ifndef CP_SOCKET_H
#define CP_SOCKET_H

#include "CPInt.h"
#include "CPPub.h"
#include "CPThread.h"
//#include "CPQueue.h"

#include <limits.h>

#ifdef WIN32
#define WINDOWS_PLATFORM
#endif

#ifdef WIN64
#define WINDOWS_PLATFORM
#endif

#ifdef WINDOWS_PLATFORM //��WINDOWSƽ̨
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include "mswsock.h"
typedef SOCKET sock_descriptor;
typedef int socklen_t;
#define sock_invalid INVALID_SOCKET
#define sock_error SOCKET_ERROR
#define CloseSocket(sock) closesocket(sock)
typedef void (PASCAL FAR *GETACCEPTEXSOCKADDRS)(PVOID, DWORD, DWORD, DWORD, LPSOCKADDR*, LPINT, LPSOCKADDR*, LPINT);
typedef WSAEVENT EVENT;
#else
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <sys/epoll.h>
typedef int sock_descriptor;
#define sock_invalid (-1)
#define sock_error (-1)
#define CloseSocket(sock) close(sock)
#define ioctlsocket ioctl
typedef int EVENT;
#define __FD_SETSIZE 1024
#endif

#define FOREVER_TIME_OUT	(30*24*60*60)
#define GROP_CHNL_SIZE  64    // һ���̹߳���ͨ�����������
namespace nsCPSocket
{

//����Socket����
class CSock
{
public:
	CSock()
	{
		//printf("CSock called\n");
#ifdef WINDOWS_PLATFORM
		WSADATA WsaData;
		WSAStartup(MAKEWORD(2,2), &WsaData);
#else
		//signal(SIGPIPE,SIG_IGN);    //ignore signal SIGPIPE
		sigset_t signal_mask;
		sigemptyset(&signal_mask);
		sigaddset(&signal_mask, SIGPIPE);
		if(pthread_sigmask(SIG_BLOCK, &signal_mask, NULL))
		{
			printf("block sigpipe error\n");
			return;
		}
#endif
	}
	~CSock()
	{
#ifdef WINDOWS_PLATFORM
		WSACleanup();
#endif
	}
	//FUNC  ���ó�ʱʱ��
	//PARAM sock��socket
	//      iRecvTimeout�����ճ�ʱ����λΪ����
	//      iSendTimeout�����ͳ�ʱ����λΪ����
	//RET   0�����óɹ�
	//      ���������ô���
	int SetTimeout(sock_descriptor sock, int iRecvTimeout, int iSendTimeout)
	{
		if (sock == sock_invalid)
		{
			return -1;
		}

#ifdef WINDOWS_PLATFORM
		//���ó�ʱʱ��
		int iRet = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&iRecvTimeout, sizeof(int));
		if (iRet == sock_error)
		{
			return -10;
		}

		iRet = setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char *)&iSendTimeout, sizeof(int));
		if (iRet == sock_error)
		{
			return -20;
		}
#else
		struct timeval sOverTime;
		sOverTime.tv_sec = iRecvTimeout/1000;
		sOverTime.tv_usec = (iRecvTimeout%1000)*1000;
		int iRet = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&sOverTime, sizeof(sOverTime));
		if (iRet == sock_error)
		{
			return -10;
		}

		sOverTime.tv_sec = iSendTimeout/1000;
		sOverTime.tv_usec = (iSendTimeout%1000)*1000;
		iRet = setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&sOverTime, sizeof(sOverTime));
		if (iRet == sock_error)
		{
			return -20;
		}

#endif
		return 0;
	}
	//FUNC  ��������
	//PARAM sock��socket
	//      pSendBuf����������������Buf
	//      TotalSendLen�����������ݳ���
	//RET   >=0���ɹ����͵��ֽ���
	//      ��������������
	//NOTE  ���سɹ����͵��ֽ�������С��TotalSendLen
	int BlockingSend(sock_descriptor sock, const char *pSendBuf, size_t TotalSendLen)
	{
		if (sock == sock_invalid)
		{
			return -1;
		}

		size_t HadSendLen = 0;
		int iSendLen = 0;
		do
		{
			iSendLen = send(sock, pSendBuf+HadSendLen, TotalSendLen-HadSendLen, 0);
			if (iSendLen > 0)
			{
				HadSendLen += iSendLen;
			}
			else
			{
				printf("iSendLen=%d, LastError=%d\n", iSendLen, nsCPPub::LastError());
			}
		} while(iSendLen > 0 && HadSendLen < HadSendLen);

		return HadSendLen;
	}
	//FUNC  ��������
	//PARAM sock��socket
	//      pRecvBuf������Buf
	//      TotalRecvLen���������յ������ܳ�
	//RET   >=0���ɹ����յ��ֽ���
	//      ��������������
	//NOTE  ���سɹ����յ��ֽ�������С��TotalRecvLen
	int BlockingRecv(sock_descriptor sock, char *pRecvBuf, size_t TotalRecvLen, int iReportLoop = 100)
	{
		if (sock == sock_invalid)
		{
			return -1;
		}

		//ȡ��ʱֵ
		int iRecvTimeout = 0;
#ifdef WINDOWS_PLATFORM
		int iSize = sizeof(iRecvTimeout);
		int iRet = getsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&iRecvTimeout, &iSize);
		if (iRet == sock_error)
		{
			iRecvTimeout = 5*60*1000;
		}
#else
		struct timeval sOverTime = {0};
		socklen_t iSize = sizeof(sOverTime);
		int iRet = getsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&sOverTime, &iSize);
		if (iRet == sock_error)
		{
			iRecvTimeout = 5*60*1000;
		}
		else
		{
			iRecvTimeout = sOverTime.tv_sec*1000+sOverTime.tv_usec/1000;
		}
#endif
		int iLoopCount = 0;

		size_t HadRecvLen = 0;
		int iRecvLen = 0;
		int iTimeRecvBeg = nsCPPub::GetTickCnt();
		do
		{
			iRecvLen = recv(sock, pRecvBuf+HadRecvLen, TotalRecvLen-HadRecvLen, 0);
			if (iRecvLen > 0)
			{
				HadRecvLen += iRecvLen;
			}
#ifndef WINDOWS_PLATFORM
			else if (iRecvLen < 0)
			{
				int iError = nsCPPub::LastError();
				if (iError == EINTR  || iError == EWOULDBLOCK || iError == EAGAIN)
				{
					if (++iLoopCount == iReportLoop)
					{
						printf("BlockingRecv::iRecvLen=%d, LastError=%d\n", iRecvLen, iError);
						iLoopCount = 0;
					}

					if (iRecvTimeout > 0)
					{
						//��鳬ʱֵ
						int iTimeRecvEnd = nsCPPub::GetTickCnt();
						if (iTimeRecvEnd-iTimeRecvBeg > iRecvTimeout)
						{//�Ѵﳬʱֵ
							break;
						}
					}
					nsCPPub::SleepMilliSecond(1);
					continue;
				}
			}
#endif
			else
			{
				//printf("iRecvLen=%d, LastError=%d\n", iRecvLen, nsCPPub::LastError());
				break;
			}
		} while(HadRecvLen < TotalRecvLen);

		return HadRecvLen;
	}
	//FUNC  �ر�socket
	//PARAM sock��socket
	void Close(sock_descriptor sock)
	{
		if (sock != sock_invalid)
		{
			shutdown(sock, 0x02);   //0x02 means SD_BOTH(windows) or SHUT_RDWR(linux)
			CloseSocket(sock);
		}
	}
	//FUNC  �õ���������ѡIP��ַ
	const char *GetIPStr()
	{
		char szHostName[1024] = {0};
		gethostname(szHostName, 255);

		struct hostent *pLocalHost = gethostbyname(szHostName);
		if (pLocalHost == NULL)
		{
			return NULL;
		}

		int i = 0;
		while (pLocalHost->h_addr_list[i] != NULL)
		{
			char *pIP = inet_ntoa(*((struct in_addr *)pLocalHost->h_addr_list[i]));
			if (memcmp(pIP, "127.", 4) != 0)
			{//isn't 127.*.*.*
				return pIP;
			}
			else
			{
				++i;
			}
		}

		return NULL;
	}
	//FUNC  ����socket���õ��Է�IP��ַ
	const char *GetIPStr(sock_descriptor sock)
	{
		struct sockaddr cIP;
		socklen_t iIPLen = sizeof(struct sockaddr);

		if (getpeername(sock, &cIP, &iIPLen) != 0)
		{
			return NULL;
		}

		return inet_ntoa(((struct sockaddr_in*)&cIP)->sin_addr);
	}
#ifdef WINDOWS_PLATFORM
	//FUNC  ȡ�����Ӷ�IP��ַ(��ɶ˿�ģ��)
	const char* GetIPStr(sock_descriptor in_sListen, char *buffer, GETACCEPTEXSOCKADDRS fpFunPoint = NULL)
	{
		int locallen, remotelen;
		SOCKADDR *pLocal = 0, *pRemote = 0;

		if (fpFunPoint == NULL)
		{
			GETACCEPTEXSOCKADDRS out_fpFunPoint = NULL;
			GetGetAcceptExSockaddrs(in_sListen, fpFunPoint);
		}

		fpFunPoint(buffer, 0, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, &pLocal, &locallen, &pRemote, &remotelen);

		SOCKADDR_IN addr;
		memcpy(&addr, pRemote, sizeof(sockaddr_in));

		return inet_ntoa(addr.sin_addr);
	}
#endif
protected:
private:
#ifdef WINDOWS_PLATFORM
	// �õ�GetAcceptExSockaddrs�ĺ���ָ��
	long GetGetAcceptExSockaddrs(sock_descriptor in_sListen, GETACCEPTEXSOCKADDRS& out_fpFunPoint)
	{

		DWORD dwRetVal = 0;
		GUID GUIDName = WSAID_GETACCEPTEXSOCKADDRS;
		if (WSAIoctl(in_sListen, SIO_GET_EXTENSION_FUNCTION_POINTER, &GUIDName, sizeof(GUIDName), &out_fpFunPoint, sizeof(out_fpFunPoint), &dwRetVal, NULL, NULL))
		{
			return -1;
		}

		return 0;
	}
#endif
};

static CSock s_SockObj; //just for call signal(SIGPIPE,SIG_IGN);    //ignore signal SIGPIPE

//TCP�����
class CTcpServer : public CSock
{
public:
	CTcpServer()
	{
		m_sockListen = sock_invalid;
		m_bRun = false;
	}
	~CTcpServer()
	{
		Cleanup();
	}
	//FUNC  ��������
	//PARAM u16Port�������Ķ˿�
	//      MaxAcceptQueueLen��Accept���г���
	//      pIP��������IP��ַ
	//RET   0�������ɹ�
	//      ����������ʧ�ܣ����ش�����
	int Startup(uint16_t u16Port, EVENT* eventArray, sock_descriptor* sockArray, u_short* sockPort , u_long* stnAddr, int* stnAddrNum, char *pIP = NULL )
	{
		this->m_eventArray_p = eventArray;
		this->m_sockArray_p  = sockArray;
		this->m_stnPort_p     = sockPort;
		this->m_stnAddrNum_p = stnAddrNum;
		this->m_stnAddr_p    = stnAddr;
#ifdef WINDOWS_PLATFORM

		//��������Socket
		m_sockListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

#else
		this->m_epfd_p = epoll_create(*stnAddrNum);

		//��������Socket
		m_sockListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


#endif // WINDOWS_PLATFORM
		if (m_sockListen == sock_invalid)
		{
			return -10;
		}

		//�γɷ�������ַ�ṹ
		struct sockaddr_in sServerAddr;
		memset(&sServerAddr, 0, sizeof(sockaddr_in));
		sServerAddr.sin_family = AF_INET;
		if (pIP != NULL)
		{
			sServerAddr.sin_addr.s_addr = inet_addr(pIP);
		}
		else
		{
			sServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		}
		sServerAddr.sin_port = htons(u16Port);

		//bind
		int iRet =/*0;// */bind(m_sockListen, (struct sockaddr *)&sServerAddr,  sizeof(sServerAddr));
#ifndef WINDOWS_PLATFORM
		struct epoll_event ev;						  //epoll�¼�
		memset(&ev , 0 , sizeof(epoll_event ));
		ev.data.fd = m_sockListen;
		//����Ҫ������¼�����
		ev.events = EPOLLIN | EPOLLOUT;
		epoll_ctl(m_epfd_p, EPOLL_CTL_ADD , m_sockListen , &ev);
#endif 
		if (iRet == sock_error)
		{
			return -20;
		}

		//listen
		iRet = listen(m_sockListen, INT_MAX);
		if (iRet == sock_error)
		{
			return -30;
		}

		//         //��ʼ������
		//         iRet = m_AcceptQueue.Init(sizeof(sock_descriptor)+sizeof(sockaddr_in), MaxAcceptQueueLen);
		//         if (iRet != 0)
		//         {
		//             return -40;
		//         }

		//���������߳�
		m_bRun = true;
		iRet = m_AcceptThread.StartThread(AcceptThread, this);
		if (iRet != 0)
		{
			return -50;
		}

		return 0;
	}
	//FUNC  ֹͣ����
	int Cleanup()
	{
		int addrNum = *(this->m_stnAddrNum_p);
		u_short num = addrNum / GROP_CHNL_SIZE;
		u_short modNum = addrNum % GROP_CHNL_SIZE;
		if (modNum != 0)
		{
			num += 1;
		}
		for (int i = 0; i < (num*GROP_CHNL_SIZE); i++)
		{
			if (this->m_sockArray_p[i] != sock_invalid)
			{
				this->Close(this->m_sockArray_p[i]);
				printf("Close fd \r\n");
				this->m_sockArray_p[i] = sock_invalid;
			}
		}

		if ( m_bRun )
		{
			m_bRun = false;
			m_AcceptThread.WaitForQuit();
		}

		if (m_sockListen != sock_invalid)
		{
			shutdown(m_sockListen, 0x02);
			Close(m_sockListen);
			m_sockListen = sock_invalid;
		}
		return 0;
	}
	//FUNC  �õ������ӵ���Ϣ
	//PARAM pClientAddr�������ӵĿͻ���ַ��Ϣ
	//RET   sock_invalid����ȡʧ��
	//      ��������ȡ�ɹ���Ϊ���Ӷ�Ӧ��socket
	sock_descriptor AcceptConn(struct sockaddr *pClientAddr = NULL)
	{
		char ConnInfo[sizeof(sock_descriptor)+sizeof(struct sockaddr)] = "";
		//size_t InfoLen = 0;

		//�Ӷ�����ȡ
		//         int iRet = m_AcceptQueue.DeQueue(ConnInfo, InfoLen);
		//         if (iRet != 0
		//             || InfoLen != sizeof(sock_descriptor)+sizeof(struct sockaddr))
		//         {
		//             return sock_invalid;
		//         }

		sock_descriptor sockConn = *(sock_descriptor*)ConnInfo;
		if (pClientAddr != NULL)
		{//��Ҫ�ͻ���ַ��Ϣ
			memcpy(pClientAddr, ConnInfo+sizeof(sock_descriptor), sizeof(struct sockaddr));
		}
		return sockConn;
	}
	sock_descriptor AcceptConn(int iTimeOut)
	{

		sock_descriptor sockConn;
		struct sockaddr sClientAddr;
		fd_set fdRead;

		FD_ZERO(&fdRead);
		FD_SET(m_sockListen, &fdRead);
		struct timeval sTimeOut;
		sTimeOut.tv_sec = iTimeOut;
		sTimeOut.tv_usec = 0;
		//select on fdRead
		if (select(m_sockListen+1, &fdRead, NULL, NULL, &sTimeOut) <= 0)
		{

			//	nsCPPub::SleepMilliSecond(1);
			return sock_invalid;
		}

		if (!FD_ISSET(m_sockListen, &fdRead))
		{
			return sock_invalid;
		}

		socklen_t iAddrSize = sizeof(sClientAddr);
		sockConn = accept(m_sockListen, &sClientAddr, &iAddrSize);
		if (sockConn == sock_invalid)
		{
			return sock_invalid;
		}

		return sockConn;
	}
protected:
	//Accept�̣߳����ڽ�������Ϣ�������
	static void AcceptThread(void *pParam)
	{
		struct sockaddr_in clientAddr;
		int clientLen = 0;
#ifdef WINDOWS_PLATFORM
		CTcpServer *pThis = (CTcpServer *)pParam;
		int nEvent = 0; //�¼�������������
		EVENT Event = WSACreateEvent();
		int snum = WSAEventSelect(pThis->m_sockListen, Event, FD_ACCEPT);
		nEvent ++;
		u_long locAddr = inet_addr("127.0.0.1");

		int iCnt = 0;
		int iArrLen = 0;
		while(pThis->m_bRun)
		{
			int Index = WSAWaitForMultipleEvents(nEvent, &Event, false, WSA_INFINITE, false);

			if (Index == WSA_WAIT_IO_COMPLETION || Index == WSA_WAIT_TIMEOUT)
			{
				break;
			}

			Index = Index - WSA_WAIT_EVENT_0;
			WSANETWORKEVENTS netEvent;
			WSAEnumNetworkEvents(pThis->m_sockListen, Event, &netEvent);

			if ( !(netEvent.lNetworkEvents & FD_ACCEPT) )
			{
				continue;
			}
			if (netEvent.iErrorCode[FD_ACCEPT_BIT] == 0)
			{
				iArrLen = *(pThis->m_stnAddrNum_p)/64;
				iArrLen += *(pThis->m_stnAddrNum_p)%64 > 0 ? 1 : 0;
				if ( Index > nEvent /*|| ( (iCnt > iArrLen*64) && pThis->m_stnAddr == NULL)*/ )
				{
					nsCPPub::SleepMilliSecond(1);
					continue;
				}
				iCnt++;

				sock_descriptor sNew = accept(pThis->m_sockListen ,  (struct sockaddr*)&clientAddr, (socklen_t*)&clientLen);

				//u_long addr = inet_addr(pThis->GetIPStr(sNew));
				u_long addr = clientAddr.sin_addr.S_un.S_addr;
				u_short accPort = ntohs( clientAddr.sin_port);
				short stChnlNo = addr != locAddr ? -1 : iCnt-1; // ��Զ��վ��ϢУ��ʱ��������У�飬zhaojj��20170412
				for (int i = 0; i < *(pThis->m_stnAddrNum_p) && pThis->m_stnAddr_p; i++)
				{
					if (addr == htonl(*(pThis->m_stnAddr_p + i)))
					{
						stChnlNo = i;
						break;
					}
				}
				if ( stChnlNo < 0 )
				{
					continue;
				}
				if (pThis->m_eventArray_p[stChnlNo] == NULL)//���¼�
				{
					EVENT newEvent = WSACreateEvent();
					WSAEventSelect(sNew , newEvent , FD_READ /*| FD_CLOSE | FD_WRITE*/);
					pThis->m_eventArray_p[stChnlNo] = newEvent;
					pThis->m_sockArray_p[stChnlNo] = sNew;
				}
				else
				{
					sock_descriptor old = pThis->m_sockArray_p[stChnlNo];
					pThis->m_sockArray_p[stChnlNo] = sNew;

					WSAEventSelect(sNew , pThis->m_eventArray_p[stChnlNo], FD_READ /*| FD_CLOSE */);
					CloseSocket(old);
				}
			}

		} // ! while
#else

		CTcpServer *pThis = (CTcpServer *)pParam;
		epoll_event events[64];
		int iCnt = 0;
		u_long locAddr = inet_addr("127.0.0.1");
		while(pThis->m_bRun)
		{
			//�ȴ�epoll�¼�����
			int nfds = epoll_wait(pThis->m_epfd_p ,events ,64 ,200);
			iCnt ++;
			for (int i = 0;i < nfds; i++)
			{
				if (events[i].data.fd == pThis->m_sockListen)
				{
					//sock_descriptor sNew =  accept(pThis->m_sockListen ,  NULL, NULL);
					sock_descriptor sNew = accept(pThis->m_sockListen ,  (struct sockaddr*)&clientAddr, (socklen_t*)&clientLen);

					//u_long addr = inet_addr(pThis->GetIPStr(sNew));
					
					if(sNew < 0)
					{
						perror("connfd < 0");
						exit(1);
					}
					//char *cntAddr[16];
					//u_long addr = inet_addr(pThis->GetIPStr(sNew));
					u_long addr = clientAddr.sin_addr.s_addr;
				   // u_short accPort =  ntohs(clientAddr.sin_port);
					short stChnlNo = addr != locAddr ? -1 : iCnt-1; // ��Զ��վ��ϢУ��ʱ��������У�飬zhaojj��20170412
					for (int i = 0; i < *(pThis->m_stnAddrNum_p) && pThis->m_stnAddr_p; i++)
					{
						if (addr == htonl(*(pThis->m_stnAddr_p + i)) )
						{
							stChnlNo = i;
							break;
						}
					}
					if ( stChnlNo < 0 )
					{
						continue;
					}
					if (pThis->m_sockArray_p[stChnlNo] == 0)//���¼�
					{
						int eventNun = *(pThis->m_stnAddrNum_p)/64;
						if (pThis->m_eventArray_p[eventNun] == 0)
						{
							pThis->m_eventArray_p[eventNun] = epoll_create(64);
						}

						pThis->m_sockArray_p[stChnlNo] = sNew;
						struct epoll_event ev;
						memset(&ev , 0 , sizeof(epoll_event ));
						ev.data.fd = sNew;
						ev.events = EPOLLIN | EPOLLOUT;
						//epoll_ctl(pThis->m_epfd_p , EPOLL_CTL_ADD , sNew , &ev);
						epoll_ctl(pThis->m_eventArray_p[eventNun] , EPOLL_CTL_ADD , sNew , &ev);
					}
					else
					{
						int eventNun = *(pThis->m_stnAddrNum_p)/64;
						sock_descriptor old = pThis->m_sockArray_p[stChnlNo];
						struct epoll_event evOld;
						evOld.data.fd = sNew;
						evOld.events = EPOLLIN | EPOLLOUT;
						epoll_ctl(pThis->m_epfd_p , EPOLL_CTL_DEL , old , &evOld);
						epoll_ctl(pThis->m_eventArray_p[eventNun] , EPOLL_CTL_DEL , old , &evOld);
						CloseSocket(old);
						pThis->m_sockArray_p[stChnlNo] = sNew;
						if (pThis->m_eventArray_p[eventNun] == 0)
						{
							pThis->m_eventArray_p[eventNun] = epoll_create(64);
						}
						struct epoll_event ev;
						ev.data.fd = sNew;
						ev.events = EPOLLIN | EPOLLOUT;
						//epoll_ctl(pThis->m_epfd_p , EPOLL_CTL_ADD , sNew , &ev);
						epoll_ctl(pThis->m_eventArray_p[eventNun] , EPOLL_CTL_ADD , sNew , &ev);

					}


				}
			}
		}
#endif // WINDOWS_PLATFORM
		//sock_descriptor sockConn;
		//struct sockaddr sClientAddr;
		//fd_set fdRead;
		//while (pThis->m_bRun)
		//{
		//    FD_ZERO(&fdRead);
		//    FD_SET(pThis->m_sockListen, &fdRead);
		//    struct timeval sTimeOut = {1, 0};
		//    //select on fdRead
		//    if (select(pThis->m_sockListen+1, &fdRead, NULL, NULL, &sTimeOut) <= 0)
		//    {
		//        nsCPPub::SleepMilliSecond(1);
		//        continue;
		//    }

		//    if (!FD_ISSET(pThis->m_sockListen, &fdRead))
		//    {
		//        nsCPPub::SleepMilliSecond(1);
		//        continue;
		//    }

		//    socklen_t iAddrSize = sizeof(sClientAddr);
		//    sockConn = accept(pThis->m_sockListen, &sClientAddr, &iAddrSize);
		//    if (sockConn == sock_invalid)
		//    {
		//        nsCPPub::SleepMilliSecond(1);
		//        continue;
		//    }

		//    //�õ���һ��������Ϣ
		//    char ConnInfo[sizeof(sock_descriptor)+sizeof(sClientAddr)] = "";
		//    *(sock_descriptor*)ConnInfo = sockConn;
		//    memcpy(ConnInfo+sizeof(sock_descriptor), &sClientAddr, sizeof(sClientAddr));

		//    //���
		//    while (pThis->m_bRun)
		//    {
		//       // int iRet = pThis->m_AcceptQueue.EnQueue(ConnInfo, sizeof(sock_descriptor)+sizeof(sClientAddr));
		//        if (iRet != 0)
		//        {
		//            nsCPPub::SleepMilliSecond(1);
		//        }
		//        else
		//        {
		//            break;
		//        }
		//    }
		//}
	}
private:
	sock_descriptor  m_sockListen;                //����socket
	EVENT*	         m_eventArray_p;			  //�����¼�����,linux����Ч
	sock_descriptor* m_sockArray_p;				  //�׽�������

    sock_descriptor  m_epfd_p;                    //Linuxϵͳ��epoll���
	u_long*		     m_stnAddr_p;				  //Զ��վ��������Ϣ(IP)��ַ
	u_short*         m_stnPort_p;                 //Զ��վ�˿�����
	int*		     m_stnAddrNum_p;			  //ָ��Զ��վ������ַ��δ��֧��Զ��վ��̬��ɾ
	bool		     m_bRun;                      //�߳����б�־
	nsCPThread::CThread m_AcceptThread;           //Accpet�߳�
	// nsCPQueue::CBlockListQueue m_AcceptQueue;  //Accept����
};

//TCP�ͻ���
class CTcpClient : public CSock
{
public:
	//FUNC  ���ӷ�����
	//PARAM ulINETAddr��IP��ַ�����ֱ�ʾ
	//      usPort���˿�
	//      uimsTimeout�����ӵĳ�ʱʱ�䡣��λΪ���롣Ĭ�ϲ���������ʱ��
	//RET   sock_invalid������ʧ��
	//      ���������ӳɹ����������Ӷ�Ӧ��socket
	sock_descriptor ConnServer(unsigned long ulINETAddr, unsigned short usPort,unsigned long uClientAddr = 0, unsigned short ClientPort = 0, unsigned int uimsTimeout = 0 )
	{
		//����socket
		sock_descriptor sockConn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sockConn == sock_invalid)
		{
			return sock_invalid;
		}
		//��ʼ����������ַ
		struct sockaddr_in sServerAddr;
		memset(&sServerAddr, 0, sizeof(sockaddr_in));
		sServerAddr.sin_family = AF_INET;
		sServerAddr.sin_addr.s_addr = ulINETAddr;
		sServerAddr.sin_port = htons(usPort);
/*
		struct sockaddr_in sClientAddr;
		memset(&sClientAddr, 0, sizeof(sockaddr_in));
		sClientAddr.sin_family = AF_INET;
		sClientAddr.sin_addr.s_addr = uClientAddr;
		sClientAddr.sin_port = ClientPort;
		int iRet = bind(sockConn, (struct sockaddr*)&sClientAddr, sizeof(struct sockaddr));
		if( iRet < 0)
		{
			printf("bind::errno= %d,errorMsg =%s\r\n", errno,strerror(errno));
		}*/
		//���ӷ�����
		if (uimsTimeout == 0)
		{//Ĭ��ʹ��������ʽ
		  int  iRet = connect(sockConn, (struct sockaddr *)&sServerAddr, sizeof(sServerAddr));
			if (iRet == sock_error)
			{//������
				Close(sockConn);
				return sock_invalid;
			}
			else
			{
				return sockConn;
			}
		}
		else
		{//ʹ�÷�������ʽ
			timeval sTimeOut;
			sTimeOut.tv_sec = uimsTimeout/1000;
			sTimeOut.tv_usec = (uimsTimeout%1000)*1000;
			fd_set fdWrite;
			unsigned long ul = 1;
			ioctlsocket(sockConn, FIONBIO, &ul); //����Ϊ������ģʽ
			bool bSucc = false;
			if (connect(sockConn, (struct sockaddr *)&sServerAddr, sizeof(sServerAddr)) == sock_error)
			{
				FD_ZERO(&fdWrite);
				FD_SET(sockConn, &fdWrite);
				if (select(sockConn+1, NULL, &fdWrite, NULL, &sTimeOut) > 0)
				{
					int error = -1;
					socklen_t len = sizeof(int);
					getsockopt(sockConn, SOL_SOCKET, SO_ERROR, (char *)&error, &len);
					if (error == 0)
					{
						bSucc = true;
					}
					else
					{
						bSucc = false;
					}
				}
				else
				{
					bSucc = false;
				}
			}
			else
			{
				bSucc = true;
			}

			if (bSucc)
			{
				ul = 0;
				ioctlsocket(sockConn, FIONBIO, &ul); //����Ϊ����ģʽ
				return sockConn;
			}
			else
			{
				Close(sockConn);
				return sock_invalid;
			}
		}
	}
	//FUNC  ���ӷ�����
	//PARAM pIP��IP��ַ�ġ����ʮ���ơ��ַ���
	//      usPort���˿�
	//      uimsTimeout�����ӵĳ�ʱʱ�䡣��λΪ���롣Ĭ�ϲ���������ʱ��
	//RET   sock_invalid������ʧ��
	//      ���������ӳɹ����������Ӷ�Ӧ��socket
	sock_descriptor ConnServer(const char *pIP, unsigned short usPort,const char *uClientAddr = 0, unsigned short ClientPort = 0, unsigned int uimsTimeout = 0)
	{
		return ConnServer(inet_addr(pIP), usPort,inet_addr(uClientAddr) ,ClientPort, uimsTimeout);
	}
protected:
private:
};

}   // end of namespace

#endif  // end of #ifndef

