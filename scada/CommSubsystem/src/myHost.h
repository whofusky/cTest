
#pragma once
#include "sIface.h"

#define SCADA


#define DCNTR_APP		(0)
#define SCADA_APP		(1)
#define FILEST_APP		(2)
#define THRYPWR_APP		(3)
#define DSPCH_APP		(4)
#define USHTFCST_APP	(5)
#define CLNTPNT_APP		(6)
#define RSV1_APP		(7)
#define RSV2_APP		(8)
#define RSV3_APP		(9)
#define RSV4_APP		(10)
#define RSV5_APP		(11)
#define RSV6_APP		(12)
#define RSV7_APP		(13)
#define RSV8_APP		(14)
#define RSV9_APP		(15)

typedef int(*transToken_T)(unsigned char* inOutData_p, int* inOutBufLng_p, int oprCd);
typedef int(*exeCmdAndCfg_T)(unsigned char* inOutData_p, int* inOutBufLng_p, int oprCd);
typedef int(*exeReqAcs_T)(unsigned char* inOutData_p, int* inOutBufLng_p, int oprCd);
typedef int(*getFixTmData_T)(unsigned char* inOutData_p, int* inOutBufLng_p, int oprCd);
typedef int(*getAnyTmData_T)(unsigned char* inOutData_p, int* inOutBufLng_p, int oprCd);

int dummySrv(unsigned char* inOutPara_p, int* inOutParaLng_p, int ctrFld);

int ldSvrFun(sIface* regPlace_p, int mySrvNo, int myID);






