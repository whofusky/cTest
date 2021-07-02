#pragma once
#include <stdlib.h>
#include <vector>
/*	comm_lib_so  v0.1
	功能：	风功率预测V2.0内部通信协议
			IPC，socket通信方式

			魏志达  2017-04-17
*/



///////////////////////////////////////////////////
#define	LENGHT1K		1024
#define	PARAMLEN		30

#define	MAXRETRYTIME		3


using namespace  std;

typedef struct  
{
	unsigned char param[PARAMLEN];
	unsigned char controlCode;
}paramData_t;

typedef struct  
{
    unsigned char streamBuf[1024*32];
	int streamDataLen;
}streamData_t;

typedef struct  
{
	unsigned char controlCode;
	streamData_t streamData;				//流数据
	int commType;							//通信类型
	int srcAppCode;						//源appcode
	int dstAppCode;						//目的appcode
}rowData_t;

enum {
    LOGDEBUG,
    LOGERROR,
};


enum{
	EPARAMCOMMTYPE			= -1,	//参数错误 无效的通信类型
	EPARAMSOCKFD			= -2,	//参数错误 无效的socket fd
	EPARAMSTREAMDATA		= -3,	//参数错误 无效的streamData
	EPARAMPARAMDATA		= -4,	//参数错误 无效的paramData
	EPARAMSTREAMDATALEN	= -3,	//参数错误 无效的streamDatalen
	EPARAMPARAMDATALEN	= -4,	//参数错误 无效的paramDatalen
	EPARAMPORT				= -8,	//参数错误 无效的port
	EPARAMIP				= -9,	//参数错误 无效的ip
	EPARAMMEMSIZE			= -10,	//参数错误 无效的memsize
	EPARAMCONTROLCODE		= -11,	//参数错误 无效的controlCode
	EPARAMAPPCODE			= -12,	//参数错误 无效的appCode
	EPARAMROWDATA			= -13,	//参数错误 无效的rowdata

	/////////////////////////////////////////////////////////////
	ESOCKSEND			= -50,	//sock操作错误 发送失败
	ESOCKSENDLEN		= -51,	//sock操作错误 无效的发送长度
	ESOCKRECVTIMEO		= -52,	//sock操作错误 接收超时
	ESOCKCLOSED		= -53,	//sock操作错误 对端已关闭
	ESOCKRECV			= -54,	//sock操作错误 接收失败
	ESOCKCREATE		= -55,	//sock操作错误 建立socekt失败
	ESOCKBIND			= -56,	//sock操作错误 bind失败
	ESOCKLISTEN		= -57,	//sock操作错误 listen失败
	ESOCKACCEPT		= -58,	//sock操作错误 accept失败
	ESOCKCONN			= -59,	//sock操作错误 connect失败
	ESOCKFD				= -60,	//sock操作错误 无效的socket fd
	ESOCKSETOPT		= -61,	//sock操作错误 setsockopt出错

	/////////////////////////////////////////////////////////////
	ECOMMONRETRYFAIL	= -100,	//通用操作错误 重试三次都失败 视为放弃

	/////////////////////////////////////////////////////////////
	EFRAMEINCOMPLETE	= -150,	//帧错误 收到的帧不完整(可能是丢中间帧，头帧，结束帧)
	EFRAMEDATAAREALEN	= -151,	//帧错误 无效的数据域长度
	EFRAMEPACKTYPE		= -152,	//帧错误 无效的类型域包类型
	EFRAMECOMMTYPE		= -153,	//帧错误 无效的通信类型
	EFRAMELEN			= -154,	//帧错误 无效的帧长度
	EFRAMENEGCONTENT	= -155,	//帧错误 无效的否定确认帧内容
	EFRAMEAPPCODE		= -156,	//帧错误 无效的应用表述码

	/////////////////////////////////////////////////////////////
	EPACKLEN			= -200,	//包错误 无效的包长度

	/////////////////////////////////////////////////////////////
	ESHMGET				= -300,	//SHM错误 shmget失败
	ESHMAT				= -301,	//SHM错误 shmat失败
	ESHMDT				= -302,	//SHM错误 shmdt失败
	ESHMCTL				= -303,	//SHM错误 shmctl失败

	/////////////////////////////////////////////////////////////
	ESEMGET				= -350,	//SEM错误 semget失败
	ESEMCTL				= -351,	//SEM错误 semctl失败
	ESEMOP				= -352,	//SEM错误 semop失败

};

/*
//const char *errnoMsg[1024] = 
//{
//	errnoMsg[abs(EPARAMCOMMTYPE		)] = "param error, invalid commType",
//	errnoMsg[abs(EPARAMSOCKFD		)] = "param error, invalid socket fd",
//	errnoMsg[abs(EPARAMSTREAMDATA	)] = "param error, invalid streamData",
//	errnoMsg[abs(EPARAMPARAMDATA	)] = "param error, invalid paramData",
//	errnoMsg[abs(EPARAMSTREAMDATALEN)] = "param error, invalid streamDatalen",
//	errnoMsg[abs(EPARAMPARAMDATALEN	)] = "param error, invalid paramDatalen",
//	errnoMsg[abs(EPARAMPORT			)] = "param error, invalid port",
//	errnoMsg[abs(EPARAMIP			)] = "param error, invalid ip",
//	errnoMsg[abs(EPARAMMEMSIZE		)] = "param error, invalid memsize",
//	errnoMsg[abs(EPARAMCONTROLCODE	)] = "param error, invalid controlCode",
//	errnoMsg[abs(EPARAMAPPCODE		)] = "param error, invalid appCode",
//	errnoMsg[abs(EPARAMROWDATA		)] = "param error, invalid rowdata",
//
//	///////////////////////////////////////////////////////////////////////
//	errnoMsg[abs(ESOCKSEND			)] = "socket error, send fail",
//	errnoMsg[abs(ESOCKSENDLEN		)] = "socket error, invalid send len",
//	errnoMsg[abs(ESOCKRECVTIMEO		)] = "socket error, recv time out",
//	errnoMsg[abs(ESOCKCLOSED		)] = "socket error, peer sock closed",
//	errnoMsg[abs(ESOCKRECV			)] = "socket error, recv fail",
//	errnoMsg[abs(ESOCKCREATE		)] = "socket error, create socekt fail",
//	errnoMsg[abs(ESOCKBIND			)] = "socket error, bind fail",
//	errnoMsg[abs(ESOCKLISTEN		)] = "socket error, listen fail",
//	errnoMsg[abs(ESOCKACCEPT		)] = "socket error, accept fail",
//	errnoMsg[abs(ESOCKCONN			)] = "socket error, connect fail",
//	errnoMsg[abs(ESOCKFD			)] = "socket error, invalid socket fd",
//	errnoMsg[abs(ESOCKSETOPT		)] = "socket error, setsockopt fail",
//
//	////////////////////////////////////////////////////////////////////////
//	errnoMsg[abs(ECOMMONRETRYFAIL	)] = "common error, fail and retry 3 times, give up",
//
//	////////////////////////////////////////////////////////////////////////
//	errnoMsg[abs(EFRAMEINCOMPLETE	)] = "frame error, recv incomplete frameList(maybe lost first/middle/end frame)",
//	errnoMsg[abs(EFRAMEDATAAREALEN	)] = "frame error, invalid data area len",
//	errnoMsg[abs(EFRAMEPACKTYPE		)] = "frame error, invalid pack type",
//	errnoMsg[abs(EFRAMECOMMTYPE		)] = "frame error, invalid comm type",
//	errnoMsg[abs(EFRAMELEN			)] = "frame error, invalid frame len",
//	errnoMsg[abs(EFRAMENEGCONTENT	)] = "frame error, invalid negative frame",
//	errnoMsg[abs(EFRAMEAPPCODE		)] = "frame error, invalid app code",
//
//	/////////////////////////////////////////////////////////////////////////
//	errnoMsg[abs(EPACKLEN			)] = "pack error, invalid pack len",
//
//	////////////////////////////////////////////////////////////////////////
//	errnoMsg[abs(ESHMGET			)] = "share memory error, shmget fail",
//	errnoMsg[abs(ESHMAT				)] = "share memory error, shmat fail",
//	errnoMsg[abs(ESHMDT				)] = "share memory error, shmdt fail",
//	errnoMsg[abs(ESHMCTL			)] = "share memory error, shmctl fail",
//
//	////////////////////////////////////////////////////////////////////////
//	errnoMsg[abs(ESEMGET			)] = "sem lock error, semget fail",
//	errnoMsg[abs(ESEMCTL			)] = "sem lock error, semctl fail",
//	errnoMsg[abs(ESEMOP				)] = "sem lock error, semop fail",
//
//
//
//};*/


#pragma pack(push,1)
typedef struct
{
    unsigned short year;
    unsigned char  month;
    unsigned char  day;
    unsigned char  hour;
    unsigned char  minute;
    unsigned char  second;
}DATEFORMAT_T;
#pragma pack(pop)




void _ShowLog(const char * FILE, int LINE, int logLevel, const char* strMsg);

#define ShowLog(logLevel, strMsg)	\
		_ShowLog(__FILE__, __LINE__, logLevel, strMsg);



