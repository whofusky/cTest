/*
* �ļ���TransCtrlLayer.cpp
* ˵����ʵ�ִ�����Ʋ㷢�Ͳ�ͽ��ղ����Ӧ���ܡ�
*/
#include "TransCtrlLayer.h"
#include "sysMgr.h"
#include "duGrp_BCPQ.h"
#include <error.h>
#include <time.h>
#include "cfgManage.h"

/******************************************************************************/
/*                        �ⲿȫ�ֱ���                                         */
/******************************************************************************/
//---------  "duGrp_BCPQ.h" ---------------------
//���ö���ȫ�ֱ�����������ƶ����뷢��ASDU���й���һ�����У�
extern C_DuGrp_BCPQ ASDUDuQ;
//���ö���ȫ�ֱ�������Ӧ�ò㴫��ASDU�Ķ��У�
extern C_DuGrp_BCPQ ASDUInfoQ;
//---------   "sysMgr.h"    ---------------------
//����ͨ�������ȫ�ֱ���
extern C_ChnlMgr chnlMgr;
extern nsCPThread::CThread* thdObj_p;
// �����¼�������׽��־����
extern EVENT**             g_evtArr_pp;
extern sock_descriptor**   g_sockArr_pp;
// ͨ�����ĵ�ַ
extern u_char*   g_chnlQCnxt_p;

extern int ssnFnshNfyID;
extern nsCPSocket::CTcpClient g_locClient;
extern int                      g_locPort;
extern C_SsnCtnrObj ssnCtnr; // �Ự����

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
///////////////////  1.���Ͳ�Ĺ���ʵ��         //////////////////////////////////
/*******************************************************************************
* ���ܣ�������Ʋ㷢�Ͳ��̺߳���
* �����������ȴ������Դ�����ƶ��е������¼�,��û�д�����������¼�,�ٴ�������ASDU���е�
*      �����¼������У������¼��Ĵ����Ѿ��ɶ��ж�����ɡ�ͨ�����ó��Ӻ������˴��Գ�������
*      ������Ӧ�ش���
* ���ݴ���ӿڣ�
*     1.hdlTransCtrlReqEvnt()   - �������Դ��������Ϣ���е�����
*     2.hdlASDUInfoSndReqEvnt() - ��������ASDU����������е�����
*******************************************************************************/
u_long Tsys_NetSndProc ( void* lpParam )
{
    // ���������Ϣ���е����ݵ�Ԫ���ȣ�
    u_char ctrLng = sizeof ( qTransCtrlInfoUnit );
    // ASDU���е����ݵ�Ԫ���ȣ�
    u_char datLng = sizeof ( qASDUInfoRcvUnit );
    // �������ӵ����ݵ�Ԫ���ȣ���Ҫ��������������ݿռ䣩
    const u_char InfoLng = ( ctrLng > datLng ) ? ctrLng : datLng;

    // �������ӵ����ݵ�Ԫ
    char* InfoData ;
    InfoData = ( char* ) calloc ( InfoLng, sizeof ( char ) );

    // ���ӽ��������
    int tRtn;

    while ( true )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_1, "Tsys_NetSndProc: ASDUDuQ.popQ\r\n" );
        // ���ȴ�������ƶ����¼���
        tRtn = 0;
        u_char infoLng = InfoLng;
        tRtn = ASDUDuQ.popQ ( BOTH_GROUP, InfoData, &infoLng );
		cfgMng->tclLog.LPrintf(DEBUG, DEBUG_LEVEL_1, "Tsys_NetSndProc: ASDUDuQ.popQ end\r\n");
        // ���Ӷ���Ϊ������ƶ���
        if ( ( tRtn == ( SCD_GROUP | 0x10 ) ) && ( infoLng > 0 ) )
        {
            hdlTransCtrlReqEvnt ( ( qTransCtrlInfoUnit* ) InfoData, infoLng );
        }

        // ���Ӷ���ΪASDU����
        else if ( ( tRtn == ( FST_GROUP | 0x10 ) ) && ( infoLng > 0 ) )
        {
            hdlASDUInfoSndReqEvnt ( ( qASDUInfoRcvUnit* ) InfoData, infoLng );
        }
        // ���Ӵ���
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

    //�ͷſռ�
    free ( InfoData );
    return 0;
}

/*******************************************************************************
* ���ܣ������������Ϣ���е�����
* ���������ȣ�����������Ϣ����ȡͨ��ID(chnlID)��ͨ������ͨ����������Ϳ��Ի��ͨ������
*      ���ţ�����ͨ����������ȡͨ���Ľ�ɫ�������������Լ�����Լ����������������ݷ��͵Ĳ�
*      ������Ϣ��Ȼ�󣬸��ݹ�Լ��鷢�Ͷ˵�׼������(������״̬��Ǩ��ͼ)�����׼������������
*      ��ִ�з��Ͳ����������ͳɹ������·��Ͷ˵Ŀ��Ʋ�������󣬸��ݿ���Ҫ��Ե�ǰͨ������
*      ��ʱ��ʱ�����ú������������ơ�
* �ӿڲ�����
*     1.ctrlUnitObj_p  ���������Ϣ���е����ݵ�Ԫ����ָ�룻
*     2.dBlkLng        ���ݳ��ȣ�
*******************************************************************************/
void hdlTransCtrlReqEvnt ( qTransCtrlInfoUnit* ctrlUnitObj_p, u_char dBlkLng )
{
    /**************************  �����ݵ�Ԫ����ȡ�����Ϣ **************************/
    // ��ȡͨ��(��)����,ͨ��ͨ��������ȡ
    C_ChnlObj* chnlObj_p = chnlMgr.getChnlObj ( ctrlUnitObj_p->chanlID );

    // ��ȡ��Լ����(��Լ�ļ����)
    HMODULE* ptclMdl_p = chnlObj_p->getPtclMdl();
    // ��ȡͨ���������,����sockets��
    HANDLE hChnlSend = chnlObj_p->getSockHdl();
    // ��ȡ������Ʋ�ʵʱ����ָ��
    void* realParam_p = chnlObj_p->getRealParam();
    // ��ȡ������Ʋ����ò���ָ��
    void* tranCtrlCfg_p = chnlObj_p->getTranCtrlCfg();
    // ��ȡվ��Ľ�ɫ
    int sndSiteRole = chnlObj_p->getRole();

    /**************************  ���ع�Լ������ **********************************/
    //1. ��ȡЭ���ID
    DLL_getPtclID getPtclID;
    getPtclID = ( DLL_getPtclID ) GetProcAddress ( *ptclMdl_p, "getPtclID" );

    if ( !getPtclID )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function getPtclID()!\n" );
        return;
    }

    //2. ��װAPDU����
    DLL_packAPDU packAPDU;
    packAPDU = ( DLL_packAPDU ) GetProcAddress ( *ptclMdl_p, "packAPDU" );

    if ( !packAPDU )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function packAPDU()!\n" );
        return;
    }

    //3. ��ȡASDU���ݶ����е�����,�����APDU�е��ֽ�������
    DLL_getDU getDU;
    getDU = ( DLL_getDU ) GetProcAddress ( *ptclMdl_p, "getDU" );

    if ( !getDU )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function packAPDU()!\n" );
        return;
    }

    // 4.��鷢��״̬
    DLL_CheckSendState CheckSendState;
    CheckSendState = ( DLL_CheckSendState ) GetProcAddress ( *ptclMdl_p, "CheckSendState" );

    if ( !CheckSendState )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function CheckSendState()!\n" );
        return;
    }

    //5.���·���״̬��ʵʱ����
    DLL_UpdateSendState UpdateSendState;
    UpdateSendState = ( DLL_UpdateSendState ) GetProcAddress ( *ptclMdl_p, "UpdateSendState" );

    if ( !UpdateSendState )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function UpdateSendState()!\n" );
        return;
    }

    /****************************  ���͹��ܴ���  **********************************/
    //1.��ù�ԼЭ���
    u_char ptclID = getPtclID() % 2;
    // ����������ʽӿڱ���
    DEVICES dev;
    dev.chnlID = ctrlUnitObj_p->chanlID;
    dev.type = ptclID;
    dev.hIntance = hChnlSend;

    //2.���ݹ�Լ��鷢��ǰ��״̬�Ƿ����
    //����ֵ�ĸ�16λ��ʾ����״̬��Ϣ,��16λ��ʾ��Ҫִ�еĶ���
    u_int statusInfo;
    statusInfo = CheckSendState (
                 sndSiteRole, // վ���ɫ
                 realParam_p, // ͨ��ʵʱ����
                 ( u_char* ) ctrlUnitObj_p->ctrlFrmDsc.ctrlFrm_p, // ��������
                 ctrlUnitObj_p->ctrlFrmDsc.frmLng     // ���ݳ���
                 );

    //2.1������״̬��Ϣ
    u_short errorInfo = statusInfo & 0xFFFF;

    switch ( errorInfo )
    {
        case SEND_TRANSITION_ERROR:  // ״̬Ǩ�ƴ���
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ERROR: CheckSendState() found STATE TRANSITION Wrong!\n" );
            break;

        case SEND_ROLE_ERROR:        // �Ƿ���ɫ
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ERROR: CheckSendState() found ROLE Wrong!\n" );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ChnlID:= %d\n", dev.chnlID );
            break;

        case SEND_FRAME_ERROR:       // ����֡����
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ERROR: CheckSendState() found FRAME DATA Wrong!\n" );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ChnlID:= %d\n", dev.chnlID );
            break;

        case SEND_STATUS_ERROR:      // �Ƿ�״̬
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ERROR: CheckSendState() found the status doesn't exist!\n" );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ChnlID:= %d\n", dev.chnlID );
            break;

        case SEND_TIMESCALE_ERROR:   // ��ʱ����
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
    //2.2������Ҫִ�еĶ���
    u_short execInfo = ( statusInfo >> 16 ) & 0xFFFF;
    // ����״̬�Ͳ����ĸ��±�־��Ĭ��0��ʾ������
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
            // ����ͨ������,֪ͨ�ϲ���
            //g_chnlQCnxt_p[dev.chnlID] = 0;
            //ASDUDuQ.setCnxtQ(0x1, dev.chnlID, &g_chnlQCnxt_p[dev.chnlID]);
            break;

        case RETRY_CONNECT:
            updateFlag = 0;
            //RetryConnect(dev);
            break;

        case WRITE_CONNECT: // ��������֡
            updateFlag = WriteToConnect (
                         dev,           // ͨ�����飩������ʵĽӿ���Ϣ
                         ctrlUnitObj_p->ctrlFrmDsc.ctrlFrm_p, // ��������
                         ctrlUnitObj_p->ctrlFrmDsc.frmLng,    // ���ݳ���
                         tranCtrlCfg_p  // ������Ʋ����ò���(ָ������ģʽ)
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

    // 3.�Ƿ���·��Ϳ��Ƶ�״̬��ʵʱ����,ֻ�з��ͳɹ�,�Ż���²���
    //����ֵ��λ��������,λֵΪ1ʱ��ʾ��������,0��ʾ�������;
    //���λ:���ö�ʱ��
    //�ε�λ:��������
    //����λ:����
    if ( updateFlag )
    {
        // Э�����Ʊ���״̬�Ͳ����ı�־
        char ctrlFlag = 0;
        //��ʱ��������
        u_char tmCtrlCode;

        ctrlFlag = UpdateSendState (
                   sndSiteRole,    // վ���ɫ
                   realParam_p,    // ͨ��ʵʱ����
                   tranCtrlCfg_p,  // ͨ�����ò���
                   &tmCtrlCode,    // ��ʱ��������
                   ( u_char* ) ctrlUnitObj_p->ctrlFrmDsc.ctrlFrm_p, // ��������
                   ctrlUnitObj_p->ctrlFrmDsc.frmLng     // ���ݳ���
                   );

        // 3.1�ж��Ƿ�Զ�ʱ����������
        if ( ctrlFlag & 0x1 )
        {
            SetCtrlTimer ( chnlObj_p, tmCtrlCode );    // ���ö�ʱ��
        }

        // 3.2�ж��Ƿ�ֹͣ�������ݣ����������п��ƣ��˴���ȻΪ�رղ�����
        if ( ctrlFlag & 0x2 )
        {
            g_chnlQCnxt_p[dev.chnlID] = 0x0;
            // ����ͨ��������״̬
            SetFlowCtrl (
            0x1,         // �������������б�־
            dev.chnlID,  // ͨ�����飩ID��
            &g_chnlQCnxt_p[dev.chnlID]  // ͨ��״ֵ̬
            );
        }
    }

}

/*******************************************************************************
* ���ܣ�����ASDU��������е�����
* ���������ȣ��������ݵ�Ԫ����ȡͨ��ID(chnlNo)��ͨ������ͨ����������Ϳ��Ի��ͨ������
*      ���ţ�����ͨ����������ȡͨ���Ľ�ɫ�������������Լ�����Լ����������������ݷ��͵Ĳ�
*      ������Ϣ�����ţ�������ȡ��ASDU������Ϣ����װAPDU���ݡ�Ȼ�󣬸��ݹ�Լ��鷢�Ͷ˵�
*      ׼������(������״̬��Ǩ��ͼ)�����׼��������������ִ�з��Ͳ����������ͳɹ������·�
*      �Ͷ˵Ŀ��Ʋ�������󣬸��ݿ��Ʋ������ö�Ӧͨ����
*      ������״̬���Լ�ͨ����ʱ���ȡ�
* �ӿڲ�����
*     1.datUnitObj_p  ASDU���е����ݵ�Ԫ����ָ�룻
*     2.dBlkLng       ���ݳ��ȣ�
*******************************************************************************/
void hdlASDUInfoSndReqEvnt ( qASDUInfoRcvUnit* datUnitObj_p, u_char dBlkLng )
{
    /**************************  �����ݵ�Ԫ����ȡ�����Ϣ **************************/
    // ��ȡͨ��(��)����,ͨ��ͨ��������ȡ
    C_ChnlObj* chnlObj_p = chnlMgr.getChnlObj ( datUnitObj_p->chanlID );
    // ��ȡ��Լ����(��Լ�ļ����)
    HMODULE* ptclMdl_p = chnlObj_p->getPtclMdl();
    // ��ȡͨ���������,����sockets��
    HANDLE hChnlSend = chnlObj_p->getSockHdl();
    // ��ȡ������Ʋ�ʵʱ����ָ��
    void* realParam_p = chnlObj_p->getRealParam();
    // ��ȡ������Ʋ����ò���ָ��
    void* tranCtrlCfg_p = chnlObj_p->getTranCtrlCfg();
    // ��ȡ������Ʋ����ò���ָ��
    void* tcOtherCfg_p = NULL;//chnlObj_p->getTranCtrlCfg(); // �ݲ�ʵ��
    // ��ȡվ��Ľ�ɫ
    int sndSiteRole = chnlObj_p->getRole();

    //
    datUnitObj_p->ASDU_Lng = dBlkLng;   // modify by zhaojj,20170420

    /**************************  ���ع�Լ������ **********************************/
    //1. ��ȡЭ���ID
    DLL_getPtclID getPtclID;
    getPtclID = ( DLL_getPtclID ) GetProcAddress ( *ptclMdl_p, "getPtclID" );

    if ( !getPtclID )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function getPtclID()!\n" );
        return;
    }

    //2. ��װAPDU����
    DLL_packAPDU packAPDU;
    packAPDU = ( DLL_packAPDU ) GetProcAddress ( *ptclMdl_p, "packAPDU" );

    if ( !packAPDU )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function packAPDU()!\n" );
        return;
    }

    //3. ��ȡASDU���ݶ����е�����,�����APDU�е��ֽ�������
    DLL_getDU getDU;
    getDU = ( DLL_getDU ) GetProcAddress ( *ptclMdl_p, "getDU" );

    if ( !getDU )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function packAPDU()!\n" );
        return;
    }

    // 4.��鷢��״̬
    DLL_CheckSendState CheckSendState;
    CheckSendState = ( DLL_CheckSendState ) GetProcAddress ( *ptclMdl_p, "CheckSendState" );

    if ( !CheckSendState )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function CheckSendState()!\n" );
        return;
    }

    //5.���·���״̬��ʵʱ����
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
        // ͨ�����ӽ��� ֡
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

    /****************************  ���͹��ܴ���  **********************************/
    //1. ��ȡЭ���ID��Ŀ�����ֳ�TCP�ʹ���    // ע������
    u_char ptclID = getPtclID() % 2;
    // ����������ʽӿڱ���
    DEVICES dev;
    dev.chnlID = datUnitObj_p->chanlID;
    dev.type = ptclID;
    dev.hIntance = hChnlSend;

    //2. ��װAPDU����
    // packAPDU��������ASDU���ݶ���ʵʱ���������ò��������APDU������
    u_char apdu_data_p[256];
    u_char apdu_data_len;
    packAPDU (
    realParam_p,    // ͨ��ʵʱ���������磺���ͽ�����ţ�K��W
    tranCtrlCfg_p,  // ͨ�����ò���������101Э���У���·��ַ�����ò���
    ( u_char* ) datUnitObj_p->ASDU_p, // ASDU���ݶ�����
    datUnitObj_p->ASDU_Lng,         // ASDU���ݳ���
    apdu_data_p,    // APDU����
    apdu_data_len   // APDU�ĳ���
    );
    cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_1, "chnl = %d,packAPDU\r\n", dev.chnlID );
    //3. ���ݹ�Լ��鷢��ǰ��״̬�Ƿ����
    //����ֵ�ĸ�16λ��ʾ����״̬��Ϣ,��16λ��ʾ��Ҫִ�еĶ���
    u_int statusInfo;
    statusInfo = CheckSendState (
                 sndSiteRole,  // վ���ɫ
                 realParam_p,  // ͨ��ʵʱ����
                 apdu_data_p,  // ��������
                 apdu_data_len // ���ݳ���
                 );
    //3.1 ������״̬��Ϣ,������״̬����־����
    u_short errorInfo = statusInfo & 0xFFFF;

    switch ( errorInfo )
    {
        case SEND_TRANSITION_ERROR:  // ״̬Ǩ�ƴ���
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "ERROR: CheckSendState() found TRANSITION Wrong!\n" );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "ChnlID:= %d\n", dev.chnlID );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "cur_index:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.cur_index );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "max_index:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.max_index );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "nex_index:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.nex_index );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "func_id:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.one_node_p[0].func_id );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "trans_id:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.one_node_p[0].trans_id );
            break;

        case SEND_ROLE_ERROR:        // �Ƿ���ɫ
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "ERROR: CheckSendState() found ROLE Wrong!\n" );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "ChnlID:= %d\n", dev.chnlID );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "cur_index:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.cur_index );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "max_index:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.max_index );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "nex_index:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.nex_index );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "func_id:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.one_node_p[0].func_id );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "trans_id:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.one_node_p[0].trans_id );
            break;

        case SEND_FRAME_ERROR:       // ����֡����
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "ERROR: CheckSendState() found FRAME DATA Wrong!\n" );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "ChnlID:= %d\n", dev.chnlID );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "cur_index:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.cur_index );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "max_index:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.max_index );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "nex_index:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.nex_index );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "func_id:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.one_node_p[0].func_id );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "trans_id:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.one_node_p[0].trans_id );
            break;

        case SEND_STATUS_ERROR:      // �Ƿ�״̬
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "ERROR: Sand::CheckSendState() found the status doesn't exist!\n" );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "ChnlID:= %d\n", dev.chnlID );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "cur_index:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.cur_index );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "max_index:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.max_index );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "nex_index:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.nex_index );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "func_id:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.one_node_p[0].func_id );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "trans_id:= %d\n", ( ( realTimeValue* ) realParam_p )->chkData.one_node_p[0].trans_id );
            break;

        case SEND_TIMESCALE_ERROR:   // ��ʱ����
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
    //3.2 ������Ҫִ�еĶ���
    u_short execInfo = ( statusInfo >> 16 ) & 0xFFFF;
    // ����״̬�Ͳ����ĸ��±�־��Ĭ��0��ʾ������
    char updateFlag = 0;

    switch ( execInfo )
    {
        case OPEN_CONNECT:
            //OpenConnect(dev);
            break;

        case CLOSE_CONNECT:
            //CloseConnect(dev);
            //g_chnlQCnxt_p[dev.chnlID] = 0;
            //ASDUDuQ.setCnxtQ(0x1, dev.chnlID, &g_chnlQCnxt_p[dev.chnlID]);  // ����ͨ������,֪ͨ�ϲ���
            updateFlag = 0;
            break;

        case RETRY_CONNECT:
            //RetryConnect(dev);
            break;

        case WRITE_CONNECT: // ��������֡

            updateFlag = WriteToConnect (
                         dev,           // ͨ�����飩������ʵĽӿ���Ϣ
                         ( char* ) apdu_data_p, // ��������
                         apdu_data_len,      // ���ݳ���
                         tcOtherCfg_p/*tranCtrlCfg_p*/  // ������Ʋ����ò���
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

    // 4.���·��Ϳ��Ƶ�״̬��ʵʱ����
    if ( updateFlag )
    {
        // Э�����Ʊ���״̬�Ͳ����ı�־
        char ctrlFlag = 0;
        //��ʱ��������
        u_char tmCtrlCode = 0;

        //5.���·��Ͷ˵�״̬��ʵʱ����,
        //����ֵ��λ��������,λֵΪ1ʱ��ʾ��������,0��ʾ�������;
        //���λ:���ö�ʱ��
        //�ε�λ:��������
        //����λ:����
        ctrlFlag = UpdateSendState (
                   sndSiteRole,   // վ���ɫ
                   realParam_p,   // ͨ��ʵʱ����
                   tranCtrlCfg_p, // ͨ�����ò���
                   &tmCtrlCode,   // ��ʱ��������
                   ( u_char* ) apdu_data_p, // ��������
                   apdu_data_len  // ���ݳ���
                   );

        // 5.1 �ж��Ƿ�Զ�ʱ����������
        if ( ctrlFlag & 0x1 )
        {
            printf ( "transCtrLayer tmCtrlCode = %d", tmCtrlCode );
            SetCtrlTimer ( chnlObj_p, tmCtrlCode ); // ���ö�ʱ��
        }

        // 5.2 �ж��Ƿ�ֹͣ�������ݣ����������п��ƣ��˴���ȻΪ�رղ�����
        if ( ctrlFlag & 0x2 )
        {
            g_chnlQCnxt_p[dev.chnlID] = 0x0;
            // ����ͨ��������״̬
            SetFlowCtrl (
            0x1,         // �������������б�־
            dev.chnlID,  // ͨ�����飩ID��
            &g_chnlQCnxt_p[dev.chnlID]  // ͨ��״ֵ̬
            );
        }
    }
    else
    {
        // ��ȡͨ��(��)����,ͨ��ͨ��������ȡ
        // ��ȡ��Լ����(��Լ�ļ����)
        HMODULE* ptclMdl_p = chnlObj_p->getPtclMdl();
        // ��ȡЭ���ID(����ָ������������ͣ��������ڡ����ڵ�)
        u_char ptclID = getPtclID() % 2;
        // ��ȡͨ���������,����sockets��
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
        chnlMgr.clrChnlState ( dev.chnlID, MC_CHNL_BIT );   // ����ͨ��״̬��ͨ��״̬λ����
        // ����������Ҫ�ϲ�����ϣ���Ҫ�����źż���Ϣ�����ϲ�
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

///////////////////  2.���ղ�Ĺ���ʵ��         //////////////////////////////////
/*******************************************************************************
* ���ܣ�������Ʋ���ղ��̺߳����������������ݣ��������ʹ���APDU����֡��
* ��������ʹ����Windows���¼�ѡ��ģ��(WsaEventSelect)��ÿ���̶߳���Թ����ά��64����
*      ��ͨѶ�ӿڣ���64��ͨ�����ӡ�������ģ���У�ÿ������(socket)�������¼�һһ��Ӧ����
*      һ��������������֡��ʱ��ģ�;ͻ������Ӧ���¼���ͨ��WSAWaitForMultipleEvents()
*      ���Ϳ�������ȡ�����¼���Ӧ�Ľ������ݡ�
*      ��ϵͳ����У���15���������̸߳��������������ӿڵ����ݡ���ˣ�ÿ��վ����������
*      ��960(64*15)������վ��(Socket)��ÿ���߳̾����Լ��ı�ʶ���̱߳�ţ����������̴߳�
*      �������Ĳ������ݸ��߳̽��պ�����
*
* ��������ӿڣ�
*     1.qTransCtrlInfoUnit
*     2.qASDUInfoRcvUnit
*******************************************************************************/
u_long Tsys_NetRevProc ( void* lpParam )
{

    // ����ͨ�����飩��
    u_short chnlID = 0;
    // ȡ�ý����̶߳�Ӧ�ķ���ţ���Ŵ�0��1,...14
    int revThreadNo = threadNum ++;


    // ��ʱ�洢��������recv()��������
    char recvDataBuf[MAX_RECV_SIZE];
    // �������ݴ洢��ָ��,�ṹΪrevBuf,��ָ��ͨ�������н��ն����ݻ���ָ��
    revBuf recvDataFrm;

    // 16���̵߳�˫�����־,ÿλ��ʾһ��ͨ���Ļ���ʹ�����,64λ��ʾ64��ͨ��
    unsigned long long bufFlag = 0;

#ifndef WINDOWS_PLATFORM
    epoll_event events[GROP_CHNL_SIZE];
    sock_descriptor sockfd = -1;
#endif

    // �����̵߳Ľ��մ�����
    while ( true )
    {
#ifdef WINDOWS_PLATFORM
        // �ȴ������¼�
        DWORD nIndex = WSAWaitForMultipleEvents (
                       GROP_CHNL_SIZE,           // �¼���������������ֵΪWSA_MAXIMUM_WAIT_EVENTS��64��
                       g_evtArr_pp[revThreadNo], // �¼���������ָ�룻
                       FALSE,        // �ȴ������ͣ��������һ���¼���Ч�����������ִ�У�
                       WSA_INFINITE,//5000,// // ��ʱʱ�䣬 ���ΪWSA_INFINITE����������ʱ��
                       FALSE );      // I/Oִ�н���״̬���ͣ�FALSE��ʾ���õȴ�I/Oִ����� ��

        // �����¼�������ֵ�������±꣩�������ڶ���¼���������ǰ���һ���������
        nIndex = nIndex - WSA_WAIT_EVENT_0;

        if ( nIndex == WSA_WAIT_FAILED || nIndex == WSA_WAIT_TIMEOUT )
        {
            //Sleep(100);
            // ���Գ�ʱ����һ����˵û�������¼��϶���ʱ
            continue;
        }

        // �Ե�ǰ�������Ժ���¼����ٴε���WSAWaitForMultipleEvents()������
        // �Ա�ȷ�Ϻ����¼���״̬
        int i;

        for ( i = nIndex; i < GROP_CHNL_SIZE; i++ )
        {
            nIndex = WSAWaitForMultipleEvents (
                     1,     // �¼����������;
                     &g_evtArr_pp[revThreadNo][i], // �¼���������ָ��;
                     TRUE,   // �ȴ�������;
                     0,   // ��ʱʱ��;
                     FALSE );

            if ( nIndex == WSA_WAIT_FAILED || nIndex == WSA_WAIT_TIMEOUT )
            {
                //  Sleep(50);
                // ���Գ�ʱ����һ����˵û�������¼��϶���ʱ
                continue;
            }
            else
            {
                // �ҵ������¼��� ����������¼���Ӧ��ͨ����
                chnlID = ( revThreadNo << 4 ) + i;
                // ��ȡͨ��(��)����,ͨ��ͨ��������ȡ
                C_ChnlObj* chnlObj_p = chnlMgr.getChnlObj ( chnlID );
                // ��ȡ��Լ����(��Լ�ļ����)
                HMODULE* ptclMdl_p = chnlObj_p->getPtclMdl();
                //���ع�Լ����
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

                // ��ȡ�׽����Ϸ����������¼�
                // WSAEnumNetworkEvents()�������Զ����������¼�
                WSANETWORKEVENTS NetworkEvents;
                int result = WSAEnumNetworkEvents (
                             g_sockArr_pp[revThreadNo][i],  // ��ǰ���׽���
                             g_evtArr_pp[revThreadNo][i],   // ��ǰ���¼�������
                             &NetworkEvents                // �����¼�
                             );

                // ��ȡ�����¼�����ִ�д������������ܵ�����д��LOG
                if ( result == SOCKET_ERROR )
                {
                    cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                             "Error: Socket error of ReceiveNetData() of ChnlID:%d!\n", chnlID );
                    // �����������
                    hdlRecvError ( chnlID );
                }
                else
                {
                    // ����FD_READ��Ϣ
                    if ( NetworkEvents.lNetworkEvents & FD_READ )
                    {
                        //�����ȡ�¼�
                        if ( NetworkEvents.iErrorCode[FD_READ_BIT] == 0 )
                        {
                            // �����ʱ�洢��
                            //ZeroMemory(recvDataBuf,MAX_RECV_SIZE);
                            memset ( recvDataBuf, 0, MAX_RECV_SIZE );
                            //��ȡ���ݳ���---�ӹ�Լ��ȡ
                            u_char headLng = getHeaderlng();

                            if ( headLng == 0 )
                            {
                                continue;
                            }

                            //��SOCKET���ջ�������ȡ����
                            int nRecvBytes = 0;
                            nRecvBytes = recv (
                                         g_sockArr_pp[revThreadNo][i], // ��ǰ���׽���
                                         ( char* ) recvDataBuf, // �������ݻ���
                                         headLng,            // ���ճ���
                                         MSG_PEEK
                                         );

                            if ( nRecvBytes != headLng )
                            {
                                continue;
                            }

                            // ��ȡ���ݳ���--�ӹ�Լ��ȡ
                            int dataLng = getApduLngFrmBuf ( recvDataBuf, nRecvBytes );

                            if ( dataLng < 0 )
                            {
                                dataLng = 1;
                            }

                            //��SOCKET���ջ����л�ȡ��������
                            nRecvBytes = 0;
                            memset ( recvDataBuf, 0, MAX_RECV_SIZE );
                            nRecvBytes = recv (
                                         g_sockArr_pp[revThreadNo][i], // ��ǰ���׽���
                                         ( char* ) recvDataBuf, // �������ݻ���
                                         dataLng,            // ���ճ���
                                         0
                                         );

                            if ( nRecvBytes != dataLng || dataLng == 1 )
                            {
                                continue;
                            }

                            // ���»������ĳ���
                            recvDataFrm.APDU_Buf.APDU_Lng = nRecvBytes;
                            recvDataFrm.APDU_Buf.APDU_Data_p = ( unsigned char* ) recvDataBuf;

                            //
                            nsCPPub::SleepMilliSecond ( 300 );

                            // �����������
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
                    if ( sockfd == g_sockArr_pp[revThreadNo][j] ) //��sock�������ҵ����¼�������sockfd
                    {
                        chnlID = ( revThreadNo << 4 ) + j;
                        break;
                    }
                }

                C_ChnlObj* chnlObj_p = chnlMgr.getChnlObj ( chnlID );
                // ��ȡ��Լ����(��Լ�ļ����)
                HMODULE* ptclMdl_p = chnlObj_p->getPtclMdl();
                //���ع�Լ����

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


                //�¼�����
                // �����ʱ�洢��
                memset ( recvDataBuf, 0, MAX_RECV_SIZE );
                //��ȡ���ݳ���---�ӹ�Լ��ȡ
                u_char headLng = getHeaderlng();

                if ( headLng == 0 )
                {
                    continue;
                }

                //��SOCKET���ջ�������ȡ����
                int nRecvBytes = 0;
                nRecvBytes = recv (
                             sockfd, // ��ǰ���׽���
                             ( char* ) recvDataBuf, // �������ݻ���
                             headLng,            // ���ճ���
                             MSG_PEEK
                             );

                if ( nRecvBytes != headLng )
                {
                    continue;
                }

                // ��ȡ���ݳ���--�ӹ�Լ��ȡ
                int dataLng = getApduLngFrmBuf ( recvDataBuf, nRecvBytes );

                if ( dataLng < 0 )
                {
                    dataLng = 1;
                }

                //��SOCKET���ջ����л�ȡ��������
                nRecvBytes = 0;
                nRecvBytes = recv (
                             sockfd, // ��ǰ���׽���
                             ( char* ) recvDataBuf, // �������ݻ���
                             dataLng,            // ���ճ���
                             0
                             );
                APCI_104* apci_p = ( APCI_104* ) &recvDataBuf;
                I_FRM* i_frm_p = ( I_FRM* ) apci_p->ctlField;
                cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_1, "SNo = %d\r\n", i_frm_p->SNo );

                if ( nRecvBytes != dataLng || dataLng == 1 )
                {
                    continue;
                }

                // ���»������ĳ���
                recvDataFrm.APDU_Buf.APDU_Lng = nRecvBytes;
                recvDataFrm.APDU_Buf.APDU_Data_p = ( unsigned char* ) recvDataBuf;

                //
                //Sleep(200);

                // �����������
                hdlRecvData ( chnlID, &bufFlag, &recvDataFrm );
            }
        }

#endif//WINDOWS_PLATFORM
    } // ! while

    return 0;
}

/*******************************************************************************
* ���ܣ���ȷ�������ݺ󣬴�����Щ��������
* ���������ȣ�����������Ϣ����ȡͨ��ID(chnlID)��ͨ������ͨ����������Ϳ��Ի��ͨ������
*      ���ţ�����ͨ����������ȡͨ���Ľ�ɫ�������������Լ�����Լ�������������Ϣ��
*      Ȼ�󣬸��ݹ�Լ�����������ݡ�
*
*******************************************************************************/
void hdlRecvData ( u_short chnlID, unsigned long long* bufFlag, revBuf* DataFrm )
{
    /**************************  �����ݵ�Ԫ����ȡ�����Ϣ **************************/
    // ��ȡͨ��(��)����,ͨ��ͨ��������ȡ
    C_ChnlObj* chnlObj_p = chnlMgr.getChnlObj ( chnlID );
    // ��ȡ��Լ����(��Լ�ļ����)
    HMODULE* ptclMdl_p = chnlObj_p->getPtclMdl();
    // ��ȡ������Ʋ�ʵʱ����ָ��
    void* realParam_p = chnlObj_p->getRealParam();
    // ��ȡ������Ʋ����ò���ָ��
    void* tranCtrlCfg_p = chnlObj_p->getTranCtrlCfg();
    // ��ȡվ��Ľ�ɫ
    int revSiteRole = chnlObj_p->getRole();
    // ��ȡͨ�����ջ��������׵�ַ,���ڴ洢104Э���е�I֡
    revBuf* recv_p =  ( revBuf* ) chnlObj_p->getRcvBufPtr();

    int iDataLng = DataFrm->ASDU_Buf.ASDU_Lng;
    // ����APDU������Ϣ��������״̬ʹ��
    revBuf revAPDU;
    memcpy ( &revAPDU, DataFrm, sizeof ( revBuf ) );


    /**************************  ���ع�Լ������ **********************************/

    DLL_CheckRecvState chkRecvState;
    chkRecvState = ( DLL_CheckRecvState ) GetProcAddress ( *ptclMdl_p, "CheckRecvState" );

    if ( !chkRecvState )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function CheckRecvState()!\n" );
        return ;
    }

    //1.����APDU����
    DLL_chkAPDUData chkAPDUData;
    chkAPDUData = ( DLL_chkAPDUData ) GetProcAddress ( *ptclMdl_p, "chkAPDUData" );

    if ( !chkAPDUData )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function chkAPDUData()!\n" );
        return ;
    }

    //2.���½��ն˵�״̬��ʵʱ����
    DLL_UpdateRecvState UpdateRecvState;
    UpdateRecvState = ( DLL_UpdateRecvState ) GetProcAddress ( *ptclMdl_p, "UpdateRecvState" );

    if ( !UpdateRecvState )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function UpdateRecvState()!\n" );
        return;
    }

    //3.����Ӧ��֡����
    DLL_RespCtrlFrame RespCtrlFrame;
    RespCtrlFrame = ( DLL_RespCtrlFrame ) GetProcAddress ( *ptclMdl_p, "RespCtrlFrame" );

    if ( !RespCtrlFrame )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function RespCtrlFrame()!\n" );
        return;
    }

    //4.��Լָ���Ĵ�����
    DLL_HandleRecvStateError HandleRecvStateError;
    HandleRecvStateError = ( DLL_HandleRecvStateError ) GetProcAddress ( *ptclMdl_p, "HandleRecvStateError" );

    if ( !HandleRecvStateError )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function HandleRecvStateError()!\n" );
        return;
    }

    //5. ��ȡЭ���ID
    DLL_getPtclID getPtclID;
    getPtclID = ( DLL_getPtclID ) GetProcAddress ( *ptclMdl_p, "getPtclID" );

    if ( !getPtclID )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function getPtclID()!\n" );
        return;
    }

    /****************************  ���չ��ܴ���  **********************************/


    //2.���ݹ�Լ��鷢��ǰ��״̬�Ƿ����
    //����ֵ�ĸ�16λ��ʾ����״̬��Ϣ,��16λ��ʾ��Ҫִ�еĶ���
    u_int status;
    status = chkRecvState (
             revSiteRole, // վ���ɫ
             realParam_p, // ͨ��ʵʱ����
             ( u_char* ) revAPDU.APDU_Buf.APDU_Data_p, // ��������
             revAPDU.APDU_Buf.APDU_Lng     // ���ݳ���
             );

    //2.1������״̬��Ϣ
    u_short errorInfo = status & 0xFFFF;

    switch ( errorInfo )
    {
        case SEND_TRANSITION_ERROR:  // ״̬Ǩ�ƴ���
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ERROR: CheckRecvState() found STATE TRANSITION Wrong!\n" );
            break;

        case SEND_ROLE_ERROR:        // �Ƿ���ɫ
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ERROR: CheckRecvState() found ROLE Wrong!\n" );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ChnlID:= %d\n", chnlID );
            break;

        case SEND_FRAME_ERROR:       // ����֡����
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ERROR: CheckRecvState() found FRAME DATA Wrong!\n" );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ChnlID:= %d\n", chnlID );
            break;

        case SEND_STATUS_ERROR:      // �Ƿ�״̬
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ERROR: CheckRecvState() found the status doesn't exist!\n" );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ChnlID:= %d\n", chnlID );
            break;

        case SEND_TIMESCALE_ERROR:   // ��ʱ����
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ERROR: CheckRecvState() found the time of data  is out!\n" );
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                     "ChnlID:= %d\n", chnlID );
            break;
    }

    //2.2������Ҫִ�еĶ���
    u_short execInfo = ( status >> 16 ) & 0xFFFF;
    // ����״̬�Ͳ����ĸ��±�־��Ĭ��0��ʾ������
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
            //// ����ͨ������,֪ͨ�ϲ���
            //g_chnlQCnxt_p[dev.chnlID] = 0;
            //ASDUDuQ.setCnxtQ(0x1, dev.chnlID, &g_chnlQCnxt_p[dev.chnlID]);
            break;

        case RETRY_CONNECT:
            updateFlag = 0;
            //RetryConnect(dev);
            break;

        case READ_CONNECT: // ��ȡ����֡
            updateFlag = 1;

            break;

        default:
            updateFlag = 0;
    }

    if ( !updateFlag )
    {
        return ;
    }

    //1.����APDU����
    // ����ֵ�ĸ��ֽ�Ϊ֡����/�������ִ�в��������ֽ�Ϊ����״̬
    // ������״̬ΪMC_APDU_OK,���ֽڱ�ʾ֡����;����,���ֽڱ�ʾ�������ִ�в�����
    // DataFrmֱ�ӷ���ASDU����֡
    u_short statusInfo;
    statusInfo = chkAPDUData (
                 DataFrm,       // ���յ�����֡
                 realParam_p,   // ͨ��ʵʱ����
                 tranCtrlCfg_p  // ��Լ���ò���
                 );
    // ��ȡ��λ�ֽ�,����״̬
    u_char errInfo = statusInfo & 0xFF;
    // ��ȡ��λ�ֽ�,֡����/������
    u_char frmType = ( statusInfo >> 8 ) & 0xFF;
    // Э�����Ʊ���״̬�Ͳ����ı�־
    u_short ctrlFlag = 0;
    //��ʱ��������
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

    // ����APDU�ɹ�������˫�����ַ�����洢��������
    if ( ( errInfo == MC_APDU_OK ) && ( ( frmType == I_F ) || frmType == U_F ) )
    {
        // ����ͨ���ϵ�˫������ջ����ַ���
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

        // ����ͨ���Ϸ������ݳ���
        recv_p[bufIndex].ASDU_Buf.ASDU_Lng =  DataFrm->ASDU_Buf.ASDU_Lng;
        // ��I֡���ݸ��Ƶ�ͨ���Ϸ�,ASDU�������ݹ��ϲ���ʹ��
        char* data_p = ( char* ) recv_p[bufIndex].ASDU_Buf.ASDU_Data_p;
        int iCpyLng = frmType != I_F ? iDataLng : DataFrm->ASDU_Buf.ASDU_Lng;
        memcpy (
        data_p,
        DataFrm->ASDU_Buf.ASDU_Data_p,  // ��������
        iCpyLng                         // ���ݳ���
        );

        // ��������ASDU��Ԫ
        qASDUInfoRcvUnit ASDUUnit;
        ASDUUnit.chanlID = chnlID;
        ASDUUnit.ASDU_Lng = DataFrm->ASDU_Buf.ASDU_Lng;
        ASDUUnit.ASDU_p = ( u_char* ) data_p;
        ASDUUnit.realData_lng = iCpyLng;
        char flag = 0;
		cfgMng->tclLog.LPrintf(DEBUG, DEBUG_LEVEL_0, "hdlRecvData:ASDUInfoQ.pshQ \r\n");
        flag = ASDUInfoQ.pshQ ( FST_GROUP, chnlID, ( char* ) &ASDUUnit, sizeof ( ASDUUnit ) );
		cfgMng->tclLog.LPrintf(DEBUG, DEBUG_LEVEL_0, "hdlRecvData:: ASDUInfoQ.pshQ end chnl=%d \r\n", chnlID);
        if ( flag < 0 ) // ���ʧ��
        {
            cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0, "ASDU push error:%d", flag );
            return;
        }

        //2.ASDU��ӳɹ��󣬸��½��ն˵�״̬��ʵʱ����
        // ����ֵ����Э����ܿ��ƣ�ÿλ�������£�
        // �͵�1λ��0�������ö�ʱ���������ʾ��Ҫ��ܶԶ�ʱ���������ã�������Ҫ����
        // �͵�2λ��1���������������ƣ������ʾ��Ҫ��ܶԷ��Ͳ�������������������Ҫ����
        // �͵�3λ��2�����Ƿ���Ҫ���Ͷ�Э�����ǵĻ�����Ҫ���촫���������
        // ��8-16λ��֡���ͱ�ʶ
        ctrlFlag = UpdateRecvState (
                   revSiteRole,   // վ���ɫ
                   realParam_p,   // ͨ��ʵʱ����
                   tranCtrlCfg_p, // ͨ�����ò���
                   &tmCtrlCode,   // ��ʱ��������
                   ( u_char* ) /*DataFrm*/revAPDU.ASDU_Buf.ASDU_Data_p, // ��������
                   /*DataFrm->*/revAPDU.ASDU_Buf.ASDU_Lng     // ���ݳ���
                   );

        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_1,
                                 "realParam_p:: sendnum=%d, receivenum=%d, wValue=%d, kValue=%d \r\n \r\n",
                                 ( * ( realTimeValue* ) realParam_p ).sendnum,
                                 ( * ( realTimeValue* ) realParam_p ).receivenum,
                                 ( * ( realTimeValue* ) realParam_p ).wValue,
                                 ( * ( realTimeValue* ) realParam_p ).kValue );

        // �ж��Ƿ�Զ�ʱ����������
        if ( ctrlFlag & 0x1 )
        {
            SetCtrlTimer ( chnlObj_p, tmCtrlCode );    // ���ö�ʱ��
        }

        // �ж��Ƿ�ֹͣ���ݣ����������п��ƣ��˴���ȻΪ�رղ�����
        if ( ctrlFlag & 0x2 )
        {
            g_chnlQCnxt_p[chnlID] = 0x1;
            // ����ͨ��������״̬
            SetFlowCtrl (
            0x1,         // �������������б�־
            chnlID,      // ͨ�����飩ID��
            &g_chnlQCnxt_p[chnlID]  // ͨ��״ֵ̬
            );
        }

        // �Ƿ���Ҫ���Ͳ�Э��,��8λ����֡������,��U��I��S��
        if ( ctrlFlag & 0x4 )
        {
            // ��ͨ���ϴ������ָ�봦, ����������Ƶ�Ԫ
            void* ctrl_p = chnlObj_p->getTranCtrl();

            // �������ش�����Ƶ�Ԫ
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
            if ( flag < 0 ) // ���ʧ��
            {
                cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                         "CtrlUnit push error:%d\n", flag );
                return;
            }
        }
    }

    // ����APDU�ɹ�����APDUΪ����֡
    else if ( ( errInfo == MC_APDU_OK ) && ( frmType == S_F/*CTRL_FRAME*/ ) )
    {
        // ���½��ն˵�״̬��ʵʱ����
        // ����ֵ����Э����ܿ��ƣ�ÿλ�������£�
        // �͵�1λ��0�������ö�ʱ���������ʾ��Ҫ��ܶԶ�ʱ���������ã�������Ҫ����
        // �͵�2λ��1���������������ƣ������ʾ��Ҫ��ܶԷ��Ͳ�������������������Ҫ����
        // �͵�3λ��2�����Ƿ���Ҫ���Ͷ�Э�����ǵĻ�����Ҫ���촫���������
        // ��8-16λ��֡���ͱ�ʶ
        ctrlFlag = UpdateRecvState (
                   revSiteRole,   // վ���ɫ
                   realParam_p,   // ͨ��ʵʱ����
                   tranCtrlCfg_p, // ͨ�����ò���
                   &tmCtrlCode,   // ��ʱ��������
                   ( u_char* ) revAPDU.ASDU_Buf.ASDU_Data_p, // ��������
                   revAPDU.ASDU_Buf.ASDU_Lng     // ���ݳ���
                   );

        // �ж��Ƿ�Զ�ʱ����������
        if ( ctrlFlag & 0x1 )
        {
            SetCtrlTimer ( chnlObj_p, tmCtrlCode );    // ���ö�ʱ��
        }

        // �ж��Ƿ�ֹͣ�������ݣ����������п��ƣ��˴���ȻΪ�رղ�����
        if ( ctrlFlag & 0x2 )
        {
            g_chnlQCnxt_p[chnlID] = 0x1;
            // ����ͨ��������״̬
            SetFlowCtrl (
            0x1,         // �������������б�־
            chnlID,      // ͨ�����飩ID��
            &g_chnlQCnxt_p[chnlID]  // ͨ��״ֵ̬
            );
        }

        // ��Ҫ���Ͳ�Э��,notifyFlag =1
        // ctrlflag ��8λ����֡������,��U��I��S��
        if ( ctrlFlag & 0x4 )
        {
            // ��ͨ���ϴ������ָ�봦, ����������Ƶ�Ԫ
            void* ctrl_p = chnlObj_p->getTranCtrl();

            qTransCtrlInfoUnit CtrlUnit;
            CtrlUnit.chanlID = chnlID;
            CtrlUnit.ctrlFrmDsc.ctrlFrm_p = ( char* ) ctrl_p;

            RespCtrlFrame (
            &CtrlUnit.ctrlFrmDsc, //���ݵ�Ԫ�ṹ
            realParam_p,           //ʵʱ����
            ( ctrlFlag >> 8 ) & 0xFF //����֡����
            );
            char flag = 0;
			cfgMng->tclLog.LPrintf(DEBUG, DEBUG_LEVEL_0, "hdlRecvData:ASDUDuQ.pshQ \r\n");		
            flag = ASDUDuQ.pshQ ( SCD_GROUP, chnlID, ( char* ) &CtrlUnit, sizeof ( CtrlUnit ) );
			cfgMng->tclLog.LPrintf(DEBUG, DEBUG_LEVEL_0, "hdlRecvData:: ASDUDuQ.pshQ end chnl=%d \r\n", chnlID);
            if ( flag < 0 ) // ���ʧ��
            {
                cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                         "CtrlUnit push error:%d", flag );
            }
        }
    }


    // ����APDUʧ�ܣ���������ƴ���
    else
    {
        char opt = statusInfo >> 8 & 0xFF;
        // �������ش�����Ƶ�Ԫ
        qTransCtrlInfoUnit CtrlUnit;
        CtrlUnit.chanlID = chnlID;
        char notifyFlag = HandleRecvStateError ( &CtrlUnit.ctrlFrmDsc, realParam_p, errInfo );

        // ��Ҫ���Ͳ�Э��
        if ( notifyFlag & 0x1 )
        {
            // ָ��ͨ���ϵĴ������ָ��
            char* ctrl_p = ( char* ) chnlObj_p->getTranCtrl();
            // �����ݸ��Ƶ�ͨ���Ϸ��Ĵ������֡
            memcpy ( ctrl_p, &CtrlUnit, sizeof ( CtrlUnit ) );

            char flag = 0;
			cfgMng->tclLog.LPrintf(DEBUG, DEBUG_LEVEL_0, "hdlRecvData:ASDUDuQ.pshQ \r\n");
            flag = ASDUDuQ.pshQ ( SCD_GROUP, chnlID, ( char* ) &CtrlUnit, sizeof ( CtrlUnit ) );
			cfgMng->tclLog.LPrintf(DEBUG, DEBUG_LEVEL_0, "hdlRecvData:: ASDUDuQ.pshQ end chnl=%d \r\n", chnlID);
            if ( flag < 0 ) // ���ʧ��
            {
                cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                         "CtrlUnit push error:%d", flag );
            }
        }

        // ����Ҫ���Ͳ�Э��,��notifyFlag !=1
        else if ( CtrlUnit.ctrlFrmDsc.ctrlType != 1 )
        {
            // ����αASDU֡
            qASDUInfoRcvUnit ASDUUnit;
            ASDUUnit.ASDU_Lng = errInfo;
            ASDUUnit.realData_lng = 0;
            ASDUUnit.ASDU_p = 0;
            ASDUUnit.chanlID = chnlID;

            char flag = 0;
            flag = ASDUInfoQ.pshQ ( FST_GROUP, chnlID, ( char* ) &ASDUUnit, sizeof ( ASDUUnit ) );
            printf ( "hdlRecvData:: ASDUInfoQ.pshQ chnl=%d \r\n", chnlID );

            if ( flag < 0 ) // ���ʧ��
            {
                cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                         "ASDU push error:%d\n", flag );
            }
        }

        // ִ��Э��ָ���Ĵ�������,����Э����չ��Ŀǰֻд�˹ر�ͨ������
        switch ( opt )
        {
            case CLOSE_CONNECT:
                // ��ȡͨ��(��)����,ͨ��ͨ��������ȡ
                C_ChnlObj* chnlObj_p = chnlMgr.getChnlObj ( chnlID );
                // ��ȡ��Լ����(��Լ�ļ����)
                HMODULE* ptclMdl_p = chnlObj_p->getPtclMdl();
                // ��ȡЭ���ID(����ָ������������ͣ��������ڡ����ڵ�)
                u_char ptclID = getPtclID() % 2;
                // ��ȡͨ���������,����sockets��
                HANDLE hChnlSend = chnlObj_p->getSockHdl();
                DEVICES dev;
                dev.chnlID = chnlID;
                dev.hIntance = hChnlSend;
                dev.type = ptclID;
                CloseConnect ( dev );
                u_int msgBdy[2] = {0};
                msgBdy[0] = dev.chnlID;
                msgBdy[1] = 0x2;
                chnlMgr.clrChnlState ( dev.chnlID, MC_CHNL_BIT );   // ����ͨ��״̬��ͨ��״̬λ����

                // ����������Ҫ�ϲ�����ϣ���Ҫ�����źż���Ϣ�����ϲ�
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
* ���ܣ�����������մ����¼�
* ���������ȣ�����������Ϣ����ȡͨ��ID(chnlID)��ͨ������ͨ����������Ϳ��Ի��ͨ������
*      ���ţ�����ͨ����������ȡͨ���Ľ�ɫ�������������Լ�����Լ�������������Ϣ��
*      Ȼ�󣬸��ݹ�Լ�����������ݡ�
*
*******************************************************************************/
void hdlRecvError ( u_short chnlID )
{
    // ���������¼���Ӧ���׽��ִ���
    cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_1,
                             "ERROR: Get SOCKET information error!" );

    // ��ȡͨ��(��)����,ͨ��ͨ��������ȡ
    C_ChnlObj* chnlObj_p = chnlMgr.getChnlObj ( chnlID );
    // ��ȡ��Լ����(��Լ�ļ����)
    HMODULE* ptclMdl_p = chnlObj_p->getPtclMdl();
    // ��ȡЭ���ID(����ָ������������ͣ��������ڡ����ڵ�)
    DLL_getPtclID getPtclID;
    getPtclID = ( DLL_getPtclID ) GetProcAddress ( *ptclMdl_p, "getPtclID" );

    if ( !getPtclID )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function getPtclID()!\n" );
        return;
    }

    u_char ptclID = getPtclID() % 2;
    // ��ȡͨ���������,����sockets��
    HANDLE hChnlSend = chnlObj_p->getSockHdl();

    // ����������ʽӿڱ���
    DEVICES dev;
    dev.chnlID = chnlID;
    dev.type = ptclID;
    dev.hIntance = hChnlSend;
    // �ر�ͨ��
    CloseConnect ( dev );
}


///////////////////  3.ͨ���������ܵ�ʵ��       //////////////////////////////////

/*******************************************************************************
* ���ܣ��ر�ͨ������
* �����������豸����dev_type��ֵ��ѡ�����Ӧ�Ĺر�ͨ�����������У�����ر�ͨ�����ӵ�����
*      ���ɾ���Э���׼��������ͨ�����ñ�������ɹرղ�����
* �ӿڲ�����
*     1.dev      �豸�ӿ���Ϣ��
* ����ֵ��0���ر�ͨ�����ӳɹ��� >=1���ر�ͨ������ʧ�ܣ��Ƿ����豸����
*******************************************************************************/
int CloseConnect ( DEVICES dev )
{
    switch ( dev.type )
    {
        case DEVICE_COM:  // ����
            //CloseHandle(dev.hIntance);
            break;

        case DEVICE_NET:  // ����
#ifdef WINDOWS_PLATFORM
            shutdown ( * ( sock_descriptor* ) dev.hIntance, 0x02 );
            closesocket ( * ( sock_descriptor* ) dev.hIntance ); // ���ܹر�TCP�����Ҫ�ȴ��µľ���滻����zhaojj,20170419��
#else
            shutdown ( * ( sock_descriptor* ) dev.hIntance, SHUT_RDWR );
            close ( * ( sock_descriptor* ) dev.hIntance );
#endif

            * ( sock_descriptor* ) dev.hIntance = sock_invalid; // �رպ󣬶˿ں������zhaojj,20170620
            break;

        default:  // �Ƿ����豸����
            return 1;
    }

    return 0;
}

/*******************************************************************************
* ���ܣ�ͨ�����ݵĵײ㷢�Ͳ���
* �����������豸����dev_type��ֵ���ȣ�ѡ�����Ӧ�ĵײ����ݷ��ͷ�����
* �ӿڲ�����
*     1.dev      �豸�ӿ���Ϣ��
*     2.buf      ͨ�����ݣ�
*     3.length   ���ݳ��ȣ�
*     4.cfgParam �豸(ͨ��)�����ò������˴�ֻ��Ҫ�����Ϳ��Ʋ���
* ����ֵ��
*     >0�� ���ͳɹ���0������ʧ��
*******************************************************************************/
int WriteToConnect ( DEVICES dev, char* buf, int length, void* cfgParam )
{
    // ���ݷ��͵Ŀ���ģʽ, ����3�ֹ���ģʽ��
    // SEND_NO_CONTROL���޿���ģʽ��
    // SEND_TIMEOUT_CONTROL: ��ʱ����ģʽ
    // SEND_RETYR_CONTROL: ���Դ�������ģʽ
    char mode = cfgParam != NULL ? * ( ( char* ) cfgParam ) : 0; // cfgParam.mode;

    // ���ݷ��͵Ŀ��Ʋ�����
    char mparam = cfgParam != NULL ? ( * ( char* ) cfgParam ) : 0; // cfgParam.mparam;

    // ���Ϳ���ģʽΪ�������ƣ����Դ�����
    switch ( mode )
    {
        // �޿���ģʽ
        case SEND_NO_CONTROL:
            switch ( dev.type )
            {
                case DEVICE_COM:  // ����API���ͺ���
#ifdef WINDOWS_PLATFORM
                    u_char writeStatus;
                    writeStatus = WriteFile ( dev.hIntance, buf, length, NULL, NULL );

                    if ( !writeStatus )
                    {
                        cfgMng->tclLog.LPrintf ( true, DEBUG_LEVEL_0, "sand failed!" );
                        return 0;
                    }// ��������ʧ��
                    else
                    {
                        return 1;
                    }

#else
                    int writeStatus;
                    writeStatus = write ( * ( int* ) ( dev.hIntance ), buf, length );

                    if ( writeStatus != 0 )
                    {
                        return writeStatus;  // ��������ʧ��
                    }
                    else
                    {
                        return 0;
                    }

#endif

                    break;

                case DEVICE_NET:  // ����API���ͺ���
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
                        return 0;  // ��������ʧ��
                    }
                    else
                    {
                        return 1;
                    }

                    break;

                default:  // �Ƿ����豸����
                    return 0;
            }

            break;

        // ��ʱ����ģʽ
        case SEND_TIMEOUT_CONTROL:
            double start;
#ifdef WINDOWS_PLATFORM
            start = GetTickCount(); // ��ȡϵͳ��ǰ������ʱ��

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
                    case DEVICE_COM:  // ����API���ͺ���
#ifdef WINDOWS_PLATFORM
                        u_char writeStatus;
                        writeStatus = WriteFile ( dev.hIntance, buf, length, NULL, NULL );

                        if ( !writeStatus ) // ���ݷ���ʧ��
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
                            return writeStatus;  // ��������ʧ��
                        }
                        else
                        {
                            return 0;
                        }

#endif
                        break;

                    case DEVICE_NET:  // ����API���ͺ���
                        u_char sendBytes;
                        sendBytes = send ( * ( sock_descriptor* ) ( &dev.hIntance ), buf, length, 0 );

                        // ���ݷ���ʧ��
                        if ( sendBytes != length )
                        {
                            return 1;
                        }
                        else
                        {
                            return 0;
                        }

                        break;

                    default:  // �Ƿ����豸����
                        return 0;
                }

#ifndef WINDOWS_PLATFORM
                gettimeofday ( &now_tv, NULL );
#endif
            }

            return 0; //��ʱ����
            break;

        // ���Դ�������ģʽ
        case SEND_RETYR_CONTROL:
            int i = 0;  // ���Դ�������

            while ( i < mparam )
            {
                switch ( dev.type )
                {
                    // ����API���ͺ���
                    case DEVICE_COM:
#ifdef WINDOWS_PLATFORM
                        u_char writeStatus;
                        writeStatus = WriteFile ( dev.hIntance, buf, length, NULL, NULL );

                        // �����ݷ���ʧ��
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
                            i++;  // ��������ʧ��
                        }
                        else
                        {
                            return 0;
                        }

#endif
                        break;

                    // ����API���ͺ���
                    case DEVICE_NET:
                        u_char sendBytes;
                        sendBytes = send ( * ( sock_descriptor* ) ( &dev.hIntance ), buf, length, 0 );

                        // �����ݷ���ʧ��
                        if ( sendBytes != length )
                        {
                            i++;
                        }
                        else
                        {
                            return 0;
                        }

                        break;

                    default:  // �Ƿ����豸����
                        return 0;
                }
            }

            // ���Դ�������ģʽ��������ʧ��
            return 0;
            break;
    }

    // �Ƿ��ķ���ģʽ
    return 0;
}

/*******************************************************************************
* ���ܣ��ײ�ͨ�����Ͷ���������
* ��������Ҫ���ڴ�����Ʋ㷢�Ͷ˵���������������C_DuGrp_BCPQ���еĳ���������״̬������
*      ͨ��״̬���ݻ����߼����Ӷ�����ȡ���������ݡ�
*      �����ϲ����еĶ�ʱ����������ctlTmr()��ɶ�ʱ��������
* �ӿڲ�����
*     1.grpId   ��Id��=1����1��(��������������)��=2����2�飨ASDU������У���
*     2.qIdx    ����������ͨ�����飩�ţ���=0xFFFF FFFF������ȫ��
*     3.cnxt_p  ������״ָ̬�루=1������ͨ���� =0���ر�ͨ����
* ����ֵ����
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


///////////////////  4.��ʱ�����ܵ�ʵ��       ////////////////////////////////////
/*******************************************************************************
* ���ܣ�����ͨ����ʱ��
* ��������Ҫ���ڴ�����Ʋ�ĳ�ʱ�������ȿ������ڵײ㷢�Ͷˣ�Ҳ�������ڵײ���նˡ����У�
*      ��ʱ���Ŀ����뱣������ͨ�����飩�����У���������UpdateSendState()������
*      UpdateReceiveState()��������ɸ�����ֵ�Ĳ������˴����ҵ���Ҫ���õĶ�ʱ��ID����
*      �����ϲ����еĶ�ʱ������C_TmObj�ķ���ctlTmr()����ɶ�ʱ��������
* �ӿڲ�����
*     1.chnlObj_p  ͨ������ָ�룻
*     2.tmCtrlCode ��ʱ����Ĳ����룻ÿ2λ��ʾһ�ֶ�ʱ�����京���ʾ���£�
*        0x0: �޲�����0x1:������ʱ����0x2��ֹͣ�����¸�λ��ʱ��������ֵ��
* ����ֵ����
*******************************************************************************/
void SetCtrlTimer ( C_ChnlObj* chnlObj_p, u_char tmCtrlCode )
{
    // ��ȡ������Ʋ㶨ʱ����Դͷָ��
    tmChnl* tmChnl_p = NULL;
    chnlObj_p->getTimer ( tmChnl_p );

    // ��ʱ�����
    u_char stID = 0;

    // ���ұ������Ķ�ʱ����������Ӧ�Ķ�ʱ�����������
    while ( tmCtrlCode != TIMER_NOP )
    {
        if ( ( tmCtrlCode & 0x3 ) !=  TIMER_NOP )
        {
            // ����ͨ�������룬���ϲ���ʹ��
            chnlObj_p->setCtrlCode ( stID, tmCtrlCode & 0x3 );
            tmFclty.ctlTmr ( tmChnl_p, stID, ( tmCtrlCode & 0x3 ) ); // 20170603, by zhaojj
        }

        stID++;
        tmCtrlCode = tmCtrlCode >> 2;
    }
}

/////////////////////// ��ʱ����ʱ���ƺ��� /////////////////////
// ������ݹ�Լ���˲��ֺ����п�����Ҫ�ع���
/*******************************************************************************
* ���ܣ�ͨ�ö�ʱ���ص�����
* ����������IEC104��Լ, �����t0ʱ����û�гɹ��������ӡ�
* �ӿڲ�����
*     1.chnlID   ͨ�����飩����Ϣ��
* ����ֵ��0�����ͳɹ��� >=1������ʧ��
*     1�� �Ƿ��Ŀ���ģʽ��             2�� �Ƿ����豸����,��������Ĵ�����ʵĽӿ����ͣ�
*******************************************************************************/
void TimerService ( void* param )
{
    int iRet = 0;


    // ���ͨ���Ĵ������
    PDEVICES dev = ( PDEVICES ) param;

    // ��ȡͨ��(��)����,ͨ��ͨ��������ȡ
    C_ChnlObj* chnlObj_p = chnlMgr.getChnlObj ( dev->chnlID );

    if ( chnlObj_p == NULL )
    {
        return ;
    }

    HMODULE* hdl = chnlObj_p->getPtclMdl();

    // ��ȡ��Լ��ӿ�ָ�룬�õ���Ӧ������
    DLL_timerRingAdd tTimeOut = ( DLL_timerRingAdd ) GetProcAddress ( *hdl, "timerRingAdd" );

    if ( tTimeOut == NULL )
    {
        return ;
    }

    int retCode = tTimeOut ( dev->type );

    DEVICES tmpDev ;
    memcpy ( &tmpDev, dev, sizeof ( DEVICES ) );

    // �͵�1λ��λ0������1Ϊ�رն�ʱ��; �͵�2λ��λ1������1Ϊ�ر�����
    // �͵�3λ��λ2������1Ϊ����S֡; �͵�4λ��λ3������1Ϊ���Ͳ��Լ���
    if ( retCode & 0x4 ) // ����S֡
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

                // ��ȡͨ���������,����sockets��
                tmpDev.hIntance = chnlObj_p->getSockHdl();
                // �ر�����
                CloseConnect ( tmpDev );
                //
                //*(sock_descriptor*)tmpDev.hIntance = 0;

                cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_1,
                                         "[WARNING]TimerService:: close socket of chnl=%d \r\n", dev->chnlID );
                chnlMgr.clrChnlState ( dev->chnlID, MC_CHNL_BIT );  // ����ͨ��״̬��ͨ��״̬λ����
            }

        }

        // FORMAT: CHNL_ID, CODE
        u_int msgBdy[2] = {0};
        msgBdy[0] = dev->chnlID;
        msgBdy[1] = retCode;

        // ����������Ҫ�ϲ�����ϣ���Ҫ�����źż���Ϣ�����ϲ�
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
* ���ܣ���ʱ��T0
* ����������IEC104��Լ,�����t0ʱ����û�гɹ��������ӡ�
* �ӿڲ�����
*     1.chnlID   ͨ�����飩����Ϣ��
* ����ֵ��0�����ͳɹ��� >=1������ʧ��
*     1�� �Ƿ��Ŀ���ģʽ��             2�� �Ƿ����豸����,��������Ĵ�����ʵĽӿ����ͣ�
*******************************************************************************/
void Timer0Service ( void )
{

}

/*******************************************************************************
* ���ܣ���ʱ��T1
* ����������IEC104��Լ,�������һ��I��U��ʽ���ĺ󣬱�����t1ʱ���ڣ�Ĭ��ֵΪ15�룩�õ��Է�
*      ���Ͽɣ������������ӡ�
* �ӿڲ�����
*     1.param    ��ʱ�����������Ϣ��
* ����ֵ��
*     ��;
*******************************************************************************/
void Timer1Service ( void* param )
{
    /*tTmrCfg.name_p = tTmrNm;
    tTmrCfg.tmFunHdl = ssnTmOutFun_p;
    tTmrCfg.tmFunHdlPara_p = (void *)
    tTmrCfg.type = 0;
    tTmrCfg.fstXpirTm = tSsnReqBlk.timeout;

    if(!tmFclty.addTmr(tmChnlID), &tTmrCfg)
    ��Ӷ�ʱ��ʱ����ֵ����Ӧ�ֶ�*/

    //PDEVICES dev = (DEVICES*)param;

    //��������
    //RetryConnect(*dev);

}

/*******************************************************************************
* ���ܣ���ʱ��T2
* ����������IEC104��Լ,����һ��I��ʽ�����Ժ������t2ʱ���ڣ�Ĭ��ֵΪ10�룩û�����յ��µ�
*      I��ʽ���ģ��ͱ��뷢��S��ʽ֡��I��ʽ�����Ͽɡ�
* �ӿڲ�����
*     1.param    ��ʱ�����������Ϣ��
* ����ֵ��
*     ��;
*******************************************************************************/
void Timer2Service ( void* param )
{
    // ���ͨ���Ĵ������
    PDEVICES dev = ( DEVICES* ) param;
    // ��ȡͨ��(��)����,ͨ��ͨ��������ȡ
    C_ChnlObj* chnlObj_p = chnlMgr.getChnlObj ( dev->chnlID );
    // ��ȡ������Ʋ�ʵʱ����ָ��
    void* realParam_p = chnlObj_p->getRealParam();

    // ��ȡ��Լ����(��Լ�ļ����)
    HMODULE* ptclMdl_p = chnlObj_p->getPtclMdl();

    // ��ȡЭ���ID(����ָ������������ͣ��������ڡ����ڵ�)
    DLL_CtrlFrame CtrlFrame;
    CtrlFrame = ( DLL_CtrlFrame ) GetProcAddress ( *ptclMdl_p, "CtrlFrame" );

    if ( !CtrlFrame )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function CtrlFrame()!\n" );
        return;
    }

    // ������Ƶ�Ԫ
    char sendBuf[MC_DEF_MAX_LEN] = {0};
    qTransCtrlInfoUnit CtrlUnit;
    CtrlUnit.ctrlFrmDsc.ctrlFrm_p = sendBuf;
    // ����ȷ��֡-S֡
    CtrlFrame (
    &CtrlUnit.ctrlFrmDsc,
    realParam_p,
    S_F
    );

    if ( CtrlUnit.ctrlFrmDsc.frmLng <= 0 )
    {
        // ����Ҫ����s֡
        return ;
    }

    // ����Ӧ��֡
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
* ���ܣ���ʱ��T3
* ����������IEC104��Լ,ÿ����һ��I��ʽ��S��ʽ��U��ʽ�ı������¼����������t3ʱ���ڣ�Ĭ��ֵ
*      Ϊ20�룩û�����յ��κα��ģ���Ҫ��Է�������·����֡TestFR��
* �ӿڲ�����
*     1.dev      �豸�ӿ���Ϣ��
* ����ֵ��0�����ͳɹ��� >=1������ʧ��
*     1�� �Ƿ��Ŀ���ģʽ��             2�� �Ƿ����豸����,��������Ĵ�����ʵĽӿ����ͣ�
*******************************************************************************/
void Timer3Service ( void* param )
{
    // ����ӣ��˴��ж��Ƿ����ϲ��ṩ�ĳ�ʱ������������У������ϲ��ṩ�Ĵ�������
    // ...
    // û�У��������洦������


    // ���ͨ���Ĵ������
    PDEVICES dev = ( DEVICES* ) param;
    // ��ȡͨ��(��)����,ͨ��ͨ��������ȡ
    C_ChnlObj* chnlObj_p = chnlMgr.getChnlObj ( dev->chnlID );
    // ��ȡ������Ʋ�ʵʱ����ָ��
    void* realParam_p = chnlObj_p->getRealParam();

    // ��ȡ��Լ����(��Լ�ļ����)
    HMODULE* ptclMdl_p = chnlObj_p->getPtclMdl();

    // ��ȡЭ���ID(����ָ������������ͣ��������ڡ����ڵ�)
    DLL_CtrlFrame CtrlFrame;
    CtrlFrame = ( DLL_CtrlFrame ) GetProcAddress ( *ptclMdl_p, "CtrlFrame" );

    if ( !CtrlFrame )
    {
        cfgMng->tclLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                                 "Error: Can't not load function CtrlFrame()!\n" );
        return;
    }

    // ����TESTFR����֡
    // ������Ƶ�Ԫ
    static qTransCtrlInfoUnit CtrlUnit;
    // ����ȷ��֡-U֡
    CtrlFrame (
    &CtrlUnit.ctrlFrmDsc,
    realParam_p,
    IEC104_TESTFR_ACT
    );

    // ���Ͳ���֡
    //WriteToConnect(DEVICES dev, char * buf, int length, void *cfgParam);
}


