#ifndef __TRANS_CTRL_LAYER_H__
#define __TRANS_CTRL_LAYER_H__

//#define WIN32_LEAN_AND_MEAN

/*****************************************************************************/
/*    ͷ�ļ����ⲿȫ�ֱ���������                              	                 */
/*****************************************************************************/


#include "FrameGlobal.h"
#include "DataBase.h"
#include "Timer.h"

#ifdef WINDOWS_PLATFORM //��WINDOWSƽ̨
#include <winsock2.h>
#include <windows.h> 
#else
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <signal.h>
#endif

// ��־����
#define LOG(...) printf(__VA_ARGS__)

/*****************************************************************************/
/*    ��վ���йص���Ϣ                                    	                 */
/*****************************************************************************/
// վ���ɫ
// #define MASTER_STATION     0x1  // ����վ����վ��
// #define SLAVE_STATION      0x2  // ������վ����վ��
// #define COMBINED_STATION   0x3  // ϵͳվ���ۺ�վ��

// ����վ����վ����״̬����
#define MASTER_STOPPED              0x0  // STOPPED����
#define MASTER_STARTED              0x1  // STARTED����
#define MASTER_UNCONFIRMED_STOPPED  0x2  // ����δȷ��STOPPED����
#define MASTER_PEND_STARTED         0x3  // ����STARTED����
#define MASTER_PEND_STOPPED         0x4  // ����STOPPED����

// ������վ����վ����״̬����
#define SLAVE_STOPPED               0x0  // STOPPED����
#define SLAVE_STARTED               0x1  // STOPPED����
#define SLAVE_UNCONFIRMED_STOPPED   0x2  // ����δȷ��STOPPED����


//
void Timer2Service(void * param);
void TimerService( void * param );

/*****************************************************************************/
/*    ��ͨ����ص���Ϣ:������ʵ����ݽṹ���ӿں����Ͳ�������                      */
/*****************************************************************************/
// ��������㴫��ӿ�����
#define DEVICE_COM  0x1  // ���� 
#define DEVICE_NET  0x0  // ����

static u_char threadNum ;
// ͨ��(��)�Ŵ�����ʵĽӿ�
typedef struct
{
	u_short chnlID;    // ͨ��(��)��
	u_char  type;      // ͨ��(��)���豸1���ͣ�DEVICE_COM��DEVICE_NET
	HANDLE  hIntance;  // ͨ��(��)���豸1�Ĳ������
}DEVICES, *PDEVICES;

// ͨ��ִ�б���
#define NO_OPERATION   0x0  // ������·���д���
#define OPEN_CONNECT   0x1  // ����·
#define CLOSE_CONNECT  0x2  // �ر���·
#define RETRY_CONNECT  0x3  // ������·
#define TEST_CONNECT   0x4  // ��ȡ��·״̬��Ϣ
#define WRITE_CONNECT  0x5  // ͨ����·��������
#define READ_CONNECT   0x6  // ͨ����·��������
#define PROC_RECV_CTRL 0x7  // ͨ����·��������յĴ����������
#define PROC_RECV_ASDU 0x8  // ͨ����·��������յ�ASDU����

// �������ݵĿ���ģʽ
#define SEND_NO_CONTROL       0x0  // �޿���ģʽ
#define SEND_TIMEOUT_CONTROL  0x1  // ��ʱ����ģʽ
#define SEND_RETYR_CONTROL    0x2  // ���Դ�������ģʽ

// ������
int OpenConnect(DEVICES dev);
// �ر�����
int CloseConnect(DEVICES dev);
// ��������
int RetryConnect(DEVICES dev);
// ��ȡ���ӵ�״̬
int GetConnectStatus(DEVICES dev);
// �������ݲ���
int WriteToConnect(DEVICES dev, char * buf, int length, void *cfgParam);
// �������ݲ���
int ReadFromConnect(DEVICES dev, char * buf, int length, void *cfgParam);

/*****************************************************************************/
/*    �붨ʱ����ص���Ϣ:�������롢�ӿں����ͻص�����          	                 */
/*****************************************************************************/
// ��ʱ�������붨��
//#define TIMER_NOP        0x0    // ��ʱ���ղ���������ԭ״̬������ͣ�ָ���
//#define TIMER_START      0x1    // ��ʱ����������
//#define TIMER_STOP       0x2    // ��ʱ��ֹͣ���������¸�λ
//#define TIMER_PAUSE      0x3    // ��ʱ����ͣ����
//#define TIMER_INSTALL    0x4    // ��ʱ���Ѱ�װ
//#define TIMER_UNINSTALL  0x5    // ��ʱ��δ��װ

// ���ö�ʱ��
void SetCtrlTimer(C_ChnlObj *chnlObj_p, u_char tmCtrlCode);
// �ײ�ͨ�����Ͷ���������
void SetFlowCtrl(u_char grpId, u_short qIdx, void* cnxt_p);


/*****************************************************************************/
/*    ������Ʋ����Լ�ӿں���                              	                 */
/*****************************************************************************/
// ��ȡ��ԼЭ��汾��
typedef int (*DLL_getPtclID)();
// ��ȡAPDU��ASDU�����е���������
typedef  u_char * (*DLL_getDU)(u_char* apduObj_p , u_short &apduLng , char duType);

///////////    ���Ͳ�    /////////// 
// ��ASDU������װ��APDU����
typedef void (*DLL_packAPDU)(void* rtmParam_p ,void* cfgParam_p, u_char* ASDUFrm_p ,u_short ASDUFrm_Lng , u_char* APDU_buf_p , u_char& APDU_bufLng);
// ��鷢�Ͳ�ķ���״̬
typedef u_int (*DLL_CheckSendState)(short role, void *rtmParam_p, u_char *data_p, u_short length);
// ���·��Ͳ��ʵʱ������״̬Ǩ��
typedef int (*DLL_UpdateSendState)(short role, void *rtmParam_p, void *cfgParam_p ,u_char* tmrCtrlCode_p, u_char *data_p, u_short length);

///////////    ���ղ�    /////////// 
// �������֡ͷ������Ϣ
typedef  u_char (*DLL_getHeaderlng)();
// ���APDU���ݳ�����Ϣ
typedef  int (*DLL_getApduLngFrmBuf)(char*  bufferAdd, int bufferLng);
// ����APDU����,����ASDU����
typedef u_short (*DLL_chkAPDUData)(revBuf* revDataFrm , void* rtmParam_p , void* cfgParam_p);
// ����APDU���ݽ�������
typedef int (*DLL_HandleRecvStateError)(transCtrlFrmDsc* ctrlFrm , void *rtmParam_p ,int error);
// ���½��ղ��ʵʱ������״̬Ǩ��
typedef int (*DLL_UpdateRecvState)(short role, void *rtmParam_p , void *cfgParam_p , u_char *tmrCtrlCode_p , u_char *data_p , u_short length);
// ���ݿ���֡,����Ӧ��֡
typedef int (*DLL_RespCtrlFrame)(transCtrlFrmDsc* ctrlFrm , void *rtmParam_p ,int result);
// ֱ�ӹ������֡
typedef int (*DLL_CtrlFrame)(transCtrlFrmDsc* ctrlFrm , void *rtmParam_p , int result);
//
typedef u_int (*DLL_CheckRecvState)(short role, void *rtmParam_p, u_char *data_p, u_short length);

//
typedef int (*DLL_timerRingAdd) (u_char timerId);


/*****************************************************************************/
/*    ���Ͳ�ı�����ӿں���                                 	                 */
/*****************************************************************************/
/*���ܣ���ʼ���߳�
*
*/
int ThreadInit(u_short standNum ,u_char sandTFlag ,u_char recvTFlag);

// ���Ͳ��̺߳���
u_long  Tsys_NetSndProc(void* lpParam);

// ��������ƶ����е�����
void hdlTransCtrlReqEvnt(qTransCtrlInfoUnit *ctrlUnitObj_p, u_char dBlkLng);
// ����ASDU��������е�����
void hdlASDUInfoSndReqEvnt(qASDUInfoRcvUnit *datUnitObj_p, u_char dBlkLng);

// ��׼����������֮ǰ����鷢��״̬������״̬�ı���
#define SEND_READY_STATUS       0x0  // ����׼��״̬���������Ͳ���׼��������
#define SEND_ROLE_ERROR         0x1  // վ��Ľ�ɫ����
#define SEND_FRAME_ERROR        0x2  // �Ƿ�������֡������U֡ͬʱ���ڶ��ֹ��ܣ�
#define SEND_STATUS_ERROR       0x3  // �Ƿ��ķ���״̬����״̬���в����ڵ�״̬��
#define SEND_TRANSITION_ERROR   0x4  // ״̬Ǩ����󣬼������㷢�����ݵ�������
#define SEND_TIMESCALE_ERROR    0x5  // ����ʱ�����

/*****************************************************************************/
/*    ���ղ������ӿں���                                 	                 */
/*****************************************************************************/
// APDU������������ 
#define  MC_APDU_OK         0  // APDU���ݽ����ɹ�
#define  MC_REMOTE_WARNING  1  // Զ�̴�����Ƹ�֪����
#define  MC_NATIVE_WARNING  2  // ���س�ʱ��֪����
#define  MC_REMOTE_FLOWCTRL 3  // Զ�̴������Ҫ����������
#define  MC_NATIVE_FLOWCTRL 4  // ����Ҫ����������
#define  MC_APDU_DISORDER   5  // APDU֡ʧ�����
#define  MC_APDU_CHECKSUM   6  // APDU֡У��ʧ��
#define  MC_APDU_UNKNOWN    7  // �޷�ʶ���֡

// APDU������
#define  CTRL_FRAME    1  // �������֡
#define  DATA_FRAME    0  // ����֡

#define MAX_RECV_SIZE   256   // �������ݻ���Ĵ�С

#define GROP_CHNL_SIZE  64    // һ���̹߳���ͨ�����������
#define MAX_GROP_SIZE   16    // ϵͳ��������̵߳��������
#define MAX_CHNL_SIZE   1024  // ϵͳ��������̵߳��������


// ���ղ��̺߳���
u_long Tsys_NetRevProc(void* lpParam);
// ���ݴ�������ֱ�����������ݱ���������
void hdlRecvData(u_short chnlID, unsigned long long *bufFlag, revBuf *DataFrm);
// ����������մ���
void hdlRecvError(u_short chnlID);

typedef struct{
	u_short frameType:1;
	u_short SNo:15;
	u_short pad:1;
	u_short RNo:15;
}I_FRM;
/*****************************************************************************/
/*    ����104��Լ��صı��룬��Ҫ���ڶ�ʱ����ʱ�Ļص��������д��޸�                */
/*****************************************************************************/
// ��������֡�����͵Ĺ���
// #define IEC104_I  0x0  // IEC104Э���I֡
// #define IEC104_S  0x1  // IEC104Э���S֡
// #define IEC104_U  0x3  // IEC104Э���U֡
// #define IEC104_TESTFR_ACK   0x83  // TESTFRȷ��
// #define IEC104_TESTFR_ACT   0x43  // TESTFR����
// #define IEC104_STOPDT_ACK   0x23  // STOPDTȷ��
// #define IEC104_STOPDT_ACT   0x13  // STOPDT����
// #define IEC104_STARTDT_ACK  0x0B  // STARTDTȷ��
// #define IEC104_STARTDT_ACT  0x07  // STARTDT����

#endif // __TRANS_CTRL_LAYER_H__
