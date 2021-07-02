#include<stdio.h>
#include "DataType.h"
#include "APDU101.h"
#include "protocol104.h"

//#define FUSK_TEST_C_IC_NA
//#define FUSK_TEST_C_RD_NA
//#define FUSK_TEST_C_CS_NA
//#define FUSK_TEST



typedef struct
{
    u_char vsq_n: 7;
    u_char vsq_sq: 1;
} T_VSQ;

int main()
{
    T_snnCfg* tmp_stnSnnCfg_p = new T_snnCfg();
    T_snnPara* t_snnPara = new T_snnPara();
    ONE_STEP* preStepRul_p = new ONE_STEP();

    T_snnCfg* allSnnCfg_p = new T_snnCfg();
    T_ptclFunDsc* allPtclFuncDsc = new T_ptclFunDsc();

    u_int tt_test = 0xffffffff;

    /* u_short tmpShort = 0;
     vrfItem tmp_vrfItem = {};
     tmp_vrfItem.vrfCd = 4;
     tmpShort = tmp_vrfItem.vrfCd & 3;*/

    /*u_char tt_UChar = 123;
    u_int tt_UInt = tt_UChar;*/

    //APDU_DSCR
    APDU_DSCR tmpApduDscr;
    memset ( &tmpApduDscr, 0, sizeof ( APDU_DSCR ) );
    tmpApduDscr.ASDU_TID = 100;
    tmpApduDscr.ASDU_CMTBL[0] = 6;//APCI
    tmpApduDscr.ASDU_CMTBL[1] = 1;//ASDU_TID
    tmpApduDscr.ASDU_CMTBL[2] = 0;//ASDU_LNG
    tmpApduDscr.ASDU_CMTBL[3] = 1;//ASDU_VSQ
    tmpApduDscr.ASDU_CMTBL[4] = 1;//ASDU_COT
    tmpApduDscr.ASDU_CMTBL[5] = 2;//ASDU_ADD
    tmpApduDscr.ASDU_CMTBL[6] = 0;//BODY_TID
    tmpApduDscr.ASDU_CMTBL[7] = 3;//BODY_ADD
    tmpApduDscr.ASDU_CMTBL[8] = 2;//BODY_SET
    tmpApduDscr.ASDU_CMTBL[9] = 4;//BODY_TSP
    tmpApduDscr.ASDU_CMTBL[10] = 0; //ASDU_TSP
    tmpApduDscr.InfoEleLng = 4;

    //1、调用createAPDU
    u_char* apdu_p = createAPDU ( &tmpApduDscr, ASDU_TYPE, 1 );
    u_short apduLong = 0;
    u_char duType = ( ( APDU104* ) apdu_p )->getDUType();
    u_char* DP_p = ( ( APDU104* ) apdu_p )->getDU ( apduLong, duType );

#ifdef FUSK_TEST


    /*FUN_RL* vRl_P=createRl_P(6 , ONLY_ONE,allSnnCfg_p,allPtclFuncDsc);

    int vret1 = getFunStep( allPtclFuncDsc,
    MC_TID_1 ,
    MC_COT_20,
    NULL,
    NULL,
    0
    );
    freeRl_P(vRl_P,allSnnCfg_p,allPtclFuncDsc);*/
    //Is it a  valid comment?

    struct T_QCC
    {
        u_char RQT: 6;
        u_char FRZ: 2;
    } qcc = {5, 0};

    u_char tid = 21;

    if ( CCIFTID ( tid ) )
    {
        printf ( "hahhahahah\n" );
    }

    u_char memVal[11] = {};
    ELE_TYPE eleVal[11] =
    {
        {APDU_APCI_INDEX, 0, &memVal[0]},
        {ASDU_TID_INDEX, 0, &memVal[1]},
        {ASDU_LNG_INDEX, 0, &memVal[2]},
        {ASDU_VSQ_INDEX, 0, &memVal[3]},
        {ASDU_COT_INDEX, 0, &memVal[4]},
        {ASDU_ADDR_INDEX, 0, &memVal[5]},
        {BODY_TID_INDEX, 0, &memVal[6]},
        {BODY_ADDR_INDEX, 0, &memVal[7]},
        {BODY_SET_INDEX, 0, &memVal[8]},
        {BODY_TSP_INDEX, 0, &memVal[9]},
        {ASDU_TSP_INDEX, 0, &memVal[10]},
    };

    * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_102;

    char tt = 2;
    u_char yi = 1 << 7 | tt;
    u_char gao = 127;
    u_char yi1 = gao >> 7;
    char* kk = &tt;

    char tt1 = '2';
    char* kk1 = &tt1;

    char A[10] = {'a', 'b', 'A', 'B', 1, 2, 3, 4};

    char* s = "abcdefgh //hijklmn";
    printf ( "%s\n", s );

    enum Color
    {
        GREEN = 1,
        RED,
        BLUE,
        GREEN_RED = 10,
        GREEN_BLUE
    } ColorVal;

    int s_c = sizeof ( ColorVal );
    int s_e = sizeof ( enum Color );
    //检查是大端模式还是小端模式存储
    union checkBigSmall
    {
        int i;
        u_char c;
    } tmpCheck;

    tmpCheck.i = 1;

    int a[5] = {1, 2, 3, 4, 5};
    int* ptr1 = ( int* ) ( &a + 1 );
    int* ptr2 = ( int* ) ( ( int ) a + 1 );
    printf ( "%x,%x", ptr1[-1], *ptr2 );


    u_char acsMthd = 130;
    u_char tmpT = 0;
    tmpT = acsMthd << 1;
    tmpT = ( acsMthd << 1 ) >> 1;
    tmpT = acsMthd & 0x7f;

    //T_ptclFunDsc tmpFuncDsc     = {CYC_TRNS,PASSIVE_TYPE};

    //u_short apduLng = 0;
    //u_char *asdu_p = ((APDU104*)apdu_p)->getASDU(apduLng,APDU_TYPE);
    //u_short tt = sizeof(BODY_ADDR_VAL);

    /*u_char tt_startChar = SRTCHAR_F1_2_V;
    u_char tt_long = 50;
    memcpy(DP_p,&tt_startChar,1);
    memcpy(DP_p+1,&tt_long,1);

    int tt2 = getApduLngFrmBuf((char*)DP_p,apduLong);*/

    //int t_tfu = ERR_POINTER_NULL;

    tt_test = sizeof ( frameType );
    u_int tt_test_1 = sizeof ( u_short );
    u_int tt_test_2 = sizeof ( unsigned short );

    /*u_short chkAPDUData(APDU_Buf* revDataFrm , void* rtmParam_p , void* cfgParam_p)*/
    revBuf tmp_APDU_Buf = {0};
    // tmp_APDU_Buf.APDU_Buf.APDU_Data_p = ((APDU104*)apdu_p)->getAPDU();
    tmp_APDU_Buf.APDU_Buf.APDU_Lng    = apduLong;

    APCI_104* apci_p = ( APCI_104* ) tmp_APDU_Buf.APDU_Buf.APDU_Data_p;
    apci_p->startChar = SRTCHAR_F1_2_V;
    apci_p->APDULng = apduLong - 2;

    realTimeThreshold tmpRealTimeThreshold_1 = {};
    realTimeValue tmpRealTimeValue_1 = {};
    u_short vUShort = chkAPDUData ( &tmp_APDU_Buf, &tmpRealTimeValue_1,
                                    &tmpRealTimeThreshold_1 );

    int vRet = 0;

    /*int UpdateSendState(short role, void *rtmParam_p, void *cfgParam_p ,u_char* tmrCtrlCode_p, u_char *data_p, u_short length)*/
    realTimeValue tmpRealTimeValue = {};
    realTimeThreshold tmpRealTimeThreshold = {};
    timerType tmpTimerType = {};
    APCI_104 tmpAPCI_104 = {};
    tmpRealTimeValue.stage = MASTER_PEND_STARTED;
    tmpAPCI_104.ctlField[0] = IEC104_STARTDT_ACK;
    tmpRealTimeThreshold.K = 2;
    vRet = UpdateSendState ( MASTER_STATION, &tmpRealTimeValue,
                             &tmpRealTimeThreshold,
                             ( u_char* ) &tmpTimerType, ( u_char* ) &tmpAPCI_104, 6 );
    /*int UpdateRecvState(short role, void *rtmParam_p, void *cfgParam_p, u_char *tmrCtrlCode_p, u_char *data_p, u_short length)*/
    vRet = UpdateRecvState ( MASTER_STATION, &tmpRealTimeValue,
                             &tmpRealTimeThreshold,
                             ( u_char* ) &tmpTimerType, ( u_char* ) &tmpAPCI_104, 6 );

    /*int RespCtrlFrame(transCtrlFrmDsc* ctrlFrm , void *rtmParam_p , int result)*/
    transCtrlFrmDsc tmpTransCtrlFrmDsc = {};
    vRet = 0;
    int tmpResult = I_F;
    APCI_104 tmp_APCI_104 = {};
    tmpTransCtrlFrmDsc.ctrlFrm_p = ( char* ) &tmp_APCI_104;
    //vRet = RespCtrlFrame(&tmpTransCtrlFrmDsc,&tmpRealTimeValue,tmpResult);

    /*int CtrlFrame(transCtrlFrmDsc* ctrlFrm , void *rtmParam_p , int result)*/
    tmpResult = S_F;
    vRet = CtrlFrame ( &tmpTransCtrlFrmDsc, &tmpRealTimeValue, tmpResult );

#endif//end FUSK_TEST



#ifdef FUSK_TEST_C_IC_NA
    const u_int stepNum = 2;//总共有多少步
    const u_int oneReqFrmNum = 1;//第一步请求帧数量
    const u_int oneResqFrmNum = 0;//第一步响应帧数量
    const u_int twoReqFrmNum = 0;//第二步请求帧数量
    const u_int twoResqFrmNum = 2;//第二步响应帧数量

    tmp_stnSnnCfg_p->ssnID = STN_INRGTN; //会话号
    tmp_stnSnnCfg_p->actFlag = ACTIVE_TYPE; //会话的主动性，保留
    tmp_stnSnnCfg_p->infoType =
    0; //会话的信息类型，通用信息对象或文件信息对象
    tmp_stnSnnCfg_p->cmnAddr = 1; //公共地址
    tmp_stnSnnCfg_p->stepNum = stepNum; //包含的会话总步数

    //////////////////////////////构造会话的配置的步数据///////////////////
    T_ssnStepCfg tmp_ssnStepCfg[stepNum] = {};//一共有stepNum步
    tmp_stnSnnCfg_p->stepCfg_p =
    tmp_ssnStepCfg;//把步结构关联到会话结构中去




    T_stnAcsFlag tmp_StnAcsFlag = {0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0}; //初始访问控制值
    tmp_ssnStepCfg[0].stnAcsFlag = * ( ( u_short* ) &tmp_StnAcsFlag );

    //====第1步的请求帧配置
    tmp_ssnStepCfg[0].reqAPDUNum = oneReqFrmNum; //请求帧个数
    u_char req_setNum[oneReqFrmNum] = {};//每一请求帧配置的点数 数据结构
    req_setNum[0] = 2;//赋值
    tmp_ssnStepCfg[0].reqBody_setNum_p = req_setNum;//关联到请求结构中去

    //构造第一步请求帧的数据结构
    ELE_TYPE reqOneEleVal[1][11] = {};
    /*ELE_TYPE** tReqEleVal_pp;
    ELE_TYPE* tReqEleVal_p;
    tReqEleVal_p = &reqEleVal[0][0];
    tReqEleVal_pp = &tReqEleVal_p;*/

    tmp_ssnStepCfg[0].reqEleVal_pp = reqOneEleVal;

    ////构造tid fusk test
    //u_char ftid = C_IC_NA;//召唤命令类型标识(tid)
    //reqEleVal[1][1].sysEleID = ASDU_TID_INDEX;
    //reqEleVal[1][1].lng = sizeof(u_char);
    //reqEleVal[1][1].p_Mem = &ftid;

    //ELE_TYPE f_ele_type = tmp_ssnStepCfg[0].reqEleVal_pp[1][1];


    //u_int tt_ss = sizeof(tmp_ssnStepCfg[0].reqEleVal_pp);

    ////////////////构造第一步请求帧数据
    //构造apci
    APCI_104 tmp_apci104;
    memset ( &tmp_apci104, 0, sizeof ( APCI_104 ) );
    tmp_apci104.startChar = 0x68;
    tmp_apci104.APDULng = ( u_char ) apduLong;
    tmp_apci104.ctlField[0] = 0;
    tmp_apci104.ctlField[1] = 0;
    tmp_apci104.ctlField[2] = 0;
    tmp_apci104.ctlField[3] = 0;
    tmp_ssnStepCfg[0].reqEleVal_pp[0][APDU_APCI_INDEX].sysEleID = APDU_APCI_INDEX;
    tmp_ssnStepCfg[0].reqEleVal_pp[0][APDU_APCI_INDEX].lng = sizeof ( APCI_104 );
    tmp_ssnStepCfg[0].reqEleVal_pp[0][APDU_APCI_INDEX].p_Mem =
    ( u_char* ) &tmp_apci104;
    //构造tid
    u_char tid = MC_TID_100;//召唤命令类型标识(tid)
    tmp_ssnStepCfg[0].reqEleVal_pp[0][ASDU_TID_INDEX].sysEleID = ASDU_TID_INDEX;
    tmp_ssnStepCfg[0].reqEleVal_pp[0][ASDU_TID_INDEX].lng = sizeof ( u_char );
    tmp_ssnStepCfg[0].reqEleVal_pp[0][ASDU_TID_INDEX].p_Mem = &tid;
    //构造vsq
    ASDU_VSQ_VAL vsq = {};
    memset ( &vsq, 0, sizeof ( T_VSQ ) );
    vsq.N = 1;
    vsq.SQ = 0;
    tmp_ssnStepCfg[0].reqEleVal_pp[0][ASDU_VSQ_INDEX].sysEleID = ASDU_VSQ_INDEX;
    tmp_ssnStepCfg[0].reqEleVal_pp[0][ASDU_VSQ_INDEX].lng = sizeof ( T_VSQ );
    tmp_ssnStepCfg[0].reqEleVal_pp[0][ASDU_VSQ_INDEX].p_Mem = ( u_char* ) &vsq;
    //构造cot
    ASDU_COT1_VAL cot;
    memset ( &cot, 0, sizeof ( ASDU_COT1_VAL ) );
    cot.CAUSE = MC_COT_6;
    tmp_ssnStepCfg[0].reqEleVal_pp[0][ASDU_COT_INDEX].sysEleID = ASDU_COT_INDEX;
    tmp_ssnStepCfg[0].reqEleVal_pp[0][ASDU_COT_INDEX].lng = sizeof (
            ASDU_COT1_VAL );
    tmp_ssnStepCfg[0].reqEleVal_pp[0][ASDU_COT_INDEX].p_Mem = ( u_char* ) &cot;
    //构造cmnAddr
    //u_char cmnAddr = 5;
    ASDU_ADDR_VAL cmnAddr = {0};
    cmnAddr.shortAddr = 5 ;
    tmp_ssnStepCfg[0].reqEleVal_pp[0][ASDU_ADDR_INDEX].sysEleID = ASDU_ADDR_INDEX;
    tmp_ssnStepCfg[0].reqEleVal_pp[0][ASDU_ADDR_INDEX].lng = sizeof (
            ASDU_ADDR_VAL );
    tmp_ssnStepCfg[0].reqEleVal_pp[0][ASDU_ADDR_INDEX].p_Mem = ( u_char* ) &cmnAddr;
    //构造信息体地址BODY_ADDR
    BODY_ADDR_VAL bodyAddr = {0};
    bodyAddr.shortAddr = 0;
    tmp_ssnStepCfg[0].reqEleVal_pp[0][BODY_ADDR_INDEX].sysEleID = BODY_ADDR_INDEX;
    tmp_ssnStepCfg[0].reqEleVal_pp[0][BODY_ADDR_INDEX].lng = sizeof (
            BODY_ADDR_VAL );
    tmp_ssnStepCfg[0].reqEleVal_pp[0][BODY_ADDR_INDEX].p_Mem =
    ( u_char* ) &bodyAddr;
    //构造QCC 计数量召唤命令限定词
    u_char* QCC = new u_char();
    *QCC = 20;//站召唤
    tmp_ssnStepCfg[0].reqEleVal_pp[0][BODY_SET_INDEX].sysEleID = BODY_SET_INDEX;
    tmp_ssnStepCfg[0].reqEleVal_pp[0][BODY_SET_INDEX].lng = sizeof ( u_char );
    tmp_ssnStepCfg[0].reqEleVal_pp[0][BODY_SET_INDEX].p_Mem = QCC;

    //====第1步的响应帧配置
    tmp_ssnStepCfg[0].respAPDUNum = oneResqFrmNum; //响应帧个数0

    //第2步＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
    //请求
    tmp_ssnStepCfg[1].reqAPDUNum = twoReqFrmNum;//0
    //====响应
    tmp_ssnStepCfg[1].respAPDUNum = twoResqFrmNum;
    //下面构造步里的 ELE_TYPE***      respEleVal
    ELE_TYPE respEleVal[twoResqFrmNum][11] = {0};
    /*ELE_TYPE** tRespEleVal_pp;
    ELE_TYPE* tRespEleVal_p;
    tRespEleVal_p = &respEleVal[0][0];
    tRespEleVal_pp = &tRespEleVal_p;*/
    tmp_ssnStepCfg[1].respEleVal_pp = respEleVal;
    u_char resp_setNum[twoResqFrmNum] = {2, 1}; //每一响应帧点数量
    tmp_ssnStepCfg[1].respBody_setNum_p = resp_setNum;//响应点数量
    /***************************第一帧***********************************/
    //构造apci
    APCI_104 tmp_resp_apci104;
    memset ( &tmp_resp_apci104, 0, sizeof ( APCI_104 ) );
    tmp_resp_apci104.startChar = 0x68;
    tmp_resp_apci104.APDULng = ( u_char ) apduLong;
    tmp_resp_apci104.ctlField[0] = 0;
    tmp_resp_apci104.ctlField[1] = 0;
    tmp_resp_apci104.ctlField[2] = 0;
    tmp_resp_apci104.ctlField[3] = 0;
    tmp_ssnStepCfg[1].respEleVal_pp[0][APDU_APCI_INDEX].sysEleID = APDU_APCI_INDEX;
    tmp_ssnStepCfg[1].respEleVal_pp[0][APDU_APCI_INDEX].lng = sizeof ( APCI_104 );
    tmp_ssnStepCfg[1].respEleVal_pp[0][APDU_APCI_INDEX].p_Mem =
    ( u_char* ) &tmp_resp_apci104;
    //构造tid
    u_char resp_tid = MC_TID_1;//召唤命令类型标识(tid)
    tmp_ssnStepCfg[1].respEleVal_pp[0][ASDU_TID_INDEX].sysEleID = ASDU_TID_INDEX;
    tmp_ssnStepCfg[1].respEleVal_pp[0][ASDU_TID_INDEX].lng = sizeof ( u_char );
    tmp_ssnStepCfg[1].respEleVal_pp[0][ASDU_TID_INDEX].p_Mem = &resp_tid;
    //构造vsq
    ASDU_VSQ_VAL resp_vsq;
    memset ( &resp_vsq, 0, sizeof ( T_VSQ ) );
    resp_vsq.N = 2;
    resp_vsq.SQ = 1;
    tmp_ssnStepCfg[1].respEleVal_pp[0][ASDU_VSQ_INDEX].sysEleID = ASDU_VSQ_INDEX;
    tmp_ssnStepCfg[1].respEleVal_pp[0][ASDU_VSQ_INDEX].lng = sizeof ( T_VSQ );
    tmp_ssnStepCfg[1].respEleVal_pp[0][ASDU_VSQ_INDEX].p_Mem =
    ( u_char* ) &resp_vsq;
    //构造cot
    ASDU_COT1_VAL resp_cot;
    memset ( &resp_cot, 0, sizeof ( ASDU_COT1_VAL ) );
    resp_cot.CAUSE = MC_COT_20;
    tmp_ssnStepCfg[1].respEleVal_pp[0][ASDU_COT_INDEX].sysEleID = ASDU_COT_INDEX;
    tmp_ssnStepCfg[1].respEleVal_pp[0][ASDU_COT_INDEX].lng = sizeof (
            ASDU_COT1_VAL );
    tmp_ssnStepCfg[1].respEleVal_pp[0][ASDU_COT_INDEX].p_Mem =
    ( u_char* ) &resp_cot;
    //构造cmnAddr
    //u_char resp_cmnAddr = 51;
    ASDU_ADDR_VAL resp_cmnAddr = {0};
    resp_cmnAddr.shortAddr = 51 ;
    tmp_ssnStepCfg[1].respEleVal_pp[0][ASDU_ADDR_INDEX].sysEleID = ASDU_ADDR_INDEX;
    tmp_ssnStepCfg[1].respEleVal_pp[0][ASDU_ADDR_INDEX].lng = sizeof (
            ASDU_ADDR_VAL );
    tmp_ssnStepCfg[1].respEleVal_pp[0][ASDU_ADDR_INDEX].p_Mem =
    ( u_char* ) &resp_cmnAddr;
    //构造信息体地址BODY_ADDR
    //u_char resp_bodyAddr = 71;
    BODY_ADDR_VAL resp_bodyAddr = {0};
    resp_bodyAddr.shortAddr = 71;
    tmp_ssnStepCfg[1].respEleVal_pp[0][BODY_ADDR_INDEX].sysEleID = BODY_ADDR_INDEX;
    tmp_ssnStepCfg[1].respEleVal_pp[0][BODY_ADDR_INDEX].lng = sizeof (
            BODY_ADDR_VAL );
    tmp_ssnStepCfg[1].respEleVal_pp[0][BODY_ADDR_INDEX].p_Mem =
    ( u_char* ) &resp_bodyAddr;
    //构造信息元素集 BODY_SET
    //构造2个点值
    u_int rspPntVal1 = 222;
    u_int rspPntVal2 = 223;
    u_char* rspUChar_p = ( u_char* ) new unsigned int[2]();
    memcpy ( rspUChar_p, &rspPntVal1, sizeof ( u_int ) );
    memcpy ( rspUChar_p + sizeof ( u_int ), &rspPntVal2, sizeof ( u_int ) );
    tmp_ssnStepCfg[1].respEleVal_pp[0][BODY_SET_INDEX].sysEleID = BODY_SET_INDEX;
    tmp_ssnStepCfg[1].respEleVal_pp[0][BODY_SET_INDEX].lng = sizeof ( u_int );
    tmp_ssnStepCfg[1].respEleVal_pp[0][BODY_SET_INDEX].p_Mem = rspUChar_p;
    //信息体时标 BODY_TSP
    CP24TIME2a resp_tspVal = {0};
    resp_tspVal.mScnd = 8;
    resp_tspVal.min = 1;
    resp_tspVal.GEN = 1;
    resp_tspVal.IV = 1;
    tmp_ssnStepCfg[1].respEleVal_pp[0][BODY_TSP_INDEX].sysEleID = BODY_TSP_INDEX;
    tmp_ssnStepCfg[1].respEleVal_pp[0][BODY_TSP_INDEX].lng = sizeof ( CP24TIME2a );
    tmp_ssnStepCfg[1].respEleVal_pp[0][BODY_TSP_INDEX].p_Mem =
    ( u_char* ) &resp_tspVal;

    /***************************第二帧***********************************/
    //构造apci
    APCI_104 tmp_resp2_apci104;
    memset ( &tmp_resp2_apci104, 0, sizeof ( APCI_104 ) );
    tmp_resp2_apci104.startChar = 0x68;
    tmp_resp2_apci104.APDULng = ( u_char ) apduLong;
    tmp_resp2_apci104.ctlField[0] = 0;
    tmp_resp2_apci104.ctlField[1] = 0;
    tmp_resp2_apci104.ctlField[2] = 0;
    tmp_resp2_apci104.ctlField[3] = 0;
    tmp_ssnStepCfg[1].respEleVal_pp[1][APDU_APCI_INDEX].sysEleID = APDU_APCI_INDEX;
    tmp_ssnStepCfg[1].respEleVal_pp[1][APDU_APCI_INDEX].lng = sizeof ( APCI_104 );
    tmp_ssnStepCfg[1].respEleVal_pp[1][APDU_APCI_INDEX].p_Mem =
    ( u_char* ) &tmp_resp2_apci104;
    //构造tid
    u_char resp2_tid = MC_TID_3;//召唤命令类型标识(tid)
    tmp_ssnStepCfg[1].respEleVal_pp[1][ASDU_TID_INDEX].sysEleID = ASDU_TID_INDEX;
    tmp_ssnStepCfg[1].respEleVal_pp[1][ASDU_TID_INDEX].lng = sizeof ( u_char );
    tmp_ssnStepCfg[1].respEleVal_pp[1][ASDU_TID_INDEX].p_Mem = &resp2_tid;
    //构造vsq
    ASDU_VSQ_VAL resp2_vsq;
    memset ( &resp2_vsq, 0, sizeof ( T_VSQ ) );
    resp2_vsq.N = 2;
    resp2_vsq.SQ = 1;
    tmp_ssnStepCfg[1].respEleVal_pp[1][ASDU_VSQ_INDEX].sysEleID = ASDU_VSQ_INDEX;
    tmp_ssnStepCfg[1].respEleVal_pp[1][ASDU_VSQ_INDEX].lng = sizeof ( T_VSQ );
    tmp_ssnStepCfg[1].respEleVal_pp[1][ASDU_VSQ_INDEX].p_Mem =
    ( u_char* ) &resp2_vsq;
    //构造cot
    ASDU_COT1_VAL resp2_cot;
    memset ( &resp2_cot, 0, sizeof ( ASDU_COT1_VAL ) );
    resp2_cot.CAUSE = MC_COT_20;
    tmp_ssnStepCfg[1].respEleVal_pp[1][ASDU_COT_INDEX].sysEleID = ASDU_COT_INDEX;
    tmp_ssnStepCfg[1].respEleVal_pp[1][ASDU_COT_INDEX].lng = sizeof (
            ASDU_COT1_VAL );
    tmp_ssnStepCfg[1].respEleVal_pp[1][ASDU_COT_INDEX].p_Mem =
    ( u_char* ) &resp2_cot;
    //构造cmnAddr
    //u_char resp2_cmnAddr = 51;
    ASDU_ADDR_VAL resp2_cmnAddr = {0};
    resp2_cmnAddr.shortAddr = 52 ;
    tmp_ssnStepCfg[1].respEleVal_pp[1][ASDU_ADDR_INDEX].sysEleID = ASDU_ADDR_INDEX;
    tmp_ssnStepCfg[1].respEleVal_pp[1][ASDU_ADDR_INDEX].lng = sizeof (
            ASDU_ADDR_VAL );
    tmp_ssnStepCfg[1].respEleVal_pp[1][ASDU_ADDR_INDEX].p_Mem =
    ( u_char* ) &resp2_cmnAddr;
    //构造信息体地址BODY_ADDR
    //u_char resp2_bodyAddr = 71;
    BODY_ADDR_VAL resp2_bodyAddr = {0};
    resp2_bodyAddr.shortAddr = 72;
    tmp_ssnStepCfg[1].respEleVal_pp[1][BODY_ADDR_INDEX].sysEleID = BODY_ADDR_INDEX;
    tmp_ssnStepCfg[1].respEleVal_pp[1][BODY_ADDR_INDEX].lng = sizeof (
            BODY_ADDR_VAL );
    tmp_ssnStepCfg[1].respEleVal_pp[1][BODY_ADDR_INDEX].p_Mem =
    ( u_char* ) &resp2_bodyAddr;
    //构造信息元素集 BODY_SET
    //构造1个点值
    u_int rsp2PntVal1 = 124;
    u_char* rsp2UChar_p = ( u_char* ) new unsigned int();
    memcpy ( rsp2UChar_p, &rsp2PntVal1, sizeof ( u_int ) );
    tmp_ssnStepCfg[1].respEleVal_pp[1][BODY_SET_INDEX].sysEleID = BODY_SET_INDEX;
    tmp_ssnStepCfg[1].respEleVal_pp[1][BODY_SET_INDEX].lng = sizeof ( u_int );
    tmp_ssnStepCfg[1].respEleVal_pp[1][BODY_SET_INDEX].p_Mem = rsp2UChar_p;
    //信息体时标 BODY_TSP
    CP24TIME2a resp2_tspVal = {0};
    resp2_tspVal.mScnd = 9;
    resp2_tspVal.min = 2;
    resp2_tspVal.GEN = 2;
    resp2_tspVal.IV = 2;
    tmp_ssnStepCfg[1].respEleVal_pp[1][BODY_TSP_INDEX].sysEleID = BODY_TSP_INDEX;
    tmp_ssnStepCfg[1].respEleVal_pp[1][BODY_TSP_INDEX].lng = sizeof ( CP24TIME2a );
    tmp_ssnStepCfg[1].respEleVal_pp[1][BODY_TSP_INDEX].p_Mem =
    ( u_char* ) &resp2_tspVal;


    int vRetNxtStop = 0;
    int vRetChkFun  = 0;
    u_int nxtStepNo = 0;
    u_int* nxtStepNo_p = &nxtStepNo;
    int vRetSrtStep = 0;

    //2、调用createRl_P
    FUN_RL* vRl_P = createRl_P ( 6, ONLY_ONE, allSnnCfg_p, allPtclFuncDsc );

    //u_char tmp_acsMthd[2] = {0};
    u_int tmp_ASDUID[5] = {0};
    T_snnUsrData tmp_snnUsrData[5] = {0};
    tmp_snnUsrData[0].gData.tmStmp_p = new CP24TIME2a();
    tmp_snnUsrData[1].gData.tmStmp_p = new CP24TIME2a();
    tmp_snnUsrData[2].gData.tmStmp_p = new CP24TIME2a();
    tmp_snnUsrData[3].gData.tmStmp_p = new CP24TIME2a();
    tmp_snnUsrData[4].gData.tmStmp_p = new CP24TIME2a();
    //t_snnPara->acsMthd = tmp_acsMthd;
    t_snnPara->ASDUID_p = tmp_ASDUID;
    t_snnPara->usrData_p = tmp_snnUsrData;
    APDU_DSCR* apdu_dscr = ( ( APDU104* ) apdu_p )->getDU_Info();

    getGLoadPnt ( allSnnCfg_p, 1, t_snnPara );



    //为preStepRul_p中的指针变量分配空间
    ///////////////////////////////////////////站召唤 第一步主动/////////////////////////////////////////////////
    preStepRul_p->reqAPDU_Obj_p = apdu_p;
    preStepRul_p->respAPDU_Obj_p = apdu_p;
    preStepRul_p->sVerifyBlock_p = new vrfItem();
    preStepRul_p->sVerifyBlock_p->chkRslt_p = new int();
    ///////////////////////////////////////////站召唤 第一步被动 不再调用:startStep/////////////////////////////////////////////////
    //vRetSrtStep = startStep(U_TESTFR,ONLY_ONE,preStepRul_p,t_snnPara);
    vRetSrtStep = startStep ( STN_INRGTN, ONLY_ONE, preStepRul_p, t_snnPara );

    allPtclFuncDsc->act_prio_chnlID = ACTIVE_TYPE;
    preStepRul_p->stepNo = 1;
    preStepRul_p->reqPDU_Num = 1;
    vRetNxtStop = nxtStep ( vRl_P, preStepRul_p, allPtclFuncDsc, t_snnPara,
                            nxtStepNo_p );
    allPtclFuncDsc->act_prio_chnlID = PASSIVE_TYPE;
    vRetChkFun = chkFun ( vRl_P, preStepRul_p, allPtclFuncDsc, t_snnPara,
                          nxtStepNo_p );

    t_snnPara->actFlag = PASSIVE_TYPE;
    getGLoadPnt ( allSnnCfg_p, 2, t_snnPara );

    preStepRul_p->stepNo = 2;
    preStepRul_p->reqPDU_Num = 0;
    preStepRul_p->respPDU_Num = 0;
    allPtclFuncDsc->act_prio_chnlID = PASSIVE_TYPE;
    vRetNxtStop = nxtStep ( vRl_P, preStepRul_p, allPtclFuncDsc, t_snnPara,
                            nxtStepNo_p );

    allPtclFuncDsc->act_prio_chnlID = ACTIVE_TYPE;
    preStepRul_p->respPDU_Num = 1;
    vRetChkFun = chkFun ( vRl_P, preStepRul_p, allPtclFuncDsc, t_snnPara,
                          nxtStepNo_p );

    T_ptclFunDsc tmpFunDsc;
    memset ( &tmpFunDsc, 0, sizeof ( T_ptclFunDsc ) );

    tmpFunDsc.ssnDscNo = STN_INRGTN;//回话号
    //tmpFunDsc.act_prio_chnlID = ACTIVE_TYPE;//ACTIVE_TYPE  PASSIVE_TYPE
    ///////////////////////////////////////////站召唤 第一步被动 不再调用:startStep/////////////////////////////////////////////////
    tmpFunDsc.act_prio_chnlID = PASSIVE_TYPE;//ACTIVE_TYPE  PASSIVE_TYPE
    preStepRul_p->stepNo = 2;
    preStepRul_p->reqPDU_Num = 0;
    vRetNxtStop = nxtStep ( vRl_P, preStepRul_p, &tmpFunDsc, t_snnPara,
                            nxtStepNo_p );

    //6、调用chkFun
    /*int chkFun(FUN_RL* Fun_p ,ONE_STEP* stepRl_p , T_ptclFunDsc* funDsc_p ,
           T_snnPara* ssnPar_p , u_int* nxtStepNo_p)*/
    tmpFunDsc.act_prio_chnlID = ACTIVE_TYPE;
    preStepRul_p->respPDU_Num = 1;

    //apdu_dscr->ASDU_CMTBL[BODY_SET_INDEX]=1;
    vRetChkFun = chkFun ( vRl_P, preStepRul_p, &tmpFunDsc, t_snnPara, nxtStepNo_p );

    tmpFunDsc.act_prio_chnlID = PASSIVE_TYPE;
    preStepRul_p->respPDU_Num = 1;
    vRetNxtStop = nxtStep ( vRl_P, preStepRul_p, &tmpFunDsc, t_snnPara,
                            nxtStepNo_p );

    tmpFunDsc.act_prio_chnlID = ACTIVE_TYPE;
    preStepRul_p->respPDU_Num = 2;
    vRetChkFun = chkFun ( vRl_P, preStepRul_p, &tmpFunDsc, t_snnPara, nxtStepNo_p );


#endif//FUSK_TEST_C_IC_NA

#ifdef FUSK_TEST_C_RD_NA
    tmp_stnSnnCfg_p->ssnID = QUE_DATA; //会话号
    tmp_stnSnnCfg_p->actFlag = ACTIVE_TYPE; //会话的主动性，保留
    tmp_stnSnnCfg_p->infoType =
    0; //会话的信息类型，通用信息对象或文件信息对象
    tmp_stnSnnCfg_p->cmnAddr = 1; //公共地址
    tmp_stnSnnCfg_p->stepNum = 1; //包含的会话总步数

    //////////////////////////////构造第一步的T_ssnStepCfg会话步配置
    //会话步的一步配置
    T_ssnStepCfg tmp_ssnStepCfg[1] = {0};
    T_stnAcsFlag tmp_StnAcsFlag = {0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0};
    tmp_ssnStepCfg[0].stnAcsFlag = * ( ( u_short* ) &tmp_StnAcsFlag );

    //====请求
    tmp_ssnStepCfg[0].reqAPDUNum = 1; //请求帧个数
    u_char req_setNum[1] = {0};
    tmp_ssnStepCfg[0].reqBody_setNum_p = req_setNum; //点数量
    //====响应
    tmp_ssnStepCfg[0].respAPDUNum = 1;
    u_char resp_setNum[1] = {2};
    tmp_ssnStepCfg[0].respBody_setNum_p = resp_setNum;//响应点数量

    //============请求
    //下面构造步里的 ELE_TYPE***      reqEleVal
    ELE_TYPE reqOneEleVal[1][11] = {0};
    /*ELE_TYPE** tReqEleVal_pp;
    ELE_TYPE* tReqEleVal_p;
    tReqEleVal_p = &reqEleVal[0][0];
    tReqEleVal_pp = &tReqEleVal_p;*/

    tmp_ssnStepCfg[0].reqEleVal_pp = reqOneEleVal;

    //构造apci
    APCI_104 tmp_apci104 = {};
    //memset(&tmp_apci104,0,sizeof(APCI_104));
    tmp_apci104.startChar = 0x68;
    tmp_apci104.APDULng = apduLong;
    tmp_ssnStepCfg[0].reqEleVal_pp[0][APDU_APCI_INDEX].sysEleID = APDU_APCI_INDEX;
    tmp_ssnStepCfg[0].reqEleVal_pp[0][APDU_APCI_INDEX].lng = sizeof ( APCI_104 );
    tmp_ssnStepCfg[0].reqEleVal_pp[0][APDU_APCI_INDEX].p_Mem =
    ( u_char* ) &tmp_apci104;

    //构造tid
    u_char tid = C_RD_NA;//读命令类型标识(tid)
    tmp_ssnStepCfg[0].reqEleVal_pp[0][ASDU_TID_INDEX].sysEleID = ASDU_TID_INDEX;
    tmp_ssnStepCfg[0].reqEleVal_pp[0][ASDU_TID_INDEX].lng = sizeof ( u_char );
    tmp_ssnStepCfg[0].reqEleVal_pp[0][ASDU_TID_INDEX].p_Mem = &tid;

    //构造vsq
    T_VSQ vsq;
    memset ( &vsq, 0, sizeof ( T_VSQ ) );
    vsq.vsq_n = 0;
    vsq.vsq_sq = 1;
    tmp_ssnStepCfg[0].reqEleVal_pp[0][ASDU_VSQ_INDEX].sysEleID = ASDU_VSQ_INDEX;
    tmp_ssnStepCfg[0].reqEleVal_pp[0][ASDU_VSQ_INDEX].lng = sizeof ( T_VSQ );
    tmp_ssnStepCfg[0].reqEleVal_pp[0][ASDU_VSQ_INDEX].p_Mem = ( u_char* ) &vsq;

    //构造cot
    ASDU_COT1_VAL cot;
    memset ( &cot, 0, sizeof ( ASDU_COT1_VAL ) );
    cot.CAUSE = MC_COT_5;
    tmp_ssnStepCfg[0].reqEleVal_pp[0][ASDU_COT_INDEX].sysEleID = ASDU_COT_INDEX;
    tmp_ssnStepCfg[0].reqEleVal_pp[0][ASDU_COT_INDEX].lng = sizeof (
            ASDU_COT1_VAL );
    tmp_ssnStepCfg[0].reqEleVal_pp[0][ASDU_COT_INDEX].p_Mem = ( u_char* ) &cot;

    //构造cmnAddr
    //u_char cmnAddr = 5;
    ASDU_ADDR_VAL cmnAddr = {0};
    cmnAddr.shortAddr = 15 ;
    tmp_ssnStepCfg[0].reqEleVal_pp[0][ASDU_ADDR_INDEX].sysEleID = ASDU_ADDR_INDEX;
    tmp_ssnStepCfg[0].reqEleVal_pp[0][ASDU_ADDR_INDEX].lng = sizeof (
            ASDU_ADDR_VAL );
    tmp_ssnStepCfg[0].reqEleVal_pp[0][ASDU_ADDR_INDEX].p_Mem = ( u_char* ) &cmnAddr;

    ////构造信息体地址BODY_ADDR
    ////u_char bodyAddr = 7;
    //BODY_ADDR_VAL bodyAddr ={0};
    //bodyAddr.shortAddr = 17;
    //tmp_ssnStepCfg[0].reqEleVal_pp[0][BODY_ADDR_INDEX].sysEleID = BODY_ADDR_INDEX;
    //tmp_ssnStepCfg[0].reqEleVal_pp[0][BODY_ADDR_INDEX].lng = sizeof(BODY_ADDR_VAL);
    //tmp_ssnStepCfg[0].reqEleVal_pp[0][BODY_ADDR_INDEX].p_Mem = (u_char*)&bodyAddr;

    ////构造信息元素集 BODY_SET
    ////构造2个点值
    //u_int tPntVal1 = 110;
    //u_int tPntVal2 = 113;
    //u_char* tmpUChar_p =(u_char*) new unsigned int[2]();
    //memcpy(tmpUChar_p,&tPntVal1,sizeof(u_int));
    //memcpy(tmpUChar_p+sizeof(u_int),&tPntVal2,sizeof(u_int));

    //tmp_ssnStepCfg[0].reqEleVal_pp[0][BODY_SET_INDEX].sysEleID = BODY_SET_INDEX;
    //tmp_ssnStepCfg[0].reqEleVal_pp[0][BODY_SET_INDEX].lng = sizeof(u_int) * 2;
    //tmp_ssnStepCfg[0].reqEleVal_pp[0][BODY_SET_INDEX].p_Mem = tmpUChar_p;




    //=================================================响应
    //下面构造步里的 ELE_TYPE***      respEleVal
    ELE_TYPE respEleVal[1][11] = {0};
    /*ELE_TYPE** tRespEleVal_pp;
    ELE_TYPE* tRespEleVal_p;
    tRespEleVal_p = &respEleVal[0][0];
    tRespEleVal_pp = &tRespEleVal_p;*/

    tmp_ssnStepCfg[0].respEleVal_pp = respEleVal;

    //构造apci
    APCI_104 tmp_resp_apci104;
    memset ( &tmp_resp_apci104, 0, sizeof ( APCI_104 ) );
    tmp_resp_apci104.startChar = 0x68;
    tmp_resp_apci104.APDULng = apduLong;
    tmp_resp_apci104.ctlField[0] = 0;
    tmp_resp_apci104.ctlField[1] = 0;
    tmp_resp_apci104.ctlField[2] = 0;
    tmp_resp_apci104.ctlField[3] = 0;
    tmp_ssnStepCfg[0].respEleVal_pp[0][APDU_APCI_INDEX].sysEleID = APDU_APCI_INDEX;
    tmp_ssnStepCfg[0].respEleVal_pp[0][APDU_APCI_INDEX].lng = sizeof ( APCI_104 );
    tmp_ssnStepCfg[0].respEleVal_pp[0][APDU_APCI_INDEX].p_Mem =
    ( u_char* ) &tmp_resp_apci104;

    //构造tid
    u_char resp_tid = C_RD_NA;//召唤命令类型标识(tid)
    tmp_ssnStepCfg[0].respEleVal_pp[0][ASDU_TID_INDEX].sysEleID = ASDU_TID_INDEX;
    tmp_ssnStepCfg[0].respEleVal_pp[0][ASDU_TID_INDEX].lng = sizeof ( u_char );
    tmp_ssnStepCfg[0].respEleVal_pp[0][ASDU_TID_INDEX].p_Mem = &resp_tid;

    //构造vsq
    T_VSQ resp_vsq;
    memset ( &resp_vsq, 0, sizeof ( T_VSQ ) );
    resp_vsq.vsq_n = 2;
    resp_vsq.vsq_sq = 1;
    tmp_ssnStepCfg[0].respEleVal_pp[0][ASDU_VSQ_INDEX].sysEleID = ASDU_VSQ_INDEX;
    tmp_ssnStepCfg[0].respEleVal_pp[0][ASDU_VSQ_INDEX].lng = sizeof ( T_VSQ );
    tmp_ssnStepCfg[0].respEleVal_pp[0][ASDU_VSQ_INDEX].p_Mem =
    ( u_char* ) &resp_vsq;

    //构造cot
    ASDU_COT1_VAL resp_cot;
    memset ( &resp_cot, 0, sizeof ( ASDU_COT1_VAL ) );
    resp_cot.CAUSE = MC_COT_5;
    tmp_ssnStepCfg[0].respEleVal_pp[0][ASDU_COT_INDEX].sysEleID = ASDU_COT_INDEX;
    tmp_ssnStepCfg[0].respEleVal_pp[0][ASDU_COT_INDEX].lng = sizeof (
            ASDU_COT1_VAL );
    tmp_ssnStepCfg[0].respEleVal_pp[0][ASDU_COT_INDEX].p_Mem =
    ( u_char* ) &resp_cot;

    //构造cmnAddr
    //u_char resp_cmnAddr = 51;
    ASDU_ADDR_VAL resp_cmnAddr = {0};
    resp_cmnAddr.shortAddr = 16 ;
    tmp_ssnStepCfg[0].respEleVal_pp[0][ASDU_ADDR_INDEX].sysEleID = ASDU_ADDR_INDEX;
    tmp_ssnStepCfg[0].respEleVal_pp[0][ASDU_ADDR_INDEX].lng = sizeof (
            ASDU_ADDR_VAL );
    tmp_ssnStepCfg[0].respEleVal_pp[0][ASDU_ADDR_INDEX].p_Mem =
    ( u_char* ) &resp_cmnAddr;

    //构造信息体地址BODY_ADDR
    //u_char resp_bodyAddr = 71;
    BODY_ADDR_VAL resp_bodyAddr = {0};
    resp_bodyAddr.shortAddr = 18;
    tmp_ssnStepCfg[0].respEleVal_pp[0][BODY_ADDR_INDEX].sysEleID = BODY_ADDR_INDEX;
    tmp_ssnStepCfg[0].respEleVal_pp[0][BODY_ADDR_INDEX].lng = sizeof (
            BODY_ADDR_VAL );
    tmp_ssnStepCfg[0].respEleVal_pp[0][BODY_ADDR_INDEX].p_Mem =
    ( u_char* ) &resp_bodyAddr;

    //构造信息元素集 BODY_SET
    //构造2个点值
    u_int rspPntVal1 = 121;
    u_int rspPntVal2 = 124;
    u_char* rspUChar_p = ( u_char* ) new unsigned int[2]();
    memcpy ( rspUChar_p, &rspPntVal1, sizeof ( u_int ) );
    memcpy ( rspUChar_p + sizeof ( u_int ), &rspPntVal2, sizeof ( u_int ) );
    tmp_ssnStepCfg[0].respEleVal_pp[0][BODY_SET_INDEX].sysEleID = BODY_SET_INDEX;
    tmp_ssnStepCfg[0].respEleVal_pp[0][BODY_SET_INDEX].lng = sizeof ( u_int );
    tmp_ssnStepCfg[0].respEleVal_pp[0][BODY_SET_INDEX].p_Mem = rspUChar_p;

    //信息体时标 BODY_TSP
    CP24TIME2a resp_tspVal = {0};
    resp_tspVal.mScnd = 8;
    resp_tspVal.min = 1;
    resp_tspVal.GEN = 1;
    resp_tspVal.IV = 1;
    tmp_ssnStepCfg[0].respEleVal_pp[0][BODY_TSP_INDEX].sysEleID = BODY_TSP_INDEX;
    tmp_ssnStepCfg[0].respEleVal_pp[0][BODY_TSP_INDEX].lng = sizeof ( CP24TIME2a );
    tmp_ssnStepCfg[0].respEleVal_pp[0][BODY_TSP_INDEX].p_Mem =
    ( u_char* ) &resp_tspVal;



    tmp_stnSnnCfg_p->stepCfg_p = tmp_ssnStepCfg;


    //2、调用createRl_P
    FUN_RL* vRl_P = createRl_P ( QUE_DATA, RD_DATA, allSnnCfg_p );

    u_char tmp_acsMthd[2] = {0};
    u_int tmp_ASDUID[2] = {0};
    T_snnUsrData tmp_snnUsrData[2] = {0};
    tmp_snnUsrData[0].gData.tmStmp_p = new CP24TIME2a();
    tmp_snnUsrData[1].gData.tmStmp_p = new CP24TIME2a();
    // t_snnPara->acsMthd = tmp_acsMthd;
    t_snnPara->ASDUID_p = tmp_ASDUID;
    t_snnPara->usrData_p = tmp_snnUsrData;

    t_snnPara->actFlag = ACTIVE_TYPE;
    getGLoadPnt ( tmp_stnSnnCfg_p, 1, t_snnPara );

    //4、调用 startStep
    //int startStep(u_short ssnNo ,u_char usingNode,allSnnCfg_p,ONE_STEP* preStepRul_p , T_snnPara* statrtStepStnD_p)
    int vRetSrtStep = 0;

    //为preStepRul_p中的指针变量分配空间
    ///////////////////////////////////////////站召唤 第一步主动/////////////////////////////////////////////////
    preStepRul_p->reqAPDU_Obj_p = apdu_p;
    preStepRul_p->respAPDU_Obj_p = apdu_p;
    //preStepRul_p->sVerifyBlock_p = new vrfItem();
    //preStepRul_p->sVerifyBlock_p->chkRslt_p = new int();
    ///////////////////////////////////////////站召唤 第一步被动 不再调用:startStep/////////////////////////////////////////////////
    vRetSrtStep = startStep ( QUE_DATA, RD_DATA, preStepRul_p, t_snnPara );

    //5、调用nxtStep

    T_ptclFunDsc tmpFunDsc = {};
    u_int nxtStepNo = 0;
    u_int* nxtStepNo_p = &nxtStepNo;
    tmpFunDsc.ssnDscNo = QUE_DATA;//回话号
    tmpFunDsc.act_prio_chnlID = PASSIVE_TYPE;//ACTIVE_TYPE  PASSIVE_TYPE

    //preStepRul_p->stepNo = 2;
    int vRetNxtStop = nxtStep ( vRl_P, preStepRul_p, &tmpFunDsc, t_snnPara,
                                nxtStepNo_p );

    //6、调用chkFun
    /*int chkFun(FUN_RL* Fun_p ,ONE_STEP* stepRl_p , T_ptclFunDsc* funDsc_p ,
           T_snnPara* ssnPar_p , u_int* nxtStepNo_p)*/
    int vRetChkFun = chkFun ( vRl_P, preStepRul_p, &tmpFunDsc, t_snnPara,
                              nxtStepNo_p );

    int vRet = 0;
    u_char tmpFunNum = 0;
    T_ptclFunDsc tmpGetFuncDsc[13] = {};
    //vRet = getSsnSet(1 , C_COT_5 , tmpGetFuncDsc ,&tmpFunNum);
    vRet = getSsnSet ( 102, MC_COT_5, tmpGetFuncDsc, &tmpFunNum );
    DP_p = ( ( APDU104* ) apdu_p )->getDU ( apduLong, ASDU_TYPE );
    ASDU_VSQ_VAL ttVSQ = {1, 0};
    ( ( APDU104* ) apdu_p )->setASDU_VSQ_VAL ( &ttVSQ );
    vRet = getFunStep ( &tmpGetFuncDsc[0],
                        102, MC_COT_5,
                        apdu_p,
                        DP_p,
                        apduLong
                      );


#endif//FUSK_TEST_C_RD_NA


#ifdef FUSK_TEST
    T_ptclFunDsc tmpFunDsc = {};

    /////////freeRl_P(vRl_P);

    //int chkASDU(u_char* ASDU_p , u_short ASDU_lng , u_int* TID , u_int* COT)
    u_int chkASDU_TID = 0;
    u_int chkASDU_COT = 0;
    //int vRetChkASDU = chkASDU(getASDU(apdu_p),apduLong - 6,&chkASDU_TID,&chkASDU_COT);

    //7、调用getSsnSet
    //int getSsnSet(u_int TID , u_int COT , T_ptclFunDsc* funDsc_p , u_char* funNum_p)
    u_char funNum = 0;
    T_ptclFunDsc tmpFunDscArr[13] = {};
    //int vRetGetSsnSet = getSsnSet(1,5,tmpFunDscArr,&funNum);
    int vRetGetSsnSet = getSsnSet ( 59, 45, tmpFunDscArr, &funNum );

    //8、调用 getFunStep
    //int getFunStep( T_ptclFunDsc* funDsc_p ,u_int TID , u_int COT , u_char* PDUObj_p , u_char* ASDU_p , u_short ASDUlng)
    //    u_char* tmp_AUPD_p = ((APDU104*)apdu_p)->getAPDU();
    /*u_char* apdu_p1=createAPDU(&tmpApduDscr,1,1);
    u_char* tmp_AUPD_p1 = ((APDU104*)apdu_p1)->getAPDU();
    ((APDU104*)apdu_p)->bindToMem(tmp_AUPD_p1, 0);*/

    tmpFunDsc.act_prio_chnlID = PASSIVE_TYPE; //ACTIVE_TYPE PASSIVE_TYPE
    //    int vRetGetFuncStep = getFunStep(&tmpFunDsc,12,20,apdu_p,tmp_AUPD_p+6,apduLong - 1);

    //9、调用packAPDU
    realTimeValue tmp_rtmParam = {0};
    tmp_rtmParam.sendnum = 234;
    tmp_rtmParam.receivenum = 232;

    realTimeThreshold tmp_realTimeThreshold = {};
    //U帧调用
    //packAPDU((void*)(&tmp_rtmParam),apdu_p,0);
    //I帧调用
    //packAPDU(&tmp_rtmParam,&tmp_realTimeThreshold,apdu_p,apduLong - 6);

    //10、调用setTransCtrlParam
    realTimeValue tmp_rtmParam1 = {0};
    timerType     tmp_timerType = {0};
    //    setTransCtrlParam(0,&tmp_rtmParam1,(u_char*)&tmp_timerType);


    //11、调用getChnlStatus
    realTimeThreshold tmp_cfgRealTimeParam = {0};
    tmp_cfgRealTimeParam.K = 1;
    tmp_cfgRealTimeParam.W = 2;
    //    u_char vUChar = getChnlStatus(&tmp_rtmParam1,&tmp_cfgRealTimeParam);

    //12、调用chkAPDUData
    //revBuf tmp_APDU_Buf = {0};
    u_char tmp_uChar = 0;
    //tmp_APDU_Buf.APDU_Buf.APDU_Data_p = ((APDU104*)apdu_p)->getAPDU();
    tmp_APDU_Buf.APDU_Buf.APDU_Lng    = apduLong;
    //    char vChar = chkAPDUData(&tmp_APDU_Buf,&tmp_rtmParam1,&tmp_cfgRealTimeParam,&tmp_uChar);



    //13、调用CheckSendState
    //int   CheckSendState(short role, void *rtmParam, u_char *data, u_short length)
    tmp_rtmParam1.stage = MASTER_UNCONFIRMED_STOPPED;
    //u_char* tmpAPDU_p = ((APDU104*)apdu_p)->getAPDU();
    //int vRetChSta = CheckSendState(MASTER_STATION,&tmp_rtmParam1,tmpAPDU_p,apduLong);


    //14、调用UpdateSendState
    //int UpdateSendState(short role, void *rtmParam, void *cfgParam ,u_char* tmrCtrlCode_p, u_char *data, u_short length)
    /*int vRetUpSta = UpdateSendState(MASTER_STATION,&tmp_rtmParam1,
    &tmp_cfgRealTimeParam,(u_char*)&tmp_timerType,tmpAPDU_p,apduLong);*/
#endif//END FUSK_TEST


    return 0;
}