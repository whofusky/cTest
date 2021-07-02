#ifndef __TRANS_CTRL_LAYER_H__
#define __TRANS_CTRL_LAYER_H__

//#define WIN32_LEAN_AND_MEAN

/*****************************************************************************/
/*    头文件和外部全局变量的引用                              	                 */
/*****************************************************************************/


#include "FrameGlobal.h"
#include "DataBase.h"
#include "Timer.h"

#ifdef WINDOWS_PLATFORM //是WINDOWS平台
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

// 日志方法
#define LOG(...) printf(__VA_ARGS__)

/*****************************************************************************/
/*    与站点有关的信息                                    	                 */
/*****************************************************************************/
// 站点角色
// #define MASTER_STATION     0x1  // 控制站（主站）
// #define SLAVE_STATION      0x2  // 被控制站（子站）
// #define COMBINED_STATION   0x3  // 系统站（综合站）

// 控制站（主站）的状态类型
#define MASTER_STOPPED              0x0  // STOPPED连接
#define MASTER_STARTED              0x1  // STARTED连接
#define MASTER_UNCONFIRMED_STOPPED  0x2  // 挂起未确认STOPPED连接
#define MASTER_PEND_STARTED         0x3  // 挂起STARTED连接
#define MASTER_PEND_STOPPED         0x4  // 挂起STOPPED连接

// 被控制站（子站）的状态类型
#define SLAVE_STOPPED               0x0  // STOPPED连接
#define SLAVE_STARTED               0x1  // STOPPED连接
#define SLAVE_UNCONFIRMED_STOPPED   0x2  // 挂起未确认STOPPED连接


//
void Timer2Service(void * param);
void TimerService( void * param );

/*****************************************************************************/
/*    与通道相关的信息:传输介质的数据结构、接口函数和操作编码                      */
/*****************************************************************************/
// 定义物理层传输接口类型
#define DEVICE_COM  0x1  // 串口 
#define DEVICE_NET  0x0  // 网口

static u_char threadNum ;
// 通道(组)号传输介质的接口
typedef struct
{
	u_short chnlID;    // 通道(组)号
	u_char  type;      // 通道(组)中设备1类型，DEVICE_COM或DEVICE_NET
	HANDLE  hIntance;  // 通道(组)中设备1的操作句柄
}DEVICES, *PDEVICES;

// 通道执行编码
#define NO_OPERATION   0x0  // 不对链路进行处理
#define OPEN_CONNECT   0x1  // 打开链路
#define CLOSE_CONNECT  0x2  // 关闭链路
#define RETRY_CONNECT  0x3  // 重启链路
#define TEST_CONNECT   0x4  // 获取链路状态信息
#define WRITE_CONNECT  0x5  // 通过链路发送数据
#define READ_CONNECT   0x6  // 通过链路发送数据
#define PROC_RECV_CTRL 0x7  // 通过链路来处理接收的传输控制数据
#define PROC_RECV_ASDU 0x8  // 通过链路来处理接收的ASDU数据

// 发送数据的控制模式
#define SEND_NO_CONTROL       0x0  // 无控制模式
#define SEND_TIMEOUT_CONTROL  0x1  // 超时控制模式
#define SEND_RETYR_CONTROL    0x2  // 重试次数控制模式

// 打开连接
int OpenConnect(DEVICES dev);
// 关闭连接
int CloseConnect(DEVICES dev);
// 重新连接
int RetryConnect(DEVICES dev);
// 获取连接的状态
int GetConnectStatus(DEVICES dev);
// 发送数据操作
int WriteToConnect(DEVICES dev, char * buf, int length, void *cfgParam);
// 接收数据操作
int ReadFromConnect(DEVICES dev, char * buf, int length, void *cfgParam);

/*****************************************************************************/
/*    与定时器相关的信息:操作编码、接口函数和回调函数          	                 */
/*****************************************************************************/
// 定时器控制码定义
//#define TIMER_NOP        0x0    // 定时器空操作，继续原状态（或暂停恢复）
//#define TIMER_START      0x1    // 定时器启动操作
//#define TIMER_STOP       0x2    // 定时器停止操作，重新复位
//#define TIMER_PAUSE      0x3    // 定时器暂停操作
//#define TIMER_INSTALL    0x4    // 定时器已安装
//#define TIMER_UNINSTALL  0x5    // 定时器未安装

// 设置定时器
void SetCtrlTimer(C_ChnlObj *chnlObj_p, u_char tmCtrlCode);
// 底层通道发送端流量控制
void SetFlowCtrl(u_char grpId, u_short qIdx, void* cnxt_p);


/*****************************************************************************/
/*    传输控制层与规约接口函数                              	                 */
/*****************************************************************************/
// 获取规约协议版本号
typedef int (*DLL_getPtclID)();
// 获取APDU或ASDU对象中的数据内容
typedef  u_char * (*DLL_getDU)(u_char* apduObj_p , u_short &apduLng , char duType);

///////////    发送侧    /////////// 
// 将ASDU数据组装成APDU数据
typedef void (*DLL_packAPDU)(void* rtmParam_p ,void* cfgParam_p, u_char* ASDUFrm_p ,u_short ASDUFrm_Lng , u_char* APDU_buf_p , u_char& APDU_bufLng);
// 检查发送侧的发送状态
typedef u_int (*DLL_CheckSendState)(short role, void *rtmParam_p, u_char *data_p, u_short length);
// 更新发送侧的实时参数和状态迁徙
typedef int (*DLL_UpdateSendState)(short role, void *rtmParam_p, void *cfgParam_p ,u_char* tmrCtrlCode_p, u_char *data_p, u_short length);

///////////    接收侧    /////////// 
// 获得数据帧头长度信息
typedef  u_char (*DLL_getHeaderlng)();
// 获得APDU数据长度信息
typedef  int (*DLL_getApduLngFrmBuf)(char*  bufferAdd, int bufferLng);
// 解析APDU数据,返回ASDU数据
typedef u_short (*DLL_chkAPDUData)(revBuf* revDataFrm , void* rtmParam_p , void* cfgParam_p);
// 处理APDU数据解析错误
typedef int (*DLL_HandleRecvStateError)(transCtrlFrmDsc* ctrlFrm , void *rtmParam_p ,int error);
// 更新接收侧的实时参数和状态迁徙
typedef int (*DLL_UpdateRecvState)(short role, void *rtmParam_p , void *cfgParam_p , u_char *tmrCtrlCode_p , u_char *data_p , u_short length);
// 根据控制帧,构造应答帧
typedef int (*DLL_RespCtrlFrame)(transCtrlFrmDsc* ctrlFrm , void *rtmParam_p ,int result);
// 直接构造控制帧
typedef int (*DLL_CtrlFrame)(transCtrlFrmDsc* ctrlFrm , void *rtmParam_p , int result);
//
typedef u_int (*DLL_CheckRecvState)(short role, void *rtmParam_p, u_char *data_p, u_short length);

//
typedef int (*DLL_timerRingAdd) (u_char timerId);


/*****************************************************************************/
/*    发送侧的编码与接口函数                                 	                 */
/*****************************************************************************/
/*功能：初始化线程
*
*/
int ThreadInit(u_short standNum ,u_char sandTFlag ,u_char recvTFlag);

// 发送侧线程函数
u_long  Tsys_NetSndProc(void* lpParam);

// 处理传输控制队列中的数据
void hdlTransCtrlReqEvnt(qTransCtrlInfoUnit *ctrlUnitObj_p, u_char dBlkLng);
// 处理ASDU请求队列中的数据
void hdlASDUInfoSndReqEvnt(qASDUInfoRcvUnit *datUnitObj_p, u_char dBlkLng);

// 在准备发送数据之前，检查发送状态，返回状态的编码
#define SEND_READY_STATUS       0x0  // 发送准备状态正常，发送操作准备就绪；
#define SEND_ROLE_ERROR         0x1  // 站点的角色错误；
#define SEND_FRAME_ERROR        0x2  // 非法的数据帧，比如U帧同时存在多种功能；
#define SEND_STATUS_ERROR       0x3  // 非法的发送状态，即状态机中不存在的状态；
#define SEND_TRANSITION_ERROR   0x4  // 状态迁徙错误，即不满足发送数据的条件；
#define SEND_TIMESCALE_ERROR    0x5  // 发送时标错误；

/*****************************************************************************/
/*    接收侧编码与接口函数                                 	                 */
/*****************************************************************************/
// APDU解析错误类型 
#define  MC_APDU_OK         0  // APDU数据解析成功
#define  MC_REMOTE_WARNING  1  // 远程传输控制告知错误
#define  MC_NATIVE_WARNING  2  // 本地超时告知错误
#define  MC_REMOTE_FLOWCTRL 3  // 远程传输控制要求流量控制
#define  MC_NATIVE_FLOWCTRL 4  // 本地要求流量控制
#define  MC_APDU_DISORDER   5  // APDU帧失序错误
#define  MC_APDU_CHECKSUM   6  // APDU帧校验失败
#define  MC_APDU_UNKNOWN    7  // 无法识别的帧

// APDU的类型
#define  CTRL_FRAME    1  // 传输控制帧
#define  DATA_FRAME    0  // 数据帧

#define MAX_RECV_SIZE   256   // 接收数据缓存的大小

#define GROP_CHNL_SIZE  64    // 一个线程管理通道的最大数量
#define MAX_GROP_SIZE   16    // 系统管理接收线程的最大数量
#define MAX_CHNL_SIZE   1024  // 系统管理接收线程的最大数量


// 接收侧线程函数
u_long Tsys_NetRevProc(void* lpParam);
// 数据处理函数，直至将接收数据保存至队列
void hdlRecvData(u_short chnlID, unsigned long long *bufFlag, revBuf *DataFrm);
// 处理网络接收错误
void hdlRecvError(u_short chnlID);

typedef struct{
	u_short frameType:1;
	u_short SNo:15;
	u_short pad:1;
	u_short RNo:15;
}I_FRM;
/*****************************************************************************/
/*    仅与104规约相关的编码，主要用于定时器超时的回调函数，有待修改                */
/*****************************************************************************/
// 定义数据帧的类型的功能
// #define IEC104_I  0x0  // IEC104协议的I帧
// #define IEC104_S  0x1  // IEC104协议的S帧
// #define IEC104_U  0x3  // IEC104协议的U帧
// #define IEC104_TESTFR_ACK   0x83  // TESTFR确认
// #define IEC104_TESTFR_ACT   0x43  // TESTFR激活
// #define IEC104_STOPDT_ACK   0x23  // STOPDT确认
// #define IEC104_STOPDT_ACT   0x13  // STOPDT激活
// #define IEC104_STARTDT_ACK  0x0B  // STARTDT确认
// #define IEC104_STARTDT_ACT  0x07  // STARTDT激活

#endif // __TRANS_CTRL_LAYER_H__
