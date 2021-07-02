/*
* 文件：TransCtrlLayer.cpp
* 说明：实现传输控制层发送侧和接收侧的相应功能。
*/
#include "TransCtrlLayer.h"
#include "sysMgr.h"
#include "duGrp_BCPQ.h"
#include <error.h>
#include <time.h>
#include "cfgManage.h"

/******************************************************************************/
/*                        外部全局变量                                         */
/******************************************************************************/
//---------  "duGrp_BCPQ.h" ---------------------
//引用队列全局变量（传输控制队列与发送ASDU队列公用一个队列）
extern C_DuGrp_BCPQ ASDUDuQ;
//引用队列全局变量（向应用层传输ASDU的队列）
extern C_DuGrp_BCPQ ASDUInfoQ;
//---------   "sysMgr.h"    ---------------------
//引用通道管理表全局变量
extern C_ChnlMgr chnlMgr;
extern nsCPThread::CThread* thdObj_p;
// 创建事件句柄和套节字句柄表
extern EVENT**             g_evtArr_pp;
extern sock_descriptor**   g_sockArr_pp;
// 通道上文地址
extern u_char*   g_chnlQCnxt_p;

extern int ssnFnshNfyID;
extern nsCPSocket::CTcpClient g_locClient;
extern int                      g_locPort;
extern C_SsnCtnrObj ssnCtnr; // 会话容器

extern int finishBlock ( ssnCtlBlk* ssnCtlBlk_p, T_TID trdID, u_short ssnID, u_short chnlNo, u_int errCd );

//nsCPLog::CDailyLog g_tclLog;

extern T_TID        g_errTID;

extern cfgManage* cfgMng;

int ThreadInit ( u_short thdNum, u_char sandTFlag, u_char recvTFlag )
{

    //  u_short modNum = standNum%GROP_CHNL_SIZE;
    //  if (modNum != 0)
    //  {
    //      num += 1;
    //  }
    //  g_evtArr_pp = new WSAEVENT* [num]();
    //  g_sockArr_pp = new SOCKET*  [num]();
    //  int tRtn = 0;
    //  //int *tmp = new int;
    //  for (int i = 0; i < num; i++)
    //  {
    //      g_evtArr_pp[i] = new WSAEVENT [GROP_CHNL_SIZE]();
    //      g_sockArr_pp[i] = new SOCKET  [GROP_CHNL_SIZE]();
    //  }

    /*if ( g_tclLog.Init("../trylog/", "apdu") < 0)
    {
        return 0;
    }*/

    int tRtn = 0;

    if ( recvTFlag != 0 )
    {
#ifdef WINDOWS_PLATFORM
        tRtn = thdObj_p->StartThread ( ( FunTypeOne ) Tsys_NetRevProc, NULL, thdNum, THREAD_PRIORITY_HIGHEST );
#else
        tRtn = thdObj_p->StartThread ( ( FunTypeOne ) Tsys_NetRevProc, NULL, thdNum, 79, SCHED_RR );
#endif
    }

    if ( sandTFlag != 0 )
    {
#ifdef WINDOWS_PLATFORM
        tRtn = thdObj_p->StartThread ( ( FunTypeOne ) Tsys_NetSndProc, NULL, 1, THREAD_PRIORITY_ABOVE_NORMAL );
#else
        tRtn = thdObj_p->StartThread ( ( FunTypeOne ) Tsys_NetSndProc, NULL, 1, 78, SCHED_RR );
#endif
    }

    return 0;
}
///////////////////  1.发送侧的功能实现         //////////////////////////////////
/*******************************************************************************
* 功能：传输控制层发送侧线程函数
* 描述：它首先处理来自传输控制队列的请求事件,若没有传输控制请求事件,再处理来自ASDU队列的
*      请求事件。其中，请求事件的处理已经由队列对象完成。通过调用出队函数，此处对出队数据
*      进行相应地处理。
* 数据处理接口：
*     1.hdlTransCtrlReqEvnt()   - 处理来自传输控制信息队列的数据
*     2.hdlASDUInfoSndReqEvnt() - 处理来自ASDU发送请求队列的数据
*******************************************************************************/
u_long Tsys_NetSndProc ( void* lpParam )
{
    // 传输控制信息队列的数据单元长度；
    u_char ctrLng = sizeof ( qTransCtrlInfoUnit );
    // ASDU队列的数据单元长度；
    u_char datLng = sizeof ( qASDUInfoRcvUnit );
    // 声明出队的数据单元长度（主要用于申请出队数据空间）
    const u_char InfoLng = ( ctrLng > datLng ) ? ctrLng : datLng;

    // 声明出队的数据单元
    char* InfoData ;
    InfoData = ( char* ) calloc ( InfoLng, sizeof ( char ) );

    // 出队结果变量；
    int tRtn;

    while ( true )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_1, "Tsys_NetSndProc: ASDUDuQ.popQ\r\n" );
        // 优先处理传输控制队列事件组
        tRtn = 0;
        u_char infoLng = InfoLng;
        tRtn = ASDUDuQ.popQ ( BOTH_GROUP, InfoData, &infoLng );
		cfgMng->tclLog.LPrintf(DEBUG, DEBUG_LEVEL_1, "Tsys_NetSndProc: ASDUDuQ.popQ end\r\n");
        // 出队对象为传输控制对列
        if ( ( tRtn == ( SCD_GROUP | 0x10 ) ) && ( infoLng > 0 ) )
        {
            hdlTransCtrlReqEvnt ( ( qTransCtrlInfoUnit* ) InfoData, infoLng );
        }

        // 出队对象为ASDU队列
        else if ( ( tRtn == ( FST_GROUP | 0x10 ) ) && ( infoLng > 0 ) )
        {
            hdlASDUInfoSndReqEvnt ( ( qASDUInfoRcvUnit* ) InfoData, infoLng );
        }
        // 出队错误
        else
		{
			if (((( qASDUInfoRcvUnit* ) InfoData)->DU_type &U_F) == U_F)
			{
				hdlASDUInfoSndReqEvnt ( ( qASDUInfoRcvUnit* ) InfoData, infoLng );
			}
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "WARNING: popQ() CAN'T pop data from Queue, ErrorCode = %d!\n", tRtn );
		}
    }

    //释放空间
    free ( InfoData );
    return 0;
}

/*******************************************************************************
* 功能：处理传输控制信息队列的数据
* 描述：首先，它从数据信息中提取通道ID(chnlID)，通过它和通道管理表对象就可以获得通道对象。
*      接着，它从通道对象中提取通道的角色、操作句柄、规约类型以及其他用来控制数据发送的参
*      数等信息。然后，根据规约检查发送端的准备工作(即发送状态的迁徙图)。如果准备工作就绪，
*      就执行发送操作。若发送成功，更新发送端的控制参数。最后，根据控制要求对当前通道进行
*      超时定时器设置和数据流量控制。
* 接口参数：
*     1.ctrlUnitObj_p  传输控制信息队列的数据单元对象指针；
*     2.dBlkLng        数据长度；
*******************************************************************************/
void hdlTransCtrlReqEvnt ( qTransCtrlInfoUnit* ctrlUnitObj_p, u_char dBlkLng )
{
    /**************************  从数据单元中提取相关信息 **************************/
    // 获取通道(组)对象,通过通道管理表获取
    C_ChnlObj* chnlObj_p = chnlMgr.getChnlObj ( ctrlUnitObj_p->chanlID );

    // 获取规约描述(规约文件句柄)
    HMODULE* ptclMdl_p = chnlObj_p->getPtclMdl();
    // 获取通道操作句柄,比如sockets号
    HANDLE hChnlSend = chnlObj_p->getSockHdl();
    // 获取传输控制层实时参数指针
    void* realParam_p = chnlObj_p->getRealParam();
    // 获取传输控制层配置参数指针
    void* tranCtrlCfg_p = chnlObj_p->getTranCtrlCfg();
    // 获取站点的角色
    int sndSiteRole = chnlObj_p->getRole();

    /**************************  加载规约处理函数 **********************************/
    //1. 获取协议号ID
    DLL_getPtclID getPtclID;
    getPtclID = ( DLL_getPtclID ) GetProcAddress ( *ptclMdl_p, "getPtclID" );

    if ( !getPtclID )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function getPtclID()!\n" );
        return;
    }

    //2. 组装APDU数据
    DLL_packAPDU packAPDU;
    packAPDU = ( DLL_packAPDU ) GetProcAddress ( *ptclMdl_p, "packAPDU" );

    if ( !packAPDU )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function packAPDU()!\n" );
        return;
    }

    //3. 获取ASDU数据对象中的数据,即获得APDU中的字节流数据
    DLL_getDU getDU;
    getDU = ( DLL_getDU ) GetProcAddress ( *ptclMdl_p, "getDU" );

    if ( !getDU )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function packAPDU()!\n" );
        return;
    }

    // 4.检查发送状态
    DLL_CheckSendState CheckSendState;
    CheckSendState = ( DLL_CheckSendState ) GetProcAddress ( *ptclMdl_p, "CheckSendState" );

    if ( !CheckSendState )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function CheckSendState()!\n" );
        return;
    }

    //5.更新发送状态及实时参数
    DLL_UpdateSendState UpdateSendState;
    UpdateSendState = ( DLL_UpdateSendState ) GetProcAddress ( *ptclMdl_p, "UpdateSendState" );

    if ( !UpdateSendState )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function UpdateSendState()!\n" );
        return;
    }

    /****************************  发送功能处理  **********************************/
    //1.获得规约协议号
    u_char ptclID = getPtclID() % 2;
    // 声明传输介质接口变量
    DEVICES dev;
    dev.chnlID = ctrlUnitObj_p->chanlID;
    dev.type = ptclID;
    dev.hIntance = hChnlSend;

    //2.根据规约检查发送前的状态是否合理
    //返回值的高16位表示发送状态信息,低16位表示需要执行的动作
    u_int statusInfo;
    statusInfo = CheckSendState (
                 sndSiteRole, // 站点角色
                 realParam_p, // 通道实时参数
                 ( u_char* ) ctrlUnitObj_p->ctrlFrmDsc.ctrlFrm_p, // 发送数据
                 ctrlUnitObj_p->ctrlFrmDsc.frmLng     // 数据长度
                 );

    //2.1处理发送状态信息
    u_short errorInfo = statusInfo & 0xFFFF;

    switch ( errorInfo )
    {
        case SEND_TRANSITION_ERROR:  // 状态迁移错误
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ERROR: CheckSendState() found STATE TRANSITION Wrong!\n" );
            break;

        case SEND_ROLE_ERROR:        // 非法角色
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ERROR: CheckSendState() found ROLE Wrong!\n" );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ChnlID:= %d\n", dev.chnlID );
            break;

        case SEND_FRAME_ERROR:       // 数据帧错误
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ERROR: CheckSendState() found FRAME DATA Wrong!\n" );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ChnlID:= %d\n", dev.chnlID );
            break;

        case SEND_STATUS_ERROR:      // 非法状态
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ERROR: CheckSendState() found the status doesn't exist!\n" );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ChnlID:= %d\n", dev.chnlID );
            break;

        case SEND_TIMESCALE_ERROR:   // 超时错误
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ERROR: CheckSendState() found the time of data  is out!\n" );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ChnlID:= %d\n", dev.chnlID );
            break;
    }

    char szType[10] = {'I', 'S', ' ', 'U'};
    char szTmp[256] = {0};

    for ( int i = 0; i < ctrlUnitObj_p->ctrlFrmDsc.frmLng; i++ )
    {
        sprintf ( szTmp + ( i * 3 ), "%02x ", ctrlUnitObj_p->ctrlFrmDsc.ctrlFrm_p[i] );
    }

    int cType = ctrlUnitObj_p->ctrlFrmDsc.ctrlType & 3;
    cfgMng->tclLog.LPrintf ( cType, DEBUG_LEVEL_1, "s::%c %s\r\n", szType[cType], szTmp );
    //2.2处理需要执行的动作
    u_short execInfo = ( statusInfo >> 16 ) & 0xFFFF;
    // 本地状态和参数的更新标志，默认0表示不更新
    char updateFlag = 0;

    switch ( execInfo )
    {
        case NO_OPERATION:
            updateFlag = 0;
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "ERROR: No Operation()!\n" );
            break;

        case OPEN_CONNECT:
            updateFlag = 0;
            //OpenConnect(dev);
            break;

        case CLOSE_CONNECT:
            updateFlag = 0;
            CloseConnect ( dev );
            // 设置通道故障,通知上层框架
            //g_chnlQCnxt_p[dev.chnlID] = 0;
            //ASDUDuQ.setCnxtQ(0x1, dev.chnlID, &g_chnlQCnxt_p[dev.chnlID]);
            break;

        case RETRY_CONNECT:
            updateFlag = 0;
            //RetryConnect(dev);
            break;

        case WRITE_CONNECT: // 发送数据帧
            updateFlag = WriteToConnect (
                         dev,           // 通道（组）传输介质的接口信息
                         ctrlUnitObj_p->ctrlFrmDsc.ctrlFrm_p, // 发送数据
                         ctrlUnitObj_p->ctrlFrmDsc.frmLng,    // 数据长度
                         tranCtrlCfg_p  // 传输控制层配置参数(指定重试模式)
                         );
            /*
            if (updateFlag == 0)
            {
                stnOrVew *stnVew_p = chnlMgr.getStnOrVew(dev.chnlID);
                nsCPSocket::CTcpClient tmpCli;

                int i = dev.chnlID / GROP_CHNL_SIZE;
                int j = dev.chnlID % GROP_CHNL_SIZE;
                sock_descriptor old = g_sockArr_pp[i][j];
                g_locClient.Close(old);
            #ifndef WINDOWS_PLATFORM
                struct epoll_event evOld;
                evOld.data.fd = old;
                evOld.events = EPOLLIN | EPOLLOUT;
                epoll_ctl(g_evtArr_pp[0][i], EPOLL_CTL_DEL, old, &evOld);
            #endif
                cfgMng->tclLog.LPrintf(DEBUG ,DEBUG_LEVEL_1,"close hintance!");

                sock_descriptor iRet = tmpCli.ConnServer(htonl(stnVew_p->stnAddr),stnVew_p->stnAppID, stnVew_p->localAddr,stnVew_p->localPort, 100);
                if (iRet == sock_invalid)
                {
                    iRet = 0xFFFF0000;
                }
                sock_descriptor sNew = iRet;

                g_sockArr_pp[i][j] = sNew;
            #ifdef WINDOWS_PLATFORM
                WSAEventSelect(sNew, g_evtArr_pp[i][j], FD_READ);
            #else
                struct epoll_event ev;
                ev.data.fd = sNew;
                ev.events = EPOLLIN | EPOLLOUT;
                epoll_ctl(g_evtArr_pp[0][i], EPOLL_CTL_ADD, sNew, &ev);
            #endif
            }

            cfgMng->tclLog.LPrintf(DEBUG, DEBUG_LEVEL_1,
                "realParam_p:: sendnum=%d, receivenum=%d, wValue=%d, kValue=%d \r\n \r\n",
                (*(realTimeValue*)realParam_p).sendnum,
                (*(realTimeValue*)realParam_p).receivenum,
                (*(realTimeValue*)realParam_p).wValue,
                (*(realTimeValue*)realParam_p).kValue);
                */
            break;

        default:
            updateFlag = 0;
    }

    // 3.是否更新发送控制的状态和实时参数,只有发送成功,才会更新参数
    //返回值按位表述功能,位值为1时表示开启操作,0表示无需操作;
    //最低位:设置定时器
    //次低位:流量控制
    //其他位:保留
    if ( updateFlag )
    {
        // 协调控制本地状态和参数的标志
        char ctrlFlag = 0;
        //定时器操作码
        u_char tmCtrlCode;

        ctrlFlag = UpdateSendState (
                   sndSiteRole,    // 站点角色
                   realParam_p,    // 通道实时参数
                   tranCtrlCfg_p,  // 通道配置参数
                   &tmCtrlCode,    // 定时器操作码
                   ( u_char* ) ctrlUnitObj_p->ctrlFrmDsc.ctrlFrm_p, // 发送数据
                   ctrlUnitObj_p->ctrlFrmDsc.frmLng     // 数据长度
                   );

        // 3.1判定是否对定时器进行设置
        if ( ctrlFlag & 0x1 )
        {
            SetCtrlTimer ( chnlObj_p, tmCtrlCode );    // 设置定时器
        }

        // 3.2判定是否停止发送数据（对流量进行控制，此处必然为关闭操作）
        if ( ctrlFlag & 0x2 )
        {
            g_chnlQCnxt_p[dev.chnlID] = 0x0;
            // 设置通道上下文状态
            SetFlowCtrl (
            0x1,         // 传输控制请求队列标志
            dev.chnlID,  // 通道（组）ID号
            &g_chnlQCnxt_p[dev.chnlID]  // 通道状态值
            );
        }
    }

}

/*******************************************************************************
* 功能：处理ASDU请求队列中的数据
* 描述：首先，它从数据单元中提取通道ID(chnlNo)，通过它和通道管理表对象就可以获得通道对象。
*      接着，它从通道对象中提取通道的角色、操作句柄、规约类型以及其他用来控制数据发送的参
*      数等信息。接着，根据提取的ASDU数据信息，组装APDU数据。然后，根据规约检查发送端的
*      准备工作(即发送状态的迁徙图)。如果准备工作就绪，就执行发送操作。若发送成功，更新发
*      送端的控制参数。最后，根据控制参数设置对应通道的
*      上下文状态，以及通道定时器等。
* 接口参数：
*     1.datUnitObj_p  ASDU队列的数据单元对象指针；
*     2.dBlkLng       数据长度；
*******************************************************************************/
void hdlASDUInfoSndReqEvnt ( qASDUInfoRcvUnit* datUnitObj_p, u_char dBlkLng )
{
    /**************************  从数据单元中提取相关信息 **************************/
    // 获取通道(组)对象,通过通道管理表获取
    C_ChnlObj* chnlObj_p = chnlMgr.getChnlObj ( datUnitObj_p->chanlID );
    // 获取规约描述(规约文件句柄)
    HMODULE* ptclMdl_p = chnlObj_p->getPtclMdl();
    // 获取通道操作句柄,比如sockets号
    HANDLE hChnlSend = chnlObj_p->getSockHdl();
    // 获取传输控制层实时参数指针
    void* realParam_p = chnlObj_p->getRealParam();
    // 获取传输控制层配置参数指针
    void* tranCtrlCfg_p = chnlObj_p->getTranCtrlCfg();
    // 获取传输控制层配置参数指针
    void* tcOtherCfg_p = NULL;//chnlObj_p->getTranCtrlCfg(); // 暂不实现
    // 获取站点的角色
    int sndSiteRole = chnlObj_p->getRole();

    //
    datUnitObj_p->ASDU_Lng = dBlkLng;   // modify by zhaojj,20170420

    /**************************  加载规约处理函数 **********************************/
    //1. 获取协议号ID
    DLL_getPtclID getPtclID;
    getPtclID = ( DLL_getPtclID ) GetProcAddress ( *ptclMdl_p, "getPtclID" );

    if ( !getPtclID )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function getPtclID()!\n" );
        return;
    }

    //2. 组装APDU数据
    DLL_packAPDU packAPDU;
    packAPDU = ( DLL_packAPDU ) GetProcAddress ( *ptclMdl_p, "packAPDU" );

    if ( !packAPDU )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function packAPDU()!\n" );
        return;
    }

    //3. 获取ASDU数据对象中的数据,即获得APDU中的字节流数据
    DLL_getDU getDU;
    getDU = ( DLL_getDU ) GetProcAddress ( *ptclMdl_p, "getDU" );

    if ( !getDU )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function packAPDU()!\n" );
        return;
    }

    // 4.检查发送状态
    DLL_CheckSendState CheckSendState;
    CheckSendState = ( DLL_CheckSendState ) GetProcAddress ( *ptclMdl_p, "CheckSendState" );

    if ( !CheckSendState )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function CheckSendState()!\n" );
        return;
    }

    //5.更新发送状态及实时参数
    DLL_UpdateSendState UpdateSendState;
    UpdateSendState = ( DLL_UpdateSendState ) GetProcAddress ( *ptclMdl_p, "UpdateSendState" );

    if ( !UpdateSendState )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function UpdateSendState()!\n" );
        return;
    }

    //6.
    T_getFrameType tGetType = ( T_getFrameType ) GetProcAddress ( *ptclMdl_p, "getFrameType" );

    if ( !tGetType )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function getFrameType()!\n" );
        return;
    }

    //
    u_char ucPtclCd = datUnitObj_p->DU_type;

    if ( ucPtclCd == IEC104_NEWHOLD_ACT )
    {
        // 通道连接建立 帧
        stnOrVew* stnVew_p = chnlMgr.getStnOrVew ( datUnitObj_p->chanlID );
        nsCPSocket::CTcpClient tmpCli;
        sock_descriptor iRet = g_locClient.ConnServer ( htonl ( stnVew_p->stnAddr ), stnVew_p->stnAppID, stnVew_p->localAddr, stnVew_p->localPort, 100 );

        if ( iRet == sock_invalid )
        {
            iRet = 0xFFFF0000;
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "Failed to estabish remote connection!\n" );
        }

        //
        u_int errCd = iRet;
        ssnCtlBlk* tSsnCtlBlk_p = NULL;
        T_TID tID = 0;
        iRet = finishBlock ( tSsnCtlBlk_p, tID, ucPtclCd, datUnitObj_p->chanlID, errCd );

        if ( iRet < 0 )
        {
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "Error: finishBlock is failed!\n" );
        }

        return ;
    }

    //

    /****************************  发送功能处理  **********************************/
    //1. 获取协议号ID，目的区分出TCP和串行    // 注意区分
    u_char ptclID = getPtclID() % 2;
    // 声明传输介质接口变量
    DEVICES dev;
    dev.chnlID = datUnitObj_p->chanlID;
    dev.type = ptclID;
    dev.hIntance = hChnlSend;

    //2. 组装APDU数据
    // packAPDU函数输入ASDU数据对象、实时参数和配置参数，输出APDU数据流
    u_char apdu_data_p[256];
    u_char apdu_data_len;
    packAPDU (
    realParam_p,    // 通道实时参数，比如：发送接收序号，K，W
    tranCtrlCfg_p,  // 通道配置参数，比如101协议中，链路地址域配置参数
    ( u_char* ) datUnitObj_p->ASDU_p, // ASDU数据对象句柄
    datUnitObj_p->ASDU_Lng,         // ASDU数据长度
    apdu_data_p,    // APDU数据
    apdu_data_len   // APDU的长度
    );
    cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_1, "chnl = %d,packAPDU\r\n", dev.chnlID );
    //3. 根据规约检查发送前的状态是否合理
    //返回值的高16位表示发送状态信息,低16位表示需要执行的动作
    u_int statusInfo;
    statusInfo = CheckSendState (
                 sndSiteRole,  // 站点角色
                 realParam_p,  // 通道实时参数
                 apdu_data_p,  // 发送数据
                 apdu_data_len // 数据长度
                 );
    //3.1 处理发送状态信息,即错误状态的日志处理
    u_short errorInfo = statusInfo & 0xFFFF;

    switch ( errorInfo )
    {
        case SEND_TRANSITION_ERROR:  // 状态迁移错误
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "ERROR: CheckSendState() found TRANSITION Wrong!\n" );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "ChnlID:= %d\n", dev.chnlID );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "cur_index:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.cur_index );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "max_index:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.max_index );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "nex_index:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.nex_index );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "func_id:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.one_node_p[0].func_id );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "trans_id:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.one_node_p[0].trans_id );
            break;

        case SEND_ROLE_ERROR:        // 非法角色
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "ERROR: CheckSendState() found ROLE Wrong!\n" );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "ChnlID:= %d\n", dev.chnlID );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "cur_index:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.cur_index );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "max_index:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.max_index );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "nex_index:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.nex_index );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "func_id:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.one_node_p[0].func_id );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "trans_id:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.one_node_p[0].trans_id );
            break;

        case SEND_FRAME_ERROR:       // 数据帧错误
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "ERROR: CheckSendState() found FRAME DATA Wrong!\n" );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "ChnlID:= %d\n", dev.chnlID );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "cur_index:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.cur_index );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "max_index:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.max_index );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "nex_index:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.nex_index );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "func_id:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.one_node_p[0].func_id );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "trans_id:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.one_node_p[0].trans_id );
            break;

        case SEND_STATUS_ERROR:      // 非法状态
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "ERROR: Sand::CheckSendState() found the status doesn't exist!\n" );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "ChnlID:= %d\n", dev.chnlID );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "cur_index:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.cur_index );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "max_index:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.max_index );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "nex_index:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.nex_index );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "func_id:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.one_node_p[0].func_id );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "trans_id:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.one_node_p[0].trans_id );
            break;

        case SEND_TIMESCALE_ERROR:   // 超时错误
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "ERROR: CheckSendState() found the time of data  is out!\n" );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "ChnlID:= %d\n", dev.chnlID );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "cur_index:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.cur_index );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "max_index:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.max_index );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "nex_index:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.nex_index );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "func_id:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.one_node_p[0].func_id );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "trans_id:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.one_node_p[0].trans_id );
            break;
    }

    //
    char szType[10] = {'I', 'S', ' ', 'U'};
    char szTmp[256] = {0};

    for ( int i = 0; i < apdu_data_len; i++ )
    {
        sprintf ( szTmp + ( i * 3 ), "%02x ", apdu_data_p[i] );
    }

    //char cType = datUnitObj_p->DU_type & 3;
	char cType = tGetType(apdu_data_p ,apdu_data_len );
	cType = cType & 3;
    cfgMng->tclLog.LPrintf ( cType, DEBUG_LEVEL_1, "s::%c %s\r\n", szType[cType], szTmp );
    //3.2 处理需要执行的动作
    u_short execInfo = ( statusInfo >> 16 ) & 0xFFFF;
    // 本地状态和参数的更新标志，默认0表示不更新
    char updateFlag = 0;

    switch ( execInfo )
    {
        case OPEN_CONNECT:
            //OpenConnect(dev);
            break;

        case CLOSE_CONNECT:
            //CloseConnect(dev);
            //g_chnlQCnxt_p[dev.chnlID] = 0;
            //ASDUDuQ.setCnxtQ(0x1, dev.chnlID, &g_chnlQCnxt_p[dev.chnlID]);  // 设置通道故障,通知上层框架
            updateFlag = 0;
            break;

        case RETRY_CONNECT:
            //RetryConnect(dev);
            break;

        case WRITE_CONNECT: // 发送数据帧

            updateFlag = WriteToConnect (
                         dev,           // 通道（组）传输介质的接口信息
                         ( char* ) apdu_data_p, // 发送数据
                         apdu_data_len,      // 数据长度
                         tcOtherCfg_p/*tranCtrlCfg_p*/  // 传输控制层配置参数
                         );
#if 0

            if ( updateFlag == 0 )
            {

                stnOrVew* stnVew_p = chnlMgr.getStnOrVew ( dev.chnlID );
                nsCPSocket::CTcpClient tmpCli;

                int i = dev.chnlID / GROP_CHNL_SIZE;
                int j = dev.chnlID % GROP_CHNL_SIZE;

                sock_descriptor old = g_sockArr_pp[i][j];
                g_locClient.Close ( old );
                struct epoll_event evOld;
                evOld.data.fd = old;
                evOld.events = EPOLLIN | EPOLLOUT;
                epoll_ctl ( g_evtArr_pp[0][i], EPOLL_CTL_ADD, old, &evOld );
                cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_1, "close hintance!" );

                sock_descriptor iRet = tmpCli.ConnServer ( htonl ( stnVew_p->stnAddr ), stnVew_p->stnAppID, stnVew_p->localAddr, stnVew_p->localPort, 100 );

                if ( iRet == sock_invalid )
                {
                    iRet = 0xFFFF0000;
                }

                sock_descriptor sNew = iRet;

                g_sockArr_pp[i][j] = sNew;
#ifdef WINDOWS_PLATFORM
                WSAEventSelect ( sNew, g_evtArr_pp[i][j], FD_READ );
#else
                struct epoll_event ev;
                ev.data.fd = sNew;
                ev.events = EPOLLIN | EPOLLOUT;
                epoll_ctl ( g_evtArr_pp[0][i], EPOLL_CTL_ADD, sNew, &ev );
#endif
            }

#endif
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_1,
                                     "realParam_p:: sendnum=%d, receivenum=%d, wValue=%d, kValue=%d \r\n \r\n",
                                     ( * ( realTimeValue* ) realParam_p ).sendnum,
                                     ( * ( realTimeValue* ) realParam_p ).receivenum,
                                     ( * ( realTimeValue* ) realParam_p ).wValue,
                                     ( * ( realTimeValue* ) realParam_p ).kValue );
            break;

        default:
            updateFlag = 0;
    }

    // 4.更新发送控制的状态和实时参数
    if ( updateFlag )
    {
        // 协调控制本地状态和参数的标志
        char ctrlFlag = 0;
        //定时器操作码
        u_char tmCtrlCode = 0;

        //5.更新发送端的状态和实时参数,
        //返回值按位表述功能,位值为1时表示开启操作,0表示无需操作;
        //最低位:设置定时器
        //次低位:流量控制
        //其他位:保留
        ctrlFlag = UpdateSendState (
                   sndSiteRole,   // 站点角色
                   realParam_p,   // 通道实时参数
                   tranCtrlCfg_p, // 通道配置参数
                   &tmCtrlCode,   // 定时器控制码
                   ( u_char* ) apdu_data_p, // 发送数据
                   apdu_data_len  // 数据长度
                   );

        // 5.1 判定是否对定时器进行设置
        if ( ctrlFlag & 0x1 )
        {
            printf ( "transCtrLayer tmCtrlCode = %d", tmCtrlCode );
            SetCtrlTimer ( chnlObj_p, tmCtrlCode ); // 设置定时器
        }

        // 5.2 判定是否停止发送数据（对流量进行控制，此处必然为关闭操作）
        if ( ctrlFlag & 0x2 )
        {
            g_chnlQCnxt_p[dev.chnlID] = 0x0;
            // 设置通道上下文状态
            SetFlowCtrl (
            0x1,         // 传输控制请求队列标志
            dev.chnlID,  // 通道（组）ID号
            &g_chnlQCnxt_p[dev.chnlID]  // 通道状态值
            );
        }
    }
    else
    {
        // 获取通道(组)对象,通过通道管理表获取
        // 获取规约描述(规约文件句柄)
        HMODULE* ptclMdl_p = chnlObj_p->getPtclMdl();
        // 获取协议号ID(用来指明传输介质类型，比如网口、串口等)
        u_char ptclID = getPtclID() % 2;
        // 获取通道操作句柄,比如sockets号
        HANDLE hChnlSend = chnlObj_p->getSockHdl();
        DEVICES dev;
        dev.chnlID = datUnitObj_p->chanlID;
        dev.hIntance = hChnlSend;
        dev.type = ptclID;
        CloseConnect ( dev );
        // FORMAT: CHNL_ID, CODE
        u_int msgBdy[2] = {0};
        msgBdy[0] = dev.chnlID;
        msgBdy[1] = 0x2;
        chnlMgr.clrChnlState ( dev.chnlID, MC_CHNL_BIT );   // 更新通道状态，通道状态位置零
        // 其他操作需要上层框架配合，需要发送信号及信息包到上层
        int ret = -1;

        if ( g_errTID == VALIDTTID )
        {
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "g_errTID = NULL/n!" );
        }
        else
        {
            ret =  ssnFnshNfy ( g_errTID, ( char* ) msgBdy, 2 * sizeof ( u_int ), ssnFnshNfyID );
        }

        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_1,
                                 "hdlASDUInfoSndReqEvnt:: tm-%d send to E_takeErr, msg=%u %u ret = %d  g_errTID= %d\r\n",
                                 dev.type, msgBdy[0], msgBdy[1], ret, g_errTID );
    }
}

///////////////////  2.接收侧的功能实现         //////////////////////////////////
/*******************************************************************************
* 功能：传输控制层接收侧线程函数，接收网口数据，并解析和处理APDU传输帧。
* 描述：它使用了Windows的事件选择模型(WsaEventSelect)，每个线程多可以管理和维护64个网
*      络通讯接口，即64个通道连接。在这种模型中，每个连接(socket)与网络事件一一对应。若
*      一个连接上有网络帧到时，模型就会产生对应的事件，通过WSAWaitForMultipleEvents()
*      函就可以数获取网络事件对应的接收数据。
*      在系统设计中，由15个独立的线程负责接收来自网络接口的数据。因此，每个站点最多可以连
*      接960(64*15)个网络站点(Socket)。每个线程具有自己的标识（线程编号），它是在线程创
*      建函数的参数传递给线程接收函数。
*
* 数据输出接口：
*     1.qTransCtrlInfoUnit
*     2.qASDUInfoRcvUnit
*******************************************************************************/
u_long Tsys_NetRevProc ( void* lpParam )
{

    // 定义通道（组）号
    u_short chnlID = 0;
    // 取得接收线程对应的分组号，编号从0，1,...14
    int revThreadNo = threadNum ++;


    // 临时存储区，用于recv()接收数据
    char recvDataBuf[MAX_RECV_SIZE];
    // 接收数据存储区指针,结构为revBuf,它指向通道对象中接收端数据缓冲指针
    revBuf recvDataFrm;

    // 16个线程的双缓冲标志,每位表示一个通道的缓冲使用情况,64位表示64个通道
    unsigned long long bufFlag = 0;

#ifndef WINDOWS_PLATFORM
    epoll_event events[GROP_CHNL_SIZE];
    sock_descriptor sockfd = -1;
#endif

    // 接收线程的接收处理功能
    while ( true )
    {
#ifdef WINDOWS_PLATFORM
        // 等待网络事件
        DWORD nIndex = WSAWaitForMultipleEvents (
                       GROP_CHNL_SIZE,           // 事件句柄的数量，最大值为WSA_MAXIMUM_WAIT_EVENTS（64）
                       g_evtArr_pp[revThreadNo], // 事件句柄数组的指针；
                       FALSE,        // 等待的类型，如果其中一个事件有效，程序就向下执行；
                       WSA_INFINITE,//5000,// // 超时时间， 如果为WSA_INFINITE，则永不超时；
                       FALSE );      // I/O执行进程状态类型，FALSE表示不用等待I/O执行完成 ；

        // 发生事件的索引值（数组下标），若存在多个事件，返回最前面的一个索引编号
        nIndex = nIndex - WSA_WAIT_EVENT_0;

        if ( nIndex == WSA_WAIT_FAILED || nIndex == WSA_WAIT_TIMEOUT )
        {
            //Sleep(100);
            // 忽略超时错误，一般来说没发生的事件肯定超时
            continue;
        }

        // 对当前索引号以后的事件，再次调用WSAWaitForMultipleEvents()函数，
        // 以便确认后续事件的状态
        int i;

        for ( i = nIndex; i < GROP_CHNL_SIZE; i++ )
        {
            nIndex = WSAWaitForMultipleEvents (
                     1,     // 事件句柄的数量;
                     &g_evtArr_pp[revThreadNo][i], // 事件句柄数组的指针;
                     TRUE,   // 等待的类型;
                     0,   // 超时时间;
                     FALSE );

            if ( nIndex == WSA_WAIT_FAILED || nIndex == WSA_WAIT_TIMEOUT )
            {
                //  Sleep(50);
                // 忽略超时错误，一般来说没发生的事件肯定超时
                continue;
            }
            else
            {
                // 找到受信事件， 计算出网络事件对应的通道号
                chnlID = ( revThreadNo << 4 ) + i;
                // 获取通道(组)对象,通过通道管理表获取
                C_ChnlObj* chnlObj_p = chnlMgr.getChnlObj ( chnlID );
                // 获取规约描述(规约文件句柄)
                HMODULE* ptclMdl_p = chnlObj_p->getPtclMdl();
                //加载规约函数
                DLL_getHeaderlng getHeaderlng;
                getHeaderlng = ( DLL_getHeaderlng ) GetProcAddress ( *ptclMdl_p, "getHeaderlng" );

                if ( !getHeaderlng )
                {
                    cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                             "Error: Can't not load function getHeaderlng()!\n" );
                    continue ;
                }

                DLL_getApduLngFrmBuf getApduLngFrmBuf;
                getApduLngFrmBuf = ( DLL_getApduLngFrmBuf ) GetProcAddress ( *ptclMdl_p, "getApduLngFrmBuf" );

                if ( !getApduLngFrmBuf )
                {
                    cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                             "Error: Can't not load function getApduLngFrmBuf()!\n" );
                    continue;
                }

                if ( g_sockArr_pp[revThreadNo][i] == sock_invalid )
                {
                    continue;
                }

                // 获取套接字上发生的网络事件
                // WSAEnumNetworkEvents()函数会自动重置授信事件
                WSANETWORKEVENTS NetworkEvents;
                int result = WSAEnumNetworkEvents (
                             g_sockArr_pp[revThreadNo][i],  // 当前的套接字
                             g_evtArr_pp[revThreadNo][i],   // 当前的事件对象句柄
                             &NetworkEvents                // 网络事件
                             );

                // 获取网络事件错误，执行处理操作，比如管道操作写入LOG
                if ( result == SOCKET_ERROR )
                {
                    cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                             "Error: Socket error of ReceiveNetData() of ChnlID:%d!\n", chnlID );
                    // 处理网络故障
                    hdlRecvError ( chnlID );
                }
                else
                {
                    // 处理FD_READ消息
                    if ( NetworkEvents.lNetworkEvents & FD_READ )
                    {
                        //处理读取事件
                        if ( NetworkEvents.iErrorCode[FD_READ_BIT] == 0 )
                        {
                            // 清空临时存储区
                            //ZeroMemory(recvDataBuf,MAX_RECV_SIZE);
                            memset ( recvDataBuf, 0, MAX_RECV_SIZE );
                            //瞄取数据长度---从规约获取
                            u_char headLng = getHeaderlng();

                            if ( headLng == 0 )
                            {
                                continue;
                            }

                            //从SOCKET接收缓冲中瞄取数据
                            int nRecvBytes = 0;
                            nRecvBytes = recv (
                                         g_sockArr_pp[revThreadNo][i], // 当前的套接字
                                         ( char* ) recvDataBuf, // 接收数据缓冲
                                         headLng,            // 接收长度
                                         MSG_PEEK
                                         );

                            if ( nRecvBytes != headLng )
                            {
                                continue;
                            }

                            // 获取数据长度--从规约获取
                            int dataLng = getApduLngFrmBuf ( recvDataBuf, nRecvBytes );

                            if ( dataLng < 0 )
                            {
                                dataLng = 1;
                            }

                            //从SOCKET接收缓冲中获取接收数据
                            nRecvBytes = 0;
                            memset ( recvDataBuf, 0, MAX_RECV_SIZE );
                            nRecvBytes = recv (
                                         g_sockArr_pp[revThreadNo][i], // 当前的套接字
                                         ( char* ) recvDataBuf, // 接收数据缓冲
                                         dataLng,            // 接收长度
                                         0
                                         );

                            if ( nRecvBytes != dataLng || dataLng == 1 )
                            {
                                continue;
                            }

                            // 更新缓冲区的长度
                            recvDataFrm.APDU_Buf.APDU_Lng = nRecvBytes;
                            recvDataFrm.APDU_Buf.APDU_Data_p = ( unsigned char* ) recvDataBuf;

                            //
                            nsCPPub::SleepMilliSecond ( 300 );

                            // 处理接收数据
                            hdlRecvData ( chnlID, &bufFlag, &recvDataFrm );

                        }
                    }
                }
            }
        } // ! for

#else
        int nfds = epoll_wait ( g_evtArr_pp[0][revThreadNo], events, GROP_CHNL_SIZE, 150 );
        DLL_getHeaderlng getHeaderlng;
        DLL_getApduLngFrmBuf getApduLngFrmBuf;

        for ( int i = 0; i < nfds; i++ )
        {
            if ( events[i].events & EPOLLIN )
            {
                if ( ( sockfd = events[i].data.fd ) < 0 )
                {
                    continue;
                }

                for ( int j = 0; j < GROP_CHNL_SIZE; j++ )
                {
                    if ( sockfd == g_sockArr_pp[revThreadNo][j] ) //从sock集合中找到有事件发生的sockfd
                    {
                        chnlID = ( revThreadNo << 4 ) + j;
                        break;
                    }
                }

                C_ChnlObj* chnlObj_p = chnlMgr.getChnlObj ( chnlID );
                // 获取规约描述(规约文件句柄)
                HMODULE* ptclMdl_p = chnlObj_p->getPtclMdl();
                //加载规约函数

                getHeaderlng = ( DLL_getHeaderlng ) GetProcAddress ( *ptclMdl_p, "getHeaderlng" );

                if ( !getHeaderlng )
                {
                    cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                             "Error: Can't not load function getHeaderlng()!\n" );
                    continue ;
                }

                getApduLngFrmBuf = ( DLL_getApduLngFrmBuf ) GetProcAddress ( *ptclMdl_p, "getApduLngFrmBuf" );

                if ( !getApduLngFrmBuf )
                {
                    cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                             "Error: Can't not load function getApduLngFrmBuf()!\n" );
                    continue;
                }

                if ( g_sockArr_pp[revThreadNo][i] == sock_invalid )
                {
                    continue;
                }


                //事件处理
                // 清空临时存储区
                memset ( recvDataBuf, 0, MAX_RECV_SIZE );
                //瞄取数据长度---从规约获取
                u_char headLng = getHeaderlng();

                if ( headLng == 0 )
                {
                    continue;
                }

                //从SOCKET接收缓冲中瞄取数据
                int nRecvBytes = 0;
                nRecvBytes = recv (
                             sockfd, // 当前的套接字
                             ( char* ) recvDataBuf, // 接收数据缓冲
                             headLng,            // 接收长度
                             MSG_PEEK
                             );

                if ( nRecvBytes != headLng )
                {
                    continue;
                }

                // 获取数据长度--从规约获取
                int dataLng = getApduLngFrmBuf ( recvDataBuf, nRecvBytes );

                if ( dataLng < 0 )
                {
                    dataLng = 1;
                }

                //从SOCKET接收缓冲中获取接收数据
                nRecvBytes = 0;
                nRecvBytes = recv (
                             sockfd, // 当前的套接字
                             ( char* ) recvDataBuf, // 接收数据缓冲
                             dataLng,            // 接收长度
                             0
                             );
                APCI_104* apci_p = ( APCI_104* ) &recvDataBuf;
                I_FRM* i_frm_p = ( I_FRM* ) apci_p->ctlField;
                cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_1, "SNo = %d\r\n", i_frm_p->SNo );

                if ( nRecvBytes != dataLng || dataLng == 1 )
                {
                    continue;
                }

                // 更新缓冲区的长度
                recvDataFrm.APDU_Buf.APDU_Lng = nRecvBytes;
                recvDataFrm.APDU_Buf.APDU_Data_p = ( unsigned char* ) recvDataBuf;

                //
                //Sleep(200);

                // 处理接收数据
                hdlRecvData ( chnlID, &bufFlag, &recvDataFrm );
            }
        }

#endif//WINDOWS_PLATFORM
    } // ! while

    return 0;
}

/*******************************************************************************
* 功能：正确接收数据后，处理这些接收数据
* 描述：首先，它从数据信息中提取通道ID(chnlID)，通过它和通道管理表对象就可以获得通道对象。
*      接着，它从通道对象中提取通道的角色、操作句柄、规约类型以及其他参数等信息。
*      然后，根据规约解析接收数据。
*
*******************************************************************************/
void hdlRecvData ( u_short chnlID, unsigned long long* bufFlag, revBuf* DataFrm )
{
    /**************************  从数据单元中提取相关信息 **************************/
    // 获取通道(组)对象,通过通道管理表获取
    C_ChnlObj* chnlObj_p = chnlMgr.getChnlObj ( chnlID );
    // 获取规约描述(规约文件句柄)
    HMODULE* ptclMdl_p = chnlObj_p->getPtclMdl();
    // 获取传输控制层实时参数指针
    void* realParam_p = chnlObj_p->getRealParam();
    // 获取传输控制层配置参数指针
    void* tranCtrlCfg_p = chnlObj_p->getTranCtrlCfg();
    // 获取站点的角色
    int revSiteRole = chnlObj_p->getRole();
    // 获取通道接收缓冲区的首地址,用于存储104协议中的I帧
    revBuf* recv_p =  ( revBuf* ) chnlObj_p->getRcvBufPtr();

    int iDataLng = DataFrm->ASDU_Buf.ASDU_Lng;
    // 保存APDU完整信息，供更新状态使用
    revBuf revAPDU;
    memcpy ( &revAPDU, DataFrm, sizeof ( revBuf ) );


    /**************************  加载规约处理函数 **********************************/

    DLL_CheckRecvState chkRecvState;
    chkRecvState = ( DLL_CheckRecvState ) GetProcAddress ( *ptclMdl_p, "CheckRecvState" );

    if ( !chkRecvState )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function CheckRecvState()!\n" );
        return ;
    }

    //1.解析APDU数据
    DLL_chkAPDUData chkAPDUData;
    chkAPDUData = ( DLL_chkAPDUData ) GetProcAddress ( *ptclMdl_p, "chkAPDUData" );

    if ( !chkAPDUData )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function chkAPDUData()!\n" );
        return ;
    }

    //2.更新接收端的状态和实时参数
    DLL_UpdateRecvState UpdateRecvState;
    UpdateRecvState = ( DLL_UpdateRecvState ) GetProcAddress ( *ptclMdl_p, "UpdateRecvState" );

    if ( !UpdateRecvState )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function UpdateRecvState()!\n" );
        return;
    }

    //3.构造应答帧数据
    DLL_RespCtrlFrame RespCtrlFrame;
    RespCtrlFrame = ( DLL_RespCtrlFrame ) GetProcAddress ( *ptclMdl_p, "RespCtrlFrame" );

    if ( !RespCtrlFrame )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function RespCtrlFrame()!\n" );
        return;
    }

    //4.规约指定的错误处理
    DLL_HandleRecvStateError HandleRecvStateError;
    HandleRecvStateError = ( DLL_HandleRecvStateError ) GetProcAddress ( *ptclMdl_p, "HandleRecvStateError" );

    if ( !HandleRecvStateError )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function HandleRecvStateError()!\n" );
        return;
    }

    //5. 获取协议号ID
    DLL_getPtclID getPtclID;
    getPtclID = ( DLL_getPtclID ) GetProcAddress ( *ptclMdl_p, "getPtclID" );

    if ( !getPtclID )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function getPtclID()!\n" );
        return;
    }

    /****************************  接收功能处理  **********************************/


    //2.根据规约检查发送前的状态是否合理
    //返回值的高16位表示发送状态信息,低16位表示需要执行的动作
    u_int status;
    status = chkRecvState (
             revSiteRole, // 站点角色
             realParam_p, // 通道实时参数
             ( u_char* ) revAPDU.APDU_Buf.APDU_Data_p, // 发送数据
             revAPDU.APDU_Buf.APDU_Lng     // 数据长度
             );

    //2.1处理发送状态信息
    u_short errorInfo = status & 0xFFFF;

    switch ( errorInfo )
    {
        case SEND_TRANSITION_ERROR:  // 状态迁移错误
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ERROR: CheckRecvState() found STATE TRANSITION Wrong!\n" );
            break;

        case SEND_ROLE_ERROR:        // 非法角色
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ERROR: CheckRecvState() found ROLE Wrong!\n" );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ChnlID:= %d\n", chnlID );
            break;

        case SEND_FRAME_ERROR:       // 数据帧错误
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ERROR: CheckRecvState() found FRAME DATA Wrong!\n" );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ChnlID:= %d\n", chnlID );
            break;

        case SEND_STATUS_ERROR:      // 非法状态
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ERROR: CheckRecvState() found the status doesn't exist!\n" );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ChnlID:= %d\n", chnlID );
            break;

        case SEND_TIMESCALE_ERROR:   // 超时错误
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ERROR: CheckRecvState() found the time of data  is out!\n" );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ChnlID:= %d\n", chnlID );
            break;
    }

    //2.2处理需要执行的动作
    u_short execInfo = ( status >> 16 ) & 0xFFFF;
    // 本地状态和参数的更新标志，默认0表示不更新
    char updateFlag = 0;

    switch ( execInfo )
    {
        case NO_OPERATION:
            updateFlag = 0;
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "ERROR: No Operation()!\n" );
            break;

        case OPEN_CONNECT:
            updateFlag = 0;
            //OpenConnect(dev);
            break;

        case CLOSE_CONNECT:
            updateFlag = 0;
            //CloseConnect(dev);
            //// 设置通道故障,通知上层框架
            //g_chnlQCnxt_p[dev.chnlID] = 0;
            //ASDUDuQ.setCnxtQ(0x1, dev.chnlID, &g_chnlQCnxt_p[dev.chnlID]);
            break;

        case RETRY_CONNECT:
            updateFlag = 0;
            //RetryConnect(dev);
            break;

        case READ_CONNECT: // 读取数据帧
            updateFlag = 1;

            break;

        default:
            updateFlag = 0;
    }

    if ( !updateFlag )
    {
        return ;
    }

    //1.解析APDU数据
    // 返回值的高字节为帧类型/错误处理的执行操作，低字节为传输状态
    // 当传输状态为MC_APDU_OK,高字节表示帧类型;否则,高字节表示错误处理的执行操作。
    // DataFrm直接返回ASDU数据帧
    u_short statusInfo;
    statusInfo = chkAPDUData (
                 DataFrm,       // 接收的数据帧
                 realParam_p,   // 通道实时参数
                 tranCtrlCfg_p  // 规约配置参数
                 );
    // 获取低位字节,传输状态
    u_char errInfo = statusInfo & 0xFF;
    // 获取高位字节,帧类型/错误处理
    u_char frmType = ( statusInfo >> 8 ) & 0xFF;
    // 协调控制本地状态和参数的标志
    u_short ctrlFlag = 0;
    //定时器操作码
    u_char tmCtrlCode = 0;

    //
    char szType[5] = {'I', 'S', ' ', 'U'};
    char szTmp[256 * 3] = {0};

    for ( int i = 0; i < revAPDU.APDU_Buf.APDU_Lng; i++ )
    {
        sprintf ( szTmp + ( i * 3 ), "%02x ", revAPDU.APDU_Buf.APDU_Data_p[i] );
    }

    char cType = frmType & 3;
    cfgMng->tclLog.LPrintf ( cType, DEBUG_LEVEL_1, "r::%c %s\r\n", szType[cType], szTmp );

    // 解析APDU成功，计算双缓冲地址，并存储接收数据
    if ( ( errInfo == MC_APDU_OK ) && ( ( frmType == I_F ) || frmType == U_F ) )
    {
        // 计算通道上的双缓冲接收缓冲地址序号
        u_char bufIndex;
        u_char i = chnlID % 64;

        if ( bufFlag[0] & ( 0x1 << i ) )
        {
            bufIndex = 0;
            bufFlag[0] &= ~ ( 0x1 << i );
        }
        else
        {
            bufIndex = 1;
            bufFlag[0] |= ( 0x1 << i );
        }

        // 更新通道上方的数据长度
        recv_p[bufIndex].ASDU_Buf.ASDU_Lng =  DataFrm->ASDU_Buf.ASDU_Lng;
        // 将I帧数据复制到通道上方,ASDU数据内容供上层框架使用
        char* data_p = ( char* ) recv_p[bufIndex].ASDU_Buf.ASDU_Data_p;
        int iCpyLng = frmType != I_F ? iDataLng : DataFrm->ASDU_Buf.ASDU_Lng;
        memcpy (
        data_p,
        DataFrm->ASDU_Buf.ASDU_Data_p,  // 数据内容
        iCpyLng                         // 数据长度
        );

        // 构建本地ASDU单元
        qASDUInfoRcvUnit ASDUUnit;
        ASDUUnit.chanlID = chnlID;
        ASDUUnit.ASDU_Lng = DataFrm->ASDU_Buf.ASDU_Lng;
        ASDUUnit.ASDU_p = ( u_char* ) data_p;
        ASDUUnit.realData_lng = iCpyLng;
        char flag = 0;
		cfgMng->tclLog.LPrintf(DEBUG, DEBUG_LEVEL_0, "hdlRecvData:ASDUInfoQ.pshQ \r\n");
        flag = ASDUInfoQ.pshQ ( FST_GROUP, chnlID, ( char* ) &ASDUUnit, sizeof ( ASDUUnit ) );
		cfgMng->tclLog.LPrintf(DEBUG, DEBUG_LEVEL_0, "hdlRecvData:: ASDUInfoQ.pshQ end chnl=%d \r\n", chnlID);
        if ( flag < 0 ) // 入队失败
        {
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "ASDU push error:%d", flag );
            return;
        }

        //2.ASDU入队成功后，更新接收端的状态和实时参数
        // 返回值用于协调框架控制，每位含义如下：
        // 低第1位（0）：设置定时器，非零表示需要框架对定时器进行设置，否则不需要设置
        // 低第2位（1）：设置流量控制，非零表示需要框架对发送操作控制流量，否则不需要设置
        // 低第3位（2）：是否需要发送端协作，是的话，需要构造传输控制数据
        // 低8-16位：帧类型标识
        ctrlFlag = UpdateRecvState (
                   revSiteRole,   // 站点角色
                   realParam_p,   // 通道实时参数
                   tranCtrlCfg_p, // 通道配置参数
                   &tmCtrlCode,   // 定时器控制码
                   ( u_char* ) /*DataFrm*/revAPDU.ASDU_Buf.ASDU_Data_p, // 数据内容
                   /*DataFrm->*/revAPDU.ASDU_Buf.ASDU_Lng     // 数据长度
                   );

        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_1,
                                 "realParam_p:: sendnum=%d, receivenum=%d, wValue=%d, kValue=%d \r\n \r\n",
                                 ( * ( realTimeValue* ) realParam_p ).sendnum,
                                 ( * ( realTimeValue* ) realParam_p ).receivenum,
                                 ( * ( realTimeValue* ) realParam_p ).wValue,
                                 ( * ( realTimeValue* ) realParam_p ).kValue );

        // 判定是否对定时器进行设置
        if ( ctrlFlag & 0x1 )
        {
            SetCtrlTimer ( chnlObj_p, tmCtrlCode );    // 设置定时器
        }

        // 判定是否停止数据（对流量进行控制，此处必然为关闭操作）
        if ( ctrlFlag & 0x2 )
        {
            g_chnlQCnxt_p[chnlID] = 0x1;
            // 设置通道上下文状态
            SetFlowCtrl (
            0x1,         // 传输控制请求队列标志
            chnlID,      // 通道（组）ID号
            &g_chnlQCnxt_p[chnlID]  // 通道状态值
            );
        }

        // 是否需要发送侧协调,高8位代表帧的类型,如U、I和S等
        if ( ctrlFlag & 0x4 )
        {
            // 在通道上传输控制指针处, 构建传输控制单元
            void* ctrl_p = chnlObj_p->getTranCtrl();

            // 构建本地传输控制单元
            qTransCtrlInfoUnit CtrlUnit;
            CtrlUnit.chanlID = chnlID;
            CtrlUnit.ctrlFrmDsc.ctrlFrm_p = ( char* ) ctrl_p;

            RespCtrlFrame (
            &CtrlUnit.ctrlFrmDsc,
            realParam_p,
            ( ctrlFlag >> 8 ) & 0xFF
            );

            char flag = 0;
			cfgMng->tclLog.LPrintf(DEBUG, DEBUG_LEVEL_0, "hdlRecvData:ASDUDuQ.pshQ \r\n");
            flag = ASDUDuQ.pshQ ( SCD_GROUP, chnlID, ( char* ) &CtrlUnit, sizeof ( CtrlUnit ) );
			cfgMng->tclLog.LPrintf(DEBUG, DEBUG_LEVEL_0, "hdlRecvData:: ASDUDuQ.pshQ end chnl=%d \r\n", chnlID);
            if ( flag < 0 ) // 入队失败
            {
                cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                         "CtrlUnit push error:%d\n", flag );
                return;
            }
        }
    }

    // 解析APDU成功，且APDU为控制帧
    else if ( ( errInfo == MC_APDU_OK ) && ( frmType == S_F/*CTRL_FRAME*/ ) )
    {
        // 更新接收端的状态和实时参数
        // 返回值用于协调框架控制，每位含义如下：
        // 低第1位（0）：设置定时器，非零表示需要框架对定时器进行设置，否则不需要设置
        // 低第2位（1）：设置流量控制，非零表示需要框架对发送操作控制流量，否则不需要设置
        // 低第3位（2）：是否需要发送端协作，是的话，需要构造传输控制数据
        // 低8-16位：帧类型标识
        ctrlFlag = UpdateRecvState (
                   revSiteRole,   // 站点角色
                   realParam_p,   // 通道实时参数
                   tranCtrlCfg_p, // 通道配置参数
                   &tmCtrlCode,   // 定时器控制码
                   ( u_char* ) revAPDU.ASDU_Buf.ASDU_Data_p, // 数据内容
                   revAPDU.ASDU_Buf.ASDU_Lng     // 数据长度
                   );

        // 判定是否对定时器进行设置
        if ( ctrlFlag & 0x1 )
        {
            SetCtrlTimer ( chnlObj_p, tmCtrlCode );    // 设置定时器
        }

        // 判定是否停止发送数据（对流量进行控制，此处必然为关闭操作）
        if ( ctrlFlag & 0x2 )
        {
            g_chnlQCnxt_p[chnlID] = 0x1;
            // 设置通道上下文状态
            SetFlowCtrl (
            0x1,         // 传输控制请求队列标志
            chnlID,      // 通道（组）ID号
            &g_chnlQCnxt_p[chnlID]  // 通道状态值
            );
        }

        // 需要发送侧协调,notifyFlag =1
        // ctrlflag 高8位代表帧的类型,如U、I和S等
        if ( ctrlFlag & 0x4 )
        {
            // 在通道上传输控制指针处, 构建传输控制单元
            void* ctrl_p = chnlObj_p->getTranCtrl();

            qTransCtrlInfoUnit CtrlUnit;
            CtrlUnit.chanlID = chnlID;
            CtrlUnit.ctrlFrmDsc.ctrlFrm_p = ( char* ) ctrl_p;

            RespCtrlFrame (
            &CtrlUnit.ctrlFrmDsc, //数据单元结构
            realParam_p,           //实时参数
            ( ctrlFlag >> 8 ) & 0xFF //数据帧类型
            );
            char flag = 0;
			cfgMng->tclLog.LPrintf(DEBUG, DEBUG_LEVEL_0, "hdlRecvData:ASDUDuQ.pshQ \r\n");		
            flag = ASDUDuQ.pshQ ( SCD_GROUP, chnlID, ( char* ) &CtrlUnit, sizeof ( CtrlUnit ) );
			cfgMng->tclLog.LPrintf(DEBUG, DEBUG_LEVEL_0, "hdlRecvData:: ASDUDuQ.pshQ end chnl=%d \r\n", chnlID);
            if ( flag < 0 ) // 入队失败
            {
                cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                         "CtrlUnit push error:%d", flag );
            }
        }
    }


    // 解析APDU失败，处理传输控制错误
    else
    {
        char opt = statusInfo >> 8 & 0xFF;
        // 构建本地传输控制单元
        qTransCtrlInfoUnit CtrlUnit;
        CtrlUnit.chanlID = chnlID;
        char notifyFlag = HandleRecvStateError ( &CtrlUnit.ctrlFrmDsc, realParam_p, errInfo );

        // 需要发送侧协调
        if ( notifyFlag & 0x1 )
        {
            // 指向通道上的传输控制指针
            char* ctrl_p = ( char* ) chnlObj_p->getTranCtrl();
            // 将数据复制到通道上方的传输控制帧
            memcpy ( ctrl_p, &CtrlUnit, sizeof ( CtrlUnit ) );

            char flag = 0;
			cfgMng->tclLog.LPrintf(DEBUG, DEBUG_LEVEL_0, "hdlRecvData:ASDUDuQ.pshQ \r\n");
            flag = ASDUDuQ.pshQ ( SCD_GROUP, chnlID, ( char* ) &CtrlUnit, sizeof ( CtrlUnit ) );
			cfgMng->tclLog.LPrintf(DEBUG, DEBUG_LEVEL_0, "hdlRecvData:: ASDUDuQ.pshQ end chnl=%d \r\n", chnlID);
            if ( flag < 0 ) // 入队失败
            {
                cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                         "CtrlUnit push error:%d", flag );
            }
        }

        // 不需要发送侧协调,且notifyFlag !=1
        else if ( CtrlUnit.ctrlFrmDsc.ctrlType != 1 )
        {
            // 构造伪ASDU帧
            qASDUInfoRcvUnit ASDUUnit;
            ASDUUnit.ASDU_Lng = errInfo;
            ASDUUnit.realData_lng = 0;
            ASDUUnit.ASDU_p = 0;
            ASDUUnit.chanlID = chnlID;

            char flag = 0;
            flag = ASDUInfoQ.pshQ ( FST_GROUP, chnlID, ( char* ) &ASDUUnit, sizeof ( ASDUUnit ) );
            printf ( "hdlRecvData:: ASDUInfoQ.pshQ chnl=%d \r\n", chnlID );

            if ( flag < 0 ) // 入队失败
            {
                cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                         "ASDU push error:%d\n", flag );
            }
        }

        // 执行协议指定的错误处理方法,根据协议扩展，目前只写了关闭通道连接
        switch ( opt )
        {
            case CLOSE_CONNECT:
                // 获取通道(组)对象,通过通道管理表获取
                C_ChnlObj* chnlObj_p = chnlMgr.getChnlObj ( chnlID );
                // 获取规约描述(规约文件句柄)
                HMODULE* ptclMdl_p = chnlObj_p->getPtclMdl();
                // 获取协议号ID(用来指明传输介质类型，比如网口、串口等)
                u_char ptclID = getPtclID() % 2;
                // 获取通道操作句柄,比如sockets号
                HANDLE hChnlSend = chnlObj_p->getSockHdl();
                DEVICES dev;
                dev.chnlID = chnlID;
                dev.hIntance = hChnlSend;
                dev.type = ptclID;
                CloseConnect ( dev );
                u_int msgBdy[2] = {0};
                msgBdy[0] = dev.chnlID;
                msgBdy[1] = 0x2;
                chnlMgr.clrChnlState ( dev.chnlID, MC_CHNL_BIT );   // 更新通道状态，通道状态位置零

                // 其他操作需要上层框架配合，需要发送信号及信息包到上层
                if ( g_errTID == VALIDTTID )
                {
                    cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "g_errTID = NULL/n!" );
                }
                else
                {
                    ssnFnshNfy ( g_errTID, ( char* )msgBdy, 2 * sizeof ( u_int ), ssnFnshNfyID );
                }

                cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_1,
                                         "hdlRecvData:: tm-%d send to E_takeErr, msg=%d %d \r\n", dev.type, msgBdy[0], msgBdy[1] );
                break;
                //case NO_OPERATION:
        }
    }

}


/*******************************************************************************
* 功能：处理网络接收错误事件
* 描述：首先，它从数据信息中提取通道ID(chnlID)，通过它和通道管理表对象就可以获得通道对象。
*      接着，它从通道对象中提取通道的角色、操作句柄、规约类型以及其他参数等信息。
*      然后，根据规约解析接收数据。
*
*******************************************************************************/
void hdlRecvError ( u_short chnlID )
{
    // 处理网络事件对应的套接字错误
    cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_1,
                             "ERROR: Get SOCKET information error!" );

    // 获取通道(组)对象,通过通道管理表获取
    C_ChnlObj* chnlObj_p = chnlMgr.getChnlObj ( chnlID );
    // 获取规约描述(规约文件句柄)
    HMODULE* ptclMdl_p = chnlObj_p->getPtclMdl();
    // 获取协议号ID(用来指明传输介质类型，比如网口、串口等)
    DLL_getPtclID getPtclID;
    getPtclID = ( DLL_getPtclID ) GetProcAddress ( *ptclMdl_p, "getPtclID" );

    if ( !getPtclID )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function getPtclID()!\n" );
        return;
    }

    u_char ptclID = getPtclID() % 2;
    // 获取通道操作句柄,比如sockets号
    HANDLE hChnlSend = chnlObj_p->getSockHdl();

    // 声明传输介质接口变量
    DEVICES dev;
    dev.chnlID = chnlID;
    dev.type = ptclID;
    dev.hIntance = hChnlSend;
    // 关闭通道
    CloseConnect ( dev );
}


///////////////////  3.通道操作功能的实现       //////////////////////////////////

/*******************************************************************************
* 功能：关闭通道连接
* 描述：根据设备类型dev_type的值，选择相对应的关闭通道方法。其中，发起关闭通道连接的命令
*      是由具体协议标准来负责，它通过调用本函数完成关闭操作。
* 接口参数：
*     1.dev      设备接口信息；
* 返回值：0：关闭通道连接成功； >=1：关闭通道连接失败，非法的设备类型
*******************************************************************************/
int CloseConnect ( DEVICES dev )
{
    switch ( dev.type )
    {
        case DEVICE_COM:  // 串口
            //CloseHandle(dev.hIntance);
            break;

        case DEVICE_NET:  // 网口
#ifdef WINDOWS_PLATFORM
            shutdown ( * ( sock_descriptor* ) dev.hIntance, 0x02 );
            closesocket ( * ( sock_descriptor* ) dev.hIntance ); // 不能关闭TCP句柄，要等待新的句柄替换掉（zhaojj,20170419）
#else
            shutdown ( * ( sock_descriptor* ) dev.hIntance, SHUT_RDWR );
            close ( * ( sock_descriptor* ) dev.hIntance );
#endif

            * ( sock_descriptor* ) dev.hIntance = sock_invalid; // 关闭后，端口号清掉，zhaojj,20170620
            break;

        default:  // 非法的设备类型
            return 1;
    }

    return 0;
}

/*******************************************************************************
* 功能：通道数据的底层发送操作
* 描述：根据设备类型dev_type的值首先，选择相对应的底层数据发送方法。
* 接口参数：
*     1.dev      设备接口信息；
*     2.buf      通道数据；
*     3.length   数据长度；
*     4.cfgParam 设备(通道)的配置参数，此处只需要处理发送控制参数
* 返回值：
*     >0： 发送成功，0：发送失败
*******************************************************************************/
int WriteToConnect ( DEVICES dev, char* buf, int length, void* cfgParam )
{
    // 数据发送的控制模式, 它有3种工作模式。
    // SEND_NO_CONTROL：无控制模式；
    // SEND_TIMEOUT_CONTROL: 超时控制模式
    // SEND_RETYR_CONTROL: 重试次数控制模式
    char mode = cfgParam != NULL ? * ( ( char* ) cfgParam ) : 0; // cfgParam.mode;

    // 数据发送的控制参数，
    char mparam = cfgParam != NULL ? ( * ( char* ) cfgParam ) : 0; // cfgParam.mparam;

    // 发送控制模式为次数限制（重试次数）
    switch ( mode )
    {
        // 无控制模式
        case SEND_NO_CONTROL:
            switch ( dev.type )
            {
                case DEVICE_COM:  // 串口API发送函数
#ifdef WINDOWS_PLATFORM
                    u_char writeStatus;
                    writeStatus = WriteFile ( dev.hIntance, buf, length, NULL, NULL );

                    if ( !writeStatus )
                    {
                        cfgMng->tclLog.LPrintf ( true, DEBUG_LEVEL_0, "sand failed!" );
                        return 0;
                    }// 发送数据失败
                    else
                    {
                        return 1;
                    }

#else
                    int writeStatus;
                    writeStatus = write ( * ( int* ) ( dev.hIntance ), buf, length );

                    if ( writeStatus != 0 )
                    {
                        return writeStatus;  // 发送数据失败
                    }
                    else
                    {
                        return 0;
                    }

#endif

                    break;

                case DEVICE_NET:  // 网口API发送函数
                    int sendBytes;
                    sendBytes = send ( * ( sock_descriptor* ) ( dev.hIntance ), buf, length, 0 );
                    cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_1,
                                             "WriteToConnect::sendBytes=%d, send len=%d ,errno= %d,errorMsg =%s ,hIntance =%d, chnl = %d\r\n",
                                             sendBytes, length, errno, strerror ( errno ), * ( int* ) ( dev.hIntance ), dev.chnlID );

                    if ( sendBytes != length )
                    {
                        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                                 "WriteToConnect:: send len=%d ,errno= %d,errorMsg =%s ,hIntance =%d, chnl = %d\r\n",
                                                 length, errno, strerror ( errno ), * ( int* ) ( dev.hIntance ), dev.chnlID );
                        return 0;  // 发送数据失败
                    }
                    else
                    {
                        return 1;
                    }

                    break;

                default:  // 非法的设备类型
                    return 0;
            }

            break;

        // 超时控制模式
        case SEND_TIMEOUT_CONTROL:
            double start;
#ifdef WINDOWS_PLATFORM
            start = GetTickCount(); // 获取系统当前的运行时间

            while ( ( GetTickCount() - start ) < mparam )

#else
            struct timeval old_tv;

            struct timeval now_tv;
            gettimeofday ( &old_tv, NULL );
            gettimeofday ( &now_tv, NULL );

            while ( ( ( now_tv.tv_sec * 1000 ) - ( old_tv.tv_sec * 1000 ) ) < mparam )
#endif

            {
                switch ( dev.type )
                {
                    case DEVICE_COM:  // 串口API发送函数
#ifdef WINDOWS_PLATFORM
                        u_char writeStatus;
                        writeStatus = WriteFile ( dev.hIntance, buf, length, NULL, NULL );

                        if ( !writeStatus ) // 数据发送失败
                        {
                            return 1;
                        }
                        else
                        {
                            return 0;
                        }

#else
                        int writeStatus;
                        writeStatus = write ( * ( int* ) ( dev.hIntance ), buf, length );

                        if ( writeStatus != 0 )
                        {
                            return writeStatus;  // 发送数据失败
                        }
                        else
                        {
                            return 0;
                        }

#endif
                        break;

                    case DEVICE_NET:  // 网口API发送函数
                        u_char sendBytes;
                        sendBytes = send ( * ( sock_descriptor* ) ( &dev.hIntance ), buf, length, 0 );

                        // 数据发送失败
                        if ( sendBytes != length )
                        {
                            return 1;
                        }
                        else
                        {
                            return 0;
                        }

                        break;

                    default:  // 非法的设备类型
                        return 0;
                }

#ifndef WINDOWS_PLATFORM
                gettimeofday ( &now_tv, NULL );
#endif
            }

            return 0; //超时返回
            break;

        // 重试次数控制模式
        case SEND_RETYR_CONTROL:
            int i = 0;  // 重试次数变量

            while ( i < mparam )
            {
                switch ( dev.type )
                {
                    // 串口API发送函数
                    case DEVICE_COM:
#ifdef WINDOWS_PLATFORM
                        u_char writeStatus;
                        writeStatus = WriteFile ( dev.hIntance, buf, length, NULL, NULL );

                        // 若数据发送失败
                        if ( !writeStatus )
                        {
                            i++;
                        }
                        else
                        {
                            return 0;
                        }

#else
                        int writeStatus;
                        writeStatus = write ( * ( int* ) ( dev.hIntance ), buf, length );

                        if ( writeStatus != 0 )
                        {
                            i++;  // 发送数据失败
                        }
                        else
                        {
                            return 0;
                        }

#endif
                        break;

                    // 网口API发送函数
                    case DEVICE_NET:
                        u_char sendBytes;
                        sendBytes = send ( * ( sock_descriptor* ) ( &dev.hIntance ), buf, length, 0 );

                        // 若数据发送失败
                        if ( sendBytes != length )
                        {
                            i++;
                        }
                        else
                        {
                            return 0;
                        }

                        break;

                    default:  // 非法的设备类型
                        return 0;
                }
            }

            // 重试次数控制模式发送数据失败
            return 0;
            break;
    }

    // 非法的发送模式
    return 0;
}

/*******************************************************************************
* 功能：底层通道发送端流量控制
* 描述：主要用于传输控制层发送端的流量处理，即设置C_DuGrp_BCPQ队列的出队上下文状态，调节
*      通道状态，暂缓或者继续从队列中取出发送数据。
*      调用上层框架中的定时器操作函数ctlTmr()完成定时器操作。
* 接口参数：
*     1.grpId   组Id。=1，第1组(传输控制请求队列)；=2，第2组（ASDU请求队列）；
*     2.qIdx    队列索引（通道（组）号）。=0xFFFF FFFF，设置全组
*     3.cnxt_p  上下文状态指针（=1，开启通道； =0，关闭通道）
* 返回值：无
*******************************************************************************/
void SetFlowCtrl ( u_char grpId, u_short qIdx, void* cnxt_p )
{
    if ( ASDUDuQ.setCnxtQ ( grpId, qIdx, cnxt_p ) != 0 )
    {
        if ( grpId == 1 )
        {
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ERROR: Can't set context of TransCtrl Request Queue\n" );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ERROR: The current channel is %d\n", qIdx );
        }

        if ( grpId == 2 )
        {
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ERROR: Can't set context of ASDU Request Queue\n" );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ERROR: The current channel is %d\n", qIdx );
        }
    }
}


///////////////////  4.定时器功能的实现       ////////////////////////////////////
/*******************************************************************************
* 功能：设置通道定时器
* 描述：主要用于传输控制层的超时处理，它既可以用于底层发送端，也可以用于底层接收端。其中，
*      定时器的控制码保存在于通道（组）对象中，它可以在UpdateSendState()函数或
*      UpdateReceiveState()函数中完成更新数值的操作。此处仅找到需要设置的定时器ID，再
*      调用上层框架中的定时器对象C_TmObj的方法ctlTmr()来完成定时器操作。
* 接口参数：
*     1.chnlObj_p  通道对象指针；
*     2.tmCtrlCode 定时器组的操作码；每2位表示一种定时器，其含义表示如下：
*        0x0: 无操作；0x1:启动定时器；0x2：停止或重新复位定时器；其他值略
* 返回值：无
*******************************************************************************/
void SetCtrlTimer ( C_ChnlObj* chnlObj_p, u_char tmCtrlCode )
{
    // 获取传输控制层定时器资源头指针
    tmChnl* tmChnl_p = NULL;
    chnlObj_p->getTimer ( tmChnl_p );

    // 定时器编号
    u_char stID = 0;

    // 查找被操作的定时器，并对相应的定时器对象进行置
    while ( tmCtrlCode != TIMER_NOP )
    {
        if ( ( tmCtrlCode & 0x3 ) !=  TIMER_NOP )
        {
            // 设置通道控制码，供上层框架使用
            chnlObj_p->setCtrlCode ( stID, tmCtrlCode & 0x3 );
            tmFclty.ctlTmr ( tmChnl_p, stID, ( tmCtrlCode & 0x3 ) ); // 20170603, by zhaojj
        }

        stID++;
        tmCtrlCode = tmCtrlCode >> 2;
    }
}

/////////////////////// 定时器超时控制函数 /////////////////////
// 必须根据规约，此部分函数有可能需要重构。
/*******************************************************************************
* 功能：通用定时器回调函数
* 描述：根据IEC104规约, 如果在t0时间内没有成功建立连接。
* 接口参数：
*     1.chnlID   通道（组）号信息；
* 返回值：0：发送成功； >=1：发送失败
*     1： 非法的控制模式；             2： 非法的设备类型,即不合理的传输介质的接口类型；
*******************************************************************************/
void TimerService ( void* param )
{
    int iRet = 0;


    // 获得通道的传输介质
    PDEVICES dev = ( PDEVICES ) param;

    // 获取通道(组)对象,通过通道管理表获取
    C_ChnlObj* chnlObj_p = chnlMgr.getChnlObj ( dev->chnlID );

    if ( chnlObj_p == NULL )
    {
        return ;
    }

    HMODULE* hdl = chnlObj_p->getPtclMdl();

    // 获取规约库接口指针，得到相应操作码
    DLL_timerRingAdd tTimeOut = ( DLL_timerRingAdd ) GetProcAddress ( *hdl, "timerRingAdd" );

    if ( tTimeOut == NULL )
    {
        return ;
    }

    int retCode = tTimeOut ( dev->type );

    DEVICES tmpDev ;
    memcpy ( &tmpDev, dev, sizeof ( DEVICES ) );

    // 低第1位（位0）：至1为关闭定时器; 低第2位（位1）：至1为关闭链接
    // 低第3位（位2）：至1为发送S帧; 低第4位（位3）：至1为发送测试激活
    if ( retCode & 0x4 ) // 发送S帧
    {
        Timer2Service ( /*param*/ ( void* ) &tmpDev );
        return ;
    }
    else
    {
        //
        if ( retCode & 0x2 )
        {
            u_short chnlState = chnlMgr.getChnlState ( dev->chnlID );

            if ( ( chnlState & MC_CHNL_BIT ) != 0 )
            {
                // chnl's error

                // 获取通道操作句柄,比如sockets号
                tmpDev.hIntance = chnlObj_p->getSockHdl();
                // 关闭连接
                CloseConnect ( tmpDev );
                //
                //*(sock_descriptor*)tmpDev.hIntance = 0;

                cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_1,
                                         "[WARNING]TimerService:: close socket of chnl=%d \r\n", dev->chnlID );
                chnlMgr.clrChnlState ( dev->chnlID, MC_CHNL_BIT );  // 更新通道状态，通道状态位置零
            }

        }

        // FORMAT: CHNL_ID, CODE
        u_int msgBdy[2] = {0};
        msgBdy[0] = dev->chnlID;
        msgBdy[1] = retCode;

        // 其他操作需要上层框架配合，需要发送信号及信息包到上层
        if ( g_errTID == VALIDTTID )
        {
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "g_errTID = NULL/n!" );
        }
        else
        {
            ssnFnshNfy ( g_errTID, ( char* )msgBdy, 2 * sizeof ( u_int ), ssnFnshNfyID );
        }

        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_1,
                                 "TimerService:: tm-%d send to E_takeErr, msg=%d %d \r\n",
                                 dev->type, msgBdy[0], msgBdy[1] );
    }

}
/*******************************************************************************
* 功能：定时器T0
* 描述：根据IEC104规约,如果在t0时间内没有成功建立连接。
* 接口参数：
*     1.chnlID   通道（组）号信息；
* 返回值：0：发送成功； >=1：发送失败
*     1： 非法的控制模式；             2： 非法的设备类型,即不合理的传输介质的接口类型；
*******************************************************************************/
void Timer0Service ( void )
{

}

/*******************************************************************************
* 功能：定时器T1
* 描述：根据IEC104规约,如果发送一个I或U格式报文后，必须在t1时间内（默认值为15秒）得到对方
*      的认可，否则重新连接。
* 接口参数：
*     1.param    定时器输入参数信息；
* 返回值：
*     无;
*******************************************************************************/
void Timer1Service ( void* param )
{
    /*tTmrCfg.name_p = tTmrNm;
    tTmrCfg.tmFunHdl = ssnTmOutFun_p;
    tTmrCfg.tmFunHdlPara_p = (void *)
    tTmrCfg.type = 0;
    tTmrCfg.fstXpirTm = tSsnReqBlk.timeout;

    if(!tmFclty.addTmr(tmChnlID), &tTmrCfg)
    添加定时器时，赋值给相应字段*/

    //PDEVICES dev = (DEVICES*)param;

    //重新连接
    //RetryConnect(*dev);

}

/*******************************************************************************
* 功能：定时器T2
* 描述：根据IEC104规约,接收一个I格式报文以后，如果在t2时间内（默认值为10秒）没有再收到新的
*      I格式报文，就必须发送S格式帧对I格式进行认可。
* 接口参数：
*     1.param    定时器输入参数信息；
* 返回值：
*     无;
*******************************************************************************/
void Timer2Service ( void* param )
{
    // 获得通道的传输介质
    PDEVICES dev = ( DEVICES* ) param;
    // 获取通道(组)对象,通过通道管理表获取
    C_ChnlObj* chnlObj_p = chnlMgr.getChnlObj ( dev->chnlID );
    // 获取传输控制层实时参数指针
    void* realParam_p = chnlObj_p->getRealParam();

    // 获取规约描述(规约文件句柄)
    HMODULE* ptclMdl_p = chnlObj_p->getPtclMdl();

    // 获取协议号ID(用来指明传输介质类型，比如网口、串口等)
    DLL_CtrlFrame CtrlFrame;
    CtrlFrame = ( DLL_CtrlFrame ) GetProcAddress ( *ptclMdl_p, "CtrlFrame" );

    if ( !CtrlFrame )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function CtrlFrame()!\n" );
        return;
    }

    // 传输控制单元
    char sendBuf[MC_DEF_MAX_LEN] = {0};
    qTransCtrlInfoUnit CtrlUnit;
    CtrlUnit.ctrlFrmDsc.ctrlFrm_p = sendBuf;
    // 构造确认帧-S帧
    CtrlFrame (
    &CtrlUnit.ctrlFrmDsc,
    realParam_p,
    S_F
    );

    if ( CtrlUnit.ctrlFrmDsc.frmLng <= 0 )
    {
        // 不需要发送s帧
        return ;
    }

    // 发送应答帧
    dev->type = DEVICE_NET;
    dev->hIntance = chnlObj_p->getSockHdl();
    int iRet = WriteToConnect ( *dev, CtrlUnit.ctrlFrmDsc.ctrlFrm_p, CtrlUnit.ctrlFrmDsc.frmLng, NULL );

    if ( iRet == 0 )
    {
        // fail
        cfgMng->tclLog.LPrintf ( true, DEBUG_LEVEL_0,
                                 "Error: WriteToConnect() failed! \n" );
        return ;
    }

    return ;
}

/*******************************************************************************
* 功能：定时器T3
* 描述：根据IEC104规约,每接收一个I格式、S格式或U格式的报，重新计数。如果在t3时间内（默认值
*      为20秒）没有再收到任何报文，需要向对方发送链路测试帧TestFR。
* 接口参数：
*     1.dev      设备接口信息；
* 返回值：0：发送成功； >=1：发送失败
*     1： 非法的控制模式；             2： 非法的设备类型,即不合理的传输介质的接口类型；
*******************************************************************************/
void Timer3Service ( void* param )
{
    // 待添加：此处判断是否有上层提供的超时处理函数，如果有，则走上层提供的处理流程
    // ...
    // 没有，则走下面处理流程


    // 获得通道的传输介质
    PDEVICES dev = ( DEVICES* ) param;
    // 获取通道(组)对象,通过通道管理表获取
    C_ChnlObj* chnlObj_p = chnlMgr.getChnlObj ( dev->chnlID );
    // 获取传输控制层实时参数指针
    void* realParam_p = chnlObj_p->getRealParam();

    // 获取规约描述(规约文件句柄)
    HMODULE* ptclMdl_p = chnlObj_p->getPtclMdl();

    // 获取协议号ID(用来指明传输介质类型，比如网口、串口等)
    DLL_CtrlFrame CtrlFrame;
    CtrlFrame = ( DLL_CtrlFrame ) GetProcAddress ( *ptclMdl_p, "CtrlFrame" );

    if ( !CtrlFrame )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function CtrlFrame()!\n" );
        return;
    }

    // 构造TESTFR测试帧
    // 传输控制单元
    static qTransCtrlInfoUnit CtrlUnit;
    // 构造确认帧-U帧
    CtrlFrame (
    &CtrlUnit.ctrlFrmDsc,
    realParam_p,
    IEC104_TESTFR_ACT
    );

    // 发送测试帧
    //WriteToConnect(DEVICES dev, char * buf, int length, void *cfgParam);
}


