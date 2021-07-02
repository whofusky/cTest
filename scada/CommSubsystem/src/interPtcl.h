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

//sndBuf�����ͻ�����
//requestStr��Э�飬�������������������ʲ���
//requestLen��Э�飬�������������������ʲ�������
//srcAppCode��Э�飬Ӧ�ñ����򣬵�4λ��Ч��0~15��
//dstAppCode��Э�飬Ӧ�ñ����򣬸�4λ��Ч��0~15��
//commType��Э�飬ͨ�����ͣ���ʶ������
//controlCode����Ӧ����--��ͨ������Ϊ���ݿ������İ���1Ϊ��⣬2Ϊ��ѯ
//flagRepose����Ӧ����--��ʶ�Ƿ���Ҫ��Ӧ
//step�����ݿ�ķ��ʲ��������ʱ�䣨��δʹ�ã�
//startTime�����ݿ���ʲ�����DB.table.DD��
//endTime�����ݿ���ʲ�����DB.table.DD��
//forcastTime�����ݷ��ʲ�����DB.table.PD��


/*******************************
****�������ʽ��
[������ctlFld]|<������paraFld>

****DB��������Ŀ������������룬ָʾ���еĲ������Լ�DB��������Ĳ���������IDx֮ǰ����ɹ�ϵ
��2λָʾ�������ͣ�01b-��⣻10b-���⣻00b-SQL��ʽ��⣻11b-SQL��ʽ����
��6λָʾ����IDǰ��׷����Ϣ����λ��	λ2-ҳ�ż�ҳ��λ��λ3-���ݿ�ʼʱ��λ��λ4-���ݽ���ʱ��λ��
λ5-����ʱ����λ��λ6-Ԥ��ʱ��λ��λ7�������������������λ��0����Ϊ������ʽ��1Ŀǰ��ʶ��
����λΪ1��ʾ�������ݣ�0��ʾ�޴����ݣ����������޴���

****DB��������Ĳ�����ȫ����ʽ��
������ = [b0~1=��������]|[b2=ҳ��+ҳ��]|[b3=���ݿ�ʼʱ��]|[b4=���ݽ���ʱ��]|[b5=���ݼ��ʱ��]|[b=6Ԥ��ʱ��]|[b7 = 0]
������[������!=0/3]<[ҳ��]/[ҳ��]/[���ݿ�ʼʱ��]/[���ݽ���ʱ��]/[���ݼ��ʱ��]/[Ԥ��ʱ��][b7 = 0]/[����ID1]...[IDn]>
������[reqCd=0/3]<[SQL insert statement/SQL select statement]>
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



//���������������������
typedef struct pDBAcsDFDscb
{
	unsigned char ctlFld;	//������
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
    unsigned char cmuTyp;//ͨ�����ͣ�Э��������
	unsigned char pktTyp;//�����ͣ�֡��ţ�Э��������
	unsigned char appTgtID;//Ŀ��Ӧ��ID��ͨ�������ʹ��Ŀ��Ӧ�ã�Э��Ӧ�ñ�����
	unsigned char appSrcID;//ԴӦ��ID��ͨ�����ݵķ�����Ӧ�ã�Э��Ӧ�ñ�����
	unsigned int dFldLen;//�����򳤶ȣ�Э�鳤����

	//fstFrm[PROB_PKTHD_LNG] = ��{~typFld, typFld, appDscb, lng1, lng2,  lng3,  [dFld1, dFld2]}
	//						   ��{~typFld, typFld, appDscb, lng1, [dFld1, dFld2, dFld3, dFld4]}
	//fstTmpDBuf[PROB_PKTHD_LNG - 4] = ��{dFld1, dFld2,  -- ,   --  }
	//								 = ��{dFld1, dFld2, dFld3, dFld4}
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
