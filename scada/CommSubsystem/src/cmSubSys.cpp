//�ֳ������ۺ�ͨ��ϵͳ--ͨ����ϵͳ����ͷ�ļ�
//
//��λ���׷����Ϸ��ܹ���(�Ϻ�)���޹�˾
//��д�ˣ��Խ��
//��дʱ�䣺2016��12��
//

#include "cmSubSys.h"



#define MC_DEF_Q_NUM    10      // Ĭ�϶�������Ҫ���ڵ���Զ��վ���� ��MC_DEF_STN_NUM��
#define MC_DEF_QU_NUM   20
#define MC_STEP_MAX_NUM 32      // �����лỰһ�����֡��
#define MC_DEF_SSN_TM_OUT   30*10  // �Ự��ʱʱ�䣬���û��߳�ʹ�ã���λ�루s����Ҫ��ʱ��Ҫ�㹻�󣬲��ܹ���֤
#define MC_MAX_PNT_NUM      520     // �Ựһ����֧�ֵ�������



/************************���ļ��о�̬����������*********************************/
static void ssnTmOutDftHdl ( void* tmoutInfo_p );
static void minTimerDftHdl ( void* tmoutInfo_p );



/*************************ȫ�ֱ���������***************************************/

//�Զ����˫�����
crtGrpQPar ssnQPar = { 1, MC_DEF_Q_NUM, sizeof ( qSsnReqUint ) /*50*/, MC_DEF_QU_NUM }; //ltG, q-num=10,q-unit lng = 50, q-unit num = 10
crtGrpQPar ssnRdyQPar = { 2, 0, 0, 0 };//lgG, q-num=0,q-unit lng = 0, q-unit num = 0
C_DuGrp_BCPQ ssnDuQ ( ssnQPar, ssnRdyQPar ); //du-queue. fstGrpQ-rdy; scdGrpQ-ssn

crtGrpQPar ASDUQPar = { 1, MC_DEF_Q_NUM, sizeof ( qASDUInfoRcvUnit ), MC_DEF_QU_NUM }; //ltG, q-num=10,q-unit lng = 50, q-unit num = 10
crtGrpQPar transCtlQPar = { 2, MC_DEF_Q_NUM, sizeof ( qTransCtrlInfoUnit ), MC_DEF_QU_NUM };/*transCtrlF*//*transCtrlFrmDsc*/ //lgG, q-num=0,q-unit lng = 0, q-unit num = 0
C_DuGrp_BCPQ ASDUDuQ ( ASDUQPar, transCtlQPar ); //du-queue.scdGrpQ-ASDU; fstGrpQ-transCtl

crtGrpQPar ASDUInfoQPar = { 1, MC_DEF_Q_NUM, sizeof ( qASDUInfoRcvUnit ), MC_DEF_QU_NUM }; //ltG, q-num=1,q-unit lng = 50, q-unit num = 10
C_DuGrp_BCPQ ASDUInfoQ ( ASDUInfoQPar ); //du-queue.fstGrpQ-ASDU_Info

u_char* g_chnlQCnxt_p = NULL; // ����ͨ��������


//�Ự���
C_SsnObj g_ssnDsc_1;             //a needed to config ssn obj.
C_SsnMgr ssnMgr;                  // ssn manager
C_SsnCtnrObj ssnCtnr;            // �Ự����
T_snnCfg* g_snnCfg_p     = NULL; // �Ự����
T_snnPara* g_snnPara_p   = NULL; // �Ự����
ONE_STEP* g_oneStepRul_p = NULL; // ������

//Զ��վ
C_StnObj stnObj; // Զ��վ����
int g_stnNum        = MC_DEF_STN_NUM; // Զ��վ�����������ᶯ̬�仯
u_long* g_stnAddr_p = NULL;          // Զ��վ��ַ���飬���ڼ�������У��
u_short* g_stnPort_p   = NULL;          // Զ��վ�˿����飬���ڼ�������У��
//ͨ��
C_ChnlMgr chnlMgr; // chnl manager


//��־����
//nsCPLog::CDailyLog g_TryLog;

//socket
EVENT** g_evtArr_pp            = NULL; // �¼���ά���飬����������ղ�
sock_descriptor** g_sockArr_pp = NULL; // �����ά���飬����������ղ�
nsCPSocket::CTcpServer g_locServer;    // ���ڳ�ʼ��g_evtArr_pp
nsCPSocket::CTcpClient g_locClient;
int g_locPort = 0;


C_TmObj tmFclty; // ��ʱ������
//extern C_TmObj tmFclty;


//��ʱ
//FUN_RL* g_vRl_p;
FunTypeOne ssnTmOutFun_p = NULL; // �Ự��ʱ�ص�����
FunTypeOne minTimerFun_p = minTimerDftHdl;  //1�������ݷ��ͻص�����
int ssnFnshNfyID         = 0;
int t_Tid                = 0;


//�߳�
nsCPThread::CThread* thdObj_p = NULL; // �̹߳������
int text_Tid                  = 0;    //���Զ��û��߳�ʱ��¼�̺߳�
char g_cUsrCnt                = 2;    // �û��̼߳�����
T_TID g_errTID                = VALIDTTID;
T_TID g_appTID                = VALIDTTID;
T_TID g_putStorTID            = VALIDTTID;
T_TID g_outStorTID            = VALIDTTID;


//������
nsCPLock::CLock g_ssnLock; // �Ự�������ڻỰ��������


//���ù���
/***************************************************
*����ʱ����ͨ�����޸ĵĵط�
*TsysUsr�߳�
*getDataID����
*
***************************************************/
//ModbusChnlAddrSign *modbusChnlAddrSign_p;
BufData* bufData_p = NULL;
cfgManage* cfgMng = NULL;//new cfgManage ( MCT_SSN_NUM );
chnlDataBuf recvDataBuf[MC_DEF_STN_NUM];//���ַͨ�ö���洢(���ջ�����)
chnlDataBuf sendDataBuf[MC_DEF_STN_NUM];//���ַͨ�ö���洢(���ͻ�����)
countManage calc;
C_unitInfData unitInfData ;


//vbus
IvBus* g_vbus_p = NULL; 


void sysInit() //ģ��������ϵͳ
{
    //int bufNum = 300; //���ø���
    //��ʼ��������
    cfgMng = new cfgManage ( MCT_SSN_NUM );

    if ( cfgMng == NULL )
    {
        exit ( -1 );
    }

    chnlObjCfg* chnfcfg =  cfgMng->getchnlObjCfg();
    int iRet = 0;

    iRet =  unitInfData.initM ( MC_DEF_STN_NUM, cfgMng->getChnUnit() );
    if ( iRet != 0 )
    {
        cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
            "%s|%s|%d: unitInfData.initM return error, iRet = %d\r\n",
            __FILE__, __func__, __LINE__, iRet );
        exit ( -1 );
    }

#ifdef RUN_VBUS_FLAG  //�Ƿ���Ҫvbus
    //vbus ��ʼ��
    g_vbus_p = IvBus::Instance(VBUS_SCADA_APPID, VBUS_SCADA_HOSTID);
    if( g_vbus_p == NULL )
    {
        cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
            "%s|%s|%d: IvBus::Instance return NULL\r\n",
            __FILE__, __func__, __LINE__ );
        exit ( -1 );
    }
#endif

    memset ( recvDataBuf, 0, sizeof ( recvDataBuf ) );
    memset ( sendDataBuf, 0, sizeof ( sendDataBuf ) );

    for ( int chnlN = 0; chnlN < MC_DEF_STN_NUM; chnlN++ )
    {
        recvDataBuf[chnlN].addrIdx_p = new addrRange[chnfcfg[chnlN].cfgLen]();
        recvDataBuf[chnlN].arLen     = chnfcfg[chnlN].cfgLen;
        recvDataBuf[chnlN].buf_pp    = new T_snnUsrData*[chnfcfg[chnlN].cfgLen]();

        sendDataBuf[chnlN].addrIdx_p = new addrRange[chnfcfg[chnlN].cfgLen]();
        sendDataBuf[chnlN].arLen     = chnfcfg[chnlN].cfgLen;
        sendDataBuf[chnlN].buf_pp    = new T_snnUsrData*[chnfcfg[chnlN].cfgLen]();

        memcpy ( recvDataBuf[chnlN].addrIdx_p, chnfcfg[chnlN].cfgAddr_p, ( sizeof ( addrRange ) * chnfcfg[chnlN].cfgLen ) );
        memcpy ( sendDataBuf[chnlN].addrIdx_p, chnfcfg[chnlN].cfgAddr_p, ( sizeof ( addrRange ) * chnfcfg[chnlN].cfgLen ) );

        for ( int idxNum = 0; idxNum < chnfcfg[chnlN].cfgLen; idxNum++ )
        {
            int recvbuflng                                   = recvDataBuf[chnlN].addrIdx_p[idxNum].endAddr - recvDataBuf[chnlN].addrIdx_p[idxNum].begAddr + 1;
            recvDataBuf[chnlN].buf_pp[idxNum]                = new T_snnUsrData[recvbuflng]();

            for ( int i = 0; i < recvbuflng; i++ )
            {
                recvDataBuf[chnlN].buf_pp[idxNum][i].gData.tmStmp_p        = new u_char[chnfcfg[chnlN].pntTmStmpLng];
                recvDataBuf[chnlN].buf_pp[idxNum][i].fData.tmStmp_p        = new u_char[chnfcfg[chnlN].pntTmStmpLng];
            }

            int sendbuflng                                   = sendDataBuf[chnlN].addrIdx_p[idxNum].endAddr - sendDataBuf[chnlN].addrIdx_p[idxNum].begAddr + 1;
            sendDataBuf[chnlN].buf_pp[idxNum]                = new T_snnUsrData[sendbuflng]();
            u_char* sendTmStmpLng                            = new u_char[sendDataBuf[chnlN].tmStmpLng];
            sendDataBuf[chnlN].buf_pp[idxNum]->gData.tmStmp_p = ( void* ) sendTmStmpLng;

            for ( int i = 0; i < recvbuflng; i++ )
            {
                sendDataBuf[chnlN].buf_pp[idxNum][i].gData.tmStmp_p        = new u_char[chnfcfg[chnlN].pntTmStmpLng];
                sendDataBuf[chnlN].buf_pp[idxNum][i].fData.tmStmp_p        = new u_char[chnfcfg[chnlN].pntTmStmpLng];
            }
        }
    }

    u_char ucUsrNum = 1; // �û��߳���

    // վ���� ////////////////////////////////////////////////////

    //g_stnNum = MC_DEF_STN_NUM; // Զ��վ�����������ᶯ̬�仯
    bufData_p  = new BufData[g_stnNum];
    //modbusChnlAddrSign_p = new ModbusChnlAddrSign[g_stnNum]();
    int iLocPort = 2404; // ���ض˶˿ں�ֵ
    //#endif
    g_locPort = iLocPort;


    ssnSets tSsnSets[MC_DEF_STN_NUM][MCT_SSN_NUM * 2]; // Զ��վ֧�ֵĻỰ����һά��ʾԶ��վ�ţ���ά��ʾԶ��վ֧�ֵĻỰ����

    // ���������ĵ�ַ�ռ�
    g_chnlQCnxt_p = new u_char[g_stnNum];

    if ( g_chnlQCnxt_p == NULL )
    {
        return;
    }

    memset ( g_chnlQCnxt_p, 0, sizeof ( u_char ) * g_stnNum );

    //////////////////////////////////////////////////////
    // ȫ�ֵ�Զ��վ��ַ��Ϣ���¼������
    g_stnAddr_p = new u_long[g_stnNum];
    g_stnPort_p = new u_short [g_stnNum];
    memset ( g_stnAddr_p, 0, sizeof ( u_long ) * g_stnNum );
    memset ( g_stnPort_p, 0, sizeof ( u_short ) * g_stnNum );
    for ( int i = 0; i < g_stnNum ; i++ )
    {
        g_stnAddr_p[i] = ntohl ( inet_addr ( ( cfgMng->getchnlObjCfg() [i] ).szStnIp ) );
    }

    //
    u_short num = g_stnNum / GROP_CHNL_SIZE;
    u_short modNum = g_stnNum % GROP_CHNL_SIZE;

    if ( modNum != 0 )
    {
        num += 1;
    }

    g_evtArr_pp = new EVENT*[num]();
    g_sockArr_pp = new sock_descriptor*[num]();

    for ( int i = 0; i < num; i++ )
    {
        g_evtArr_pp[i] = new EVENT[GROP_CHNL_SIZE]();
        g_sockArr_pp[i] = new sock_descriptor[GROP_CHNL_SIZE]();
    }

    //char m_ip[16] = "192.168.0.87";
    // ���ط����������Ϊ�˳�ʼ��ȫ�ֵ��¼������ֵ

    iRet = g_locServer.Startup ( iLocPort, g_evtArr_pp[0], g_sockArr_pp[0], g_stnPort_p, g_stnAddr_p, & ( g_stnNum ) );

    if ( iRet != 0 )
    {
        cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "SYS_INIT:: local's server startup is failed, port = %d, err=%d \r\n",
                                 iLocPort, iRet );
        exit (-1);
    }

    cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                       "SYS_INIT:: local's server startup, port = %d , iRet = %d\r\n", 
                       iLocPort, iRet );

    // ��ʼ���¼������ֵ

    for ( int in = 0; in < num * g_stnNum; in++ )
    {
        sock_descriptor sockConn = g_locClient.ConnServer ( "127.0.0.1", iLocPort, "127.0.0.1", in + 20000, 0 );

        if ( sockConn == sock_invalid )
        {
            in--;
        }
    }

    nsCPPub::SleepSecond ( 5 );
    // �Ự ��ӵ���������
    //apply for ssnItem to ssnMgr
    int iSucc = 0;
    ssnTmOutFun_p = ssnTmOutDftHdl;
    int tSsnID, tChnlID;
    C_SsnObj* tSsnDsc_p = NULL;
    HMODULE fHdl = NULL;

    for ( int i = 0; i < MCT_SSN_NUM; i++ )
    {
        snnAppCfg ptclSnnCfg = ( cfgMng->getAppSnnCfg() ) [i];
        tSsnID = ssnMgr.aplSsnItem ( ptclSnnCfg, fHdl ); // ���Ự��ӵ��Ự�����У�����ֵΪ��Ӧ�ã��Ự��

        if ( tSsnID < 0 )
        {
            continue;
        }

        tSsnDsc_p = &g_ssnDsc_1;
        ssnMgr.regSsnObj ( tSsnID, tSsnDsc_p ); //register ssnObj into ssnTbl
        iSucc++;
        int idx = i * 2;

        // ͨ����֧�ֵ���Ӧ�Ự����
        for ( int iss = 0; iss < g_stnNum; iss++ )
        {
            chnlObjCfg chnlCfg = ( cfgMng->getchnlObjCfg() ) [iss];
            // Զ��վ֧�ֵĻỰ��
            tSsnSets[iss][idx].actFlag   = chnlCfg.role == MC_CTL_STN ? MC_ACT_FLAG : MC_PSV_FLAG;
            tSsnSets[iss][idx].setNum    = 0;
            tSsnSets[iss][idx].ssnNo     = i;
            tSsnSets[iss][idx].stepDsc_p = NULL;

            tSsnSets[iss][idx + 1].actFlag   = chnlCfg.role == MC_CTL_STN ? MC_PSV_FLAG : MC_ACT_FLAG;
            tSsnSets[iss][idx + 1].setNum    = 0;
            tSsnSets[iss][idx + 1].ssnNo     = i;
            tSsnSets[iss][idx + 1].stepDsc_p = NULL;
        }

    }

    //printf("SYS_INIT:: ssn's num = %d \r\n", iSucc);

    // ͨ����س�ʼ��
    C_ChnlObj* tChnlObj_p;
    void* sckHandle_p = NULL;

    for ( int i = 0; i < g_stnNum; i++ )
    {
        // ͨ��
        // chnl-0
        //apply for chnlItem to chnlMg
        //ȡ������Ϣ
        chnlObjCfg chnlCfg = ( cfgMng->getchnlObjCfg() ) [i];
        // ���벢����ͨ��������(chnlTblItem*)(chnlTblHd_p+i)��������i
        tChnlID = chnlMgr.aplChnlItem(); 

        //��ʼ��ͨ������:ͨ��������
        u_char ucState = 0;
        // �ڶ�������Ϊapdu���ȣ�Ӧȡֵ�����ã���ע��������apdu����ʱ����һ��
        tChnlObj_p = new C_ChnlObj ( ucState, chnlCfg.bufLng, chnlCfg.ptclMdlNum ); 

        if ( tChnlObj_p == NULL )
        {
            return;
        }

        // ͨ��������Ϣ��ʼ�����綨ʱ��Դ��
        // ...
        stnOrVew tStnOverView;
        tStnOverView.role      = chnlCfg.stnRole;//0x1-ctl stn;0x2-ctled stn;0x3-cplx
        tStnOverView.stnAddr   = ntohl ( inet_addr ( chnlCfg.szStnIp ) ); //long net to host
        tStnOverView.localAddr = ntohl ( inet_addr ( chnlCfg.localszStnIp ) ); //long net to host
        tStnOverView.stnAppID  = chnlCfg.szStnPort; //
        tStnOverView.localPort = chnlCfg.localPort; //
        tStnOverView.stnID     = tChnlID;
        chnlMgr.regChnlObj ( tChnlID, tChnlObj_p );
        chnlMgr.regStnOrVew ( tChnlID, &tStnOverView );

        // Զ��վ��ַ���뵽ȫ��Զ��վ��ַ��Ӧλ���У�ͨ���ż��±�
        g_stnAddr_p[i] = tStnOverView.stnAddr;
        g_stnPort_p[i] = tStnOverView.stnAppID;
        tChnlObj_p->setChnlNo ( tChnlID );
        // ��ɫ��ʼ��
        tChnlObj_p->setRole ( chnlCfg.role );

        // ����֧�ֵĻỰ��
        tChnlObj_p->setSsnSets ( tSsnSets[tChnlID], iSucc * 2 );

        // �������֡�ڴ�ռ��ʼ��
        char* tranCtrlBuf_p = new char[MC_DEF_MAX_LEN];
        memset ( tranCtrlBuf_p, 0, MC_DEF_MAX_LEN );
        tChnlObj_p->setTranCtrl ( tranCtrlBuf_p, MC_DEF_MAX_LEN );

        // ʵʱ��������
        realTimeThreshold* tranCtrl_p = new realTimeThreshold;
        tranCtrl_p->Mode              = SEND_NO_CONTROL; // Ŀǰ104 ��Ĭ��ֵ����������
        tranCtrl_p->K                 = 12; // Ŀǰ104�� Ĭ��ֵ������Э�����
        tranCtrl_p->W                 = 8;//1;//
        tChnlObj_p->setTranCtrlCfg ( ( void* ) tranCtrl_p );

        // ʵʱ����ֵ
        char* realTranCtrl_p = ( char* ) new realTimeValue();
        memset ( realTranCtrl_p, 0, sizeof ( realTimeValue ) );
        ( ( realTimeValue* ) realTranCtrl_p )->stage = chnlCfg.stage;
        tChnlObj_p->setRealParam ( realTranCtrl_p );

        // ��Լ����
        HMODULE* tPtclMdl_p = tChnlObj_p->getPtclMdl();
        T_getPtclID tGetPtclID = ( T_getPtclID ) GetProcAddress ( *tPtclMdl_p, "getPtclID" );

        int iPtclID = tGetPtclID();

        /**/
        // ��ʱ�� ע��ͨ�������ṹ����Ӧ��ʱ������Ҫ��ֵ�������涨ʱ��Ҫ������
        u_short tmChnlID = tChnlID;
        int iMax = iPtclID == 104 ? MC_104_CHNL_TIME_MAX : 1; // 104 Ŀǰʱ����Ϊ 4 ��a0,a1,a2,a3��
        iRet = tmFclty.addTmChnl ( iMax, &tmChnlID ); //this tm chnl only include 1 tmr.

        for ( int itm = 0; itm < iMax; itm++ )
        {
            tmCfg tTmrCfg;
            char tTmrNm[20];
            sprintf ( tTmrNm, "chnl time:%d\n", itm );
            tTmrCfg.ID             = itm; // ʱ��ID ҪΨһ��һ��ͨ���ڣ� ��������ע�ⲻҪ��Ӧ�ü����Ự��ʱ���ظ�
            tTmrCfg.name_p         = tTmrNm; // ��ʱ������
            tTmrCfg.tmFunHdl       = TimerService;//ssnTmOutFun_p;    // �ص�������ַ
            PDEVICES dev           = new DEVICES; // Ŀǰ104����ʱ����0-3���ڳ��������м䲻��ɾ�������Դ��ڴ������ͷţ�ע��
            dev->chnlID            = tChnlID;
            dev->hIntance          = NULL;
            dev->type              = itm;
            tTmrCfg.tmFunHdlPara_p = ( void* ) dev; // �ص��������

            tTmrCfg.type = itm == 2 ? TM_PERIOD : TM_ONCE; // once
            tTmrCfg.fstXpirTm = itm == 0 ? 30 : ( itm == 1 ? 15 : ( itm == 2 ? 10 : ( itm == 3 ? 20 : 10 ) ) ); // ��ʱʱ�䣬ĿǰĬ��10s����������

            if ( !tmFclty.addTmr ( tmChnlID, &tTmrCfg ) )
            {
                //  tmChnl *TmHead_p = tmFclty.getTmChnlPtrByID(tmChnlID);
                //  tmFclty.ctlTmr(TmHead_p, tTmrCfg.ID, 2);//start app tmr!
                //
                tChnlObj_p->setOutTime ( tTmrCfg.ID, tTmrCfg.fstXpirTm );
                tChnlObj_p->setCtrlCode ( tTmrCfg.ID, TM_STOP );
            }
            else
            {
                // error
            }

        }

        //
        tmChnl* TmHead_p = tmFclty.getTmChnlPtrByID ( tmChnlID );
        tChnlObj_p->setTimer ( TmHead_p );
        // outtime and  ctrlcode ...


        // ͨ�����������ʼ��...
        //ctl stn--client;ctled stn--ser;cplx--client/ser
        if ( sckHandle_p == NULL )
        {
            // һ��ϵͳһ��������ϵͳ��������ʱ���Բ���net���ã�����Ҫʹ��
            if ( tStnOverView.role == MC_CTL_STN )
            {
                // client
                nsCPSocket::CTcpClient* sckClient_p = new nsCPSocket::CTcpClient;

                sckHandle_p = ( void* ) sckClient_p;

            }
            else if ( tStnOverView.role == MC_CTLED_STN )
            {
                //server
                nsCPSocket::CTcpServer* sckServer_p = new nsCPSocket::CTcpServer;
                //             iRet = sckServer_p->Startup(tStnOverView.stnAppID-1, &g_evtArr_pp[0][0], &g_sockArr_pp[0][0], g_stnAddr_p, &g_stnNum);
                //            if ( iRet != 0 )
                //            {
                //               return ;
                //            }

                sckHandle_p = ( void* ) sckServer_p;
            }
            else if ( tStnOverView.role == MC_CPLX_STN )
            {
                //client and server

                nsCPSocket::CTcpServer* sckServer_p = new nsCPSocket::CTcpServer;
                // iRet = sckServer_p->Startup(tStnOverView.stnAppID, &g_evtArr_pp[0][0], &g_sockArr_pp[0][0], g_stnAddr_p, &g_stnNum);
                // if ( iRet != 0 )
                // {
                //     return ;
                // }
                sckHandle_p = ( void* ) sckServer_p;
            }
        }

        sock_descriptor* sock_p = &g_sockArr_pp[0][0];

        tChnlObj_p->setSockHdl ( & ( sock_p[tChnlID] ) );

        // ��ʼ��Զ��վ
        stnDsc stn;
        memset ( &stn, 0, sizeof ( stnDsc ) );
        stn.chnlID  = tChnlID;
        stn.stnID   = tChnlID;
        stn.stnRole = chnlCfg.stnRole; //0x1-ctl stn;0x2-ctled stn;0x3-cplx

        // ��Զ��վ���뵽�����У��൱����������
        stnObj.addPscStn ( &stn );

        stnDsc* tStnDsc = stnObj.getStnDsc ( stn.stnID );

        // ���״洢��Ԫ�����ͣ���ʼ������Զ��վ����Զ��վ������ʱ��ʼ�����״洢����
        // ע����Ӧ��ֵΪ��ʼ����
        stnObj.initDStorage ( tStnDsc, chnlCfg.cfgAddr_p, chnlCfg.cfgLen, chnlCfg.usrType );

        // ͨ����������Զ��վ����ָ��
        tChnlObj_p->setStnObj ( &stnObj );

        //////////////////////////////////////////////////////////////////////////
        // ���ɱ����Ự������
        if ( chnlCfg.role == MC_CTL_STN && iPtclID == 104 )
        {
            //
            for ( int i = 0; i <= MCT_SSN_NUM; i++ )
            {
                u_short ssnNo = ssnMgr.getSsnDscNo ( i );

                if (  ssnNo != ACQ_EVT && ssnNo != U_TESTFR && ssnNo != U_STARTDT && ssnNo != U_STOPDT )
                {
                    continue;
                }

                T_snnPara* snnPara_p = NULL;
                getSnnParaMem ( snnPara_p );

                snnPara_p->COT = 0;
                snnPara_p->actFlag = MC_PSV_FLAG;
                snnPara_p->cmnAddr = 0;

                // every chnl's every
                qSsnReqUint tSsnReqBlk;
                tSsnReqBlk.chnlID = tChnlID;
                tSsnReqBlk.role = chnlCfg.role;
                tSsnReqBlk.snnPara_p = snnPara_p;

                ptclSnnKey ptclKey;
                ptclKey.ptclNo         = 104;
                ptclKey.snnDscNo       = ssnNo;
                tSsnReqBlk.ssnID      = /*i;//*/ssnMgr.getSsnID ( ptclKey ); // �Ự���
                tSsnReqBlk.timeout    = 0;
                tSsnReqBlk.tID        = 0;
                //
                ssnCtlBlk* tSsnCtlBlk_p = NULL;
                u_int tErrFlag = 0;
                iRet = crtSsn ( tSsnDsc_p, tSsnReqBlk, tChnlObj_p, tPtclMdl_p, tSsnCtlBlk_p, tErrFlag, MC_PSV_FLAG );

                if ( iRet != 0 || tErrFlag != 0 )
                {
                    exit ( -3 );
                }
            }
        }
        else if ( chnlCfg.role == MC_CTLED_STN )
        {
            // ctrled stn
            for ( int i = 0; i < MCT_SSN_NUM; i++ )
            {
                if ( iPtclID != ssnMgr.getSsnPtclNo ( i ) )
                {
                    continue;;
                }

                T_snnPara* snnPara_p = NULL;
                getSnnParaMem ( snnPara_p );

                snnPara_p->COT     = 0;
                snnPara_p->actFlag = MC_PSV_FLAG;
                snnPara_p->cmnAddr = 0;
                // every chnl's every
                qSsnReqUint tSsnReqBlk;
                tSsnReqBlk.chnlID    = tChnlID;
                tSsnReqBlk.role      = chnlCfg.role;
                tSsnReqBlk.snnPara_p = snnPara_p;
                tSsnReqBlk.ssnID     = i; // �Ự���
                tSsnReqBlk.timeout   = 0;
                tSsnReqBlk.tID       = 0;
                //
                ssnCtlBlk* tSsnCtlBlk_p = NULL;
                u_int tErrFlag = 0;
                iRet = crtSsn ( tSsnDsc_p, tSsnReqBlk, tChnlObj_p, tPtclMdl_p, tSsnCtlBlk_p, tErrFlag, MC_PSV_FLAG );

                if ( iRet != 0 || tErrFlag != 0 )
                {
                    exit ( -3 );
                }

            }
        }

        HMODULE* ptclMdl_p = tChnlObj_p->getPtclMdl();
        DLL_getPtclID getPtclID;
        getPtclID = ( DLL_getPtclID ) GetProcAddress ( *ptclMdl_p, "getPtclID" );

        if ( !getPtclID )
        {
            cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "Error: Can't not load function getPtclID()!\n" );
            return;
        }
    }

    // start all thread...

    thdObj_p = new nsCPThread::CThread[1];// �̸߳���

    if ( thdObj_p == NULL )
    {
        return;
    }

    int idx = 0;

#if 1
    //1. net
    ThreadInit ( num, 1, 1 );

    //2. snd
#ifdef WINDOWS_PLATFORM
    iRet = thdObj_p[0].StartThread ( ( FunTypeOne ) Et_sys_AppSnd );
#else
    iRet = thdObj_p[0].StartThread ( ( FunTypeOne ) Et_sys_AppSnd, NULL, 1, 81, SCHED_RR );
#endif

    if ( iRet != 0 )
    {
        delete[] thdObj_p;
        return;
    }

    idx = 1;


    //3. rec
#ifdef WINDOWS_PLATFORM
    iRet = thdObj_p[0].StartThread ( ( FunTypeOne ) Et_sys_AppRec, NULL, 1, THREAD_PRIORITY_ABOVE_NORMAL );
#else
    iRet = thdObj_p[0].StartThread ( ( FunTypeOne ) Et_sys_AppRec, NULL, 1, 80, SCHED_RR );
#endif

    if ( iRet != 0 )
    {
        delete[] thdObj_p;
        return;
    }


    //4. usr  MC_DEF_STN_NUM
    for ( int i = 0; i < MC_DEF_STN_NUM; i++ )
    {
        chnlObjCfg chnlCfg = ( cfgMng->getchnlObjCfg() ) [i];

        //if ( chnlCfg.role == MC_CTL_STN )
        //{
#ifdef WINDOWS_PLATFORM
        iRet = thdObj_p[0].StartThread ( ( FunTypeOne ) Tsys_Usr, & ( cfgMng->getchnlObjCfg() [i] ), ucUsrNum );
#else
        iRet = thdObj_p[0].StartThread ( ( FunTypeOne ) Tsys_Usr, & ( cfgMng->getchnlObjCfg() [i] ), ucUsrNum, 77, SCHED_RR );
#endif

        if ( iRet != 0 )
        {
            delete[] thdObj_p;
            cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "startthread failed !!\r\n" );
            return;
        }

        // }
    }

    
    #ifdef WINDOWS_PLATFORM
        iRet = thdObj_p[0].StartThread ( ( FunTypeOne ) Et_sys_DataPutStor );
    #else
        iRet = thdObj_p[0].StartThread ( ( FunTypeOne ) Et_sys_DataPutStor, NULL, 1,
                                         20, SCHED_OTHER );
    #endif

        if ( iRet != 0 )
        {
            delete[] thdObj_p;
            cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "startthread failed !!\r\n" );
            return;
        }
    

#ifdef WINDOWS_PLATFORM
    iRet = thdObj_p[0].StartThread ( ( FunTypeOne ) Et_sys_DataOutStor );
#else
    iRet = thdObj_p[0].StartThread ( ( FunTypeOne ) Et_sys_DataOutStor, NULL, 1,
                                     20, SCHED_OTHER );
#endif

    if ( iRet != 0 )
    {
        delete[] thdObj_p;
        cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "startthread failed !!\r\n" );
        return;
    }

    // take_err

#ifdef WINDOWS_PLATFORM
    iRet = thdObj_p[0].StartThread ( ( FunTypeOne ) Et_sys_TakeErr ); // ���ȼ��������Կ������
#else
    iRet = thdObj_p[0].StartThread ( ( FunTypeOne ) Et_sys_TakeErr, NULL, 1, 81, SCHED_OTHER ); // ���ȼ��������Կ������
#endif

    if ( iRet != 0 )
    {
        delete[] thdObj_p;
        cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "startthread failed !!\r\n" );
        return;
    }

#else // test
    // test to respand
    iRet = thdObj_p[0].StartThread ( ( FunTypeOne ) Test_NR );

    if ( iRet != 0 )
    {
        delete []thdObj_p;
        return;
    }

#endif

    cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "startthread success!!\r\n" );
    // ...

    //
    return;

}

int getSnnParaMem ( T_snnPara*& snnPara_p )
{
    if ( snnPara_p != NULL )
    {
        return 0;
    }

    snnPara_p = new T_snnPara();

    if ( snnPara_p == NULL )
    {
        return -1;
    }

    memset ( snnPara_p, 0, sizeof ( T_snnPara ) );

    snnPara_p->acsMthd = 0;//(1<<7);
    snnPara_p->ASDUID_p = new u_int[MC_STEP_MAX_NUM];
    memset ( snnPara_p->ASDUID_p, 0, sizeof ( u_int ) * MC_STEP_MAX_NUM );

    snnPara_p->usrData_p = new T_snnUsrData[MC_MAX_PNT_NUM];
    memset ( snnPara_p->usrData_p, 0, sizeof ( T_snnUsrData ) * MC_MAX_PNT_NUM );

    for ( int i = 0; i < MC_MAX_PNT_NUM; i++ )
    {
        snnPara_p->usrData_p[i].gData.tmStmp_p = new char[8]();
    }

    return 0;
}

/************************************************************/
/*                     �����Ự����API                    */
/************************************************************/
//
//
////////�����Ự����API
////
//���ܣ�����Ự��������Ϣ��ӣ����������¼�����ʱֵΪ0ʱ�������أ���������̵߳ȴ�
//      �Ự���ء��̹߳����ָ�ִ�е�ԭ�������1��Ӧ��֡�����̵߳õ��Ự������Ϣ��
//      �����ͻỰ����֪ͨ��2)��ʱʱ�䵽���ϵͳ���õĳ�ʱ��������ͻỰ��
//      ��֪ͨ������ʱֵΪ0ʱ����API��������Ự�������ϵͳ�߳̽��ոûỰ����֪ͨ��
//      Ȼ���������ϵͳ���ػỰִ��״̬��
//���룺
//      reqInfo_p�������Ự��������Ϣ
//      ssnKey���Ự�����������Ự����Э�飬�Ự�ڸ�Э���й̶�����ĻỰ��
//      snnStartPar_p����������ʱ����Ϊ�Ự��������
//      timeout���Ự�ĳ�ʱʱ�䣬=0ʱ��ʾ�������أ���0ʱ��ʾAPI�������̹߳�����ʱ��
//�����
//      snnStartPar_p���������ʱ�����ػỰ���һ����Ӧ������ֵ���м䲽���ص�Ӧ������
//                  Ӧͨ���洢��ϵͳ���ʻ��
//����ֵ��= 0������������Ự�ɹ���ɣ�<0������������Ựִ��ʧ��
//
//** ע������������ͬһ�Ự��ͬһͨ����ֻ������һ���û��߳� ��**
int reqSnn ( reqInfo* reqInfo_p, ssnReqCnt ssnKey, T_snnPara* snnStartPar_p, u_short timeout, PMSG& msg_p )
{
    if ( reqInfo_p == NULL || snnStartPar_p == NULL )
    {
        return MC_SNNST_ENQ_ERR;
    }

    qSsnReqUint tSsnReqBlk;
    tSsnReqBlk.chnlID     = reqInfo_p->chnlID;
    tSsnReqBlk.role       = reqInfo_p->role;
    tSsnReqBlk.ssnID      = ssnMgr.getSsnID ( ssnKey.ssn );
    tSsnReqBlk.addrStart  = ssnKey.addrStart;
    tSsnReqBlk.accumValue = ssnKey.accumValue;

    if ( 0xFFFF == tSsnReqBlk.ssnID )
    {
        return MC_SNNST_ENQ_ERR;
    }

    tSsnReqBlk.timeout = timeout;
    tSsnReqBlk.snnPara_p = snnStartPar_p;
#ifdef WINDOWS_PLATFORM
    tSsnReqBlk.tID = getThread();
#else
    //t_Tid = getThread();
    //tSsnReqBlk.tID = (T_TID*)&t_Tid;
    tSsnReqBlk.tID = getThread();
#endif

    u_int tSsnPrio = ssnMgr.getSsnPrio ( tSsnReqBlk.ssnID );
    C_SsnObj* tSsnObj_p = ssnMgr.getSsnObj ( tSsnReqBlk.ssnID );
    if ( tSsnObj_p == NULL )
    {
        return MC_SNNST_ENQ_ERR;
    }

    // create message queue!(PMSG)0xFFFF is used in linux
    if ( msg_p == NULL )
    {
        msg_p = new MSG();
#ifdef WINDOWS_PLATFORM
        PeekMessage ( msg_p, NULL, WM_USER, WM_USER, PM_NOREMOVE );
#else

        if ( ssnFnshNfyID == 0 )
        {
            int qid = initSsnAsynRsrc ( msg_p );

            if ( qid == -1 )
            {
                return qid;
            }
            else
            {
                ssnFnshNfyID = qid;
            }
        }

#endif
    }

#ifdef WINDOWS_PLATFORM
    int tid = GetThreadId ( tSsnReqBlk.tID );
#else
    //int tid = syscall(SYS_gettid);
    T_TID tid = getThread();
#endif

    //  printf("req tid=%d, ssnid=%d, start push into queue !\r\n", tid, tSsnReqBlk.ssnID);
    // �������,���ͻỰ�����¼�
    //set app timer when create ssn instance
	cfgMng->tryLog.LPrintf(DEBUG, DEBUG_LEVEL_1, "    reqSnn::ssnDuQ.pshQ\r\n");
    int iRet = ssnDuQ.pshQ ( FST_GROUP, tSsnPrio, ( char* ) &tSsnReqBlk, sizeof ( qSsnReqUint ) );
	cfgMng->tryLog.LPrintf(DEBUG, DEBUG_LEVEL_1, "    reqSnn::ssnDuQ.pshQ end\r\n");
    if ( iRet < 0 )
    {
        return MC_SNNST_ENQ_ERR;
    }

    //printf("req tid=%d, ssnid=%d, success push into queue !\r\n", tid, tSsnReqBlk.ssnID);

    if ( !timeout )
    {
        return 0;
    }

    cfgMng->tryLog.LPrintf(DEBUG, DEBUG_LEVEL_1, "req tid=%ld, ssnid=%d, ssnWaitFnsh !\r\n", tSsnReqBlk.tID,
             tSsnReqBlk.ssnID );
    u_int ssnRslt[2] = { 0 };
    ssnWaitFnsh ( tSsnReqBlk.tID, ( char* ) ssnRslt, sizeof ( u_int ) * 2, msg_p, ssnFnshNfyID ); // W:: linux param ...

    int debug = 0;
    if ( tSsnReqBlk.role == 9 )
    {
        debug = 0;
    }

	cfgMng->tryLog.LPrintf(DEBUG, DEBUG_LEVEL_1, "req chnl=%d , tid=%ld, ssnid=%d, role=%d, ssn is finish ! msg=%d, err=%d \r\n",tSsnReqBlk.chnlID,
             /*tid*/tSsnReqBlk.tID, tSsnReqBlk.ssnID, tSsnReqBlk.role, ssnRslt[0],
             ssnRslt[1] );
    return 0;

    //��ʱ����ʱ����ʱ������������ssnDsc_p->setActSsnRslt(ssnReq.chanlID)���ûỰ���
    //�������ʱ��Ӧ��֡�����̣߳������飩�е���ssnDsc_p->setActSsnRslt(ssnReq.chanlID)
    //���ûỰ���
    //�����������򶼻�����¼�֪ͨ���̣��Խ�����̵߳Ĺ���״̬
    // get chnl's ssnctlblk ptr


    //  C_ChnlObj *tChnlObj_p = chnlMgr.getChnlObj(tSsnReqBlk.chnlID);
    //  if ( tChnlObj_p == NULL )
    //  {
    //      return MC_SNNST_RES_TIMEOUT;
    //  }
    //  ssnCtlBlk *tSsnCtlBlk_p = tChnlObj_p->actSsnCtlBlk(tSsnPrio);
    //  if ( tSsnCtlBlk_p == NULL )
    //  {
    //      return MC_SNNST_RES_TIMEOUT;
    //  }
    //  return *(tSsnObj_p->getResultInfo(tSsnCtlBlk_p));


}

/************************************************************/
/*                      Ӧ��֡�����߳�                   */
/************************************************************/
//
//
//�Ự�����Ͳ����������Ĳ���˵��
//���룺
//      ssnNo���Ự��
//      usingMode���Ự������ģʽ
//      startStepStnD_p��������վ�Ự������վ���ݣ�ָ�루������ʵ�����ƿ��У���
//                       �ڲ��洢��־=0��ָʾ����Ҫִ�з�װ��ASDU���͵�����
//      nxtStnD_p����һ��վ�Ự������վ���ݣ�ָ�루������ʵ�����ƿ��У�,�ڲ���
//                 ����־=0��ָʾ����Ҫִ�б�����ѽ�����ASDU����
//      preStepRul_p��ǰһ�Ự������ָ�룬����APDU����ָ�루ÿ���Ự��ͨ����Ϣ���б����Լ�ָ�룩
//�������
//���أ�
//      ָʾ��֧��ת����
typedef int ( *T_ssnStartF ) ( u_char ssnNo, u_char usingMode, ONE_STEP* preStepRul_p,  T_snnPara* startStepStnD_p );
typedef int ( *T_ssnStepF ) ( FUN_RL* Fun_p, ONE_STEP* preStepRul_p, T_ptclFunDsc* funDsc_p, T_snnPara* nxtStnD_p, u_int* nxtStepNo_p );
//typedef int (*ssnStepF)(FUN_RL* Fun_p, u_char ssnNo, u_char usingMode, ONE_STEP* preStepRul_p, T_snnPara* nxtStnD_p);
//
//
////////�Ự��ɿ�
////����:
//      1. ֹͣ(��λ)Ӧ�ö�ʱ��
//      2. ɾ�������Ựʹ�õ���ʱ����Դ
//      3. ���ͻỰ���֪ͨ��֪ͨ�Ự������
//           �����߻Ự�����״̬(�ɹ����)
//           (linux������Ϣ,windowsֱ�ӻָ��Ự�����߳�)
//����:
//       ssnCtlBlk_p:�Ự���ƿ�ָ��
//            ssnNo:�Ự��(ȫ��)
//       chnlNo:ͨ����
//       errCd:�Ựʧ����:
/*      0x00 0000--�Ự�ɹ����
0x00 0001--���Ӵ�
0x00 0002--�޷����ݻỰ����õ��Ự����
0x00 0004--���������Ựʵ��ʧ�ܣ�
0x00 0008--�޷����ݻỰ����õ��Ựʵ��ʹ�õ�ͨ������
0x00 0010--Ϊ����ĻỰʵ�����Ӧ�ö�ʱ��ͨ��ʧ��
0x00 0020--���ȵ���ʵ���޻Ự����
0x00 0040--���ȵ��ĻỰʵ��û��asdu(apdu)����
0x00 0080--���ȵ��ĻỰ����û������ͨ����Ϣ��
0x00 0100--���ȵ��ĻỰʵ��û�з���ͨ������
0x00 0200--���ȵ��ĻỰʵ��û�жԶ˶�վ����
0x00 0400--���ȵ��ĻỰʵ��Ӧ�ü�����δͨ���������
0x00 0800--���ȵ��ĻỰʵ��Ӧ�ü����ݴ洢ʧ��
0x00 1000--���ȵ��ĻỰʵ��Ӧ�ü����ݼ���ʧ��
0x00 2000--���ȵ��ĻỰʵ��Ӧ�ü�����(��Ϣ����)�����޷�ʶ��
0x00 4000--���ȵ��ĻỰʵ���޷�ͨ���Ự�Ż�ù��ܺ�
0x00 8000--���ȵ��ĻỰʵ��������asdu���ʧ��
0x01 0000--���ȵ��ĻỰʵ���Ự���������޷�ʶ��ķ�֧��
0x02 0000--�����Ự��Ӧ��ʱ
*/
//���:��
//����ֵ:��ɿ�ִ��״̬��<0--ʧ�ܣ�>=0--�ɹ�
int finishBlock ( ssnCtlBlk* ssnCtlBlk_p, T_TID trdID, u_short ssnID, u_short chnlNo, u_int errCd )
{
    C_SsnObj* ssnObj_p;
    u_int msgBdy[2];

    // ��Ӹ���ssnID����ssnCtlBlk_p ΪU ֡(���ӡ��ر�)�������ϱ����ӿ�
    //
    if ( ssnCtlBlk_p == NULL && ( ssnID == IEC104_NEWHOLD_ACT ) )
    {
        //
        //ssnObj_p = ssnMgr.getSsnObj(ssnID);
        ssnCtlBlk_p = ssnCtnr.schdSsnObj();

        if ( ssnCtlBlk_p && ssnCtlBlk_p->ptclCtlCd != ssnID )
        {
            // error
            return -10;
        }

        // ok , find the ssn of ssnID, and finishBlk
        ssnID = ssnCtlBlk_p->ssnNo;
        chnlNo = ssnCtlBlk_p->chnlID;
        trdID = ssnCtlBlk_p->hostTID;
#ifndef WINDOWS_PLATFORM

#endif

        //
        if ( ! ( errCd & 0xFFFF0000 ) )
        {
            // sock
            int i = chnlNo / GROP_CHNL_SIZE;
            int j = chnlNo % GROP_CHNL_SIZE;

            sock_descriptor old  = g_sockArr_pp[i][j];

            sock_descriptor sNew = errCd;


#ifdef WINDOWS_PLATFORM
            WSAEventSelect ( sNew, g_evtArr_pp[i][j], FD_READ );
#else
            g_locClient.Close ( old );
            struct epoll_event evOld;
            memset ( &evOld, 0, sizeof ( epoll_event ) );
            evOld.data.fd = old;
            evOld.events  = EPOLLIN | EPOLLOUT;
            epoll_ctl ( g_evtArr_pp[0][i], EPOLL_CTL_DEL, old, &evOld );

            struct epoll_event ev;
            memset ( &ev, 0, sizeof ( epoll_event ) );
            ev.data.fd = sNew;
            ev.events  = EPOLLIN | EPOLLOUT;
            g_sockArr_pp[i][j] = sNew;
            epoll_ctl ( g_evtArr_pp[0][i], EPOLL_CTL_ADD, sNew, &ev );

#endif

        }
    }

    //

    if ( !ssnCtlBlk_p )
    {
        msgBdy[0] = chnlNo | ( ssnID << 16 );
        msgBdy[1] = errCd;
        ssnFnshNfy ( trdID, ( char* ) msgBdy, 2 * sizeof ( u_int ), ssnFnshNfyID );

        ssnDuQ.pshQ ( SCD_GROUP, 0, NULL, 0 ); //send ready inform

        return 0;
    }

    ssnObj_p = ssnMgr.getSsnObj ( ssnCtlBlk_p->ssnNo );

    if ( ssnObj_p == NULL && ssnCtlBlk_p->ssnNo == DEF_ERROR_SSN_NO )
    {
        // error blk

        //notify to inner-thread ( system thread)!
        //un-implement, temporarily

        //
        ssnCtnr.delSsnObj ( ssnCtlBlk_p->chnlID, ssnCtlBlk_p );
        ssnCtlBlk_p = NULL;

        ssnDuQ.pshQ ( SCD_GROUP, 0, NULL, 0 ); //send ready inform
        return 0;
    }

    /////////////////un-set ssn timer, or ssn timeout =0
    //active ssnInst or passive ssnInst.
    C_ChnlObj* tChnlObj_p = chnlMgr.getChnlObj ( ssnCtlBlk_p->chnlID );

    if ( ssnCtlBlk_p->ptclCtlCd != S_F && ssnCtlBlk_p->ptclCtlCd != I_F && tChnlObj_p != NULL )
    {
        realTimeValue* realTV_p = ( realTimeValue* ) tChnlObj_p->getRealParam();
        //  tChnlObj_p->setChnlState(realTV_p->stage);
        u_short usState = realTV_p->stage << 8; // ��Լ״̬λ
        usState |= realTV_p->stage == 1 ? MC_CHNL_BIT : 0; // ͨ��״̬λͬʱ��ֵ
        chnlMgr.setChnlState ( ssnCtlBlk_p->chnlID, usState );
    }

    //
    if ( !ssnCtlBlk_p->ssnTmHead_p )
    {
        if ( ssnCtlBlk_p->actFlg == MC_ACT_FLAG ) //act ssnInst, free resource..
        {
            T_freeRl_P tFreeRl_p = ( T_freeRl_P ) GetProcAddress ( * ( tChnlObj_p->getPtclMdl() ), "freeRl_P" );

            if ( tFreeRl_p )
            {
                T_ptclFunDsc tPtclDsc;
                ssnSets* tSsnSet_p = tChnlObj_p->getSsnSet ( ssnCtlBlk_p->ssnNo );

                tPtclDsc.remoteSet_p     = tSsnSet_p->stepDsc_p;
                tPtclDsc.setNum          = tSsnSet_p->setNum;
                tPtclDsc.act_prio_chnlID = ssnCtlBlk_p->actFlg;
                tPtclDsc.ssnDscNo        = ssnMgr.getSsnDscNo ( ssnCtlBlk_p->ssnNo );

                if ( ssnCtlBlk_p->myChnlInfoBlk_p != NULL )
                {
                    ( tFreeRl_p ) ( ssnCtlBlk_p->myChnlInfoBlk_p->funRl_p, ssnCtlBlk_p->myChnlInfoBlk_p->ssnStnCnfg_p, &tPtclDsc );
                    ssnCtlBlk_p->myChnlInfoBlk_p->funRl_p = NULL;
                }

                tSsnSet_p->stepDsc_p = NULL;
            }

            ssnObj_p->delActiveSsn ( ssnCtlBlk_p );
        }
        else
        {
            // psv's ssn, reset!!!

            if ( tChnlObj_p != NULL )
            {
                short ssnPrio = ssnMgr.getSsnPrio ( ssnCtlBlk_p->ssnNo );
                ssnCtnr.delSsnObj ( ssnPrio, ssnCtlBlk_p );
            }

            ssnCtlBlk_p->rtRunStOrRslt = MC_SNNST_OK;
            //ssnCtlBlk_p->ssnCtlLock.UnLock();
            //printf("finishblk::tSsnCtlBlk_p->ssnCtlLock.UnLock()\r\n");
			cfgMng->tryLog.LPrintf(DEBUG, DEBUG_LEVEL_1, "++++finishblk:: ssnCtlBlk_p->rtRunStOrRslt = %d\r\n", ssnCtlBlk_p->rtRunStOrRslt);
            cfgMng->tryLog.LPrintf (
            DEBUG, DEBUG_LEVEL_1,
            "++++finishblk:: chnl=%d\r\n",
            ssnCtlBlk_p->chnlID );
        }

        //notify to inner-thread ( system thread)!
        //un-implement, temporarily
        //..

        ssnDuQ.pshQ ( SCD_GROUP, 0, NULL, 0 ); //send ready inform
        return 0;
    }

    /////////////////timeout >0: act ssnInst or passive ssnInst
    // Ӧ�ü���ʱ������ɾ������Ϊ��û�зŵ�ͨ����ʱ�������У�����һ�´��벻��Ҫ��modify by zhaojj��20170602
    // 1. stop app tmr.
    u_short tTmChnlID = ssnCtlBlk_p->ssnTmHead_p->tmChnlID;
    u_char tTmrNum    = ssnCtlBlk_p->ssnTmHead_p->tmrNum;

    //  tTmChnlID = tmFclty.getTmChnlIDByPtr(ssnCtlBlk_p->ssnTmHead_p);
    //  tTmrNum = tmFclty.getTmChnlPara(tTmChnlID, 1);

    for ( u_char i = 0; i < tTmrNum; i++ )
    {
        tmFclty.ctlTmr ( ssnCtlBlk_p->ssnTmHead_p, i, TM_STOP );
        //      if(!ssnCtlBlk_p->actFlg)//act ssn Inst
        //          tmFclty.delTmr(tTmChnlID, i);
    }

    /////////////////timeout >0: act ssnInst
    if ( ssnCtlBlk_p->actFlg == MC_ACT_FLAG ) //act ssnInst
    {
        //  tmFclty.delTmChnlByID(tTmChnlID);
        tmFclty.delAppTmChnl ( ssnCtlBlk_p->chnlID, ssnCtlBlk_p->ssnTmHead_p );

        // 2. prepare usr info.
        msgBdy[0]     = ( ssnCtlBlk_p->chnlID ) | ( ssnCtlBlk_p->ssnNo << 16 ); //(u_short)chnlNo;
        msgBdy[1]     = errCd;
        T_TID hostTID = ssnCtlBlk_p->hostTID;

        // 3. clear ssnInst & other resource...
        // del ssnCtlBlk from chnl ... and other...

        if ( tChnlObj_p != NULL )
        {
            short ssnPrio = ssnMgr.getSsnPrio ( ssnCtlBlk_p->ssnNo );
            tChnlObj_p->delSsn ( ssnCtlBlk_p, ssnPrio );
            ssnCtnr.delSsnObj ( ssnPrio, ssnCtlBlk_p );
        }

        int role = ssnCtlBlk_p->role;

        if ( ssnCtlBlk_p->myChnlInfoBlk_p != NULL )
        {
            cfgMng->tryLog.LPrintf (
            DEBUG, DEBUG_LEVEL_1,
            "++++finishblk:: chnl=%d, ssn=%p, StnCfg=%p, Rl=%p, req=%p, resp=%p, cblk=%p, step=%p \r\n",
            ssnCtlBlk_p->chnlID, ssnCtlBlk_p,
            ssnCtlBlk_p->myChnlInfoBlk_p->ssnStnCnfg_p,
            ssnCtlBlk_p->myChnlInfoBlk_p->funRl_p,
            ssnCtlBlk_p->drvPrmtr_p->reqAPDU_Obj_p,
            ssnCtlBlk_p->drvPrmtr_p->respAPDU_Obj_p,
            ssnCtlBlk_p->myChnlInfoBlk_p, ssnCtlBlk_p->drvPrmtr_p );
        }

        //
        HMODULE* hMdl_p = tChnlObj_p->getPtclMdl();
        T_freeAPDU tFreeAPDU = ( T_freeAPDU ) GetProcAddress ( *hMdl_p, "freeAPDU" );
        T_freeRl_P tFreeRl_p = ( T_freeRl_P ) GetProcAddress ( *hMdl_p, "freeRl_P" );

        if ( tFreeRl_p )
        {
            T_ptclFunDsc tPtclDsc;
            ssnSets* tSsnSet_p = tChnlObj_p->getSsnSet ( ssnCtlBlk_p->ssnNo, ssnCtlBlk_p->actFlg );

            tPtclDsc.remoteSet_p     = tSsnSet_p->stepDsc_p;
            tPtclDsc.setNum          = tSsnSet_p->setNum;
            tPtclDsc.act_prio_chnlID = ssnCtlBlk_p->actFlg;
            tPtclDsc.ssnDscNo        = ssnMgr.getSsnDscNo ( ssnCtlBlk_p->ssnNo );

            if ( ssnCtlBlk_p->myChnlInfoBlk_p != NULL )
            {
                ( tFreeRl_p ) ( ssnCtlBlk_p->myChnlInfoBlk_p->funRl_p, ssnCtlBlk_p->myChnlInfoBlk_p->ssnStnCnfg_p, &tPtclDsc );
                ssnCtlBlk_p->myChnlInfoBlk_p->funRl_p = NULL;
                cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_1, " free ssnCtlBlk_p->myChnlInfoBlk_p\r\n" );
            }

            //ssnCtlBlk_p->myChnlInfoBlk_p->ssnStnCnfg_p = NULL;
            tSsnSet_p->stepDsc_p = NULL;

            // ...
            tSsnSet_p->setNum = 0;
        }

        if ( tFreeAPDU || ( ssnCtlBlk_p->myChnlInfoBlk_p != NULL ) )
        {
            ( tFreeAPDU ) ( ssnCtlBlk_p->myChnlInfoBlk_p->reqAPDU_Obj_p );
            ( tFreeAPDU ) ( ssnCtlBlk_p->myChnlInfoBlk_p->respAPDU_Obj_p );
            ssnCtlBlk_p->myChnlInfoBlk_p->reqAPDU_Obj_p  = NULL;
            ssnCtlBlk_p->myChnlInfoBlk_p->respAPDU_Obj_p = NULL;
            cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_1, " free ssnCtlBlk_p->myChnlInfoBlk_p->reqAPDU_Obj_p\r\n" );
        }

        // del ssnCtlBlk
        ssnObj_p->delActiveSsn ( ssnCtlBlk_p );

        // 4. notify to usr
        ssnFnshNfy ( hostTID, ( char* ) msgBdy, 2 * sizeof ( u_int ), ssnFnshNfyID );

        int debug = 0;
        if ( role == 9 )
        {
            debug = 0;
        }

        nsCPPub::SleepMilliSecond ( 1 );
#ifdef WINDOWS_PLATFORM
        int tid = GetThreadId ( hostTID );
#else
        //int tid = syscall(SYS_gettid);
        T_TID tid = getThread();
#endif
        //  printf("req tid=%d, ssnid=%d, chnl=%d, ssnFnshNfy !\r\n", tid,
        //  (short) (msgBdy[0] >> 16), (short) msgBdy[0]);

        ssnDuQ.pshQ ( SCD_GROUP, 0, NULL, 0 ); //send ready inform
        return 0;
    }
    else
    {
        // psv's ssn

        if ( tChnlObj_p != NULL )
        {
            short ssnPrio = ssnMgr.getSsnPrio ( ssnCtlBlk_p->ssnNo );
            ssnCtnr.delSsnObj ( ssnPrio, ssnCtlBlk_p );
        }
        ssnCtlBlk_p->rtRunStOrRslt = MC_SNNST_OK;
		cfgMng->tryLog.LPrintf(DEBUG, DEBUG_LEVEL_1, "    Et_sys_AppRec:: ssnCtlBlk_p->rtRunStOrRslt = %d\r\n", ssnCtlBlk_p->rtRunStOrRslt);
    }

    /////////////////timeout >0: passive ssnInst
    //notify to inner-thread ( system thread)!
    //un-implement, temporarily
    //..
    //printf("req no ssnFnshNfy !\r\n");

    ssnDuQ.pshQ ( SCD_GROUP, 0, NULL, 0 ); //send ready inform
    return 0;
}

static void ssnTmOutDftHdl ( void* tmoutInfo_p )
{
    ssnCtlBlk* tSsnCtlBlk_p;

    // lock
    //  nsCPLock::CAutolock tAutoLock(&g_ssnLock);

    tSsnCtlBlk_p = ( ssnCtlBlk* ) tmoutInfo_p;

    int iRet = 0;

    if ( tSsnCtlBlk_p )
    {
        cfgMng->tryLog.LPrintf(DEBUG, DEBUG_LEVEL_1, "ssnTmOutDftHdl:: tid=%lu, ssnid=%lu, chnl=%d, ssnFnshNfy !\r\n",
                 tSsnCtlBlk_p->hostTID, tSsnCtlBlk_p->ssnNo, tSsnCtlBlk_p->chnlID );
        iRet = finishBlock ( tSsnCtlBlk_p, tSsnCtlBlk_p->hostTID,
                             tSsnCtlBlk_p->ssnNo, tSsnCtlBlk_p->chnlID, 0x020000 );
    }
    else
    {
#ifndef WINDOWS_PLATFORM
        iRet = finishBlock ( tSsnCtlBlk_p, 0, 0, 0, 0x020000 );
#else
        iRet = finishBlock ( tSsnCtlBlk_p, g_errTID, 0, 0, 0x020000 );
#endif
    }

    return;
}

int resetPsvSsn ( ssnCtlBlk* ssnCtlBlk_p, u_short usSsnNo, u_short usChnlNo )
{

    //
    C_SsnObj* tCurSsnObj_p = ssnMgr.getSsnObj ( ssnCtlBlk_p->ssnNo );

    if ( !tCurSsnObj_p )
    {
        return -10;
    }

    //
    HMODULE* tModule_p;
    C_ChnlObj* tChnlObj_p = chnlMgr.getChnlObj ( ssnCtlBlk_p->chnlID );
    HMODULE* tPtclMdl_p = tChnlObj_p->getPtclMdl();
    T_getPtclID tGetPtclID =
    ( T_getPtclID ) GetProcAddress ( *tPtclMdl_p, "getPtclID" );
    int iPtclID = tGetPtclID();//��ȡЭ���

    if ( tChnlObj_p )
    {
        tModule_p = ( HMODULE* ) ( tChnlObj_p->getPtclMdl() );
    }

    if ( tModule_p == NULL )
    {
        return -20;
    }

    //
    //  T_createAPDU crtAPDU_f = (T_createAPDU)GetProcAddress(*tModule_p, "createAPDU");

    T_freeRl_P tFreeRl_p = ( T_freeRl_P ) GetProcAddress ( * ( tChnlObj_p->getPtclMdl() ), "freeRl_P" );
    T_createRl_P tCrt_FunRl_F = ( T_createRl_P ) GetProcAddress ( *tModule_p, "createRl_P" ); //���޸�Ϊwin,lnxͨ����ʽ
    T_setAPDU_DSCR tSetDscr_F = ( T_setAPDU_DSCR ) GetProcAddress ( *tModule_p, "setAPDU_DSCR" );

    if ( tFreeRl_p == NULL || tCrt_FunRl_F == NULL || tSetDscr_F == NULL )
    {
        return -10;
    }

    // reset
    // chnl info blk
    chnlInfoBlk* tChnlInfoBlk_p = ssnCtlBlk_p->myChnlInfoBlk_p;//tCurSsnObj_p->getChnlInfoBlk(ssnCtlBlk_p->chnlID);
    tChnlInfoBlk_p->chnlNo = usChnlNo;
    tChnlInfoBlk_p->ssnRlMode = 0;
    //��ȡ��Լ�⹦�ܺ�
    u_short ssnDscNo = ssnMgr.getSsnDscNo ( usSsnNo );

    // snnCfg
    //tChnlInfoBlk_p->ssnStnCnfg_p = new T_snnCfg();
    //memset(tChnlInfoBlk_p->ssnStnCnfg_p, 0, sizeof(T_snnCfg));
    tChnlInfoBlk_p->ssnStnCnfg_p->actFlag = ssnCtlBlk_p->actFlg;
    tChnlInfoBlk_p->ssnStnCnfg_p->infoType = 0;
    //tChnlInfoBlk_p->ssnStnCnfg_p->ssnID = usSsnNo;

    //
    T_snnPara* snnPara_p = ssnCtlBlk_p->ssnPara_p;
    snnPara_p->acsMthd = 0;//(1<<7);

    if ( snnPara_p->ASDUID_p == NULL )
    {
        snnPara_p->ASDUID_p = new u_int ( 0 );
    }

    memset ( snnPara_p->usrData_p, 0, sizeof ( T_snnUsrData ) );
    snnPara_p->COT     = 0;
    snnPara_p->actFlag = MC_PSV_FLAG;
    snnPara_p->cmnAddr = 0;

    // ��ȡ���ã���û�У�����ù�Լ�ⴴ����ʹ��Ĭ��ֵ��Ŀǰʹ��Ĭ������
    ssnSets* tSsnSet_p = tChnlObj_p->getSsnSet ( usSsnNo/*ssnCtlBlk_p->ssnNo*/, MC_PSV_FLAG );

    if ( ssnCtlBlk_p->ssnNo != usSsnNo || tSsnSet_p->stepDsc_p == NULL )
    {
        T_ptclFunDsc tPtclFun;

        if ( tSsnSet_p->stepDsc_p != NULL )
        {
            tPtclFun.remoteSet_p     = tSsnSet_p->stepDsc_p;
            tPtclFun.setNum          = tSsnSet_p->setNum;
            tPtclFun.act_prio_chnlID = ssnCtlBlk_p->actFlg;
            tPtclFun.ssnDscNo        = ssnMgr.getSsnDscNo ( ssnCtlBlk_p->ssnNo );

            ( tFreeRl_p ) ( ssnCtlBlk_p->myChnlInfoBlk_p->funRl_p, ssnCtlBlk_p->myChnlInfoBlk_p->ssnStnCnfg_p, &tPtclFun );
            ssnCtlBlk_p->myChnlInfoBlk_p->funRl_p = NULL;
            tSsnSet_p->stepDsc_p                  = NULL;
            tSsnSet_p->setNum                     = 0;

        }

        if ( tSsnSet_p->setNum == 0 || tSsnSet_p->stepDsc_p == NULL )
        {
            //tChnlInfoBlk_p->ssnRlMode = 0xFF;
            tChnlInfoBlk_p->ssnRlMode = tPtclFun.usingMode;
            tChnlInfoBlk_p->funRl_p = tCrt_FunRl_F ( ssnDscNo, tChnlInfoBlk_p->ssnRlMode, tChnlInfoBlk_p->ssnStnCnfg_p, &tPtclFun ); // 104 usingMode=0; ��ֵ�����ø���������һ���Ự��ֵ�̶�

            if ( tChnlInfoBlk_p->funRl_p == NULL )
            {
                ;
            }

            if ( tChnlInfoBlk_p->ssnRlMode == 0xFF )
            {
                tChnlInfoBlk_p->ssnRlMode = tPtclFun.usingMode;
            }

            tSsnSet_p->setNum = tPtclFun.setNum;
            tSsnSet_p->stepDsc_p = tPtclFun.remoteSet_p;
        }

        //
    }

    // asdu for chnl's info_blk Ĭ�Ϲ�����APDUû�в�ͬ��ʱ����Ҫ
    APDU_DSCR tmpApduDscr;
    memset ( &tmpApduDscr, 0, sizeof ( APDU_DSCR ) );
    tmpApduDscr.ASDU_TID = 0;//100 // Ĭ�Ϲ����ã�������ϵͳʱ Ҫע����ݻỰ�Ų�ͬ������

    // if ( iPtclID == cfgMng->getchnlObjCfg()[tChnlInfoBlk_p->chnlNo]->ptclMdlNum )
    //{
    tmpApduDscr = ( cfgMng->getchnlObjCfg() ) [tChnlInfoBlk_p->chnlNo].tmpApduDscr;
    // }
    // else
    //{
    //      cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,"tmpApduDscr, Configuration error " );
    //     exit ( 0 );
    // }

    /*
    // create apdu_obj
    // tChnlInfoBlk_p->reqAPDU_Obj_p = crtAPDU_f(&tmpApduDscr, 1, 0);
    // tChnlInfoBlk_p->respAPDU_Obj_p = crtAPDU_f(&tmpApduDscr, 1, 0);;
    */

    // init
    //
    tSetDscr_F ( tChnlInfoBlk_p->reqAPDU_Obj_p, tmpApduDscr );
    tSetDscr_F ( tChnlInfoBlk_p->respAPDU_Obj_p, tmpApduDscr );
    //
    ssnCtlBlk_p->ssnNo = usSsnNo;
    ssnCtlBlk_p->chnlID = usChnlNo;
    //  ssnCtlBlk_p->myChnlInfoBlk_p = tCurSsnObj_p->getChnlInfoBlk(tSsnReqBlk.chnlID);
    ssnCtlBlk_p->drvStepNo = 1; // ������ʼֵ�� 1������Ϊ��
    ssnCtlBlk_p->hostTID = 0;
    ssnCtlBlk_p->checkStd = 0;
    ssnCtlBlk_p->retryNum = 0; // ��ʱ����
    ssnCtlBlk_p->ssnTmout = 0;//300; // second

    ssnCtlBlk_p->drvPrmtr_p->reqPDU_Num  = 0;
    ssnCtlBlk_p->drvPrmtr_p->respPDU_Num = 0;
    ssnCtlBlk_p->drvPrmtr_p->stepNo      = ssnCtlBlk_p->drvStepNo;

    return 0;
}

// �����Ựʵ������ӵ����ȱ���
int crtSsn ( C_SsnObj*& tCurSsnObj_p, qSsnReqUint& tSsnReqBlk, C_ChnlObj*& tChnlObj_p,
             HMODULE*& tModule_p, ssnCtlBlk*& tSsnCtlBlk_p, u_int& tErrFlag, u_int actFlag/* = MC_ACT_FLAG*/ )
{
    //
    int tRtn = 0;
    u_int tPrio;
    u_short tmChnlID;
    tmCfg tTmrCfg;
    //create session
    tCurSsnObj_p = ssnMgr.getSsnObj ( tSsnReqBlk.ssnID );

    if ( !tCurSsnObj_p )
    {
        tErrFlag = tErrFlag | 0x000002;
    }

    tChnlObj_p = chnlMgr.getChnlObj ( tSsnReqBlk.chnlID );

    if ( tChnlObj_p )
    {
        tModule_p = ( HMODULE* ) ( tChnlObj_p->getPtclMdl() );
    }

    if ( tModule_p == NULL )
    {
        return -1;
    }

    //
    T_getPtclID tGetPtclID =
    ( T_getPtclID ) GetProcAddress ( *tModule_p, "getPtclID" );
    int iPtclID = tGetPtclID();//��ȡЭ���

    T_createRl_P tCrt_FunRl_F = ( T_createRl_P ) GetProcAddress ( *tModule_p, "createRl_P" ); //���޸�Ϊwin,lnxͨ����ʽ
    T_createAPDU crtAPDU_f = ( T_createAPDU ) GetProcAddress ( *tModule_p, "createAPDU" );

    if ( tCrt_FunRl_F == NULL || crtAPDU_f == NULL )
    {
        return -10;
    }

    //!�����Ựʵ��ʱ�����Ự�����������Ựʵ������
    //���лỰ�����ṩ��ȡ�Ự������������
    if ( actFlag == MC_ACT_FLAG )
    {
        tCurSsnObj_p->crtActiveSsn ( &tSsnCtlBlk_p, tSsnReqBlk.snnPara_p );
    }
    else
    {
        tCurSsnObj_p->crtPassiveSsn ( &tSsnCtlBlk_p, tSsnReqBlk.snnPara_p );
    }

    if ( !tSsnCtlBlk_p )
    {
        tErrFlag = tErrFlag | 0x000004;
        return tErrFlag;
    }

    //printf("----crtActiveSsn \r\n");

    // chnl info blk
    chnlInfoBlk tChnlInfoBlk;
    tChnlInfoBlk.chnlNo    = tSsnReqBlk.chnlID;
    tChnlInfoBlk.ssnRlMode = 0xFF; // Ĭ��ֵΪ 8λȫ1��ʹ��Ĭ�Ϲ��������ֵ
    u_short ssnDscNo       = ssnMgr.getSsnDscNo ( tSsnReqBlk.ssnID );
    // snnCfg
    //tChnlInfoBlk.ssnStnCnfg_p = new T_snnCfg();
    //memset(tChnlInfoBlk.ssnStnCnfg_p, 0, sizeof(T_snnCfg));
    //tChnlInfoBlk.ssnStnCnfg_p->actFlag = actFlag;
    //tChnlInfoBlk.ssnStnCnfg_p->infoType = 0;
    //tChnlInfoBlk.ssnStnCnfg_p->ssnID = tSsnReqBlk.ssnID;
    tChnlInfoBlk.ssnStnCnfg_p = cfgMng->getSnnCfg ( ssnDscNo, tSsnReqBlk.chnlID );
    setSsnCfg ( tChnlInfoBlk.ssnStnCnfg_p, tSsnReqBlk.addrStart, tSsnReqBlk.accumValue, iPtclID );
    // ��ȡ���ã���û�У�����ù�Լ�ⴴ����ʹ��Ĭ��ֵ��Ŀǰʹ��Ĭ������
    ssnSets* tSsnSets_p = tChnlObj_p->getSsnSet ( tSsnReqBlk.ssnID, actFlag ); // ����Ҫ���ͨ��֧�ֵĻỰ�����Ӧ

    if ( tSsnSets_p == NULL )
    {
        tErrFlag = tErrFlag | 0x000004;
        return tErrFlag;
    }

    if ( tSsnSets_p->setNum == 0 && tSsnSets_p->stepDsc_p == NULL )
    {
        T_ptclFunDsc tPtclFun  = * ( cfgMng->getFunDsc ( ssnDscNo, tSsnReqBlk.chnlID ) ); // ��ʱ���ø�ֵ��dll��ӿڻ��Ĭ��ֵ
        tChnlInfoBlk.ssnRlMode = tPtclFun.usingMode;
        tChnlInfoBlk.funRl_p = tCrt_FunRl_F ( ssnDscNo, tChnlInfoBlk.ssnRlMode, tChnlInfoBlk.ssnStnCnfg_p, &tPtclFun ); // 104 usingMode=0; ��ֵ�����ø���������һ���Ự��ֵ�̶�
        cfgMng->tryLog.LPrintf (
        DEBUG, DEBUG_LEVEL_1,
        "++++funRl_p:: ssnDscNo=%d, ssnRlMode=%d \r\n",
        ssnDscNo,
        tChnlInfoBlk.ssnRlMode );

        if ( tChnlInfoBlk.funRl_p == NULL )
        {
            tErrFlag = tErrFlag | 0x000004;
        }
        else
        {
            int chnl = tChnlObj_p->getChnlNo();

            //setSsnCfg(tChnlInfoBlk.ssnStnCnfg_p,chnl,iPtclID , bufData_p);
            if ( tChnlInfoBlk.ssnRlMode == 0xFF )
            {
                tChnlInfoBlk.ssnRlMode = tPtclFun.usingMode;
            }

            tSsnSets_p->setNum    = tPtclFun.setNum;
            tSsnSets_p->stepDsc_p = tPtclFun.remoteSet_p;
        }
    }

    // create asdu for chnl's info_blk


    APDU_DSCR tmpApduDscr ;
    memset ( &tmpApduDscr, 0, sizeof ( APDU_DSCR ) );
    ////104����
    //   if ( iPtclID == cfgMng->getchnlObjCfg()->ptclMdlNum )
    //   {
    tmpApduDscr = ( cfgMng->getchnlObjCfg() ) [tChnlInfoBlk.chnlNo].tmpApduDscr;
    //   }
    //  else
    //  {
    //      cfgMng->tryLog.LPrintf ( DEBUG,DEBUG_LEVEL_0, "tmpApduDscr, Configuration error " );
    //     exit ( 0 );
    //  }

    // create apdu_obj
    tChnlInfoBlk.reqAPDU_Obj_p  = crtAPDU_f ( &tmpApduDscr, 1, 0 );
    tChnlInfoBlk.respAPDU_Obj_p = crtAPDU_f ( &tmpApduDscr, 1, 0 );
    // create
    chnlInfoBlk* tmpChnlInfoBlk_p = &tChnlInfoBlk;
    tRtn = tCurSsnObj_p->crtChnlInfo ( tmpChnlInfoBlk_p );

    if ( tRtn != 0 )
    {
        tErrFlag = tErrFlag | 0x000009;

        if ( actFlag == MC_ACT_FLAG )
        {
            tCurSsnObj_p->delActiveSsn ( tSsnCtlBlk_p );
        }
        else
        {
            // ��ɾ����ֻ�ı䱻���Ự״̬��ͬʱ������Ӧ�ֶ�ֵ
            // tCurSsnObj_p->delPassiveSsn(tSsnCtlBlk_p);
        }

        // ...
    }

    //
    tSsnCtlBlk_p->chnlID          = tSsnReqBlk.chnlID;
    tSsnCtlBlk_p->myChnlInfoBlk_p = tmpChnlInfoBlk_p;//tCurSsnObj_p->getChnlInfoBlk(tSsnReqBlk.chnlID);
    tSsnCtlBlk_p->actFlg          = actFlag;
    tSsnCtlBlk_p->drvStepNo       = 1; // ������ʼֵ�� 1������Ϊ��
    tSsnCtlBlk_p->hostTID         = tSsnReqBlk.tID;
    tSsnCtlBlk_p->checkStd        = 0;
    tSsnCtlBlk_p->retryNum        = 0; // ��ʱ����
    tSsnCtlBlk_p->role            = tSsnReqBlk.role;
    tSsnCtlBlk_p->ssnNo           = tSsnReqBlk.ssnID;
    tSsnCtlBlk_p->ssnTmout        = tSsnReqBlk.timeout;

    //tSsnCtlBlk_p->ssnPara_p = g_snnPara_p;                    // test, get global's var,

    // init the one_step's two apdu
    tSsnCtlBlk_p->drvPrmtr_p->reqAPDU_Obj_p  = tSsnCtlBlk_p->myChnlInfoBlk_p->reqAPDU_Obj_p;
    tSsnCtlBlk_p->drvPrmtr_p->respAPDU_Obj_p = tSsnCtlBlk_p->myChnlInfoBlk_p->respAPDU_Obj_p;
    // ...  ע��APDU��ͨ����Ϣ�飬��ASDU���ȸ�ֵ���Ựʵ������

    // ע��ASDU ����Ҫ��ASDU���е����ݳ���һ�£���
    tCurSsnObj_p->setASDUlen ( /*50*/sizeof ( qASDUInfoRcvUnit ), tSsnCtlBlk_p, tSsnCtlBlk_p->actFlg ); //tSsnCtlBlk_p->actASDU_lng = 1024;// test

    //get chnl(g).
    if ( !tSsnCtlBlk_p/*tChnlObj_p*/ )
    {
        tErrFlag = tErrFlag | 0x000008;

        if ( actFlag == MC_ACT_FLAG )
        {
            tCurSsnObj_p->delActiveSsn ( tSsnCtlBlk_p );
        }
        else
        {
            tCurSsnObj_p->delPassiveSsn ( tSsnCtlBlk_p );
        }
    }

    //printf("----getChnlObj err=%d\r\n", tErrFlag);
    cfgMng->tryLog.LPrintf (
    DEBUG, DEBUG_LEVEL_1,
    "++++crtSsn:: chnl=%d, ssn=%p, StnCfg=%p, Rl=%p, req=%p, resp=%p, cblk=%p, step=%p \r\n",
    tSsnReqBlk.chnlID, tSsnCtlBlk_p,
    tSsnCtlBlk_p->myChnlInfoBlk_p->ssnStnCnfg_p,
    tSsnCtlBlk_p->myChnlInfoBlk_p->funRl_p,
    tSsnCtlBlk_p->drvPrmtr_p->reqAPDU_Obj_p,
    tSsnCtlBlk_p->drvPrmtr_p->respAPDU_Obj_p, tSsnCtlBlk_p->myChnlInfoBlk_p,
    tSsnCtlBlk_p->drvPrmtr_p );

    if ( !tErrFlag )
    {
        //printf("----addSsn \r\n");
        tPrio = ssnMgr.getSsnPrio ( tSsnReqBlk.ssnID );
        tChnlObj_p->addSsn ( tSsnCtlBlk_p, tPrio );

        //printf("----addSsn \r\n");
        //create app timer if tSsnReqBlk.timeout!=0, then start it.
        //app timer timeout handler using system default routine
        tmChnlID = tSsnReqBlk.chnlID;

        if ( tSsnReqBlk.timeout )
        {
            //  tRtn = tmFclty.addTmChnl(1, &tmChnlID);//this tm chnl only include 1 tmr.// Ӧ�ü���ʱ������ӵ�ͨ����ʱ����

            //printf("----addTmChnl ret=%d\r\n", tRtn);

            tTmrCfg.ID = 0;//lzq, 2017.1.25
            char tTmrNm[20];
            sprintf ( tTmrNm, "act ssnID:%d\n", tSsnReqBlk.ssnID );
            tTmrCfg.name_p         = tTmrNm;
            tTmrCfg.tmFunHdl       = ssnTmOutFun_p;
            tTmrCfg.tmFunHdlPara_p = ( void* ) tSsnCtlBlk_p;
            tTmrCfg.type           = 0;//once
            tTmrCfg.fstXpirTm      = tSsnReqBlk.timeout;
            // Ӧ�ü���ʱ��������Ӧ�ã������뵽ͨ����ʱ���У�20170601��by zhaojj
            tSsnCtlBlk_p->ssnTmHead_p = tmFclty.ctrAppTmChnl ( tmChnlID, &tTmrCfg );

            if ( tSsnCtlBlk_p->ssnTmHead_p ) //!tmFclty.addTmr(tmChnlID, &tTmrCfg))
            {
                if ( actFlag == MC_ACT_FLAG )
                {
                    //printf("----addTmr \r\n");
                    //tSsnCtlBlk_p->ssnTmHead_p = tmFclty.getTmChnlPtrByID(tmChnlID);
                    tmFclty.ctlTmr ( tSsnCtlBlk_p->ssnTmHead_p, tTmrCfg.ID, TM_START ); //start app tmr!
                    //printf("----addTmr-- \r\n");
                }
            }
            else
            {
                tErrFlag = tErrFlag | 0x000010;
            }
        }

        //apply for adopt ssn inst to sin container
        if ( !tErrFlag && tSsnCtlBlk_p->actFlg == MC_ACT_FLAG )
        {
            tRtn = ssnCtnr.aplyAdtSsnInst ( tPrio, tSsnCtlBlk_p );

            if ( tRtn != MC_SNNST_OK )
            {
                cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "----aplyAdtSsnInst error \r\n" );
            }
            else
            {
                cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_1, "----aplyAdtSsnInst ok\r\n" );
            }
        }
        else
        {
            cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_1, "----no aplyAdtSsnInst \r\n" );
        }

    } // ! if

    return tErrFlag;
}

//ִ�й����д����붨��:
//0x00 0001--���Ӵ�
//0x00 0002--�޷����ݻỰ����õ��Ự����
//0x00 0004--���������Ựʵ��ʧ�ܣ�
//0x00 0008--�޷����ݻỰ����õ��Ựʵ��ʹ�õ�ͨ������
//0x00 0010--Ϊ����ĻỰʵ�����Ӧ�ö�ʱ��ͨ��ʧ��
//0x00 0020--���ȵ���ʵ���޻Ự����
//0x00 0040--���ȵ��ĻỰʵ��û��asdu(apdu)����
//0x00 0080--���ȵ��ĻỰ����û������ͨ����Ϣ��
//0x00 0100--���ȵ��ĻỰʵ��û�з���ͨ������
//0x00 0200--���ȵ��ĻỰʵ��û�жԶ˶�վ����
//0x00 0400--���ȵ��ĻỰʵ��Ӧ�ü�����δͨ���������
//0x00 0800--���ȵ��ĻỰʵ��Ӧ�ü����ݴ洢ʧ��
//0x00 1000--���ȵ��ĻỰʵ��Ӧ�ü����ݼ���ʧ��
//0x00 2000--���ȵ��ĻỰʵ��Ӧ�ü�����(��Ϣ����)�����޷�ʶ��
//0x00 4000--���ȵ��ĻỰʵ���޷�ͨ���Ự�Ż�ù��ܺ�
//0x00 8000--���ȵ��ĻỰʵ��������asdu���ʧ�� 
//0x01 0000--���ȵ��ĻỰʵ���Ự���������޷�ʶ��ķ�֧��
//0x02 0000--�����Ự��Ӧ��ʱ
//0x04 0000--����Ϣ����ʧ��
//--checkstate's code-------------------------------------------
//0x00 0000--��ȷ����
//0x80 0000--�ȴ����ն���Ӧ 20170324
//0x80 0000--�ȴ����ն���Ӧ 20170324


void Et_sys_AppSnd()
{
    ssnCtlBlk* tSsnCtlBlk_p = NULL;
    T_ssnInstInfoSet tSsnInstInfoSet;
    C_ChnlObj* tChnlObj_p;
    C_SsnObj* tCurSsnObj_p = NULL;
    u_char* tASDU_p = NULL;
    int tEvtType;
    C_StnObj* tStnObj_p;
    HMODULE* tModule_p = NULL;
    u_short tSsnID, tStnID;
    u_char tFunNo;
    u_int tErrFlag              = 0;
    chnlInfoBlk* tChnlInfoBlk_p = NULL;
    int chnlRole = 0;
    qSsnReqUint tSsnReqBlk;
    u_char tSsnInfoLng;

    tSsnReqBlk.chnlID = 0xFFFF;
    tSsnReqBlk.ssnID = 0xFFFF;

    //
    T_loadPnt loadPnt_F;
    T_ssnStartF tSsnStartF_p;
    T_ssnStepF tSsnDrvStepF_p;
    u_char tBrnchGuide;

    //

    int iPopQCnt = 0;
    int tRtn = 0;

    while ( 1 )
    {
        memset ( &tSsnReqBlk, 0, sizeof ( qSsnReqUint ) );
        memset ( &tSsnInstInfoSet, 0, sizeof ( T_ssnInstInfoSet ) );

        tSsnInfoLng = sizeof ( qSsnReqUint );
        tSsnCtlBlk_p = NULL;
        tChnlObj_p = NULL;
        tCurSsnObj_p = NULL;
        tChnlInfoBlk_p = NULL;
        tErrFlag = 0;
        //rcv ssn rdy evt or ssn req info
        cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_1, "  Et_sys_AppSnd::ssnDuQ.popQ\r\n" );
        tEvtType = ssnDuQ.popQ ( BOTH_GROUP, ( char* ) ( &tSsnReqBlk ), &tSsnInfoLng );
#ifdef WINDOWS_PLATFORM
        int tid = GetThreadId ( tSsnReqBlk.tID );
#else
        //int tid = syscall(SYS_gettid);
        int tid = ( int ) getThread();
#endif
        cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_1,
                                 "Et_sys_AppSnd::ssnDuQ.popQ type=%d, chnl=%d, ssn=%d, tid=%d, role=%d \r\n",
                                 tEvtType, tSsnReqBlk.chnlID, tSsnReqBlk.ssnID, tid, tSsnReqBlk.role );

        if ( tEvtType <= 0 )
        {
            tErrFlag = tErrFlag | 0x000001;
        }
        else if ( FST_GROUP == ( tEvtType & 0x0f ) ) //ssn req
        {
            // �����Ự������
            tRtn = crtSsn ( tCurSsnObj_p, tSsnReqBlk, tChnlObj_p, tModule_p, tSsnCtlBlk_p, tErrFlag, MC_ACT_FLAG );

            if ( tRtn < 0 )
            {
                printf ( "create Ssn failuer!\r\n" );
                continue;
            }
        } // ! else if

        if ( tErrFlag )
        {
            // error
            if ( !tSsnCtlBlk_p )
            {
                finishBlock ( NULL, tSsnReqBlk.tID, tSsnReqBlk.ssnID, tSsnReqBlk.chnlID, tErrFlag );
            }
            else
            {
                finishBlock ( tSsnCtlBlk_p, tSsnReqBlk.tID, tSsnReqBlk.ssnID, tSsnReqBlk.chnlID, tErrFlag );
            }

            tErrFlag = 0;
            continue;
        }

        //      printf("Et_sys_AppSnd:: ----schdSsnObj \r\n");

        nsCPLock::CAutolock tAutoLock ( &g_ssnLock );
        /////////////////////////schedule ssn
        //the other ssn! get a tSsnCtlBlk_p!
        ssnCtlBlk* tmpSsnCtlBlk = ssnCtnr.schdSsnObj ( /*tCurSsnObj_p*/ ); //return inst ctrl blk

        if ( !tmpSsnCtlBlk ) //�޷����ش�����Ϣ
        {
            cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_1, "No schedulable ssnCtlBlk!\r\n" );
            continue;
        }

        tSsnCtlBlk_p = tmpSsnCtlBlk;
        /////////////////////////new ssn inst begin!!!
        u_short usChnlState = 0;
        tSsnID = tSsnCtlBlk_p->ssnNo;
        //drive nxt step
        tFunNo = ssnMgr.getSsnDscNo ( tSsnID ); //global shift to funNo
        tCurSsnObj_p = ssnMgr.getSsnObj ( tSsnID );

        if ( tCurSsnObj_p )
        {
            tCurSsnObj_p->getSsnInstInfo ( tSsnCtlBlk_p, &tSsnInstInfoSet );

            tChnlInfoBlk_p = tSsnCtlBlk_p->myChnlInfoBlk_p;//tCurSsnObj_p->getChnlInfoBlk(tSsnInstInfoSet.chnlID);

            if ( !tChnlInfoBlk_p )
            {
                tErrFlag = tErrFlag | 0x000080;
            }

            tASDU_p = tCurSsnObj_p->getAPDU ( tSsnInstInfoSet.actFlg, tChnlInfoBlk_p ); //tCurSsnObj_p->getAPDU(tSsnInstInfoSet.actFlg, tSsnInstInfoSet.chnlID);//->m_snnDescript.ASDU_p;//��parameter: ssnID,activity, chnlID,return apduObj_p��

            if ( !tASDU_p )
            {
                tErrFlag = tErrFlag | 0x000040;
            }

            tChnlObj_p = chnlMgr.getChnlObj ( tSsnInstInfoSet.chnlID );

            if ( !tChnlObj_p )
            {
                tErrFlag = tErrFlag | 0x000100;
            }
            else
            {
                chnlRole = tChnlObj_p->getRole();
                usChnlState = chnlMgr.getChnlState ( tSsnInstInfoSet.chnlID );
                tStnID = tSsnInstInfoSet.chnlID; //stnID = chnlID
                tStnObj_p = tChnlObj_p->getStn();

                if ( !tStnObj_p )
                {
                    tErrFlag = tErrFlag | 0x000200;
                }
            }

            tModule_p = ( HMODULE* ) ( tChnlObj_p->getPtclMdl() );
        }
        else
        {
            tErrFlag = tErrFlag | 0x000020;
        }

        // tͨ��״̬���������£�ֻ����U֡ͨ��
        if ( ssnMgr.getSsnPtclNo ( tSsnID ) < U_STARTDT ) // Ŀǰ104 u֡Э���
        {
            tErrFlag |= usChnlState & MC_CHNL_BIT ? tErrFlag : 0x000101;
        }

        if ( tErrFlag )
        {
            finishBlock ( tSsnCtlBlk_p, tSsnCtlBlk_p->hostTID, tSsnID, tSsnCtlBlk_p->chnlID, tErrFlag );
            tErrFlag = 0;
            continue;
        }

        // continue, one more PDU of one step
        do
        {
            //
            int iCurNum = -1;

            if ( tSsnInstInfoSet.checkState < 0 ) //rcv side check err
            {
                tErrFlag = tErrFlag | 0x000400;
                tBrnchGuide = 0x8;
                //cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "rcv side check err = %0x\r\n", tErrFlag );
                cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                         "%s|%s|%d:rcv side check err = %0x\r\n",
                                         __FILE__, __func__, __LINE__, tErrFlag );
            }
            else if ( tSsnInstInfoSet.checkState == MC_NO_SEND_WAIT_CHECK )
            {
                tBrnchGuide = MC_NO_SEND_WAIT_CHECK;
                cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_1, " MC_NO_SEND_WAIT_CHECK" );
            }
            else//handle rcved usr data!
            {
                //
                if ( chnlRole == MC_CTL_STN )
                {
                    if ( tSsnInstInfoSet.actFlg == MC_ACT_FLAG )
                    {
                        iCurNum = tSsnInstInfoSet.drvPrmtr_p->respPDU_Num;
                    }
                    else
                    {
                        iCurNum = tSsnInstInfoSet.drvPrmtr_p->reqPDU_Num;
                    }
                }
                else
                {
                    if ( tSsnInstInfoSet.actFlg == MC_ACT_FLAG )
                    {
                        iCurNum = tSsnInstInfoSet.drvPrmtr_p->reqPDU_Num;
                    }
                    else
                    {
                        iCurNum = tSsnInstInfoSet.drvPrmtr_p->respPDU_Num;
                    }
                }

                cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_1,
                                         "Et_sys_AppSnd::checkstate is ok ! chnl=%d, pnt=%d-- \r\n",
                                         tSsnInstInfoSet.chnlID, tSsnInstInfoSet.ssnPara_p->pntNum );

                //
                // 1.store data into stn, data of current step
                T_getPtclID tGetPtclID =
                ( T_getPtclID ) GetProcAddress ( *tModule_p, "getPtclID" );
                int iPtclID = tGetPtclID();

                if ( ( tSsnInstInfoSet.ssnPara_p->stnAcsFlag ) & 0x1 ) //call memSubSys store rcv data into stn mem
                {
                    if ( !tSsnInstInfoSet.ssnPara_p->infoType ) //store info to stn mem
                    {
                        // if ( 0 > tStnObj_p->storeStnData ( tSsnInstInfoSet.ssnPara_p, tStnID, 0, ( tSsnInstInfoSet.ssnPara_p->stnAcsFlag >> 3 ), iPtclID ) )
                        // {
                        //    tErrFlag = tErrFlag | 0x000800;
                        // }
                        // else
                        // {
                        if ( optData ( tSsnInstInfoSet.ssnPara_p, tStnID, 0, SAVE_BUFF, tFunNo ) )
                        {
                            tErrFlag = tErrFlag | 0x000800;
                        }
                        else
                        {
                            DATAFORMAT_DATE7B1 sysTime;
                            time_t tvSec = 0;
                            getSysTime ( sysTime, tvSec );
                            unitInfData.setChnTmStmp ( tStnID, &sysTime );
                        }

                        //   }
                    }

                    if ( 1 == tSsnInstInfoSet.ssnPara_p->infoType ) //store file to stn mem
                    {
                        //  if ( 0 > tStnObj_p->storeStnData ( tSsnInstInfoSet.ssnPara_p, tStnID, 1, ( tSsnInstInfoSet.ssnPara_p->stnAcsFlag >> 3 ), iPtclID ) )
                        //  {
                        //      tErrFlag = tErrFlag | 0x000800;
                        //  }
                        //  else
                        //   {
                        if ( optData ( tSsnInstInfoSet.ssnPara_p, tStnID, 1, SAVE_BUFF, tFunNo ) )
                        {
                            tErrFlag = tErrFlag | 0x000800;
                        }
                        else
                        {
                            DATAFORMAT_DATE7B1 sysTime;
                            time_t tvSec = 0;
                            getSysTime ( sysTime, tvSec );
                            unitInfData.setChnTmStmp ( tStnID, &sysTime );
                        }

                        //    }
                    }
                }

                if ( tSsnInstInfoSet.drvStepNo != tSsnInstInfoSet.drvPrmtr_p->stepNo // ��ԭ����֡���أ��ĳɲ�����(by zhaojj,20170303)
                     || ( tSsnInstInfoSet.drvStepNo == 1 && iCurNum == 0 ) ) // ��һ������һ֡
                {
                    switch ( tSsnInstInfoSet.ssnPara_p->infoType )
                        // �ı��飺������-��֡����-�������أ�20170504
                    {
                        case 0x0:
                            loadPnt_F = ( T_loadPnt ) GetProcAddress ( *tModule_p, "getGLoadPnt" ); //���޸�Ϊwin,lnxͨ����ʽ
                            break;

                        case 0x1:
                            loadPnt_F = ( T_loadPnt ) GetProcAddress ( *tModule_p, "getFLoadPnt" ); //���޸�Ϊwin,lnxͨ����ʽ
                            break;

                        default:
                            loadPnt_F = NULL;
                            break;
                    }

                    if ( loadPnt_F )
                    {
                        tSsnInstInfoSet.ssnPara_p->actFlag = tSsnInstInfoSet.actFlg;

                        if ( 0 != loadPnt_F ( tChnlInfoBlk_p->ssnStnCnfg_p, tSsnInstInfoSet.drvStepNo, tSsnInstInfoSet.ssnPara_p ) )
                        {
                            tErrFlag = tErrFlag | 0x000401;
                            cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_1,
                                                     "loadPnt error ! chnl=%d \r\n",
                                                     tSsnInstInfoSet.chnlID );
                        }
                    }
                    else
                    {
                        tErrFlag = tErrFlag | 0x000402;
                    }

                    // drump step, reset one_step
                    ONE_STEP* tRtFunRl_p = tSsnInstInfoSet.drvPrmtr_p;
                    tRtFunRl_p->reqPDU_Num    = 0;
                    tRtFunRl_p->reqPDU_FCode  = 0;

                    tRtFunRl_p->respPDU_Num   = 0;
                    tRtFunRl_p->respPDU_FCode = 0;

                    // 2.load data from stn, data of tSsnInstInfoSet.drvStepNo(=nxtStepNo)
                    if ( ( tSsnInstInfoSet.ssnPara_p->stnAcsFlag ) & 0x2 ) //load all stn data of this step.
                    {
                        //  tModule_p = (HMODULE*)(tChnlObj_p->getPtclMdl());
                        //get usr data, read stn pnt table.
                        //accord *tSsnStartPara_p load data in station.(tSsnInstInfoSet.drvStepNo=nxt step)
                        switch ( tSsnInstInfoSet.ssnPara_p->infoType )
                        {
                            case 0x0:
                            {
                                if ( 0 != optData ( tSsnInstInfoSet.ssnPara_p, tStnID, 0, LOAD_BUFF, tFunNo ) )
                                {
                                    tErrFlag = tErrFlag | 0x001000;
                                }

                                /*
                                if ( 0 > tStnObj_p->loadStnData (
                                     tSsnInstInfoSet.ssnPara_p, tStnID, 0,
                                     ( tSsnInstInfoSet.ssnPara_p->stnAcsFlag >> 3 ),
                                     iPtclID ) )
                                {
                                    tErrFlag = tErrFlag | 0x001000;
                                }*/

                                break;
                            }

                            case 0x1:
                            {
                                if ( 0 != optData ( tSsnInstInfoSet.ssnPara_p, tStnID, 1, LOAD_BUFF, tFunNo ) )
                                {
                                    tErrFlag = tErrFlag | 0x001000;
                                }

                                /*
                                    if ( 0 > tStnObj_p->loadStnData (
                                         tSsnInstInfoSet.ssnPara_p, tStnID, 1,
                                         ( tSsnInstInfoSet.ssnPara_p->stnAcsFlag >> 3 ),
                                         iPtclID ) )
                                    {
                                        tErrFlag = tErrFlag | 0x001000;
                                    }*/

                                break;
                            }

                            //simulate usr send data
                            /*                  case 0x2:
                            case 0x3:
                            break;*/
                            default:
                            {
                                tErrFlag = tErrFlag | 0x002000;
                                break;
                            }
                        }
                    } // ! if

                } // if



                if ( tFunNo < 0 )
                {
                    tErrFlag = tErrFlag | 0x004000;
                }

                //
                tSsnInstInfoSet.drvPrmtr_p->stepNo = tSsnInstInfoSet.drvStepNo; // 20170302, by zhaojj

                // bind
                revBuf* sndBuf_p = ( revBuf* ) tChnlObj_p->getSendBuf();

                if ( sndBuf_p == NULL )
                {
                    tErrFlag = tErrFlag | 0x004010;
                }

                T_bindToMem tBindToMem_f = ( T_bindToMem ) GetProcAddress ( *tModule_p, "bindToMem" );

                if ( tBindToMem_f )
                {
                    u_char* ASDU_p = ( u_char* ) ( sndBuf_p[tSsnCtlBlk_p->actFlg] ).ASDU_Buf.ASDU_Data_p;
                    u_short ASDULng = ( sndBuf_p[tSsnCtlBlk_p->actFlg] ).ASDU_Buf.ASDU_Lng;

                    T_ptclFunDsc tFunDsc;
                    ssnSets* tmpSsnSet_p = tChnlObj_p->getSsnSet ( tSsnCtlBlk_p->ssnNo, tSsnCtlBlk_p->actFlg );
                    tFunDsc.remoteSet_p = tmpSsnSet_p->stepDsc_p;
                    tFunDsc.setNum = tmpSsnSet_p->setNum;
                    tFunDsc.ssnDscNo = tSsnCtlBlk_p->ssnNo;
                    tFunDsc.act_prio_chnlID = tSsnCtlBlk_p->actFlg;
                    tFunDsc.chnlRole = chnlRole;

                    if ( tFunDsc.chnlRole == MASTER_STATION ) // ����վ����վ��
                    {

                        tFunDsc.frmDirect = MC_CONTROL_DIRECTION ;//���Ʒ���

                    }
                    else //������վ����վ��
                    {
                        tFunDsc.frmDirect =  MC_MONITOR_DIRECTION ;//���ӷ���

                    }

                    if ( chnlRole == MC_CTL_STN )
                    {
                        if ( tSsnInstInfoSet.actFlg == MC_ACT_FLAG )
                        {
                            iCurNum = tSsnInstInfoSet.drvPrmtr_p->respPDU_Num;
                        }
                        else
                        {
                            iCurNum = tSsnInstInfoSet.drvPrmtr_p->reqPDU_Num;
                        }
                    }
                    else
                    {
                        if ( tSsnInstInfoSet.actFlg == MC_ACT_FLAG )
                        {
                            iCurNum = tSsnInstInfoSet.drvPrmtr_p->reqPDU_Num;
                        }
                        else
                        {
                            iCurNum = tSsnInstInfoSet.drvPrmtr_p->respPDU_Num;
                        }
                    }

                    tFunDsc.realTID = tSsnCtlBlk_p->ssnPara_p->ASDUID_p[iCurNum];

                    u_char* apdu_p = tCurSsnObj_p->getAPDU ( tSsnCtlBlk_p->actFlg, tSsnCtlBlk_p->chnlID );
                    int bindError = tBindToMem_f ( tSsnCtlBlk_p->drvPrmtr_p, &tFunDsc, /*apdu_p, */ASDU_p, ASDULng, 1 );

                    if ( bindError )
                    {
                        cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "bindError = %d\r\n", bindError );
						tErrFlag = tErrFlag | 0x040000;
						tSsnCtlBlk_p->checkState = MC_NO_SEND_SSN_STOP;
                    }
                }
                else
                {
                    tErrFlag = tErrFlag | 0x004020;
                }

                //
                u_int nxtStepNo = 0;

                if ( tSsnInstInfoSet.drvStepNo == 1 && iCurNum == 0 && tSsnInstInfoSet.actFlg == MC_ACT_FLAG ) //startStep: drvStepNo = 0, ��һ֡
                {
                    tSsnCtlBlk_p->checkState = MC_NO_SEND_WAIT_CHECK;
                    tSsnStartF_p    = ( T_ssnStartF ) GetProcAddress ( *tModule_p, "startStep" ); //tCurSsnObj_p->getPtclSnnFun(0);//get start reg fun addr.

                    if ( tSsnStartF_p )
                    {
                        tBrnchGuide = ( *tSsnStartF_p ) ( tFunNo, tChnlInfoBlk_p->ssnRlMode, tSsnCtlBlk_p->drvPrmtr_p, tSsnInstInfoSet.ssnPara_p );
                    }
                    else
                    {
                        tBrnchGuide = 0x9;
                    }
                }
                else if ( * ( tSsnInstInfoSet.actSsnRprtInfo_p ) >= 0 ) //step No, allway succ executed
                {
                    tSsnCtlBlk_p->checkState = MC_NO_SEND_WAIT_CHECK;
                    tSsnDrvStepF_p = ( T_ssnStepF ) GetProcAddress ( *tModule_p, "nxtStep" ); //tCurSsnObj_p->getPtclSnnFun(1);//get drv reg fun addr

                    if ( tSsnDrvStepF_p )
                    {
                        T_ptclFunDsc ptclFunDsc;
                        ptclFunDsc.act_prio_chnlID = tSsnCtlBlk_p->actFlg;
                        ptclFunDsc.ssnDscNo = ssnMgr.getSsnDscNo ( tSsnCtlBlk_p->ssnNo );
                        ptclFunDsc.chnlRole  = chnlRole;
                        u_int uiState = 0;
                        nxtStepNo = ( *tSsnDrvStepF_p ) ( tChnlInfoBlk_p->funRl_p, tSsnInstInfoSet.drvPrmtr_p,
                                                          &ptclFunDsc, tSsnInstInfoSet.ssnPara_p, &uiState );
                        tBrnchGuide = uiState;

                        if ( nxtStepNo > 0 )
                        {
                            tSsnCtlBlk_p->drvStepNo = nxtStepNo;
                            tSsnInstInfoSet.drvStepNo = nxtStepNo;
                        }

                        cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_1, "nxtStepNo = %d ,tBrnchGuide= %d \r\n", nxtStepNo, tBrnchGuide );
                    }
                    else
                    {
                        tBrnchGuide = 0x10;
                    }

                }
                else
                    //ssn err!
                {
                    tBrnchGuide = MC_NO_SEND_SSN_STOP;    //no send, abort
                }

                if ( ( tSsnInstInfoSet.drvStepNo == 1 && tSsnInstInfoSet.actFlg == MC_ACT_FLAG )
                     || ( tSsnInstInfoSet.drvStepNo == 2 && tSsnInstInfoSet.actFlg == MC_PSV_FLAG ) )
                {
                    T_getFrameType tGetType = ( T_getFrameType ) GetProcAddress ( *tModule_p, "getFrameType" );
                    T_getDU tGetDu = ( T_getDU ) GetProcAddress ( *tModule_p, "getDU" );

                    if ( tGetDu != NULL && tGetDu != NULL )
                    {
                        u_short usApduLen = 0;
                        u_char* tmpASDU_p = tGetDu ( tASDU_p, usApduLen, ASDU_TYPE );
                        tSsnCtlBlk_p->ptclCtlCd = tGetType ( tmpASDU_p, usApduLen );

                        if ( usApduLen > 0 )
                        {
                            tSsnCtlBlk_p->ptclCtlCd = I_F;
                        }
                    }
                }
            } // ! else

           cfgMng->tryLog.LPrintf(DEBUG, DEBUG_LEVEL_1,
            "Et_sys_AppSnd:: step=%d, state=%d, req_resp_num=%d-%d, err=%d \r\n",
            tSsnInstInfoSet.drvStepNo, tBrnchGuide,
            tSsnCtlBlk_p->drvPrmtr_p->reqPDU_Num,
            tSsnCtlBlk_p->drvPrmtr_p->respPDU_Num, tErrFlag );

            qASDUInfoRcvUnit asduUnit;
            asduUnit.ASDU_p = tASDU_p;
            asduUnit.chanlID = tSsnInstInfoSet.chnlID;
            asduUnit.ASDU_Lng = tSsnInstInfoSet.ASDU_lng;
            asduUnit.DU_type = tSsnCtlBlk_p->ptclCtlCd; // ֡����

            //tBrnchGuide = MC_SEND_SSN_FNSH; // test

            switch ( tBrnchGuide )
            {
                case MC_NO_SEND_CNUE_EXEC://no send, continue
                case MC_NO_SEND_WAIT_CHECK://no send, wait check
                    cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_1, "Et_sys_AppSnd::no send, wait to check! chnl=%d \r\n", tSsnInstInfoSet.chnlID );
                    continue;

                case MC_SEND_CNUE_EXEC://send, continue. goto case 0x4
                case MC_SEND_WAIT_CHECK://send, wait check. *tASDU_p is a new frame. goto case 0x5
                case MC_SEND_SSN_RETRY://send, retry. direct enter is old frame. else new frame.
                {
					cfgMng->tryLog.LPrintf(DEBUG, DEBUG_LEVEL_1, "   ASDUDuQ.pshQ\r\n");
                    if ( ASDUDuQ.pshQ ( 1, tSsnInstInfoSet.chnlID, ( char* ) &asduUnit, sizeof ( qASDUInfoRcvUnit ) ) >= 0 )
                    {
						cfgMng->tryLog.LPrintf(DEBUG, DEBUG_LEVEL_1, "   ASDUDuQ.pshQ end\r\n");
                        // ��ʱֻʹ����104��������101���������Ҫ���⴦��
                        if ( tSsnInstInfoSet.actFlg == MC_ACT_FLAG )
                        {
                            tSsnInstInfoSet.drvPrmtr_p->reqPDU_Num++;
                        }
                        else
                        {
                            tSsnInstInfoSet.drvPrmtr_p->respPDU_Num++;
                        }

                        if ( tChnlInfoBlk_p->ssnStnCnfg_p->ssnID != U_NEWHOLD )
                        {
                            tSsnCtlBlk_p->rtRunStOrRslt = MC_SNNST_BLK;
                        }

                        //nsCPPub::SleepMilliSecond ( 100 );
                        break;
                    }
                    else //goto no send, abort
                    {
                        tErrFlag = tErrFlag | 0x008000;
                        tBrnchGuide = 0x8;
                        // break;
                    }
                }

                case MC_SEND_SSN_FNSH://send, finish
                {
					cfgMng->tryLog.LPrintf(DEBUG, DEBUG_LEVEL_1, "   ASDUDuQ.pshQ\r\n");
                    if ( ASDUDuQ.pshQ ( 1, tSsnInstInfoSet.chnlID, ( char* ) &asduUnit, sizeof ( qASDUInfoRcvUnit ) ) >= 0 )
                    {
						cfgMng->tryLog.LPrintf(DEBUG, DEBUG_LEVEL_1, "   ASDUDuQ.pshQ end\r\n");
                        //
                        if ( tSsnInstInfoSet.actFlg == MC_ACT_FLAG )
                        {
                            tSsnInstInfoSet.drvPrmtr_p->reqPDU_Num++;
                        }
                        else
                        {
                            tSsnInstInfoSet.drvPrmtr_p->respPDU_Num++;
                        }

                        // nsCPPub::SleepMilliSecond ( 100 );
                        tErrFlag = 0;
                        finishBlock ( tSsnCtlBlk_p, tSsnCtlBlk_p->hostTID, tSsnID, tSsnInstInfoSet.chnlID, tErrFlag );
                        continue;
                    }
                    else //goto no send, abort
                    {
                        tErrFlag = tErrFlag | 0x008000;
                        tBrnchGuide = 0x8;
                    }
                }

                case MC_NO_SEND_SSN_FNSH://no send, finish
                {
                    tErrFlag = 0;
                    finishBlock ( tSsnCtlBlk_p, tSsnCtlBlk_p->hostTID, tSsnID, tSsnInstInfoSet.chnlID, tErrFlag );
                    break;
                }

                case MC_NO_SEND_SSN_STOP://no send, abort
                {
                    //tErrFlag has been set!
                    finishBlock ( tSsnCtlBlk_p, tSsnCtlBlk_p->hostTID, tSsnID, tSsnInstInfoSet.chnlID, tErrFlag );
                    break;
                }

                default:
                {
                    tErrFlag = tErrFlag | 0x010000;
                    finishBlock ( tSsnCtlBlk_p, tSsnCtlBlk_p->hostTID, tSsnID, tSsnInstInfoSet.chnlID, tErrFlag );
                    break;
                }
            } // ! switch

        } while ( tBrnchGuide == MC_SEND_CNUE_EXEC /*&& tSsnInstInfoSet.ssnPara_p->pntPos < tSsnInstInfoSet.ssnPara_p->pntNum*/ );

        //

    } // ! while

    return;
}

/************************************************************/
/*                      Ӧ��֡�����߳�                   */
/************************************************************/
//
//
////////Ӧ��֡�����߳�
void
Et_sys_AppRec()
{
    qASDUInfoRcvUnit tASDUInfo;
    qASDUInfoRcvUnit tASDUInfoCp;
    tASDUInfoCp.ASDU_p = new u_char[256]();
    u_char tASDUInfoLng, tFunNum = 0;
    int tRtn;

    u_short tChnlID, tSsnID;
    C_ChnlObj* tChnlObj_p;
    HMODULE* tModule_p;

    T_chk_ASDU tChk_ASDU_F = NULL ;
    T_getSsnSet tGetSsnnSet_F;

    u_int tTID, tCOT;
    ptclSnnKey tSsnKey;
    ssnCtlBlk* tSsnCtlBlk_p;
    int chnlRole = 0;
    int tIndex = 0;
    T_ptclFunDsc tMaybeFunSet[12];
    T_ptclFunDsc tMaybeSsnSet[12] =
    {
        { 0xFFFF, 0x0200 },
        { 0xFFFF, 0x0200 },
        { 0xFFFF, 0x0200 },
        { 0xFFFF, 0x0200 },
        { 0xFFFF, 0x0200 },
        { 0xFFFF, 0x0200 },
        { 0xFFFF, 0x0200 },
        { 0xFFFF, 0x0200 },
        { 0xFFFF, 0x0200 },
        { 0xFFFF, 0x0200 },
        { 0xFFFF, 0x0200 },
        { 0xFFFF, 0xFFFF }
    };

    while ( 1 )
    {
        tIndex = 0;
        tASDUInfoLng = sizeof ( qASDUInfoRcvUnit );
        memset ( tASDUInfoCp.ASDU_p, 0, 256 );
        tASDUInfoCp.ASDU_Lng     = 0;
        tASDUInfoCp.chanlID      = 0;
        tASDUInfoCp.realData_lng = 0;
		cfgMng->tryLog.LPrintf(DEBUG, DEBUG_LEVEL_1, "  Et_sys_AppRec:: ASDUInfoQ.popQ\r\n");
        tRtn = ASDUInfoQ.popQ ( FST_GROUP, ( char* ) &tASDUInfo, &tASDUInfoLng );
        tASDUInfoCp.ASDU_Lng = tASDUInfo.ASDU_Lng;
        tASDUInfoCp.realData_lng = tASDUInfo.realData_lng;
        tASDUInfoCp.chanlID = tASDUInfo.chanlID;
        memcpy ( tASDUInfoCp.ASDU_p, tASDUInfo.ASDU_p, tASDUInfo.realData_lng );
        cfgMng->tryLog.LPrintf(DEBUG, DEBUG_LEVEL_1, "Et_sys_AppRec:: ASDUInfoQ.popQ end chnl=%d \r\n", tChnlID );

        /*
        char szTmp[256 * 3] = {0};

        for ( int i = 0; i < tASDUInfoCp.ASDU_Lng; i++ )
        {
            sprintf ( szTmp + ( i * 3 ), "%02x ", tASDUInfoCp.ASDU_p[i] );
        }

        cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_1, " %s\r\n", szTmp );
        */
        if ( tRtn < 0 || tRtn != ( FST_GROUP | 0x10 ) )
        {
            continue;
        }

        //exit(1);//q opr err!

        tSsnID = DEF_ERROR_SSN_NO;
        // step 1.ȡASDU֡��ͨ�����飩��
        tChnlID = tASDUInfoCp.chanlID;
        tChnlObj_p = chnlMgr.getChnlObj ( tChnlID );

        if ( !tChnlObj_p )
        {
            exit ( 2 );
        }



        if ( ( tASDUInfoCp.ASDU_p ) /*&& (tASDUInfo.ASDU_Lng)*/ ) //non pseudo ASUD! // U ֡������ASDU��û����
        {
            // step 2.��ȡ��Լ��ģ����
            tModule_p = ( HMODULE* ) ( tChnlObj_p->getPtclMdl() );

            // step 3.ASDU������

            tChk_ASDU_F = ( T_chk_ASDU ) GetProcAddress ( *tModule_p, "chkASDU" ); // ���޸�Ϊwin,lnxͨ����ʽ

            if ( !tChk_ASDU_F )
            {
                assert ( "@@ERROR::GetProcAddress( chkASDU ) failed! " );
            }

            tTID = 0;
            tCOT = 0;
            tRtn  = tChk_ASDU_F ( tASDUInfoCp.ASDU_p, tASDUInfoCp.ASDU_Lng, &tTID, &tCOT );
            cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_1, "tTID = %d, tCOT = %d\r\n", tTID, tCOT );

            if ( tRtn < 0 )
            {
                cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_1, "chkASDU = %d\r\n", tRtn );
            }
            //printf("tChk_ASDU_F,tTID=%d \r\n",tTID);
            else// step 3 OK,else jump over step 4 and 5.
            {
                //modbus���ȡվ��ɫ��ʼ���ػ������ԣ�2017.06.23��
                chnlRole = tChnlObj_p->getRole();

                for ( int i = 0; i < 12; i++ )
                {
                    if ( chnlRole == MC_CTL_STN )
                    {
                        tMaybeFunSet[i].act_prio_chnlID = MC_ACT_FLAG;
                    }
                    else
                    {
                        tMaybeFunSet[i].act_prio_chnlID = MC_PSV_FLAG;
                    }

                    tMaybeFunSet[i].chnlRole = chnlRole;
                }

                // step 4.��ȡ���ܵĻỰKEY����
                tGetSsnnSet_F  = ( T_getSsnSet ) GetProcAddress ( *tModule_p, "getSsnSet" ); // ���޸�Ϊwin,lnxͨ����ʽ
                // set tMaybeFunSet[].ssnDscNo(fun ID in ptcl) and tMaybeFunSet[].act_prio_chnlID(actFlag)
                tSsnKey.ptclNo = tGetSsnnSet_F ( tTID, tCOT, tMaybeFunSet, &tFunNum ); // max fun number =4�����һ��������ͨ���ţ���ܣ�

                //printf("tGetSsnnSet_F,tTID=%d \r\n",tTID);
                if ( 0xFF != tSsnKey.ptclNo ) //get maybe Fun set
                {
                    // step 5.��ȡ���ܵĻỰ�ż���
                    for ( int i = 0; i < tFunNum; i++ )
                    {
                        tSsnKey.snnDscNo = tMaybeFunSet[i].ssnDscNo;
                        tMaybeSsnSet[tIndex].ssnDscNo = ssnMgr.getSsnID ( tSsnKey ); // here, ssnDscNo using for ssnID

                        if ( DEF_ERROR_SSN_NO != tMaybeSsnSet[tIndex].ssnDscNo ) // valid ssnID
                        {
                            tMaybeSsnSet[tIndex].act_prio_chnlID = tMaybeFunSet[i].act_prio_chnlID;//activity! low 8 bit
                            u_char prio = ssnMgr.getSsnPrio ( tMaybeSsnSet[tIndex].ssnDscNo );
                            tMaybeSsnSet[tIndex].act_prio_chnlID = ( ( prio ) << 8 ) | tMaybeSsnSet[tIndex].act_prio_chnlID;
                            tIndex++;
                        }
                    }

                    if ( tIndex ) //match ssn flag
                    {
                        tFunNum = tIndex;
                        tSsnID = 0x0;
                    }
                } // ! if
            } // ! if
        } // !if

        //step 6.�Ự�ɷ�
        //err(tRtn<0(ASDU err)||tSsnKey.ptclNo=0xFF(fun dis-match)
        //                  ||tIndex=0(ssn dis-match)||pseudo ASUD):
        //      tFunNum=1,
        //      tMaybeSsnSet[0].ssnDscNo=0xFFFF,
        //      MaybeSsnSet[0].act_prio_chnlID=real channel ID
        //OK:
        //      tFunNum>=1,
        //      tMaybeSsnSet[i].ssnDscNo = real ssnID,
        //      tMaybeSsnSet[i].act_prio_chnlID = (prio<<8)|act
        if ( DEF_ERROR_SSN_NO == tSsnID )
        {
            tMaybeSsnSet[0].ssnDscNo = tSsnID;//ok: = real ssnID, err:=0xFFFF
            tMaybeSsnSet[0].act_prio_chnlID = tChnlID;
            tFunNum = 1;
        }

        //
        tMaybeSsnSet[tFunNum].act_prio_chnlID = tChnlID;

        //      printf("Et_sys_AppRec:: dispbSsnObj chnl=%d \r\n", tChnlID);

        // lock
        nsCPLock::CAutolock tAutoLock ( &g_ssnLock );

        //ssnInst or fault Inst block!
        //para1:in, ssnTbl ptr; para2:in, maybe ssn set;
        //para3:in, element number in maybe ssn set
        //para4: out,dispatched maybe ssn element index of tMaybeSsnSet,
        //if dispatch fualt inst ctl blk tIndex = -1;
        tSsnCtlBlk_p = ssnCtnr.dispbSsnObj ( tChnlObj_p->getSsnTbl(), tMaybeSsnSet, tFunNum, &tIndex );

        if ( ( DEF_ERROR_SSN_NO == tSsnID ) || ( -1 == tIndex ) //if erro or  mis-dispatch, fault Inst block or will be scheduled in send side
             /*|| tSsnCtlBlk_p->checkState == MC_NO_SEND_WAIT_CHECK */ )
        {
            ssnDuQ.pshQ ( SCD_GROUP, 0, NULL, 0 ); //send ready inform
            continue;
        }

        // step 7.��ȡ�Ự��
        // dispacth snn: tMaybeSsnSet[tIndex]

        tSsnID = tMaybeSsnSet[tIndex].ssnDscNo;
        C_SsnObj* tSsnObj_p;
        tSsnObj_p = ssnMgr.getSsnObj ( tSsnID );

        //
        if ( tSsnCtlBlk_p->actFlg == MC_PSV_FLAG )
        {
            //�Ự���ƿ����
            //tSsnCtlBlk_p->ssnCtlLock.Lock();
            //printf("Et_sys_AppRec::tSsnCtlBlk_p->ssnCtlLock.Lock(),"
            //        "tSsnCtlBlk_p->rtRunStOrRslt=[%d]\r\n",
            //        tSsnCtlBlk_p->rtRunStOrRslt);

            // ��ʼ�������Ự
            //
            if ( tSsnCtlBlk_p->rtRunStOrRslt == MC_SNNST_OK )
            {
                tSsnCtlBlk_p->ssnNo = 0;
                tRtn = resetPsvSsn ( tSsnCtlBlk_p, tSsnID, tChnlID );

                int tPrio = ssnMgr.getSsnPrio ( /*tSsnCtlBlk_p->ssnNo*/tSsnID );
                tRtn = ssnCtnr.aplyAdtSsnInst ( tPrio, tSsnCtlBlk_p );

                tSsnCtlBlk_p->rtRunStOrRslt = MC_SNNST_RDY;
                tSsnCtlBlk_p->drvPrmtr_p->stepNo = 1;
				cfgMng->tryLog.LPrintf(DEBUG, DEBUG_LEVEL_1, 
                    "Et_sys_AppRec:: tSsnCtlBlk_p->rtRunStOrRslt = %d,"
                    "tPrio=[%d],tRtn=[%d]\r\n", 
                    tSsnCtlBlk_p->rtRunStOrRslt,tPrio,tRtn
                    );
            }
            else
            {
                 tSsnCtlBlk_p->drvPrmtr_p->stepNo = 1;
				cfgMng->tryLog.LPrintf(DEBUG, DEBUG_LEVEL_1,"Et_sys_AppRec:: no dispbSsnObj, chnl=%d, ssnID=%d, actFlag=%d \r\n", tChnlID, tSsnID, tSsnCtlBlk_p->actFlg );
            }

        }

        //  printf("Et_sys_AppRec:: dispbSsnObj chnl=%d, ssnID=%d, actFlag=%d \r\n",
        //  tChnlID, tSsnID, tSsnCtlBlk_p->actFlg);

        //
        // APDU obj assigned when req active ssn to create active ssn inst or init passive ssn inst
        u_char* tAPDU1Obj_p;//type u_char
        tAPDU1Obj_p = tSsnObj_p->getAPDU ( ( u_char ) tMaybeSsnSet[tIndex].act_prio_chnlID, tChnlID ); //

        if ( tAPDU1Obj_p == NULL )
        {
            /*if ( tSsnCtlBlk_p->actFlg == MC_PSV_FLAG )
            {
                tSsnCtlBlk_p->ssnCtlLock.UnLock();
                printf("--1--Et_sys_AppRec::tSsnCtlBlk_p->ssnCtlLock.UnLock()\r\n");
            }*/
            continue;
        }

        // bindToMem // U֡���������������
        //      assert ((tASDUInfo.ASDU_p != NULL) && (tASDUInfo.ASDU_Lng > 0));
        ssnSets* tmpSsnSet_p = tChnlObj_p->getSsnSet ( tSsnCtlBlk_p->ssnNo, tSsnCtlBlk_p->actFlg );
        tMaybeFunSet[tIndex].remoteSet_p = tmpSsnSet_p->stepDsc_p;
        tMaybeFunSet[tIndex].setNum = tmpSsnSet_p->setNum;
        tMaybeFunSet[tIndex].ssnDscNo = ssnMgr.getSsnDscNo ( tMaybeSsnSet[tIndex].ssnDscNo );

        tMaybeFunSet[tIndex].act_prio_chnlID = tMaybeSsnSet[tIndex].act_prio_chnlID;
        tMaybeFunSet[tIndex].realTID = ( u_char ) tTID;
        tMaybeFunSet[tIndex].chnlRole = chnlRole;
        T_bindToMem tBindToMem_F = ( T_bindToMem ) GetProcAddress ( *tModule_p, "bindToMem" ); //���޸�Ϊwin,lnxͨ����ʽ
        tRtn = tBindToMem_F ( tSsnCtlBlk_p->drvPrmtr_p, &tMaybeFunSet[tIndex], /*tAPDU1Obj_p, */tASDUInfoCp.ASDU_p, tASDUInfoCp.ASDU_Lng, 1 );

        //printf("tBindToMem_F,tTID=%d \r\n",tTID);
        if ( tRtn != 0 )
        {
            cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "RecBindError = %d\r\n", tRtn );
            // -1:APDU_lng's err; -2: VSQ �޶��ʳ��ȴ���;
            ssnDuQ.pshQ ( SCD_GROUP, 0, NULL, 0 ); //send ready inform
            continue;
        }

        T_getFunStep tGetFunStep_F;
        tGetFunStep_F = ( T_getFunStep ) GetProcAddress ( *tModule_p, "getFunStep" ); //���޸�Ϊwin,lnxͨ����ʽ

        // tRtFunRl_p->stepNo no mis-match case!
        ONE_STEP* tRtFunRl_p = tSsnCtlBlk_p->drvPrmtr_p;//method? + own set method
        tRtFunRl_p->stepNo = tGetFunStep_F ( &tMaybeFunSet[tIndex], tTID, tCOT,
                                             tAPDU1Obj_p, tASDUInfoCp.ASDU_p, tASDUInfoCp.ASDU_Lng );

        //if ( tRtFunRl_p->stepNo == 255 )
        //{
        cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_1, "tRtFunRl_p->stepNo = %d\r\n", tRtFunRl_p->stepNo );
        // }

        //printf("tGetFunStep_F,tTID=%d \r\n",tTID);
        // step 8.
        T_snnPara* tSsnPara_p;
        // modify by zjj
        //tSsnObj_p->getSsnPara(tSsnCtlBlk_p,tSsnPara_p);//method? get: tSsnCtlBlk_p->ssnPara_p;
        tSsnPara_p = tSsnCtlBlk_p->ssnPara_p;

        tRtFunRl_p->nextStep_p = NULL;

        if ( tRtFunRl_p->stepNo != tSsnCtlBlk_p->drvStepNo ) // drump step, reset one_step
        {
            tRtFunRl_p->reqPDU_Num = 0;
            tRtFunRl_p->reqPDU_FCode = 0;

            tRtFunRl_p->respPDU_Num = 0;
            tRtFunRl_p->respPDU_FCode = 0;
            cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_1, "Jump = %d\r\n", tRtFunRl_p->stepNo );
        }

        if ( chnlRole == MC_CTL_STN )
        {
            if ( ( u_char ) ( tMaybeSsnSet[tIndex].act_prio_chnlID ) == MC_ACT_FLAG ) //active ssn
            {
                //  tRtFunRl_p->respAPDU_Obj_p = tAPDU1Obj_p;
                tRtFunRl_p->respPDU_Num++;
                cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_1, "respPDU_Num + 1 = %d\r\n", tRtFunRl_p->respPDU_Num );
            } // ! if
            else
            {
                tRtFunRl_p->reqPDU_Num++;
                cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_1, "reqPDU_Num + 1 = %d\r\n", tRtFunRl_p->reqPDU_Num );
            }
        }
        else//passive ssn, we are responder, protocol verify according to function rule!
        {

            tRtFunRl_p->reqPDU_Num++;
            cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_1, "reqPDU_Num + 1 = %d\r\n", tRtFunRl_p->reqPDU_Num );
        }

        //
        chnlInfoBlk* tChnlInfoBlk_p = tSsnCtlBlk_p->myChnlInfoBlk_p; //tSsnObj_p->getChnlInfoBlk(tChnlID);

        if ( tChnlInfoBlk_p == NULL )
        {
            /*if ( tSsnCtlBlk_p->actFlg == MC_PSV_FLAG )
            {
                tSsnCtlBlk_p->ssnCtlLock.UnLock();
                printf("--2--Et_sys_AppRec::tSsnCtlBlk_p->ssnCtlLock.UnLock()\r\n");
            }*/
            continue;
        }

        tMaybeFunSet[tIndex].usingMode = tChnlInfoBlk_p->ssnRlMode;

        u_int* tNxtStepNo_p = NULL;
        tNxtStepNo_p = ( u_int* ) &tSsnCtlBlk_p->drvStepNo; //tSsnO bj_p->getDrvStep(tSsnCtlBlk_p);//tNxtStepNo_p=0xFF no data load
        T_chkFun tChkFun_F;
        tChkFun_F = ( T_chkFun ) GetProcAddress ( *tModule_p, "chkFun" ); //���޸�Ϊwin,lnxͨ����ʽ

        tRtn = tChkFun_F ( tChnlInfoBlk_p->funRl_p, tRtFunRl_p, &tMaybeFunSet[tIndex], tSsnPara_p, tNxtStepNo_p );

        if ( tRtn < 0 )
        {
            cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "tChkFun_F tRtn = %d\r\n", tRtn );
        }

        tSsnCtlBlk_p->checkState = tRtn;//method?

        //tSsnCtlBlk_p->actSsnRprtInfo = &(snnInfo.ssnRslt):*/
        //update ssn step result step
        //tRtn:
        //0x0: start step, finish step succ
        //>0x0: step No, allway succ executed
        //<0x0:ssn err! start Step err = -255, other step err = 0-s tepNo
        if ( ( u_char ) tMaybeFunSet[tIndex].act_prio_chnlID == MC_PSV_FLAG ) //passive ssn
        {
            chnlInfoBlk* tChnlInfoBlk_p = tSsnCtlBlk_p->myChnlInfoBlk_p;
            tSsnObj_p->setPsvResInfo ( tChnlInfoBlk_p, &tRtn ); // rtn is int
        }
        else
            //active ssn
        {
            tSsnObj_p->setResultInfo ( tSsnCtlBlk_p, tRtn );    // rtn is int
        }

        //step 10.���ͻỰ����֪ͨ
        if ( tSsnCtlBlk_p->actFlg == MC_ACT_FLAG )
        {
            tSsnCtlBlk_p->rtRunStOrRslt = MC_SNNST_RDY;
        }

        cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_1, "    Et_sys_AppRec:: tSsnCtlBlk_p->rtRunStOrRslt = %d\r\n", tSsnCtlBlk_p->rtRunStOrRslt );
        cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_1, "    Et_sys_AppRec::ssnDuQ.pshQ\r\n" );
        ssnDuQ.pshQ ( SCD_GROUP, 0, NULL, 0 ); //send ready inform
        //nsCPPub::SleepMilliSecond ( 100 );
        printf (
        "Et_sys_AppRec:: ssnDuQ.pshQ end chnl=%d, ssnID=%d, checkstate=%d \r\n",
        tChnlID, tSsnID, tRtn );

        continue;
    }

    delete[]tASDUInfoCp.ASDU_p;
    return;
}

void Tsys_Usr ( void* pParam )
{
    chnlObjCfg* chnlCfg = ( chnlObjCfg* ) pParam;
    ptclSnnKey ssn;
    //int iTid = nsCPThread::GetThreadID();
    //printf("start reqSnn tid=%d \r\n", iTid);
    // do requst as usr or sys
    ssnReqInfo Info = chnlCfg->ssnRInfo ;
    ssnReqInfo srInfo;
    srInfo.cntNum = Info.cntNum;
    srInfo.tReqInfo = Info.tReqInfo;
    srInfo.sRCnt_p = new ssnReqCnt[srInfo.cntNum]();
    memcpy ( srInfo.sRCnt_p, Info.sRCnt_p, sizeof ( ssnReqCnt ) * ( Info.cntNum ) );

    for ( int i = 0; i < srInfo.cntNum; i ++ )
    {
        srInfo.sRCnt_p[i].collMethods = true;
    }

    reqInfo tReqInfo = srInfo.tReqInfo;
    short sUsrNo = g_cUsrCnt;

    T_snnPara* snnPara_p = NULL;
    getSnnParaMem ( snnPara_p );
    char stmp[8] = { 0 };
    snnPara_p->usrData_p->gData.tmStmp_p = stmp;

    // Ҫ��ʱ��Ҫ�㹻�󣬲��ܹ���֤
    u_short timeout = MC_DEF_SSN_TM_OUT /*+ tReqInfo.chnlID*100*/; // ��λ:��(s) // �Ự��ʱʱ��
    PMSG msg_p = NULL;
    //int iCnt = 0;
    T_snnUsrData usrData[10];
    PNT_ADDR_TYPE pntAddr[10];
    double tmStmp[10];
    C_ChnlObj* tChnlObj_p = chnlMgr.getChnlObj ( tReqInfo.chnlID );
    realTimeValue* realTranCtrl_p;

    while ( 1 )
    {
        realTranCtrl_p = ( realTimeValue* ) tChnlObj_p->getRealParam();

        if ( chnlCfg->role == MC_CTLED_STN && realTranCtrl_p->stage !=  SLAVE_STARTED )
        {
            nsCPPub::SleepSecond ( 3 );
            continue;
        }

        for ( int iCnt = 0; iCnt <  srInfo.cntNum ; iCnt ++ )
        {
            // ÿ��ͨ������ I ֡����ǰ��Ҫ �����Ӻ�START
            if ( srInfo.sRCnt_p[iCnt].collMethods != true )
            {
                continue;
            }

            snnPara_p->stepNo = 0;
            snnPara_p->infoType = 0;
            snnPara_p->stnAcsFlag = 0;
            snnPara_p->actFlag = MC_ACT_FLAG;
            snnPara_p->ASDUIDNum = 0;
            snnPara_p->COT = 0;
            snnPara_p->cmnAddr = 0;
            snnPara_p->acsMthd = 0;
            snnPara_p->pntNum = 0;
            snnPara_p->pntPos = 0;
            snnPara_p->tmStmpType = 0;
            snnPara_p->pntStorNum = 0;
            snnPara_p->pntLoadNum = 0;
            snnPara_p->usrMaxIndex = 0;
            u_char ucRet = 0;
            ucRet = reqSnn ( &tReqInfo, srInfo.sRCnt_p[iCnt], snnPara_p/*g_snnPara_p*/, timeout, msg_p );

            if ( ucRet != 0 )
            {
                cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "[ERR]Tsys_Usr::reqSnn() is failed! ret=%d \r\n", ucRet );
                return;
            }

            //printf(
            //  "Tsys_Usr(%d)::reqSnn() success! ptclNo=%d, snnDscID=%d, actFlag=%d, chnl=%d\r\n",
            //  0, ssn.ptclNo, ssn.snnDscNo, snnPara_p->actFlag, tReqInfo.chnlID);
            //tReqInfo.role++;
            snnPara_p->usrData_p->gData.tmStmpType = 0;
            nsCPPub::SleepSecond ( srInfo.sRCnt_p[iCnt].cycTm );
            srInfo.sRCnt_p[iCnt].collMethods = Info.sRCnt_p[iCnt].collMethods;
            // iCnt = ( ( iCnt + 1 ) % srInfo.cntNum );
            //nsCPPub::SleepSecond ( 3 );
        }

        //������������������
        for ( u_int i = 0; i <  chnlCfg->phyTypeNum; i++ )
        {
            recvDataBuf[tReqInfo.chnlID].datalock.Lock();
            char tDIdBinStr_a[MC_DIDBIN_LEN] = {0};
            T_phyValCfg dtCfg ;
            memset ( &dtCfg, 0, sizeof ( T_phyValCfg ) );
            T_phyNCfg   dataCfg;//��ǰͨ������������
            memset ( &dataCfg, 0, sizeof ( T_phyNCfg ) );
            T_phyObjVal daVal;
            memset ( &daVal, 0, sizeof ( T_phyObjVal ) );
            T_phyNVal   dtValue;//Ҫȡ��ͨ��������������
            double phyDtValue = 0;
            memset ( &dtValue, 0, sizeof ( T_phyNVal ) );
            dtValue.phyObjVal_p = &daVal;
            dtValue.phyUnitNum = 1;
            dataCfg.phyValCfg_p = &dtCfg;
            dataCfg.phyUnitNum = 1;
            dtCfg.phyType = chnlCfg->phyType_p[i];
            dtCfg.dataId_p = tDIdBinStr_a;
            dtCfg.pntAddr_p = pntAddr;
            memset ( dtCfg.pntAddr_p, 0, ( sizeof ( PNT_ADDR_TYPE ) * 10 ) );
            daVal.pntObj_p = NULL;
            daVal.phyObjVal_p = &phyDtValue;
            daVal.phyType = chnlCfg->phyType_p[i];
            //�ҵ���Ӧ��������Ӧ�ĵ��ַ
            int ret =  unitInfData.getM ( tReqInfo.chnlID,
                                          tReqInfo.chnlID,
                                          &dtValue,
                                          &dataCfg );

            if ( ret == 0 )
            {
                memset ( usrData, 0, sizeof ( T_snnUsrData ) * 10 );

                for ( int k = 0; k < 10 ; k++ )
                {
                    usrData[k].gData.tmStmp_p = &tmStmp[k];
                    usrData[k].fData.tmStmp_p = &tmStmp[k];
                }

                for ( u_int j = 0; j < dtCfg.numOfAdd ; j++ )
                {
                    // usrData[j] = *findData ( dtCfg.pntAddr_p[j], & ( recvDataBuf[tReqInfo.chnlID] ) );
                    memcpy ( & ( usrData[j] ),
                             findData ( dtCfg.pntAddr_p[j], & ( recvDataBuf[tReqInfo.chnlID] ) ),
                             sizeof ( T_snnUsrData ) );
                }

                daVal.pntObj_p = usrData;
                calc.calcFuncAddToVal ( usrData, &dtCfg, &daVal );
                unitInfData.setM ( tReqInfo.chnlID, &dtValue );

            }

            recvDataBuf[tReqInfo.chnlID].datalock.UnLock();
            //convPhyToSndChn ( &unitInfData, tReqInfo.chnlID, 1, & ( dtCfg.phyType ), 1 );
        }//for

        //nsCPPub::SleepSecond ( 10 );
        //nsCPPub::SleepMilliSecond ( 50 );
        //nsCPPub::SleepSecond ( 1000 );
    }

    if ( msg_p != NULL && msg_p != ( PMSG ) 0xFFFF )
    {
        delete msg_p;
        msg_p = NULL;
    }

    if ( bufData_p != NULL )
    {
        if ( bufData_p[tReqInfo.chnlID].usrdata_p->data_Id_p != NULL )
        {
            delete[] bufData_p->usrdata_p->data_Id_p;
        }

        if ( bufData_p[tReqInfo.chnlID].usrdata_p->data_p != NULL )
        {
            delete[] bufData_p->usrdata_p->data_p;
        }

        if ( bufData_p[tReqInfo.chnlID].MCAS.accumValue_p != NULL )
        {
            delete[]bufData_p[tReqInfo.chnlID].MCAS.accumValue_p;
        }

        if ( bufData_p[tReqInfo.chnlID].MCAS.addrStart_p != NULL )
        {
            delete[]bufData_p[tReqInfo.chnlID].MCAS.addrStart_p;
        }

        if ( bufData_p[tReqInfo.chnlID].MCAS.funCode_p != NULL )
        {
            delete[]bufData_p[tReqInfo.chnlID].MCAS.funCode_p;
        }
    }

    if ( srInfo.sRCnt_p != NULL )
    {
        delete[] srInfo.sRCnt_p;
    }

    if ( srInfo.sRCnt_p != NULL )
    {
        delete[]srInfo.sRCnt_p;
    }

    return;
}

void Tsys_DtPross ( void* pParam )
{
    int iRet = 0;

    countManage  tCtManage;
    T_phyValCfg  outData[256];
    T_snnUsrData usrData[10];
    double       tmStmp[10];
    PMSG msg_p = new MSG;

    if ( msg_p == NULL )
    {
        return;
    }

    memset ( outData, 0, sizeof ( outData ) );
    memset ( usrData, 0, sizeof ( usrData ) );
    memset ( tmStmp, 0, sizeof ( tmStmp ) );
    //����did�����ƴ��ռ�
    char tDIdBinStr_aa[256][MC_DIDBIN_LEN];
    memset ( tDIdBinStr_aa, 0, sizeof ( tDIdBinStr_aa ) );

    for ( u_int i = 0; i < 255; i++ )
    {
        outData[i].dataId_p = tDIdBinStr_aa[i];
    }

#ifdef WINDOWS_PLATFORM
    PeekMessage ( msg_p, NULL, WM_USER, WM_USER, PM_NOREMOVE );

    T_TID tID = ( T_TID ) getThread();
    g_errTID = tID;
#else

    if ( ssnFnshNfyID == 0 )
    {
        int qid = initSsnAsynRsrc ( msg_p );

        if ( qid == -1 )
        {
            return exit ( 1 );
        }
        else
        {
            ssnFnshNfyID = qid;
        }
    }

    g_appTID = getThread();

#endif

    while ( 1 )
    {
        dataMsgP dtMsgP;
        u_int outNum;
        iRet = ssnWaitFnsh ( g_appTID, ( char* ) &dtMsgP, sizeof ( dataMsgP ), msg_p, ssnFnshNfyID ); // W:: linux param ...

        if ( iRet < 0 )
        {
            continue;
        }

        unitInfData.getPhyCfgByNPnt ( dtMsgP.chnlNo, dtMsgP.usrdataP, dtMsgP.pNum, outData, &outNum );

        for ( u_int i = 0; i < outNum; i++ )
        {
            recvDataBuf[dtMsgP.chnlNo].datalock.Lock();
            T_phyObjVal daVal;
            memset ( &daVal, 0, sizeof ( T_phyObjVal ) );
            T_phyNVal   dtValue;//Ҫȡ��ͨ��������������
            double phyDtValue;
            memset ( &dtValue, 0, sizeof ( T_phyNVal ) );
            dtValue.phyObjVal_p = &daVal;
            dtValue.phyUnitNum = 1;
            daVal.phyType = outData[i].phyType;
            daVal.phyObjVal_p = ( void* ) &phyDtValue;
            int ret = unitInfData.getM ( dtMsgP.chnlNo,
                                         dtMsgP.chnlNo,
                                         &dtValue,
                                         NULL );

            if ( ret == 0 )
            {
                memset ( usrData, 0, sizeof ( T_snnUsrData ) * 10 );

                for ( u_int j = 0; j < outData[i].numOfAdd ; j++ )
                {
                    //usrData[j] = *findData ( outData[i].pntAddr_p[j], & ( recvDataBuf[dtMsgP.chnlNo] ) );
                    memcpy ( & ( usrData[j] ),
                             findData ( outData[i].pntAddr_p[j], & ( recvDataBuf[dtMsgP.chnlNo] ) ),
                             sizeof ( T_snnUsrData ) );
                }

                calc.calcFuncAddToVal ( usrData, & ( outData[i] ), &daVal );
                daVal.pntObj_p = usrData;
                unitInfData.setM ( dtMsgP.chnlNo, &dtValue );
            }

            recvDataBuf[dtMsgP.chnlNo].datalock.UnLock();
        }
    }
}
//#ifndef WINDOWS_PLATFORM
void Et_sys_DataPutStor ( void* pParam )
{
    nsCPPub::SleepSecond ( 10 );
    int ret                      = 0;
    int srcAppCode               = 3;
    int dstAppCode               = 3;
    int commType                 = 4;
    u_char controlCode           = 1;
    sock_descriptor sock         = 0;
    DATEFORMAT_T    startTime    = { 0 };
    DATEFORMAT_T    endTime      = { 0 };
    DATEFORMAT_T    forcastTime  = { 0 };
    unsigned int    step         = 0;
    //const int       timeOut      = 2;
    const int tSndTmOut = 50000;                        //���ͳ�ʱʱ��
    const int tRcvTmOut = 10000;                        //���ճ�ʱʱ��
    char            ip[32]       = "";
    memcpy ( ip, cfgMng->getDataBaseIP()->rmtDataInIp.ipAddr, MC_IP_LEN );
    int  port = cfgMng->getDataBaseIP()->rmtDataInIp.ipPort;
    char m_ip [32]             = "";
    memcpy ( m_ip, cfgMng->getDataBaseIP()->lcalDataInIp.ipAddr, MC_IP_LEN );
    int localport = cfgMng->getDataBaseIP()->lcalDataInIp.ipPort;
    //sockCommActive ins ( ip, port, m_ip, localport, timeOut );
    cIface tSQLMeteor ( 1 );
    tSQLMeteor.istlAppChnl ( 0, m_ip, localport );
    ret = tSQLMeteor.setAppChnl ( 0, ip, port, tRcvTmOut, tSndTmOut, 0 );
	bool vBusFlag =false;
    if ( ret != 0 )
    {
        cfgMng->tryLog.LPrintf ( DEBUG,
                                 DEBUG_LEVEL_0,
                                 "setAppChnl error %d\r\n"
                               );
    }

    time_t time_p;
    struct tm p =
    { 0 };
    struct tm newTm;
    DBuffer ids [TIMER_PUT_DB_NUM][MC_DEF_STN_NUM];
    DBuffer idsAvg [TIMER_PUT_DB_NUM][MC_DEF_STN_NUM];
    DBuffer idsDbg;

    //��ӡid buffer
    //char dbgBuf[1024 * 32];

    char* tDbgBuf_p = new char[MC_DBBUF_MAX];

    if ( tDbgBuf_p == NULL )
    {
        cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "%s|%s|%d: tDbgBuf_p new return null\r\n",
                                 __FILE__, __func__, __LINE__ );
        return;
    }

    memset ( tDbgBuf_p, 0, MC_DBBUF_MAX );

    DID didValid;
    //DATA_ID dataTmId;
    bool arrival;
    //ret = ins.sockInitActive();
    //sleep(60);
    //time(&time_p);
    //p = *(localtime(&time_p));
    int datavalue = 0;
    //���Դ�ӡ��ǰʱ��
    struct timeval tm;
    char tmlog[256];

    u_char minTimer = 1;

    /*
    tmChnl* tmHead_p;
    tmCfg tTmrCfg;
    char tTmrNm[20];
    tTmrCfg.ID = 0;//lzq, 2017.1.25
    sprintf ( tTmrNm, "act ssnID:%d\n",  );
    tTmrCfg.name_p         = tTmrNm;
    tTmrCfg.tmFunHdl       = minTimerFun_p;
    tTmrCfg.tmFunHdlPara_p = ( void* ) &minTimer ;
    tTmrCfg.type           = 0;//once
    tTmrCfg.fstXpirTm      = minTimer;

    // Ӧ�ü���ʱ��������Ӧ�ã������뵽ͨ����ʱ���У�20170601��by zhaojj
    tmHead[i] = tmFclty.ctrAppTmChnl ( i, &tTmrCfg );

    PMSG msg_p = new MSG;

    if ( msg_p == NULL )
    {
        return;
    }

    #ifdef WINDOWS_PLATFORM
    PeekMessage ( msg_p, NULL, WM_USER, WM_USER, PM_NOREMOVE );

    T_TID tID = ( T_TID ) getThread();
    g_putStorTID = tID;
    #else

    if ( ssnFnshNfyID == 0 )
    {
        int qid = initSsnAsynRsrc ( msg_p );

        if ( qid == -1 )
        {
            return exit ( 1 );
        }
        else
        {
            ssnFnshNfyID = qid;
        }
    }

    //t_Tid = getThread();
    //g_errTID = (T_TID*)&t_Tid;
    g_putStorTID = getThread();

    #endif
    */

    int phyCatName_p[TIMER_PUT_DB_NUM];
    memset ( phyCatName_p, 0, sizeof ( phyCatName_p ) );
    time_t tvSec    = 0;
    time_t tvMin1   = 0;
    time_t tvMin5   = 0;
    time_t tvMin15  = 0;
    const int tOneMinSec = 60; //1����=60��
    const int tFivMinSec = 300;//5����=300��
    const int tFifMinSec = 900;//15����=900��

    tvSec = time ( NULL );
    tvSec = tvSec + MC_TMSTMP_OFFSET;//����ʱ��ƫ��

    //��ǰʱ���Ӧ�ĳ�ʼֵ
    tvMin1   = tvSec / tOneMinSec;
    tvMin5   = tvSec / tFivMinSec;
    tvMin15  = tvSec / tFifMinSec;

    while ( 1 )
    {
        /*if ( ret < 0 )
        {
            cfgMng->tryLog.LPrintf ( DEBUG, "sockInitActive error %d\n", sock );

            if ( 0 == ( ret = ins.sockInitActive() ) )
            {
                cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_1,"sockInitActive OK\n" );
            }
            else
            {
                ins.sockCommEnd();
            }

            nsCPPub::SleepSecond  ( 10 );
            continue;
        }

        arrival = true;*/
        /***************************************************/

        nsCPPub::SleepSecond ( 1 );
        tvSec = time ( NULL );
        tvSec = tvSec + MC_TMSTMP_OFFSET;//����ʱ��ƫ��

        DATAFORMAT_DATE7B1 lcalTmStmp = {0};

        //�ж�������
        if ( ( tvSec / tOneMinSec ) != tvMin1 )
        {
            tvMin1 = tvSec / tOneMinSec;
            phyCatName_p[0] = 1;
            cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_1, "1 minute data !\r\n" );
            //ȡ�ñ���ʱ��
            getSysTime ( lcalTmStmp, tvSec );
            //debug ��� ����
            cfgMng->tryLog.LPrintf ( DEBUG,
                                     DEBUG_LEVEL_1,
                                     "lcalTmStmp=[%d/%d/%d %d:%d:%d] !\r\n",
                                     lcalTmStmp.year, lcalTmStmp.month,
                                     lcalTmStmp.day, lcalTmStmp.hour,
                                     lcalTmStmp.minute, lcalTmStmp.second
                                   );
        }
        else //����������
        {
            phyCatName_p[0] = 0;
        }

        //�ж���5����
        if ( ( tvSec / tFivMinSec ) != tvMin5 )
        {
            tvMin5 = tvSec / tFivMinSec;
            phyCatName_p[1] = 5;
            cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_1, "5 minute data !\r\n" );
        }
        else //������5����
        {
            phyCatName_p[1] = 0;
        }

        //�ж���15����
        if ( ( tvSec / tFifMinSec ) != tvMin15 )
        {
            tvMin15 = tvSec / tFifMinSec;
            phyCatName_p[2] = 15;
            cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "15 minute data !\r\n" );
        }
        else //������15����
        {
            phyCatName_p[2] = 0;
        }

		vBusFlag = false;
        //�鿴���������ʱ��Ҫ�������
        for ( int timerNum = 0; timerNum < TIMER_PUT_DB_NUM; timerNum++ )
        {

            if ( phyCatName_p[timerNum] <= 0 )
            {
                //��û�е���Ӧ�����������ݴ���ʱ��
                continue;
            }

            /************phyCatNameArr[timerNum]���ӵĴ���************************/
            for ( int i = 0 ; i < MC_DEF_STN_NUM ; i++ )
            {
                ids[timerNum][i].Reset();
                idsAvg[timerNum][i].Reset();
                chnlObjCfg* chnlCfg = & ( cfgMng->getchnlObjCfg() [i] );

                if ( chnlCfg->putStagFlag != 1 && chnlCfg->putStagFlag != 3 )
                {
                    continue;
                }

                //��¼ͨ��ʱ��
                //ret = unitInfData.setChnTmStmp ( i, &lcalTmStmp );

                /*if ( ret != 0 )
                {
                    cfgMng->tryLog.LPrintf  ( DEBUG,
                                              DEBUG_LEVEL_0,
                                              " [chnl%d],unitInfData.setChnTmStmp return[%d]\r\n",
                                              i, ret );
                }*/

                u_int buflng = 0 ;

                const  u_int* typeBuf_p = unitInfData.getPhyCatBuf ( i,
                                          phyCatName_p[timerNum],
                                          buflng
                                                                   );

                for ( u_int j = 0; j < buflng; j++ )
                {
                    u_char dTLng = 0;

                    if ( ( ret = unitInfData.getPhyValLng ( i,
                                                            & ( typeBuf_p[j] ),
                                                            1,
                                                            &dTLng )
                         ) != 0
                       )
                    {
                        cfgMng->tryLog.LPrintf ( DEBUG,
                                                 DEBUG_LEVEL_0,
                                                 "Failed to obtain data length,ret=[%d] !", ret );
                        continue;
                    }

                    if ( dTLng == 0 )
                    {
                        continue;
                    }

                    char tDIdBinStr_a[MC_DIDBIN_LEN] = {0};
                    DID dataIdObj;
                    T_phyValCfg dtCfg ;
                    memset ( &dtCfg, 0, sizeof ( T_phyValCfg ) );
                    dtCfg.phyType = typeBuf_p[j];
                    dtCfg.dataId_p = tDIdBinStr_a;
                    dtCfg.pntAddr_p = NULL;

                    T_phyNCfg   dataCfg;//��ǰͨ������������
                    memset ( &dataCfg, 0, sizeof ( T_phyNCfg ) );
                    dataCfg.phyValCfg_p = &dtCfg;
                    dataCfg.phyUnitNum = 1;

                    T_phyObjVal daVal;
                    double phyDtValue = 0;
                    memset ( &daVal, 0, sizeof ( T_phyObjVal ) );
                    daVal.pntObj_p = NULL;
                    daVal.phyObjVal_p = &phyDtValue;
                    daVal.phyType = typeBuf_p[j];

                    T_phyNVal   dtValue;//Ҫȡ��ͨ��������������
                    memset ( &dtValue, 0, sizeof ( T_phyNVal ) );
                    dtValue.phyObjVal_p = &daVal;
                    dtValue.phyUnitNum = 1;

                    if ( ( ret = unitInfData.getM ( i, i, &dtValue, &dataCfg )
                         ) != 0
                       )
                    {
                        continue;
                    }

                    dataIdObj.Import ( tDIdBinStr_a );

                    if ( dataIdObj.IsValid() )
                    {
                        /*
                        if ( dataId.FieldGet ( IDFIELD_KIND ) == DATAKIND_AVG )
                        {
                            if ( daVal.acmTimes == 0 )
                            {
                                cfgMng->didLog.LPrintf ( true, DEBUG_LEVEL_1, " [chnl%d],AVG Cannot be zero %d\r\n", i, daVal.phyType );
                            }

                            double val = phyDtValue / daVal.acmTimes;
                            daVal.acmTimes = 0;
                            phyDtValue = 0;
                            idsAvg[timerNum][i].Build ( dataId.IdData(), ( char* ) &val );
                            unitInfData.setM ( i, &dtValue );
                        }
                        else
                        {
                            ids[timerNum][i].Build ( dataId.IdData(), ( char* ) &phyDtValue );
                        }
                        */
                        if ( dataIdObj.FieldGet ( IDFIELD_KIND ) == DATAKIND_AVG )
                        {
                            double val = 0;

                            if ( daVal.acmTimes == 0 )
                            {
                                cfgMng->tryLog.LPrintf  ( DEBUG,
                                                          DEBUG_LEVEL_1,
                                                          " [chnl%d],AVG Cannot be zero %d\r\n",
                                                          i, daVal.phyType );
                            }
                            else
                            {
                                val = phyDtValue / daVal.acmTimes;
                            }

                            daVal.acmTimes = 1;
                            phyDtValue = 0;
                            ids[timerNum][i].Build ( dataIdObj.IdData(), ( char* ) &val );

                            ret = unitInfData.setM ( i, &dtValue );

                            if ( ret != 0 )
                            {
                                cfgMng->tryLog.LPrintf  ( DEBUG,
                                                          DEBUG_LEVEL_0,
                                                          " chnl[%d],phyType[%d],unitInfData.setM return [%d]\r\n",
                                                          i, daVal.phyType, ret );
                            }
							vBusFlag = true;
                        }
                        else
                        {
                            ids[timerNum][i].Build ( dataIdObj.IdData(), ( char* ) &phyDtValue );
							vBusFlag = true;
                        }
                    }

                }

            }
        }

        //vbus ���Ȩ��
        if( g_vbus_p != NULL && vBusFlag == true)
        {
            cfgMng->tryLog.LPrintf  ( DEBUG,
                DEBUG_LEVEL_1,
                "%s|%s|%d:g_vbus_p->getInteractiveAuth start...\r\n",
                __FILE__, __func__, __LINE__);

            ret = g_vbus_p->getInteractiveAuth(IvBus::PERIODIC);
            if ( ret != 0 )
            {
                
                cfgMng->tryLog.LPrintf  ( DEBUG,
                    DEBUG_LEVEL_0,
                    "%s|%s|%d:g_vbus_p->getInteractiveAuth [%d]\r\n",
                    __FILE__, __func__, __LINE__,ret );
                //continue;
            }
			else
			{
				cfgMng->tryLog.LPrintf(DEBUG,
					DEBUG_LEVEL_1,
					"%s|%s|%d:g_vbus_p->getInteractiveAuth Success!\r\n",
					__FILE__, __func__, __LINE__);
			}
            
        }
       
 
        for ( int timerNum = 0; timerNum < TIMER_PUT_DB_NUM; timerNum++ )
        {

            if ( phyCatName_p[timerNum] <= 0 )
            {
                //��û�е���Ӧ�����������ݴ���ʱ��
                continue;
            }


            for ( int i = 0 ; i < MC_DEF_STN_NUM ; i++ )
            {
                chnlObjCfg* chnlCfg = & ( cfgMng->getchnlObjCfg() [i] );

                if ( chnlCfg->putStagFlag != 1 && chnlCfg->putStagFlag != 3 )
                {
                    continue;
                }

                int tIDbufLng = ids[timerNum][i].Length();
                if( tIDbufLng <=0 )//ids buffer��û��ֵ
                {
                    continue;
                }

                DID dataTmIdObj;
                char tDIdBinStr_a[MC_DIDBIN_LEN] = {0};
                DATAFORMAT_DATE7B1 dtTime = {0};
                T_untChnTmStmp stmp = {0};

                stmp.tmStmp_p  = &dtTime;
                stmp.dataId_p = &tDIdBinStr_a;
                unitInfData.getChnTmStmp ( i, &stmp );
                dtTime.second = 0;

                dataTmIdObj.Import ( tDIdBinStr_a );

                if ( dataTmIdObj.IsValid() )
                {
                    //Ĩȥ��ֵ
                    lcalTmStmp.second = 0;
                    ids[timerNum][i].Build (  dataTmIdObj.IdData(),
                                              ( char* ) &lcalTmStmp,
                                              sizeof ( DATAFORMAT_DATE7B1 )
                                           );
                }

                memset ( tDbgBuf_p, 0, MC_DBBUF_MAX );
                idsDbg.Diagnose ( ids[timerNum][i].Pointer(),
                                  ids[timerNum][i].Length(),
                                  true,
                                  tDbgBuf_p,
                                  MC_DBBUF_MAX
                                );

                cfgMng->didLog.LPrintf ( false,
                                         DEBUG_LEVEL_1,
                                         " fuskTesttIDbufLng=[%d],[chnl%d],%s\r\n",
                                         tIDbufLng,i, tDbgBuf_p );
                /* ret = ins.sockRequestAndResult ( ids[timerNum][i].Pointer(),
                                                  ids[timerNum][i].Length(),
                                                  NULL,
                                                  NULL,
                                                  controlCode,
                                                  srcAppCode,
                                                  dstAppCode,
                                                  commType,
                                                  startTime,
                                                  endTime,
                                                  step,
                                                  forcastTime
                                                );*/

                ret = tSQLMeteor.SQLInsrtDBSsn ( ( u_char* ) ids[timerNum][i].Pointer(),
                                                 ids[timerNum][i].Length(),
                                                 tRcvTmOut,
                                                 tSndTmOut,
                                                 1
                                               );

                if ( ret != 0 )
                {
                    //ins.sockCommEnd();
                    cfgMng->tryLog.LPrintf ( DEBUG,
                                             DEBUG_LEVEL_0,
                                             "sockCommunicationActive error %d, "
                                             "tIDbufLng=[%d],[chnl%d],Length=[%d],tDbgBuf_p=[%s]\r\n",
                                             ret,tIDbufLng, i, ids[timerNum][i].Length(),
                                             tDbgBuf_p
                                           );
                }

                /*
                dtTime.second = 0;
                idsAvg[timerNum][i].Build ( dataTmId.IdData(),
                                            ( char* ) stmp.tmStmp_p,
                                            sizeof ( DATAFORMAT_DATE7B1 ) );
                idsDbg.Diagnose ( idsAvg[timerNum][i].Pointer(), idsAvg[timerNum][i].Length(), true, dbgBuf, 1024 * 32 );
                cfgMng->didLog.LPrintf ( false, DEBUG_LEVEL_1, " [chnl%d],%s\r\n", i, dbgBuf );
                ret = ins.sockRequestAndResult ( idsAvg[timerNum][i].Pointer(), idsAvg[timerNum][i].Length(), NULL, NULL,
                                                 controlCode, srcAppCode, dstAppCode, commType,
                                                 startTime, endTime, step, forcastTime );



                printf ( "%s\r\n", dbgBuf );

                if ( ret < 0 )
                {
                    ins.sockCommEnd();
                    cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "sockCommunicationActive error %d\n", ret );
                }
                */
                //nsCPPub::SleepMilliSecond ( 300 );
            }
        }

        /************************
        time ( &time_p );
        p = * ( localtime ( &time_p ) );

        //else
        //{
        //printf("sockCommunicationActive OK\n");
        while ( arrival )
        {
            time ( &time_p );
            newTm = * ( localtime ( &time_p ) );

            //if (((mktime(&newTm)) - (mktime(&p)))%60 == 0)
            if ( ( ( mktime ( &newTm ) ) - ( mktime ( &p ) ) ) >= 60 )
            {
                arrival = false;
            }
            else
            {
                nsCPPub::SleepSecond ( 2 );
                continue;
            }
        }
        ***************************/
        //}

    }

    //ins.sockCommEnd();
}
//#endif

void Et_sys_DataOutStor ( void* pParam )
{
	bool vBusFlag = false;
    int ret                      = 0;
    int srcAppCode               = 3;
    int dstAppCode               = 3;
    int commType                 = 4;
    u_char controlCode           = 2;
    sock_descriptor sock         = 0;
    DATEFORMAT_T    startTime    = { 0 };
    DATEFORMAT_T    endTime      = { 0 };
    DATEFORMAT_T    forcastTime  = { 0 };
    unsigned int    step         = 0;
    pDBADFD_T reqPktDF;
    memset ( &reqPktDF, 0, sizeof ( pDBADFD_T ) );
    //const int       timeOut      = 2;
    const int tSndTmOut          = 50000;                       //���ͳ�ʱʱ��
    const int tRcvTmOut          = 10000;                       //���ճ�ʱʱ��
    char            ip[32]       = "";
    memcpy ( ip,
             cfgMng->getDataBaseIP()->rmtDataOutIp.ipAddr,
             MC_IP_LEN
           );
    int  port = cfgMng->getDataBaseIP()->rmtDataOutIp.ipPort;

    char m_ip [32]             = "";
    memcpy ( m_ip,
             cfgMng->getDataBaseIP()->lcalDataOutIp.ipAddr,
             MC_IP_LEN
           );
    int localport = cfgMng->getDataBaseIP()->lcalDataOutIp.ipPort;
    //sockCommActive ins ( ip, port, m_ip, localport, timeOut );
    cIface tSQLMeteor ( 1 );
    tSQLMeteor.istlAppChnl ( 0, m_ip, localport );

    ret = tSQLMeteor.setAppChnl ( 0, ip, port, tRcvTmOut, tSndTmOut, 0 );

    if ( ret != 0 )
    {
        cfgMng->tryLog.LPrintf ( DEBUG,
                                 DEBUG_LEVEL_0,
                                 "setAppChnl error %d\r\n"
                               );
    }

    DBuffer ids[MC_DEF_STN_NUM];
    char  rcvBuf[ 1024 * 32 ] = { 0 };
    d_data* rcvData = NULL;
    int     rcvNum  = 0;
    //char dbgBuf[1024 * 32] = { 0 };
    char* tDbgBuf_p = new char[MC_DBBUF_MAX];

    if ( tDbgBuf_p == NULL )
    {
        cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "%s|%s|%d: tDbgBuf_p new return null\r\n",
                                 __FILE__, __func__, __LINE__ );
        return;
    }

    memset ( tDbgBuf_p, 0, MC_DBBUF_MAX );

    time_t tvSec    = 0;
    time_t tvMin1   = 0;
    const int tOneMinSec = 60; //1����=60��
    tvSec = time ( NULL );
    tvSec = tvSec + MC_TMSTMP_OUTOFFSET;//����ʱ��ƫ��
    //��ǰʱ���Ӧ�ĳ�ʼֵ
    tvMin1   = tvSec / tOneMinSec;

    DBuffer tDiaBuffer;//����Diagnose���д�ӡ��Ϣ�����DBuffer

    while ( 1 )
    {
        nsCPPub::SleepSecond ( 1 );
        tvSec = time ( NULL );
        tvSec = tvSec + MC_TMSTMP_OUTOFFSET;//����ʱ��ƫ��

        DATAFORMAT_DATE7B1 lcalTmStmp = {0};

        //�ж�������
        if ( ( tvSec / tOneMinSec ) != tvMin1 )
        {
            tvMin1 = tvSec / tOneMinSec;
            cfgMng->tryLog.LPrintf ( DEBUG,
                                     DEBUG_LEVEL_1,
                                     "Et_sys_DataOutStor 1 minute out database !\r\n" );
            //ȡ�ñ���ʱ��
            getSysTime ( lcalTmStmp, tvSec );
            //debug ��� ����
            cfgMng->tryLog.LPrintf ( DEBUG,
                                     DEBUG_LEVEL_1,
                                     "Et_sys_DataOutStorlcalTmStmp=[%d/%d/%d %d:%d:%d] !\r\n",
                                     lcalTmStmp.year, lcalTmStmp.month,
                                     lcalTmStmp.day, lcalTmStmp.hour,
                                     lcalTmStmp.minute, lcalTmStmp.second
                                   );
            //ȡ���������ݵ�ʱ��
            DValue::SetDateMax(&(reqPktDF.dStartTm), &(reqPktDF.dEndTm));

            ////����ʼʱ�丳ֵ
            //reqPktDF.dStartTm.year   = lcalTmStmp.year;
            //reqPktDF.dStartTm.month  = lcalTmStmp.month;
            //reqPktDF.dStartTm.day    = lcalTmStmp.day;
            //reqPktDF.dStartTm.hour   = lcalTmStmp.hour;
            //reqPktDF.dStartTm.minute = lcalTmStmp.minute;
            //reqPktDF.dStartTm.second = 0;
            ////������ʱ�丳ֵ
            //reqPktDF.dEndTm.year   = lcalTmStmp.year;
            //reqPktDF.dEndTm.month  = lcalTmStmp.month;
            //reqPktDF.dEndTm.day    = lcalTmStmp.day;
            //reqPktDF.dEndTm.hour   = lcalTmStmp.hour;
            //reqPktDF.dEndTm.minute = lcalTmStmp.minute;
            //reqPktDF.dEndTm.second = 0;
        }
        else //����������
        {
            continue;
        }

		vBusFlag = false;

        for ( int i = 0 ; i < MC_DEF_STN_NUM ; i++ )
        {
            /**********************������Ҫ�޸�*******************/
            ids[i].Reset();
            chnlObjCfg* chnlCfg = & ( cfgMng->getchnlObjCfg() [i] );

            if ( chnlCfg == NULL )
            {
                cfgMng->tryLog.LPrintf ( DEBUG,
                                         DEBUG_LEVEL_0,
                                         "%s|%s|%d: cfgMng->getchnlObjCfg() [%d] is NULL\r\n",
                                         __FILE__, __func__, __LINE__, i );
                continue;
            }

            if ( chnlCfg->putStagFlag != 2 && chnlCfg->putStagFlag != 3 )
            {
                continue;
            }

            for ( u_int j = 0; j < chnlCfg->phyTypeNum; j++ )
            {
                char dtId[MC_DIDBIN_LEN] = { 0 };
                DID dataId;
                ret = unitInfData.getPhyValADid ( i,
                                                  & ( chnlCfg->phyType_p[j] ),
                                                  1,
                                                  sizeof ( float ),
                                                  NULL,
                                                  NULL,
                                                  dtId,
                                                  NULL
                                                );

                if ( ret != 0 )
                {
                    cfgMng->tryLog.LPrintf ( DEBUG,
                                             DEBUG_LEVEL_0,
                                             "%s|%s|%d: unitInfData.getPhyValADid,i=[%d],ret=[%d]\r\n",
                                             __FILE__, __func__, __LINE__, i, ret );
                    continue;
                }

                dataId.Import ( dtId );

                if ( dataId.IsValid() )
                {
                    ids[i].Build ( dataId.IdData(), NULL, 0 );
					vBusFlag = true;
                }
            }
        }
        
        //vbus ���Ȩ��
        if( g_vbus_p != NULL && vBusFlag == true)
        {
            cfgMng->tryLog.LPrintf  ( DEBUG,
                DEBUG_LEVEL_1,
                "%s|%s|%d:g_vbus_p->getInteractiveAuth start...\r\n",
                __FILE__, __func__, __LINE__);

            ret = g_vbus_p->getInteractiveAuth(IvBus::PERIODIC);
            if ( ret != 0 )
            {

                cfgMng->tryLog.LPrintf  ( DEBUG,
                    DEBUG_LEVEL_0,
                    "%s|%s|%d:g_vbus_p->getInteractiveAuth [%d]\r\n",
                    __FILE__, __func__, __LINE__,ret );
                //continue;
            }
			else
			{
				cfgMng->tryLog.LPrintf(DEBUG,
					DEBUG_LEVEL_1,
					"%s|%s|%d:g_vbus_p->getInteractiveAuth Success!\r\n",
					__FILE__, __func__, __LINE__);
			}
            
        }

        for ( int i = 0 ; i < MC_DEF_STN_NUM ; i++ )
        {
            chnlObjCfg* chnlCfg = & ( cfgMng->getchnlObjCfg() [i] );

            if ( chnlCfg->putStagFlag != 2 && chnlCfg->putStagFlag != 3 )
            {
                continue;
            }

            memset ( tDbgBuf_p, 0, MC_DBBUF_MAX );
            ret = tDiaBuffer.Diagnose ( ids[i].Pointer(),
                                        ids[i].Length(),
                                        true,
                                        tDbgBuf_p,
                                        MC_DBBUF_MAX
                                      );
            cfgMng->didLog.LPrintf ( false,
                                     DEBUG_LEVEL_1,
                                     "%s|%s|%d: [chnl%d],ret=[%d],Length=[%d],"
                                     "tDbgBuf_p=[%s]\r\n",
                                     __FILE__, __func__, __LINE__,
                                     i, ret, ids[i].Length(), tDbgBuf_p );

            int   rcvLen = 0;
            memset ( rcvBuf, 0, ( 1024 * 32 ) );
            //getNAndBTime ( endTime, startTime, 60 );
            /*ret =   ins.sockRequestAndResult ( ids[i].Pointer(),
                                               ids[i].Length(),
                                               ( u_char* ) rcvBuf,
                                               &rcvLen,
                                               controlCode,
                                               srcAppCode,
                                               dstAppCode,
                                               commType,
                                               startTime,
                                               endTime,
                                               step,
                                               forcastTime
                                             );*/
            int bufLength = ids[i].Length();
            memcpy ( rcvBuf, ids[i].Pointer(), ids[i].Length() );
            ret = tSQLMeteor.dItemSlctDBSsn ( ( unsigned char* ) rcvBuf,
                                              &bufLength,
                                              1024 * 32,
                                              &reqPktDF,
                                              tRcvTmOut,
                                              tSndTmOut,
                                              1
                                            );
            //printf ( "idsLng = %d\r\n", ids[i].Length() );

            if ( ret != 0 )
            {
                cfgMng->tryLog.LPrintf ( DEBUG,
                                         DEBUG_LEVEL_0,
                                         "%s|%s|%d: dItemSlctDBSsn,i=[%d],ret=[%d],"
                                         "Length=[%d],tDbgBuf_p=[%s]\r\n",
                                         __FILE__, __func__, __LINE__, i, ret,
                                         ids[i].Length(), tDbgBuf_p );
            }

            if ( strncmp ( rcvBuf, "no record", strlen ( "no record" ) ) == 0 )
            {
                cfgMng->tryLog.LPrintf ( DEBUG,
                                         DEBUG_LEVEL_1,
                                         "%s|%s|%d: dItemSlctDBSsn,i=[%d],rcvBuf=[%s]\r\n",
                                         __FILE__, __func__, __LINE__, i, rcvBuf );
                //printf ( "no record" );
                continue;
            }

            memset ( tDbgBuf_p, 0, MC_DBBUF_MAX );
            ret = tDiaBuffer.Diagnose ( rcvBuf,
                                        bufLength,
                                        true,
                                        tDbgBuf_p,
                                        MC_DBBUF_MAX
                                      );
            cfgMng->didLog.LPrintf ( false,
                                     DEBUG_LEVEL_1,
                                     "%s|%s|%d: [chnl%d],ret=[%d],Length=[%d],"
                                     "tDbgBuf_p=[%s]\r\n",
                                     __FILE__, __func__, __LINE__,
                                     i, ret, bufLength, tDbgBuf_p );

            //ids[i].Fetch ( rcvBuf, rcvLen, &rcvData, rcvNum, true );
            tDiaBuffer.Fetch ( rcvBuf, bufLength, &rcvData, rcvNum, true );

            for ( u_int j = 0; j < chnlCfg->phyTypeNum; j++ )
            {
                char dtId[MC_DIDBIN_LEN] = {0};
                DID dataId;

                T_phyValCfg dtCfg ;
                memset ( &dtCfg, 0, sizeof ( T_phyValCfg ) );
                dtCfg.phyType       = chnlCfg->phyType_p[j];
                dtCfg.dataId_p      = dtId;
                dtCfg.pntAddr_p     = NULL;

                T_phyNCfg   dataCfg;//��ǰͨ������������
                memset ( &dataCfg, 0, sizeof ( T_phyNCfg ) );
                dataCfg.phyValCfg_p = &dtCfg;
                dataCfg.phyUnitNum  = 1;

                T_snnUsrData usrDt[4];
                memset ( usrDt, 0, sizeof ( T_snnUsrData ) * 4 );
                double tmStmp[4];
                memset ( tmStmp, 0, sizeof ( tmStmp ) );

                for ( int k = 0; k < 4; k++ )
                {
                    usrDt[k].gData.tmStmp_p = &tmStmp[k];
                    usrDt[k].fData.tmStmp_p = &tmStmp[k];
                }

                T_phyObjVal daVal;
                double dtV = 0;
                memset ( &daVal, 0, sizeof ( T_phyObjVal ) );
                daVal.pntObj_p = usrDt;
                daVal.phyType = chnlCfg->phyType_p[j];
                daVal.phyObjVal_p = &dtV;
                T_phyNVal   dtValue;//Ҫȡ��ͨ��������������
                memset ( &dtValue, 0, sizeof ( T_phyNVal ) );

                //memset ( dtCfg.pntAddr_p, 0, ( sizeof ( PNT_ADDR_TYPE ) * 10 ) );
                dtValue.phyObjVal_p = &daVal;
                dtValue.phyUnitNum = 1;
                ret = unitInfData.getM ( i,
                                         i,
                                         &dtValue,
                                         &dataCfg );

                if ( ret == 0 )
                {
                    for ( int index = 0; index < rcvNum; index++ )
                    {
                        //DID  tID ( ( char* ) ( ( rcvData + index )->id ) );

                        //if ( tID.IsEqual ( ( char* ) &dataId ) )
                        dataId.Import ( dtId );

                        if  ( dataId.IsEqual ( ( rcvData + index )->id ) )
                        {


                            if ( daVal.phyValLng >= dataId.ValueLength() )
                            {
                                memcpy ( &dtV,
                                         ( rcvData + index )->val,
                                         dataId.ValueLength()
                                       );
                                ret = calc.calcFuncValToAdd ( ( T_snnUsrData* ) ( daVal.pntObj_p ),
                                                              &dtCfg,
                                                              &daVal
                                                            );

                                if ( ret != 0 )
                                {
                                    cfgMng->tryLog.LPrintf ( DEBUG,
                                                             DEBUG_LEVEL_0,
                                                             "%s|%s|%d: calcFuncValToAdd,i=[%d],"
                                                             "index=[%d],ret=[%d]\r\n",
                                                             __FILE__, __func__, __LINE__, i, index, ret );
                                }
                            }



                            ret = unitInfData.setM ( i, &dtValue );

                            if ( ret != 0 )
                            {
                                cfgMng->tryLog.LPrintf ( DEBUG,
                                                         DEBUG_LEVEL_0,
                                                         "%s|%s|%d: unitInfData.setM,i=[%d],"
                                                         "index=[%d],ret=[%d]\r\n",
                                                         __FILE__, __func__, __LINE__,
                                                         i, index, ret );
                            }

                            ret = setData ( ( T_snnUsrData* ) ( dtValue.phyObjVal_p->pntObj_p ),
                                            i,
                                            chnlCfg->usrType
                                          );

                            if ( ret != 0 )
                            {
                                cfgMng->tryLog.LPrintf ( DEBUG,
                                                         DEBUG_LEVEL_0,
                                                         "%s|%s|%d: setData,i=[%d],"
                                                         "index=[%d],ret=[%d]\r\n",
                                                         __FILE__, __func__, __LINE__, i, index, ret );
                            }
                        }

                    }
                }
                else //ret!=0
                {
                    cfgMng->tryLog.LPrintf ( DEBUG,
                                             DEBUG_LEVEL_0,
                                             "%s|%s|%d: unitInfData.getM,i=[%d],ret=[%d]\r\n",
                                             __FILE__, __func__, __LINE__, i, ret );
                }
            }
        }
    }
}
/*****************************************************************************s*
�������߳�
�����յ�ʱ��֪ͨ����ȡ��Ϣ���ݣ�������Ϣ����������
��Ϣ�������롢ͨ���š��Ự�š������롣����
******************************************************************************/

void Et_sys_TakeErr ( void* pParam )
{
    int iRet = 0;

    PMSG msg_p = new MSG;

    if ( msg_p == NULL )
    {
        return;
    }

#ifdef WINDOWS_PLATFORM
    PeekMessage ( msg_p, NULL, WM_USER, WM_USER, PM_NOREMOVE );

    T_TID tID = ( T_TID ) getThread();
    g_errTID = tID;
#else

    if ( ssnFnshNfyID == 0 )
    {
        int qid = initSsnAsynRsrc ( msg_p );

        if ( qid == -1 )
        {
            return exit ( 1 );
        }
        else
        {
            ssnFnshNfyID = qid;
        }
    }

    //t_Tid = getThread();
    //g_errTID = (T_TID*)&t_Tid;
    g_errTID = getThread();

#endif

    T_snnPara* snnPara_p = NULL;
    iRet = getSnnParaMem ( snnPara_p );

    if ( iRet != 0 )
    {
        ;
    }

    while ( 1 )
    {
        // FORMAT: ���ͣ�CHNL_ID, CODE���ߣ�
        u_int errMsg[2] = { 0 };

        // 1 ���¼����ض��¼�; ��Ϣ����ʽ��
        // 2 ����Ϣ��

        iRet = ssnWaitFnsh ( g_errTID, ( char* ) errMsg, 2 * sizeof ( u_int ), msg_p, ssnFnshNfyID ); // W:: linux param ...

        if ( iRet < 0 )
        {
            if ( iRet == -2 )
            {
                //PeekMessage(msg_p, NULL, WM_USER, WM_USER, PM_NOREMOVE);
                //              tID = getThread();
                //              g_errTID = tID;
            }

            continue;
        }

        // 3 �������ݰ�
        u_int iCode = errMsg[1];
        u_int iChnlId = errMsg[0];
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_1,
                                 "Et_sys_TakeErr:: Et_sys_TakeErr, msg=%u %u %d \r\n", errMsg[1], errMsg[0], g_errTID );

        if ( iChnlId < 0 )
        {
            continue;
        }

        C_ChnlObj* tChnlObj_p = chnlMgr.getChnlObj ( iChnlId );

        if ( tChnlObj_p == NULL )
        {
            continue;
        }

        HMODULE* ptclMdl_p = tChnlObj_p->getPtclMdl();
        int iRole = tChnlObj_p->getRole();

        // ��ȡЭ���ID
        DLL_getPtclID getPtclID;
        getPtclID = ( DLL_getPtclID ) GetProcAddress ( *ptclMdl_p, "getPtclID" );

        if ( !getPtclID )
        {
            cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "Error: Can't not load function getPtclID()!\n" );
            return;
        }

        int iPtclID = getPtclID();

        // 4 ���ݹ�Լ�����ͣ�104������������Ӧ�������
        // �͵�1λ��λ0������1Ϊ�رն�ʱ��; �͵�2λ��λ1������1Ϊ�ر�����
        // �͵�3λ��λ2������1Ϊ����S֡; �͵�4λ��λ3������1Ϊ���Ͳ��Լ������λ������
        if ( ( iCode & 0x1 ) && ( iCode & 0x2 ) )
        {
            tmChnl* tmChnl_p = NULL;
            tChnlObj_p->getTimer ( tmChnl_p );

            for ( int itm = 0; tmChnl_p && itm < tmChnl_p->tmrNum; itm++ )
            {
                tmFclty.ctlTmr ( tmChnl_p, itm, TM_STOP );
            }

        }

        if ( ( iCode & 0x2 ) && iRole == MC_CTL_STN ) // ��վ�������½�������
        {
            realTimeValue* realParam_p = ( realTimeValue* ) ( tChnlObj_p->getRealParam() );
            memset ( realParam_p, 0, sizeof ( realTimeValue ) );
            reqInfo tReqInfo;
            tReqInfo.chnlID = iChnlId;
            tReqInfo.role = 0;
            tReqInfo.snnMaxRetryNum = 0;

            // Э������
            ssnReqCnt ssnKey;
            ssnKey.ssn.ptclNo = iPtclID; // Э���
            snnPara_p->stepNo = 0;
            snnPara_p->infoType = 0;
            snnPara_p->stnAcsFlag = 0;
            snnPara_p->actFlag = MC_ACT_FLAG;
            snnPara_p->ASDUIDNum = 0;
            snnPara_p->COT = 0;
            snnPara_p->cmnAddr = 0;
            snnPara_p->acsMthd = 0;
            snnPara_p->pntNum = 0;
            snnPara_p->pntPos = 0;
            snnPara_p->tmStmpType = 0;
            snnPara_p->pntStorNum = 0;
            snnPara_p->pntLoadNum = 0;
            snnPara_p->usrMaxIndex = 0;
            // Ҫ��ʱ��Ҫ�㹻�󣬲��ܹ���֤
            u_short timeout = 9;//0;//С��T1--15s     // ��λ:��(s) // �Ự��ʱʱ��
            int iCnt = 0;
            bool bFlag = true;

            while ( bFlag )
            {
                // ÿ��ͨ������ I ֡����ǰ��Ҫ �����Ӻ�START
                switch ( iCnt )
                {
                    case 0:
                        ssnKey.ssn.snnDscNo = U_NEWHOLD;

                        if ( iPtclID == 106 )
                        {
                            bFlag = false;
                        }

                        break;

                    case 1:
                        ssnKey.ssn.snnDscNo = U_STARTDT;
                        break;

                    default:
                        bFlag = false;
                        continue;
                }

                snnPara_p->actFlag = MC_ACT_FLAG;

                u_char ucRet = 0;
                ucRet = reqSnn ( &tReqInfo, ssnKey, snnPara_p, timeout, msg_p );

                if ( ucRet != 0 )
                {
                    cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                             "[ERR]Et_sys_TakeErr::reqSnn() is failed! ret=%d \r\n",
                                             ucRet );
                    //return;
                }

                //printf(
                //  "Et_sys_TakeErr::reqSnn() success! ptclNo=%d, snnDscID=%d, actFlag=%d, chnl=%d\r\n",
                //  ssn.ptclNo, ssn.snnDscNo, snnPara_p->actFlag, tReqInfo.chnlID);
                tReqInfo.role++;
                ++iCnt;
                snnPara_p->usrData_p->gData.tmStmpType = 0;
                nsCPPub::SleepSecond ( 1 );
            }

        } // ! if 0x04
        else if ( iCode & 0x8 )
        {
            reqInfo tReqInfo;
            tReqInfo.chnlID = iChnlId;
            tReqInfo.role = 0;
            tReqInfo.snnMaxRetryNum = 0;

            // Э������
            ssnReqCnt ssnKey;
            ssnKey.ssn.ptclNo = iPtclID; // Э���

            snnPara_p->stepNo = 0;
            snnPara_p->infoType = 0;
            snnPara_p->stnAcsFlag = 0;
            snnPara_p->actFlag = MC_ACT_FLAG;
            snnPara_p->ASDUIDNum = 0;
            snnPara_p->COT = 0;
            snnPara_p->cmnAddr = 0;
            snnPara_p->acsMthd = 0;
            snnPara_p->pntNum = 0;
            snnPara_p->pntPos = 0;
            snnPara_p->tmStmpType = 0;
            snnPara_p->pntStorNum = 0;
            snnPara_p->pntLoadNum = 0;
            snnPara_p->usrMaxIndex = 0;

            // Ҫ��ʱ��Ҫ�㹻�󣬲��ܹ���֤
            u_short timeout = 9;//0;//14;//С��T1--15s        // ��λ:��(s) // �Ự��ʱʱ��
            //
            ssnKey.ssn.snnDscNo = U_TESTFR;

            u_char ucRet = 0;
            ucRet = reqSnn ( &tReqInfo, ssnKey, snnPara_p, timeout, msg_p );

            if ( ucRet != 0 )
            {
                cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "[ERR]Et_sys_TakeErr::reqSnn() is failed! ret=%d \r\n", ucRet );
                // return;
            }
            else
            {
                ssnCtlBlk* tmpSsnCtlBlk = ssnCtnr.schdSsnObj ( /*tCurSsnObj_p*/ ); //return inst ctrl blk
                //finishBlock ( tmpSsnCtlBlk, tmpSsnCtlBlk->hostTID, tmpSsnCtlBlk->ssnNo,iChnlId, 0 );
            }

            //printf(
            //  "Et_sys_TakeErr::reqSnn() success! ptclNo=%c, snnDscID=%c, actFlag=%c, chnl=%d\r\n",
            //  ssn.ptclNo, ssn.snnDscNo, snnPara_p->actFlag, tReqInfo.chnlID);
            tReqInfo.role++;
            snnPara_p->usrData_p->gData.tmStmpType = 0;

#ifdef WINDOWS_PLATFORM
            T_TID tID = ( T_TID ) getThread();
            g_errTID = tID;
#else
            //t_Tid = getThread();
            //g_errTID = (T_TID*)&t_Tid;
            g_errTID = getThread();
#endif

        } // ! else if 0x8
        else if ( ( iCode & 0x2 ) && iRole == MC_CTLED_STN ) // ��վ�������½�������
        {
            realTimeValue* realParam_p = ( realTimeValue* ) ( tChnlObj_p->getRealParam() );
            memset ( realParam_p, 0, sizeof ( realTimeValue ) );
        }
        else
        {
            ;
        }
    } // ! while

    if ( snnPara_p )
    {
        if ( snnPara_p->ASDUID_p )
        {
            delete[] snnPara_p->ASDUID_p;
        }

        if ( snnPara_p->usrData_p )
        {
            delete[] snnPara_p->usrData_p;
        }

        if ( snnPara_p->tmStmp_p )
        {
            delete[] ( char* ) snnPara_p->tmStmp_p;
        }

        delete[] snnPara_p;
    }

    return;
}
// #ifndef WINDOWS_PLATFORM
// int DataForward(C_ChnlObj* tChnlObj_p , T_ptclFunDsc* funDsc_p , T_snnPara* ssnPar_p)
// {
//
//  int/* port = 0 ,timeOut = 0 ,*/ ret = 0 ,
//      controlCode = 21 , srcAppCode = 2 , dstAppCode = 3 , commType = 6 , dataType = 1;
//  sock_descriptor sock = 0;
//  rowData_t rowData;
//  void* structSet;
//  T_getSaveCom t_GetSaveCom_F;
//  HMODULE* commMdl_p = (HMODULE*)(tChnlObj_p->getCommMdl());//����ͨѶЭ��������
//  HMODULE* ptclMdl_p = (HMODULE*)(tChnlObj_p->getPtclMdl());//��Լ����;
//
//  t_GetSaveCom_F = (T_getSaveCom)GetProcAddress(*ptclMdl_p, "getSaveCom" );
//
//  if(t_GetSaveCom_F(funDsc_p) != 0)
//  {
//      return -1;
//  }
//      //sockCommActive ins(ip, port, timeOut);
//      sock = ins.sockInitActive();
//      if(sock < 0)
//      {
//          printf("sockInitActive error %d\n",sock);
//          return -2;
//      }
//      else
//      {
//          printf("sockInitActive OK\n");
//      }
//  //�ṹ���޸�
//  memcpy(structSet, "rrrrrrrrrrrtllllsssrrrrrttt", strlen("rrrrrrrrrrrtllllsssrrrrrttt"));
//  ret = ins.sockCommunicationActive(sock, structSet, dataType, strlen("rrrrrrrrrrrtllllsssrrrrrttt"),
//      (unsigned char)controlCode, srcAppCode, dstAppCode, commType);
//  if(ret<0)
//  {
//      printf("sockCommunicationActive error %d\n",ret);
//  }
//  else
//  {
//      printf("sockCommunicationActive OK\n");
//  }
//
//  ins.sockCommEnd(sock);
//  return 0;
// }
// #endif

T_snnUsrData* findData ( u_int addr, chnlDataBuf*   datBuff_p )
{
    u_int mem = 0;

    for ( u_int i = 0; i < ( u_int ) ( datBuff_p->arLen ); i++ )
    {

        if ( addr >= ( datBuff_p->addrIdx_p[i] ).begAddr && addr <= ( datBuff_p->addrIdx_p[i] ).endAddr )
        {
            mem = i;
            break;
        }
    }

    if ( mem == datBuff_p->arLen )
    {
        return NULL;//addrδ�ҵ�
    }

    int memst = addr - datBuff_p->addrIdx_p[mem].begAddr;


    return  & ( datBuff_p->buf_pp[mem][memst] );

}


//һ��д��������snnUsrData_p����������
int writeChnMPntObj ( const T_snnUsrData* snnUsrData_p,
                      const u_int& num,
                      chnlDataBuf*   datBuff_p
                    )
{
    if ( snnUsrData_p == NULL
         || num == 0
         || datBuff_p == NULL
       )
    {
        return -1;
    }

    u_char bugFlag = 0;
    datBuff_p->datalock.Lock();

    for ( u_int i = 0; i < num; i++ )
    {
        const T_snnUsrData* tUsrData_p = & ( snnUsrData_p[i] );
        u_int tPntAddr = tUsrData_p->gData.val_chkStd.pntAddr;
        T_snnUsrData* tWriteAdd_p = findData ( tPntAddr, datBuff_p );

        if ( tWriteAdd_p == NULL )
        {
            bugFlag = 1;
            continue;
        }

        if ( datBuff_p->tmStmpLng > 0
             && tWriteAdd_p->gData.tmStmp_p != NULL
             && tUsrData_p->gData.tmStmp_p != NULL
           )
        {
            memcpy ( tWriteAdd_p->gData.tmStmp_p,
                     tUsrData_p->gData.tmStmp_p,
                     datBuff_p->tmStmpLng
                   );
        }

        tWriteAdd_p->gData.pntType    = tUsrData_p->gData.pntType;
        tWriteAdd_p->gData.tmStmpType = tUsrData_p->gData.tmStmpType;
        memcpy ( & ( tWriteAdd_p->gData.val_chkStd ),
                 & ( tUsrData_p->gData.val_chkStd ),
                 sizeof ( tUsrData_p->gData.val_chkStd )
               );
        memcpy ( & ( tWriteAdd_p->gData.dDsc ),
                 & ( tUsrData_p->gData.dDsc ),
                 sizeof ( tUsrData_p->gData.dDsc )
               );

    }

    datBuff_p->datalock.UnLock();

    //�еĵ��ַ�ڻ�������û���ҵ�
    if ( bugFlag == 1 )
    {
        return 1;
    }

    return 0;
}



/*
    ����:�ӹ����ڴ���ȡĳЩ����������
         ת����Ҫ����ͨ���ĵ�������ݴ�
         �뷢��ͨ��������
    ����:�ӿڵ����в�����Ϊ�������
    ���:��
    ����:0   �ɹ�
         ��0 ʧ��( ���з���1Ϊ�������Ĳ��ֵ����д��ɹ�)
*/
int convPhyToSndChn ( C_unitInfData* unitInfData_p,  /*�����ڴ����*/
                      const u_int& getChnNum,        /*Ҫȡֵ��ͨ����*/
                      const u_int& writeChnNum,      /*Ҫд���ͨ����*/
                      const PHY_UNIT_TYPE* phyType_p,  /*Ҫ������������phyType_p[]*/
                      const u_int& phyNum            /*Ҫ����������������*/
                    )
{
    if ( unitInfData_p == NULL
         || phyType_p == NULL
         || phyNum == 0
       )
    {
        return -1;
    }

    int tRet               = 0;
    const u_int maxNum     = 20;/*���֧��һ�δ����������ĸ���*/
    const u_int maxLng     = 10;/*���֧�ֵ�������ֵ�ĳ���*/
    const u_int maxAddNum  = 10;/*���֧�ֵ�һ����������Ӧ�ĵ��ַ����*/
    const u_int maxDidLng  = sizeof ( DATA_ID ); /*����id����󳤶�*/

    if ( phyNum > maxNum )
    {
        return -2;
    }

    //��֯Ҫȡֵ�����ݽṹ T_phyNVal,T_phyNCfg
    T_phyObjVal  phyObj_p[maxNum];
    char         phyVal_p[maxNum][maxLng];
    T_snnUsrData pntObj_p[maxNum][maxAddNum];
    memset ( phyObj_p, 0, sizeof ( phyObj_p ) );
    memset ( phyVal_p,    0, sizeof ( phyVal_p ) );
    memset ( pntObj_p,    0, sizeof ( pntObj_p ) );

    T_phyValCfg   phyValCfg_p[maxNum];
    PNT_ADDR_TYPE pntAddr_p[maxNum][maxAddNum];
    char          dataId_aa[maxNum][MC_DIDBIN_LEN];
    memset ( phyValCfg_p, 0, sizeof ( phyValCfg_p ) );
    memset ( pntAddr_p,   0, sizeof ( pntAddr_p ) );
    memset ( dataId_aa,    0, sizeof ( dataId_aa ) );

    for ( u_int i = 0; i < phyNum; i++ )
    {
        phyObj_p[i].phyType      = phyType_p[i];
        phyObj_p[i].phyObjVal_p  = phyVal_p[i];
        phyObj_p[i].pntObj_p     = pntObj_p[i];

        phyValCfg_p[i].pntAddr_p = pntAddr_p[i];
        phyValCfg_p[i].dataId_p  = dataId_aa[i];
        phyValCfg_p[i].phyType   = phyType_p[i];
    }

    T_phyNVal fromNData;
    T_phyNCfg curNPhyCfg;
    memset ( &fromNData,  0, sizeof ( fromNData ) );
    memset ( &curNPhyCfg, 0, sizeof ( curNPhyCfg ) );
    fromNData.phyUnitNum   = phyNum;
    fromNData.phyObjVal_p  = phyObj_p;
    curNPhyCfg.phyUnitNum  = phyNum;
    curNPhyCfg.phyValCfg_p = phyValCfg_p;

    tRet = unitInfData_p->getM ( getChnNum,
                                 writeChnNum,
                                 &fromNData,
                                 &curNPhyCfg
                               );

    if ( tRet != 0 )
    {
        return -2;
    }

    for ( u_int i = 0; i < phyNum; i++ )
    {
        /*����Ҫȡֵ��Ҫ���͵�ͨ���ż����õ�
          ���ַ������ת����Ҫ����ͨ���ĵ��
          ַ����:*/
        if ( getChnNum == writeChnNum )
        {
            //Ҫȡֵ��Ҫ���͵�ͨ����ͬһ��ʱ:
            //����Ҫת��
        }
        else /*ȡ�뷢����ͬһ��ͨ��*/
        {
            //��������Ӧ�ĵ��ַ����һ��
            if ( phyObj_p[i].numOfAdd == phyValCfg_p[i].numOfAdd )
            {
                //�滻���ַ
                for ( u_int j = 0; j < phyObj_p[i].numOfAdd; j++ )
                {
                    pntObj_p[i][j].gData.val_chkStd.pntAddr = phyValCfg_p[i].pntAddr_p[j];
                }
            }
            else /*�����϶�Ӧ�ĵ��ַ������һ��*/
            {
                //1 ���ýӿ�ת�������������ֵַ
                countManage tCtManage;
                T_snnUsrData* usrData_p = NULL;
                usrData_p = pntObj_p[i];
                tRet = tCtManage.calcFuncValToAdd ( usrData_p,
                                                    & ( phyValCfg_p[i] ),
                                                    & ( phyObj_p[i] )
                                                  );

                if ( tRet != 0 )
                {
                    return -3;
                }

            }
        }

        //��ֵд�뵽�����ڴ���
        tRet = writeChnMPntObj ( pntObj_p[i],
                                 phyValCfg_p[i].numOfAdd,
                                 & ( sendDataBuf[writeChnNum] )
                               );

        if ( tRet != 0 )
        {
            return tRet;
        }
    }

    return 0;
}


/*
    ����:��һ֡���ݵĵ�ֵת����������ֵ���빲���ڴ浥Ԫ
    ����:ssnPara_p һ֡���ݶ�Ӧ�Ľṹ
        :chnId    ͨ��id
    ���:unitInfData_p �����ڴ����
    ����:0   �ɹ�
         ��0 ʧ��
*/
int writeFrmPntToUMem ( const T_snnPara* ssnPara_p,   /*һ֡���ݶ�Ӧ�Ľṹ*/
                        const u_int& chnId,            /*ͨ����*/
                        C_unitInfData* unitInfData_p  /*�����ڴ����*/
                      )
{
    if ( ssnPara_p == NULL
         && unitInfData_p == NULL
       )
    {
        return -1;
    }

    if ( ssnPara_p->pntNum == 0 )
    {
        return 0;
    }

    const u_short pntNum = ssnPara_p->pntNum;

    const u_short maxPntNum = 300;

    if ( pntNum > maxPntNum )
    {
        //һ������֡����Ϊ256 ����300����Ҫ�޸�maxPntNum��ֵ
        return -2;
    }

    int tRet = 0;
    u_int tOutNum = 0;
    T_phyValCfg tOutPhyValCfg_a[maxPntNum];
    T_snnUsrData* tSnnUsrData_pa[maxPntNum];
    memset ( tOutPhyValCfg_a, 0, sizeof ( tOutPhyValCfg_a ) );
    memset ( tSnnUsrData_pa, 0, sizeof ( tSnnUsrData_pa ) );

    for ( u_short i = 0; i < pntNum; i++ )
    {
        tSnnUsrData_pa[i] = & ( ssnPara_p->usrData_p[i] );
    }

    //ȡ�������ܸ���
    tRet = unitInfData_p->getPhyNumByNPnt ( chnId,
                                            tSnnUsrData_pa,
                                            pntNum,
                                            &tOutNum,
                                            NULL
                                          );

    if ( tRet != 0 )
    {
        return tRet;
    }

    //�������ĸ�������maxPntNum:300����Ҫ�޸�maxPntNum��ֵ
    if ( tOutNum > maxPntNum )
    {
        return -3;
    }

    //û�ж�Ӧ��������
    if ( tOutNum == 0 )
    {
        return 0;
    }

    const u_short numOfAdd = 16;//һ������������Ӧ16����,���ڴ�ֵӦ�޸Ĵ�ֵ
    char tDIdBinStr_aa[maxPntNum][MC_DIDBIN_LEN];
    u_int tPntAddr_aa[maxPntNum][numOfAdd];
    memset ( tDIdBinStr_aa, 0, sizeof ( tDIdBinStr_aa ) );
    memset ( tPntAddr_aa, 0, sizeof ( tPntAddr_aa ) );

    for ( u_short i = 0; i < maxPntNum; i++ )
    {
        tOutPhyValCfg_a[i].pntAddr_p = tPntAddr_aa[i];
        tOutPhyValCfg_a[i].dataId_p = tDIdBinStr_aa[i];
    }

    //ͨ��һ֡�еĵ�,�ҵ����ж�Ӧ������������
    tRet = unitInfData_p->getPhyCfgByNPnt ( chnId,
                                            tSnnUsrData_pa,
                                            pntNum,
                                            tOutPhyValCfg_a,
                                            &tOutNum
                                          );

    if ( tRet != 0 )
    {
        return tRet;
    }

    T_phyObjVal tPhyObjVal_a[maxPntNum];
    double tPhyVal[maxPntNum];
    memset ( &tPhyObjVal_a, 0, sizeof ( tPhyObjVal_a ) );
    memset ( &tPhyVal, 0, sizeof ( tPhyVal ) );

    //����tOutNum����������ֵ
    for ( u_short i = 0; i < tOutNum; i++ )
    {
        if ( tOutPhyValCfg_a[i].numOfAdd > numOfAdd )
        {
            //һ������������Ӧ16����,���ڴ�ֵӦ�޸Ĵ�ֵ
            return -4;
        }

        tPhyObjVal_a[i].phyObjVal_p = & ( tPhyVal[i] ); //��i����������ֵ
        tPhyObjVal_a[i].phyType = tOutPhyValCfg_a[i].phyType;//��i��������������

        T_snnUsrData* usrData_p = NULL;

        //�ҵ���������Ӧ�ĵ�T_snnUsrData�ṹ
        for ( u_short j = 0; j < pntNum ; j++ )
        {
            usrData_p = & ( ssnPara_p->usrData_p[j] );

            if ( tOutPhyValCfg_a[i].pntAddr_p[0] == usrData_p->gData.val_chkStd.pntAddr )
            {
                tPhyObjVal_a[i].pntObj_p = usrData_p;
                break;
            }

        }

        if ( tPhyObjVal_a[i].pntObj_p == NULL ) //û���ҵ����ַ����
        {
            return -5;
        }

        //�����i����������ֵ
        tRet = calc.calcFuncAddToVal ( usrData_p,
                                       & ( tOutPhyValCfg_a[i] ),
                                       & ( tPhyObjVal_a[i] )
                                     );

        if ( tRet != 0 )
        {
            return tRet;
        }

    }

    T_phyNVal tPhyNval;
    memset ( &tPhyNval, 0, sizeof ( tPhyNval ) );
    tPhyNval.phyObjVal_p = tPhyObjVal_a;
    tPhyNval.phyUnitNum = tOutNum;

    tRet = unitInfData_p->setM ( chnId, &tPhyNval );

    if ( tRet != 0 )
    {
        return tRet;
    }

    return 0;
}

int setData ( T_snnUsrData* ssnPara_p, short stnID, u_char ucType )
{
    chnlDataBuf*  datBuff = NULL;

    datBuff =  & ( sendDataBuf[stnID] );

    if ( ssnPara_p == NULL || stnID < 0 )
    {
        return -1;
    }

    else
    {
        datBuff->datalock.Lock();

        if ( ucType == 0 )
        {
            T_snnUsrData* usrData_p =  ssnPara_p;
            u_int addr1 = usrData_p->gData.val_chkStd.pntAddr;

            //u_int addrProcess = 1;
            //u_int i;
            u_short mem;
            u_int i ;

            for ( i = 0; i < ( u_int ) ( datBuff->arLen ); i++ )
            {

                if ( addr1 >= ( datBuff->addrIdx_p[i] ).begAddr && addr1 <= ( datBuff->addrIdx_p[i] ).endAddr )
                {
                    mem = i;
                    break;
                }
            }

            if ( i == datBuff->arLen )
            {
                //unlockQ(this->memKey);
                datBuff->datalock.UnLock();
                return -1;//addrδ�ҵ�
            }

            int memst = addr1 - datBuff->addrIdx_p[mem].begAddr;
            datBuff->buf_pp[mem][memst].gData.pntType = usrData_p->gData.pntType;
            datBuff->buf_pp[mem][memst].gData.tmStmpType = usrData_p->gData.tmStmpType;
            datBuff->buf_pp[mem][memst].gData.dDsc = usrData_p->gData.dDsc;
            datBuff->buf_pp[mem][memst].gData.val_chkStd.pntVal1 = usrData_p->gData.val_chkStd.pntVal1;
            datBuff->buf_pp[mem][memst].gData.val_chkStd.pntVal2 = usrData_p->gData.val_chkStd.pntVal2;
            memcpy ( datBuff->buf_pp[mem][memst].gData.tmStmp_p, usrData_p->gData.tmStmp_p, datBuff->tmStmpLng );
        }
        else
        {
            T_snnUsrData* usrData_p =  ssnPara_p;
            u_int addr1 = usrData_p->fData.val_chkStd.pntAddr;

            //u_int addrProcess = 1;
            u_int mem;
            u_int  i;

            for (  i = 0; i < ( u_int ) ( datBuff->arLen ); i++ )
            {

                if ( addr1 >= ( datBuff->addrIdx_p[i] ).begAddr && addr1 <= ( datBuff->addrIdx_p[i] ).endAddr )
                {
                    mem = i;
                    break;
                }
            }

            if ( i == datBuff->arLen )
            {
                //unlockQ(this->memKey);
                datBuff->datalock.UnLock();
                return -1;//addrδ�ҵ�
            }

            int memst = addr1 - datBuff->addrIdx_p[mem].begAddr;
            datBuff->buf_pp[mem][memst].fData.pntType = usrData_p->fData.pntType;
            datBuff->buf_pp[mem][memst].fData.tmStmpType = usrData_p->fData.tmStmpType;
            datBuff->buf_pp[mem][memst].fData.dDsc = usrData_p->fData.dDsc;
            datBuff->buf_pp[mem][memst].gData.val_chkStd.pntVal1 = usrData_p->gData.val_chkStd.pntVal1;
            datBuff->buf_pp[mem][memst].gData.val_chkStd.pntVal2 = usrData_p->gData.val_chkStd.pntVal2;
            memcpy ( datBuff->buf_pp[mem][memst].fData.tmStmp_p, usrData_p->fData.tmStmp_p, datBuff->tmStmpLng );
        }



        datBuff->datalock.UnLock();
    }

    return 0;
}

int optData ( T_snnPara* ssnPara_p, short stnID, u_char ucType, u_char optType, u_char ssnNo )
{
    T_snnCfg* snnCfg = cfgMng->getSnnCfg ( ssnNo, stnID );

    u_short offset = snnCfg->offset;
    const u_char dtNum = ssnPara_p->pntNum ;
    dataMsgP dtMsgP;
    dtMsgP.chnlNo = stnID;
    dtMsgP.ssnNo  = ssnNo;
    chnlDataBuf*  datBuff = NULL;

    if ( optType == LOAD_BUFF )
    {
        datBuff =  & ( sendDataBuf[stnID] );
    }
    else
    {
        datBuff =  & ( recvDataBuf[stnID] );
    }

    if ( ssnPara_p == NULL || stnID < 0 )
    {
        return -1;
    }

    else
    {
        datBuff->datalock.Lock();

        for ( int pnt = 0; pnt < dtNum; pnt++ )
        {
            if ( ucType == 0 )
            {
                T_snnUsrData* usrData_p =  & ( ssnPara_p->usrData_p[pnt] );
                u_int addr1 = usrData_p->gData.val_chkStd.pntAddr + offset;

                //u_int addrProcess = 1;
                //u_int i;
                u_short mem;
                u_int i;

                for (  i = 0; i < ( u_int ) ( datBuff->arLen ); i++ )
                {

                    if ( addr1 >= ( datBuff->addrIdx_p[i] ).begAddr && addr1 <= ( datBuff->addrIdx_p[i] ).endAddr )
                    {
                        mem = i;
                        break;
                    }
                }

                if ( i == datBuff->arLen )
                {
                    //unlockQ(this->memKey);
                    datBuff->datalock.UnLock();
                    return -1;//addrδ�ҵ�
                }

                int memst = addr1 - datBuff->addrIdx_p[mem].begAddr;

                if ( optType == LOAD_BUFF )
                {
                    usrData_p->gData.pntType = datBuff->buf_pp[mem][memst].gData.pntType;
                    usrData_p->gData.tmStmpType = datBuff->buf_pp[mem][memst].gData.tmStmpType;
                    usrData_p->gData.dDsc = datBuff->buf_pp[mem][memst].gData.dDsc;
                    usrData_p->gData.val_chkStd.pntVal1 =  datBuff->buf_pp[mem][memst].gData.val_chkStd.pntVal1;
                    usrData_p->gData.val_chkStd.pntVal2 =  datBuff->buf_pp[mem][memst].gData.val_chkStd.pntVal2;
                    memcpy ( usrData_p->gData.tmStmp_p, datBuff->buf_pp[mem][memst].gData.tmStmp_p, datBuff->tmStmpLng );
                }
                else
                {
                    datBuff->buf_pp[mem][memst].gData.pntType = usrData_p->gData.pntType;
                    datBuff->buf_pp[mem][memst].gData.tmStmpType = usrData_p->gData.tmStmpType;
                    datBuff->buf_pp[mem][memst].gData.dDsc = usrData_p->gData.dDsc;
                    datBuff->buf_pp[mem][memst].gData.val_chkStd.pntAddr = usrData_p->gData.val_chkStd.pntAddr;
                    datBuff->buf_pp[mem][memst].gData.val_chkStd.pntVal1 = usrData_p->gData.val_chkStd.pntVal1;
                    datBuff->buf_pp[mem][memst].gData.val_chkStd.pntVal2 = usrData_p->gData.val_chkStd.pntVal2;
                    memcpy ( datBuff->buf_pp[mem][memst].gData.tmStmp_p, usrData_p->gData.tmStmp_p, datBuff->tmStmpLng );
                    dtMsgP.usrdataP[pnt] =  & ( datBuff->buf_pp[mem][memst] );
                }

            }
            else
            {
                T_snnUsrData* usrData_p =  & ( ssnPara_p->usrData_p[pnt] );
                u_int addr1 = usrData_p->fData.val_chkStd.pntAddr + offset;

                //u_int addrProcess = 1;
                u_int mem;
                u_int  i;

                for ( i = 0; i < ( u_int ) ( datBuff->arLen ); i++ )
                {

                    if ( addr1 >= ( datBuff->addrIdx_p[i] ).begAddr && addr1 <= ( datBuff->addrIdx_p[i] ).endAddr )
                    {
                        mem = i;
                        break;
                    }
                }

                if ( i == datBuff->arLen )
                {
                    //unlockQ(this->memKey);
                    datBuff->datalock.UnLock();
                    return -1;//addrδ�ҵ�
                }

                int memst = addr1 - datBuff->addrIdx_p[mem].begAddr;

                if ( optType == LOAD_BUFF )
                {
                    usrData_p->fData.pntType = datBuff->buf_pp[mem][memst].fData.pntType;
                    usrData_p->fData.tmStmpType = datBuff->buf_pp[mem][memst].fData.tmStmpType;
                    usrData_p->fData.dDsc = datBuff->buf_pp[mem][memst].fData.dDsc;
                    usrData_p->gData.val_chkStd.pntVal1 =  datBuff->buf_pp[mem][memst].gData.val_chkStd.pntVal1;
                    usrData_p->gData.val_chkStd.pntVal2 =  datBuff->buf_pp[mem][memst].gData.val_chkStd.pntVal2;
                    memcpy ( usrData_p->fData.tmStmp_p, datBuff->buf_pp[mem][memst].fData.tmStmp_p, datBuff->tmStmpLng );
                }
                else
                {
                    datBuff->buf_pp[mem][memst].fData.pntType = usrData_p->fData.pntType;
                    datBuff->buf_pp[mem][memst].fData.tmStmpType = usrData_p->fData.tmStmpType;
                    datBuff->buf_pp[mem][memst].fData.dDsc = usrData_p->fData.dDsc;
                    datBuff->buf_pp[mem][memst].gData.val_chkStd.pntAddr = usrData_p->gData.val_chkStd.pntAddr;
                    datBuff->buf_pp[mem][memst].gData.val_chkStd.pntVal1 = usrData_p->gData.val_chkStd.pntVal1;
                    datBuff->buf_pp[mem][memst].gData.val_chkStd.pntVal2 = usrData_p->gData.val_chkStd.pntVal2;
                    memcpy ( datBuff->buf_pp[mem][memst].fData.tmStmp_p, usrData_p->fData.tmStmp_p, datBuff->tmStmpLng );
                    dtMsgP.usrdataP[pnt] =  & ( datBuff->buf_pp[mem][memst] );
                }
            }

        }

        datBuff->datalock.UnLock();

        if ( optType == SAVE_BUFF && ssnNo == ACQ_EVT )
        {
            int tRet = writeFrmPntToUMem ( ssnPara_p, stnID, &unitInfData );

            if ( tRet != 0 )
            {
                cfgMng->tryLog.LPrintf ( DEBUG,
                                         DEBUG_LEVEL_0,
                                         "%s|%s|%d: writeFrmPntToUMem stnID=[%d] ,tRet=[%d]\r\n",
                                         __FILE__, __func__, __LINE__, stnID, tRet );
                return tRet;
            }

            //ssnFnshNfy ( g_appTID, ( char* ) &dtMsgP,  sizeof ( dtMsgP ), ssnFnshNfyID );
        }

    }

    return 0;
}

int setSsnCfg ( T_snnCfg* ssnStnCnfg_p, u_short  addrStart, u_short  accumValue, int iPtclID )
{
    if ( iPtclID == 106 )
    {
        ssnStnCnfg_p->stepCfg_p[0].comReqData_p->modbus.pduData.sysEleID = PDU_DATA_INDEX;
        ssnStnCnfg_p->stepCfg_p[0].comReqData_p->modbus.pduData.lng = ( sizeof ( u_short ) * 2 );
        memcpy ( ssnStnCnfg_p->stepCfg_p[0].comReqData_p->modbus.pduData.p_Mem,
                 &addrStart,
                 sizeof ( u_short )
               );
        memcpy ( ( ssnStnCnfg_p->stepCfg_p[0].comReqData_p->modbus.pduData.p_Mem + ( sizeof ( u_short ) ) ),
                 &accumValue,
                 sizeof ( u_short )
               );
    }

    return 0;
}
void CloseProcess()
{
    //delete g_locServer;
    //delete g_locClient;
}

static void minTimerDftHdl ( void* tmoutInfo_p )
{

}
void min5TimerDftHdl()
{

}
void min15TimerDftHdl()
{

}

int getNAndBTime ( DATEFORMAT_T& nowTime, DATEFORMAT_T& beforeTime, int seconds )
{
    time_t nowT;
    time_t beforeT;
    struct tm* local;
    time ( &nowT );
    beforeT = nowT - seconds;
    local = localtime ( &nowT );
    nowTime.year   = local->tm_year + 1900;
    nowTime.month  = local->tm_mon + 1;
    nowTime.day    = local->tm_mday;
    nowTime.hour   = local->tm_hour;
    nowTime.minute = local->tm_min;
    nowTime.second = local->tm_sec;
    local = localtime ( &beforeT );
    beforeTime.year   = local->tm_year + 1900;
    beforeTime.month  = local->tm_mon + 1;
    beforeTime.day    = local->tm_mday;
    beforeTime.hour   = local->tm_hour;
    beforeTime.minute = local->tm_min;
    beforeTime.second = local->tm_sec;
    return 0;
}


int getSysTime ( DATAFORMAT_DATE7B1& sysTime, time_t& tTime )
{
    //time_t time_p;
    struct tm* local_p = NULL;

    if ( tTime == 0 )
    {
        tTime = time ( NULL );
    }

#ifdef WINDOWS_PLATFORM
    local_p = localtime ( &tTime );
#else
    struct tm stm4Time;
    local_p = localtime_r ( &tTime, &stm4Time );
#endif
    //local_p = localtime ( &time_p );
    sysTime.year   = local_p->tm_year + 1900;
    //memcpy(&(sysTime.year) ,&( local->tm_year) ,sizeof(short) );
    sysTime.month  = local_p->tm_mon + 1;
    //memcpy(&(sysTime.month) ,&( local->tm_mon) ,sizeof(char) );
    sysTime.day    = local_p->tm_mday;
    //memcpy(&(sysTime.day) ,&( local->tm_mday) ,sizeof(char) );
    sysTime.hour   = local_p->tm_hour;
    //memcpy(&(sysTime.hour) ,&( local->tm_hour) ,sizeof(char) );
    sysTime.minute = local_p->tm_min;
    //memcpy(&(sysTime.minute) ,&( local->tm_min) ,sizeof(char) );
    sysTime.second = local_p->tm_sec;
    //memcpy(&(sysTime.second) ,&( local->tm_sec) ,sizeof(char) );
    return 0;
}

#ifndef WINDOWS_PLATFORM
int getDateID ( DATA_ID* dataId, int chnl, int funcId, int time )
{
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    /*
    * ���ڵ������ݱ���
    * ������  ��MakeEncoding(ENCODETYPE_TURBINE,sn,16,x); *
    * �������  ��MakeDate(DATACATALOG_SD,x);               *
    * ��������  ��MakeData(DATACATALOG_DD,x);

    * ����      ��MakeMeteorSingle()
    * ����      ��MakeMeteorSingle()
    * �¶�      ��MakeMeteorSingle()
    * �����ܶ�  ��MakeMeteorSingle()
    * ��׼����  ��MakePeriodSingle()
    * ƽ������  ��MakePeriodSingle()
    * �����  ��MakePeriodSingle()
    * ��С����  ��MakePeriodSingle()
    * ƽ����������MakePeriodSingle()*/
    DID did;
    DID didClear;
    didClear.Clear();
    int turbineH = 0;//�����챸߶�
    float amtH = 10.0;//������߶�
    int turbineNum = 0;//������

    /////////////////////////////��Ȫ�ӷ��/////////////////// chnl ��ֵ srcsn = 0
    /////////////////////////////�������/////////////////// chnl ��ֵ srcsn = 1
    if ( chnl == 0 || chnl == 1 ) //���
    {
        int turbineIndex = 0;

        if ( 4 == funcId ) //������Ϊ4
        {

            for ( int i = 0 ; i < 33 ; i ++ ) //�����ѭ��
            {
                if ( chnl == 0 )
                {
                    turbineNum = i;
                }

                if ( chnl == 1 )
                {
                    turbineNum = i + 33;
                }

                //����
                dataId[turbineIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_WS,
                                       /*B_Kind*/DATAKIND_AVG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                                       /*���������豸���*/ENCODETYPE_TURBINE, turbineNum, /*������Դ*/SOURCETYPE_TSS, chnl, HEIGHTTYPE_HUB, turbineH,
                                       /*ʱ����*/INTERVALUNIT_MINUTE, time );
                turbineIndex ++;
                //����
                dataId[turbineIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( DATACATALOG_WD,
                                       DATAKIND_AVG, DATATYPE_FLOAT32, did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                                       ENCODETYPE_TURBINE, turbineNum, SOURCETYPE_TSS, chnl, HEIGHTTYPE_HUB, turbineH,
                                       INTERVALUNIT_MINUTE, time );
                turbineIndex ++;
                //�й�����
                dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_PP,
                                       /*B_Kind*/DATAKIND_AVG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                       /*encoding type*/ENCODETYPE_TURBINE, /*ecsn*/turbineNum, /*B_Source*/SOURCETYPE_TSS, /*srcsn*/chnl,
                                       /*B_Interval*/INTERVALUNIT_MINUTE, time );
                turbineIndex ++;
                //�޹�����
                dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_PQ,
                                       /*B_Kind*/DATAKIND_AVG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                       ENCODETYPE_TURBINE, turbineNum, /*B_Source*/SOURCETYPE_TSS, chnl,
                                       /*B_Interval*/INTERVALUNIT_MINUTE, time );
                turbineIndex ++;

                //ת��
                dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_RS,
                                       /*B_Kind*/DATAKIND_BLD, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                       ENCODETYPE_TURBINE, turbineNum, /*B_Source*/SOURCETYPE_TSS, chnl,
                                       /*B_Interval*/INTERVALUNIT_MINUTE, time );
                turbineIndex ++;
            }//�����ѭ��

            //���糡�й�����
            dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_PP,
                                   /*B_Kind*/DATAKIND_AVG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                   ENCODETYPE_FARM, turbineNum, /*B_Source*/SOURCETYPE_IAS, chnl,
                                   /*B_Interval*/INTERVALUNIT_MINUTE, time );
            turbineIndex ++;

            //���糡�޹�����
            dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_PQ,
                                   /*B_Kind*/DATAKIND_AVG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                   ENCODETYPE_FARM, turbineNum, /*B_Source*/SOURCETYPE_IAS, chnl,
                                   /*B_Interval*/INTERVALUNIT_MINUTE, time );
            turbineIndex ++;

            //���糡�շ�����
            dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_GE,
                                   /*B_Kind*/DATAKIND_AMV, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                   ENCODETYPE_FARM, turbineNum, /*B_Source*/SOURCETYPE_IAS, chnl,
                                   /*B_Interval*/INTERVALUNIT_MINUTE, time );
            turbineIndex ++;

            //���糡��������̨��
            dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_TA,
                                   /*B_Kind*/DATAKIND_PGG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                   ENCODETYPE_FARM, turbineNum, /*B_Source*/SOURCETYPE_IAS, chnl,
                                   /*B_Interval*/INTERVALUNIT_DAY, time );
            turbineIndex ++;
            //���糡������������
            dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_CP,
                                   /*B_Kind*/DATAKIND_PGG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                   ENCODETYPE_FARM, turbineNum, /*B_Source*/SOURCETYPE_IAS, chnl,
                                   /*B_Interval*/INTERVALUNIT_DAY, time );
            turbineIndex ++;

            //���糡�޹���̨��
            dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_TA,
                                   /*B_Kind*/DATAKIND_PGGTHV, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                   ENCODETYPE_FARM, turbineNum, /*B_Source*/SOURCETYPE_IAS, chnl,
                                   /*B_Interval*/INTERVALUNIT_DAY, time );
            turbineIndex ++;
            //���糡�޹�������
            dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_CP,
                                   /*B_Kind*/DATAKIND_PGGTHV, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                   ENCODETYPE_FARM, turbineNum, /*B_Source*/SOURCETYPE_IAS, chnl,
                                   /*B_Interval*/INTERVALUNIT_DAY, time );
            turbineIndex ++;

            //���糡����̨��
            dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_TA,
                                   /*B_Kind*/DATAKIND_WWG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                   ENCODETYPE_FARM, turbineNum, /*B_Source*/SOURCETYPE_IAS, chnl,
                                   /*B_Interval*/INTERVALUNIT_DAY, time );
            turbineIndex ++;
            //���糡��������
            dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_CP,
                                   /*B_Kind*/DATAKIND_WWG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                   ENCODETYPE_FARM, turbineNum, /*B_Source*/SOURCETYPE_IAS, chnl,
                                   /*B_Interval*/INTERVALUNIT_DAY, time );
            turbineIndex ++;

            //���糡ͣ��̨��
            dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_TA,
                                   /*B_Kind*/DATAKIND_SPG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                   ENCODETYPE_FARM, turbineNum, /*B_Source*/SOURCETYPE_IAS, chnl,
                                   /*B_Interval*/INTERVALUNIT_DAY, time );
            turbineIndex ++;
            //���糡ͣ������
            dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_CP,
                                   /*B_Kind*/DATAKIND_SPG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                   ENCODETYPE_FARM, turbineNum, /*B_Source*/SOURCETYPE_IAS, chnl,
                                   /*B_Interval*/INTERVALUNIT_DAY, time );
            turbineIndex ++;

            //���糡ͨѶ�ж�̨��
            dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_TA,
                                   /*B_Kind*/DATAKIND_CBG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                   ENCODETYPE_FARM, turbineNum, /*B_Source*/SOURCETYPE_IAS, chnl,
                                   /*B_Interval*/INTERVALUNIT_DAY, time );
            turbineIndex ++;
            //���糡ͨѶ�ж�����
            dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_CP,
                                   /*B_Kind*/DATAKIND_CBG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                   ENCODETYPE_FARM, turbineNum, /*B_Source*/SOURCETYPE_IAS, chnl,
                                   /*B_Interval*/INTERVALUNIT_DAY, time );
            turbineIndex ++;
        }//������Ϊ4

        else if ( 2 == funcId ) //���״̬
        {
            //ÿ̨�����5�����״̬:�������硢�޹��ʡ����硢ͨѶ���ϡ�ͣ��
            //  ���ݿ�û�ж�Ӧ��ֵ�����ֻ�ɵ�һ��������4����Ҫ
            //��������
            for ( int i = 0 ; i < 33 ; i ++ ) //�����ѭ��
            {
                if ( chnl == 0 )
                {
                    turbineNum = i;
                }

                if ( chnl == 1 )
                {
                    turbineNum = i + 33;
                }

                dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_SF,
                                       /*B_Kind*/DATAKIND_OGN, /*B_Type*/DATATYPE_UINT16, /*B_Length*/did.TypeSize ( DATATYPE_UINT16 ), /*B_Amount*/1,
                                       ENCODETYPE_TURBINE, turbineNum, /*B_Source*/SOURCETYPE_TSS, chnl,
                                       /*B_Interval*/INTERVALUNIT_MINUTE, time );
                turbineIndex ++ ;
                didClear.Export ( ( char* ) & ( dataId[turbineIndex] ) );
                turbineIndex ++ ;
                didClear.Export ( ( char* ) & ( dataId[turbineIndex] ) );
                turbineIndex ++ ;
                didClear.Export ( ( char* ) & ( dataId[turbineIndex] ) );
                turbineIndex ++ ;
                didClear.Export ( ( char* ) & ( dataId[turbineIndex] ) );
                turbineIndex ++ ;
            }
        }

        //���糡

    }

    else if ( chnl == 2 ) //
    {
        //ʮ�ײ��
        //˲ʱ����
        int towerIndex = 11;

        //int i = 12;
        for ( int i = 0; i < 4; i ++ )
        {
            //����ƽ��ֵ
            dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_WS,
                                 /*B_Kind*/DATAKIND_AVG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                                 /*���������豸���*/ENCODETYPE_AMT, 0, /*������Դ*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, amtH,
                                 /*ʱ����*/INTERVALUNIT_MINUTE, time );
            towerIndex ++;
            //����ƽ��ֵ
            dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_WD,
                                 /*B_Kind*/DATAKIND_AVG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                                 /*���������豸���*/ENCODETYPE_AMT, 0, /*������Դ*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, amtH,
                                 /*ʱ����*/INTERVALUNIT_MINUTE, time );
            towerIndex ++;

            //���ٱ�׼��
            dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_WS,
                                 /*B_Kind*/DATAKIND_SDV, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                                 /*���������豸���*/ENCODETYPE_AMT, 0, /*������Դ*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, amtH,
                                 /*ʱ����*/INTERVALUNIT_MINUTE, time );
            towerIndex ++;

            //˲ʱ����
            dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_WS,
                                 /*B_Kind*/DATAKIND_RTV, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                                 /*���������豸���*/ENCODETYPE_AMT, 0, /*������Դ*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, amtH,
                                 /*ʱ����*/INTERVALUNIT_MINUTE, time );
            towerIndex ++;

            //˲ʱ�����
            dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_WD,
                                 /*B_Kind*/DATAKIND_RTV, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                                 /*���������豸���*/ENCODETYPE_AMT, 0, /*������Դ*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, amtH,
                                 /*ʱ����*/INTERVALUNIT_MINUTE, time );
            towerIndex ++;
            //���������
            didClear.Export ( ( char* ) & ( dataId[towerIndex] ) );
            towerIndex ++;

            //������
            dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_WS,
                                 /*B_Kind*/DATAKIND_MAX, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                                 /*���������豸���*/ENCODETYPE_AMT, 0, /*������Դ*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, amtH,
                                 /*ʱ����*/INTERVALUNIT_MINUTE, time );
            towerIndex ++;

            //��С����
            dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_WS,
                                 /*B_Kind*/DATAKIND_MIN, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                                 /*���������豸���*/ENCODETYPE_AMT, 0, /*������Դ*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, amtH,
                                 /*ʱ����*/INTERVALUNIT_MINUTE, time );
            towerIndex ++;

            //�����׼��
            dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_WD,
                                 /*B_Kind*/DATAKIND_SDV, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                                 /*���������豸���*/ENCODETYPE_AMT, 0, /*������Դ*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, amtH,
                                 /*ʱ����*/INTERVALUNIT_MINUTE, time );
            towerIndex ++;

            //������
            dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_WD,
                                 /*B_Kind*/DATAKIND_MAX, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                                 /*���������豸���*/ENCODETYPE_AMT, 0, /*������Դ*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, amtH,
                                 /*ʱ����*/INTERVALUNIT_MINUTE, time );
            towerIndex ++;

            //��С����
            dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_WD,
                                 /*B_Kind*/DATAKIND_MIN, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                                 /*���������豸���*/ENCODETYPE_AMT, 0, /*������Դ*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, amtH,
                                 /*ʱ����*/INTERVALUNIT_MINUTE, time );
            towerIndex ++;

            if ( i == 0 )
            {
                amtH += 40;
            }
            else
            {
                amtH += 20;
            }
        }

        //10m����¶�ƽ��ֵ
        dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_AT,
                             /*B_Kind*/DATAKIND_AVG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*���������豸���*/ENCODETYPE_AMT, 0, /*������Դ*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, 10,
                             /*ʱ����*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //10m����¶�˲ʱֵ
        dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_AT,
                             /*B_Kind*/DATAKIND_RTV, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*���������豸���*/ENCODETYPE_AMT, 0, /*������Դ*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, 10,
                             /*ʱ����*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //10m����¶ȱ�׼��
        dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_AT,
                             /*B_Kind*/DATAKIND_SDV, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*���������豸���*/ENCODETYPE_AMT, 0, /*������Դ*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, 10,
                             /*ʱ����*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //10m����¶����ֵ
        dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_AT,
                             /*B_Kind*/DATAKIND_MAX, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*���������豸���*/ENCODETYPE_AMT, 0, /*������Դ*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, 10,
                             /*ʱ����*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //10m����¶���Сֵ
        dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_AT,
                             /*B_Kind*/DATAKIND_MIN, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*���������豸���*/ENCODETYPE_AMT, 0, /*������Դ*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, 10,
                             /*ʱ����*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //10m���ʪ��ƽ��ֵ
        dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_AH,
                             /*B_Kind*/DATAKIND_AVG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*���������豸���*/ENCODETYPE_AMT, 0, /*������Դ*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, 10,
                             /*ʱ����*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //10m���ʪ��˲ʱֵ
        dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_AH,
                             /*B_Kind*/DATAKIND_RTV, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*���������豸���*/ENCODETYPE_AMT, 0, /*������Դ*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, 10,
                             /*ʱ����*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //10m���ʪ�ȱ�׼��
        dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_AH,
                             /*B_Kind*/DATAKIND_SDV, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*���������豸���*/ENCODETYPE_AMT, 0, /*������Դ*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, 10,
                             /*ʱ����*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //10m���ʪ�����ֵ
        dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_AH,
                             /*B_Kind*/DATAKIND_MAX, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*���������豸���*/ENCODETYPE_AMT, 0, /*������Դ*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, 10,
                             /*ʱ����*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //10m���ʪ����Сֵ
        dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_AH,
                             /*B_Kind*/DATAKIND_MIN, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*���������豸���*/ENCODETYPE_AMT, 0, /*������Դ*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, 10,
                             /*ʱ����*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //10m�����ѹƽ��ֵ
        dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_AP,
                             /*B_Kind*/DATAKIND_AVG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*���������豸���*/ENCODETYPE_AMT, 0, /*������Դ*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, 10,
                             /*ʱ����*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //10m�����ѹ˲ʱֵ
        dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_AP,
                             /*B_Kind*/DATAKIND_RTV, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*���������豸���*/ENCODETYPE_AMT, 0, /*������Դ*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, 10,
                             /*ʱ����*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //10m�����ѹ��׼��
        dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_AP,
                             /*B_Kind*/DATAKIND_SDV, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*���������豸���*/ENCODETYPE_AMT, 0, /*������Դ*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, 10,
                             /*ʱ����*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //10m�����ѹ���ֵ
        dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_AP,
                             /*B_Kind*/DATAKIND_MAX, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*���������豸���*/ENCODETYPE_AMT, 0, /*������Դ*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, 10,
                             /*ʱ����*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //10m�����ѹ��Сֵ
        dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_AP,
                             /*B_Kind*/DATAKIND_MIN, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*���������豸���*/ENCODETYPE_AMT, 0, /*������Դ*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, 10,
                             /*ʱ����*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //��Դ��ѹƽ��ֵ
        dataId[towerIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_BL,
                             /*B_Kind*/DATAKIND_AVG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*���������豸���*/ENCODETYPE_AMT, 0, /*������Դ*/SOURCETYPE_AMT, 0,
                             /*ʱ����*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //��Դ��ѹ˲ʱֵ
        dataId[towerIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_BL,
                             /*B_Kind*/DATAKIND_RTV, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*���������豸���*/ENCODETYPE_AMT, 0, /*������Դ*/SOURCETYPE_AMT, 0,
                             /*ʱ����*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //��Դ��ѹ��׼��
        dataId[towerIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_BL,
                             /*B_Kind*/DATAKIND_SDV, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*���������豸���*/ENCODETYPE_AMT, 0, /*������Դ*/SOURCETYPE_AMT, 0,
                             /*ʱ����*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //��Դ��ѹ���ֵ
        dataId[towerIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_BL,
                             /*B_Kind*/DATAKIND_MAX, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*���������豸���*/ENCODETYPE_AMT, 0, /*������Դ*/SOURCETYPE_AMT, 0,
                             /*ʱ����*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //��Դ��ѹ��Сֵ
        dataId[towerIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_BL,
                             /*B_Kind*/DATAKIND_MIN, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*���������豸���*/ENCODETYPE_AMT, 0, /*������Դ*/SOURCETYPE_AMT, 0,
                             /*ʱ����*/INTERVALUNIT_MINUTE, time );
        towerIndex++;
    }

    return chnl;
}
#endif


// ϵͳ�˳�����Ӧ��Դ���л��մ���
void sysClear()
{

    int tRet = 0;
    //�ر�socket
    tRet = g_locServer.Cleanup();

    if ( tRet != 0 )
    {
        cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "%s|%s|%d: g_locServer.Cleanup turn error, iRet = %d\r\n",
                                 __FILE__, __func__, __LINE__, tRet );
    }

    //����ϵͳ��Ϣ����
    if ( ssnFnshNfyID > 0  )
    {
        tRet = rmSsnNfyBody ( ssnFnshNfyID );

        if ( tRet != 0 )
        {
            cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "%s|%s|%d: rmSsnNfyBody turn error, iRet = %d\r\n",
                                     __FILE__, __func__, __LINE__, tRet );
        }

        ssnFnshNfyID = 0;
    }

    if ( cfgMng != NULL )
    {
        delete cfgMng;
    }


    return;
}
