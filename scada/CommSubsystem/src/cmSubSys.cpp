//现场数据综合通信系统--通信子系统公共头文件
//
//单位：兆方美迪风能工程(上海)有限公司
//编写人：赵金杰
//编写时间：2016年12月
//

#include "cmSubSys.h"



#define MC_DEF_Q_NUM    10      // 默认队列数，要大于等于远程站个数 （MC_DEF_STN_NUM）
#define MC_DEF_QU_NUM   20
#define MC_STEP_MAX_NUM 32      // 配置中会话一步最大帧数
#define MC_DEF_SSN_TM_OUT   30*10  // 会话超时时间，给用户线程使用，单位秒（s）。要求：时间要足够大，才能够保证
#define MC_MAX_PNT_NUM      520     // 会话一步中支持的最大点数



/************************本文件中静态函数申明区*********************************/
static void ssnTmOutDftHdl ( void* tmoutInfo_p );
static void minTimerDftHdl ( void* tmoutInfo_p );



/*************************全局变量申明区***************************************/

//自定义的双组队列
crtGrpQPar ssnQPar = { 1, MC_DEF_Q_NUM, sizeof ( qSsnReqUint ) /*50*/, MC_DEF_QU_NUM }; //ltG, q-num=10,q-unit lng = 50, q-unit num = 10
crtGrpQPar ssnRdyQPar = { 2, 0, 0, 0 };//lgG, q-num=0,q-unit lng = 0, q-unit num = 0
C_DuGrp_BCPQ ssnDuQ ( ssnQPar, ssnRdyQPar ); //du-queue. fstGrpQ-rdy; scdGrpQ-ssn

crtGrpQPar ASDUQPar = { 1, MC_DEF_Q_NUM, sizeof ( qASDUInfoRcvUnit ), MC_DEF_QU_NUM }; //ltG, q-num=10,q-unit lng = 50, q-unit num = 10
crtGrpQPar transCtlQPar = { 2, MC_DEF_Q_NUM, sizeof ( qTransCtrlInfoUnit ), MC_DEF_QU_NUM };/*transCtrlF*//*transCtrlFrmDsc*/ //lgG, q-num=0,q-unit lng = 0, q-unit num = 0
C_DuGrp_BCPQ ASDUDuQ ( ASDUQPar, transCtlQPar ); //du-queue.scdGrpQ-ASDU; fstGrpQ-transCtl

crtGrpQPar ASDUInfoQPar = { 1, MC_DEF_Q_NUM, sizeof ( qASDUInfoRcvUnit ), MC_DEF_QU_NUM }; //ltG, q-num=1,q-unit lng = 50, q-unit num = 10
C_DuGrp_BCPQ ASDUInfoQ ( ASDUInfoQPar ); //du-queue.fstGrpQ-ASDU_Info

u_char* g_chnlQCnxt_p = NULL; // 队列通道上下文


//会话相关
C_SsnObj g_ssnDsc_1;             //a needed to config ssn obj.
C_SsnMgr ssnMgr;                  // ssn manager
C_SsnCtnrObj ssnCtnr;            // 会话容器
T_snnCfg* g_snnCfg_p     = NULL; // 会话配置
T_snnPara* g_snnPara_p   = NULL; // 会话参数
ONE_STEP* g_oneStepRul_p = NULL; // 步规则

//远程站
C_StnObj stnObj; // 远程站对象
int g_stnNum        = MC_DEF_STN_NUM; // 远程站个数，后续会动态变化
u_long* g_stnAddr_p = NULL;          // 远程站地址数组，用于监听服务校验
u_short* g_stnPort_p   = NULL;          // 远程站端口数组，用于监听服务校验
//通道
C_ChnlMgr chnlMgr; // chnl manager


//日志对象
//nsCPLog::CDailyLog g_TryLog;

//socket
EVENT** g_evtArr_pp            = NULL; // 事件二维数组，用于网络接收层
sock_descriptor** g_sockArr_pp = NULL; // 句柄二维数组，用于网络接收层
nsCPSocket::CTcpServer g_locServer;    // 用于初始化g_evtArr_pp
nsCPSocket::CTcpClient g_locClient;
int g_locPort = 0;


C_TmObj tmFclty; // 定时器对象
//extern C_TmObj tmFclty;


//超时
//FUN_RL* g_vRl_p;
FunTypeOne ssnTmOutFun_p = NULL; // 会话超时回调函数
FunTypeOne minTimerFun_p = minTimerDftHdl;  //1分钟数据发送回调函数
int ssnFnshNfyID         = 0;
int t_Tid                = 0;


//线程
nsCPThread::CThread* thdObj_p = NULL; // 线程管理对象
int text_Tid                  = 0;    //调试多用户线程时记录线程号
char g_cUsrCnt                = 2;    // 用户线程计数器
T_TID g_errTID                = VALIDTTID;
T_TID g_appTID                = VALIDTTID;
T_TID g_putStorTID            = VALIDTTID;
T_TID g_outStorTID            = VALIDTTID;


//互斥锁
nsCPLock::CLock g_ssnLock; // 会话锁，用于会话操作互斥


//配置管理
/***************************************************
*测试时增加通道需修改的地方
*TsysUsr线程
*getDataID哈数
*
***************************************************/
//ModbusChnlAddrSign *modbusChnlAddrSign_p;
BufData* bufData_p = NULL;
cfgManage* cfgMng = NULL;//new cfgManage ( MCT_SSN_NUM );
chnlDataBuf recvDataBuf[MC_DEF_STN_NUM];//点地址通用对象存储(接收缓冲区)
chnlDataBuf sendDataBuf[MC_DEF_STN_NUM];//点地址通用对象存储(发送缓冲区)
countManage calc;
C_unitInfData unitInfData ;


//vbus
IvBus* g_vbus_p = NULL; 


void sysInit() //模仿配置子系统
{
    //int bufNum = 300; //配置给出
    //初始化配置类
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

#ifdef RUN_VBUS_FLAG  //是否需要vbus
    //vbus 初始化
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

    u_char ucUsrNum = 1; // 用户线程数

    // 站配置 ////////////////////////////////////////////////////

    //g_stnNum = MC_DEF_STN_NUM; // 远程站个数，后续会动态变化
    bufData_p  = new BufData[g_stnNum];
    //modbusChnlAddrSign_p = new ModbusChnlAddrSign[g_stnNum]();
    int iLocPort = 2404; // 本地端端口号值
    //#endif
    g_locPort = iLocPort;


    ssnSets tSsnSets[MC_DEF_STN_NUM][MCT_SSN_NUM * 2]; // 远程站支持的会话集，一维表示远程站号，二维表示远程站支持的会话集合

    // 队列上下文地址空间
    g_chnlQCnxt_p = new u_char[g_stnNum];

    if ( g_chnlQCnxt_p == NULL )
    {
        return;
    }

    memset ( g_chnlQCnxt_p, 0, sizeof ( u_char ) * g_stnNum );

    //////////////////////////////////////////////////////
    // 全局的远程站地址信息、事件、句柄
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
    // 本地服务端启动，为了初始化全局的事件及句柄值

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

    // 初始化事件及句柄值

    for ( int in = 0; in < num * g_stnNum; in++ )
    {
        sock_descriptor sockConn = g_locClient.ConnServer ( "127.0.0.1", iLocPort, "127.0.0.1", in + 20000, 0 );

        if ( sockConn == sock_invalid )
        {
            in--;
        }
    }

    nsCPPub::SleepSecond ( 5 );
    // 会话 添加到管理类中
    //apply for ssnItem to ssnMgr
    int iSucc = 0;
    ssnTmOutFun_p = ssnTmOutDftHdl;
    int tSsnID, tChnlID;
    C_SsnObj* tSsnDsc_p = NULL;
    HMODULE fHdl = NULL;

    for ( int i = 0; i < MCT_SSN_NUM; i++ )
    {
        snnAppCfg ptclSnnCfg = ( cfgMng->getAppSnnCfg() ) [i];
        tSsnID = ssnMgr.aplSsnItem ( ptclSnnCfg, fHdl ); // 将会话添加到会话管理中，返回值为（应用）会话号

        if ( tSsnID < 0 )
        {
            continue;
        }

        tSsnDsc_p = &g_ssnDsc_1;
        ssnMgr.regSsnObj ( tSsnID, tSsnDsc_p ); //register ssnObj into ssnTbl
        iSucc++;
        int idx = i * 2;

        // 通道上支持的相应会话集合
        for ( int iss = 0; iss < g_stnNum; iss++ )
        {
            chnlObjCfg chnlCfg = ( cfgMng->getchnlObjCfg() ) [iss];
            // 远程站支持的会话集
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

    // 通道相关初始化
    C_ChnlObj* tChnlObj_p;
    void* sckHandle_p = NULL;

    for ( int i = 0; i < g_stnNum; i++ )
    {
        // 通道
        // chnl-0
        //apply for chnlItem to chnlMg
        //取配置信息
        chnlObjCfg chnlCfg = ( cfgMng->getchnlObjCfg() ) [i];
        // 申请并返回通道管理中(chnlTblItem*)(chnlTblHd_p+i)的索引号i
        tChnlID = chnlMgr.aplChnlItem(); 

        //初始化通道对象:通道描述符
        u_char ucState = 0;
        // 第二个参数为apdu长度（应取值于配置），注意与下面apdu创建时保持一致
        tChnlObj_p = new C_ChnlObj ( ucState, chnlCfg.bufLng, chnlCfg.ptclMdlNum ); 

        if ( tChnlObj_p == NULL )
        {
            return;
        }

        // 通道其他信息初始化，如定时资源等
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

        // 远程站地址加入到全局远程站地址相应位置中，通道号及下标
        g_stnAddr_p[i] = tStnOverView.stnAddr;
        g_stnPort_p[i] = tStnOverView.stnAppID;
        tChnlObj_p->setChnlNo ( tChnlID );
        // 角色初始化
        tChnlObj_p->setRole ( chnlCfg.role );

        // 设置支持的会话集
        tChnlObj_p->setSsnSets ( tSsnSets[tChnlID], iSucc * 2 );

        // 传输控制帧内存空间初始化
        char* tranCtrlBuf_p = new char[MC_DEF_MAX_LEN];
        memset ( tranCtrlBuf_p, 0, MC_DEF_MAX_LEN );
        tChnlObj_p->setTranCtrl ( tranCtrlBuf_p, MC_DEF_MAX_LEN );

        // 实时参数配置
        realTimeThreshold* tranCtrl_p = new realTimeThreshold;
        tranCtrl_p->Mode              = SEND_NO_CONTROL; // 目前104 的默认值，其他待定
        tranCtrl_p->K                 = 12; // 目前104的 默认值，其他协议待定
        tranCtrl_p->W                 = 8;//1;//
        tChnlObj_p->setTranCtrlCfg ( ( void* ) tranCtrl_p );

        // 实时传输值
        char* realTranCtrl_p = ( char* ) new realTimeValue();
        memset ( realTranCtrl_p, 0, sizeof ( realTimeValue ) );
        ( ( realTimeValue* ) realTranCtrl_p )->stage = chnlCfg.stage;
        tChnlObj_p->setRealParam ( realTranCtrl_p );

        // 规约库句柄
        HMODULE* tPtclMdl_p = tChnlObj_p->getPtclMdl();
        T_getPtclID tGetPtclID = ( T_getPtclID ) GetProcAddress ( *tPtclMdl_p, "getPtclID" );

        int iPtclID = tGetPtclID();

        /**/
        // 定时器 注：通道描述结构中相应定时器数组要赋值，和下面定时器要关联上
        u_short tmChnlID = tChnlID;
        int iMax = iPtclID == 104 ? MC_104_CHNL_TIME_MAX : 1; // 104 目前时钟数为 4 （a0,a1,a2,a3）
        iRet = tmFclty.addTmChnl ( iMax, &tmChnlID ); //this tm chnl only include 1 tmr.

        for ( int itm = 0; itm < iMax; itm++ )
        {
            tmCfg tTmrCfg;
            char tTmrNm[20];
            sprintf ( tTmrNm, "chnl time:%d\n", itm );
            tTmrCfg.ID             = itm; // 时钟ID 要唯一（一个通道内） ！！！，注意不要和应用级（会话）时钟重复
            tTmrCfg.name_p         = tTmrNm; // 定时器名字
            tTmrCfg.tmFunHdl       = TimerService;//ssnTmOutFun_p;    // 回调函数地址
            PDEVICES dev           = new DEVICES; // 目前104，定时器（0-3）在程序运行中间不会删除，所以此内存暂无释放！注！
            dev->chnlID            = tChnlID;
            dev->hIntance          = NULL;
            dev->type              = itm;
            tTmrCfg.tmFunHdlPara_p = ( void* ) dev; // 回调函数入参

            tTmrCfg.type = itm == 2 ? TM_PERIOD : TM_ONCE; // once
            tTmrCfg.fstXpirTm = itm == 0 ? 30 : ( itm == 1 ? 15 : ( itm == 2 ? 10 : ( itm == 3 ? 20 : 10 ) ) ); // 超时时间，目前默认10s，后续调整

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


        // 通道操作句柄初始化...
        //ctl stn--client;ctled stn--ser;cplx--client/ser
        if ( sckHandle_p == NULL )
        {
            // 一个系统一个，复合系统两个，暂时测试不带net不用，后续要使用
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

        // 初始化远程站
        stnDsc stn;
        memset ( &stn, 0, sizeof ( stnDsc ) );
        stn.chnlID  = tChnlID;
        stn.stnID   = tChnlID;
        stn.stnRole = chnlCfg.stnRole; //0x1-ctl stn;0x2-ctled stn;0x3-cplx

        // 将远程站加入到对象中（相当于入索引）
        stnObj.addPscStn ( &stn );

        stnDsc* tStnDsc = stnObj.getStnDsc ( stn.stnID );

        // 简易存储（元素类型）初始化，在远程站生成远程站描述符时初始化简易存储！！
        // 注：相应点值为初始化！
        stnObj.initDStorage ( tStnDsc, chnlCfg.cfgAddr_p, chnlCfg.cfgLen, chnlCfg.usrType );

        // 通道对象设置远程站对象指针
        tChnlObj_p->setStnObj ( &stnObj );

        //////////////////////////////////////////////////////////////////////////
        // 生成被动会话。。。
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
                tSsnReqBlk.ssnID      = /*i;//*/ssnMgr.getSsnID ( ptclKey ); // 会话序号
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
                tSsnReqBlk.ssnID     = i; // 会话序号
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

    thdObj_p = new nsCPThread::CThread[1];// 线程个数

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
    iRet = thdObj_p[0].StartThread ( ( FunTypeOne ) Et_sys_TakeErr ); // 优先级后续可以考虑提高
#else
    iRet = thdObj_p[0].StartThread ( ( FunTypeOne ) Et_sys_TakeErr, NULL, 1, 81, SCHED_OTHER ); // 优先级后续可以考虑提高
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
/*                     主动会话请求API                    */
/************************************************************/
//
//
////////主动会话请求API
////
//功能：请求会话，请求信息入队，发送请求事件。超时值为0时立即返回，否则挂起线程等待
//      会话返回。线程挂起后恢复执行的原因包括：1）应用帧发送线程得到会话结束信息，
//      并发送会话结束通知；2)超时时间到达，由系统内置的超时处理程序发送会话结
//      束通知。当超时值为0时，本API不负责处理会话结果，由系统线程接收该会话结束通知，
//      然后向监视子系统返回会话执行状态。
//输入：
//      reqInfo_p：主动会话的请求信息
//      ssnKey：会话描述，包括会话所在协议，会话在该协议中固定分配的会话号
//      snnStartPar_p：用作输入时，作为会话启动参数
//      timeout：会话的超时时间，=0时表示立即返回，非0时表示API调用者线程挂起的最长时间
//输出：
//      snnStartPar_p：用作输出时，返回会话最后一步的应用数据值，中间步返回的应用数据
//                  应通过存储子系统访问获得
//返回值：= 0，发起的主动会话成功完成；<0，发起的主动会话执行失败
//
//** 注：限制条件：同一会话且同一通道，只能属于一个用户线程 ！**
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
    // 请求入队,发送会话请求事件
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

    //超时发生时，超时处理程序需调用ssnDsc_p->setActSsnRslt(ssnReq.chanlID)设置会话结果
    //正常完成时，应用帧发送线程（结束块）中调用ssnDsc_p->setActSsnRslt(ssnReq.chanlID)
    //设置会话结果
    //以上两个程序都会调用事件通知过程，以解除本线程的挂起状态
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
/*                      应用帧发送线程                   */
/************************************************************/
//
//
//会话启动和步驱动函数的参数说明
//输入：
//      ssnNo：会话号
//      usingMode：会话的在用模式
//      startStepStnD_p：启动步站会话参数（站数据）指针（数据在实例控制块中），
//                       内部存储标志=0，指示是需要执行封装成ASDU发送的数据
//      nxtStnD_p：下一步站会话参数（站数据）指针（数据在实例控制块中）,内部存
//                 储标志=0，指示是需要执行保存的已解析的ASDU数据
//      preStepRul_p：前一会话步规则指针，包含APDU对象指针（每个会话的通道信息块中保存自己指针）
//输出：无
//返回：
//      指示分支跳转方向
typedef int ( *T_ssnStartF ) ( u_char ssnNo, u_char usingMode, ONE_STEP* preStepRul_p,  T_snnPara* startStepStnD_p );
typedef int ( *T_ssnStepF ) ( FUN_RL* Fun_p, ONE_STEP* preStepRul_p, T_ptclFunDsc* funDsc_p, T_snnPara* nxtStnD_p, u_int* nxtStepNo_p );
//typedef int (*ssnStepF)(FUN_RL* Fun_p, u_char ssnNo, u_char usingMode, ONE_STEP* preStepRul_p, T_snnPara* nxtStnD_p);
//
//
////////会话完成块
////功能:
//      1. 停止(复位)应用定时器
//      2. 删除主动会话使用的临时性资源
//      3. 发送会话完成通知，通知会话请求者
//           或发起者会话的完成状态(成功与否)
//           (linux发送消息,windows直接恢复会话请求线程)
//输入:
//       ssnCtlBlk_p:会话控制块指针
//            ssnNo:会话号(全局)
//       chnlNo:通道号
//       errCd:会话失败码:
/*      0x00 0000--会话成功完成
0x00 0001--出队错
0x00 0002--无法根据会话请求得到会话对象
0x00 0004--创建主动会话实例失败；
0x00 0008--无法根据会话请求得到会话实例使用的通道对象
0x00 0010--为请求的会话实例添加应用定时器通道失败
0x00 0020--调度到的实例无会话对象
0x00 0040--调度到的会话实例没有asdu(apdu)对象
0x00 0080--调度到的会话对象没有配置通道信息块
0x00 0100--调度到的会话实例没有分配通道对象
0x00 0200--调度到的会话实例没有对端端站对象
0x00 0400--调度到的会话实例应用级数据未通过检查验收
0x00 0800--调度到的会话实例应用级数据存储失败
0x00 1000--调度到的会话实例应用级数据加载失败
0x00 2000--调度到的会话实例应用级数据(信息对象)类型无法识别
0x00 4000--调度到的会话实例无法通过会话号获得功能号
0x00 8000--调度到的会话实例请求发送asdu入队失败
0x01 0000--调度到的会话实例会话驱动返回无法识别的分支向导
0x02 0000--主动会话响应超时
*/
//输出:无
//返回值:完成块执行状态，<0--失败；>=0--成功
int finishBlock ( ssnCtlBlk* ssnCtlBlk_p, T_TID trdID, u_short ssnID, u_short chnlNo, u_int errCd )
{
    C_SsnObj* ssnObj_p;
    u_int msgBdy[2];

    // 添加根据ssnID查找ssnCtlBlk_p 为U 帧(连接、关闭)处理结果上报做接口
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
        u_short usState = realTV_p->stage << 8; // 规约状态位
        usState |= realTV_p->stage == 1 ? MC_CHNL_BIT : 0; // 通道状态位同时赋值
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
    // 应用级定时器单独删除，因为并没有放到通道定时器链表中，所以一下代码不需要，modify by zhaojj，20170602
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
    int iPtclID = tGetPtclID();//获取协议号

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
    T_createRl_P tCrt_FunRl_F = ( T_createRl_P ) GetProcAddress ( *tModule_p, "createRl_P" ); //需修改为win,lnx通用形式
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
    //获取规约库功能号
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

    // 读取配置，如没有，则调用规约库创建，使用默认值！目前使用默认配置
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
            tChnlInfoBlk_p->funRl_p = tCrt_FunRl_F ( ssnDscNo, tChnlInfoBlk_p->ssnRlMode, tChnlInfoBlk_p->ssnStnCnfg_p, &tPtclFun ); // 104 usingMode=0; 此值有配置给出，并且一个会话此值固定

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

    // asdu for chnl's info_blk 默认规则下APDU没有不同暂时不需要
    APDU_DSCR tmpApduDscr;
    memset ( &tmpApduDscr, 0, sizeof ( APDU_DSCR ) );
    tmpApduDscr.ASDU_TID = 0;//100 // 默认规则不用，配置子系统时 要注意根据会话号不同而区别

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
    ssnCtlBlk_p->drvStepNo = 1; // 步号起始值是 1，不可为零
    ssnCtlBlk_p->hostTID = 0;
    ssnCtlBlk_p->checkStd = 0;
    ssnCtlBlk_p->retryNum = 0; // 暂时不用
    ssnCtlBlk_p->ssnTmout = 0;//300; // second

    ssnCtlBlk_p->drvPrmtr_p->reqPDU_Num  = 0;
    ssnCtlBlk_p->drvPrmtr_p->respPDU_Num = 0;
    ssnCtlBlk_p->drvPrmtr_p->stepNo      = ssnCtlBlk_p->drvStepNo;

    return 0;
}

// 创建会话实例，添加到调度表中
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
    int iPtclID = tGetPtclID();//获取协议号

    T_createRl_P tCrt_FunRl_F = ( T_createRl_P ) GetProcAddress ( *tModule_p, "createRl_P" ); //需修改为win,lnx通用形式
    T_createAPDU crtAPDU_f = ( T_createAPDU ) GetProcAddress ( *tModule_p, "createAPDU" );

    if ( tCrt_FunRl_F == NULL || crtAPDU_f == NULL )
    {
        return -10;
    }

    //!创建会话实例时，将会话参数拷贝到会话实例控制
    //块中会话对象提供获取会话启动参数功能
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
    tChnlInfoBlk.ssnRlMode = 0xFF; // 默认值为 8位全1，使用默认规则给出的值
    u_short ssnDscNo       = ssnMgr.getSsnDscNo ( tSsnReqBlk.ssnID );
    // snnCfg
    //tChnlInfoBlk.ssnStnCnfg_p = new T_snnCfg();
    //memset(tChnlInfoBlk.ssnStnCnfg_p, 0, sizeof(T_snnCfg));
    //tChnlInfoBlk.ssnStnCnfg_p->actFlag = actFlag;
    //tChnlInfoBlk.ssnStnCnfg_p->infoType = 0;
    //tChnlInfoBlk.ssnStnCnfg_p->ssnID = tSsnReqBlk.ssnID;
    tChnlInfoBlk.ssnStnCnfg_p = cfgMng->getSnnCfg ( ssnDscNo, tSsnReqBlk.chnlID );
    setSsnCfg ( tChnlInfoBlk.ssnStnCnfg_p, tSsnReqBlk.addrStart, tSsnReqBlk.accumValue, iPtclID );
    // 读取配置，如没有，则调用规约库创建，使用默认值！目前使用默认配置
    ssnSets* tSsnSets_p = tChnlObj_p->getSsnSet ( tSsnReqBlk.ssnID, actFlag ); // 此项要与该通道支持的会话数相对应

    if ( tSsnSets_p == NULL )
    {
        tErrFlag = tErrFlag | 0x000004;
        return tErrFlag;
    }

    if ( tSsnSets_p->setNum == 0 && tSsnSets_p->stepDsc_p == NULL )
    {
        T_ptclFunDsc tPtclFun  = * ( cfgMng->getFunDsc ( ssnDscNo, tSsnReqBlk.chnlID ) ); // 暂时不用给值，dll库接口会给默认值
        tChnlInfoBlk.ssnRlMode = tPtclFun.usingMode;
        tChnlInfoBlk.funRl_p = tCrt_FunRl_F ( ssnDscNo, tChnlInfoBlk.ssnRlMode, tChnlInfoBlk.ssnStnCnfg_p, &tPtclFun ); // 104 usingMode=0; 此值有配置给出，并且一个会话此值固定
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
    ////104配置
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
            // 不删除，只改变被动会话状态，同时重置相应字段值
            // tCurSsnObj_p->delPassiveSsn(tSsnCtlBlk_p);
        }

        // ...
    }

    //
    tSsnCtlBlk_p->chnlID          = tSsnReqBlk.chnlID;
    tSsnCtlBlk_p->myChnlInfoBlk_p = tmpChnlInfoBlk_p;//tCurSsnObj_p->getChnlInfoBlk(tSsnReqBlk.chnlID);
    tSsnCtlBlk_p->actFlg          = actFlag;
    tSsnCtlBlk_p->drvStepNo       = 1; // 步号起始值是 1，不可为零
    tSsnCtlBlk_p->hostTID         = tSsnReqBlk.tID;
    tSsnCtlBlk_p->checkStd        = 0;
    tSsnCtlBlk_p->retryNum        = 0; // 暂时不用
    tSsnCtlBlk_p->role            = tSsnReqBlk.role;
    tSsnCtlBlk_p->ssnNo           = tSsnReqBlk.ssnID;
    tSsnCtlBlk_p->ssnTmout        = tSsnReqBlk.timeout;

    //tSsnCtlBlk_p->ssnPara_p = g_snnPara_p;                    // test, get global's var,

    // init the one_step's two apdu
    tSsnCtlBlk_p->drvPrmtr_p->reqAPDU_Obj_p  = tSsnCtlBlk_p->myChnlInfoBlk_p->reqAPDU_Obj_p;
    tSsnCtlBlk_p->drvPrmtr_p->respAPDU_Obj_p = tSsnCtlBlk_p->myChnlInfoBlk_p->respAPDU_Obj_p;
    // ...  注册APDU到通道信息块，及ASDU长度赋值到会话实例块中

    // 注：ASDU 长度要和ASDU队列的数据长度一致！！
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
            //  tRtn = tmFclty.addTmChnl(1, &tmChnlID);//this tm chnl only include 1 tmr.// 应用级定时器不添加到通道定时器中

            //printf("----addTmChnl ret=%d\r\n", tRtn);

            tTmrCfg.ID = 0;//lzq, 2017.1.25
            char tTmrNm[20];
            sprintf ( tTmrNm, "act ssnID:%d\n", tSsnReqBlk.ssnID );
            tTmrCfg.name_p         = tTmrNm;
            tTmrCfg.tmFunHdl       = ssnTmOutFun_p;
            tTmrCfg.tmFunHdlPara_p = ( void* ) tSsnCtlBlk_p;
            tTmrCfg.type           = 0;//once
            tTmrCfg.fstXpirTm      = tSsnReqBlk.timeout;
            // 应用级定时器操作归应用，不参与到通道定时器中，20170601，by zhaojj
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

//执行过程中错误码定义:
//0x00 0001--出队错
//0x00 0002--无法根据会话请求得到会话对象
//0x00 0004--创建主动会话实例失败；
//0x00 0008--无法根据会话请求得到会话实例使用的通道对象
//0x00 0010--为请求的会话实例添加应用定时器通道失败
//0x00 0020--调度到的实例无会话对象
//0x00 0040--调度到的会话实例没有asdu(apdu)对象
//0x00 0080--调度到的会话对象没有配置通道信息块
//0x00 0100--调度到的会话实例没有分配通道对象
//0x00 0200--调度到的会话实例没有对端端站对象
//0x00 0400--调度到的会话实例应用级数据未通过检查验收
//0x00 0800--调度到的会话实例应用级数据存储失败
//0x00 1000--调度到的会话实例应用级数据加载失败
//0x00 2000--调度到的会话实例应用级数据(信息对象)类型无法识别
//0x00 4000--调度到的会话实例无法通过会话号获得功能号
//0x00 8000--调度到的会话实例请求发送asdu入队失败 
//0x01 0000--调度到的会话实例会话驱动返回无法识别的分支向导
//0x02 0000--主动会话响应超时
//0x04 0000--绑定信息对象失败
//--checkstate's code-------------------------------------------
//0x00 0000--正确接收
//0x80 0000--等待接收端响应 20170324
//0x80 0000--等待接收端响应 20170324


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
            // 创建会话，主动
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

        if ( !tmpSsnCtlBlk ) //无法返回错误信息
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

            tASDU_p = tCurSsnObj_p->getAPDU ( tSsnInstInfoSet.actFlg, tChnlInfoBlk_p ); //tCurSsnObj_p->getAPDU(tSsnInstInfoSet.actFlg, tSsnInstInfoSet.chnlID);//->m_snnDescript.ASDU_p;//（parameter: ssnID,activity, chnlID,return apduObj_p）

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

        // t通道状态错误的情况下，只容许U帧通信
        if ( ssnMgr.getSsnPtclNo ( tSsnID ) < U_STARTDT ) // 目前104 u帧协议号
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

                if ( tSsnInstInfoSet.drvStepNo != tSsnInstInfoSet.drvPrmtr_p->stepNo // 由原来的帧加载，改成步加载(by zhaojj,20170303)
                     || ( tSsnInstInfoSet.drvStepNo == 1 && iCurNum == 0 ) ) // 第一步，第一帧
                {
                    switch ( tSsnInstInfoSet.ssnPara_p->infoType )
                        // 改变简介：步加载-》帧加载-》不加载，20170504
                    {
                        case 0x0:
                            loadPnt_F = ( T_loadPnt ) GetProcAddress ( *tModule_p, "getGLoadPnt" ); //需修改为win,lnx通用形式
                            break;

                        case 0x1:
                            loadPnt_F = ( T_loadPnt ) GetProcAddress ( *tModule_p, "getFLoadPnt" ); //需修改为win,lnx通用形式
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

                    if ( tFunDsc.chnlRole == MASTER_STATION ) // 控制站（主站）
                    {

                        tFunDsc.frmDirect = MC_CONTROL_DIRECTION ;//控制方向

                    }
                    else //被控制站（子站）
                    {
                        tFunDsc.frmDirect =  MC_MONITOR_DIRECTION ;//监视方向

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

                if ( tSsnInstInfoSet.drvStepNo == 1 && iCurNum == 0 && tSsnInstInfoSet.actFlg == MC_ACT_FLAG ) //startStep: drvStepNo = 0, 第一帧
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
            asduUnit.DU_type = tSsnCtlBlk_p->ptclCtlCd; // 帧类型

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
                        // 暂时只使用于104，例如是101的重试情况要特殊处理
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
/*                      应用帧接收线程                   */
/************************************************************/
//
//
////////应用帧接收线程
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
        // step 1.取ASDU帧和通道（组）号
        tChnlID = tASDUInfoCp.chanlID;
        tChnlObj_p = chnlMgr.getChnlObj ( tChnlID );

        if ( !tChnlObj_p )
        {
            exit ( 2 );
        }



        if ( ( tASDUInfoCp.ASDU_p ) /*&& (tASDUInfo.ASDU_Lng)*/ ) //non pseudo ASUD! // U 帧数据有ASDU，没长度
        {
            // step 2.获取规约库模块句柄
            tModule_p = ( HMODULE* ) ( tChnlObj_p->getPtclMdl() );

            // step 3.ASDU检查解析

            tChk_ASDU_F = ( T_chk_ASDU ) GetProcAddress ( *tModule_p, "chkASDU" ); // 需修改为win,lnx通用形式

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
                //modbus需获取站角色初始化回话主动性（2017.06.23）
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

                // step 4.获取可能的会话KEY集合
                tGetSsnnSet_F  = ( T_getSsnSet ) GetProcAddress ( *tModule_p, "getSsnSet" ); // 需修改为win,lnx通用形式
                // set tMaybeFunSet[].ssnDscNo(fun ID in ptcl) and tMaybeFunSet[].act_prio_chnlID(actFlag)
                tSsnKey.ptclNo = tGetSsnnSet_F ( tTID, tCOT, tMaybeFunSet, &tFunNum ); // max fun number =4，最后一个保留给通道号（框架）

                //printf("tGetSsnnSet_F,tTID=%d \r\n",tTID);
                if ( 0xFF != tSsnKey.ptclNo ) //get maybe Fun set
                {
                    // step 5.获取可能的会话号集合
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

        //step 6.会话派发
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

        // step 7.获取会话步
        // dispacth snn: tMaybeSsnSet[tIndex]

        tSsnID = tMaybeSsnSet[tIndex].ssnDscNo;
        C_SsnObj* tSsnObj_p;
        tSsnObj_p = ssnMgr.getSsnObj ( tSsnID );

        //
        if ( tSsnCtlBlk_p->actFlg == MC_PSV_FLAG )
        {
            //会话控制块加锁
            //tSsnCtlBlk_p->ssnCtlLock.Lock();
            //printf("Et_sys_AppRec::tSsnCtlBlk_p->ssnCtlLock.Lock(),"
            //        "tSsnCtlBlk_p->rtRunStOrRslt=[%d]\r\n",
            //        tSsnCtlBlk_p->rtRunStOrRslt);

            // 初始化被动会话
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

        // bindToMem // U帧，此项不成立！！！
        //      assert ((tASDUInfo.ASDU_p != NULL) && (tASDUInfo.ASDU_Lng > 0));
        ssnSets* tmpSsnSet_p = tChnlObj_p->getSsnSet ( tSsnCtlBlk_p->ssnNo, tSsnCtlBlk_p->actFlg );
        tMaybeFunSet[tIndex].remoteSet_p = tmpSsnSet_p->stepDsc_p;
        tMaybeFunSet[tIndex].setNum = tmpSsnSet_p->setNum;
        tMaybeFunSet[tIndex].ssnDscNo = ssnMgr.getSsnDscNo ( tMaybeSsnSet[tIndex].ssnDscNo );

        tMaybeFunSet[tIndex].act_prio_chnlID = tMaybeSsnSet[tIndex].act_prio_chnlID;
        tMaybeFunSet[tIndex].realTID = ( u_char ) tTID;
        tMaybeFunSet[tIndex].chnlRole = chnlRole;
        T_bindToMem tBindToMem_F = ( T_bindToMem ) GetProcAddress ( *tModule_p, "bindToMem" ); //需修改为win,lnx通用形式
        tRtn = tBindToMem_F ( tSsnCtlBlk_p->drvPrmtr_p, &tMaybeFunSet[tIndex], /*tAPDU1Obj_p, */tASDUInfoCp.ASDU_p, tASDUInfoCp.ASDU_Lng, 1 );

        //printf("tBindToMem_F,tTID=%d \r\n",tTID);
        if ( tRtn != 0 )
        {
            cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "RecBindError = %d\r\n", tRtn );
            // -1:APDU_lng's err; -2: VSQ 限定词长度错误;
            ssnDuQ.pshQ ( SCD_GROUP, 0, NULL, 0 ); //send ready inform
            continue;
        }

        T_getFunStep tGetFunStep_F;
        tGetFunStep_F = ( T_getFunStep ) GetProcAddress ( *tModule_p, "getFunStep" ); //需修改为win,lnx通用形式

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
        tChkFun_F = ( T_chkFun ) GetProcAddress ( *tModule_p, "chkFun" ); //需修改为win,lnx通用形式

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

        //step 10.发送会话就绪通知
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

    // 要求：时间要足够大，才能够保证
    u_short timeout = MC_DEF_SSN_TM_OUT /*+ tReqInfo.chnlID*100*/; // 单位:秒(s) // 会话超时时间
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
            // 每个通道发送 I 帧数据前都要 先连接和START
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

        //遍历所有物理量数据
        for ( u_int i = 0; i <  chnlCfg->phyTypeNum; i++ )
        {
            recvDataBuf[tReqInfo.chnlID].datalock.Lock();
            char tDIdBinStr_a[MC_DIDBIN_LEN] = {0};
            T_phyValCfg dtCfg ;
            memset ( &dtCfg, 0, sizeof ( T_phyValCfg ) );
            T_phyNCfg   dataCfg;//当前通物理量的配置
            memset ( &dataCfg, 0, sizeof ( T_phyNCfg ) );
            T_phyObjVal daVal;
            memset ( &daVal, 0, sizeof ( T_phyObjVal ) );
            T_phyNVal   dtValue;//要取的通道的物理量数据
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
            //找到想应物理量对应的点地址
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
    //分配did二进制串空间
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
            T_phyNVal   dtValue;//要取的通道的物理量数据
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
    const int tSndTmOut = 50000;                        //发送超时时间
    const int tRcvTmOut = 10000;                        //接收超时时间
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

    //打印id buffer
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
    //测试打印当前时间
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

    // 应用级定时器操作归应用，不参与到通道定时器中，20170601，by zhaojj
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
    const int tOneMinSec = 60; //1分钟=60秒
    const int tFivMinSec = 300;//5分钟=300秒
    const int tFifMinSec = 900;//15分钟=900秒

    tvSec = time ( NULL );
    tvSec = tvSec + MC_TMSTMP_OFFSET;//加上时间偏移

    //当前时间对应的初始值
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
        tvSec = tvSec + MC_TMSTMP_OFFSET;//加上时间偏移

        DATAFORMAT_DATE7B1 lcalTmStmp = {0};

        //判断整分钟
        if ( ( tvSec / tOneMinSec ) != tvMin1 )
        {
            tvMin1 = tvSec / tOneMinSec;
            phyCatName_p[0] = 1;
            cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_1, "1 minute data !\r\n" );
            //取得本地时标
            getSysTime ( lcalTmStmp, tvSec );
            //debug 输出 测试
            cfgMng->tryLog.LPrintf ( DEBUG,
                                     DEBUG_LEVEL_1,
                                     "lcalTmStmp=[%d/%d/%d %d:%d:%d] !\r\n",
                                     lcalTmStmp.year, lcalTmStmp.month,
                                     lcalTmStmp.day, lcalTmStmp.hour,
                                     lcalTmStmp.minute, lcalTmStmp.second
                                   );
        }
        else //不满整分钟
        {
            phyCatName_p[0] = 0;
        }

        //判断整5分钟
        if ( ( tvSec / tFivMinSec ) != tvMin5 )
        {
            tvMin5 = tvSec / tFivMinSec;
            phyCatName_p[1] = 5;
            cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_1, "5 minute data !\r\n" );
        }
        else //不满整5分钟
        {
            phyCatName_p[1] = 0;
        }

        //判断整15分钟
        if ( ( tvSec / tFifMinSec ) != tvMin15 )
        {
            tvMin15 = tvSec / tFifMinSec;
            phyCatName_p[2] = 15;
            cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "15 minute data !\r\n" );
        }
        else //不满整15分钟
        {
            phyCatName_p[2] = 0;
        }

		vBusFlag = false;
        //查看并处理符合时间要求的数据
        for ( int timerNum = 0; timerNum < TIMER_PUT_DB_NUM; timerNum++ )
        {

            if ( phyCatName_p[timerNum] <= 0 )
            {
                //还没有到相应分钟数的数据处理时间
                continue;
            }

            /************phyCatNameArr[timerNum]分钟的处理************************/
            for ( int i = 0 ; i < MC_DEF_STN_NUM ; i++ )
            {
                ids[timerNum][i].Reset();
                idsAvg[timerNum][i].Reset();
                chnlObjCfg* chnlCfg = & ( cfgMng->getchnlObjCfg() [i] );

                if ( chnlCfg->putStagFlag != 1 && chnlCfg->putStagFlag != 3 )
                {
                    continue;
                }

                //记录通道时标
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

                    T_phyNCfg   dataCfg;//当前通物理量的配置
                    memset ( &dataCfg, 0, sizeof ( T_phyNCfg ) );
                    dataCfg.phyValCfg_p = &dtCfg;
                    dataCfg.phyUnitNum = 1;

                    T_phyObjVal daVal;
                    double phyDtValue = 0;
                    memset ( &daVal, 0, sizeof ( T_phyObjVal ) );
                    daVal.pntObj_p = NULL;
                    daVal.phyObjVal_p = &phyDtValue;
                    daVal.phyType = typeBuf_p[j];

                    T_phyNVal   dtValue;//要取的通道的物理量数据
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

        //vbus 获得权限
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
                //还没有到相应分钟数的数据处理时间
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
                if( tIDbufLng <=0 )//ids buffer中没有值
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
                    //抹去秒值
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
    const int tSndTmOut          = 50000;                       //发送超时时间
    const int tRcvTmOut          = 10000;                       //接收超时时间
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
    const int tOneMinSec = 60; //1分钟=60秒
    tvSec = time ( NULL );
    tvSec = tvSec + MC_TMSTMP_OUTOFFSET;//加上时间偏移
    //当前时间对应的初始值
    tvMin1   = tvSec / tOneMinSec;

    DBuffer tDiaBuffer;//调用Diagnose进行打印信息处理的DBuffer

    while ( 1 )
    {
        nsCPPub::SleepSecond ( 1 );
        tvSec = time ( NULL );
        tvSec = tvSec + MC_TMSTMP_OUTOFFSET;//加上时间偏移

        DATAFORMAT_DATE7B1 lcalTmStmp = {0};

        //判断整分钟
        if ( ( tvSec / tOneMinSec ) != tvMin1 )
        {
            tvMin1 = tvSec / tOneMinSec;
            cfgMng->tryLog.LPrintf ( DEBUG,
                                     DEBUG_LEVEL_1,
                                     "Et_sys_DataOutStor 1 minute out database !\r\n" );
            //取得本地时标
            getSysTime ( lcalTmStmp, tvSec );
            //debug 输出 测试
            cfgMng->tryLog.LPrintf ( DEBUG,
                                     DEBUG_LEVEL_1,
                                     "Et_sys_DataOutStorlcalTmStmp=[%d/%d/%d %d:%d:%d] !\r\n",
                                     lcalTmStmp.year, lcalTmStmp.month,
                                     lcalTmStmp.day, lcalTmStmp.hour,
                                     lcalTmStmp.minute, lcalTmStmp.second
                                   );
            //取最新有数据的时间
            DValue::SetDateMax(&(reqPktDF.dStartTm), &(reqPktDF.dEndTm));

            ////给开始时间赋值
            //reqPktDF.dStartTm.year   = lcalTmStmp.year;
            //reqPktDF.dStartTm.month  = lcalTmStmp.month;
            //reqPktDF.dStartTm.day    = lcalTmStmp.day;
            //reqPktDF.dStartTm.hour   = lcalTmStmp.hour;
            //reqPktDF.dStartTm.minute = lcalTmStmp.minute;
            //reqPktDF.dStartTm.second = 0;
            ////给结束时间赋值
            //reqPktDF.dEndTm.year   = lcalTmStmp.year;
            //reqPktDF.dEndTm.month  = lcalTmStmp.month;
            //reqPktDF.dEndTm.day    = lcalTmStmp.day;
            //reqPktDF.dEndTm.hour   = lcalTmStmp.hour;
            //reqPktDF.dEndTm.minute = lcalTmStmp.minute;
            //reqPktDF.dEndTm.second = 0;
        }
        else //不满整分钟
        {
            continue;
        }

		vBusFlag = false;

        for ( int i = 0 ; i < MC_DEF_STN_NUM ; i++ )
        {
            /**********************这里需要修改*******************/
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
        
        //vbus 获得权限
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

                T_phyNCfg   dataCfg;//当前通物理量的配置
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
                T_phyNVal   dtValue;//要取的通道的物理量数据
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
错误处理线程
负责收到时间通知后，收取信息数据，根据信息做后续处理
信息：操作码、通道号、会话号、错误码。。。
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
        // FORMAT: （低）CHNL_ID, CODE（高）
        u_int errMsg[2] = { 0 };

        // 1 等事件，特定事件; 信息包格式：
        // 2 收信息包

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

        // 3 解析数据包
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

        // 获取协议号ID
        DLL_getPtclID getPtclID;
        getPtclID = ( DLL_getPtclID ) GetProcAddress ( *ptclMdl_p, "getPtclID" );

        if ( !getPtclID )
        {
            cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "Error: Can't not load function getPtclID()!\n" );
            return;
        }

        int iPtclID = getPtclID();

        // 4 根据规约库类型（104）及操作码相应处理错误
        // 低第1位（位0）：至1为关闭定时器; 低第2位（位1）：至1为关闭链接
        // 低第3位（位2）：至1为发送S帧; 低第4位（位3）：至1为发送测试激活，其他位保留！
        if ( ( iCode & 0x1 ) && ( iCode & 0x2 ) )
        {
            tmChnl* tmChnl_p = NULL;
            tChnlObj_p->getTimer ( tmChnl_p );

            for ( int itm = 0; tmChnl_p && itm < tmChnl_p->tmrNum; itm++ )
            {
                tmFclty.ctlTmr ( tmChnl_p, itm, TM_STOP );
            }

        }

        if ( ( iCode & 0x2 ) && iRole == MC_CTL_STN ) // 主站主动重新建立连接
        {
            realTimeValue* realParam_p = ( realTimeValue* ) ( tChnlObj_p->getRealParam() );
            memset ( realParam_p, 0, sizeof ( realTimeValue ) );
            reqInfo tReqInfo;
            tReqInfo.chnlID = iChnlId;
            tReqInfo.role = 0;
            tReqInfo.snnMaxRetryNum = 0;

            // 协议配置
            ssnReqCnt ssnKey;
            ssnKey.ssn.ptclNo = iPtclID; // 协议号
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
            // 要求：时间要足够大，才能够保证
            u_short timeout = 9;//0;//小于T1--15s     // 单位:秒(s) // 会话超时时间
            int iCnt = 0;
            bool bFlag = true;

            while ( bFlag )
            {
                // 每个通道发送 I 帧数据前都要 先连接和START
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

            // 协议配置
            ssnReqCnt ssnKey;
            ssnKey.ssn.ptclNo = iPtclID; // 协议号

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

            // 要求：时间要足够大，才能够保证
            u_short timeout = 9;//0;//14;//小于T1--15s        // 单位:秒(s) // 会话超时时间
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
        else if ( ( iCode & 0x2 ) && iRole == MC_CTLED_STN ) // 主站主动重新建立连接
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
//  HMODULE* commMdl_p = (HMODULE*)(tChnlObj_p->getCommMdl());//公共通讯协议描述符
//  HMODULE* ptclMdl_p = (HMODULE*)(tChnlObj_p->getPtclMdl());//规约描述;
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
//  //结构会修改
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
        return NULL;//addr未找到
    }

    int memst = addr - datBuff_p->addrIdx_p[mem].begAddr;


    return  & ( datBuff_p->buf_pp[mem][memst] );

}


//一次写多个点对象snnUsrData_p到缓冲区中
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

    //有的点地址在缓冲区中没有找到
    if ( bugFlag == 1 )
    {
        return 1;
    }

    return 0;
}



/*
    功能:从共用内存中取某些物理量对象
         转换成要发送通道的点对象数据存
         入发送通道缓冲区
    输入:接口的所有参数皆为输入参数
    输出:无
    返回:0   成功
         非0 失败( 其中返回1为物理量的部分点对象写入成功)
*/
int convPhyToSndChn ( C_unitInfData* unitInfData_p,  /*共用内存对象*/
                      const u_int& getChnNum,        /*要取值的通道号*/
                      const u_int& writeChnNum,      /*要写入的通道号*/
                      const PHY_UNIT_TYPE* phyType_p,  /*要操作的物理量phyType_p[]*/
                      const u_int& phyNum            /*要操作的物理量个数*/
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
    const u_int maxNum     = 20;/*最大支持一次处理物理量的个数*/
    const u_int maxLng     = 10;/*最大支持的物理量值的长度*/
    const u_int maxAddNum  = 10;/*最大支持的一个物理量对应的点地址数据*/
    const u_int maxDidLng  = sizeof ( DATA_ID ); /*数据id的最大长度*/

    if ( phyNum > maxNum )
    {
        return -2;
    }

    //组织要取值的数据结构 T_phyNVal,T_phyNCfg
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
        /*根据要取值与要发送的通道号及配置的
          点地址数量来转换成要发送通道的点地
          址数据:*/
        if ( getChnNum == writeChnNum )
        {
            //要取值与要发送的通道是同一个时:
            //不需要转换
        }
        else /*取与发不是同一个通道*/
        {
            //物理量对应的点地址数量一样
            if ( phyObj_p[i].numOfAdd == phyValCfg_p[i].numOfAdd )
            {
                //替换点地址
                for ( u_int j = 0; j < phyObj_p[i].numOfAdd; j++ )
                {
                    pntObj_p[i][j].gData.val_chkStd.pntAddr = phyValCfg_p[i].pntAddr_p[j];
                }
            }
            else /*物理蛳对应的点地址数量不一致*/
            {
                //1 调用接口转换物理量到点地址值
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

        //把值写入到发送内存中
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
    功能:把一帧数据的点值转换成物理量值存入共用内存单元
    输入:ssnPara_p 一帧数据对应的结构
        :chnId    通道id
    输出:unitInfData_p 共用内存对象
    返回:0   成功
         非0 失败
*/
int writeFrmPntToUMem ( const T_snnPara* ssnPara_p,   /*一帧数据对应的结构*/
                        const u_int& chnId,            /*通道号*/
                        C_unitInfData* unitInfData_p  /*共用内存对象*/
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
        //一般数据帧长度为256 超过300则需要修改maxPntNum此值
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

    //取物理量总个数
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

    //物理量的个数大于maxPntNum:300则需要修改maxPntNum的值
    if ( tOutNum > maxPntNum )
    {
        return -3;
    }

    //没有对应的物理量
    if ( tOutNum == 0 )
    {
        return 0;
    }

    const u_short numOfAdd = 16;//一个物理量最大对应16个点,多于此值应修改此值
    char tDIdBinStr_aa[maxPntNum][MC_DIDBIN_LEN];
    u_int tPntAddr_aa[maxPntNum][numOfAdd];
    memset ( tDIdBinStr_aa, 0, sizeof ( tDIdBinStr_aa ) );
    memset ( tPntAddr_aa, 0, sizeof ( tPntAddr_aa ) );

    for ( u_short i = 0; i < maxPntNum; i++ )
    {
        tOutPhyValCfg_a[i].pntAddr_p = tPntAddr_aa[i];
        tOutPhyValCfg_a[i].dataId_p = tDIdBinStr_aa[i];
    }

    //通过一帧中的点,找到所有对应物理量的配置
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

    //计算tOutNum个物理量的值
    for ( u_short i = 0; i < tOutNum; i++ )
    {
        if ( tOutPhyValCfg_a[i].numOfAdd > numOfAdd )
        {
            //一个物理量最大对应16个点,多于此值应修改此值
            return -4;
        }

        tPhyObjVal_a[i].phyObjVal_p = & ( tPhyVal[i] ); //第i个物理量的值
        tPhyObjVal_a[i].phyType = tOutPhyValCfg_a[i].phyType;//第i个物理量的类型

        T_snnUsrData* usrData_p = NULL;

        //找到物理量对应的点T_snnUsrData结构
        for ( u_short j = 0; j < pntNum ; j++ )
        {
            usrData_p = & ( ssnPara_p->usrData_p[j] );

            if ( tOutPhyValCfg_a[i].pntAddr_p[0] == usrData_p->gData.val_chkStd.pntAddr )
            {
                tPhyObjVal_a[i].pntObj_p = usrData_p;
                break;
            }

        }

        if ( tPhyObjVal_a[i].pntObj_p == NULL ) //没有找到点地址配置
        {
            return -5;
        }

        //计算第i个物理量的值
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
                return -1;//addr未找到
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
                return -1;//addr未找到
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
                    return -1;//addr未找到
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
                    return -1;//addr未找到
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
    * 短期单机数据表——
    * 风机编号  ：MakeEncoding(ENCODETYPE_TURBINE,sn,16,x); *
    * 入库日期  ：MakeDate(DATACATALOG_SD,x);               *
    * 数据日期  ：MakeData(DATACATALOG_DD,x);

    * 风速      ：MakeMeteorSingle()
    * 风向      ：MakeMeteorSingle()
    * 温度      ：MakeMeteorSingle()
    * 空气密度  ：MakeMeteorSingle()
    * 标准功率  ：MakePeriodSingle()
    * 平均功率  ：MakePeriodSingle()
    * 最大功率  ：MakePeriodSingle()
    * 最小功率  ：MakePeriodSingle()
    * 平均发电量：MakePeriodSingle()*/
    DID did;
    DID didClear;
    didClear.Clear();
    int turbineH = 0;//风机轮毂高度
    float amtH = 10.0;//测风塔高度
    int turbineNum = 0;//风机编号

    /////////////////////////////大泉子风机/////////////////// chnl 的值 srcsn = 0
    /////////////////////////////大个岭风机/////////////////// chnl 的值 srcsn = 1
    if ( chnl == 0 || chnl == 1 ) //风机
    {
        int turbineIndex = 0;

        if ( 4 == funcId ) //功能码为4
        {

            for ( int i = 0 ; i < 33 ; i ++ ) //单风机循环
            {
                if ( chnl == 0 )
                {
                    turbineNum = i;
                }

                if ( chnl == 1 )
                {
                    turbineNum = i + 33;
                }

                //风速
                dataId[turbineIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_WS,
                                       /*B_Kind*/DATAKIND_AVG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                                       /*代表所属设备编号*/ENCODETYPE_TURBINE, turbineNum, /*数据来源*/SOURCETYPE_TSS, chnl, HEIGHTTYPE_HUB, turbineH,
                                       /*时间间隔*/INTERVALUNIT_MINUTE, time );
                turbineIndex ++;
                //风向
                dataId[turbineIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( DATACATALOG_WD,
                                       DATAKIND_AVG, DATATYPE_FLOAT32, did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                                       ENCODETYPE_TURBINE, turbineNum, SOURCETYPE_TSS, chnl, HEIGHTTYPE_HUB, turbineH,
                                       INTERVALUNIT_MINUTE, time );
                turbineIndex ++;
                //有功功率
                dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_PP,
                                       /*B_Kind*/DATAKIND_AVG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                       /*encoding type*/ENCODETYPE_TURBINE, /*ecsn*/turbineNum, /*B_Source*/SOURCETYPE_TSS, /*srcsn*/chnl,
                                       /*B_Interval*/INTERVALUNIT_MINUTE, time );
                turbineIndex ++;
                //无功功率
                dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_PQ,
                                       /*B_Kind*/DATAKIND_AVG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                       ENCODETYPE_TURBINE, turbineNum, /*B_Source*/SOURCETYPE_TSS, chnl,
                                       /*B_Interval*/INTERVALUNIT_MINUTE, time );
                turbineIndex ++;

                //转速
                dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_RS,
                                       /*B_Kind*/DATAKIND_BLD, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                       ENCODETYPE_TURBINE, turbineNum, /*B_Source*/SOURCETYPE_TSS, chnl,
                                       /*B_Interval*/INTERVALUNIT_MINUTE, time );
                turbineIndex ++;
            }//单风机循环

            //整风场有功功率
            dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_PP,
                                   /*B_Kind*/DATAKIND_AVG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                   ENCODETYPE_FARM, turbineNum, /*B_Source*/SOURCETYPE_IAS, chnl,
                                   /*B_Interval*/INTERVALUNIT_MINUTE, time );
            turbineIndex ++;

            //整风场无功功率
            dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_PQ,
                                   /*B_Kind*/DATAKIND_AVG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                   ENCODETYPE_FARM, turbineNum, /*B_Source*/SOURCETYPE_IAS, chnl,
                                   /*B_Interval*/INTERVALUNIT_MINUTE, time );
            turbineIndex ++;

            //整风场日发电量
            dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_GE,
                                   /*B_Kind*/DATAKIND_AMV, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                   ENCODETYPE_FARM, turbineNum, /*B_Source*/SOURCETYPE_IAS, chnl,
                                   /*B_Interval*/INTERVALUNIT_MINUTE, time );
            turbineIndex ++;

            //整风场正常发电台数
            dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_TA,
                                   /*B_Kind*/DATAKIND_PGG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                   ENCODETYPE_FARM, turbineNum, /*B_Source*/SOURCETYPE_IAS, chnl,
                                   /*B_Interval*/INTERVALUNIT_DAY, time );
            turbineIndex ++;
            //整风场正常发电容量
            dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_CP,
                                   /*B_Kind*/DATAKIND_PGG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                   ENCODETYPE_FARM, turbineNum, /*B_Source*/SOURCETYPE_IAS, chnl,
                                   /*B_Interval*/INTERVALUNIT_DAY, time );
            turbineIndex ++;

            //整风场限功率台数
            dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_TA,
                                   /*B_Kind*/DATAKIND_PGGTHV, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                   ENCODETYPE_FARM, turbineNum, /*B_Source*/SOURCETYPE_IAS, chnl,
                                   /*B_Interval*/INTERVALUNIT_DAY, time );
            turbineIndex ++;
            //整风场限功率容量
            dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_CP,
                                   /*B_Kind*/DATAKIND_PGGTHV, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                   ENCODETYPE_FARM, turbineNum, /*B_Source*/SOURCETYPE_IAS, chnl,
                                   /*B_Interval*/INTERVALUNIT_DAY, time );
            turbineIndex ++;

            //整风场待风台数
            dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_TA,
                                   /*B_Kind*/DATAKIND_WWG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                   ENCODETYPE_FARM, turbineNum, /*B_Source*/SOURCETYPE_IAS, chnl,
                                   /*B_Interval*/INTERVALUNIT_DAY, time );
            turbineIndex ++;
            //整风场待风容量
            dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_CP,
                                   /*B_Kind*/DATAKIND_WWG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                   ENCODETYPE_FARM, turbineNum, /*B_Source*/SOURCETYPE_IAS, chnl,
                                   /*B_Interval*/INTERVALUNIT_DAY, time );
            turbineIndex ++;

            //整风场停运台数
            dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_TA,
                                   /*B_Kind*/DATAKIND_SPG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                   ENCODETYPE_FARM, turbineNum, /*B_Source*/SOURCETYPE_IAS, chnl,
                                   /*B_Interval*/INTERVALUNIT_DAY, time );
            turbineIndex ++;
            //整风场停运容量
            dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_CP,
                                   /*B_Kind*/DATAKIND_SPG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                   ENCODETYPE_FARM, turbineNum, /*B_Source*/SOURCETYPE_IAS, chnl,
                                   /*B_Interval*/INTERVALUNIT_DAY, time );
            turbineIndex ++;

            //整风场通讯中断台数
            dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_TA,
                                   /*B_Kind*/DATAKIND_CBG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                   ENCODETYPE_FARM, turbineNum, /*B_Source*/SOURCETYPE_IAS, chnl,
                                   /*B_Interval*/INTERVALUNIT_DAY, time );
            turbineIndex ++;
            //整风场通讯中断容量
            dataId[turbineIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_CP,
                                   /*B_Kind*/DATAKIND_CBG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), /*B_Amount*/1,
                                   ENCODETYPE_FARM, turbineNum, /*B_Source*/SOURCETYPE_IAS, chnl,
                                   /*B_Interval*/INTERVALUNIT_DAY, time );
            turbineIndex ++;
        }//功能码为4

        else if ( 2 == funcId ) //风机状态
        {
            //每台风机有5个点的状态:正常发电、限功率、待风、通讯故障、停运
            //  数据库没有对应的值，因此只采第一个，后面4个不要
            //正常发电
            for ( int i = 0 ; i < 33 ; i ++ ) //单风机循环
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

        //整风场

    }

    else if ( chnl == 2 ) //
    {
        //十米层高
        //瞬时风速
        int towerIndex = 11;

        //int i = 12;
        for ( int i = 0; i < 4; i ++ )
        {
            //风速平均值
            dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_WS,
                                 /*B_Kind*/DATAKIND_AVG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                                 /*代表所属设备编号*/ENCODETYPE_AMT, 0, /*数据来源*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, amtH,
                                 /*时间间隔*/INTERVALUNIT_MINUTE, time );
            towerIndex ++;
            //风向平均值
            dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_WD,
                                 /*B_Kind*/DATAKIND_AVG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                                 /*代表所属设备编号*/ENCODETYPE_AMT, 0, /*数据来源*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, amtH,
                                 /*时间间隔*/INTERVALUNIT_MINUTE, time );
            towerIndex ++;

            //风速标准差
            dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_WS,
                                 /*B_Kind*/DATAKIND_SDV, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                                 /*代表所属设备编号*/ENCODETYPE_AMT, 0, /*数据来源*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, amtH,
                                 /*时间间隔*/INTERVALUNIT_MINUTE, time );
            towerIndex ++;

            //瞬时风速
            dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_WS,
                                 /*B_Kind*/DATAKIND_RTV, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                                 /*代表所属设备编号*/ENCODETYPE_AMT, 0, /*数据来源*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, amtH,
                                 /*时间间隔*/INTERVALUNIT_MINUTE, time );
            towerIndex ++;

            //瞬时风风向
            dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_WD,
                                 /*B_Kind*/DATAKIND_RTV, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                                 /*代表所属设备编号*/ENCODETYPE_AMT, 0, /*数据来源*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, amtH,
                                 /*时间间隔*/INTERVALUNIT_MINUTE, time );
            towerIndex ++;
            //最大阵风风速
            didClear.Export ( ( char* ) & ( dataId[towerIndex] ) );
            towerIndex ++;

            //最大风速
            dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_WS,
                                 /*B_Kind*/DATAKIND_MAX, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                                 /*代表所属设备编号*/ENCODETYPE_AMT, 0, /*数据来源*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, amtH,
                                 /*时间间隔*/INTERVALUNIT_MINUTE, time );
            towerIndex ++;

            //最小风速
            dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_WS,
                                 /*B_Kind*/DATAKIND_MIN, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                                 /*代表所属设备编号*/ENCODETYPE_AMT, 0, /*数据来源*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, amtH,
                                 /*时间间隔*/INTERVALUNIT_MINUTE, time );
            towerIndex ++;

            //风向标准差
            dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_WD,
                                 /*B_Kind*/DATAKIND_SDV, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                                 /*代表所属设备编号*/ENCODETYPE_AMT, 0, /*数据来源*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, amtH,
                                 /*时间间隔*/INTERVALUNIT_MINUTE, time );
            towerIndex ++;

            //最大风向
            dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_WD,
                                 /*B_Kind*/DATAKIND_MAX, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                                 /*代表所属设备编号*/ENCODETYPE_AMT, 0, /*数据来源*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, amtH,
                                 /*时间间隔*/INTERVALUNIT_MINUTE, time );
            towerIndex ++;

            //最小风向
            dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_WD,
                                 /*B_Kind*/DATAKIND_MIN, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                                 /*代表所属设备编号*/ENCODETYPE_AMT, 0, /*数据来源*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, amtH,
                                 /*时间间隔*/INTERVALUNIT_MINUTE, time );
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

        //10m层高温度平均值
        dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_AT,
                             /*B_Kind*/DATAKIND_AVG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*代表所属设备编号*/ENCODETYPE_AMT, 0, /*数据来源*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, 10,
                             /*时间间隔*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //10m层高温度瞬时值
        dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_AT,
                             /*B_Kind*/DATAKIND_RTV, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*代表所属设备编号*/ENCODETYPE_AMT, 0, /*数据来源*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, 10,
                             /*时间间隔*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //10m层高温度标准差
        dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_AT,
                             /*B_Kind*/DATAKIND_SDV, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*代表所属设备编号*/ENCODETYPE_AMT, 0, /*数据来源*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, 10,
                             /*时间间隔*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //10m层高温度最大值
        dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_AT,
                             /*B_Kind*/DATAKIND_MAX, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*代表所属设备编号*/ENCODETYPE_AMT, 0, /*数据来源*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, 10,
                             /*时间间隔*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //10m层高温度最小值
        dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_AT,
                             /*B_Kind*/DATAKIND_MIN, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*代表所属设备编号*/ENCODETYPE_AMT, 0, /*数据来源*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, 10,
                             /*时间间隔*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //10m层高湿度平均值
        dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_AH,
                             /*B_Kind*/DATAKIND_AVG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*代表所属设备编号*/ENCODETYPE_AMT, 0, /*数据来源*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, 10,
                             /*时间间隔*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //10m层高湿度瞬时值
        dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_AH,
                             /*B_Kind*/DATAKIND_RTV, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*代表所属设备编号*/ENCODETYPE_AMT, 0, /*数据来源*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, 10,
                             /*时间间隔*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //10m层高湿度标准差
        dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_AH,
                             /*B_Kind*/DATAKIND_SDV, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*代表所属设备编号*/ENCODETYPE_AMT, 0, /*数据来源*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, 10,
                             /*时间间隔*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //10m层高湿度最大值
        dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_AH,
                             /*B_Kind*/DATAKIND_MAX, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*代表所属设备编号*/ENCODETYPE_AMT, 0, /*数据来源*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, 10,
                             /*时间间隔*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //10m层高湿度最小值
        dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_AH,
                             /*B_Kind*/DATAKIND_MIN, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*代表所属设备编号*/ENCODETYPE_AMT, 0, /*数据来源*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, 10,
                             /*时间间隔*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //10m层高气压平均值
        dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_AP,
                             /*B_Kind*/DATAKIND_AVG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*代表所属设备编号*/ENCODETYPE_AMT, 0, /*数据来源*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, 10,
                             /*时间间隔*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //10m层高气压瞬时值
        dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_AP,
                             /*B_Kind*/DATAKIND_RTV, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*代表所属设备编号*/ENCODETYPE_AMT, 0, /*数据来源*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, 10,
                             /*时间间隔*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //10m层高气压标准差
        dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_AP,
                             /*B_Kind*/DATAKIND_SDV, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*代表所属设备编号*/ENCODETYPE_AMT, 0, /*数据来源*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, 10,
                             /*时间间隔*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //10m层高气压最大值
        dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_AP,
                             /*B_Kind*/DATAKIND_MAX, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*代表所属设备编号*/ENCODETYPE_AMT, 0, /*数据来源*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, 10,
                             /*时间间隔*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //10m层高气压最小值
        dataId[towerIndex] = * ( DATA_ID* ) did.MakeMeteorSingle ( /*B_Catalog*/DATACATALOG_AP,
                             /*B_Kind*/DATAKIND_MIN, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*代表所属设备编号*/ENCODETYPE_AMT, 0, /*数据来源*/SOURCETYPE_AMT, 0, HEIGHTTYPE_ES, 10,
                             /*时间间隔*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //电源电压平均值
        dataId[towerIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_BL,
                             /*B_Kind*/DATAKIND_AVG, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*代表所属设备编号*/ENCODETYPE_AMT, 0, /*数据来源*/SOURCETYPE_AMT, 0,
                             /*时间间隔*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //电源电压瞬时值
        dataId[towerIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_BL,
                             /*B_Kind*/DATAKIND_RTV, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*代表所属设备编号*/ENCODETYPE_AMT, 0, /*数据来源*/SOURCETYPE_AMT, 0,
                             /*时间间隔*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //电源电压标准差
        dataId[towerIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_BL,
                             /*B_Kind*/DATAKIND_SDV, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*代表所属设备编号*/ENCODETYPE_AMT, 0, /*数据来源*/SOURCETYPE_AMT, 0,
                             /*时间间隔*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //电源电压最大值
        dataId[towerIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_BL,
                             /*B_Kind*/DATAKIND_MAX, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*代表所属设备编号*/ENCODETYPE_AMT, 0, /*数据来源*/SOURCETYPE_AMT, 0,
                             /*时间间隔*/INTERVALUNIT_MINUTE, time );
        towerIndex++;

        //电源电压最小值
        dataId[towerIndex] = * ( DATA_ID* ) did.MakePeriodSingle ( /*B_Catalog*/DATACATALOG_BL,
                             /*B_Kind*/DATAKIND_MIN, /*B_Type*/DATATYPE_FLOAT32, /*B_Length*/did.TypeSize ( DATATYPE_FLOAT32 ), 1,
                             /*代表所属设备编号*/ENCODETYPE_AMT, 0, /*数据来源*/SOURCETYPE_AMT, 0,
                             /*时间间隔*/INTERVALUNIT_MINUTE, time );
        towerIndex++;
    }

    return chnl;
}
#endif


// 系统退出对相应资源进行回收处理
void sysClear()
{

    int tRet = 0;
    //关闭socket
    tRet = g_locServer.Cleanup();

    if ( tRet != 0 )
    {
        cfgMng->tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "%s|%s|%d: g_locServer.Cleanup turn error, iRet = %d\r\n",
                                 __FILE__, __func__, __LINE__, tRet );
    }

    //消除系统消息队列
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
