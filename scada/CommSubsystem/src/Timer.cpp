//��ʱ��ʵ���ļ�
//
//˵����1.֧�ֶ�ʱͨ������ʱ���飩���ڲ����䶨ʱͨ���ţ���ʱͨ��ID���������ظ������ߣ���Ϊ
//        �û�����ʱͨ��Ψһ��ʶ��ÿ������Ķ�ʱ����������ɶ��ƣ�����ʱ�����޸ġ�
//      2.���ж�ʱ������Ψһ�����ض���ʱͨ�������붨ʱ�������ڴ��ڶ�ʱͨ��������½��У���
//        ��ָ���Ķ�ʱͨ���ڲ���������Ķ�ʱ�������ж�ʱ����������Ψһ��ţ�ID����ʶ���ɵ�
//		  ���߸�����;���������෨Ϊ���ţ���ͬ��Ŷ�ʱ���������뵽ͬһ��ʱͨ���ڡ�
//      3.��ʱͨ���Ͷ�ʱ�������루��ӣ����̰߳�ȫ��֤����ʱ�����ƺͷ��ʾ��ṩ�̰߳�ȫ��֤��
//      4.��ʱ�����ư�״̬����ʽִ��,һ���Զ�ʱ�����ڴ�����תΪֹͣ״̬�������Զ�ʱ��������
//        �ٴν�������״̬��
//        (ȡ��)δ��װ<-->��װ-->����-->��ͣ<-->����
//				  |\			  |\      |     /
//				    \	 	        \     |    /
//                   \               \|  \|/ |/
//		              ͣ                     ֹ
//
//��λ���׷����Ϸ��ܹ���(�Ϻ�)���޹�˾
//��д�ˣ��Խ��
//��дʱ�䣺2016��12��
//

#include "Timer.h"



/************************************************************/
/*				        ƽ̨��ز���    	                */
/************************************************************/
//
#ifdef WINDOWS_PLATFORM

#define clrCfgTmFclty(tmFclty_p)	(clrCfgTmFclty_WIN64(tmFclty_p))
#define cfgTmFclty(tmFclty_p)		(cfgTmFclty_WIN64(tmFclty_p))
#define crtTmLock(tmKey)		(InitializeCriticalSection(&tmKey))
#define freeTmLock(tmKey)		(DeleteCriticalSection(&tmKey))
#define lockTm(tmKey)			(EnterCriticalSection(&tmKey))
#define unlockTm(tmKey)		(LeaveCriticalSection(&tmKey))
//
//
////////////////windows 64λ���ڻص�wrapper
////
//���ܣ���װΪwindows��ý�嶨ʱ���ص������׼��ʽ
//���룺����windows��ý�嶨ʱ���ص�������ʽ
//�������
//���أ���
//
void CALLBACK tmOutHdl_WIN64( UINT uID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2 )
{
    return ((C_TmObj *)dwUser)->timeOut();
}

//
//
////////////////windows 64��ʱ�����ú���
////
//���ܣ����Ʋ���ϵͳ�ṩ�Ķ�ý�嶨ʱ�����ܣ�ʵ�ֶ�ʱ��Ӧ�õĳ�ʼ������
//���룺
//		tmFacility_p����ʱ������ָ��
//�������
//���أ�ִ��״̬��=0,�ɹ�
//                =-1����������Ƿ���-10����ȡ������ϵͳ��ʱ���ֱ�����ʧ�ܣ�
//                =-11������ϵͳ��ʱ��������Ӧ�¼�ʧ��
//
static int cfgTmFclty_WIN64(C_TmObj* tmFclty_p)
{
    TIMECAPS tc;

    if(!tmFclty_p)
        return -1;
    tmFclty_p->m_tmrRes = MC_TRICK_TIME;//����ʱ����
    if(timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR)
        return -10;
    else
    {

        tmFclty_p->m_accu = min(max(tc.wPeriodMin, MC_TRICK_TIME), tc.wPeriodMax); //�жϷֱ����Ƿ�������Χ
        timeBeginPeriod(tmFclty_p->m_accu);  //���ö�ʱ���ֱ���
    }     
    //���ö�ʱ���ص��¼����ص���������fun
    if(!(tmFclty_p->m_clkSrcID = timeSetEvent(tmFclty_p->m_tmrRes, tmFclty_p->m_accu, (LPTIMECALLBACK)tmOutHdl_WIN64, (DWORD)tmFclty_p, TIME_PERIODIC)))
    {
        tmFclty_p->m_tmrRes = 0;
        tmFclty_p->m_accu = 0;
        return -11;
    }
    return 0;
}

//
//
////////////////windows 64ȡ����ʱ������
////
//���ܣ�ȡ������ϵͳ�ṩ�Ķ�ý�嶨ʱ�����ܣ�ֹͣ��ý�嶨ʱ�����ͷ���Դ
//���룺
//		tmFacility_p����ʱ������ָ��
//�������
//���أ���
//
static void clrCfgTmFclty_WIN64(C_TmObj* tmFclty_p)
{
    if(!tmFclty_p)
        return;

    //�رն�ʱ���¼�
    timeKillEvent(tmFclty_p->m_clkSrcID);
    tmFclty_p->m_clkSrcID = 0;
    //�����С��ʱ������
    timeEndPeriod(tmFclty_p->m_tmrRes);
    tmFclty_p->m_tmrRes = 0;
    tmFclty_p->m_accu= 0;
    return;
}
#else

#define clrCfgTmFclty(tmFclty_p)	(clrCfgTmFclty_LNX64(tmFclty_p))
#define cfgTmFclty(tmFclty_p)		(cfgTmFclty_LNX64(tmFclty_p))
#define crtTmLock(tmKey)		(pthread_mutex_init(&tmKey,NULL))
#define freeTmLock(tmKey)		(pthread_mutex_destroy(&tmKey))
#define lockTm(tmKey)			(pthread_mutex_lock(&tmKey))
#define unlockTm(tmKey)		(pthread_mutex_unlock(&tmKey))

extern C_TmObj		tmFclty;
typedef void (*tmOutHdl_LNX64)(int);
//
//
////////////////Linux 64λ���ڻص�wrapper
////
//���ܣ���װΪlinux itimer�ص������׼��ʽ
//���룺����linuxitimer�ص������׼��ʽ
//�������
//���أ���
//
void tmOutHdl_LNX64_Fun( int tmFclty_p)
{

  //  CRITICAL_SECTION * key = &(((C_TmObj*)tmFclty_p)->tmKey);
    //((C_TmObj*)tmFclty_p)->timeOut();
    tmFclty.timeOut();
    return;
}

//
//
////////////////Linux 64��ʱ�����ú���
////
//���ܣ����Ʋ���ϵͳ�ṩ�Ķ�ý�嶨ʱ�����ܣ�ʵ�ֶ�ʱ��Ӧ�õĳ�ʼ������
//���룺
//		tmFacility_p����ʱ������ָ��
//�������
//���أ�ִ��״̬��=0,�ɹ�
//                =-1����������Ƿ���-10����ȡ������ϵͳ��ʱ���ֱ�����ʧ�ܣ�
//                =-11������ϵͳ��ʱ��������Ӧ�¼�ʧ��
//
static int cfgTmFclty_LNX64(C_TmObj* tmFclty_p)
{
    tmOutHdl_LNX64 p_signal = tmOutHdl_LNX64_Fun;
    if(signal(SIGALRM, p_signal)==SIG_ERR)
    {
        return -11;
    }
    struct itimerval tick;
    memset(&tick , 0 , sizeof(tick));
    tick.it_value.tv_sec = 1;
    tick.it_value.tv_usec = 0;//��Ϊ0���ź��ƺ�������źŵ�������
    tick.it_interval.tv_sec  = 1;
    tick.it_interval.tv_usec = 0;
    if(!tmFclty_p)
        return -1;
    if(setitimer(ITIMER_REAL, &tick, NULL)==-1)
    {
        //printf("setitimer err %d\n",errno);
        return -10;
    }

    tmFclty_p->m_tmrRes = MC_TRICK_TIME;
    tmFclty_p->m_clkSrcID = 1;
    tmFclty_p->m_accu = 1;
    return 0;
}

//
//
////////////////windows 64ȡ����ʱ������
////
//���ܣ�ȡ������ϵͳ�ṩ��itimer��ʱ�����ܣ�ֹͣ��ʱ�����ͷ���Դ
//���룺
//		tmFacility_p����ʱ������ָ��
//�������
//���أ���
//
static void clrCfgTmFclty_LNX64(C_TmObj* tmFclty_p)
{
    struct itimerval tick;
    tick.it_value.tv_sec = 0;
    tick.it_value.tv_usec = 0;
    tick.it_interval.tv_sec  = 0;
    tick.it_interval.tv_usec = 0;

    setitimer(ITIMER_REAL, &tick, NULL);
    tmFclty_p->m_clkSrcID = 0;
    tmFclty_p->m_tmrRes = 0;
    tmFclty_p->m_accu= 0;
    return;
}
#endif

/************************************************************/
/*				        ƽ̨�޹ز���    	                */
/************************************************************/

//
//
////////////////��ʱ��ת������״̬
////
//���ܣ���ʱ���������򴥷�����״̬��ֹͣ��װת������״̬
//���룺
//		ticks����������ʱ��
//		tmrTrgrHd_pp����ʱ�����򴥷���ͷ��ַ��ָ��
//		tmr_p����Ҫ�����Ķ�ʱ��ָ��
//�������
//���أ���
//


static void tnToStart(u_int ticks, tmr** tmrTrgrHd_pp, tmr* tmr_p)
{
    tmr *tTmr_p, *tTmrProbe_p, *tTmrPos_p;

    tTmr_p = tmr_p;

    //set start and expire time
    tTmr_p->startTm = ticks;
    tTmr_p->xpirTm = tTmr_p->startTm + tTmr_p->xpirTm;//update to global time

    tTmrProbe_p = *tmrTrgrHd_pp;
    tTmrPos_p = tTmrProbe_p;
    while(tTmrProbe_p)
    {
        if(tTmrProbe_p->xpirTm <=  tTmr_p->xpirTm)
        {
            tTmrPos_p = tTmrProbe_p;
            tTmrProbe_p = tTmrProbe_p->sortTrgrNxt_p;
        }
        else
            break;
    }

    if(!(*tmrTrgrHd_pp))//empty triger chain
        *tmrTrgrHd_pp = tmr_p;
    else if(tTmrProbe_p == *tmrTrgrHd_pp)//insert head, large equal 1
    {
        tTmr_p->sortTrgrNxt_p = tTmrProbe_p;
        *tmrTrgrHd_pp = tTmr_p;
        tTmrProbe_p->sortTrgrPre_p = tTmr_p;
        tTmr_p->sortTrgrPre_p = NULL/**tmrTrgrHd_pp*/;
    }
    else if(!tTmrProbe_p)//insert tail 
    {
        tTmr_p->sortTrgrNxt_p = NULL;
        tTmr_p->sortTrgrPre_p = tTmrPos_p;
        tTmrPos_p->sortTrgrNxt_p = tTmr_p;
    }
    else//insert middle
    {
        tTmr_p->sortTrgrNxt_p = tTmrProbe_p;
        tTmr_p->sortTrgrPre_p = tTmrPos_p;
        tTmrPos_p->sortTrgrNxt_p = tTmr_p;
        tTmrProbe_p->sortTrgrPre_p = tTmr_p;
    }
    return ;
}

//
//
////////////////��ʱ��ת��ֹͣ״̬
////
//���ܣ���ʱ�������������״̬ת��ֹͣ״̬����������򴥷�����ժ��
//���룺
//		ticks����������ʱ��
//		tmrTrgrHd_pp����ʱ�����򴥷���ͷ��ַ��ָ��
//		tmr_p����Ҫ�����Ķ�ʱ��ָ��
//�������
//���أ���
//
static void tnToStop(tmr** tmrTrgrHd_pp, tmr* tmr_p)
{
    if(tmr_p/*->sortTrgrPre_p*/ == *tmrTrgrHd_pp)//pick off chain head
    {
        *tmrTrgrHd_pp = tmr_p->sortTrgrNxt_p;
        if(tmr_p->sortTrgrNxt_p)//more than 1
            tmr_p->sortTrgrNxt_p->sortTrgrPre_p = NULL/**tmrTrgrHd_pp*/;
    }
    else if(!tmr_p->sortTrgrNxt_p)//chain tail but not head
    {

        //	if (!tmr_p->sortTrgrPre_p)
        //	{
        tmr_p->sortTrgrPre_p->sortTrgrNxt_p = NULL;
        //	}
    }
    else//pick off other
    {
        tmr_p->sortTrgrPre_p->sortTrgrNxt_p = tmr_p->sortTrgrNxt_p;
        tmr_p->sortTrgrNxt_p->sortTrgrPre_p = tmr_p->sortTrgrPre_p;
    }

    tmr_p->sortTrgrNxt_p = NULL;
    tmr_p->sortTrgrPre_p = NULL;
    tmr_p->xpirTm = tmr_p->xpirTm - tmr_p->startTm;//restore to relative start time
    tmr_p->startTm = 0;
}

//
//
////////////////���캯��
////
//���ܣ���ʼ������ȡ����ʱ�䣬��ת��Ϊ�ļ�ʱ�䣬������������Ԥ����ն�ʱͨ���ۣ�
//      ����ϵͳ��ʱ����Դ
//���룺��
//�������
//���أ���
//
C_TmObj::C_TmObj()
{
    m_clkSrc.clkCntr = 0;
#ifdef  WINDOWS_PLATFORM              //ϵͳ�ⲿ���ã������ʱ���ã�ע�͵�
    SYSTEMTIME stLocal;
    ::GetLocalTime(&stLocal);
    ::SystemTimeToFileTime(&stLocal, &m_clkSrc.locFileTm);
#endif

    m_tmTrgrHd_p = NULL;
    m_tmChnlTblHd_p = NULL;
    m_clkSrcID = 0;
    m_tmrRes = 0;
    m_accu = 0;
    crtTmLock(tmKey);

    //Ԥ����MC_ALLOC_CHNL_NUM����ʱͨ���ղ�
    m_tmChnlTblHd_p = (tmChnl*)calloc(MC_ALLOC_CHNL_NUM, sizeof(tmChnl));//tmChnl tbl, get a tmChnl by its index!
    if (m_tmChnlTblHd_p == NULL )
        return;
    m_tmChnlSlotTtlNum = MC_ALLOC_CHNL_NUM;
    for(int i = 0; i<m_tmChnlSlotTtlNum; i++)
    {
        m_tmChnlTblHd_p[i].tmChnlID = i;
    }

    //	memset(m_tmChnlTblHd_p, 0, MC_ALLOC_CHNL_NUM*sizeof(tmChnl));
    //	m_tmChnlSlotIdleIdx = 0;
    cfgTmFclty(this);
}

//
//
////////////////��������
////
//���ܣ��ͷŻ���������ʱͨ������ʱ���洢
//���룺��
//�������
//���أ���
//
C_TmObj::~C_TmObj()
{
    tmChnl* tTmChnl_p;
    tmr* tTmr_p;
    u_char tTmrNum;

    clrCfgTmFclty(this);

    for(u_short i=0; i<m_tmChnlSlotTtlNum; i++)
    {
        tTmChnl_p = m_tmChnlTblHd_p+i;
		//printf("~C_TmObj lock\r\n");
        lockTm(tmKey);
        tTmrNum = tTmChnl_p->tmrNum;
        tTmr_p = tTmChnl_p->tmrTblHd_p;
        while(tTmrNum--)//free name buf
        {
            if(!tTmr_p->name_p)
            {
                free(tTmr_p->name_p);
                tTmr_p->name_p = NULL;
            }
            tTmr_p++;
        }
        if(!tTmChnl_p->tmrTblHd_p)
            free(tTmChnl_p->tmrTblHd_p);
        if(!tTmChnl_p->tmFunHdl_p)
            free(tTmChnl_p->tmFunHdl_p);
        unlockTm(tmKey);
    }
    free(m_tmChnlTblHd_p);
    freeTmLock(tmKey);
}

//
//
////////////////���Ӷ�ʱͨ��
////
//���ܣ���������ϵͳ���á�����ʹ�ô��ڵĶ�ʱͨ���ղۡ��޿ղۣ���δ����ϵͳ��ʱͨ��
//      ���������������ղۡ�ȡ��һ���ղ۷����������ʱͨ�������������ʵ����������
//      ����ʱ���ղۣ�������ʱͨ������Ϊ��װ״̬
//���룺
//      tmNum��������ʱͨ�������Ķ�ʱ������
//�����
//		tChnlID��������ʱͨ���ţ�ͨ����Ŵ�0��ʼ
//���أ�ִ��״̬��=0,�ɹ�
//                =-1����������Ƿ���-2������ϵͳ��ʱͨ�������޷������µĶ�ʱͨ����
//                =-3���洢����ʧ��
//
int C_TmObj::addTmChnl( u_char tmrNum, u_short* tmChnlID)
{
    tmChnl *tNewTmChnl_p;
    u_char tTmrNum;
    tmr* tTmr_p;
    tNewTmChnl_p = m_tmChnlTblHd_p;

    if ((!tmrNum)||(tmrNum > MC_DEF_SOFT_TIME_MAX)||(!tmChnlID))
    {
        *tmChnlID = (u_short)0xFFFF;
        return -1;
    }

    //append to assign
    if(m_tmChnlSlotUsingNum == m_tmChnlSlotTtlNum)
    {
        if(m_tmChnlSlotTtlNum == MC_DEF_CHNL_MAX)//arrive to sys vol!
        {
            *tmChnlID = 0xFFFF;
            return -2;
        }

        tNewTmChnl_p = (tmChnl*)realloc(m_tmChnlTblHd_p, (m_tmChnlSlotTtlNum + MC_ALLOC_CHNL_NUM/2) * sizeof(tmChnl));//tmChnl tbl, get a tmChnl by its index!
        if(tNewTmChnl_p)
        {
			//printf("addTmChnl lock");
            lockTm(tmKey);
            m_tmChnlTblHd_p = tNewTmChnl_p;
            unlockTm(tmKey);

            tNewTmChnl_p = tNewTmChnl_p + m_tmChnlSlotTtlNum;
            memset((void*)(tNewTmChnl_p), 0, MC_ALLOC_CHNL_NUM/2);//new save space init
            for(int i = m_tmChnlSlotTtlNum; i < m_tmChnlSlotTtlNum + MC_ALLOC_CHNL_NUM/2; i++)
            {
                m_tmChnlTblHd_p[i].tmChnlID = i;
            }

            m_tmChnlSlotTtlNum = m_tmChnlSlotTtlNum + MC_ALLOC_CHNL_NUM/2;
            tNewTmChnl_p->tmrNum = tmrNum;
        }
        else
        {
            *tmChnlID = 0xFFFF;
            return -3;
        }

        //assign tmrs
        tNewTmChnl_p->tmrTblHd_p = (tmr*)calloc(tNewTmChnl_p->tmrNum, sizeof(tmr));
        tNewTmChnl_p->tmFunHdl_p = (FunTypeOne*)calloc(tNewTmChnl_p->tmrNum, sizeof(FunTypeOne));
        if(!(((u_long)tNewTmChnl_p->tmrTblHd_p)&((u_long)tNewTmChnl_p->tmFunHdl_p)))//fault!
        {
            *tmChnlID = 0xFFFF;
            return -3;
        }
        /*		tNewTmChnl_p->tmChnlID = */
        m_tmChnlSlotUsingNum++;
        tNewTmChnl_p->state = TM_INSTALL;
        *tmChnlID = tNewTmChnl_p->tmChnlID;

        memset(tNewTmChnl_p->tmrTblHd_p, 0, tNewTmChnl_p->tmrNum*sizeof(tmr));

        return 0;
    }

    //find reusable or idle slot
    if(m_tmChnlSlotUsingNum < m_tmChnlSlotTtlNum) 
    {
        //prior reuse!
        int i;
        //for(i=0;i <= m_tmChnlSlotUsingNum;i++)//reusable slot(<=) or un-reusable slot(>)
        for(i = 0; i < m_tmChnlSlotUsingNum; i++)
        {
            tNewTmChnl_p = m_tmChnlTblHd_p+i;
            if(!tNewTmChnl_p->state)//has del, reuse it.
                break;
        }

        //this is reusable slot, tNewTmChnl_p->tmChnlID and tmrNum has exist!
        if((0 == m_tmChnlSlotUsingNum) || (i < m_tmChnlSlotUsingNum))
        {
            tTmrNum = tNewTmChnl_p->tmrNum;
            tTmr_p = tNewTmChnl_p->tmrTblHd_p;
            while(tTmrNum--)//free all tmr name buf in the tmChnl
            {
                if(tTmr_p->name_p)
                {
                    free(tTmr_p->name_p);
                    tTmr_p->name_p = NULL;
                }
                tTmr_p++;
            }

            if(tNewTmChnl_p->tmrNum != tmrNum)//free tmrs then alloc tmrs
            {
                if (tNewTmChnl_p->tmrTblHd_p)
                {
                    free(tNewTmChnl_p->tmrTblHd_p);
                    //					tNewTmChnl_p->tmrTblHd_p = NULL;
                }
                if ( tNewTmChnl_p->tmFunHdl_p )
                {
                    free(tNewTmChnl_p->tmFunHdl_p);
                    //					tNewTmChnl_p->tmFunHdl_p = NULL;
                }				

                tNewTmChnl_p->tmrNum = tmrNum;
                tNewTmChnl_p->tmrTblHd_p = (tmr*)calloc(tNewTmChnl_p->tmrNum, sizeof(tmr));
                tNewTmChnl_p->tmFunHdl_p = (FunTypeOne*)calloc(tNewTmChnl_p->tmrNum, sizeof(FunTypeOne));
                if(!((u_long)(tNewTmChnl_p->tmrTblHd_p)&((u_long)tNewTmChnl_p->tmFunHdl_p)))//err
                {
                    *tmChnlID = 0xFFFF ;
                    return -3;
                }
            }
            m_tmChnlSlotUsingNum++;
            //tNewTmChnl_p->tmChnlID = m_tmChnlSlotUsingNum++;
            tNewTmChnl_p->state = TM_INSTALL;
            *tmChnlID = tNewTmChnl_p->tmChnlID;

            memset(tNewTmChnl_p->tmrTblHd_p, 0, tNewTmChnl_p->tmrNum*sizeof(tmr)); 

            return 0;
        }
        else//idle slot,>=m_tmChnlSlotUsingNum
        {
            //alloc tmrs
            tNewTmChnl_p = m_tmChnlTblHd_p+m_tmChnlSlotUsingNum;
            tNewTmChnl_p->tmrNum = tmrNum;
            tNewTmChnl_p->tmrTblHd_p = (tmr*)calloc(tNewTmChnl_p->tmrNum, sizeof(tmr));
            tNewTmChnl_p->tmFunHdl_p = (FunTypeOne*)calloc(tNewTmChnl_p->tmrNum, sizeof(FunTypeOne)); // ?? 
            if(!(((u_long)tNewTmChnl_p->tmrTblHd_p)&((u_long)tNewTmChnl_p->tmFunHdl_p)))//fault!
            {
                *tmChnlID = 0xFFFF;
                return -3;
            }
            //tNewTmChnl_p->tmChnlID = 
            m_tmChnlSlotUsingNum++;
            tNewTmChnl_p->state = TM_INSTALL;
            *tmChnlID = tNewTmChnl_p->tmChnlID;

            memset(tNewTmChnl_p->tmrTblHd_p, 0, tNewTmChnl_p->tmrNum*sizeof(tmr));

            return 0;
        }
    }
    return 0;
}

//
//
////////////////��ʱͨ�������Ӷ�ʱ��
////
//���ܣ���������ϵͳ���á���ָ����ʱͨ��������һ����ʱ�����ɹ�ʱ���ö�ʱ��Ϊ��װ״̬
//���룺
//      tChnlID����ʱͨ����
//      tmCfg_p��������ʱ��������Ϣ����������ʱ����ţ���ʱͨ���ڱ�ţ���0��ʼ������ʱ
//               �����ƣ���'\0'��ʶ������ASCII�룩����ʱ�����ͣ����ڻ�����ڣ�������ʱ
//               �䣨����ڴ˶�ʱ�������ĵ���ʱ�䣬�ڲ�����ת��Ϊ��������������ʱ�䣩��
//               ������Ӧ������ַ
//�������
//���أ�ִ��״̬��=0,�ɹ�
//                =-1����������Ƿ���=-3���洢����ʧ��
//
int C_TmObj::addTmr(u_short tmChnlID, tmCfg* tmrCfg_p)
{
    tmChnl *tNewTmChnl_p;
    tmr *tTmr_p;

    if((m_tmChnlSlotTtlNum<=tmChnlID)||(!tmrCfg_p))
        return -1;
    tNewTmChnl_p = m_tmChnlTblHd_p+tmChnlID;
    if(tmrCfg_p->ID >= tNewTmChnl_p->tmrNum)
        return -1;
    // ?? 
    tTmr_p = tNewTmChnl_p->tmrTblHd_p + tmrCfg_p->ID;//lzq,2017.1.25
    tTmr_p->ID = tmrCfg_p->ID;

    tTmr_p->name_p = (char *)malloc(strlen(tmrCfg_p->name_p)+1);
    if(!tTmr_p->name_p)
        return -3;
    strcpy(tTmr_p->name_p, tmrCfg_p->name_p);
    tTmr_p->wkState = TM_INSTALL;//install
    tTmr_p->type = tmrCfg_p->type;
    tTmr_p->startTm = 0x0;
    tTmr_p->xpirTm = tTmr_p->startTm + tmrCfg_p->fstXpirTm;//turn to global time
    tTmr_p->tmFunHdlPara_p = tmrCfg_p->tmFunHdlPara_p;
    *(tNewTmChnl_p->tmFunHdl_p + tmrCfg_p->ID) = tmrCfg_p->tmFunHdl; //
    // by zhaojj
    tTmr_p->tmChnl_p = tNewTmChnl_p;

    return 0;
}

tmChnl* C_TmObj::ctrAppTmChnl(u_short tmChnlID, tmCfg* tmrCfg_p)
{
    if ( tmrCfg_p == NULL || (m_tmChnlSlotTtlNum <= tmChnlID) )
    {
        return NULL;
    }
    tmChnl *tNewTmChnl_p = NULL;
    tmr *tTmr_p = NULL;

    tNewTmChnl_p = new tmChnl;
    if ( tNewTmChnl_p == NULL )
    {
        return NULL;
    }
    memset(tNewTmChnl_p, 0, sizeof(tmChnl));

    // 
    tTmr_p = new tmr[1];
    if ( tTmr_p == NULL )
    {
        return NULL;
    }
    memset(tTmr_p, 0, sizeof(tmr));
    //
    tNewTmChnl_p->tmrTblHd_p = tTmr_p;
    tNewTmChnl_p->tmChnlID = tmChnlID;
    tNewTmChnl_p->tmrNum = 1;
    tNewTmChnl_p->tmFunHdl_p = new FunTypeOne;

    //
    tTmr_p->ID = tmrCfg_p->ID;

    tTmr_p->name_p = new char[strlen(tmrCfg_p->name_p)+1];
    if(!tTmr_p->name_p)
        return NULL;

    strcpy(tTmr_p->name_p, tmrCfg_p->name_p);
    tTmr_p->wkState = TM_INSTALL;//install
    tTmr_p->type = tmrCfg_p->type;
    tTmr_p->startTm = 0x0;
    tTmr_p->xpirTm = tTmr_p->startTm + tmrCfg_p->fstXpirTm;//turn to global time
    tTmr_p->tmFunHdlPara_p = tmrCfg_p->tmFunHdlPara_p;
    *(tNewTmChnl_p->tmFunHdl_p + tmrCfg_p->ID) = tmrCfg_p->tmFunHdl; //

    tTmr_p->tmChnl_p = tNewTmChnl_p;

    return tNewTmChnl_p;
}
/*
// FUN:ɾ��Ӧ�ö�ʱ��
// Param:
//		tmChnlID��ͨ����
//		tmChnlH_p��Ӧ�ü���ʱ������ͷָ��
// Ret: 0���ɹ���������ʧ�ܣ�
*/ 
int C_TmObj::delAppTmChnl(u_short tmChnlID, tmChnl* tmChnlH_p)
{
    if ( tmChnlH_p == NULL )
    {
        return 0;
    }

    if ( tmChnlH_p->tmFunHdl_p != NULL )
    {
        delete tmChnlH_p->tmFunHdl_p;
        tmChnlH_p->tmFunHdl_p = NULL;
    }

    for ( int i = 0; i < tmChnlH_p->tmrNum; i++)
    {
        tmr* tmr_p = &tmChnlH_p->tmrTblHd_p[i];
        if ( tmr_p != NULL )
        {
            if ( tmr_p->name_p != NULL )
            {
                delete[] tmr_p->name_p;
                tmr_p->name_p = NULL;
            }

        }
    }	
    if ( tmChnlH_p->tmrTblHd_p != NULL)
    {
        delete []tmChnlH_p->tmrTblHd_p;
        tmChnlH_p->tmrTblHd_p = NULL;
    }
    delete tmChnlH_p;
    tmChnlH_p = NULL;

    return 0;
}
//
//
////////////////��ʱ�����ƺ���
////
//���ܣ����̵߳��á����ƶ�ʱ����������ʱ��������ת��״̬�����������ͼ���״̬�²�
//      �ᷢ�������¼���������ת������״̬ʱ���������򴥷�����ת��ֹͣ״̬ʱ����
//      ���򴥷�����ȥ����
//���룺
//      chnl_p����ʱͨ��ָ��
//      ctlCd�������롣=2������״̬��=3����ͣ״̬��=4������״̬��=5��ֹͣ״̬
//�������
//���أ�ִ��״̬��=0,�ɹ�
//                =-1����������Ƿ���=-2����ʱ��״̬����=-3���洢����ʧ��
//
int C_TmObj::ctlTmr(tmChnl* tmChnl_p, u_char tmrID, u_char ctlCd)
{
    tmr *tTmr_p;

    if ( !tmChnl_p)
        return -1;
    if(tmrID >= tmChnl_p->tmrNum)
        return -1;                                                                                                                               

    //get this tmr
	//printf("ctlTmr lock\r\n");
    lockTm(tmKey);
    tTmr_p = tmChnl_p->tmrTblHd_p + tmrID;
    switch(tTmr_p->wkState)//timeOut routine only read workState.
    {
    case TM_UNSTALL://unstall state
        {
            unlockTm(tmKey);
			//printf("ctlTmr current state TM_UNSTALL or NOP unlock\r\n");
            return -1;//para err
        }
    case TM_INSTALL://install state
        {
            if(TM_START==ctlCd||(TM_PAUSE==ctlCd))//into start
                break;
            else
            {
                unlockTm(tmKey);
				//printf("ctlTmr  current state TM_INSTALL unlock\r\n");
                return -1;
            }
        }
    case TM_START://start state
        {
            if((TM_PAUSE==ctlCd)||(TM_STOP==ctlCd))//into stop or pause state
                break;
            else
            {
                unlockTm(tmKey);
				//printf("ctlTmr  current state TM_START unlock\r\n");
                return -1;
            }
        }
    case TM_PAUSE://pause state
        {
            if((TM_GO_ON==ctlCd)||(TM_STOP==ctlCd)||/*(TM_START==ctlCd)||*/(TM_PAUSE==ctlCd))//into goOn  or stop state
                break;
            else
            {
                unlockTm(tmKey);
				//printf("ctlTmr  current state TM_PAUSE unlock\r\n");
                return -1;
            }
        }
    case TM_GO_ON://goOn state
        {
            if((TM_PAUSE==ctlCd)||(TM_STOP==ctlCd))//into stop or pause state
                break;
            else
            {
                unlockTm(tmKey);
				//printf("ctlTmr  current state TM_GO_ON unlock\r\n");
                return -1;
            }
        }
    case TM_STOP://stop state
        {
            if(TM_START==ctlCd||(TM_PAUSE==ctlCd))//into start state
                break;
            else
            {
                unlockTm(tmKey);
				//printf("ctlTmr  current state TM_STOP unlock\r\n");
                return -1;
            }
        }
    default: //undefine state
        {
            unlockTm(tmKey);
			//printf("ctlTmr current state unkown unlock\r\n");
            return -2;//tm state err
        }
    }

    u_char ucOldStd = tTmr_p->wkState;
    tTmr_p->wkState = ctlCd;//modify workState

    switch(tTmr_p->wkState)
    {
    case TM_START://into start state, insert to triger order chain
        {
            tnToStart(m_clkSrc.clkCntr, &m_tmTrgrHd_p, tTmr_p);
            break;
        }
    case TM_PAUSE://into pause state
        if ( TM_START == ucOldStd || (TM_PAUSE == ucOldStd) )
        {
            tnToStop(&m_tmTrgrHd_p, tTmr_p);
        }
        tnToStart(m_clkSrc.clkCntr, &m_tmTrgrHd_p, tTmr_p);
        break;
    case TM_GO_ON://into goOn state, sort triger order chain
        break;
    case TM_STOP://into stop state, pick off tmr from triger order chain
        {
            tnToStop(&m_tmTrgrHd_p, tTmr_p);
            break;
        }
    }
    unlockTm(tmKey);
	//printf("ctlTmr turn to state = %d unlock\r\n", tTmr_p->wkState);
    return 0;
}

//
//
////////////////��ʱ������
////
//���ܣ�����ϵͳ���ڴ������wrapper�ĵ��ú�����ִ��ʱ�Ӽ����������������򴥷���
//      �м���Ƿ��е��ڵĶ�ʱ���������Ҵ��ڷ���ͣ״̬��ִ�ж�ʱ���Ķ�����Ӧ������
//      ���ڵ��ڵķ����ڶ�ʱ��������תΪֹͣ״̬�������򴥷�����ȥ����
//���룺��
//�������
//���أ���
//
void C_TmObj::timeOut()
{
    FunTypeOne* tRtine;
    tmr* tmrNxt_p;
	void* tCallbackPara;
	//printf("timeOut start lock\r\n");
    lockTm(tmKey);
    ++m_clkSrc.clkCntr;
    tmrNxt_p = m_tmTrgrHd_p;
    while ( tmrNxt_p != NULL )
    {
        //if((m_clkSrc.clkCntr - tmrNxt_p->startTm) < tmrNxt_p->xpirTm )
        if(m_clkSrc.clkCntr < tmrNxt_p->xpirTm )
        {			
            break;
        }
        else//triger!
        {
            if((tmrNxt_p->wkState < TM_START)||(tmrNxt_p->wkState > TM_GO_ON))
            {
                unlockTm(tmKey);
				//printf("timeOut current state %d not turn state, unlock and finish\r\n", tmrNxt_p->wkState);
                return;
            }
            else//start, goOn, (pause, stop not in linker)
            {
                if(!tmrNxt_p->type)//only respond once, pick off, turn into stop state
                {
					//printf("current state = %d turn to stop tmrNxt_p->type = %d\r\n" ,tmrNxt_p->wkState, tmrNxt_p->type );
                    tnToStop(&m_tmTrgrHd_p, tmrNxt_p);
                    tmrNxt_p->wkState = TM_STOP;
					
                }
                else//period!
                {
					//printf("current state = %d turn to stop tmrNxt_p->type = %d\r\n", tmrNxt_p->wkState ,tmrNxt_p->type );
                    tnToStop(&m_tmTrgrHd_p, tmrNxt_p);
                }
                int iType = tmrNxt_p->type;

                if(3 != tmrNxt_p->type)//respond
                {
                    tRtine = tmrNxt_p->tmChnl_p->tmFunHdl_p + tmrNxt_p->ID;
					tCallbackPara = tmrNxt_p->tmFunHdlPara_p;
					unlockTm(tmKey);
					//printf("timeOut unlock tmrNxt_p->type = %d, into callback\r\n" ,tmrNxt_p->type );
                    (*tRtine)(tCallbackPara);
					//(*tRtine)(tmrNxt_p->tmFunHdlPara_p);
					lockTm(tmKey);
					//printf("timeOut lock tmrNxt_p->type = %d, exit callback\r\n" ,tmrNxt_p->type );
                }

                if ( iType == TM_PERIOD && tmrNxt_p )
                {
					//printf("current state = %d turn to start tmrNxt_p->type = %d\r\n", tmrNxt_p->wkState ,tmrNxt_p->type );
                    tnToStart(m_clkSrc.clkCntr, &m_tmTrgrHd_p, tmrNxt_p);
					tmrNxt_p->wkState = TM_START;
                }

                tmrNxt_p = m_tmTrgrHd_p;
                //tmrNxt_p = tmrNxt_p->sortTrgrNxt_p;
                continue;
            }
        }
    }
    unlockTm(tmKey);
	//printf("timeOut finish unlock\r\n");
    return ;
}

//
//
////////////////ͨ��ָ��ɾ����ʱ������
////
//���ܣ�������ϵͳ���á�����ֹͣ��װ״̬�£�����ʱ���������򴥷����У���ɾ����ʱ
//      ����ɾ����ʱ�������ͷſռ䣬������ʱ��ת��Ϊδ��װ״̬�������Ժ����Ӷ�ʱ��
//      ʱ�ռ����á�
//���룺
//      tmr_p��Ҫɾ���Ķ�ʱ��ָ��
//�������
//���أ�ִ��״̬��=0���ɹ�
//                =-1�������Ƿ���=-2����ʱ�����ڽ�ֹɾ��״̬�޷�ɾ��
//
int C_TmObj::delTmr( tmr* tmr_p )
{
    if ( tmr_p == NULL )
    {
        return -1;
    }
    if(tmr_p->name_p)
    {
        free(tmr_p->name_p);
        tmr_p->name_p = NULL;
    }


    //pseudo del. turn to uninstall state. not modify "stDel->tmChnl_p->tmrNum"
    if((TM_STOP == tmr_p->wkState)||(TM_INSTALL == tmr_p->wkState))//in stop or install state
        tmr_p->wkState = TM_UNSTALL;
    else
        return -2;//state forbiten del
    return 0;
}

//
//
////////////////ͨ����ʱͨ���źͶ�ʱ����ɾ����ʱ������
////
//���ܣ�������ϵͳ���á�����ֹͣ��װ״̬�£�����ʱ���������򴥷����У���ɾ����ʱ
//      ����ɾ����ʱ�������ͷſռ䣬������ʱ��ת��Ϊδ��װ״̬�������Ժ����Ӷ�ʱ��
//      ʱ�ռ����á�
//���룺
//      tmChnlID��Ҫɾ���Ķ�ʱ��������ʱͨ����
//      tmID��Ҫɾ���Ķ�ʱ�����
//�������
//���أ�ִ��״̬��=0���ɹ�
//                =-1�������Ƿ���=-2����ʱ�����ڽ�ֹɾ��״̬�޷�ɾ��
//
int C_TmObj::delTmr(u_short tmChnlID, u_char tmrID)
{
    tmr* tTmr_p;
    tmChnl* tTmChnl_p;

    if(m_tmChnlSlotTtlNum <= tmChnlID)
        return -1;
    tTmChnl_p = m_tmChnlTblHd_p+tmChnlID;
    if(tmrID >= tTmChnl_p->tmrNum)
        return -1;

    tTmr_p = tTmChnl_p->tmrTblHd_p + tmrID;
    if(tTmr_p->name_p)
    {
        free(tTmr_p->name_p);
        tTmr_p->name_p = NULL;
    }
    //pseudo del. turn to uninstall state. not modify "stDel->tmChnl_p->tmrNum"
    if((TM_STOP == tTmr_p->wkState)||(TM_INSTALL == tTmr_p->wkState))//in stop or install state
        tTmr_p->wkState = TM_UNSTALL;
    else
        return -2;//state forbiten del
    return 0;
}

//
//
////////////////ͨ����ʱͨ����ɾ����ʱͨ������
////
//���ܣ�������ϵͳ���á������ڲ����ж�ʱ��������δ��װ״̬��ִ�У�����ʱͨ��������
//      ��ʱ�����������򴥷����з���ɾ����ɾ����ʱͨ�������ͷſռ䣬������ʱͨ��ת
//      ��Ϊδ��װ״̬�������Ժ����Ӷ�ʱͨ��ʱ�ռ����á�
//���룺
//      tmChnlID��Ҫɾ���Ķ�ʱͨ����
//�������
//���أ�ִ��״̬��=0���ɹ�
//                =-1�������Ƿ���=-2��ͨ���ڶ�ʱ�����ڽ�ֹɾ��״̬�޷�ɾ��ͨ��
//
int C_TmObj::delTmChnlByID(u_short tmChnlID)
{
    tmChnl* tTmChnl_p;
    u_char tWkState;

    if(m_tmChnlSlotTtlNum<=tmChnlID)
        return -1;

    tTmChnl_p = m_tmChnlTblHd_p+tmChnlID;
    if(!tTmChnl_p->state)//has del. return OK
        return 0;

    for(u_char i =0; i< tTmChnl_p->tmrNum; i++/*, tTmChnl_p->tmrTblHd_p++*/)
    {
        tmr* pTmr = &(tTmChnl_p->tmrTblHd_p[i]);
        tWkState = pTmr->wkState;
        if(tWkState)//non uninstall state
            return -2;//state err
    }
    tTmChnl_p->state = TM_UNSTALL;//uninstall!
    m_tmChnlSlotUsingNum--;
    return 0;
}

//
//
////////////////ͨ����ʱͨ��ָ��ɾ����ʱͨ������
////
//���ܣ�������ϵͳ���á������ڲ����ж�ʱ��������δ��װ״̬��ִ�У�����ʱͨ��������
//      ��ʱ�����������򴥷����з���ɾ����ɾ����ʱͨ�������ͷſռ䣬������ʱͨ��ת
//      ��Ϊδ��װ״̬�������Ժ����Ӷ�ʱͨ��ʱ�ռ����á�
//���룺
//      tmChnl_p��Ҫɾ���Ķ�ʱͨ��ָ��
//�������
//���أ�ִ��״̬��=0���ɹ�
//                =-1�������Ƿ���=-2��ͨ���ڶ�ʱ�����ڽ�ֹɾ��״̬�޷�ɾ��ͨ��
//
int C_TmObj::delTmChnlByPtr(tmChnl* tmChnl_p)
{
    tmChnl* tTmChnl_p;
    u_char tWkState;

    tTmChnl_p = tmChnl_p;
    if(!tTmChnl_p)
        return -1;

    if(!tTmChnl_p->state)//has del. return OK
        return 0;

    for(u_char i =0; i< tTmChnl_p->tmrNum; i++)
    {
        tmr* pTmr = &(tTmChnl_p->tmrTblHd_p[i]);
        tWkState = pTmr->wkState;
        if(tWkState)//tmr non uninstall state
            return -2;//state err
    }
    tTmChnl_p->state = TM_UNSTALL;//uninstall!
    m_tmChnlSlotUsingNum--;
    return 0;
}

//
//
////////////////������������ϵͳʱ�亯��
////
//���ܣ����������ʱ��¼���ļ�ʱ�䣬ת��Ϊϵͳʱ��
//���룺
//      stSys��ϵͳʱ�������
//�������
//���أ�ִ��״̬��=0���ɹ�
//
#ifdef WINDOWS_PLATFORM
int C_TmObj::getSysStartTm(SYSTEMTIME &sysTm)
{
    ::FileTimeToSystemTime(&m_clkSrc.locFileTm, &sysTm); 
    return 0;
}
#else
#endif
//
//
////////////////����������ʱ�亯��
////
//���ܣ�������������ʱ�̿�ʼ��ִ�б��ε���ֹ��������ʱ�䣬��λΪticks��
//���룺��
//�������
//���أ����о�����ticks��
//
int C_TmObj::getSysRunTm()
{
    u_int tClkCntr;
	//printf("getSysRunTm lock\r\n");
    lockTm(tmKey);
    tClkCntr = m_clkSrc.clkCntr;
    unlockTm(tmKey);
    return tClkCntr;
}

//
//
////////////////��ö�ʱͨ�����в���
////
//���ܣ�������ϵͳ���ã��õ�ָ����ʱͨ���ĵ�ǰ״̬�Ͱ����Ķ�ʱ����Ŀ
//���룺
//      tmChnlID��Ҫ��ȡ���в����Ķ�ʱͨ����
//      paraFlag��Ҫ��ȡ�Ĳ�����ʶ��=0����ȡ��ʱͨ��״̬��=1����ȡ�ں���ʱ������
//�������
//���أ����о�����ticks��
//
int C_TmObj::getTmChnlPara(u_short tmChnlID, u_char paraFlag)
{
    tmChnl* tTmChnl_p;

    if(m_tmChnlSlotTtlNum<=tmChnlID)
        return -1;

    tTmChnl_p = m_tmChnlTblHd_p + tmChnlID;
    if(!paraFlag)
        return tTmChnl_p->state;
    if(1==paraFlag)
        return tTmChnl_p->tmrNum;
    return -1;
}

//
//
////////////////ͨ����ʱͨ���Ż�ö�ʱͨ��ָ��
////
//���ܣ�ͨ����ʱͨ���Ż������ָ��
//���룺
//      tmChnlID����ʱͨ����
//�������
//���أ�=��ʱͨ��ָ�룬�ɹ���=NULL�������Ƿ�
//
tmChnl* C_TmObj::getTmChnlPtrByID(u_short tmChnlID)
{
    if(m_tmChnlSlotTtlNum<=tmChnlID)
        return NULL;

    return m_tmChnlTblHd_p + tmChnlID;
}

//
//
////////////////ͨ����ʱͨ��ָ���ö�ʱͨ����
////
//���ܣ�ͨ����ʱͨ���Ż������ָ��
//���룺
//      tmChnl_p����ʱͨ��ָ��
//�������
//���أ�=��ʱͨ���ţ��ɹ���=-1�������Ƿ�
//
int C_TmObj::getTmChnlIDByPtr(tmChnl* tmChnl_p)
{
    if(!tmChnl_p)
        return -1;
    return tmChnl_p->tmChnlID;
}

//
//
////////////////��ö�ʱ�����в���
////
//���ܣ�������ϵͳ���á��õ�ָ����ʱ���ĵ�ǰ����״̬�����ͣ����ںͷ����ڣ���������
//      ��ʱ�䣨ticks�����ͱ��μ�������ʱ�䣨ticks����
//���룺
//      tmChnlID����ʱ��������ʱͨ����
//      tmID����ʱ�����
//      paraFlag��Ҫ��ȡ�Ĳ�����ʶ��=0����ȡ��ʱ������״̬��=1����ȡ��ʱ�����ͣ�
//                =2����ȡ��ʱ����������ʱ�䣻=3����ȡ��ʱ����һ�ε���ʱ��
//�������
//���أ�=��ȡ�Ĳ���ֵ���ɹ���-1�������Ƿ�
//
int C_TmObj::getTmrPara(u_short tmChnlID, u_char tmrID, u_char paraFlag)
{
    tmChnl* tTmChnl_p;
    tmr* tTmr_p;
    int tRtn;

    if(m_tmChnlSlotTtlNum<=tmChnlID)
        return -1;
    tTmChnl_p = m_tmChnlTblHd_p+tmChnlID;
    if(tmrID >= tTmChnl_p->tmrNum)
        return -1;

    tTmr_p = tTmChnl_p->tmrTblHd_p + tmrID;
	//printf("getTmrPara lock\r\n");
    lockTm(tmKey);
    if(!paraFlag)
        tRtn = tTmr_p->wkState;
    else if(1==paraFlag)
        tRtn = tTmr_p->type;
    else if(2==paraFlag)
        tRtn = tTmr_p->startTm;
    else if(3==paraFlag)
        tRtn = tTmr_p->xpirTm;
    else
        tRtn = -1;
    unlockTm(tmKey);
    return tRtn;
}

//
//
////////////////��ö�ʱ������
////
//���ܣ�������ϵͳ���á��õ���ʱ������������'/0'Ϊ������ASCII���ַ�����
//���룺
//      tmChnlID����ʱ��������ʱͨ����
//      tmID����ʱ�����
//      name_p�����صĶ�ʱ�����ƻ����ַ
//      lng����ʱ�����Ƴ��ȣ�����'/0'�ַ�
//�������
//���أ�=��ʱ�����ƻ����ַ���ɹ���=NULL�������Ƿ�������ʧ��
//
char* C_TmObj::getTmrName(u_short tmChnlID, u_char tmrID, char* name_p, int lng)
{
    tmChnl* tTmChnl_p;
    tmr* tTmr_p;

    // 	if(m_tmChnlSlotIdleIdx<=tmChnlID)
    // 		return NULL;
    tTmChnl_p = m_tmChnlTblHd_p+tmChnlID;
    if(tmrID >= tTmChnl_p->tmrNum)
        return NULL;

    tTmr_p = tTmChnl_p->tmrTblHd_p + tmrID;
    if((int)(strlen(tTmr_p->name_p)+1)>lng)
        return NULL;

    return strcpy(name_p, tTmr_p->name_p);
}

//
//
////////////////��ö�ʱ�����
////
//���ܣ�������ϵͳ���á��õ���ʱ������������'/0'Ϊ������ASCII���ַ�����
//���룺
//      tmChnlID����ʱ��������ʱͨ����
//      name_p����ʱ������������'/0'Ϊ������ASCII���ַ�����
//�������
//���أ�=��ʱ����ţ��ɹ���=0xFF�������Ƿ�������ʧ��
//
u_char C_TmObj::getTmrID(u_short tmChnlID, char* name_p)
{
    tmChnl* tTmChnl_p;
    tmr* tTmr_p;

    // 	if(m_tmChnlSlotIdleIdx<=tmChnlID)
    // 		return 0xFF;
    tTmChnl_p = m_tmChnlTblHd_p+tmChnlID;

    tTmr_p = tTmChnl_p->tmrTblHd_p;
    for(int i=0; i<tTmChnl_p->tmrNum;i++,tTmr_p++)
    {
        if(!strcmp(tTmr_p->name_p, name_p))
            return tTmr_p->ID;
    }
    return 0xFF;
}

//
//
////////////////��ö�ʱ����������������ticksʱ�亯��
////
//���ܣ�������ϵͳ���á���ö�ʱ����������������ticksʱ��
//���룺
//      tmChnlID����ʱ��������ʱͨ����
//      tmID����ʱ�����
//�������
//���أ�=��ʱ����������������ticksʱ�䣬�ɹ���=-1�������Ƿ�
//      =0����ʱ����ǰδ�������޾���ʱ��
int C_TmObj::tmrRunTm(u_short tmChnlID, u_char tmrID)
{
    tmChnl* tTmChnl_p;
    tmr* tTmr_p;
    u_char tWkState;
    u_int tClkCntr, tStartTm;


    if(m_tmChnlSlotTtlNum<=tmChnlID)
        return -1;
    tTmChnl_p = m_tmChnlTblHd_p+tmChnlID;
    if(tmrID >= tTmChnl_p->tmrNum)
        return -1;

    tTmr_p = tTmChnl_p->tmrTblHd_p + tmrID;
	//printf("tmrRunTm lock\r\n");
    lockTm(tmKey);
    tWkState = tTmr_p->wkState;
    tClkCntr = m_clkSrc.clkCntr;
    tStartTm = tTmr_p->startTm;
    unlockTm(tmKey);
    if(tWkState > 1)//start, pause, goOn, stop
        return ( tClkCntr - tStartTm);
    else
        return 0;
}


///////////////////////////////////////////////////
////
//
////////�Ự����ʱ�������
////����:
//		1. ֹͣ(��λ)Ӧ�ö�ʱ��
//		2. ɾ�������Ựʹ�õ���ʱ����Դ
//		3. ���ͻỰ���֪ͨ��֪ͨ�Ự������
//           �����߻Ự�����״̬(�ɹ����)
//           (linux������Ϣ,windowsֱ�ӻָ��Ự�����߳�)
//����:
//		 ssnCtlBlk_p:�Ự���ƿ�ָ��
//		 errCd:�Ựʧ����:
/*		0x00 0000--�Ự�ɹ����
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
//����ֵ:��
// extern C_SsnMgr ssnMgr;
// void ssnTmOutDftHdl(void* tmoutInfo_p)
// {
// 	ssnCtlBlk* tSsnCtlBlk_p;
// 	C_SsnObj* ssnObj_p;
// 
// 	tSsnCtlBlk_p = (ssnCtlBlk*)tmoutInfo_p;
// 	ssnObj_p = ssnMgr.getSsnObj(tSsnCtlBlk_p->ssnNo);
// 
// 	/////////////////timeout >0: act ssnInst or passive ssnInst
// 	// 1. stop app tmr.
// 	u_short tTmChnlID;
// 	u_char tTmrNum;
// 	tTmChnlID = tmFclty.getTmChnlIDByPtr(tSsnCtlBlk_p->ssnTmHead_p);
// 	tTmrNum = tmFclty.getTmChnlPara(tTmChnlID, 1);
// 	for(u_char i = 0;i<tTmrNum;i++)
// 	{
// 		tmFclty.ctlTmr(tSsnCtlBlk_p->ssnTmHead_p, i, 5);
// 		if(!tSsnCtlBlk_p->actFlg)//act ssn Inst
// 			tmFclty.delTmr(tTmChnlID, i);
// 	}
// 
// 	/////////////////timeout >0: act ssnInst
// 	if(!tSsnCtlBlk_p->actFlg)//act ssnInst
// 	{
// 		tmFclty.delTmChnlByID(tTmChnlID);
// 
// 		// 2. prepare usr info.
// 		u_int msgBdy[2];
// 		msgBdy[0]= tSsnCtlBlk_p->chnlID|(tSsnCtlBlk_p->ssnNo<<16);
// 		msgBdy[1]=  0x020000;
// 
// 		// 3. clear ssnInst & other resource...
// 		ssnObj_p->delActiveSsn(tSsnCtlBlk_p);
// 
// 		// 4. notify to usr
// 		ssnFnshNfy(tSsnCtlBlk_p->hostTID,(char*)&msgBdy, 2*sizeof(u_int));
// 		return ;
// 	}
// 
// 	/////////////////timeout >0: passive ssnInst
// 	//notify to inner-thread ( system thread)!
// 	//un-implement, temporarily
// 	//..
// 	return ;
// }










