

#include "sysMgr.h"

/************************************************************/
/*				         平台相关部分      	                */
/************************************************************/
//

#ifdef WINDOWS_PLATFORM
//ssnMgr
#define crtSsnTblLock(tmKey)		(InitializeCriticalSection(&tmKey))
#define freeSsnTblLock(tmKey)		(DeleteCriticalSection(&tmKey))
#define lockSsnTbl(tmKey)			(EnterCriticalSection(&tmKey))
#define unlockSsnTbl(tmKey)			(LeaveCriticalSection(&tmKey))

//chnlMgr
#define crtChnlTblLock(tmKey)		(InitializeCriticalSection(&tmKey))
#define freeChnlTblLock(tmKey)		(DeleteCriticalSection(&tmKey))
#define lockChnlTbl(tmKey)			(EnterCriticalSection(&tmKey))
#define unlockChnlTbl(tmKey)		(LeaveCriticalSection(&tmKey))


#else
//ssnMgr
#define crtSsnTblLock(tmKey)		(pthread_mutex_init(&tmKey,NULL))
#define freeSsnTblLock(tmKey)		(pthread_mutex_destroy(&tmKey))
#define lockSsnTbl(tmKey)			(pthread_mutex_lock(&tmKey))
#define unlockSsnTbl(tmKey)			(pthread_mutex_unlock(&tmKey))

//chnlMgr
#define crtChnlTblLock(tmKey)		(pthread_mutex_init(&tmKey,NULL))
#define freeChnlTblLock(tmKey)		(pthread_mutex_destroy(&tmKey))
#define lockChnlTbl(tmKey)			(pthread_mutex_lock(&tmKey))
#define unlockChnlTbl(tmKey)		(pthread_mutex_unlock(&tmKey))
#endif

/************************************************************/
/*				         平台无关部分      	                */
/************************************************************/
//

///////////////////////////////ssn manage
////
//构造函数
C_SsnMgr::C_SsnMgr()
{
    ssnTblItemTtlNum = 0;
    ssnTblItemUsingNum = 0;
    snnTblHd_p = NULL;

    //预创建MC_ALLOC_SSNTBLITEM_NUM个会话表项空槽
    snnTblHd_p = (snnTblItem*)calloc(MC_ALLOC_SSNTBLITEM_NUM, sizeof(snnTblItem));
    if (snnTblHd_p == NULL )
        return;
    ssnTblItemTtlNum = MC_ALLOC_SSNTBLITEM_NUM;
    crtSsnTblLock(ssnTblkey);
}

C_SsnMgr::~C_SsnMgr()
{
    free(snnTblHd_p);
    freeSsnTblLock(ssnTblkey);
}
//
//
//申请会话项
int C_SsnMgr::aplSsnItem(snnAppCfg cfgPara, HMODULE fHdl)
{
    snnTblItem* tSnnTblItem_p;
    u_short tSsnID;
    // 	if(!fHdl)
    // 		return -1;

    //找空闲表项
    if(ssnTblItemUsingNum == ssnTblItemTtlNum)//append to assign!
    {
        if(ssnTblItemTtlNum == MC_DEF_SSNTBLITEM_MAX)//arrive to sys vol!
            return -2;

        tSnnTblItem_p = (snnTblItem*)realloc(snnTblHd_p, (ssnTblItemTtlNum + MC_ALLOC_SSNTBLITEM_NUM/2) * sizeof(snnTblItem));
        if(!tSnnTblItem_p)
        {
            memset((void*)(tSnnTblItem_p + ssnTblItemTtlNum),0,MC_ALLOC_SSNTBLITEM_NUM/2);//new save space init

            lockSsnTbl(ssnTblkey);
            snnTblHd_p = tSnnTblItem_p;
            unlockSsnTbl(ssnTblkey);

            //			tSnnTblItem_p = tSnnTblItem_p + ssnTblItemTtlNum;
            ssnTblItemTtlNum = ssnTblItemTtlNum + MC_ALLOC_SSNTBLITEM_NUM/2;
            tSsnID = ssnTblItemUsingNum;//new ssn ID
            ssnTblItemUsingNum++;
        }
        else
            return -3;
    }
    else
    {
        //prior reuse!
        int i;
        for(i=0;i <= ssnTblItemUsingNum; i++)//has gap(<) or no gap(=)
        {
            if(!((snnTblItem*)(snnTblHd_p+i))->usingFlg)
                break;
        }

        //i is idle item
        tSsnID = i;//new ssn ID
        ssnTblItemUsingNum++;
    }
    //set item
    ((snnTblItem*)(snnTblHd_p+tSsnID))->fHdl = fHdl;
    ((snnTblItem*)(snnTblHd_p+tSsnID))->prio = cfgPara.prio;
    ((snnTblItem*)(snnTblHd_p+tSsnID))->ssnKey.ptclNo = cfgPara.ptclNo;
    ((snnTblItem*)(snnTblHd_p+tSsnID))->ssnKey.snnDscNo = cfgPara.snnDscNo;
    ((snnTblItem*)(snnTblHd_p+tSsnID))->usingFlg = 1;
    return tSsnID;
}

//
//
//归还会话项
int C_SsnMgr::gvBkSsnItem(u_short ssnID)
{
    if(ssnID >= ssnTblItemUsingNum)
        return -1;

    ((snnTblItem*)(snnTblHd_p+ssnID))->usingFlg = 0;
    return 0;
}

//
//
//根据会话键获取会话ID（号）
u_short C_SsnMgr::getSsnID(ptclSnnKey snnKey)//using & ptcl+snnDscNo equ.
{
    for(u_short i=0; i<ssnTblItemTtlNum; i++)
    {
        snnTblItem* snnTblItem_p = (snnTblHd_p+i);
        if((1 == snnTblItem_p->usingFlg)&& \
            (snnTblItem_p->ssnKey.ptclNo == snnKey.ptclNo)&& \
            (snnTblItem_p->ssnKey.snnDscNo == snnKey.snnDscNo))
            return i;
    }
    return 0xFFFF;
}


//
//
//获取会话对象
C_SsnObj* C_SsnMgr::getSsnObj(u_short ssnID)
{
    if(ssnID >= ssnTblItemUsingNum)
        return NULL;
    else
        return ((snnTblItem*)(snnTblHd_p+ssnID))->ssnObj_p;
}


//
//
//申请注册会话对象
int C_SsnMgr::regSsnObj(u_short ssnID, C_SsnObj* ssnObj_p)
{
    if(ssnID >= ssnTblItemUsingNum)
        return NULL;
    if(!ssnObj_p)
        return NULL;
    ((snnTblItem*)(snnTblHd_p+ssnID))->ssnObj_p = ssnObj_p;
    return 0;
}


//
//
//获取会话所属协议动态连接库模块句柄（描述符）
HMODULE C_SsnMgr::getSsnFileHdl(u_short ssnID)
{
    if(ssnID>=ssnTblItemUsingNum)
        return NULL;
    else
        return ((snnTblItem*)(snnTblHd_p+ssnID))->fHdl;
}


//
//
//获取会话所属协议号
int	C_SsnMgr::getSsnPtclNo(u_short ssnID)
{
    if(ssnID>=ssnTblItemUsingNum)
        return -1;
    else
        return ((snnTblItem*)(snnTblHd_p+ssnID))->ssnKey.ptclNo;
}


//
//
//获取会话描述号（会话key=协议号+会话描述号）
int	C_SsnMgr::getSsnDscNo(u_short ssnID)
{
    if(ssnID>=ssnTblItemUsingNum)
        return -1;
    else
        return ((snnTblItem*)(snnTblHd_p+ssnID))->ssnKey.snnDscNo;
}


//
//
//获取会话优先级
int	C_SsnMgr::getSsnPrio(u_short ssnID)
{
    if(ssnID>=ssnTblItemUsingNum)
        return -1;
    else
        return ((snnTblItem*)(snnTblHd_p+ssnID))->prio;
}


///////////////////////////////channel manage
////////

C_ChnlMgr::C_ChnlMgr()
{
    chnlTblItemTtlNum = 0;
    chnlTblItemUsingNum = 0;
    chnlTblHd_p = NULL;

    //预创建MC_ALLOC_SSNTBLITEM_NUM个会话表项空槽
    chnlTblHd_p = (chnlTblItem*)calloc(MC_ALLOC_CHNLTBLITEM_NUM, sizeof(chnlTblItem));
    if (chnlTblHd_p == NULL )
        return;
    chnlTblItemTtlNum = MC_ALLOC_CHNLTBLITEM_NUM;
    crtChnlTblLock(chnlTblkey);
}


C_ChnlMgr::~C_ChnlMgr()
{
    free(chnlTblHd_p);
    freeChnlTblLock(chnlTblkey);
}


int C_ChnlMgr::aplChnlItem()//
{
    chnlTblItem* tChnlTblItem_p;
    u_short tChnlID;
    u_char tChnlRegState;

    //无空闲表项，追加分配
    if(chnlTblItemUsingNum == chnlTblItemTtlNum)//append to assign!
    {
        if(chnlTblItemTtlNum == MC_DEF_CHNLTBLITEM_MAX)//arrive to sys vol!
            return -2;

        tChnlTblItem_p = (chnlTblItem*)realloc(chnlTblHd_p, (chnlTblItemTtlNum + MC_ALLOC_CHNLTBLITEM_NUM/2) * sizeof(chnlTblItem));
        if(!tChnlTblItem_p)
        {
            memset((void*)(tChnlTblItem_p + chnlTblItemTtlNum),0,MC_ALLOC_CHNLTBLITEM_NUM/2);//new save space init

            lockChnlTbl(chnlTblkey);
            chnlTblHd_p = tChnlTblItem_p;//mgr, app rcv thread use
            unlockChnlTbl(chnlTblkey);


            chnlTblItemTtlNum = chnlTblItemTtlNum + MC_ALLOC_CHNLTBLITEM_NUM/2;
            tChnlID = chnlTblItemUsingNum;//new chnl ID
        }
        else
            return -3;
    }
    else//使用空闲表项
    {
        //prior reuse!
        int i;
        for(i=0;i <= chnlTblItemUsingNum;i++)//has gap(<) or no gap(=)
        {
            lockChnlTbl(chnlTblkey);
            tChnlRegState = ((chnlTblItem*)(chnlTblHd_p+i))->chnlState & MC_CHNLST_MASK_REG;
            unlockChnlTbl(chnlTblkey);
            if(!tChnlRegState)
                break;
        }

        //i is idle slot
        tChnlID = chnlTblItemUsingNum;
    }

    chnlTblItemUsingNum++;
    return tChnlID;
}

int C_ChnlMgr::gvBkChnlItem(u_short chnlID)
{
    u_char tChnlState;
    if(chnlID>=chnlTblItemUsingNum)
        return -1;

    lockChnlTbl(chnlTblkey);
    tChnlState = ((chnlTblItem*)(chnlTblHd_p+chnlID))->chnlState;
    ((chnlTblItem*)(chnlTblHd_p+chnlID))->chnlState = tChnlState & ~MC_CHNLST_MASK_REG;
    unlockChnlTbl(chnlTblkey);
    chnlTblItemUsingNum--;

    return chnlTblItemUsingNum;
}


int C_ChnlMgr::regChnlObj(u_short chnlID, C_ChnlObj* chnlObj_p)
{
    if((chnlID>=chnlTblItemUsingNum)||(!chnlObj_p))
        return -1;

    ((chnlTblItem*)(chnlTblHd_p+chnlID))->chnlObj_p = chnlObj_p;
    return 0;
}

C_ChnlObj* C_ChnlMgr::getChnlObj(u_short chnlID)
{
    if(chnlID>=chnlTblItemUsingNum)
        return NULL;
    return ((chnlTblItem*)(chnlTblHd_p+chnlID))->chnlObj_p;
}

int C_ChnlMgr::regStnOrVew(u_short chnlID, stnOrVew* stnOrVew_p)
{
    if((chnlID>=chnlTblItemUsingNum)||(!stnOrVew_p))
        return -1;
    ((chnlTblItem*)(chnlTblHd_p+chnlID))->stnOverView.stnID = stnOrVew_p->stnID;
    ((chnlTblItem*)(chnlTblHd_p+chnlID))->stnOverView.role = stnOrVew_p->role;
    ((chnlTblItem*)(chnlTblHd_p+chnlID))->stnOverView.stnAddr = stnOrVew_p->stnAddr;
    ((chnlTblItem*)(chnlTblHd_p+chnlID))->stnOverView.stnAppID = stnOrVew_p->stnAppID;
    ((chnlTblItem*)(chnlTblHd_p+chnlID))->stnOverView.localPort = stnOrVew_p->localPort;
    return 0;
}

stnOrVew* C_ChnlMgr::getStnOrVew(u_short chnlID)
{
    if(chnlID>=chnlTblItemUsingNum)
        return NULL;
    return &((chnlTblItem*)(chnlTblHd_p+chnlID))->stnOverView;
}


int C_ChnlMgr::setChnlState(u_short chnlID, u_short stateMsk)
{
    u_short tChnlState;
    if(chnlID>=chnlTblItemUsingNum)
        return -1;

    lockChnlTbl(chnlTblkey);
    tChnlState = ((chnlTblItem*)(chnlTblHd_p+chnlID))->chnlState;
    ((chnlTblItem*)(chnlTblHd_p+chnlID))->chnlState =  tChnlState | stateMsk;
    unlockChnlTbl(chnlTblkey);
    return 0;
}

int C_ChnlMgr::clrChnlState(u_short chnlID, u_short stateMsk)
{
    u_short tChnlState;
    if(chnlID>=chnlTblItemUsingNum)
        return -1;

    lockChnlTbl(chnlTblkey);
    tChnlState = ((chnlTblItem*)(chnlTblHd_p+chnlID))->chnlState;
    ((chnlTblItem*)(chnlTblHd_p+chnlID))->chnlState =  tChnlState & ~stateMsk;
    unlockChnlTbl(chnlTblkey);
    return 0;
}

int C_ChnlMgr::getChnlState(u_short chnlID)
{
    u_short tChnlState;
    if(chnlID>=chnlTblItemUsingNum)
        return -1;

    lockChnlTbl(chnlTblkey);
    tChnlState = ((chnlTblItem*)(chnlTblHd_p+chnlID))->chnlState;
    unlockChnlTbl(chnlTblkey);
    return tChnlState;
}



