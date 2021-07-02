#ifndef COMM_STRU
#define COMM_STRU

#include "interPtcl.h"

typedef struct appChnl
{
    cmuSkt* cmuChnl_p;
    pktHdInfo_T rcvPktInfo;
    rcvFU_T* rcvBuf_p;
    int rcvBufUsingLng;
    pktHdInfo_T sndPktInfo;
    sndFU_T* sndBuf_p;
    int sndBufUsingLng;
}appChnl_T;


#endif // COMM_STRU

