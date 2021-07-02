
#include "DataBase.h"
#include "sysMgr.h"
#include "cfgManage.h"
extern C_ChnlMgr chnlMgr;

#ifdef  WINDOWS_PLATFORM
    #define crtQLock(qKey)      (InitializeCriticalSection(&qKey))
    #define freeQLock(qKey)     (DeleteCriticalSection(&qKey))
    #define lockQ(qKey)         (EnterCriticalSection(&qKey))
    #define unlockQ(qKey)       (LeaveCriticalSection(&qKey))

#else
    #define crtQLock(qKey)      (pthread_mutex_init(&qKey,NULL))
    #define freeQLock(qKey)     (pthread_mutex_destroy(&qKey))
    #define lockQ(qKey)         (pthread_mutex_lock(&qKey))
    #define unlockQ(qKey)       (pthread_mutex_unlock(&qKey))
#endif

// �Ự����
C_SsnCtnrObj::C_SsnCtnrObj()
{
    m_uiPrio = 0;
    memset ( m_chPrioRdyBlk, 0, MC_DEF_MAX_PRIO );
    memset ( m_ssnRdyTab, 0, MC_DEF_MAX_PRIO * 128 );
}

C_SsnCtnrObj::~C_SsnCtnrObj()
{
    m_uiPrio = MC_DEF_MAX_PRIO;
}

// ������ɻỰ����
int C_SsnCtnrObj::aplyAdtSsnInst ( u_int uiPrio, ssnCtlBlk* ssnCtrlBlk_p )
{
    if ( uiPrio > MC_DEF_MAX_PRIO || ssnCtrlBlk_p == NULL
         || ( ssnCtrlBlk_p->rtRunStOrRslt != MC_SNNST_RDY && ssnCtrlBlk_p->rtRunStOrRslt != MC_SNNST_OK ) )
    {
        return -10;
    }

    m_lock.Lock();
    // add to tab
    u_short chnlNo = ssnCtrlBlk_p->chnlID;
    u_short usRow = chnlNo / 8;
    short usBit = chnlNo % 8/* - 1*/;
    usBit = usBit < 0 ? 0 : usBit;

    ++ ( m_chPrioRdyBlk[uiPrio - 1] );
    m_ssnRdyTab[uiPrio - 1][usRow] |= ( 1 << usBit );

    //
    //  ssnCtrlBlk_p->rtRunStOrRslt = MC_SNNST_RDY;

    m_lock.UnLock();

    return MC_SNNST_OK;
}
// ���պ��ų��Ự����
int C_SsnCtnrObj::delSsnObj ( u_int uiPrio, ssnCtlBlk* ssnCtrlBlk_p )
{
    if ( uiPrio > MC_DEF_MAX_PRIO || ssnCtrlBlk_p == NULL )
    {
        return -10;
    }

    if ( ssnCtrlBlk_p->ssnNo == DEF_ERROR_SSN_NO )
    {
        delete ssnCtrlBlk_p;
        m_errSsnBlk_p = NULL;
        m_errFlag = 0;
        return 0;
    }

    m_lock.Lock();
    // del to tab
    u_short chnlNo = ssnCtrlBlk_p->chnlID;
    u_short usRow = chnlNo / 8;
    short usBit = chnlNo % 8/* - 1*/;
    usBit = usBit <= 0 ? 0 : usBit;

    //  C_ChnlObj* chnlObj_p = chnlMgr.getChnlObj(chnlNo);
    //  ssnTbl* ssnTbl_p = chnlObj_p->getSsnTbl();
    //  if ( ssnTbl_p != NULL )
    //  {
    //      // ����������ʵ���Ự���ƿ�
    //      ssnCtlBlk* tmpSsn_p = chnlObj_p->psvSsnCtlBlk(uiPrio);
    //      if ( !(tmpSsn_p != NULL && tmpSsn_p->rtRunStOrRslt == MC_SNNST_RDY) )
    //      {// no psv's ssn_ctrl_blk
    //          // ����������ʵ���Ự���ƿ顢����/������ʵ��
    //          tmpSsn_p = chnlObj_p->actSsnCtlBlk(uiPrio);
    //          if ( tmpSsn_p != NULL && (tmpSsn_p->rtRunStOrRslt == MC_SNNST_RDY || tmpSsn_p->rtRunStOrRslt == MC_SNNST_OK))
    //          {
    //              m_lock.UnLock();
    //              return MC_SNNST_OK;
    //          }
    //          // no ack's ssn_ctrl_blk
    //          if ( m_chPrioRdyBlk[uiPrio-1] )
    //          {
    //              --(m_chPrioRdyBlk[uiPrio-1]);
    //          }
    //          m_ssnRdyTab[uiPrio-1][usRow] &= ~(1<<usBit);
    //      }
    //  }
    //
    if ( m_chPrioRdyBlk[uiPrio - 1] )
    {
        -- ( m_chPrioRdyBlk[uiPrio - 1] );
    }

    if ( m_chPrioRdyBlk[uiPrio - 1] == 0 )
    {
        m_ssnRdyTab[uiPrio - 1][usRow] &= ~ ( 1 << usBit );
        // 20170508,zhaojj
        m_uiPrio = m_uiPrio > 0 ? 0 : m_uiPrio;
    }

    m_lock.UnLock();

    return MC_SNNST_OK;
}
// ���ȻỰ����
ssnCtlBlk* C_SsnCtnrObj::schdSsnObj ( /*C_SsnObj* tCurSsnObj_p*/ )
{
    if ( m_uiPrio >= MC_DEF_MAX_PRIO )
    {
		printf("m_uiPrio = %d\r\n", m_uiPrio);
        return NULL;
    }

    //
    if ( m_errFlag )
    {
        // error
        m_errFlag = 0;
        return m_errSsnBlk_p;
    }

    m_lock.Lock();

    //
    for ( int i = MC_DEF_MAX_PRIO; i > 0; i-- )
    {
        if ( ! ( i > ( int ) m_uiPrio && m_chPrioRdyBlk[i - 1] > 0 ) )
        {
            // ����Ϊ 0�����ȼ�С�ڵ�ǰִ��Ȩ��
            continue;
        }

        u_short chnlNo = 0;

        for ( int j = 0; j < 128; j++ )
        {
            u_char ucCode = m_ssnRdyTab[i - 1][j];

            if ( ucCode == 0 )
            {
                continue;
            }

            u_char ucBit = 0;

            for ( ; ucBit < 8; ucBit++ )
            {
                if ( ( ( ucCode >> ucBit ) & 1 ) != 1 )
                {
                    continue;;
                }

                //
                // get chnlno
                chnlNo = ( j << 3 ) + ucBit;
                //
                C_ChnlObj* chnlObj_p = chnlMgr.getChnlObj ( chnlNo );
                ssnTbl* ssnTbl_p = chnlObj_p->getSsnTbl();

                if ( ssnTbl_p != NULL )
                {
                    // ����������ʵ���Ự���ƿ�
                    ssnCtlBlk* tmpSsn_p = chnlObj_p->psvSsnCtlBlk ( i );
					if (tmpSsn_p != NULL)
					{
						printf("schdSsnObj::tmpSsn_p->rtRunStOrRslt = %d\r\n", tmpSsn_p->rtRunStOrRslt);
					}
                    if ( tmpSsn_p != NULL && tmpSsn_p->rtRunStOrRslt == MC_SNNST_RDY )
                    {
                        m_uiPrio = i - 1;
                        m_lock.UnLock();
                        return tmpSsn_p;
                    }

                    // ����������ʵ���Ự���ƿ顢����/������ʵ��
                    tmpSsn_p = chnlObj_p->actSsnCtlBlk ( i );

                    if ( tmpSsn_p != NULL && ( tmpSsn_p->rtRunStOrRslt == MC_SNNST_RDY || tmpSsn_p->rtRunStOrRslt == MC_SNNST_OK ) )
                    {
                        m_uiPrio = i - 1;
                        m_lock.UnLock();
                        return tmpSsn_p;
                    }
                }
            } // ! for
        } // ! for
    } // ! for

    m_lock.UnLock();
	printf("No tmpSsn_p\r\n");
    return NULL;
}
// �ɷ��Ự����
ssnCtlBlk* C_SsnCtnrObj::dispbSsnObj ( ssnTbl* ssnTbl_p, T_ptclFunDsc maybeSsnSet[], u_char funNum, int* iIdnex )
{
    if ( ssnTbl_p == NULL || funNum <= 0 )
    {
        return NULL;
    }

    u_short hitSsnNo = ( short ) DEF_ERROR_SSN_NO;

    // 1.find ssnID in maybeSsnSet
    for ( int i = 0; i < funNum; i++ )
    {
        u_short ssnID = maybeSsnSet[i].ssnDscNo;
        u_short usActPrio = maybeSsnSet[i].act_prio_chnlID;

        if ( ssnID != DEF_ERROR_SSN_NO )
        {
            // right
            // 2.ssnID is right, check info, find the ssn_ctrl_blk
            char cActPrio[2];
            memcpy ( cActPrio, ( void* ) &usActPrio, sizeof ( u_short ) );

            ssnItem* ssnItem_p = & ( ssnTbl_p->ssnTblEntry[cActPrio[1] - 1] );

            //
            if ( ssnItem_p )
            {
                char cActFlag = cActPrio[0];
                ssnCtlBlk* ssnBlk_p = NULL;

                if ( cActFlag == MC_ACT_FLAG && ssnItem_p->ssnCtlBlk_p && ssnItem_p->ssnCtlBlk_p->ssnNo == ssnID )
                {
                    ssnBlk_p = ssnItem_p->ssnCtlBlk_p;
                }
                else if ( cActFlag == MC_PSV_FLAG &&  ssnItem_p->psvSsn_p && ssnItem_p->psvSsn_p->ssnNo == ssnID )
                {
                    ssnBlk_p = ssnItem_p->psvSsn_p;
                }
                else
                {
                    hitSsnNo = ( short ) DEF_ERROR_SSN_NO;
                }

                //
                if ( ssnBlk_p )
                {
                    //if ( ssnBlk_p->ssnNo == ssnID )
                    {
                        hitSsnNo = ssnID;
                        *iIdnex = i;
                        return ssnBlk_p;
                    }
                }
                else
                {
                    hitSsnNo = ( short ) DEF_ERROR_SSN_NO;
                }

            } // ! if
            else
            {
                hitSsnNo = ( short ) DEF_ERROR_SSN_NO;
            }

        } // ! if

    } // ! for

    //
    if ( ( funNum == 1 && maybeSsnSet[0].ssnDscNo == DEF_ERROR_SSN_NO )
         || hitSsnNo == DEF_ERROR_SSN_NO )
    {
        // error and miss ssnID
        // 3.if funNum == 1 && ssnID=0xffff, create err_example,and set err_flag, then iIndex is -1, return NULL
        m_errSsnBlk_p = new ssnCtlBlk;
        memset ( m_errSsnBlk_p, 0, sizeof ( ssnCtlBlk ) );
        //��ʼ��������@add@20180710
        //m_errSsnBlk_p->ssnCtlLock.Init();
        m_errSsnBlk_p->ssnNo = DEF_ERROR_SSN_NO;
        m_errSsnBlk_p->chnlID = maybeSsnSet[funNum].act_prio_chnlID;    // ���һλ������ͨ����
        m_errFlag = 1;
        //
        *iIdnex = -1;
    }

    return NULL;
}

// ���á��޸�ִ��Ȩ��
int C_SsnCtnrObj::setPriority ( u_int uiPrio )
{
    m_uiPrio = uiPrio;

    return MC_SNNST_OK;
}
// ��ȡִ��Ȩ��
int C_SsnCtnrObj::GetPriority()
{
    return m_uiPrio;
}
//

// ͨ�����飩
C_ChnlObj::C_ChnlObj ( u_char& ucState, int iBufLen, int ptclMdlNum )
{
    memset ( &m_chnlDsc, 0, sizeof ( chnlDsc ) );
    ucState = 0;
    // ��ʼ��
    m_chnlDsc.ssnTbl_p = new ssnTbl;

    if ( !m_chnlDsc.ssnTbl_p )
    {
        exit ( -10 );
    }

    memset ( m_chnlDsc.ssnTbl_p, 0, sizeof ( ssnItem ) *MC_DEF_MAX_PRIO );

    //
    switch ( ptclMdlNum )
    {
        case 104:
#ifdef WINDOWS_PLATFORM
            m_chnlDsc.ptclMdl = LoadLibrary ( "APDU.dll" );

            if ( m_chnlDsc.ptclMdl == NULL )
            {
                assert ( "LoadLibrary APDU.dll failed! \r\n" );
                return ;
            }

            break;
#else
            m_chnlDsc.ptclMdl = dlopen ( LIB_NAME_104, RTLD_LAZY );

            if ( m_chnlDsc.ptclMdl == NULL )
            {
                assert ( "LoadLibrary libAPDU.so failed! \r\n" );
                return ;
            }

            dlerror();
            break;
#endif

        case 106:

#ifdef WINDOWS_PLATFORM
            m_chnlDsc.ptclMdl = LoadLibrary ( "MODBUS.dll" );

            if ( m_chnlDsc.ptclMdl == NULL )
            {
                assert ( "LoadLibrary APDU.dll failed! \r\n" );
                return ;
            }

            break;
#else
            m_chnlDsc.ptclMdl = dlopen ( LIB_NAME_106, RTLD_LAZY );

            if ( m_chnlDsc.ptclMdl == NULL )
            {
                assert ( "LoadLibrary libAPDU.so failed! \r\n" );
                return ;
            }

            dlerror();
#endif

        default:
            break;
    }

    // ��ʼ�����պͷ������ݻ�����
    for ( int i = 0; iBufLen > 0 && i < MC_DEF_BUFF_NUM; i++ )
    {
        revBuf* revBuf_p = & ( m_chnlDsc.revBuf_p[i] );
        revBuf_p->ASDU_Buf.ASDU_Data_p = new unsigned char[iBufLen]();
        revBuf_p->ASDU_Buf.ASDU_Lng = iBufLen;

        revBuf* sndBuf_p = & ( m_chnlDsc.sendBuf_p[i] );
        sndBuf_p->ASDU_Buf.ASDU_Data_p = new unsigned char[iBufLen]();
        sndBuf_p->ASDU_Buf.ASDU_Lng = iBufLen;
    }

    // �Ƿ���Ĭ��ֵ ����
    m_chnlDsc.locSsnInfo_p = new ssnSets;
    m_chnlDsc.locSsnInfo_p->setNum = 0;
    m_chnlDsc.locSsnInfo_p->stepDsc_p = NULL;
    // ...
}

C_ChnlObj::C_ChnlObj ( int iBufLen, int ptclMdlNum )
{
    memset ( &m_chnlDsc, 0, sizeof ( chnlDsc ) );

    switch ( ptclMdlNum )
    {
        case 104:
#ifdef WINDOWS_PLATFORM
            m_chnlDsc.ptclMdl = LoadLibrary ( "APDU.dll" );

            if ( m_chnlDsc.ptclMdl == NULL )
            {
                assert ( "LoadLibrary APDU.dll failed! \r\n" );
                return ;
            }

            break;
#else
            m_chnlDsc.ptclMdl = dlopen ( LIB_NAME_104, RTLD_LAZY );

            if ( m_chnlDsc.ptclMdl == NULL )
            {
                assert ( "LoadLibrary libAPDU.so failed! \r\n" );
                return ;
            }

            dlerror();
            break;
#endif

        case 106:

#ifdef WINDOWS_PLATFORM
            m_chnlDsc.ptclMdl = LoadLibrary ( "MODBUS.dll" );

            if ( m_chnlDsc.ptclMdl == NULL )
            {
                assert ( "LoadLibrary APDU.dll failed! \r\n" );
                return ;
            }

            break;
#else
            m_chnlDsc.ptclMdl = dlopen ( LIB_NAME_106, RTLD_LAZY );

            if ( m_chnlDsc.ptclMdl == NULL )
            {
                assert ( "LoadLibrary libAPDU.so failed! \r\n" );
                return ;
            }

            dlerror();
#endif

        default:
            break;
    }

    // ��ʼ�����պͷ������ݻ�����
    for ( int i = 0; ( iBufLen > 0 ) && ( i < MC_DEF_BUFF_NUM ); i++ )
    {
        revBuf* revBuf_p = & ( m_chnlDsc.revBuf_p[i] );
        revBuf_p->ASDU_Buf.ASDU_Data_p = new unsigned char[iBufLen];
        revBuf_p->ASDU_Buf.ASDU_Lng = iBufLen;

        revBuf* sndBuf_p = & ( m_chnlDsc.sendBuf_p[i] );
        sndBuf_p->ASDU_Buf.ASDU_Data_p = new unsigned char[iBufLen];
        sndBuf_p->ASDU_Buf.ASDU_Lng = iBufLen;
    }
}

C_ChnlObj::~C_ChnlObj()
{
    memset ( &m_chnlDsc, 0, sizeof ( chnlDsc ) );

    if ( m_chnlDsc.ptclMdl != NULL )
    {
#ifdef WINDOWS_PLATFORM
        FreeLibrary ( m_chnlDsc.ptclMdl );
#else
        dlclose ( m_chnlDsc.ptclMdl );
#endif
        m_chnlDsc.ptclMdl = NULL;
    }

    if ( m_chnlDsc.commMdl != NULL )
    {
#ifdef WINDOWS_PLATFORM
        FreeLibrary ( m_chnlDsc.commMdl );
#else
        dlclose ( m_chnlDsc.commMdl );
#endif
        m_chnlDsc.commMdl = NULL;
    }

    for ( int i = 0; i < MC_DEF_BUFF_NUM; i++ )
    {
        revBuf* revBuf_p = & ( m_chnlDsc.revBuf_p[i] );

        if ( revBuf_p->ASDU_Buf.ASDU_Data_p )
        {
            delete []revBuf_p->ASDU_Buf.ASDU_Data_p;
        }

        revBuf_p->ASDU_Buf.ASDU_Data_p = NULL;
        revBuf_p->ASDU_Buf.ASDU_Lng = 0;

        revBuf* sndBuf_p = & ( m_chnlDsc.sendBuf_p[i] );

        if ( sndBuf_p->ASDU_Buf.ASDU_Data_p )
        {
            delete []sndBuf_p->ASDU_Buf.ASDU_Data_p;
        }

        sndBuf_p->ASDU_Buf.ASDU_Data_p = NULL;
        sndBuf_p->ASDU_Buf.ASDU_Lng = 0;
    }
}
// ��ȡ������ͨ����
int C_ChnlObj::getChnlNo()
{
    return m_chnlDsc.sChnlNo;
}

int C_ChnlObj::setChnlNo ( short chnlNo )
{
    m_chnlDsc.sChnlNo = chnlNo;
    return 0;
}
// ��ȡ������ͨ����
int C_ChnlObj::getChnlName ( char* chnlName_p, int iLen )
{
    if ( ( size_t ) iLen < strlen ( m_chnlDsc.chnlName_p ) )
    {
        return -10;
    }

    memcpy ( chnlName_p, m_chnlDsc.chnlName_p, strlen ( m_chnlDsc.chnlName_p ) );
    return 0;
}

int C_ChnlObj::setChnlName ( char* chnlName_p, int iLen )
{
    // ����У�飿��
    memcpy ( m_chnlDsc.chnlName_p, chnlName_p, iLen );
    return 0;
}
// // ��ȡ������k��w����ֵ
// int C_ChnlObj::getKandW( u_short &usK, u_short &usW )
// {
//
// }
//
// int C_ChnlObj::setKandW( u_short usK, u_short usW )
// {
//
// }

// ��ȡ�����ô�����Ʋ㳬ʱʱ�䡢��ʱ��Դʱ��ͨ��ָ��Ϳ�����
int C_ChnlObj::getOutTime ( char tmCode )
{
    if ( tmCode > MC_DEF_SOFT_TIME_NUM )
    {
        return -10;
    }

    return m_chnlDsc.usOutTmT0a[tmCode];
}

int C_ChnlObj::setOutTime ( char tmCode, short newValue )
{
    if ( tmCode > MC_DEF_SOFT_TIME_NUM )
    {
        return -10;
    }

    m_chnlDsc.usOutTmT0a[tmCode] = newValue;
    return 0;
}

int C_ChnlObj::getTimer ( tmChnl*& tmChnl_p )
{
    tmChnl_p = m_chnlDsc.tmChnl_p;
    return 0;
}

int C_ChnlObj::setTimer ( tmChnl* tmChnl_p )
{
    m_chnlDsc.tmChnl_p = tmChnl_p;
    return 0;
}


int C_ChnlObj::getCtrlCode ( char tmCode, char& ctrlCode )
{
    if ( tmCode > MC_DEF_SOFT_TIME_NUM )
    {
        return -10;
    }

    ctrlCode = m_chnlDsc.tmCtrlCode[tmCode];
    return 0;
}

int C_ChnlObj::setCtrlCode ( char tmCode, char ctrlCode )
{
    if ( tmCode > MC_DEF_SOFT_TIME_NUM )
    {
        return -10;
    }

    m_chnlDsc.tmCtrlCode[tmCode] = ctrlCode;
    return 0;
}

// ��ȡ�����ý�ɫ
int C_ChnlObj::getRole()
{
    return m_chnlDsc.cRole;
}

int C_ChnlObj::setRole ( short& role )
{
    m_chnlDsc.cRole = role;
    return 0;
}
// ��ȡ�ʹ������Ӿ��
void* C_ChnlObj::getSockHdl()
{
    return m_chnlDsc.locOpr_p;
}

int C_ChnlObj::setSockHdl ( void* sockHdl_p )
{
    m_chnlDsc.locOpr_p = sockHdl_p;
    return 0;
}
// ��ȡ�����ñ��������ַ
int C_ChnlObj::getLocNetAddr ( addrItem& locAddr )
{
    // IP:PORT ADDR:BAUDR
    memcpy ( &locAddr, &m_chnlDsc.localAddr, sizeof ( addrItem ) );

    return 0;
}

int C_ChnlObj::setLocNetAddr ( addrItem& locAddr )
{
    memcpy ( &m_chnlDsc.localAddr, &locAddr, sizeof ( addrItem ) );

    return 0;
}
// ��ȡ�����ô������֡��ASDU������
void* C_ChnlObj::getTranCtrl()
{
    return m_chnlDsc.tranCtrlBuf_p;
}

int C_ChnlObj::setTranCtrl ( char* tranCtrlBuf_p, int iLen )
{
    if ( iLen < MC_DEF_MAX_LEN )
    {
        return -1;
    }

    m_chnlDsc.tranCtrlBuf_p = tranCtrlBuf_p;

    return 0;
}
int C_ChnlObj::getASDUDsc()
{
    return 0;
}

int C_ChnlObj::setASDUDsc()
{
    return 0;
}
// ��ȡ������������Զ��վ
C_StnObj* C_ChnlObj::getStn()
{
    return m_chnlDsc.stnObj_p;
}
// ����Զ��վʱ����Զ��վ�������Ϣ���ظ�������
int C_ChnlObj::lockStn ( stnDsc* stnDsc_p, stnOrVew* stnOrVew_p )
{
    if ( stnDsc_p == NULL || stnOrVew_p == NULL )
    {
        return -10;
    }

    // ����Զ��վ����
    stnOrVew_p->stnID = m_chnlDsc.stnObj_p->getStnID ( stnDsc_p );
    stnOrVew_p->role = m_chnlDsc.stnObj_p->getStnRole ( stnDsc_p );
    stnOrVew_p->stnAddr = m_chnlDsc.localAddr.uiAddr;
    stnOrVew_p->stnAppID = m_chnlDsc.localAddr.usMask;

    // ����״̬��Ӧλ��3 bitͨ����Чλ
    m_ucState |= ~ ( 1 << 3 ); // ���� ��ʶ�ر� ����
    return 0;
}

int C_ChnlObj::unLockStn()
{
    // Զ��վָ������Ϊ��ʼֵ
    m_chnlDsc.stnObj_p->reSet();
    // ����״̬��Ӧλ��3 bitͨ����Чλ
    m_ucState |= MC_CHNL_BIT;   // �� 1 ��ʶ���� ����

    return 0;
}
// ��ȡ����������״̬
int C_ChnlObj::getChnlState ( char& cState )
{
    cState |= m_ucState;
    return 0;
}

int C_ChnlObj::setChnlState ( char cState )
{
    m_ucState = cState;
    return 0;
}

// �����ȥ���Ựʵ��
int C_ChnlObj::addSsn ( ssnCtlBlk* ssnCtlBlk_p, u_int uiProi )
{
    if ( ssnCtlBlk_p == NULL || uiProi <= 0 || uiProi > MC_DEF_MAX_PRIO )
    {
        return -10;
    }

    // add to ssn_table
    ssnCtlBlk* next = NULL;
    ssnCtlBlk* tail = NULL;

    switch ( ssnCtlBlk_p->actFlg )
    {
        case MC_ACT_FLAG://  �����Ự
            tail = m_chnlDsc.ssnTbl_p->ssnTblEntry[uiProi - 1].ssnCtlBlk_p;

            if ( tail == NULL )
            {
                // head
                m_chnlDsc.ssnTbl_p->ssnTblEntry[uiProi - 1].ssnCtlBlk_p = ssnCtlBlk_p;
                //printf("----MC_ACT_FLAG null \r\n");
            }
            else
            {
                while ( tail->nxtChnlSCBlk_p != NULL )
                {
                    tail = tail->nxtChnlSCBlk_p;
                }

                tail->nxtChnlSCBlk_p = ssnCtlBlk_p;
                ssnCtlBlk_p->nxtChnlSCBlk_p = NULL;
                //printf("----MC_ACT_FLAG \r\n");
            }

            m_chnlDsc.ssnTbl_p->ssnTblEntry[uiProi - 1].ucFlag |= ( 1 << MC_ACT_FLAG );

            //printf("----MC_ACT_FLAG---- \r\n");
            break;

        case MC_PSV_FLAG://  �����Ự
            next = m_chnlDsc.ssnTbl_p->ssnTblEntry[uiProi - 1].psvSsn_p;

            if ( next != NULL )
            {
                // �����Ựʵ����ʵ������
                return -20;
            }

            ssnCtlBlk_p->nxtChnlSCBlk_p = NULL;
            m_chnlDsc.ssnTbl_p->ssnTblEntry[uiProi - 1].psvSsn_p = ssnCtlBlk_p;
            m_chnlDsc.ssnTbl_p->ssnTblEntry[uiProi - 1].ucFlag |= ( 1 << MC_PSV_FLAG );

            break;

        case MC_A_P_FLAG://  ��ʵ��������
            next = m_chnlDsc.ssnTbl_p->ssnTblEntry[uiProi - 1].ssnCtlBlk_p;

            if ( next != NULL )
            {
                // ��ʵ��Ψһ����
                return -30;
            }
            else
            {
                m_chnlDsc.ssnTbl_p->ssnTblEntry[uiProi - 1].ssnCtlBlk_p = ssnCtlBlk_p;
            }

            m_chnlDsc.ssnTbl_p->ssnTblEntry[uiProi - 1].ucFlag = MC_A_P_FLAG;

            break;

        default:
            break;
    }

    return 0;
}

int C_ChnlObj::delSsn ( ssnCtlBlk* ssnCtlBlk_p, u_int uiProi, bool bNoDel )
{
    if ( ssnCtlBlk_p == NULL || uiProi <= 0 || uiProi > MC_DEF_MAX_PRIO )
    {
        return -10;
    }

    // �ӻỰʵ������ɾ����ʵ�����ƿ�
    ssnCtlBlk* next = NULL;

    switch ( ssnCtlBlk_p->actFlg )
    {
        case MC_ACT_FLAG://  �����Ự
            next = ssnCtlBlk_p->nxtChnlSCBlk_p;
            m_chnlDsc.ssnTbl_p->ssnTblEntry[uiProi - 1].ssnCtlBlk_p = next;

            if ( next == NULL )
            {
                m_chnlDsc.ssnTbl_p->ssnTblEntry[uiProi - 1].ucFlag &= ~ ( 1 << MC_ACT_FLAG );
            }

            break;

        case MC_PSV_FLAG://  �����Ự
            m_chnlDsc.ssnTbl_p->ssnTblEntry[uiProi - 1].psvSsn_p = NULL;
            m_chnlDsc.ssnTbl_p->ssnTblEntry[uiProi - 1].ucFlag &= ~ ( 1 << MC_PSV_FLAG );
            break;

        case MC_A_P_FLAG://  ��ʵ��������
            m_chnlDsc.ssnTbl_p->ssnTblEntry[uiProi - 1].ssnCtlBlk_p = NULL;
            m_chnlDsc.ssnTbl_p->ssnTblEntry[uiProi - 1].ucFlag = 0;
            break;

        default:
            break;
    }

    // ʵ�����ƿ�ռ��Ƿ��ͷţ�
    // ...
    return 0;
}

// ��ȡ�Ựʵ����ָ��
ssnTbl* C_ChnlObj::getSsnTbl()
{
    return m_chnlDsc.ssnTbl_p;
}
// ��ȡ�����Ựʵ�����ƿ�ָ��
ssnCtlBlk* C_ChnlObj::psvSsnCtlBlk ( u_int uiProi )
{
    if ( m_chnlDsc.ssnTbl_p == NULL || uiProi > MC_DEF_MAX_PRIO )
    {
        return NULL;
    }

    u_char ucMask = m_chnlDsc.ssnTbl_p->ssnTblEntry[uiProi - 1].ucFlag;
	if (m_chnlDsc.ssnTbl_p->ssnTblEntry[uiProi - 1].psvSsn_p != NULL)
	{
		printf("ucMask::chnlID = %d , ssnNo = %u\r\n", m_chnlDsc.ssnTbl_p->ssnTblEntry[uiProi - 1].psvSsn_p->chnlID,
			                                           m_chnlDsc.ssnTbl_p->ssnTblEntry[uiProi - 1].psvSsn_p->ssnNo);
	}
    //if ( ( ( ucMask >> MC_ACT_FLAG ) | 1 ) != 0 )
    if ( ( ( ucMask >> MC_PSV_FLAG ) & 1 ) != 0 )
    {
        //
        return m_chnlDsc.ssnTbl_p->ssnTblEntry[uiProi - 1].psvSsn_p;
    }

    return NULL;
}
// ��ȡ�����Ựʵ�����ƿ�ָ�롢���ߵ�ʵ���Ự���ƿ�ָ��
ssnCtlBlk* C_ChnlObj::actSsnCtlBlk ( u_int uiProi )
{
    if ( m_chnlDsc.ssnTbl_p == NULL || uiProi > MC_DEF_MAX_PRIO )
    {
        return NULL;
    }

    u_char ucMask = m_chnlDsc.ssnTbl_p->ssnTblEntry[uiProi - 1].ucFlag;

    //if ( ( ( ucMask >> MC_ACT_FLAG ) | 1 ) != 0 || ( ( ucMask >> MC_A_P_FLAG ) | 1 ) != 0 )
    if ( ( ( ucMask >> MC_ACT_FLAG ) & 1 ) != 0 || ( ( ucMask >> MC_A_P_FLAG ) & 1 ) != 0 )
    {
        // 0100 / 0001
        return m_chnlDsc.ssnTbl_p->ssnTblEntry[uiProi - 1].ssnCtlBlk_p;
    }

    return NULL;
}

C_ChnlObj* C_ChnlObj::getObj()
{
    return this;
}

ssnInstRslt* C_ChnlObj::getActSsnRslt ( u_short ssnID )
{
    if ( ssnID < 0 )
    {
        return NULL;
    }

    //
    for ( int i = MC_DEF_MAX_PRIO; i > 0 && m_chnlDsc.ssnTbl_p; i-- )
    {
        u_int uiPrio = i - 1;
        ssnCtlBlk* next = m_chnlDsc.ssnTbl_p->ssnTblEntry[uiPrio].ssnCtlBlk_p;

        while ( next != NULL )
        {
            if ( next->ssnNo == ssnID )
            {
                return &next->actSsnRprtInfo;
            }

            next = next->nxtSsnCtlBlk_p;
        }
    }

    return NULL;
}

HMODULE* C_ChnlObj::getPtclMdl()
{
    return &m_chnlDsc.ptclMdl;
}

HMODULE* C_ChnlObj::getCommMdl()
{
    return &m_chnlDsc.commMdl;
}

//setԶ��վ����ָ��
int C_ChnlObj::setStnObj ( C_StnObj* stnObj )
{
    if ( stnObj == NULL )
    {
        return -1;
    }

    m_chnlDsc.stnObj_p = stnObj;
    return 0;
}
//getԶ��վ����ָ��
C_StnObj* C_ChnlObj::getStnObj()
{
    return m_chnlDsc.stnObj_p;
}

//setԶ��վ�ص�����
int C_ChnlObj::setRtnfun ( void ( *rtnFun ) ( void* ) )
{
    if ( rtnFun == NULL )
    {
        return -1;
    }

    m_chnlDsc.rtnFun_p = ( void* ) rtnFun;
    return 0;
}
//getԶ��վ�ص��ú���
void* C_ChnlObj::getRtnfun()
{
    return m_chnlDsc.rtnFun_p;
}

// ���ն����ݻ�����ָ���ȡ
void* C_ChnlObj::getRcvBufPtr()
{
    return ( void* ) m_chnlDsc.revBuf_p;
}
// ���ն����ݻ�����ָ������
int C_ChnlObj::setRcvBufPtr ( void* rcvBufPtr[], char bufNum )
{
    if ( rcvBufPtr == NULL || bufNum <= 0 )
    {
        return -1;
    }

    for ( int i = 0; i < bufNum && i < MC_DEF_BUFF_NUM; i++ )
    {
        memcpy ( &m_chnlDsc.revBuf_p[i], rcvBufPtr[i], sizeof ( revBuf ) );
    }

    return 0;
}
//���Ͷ����ݻ���ָ���ȡ
void* C_ChnlObj::getSendBuf()
{
    return m_chnlDsc.sendBuf_p;
}
//���Ͷ����ݻ���ָ������
int C_ChnlObj::setSendBuf ( void* sendBuf[], char bufNum )
{
    if ( sendBuf == NULL || bufNum <= 0 )
    {
        return -1;
    }

    for ( int i = 0; i < bufNum && i < MC_DEF_BUFF_NUM; i++ )
    {
        memcpy ( &m_chnlDsc.sendBuf_p[i], sendBuf[i], sizeof ( revBuf ) );
        //m_chnlDsc.sendBuf_p[i] = sendBuf[i];
    }

    return 0;
}

int C_ChnlObj::setSsnSets ( ssnSets* inSsnSets, u_short usNum )
{
    if ( inSsnSets == NULL || usNum <= 0 )
    {
        return -10;
    }

    m_chnlDsc.locSsnInfo_p = new ssnSets[usNum];

    if ( m_chnlDsc.locSsnInfo_p == NULL )
    {
        return -20;
    }

    memcpy ( m_chnlDsc.locSsnInfo_p, inSsnSets, sizeof ( ssnSets ) * usNum );

    m_chnlDsc.numLocSsn = usNum;

    return 0;
}

ssnSets* C_ChnlObj::getSsnSet ( short ssnNo, u_char actFlag )
{
    if ( ssnNo < 0 )
    {
        return NULL;
    }

    for ( int i = 0; i < m_chnlDsc.numLocSsn; i++ )
    {
        if (  m_chnlDsc.locSsnInfo_p[i].actFlag == actFlag
              && m_chnlDsc.locSsnInfo_p[i].ssnNo == ssnNo )
        {
            return &m_chnlDsc.locSsnInfo_p[i];
        }
    }

    return NULL;
}

void* C_ChnlObj::getTranCtrlCfg()
{
    return m_chnlDsc.tranCtrl_p;
}

int C_ChnlObj::setTranCtrlCfg ( void* tranCtrlCfg_p )
{
    if ( tranCtrlCfg_p == NULL )
    {
        return -10;
    }

    m_chnlDsc.tranCtrl_p = tranCtrlCfg_p;
    return 0;
}

void* C_ChnlObj::getRealParam()
{
    return m_chnlDsc.realTranCtrl_p;
}

int C_ChnlObj::setRealParam ( void* realParam_p )
{
    if ( realParam_p == NULL )
    {
        return -10;
    }

    m_chnlDsc.realTranCtrl_p = realParam_p;
    return 0;
}



// �Ự����
// ���������
C_SsnObj::C_SsnObj()
{
    memset ( &m_ssnDescript, 0, sizeof ( ssnDsc ) );
    m_ssnCtrBlk_p = NULL;
}

C_SsnObj::~C_SsnObj()
{
    if ( m_ssnCtrBlk_p != NULL )
    {
        // ɾ���Ự�����лỰʵ����
        do
        {
            ssnCtlBlk* scbNext_p = m_ssnCtrBlk_p->nxtSsnCtlBlk_p;
            delete m_ssnCtrBlk_p;
            m_ssnCtrBlk_p = scbNext_p;
        } while ( m_ssnCtrBlk_p );
    }

    // ɾ���Ự����������ؿռ䣬��ͨ�����飩��Ϣ������ͷָ��
    //
    if ( m_ssnDescript.chnlInfoHead_p != NULL )
    {
        do
        {
            chnlInfoBlk* currChnl_p = m_ssnDescript.chnlInfoHead_p;
            chnlInfoBlk* nxtChnl_p = m_ssnDescript.chnlInfoHead_p->postBlk_p;

            delete currChnl_p;
            m_ssnDescript.chnlInfoHead_p = nxtChnl_p;

        } while ( m_ssnDescript.chnlInfoHead_p != NULL );
    }

    memset ( &m_ssnDescript, 0, sizeof ( ssnDsc ) );
}
// ��ȡ�Ự��
int C_SsnObj::GetSsnNo()
{
    return m_ssnDescript.ssnNo;
}
// ע��Ự����
int C_SsnObj::regSsnObj ( short ssnID, C_SsnObj* retSnnAddr_p )
{
    if ( ssnID <= 0 )
    {
        return -10;
    }

    m_ssnDescript.ssnNo = ssnID;
    retSnnAddr_p = this;

    return 0;
}
// ��ȡ�Ự���������������պ���ע���ַ
FunTypeOne C_SsnObj::getPtclSnnFun ( u_char ucMode )
{
    switch ( ucMode )
    {
        case 0x00:
            return m_ssnDescript.startSsnInst_p;

        case 0x01:
            return m_ssnDescript.driveSsnInst_p;

        case 0x02:
            return m_ssnDescript.chkSsnInst_p;

        default:
            break;
    }

    return NULL;
}
// ע��Ự�����û��ص�����
void C_SsnObj::CallBackForEnd ( FunTypeOne dstFun )
{
    if ( dstFun != NULL )
    {
        m_ssnDescript.usrCallback_p = dstFun;
    }

    return ;
}
// ע��Ự�����ASDU�洢���ַ�����úͻ�ȡ�Ựʵ����ASDU����
// int C_SnnObj::regASDUaddr(APDU* asduAddr_p)
// {
// //   asduAddr_p = m_snnDescript.ASDU_p;
//  return 0;
// }

int C_SsnObj::getASDUlen ( ssnCtlBlk* ssnCB_p/*, u_char actFlag*/ )
{
    if ( ssnCB_p )
    {
        if ( MC_ACT_FLAG == ssnCB_p->actFlg )
        {
            return ssnCB_p->actASDU_lng;
        }
        else
        {
            return ssnCB_p->psvASDU_lng;
        }
    }

    return -1;

    //  if ( actFlag == MC_ACT_FLAG )
    //  {
    //      return ssnCB_p->actASDU_lng;
    //  }
    //  else
    //  {
    //      return ssnCB_p->psvASDU_lng;
    //  }
}

int C_SsnObj::setASDUlen ( u_char ucASDUlen, ssnCtlBlk* ssnCB_p, u_char actFlag )
{
    if ( actFlag == MC_ACT_FLAG )
    {
        ssnCB_p->actASDU_lng = ucASDUlen;
    }
    else
    {
        ssnCB_p->psvASDU_lng = ucASDUlen;
    }

    return 0;
}
// ִ��ע�ắ��
int C_SsnObj::execRegFun ( u_char ucMode, void* funParam_p )
{
    switch ( ucMode )
    {
        case 0x00:// ��������
            m_ssnDescript.startSsnInst_p ( funParam_p );
            break;

        case 0x01:// ��������
            m_ssnDescript.driveSsnInst_p ( funParam_p );
            break;

        case 0x02:// ���պ���
            m_ssnDescript.chkSsnInst_p ( funParam_p );
            break;

        case 0xff:// �Ự�����û��ص�����(255)
            m_ssnDescript.usrCallback_p ( funParam_p );
            break;

        default:
            break;
    }

    return 0;
}
// ��ȡ�����������Ựʵ�������Ϣ����
//************************************
// Method:    getResultInfo
// FullName:  C_SnnObj::getResultInfo
// Access:    public
// Returns:   int <0, ssn fault; 0, ssn succ
// Qualifier:
// Parameter: ssnCtlBlk * actSsnCtlBlk_p
//
//************************************
ssnInstRslt* C_SsnObj::getResultInfo ( ssnCtlBlk* actSsnCtlBlk_p )
{
    if ( actSsnCtlBlk_p == NULL )
    {
        return NULL;
    }

    return &actSsnCtlBlk_p->actSsnRprtInfo;
}

int C_SsnObj::setResultInfo ( ssnCtlBlk* actSsnCtlBlk_p, int rstInfo )
{
    if ( actSsnCtlBlk_p == NULL )
    {
        return -10;
    }

    actSsnCtlBlk_p->actSsnRprtInfo = rstInfo;
    //if ( actSsnCtlBlk_p->actSsnRprtInfo != NULL )
    //{
    //    actSsnCtlBlk_p->actSsnRprtInfo = rstInfo;
    //    //memcpy(actSsnCtlBlk_p->actSsnRprtInfo, rstInfo_p, uiLen);
    //}

    return 0;
}
// ��ȡ�����ñ����Ựʵ�������Ϣ����
int  C_SsnObj::getPsvResInfo ( chnlInfoBlk* chnlInfo_p )
{
    if ( chnlInfo_p == NULL )
    {
        return NULL;
    }

    return chnlInfo_p->psvSsnRprtInfo;
}

int C_SsnObj::setPsvResInfo ( chnlInfoBlk* dstChnlInfo_p, int* psvResInfo_p )
{
    if ( dstChnlInfo_p == NULL || psvResInfo_p == NULL )
    {
        return -10;
    }

    /*if ( dstChnlInfo_p->psvSsnRprtInfo != NULL )
    {
    dstChnlInfo_p->psvSsnRprtInfo = *psvResInfo_p;
    }*/
    dstChnlInfo_p->psvSsnRprtInfo = *psvResInfo_p;
    return 0;
}
// ������ɾ�������Ựʵ��
int C_SsnObj::crtActiveSsn ( ssnCtlBlk** ssnCtlBlk_p, T_snnPara* ssnStartPara_p )
{
    ssnCtlBlk* newBlock = new ssnCtlBlk;

    if ( newBlock == NULL )
    {
        return -10;
    }

    memset ( newBlock, 0, sizeof ( ssnCtlBlk ) );
    //��ʼ��������@add@20180710
    //newBlock->ssnCtlLock.Init();
    //
    //  u_char *newMem = new u_char[MC_DEF_RPRT_INFO_LEN];
    //  if (newMem == NULL)
    //  {
    //      return -20;
    //  }
    newBlock->actSsnRprtInfo = 0;
    // ssnPara
    newBlock->ssnPara_p = ssnStartPara_p;
    //
    *ssnCtlBlk_p = newBlock;
    ( *ssnCtlBlk_p )->actFlg = MC_ACT_FLAG; // ��ʶ��Ϊ����
    // add to list's tail
    m_lock.Lock();
    ssnCtlBlk* next = m_ssnCtrBlk_p;

    if ( next == NULL )
    {
        m_ssnCtrBlk_p = newBlock;
    }
    else
    {
        while ( next->nxtSsnCtlBlk_p != NULL )
        {
            next = next->nxtSsnCtlBlk_p;
        }

        next->nxtSsnCtlBlk_p = newBlock;
        newBlock->preSsnCtlBlk_p = next;
    }

    // one_step
    newBlock->drvPrmtr_p = new ONE_STEP;

    if ( !newBlock->drvPrmtr_p )
    {
        m_lock.UnLock();
        return -20;
    }

    memset ( newBlock->drvPrmtr_p, 0, sizeof ( ONE_STEP ) );
    newBlock->drvPrmtr_p->reqAPDU_Obj_p = NULL;     // ϵͳ���ɻỰ����ʵ��ʱ��ʹ�ø�ȫ�ֱ���ʱ�������ֶθ�ֵ
    newBlock->drvPrmtr_p->respAPDU_Obj_p = NULL;        // ϵͳ���ɻỰ����ʵ��ʱ��ʹ�ø�ȫ�ֱ���ʱ�������ֶθ�ֵ
    newBlock->drvPrmtr_p->sVerifyBlock_p = new vrfItem();
    newBlock->drvPrmtr_p->sVerifyBlock_p->chkRslt_p = new int();

    newBlock->drvPrmtr_p->reqPDU_Num = 0;
    newBlock->drvPrmtr_p->respPDU_Num = 0;
    // APDU ��ַ�ռ䴴��  ������

    m_lock.UnLock();
    return 0;
}

int C_SsnObj::delActiveSsn ( ssnCtlBlk* ssnCtlBlk_p )
{
    if ( ssnCtlBlk_p == NULL )
    {
        return -10;
    }

    m_lock.Lock();
    // ��������ɾ�����ƿ�
    ssnCtlBlk* next = ssnCtlBlk_p->nxtSsnCtlBlk_p;
    ssnCtlBlk* pre = ssnCtlBlk_p->preSsnCtlBlk_p;

    if ( m_ssnCtrBlk_p == ssnCtlBlk_p ) // head
    {
        m_ssnCtrBlk_p = next;

        if ( next ) // more than 1
        {
            next->preSsnCtlBlk_p = NULL;
        }
    }
    else if ( !next ) // tail
    {
        pre->nxtSsnCtlBlk_p = NULL;
    }
    else
    {
        next->preSsnCtlBlk_p = pre;
        pre->nxtSsnCtlBlk_p = next;
    }


    // delete memory
    //  if ( ssnCtlBlk_p->actSsnRprtInfo != NULL )
    //  {
    //      delete ssnCtlBlk_p->actSsnRprtInfo;
    // }
    if ( ssnCtlBlk_p->myChnlInfoBlk_p )
    {
        m_lock.UnLock();
        delChnlInfo ( ssnCtlBlk_p->myChnlInfoBlk_p );
        ssnCtlBlk_p->myChnlInfoBlk_p = NULL;
        m_lock.Lock();
    }

    if ( ssnCtlBlk_p->drvPrmtr_p )
    {
        ssnCtlBlk_p->drvPrmtr_p->reqAPDU_Obj_p = NULL;
        ssnCtlBlk_p->drvPrmtr_p->respAPDU_Obj_p = NULL;

        if ( ssnCtlBlk_p->drvPrmtr_p->sVerifyBlock_p ) /////////////
        {
            if ( ssnCtlBlk_p->drvPrmtr_p->sVerifyBlock_p->chkRslt_p )
            {
                delete ssnCtlBlk_p->drvPrmtr_p->sVerifyBlock_p->chkRslt_p;
                ssnCtlBlk_p->drvPrmtr_p->sVerifyBlock_p->chkRslt_p = NULL;
            }

            delete ssnCtlBlk_p->drvPrmtr_p->sVerifyBlock_p;
            ssnCtlBlk_p->drvPrmtr_p->sVerifyBlock_p = NULL;
        }

        delete ssnCtlBlk_p->drvPrmtr_p;

        ssnCtlBlk_p->drvPrmtr_p = NULL;
    }

    // ע�������ֶοռ���ͷţ���������

    delete ssnCtlBlk_p;
    ssnCtlBlk_p = NULL;

    m_lock.UnLock();

    return 0;
}
// ������ɾ�������Ựʵ��
int C_SsnObj::crtPassiveSsn ( ssnCtlBlk** ssnCtlBlk_p, T_snnPara* ssnStartPara_p )
{
    ssnCtlBlk* newBlock = new ssnCtlBlk;

    if ( newBlock == NULL )
    {
        return -10;
    }

    memset ( newBlock, 0, sizeof ( ssnCtlBlk ) );
    //��ʼ��������@add@20180710
    //newBlock->ssnCtlLock.Init();
    //
    //  u_char *newMem = new u_char[MC_DEF_RPRT_INFO_LEN];
    //  if (newMem == NULL)
    //  {
    //      return -20;
    //  }
    //  newBlock->actSsnRprtInfo = newMem;
    //
    *ssnCtlBlk_p = newBlock;
    ( *ssnCtlBlk_p )->actFlg = MC_PSV_FLAG; // ��ʶ��Ϊ����

    m_lock.Lock();
    // add to list's tail
    ssnCtlBlk* next = m_ssnCtrBlk_p;

    if ( next == NULL )
    {
        m_ssnCtrBlk_p = newBlock;
    }
    else
    {
        while ( next->nxtSsnCtlBlk_p != NULL )
        {
            next = next->nxtSsnCtlBlk_p;
        }

        next->nxtSsnCtlBlk_p = newBlock;
        newBlock->preSsnCtlBlk_p = next;
    }

    newBlock->drvPrmtr_p = new ONE_STEP;

    if ( !newBlock->drvPrmtr_p )
    {
        m_lock.UnLock();
        return -20;
    }

    newBlock->drvPrmtr_p->reqAPDU_Obj_p = NULL;     // ϵͳ���ɻỰ����ʵ��ʱ��ʹ�ø�ȫ�ֱ���ʱ�������ֶθ�ֵ
    newBlock->drvPrmtr_p->respAPDU_Obj_p = NULL;        // ϵͳ���ɻỰ����ʵ��ʱ��ʹ�ø�ȫ�ֱ���ʱ�������ֶθ�ֵ
    newBlock->drvPrmtr_p->sVerifyBlock_p = new vrfItem();
    newBlock->drvPrmtr_p->sVerifyBlock_p->chkRslt_p = new int();

    newBlock->drvPrmtr_p->reqPDU_Num = 0;
    newBlock->drvPrmtr_p->respPDU_Num = 0;

    // ssnPara
    newBlock->ssnPara_p = ssnStartPara_p;

    m_lock.UnLock();

    return 0;
}

int C_SsnObj::delPassiveSsn ( ssnCtlBlk* ssnCtlBlk_p )
{
    if ( ssnCtlBlk_p == NULL )
    {
        return -10;
    }

    m_lock.Lock();
    // ��������ɾ�����ƿ�
    ssnCtlBlk* next = ssnCtlBlk_p->nxtSsnCtlBlk_p;
    ssnCtlBlk* pre = ssnCtlBlk_p->preSsnCtlBlk_p;
    next->preSsnCtlBlk_p = pre;
    pre->nxtSsnCtlBlk_p = next;
    // delete memory
    //  if ( ssnCtlBlk_p->actSsnRprtInfo != NULL )
    //  {
    //      delete ssnCtlBlk_p->actSsnRprtInfo;
    //  }
    m_lock.UnLock();

    //
    if ( ssnCtlBlk_p->myChnlInfoBlk_p )
    {
        delChnlInfo ( ssnCtlBlk_p->myChnlInfoBlk_p );
        ssnCtlBlk_p->myChnlInfoBlk_p = NULL;
    }

    delete ssnCtlBlk_p;

    return 0;
}
// ����ͨ����Ϣ�鵽�Ự��������
int C_SsnObj::crtChnlInfo ( chnlInfoBlk*& chnlInfoB_p  )
{
    if ( chnlInfoB_p == NULL /*|| chnlInfoB_p->funRl_p == NULL */ )
    {
        return -10;
    }

    chnlInfoBlk* newBlock = new chnlInfoBlk;

    if ( newBlock == NULL )
    {
        return -10;
    }

    memset ( newBlock, 0, sizeof ( chnlInfoBlk ) );

    m_lock.Lock();
    memcpy ( newBlock, chnlInfoB_p, sizeof ( chnlInfoBlk ) );
    newBlock->funRl_p = chnlInfoB_p->funRl_p;
    newBlock->ssnStnCnfg_p = chnlInfoB_p->ssnStnCnfg_p;

    newBlock->preBlk_p = NULL;
    newBlock->postBlk_p = NULL;

    //  newBlock->ssnStnCnfg_p = new T_snnCfg;
    //  memset(newBlock->ssnStnCnfg_p, 0, sizeof(T_snnCfg));
    //  u_char *newMem = new u_char[MC_DEF_RPRT_INFO_LEN];
    //  if (newMem == NULL)
    //  {
    //      return -20;
    //  }
    //  newBlock->psvSsnRprtInfo = newMem;


    // add to list's tail
    chnlInfoBlk* chnlInfoTail = m_ssnDescript.chnlInfoHead_p;

    if ( chnlInfoTail == NULL )
    {
        m_ssnDescript.chnlInfoHead_p = newBlock;
    }
    else
    {
        while ( chnlInfoTail->postBlk_p != NULL )
        {
            chnlInfoTail = chnlInfoTail->postBlk_p;
        }

        chnlInfoTail->postBlk_p = newBlock;
        newBlock->preBlk_p = chnlInfoTail;
    }

    m_lock.UnLock();

    chnlInfoB_p = newBlock;

    return 0;
}

int C_SsnObj::delChnlInfo ( chnlInfoBlk* chnlInfoB_p )
{
    if ( chnlInfoB_p == NULL )
    {
        return -10;
    }

    m_lock.Lock();
    chnlInfoBlk* pre = chnlInfoB_p->preBlk_p;
    chnlInfoBlk* nxt = chnlInfoB_p->postBlk_p;

    if ( pre == NULL ) // head
    {
        m_ssnDescript.chnlInfoHead_p = nxt;

        if ( nxt )
        {
            nxt->preBlk_p = NULL;
        }
    }
    else if ( nxt == NULL ) // tail
    {
        pre->postBlk_p = NULL;
    }
    else // middle
    {
        pre->postBlk_p = nxt;
        nxt->preBlk_p = pre;
    }

    //
    chnlInfoB_p->preBlk_p = NULL;
    chnlInfoB_p->postBlk_p = NULL;

    if ( chnlInfoB_p->funRl_p )
    {
        delete []chnlInfoB_p->funRl_p;
        chnlInfoB_p->funRl_p = NULL;
    }

    /*
    if ( chnlInfoB_p->ssnStnCnfg_p != NULL )
    {
    delete []chnlInfoB_p->ssnStnCnfg_p;
    chnlInfoB_p->ssnStnCnfg_p = NULL;
    }
    */

    delete chnlInfoB_p;
    chnlInfoB_p = NULL;
    //
    m_lock.UnLock();

    return 0;
}

int C_SsnObj::getSsnPartInfo ( ssnCtlBlk* ssnCtlBlk_p, ssnCtlBlk* dstSsn_p )
{
    if ( ssnCtlBlk_p == NULL && dstSsn_p == NULL )
    {
        return -10;
    }

    //  ssnInstInfo_p->actFlg = ssnCtlBlk_p->actFlg;
    //  ssnInstInfo_p->chnlNo = ssnCtlBlk_p->chnlNo;
    //  ssnInstInfo_p->drvPrmtr_p = ssnCtlBlk_p->drvPrmtr_p; // memmory
    //  ssnInstInfo_p->actSsnRprtInfo_p = ssnCtlBlk_p->actSsnRprtInfo_p;
    //  ssnInstInfo_p->ssnPara_p = ssnCtlBlk_p->ssnPara_p;  // memmory
    //  ssnInstInfo_p->checkState = ssnCtlBlk_p->checkState;
    //  ssnInstInfo_p->drvStepNo = ssnCtlBlk_p->drvStepNo;

    return 0;
}


int C_SsnObj::getSsnInstInfo ( ssnCtlBlk* ssnCtlBlk_p, T_ssnInstInfoSet* ssnInstInfo_p )
{
    if ( ssnCtlBlk_p == NULL && ssnInstInfo_p == NULL )
    {
        return -10;
    }

    ssnInstInfo_p->actFlg = ssnCtlBlk_p->actFlg;
    ssnInstInfo_p->chnlID = ssnCtlBlk_p->chnlID;
    ssnInstInfo_p->drvPrmtr_p = ssnCtlBlk_p->drvPrmtr_p; // memmory
    ssnInstInfo_p->actSsnRprtInfo_p = &ssnCtlBlk_p->actSsnRprtInfo;
    ssnInstInfo_p->ssnPara_p = ssnCtlBlk_p->ssnPara_p;  // memmory
    ssnInstInfo_p->checkState = ssnCtlBlk_p->checkState;
    ssnInstInfo_p->drvStepNo = ssnCtlBlk_p->drvStepNo;

    if ( MC_ACT_FLAG == ssnCtlBlk_p->actFlg )
    {
        ssnInstInfo_p->ASDU_lng = ssnCtlBlk_p->actASDU_lng;
    }
    else
    {
        ssnInstInfo_p->ASDU_lng = ssnCtlBlk_p->psvASDU_lng;
    }

    return 0;

}


ASDU* C_SsnObj::getAPDU ( u_char actFlag, short chnlID )
{
    if ( chnlID < 0 )
    {
        return NULL;
    }

    m_lock.Lock();
    // second:find the chnlinfo
    chnlInfoBlk* nxtChnl = m_ssnDescript.chnlInfoHead_p;

    while ( nxtChnl )
    {
        if ( nxtChnl->chnlNo == chnlID )
        {
            break;
        }

        nxtChnl = nxtChnl->postBlk_p;
    }

    if ( nxtChnl == NULL )
    {
        m_lock.UnLock();
        return NULL;
    }

    if ( actFlag == MC_ACT_FLAG )
    {
        m_lock.UnLock();
        return nxtChnl->reqAPDU_Obj_p/*.getASDU()*/;//actASDU_p;
    }
    else
    {
        m_lock.UnLock();
        return nxtChnl->respAPDU_Obj_p/*.getASDU()*/;//psvASDU_p;
    }

    m_lock.UnLock();
    return NULL;
}

ASDU* C_SsnObj::getAPDU ( u_char actFlag, chnlInfoBlk* chnlIBlk_p )
{
    if ( chnlIBlk_p == 0 )
    {
        return NULL;
    }

    m_lock.Lock();

    if ( actFlag == MC_ACT_FLAG )
    {
        m_lock.UnLock();
        return chnlIBlk_p->reqAPDU_Obj_p/*.getASDU()*/;//actASDU_p;
    }
    else
    {
        m_lock.UnLock();
        return chnlIBlk_p->respAPDU_Obj_p/*.getASDU()*/;//psvASDU_p;
    }

    m_lock.UnLock();
    return NULL;
}


chnlInfoBlk* C_SsnObj::getChnlInfoBlk ( u_short chnlID )
{
    if ( chnlID < 0 )
    {
        return NULL;
    }

    m_lock.Lock();

    chnlInfoBlk* next = m_ssnDescript.chnlInfoHead_p;

    while ( next )
    {
        if ( next->chnlNo == chnlID )
        {
            // hit
            m_lock.UnLock();
            return next;
        }

        next = next->postBlk_p;
    }

    //  // first:find the ssnctrlblock
    //  ssnCtlBlk* next = m_ssnCtrBlk_p;
    //  while ( next )
    //  {
    //      // second:find the chnlinfo
    //      chnlInfoBlk* nxtChnl = next->myChnlInfoBlk_p;
    //      while ( nxtChnl )
    //      {
    //          if ( nxtChnl->chnlNo = chnlID )
    //          {
    //              return nxtChnl;
    //          }
    //          nxtChnl = nxtChnl->postBlk_p;
    //      }
    //
    //      next = next->nxtSsnCtlBlk_p;
    //  }
    m_lock.UnLock();
    return NULL;
}

ssnInstRslt* C_SsnObj::getPsvSsnRslt ( u_short chnlID )
{
    if ( chnlID < 0 )
    {
        return NULL;
    }

    m_lock.Lock();
    // first:find the ssnctrlblock
    ssnCtlBlk* next = m_ssnCtrBlk_p;

    while ( next )
    {
        // second:find the chnlinfo
        chnlInfoBlk* nxtChnl = next->myChnlInfoBlk_p;

        while ( nxtChnl )
        {
            if ( nxtChnl->chnlNo == chnlID )
            {
                m_lock.UnLock();
                return ( ssnInstRslt* ) nxtChnl->psvSsnRprtInfo;
            }

            nxtChnl = nxtChnl->postBlk_p;
        }

        next = next->nxtSsnCtlBlk_p;
    }

    m_lock.UnLock();

    return NULL;
}

int C_SsnObj::getSsnPara ( ssnCtlBlk* ssnCtlBlk_p, T_snnPara* snnPar_p )
{
    if ( ssnCtlBlk_p == NULL || snnPar_p == NULL )
    {
        return -1;
    }

    snnPar_p = ssnCtlBlk_p->ssnPara_p;

    return 0;
}

int C_SsnObj::regASDUaddr ( chnlInfoBlk* chnlInfoBlk_p, APDU* asduAddr_p, u_char actFlag )
{
    if ( asduAddr_p == NULL || chnlInfoBlk_p == NULL )
    {
        return -10;
    }

    if ( actFlag == MC_ACT_FLAG )
    {
        chnlInfoBlk_p->respAPDU_Obj_p/*actASDU_p*/ = asduAddr_p;
    }
    else
    {
        chnlInfoBlk_p->reqAPDU_Obj_p/*psvASDU_p*/ = asduAddr_p;
    }

    return 0;
}

// void C_SnnObj::GetSnnName()
// {
//
// }
//
// void C_SnnObj::GetPriority()
// {
//
// }

// Զ��վ����
// ���ɺ�ɾ��Զ��վ
C_StnObj::C_StnObj()
{
    m_stnHead = NULL;
    m_stnNum = 0;
}

C_StnObj::~C_StnObj()
{
    // realese memory
    if ( m_stnHead != NULL )
    {
        stnDsc* next = m_stnHead;
        stnDsc* del = NULL;

        do
        {
            stnDsc* nxtStn_p = next->nxtStn_p;

            while ( next )
            {
                del = next;
                // ��ȡ��һ���߼���Զ��վ
                next = next->nxtLgcStn_p;
                // ��������������ַ�Ƿ�����Ҫ����ɾ���ģ���
                //
                delete del;
                del = NULL;
            }

            // ��ȡ��һ������Զ��վ
            next = nxtStn_p;
        } while ( next != NULL );
    }

}

// ��λ
int C_StnObj::reSet()
{
    // �����������ٻ���ָ�롢�洢ӳ�����Ǹ�������
    // �������������ݲ�ʵ�֣����Դ˽ӿ��ݲ�ʵ��
    // ...

    return 0;
}
// ��ȡ������Զ��վ��
int C_StnObj::getStnID ( stnDsc* stnDsc_p )
{
    if ( stnDsc_p == NULL )
    {
        return -10;
    }

    return stnDsc_p->stnID;
}

int C_StnObj::setStnID ( stnDsc* stnDsc_p, short stnID )
{
    if ( stnID < 0 || stnDsc_p == NULL )
    {
        return -10;
    }

    m_lock.Lock();
    stnDsc_p->stnID = stnID;
    m_lock.UnLock();

    return 0;
}
// ��ȡ���������Ӻ�
int C_StnObj::getChnlID ( stnDsc* stnDsc_p )
{
    if ( stnDsc_p == NULL )
    {
        return -10;
    }

    return stnDsc_p->chnlID;
}

int C_StnObj::setChnlID ( stnDsc* stnDsc_p, short chnlID )
{
    if ( chnlID < 0 || stnDsc_p == NULL )
    {
        return -10;
    }

    m_lock.Lock();
    stnDsc_p->chnlID = chnlID;
    m_lock.UnLock();

    return 0;
}
// ��ȡ������Զ��վ��
int C_StnObj::getStnName ( stnDsc* stnDsc_p, char* stnName_p, int& iLen )
{
    if ( stnDsc_p == NULL || stnName_p == NULL )
    {
        return -10;
    }

    m_lock.Lock();
    int iNameLen = strlen ( stnDsc_p->stnName_p );

    if ( iLen < iNameLen )
    {
        m_lock.UnLock();
        return -10;
    }

    memcpy ( stnName_p, stnDsc_p->stnName_p, iNameLen );
    iLen = iNameLen;
    //
    m_lock.UnLock();
    //
    return 0;
}

int C_StnObj::setStnName ( stnDsc* stnDsc_p, char* stnName_p, int iLen )
{
    if ( iLen <= 0 || stnName_p == NULL || stnDsc_p == NULL )
    {
        return -10;
    }

    m_lock.Lock();

    if ( stnDsc_p->stnName_p != NULL )
    {
        delete stnDsc_p->stnName_p;
        stnDsc_p->stnName_p = NULL;
    }

    stnDsc_p->stnName_p = new char[iLen + 2];

    if ( stnDsc_p->stnName_p == NULL )
    {
        m_lock.UnLock();
        return -20;
    }

    memcpy ( stnDsc_p->stnName_p, stnName_p, iLen );
    stnDsc_p->stnName_p[iLen] = '\0';
    //
    m_lock.UnLock();
    //
    return 0;
}
// ��ȡ������Զ��վ��ɫ
char C_StnObj::getStnRole ( stnDsc* stnDsc_p )
{
    if ( stnDsc_p == NULL )
    {
        return -10;
    }

    return stnDsc_p->stnRole;
}

int C_StnObj::setStnRole ( stnDsc* stnDsc_p, char stnRole )
{
    if ( stnRole < 0 || stnDsc_p == NULL )
    {
        return -10;
    }

    m_lock.Lock();
    stnDsc_p->stnRole = stnRole;
    m_lock.UnLock();
    return 0;
}
// ��ȡ�߼�վ����
int C_StnObj::getLogicStn ( short stnID, stnDsc* lgcStn_p )
{
    if ( stnID < 0 || m_stnHead == NULL || ( m_stnHead->nxtStn_p == NULL && m_stnHead->nxtLgcStn_p ) )
    {
        return -10;
    }

    lgcStn_p = NULL;

    //
    m_lock.UnLock();
    //
    // ����վ�ţ���������վ��������Ե��߼�վ��������Ѱ��
    // ���߼�������
    stnDsc* next = m_stnHead;

    do
    {
        while ( next )
        {
            if ( next->stnID == stnID )
            {
                lgcStn_p = next;
                break;
            }

            // ��ȡ��һ���߼���Զ��վ
            next = next->nxtLgcStn_p;
        }

        if ( lgcStn_p != NULL )
        {
            // hit
            break;
        }

        // ��ȡ��һ������Զ��վ
        next = next->nxtStn_p;
    } while ( next != NULL );

    m_lock.UnLock();

    return 0;
}
// ��ȡ��һԶ��վ����
int C_StnObj::getNextStn ( stnDsc* stnDsc_p, stnDsc* retStn_p )
{
    if ( stnDsc_p == NULL )
    {
        return -10;
    }

    m_lock.UnLock();
    retStn_p = stnDsc_p->nxtStn_p;
    m_lock.UnLock();
    return 0;
}
// ��Ӻ�ɾ���߼�վ���
int C_StnObj::addLogicStn ( stnDsc* dstStn_p, stnDsc* addStn_p )
{
    if ( dstStn_p == NULL || addStn_p == NULL )
    {
        return -10;
    }

    //new memory?
    stnDsc* newStn_p = new stnDsc;

    if ( newStn_p == NULL )
    {
        return -20;
    }

    int iRet = newStn_p->copyStn ( addStn_p );

    if ( iRet != 0 )
    {
        delete newStn_p;
        return iRet;
    }

    //
    m_lock.Lock();
    //
    short stnID = addStn_p->stnID;
    stnDsc* next = dstStn_p;
    stnDsc* pre = next;

    while ( next && next->nxtLgcStn_p != NULL )
    {
        if ( next->stnID > stnID )
        {
            break;
        }

        pre = next;
        next = next->nxtLgcStn_p;
    }

    next = pre->nxtLgcStn_p;
    newStn_p->nxtLgcStn_p = next;
    pre->nxtLgcStn_p = newStn_p;
    //
    ++m_stnNum;
    //
    m_lock.UnLock();
    //
    return 0;
}

// int C_StnObj::delLogicStn( stnDsc* dstStn_p, stnDsc* delStn_p )
// {
//  if ( delStn_p == NULL )
//  {
//      return -10;
//  }
//
//  if ( dstStn_p == NULL )
//  {
//      dstStn_p = m_stnHead;
//  }
//  short stnID = delStn_p->stnID;
//  stnDsc* next = dstStn_p;
//  stnDsc* pre = next;
//  while ( next && next->nxtLgcStn_p != NULL )
//  {
//      if ( next->stnID == stnID )
//      {
//          break;
//      }
//      pre = next;
//      next = next->nxtLgcStn_p;
//  }
//  if ( next == NULL )
//  {
//      return -20;
//  }
//  //
//  pre->nxtLgcStn_p = delStn_p->nxtLgcStn_p;
//  delStn_p->nxtLgcStn_p = NULL;
//  // �����ڴ�ռ䣿�� ����վ��
//  delete delStn_p;
//
//  return 0;
// }
// �Ǽǡ�ע��������
int C_StnObj::loginInfoTbl ( stnDsc* stnDsc_p, void* infoTbl_p )
{
    if ( infoTbl_p == NULL || stnDsc_p == NULL )
    {
        return -10;
    }

    m_lock.Lock();
    stnDsc_p->infoTbl_p = infoTbl_p;
    m_lock.UnLock();

    return 0;
}

int C_StnObj::logoffInfoTbl ( stnDsc* stnDsc_p )
{
    if ( stnDsc_p == NULL )
    {
        return -10;
    }

    m_lock.Lock();
    stnDsc_p->infoTbl_p = NULL;
    m_lock.UnLock();
    return 0;
}
// ���ú����ѭ�����ݴ����ʶ
int C_StnObj::setCleFlag ( stnDsc* stnDsc_p )
{
    if ( stnDsc_p == NULL )
    {
        return -10;
    }

    m_lock.Lock();
    stnDsc_p->cleTranFlag = 1;
    m_lock.UnLock();
    return 0;
}

int C_StnObj::delCleFlag ( stnDsc* stnDsc_p )
{
    if ( stnDsc_p == NULL )
    {
        return -10;
    }

    stnDsc_p->cleTranFlag = 0;

    return 0;
}
// ������ͷ�������ٻ�����
int C_StnObj::crtJumpFlw ( stnDsc* stnDsc_p )
{
    // ...
    return 0;
}

int C_StnObj::rlsJumpFlw ( stnDsc* stnDsc_p )
{
    // ...
    return 0;
}
// ������ͷŴ洢ӳ�����Ǹ�����������
int C_StnObj::crtStrgMap ( stnDsc* stnDsc_p )
{
    // ...
    return 0;
}

int C_StnObj::rlsStrgMap ( stnDsc* stnDsc_p )
{
    // ...
    return 0;
}
// �Ǽ���Ϣ��������ص�����
int C_StnObj::loginJumpFun ( stnDsc* stnDsc_p, FunTypeOne jumpFun )
{
    if ( stnDsc_p == NULL )
    {
        return -10;
    }

    stnDsc_p->jumpFlw_p = ( void* ) jumpFun;

    return 0;
}
// ��ȡ��Ϣ��������ص�����ָ��
FunTypeOne C_StnObj::getJumpFun ( stnDsc* stnDsc_p )
{
    if ( stnDsc_p == NULL )
    {
        return NULL;
    }

    return stnDsc_p->callBackFun;
}
// ��Ϣ���������¼����
int C_StnObj::execRcdExpFun ( stnDsc* stnDsc_p, void* exeParam_p )
{
    if ( stnDsc_p == NULL )
    {
        return -10;
    }

    stnDsc_p->rcdExpFun ( exeParam_p );

    return 0;
}
// ��ȡ��Ϣ���������¼����ָ��
FunTypeOne C_StnObj::getRcdExpFun ( stnDsc* stnDsc_p )
{
    if ( stnDsc_p == NULL )
    {
        return NULL;
    }

    return stnDsc_p->rcdExpFun;
}
// ��Ӻ�ɾ������Զ��վ���󣨰���Զ��վ���������У�
int C_StnObj::addPscStn ( stnDsc* stnDsc_p )
{
    if ( stnDsc_p == NULL )
    {
        return -10;
    }

    // new memory
    stnDsc* newStn_p = new stnDsc;

    if ( newStn_p == NULL )
    {
        return -20;
    }

    int iRet = newStn_p->copyStn ( stnDsc_p );

    if ( iRet != 0 )
    {
        delete newStn_p;
        return iRet;
    }

    //
    m_lock.Lock();
    //
    short stnID = stnDsc_p->stnID;
    stnDsc* next = m_stnHead;
    stnDsc* pre = next;

    while ( next && next->nxtStn_p != NULL )
    {
        if ( next->stnID > stnID )
        {
            break;
        }

        pre = next;
        next = next->nxtStn_p;
    }

    if ( pre == NULL )
    {
        // head
        m_stnHead = newStn_p;
    }
    else
    {
        next = pre->nxtStn_p;
        newStn_p->nxtStn_p = next;
        pre->nxtStn_p = newStn_p;
    }

    //
    ++m_stnNum;
    //
    m_lock.UnLock();
    //
    return 0;
}

int C_StnObj::delStn ( short stnID )
{
    if ( stnID < 0 )
    {
        return -10;
    }

    // ����վ�ţ���������վ��������Ե��߼�վ��������Ѱ��
    // ���߼�������
    stnDsc* next = m_stnHead;
    stnDsc* pre = m_stnHead;
    bool bPscFlag = true;
    //
    m_lock.Lock();

    //
    do
    {
        while ( next )
        {
            if ( next->stnID == stnID )
            {
                break;
            }

            // ��ȡ��һ���߼���Զ��վ
            pre = next;
            next = next->nxtLgcStn_p;
            bPscFlag = true;
        }

        if ( next != NULL )
        {
            // hit
            break;
        }

        // ��ȡ��һ������Զ��վ
        pre = next;
        next = next->nxtStn_p;
        bPscFlag = false;
    } while ( next != NULL );

    if ( next == NULL )
    {
        m_lock.UnLock();
        return -30;
    }

    if ( bPscFlag )
    {
        // ������
        if ( next->nxtLgcStn_p == NULL )
        {
            // logic stn's list is null
            pre->nxtStn_p = next->nxtStn_p;
        }
        else
        {
            pre->nxtStn_p = next->nxtLgcStn_p;
            // preԶ��վ�������߼�վ����һ����Զ���ҵ�ַ���ı�
            stnDsc* nxtStn_p = pre->nxtLgcStn_p;

            while ( nxtStn_p != NULL )
            {
                nxtStn_p->nxtStn_p = next->nxtLgcStn_p;
                nxtStn_p = nxtStn_p->nxtLgcStn_p;
            }
        }
    }
    else
    {
        // �߼���
        pre->nxtLgcStn_p = next->nxtLgcStn_p;
        next->nxtLgcStn_p = NULL;
    }

    // �����ڴ�ռ�
    delete next;
    next = NULL;
    //
    --m_stnNum;
    //
    m_lock.UnLock();

    return 0;
}

stnDsc* C_StnObj::getStnDsc ( short stnID )
{
    if ( stnID < 0 || m_stnHead == NULL )
    {
        return NULL;
    }

    //
    // ����վ�ţ���������վ��������Ե��߼�վ��������Ѱ��
    // ���߼�������
    stnDsc* next = m_stnHead;
    stnDsc* pre = m_stnHead;
    bool bPscFlag = true;
    //
    m_lock.Lock();

    //
    do
    {
        stnDsc* currStn = next;

        while ( next )
        {
            if ( next->stnID == stnID )
            {
                break;
            }

            // ��ȡ��һ���߼���Զ��վ
            pre = next;
            next = next->nxtLgcStn_p;
            bPscFlag = true;
        }

        if ( next != NULL )
        {
            // hit
            m_lock.UnLock();
            return next;
        }
        else
        {
            next = currStn;
        }

        // ��ȡ��һ������Զ��վ
        pre = next;
        next = next->nxtStn_p;
        bPscFlag = false;
    } while ( next != NULL );

    //
    m_lock.UnLock();

    return NULL;
}

int C_StnObj::loadStnData ( T_snnPara* ssnPara_p, short stnID, u_char ucType, u_short ucMask, int iPtclID )
{
    if ( ssnPara_p == NULL || stnID < 0 )
    {
        return -1;
    }

    //
    stnDsc* stnDsc_p = getStnDsc ( stnID );

    if ( stnDsc_p == NULL )
    {
        return -10;
    }

    //
    if ( ucType == 0 )
    {
        // gdata
        if ( stnDsc_p->gStorageObj_p == NULL )
        {
            return -20;
        }

        if ( iPtclID == 104 )
        {
            return stnDsc_p->gStorageObj_p->loadStnData ( ssnPara_p, ucMask & SYSELE_MASK_104 );
        }
        else if ( iPtclID == 106 )
        {
            return stnDsc_p->gStorageObj_p->loadStnData ( ssnPara_p, ucMask & SYSELE_MASK_MODBUS );
        }

    }
    else if ( ucType == 1 )
    {
        if ( stnDsc_p->fStorageObj_p == NULL )
        {
            return -20;
        }

        if ( iPtclID == 104 )
        {
            return stnDsc_p->fStorageObj_p->loadStnData ( ssnPara_p, ucMask & SYSELE_MASK_104 );
        }
        else if ( iPtclID == 106 )
        {
            return stnDsc_p->fStorageObj_p->loadStnData ( ssnPara_p, ucMask & SYSELE_MASK_MODBUS );
        }

    }


    return 0;
}

int C_StnObj::storeStnData ( T_snnPara* ssnPara_p, short stnID, u_char ucType, u_short ucMask, int iPtclID )
{
    if ( ssnPara_p == NULL || stnID < 0 )
    {
        return -1;
    }

    //
    stnDsc* stnDsc_p = getStnDsc ( stnID );

    if ( stnDsc_p == NULL )
    {
        return -10;
    }

    if ( ucType == 0 )
    {
        if ( stnDsc_p->gStorageObj_p == NULL )
        {
            return -20;
        }

        //
        if ( iPtclID == 104 )
        {
            return stnDsc_p->gStorageObj_p->storeStnData ( ssnPara_p, ucMask & SYSELE_MASK_104 );
        }
        else if ( iPtclID == 106 )
        {
            return stnDsc_p->gStorageObj_p->storeStnData ( ssnPara_p, ucMask & SYSELE_MASK_MODBUS );
        }
    }
    else if ( ucType == 1 )
    {
        if ( stnDsc_p->fStorageObj_p == NULL )
        {
            return -20;
        }

        //
        if ( iPtclID == 104 )
        {
            return stnDsc_p->fStorageObj_p->storeStnData ( ssnPara_p, ucMask & SYSELE_MASK_104 );
        }
        else if ( iPtclID == 106 )
        {
            return stnDsc_p->fStorageObj_p->storeStnData ( ssnPara_p, ucMask & SYSELE_MASK_MODBUS );
        }

    }

    return 0;
}
// ��ʼ��Զ��վ�������м��״洢��ע���˽ӿ�ÿ��ֻ�ܳ�ʼ��һ�����ݣ���������ּ��������ݵĻ�����Ҫ���ö��
int C_StnObj::initDStorage ( stnDsc* stnDsc_p, addrRange* cfgAddr_p, short cfgLen, u_short usrType )
{
    if ( stnDsc_p == NULL )
    {
        return -1;
    }

    if ( cfgAddr_p == NULL )
    {
        return -1;
    }

    if ( cfgLen < 0 )
    {
        return -1;
    }

    if ( usrType == 0 )
    {
        stnDsc_p->gStorageObj_p = new C_DStorage ( cfgAddr_p, cfgLen, usrType );
    }
    else if ( usrType == 1 )
    {
        stnDsc_p->fStorageObj_p = new C_DStorage ( cfgAddr_p, cfgLen, usrType );
    }
    else
    {
        return -1;
    }

    return 0;
}


C_DStorage::C_DStorage()
{

}

//
// ���״洢
C_DStorage::C_DStorage ( addrRange* cfgAddr_p, short cfgLen, u_char usrType )
{
    m_usrType = usrType;
    m_arLen = cfgLen;
    m_addrIdx = new addrRange[cfgLen];//�����ָ����
    this->m_blkLen = DEF_BLK_LEN;//����������ΪĬ��
    this->m_blkNum = 0;
    u_int sum = 0;

    //crtQLock(this->memKey);
    for ( short i = 0; i < cfgLen; i++ )
    {
        memcpy ( &m_addrIdx[i], &cfgAddr_p[i], sizeof ( addrRange ) );
        sum = sum + ( cfgAddr_p[i].endAddr - cfgAddr_p[i].begAddr );//��������е��������Ⱥ�
    }

    if ( ( ( sum / DEF_BLK_LEN ) == DEF_MAX_BLK_NUM && sum % DEF_BLK_LEN != 0 ) || ( ( sum / DEF_BLK_LEN ) > DEF_MAX_BLK_NUM ) ) //����ڵ�ǰ���õ��ڴ���Ĭ�ϳ��ȣ���װ���ڴ����ĸ����������Ĭ���ڴ�����
    {
        this->m_blkNum = DEF_MAX_BLK_NUM;//�ڴ�����������ΪĬ�ϸ���

        if ( sum % DEF_MAX_BLK_NUM == 0 )
        {
            this->m_blkLen = sum / DEF_MAX_BLK_NUM;    //����������
        }
        else
        {
            this->m_blkLen = sum / ( DEF_MAX_BLK_NUM - 1 );    //����������
        }
    }
    else
    {
        if ( sum % DEF_BLK_LEN == 0 )
        {
            this->m_blkNum = sum / DEF_BLK_LEN;
        }
        else
        {
            this->m_blkNum = sum / DEF_BLK_LEN + 1;    //���������ֱ������ڴ����
        }
    }

    // ...
    m_blkMem_pp = new union usrData*[m_blkNum];

    if ( m_blkMem_pp == NULL )
    {
        return ;
    }

    for ( short i = 0; i < m_blkNum; i++ )
    {
        m_blkMem_pp[i] = new usrData[m_blkLen];//�����ڴ����������ʼ��
        memset ( m_blkMem_pp[i], 0, sizeof ( usrData ) *m_blkLen );
    }
}

C_DStorage::~C_DStorage()
{
    delete []m_addrIdx;

    //freeQLock(this->memKey);
    for ( short i = 0; i < m_blkNum; i++ )
    {
        for ( short j = 0; j < m_blkLen; j++ )
        {
            if ( m_usrType == 0 )
            {
                if ( ( m_blkMem_pp[i] + j )->gData.tmStmp_p != NULL )
                {
                    delete [] (char*)( m_blkMem_pp[i] + j )->gData.tmStmp_p;
                }
            }

            if ( m_usrType == 1 )
            {
                if ( ( m_blkMem_pp[i] + j )->fData.tmStmp_p != NULL )
                {
                    delete [] (char*)( m_blkMem_pp[i] + j )->fData.tmStmp_p;
                }
            }
        }

        delete []m_blkMem_pp[i];
    }
}

int C_DStorage::loadStnData ( T_snnPara* ssnPara_p, u_short ucMask ) //��ȡ
{
    //lockQ(this->memKey);
    if ( m_usrType == 0 )
    {
        if ( ssnPara_p == NULL || ssnPara_p->usrData_p == NULL /*|| ssnPara_p->usrData_p->gData.tmStmp_p == NULL*/ ) //�ж����е�ָ���ǲ���Ϊ�գ�����Ƿ���-1
        {
            //unlockQ(this->memKey);
            return -1;
        }

        for ( int pnt = 0; pnt < ssnPara_p->pntNum; pnt++ )
        {
            T_snnUsrData* usrData_p = &ssnPara_p->usrData_p[pnt];
            u_int addr1 = ssnPara_p->usrData_p[pnt].gData.val_chkStd.pntAddr;//�ҵ�addrֵ
            u_int addrProcess = 0;

            u_int i;

            for ( i = 0; i < ( u_int ) m_arLen; i++ )
            {
                if ( addr1 >= ( m_addrIdx + i )->begAddr && addr1 <= ( m_addrIdx + i )->endAddr )
                {
                    addrProcess += addr1 - ( m_addrIdx + i )->begAddr;
                    break;
                }
                else
                {
                    addrProcess = ( m_addrIdx + i )->endAddr - ( m_addrIdx + i )->begAddr + 1;
                }
            }

            if ( i ==  ( u_int ) m_arLen )
            {
                //unlockQ(this->memKey);
                return -1;//addrδ�ҵ�
            }

            u_short mem = ( addrProcess - 1 ) / m_blkLen; //�ҳ��ڵڼ�����ڴ�
            int memst = addrProcess - mem * m_blkLen - 1; //�ҳ��ڸ��ڴ��еĵڼ�����

            u_short tUcMask;
            //i = 4;
            //          do
            //          {
            tUcMask = ucMask >> i;

            //              ucMask = ucMask >> i;
            //              i++;
            //              tUcMask = ucMask & 0x01;
            if ( ( tUcMask & 1 ) != 0 ) //��ucMark��һλΪ1����������ʱ��ֵvalue
            {
                //  memcpy(&(ssnPara_p->usrData_p->gData.val_chkStd) , &((m_blkMem_pp[mem]+memst)->gData.val_chkStd) , sizeof(T_gVal_ChkStd));
                usrData_p->gData.val_chkStd.pntVal1 = ( m_blkMem_pp[mem] + memst )->gData.val_chkStd.pntVal1;
                usrData_p->gData.val_chkStd.pntVal2 = ( m_blkMem_pp[mem] + memst )->gData.val_chkStd.pntVal2;
                //
                * ( char* ) ( & ( usrData_p->gData.dDsc ) ) = * ( char* ) ( & ( ( m_blkMem_pp[mem] + memst )->gData.dDsc ) );
            }

            if ( ( tUcMask & 2 ) != 0 ) //��ucMark�ڶ�λΪ1����������ʱ��ֵqds
            {
                if ( ( m_blkMem_pp[mem] + memst )->gData.tmStmp_p ) //��Ҫ�ж�void*�Ƿ�Ϊ��
                {
                    memcpy ( usrData_p->gData.tmStmp_p, ( m_blkMem_pp[mem] + memst )->gData.tmStmp_p, ( m_blkMem_pp[mem] + memst )->gData.tmStmpType );
                    usrData_p->gData.tmStmpType = ( m_blkMem_pp[mem] + memst )->gData.tmStmpType;
                }
            }

            //} while (ucMask);
        } // ! for

    }

    if ( m_usrType == 1 )
    {
        if ( ssnPara_p == NULL || ssnPara_p->usrData_p == NULL /*|| ssnPara_p->usrData_p->fData.tmStmp_p == NULL*/ ) //�ж����е�ָ���ǲ���Ϊ�գ�����Ƿ���-1
        {
            //unlockQ(this->memKey);
            return -1;
        }

        u_int addr1 = ssnPara_p->usrData_p->fData.val_chkStd.pntAddr;//�ҵ�addrֵ
        u_int addrProcess = 0;
        u_int i;

        for ( i = 0; i < ( u_int ) m_arLen; i++ )
        {
            if ( addr1 >= ( m_addrIdx + i )->begAddr && addr1 <= ( m_addrIdx + i )->endAddr )
            {
                addrProcess += addr1 - ( m_addrIdx + i )->begAddr;
                break;
            }
            else
            {
                addrProcess = ( m_addrIdx + i )->endAddr - ( m_addrIdx + i )->begAddr + 1;
            }
        }

        if ( i ==  ( u_int ) m_arLen )
        {
            //unlockQ(this->memKey);
            return -1;//addrδ�ҵ�
        }

        u_short mem = ( addrProcess - 1 ) / m_blkLen; //�ҳ��ڵڼ�����ڴ�
        int memst = addrProcess - mem * m_blkLen - 1;//�ҳ��ڸ��ڴ��еĵڼ�����

        u_short tUcMask;
        i = 3;

        do
        {
            ucMask = ucMask >> i;
            i++;
            tUcMask = ucMask & 0x01;

            if ( ( tUcMask & 1 ) != 0 ) //��ucMark��һλΪ1����������ʱ��ֵvalue
            {
                memcpy ( & ( ssnPara_p->usrData_p->fData.val_chkStd ), & ( ( m_blkMem_pp[mem] + memst )->fData.val_chkStd ), sizeof ( T_gVal_ChkStd ) );
            }

            if ( ( tUcMask & 2 ) != 0 ) //��ucMark�ڶ�λΪ1����������ʱ��ֵqds
            {
                * ( char* ) ( & ( ssnPara_p->usrData_p->fData.dDsc ) ) = * ( char* ) ( & ( ( m_blkMem_pp[mem] + memst )->fData.dDsc ) );
            }

            if ( ( tUcMask & 4 ) != 0 ) //��ucMark����λΪ1����������ʱ��ֵʱ��
            {
                if ( ( m_blkMem_pp[mem] + memst )->fData.tmStmp_p ) //��Ҫ�ж�void*�Ƿ�Ϊ��
                {
                    memcpy ( ssnPara_p->usrData_p->fData.tmStmp_p, ( m_blkMem_pp[mem] + memst )->fData.tmStmp_p, ( m_blkMem_pp[mem] + memst )->fData.tmStmpType );
                    ssnPara_p->usrData_p->fData.tmStmpType = ( m_blkMem_pp[mem] + memst )->fData.tmStmpType;
                }
            }
        } while ( ucMask );
    }

    //unlockQ(this->memKey);
    //ȡ����ϣ�
    return 0;
}

int C_DStorage::storeStnData ( T_snnPara* ssnPara_p, u_short ucMask /*= 255*/ ) //д��
{
    //lockQ(this->memKey);
    if ( m_usrType == 0 )
    {
        if ( ssnPara_p == NULL || ssnPara_p->usrData_p == NULL )
        {
            //unlockQ(this->memKey);
            return -1;
        }

        int iBeg = ssnPara_p->pntPos;
        int iEnd = ssnPara_p->pntNum;

        for ( int pnt = iBeg; pnt < iEnd; pnt++ )
        {
            if ( ( ssnPara_p->usrData_p->gData.tmStmp_p == NULL && ssnPara_p->tmStmp_p == NULL ) && ( ucMask & 4 ) != 0 )
            {
                //unlockQ(this->memKey);
                return -1;
            }

            T_snnUsrData* usrData_p = &ssnPara_p->usrData_p[pnt];
            u_int addr1 = ssnPara_p->usrData_p[pnt].gData.val_chkStd.pntAddr;//�ҵ�addrֵ

            //          printf("--storeStnData:: wait to save add=%d --\r\n", addr1);

            u_int addrProcess = 1;
            u_int i;

            for ( i = 0; i < ( u_int ) m_arLen; i++ )
            {

                if ( addr1 >= ( m_addrIdx + i )->begAddr && addr1 <= ( m_addrIdx + i )->endAddr )
                {
                    addrProcess += addr1 - ( m_addrIdx + i )->begAddr;
                    break;
                }
                else
                {
                    addrProcess = ( m_addrIdx + i )->endAddr - ( m_addrIdx + i )->begAddr + 1;
                }
            }

            if ( i ==  ( u_int ) m_arLen )
            {
                //unlockQ(this->memKey);
                return -1;//addrδ�ҵ�
            }

            u_short mem = ( addrProcess - 1 ) / m_blkLen; //�ҳ��ڵڼ�����ڴ�
            int memst = addrProcess - mem * m_blkLen - 1;//�ҳ��ڸ��ڴ��еĵڼ�����

            u_short tUcMask;
            i = 4;
            //          do
            //          {
            tUcMask = ucMask >> i;

            //              ucMask = ucMask >> i;
            //              i++;
            //              tUcMask = ucMask & 0x01;
            if ( ( tUcMask & 1 ) != 0 ) //��ucMark��һλΪ1����������ʱ��ֵvalue
            {
                memcpy ( & ( ( m_blkMem_pp[mem] + memst )->gData.val_chkStd ), & ( usrData_p->gData.val_chkStd ), sizeof ( T_gVal_ChkStd ) );

                * ( char* ) ( & ( m_blkMem_pp[mem] + memst )->gData.dDsc ) = * ( char* ) ( & ( usrData_p->gData.dDsc ) );
            }

            if ( ( tUcMask & 2 ) != 0 ) //��ucMark�ڶ�λΪ1����������ʱ��ֵqds
            {
                if ( ( m_blkMem_pp[mem] + memst )->gData.tmStmp_p == NULL ) //�ж��û�ʱ���Ƿ�Ϊ�գ�����ǵĻ�����Ҫ���ڴ�
                {
                    ( m_blkMem_pp[mem] + memst )->gData.tmStmp_p = new char [ ( m_blkMem_pp[mem] + memst )->gData.tmStmpType];
                }

                if ( usrData_p->gData.tmStmp_p != NULL && usrData_p->gData.tmStmpType != 0 ) //������û�ʱ�꣬д���û�ʱ��
                {
                    ( m_blkMem_pp[mem] + memst )->gData.tmStmpType = usrData_p->gData.tmStmpType;
                    memcpy ( ( m_blkMem_pp[mem] + memst )->gData.tmStmp_p, usrData_p->gData.tmStmp_p, ( m_blkMem_pp[mem] + memst )->gData.tmStmpType );
                }
                else if ( ssnPara_p->tmStmp_p != NULL && ssnPara_p->tmStmpType != 0 ) //û���û�ʱ�꣬д�빫��ʱ��
                {
                    ( m_blkMem_pp[mem] + memst )->gData.tmStmpType = ssnPara_p->tmStmpType;
                    memcpy ( ( m_blkMem_pp[mem] + memst )->gData.tmStmp_p, ssnPara_p->tmStmp_p, ( m_blkMem_pp[mem] + memst )->gData.tmStmpType );
                }
            }

            /*          } while ( ucMask );*/

        } // ! for

    } // ! if

    if ( m_usrType == 1 )
    {
        if ( ssnPara_p == NULL || ssnPara_p->usrData_p == NULL )
            //unlockQ(this->memKey);
        {
            return -1;
        }

        if ( ( ssnPara_p->usrData_p->fData.tmStmp_p == NULL && ssnPara_p->tmStmp_p == NULL ) && ( ucMask & 4 ) != 0 )
            //unlockQ(this->memKey);
        {
            return -1;
        }

        u_int addr1 = ssnPara_p->usrData_p->fData.val_chkStd.pntAddr;//�ҵ�addrֵ
        u_int addrProcess = 0;
        u_int i;

        for ( i = 0; i < ( u_int ) m_arLen; i++ )
        {
            addrProcess = ( m_addrIdx + i )->endAddr - ( m_addrIdx + i )->begAddr + 1;

            if ( addr1 >= ( m_addrIdx + i )->begAddr && addr1 <= ( m_addrIdx + i )->endAddr )
            {
                addrProcess += addr1 - ( m_addrIdx + i )->begAddr;
                break;
            }
            else
            {
                addrProcess = ( m_addrIdx + i )->endAddr - ( m_addrIdx + i )->begAddr + 1;
            }
        }

        if ( i ==  ( u_int ) m_arLen )
        {
            //unlockQ(this->memKey);
            return -1;//addrδ�ҵ�
        }

        u_short mem = ( addrProcess - 1 ) / m_blkLen; //�ҳ��ڵڼ�����ڴ�
        int memst = addrProcess - mem * m_blkLen - 1; //�ҳ��ڸ��ڴ��еĵڼ�����

        u_short tUcMask;
        i = 3;

        do
        {
            ucMask = ucMask >> i;
            i++;
            tUcMask = ucMask & 0x01;

            if ( ( tUcMask & 1 ) != 0 ) //��ucMark��һλΪ1����������ʱ��ֵvalue
            {
                memcpy ( & ( ( m_blkMem_pp[mem] + memst )->fData.val_chkStd ), & ( ssnPara_p->usrData_p->fData.val_chkStd ), sizeof ( T_gVal_ChkStd ) );
            }

            if ( ( tUcMask & 2 ) != 0 ) //��ucMark�ڶ�λΪ1����������ʱ��ֵqds
            {
                * ( char* ) ( & ( m_blkMem_pp[mem] + memst )->fData.dDsc ) = * ( char* ) ( & ( ssnPara_p->usrData_p->fData.dDsc ) );
            }

            if ( ( tUcMask & 4 ) != 0 ) //��ucMark����λΪ1����������ʱ��ֵʱ��
            {
                if ( ( m_blkMem_pp[mem] + memst )->fData.tmStmp_p == NULL ) //�ж��û�ʱ���Ƿ�Ϊ�գ�����ǵĻ�����Ҫ���ڴ�
                {
                    ( m_blkMem_pp[mem] + memst )->fData.tmStmp_p = new char [ ( m_blkMem_pp[mem] + memst )->fData.tmStmpType];
                }

                if ( ssnPara_p->usrData_p->fData.tmStmp_p != NULL && ssnPara_p->usrData_p->fData.tmStmpType != 0 ) //������û�ʱ�꣬д���û�ʱ��
                {
                    ( m_blkMem_pp[mem] + memst )->fData.tmStmpType = ssnPara_p->usrData_p->fData.tmStmpType;
                    memcpy ( ( m_blkMem_pp[mem] + memst )->fData.tmStmp_p, ssnPara_p->usrData_p->fData.tmStmp_p, ( m_blkMem_pp[mem] + memst )->fData.tmStmpType );
                }
                else if ( ssnPara_p->tmStmp_p != NULL && ssnPara_p->tmStmpType != 0 ) //û���û�ʱ�꣬д�빫��ʱ��
                {
                    ( m_blkMem_pp[mem] + memst )->fData.tmStmpType = ssnPara_p->tmStmpType;
                    memcpy ( ( m_blkMem_pp[mem] + memst )->fData.tmStmp_p, ssnPara_p->tmStmp_p, ( m_blkMem_pp[mem] + memst )->fData.tmStmpType );
                }
            }
        } while ( ucMask );
    }

    //unlockQ(this->memKey);
    return 0;
}
int C_DStorage::getData ( union usrData* blkMem_p, u_int addr )
{
    //lockQ(this->memKey);
    u_int addrProcess = 0;
    u_int i;

    for ( i = 0; i < ( u_int ) m_arLen; i++ )
    {
        addrProcess = ( m_addrIdx + i )->endAddr - ( m_addrIdx + i )->begAddr + 1;

        if ( addr >= ( m_addrIdx + i )->begAddr && addr <= ( m_addrIdx + i )->endAddr )
        {
            addrProcess += addr - ( m_addrIdx + i )->begAddr;
            break;
        }
        else
        {
            addrProcess = ( m_addrIdx + i )->endAddr - ( m_addrIdx + i )->begAddr + 1;
        }
    }

    if ( i ==  ( u_int ) m_arLen )
    {
        // unlockQ(this->memKey);
        return -1;//addrδ�ҵ�
    }

    u_short mem = ( addrProcess - 1 ) / m_blkLen; //�ҳ��ڵڼ�����ڴ�
    int memst = addrProcess - mem * m_blkLen - 1; //�ҳ��ڸ��ڴ��еĵڼ�����
    memcpy ( blkMem_p, m_blkMem_pp[mem] + memst, sizeof ( union usrData ) );
    //unlockQ(this->memKey);
    return 0;
}
extern C_SsnCtnrObj ssnCtnr;    // �Ự����
//
// extern  int finishBlock(ssnCtlBlk* ssnCtlBlk_p, HANDLE trdID, u_short ssnID,u_short chnlNo,u_int errCd);
// void ssnTmOutDftHdl(void* tmoutInfo_p)
// {
//  ssnCtlBlk* tSsnCtlBlk_p;
//  C_SsnObj* ssnObj_p;
//  u_int msgBdy[2];
//
//  tSsnCtlBlk_p = (ssnCtlBlk*)tmoutInfo_p;
//
//  int iRet = 0;
//  if ( tSsnCtlBlk_p )
//  {
//      iRet = finishBlock(tSsnCtlBlk_p, tSsnCtlBlk_p->hostTID, tSsnCtlBlk_p->ssnNo, tSsnCtlBlk_p->chnlID, 0x020000);
//  }
//  else
//  {
//      iRet = finishBlock(tSsnCtlBlk_p, 0, 0, 0, 0x020000);
//  }
//
//  return ;
//
//  if(!tSsnCtlBlk_p)
//  {
//      msgBdy[0]= 0/*tSsnCtlBlk_p->chnlID|(tSsnCtlBlk_p->ssnNo<<16)*/;
//      msgBdy[1]= 0x020000;
//      ssnFnshNfy(tSsnCtlBlk_p->hostTID, (char*)&msgBdy, 2*sizeof(u_int));
//          return ;
//  }
//
//  ssnObj_p = ssnMgr.getSsnObj(tSsnCtlBlk_p->ssnNo);
//  if ( ssnObj_p == NULL )
//  {
//
//      return ;
//  }
//  /////////////////un-set ssn timer, or ssn timeout =0
//  //active ssnInst or passive ssnInst.
//  C_ChnlObj *tChnlObj_p = chnlMgr.getChnlObj(tSsnCtlBlk_p->chnlID);
//  if(!tSsnCtlBlk_p->ssnTmHead_p)
//  {
//      if(!tSsnCtlBlk_p->actFlg)//act ssnInst, free resource..
//      {
//          T_freeRl_P tFreeRl_p = (T_freeRl_P)GetProcAddress(*(tChnlObj_p->getPtclMdl()), "freeRl_P");
//          if ( tFreeRl_p )
//          {
//              (tFreeRl_p)(tSsnCtlBlk_p->myChnlInfoBlk_p->funRl_p);
//              tSsnCtlBlk_p->myChnlInfoBlk_p->funRl_p = NULL;
//          }
//          ssnObj_p->delActiveSsn(tSsnCtlBlk_p);
//      }
//
//      //notify to inner-thread ( system thread)!
//      //un-implement, temporarily
//      //..
//      return ;
//  }
//  /////////////////timeout >0: act ssnInst or passive ssnInst
//  // 1. stop app tmr.
//  u_short tTmChnlID;
//  u_char tTmrNum;
//  tTmChnlID = tmFclty.getTmChnlIDByPtr(tSsnCtlBlk_p->ssnTmHead_p);
//  tTmrNum = tmFclty.getTmChnlPara(tTmChnlID, 1);
//  for(u_char i = 0;i<tTmrNum;i++)
//  {
//      tmFclty.ctlTmr(tSsnCtlBlk_p->ssnTmHead_p, i, 5);
//      if(!tSsnCtlBlk_p->actFlg)//act ssn Inst
//          tmFclty.delTmr(tTmChnlID, i);
//  }
//
//  /////////////////timeout >0: act ssnInst
//  if(!tSsnCtlBlk_p->actFlg)//act ssnInst
//  {
//      tmFclty.delTmChnlByID(tTmChnlID);
//
//      // 2. prepare usr info.
//      msgBdy[0]= tSsnCtlBlk_p->chnlID|(tSsnCtlBlk_p->ssnNo<<16);
//      msgBdy[1]=  0x020000;
//      T_TID hostTID = tSsnCtlBlk_p->hostTID;
//
//      // 3. clear ssnInst & other resource...
//      // del ssnCtlBlk from chnl ... and other...
//      if ( tChnlObj_p != NULL )
//      {
//          short ssnPrio = ssnMgr.getSsnPrio(tSsnCtlBlk_p->ssnNo);
//          tChnlObj_p->delSsn(tSsnCtlBlk_p, ssnPrio);
//          ssnCtnr.delSsnObj(ssnPrio, tSsnCtlBlk_p);
//      }
//      int role = tSsnCtlBlk_p->role;
//
//      //
//      T_freeRl_P tFreeRl_p = (T_freeRl_P)GetProcAddress(*(tChnlObj_p->getPtclMdl()), "freeRl_P");
//      if ( tFreeRl_p )
//      {
//          (tFreeRl_p)(tSsnCtlBlk_p->myChnlInfoBlk_p->funRl_p);
//          tSsnCtlBlk_p->myChnlInfoBlk_p->funRl_p = NULL;
//      }
//
//      // del ssnCtlBlk
//      ssnObj_p->delActiveSsn(tSsnCtlBlk_p);
//      tSsnCtlBlk_p = NULL;
//
//      // 4. notify to usr
//      ssnFnshNfy(hostTID,(char*)&msgBdy, 2*sizeof(u_int));
//
//      int tid = GetThreadId(hostTID);
//      printf("ssnTmOutDftHdl:: tid=%d, ssnid=%d, chnl=%d, ssnFnshNfy !\r\n", tid, (short)(msgBdy[0]>>16), (short)msgBdy[0]);
//      return ;
//  }
//
//  /////////////////timeout >0: passive ssnInst
//  //notify to inner-thread ( system thread)!
//  //un-implement, temporarily
//  //..
//  return ;
// }


