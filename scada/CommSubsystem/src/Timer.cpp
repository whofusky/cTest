//定时器实现文件
//
//说明：1.支持定时通道（定时器组），内部分配定时通道号（定时通道ID），并返回给调用者，作为
//        用户级定时通道唯一标识，每组包含的定时器最大数量可定制，运行时不可修改。
//      2.所有定时器必须唯一属于特定定时通道，申请定时器必须在存在定时通道的情况下进行，按
//        在指定的定时通道内插入所申请的定时器。所有定时器采用组内唯一编号（ID）标识，由调
//		  用者根据用途或其它分类法为其编号，相同编号定时器不能申请到同一定时通道内。
//      3.定时通道和定时器的申请（添加）无线程安全保证，定时器控制和访问均提供线程安全保证。
//      4.定时器控制按状态机方式执行,一次性定时器到期触发后转为停止状态，周期性定时器触发后
//        再次进入启动状态：
//        (取消)未安装<-->安装-->启动-->暂停<-->继续
//				  |\			  |\      |     /
//				    \	 	        \     |    /
//                   \               \|  \|/ |/
//		              停                     止
//
//单位：兆方美迪风能工程(上海)有限公司
//编写人：赵金杰
//编写时间：2016年12月
//

#include "Timer.h"



/************************************************************/
/*				        平台相关部分    	                */
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
////////////////windows 64位到期回调wrapper
////
//功能：封装为windows多媒体定时器回调程序标准形式
//输入：符合windows多媒体定时器回调程序形式
//输出：无
//返回：无
//
void CALLBACK tmOutHdl_WIN64( UINT uID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2 )
{
    return ((C_TmObj *)dwUser)->timeOut();
}

//
//
////////////////windows 64定时器配置函数
////
//功能：定制操作系统提供的多媒体定时器功能，实现定时器应用的初始化过程
//输入：
//		tmFacility_p：定时器对象指针
//输出：无
//返回：执行状态。=0,成功
//                =-1，输入参数非法；-10，获取或设置系统定时器分辨能力失败；
//                =-11，设置系统定时器到期响应事件失败
//
static int cfgTmFclty_WIN64(C_TmObj* tmFclty_p)
{
    TIMECAPS tc;

    if(!tmFclty_p)
        return -1;
    tmFclty_p->m_tmrRes = MC_TRICK_TIME;//定义时间间隔
    if(timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR)
        return -10;
    else
    {

        tmFclty_p->m_accu = min(max(tc.wPeriodMin, MC_TRICK_TIME), tc.wPeriodMax); //判断分辨率是否在允许范围
        timeBeginPeriod(tmFclty_p->m_accu);  //设置定时器分辨率
    }     
    //设置定时器回调事件，回调函数形如fun
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
////////////////windows 64取消定时器函数
////
//功能：取消操作系统提供的多媒体定时器功能，停止多媒体定时器，释放资源
//输入：
//		tmFacility_p：定时器对象指针
//输出：无
//返回：无
//
static void clrCfgTmFclty_WIN64(C_TmObj* tmFclty_p)
{
    if(!tmFclty_p)
        return;

    //关闭定时器事件
    timeKillEvent(tmFclty_p->m_clkSrcID);
    tmFclty_p->m_clkSrcID = 0;
    //清除最小定时器精度
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
////////////////Linux 64位到期回调wrapper
////
//功能：封装为linux itimer回调程序标准形式
//输入：符合linuxitimer回调程序标准形式
//输出：无
//返回：无
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
////////////////Linux 64定时器配置函数
////
//功能：定制操作系统提供的多媒体定时器功能，实现定时器应用的初始化过程
//输入：
//		tmFacility_p：定时器对象指针
//输出：无
//返回：执行状态。=0,成功
//                =-1，输入参数非法；-10，获取或设置系统定时器分辨能力失败；
//                =-11，设置系统定时器到期响应事件失败
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
    tick.it_value.tv_usec = 0;//改为0后信号似乎造成了信号的阻塞。
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
////////////////windows 64取消定时器函数
////
//功能：取消操作系统提供的itimer定时器功能，停止定时器，释放资源
//输入：
//		tmFacility_p：定时器对象指针
//输出：无
//返回：无
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
/*				        平台无关部分    	                */
/************************************************************/

//
//
////////////////定时器转入启动状态
////
//功能：定时器加入排序触发链，状态由停止或安装转入启动状态
//输入：
//		ticks：绝对启动时间
//		tmrTrgrHd_pp：定时器排序触发链头地址的指针
//		tmr_p：需要启动的定时器指针
//输出：无
//返回：无
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
////////////////定时器转入停止状态
////
//功能：定时器从启动或继续状态转入停止状态，将其从排序触发链中摘除
//输入：
//		ticks：绝对启动时间
//		tmrTrgrHd_pp：定时器排序触发链头地址的指针
//		tmr_p：需要启动的定时器指针
//输出：无
//返回：无
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
////////////////构造函数
////
//功能：初始化，获取本地时间，并转换为文件时间，创建互斥锁，预分配空定时通道槽，
//      配置系统定时器资源
//输入：无
//输出：无
//返回：无
//
C_TmObj::C_TmObj()
{
    m_clkSrc.clkCntr = 0;
#ifdef  WINDOWS_PLATFORM              //系统外部调用，框架暂时不用，注释掉
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

    //预创建MC_ALLOC_CHNL_NUM个定时通道空槽
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
////////////////析构函数
////
//功能：释放互斥锁，定时通道及定时器存储
//输入：无
//输出：无
//返回：无
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
////////////////增加定时通道
////
//功能：由配置子系统调用。优先使用存在的定时通道空槽。无空槽，若未超出系统定时通道
//      容量，定量创建空槽。取出一个空槽分配给新增定时通道，按输入参数实例化，并创
//      建定时器空槽，新增定时通道设置为安装状态
//输入：
//      tmNum：新增定时通道包含的定时器数量
//输出：
//		tChnlID：新增定时通道号，通道编号从0开始
//返回：执行状态。=0,成功
//                =-1，输入参数非法；-2，超出系统定时通道容量无法增加新的定时通道；
//                =-3，存储操作失败
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
////////////////向定时通道中增加定时器
////
//功能：由配置子系统调用。向指定定时通道中增加一个定时器，成功时设置定时器为安装状态
//输入：
//      tChnlID：定时通道号
//      tmCfg_p：新增定时器配置信息，包括：定时器编号（定时通道内编号，从0开始）；定时
//               器名称（以'\0'标识结束的ASCII码）；定时器类型（周期或非周期）；到期时
//               间（相对于此定时器启动的到期时间，内部将期转换为相对于软件启动的时间）；
//               到期响应函数地址
//输出：无
//返回：执行状态。=0,成功
//                =-1，输入参数非法；=-3，存储操作失败
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
// FUN:删除应用定时器
// Param:
//		tmChnlID：通道号
//		tmChnlH_p：应用级定时器链表头指针
// Ret: 0，成功；其他，失败；
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
////////////////定时器控制函数
////
//功能：多线程调用。控制定时器，驱动定时器按规则转换状态，仅在启动和继续状态下才
//      会发生到期事件。定制器转入启动状态时，加入排序触发链，转入停止状态时，从
//      排序触发链中去除。
//输入：
//      chnl_p：定时通道指针
//      ctlCd：控制码。=2，启动状态；=3，暂停状态；=4，继续状态；=5，停止状态
//输出：无
//返回：执行状态。=0,成功
//                =-1，输入参数非法；=-2，定时器状态错误，=-3，存储操作失败
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
////////////////超时处理函数
////
//功能：操作系统到期处理过程wrapper的调用函数，执行时钟计数器单增，在排序触发链
//      中检查是否有到期的定时器，若有且处于非暂停状态，执行定时器的定制响应函数。
//      对于到期的非周期定时器，将其转为停止状态，从排序触发链中去除。
//输入：无
//输出：无
//返回：无
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
////////////////通过指针删除定时器函数
////
//功能：配置子系统调用。处于停止或安装状态下，即定时器不在排序触发链中，可删除定时
//      器。删除定时器并不释放空间，仅将定时器转换为未安装状态，用于以后增加定时器
//      时空间重用。
//输入：
//      tmr_p：要删除的定时器指针
//输出：无
//返回：执行状态。=0，成功
//                =-1，参数非法；=-2，定时器处于禁止删除状态无法删除
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
////////////////通过定时通道号和定时器号删除定时器函数
////
//功能：配置子系统调用。处于停止或安装状态下，即定时器不在排序触发链中，可删除定时
//      器。删除定时器并不释放空间，仅将定时器转换为未安装状态，用于以后增加定时器
//      时空间重用。
//输入：
//      tmChnlID：要删除的定时器所属定时通道号
//      tmID：要删除的定时器编号
//输出：无
//返回：执行状态。=0，成功
//                =-1，参数非法；=-2，定时器处于禁止删除状态无法删除
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
////////////////通过定时通道号删除定时通道函数
////
//功能：配置子系统调用。仅在内部所有定时器均处于未安装状态下执行，即定时通道内所有
//      定时器均不在排序触发链中方可删除。删除定时通道并不释放空间，仅将定时通道转
//      换为未安装状态，用于以后增加定时通道时空间重用。
//输入：
//      tmChnlID：要删除的定时通道号
//输出：无
//返回：执行状态。=0，成功
//                =-1，参数非法；=-2，通道内定时器处于禁止删除状态无法删除通道
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
////////////////通过定时通道指针删除定时通道函数
////
//功能：配置子系统调用。仅在内部所有定时器均处于未安装状态下执行，即定时通道内所有
//      定时器均不在排序触发链中方可删除。删除定时通道并不释放空间，仅将定时通道转
//      换为未安装状态，用于以后增加定时通道时空间重用。
//输入：
//      tmChnl_p：要删除的定时通道指针
//输出：无
//返回：执行状态。=0，成功
//                =-1，参数非法；=-2，通道内定时器处于禁止删除状态无法删除通道
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
////////////////获得软件启动的系统时间函数
////
//功能：将软件启动时记录的文件时间，转换为系统时间
//输入：
//      stSys：系统时间的引用
//输出：无
//返回：执行状态。=0，成功
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
////////////////获得软件运行时间函数
////
//功能：获得软件自启动时刻开始至执行本次调用止，经历的时间，单位为ticks数
//输入：无
//输出：无
//返回：运行经历的ticks数
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
////////////////获得定时通道运行参数
////
//功能：配置子系统调用，得到指定定时通道的当前状态和包含的定时器数目
//输入：
//      tmChnlID：要获取运行参数的定时通道号
//      paraFlag：要获取的参数标识。=0，获取定时通道状态；=1，获取内含定时器数量
//输出：无
//返回：运行经历的ticks数
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
////////////////通过定时通道号获得定时通道指针
////
//功能：通过定时通道号获得它的指针
//输入：
//      tmChnlID：定时通道号
//输出：无
//返回：=定时通道指针，成功；=NULL，参数非法
//
tmChnl* C_TmObj::getTmChnlPtrByID(u_short tmChnlID)
{
    if(m_tmChnlSlotTtlNum<=tmChnlID)
        return NULL;

    return m_tmChnlTblHd_p + tmChnlID;
}

//
//
////////////////通过定时通道指针获得定时通道号
////
//功能：通过定时通道号获得它的指针
//输入：
//      tmChnl_p：定时通道指针
//输出：无
//返回：=定时通道号，成功；=-1，参数非法
//
int C_TmObj::getTmChnlIDByPtr(tmChnl* tmChnl_p)
{
    if(!tmChnl_p)
        return -1;
    return tmChnl_p->tmChnlID;
}

//
//
////////////////获得定时器运行参数
////
//功能：配置子系统调用。得到指定定时器的当前工作状态、类型（周期和非周期）、本次启
//      动时间（ticks数）和本次即将到期时间（ticks数）
//输入：
//      tmChnlID：定时器所属定时通道号
//      tmID：定时器编号
//      paraFlag：要获取的参数标识。=0，获取定时器工作状态；=1，获取定时器类型；
//                =2，获取定时器本次启动时间；=3，获取定时器下一次到期时间
//输出：无
//返回：=获取的参数值，成功；-1，参数非法
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
////////////////获得定时器名称
////
//功能：配置子系统调用。得到定时器描述名（以'/0'为结束的ASCII码字符串）
//输入：
//      tmChnlID：定时器所属定时通道号
//      tmID：定时器编号
//      name_p：返回的定时器名称缓存地址
//      lng：定时器名称长度，不含'/0'字符
//输出：无
//返回：=定时器名称缓存地址，成功；=NULL，参数非法或其它失败
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
////////////////获得定时器编号
////
//功能：配置子系统调用。得到定时器描述名（以'/0'为结束的ASCII码字符串）
//输入：
//      tmChnlID：定时器所属定时通道号
//      name_p：定时器描述名（以'/0'为结束的ASCII码字符串）
//输出：无
//返回：=定时器编号，成功；=0xFF，参数非法或其它失败
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
////////////////获得定时器本次启动后经历的ticks时间函数
////
//功能：配置子系统调用。获得定时器本次启动后经历的ticks时间
//输入：
//      tmChnlID：定时器所属定时通道号
//      tmID：定时器编号
//输出：无
//返回：=定时器本次启动后经历的ticks时间，成功；=-1，参数非法
//      =0，定时器当前未启动，无经历时间
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
////////会话级超时处理过程
////功能:
//		1. 停止(复位)应用定时器
//		2. 删除主动会话使用的临时性资源
//		3. 发送会话完成通知，通知会话请求者
//           或发起者会话的完成状态(成功与否)
//           (linux发送消息,windows直接恢复会话请求线程)
//输入:
//		 ssnCtlBlk_p:会话控制块指针
//		 errCd:会话失败码:
/*		0x00 0000--会话成功完成
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
//返回值:无
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










