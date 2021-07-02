#pragma once

#include "common_comm.h"


#define MAXDATAAREALEN		0xFFFFFF
#define MAXPACKTYPE			0x1F							//每包最大31帧
#define MAXFRAMEDATALEN		1024							//不带头部的帧最大1024字节
//#define MAXFRAMEDATALEN		50
//#define MAXPACKTYPE			31
#define MAXFRAMELEN			(MAXFRAMEDATALEN+10)				//带有头部的帧长度
#define MAXPACKDATALEN		(MAXFRAMEDATALEN*MAXPACKTYPE)		//最大包原始数据长度
#define MAXPACKLEN			((MAXFRAMEDATALEN+6)+(MAXFRAMEDATALEN+1)*30)	//带有头部的包最大长度
//COMM_TYPE_REAL_TIME_DATA_TRANS 类型 最长

typedef struct
{
    int len;
    unsigned char frame[MAXFRAMELEN];
}frame_t;

enum
{
    COMM_TYPE_MANAGE				= 0x00,		// 管理类码值
    COMM_TYPE_CONFIG_CONTROL		= 0x01,		// 配置和控制命令
    COMM_TYPE_CONFIG_CONTROL_RSP	= 0x02,		// 配置和控制命令应答
    COMM_TYPE_VISIT_REQUEST			= 0x03,		// 访问请求
    COMM_TYPE_VISIT_REQUEST_RSP		= 0x04,		// 访问请求响应
    COMM_TYPE_TIMING_DATA_TRANS		= 0x05,		// 定时数据传输
    COMM_TYPE_REAL_TIME_DATA_TRANS	= 0x06,		// 即时数据传输
    COMM_TYPE_NEGATIVE_CONFIRM		= 0x07,		// 否定确认
};

enum
{
    SIGNLE_FRAME,			//单帧
    START_FRAME,			//起始帧
    MIDDLE_FRAME,			//中间帧
    END_FRAME,				//结束帧
};




int makeBufToFrameList(int commType, const int srcAppCode, const int dstAppCode, int* frameIdx, 
    const unsigned char *inBuf, const int inBufLen, frame_t *frameList);
int makeFrameListToBuf(frame_t* frame, const int srcAppCode, const int dstAppCode, 
    unsigned char *outBuf);
int makePackToFrameList(const unsigned char * inBuf, const int inBufLen, rowData_t *rowData, 
    frame_t *frameList, int *outBufLen);
int getCommTypefromFrame(const unsigned char* frame);
int isEndFrame(const unsigned char * frame);
int getIncompleteFrame(frame_t *frameList, int *incompleteFrameIdx, int packDataLen);
int getRetryFrameIdx(void* negtiveFrame, int * retryFrameIdx);
int makeRowDataToBuf(rowData_t *rowData, unsigned char *inBuf, unsigned char **sendStart);
int makeBufTORowData(rowData_t *rowData, unsigned char* dataBuf, const int packDataLen, const int sumOutLen);
int depackLenghtArea(int commType, const unsigned char *inBuf);

int isFirstFrame(const unsigned char* frame);
int getMemLen(void *oneFrame, int maxLen);
int maxArrayIdx(frame_t * frameList);
int isMemNull(void* m, int len);
void initArray(int *idx, int arrayAize, int val);
int arrayTest(int * arr, int sizeArr, int val);
int parseFirstFrame(unsigned char *peek, int peekLen, int *packLen);

void printByte(const int commType, const int frameType, const int packType, const unsigned char *frame, const int len);
