#pragma once
#include "interCmu.h"
#include "cmuSkt.h"
#include "data.h"

#define MAXFRAMELEN (1024)
#define MAXFRAMENUM (31)
#define UNKNOWN_PACKET (0xFFFFFFFF)
#define MULTIFRAME_PACKET (0xFFFFFFFE)
#define SINGLEFRAME_PACKET (0x1)
#define PROB_PKTHD_LNG	(8) //must >=8

//sndBuf：发送缓冲区
//requestStr：协议，包数据域解析，请求访问参数
//requestLen：协议，包数据域解析，请求访问参数长度
//srcAppCode：协议，应用表述域，低4位有效（0~15）
//dstAppCode：协议，应用表述域，高4位有效（0~15）
//commType：协议，通信类型，标识包类型
//controlCode：响应操作--对通信类型为数据库访问求的包：1为入库，2为查询
//flagRepose：响应操作--标识是否需要响应
//step：数据库的访问参数，间隔时间（暂未使用）
//startTime：数据库访问参数（DB.table.DD）
//endTime：数据库访问参数（DB.table.DD）
//forcastTime：数据访问参数（DB.table.PD）


/*******************************
****数据域格式：
[控制域ctlFld]|<参数域paraFld>

****DB访问请求的控制域存放请求码，指示进行的操作，以及DB访问请求的参数域数据IDx之前的组成关系
低2位指示操作类型：01b-入库；10b-出库；00b-SQL方式入库；11b-SQL方式出库
高6位指示数据ID前的追加信息掩码位：	位2-页号及页长位，位3-数据开始时间位，位4-数据结束时间位，
位5-数据时间间隔位，位6-预测时间位，位7保留用作控制域的配置位，0解释为上述方式，1目前不识别。
掩码位为1表示含此数据，0表示无此数据，参数域中无此项

****DB访问请求的参数域全集格式：
请求码 = [b0~1=操作类型]|[b2=页号+页长]|[b3=数据开始时间]|[b4=数据结束时间]|[b5=数据间隔时间]|[b=6预测时间]|[b7 = 0]
数据域：[请求码!=0/3]<[页号]/[页长]/[数据开始时间]/[数据结束时间]/[数据间隔时间]/[预测时间][b7 = 0]/[数据ID1]...[IDn]>
数据域：[reqCd=0/3]<[SQL insert statement/SQL select statement]>
*/
#define PKT_SNGLFRM	(0)
#define PKT_MULTFRM	(1)

#define INSERT (1)
#define SELECT (1<<1)
#define DPAG (1<<2)
#define DTM_S (1<<3)
#define DTM_E (1<<4)
#define DTM_STEP (1<<5)
#define DTM_FC (1<<6)
#define DCF_CFG (1<<7)

#define DBREQ_INSERT (1)
#define DBREQ_SELECT (2)
#define SQLREQ_INSERT (0)
#define SQLREQ_SELECT (3)

#define getOpr(cltCd) (cltCd & 0x3)
#define isNAck(cmuTyp,pktTyp) ((7 == cmuTyp) && (!pktTyp))	


#define MNGR_CMUTYP (0)//manage packet
#define CFGCMD_CMUTYP (1)//config and command packet
#define CFGCMD_RSPD_CMUTYP (2)	//config and commond respond packet
#define REQACS_CMUTYP (3)		//require access packet
#define REQACS_RSND_CMUTYP (4)	//require access respond packet
#define FIXTM_CMUTYP (5)		//fix time packet
#define ANYTM_CMUTYP (6)		//any time packet
#define NACK_CMUTYP (7)			//NACK packet
#define RSRV_CMUTYP (7)			//reserve packet



//访问请求包内数据域描述
typedef struct pDBAcsDFDscb
{
	unsigned char ctlFld;	//控制域
	unsigned int dPageNo;
	unsigned int dPageLng;
	DATAFORMAT_DATE7B1 dStartTm;
	DATAFORMAT_DATE7B1 dEndTm;
	DATAFORMAT_DATE7B1 dFCTm;
	unsigned int dStepTm;
}pDBADFD_T;

/*
typedef struct tRcvDscb
{
	unsigned int xptFrmDFldLng;
	unsigned char* ldPos;
	unsigned char xptPktTyp;
}rcvFrmDscb_T;*/

typedef struct pktHdInfo
{
    unsigned char cmuTyp;//通信类型：协议类型域
	unsigned char pktTyp;//包类型，帧序号：协议类型域
	unsigned char appTgtID;//目标应用ID，通信数据送达的目的应用：协议应用表述域
	unsigned char appSrcID;//源应用ID，通信数据的发送者应用：协议应用表述域
	unsigned int dFldLen;//数据域长度：协议长度域

	//fstFrm[PROB_PKTHD_LNG] = ：{~typFld, typFld, appDscb, lng1, lng2,  lng3,  [dFld1, dFld2]}
	//						   ：{~typFld, typFld, appDscb, lng1, [dFld1, dFld2, dFld3, dFld4]}
	//fstTmpDBuf[PROB_PKTHD_LNG - 4] = ：{dFld1, dFld2,  -- ,   --  }
	//								 = ：{dFld1, dFld2, dFld3, dFld4}
 //   unsigned char fstTmpDBuf[PROB_PKTHD_LNG - 4];
}pktHdInfo_T;

typedef struct rcvFrmUnitBuf
{
    int len;//data field length
    unsigned char frame[MAXFRAMELEN];//frame[0...MAXFRAMELEN-1]: data fild, no type fild
}rcvFU_T;

typedef struct sndFrmUnitBuf
{
	int len;//data field length
	//start frame:
	//frame[0]:~type field; frame[1]:type field; frame[2]:app discrib field
	//frame[3]/frame[3~5]:data length field; frame[4/6,...MAXFRAMELEN+3/MAXFRAMELEN+5]: data fild
	//mid-frame or last frame:
	//frame[0]:type field; frame[1~MAXFRAMELEN]:data field
	unsigned char frame[MAXFRAMELEN+6];
}sndFU_T;


int reqDItemAcsDB(pktHdInfo_T* pktInfo_p, sndFU_T* sndBuf_p, unsigned char* usrData_p, int usrDataLng, cmuSkt* cmuSkt_p, pDBADFD_T* reqPktDF_p);
int reqSQLAcsDB(cmuSkt* cmuSkt_p, pktHdInfo_T* pktInfo_p, sndFU_T* sndBuf_p, unsigned char* usrData_p, int usrdDataLng, unsigned char reqCd);
//int respSlctDB(cmuSkt* cmuSkt_p, pktHdInfo_T* pktInfo_p, sndFU_T* sndBuf_p, unsigned char* usrData_p, int usrDataLng, pDBADFD_T* reqPktDF_p);
//int respInsrtDB(cmuSkt* cmuSkt_p, pktHdInfo_T* pktInfo_p, sndFU_T* sndBuf_p, unsigned char* usrData_p, int usrDataLng, unsigned char reqCd);
int prsRtnPkt(cmuSkt* cmuSkt_p, rcvFU_T* rcvBuf_p, sndFU_T* sndBuf_p, pktHdInfo_T* pktInfo_p, int myID,int dFldLen);

int prsSsnStarPkt(cmuSkt* cmuSkt_p, rcvFU_T* rcvPktBuf_p, int myID, unsigned char* ssnStartData_p, int ssnStartDataLng, pktHdInfo_T*  pktInfo_p);
int rtnRespPkt(cmuSkt* cmuSkt_p, pktHdInfo_T* pktInfo_p, sndFU_T* sndBuf_p, unsigned char* usrData_p, int usrDataLng, unsigned char rspdCd);
