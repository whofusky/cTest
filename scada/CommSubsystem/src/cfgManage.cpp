//
// Created by 张赟亮 on 2018/1/29.
//


#include "cfgManage.h"
#include"tinyxml2.h"

using namespace  tinyxml2;

cfgManage::cfgManage ( u_char Num )
{
    //日志对象初始化
    int tRet = 0;
    tRet = tryLog.Init ( SYS_LOG_PATH, "scada", true, 1, 7, 2 );

    if ( tRet != 0 )
    {
        exit ( -1 );
    }

    tRet = tclLog.Init ( SYS_LOG_PATH, "apdu", true, 1, 30, 2 );

    if ( tRet != 0 )
    {
        tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                         "%s|%s|%d: tclLog.Init is failed, tRet = %d\r\n",
                         __FILE__, __func__, __LINE__, tRet );
        exit ( -1 );
    }

    tRet = didLog.Init ( SYS_LOG_PATH, "did", true, 1, 30, 2 );

    if ( tRet != 0 )
    {
        tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                         "%s|%s|%d: tclLog.Init is failed, tRet = %d\r\n",
                         __FILE__, __func__, __LINE__, tRet );
        exit ( -1 );
    }

    //Num = 20;
    ssnNum = Num;
    memset ( snnUseNum_p, 0, sizeof ( snnUseNum_p ) );

    crtFunDsc ( ssnNum );
    crtStmpCfg ( ssnNum ); //创建了10个会话步配置表


    int tChnId = 0;//通道id

    for ( int i = 0; i < 6; i++ )
    {
        tChnId = 0;

        setModbsQueDataCfg ( i, t_snnCfg[tChnId], ptclFunDsc[tChnId], snnUseNum_p[tChnId], 0 );

        snnUseNum_p[tChnId]++;

        tChnId = 1;

        if ( i == 3 )
        {
            setModbsQueDataCfg ( i, t_snnCfg[tChnId], ptclFunDsc[tChnId], snnUseNum_p[tChnId], 40001 );
        }
        else
        {
            setModbsQueDataCfg ( i, t_snnCfg[tChnId], ptclFunDsc[tChnId], snnUseNum_p[tChnId], 0 );
        }

        snnUseNum_p[tChnId]++;
    }

    tChnId = 0;
    setModbsQueDataCfg ( MC_MBFC_0xFE, t_snnCfg[tChnId], ptclFunDsc[tChnId], snnUseNum_p[tChnId], 0 );
    snnUseNum_p[tChnId]++;

    tChnId = 1;
    setModbsQueDataCfg ( MC_MBFC_0xFE, t_snnCfg[tChnId], ptclFunDsc[tChnId], snnUseNum_p[tChnId], 0 );
    snnUseNum_p[tChnId]++;
    tChnId = 2;

    setStnInrgtnCfg ( t_snnCfg[tChnId], ptclFunDsc[tChnId], snnUseNum_p[tChnId] );
    snnUseNum_p[tChnId]++;

    setAcqevtCfg (    t_snnCfg[tChnId], ptclFunDsc[tChnId], snnUseNum_p[tChnId] );
    snnUseNum_p[tChnId]++;

    setUstartdtCfg (  t_snnCfg[tChnId], ptclFunDsc[tChnId], snnUseNum_p[tChnId] );
    snnUseNum_p[tChnId]++;

    setUstopdtCfg (   t_snnCfg[tChnId], ptclFunDsc[tChnId], snnUseNum_p[tChnId] );
    snnUseNum_p[tChnId]++;

    setTestfrCfg (    t_snnCfg[tChnId], ptclFunDsc[tChnId], snnUseNum_p[tChnId] );
    snnUseNum_p[tChnId]++;

    setNewholdCfg (   t_snnCfg[tChnId], ptclFunDsc[tChnId], snnUseNum_p[tChnId] );
    snnUseNum_p[tChnId]++;

    setDelholdCfg (   t_snnCfg[tChnId], ptclFunDsc[tChnId], snnUseNum_p[tChnId] );
    snnUseNum_p[tChnId]++;


    tChnId = 3;

    setStnInrgtnCfgGLS ( t_snnCfg[tChnId], ptclFunDsc[tChnId], snnUseNum_p[tChnId]  );
    snnUseNum_p[tChnId]++;

    setAcqevtCfgGLS (   t_snnCfg[tChnId], ptclFunDsc[tChnId], snnUseNum_p[tChnId] );
    snnUseNum_p[tChnId]++;

    setUstartdtCfg (  t_snnCfg[tChnId], ptclFunDsc[tChnId], snnUseNum_p[tChnId]  );
    snnUseNum_p[tChnId]++;

    setUstopdtCfg (   t_snnCfg[tChnId], ptclFunDsc[tChnId], snnUseNum_p[tChnId]  );
    snnUseNum_p[tChnId]++;

    setTestfrCfg (    t_snnCfg[tChnId], ptclFunDsc[tChnId], snnUseNum_p[tChnId]  );
    snnUseNum_p[tChnId]++;

    setNewholdCfg (   t_snnCfg[tChnId], ptclFunDsc[tChnId], snnUseNum_p[tChnId]  );
    snnUseNum_p[tChnId]++;

    setDelholdCfg (   t_snnCfg[tChnId], ptclFunDsc[tChnId], snnUseNum_p[tChnId]  );
    snnUseNum_p[tChnId]++;

    setAppSnnCfg();
    //setchnlObjCfg();
    //crtSsnRInfoCfg ( & ( chnlCfg[0].ssnRInfo ), 3 );

    u_int chnlNum = 0;
    tRet = getScadaAppCfgFxml ( CFG_FILE_PATH,     /*xml配置文件(带路径)*/
                                chnlNum,             /*出参:通道总数*/
                                &chnlCfg_p,/*出参:返回非共用内存的配置*/
                                &outChnUnit_p,/*出参:返回共用内存的配置*/
                                &outLcalIps       /*出参:返回本地ip*/
                              );

    if ( tRet < 0 )
    {
        tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                         "%s|%s|%d: getScadaAppCfgFxml return error, tRet = %d\r\n",
                         __FILE__, __func__, __LINE__, tRet );
        exit ( -1 );
    }
    else
    {
        if ( chnlNum != MC_DEF_STN_NUM )
        {
            tryLog.LPrintf ( DEBUG, DEBUG_LEVEL_0,
                             "%s|%s|%d: getScadaAppCfgFxml return chnlNum=[%d], MC_DEF_STN_NUM = %d\r\n",
                             __FILE__, __func__, __LINE__, chnlNum, MC_DEF_STN_NUM );
            exit ( -1 );
        }
    }

    /*
    setSsnReqInfoCfg ( 0, U_NEWHOLD, 104, 0, 0, 0, 0, false, & ( chnlCfg[0].ssnRInfo ) );
    setSsnReqInfoCfg ( 0, U_STARTDT, 104, 1, 0, 0, 0, false, & ( chnlCfg[0].ssnRInfo ) );
    setSsnReqInfoCfg ( 0, STN_INRGTN, 104, 2, 0, 0, 0, true, & ( chnlCfg[0].ssnRInfo ) );

    //crtSsnRInfoCfg ( & ( chnlCfg[0].ssnRInfo ), 1 );
    //setSsnReqInfoCfg ( 0, U_NEWHOLD, 104, 0, 0, 0, 0, false, & ( chnlCfg[0].ssnRInfo ) );

    crtSsnRInfoCfg ( & ( chnlCfg[1].ssnRInfo ), 2 );
    setSsnReqInfoCfg ( 0, MC_MBFC_0x00, 106, 0, 0, 0, 0, false, & ( chnlCfg[1].ssnRInfo ) );
    setSsnReqInfoCfg ( 0, MC_MBFC_0x03, 106, 1, 0, 0, 0, true, & ( chnlCfg[1].ssnRInfo ) );*/
}

cfgManage ::~cfgManage()
{
    printf ( "~cfgManage" );

    for ( int i = 0; i < MC_DEF_STN_NUM; i++ )
    {
        if ( t_snnCfg[i] != NULL ) //T_snnCfg释放
        {
            if ( t_snnCfg[i]->stepCfg_p != NULL )
            {
                for ( u_int j = 0; j < t_snnCfg[i]->stepNum; j++ )
                {
                    if ( t_snnCfg[i]->stepCfg_p[j].reqBody_setNum_p != NULL )
                    {
                        delete [] t_snnCfg[i]->stepCfg_p[j].reqBody_setNum_p;
                    }

                    //释放请求帧空间
                    if ( t_snnCfg[i]->stepCfg_p[j].reqEleVal_pp != NULL )
                    {
                        for ( u_int k = 0; k < t_snnCfg[i]->stepCfg_p[j].reqAPDUNum; k++ )
                        {
                            for ( u_int k1 = APDU_APCI_INDEX; k1 <= ASDU_TSP_INDEX; k1++ )
                            {
                                if ( t_snnCfg[i]->stepCfg_p[j].reqEleVal_pp[k][k1].p_Mem != NULL )
                                {
                                    delete [] t_snnCfg[i]->stepCfg_p[j].reqEleVal_pp[k][k1].p_Mem;
                                }
                            }
                        }

                        delete [] t_snnCfg[i]->stepCfg_p[j].reqEleVal_pp;
                    }

                    if ( t_snnCfg[i]->stepCfg_p[j].respBody_setNum_p != NULL )
                    {
                        delete [] t_snnCfg[i]->stepCfg_p[j].respBody_setNum_p;
                    }

                    //释放响应帧空间
                    if ( t_snnCfg[i]->stepCfg_p[j].respEleVal_pp != NULL )
                    {
                        for ( u_int t = 0; t < t_snnCfg[i]->stepCfg_p[j].respAPDUNum; t++ )
                        {
                            for ( u_int t1 = APDU_APCI_INDEX; t1 <= ASDU_TSP_INDEX; t1++ )
                            {
                                if ( t_snnCfg[i]->stepCfg_p[j].respEleVal_pp[t][t1].p_Mem != NULL )
                                {
                                    delete [] t_snnCfg[i]->stepCfg_p[j].respEleVal_pp[t][t1].p_Mem;
                                }
                            }
                        }

                        delete [] t_snnCfg[i]->stepCfg_p[j].respEleVal_pp;
                    }

                    if ( t_snnCfg[i]->stepCfg_p[j].reqData_p != NULL )
                    {
                        for ( u_int t = 0; t < t_snnCfg[i]->stepCfg_p[j].reqAPDUNum; t++ )
                        {
                            if ( t_snnCfg[i]->stepCfg_p[j].reqData_p->asduTid.p_Mem != NULL )
                            {
                                delete [] t_snnCfg[i]->stepCfg_p[j].reqData_p->asduTid.p_Mem;
                            }

                            if ( t_snnCfg[i]->stepCfg_p[j].reqData_p->asduCot.p_Mem != NULL )
                            {
                                delete [] t_snnCfg[i]->stepCfg_p[j].reqData_p->asduCot.p_Mem;
                            }

                            if ( t_snnCfg[i]->stepCfg_p[j].reqData_p->asduLng.p_Mem != NULL )
                            {
                                delete [] t_snnCfg[i]->stepCfg_p[j].reqData_p->asduLng.p_Mem;
                            }

                            if ( t_snnCfg[i]->stepCfg_p[j].reqData_p->asduAddr.p_Mem != NULL )
                            {
                                delete [] t_snnCfg[i]->stepCfg_p[j].reqData_p->asduAddr.p_Mem;
                            }



                            if ( t_snnCfg[i]->stepCfg_p[j].reqData_p->asduTsp.p_Mem != NULL )
                            {
                                delete [] t_snnCfg[i]->stepCfg_p[j].reqData_p->asduTsp.p_Mem;
                            }

                            ASDU_VSQ_VAL* vsq = ( ASDU_VSQ_VAL* ) ( t_snnCfg[i]->stepCfg_p[j].reqData_p->asduVsq.p_Mem );

                            for ( int m = 0; m < vsq->N; i++ )
                            {
                                delete t_snnCfg[i]->stepCfg_p[j].reqData_p->bodyObj_p[m].body_Addr.p_Mem;
                                delete t_snnCfg[i]->stepCfg_p[j].reqData_p->bodyObj_p[m].body_Set.p_Mem;
                                delete t_snnCfg[i]->stepCfg_p[j].reqData_p->bodyObj_p[m].body_Tid.p_Mem;
                                delete t_snnCfg[i]->stepCfg_p[j].reqData_p->bodyObj_p[m].body_Tsp.p_Mem;
                            }

                            delete [] t_snnCfg[i]->stepCfg_p[j].reqData_p->bodyObj_p;

                            if ( t_snnCfg[i]->stepCfg_p[j].reqData_p->asduVsq.p_Mem != NULL )
                            {
                                delete [] t_snnCfg[i]->stepCfg_p[j].reqData_p->asduVsq.p_Mem;
                            }
                        }

                        delete [] t_snnCfg[i]->stepCfg_p[j].reqData_p;
                    }

                    if ( t_snnCfg[i]->stepCfg_p[j].respData_p != NULL )
                    {
                        for ( u_int t = 0; t < t_snnCfg[i]->stepCfg_p[j].reqAPDUNum; t++ )
                        {
                            if ( t_snnCfg[i]->stepCfg_p[j].respData_p->asduTid.p_Mem != NULL )
                            {
                                delete [] t_snnCfg[i]->stepCfg_p[j].respData_p->asduTid.p_Mem;
                            }

                            if ( t_snnCfg[i]->stepCfg_p[j].respData_p->asduCot.p_Mem != NULL )
                            {
                                delete [] t_snnCfg[i]->stepCfg_p[j].respData_p->asduCot.p_Mem;
                            }

                            if ( t_snnCfg[i]->stepCfg_p[j].respData_p->asduLng.p_Mem != NULL )
                            {
                                delete [] t_snnCfg[i]->stepCfg_p[j].respData_p->asduLng.p_Mem;
                            }

                            if ( t_snnCfg[i]->stepCfg_p[j].respData_p->asduAddr.p_Mem != NULL )
                            {
                                delete [] t_snnCfg[i]->stepCfg_p[j].respData_p->asduAddr.p_Mem;
                            }



                            if ( t_snnCfg[i]->stepCfg_p[j].respData_p->asduTsp.p_Mem != NULL )
                            {
                                delete [] t_snnCfg[i]->stepCfg_p[j].respData_p->asduTsp.p_Mem;
                            }

                            ASDU_VSQ_VAL* vsq = ( ASDU_VSQ_VAL* ) ( t_snnCfg[i]->stepCfg_p[j].respData_p->asduVsq.p_Mem );

                            for ( int n = 0; n < vsq->N; i++ )
                            {
                                delete t_snnCfg[i]->stepCfg_p[j].respData_p->bodyObj_p[n].body_Addr.p_Mem;
                                delete t_snnCfg[i]->stepCfg_p[j].respData_p->bodyObj_p[n].body_Set.p_Mem;
                                delete t_snnCfg[i]->stepCfg_p[j].respData_p->bodyObj_p[n].body_Tid.p_Mem;
                                delete t_snnCfg[i]->stepCfg_p[j].respData_p->bodyObj_p[n].body_Tsp.p_Mem;
                            }

                            delete [] t_snnCfg[i]->stepCfg_p[j].respData_p->bodyObj_p;

                            if ( t_snnCfg[i]->stepCfg_p[j].respData_p->asduVsq.p_Mem != NULL )
                            {
                                delete [] t_snnCfg[i]->stepCfg_p[j].respData_p->asduVsq.p_Mem;
                            }
                        }

                        delete [] t_snnCfg[i]->stepCfg_p[j].respData_p;
                    }
                }//end for stepNum

                delete [] t_snnCfg[i]->stepCfg_p;
                t_snnCfg[i]->stepCfg_p = NULL;
                t_snnCfg[i]->stepNum = 0;
            }//end t_snnCfg->stepCfg_p
        }

        //delete [] t_snnCfg;//t_snnCfg 不在 createRl_P 中创建，所以不在此释放
        if ( ptclFunDsc[i] != NULL ) //T_ptclFunDsc 释放
        {
            if ( ptclFunDsc[i]->remoteSet_p != NULL )
            {
                for ( int n = 0; n < ptclFunDsc[i]->setNum; i++ )
                {
                    if ( ptclFunDsc[i]->remoteSet_p[n].TID_p != NULL )
                    {
                        delete [] ptclFunDsc[i]->remoteSet_p[n].TID_p;
                        ptclFunDsc[i]->remoteSet_p[n].TID_p = NULL;
                    }

                    if ( ptclFunDsc[i]->remoteSet_p[n].TIDLng_p != NULL )
                    {
                        delete [] ptclFunDsc[i]->remoteSet_p[n].TIDLng_p;
                        ptclFunDsc[i]->remoteSet_p[n].TIDLng_p = NULL;
                    }
                }

                delete [] ptclFunDsc[i]->remoteSet_p;
                ptclFunDsc[i]->remoteSet_p = NULL;
                ptclFunDsc[i]->setNum = 0;
            }

            //delete [] ptclFunDsc;//ptclFunDsc 不在 createRl_P 中创建，所以不在此释放
        }

    }
}

//给t_snnCfg的MC_DEF_STN_NUM个元素分配空间
void cfgManage::crtStmpCfg ( char snnNum )
{
    for ( int i = 0; i < MC_DEF_STN_NUM && snnNum > 0; i++ )
    {
        t_snnCfg[i] = new T_snnCfg[snnNum]();
        memset ( t_snnCfg[i], 0, sizeof ( T_snnCfg ) * snnNum );
    }
}

//设置某一个会话并给步结构开空间
void cfgManage::setCfg ( T_snnCfg* snnCfg, u_char ssnID, u_char actFlag, u_char infoType, u_short cmnAddr, u_char stepNum )
{

    snnCfg->ssnID     = ssnID;
    snnCfg->actFlag   = actFlag;
    snnCfg->infoType  = infoType;
    snnCfg->cmnAddr   = cmnAddr;
    snnCfg->stepNum   = stepNum;
    snnCfg->stepCfg_p = new T_ssnStepCfg[stepNum]();
}

//设置某一步的主被动,帧数并给帧结构开空间
void cfgManage::setStepCfg ( T_ssnStepCfg* ssnStepCfg, u_short stnAcsFlag, u_char reqAPDUNum, u_char respAPDUNum )
{
    ssnStepCfg->stnAcsFlag  = stnAcsFlag;
    ssnStepCfg->reqAPDUNum  = reqAPDUNum;

    if ( reqAPDUNum != 0 )
    {
        ssnStepCfg->reqBody_setNum_p = new u_char[reqAPDUNum]();
        ssnStepCfg->reqData_p        = new dataFrame[reqAPDUNum]();
    }

    ssnStepCfg->respAPDUNum = respAPDUNum;

    if ( respAPDUNum != 0 )
    {
        ssnStepCfg->respBody_setNum_p = new u_char[respAPDUNum]();
        ssnStepCfg->respData_p        = new dataFrame[respAPDUNum]();
    }

}

//modbus设置某一步的主被动,帧数并给帧结构开空间
void cfgManage::setModbsStepCfg ( T_ssnStepCfg* ssnStepCfg, u_short stnAcsFlag, u_char reqAPDUNum, u_char respAPDUNum )
{
    ssnStepCfg->stnAcsFlag  = stnAcsFlag;
    ssnStepCfg->reqAPDUNum  = reqAPDUNum;

    if ( reqAPDUNum != 0 )
    {
        ssnStepCfg->reqBody_setNum_p = new u_char[reqAPDUNum]();
        ssnStepCfg->comReqData_p     = new T_comFrame[reqAPDUNum]();
    }

    ssnStepCfg->respAPDUNum = respAPDUNum;

    if ( respAPDUNum != 0 )
    {
        ssnStepCfg->respBody_setNum_p = new u_char[respAPDUNum]();
        ssnStepCfg->comRespData_p     = new T_comFrame[respAPDUNum]();
    }

}

//帧结构的信息对象开空间
void cfgManage::setData ( dataFrame* data, u_char objNum )
{
    data->bodyObj_p = new BODY_OBJ[objNum];
}


//ELE_TYPE元素拷贝
void cfgManage::setEleVal ( ELE_TYPE* destEleVal, ELE_TYPE srcEleVal )
{
    destEleVal->lng      = srcEleVal.lng;
    destEleVal->sysEleID = srcEleVal.sysEleID;

    if ( srcEleVal.lng != 0 )
    {
        destEleVal->p_Mem    = new u_char[srcEleVal.lng]();
        memcpy ( destEleVal->p_Mem, srcEleVal.p_Mem, srcEleVal.lng );
    }
}

//申请T_ptclFunDsc结构空间
void cfgManage::crtFunDsc ( char ssnNum )
{
    for ( int i = 0; i < MC_DEF_STN_NUM; i++ )
    {
        ptclFunDsc[i] = new T_ptclFunDsc[ssnNum]();
    }

}


//设置某一会话功能描述并给步描述结构开空间
void cfgManage::setFunDsc ( T_ptclFunDsc* funDsc, u_short ssnDscNo, u_short act_prio_chnlID,
                            u_char usingMode, u_char setNum )
{
    funDsc->ssnDscNo        = ssnDscNo;
    funDsc->act_prio_chnlID = act_prio_chnlID;
    funDsc->usingMode       = usingMode;
    funDsc->setNum          = setNum;

    if ( setNum != 0 )
    {
        funDsc->remoteSet_p     = new ptclStep[setNum]();
    }

}

//为某一步可能包括的tid与cot开空间
void cfgManage::setPtclStep ( ptclStep* stepDsc_p, u_short COT, u_short MaxLen, u_char TidNum )
{
    stepDsc_p->COT      = COT;
    stepDsc_p->MaxLen   = MaxLen;
    stepDsc_p->TidNum   = TidNum;

    if ( TidNum != 0 )
    {
        stepDsc_p->TID_p    = new u_short[TidNum]();
        stepDsc_p->TIDLng_p = new u_char[TidNum]();
    }
}


//突发会话:规则步及相关配置数据组装
void cfgManage::setAcqevtCfgGLS ( T_snnCfg* snnCfg, T_ptclFunDsc* funDsc, const int& index )
{
    T_stnAcsFlag tmpStnAcsFlag = {0, 0, 1, 1, 1, 0, 1, 1, 1, 0};
    u_char ssnid               = ACQ_EVT;
    u_char actFlig             = 1;//主被动型,由规约设置
    u_char infoType            = GINFOTYPE;//
    u_short cmnAddr            = 1;//公共地址
    u_char stepNum             = 1;//步数
    /*******************************************设置事件收集会话**********************************************/
    setCfg ( & ( snnCfg[index] ), ssnid, actFlig, infoType, cmnAddr, stepNum );
    tmpStnAcsFlag.ctlType  = 2;
    tmpStnAcsFlag.setValid = 1;
    u_char reqAPDUNum      = 1;//请求帧个数
    u_char respAPDUNum     = 0;//响应帧个数
    u_char body_setNum     = 5;//一帧数据中，信息对象或信息元素的个数
    /*******************************************设置事件收集会话的第一步**********************************************/


    u_char memVal[11]           = {};
    BODY_ADDR_VAL  bodyAddr_val = {};
    ELE_TYPE eleVal[11]         =
    {
        {APDU_APCI_INDEX, 0, &memVal[0]},
        {ASDU_TID_INDEX, 1, &memVal[1]},
        {ASDU_LNG_INDEX, 0, &memVal[2]},
        {ASDU_VSQ_INDEX, 1, &memVal[3]},
        {ASDU_COT_INDEX, 2, &memVal[4]},
        {ASDU_ADDR_INDEX, sizeof ( u_short ), ( u_char* )& cmnAddr},
        {BODY_TID_INDEX, 0, &memVal[6]},
        {BODY_ADDR_INDEX, sizeof ( BODY_ADDR_VAL ), ( u_char* )& bodyAddr_val},
        {BODY_SET_INDEX, 5, &memVal[8]},
        {BODY_TSP_INDEX, 0, &memVal[9]},
        {ASDU_TSP_INDEX, 0, &memVal[10]}
    };
    * ( eleVal[ASDU_TID_INDEX].p_Mem ) = 13;
    * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = 0 << 7 | body_setNum;
    * ( eleVal[ASDU_COT_INDEX].p_Mem ) = 3;
    * ( eleVal[BODY_SET_INDEX].p_Mem ) = 0;

    ASDU_VSQ_VAL* vsq = ( ASDU_VSQ_VAL* ) ( eleVal[3].p_Mem );
    vsq->SQ = 0;
    setStepCfg ( & ( snnCfg[index].stepCfg_p[0] ), * ( u_short* ) &tmpStnAcsFlag, reqAPDUNum, respAPDUNum );


    if ( vsq->SQ == 0 )
    {
		snnCfg[index].stepCfg_p[0].reqBody_setNum_p[0] = 1;
        setData ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0] ), body_setNum );
    }
    else
    {
		snnCfg[index].stepCfg_p[0].reqBody_setNum_p[0] = 5;
        setData ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0] ), 1 );
    }
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduTid ), eleVal[1] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduLng ), eleVal[2] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduVsq ), eleVal[3] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduCot ), eleVal[4] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduAddr ), eleVal[5] );

    if ( vsq->SQ == 1 )
    {
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Tid ), eleVal[6] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Addr ), eleVal[7] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Set ), eleVal[8] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Tsp ), eleVal[9] );
    }
    else
    {
        for ( int i = 0; i < vsq->N; i++ )
        {
            bodyAddr_val.shortlngAddr = 16385 + i ;
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Tid ), eleVal[6] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Addr ), eleVal[7] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Set ), eleVal[8] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Tsp ), eleVal[9] );
        }

    }

    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduTsp ), eleVal[10] );
    int usingMode = 0;
    int cot       = 3;
    int tid_lng   = 5;
    int tid       = 13;
    setFunDsc ( & ( funDsc[index] ), ACQ_EVT, PASSIVE_TYPE, usingMode, stepNum );

    setPtclStep ( ( &funDsc[index].remoteSet_p[0] ), cot, 249, 2 );
    funDsc[index].remoteSet_p[0].TIDLng_p[0] = tid_lng;// 这个数组的大小取决于remoteSet_p->setNum的值
    funDsc[index].remoteSet_p[0].TID_p[0]    = tid;
    tid_lng   =  1;
    tid       = 1;
    funDsc[index].remoteSet_p[0].TIDLng_p[1] = tid_lng;// 这个数组的大小取决于remoteSet_p->setNum的值
    funDsc[index].remoteSet_p[0].TID_p[1]    = tid;
}

void cfgManage::setAcqevtCfg ( T_snnCfg* snnCfg, T_ptclFunDsc* funDsc, const int& index )
{
    T_stnAcsFlag tmpStnAcsFlag = {0, 0, 1, 1, 1, 0, 1, 1, 1, 0};
    u_char ssnid               = ACQ_EVT;
    u_char actFlig             = 1;//主被动型,由规约设置
    u_char infoType            = GINFOTYPE;//
    u_short cmnAddr            = 1;//公共地址
    u_char stepNum             = 1;//步数
    /*******************************************设置事件收集会话**********************************************/
    setCfg ( & ( snnCfg[index] ), ssnid, actFlig, infoType, cmnAddr, stepNum );
    tmpStnAcsFlag.ctlType  = 2;
    tmpStnAcsFlag.setValid = 1;
    u_char reqAPDUNum      = 1;//请求帧个数
    u_char respAPDUNum     = 0;//响应帧个数
    u_char body_setNum     = 5;//一帧数据中，信息对象或信息元素的个数
    /*******************************************设置事件收集会话的第一步**********************************************/


    u_char memVal[11]           = {};
    BODY_ADDR_VAL  bodyAddr_val = {};
    bodyAddr_val.shortAddr      = 4;
    ELE_TYPE eleVal[11]         =
    {
        {APDU_APCI_INDEX, 0, &memVal[0]},
        {ASDU_TID_INDEX, 1, &memVal[1]},
        {ASDU_LNG_INDEX, 0, &memVal[2]},
        {ASDU_VSQ_INDEX, 1, &memVal[3]},
        {ASDU_COT_INDEX, 2, &memVal[4]},
        {ASDU_ADDR_INDEX, sizeof ( u_short ), ( u_char* )& cmnAddr},
        {BODY_TID_INDEX, 0, &memVal[6]},
        {BODY_ADDR_INDEX, sizeof ( BODY_ADDR_VAL ), ( u_char* )& bodyAddr_val},
        {BODY_SET_INDEX, 4, &memVal[8]},
        {BODY_TSP_INDEX, 0, &memVal[9]},
        {ASDU_TSP_INDEX, 0, &memVal[10]}
    };
    * ( eleVal[ASDU_TID_INDEX].p_Mem ) = 13;
    * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = 0 << 7 | body_setNum;
    * ( eleVal[ASDU_COT_INDEX].p_Mem ) = 3;
    * ( eleVal[BODY_SET_INDEX].p_Mem ) = 0;
    ASDU_VSQ_VAL* vsq = ( ASDU_VSQ_VAL* ) ( eleVal[3].p_Mem );
    setStepCfg ( & ( snnCfg[index].stepCfg_p[0] ), * ( u_short* ) &tmpStnAcsFlag, reqAPDUNum, respAPDUNum );

	if ( vsq->SQ == 0 )
	{
		snnCfg[index].stepCfg_p[0].reqBody_setNum_p[0] = 1;
		setData ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0] ), body_setNum );
	}
	else
	{
		snnCfg[index].stepCfg_p[0].reqBody_setNum_p[0] = body_setNum;
		setData ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0] ), 1 );
	}

    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduTid ), eleVal[1] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduLng ), eleVal[2] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduVsq ), eleVal[3] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduCot ), eleVal[4] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduAddr ), eleVal[5] );

    if ( vsq->SQ == 1 )
    {
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Tid ), eleVal[6] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Addr ), eleVal[7] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Set ), eleVal[8] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Tsp ), eleVal[9] );
    }
    else
    {
        for ( int i = 0; i < vsq->N; i++ )
        {
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Tid ), eleVal[6] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Addr ), eleVal[7] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Set ), eleVal[8] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Tsp ), eleVal[9] );
        }

    }

    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduTsp ), eleVal[10] );
    int usingMode = 0;
    int cot       = 3;
    int tid_lng   = 5;
    int tid       = 13;
    setFunDsc ( & ( funDsc[index] ), ACQ_EVT, PASSIVE_TYPE, usingMode, stepNum );
    setPtclStep ( ( &funDsc[index].remoteSet_p[0] ), cot, 249, 2 );
    funDsc[index].remoteSet_p[0].TIDLng_p[0] = tid_lng;// 这个数组的大小取决于remoteSet_p->setNum的值
    funDsc[index].remoteSet_p[0].TID_p[0]    = tid;
    tid_lng   =  1;
    tid       = 1;
    funDsc[index].remoteSet_p[0].TIDLng_p[1] = tid_lng;// 这个数组的大小取决于remoteSet_p->setNum的值
    funDsc[index].remoteSet_p[0].TID_p[1]    = tid;
}

void cfgManage::setStnInrgtnCfgGLS ( T_snnCfg* snnCfg, T_ptclFunDsc* funDsc, const int& index  )
{
    T_stnAcsFlag tmpStnAcsFlag = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    u_char ssnid               = STN_INRGTN;
    u_char actFlig             = 0;//主被动型,由规约设置
    u_char infoType            = GINFOTYPE;//
    u_short cmnAddr            = 2;//公共地址
    u_char stepNum             = 4;//步数
    /*******************************************设置站召唤会话**********************************************/
    setCfg ( & ( snnCfg[index] ), ssnid, actFlig, infoType, cmnAddr, stepNum );

    u_char reqAPDUNum  = 1;//请求帧个数
    u_char respAPDUNum = 0;//响应帧个数
    u_char body_setNum = 1;//一帧数据中，信息对象或信息元素的个数
    /*******************************************设置站召唤会话的第一步**********************************************/


    u_char memVal[11] = {0};
    BODY_ADDR_VAL  bodyAddr_val = {};
    ELE_TYPE eleVal[11] =
    {
        {APDU_APCI_INDEX, 0, &memVal[0]},
        {ASDU_TID_INDEX, 1, &memVal[1]},
        {ASDU_LNG_INDEX, 0, &memVal[2]},
        {ASDU_VSQ_INDEX, 1, &memVal[3]},
        {ASDU_COT_INDEX, 2, &memVal[4]},
        {ASDU_ADDR_INDEX, sizeof ( u_short ), ( u_char* )& cmnAddr},
        {BODY_TID_INDEX, 0, &memVal[6]},
        {BODY_ADDR_INDEX, 2, ( u_char* )& bodyAddr_val},
        {BODY_SET_INDEX, 5, &memVal[8]},
        {BODY_TSP_INDEX, 0, &memVal[9]},
        {ASDU_TSP_INDEX, 0, &memVal[10]}
    };
    //设置站召唤会话的召唤命令
    * ( eleVal[ASDU_TID_INDEX].p_Mem ) = 100;
    * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = 1 << 7 | body_setNum;
    * ( eleVal[ASDU_COT_INDEX].p_Mem ) = 6;
    eleVal[BODY_SET_INDEX].lng = sizeof ( u_char );
    * ( eleVal[BODY_SET_INDEX].p_Mem ) = 20;
    ASDU_VSQ_VAL* vsq = ( ASDU_VSQ_VAL* ) ( eleVal[3].p_Mem );
    //请求帧=1，响应帧=0
    setStepCfg ( & ( snnCfg[index].stepCfg_p[0] ), * ( u_short* ) &tmpStnAcsFlag, reqAPDUNum, respAPDUNum );
    
	if ( vsq->SQ == 0 )
	{
		snnCfg[index].stepCfg_p[0].reqBody_setNum_p[0] = 1;
		setData ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0] ), body_setNum );
	}
	else
	{
		snnCfg[index].stepCfg_p[0].reqBody_setNum_p[0] = body_setNum;
		setData ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0] ), 1 );
	}
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduTid ), eleVal[1] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduLng ), eleVal[2] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduVsq ), eleVal[3] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduCot ), eleVal[4] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduAddr ), eleVal[5] );

    if ( vsq->SQ == 1 )
    {
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Tid ), eleVal[6] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Addr ), eleVal[7] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Set ), eleVal[8] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Tsp ), eleVal[9] );
    }
    else
    {
        for ( int i = 0; i < vsq->N; i++ )
        {
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Tid ), eleVal[6] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Addr ), eleVal[7] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Set ), eleVal[8] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Tsp ), eleVal[9] );
        }

    }

    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduTsp ), eleVal[10] );
    /*******************************************设置站召唤会话的第二步**********************************************/
    tmpStnAcsFlag.ctlType = 0;
    memset ( &tmpStnAcsFlag, 0, sizeof ( T_stnAcsFlag ) );
    reqAPDUNum = 0;
    respAPDUNum = 1;
    body_setNum = 1;
    setStepCfg ( & ( snnCfg[index].stepCfg_p[1] ), * ( u_short* ) &tmpStnAcsFlag, reqAPDUNum, respAPDUNum );
	if ( vsq->SQ == 0 )
	{
		snnCfg[index].stepCfg_p[1].respBody_setNum_p[0] = 1;
		setData ( & ( snnCfg[index].stepCfg_p[1].respData_p[0] ), body_setNum );
	}
	else
	{
		snnCfg[index].stepCfg_p[1].respBody_setNum_p[0] = body_setNum;
		setData ( & ( snnCfg[index].stepCfg_p[1].respData_p[0] ), 1 );
	}
    * ( eleVal[ASDU_COT_INDEX].p_Mem ) = 7;
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduTid ), eleVal[1] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduLng ), eleVal[2] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduVsq ), eleVal[3] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduCot ), eleVal[4] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduAddr ), eleVal[5] );

    if ( vsq->SQ == 1 )
    {
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Tid ), eleVal[6] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Addr ), eleVal[7] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Set ), eleVal[8] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Tsp ), eleVal[9] );
    }
    else
    {
        for ( int i = 0; i < vsq->N; i++ )
        {
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Tid ), eleVal[6] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Addr ), eleVal[7] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Set ), eleVal[8] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Tsp ), eleVal[9] );
        }

    }

    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduTsp ), eleVal[10] );
    /*******************************************设置站召唤会话的第三步**********************************************/
    tmpStnAcsFlag.ctlType = 1;
    tmpStnAcsFlag.setValid = 1;
    reqAPDUNum = 0;
    respAPDUNum = 1;
    body_setNum = 5;
    * ( eleVal[ASDU_TID_INDEX].p_Mem ) = 13;
    * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = 1 << 7 | body_setNum;
    * ( eleVal[ASDU_COT_INDEX].p_Mem ) = 20;
    eleVal[BODY_SET_INDEX].lng = sizeof ( u_short );
    * ( eleVal[BODY_SET_INDEX].p_Mem ) = 0;
    bodyAddr_val.shortlngAddr = 16385;
    //响应帧第一帧，如果作为主站本步需要给出该步的帧数，其他的不需要配置，如作为被动站则需要
    //float bodyVlu = 3.1415;
    setStepCfg ( & ( snnCfg[index].stepCfg_p[2] ), * ( u_short* ) &tmpStnAcsFlag, reqAPDUNum, respAPDUNum );
	if ( vsq->SQ == 0 )
	{
		snnCfg[index].stepCfg_p[2].respBody_setNum_p[0] = 1;
		setData ( & ( snnCfg[index].stepCfg_p[2].respData_p[0] ), body_setNum );
	}
	else
	{
		snnCfg[index].stepCfg_p[2].respBody_setNum_p[0] = body_setNum;
		setData ( & ( snnCfg[index].stepCfg_p[2].respData_p[0] ), 1 );
	}
    //eleVal[BODY_SET_INDEX].p_Mem = ( u_char* ) &bodyVlu;
    setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[0].asduTid ), eleVal[1] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[0].asduLng ), eleVal[2] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[0].asduVsq ), eleVal[3] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[0].asduCot ), eleVal[4] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[0].asduAddr ), eleVal[5] );

    if ( vsq->SQ == 1 )
    {
        setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[0].bodyObj_p[0].body_Tid ), eleVal[6] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[0].bodyObj_p[0].body_Addr ), eleVal[7] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[0].bodyObj_p[0].body_Set ), eleVal[8] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[0].bodyObj_p[0].body_Tsp ), eleVal[9] );
    }
    else
    {
        for ( int i = 0; i < vsq->N; i++ )
        {
            setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[0].bodyObj_p[i].body_Tid ), eleVal[6] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[0].bodyObj_p[i].body_Addr ), eleVal[7] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[0].bodyObj_p[i].body_Set ), eleVal[8] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[0].bodyObj_p[i].body_Tsp ), eleVal[9] );
        }

    }

    setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[0].asduTsp ), eleVal[10] );
    /*******************************************设置站召唤会话的第四步**********************************************/
    tmpStnAcsFlag.ctlType = 0;
    tmpStnAcsFlag.setValid = 1;
    reqAPDUNum = 0;
    respAPDUNum = 1;
    body_setNum = 1;
	bodyAddr_val.shortlngAddr = 0;
    * ( eleVal[ASDU_TID_INDEX].p_Mem ) = 100;
    * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = 1 << 7 | body_setNum;
    * ( eleVal[ASDU_COT_INDEX].p_Mem ) = 10;
    eleVal[BODY_SET_INDEX].lng = sizeof ( u_char );
    * ( eleVal[BODY_SET_INDEX].p_Mem ) = 20;
    setStepCfg ( & ( snnCfg[index].stepCfg_p[3] ), * ( u_short* ) &tmpStnAcsFlag, reqAPDUNum, respAPDUNum );
	if ( vsq->SQ == 0 )
	{
		snnCfg[index].stepCfg_p[3].respBody_setNum_p[0] = 1;
		setData ( & ( snnCfg[index].stepCfg_p[3].respData_p[0] ), body_setNum );
	}
	else
	{
		snnCfg[index].stepCfg_p[3].respBody_setNum_p[0] = body_setNum;
		setData ( & ( snnCfg[index].stepCfg_p[3].respData_p[0] ), 1 );
	}
    setEleVal ( & ( snnCfg[index].stepCfg_p[3].respData_p[0].asduTid ), eleVal[1] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[3].respData_p[0].asduLng ), eleVal[2] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[3].respData_p[0].asduVsq ), eleVal[3] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[3].respData_p[0].asduCot ), eleVal[4] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[3].respData_p[0].asduAddr ), eleVal[5] );

    if ( vsq->SQ == 1 )
    {
        setEleVal ( & ( snnCfg[index].stepCfg_p[3].respData_p[0].bodyObj_p[0].body_Tid ), eleVal[6] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[3].respData_p[0].bodyObj_p[0].body_Addr ), eleVal[7] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[3].respData_p[0].bodyObj_p[0].body_Set ), eleVal[8] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[3].respData_p[0].bodyObj_p[0].body_Tsp ), eleVal[9] );
    }
    else
    {
        for ( int i = 0; i < vsq->N; i++ )
        {
            setEleVal ( & ( snnCfg[index].stepCfg_p[3].respData_p[0].bodyObj_p[i].body_Tid ), eleVal[6] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[3].respData_p[0].bodyObj_p[i].body_Addr ), eleVal[7] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[3].respData_p[0].bodyObj_p[i].body_Set ), eleVal[8] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[3].respData_p[0].bodyObj_p[i].body_Tsp ), eleVal[9] );
        }

    }

    setEleVal ( & ( snnCfg[index].stepCfg_p[3].respData_p[0].asduTsp ), eleVal[10] );
    int usingMode = 0;
    int cot = 6;
    int tid_lng = 1;
    int tid = 100;
    setFunDsc ( & ( funDsc[index] ), STN_INRGTN, ACTIVE_TYPE, usingMode, stepNum );

    setPtclStep ( & ( funDsc[index].remoteSet_p[0] ), cot, 249, 1 );
    funDsc[index].remoteSet_p[0].TIDLng_p[0] = tid_lng;// 这个数组的大小取决于remoteSet_p->setNum的值
    funDsc[index].remoteSet_p[0].TID_p[0] = tid;

    cot = 7;
    tid_lng = 1;
    tid = 100;
    setPtclStep ( & ( funDsc[index].remoteSet_p[1] ), cot, 249, 1 );
    funDsc[index].remoteSet_p[1].TIDLng_p[0] = tid_lng;// 这个数组的大小取决于remoteSet_p->setNum的值
    funDsc[index].remoteSet_p[1].TID_p[0] = tid;

    cot = 20;
    tid_lng = 1;
    tid = 1;
    setPtclStep ( & ( funDsc[index].remoteSet_p[2] ), cot, 249, 2 );
    funDsc[index].remoteSet_p[2].TIDLng_p[0] = tid_lng;// 这个数组的大小取决于remoteSet_p->setNum的值
    funDsc[index].remoteSet_p[2].TID_p[0] = tid;

    tid_lng = 5;
    tid = 13;
    funDsc[index].remoteSet_p[2].TIDLng_p[1] = tid_lng;// 这个数组的大小取决于remoteSet_p->setNum的值
    funDsc[index].remoteSet_p[2].TID_p[1] = tid;

    cot = 10;
    tid_lng = 1;
    tid = 100;
    setPtclStep ( & ( funDsc[index].remoteSet_p[3] ), cot, 249, 1 );
    funDsc[index].remoteSet_p[3].TIDLng_p[0] = tid_lng;// 这个数组的大小取决于remoteSet_p->setNum的值
    funDsc[index].remoteSet_p[3].TID_p[0] = tid;
}


//站召唤会话:规则步及相关配置数据组装
void cfgManage::setStnInrgtnCfg ( T_snnCfg* snnCfg, T_ptclFunDsc* funDsc, const int& index  )
{
    T_stnAcsFlag tmpStnAcsFlag = {0, 0, 1, 1, 1, 0, 1, 1, 1, 0};
    u_char ssnid               = STN_INRGTN;
    u_char actFlig             = 0;//主被动型,由规约设置
    u_char infoType            = GINFOTYPE;//
    u_short cmnAddr            = 2;//公共地址
    u_char stepNum             = 4;//步数
    /*******************************************设置站召唤会话**********************************************/
    setCfg ( & ( snnCfg[index] ), ssnid, actFlig, infoType, cmnAddr, stepNum );

    u_char reqAPDUNum  = 1;//请求帧个数
    u_char respAPDUNum = 0;//响应帧个数
    u_char body_setNum = 1;//一帧数据中，信息对象或信息元素的个数
    /*******************************************设置站召唤会话的第一步**********************************************/


    u_char memVal[11] = {0};
    BODY_ADDR_VAL  bodyAddr_val = {};
    ELE_TYPE eleVal[11] =
    {
        {APDU_APCI_INDEX, 0, &memVal[0]},
        {ASDU_TID_INDEX, 1, &memVal[1]},
        {ASDU_LNG_INDEX, 0, &memVal[2]},
        {ASDU_VSQ_INDEX, 1, &memVal[3]},
        {ASDU_COT_INDEX, 2, &memVal[4]},
        {ASDU_ADDR_INDEX, sizeof ( u_short ), ( u_char* )& cmnAddr},
        {BODY_TID_INDEX, 0, &memVal[6]},
        {BODY_ADDR_INDEX, sizeof ( BODY_ADDR_VAL ), ( u_char* )& bodyAddr_val},
        {BODY_SET_INDEX, 4, &memVal[8]},
        {BODY_TSP_INDEX, 0, &memVal[9]},
        {ASDU_TSP_INDEX, 0, &memVal[10]}
    };
    //设置站召唤会话的召唤命令
    * ( eleVal[ASDU_TID_INDEX].p_Mem ) = 100;
     * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = 1 << 7 | body_setNum;
    * ( eleVal[ASDU_COT_INDEX].p_Mem ) = 6;
    eleVal[BODY_SET_INDEX].lng = sizeof ( u_char );
    * ( eleVal[BODY_SET_INDEX].p_Mem ) = 20;
    ASDU_VSQ_VAL* vsq = ( ASDU_VSQ_VAL* ) ( eleVal[3].p_Mem );
    //请求帧=1，响应帧=0
    setStepCfg ( & ( snnCfg[index].stepCfg_p[0] ), * ( u_short* ) &tmpStnAcsFlag, reqAPDUNum, respAPDUNum );
	if ( vsq->SQ == 0 )
	{
		snnCfg[index].stepCfg_p[0].reqBody_setNum_p[0] = 1;
		setData ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0] ), body_setNum );
	}
	else
	{
		snnCfg[index].stepCfg_p[0].reqBody_setNum_p[0] = body_setNum;
		setData ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0] ), 1 );
	}
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduTid ), eleVal[1] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduLng ), eleVal[2] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduVsq ), eleVal[3] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduCot ), eleVal[4] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduAddr ), eleVal[5] );

    if ( vsq->SQ == 1 )
    {
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Tid ), eleVal[6] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Addr ), eleVal[7] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Set ), eleVal[8] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Tsp ), eleVal[9] );
    }
    else
    {
        for ( int i = 0; i < vsq->N; i++ )
        {
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Tid ), eleVal[6] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Addr ), eleVal[7] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Set ), eleVal[8] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Tsp ), eleVal[9] );
        }

    }

    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduTsp ), eleVal[10] );
    /*******************************************设置站召唤会话的第二步**********************************************/
    tmpStnAcsFlag.ctlType = 0;
    reqAPDUNum = 0;
    respAPDUNum = 1;
    body_setNum = 1;
    setStepCfg ( & ( snnCfg[index].stepCfg_p[1] ), * ( u_short* ) &tmpStnAcsFlag, reqAPDUNum, respAPDUNum );
	if ( vsq->SQ == 0 )
	{
		snnCfg[index].stepCfg_p[1].respBody_setNum_p[0] = 1;
		setData ( & ( snnCfg[index].stepCfg_p[1].respData_p[0] ), body_setNum );
	}
	else
	{
		snnCfg[index].stepCfg_p[1].respBody_setNum_p[0] = body_setNum;
		setData ( & ( snnCfg[index].stepCfg_p[1].respData_p[0] ), 1 );
	}
    * ( eleVal[ASDU_COT_INDEX].p_Mem ) = 7;
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduTid ), eleVal[1] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduLng ), eleVal[2] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduVsq ), eleVal[3] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduCot ), eleVal[4] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduAddr ), eleVal[5] );

    if ( vsq->SQ == 1 )
    {
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Tid ), eleVal[6] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Addr ), eleVal[7] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Set ), eleVal[8] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Tsp ), eleVal[9] );
    }
    else
    {
        for ( int i = 0; i < vsq->N; i++ )
        {
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Tid ), eleVal[6] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Addr ), eleVal[7] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Set ), eleVal[8] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Tsp ), eleVal[9] );
        }

    }

    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduTsp ), eleVal[10] );
    /*******************************************设置站召唤会话的第三步**********************************************/
    tmpStnAcsFlag.ctlType = 1;
    tmpStnAcsFlag.setValid = 1;
    reqAPDUNum = 0;
    respAPDUNum = 4;
    body_setNum = 40;
    * ( eleVal[ASDU_TID_INDEX].p_Mem ) = 13;
    * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = 1 << 7 | 20;
    * ( eleVal[ASDU_COT_INDEX].p_Mem ) = 20;
    eleVal[BODY_SET_INDEX].lng = sizeof ( u_short );
    * ( eleVal[BODY_SET_INDEX].p_Mem ) = 0;
    bodyAddr_val.shortlngAddr = 4001;
    //响应帧第一帧，如果作为主站本步需要给出该步的帧数，其他的不需要配置，如作为被动站则需要
    // float bodyVlu = 3.1415;
    setStepCfg ( & ( snnCfg[index].stepCfg_p[2] ), * ( u_short* ) &tmpStnAcsFlag, reqAPDUNum, respAPDUNum );
	if ( vsq->SQ == 0 )
	{
		snnCfg[index].stepCfg_p[2].respBody_setNum_p[0] = 1;
		setData ( & ( snnCfg[index].stepCfg_p[2].respData_p[0] ), body_setNum );
	}
	else
	{
		snnCfg[index].stepCfg_p[2].respBody_setNum_p[0] = body_setNum;
		setData ( & ( snnCfg[index].stepCfg_p[2].respData_p[0] ), 1 );
	}
    //eleVal[BODY_SET_INDEX].p_Mem = ( u_char* ) &bodyVlu;
    setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[0].asduTid ), eleVal[1] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[0].asduLng ), eleVal[2] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[0].asduVsq ), eleVal[3] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[0].asduCot ), eleVal[4] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[0].asduAddr ), eleVal[5] );

    if ( vsq->SQ == 1 )
    {
        setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[0].bodyObj_p[0].body_Tid ), eleVal[6] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[0].bodyObj_p[0].body_Addr ), eleVal[7] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[0].bodyObj_p[0].body_Set ), eleVal[8] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[0].bodyObj_p[0].body_Tsp ), eleVal[9] );
    }
    else
    {
        for ( int i = 0; i < vsq->N; i++ )
        {
            setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[0].bodyObj_p[i].body_Tid ), eleVal[6] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[0].bodyObj_p[i].body_Addr ), eleVal[7] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[0].bodyObj_p[i].body_Set ), eleVal[8] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[0].bodyObj_p[i].body_Tsp ), eleVal[9] );
        }

    }

    setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[0].asduTsp ), eleVal[10] );
    //响应帧第二帧
    // bodyVlu = 321.123;
	if ( vsq->SQ == 0 )
	{
		snnCfg[index].stepCfg_p[2].respBody_setNum_p[1] = 1;
		setData ( & ( snnCfg[index].stepCfg_p[2].respData_p[1] ), body_setNum );
	}
	else
	{
		snnCfg[index].stepCfg_p[2].respBody_setNum_p[1] = body_setNum;
		setData ( & ( snnCfg[index].stepCfg_p[2].respData_p[1] ), 1 );
	}
    //eleVal[BODY_SET_INDEX].p_Mem = ( u_char* ) &bodyVlu;
    setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[1].asduTid ), eleVal[1] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[1].asduLng ), eleVal[2] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[1].asduVsq ), eleVal[3] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[1].asduCot ), eleVal[4] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[1].asduAddr ), eleVal[5] );

    if ( vsq->SQ == 1 )
    {
        setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[1].bodyObj_p[0].body_Tid ), eleVal[6] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[1].bodyObj_p[0].body_Addr ), eleVal[7] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[1].bodyObj_p[0].body_Set ), eleVal[8] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[1].bodyObj_p[0].body_Tsp ), eleVal[9] );
    }
    else
    {
        for ( int i = 0; i < vsq->N; i++ )
        {
            setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[1].bodyObj_p[i].body_Tid ), eleVal[6] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[1].bodyObj_p[i].body_Addr ), eleVal[7] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[1].bodyObj_p[i].body_Set ), eleVal[8] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[1].bodyObj_p[i].body_Tsp ), eleVal[9] );
        }

    }

    setEleVal ( & ( snnCfg[index].stepCfg_p[2].respData_p[1].asduTsp ), eleVal[10] );
    /*******************************************设置站召唤会话的第四步**********************************************/
    tmpStnAcsFlag.ctlType = 2;
    tmpStnAcsFlag.setValid = 1;
    reqAPDUNum = 0;
    respAPDUNum = 1;
    body_setNum = 1;

    * ( eleVal[ASDU_TID_INDEX].p_Mem ) = 100;
    * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = 1 << 7 | body_setNum;
    * ( eleVal[ASDU_COT_INDEX].p_Mem ) = 10;
    eleVal[BODY_SET_INDEX].lng = sizeof ( u_char );
    * ( eleVal[BODY_SET_INDEX].p_Mem ) = 20;
    setStepCfg ( & ( snnCfg[index].stepCfg_p[3] ), * ( u_short* ) &tmpStnAcsFlag, reqAPDUNum, respAPDUNum );
	if ( vsq->SQ == 0 )
	{
		snnCfg[index].stepCfg_p[3].respBody_setNum_p[0] = 1;
		setData ( & ( snnCfg[index].stepCfg_p[3].respData_p[0] ), body_setNum );
	}
	else
	{
		snnCfg[index].stepCfg_p[3].respBody_setNum_p[0] = body_setNum;
		setData ( & ( snnCfg[index].stepCfg_p[3].respData_p[0] ), 1 );
	}
    setEleVal ( & ( snnCfg[index].stepCfg_p[3].respData_p[0].asduTid ), eleVal[1] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[3].respData_p[0].asduLng ), eleVal[2] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[3].respData_p[0].asduVsq ), eleVal[3] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[3].respData_p[0].asduCot ), eleVal[4] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[3].respData_p[0].asduAddr ), eleVal[5] );

    if ( vsq->SQ == 1 )
    {
        setEleVal ( & ( snnCfg[index].stepCfg_p[3].respData_p[0].bodyObj_p[0].body_Tid ), eleVal[6] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[3].respData_p[0].bodyObj_p[0].body_Addr ), eleVal[7] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[3].respData_p[0].bodyObj_p[0].body_Set ), eleVal[8] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[3].respData_p[0].bodyObj_p[0].body_Tsp ), eleVal[9] );
    }
    else
    {
        for ( int i = 0; i < vsq->N; i++ )
        {
            setEleVal ( & ( snnCfg[index].stepCfg_p[3].respData_p[0].bodyObj_p[i].body_Tid ), eleVal[6] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[3].respData_p[0].bodyObj_p[i].body_Addr ), eleVal[7] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[3].respData_p[0].bodyObj_p[i].body_Set ), eleVal[8] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[3].respData_p[0].bodyObj_p[i].body_Tsp ), eleVal[9] );
        }

    }

    setEleVal ( & ( snnCfg[index].stepCfg_p[3].respData_p[0].asduTsp ), eleVal[10] );
    int usingMode = 0;
    int cot = 6;
    int tid_lng = 1;
    int tid = 100;
    setFunDsc ( & ( funDsc[index] ), STN_INRGTN, ACTIVE_TYPE, usingMode, stepNum );

    setPtclStep ( & ( funDsc[index].remoteSet_p[0] ), cot, 249, 1 );
    funDsc[index].remoteSet_p[0].TIDLng_p[0] = tid_lng;// 这个数组的大小取决于remoteSet_p->setNum的值
    funDsc[index].remoteSet_p[0].TID_p[0] = tid;

    cot = 7;
    tid_lng = 1;
    tid = 100;
    setPtclStep ( & ( funDsc[index].remoteSet_p[1] ), cot, 249, 1 );
    funDsc[index].remoteSet_p[1].TIDLng_p[0] = tid_lng;// 这个数组的大小取决于remoteSet_p->setNum的值
    funDsc[index].remoteSet_p[1].TID_p[0] = tid;

    cot = 20;
    tid_lng = 1;
    tid = 1;
    setPtclStep ( & ( funDsc[index].remoteSet_p[2] ), cot, 249, 2 );
    funDsc[index].remoteSet_p[2].TIDLng_p[0] = tid_lng;// 这个数组的大小取决于remoteSet_p->setNum的值
    funDsc[index].remoteSet_p[2].TID_p[0] = tid;

    tid_lng = 5;
    tid = 13;
    funDsc[index].remoteSet_p[2].TIDLng_p[1] = tid_lng;// 这个数组的大小取决于remoteSet_p->setNum的值
    funDsc[index].remoteSet_p[2].TID_p[1] = tid;

    cot = 10;
    tid_lng = 1;
    tid = 100;
    setPtclStep ( & ( funDsc[index].remoteSet_p[3] ), cot, 249, 1 );
    funDsc[index].remoteSet_p[3].TIDLng_p[0] = tid_lng;// 这个数组的大小取决于remoteSet_p->setNum的值
    funDsc[index].remoteSet_p[3].TID_p[0] = tid;

}

//查询数据会话:规则步及相关配置数据组装
void cfgManage::setQueDataCfg ( T_snnCfg* snnCfg, T_ptclFunDsc* funDsc, const int& index  )
{
    T_stnAcsFlag tmpStnAcsFlag = {0, 0, 1, 1, 1, 0, 1, 1, 1, 0};
    u_char ssnid = QUE_DATA;
    u_char actFlig = 0;//主被动型,由规约设置
    u_char infoType = GINFOTYPE;//
    u_short cmnAddr = 2;//公共地址
    u_char stepNum = 2;//步数
    /*******************************************设置查询数据会话**********************************************/
    setCfg ( & ( snnCfg[index] ), ssnid, actFlig, infoType, cmnAddr, stepNum );

    u_char reqAPDUNum = 1;//请求帧个数
    u_char respAPDUNum = 0;//响应帧个数
    u_char body_setNum = 1;//一帧数据中，信息对象或信息元素的个数
    /*******************************************设置站查询数据会话的第一步**********************************************/


    u_char memVal[11] = {};
    BODY_ADDR_VAL  bodyAddr_val = {};
    bodyAddr_val.shortAddr = 4;
    ELE_TYPE eleVal[11] =
    {
        {APDU_APCI_INDEX, 0, &memVal[0]},
        {ASDU_TID_INDEX, 1, &memVal[1]},
        {ASDU_LNG_INDEX, 0, &memVal[2]},
        {ASDU_VSQ_INDEX, 1, &memVal[3]},
        {ASDU_COT_INDEX, 2, &memVal[4]},
        {ASDU_ADDR_INDEX, sizeof ( u_short ), ( u_char* )& cmnAddr},
        {BODY_TID_INDEX, 0, &memVal[6]},
        {BODY_ADDR_INDEX, sizeof ( BODY_ADDR_VAL ), ( u_char* )& bodyAddr_val},
        {BODY_SET_INDEX, 4, &memVal[8]},
        {BODY_TSP_INDEX, 0, &memVal[9]},
        {ASDU_TSP_INDEX, 0, &memVal[10]}
    };
    * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = 1 << 7 | body_setNum;
    * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = 1;
    * ( eleVal[ASDU_COT_INDEX].p_Mem ) = 5;
    ASDU_VSQ_VAL* vsq = ( ASDU_VSQ_VAL* ) ( eleVal[3].p_Mem );
    setStepCfg ( & ( snnCfg[index].stepCfg_p[0] ), * ( u_short* ) &tmpStnAcsFlag, reqAPDUNum, respAPDUNum );
	if ( vsq->SQ == 0 )
	{
		snnCfg[index].stepCfg_p[0].reqBody_setNum_p[0] = 1;
		setData ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0] ), body_setNum );
	}
	else
	{
		snnCfg[index].stepCfg_p[0].reqBody_setNum_p[0] = body_setNum;
		setData ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0] ), 1 );
	}
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduTid ), eleVal[1] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduLng ), eleVal[2] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduVsq ), eleVal[3] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduCot ), eleVal[4] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduAddr ), eleVal[5] );

    if ( vsq->SQ == 1 )
    {
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Tid ), eleVal[6] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Addr ), eleVal[7] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Set ), eleVal[8] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Tsp ), eleVal[9] );
    }
    else
    {
        for ( int i = 0; i < vsq->N; i++ )
        {
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Tid ), eleVal[6] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Addr ), eleVal[7] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Set ), eleVal[8] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Tsp ), eleVal[9] );
        }

    }

    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduTsp ), eleVal[10] );
    /*******************************************设置站查询数据会话的第二步**********************************************/
    tmpStnAcsFlag.ctlType = 2;
    tmpStnAcsFlag.setValid = 1;
    reqAPDUNum = 0;
    respAPDUNum = 1;
    body_setNum = 1;

    * ( eleVal[ASDU_TID_INDEX].p_Mem ) = 22;
    * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = 1 << 7 | 2;
    * ( eleVal[ASDU_COT_INDEX].p_Mem ) = 5;
    eleVal[BODY_SET_INDEX].lng = sizeof ( u_char );
    * ( eleVal[BODY_SET_INDEX].p_Mem ) = 121;
    setStepCfg ( & ( snnCfg[index].stepCfg_p[1] ), * ( u_short* ) &tmpStnAcsFlag, reqAPDUNum, respAPDUNum );
	if ( vsq->SQ == 0 )
	{
		snnCfg[index].stepCfg_p[1].respBody_setNum_p[0] = 1;
		setData ( & ( snnCfg[index].stepCfg_p[1].respData_p[0] ), body_setNum );
	}
	else
	{
		snnCfg[index].stepCfg_p[1].respBody_setNum_p[0] = body_setNum;
		setData ( & ( snnCfg[index].stepCfg_p[1].respData_p[0] ), 1 );
	}
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduTid ), eleVal[1] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduLng ), eleVal[2] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduVsq ), eleVal[3] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduCot ), eleVal[4] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduAddr ), eleVal[5] );

    if ( vsq->SQ == 1 )
    {
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Tid ), eleVal[6] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Addr ), eleVal[7] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Set ), eleVal[8] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Tsp ), eleVal[9] );
    }
    else
    {
        for ( int i = 0; i < vsq->N; i++ )
        {
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Tid ), eleVal[6] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Addr ), eleVal[7] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Set ), eleVal[8] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Tsp ), eleVal[9] );
        }

    }

    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduTsp ), eleVal[10] );
    int usingMode = 2;
    int cot = 5;
    int tid_lng = 1;
    int tid = 102;
    setFunDsc ( & ( funDsc[index] ), QUE_DATA, ACTIVE_TYPE, usingMode, stepNum );

    setPtclStep ( & ( funDsc[index].remoteSet_p[0] ), cot, 249, 1 );
    funDsc[index].remoteSet_p[0].TIDLng_p[0] = tid_lng;// 这个数组的大小取决于remoteSet_p->setNum的值
    funDsc[index].remoteSet_p[0].TID_p[0] = tid;

    cot = 5;
    tid_lng = 1;
    tid = 22;
    setPtclStep ( & ( funDsc[index].remoteSet_p[1] ), cot, 249, 1 );
    funDsc[index].remoteSet_p[1].TIDLng_p[0] = tid_lng;// 这个数组的大小取决于remoteSet_p->setNum的值
    funDsc[index].remoteSet_p[1].TID_p[0] = tid;

}


//U帧STARTDT会话:规则步及相关配置数据组装
void cfgManage::setUstartdtCfg ( T_snnCfg* snnCfg, T_ptclFunDsc* funDsc, const int& index  )
{
    u_char ssnid = U_STARTDT;
    u_char actFlig = 0;//主被动型,由规约设置
    u_char infoType = GINFOTYPE;//
    u_short cmnAddr = 0;//公共地址
    u_char stepNum = 2;//步数
    /*******************************************设置激活会话**********************************************/
    setCfg ( & ( snnCfg[index] ), ssnid, actFlig, infoType, cmnAddr, stepNum );

    u_char reqAPDUNum = 1;//请求帧个数
    u_char respAPDUNum = 0;//响应帧个数
    u_char body_setNum = 1;//一帧数据中，信息对象或信息元素的个数
    /*******************************************设置激活会话的第一步**********************************************/


    u_char memVal[11] = {};
    BODY_ADDR_VAL  bodyAddr_val = {};
    bodyAddr_val.shortAddr = 4;
    ELE_TYPE eleVal[11] =
    {
        {APDU_APCI_INDEX, 0, &memVal[0]},
        {ASDU_TID_INDEX, 1, &memVal[1]},
        {ASDU_LNG_INDEX, 0, &memVal[2]},
        {ASDU_VSQ_INDEX, 1, &memVal[3]},
        {ASDU_COT_INDEX, 2, &memVal[4]},
        {ASDU_ADDR_INDEX, sizeof ( u_short ), ( u_char* )& cmnAddr},
        {BODY_TID_INDEX, 0, &memVal[6]},
        {BODY_ADDR_INDEX, sizeof ( BODY_ADDR_VAL ), ( u_char* )& bodyAddr_val},
        {BODY_SET_INDEX, 4, &memVal[8]},
        {BODY_TSP_INDEX, 0, &memVal[9]},
        {ASDU_TSP_INDEX, 0, &memVal[10]}
    };
    * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = 1 << 7 | body_setNum;
    * ( eleVal[ASDU_TID_INDEX].p_Mem ) = 136;
    * ( eleVal[ASDU_COT_INDEX].p_Mem ) = 6;
    ASDU_VSQ_VAL* vsq = ( ASDU_VSQ_VAL* ) ( eleVal[3].p_Mem );
    setStepCfg ( & ( snnCfg[index].stepCfg_p[0] ), 0, reqAPDUNum, respAPDUNum );
	if ( vsq->SQ == 0 )
	{
		snnCfg[index].stepCfg_p[0].reqBody_setNum_p[0] = 1;
		setData ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0] ), body_setNum );
	}
	else
	{
		snnCfg[index].stepCfg_p[0].reqBody_setNum_p[0] = body_setNum;
		setData ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0] ), 1 );
	}
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduTid ), eleVal[1] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduLng ), eleVal[2] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduVsq ), eleVal[3] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduCot ), eleVal[4] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduAddr ), eleVal[5] );

    if ( vsq->SQ == 1 )
    {
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Tid ), eleVal[6] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Addr ), eleVal[7] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Set ), eleVal[8] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Tsp ), eleVal[9] );
    }
    else
    {
        for ( int i = 0; i < vsq->N; i++ )
        {
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Tid ), eleVal[6] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Addr ), eleVal[7] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Set ), eleVal[8] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Tsp ), eleVal[9] );
        }

    }

    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduTsp ), eleVal[10] );
    /*******************************************设置激活会话的第二步**********************************************/
    reqAPDUNum = 0;
    respAPDUNum = 1;
    body_setNum = 0;

    * ( eleVal[ASDU_TID_INDEX].p_Mem ) = 136;
    * ( eleVal[ASDU_COT_INDEX].p_Mem ) = 7;
    setStepCfg ( & ( snnCfg[index].stepCfg_p[1] ), 0, reqAPDUNum, respAPDUNum );
	if ( vsq->SQ == 0 )
	{
		snnCfg[index].stepCfg_p[1].respBody_setNum_p[0] = 1;
		setData ( & ( snnCfg[index].stepCfg_p[1].respData_p[0] ), body_setNum );
	}
	else
	{
		snnCfg[index].stepCfg_p[1].respBody_setNum_p[0] = body_setNum;
		setData ( & ( snnCfg[index].stepCfg_p[1].respData_p[0] ), 1 );
	}
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduTid ), eleVal[1] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduLng ), eleVal[2] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduVsq ), eleVal[3] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduCot ), eleVal[4] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduAddr ), eleVal[5] );

    if ( vsq->SQ == 1 )
    {
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Tid ), eleVal[6] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Addr ), eleVal[7] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Set ), eleVal[8] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Tsp ), eleVal[9] );
    }
    else
    {
        for ( int i = 0; i < vsq->N; i++ )
        {
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Tid ), eleVal[6] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Addr ), eleVal[7] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Set ), eleVal[8] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Tsp ), eleVal[9] );
        }

    }

    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduTsp ), eleVal[10] );
    int usingMode = 0;
    int cot = 6;
    int tid_lng = 1;
    int tid = 136;
    setFunDsc ( & ( funDsc[index] ), U_STARTDT, ACTIVE_TYPE, usingMode, stepNum );

    setPtclStep ( & ( funDsc[index].remoteSet_p[0] ), cot, 249, 1 );
    funDsc[index].remoteSet_p[0].TIDLng_p[0] = tid_lng;// 这个数组的大小取决于remoteSet_p->setNum的值
    funDsc[index].remoteSet_p[0].TID_p[0] = tid;
    cot = 7;
    setPtclStep ( & ( funDsc[index].remoteSet_p[1] ), cot, 249, 1 );
    funDsc[index].remoteSet_p[1].TIDLng_p[0] = tid_lng;// 这个数组的大小取决于remoteSet_p->setNum的值
    funDsc[index].remoteSet_p[1].TID_p[0] = tid;
}


//U帧STOPDT会话:规则步及相关配置数据组装
void cfgManage::setUstopdtCfg ( T_snnCfg* snnCfg, T_ptclFunDsc* funDsc, const int& index )
{
    u_char ssnid = U_STOPDT;
    u_char actFlig = 0;//主被动型,由规约设置
    u_char infoType = GINFOTYPE;//
    u_short cmnAddr = 0;//公共地址
    u_char stepNum = 2;//步数
    /*******************************************设置停止激活会话**********************************************/
    setCfg ( & ( snnCfg[index] ), ssnid, actFlig, infoType, cmnAddr, stepNum );

    u_char reqAPDUNum = 1;//请求帧个数
    u_char respAPDUNum = 0;//响应帧个数
    u_char body_setNum = 1;//一帧数据中，信息对象或信息元素的个数
    /*******************************************设置停止激活会话的第一步**********************************************/


    u_char memVal[11] = {};
    BODY_ADDR_VAL  bodyAddr_val = {};
    bodyAddr_val.shortAddr = 4;
    ELE_TYPE eleVal[11] =
    {
        {APDU_APCI_INDEX, 0, &memVal[0]},
        {ASDU_TID_INDEX, 1, &memVal[1]},
        {ASDU_LNG_INDEX, 0, &memVal[2]},
        {ASDU_VSQ_INDEX, 1, &memVal[3]},
        {ASDU_COT_INDEX, 2, &memVal[4]},
        {ASDU_ADDR_INDEX, sizeof ( u_short ), ( u_char* )& cmnAddr},
        {BODY_TID_INDEX, 0, &memVal[6]},
        {BODY_ADDR_INDEX, sizeof ( BODY_ADDR_VAL ), ( u_char* )& bodyAddr_val},
        {BODY_SET_INDEX, 4, &memVal[8]},
        {BODY_TSP_INDEX, 0, &memVal[9]},
        {ASDU_TSP_INDEX, 0, &memVal[10]}
    };
    * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = 1 << 7 | body_setNum;
    * ( eleVal[ASDU_TID_INDEX].p_Mem ) = 137;
    * ( eleVal[ASDU_COT_INDEX].p_Mem ) = 6;
    ASDU_VSQ_VAL* vsq = ( ASDU_VSQ_VAL* ) ( eleVal[3].p_Mem );
    setStepCfg ( & ( snnCfg[index].stepCfg_p[0] ), 0, reqAPDUNum, respAPDUNum );
    snnCfg[index].stepCfg_p[0].reqBody_setNum_p[0] = body_setNum;
	if ( vsq->SQ == 0 )
	{
		snnCfg[index].stepCfg_p[0].reqBody_setNum_p[0] = 1;
		setData ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0] ), body_setNum );
	}
	else
	{
		snnCfg[index].stepCfg_p[0].reqBody_setNum_p[0] = body_setNum;
		setData ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0] ), 1 );
	}
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduLng ), eleVal[2] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduVsq ), eleVal[3] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduCot ), eleVal[4] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduAddr ), eleVal[5] );

    if ( vsq->SQ == 1 )
    {
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Tid ), eleVal[6] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Addr ), eleVal[7] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Set ), eleVal[8] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Tsp ), eleVal[9] );
    }
    else
    {
        for ( int i = 0; i < vsq->N; i++ )
        {
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Tid ), eleVal[6] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Addr ), eleVal[7] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Set ), eleVal[8] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Tsp ), eleVal[9] );
        }

    }

    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduTsp ), eleVal[10] );
    /*******************************************设置停止激活会话的第二步**********************************************/
    reqAPDUNum = 0;
    respAPDUNum = 1;
    body_setNum = 0;

    * ( eleVal[ASDU_TID_INDEX].p_Mem ) = 137;
    * ( eleVal[ASDU_COT_INDEX].p_Mem ) = 7;
    setStepCfg ( & ( snnCfg[index].stepCfg_p[1] ), 0, reqAPDUNum, respAPDUNum );
	if ( vsq->SQ == 0 )
	{
		snnCfg[index].stepCfg_p[1].respBody_setNum_p[0] = 1;
		setData ( & ( snnCfg[index].stepCfg_p[1].respData_p[0] ), body_setNum );
	}
	else
	{
		snnCfg[index].stepCfg_p[1].respBody_setNum_p[0] = body_setNum;
		setData ( & ( snnCfg[index].stepCfg_p[1].respData_p[0] ), 1 );
	}
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduTid ), eleVal[1] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduLng ), eleVal[2] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduVsq ), eleVal[3] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduCot ), eleVal[4] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduAddr ), eleVal[5] );

    if ( vsq->SQ == 1 )
    {
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Tid ), eleVal[6] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Addr ), eleVal[7] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Set ), eleVal[8] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Tsp ), eleVal[9] );
    }
    else
    {
        for ( int i = 0; i < vsq->N; i++ )
        {
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Tid ), eleVal[6] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Addr ), eleVal[7] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Set ), eleVal[8] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Tsp ), eleVal[9] );
        }

    }

    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduTsp ), eleVal[10] );

    int usingMode = 0;
    int cot = 6;
    int tid_lng = 1;
    int tid = 137;
    setFunDsc ( & ( funDsc[index] ), U_STOPDT, ACTIVE_TYPE, usingMode, stepNum );

    setPtclStep ( & ( funDsc[index].remoteSet_p[0] ), cot, 249, 1 );
    funDsc[index].remoteSet_p[0].TIDLng_p[0] = tid_lng;// 这个数组的大小取决于remoteSet_p->setNum的值
    funDsc[index].remoteSet_p[0].TID_p[0] = tid;
    cot = 7;
    setPtclStep ( & ( funDsc[index].remoteSet_p[1] ), cot, 249, 1 );
    funDsc[index].remoteSet_p[1].TIDLng_p[0] = tid_lng;// 这个数组的大小取决于remoteSet_p->setNum的值
    funDsc[index].remoteSet_p[1].TID_p[0] = tid;
}



//U帧TESTFR会话:规则步及相关配置数据组装
void cfgManage::setTestfrCfg ( T_snnCfg* snnCfg, T_ptclFunDsc* funDsc, const int& index )
{
    u_char ssnid = U_TESTFR;
    u_char actFlig = 0;//主被动型,由规约设置
    u_char infoType = GINFOTYPE;//
    u_short cmnAddr = 0;//公共地址
    u_char stepNum = 2;//步数
    /*******************************************设置测试会话**********************************************/
    setCfg ( & ( snnCfg[index] ), ssnid, actFlig, infoType, cmnAddr, stepNum );

    u_char reqAPDUNum = 1;//请求帧个数
    u_char respAPDUNum = 0;//响应帧个数
    u_char body_setNum = 1;//一帧数据中，信息对象或信息元素的个数
    /*******************************************设置测试会话的第一步**********************************************/


    u_char memVal[11] = {};
    BODY_ADDR_VAL  bodyAddr_val = {};
    bodyAddr_val.shortAddr = 4;
    ELE_TYPE eleVal[11] =
    {
        {APDU_APCI_INDEX, 0, &memVal[0]},
        {ASDU_TID_INDEX, 1, &memVal[1]},
        {ASDU_LNG_INDEX, 0, &memVal[2]},
        {ASDU_VSQ_INDEX, 1, &memVal[3]},
        {ASDU_COT_INDEX, 2, &memVal[4]},
        {ASDU_ADDR_INDEX, sizeof ( u_short ), ( u_char* )& cmnAddr},
        {BODY_TID_INDEX, 0, &memVal[6]},
        {BODY_ADDR_INDEX, sizeof ( BODY_ADDR_VAL ), ( u_char* )& bodyAddr_val},
        {BODY_SET_INDEX, 4, &memVal[8]},
        {BODY_TSP_INDEX, 0, &memVal[9]},
        {ASDU_TSP_INDEX, 0, &memVal[10]}
    };
    * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = 1 << 7 | body_setNum;
    * ( eleVal[ASDU_TID_INDEX].p_Mem ) = 138;
    * ( eleVal[ASDU_COT_INDEX].p_Mem ) = 6;
    ASDU_VSQ_VAL* vsq = ( ASDU_VSQ_VAL* ) ( eleVal[3].p_Mem );
    setStepCfg ( & ( snnCfg[index].stepCfg_p[0] ), 0, reqAPDUNum, respAPDUNum );
	if ( vsq->SQ == 0 )
	{
		snnCfg[index].stepCfg_p[0].reqBody_setNum_p[0] = 1;
		setData ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0] ), body_setNum );
	}
	else
	{
		snnCfg[index].stepCfg_p[0].reqBody_setNum_p[0] = body_setNum;
		setData ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0] ), 1 );
	}
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduTid ), eleVal[1] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduLng ), eleVal[2] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduVsq ), eleVal[3] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduCot ), eleVal[4] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduAddr ), eleVal[5] );

    if ( vsq->SQ == 1 )
    {
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Tid ), eleVal[6] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Addr ), eleVal[7] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Set ), eleVal[8] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Tsp ), eleVal[9] );
    }
    else
    {
        for ( int i = 0; i < vsq->N; i++ )
        {
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Tid ), eleVal[6] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Addr ), eleVal[7] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Set ), eleVal[8] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Tsp ), eleVal[9] );
        }

    }

    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduTsp ), eleVal[10] );
    /*******************************************设置测试会话的第二步**********************************************/
    reqAPDUNum = 0;
    respAPDUNum = 1;
    body_setNum = 0;

    * ( eleVal[ASDU_TID_INDEX].p_Mem ) = 138;
    * ( eleVal[ASDU_COT_INDEX].p_Mem ) = 7;
    setStepCfg ( & ( snnCfg[index].stepCfg_p[1] ), 0, reqAPDUNum, respAPDUNum );
	if ( vsq->SQ == 0 )
	{
		snnCfg[index].stepCfg_p[1].respBody_setNum_p[0] = 1;
		setData ( & ( snnCfg[index].stepCfg_p[1].respData_p[0] ), body_setNum );
	}
	else
	{
		snnCfg[index].stepCfg_p[1].respBody_setNum_p[0] = body_setNum;
		setData ( & ( snnCfg[index].stepCfg_p[1].respData_p[0] ), 1 );
	}
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduTid ), eleVal[1] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduLng ), eleVal[2] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduVsq ), eleVal[3] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduCot ), eleVal[4] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduAddr ), eleVal[5] );

    if ( vsq->SQ == 1 )
    {
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Tid ), eleVal[6] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Addr ), eleVal[7] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Set ), eleVal[8] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Tsp ), eleVal[9] );
    }
    else
    {
        for ( int i = 0; i < vsq->N; i++ )
        {
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Tid ), eleVal[6] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Addr ), eleVal[7] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Set ), eleVal[8] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Tsp ), eleVal[9] );
        }

    }

    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduTsp ), eleVal[10] );
    int usingMode = 0;
    int cot = 6;
    int tid_lng = 1;
    int tid = 138;
    setFunDsc ( & ( funDsc[index] ), U_TESTFR, ACTIVE_TYPE, usingMode, stepNum );

    setPtclStep ( & ( funDsc[index].remoteSet_p[0] ), cot, 249, 1 );
    funDsc[index].remoteSet_p[0].TIDLng_p[0] = tid_lng;// 这个数组的大小取决于remoteSet_p->setNum的值
    funDsc[index].remoteSet_p[0].TID_p[0] = tid;
    cot = 7;
    setPtclStep ( & ( funDsc[index].remoteSet_p[1] ), cot, 249, 1 );
    funDsc[index].remoteSet_p[1].TIDLng_p[0] = tid_lng;// 这个数组的大小取决于remoteSet_p->setNum的值
    funDsc[index].remoteSet_p[1].TID_p[0] = tid;
}


//U帧新联一个通道 会话:规则步及相关配置数据组装
void cfgManage::setNewholdCfg ( T_snnCfg* snnCfg, T_ptclFunDsc* funDsc, const int& index )
{
    u_char ssnid = U_NEWHOLD;
    u_char actFlig = 0;//主被动型,由规约设置
    u_char infoType = GINFOTYPE;//
    u_short cmnAddr = 0;//公共地址
    u_char stepNum = 1;//步数
    /*******************************************设置停止激活会话**********************************************/
    setCfg ( & ( snnCfg[index] ), ssnid, actFlig, infoType, cmnAddr, stepNum );

    u_char reqAPDUNum = 1;//请求帧个数
    u_char respAPDUNum = 0;//响应帧个数
    u_char body_setNum = 0;//一帧数据中，信息对象或信息元素的个数
    /*******************************************设置创建通道会话的第一步**********************************************/


    u_char memVal[11] = {};
    BODY_ADDR_VAL  bodyAddr_val = {};
    bodyAddr_val.shortAddr = 4;
    ELE_TYPE eleVal[11] =
    {
        {APDU_APCI_INDEX, 0, &memVal[0]},
        {ASDU_TID_INDEX, 1, &memVal[1]},
        {ASDU_LNG_INDEX, 0, &memVal[2]},
        {ASDU_VSQ_INDEX, 1, &memVal[3]},
        {ASDU_COT_INDEX, 2, &memVal[4]},
        {ASDU_ADDR_INDEX, sizeof ( u_short ), ( u_char* )& cmnAddr},
        {BODY_TID_INDEX, 0, &memVal[6]},
        {BODY_ADDR_INDEX, sizeof ( BODY_ADDR_VAL ), ( u_char* )& bodyAddr_val},
        {BODY_SET_INDEX, 4, &memVal[8]},
        {BODY_TSP_INDEX, 0, &memVal[9]},
        {ASDU_TSP_INDEX, 0, &memVal[10]}
    };
    * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = 1 << 7 | body_setNum;
    * ( eleVal[ASDU_TID_INDEX].p_Mem ) = 139;
    * ( eleVal[ASDU_COT_INDEX].p_Mem ) = 6;
    ASDU_VSQ_VAL* vsq = ( ASDU_VSQ_VAL* ) ( eleVal[3].p_Mem );
    setStepCfg ( & ( snnCfg[index].stepCfg_p[0] ), 0, reqAPDUNum, respAPDUNum );
	if ( vsq->SQ == 0 )
	{
		snnCfg[index].stepCfg_p[0].reqBody_setNum_p[0] = 1;
		setData ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0] ), body_setNum );
	}
	else
	{
		snnCfg[index].stepCfg_p[0].reqBody_setNum_p[0] = body_setNum;
		setData ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0] ), 1 );
	}
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduTid ), eleVal[1] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduLng ), eleVal[2] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduVsq ), eleVal[3] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduCot ), eleVal[4] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduAddr ), eleVal[5] );

    if ( vsq->SQ == 1 )
    {
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Tid ), eleVal[6] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Addr ), eleVal[7] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Set ), eleVal[8] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Tsp ), eleVal[9] );
    }
    else
    {
        for ( int i = 0; i < vsq->N; i++ )
        {
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Tid ), eleVal[6] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Addr ), eleVal[7] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Set ), eleVal[8] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Tsp ), eleVal[9] );
        }

    }

    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduTsp ), eleVal[10] );

    /*******************************************设置创建通道会话的第二步**********************************************/
    /*
        reqAPDUNum = 0;
        respAPDUNum = 1;
        body_setNum = 0;

        * ( eleVal[ASDU_TID_INDEX].p_Mem ) = 139;
        * ( eleVal[ASDU_COT_INDEX].p_Mem ) = 7;
        setStepCfg ( & ( snnCfg[index].stepCfg_p[1] ), 0, reqAPDUNum, respAPDUNum );
        snnCfg[index].stepCfg_p[1].respBody_setNum_p[0] = body_setNum;
        setData ( & ( snnCfg[index].stepCfg_p[1].respData_p[0] ), snnCfg[index].stepCfg_p[1].respBody_setNum_p[0] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduTid ), eleVal[1] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduLng ), eleVal[2] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduVsq ), eleVal[3] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduCot ), eleVal[4] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduAddr ), eleVal[5] );

        if ( vsq->SQ == 1 )
        {
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Tid ), eleVal[6] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Addr ), eleVal[7] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Set ), eleVal[8] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Tsp ), eleVal[9] );
        }
        else
        {
            for ( int i = 0; i < vsq->N; i++ )
            {
                setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Tid ), eleVal[6] );
                setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Addr ), eleVal[7] );
                setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Set ), eleVal[8] );
                setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Tsp ), eleVal[9] );
            }

        }

        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduTsp ), eleVal[10] );
        */
    int usingMode = 0;
    int cot = 6;
    int tid_lng = 1;
    int tid = 139;
    setFunDsc ( & ( funDsc[index] ), U_NEWHOLD, ACTIVE_TYPE, usingMode, stepNum );

    setPtclStep ( & ( funDsc[index].remoteSet_p[0] ), cot, 249, 1 );
    funDsc[index].remoteSet_p[0].TIDLng_p[0] = tid_lng;// 这个数组的大小取决于remoteSet_p->setNum的值
    funDsc[index].remoteSet_p[0].TID_p[0] = tid;
    /*
    cot = 7;
    setPtclStep ( & ( funDsc[index].remoteSet_p[1] ), cot, 249, 1 );
    funDsc[index].remoteSet_p[0].TIDLng_p[0] = tid_lng;// 这个数组的大小取决于remoteSet_p->setNum的值
    funDsc[index].remoteSet_p[0].TID_p[0] = tid;
    */
}



//U帧关一个通道  会话:规则步及相关配置数据组装
void cfgManage::setDelholdCfg ( T_snnCfg* snnCfg, T_ptclFunDsc* funDsc, const int& index )
{
    u_char ssnid = U_DELHOLD;
    u_char actFlig = 0;//主被动型,由规约设置
    u_char infoType = GINFOTYPE;//
    u_short cmnAddr = 0;//公共地址
    u_char stepNum = 1;//步数
    /*******************************************设置关闭通道会话**********************************************/
    setCfg ( & ( snnCfg[index] ), ssnid, actFlig, infoType, cmnAddr, stepNum );

    u_char reqAPDUNum = 1;//请求帧个数
    u_char respAPDUNum = 0;//响应帧个数
    u_char body_setNum = 1;//一帧数据中，信息对象或信息元素的个数
    /*******************************************设置关闭通道会话的第一步**********************************************/


    u_char memVal[11] = {};
    BODY_ADDR_VAL  bodyAddr_val = {};
    bodyAddr_val.shortAddr = 4;
    ELE_TYPE eleVal[11] =
    {
        {APDU_APCI_INDEX, 0, &memVal[0]},
        {ASDU_TID_INDEX, 1, &memVal[1]},
        {ASDU_LNG_INDEX, 0, &memVal[2]},
        {ASDU_VSQ_INDEX, 1, &memVal[3]},
        {ASDU_COT_INDEX, 2, &memVal[4]},
        {ASDU_ADDR_INDEX, sizeof ( u_short ), ( u_char* )& cmnAddr},
        {BODY_TID_INDEX, 0, &memVal[6]},
        {BODY_ADDR_INDEX, sizeof ( BODY_ADDR_VAL ), ( u_char* )& bodyAddr_val},
        {BODY_SET_INDEX, 4, &memVal[8]},
        {BODY_TSP_INDEX, 0, &memVal[9]},
        {ASDU_TSP_INDEX, 0, &memVal[10]}
    };
    * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = 1 << 7 | body_setNum;
    * ( eleVal[ASDU_TID_INDEX].p_Mem ) = 140;
    * ( eleVal[ASDU_COT_INDEX].p_Mem ) = 6;
    ASDU_VSQ_VAL* vsq = ( ASDU_VSQ_VAL* ) ( eleVal[3].p_Mem );
    setStepCfg ( & ( snnCfg[index].stepCfg_p[0] ), 0, reqAPDUNum, respAPDUNum );
	if ( vsq->SQ == 0 )
	{
		snnCfg[index].stepCfg_p[0].reqBody_setNum_p[0] = 1;
		setData ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0] ), body_setNum );
	}
	else
	{
		snnCfg[index].stepCfg_p[0].reqBody_setNum_p[0] = body_setNum;
		setData ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0] ), 1 );
	}
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduTid ), eleVal[1] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduLng ), eleVal[2] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduVsq ), eleVal[3] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduCot ), eleVal[4] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduAddr ), eleVal[5] );

    if ( vsq->SQ == 1 )
    {
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Tid ), eleVal[6] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Addr ), eleVal[7] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Set ), eleVal[8] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[0].body_Tsp ), eleVal[9] );
    }
    else
    {
        for ( int i = 0; i < vsq->N; i++ )
        {
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Tid ), eleVal[6] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Addr ), eleVal[7] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Set ), eleVal[8] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].bodyObj_p[i].body_Tsp ), eleVal[9] );
        }

    }

    setEleVal ( & ( snnCfg[index].stepCfg_p[0].reqData_p[0].asduTsp ), eleVal[10] );


    /*******************************************设置关闭通道会话的第二步**********************************************/
    /*
    reqAPDUNum = 0;
    respAPDUNum = 1;
    body_setNum = 0;

    * ( eleVal[ASDU_TID_INDEX].p_Mem ) = 140;
    * ( eleVal[ASDU_COT_INDEX].p_Mem ) = 7;
    setStepCfg ( & ( snnCfg[index].stepCfg_p[1] ), 0, reqAPDUNum, respAPDUNum );
    snnCfg[index].stepCfg_p[1].respBody_setNum_p[0] = body_setNum;
    setData ( & ( snnCfg[index].stepCfg_p[1].respData_p[0] ), snnCfg[index].stepCfg_p[1].respBody_setNum_p[0] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduTid ), eleVal[1] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduLng ), eleVal[2] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduVsq ), eleVal[3] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduCot ), eleVal[4] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduAddr ), eleVal[5] );

    if ( vsq->SQ == 1 )
    {
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Tid ), eleVal[6] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Addr ), eleVal[7] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Set ), eleVal[8] );
        setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[0].body_Tsp ), eleVal[9] );
    }
    else
    {
        for ( int i = 0; i < vsq->N; i++ )
        {
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Tid ), eleVal[6] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Addr ), eleVal[7] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Set ), eleVal[8] );
            setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].bodyObj_p[i].body_Tsp ), eleVal[9] );
        }

    }

    setEleVal ( & ( snnCfg[index].stepCfg_p[1].respData_p[0].asduTsp ), eleVal[10] );
    */
    int usingMode = 0;
    int cot = 6;
    int tid_lng = 1;
    int tid = 140;
    setFunDsc ( & ( funDsc[index] ), U_DELHOLD, ACTIVE_TYPE, usingMode, stepNum );

    setPtclStep ( & ( funDsc[index].remoteSet_p[0] ), cot, 249, 1 );
    funDsc[index].remoteSet_p[0].TIDLng_p[0] = tid_lng;// 这个数组的大小取决于remoteSet_p->setNum的值
    funDsc[index].remoteSet_p[0].TID_p[0] = tid;
#if 0
    cot = 7;
    setPtclStep ( & ( funDsc[index].remoteSet_p[1] ), cot, 249, 1 );
    funDsc[index].remoteSet_p[1].TIDLng_p[0] = tid_lng;// 这个数组的大小取决于remoteSet_p->setNum的值
    funDsc[index].remoteSet_p[1].TID_p[0] = tid;
#endif
}


//取得通道chnId的ssnid会话配置
T_snnCfg* cfgManage::getSnnCfg ( u_char ssnid, int chnlId )
{
    for ( int i = 0 ; i < snnUseNum_p[chnlId]; i++ )
    {
        if ( ssnid == this->t_snnCfg[chnlId][i].ssnID )
        {
            return & ( this->t_snnCfg[chnlId][i] );
        }
    }

    return NULL;
}

//取得通道chnId的ssnid会话功能描述
T_ptclFunDsc* cfgManage::getFunDsc ( u_char ssnid,  int chnlId )
{
    for ( int i = 0; i < snnUseNum_p[chnlId]; i++ )
    {
        if ( ssnid == ptclFunDsc[chnlId][i].ssnDscNo )
        {
            return & ( ptclFunDsc[chnlId][i] );
        }
    }

    return NULL;
}
/*******************************************************************************************************************/
/*
* 设置应用系统支持的会话集
*/
void cfgManage::setAppSnnCfg()
{
    //104配置
    ptclSnnCfg[0].prio = 7;             // 优先级
    ptclSnnCfg[0].ptclNo = 104;         // 协议号
    ptclSnnCfg[0].snnDscNo = U_NEWHOLD; // 会话号（协议），连接会话，每个通道通信开始必备

    ptclSnnCfg[1].prio = 6;
    ptclSnnCfg[1].ptclNo = 104;
    ptclSnnCfg[1].snnDscNo = U_STARTDT;     // U帧 START，I帧开始前必备（每个通道）

    ptclSnnCfg[2].prio = 2;
    ptclSnnCfg[2].ptclNo = 104;
    ptclSnnCfg[2].snnDscNo = U_TESTFR;

    ptclSnnCfg[3].prio = 5;
    ptclSnnCfg[3].ptclNo = 104;
    ptclSnnCfg[3].snnDscNo = ACQ_EVT;

    ptclSnnCfg[4].prio = 3;
    ptclSnnCfg[4].ptclNo = 104;
    ptclSnnCfg[4].snnDscNo = U_STOPDT;

    ptclSnnCfg[5].prio = 4;
    ptclSnnCfg[5].ptclNo = 104;
    ptclSnnCfg[5].snnDscNo = STN_INRGTN;//STN_INRGTN;

    ptclSnnCfg[6].prio = 8;
    ptclSnnCfg[6].ptclNo = 106;
    ptclSnnCfg[6].snnDscNo = MC_MBFC_0xFE;

    ptclSnnCfg[7].prio = 2;
    ptclSnnCfg[7].ptclNo = 106;
    ptclSnnCfg[7].snnDscNo = MC_MBFC_0x01;

    ptclSnnCfg[8].prio = 3;
    ptclSnnCfg[8].ptclNo = 106;
    ptclSnnCfg[8].snnDscNo = MC_MBFC_0x02;

    ptclSnnCfg[9].prio = 4;
    ptclSnnCfg[9].ptclNo = 106;
    ptclSnnCfg[9].snnDscNo = MC_MBFC_0x03;

    ptclSnnCfg[10].prio = 5;
    ptclSnnCfg[10].ptclNo = 106;
    ptclSnnCfg[10].snnDscNo = MC_MBFC_0x04;

    ptclSnnCfg[11].prio = 1;
    ptclSnnCfg[11].ptclNo = 106;
    ptclSnnCfg[11].snnDscNo = MC_MBFC_0x05;

    ptclSnnCfg[12].prio = 6;
    ptclSnnCfg[12].ptclNo = 106;
    ptclSnnCfg[12].snnDscNo = MC_MBFC_0x06;

    ptclSnnCfg[13].prio = 7;
    ptclSnnCfg[13].ptclNo = 106;
    ptclSnnCfg[13].snnDscNo = MC_MBFC_0x0F;

    ptclSnnCfg[14].prio = 8;
    ptclSnnCfg[14].ptclNo = 106;
    ptclSnnCfg[14].snnDscNo = MC_MBFC_0x10;
}

snnAppCfg* cfgManage::getAppSnnCfg()
{
    return ptclSnnCfg;
}


#if 0 //现在的版本废弃此接口
void cfgManage::setchnlObjCfg()
{
    //modbus配置
    chnlCfg_p[0].stage = 0;
    chnlCfg_p[0].role = MC_CTL_STN;
    chnlCfg_p[0].stnRole = MC_CTLED_STN;
    chnlCfg_p[0].cfgLen = 5;
    chnlCfg_p[0].ptclMdlNum = 106;
    chnlCfg_p[0].bufLng = 255;
    chnlCfg_p[0].usrType = 0;
    chnlCfg_p[0].cfgAddr_p = new addrRange[chnlCfg_p[1].cfgLen];
    chnlCfg_p[0].cfgAddr_p[0].useNum = 520;// 实际在用点数
    chnlCfg_p[0].cfgAddr_p[0].begAddr = 0x4001;
    chnlCfg_p[0].cfgAddr_p[0].endAddr = 0x5209;
    chnlCfg_p[0].cfgAddr_p[1].useNum = 260;// 实际在用点数
    chnlCfg_p[0].cfgAddr_p[1].begAddr = 0x01;
    chnlCfg_p[0].cfgAddr_p[1].endAddr = 0x600;
    chnlCfg_p[0].cfgAddr_p[2].useNum = 40;// 实际在用点数
    chnlCfg_p[0].cfgAddr_p[2].begAddr = 0x1080;
    chnlCfg_p[0].cfgAddr_p[2].endAddr = 0x1120;
    chnlCfg_p[0].cfgAddr_p[2].useNum = 20;// 实际在用点数
    chnlCfg_p[0].cfgAddr_p[2].begAddr = 0x14;
    chnlCfg_p[0].cfgAddr_p[2].endAddr = 0x1e;

    //modbus配置
    chnlCfg_p[0].tmpApduDscr.ASDU_TID = 0;//100 // 默认规则不用，配置子系统时 要注意根据会话号不同而区别
    chnlCfg_p[0].tmpApduDscr.ASDU_CMTBL[0] = 7; //6;//APCI
    chnlCfg_p[0].tmpApduDscr.ASDU_CMTBL[1] = 0;//1;//ASDU_TID
    chnlCfg_p[0].tmpApduDscr.ASDU_CMTBL[2] = 1;//0;//ASDU_LNG // 以下值为 0 的字段位是104暂时不需要
    chnlCfg_p[0].tmpApduDscr.ASDU_CMTBL[3] = 248;//1;//ASDU_VSQ
    chnlCfg_p[0].tmpApduDscr.ASDU_CMTBL[4] = 0;//ASDU_COT
    chnlCfg_p[0].tmpApduDscr.ASDU_CMTBL[5] = 2;//ASDU_ADD
    chnlCfg_p[0].tmpApduDscr.ASDU_CMTBL[6] = 0;//BODY_TID
    chnlCfg_p[0].tmpApduDscr.ASDU_CMTBL[7] = 3;//BODY_ADD
    chnlCfg_p[0].tmpApduDscr.ASDU_CMTBL[8] = 40;//BODY_SET
    chnlCfg_p[0].tmpApduDscr.ASDU_CMTBL[9] = 0;//BODY_TSP
    chnlCfg_p[0].tmpApduDscr.ASDU_CMTBL[10] = 0; //ASDU_TSP
    chnlCfg_p[0].tmpApduDscr.InfoEleLng = 1;

    //modbus配置
    chnlCfg_p[1].stage = 0;
    chnlCfg_p[1].role = MC_CTL_STN;
    chnlCfg_p[1].stnRole = MC_CTLED_STN;
    chnlCfg_p[1].cfgLen = 5;
    chnlCfg_p[1].ptclMdlNum = 106;
    chnlCfg_p[1].bufLng = 255;
    chnlCfg_p[1].usrType = 0;
    chnlCfg_p[1].cfgAddr_p = new addrRange[chnlCfg_p[1].cfgLen];
    chnlCfg_p[1].cfgAddr_p[0].useNum = 520;// 实际在用点数
    chnlCfg_p[1].cfgAddr_p[0].begAddr = 0x4001;
    chnlCfg_p[1].cfgAddr_p[0].endAddr = 0x5209;
    chnlCfg_p[1].cfgAddr_p[1].useNum = 260;// 实际在用点数
    chnlCfg_p[1].cfgAddr_p[1].begAddr = 0x01;
    chnlCfg_p[1].cfgAddr_p[1].endAddr = 0x600;
    chnlCfg_p[1].cfgAddr_p[2].useNum = 40;// 实际在用点数
    chnlCfg_p[1].cfgAddr_p[2].begAddr = 0x1080;
    chnlCfg_p[1].cfgAddr_p[2].endAddr = 0x1120;
    chnlCfg_p[1].cfgAddr_p[2].useNum = 20;// 实际在用点数
    chnlCfg_p[1].cfgAddr_p[2].begAddr = 0x14;
    chnlCfg_p[1].cfgAddr_p[2].endAddr = 0x1e;

    //modbus配置
    chnlCfg_p[1].tmpApduDscr.ASDU_TID = 0;//100 // 默认规则不用，配置子系统时 要注意根据会话号不同而区别
    chnlCfg_p[1].tmpApduDscr.ASDU_CMTBL[0] = 7; //6;//APCI
    chnlCfg_p[1].tmpApduDscr.ASDU_CMTBL[1] = 0;//1;//ASDU_TID
    chnlCfg_p[1].tmpApduDscr.ASDU_CMTBL[2] = 1;//0;//ASDU_LNG // 以下值为 0 的字段位是104暂时不需要
    chnlCfg_p[1].tmpApduDscr.ASDU_CMTBL[3] = 248;//1;//ASDU_VSQ
    chnlCfg_p[1].tmpApduDscr.ASDU_CMTBL[4] = 0;//ASDU_COT
    chnlCfg_p[1].tmpApduDscr.ASDU_CMTBL[5] = 2;//ASDU_ADD
    chnlCfg_p[1].tmpApduDscr.ASDU_CMTBL[6] = 0;//BODY_TID
    chnlCfg_p[1].tmpApduDscr.ASDU_CMTBL[7] = 3;//BODY_ADD
    chnlCfg_p[1].tmpApduDscr.ASDU_CMTBL[8] = 40;//BODY_SET
    chnlCfg_p[1].tmpApduDscr.ASDU_CMTBL[9] = 0;//BODY_TSP
    chnlCfg_p[1].tmpApduDscr.ASDU_CMTBL[10] = 0; //ASDU_TSP
    chnlCfg_p[1].tmpApduDscr.InfoEleLng = 1;


    // 将远程站指针关联到 通道数据结构中
    chnlCfg_p[2].stage = 0;
    chnlCfg_p[2].role = MC_CTL_STN;
    chnlCfg_p[2].stnRole = MC_CTLED_STN;
    chnlCfg_p[2].cfgLen = 4;
    chnlCfg_p[2].ptclMdlNum = 104;
    chnlCfg_p[2].bufLng = 255;
    chnlCfg_p[2].cfgAddr_p = new addrRange[chnlCfg_p[0].cfgLen]();
    chnlCfg_p[2].usrType  = 0;
    chnlCfg_p[2].cfgAddr_p[0].useNum = 520;// 实际在用点数
    chnlCfg_p[2].cfgAddr_p[0].begAddr = 0x4001;
    chnlCfg_p[2].cfgAddr_p[0].endAddr = 0x5209;
    chnlCfg_p[2].cfgAddr_p[1].useNum = 260;// 实际在用点数
    chnlCfg_p[2].cfgAddr_p[1].begAddr = 0x01;
    chnlCfg_p[2].cfgAddr_p[1].endAddr = 0x600;
    chnlCfg_p[2].cfgAddr_p[2].useNum = 40;// 实际在用点数
    chnlCfg_p[2].cfgAddr_p[2].begAddr = 0x1080;
    chnlCfg_p[2].cfgAddr_p[2].endAddr = 0x1120;
    chnlCfg_p[2].cfgAddr_p[2].useNum = 20;// 实际在用点数
    chnlCfg_p[2].cfgAddr_p[2].begAddr = 0x14;
    chnlCfg_p[2].cfgAddr_p[2].endAddr = 0x1e;

    chnlCfg_p[2].tmpApduDscr.ASDU_TID = 0;//100 // 默认规则不用，配置子系统时 要注意根据会话号不同而区别
    chnlCfg_p[2].tmpApduDscr.ASDU_CMTBL[0] = 6;//APCI
    chnlCfg_p[2].tmpApduDscr.ASDU_CMTBL[1] = 1;//ASDU_TID
    chnlCfg_p[2].tmpApduDscr.ASDU_CMTBL[2] = 0;//ASDU_LNG // 以下值为 0 的字段位是104暂时不需要
    chnlCfg_p[2].tmpApduDscr.ASDU_CMTBL[3] = 1;//ASDU_VSQ
    chnlCfg_p[2].tmpApduDscr.ASDU_CMTBL[4] = 2;//ASDU_COT
    chnlCfg_p[2].tmpApduDscr.ASDU_CMTBL[5] = 2;//ASDU_ADD
    chnlCfg_p[2].tmpApduDscr.ASDU_CMTBL[6] = 0;//BODY_TID
    chnlCfg_p[2].tmpApduDscr.ASDU_CMTBL[7] = 3;//BODY_ADD
    chnlCfg_p[2].tmpApduDscr.ASDU_CMTBL[8] = 48;//BODY_SET
    chnlCfg_p[2].tmpApduDscr.ASDU_CMTBL[9] = 0;//BODY_TSP
    chnlCfg_p[2].tmpApduDscr.ASDU_CMTBL[10] = 0; //ASDU_TSP
    chnlCfg_p[2].tmpApduDscr.InfoEleLng = 5;
    // 将远程站指针关联到 通道数据结构中
    chnlCfg_p[2].stage = 0;
    chnlCfg_p[2].role = MC_CTLED_STN;
    chnlCfg_p[2].stnRole = MC_CTL_STN;
    chnlCfg_p[2].cfgLen = 4;
    chnlCfg_p[2].ptclMdlNum = 104;
    chnlCfg_p[2].bufLng = 255;
    chnlCfg_p[2].cfgAddr_p = new addrRange[chnlCfg_p[0].cfgLen]();
    chnlCfg_p[2].usrType  = 0;
    chnlCfg_p[2].cfgAddr_p[0].useNum = 520;// 实际在用点数
    chnlCfg_p[2].cfgAddr_p[0].begAddr = 0x4001;
    chnlCfg_p[2].cfgAddr_p[0].endAddr = 0x5209;
    chnlCfg_p[2].cfgAddr_p[1].useNum = 260;// 实际在用点数
    chnlCfg_p[2].cfgAddr_p[1].begAddr = 0x01;
    chnlCfg_p[2].cfgAddr_p[1].endAddr = 0x600;
    chnlCfg_p[2].cfgAddr_p[2].useNum = 40;// 实际在用点数
    chnlCfg_p[2].cfgAddr_p[2].begAddr = 0x1080;
    chnlCfg_p[2].cfgAddr_p[2].endAddr = 0x1120;
    chnlCfg_p[2].cfgAddr_p[2].useNum = 20;// 实际在用点数
    chnlCfg_p[2].cfgAddr_p[2].begAddr = 0x14;
    chnlCfg_p[2].cfgAddr_p[2].endAddr = 0x1e;

    chnlCfg_p[2].tmpApduDscr.ASDU_TID = 0;//100 // 默认规则不用，配置子系统时 要注意根据会话号不同而区别
    chnlCfg_p[2].tmpApduDscr.ASDU_CMTBL[0] = 6;//APCI
    chnlCfg_p[2].tmpApduDscr.ASDU_CMTBL[1] = 1;//ASDU_TID
    chnlCfg_p[2].tmpApduDscr.ASDU_CMTBL[2] = 0;//ASDU_LNG // 以下值为 0 的字段位是104暂时不需要
    chnlCfg_p[2].tmpApduDscr.ASDU_CMTBL[3] = 1;//ASDU_VSQ
    chnlCfg_p[2].tmpApduDscr.ASDU_CMTBL[4] = 2;//ASDU_COT
    chnlCfg_p[2].tmpApduDscr.ASDU_CMTBL[5] = 2;//ASDU_ADD
    chnlCfg_p[2].tmpApduDscr.ASDU_CMTBL[6] = 0;//BODY_TID
    chnlCfg_p[2].tmpApduDscr.ASDU_CMTBL[7] = 3;//BODY_ADD
    chnlCfg_p[2].tmpApduDscr.ASDU_CMTBL[8] = 48;//BODY_SET
    chnlCfg_p[2].tmpApduDscr.ASDU_CMTBL[9] = 0;//BODY_TSP
    chnlCfg_p[2].tmpApduDscr.ASDU_CMTBL[10] = 0; //ASDU_TSP
    chnlCfg_p[2].tmpApduDscr.InfoEleLng = 5;

    //取ip
    int vRet = docXml.LoadFile ( "./scada_cfg.xml" );

    if ( vRet != 0 )
    {
        printf ( "errName=[%s]", docXml.ErrorName() );
        return;
    }

    const char* ip1_p = docXml.FirstChildElement ( "scada" )->FirstChildElement ( "remoteIP1" )->Attribute ( "ip" );
    const char* ip2_p = docXml.FirstChildElement ( "scada" )->FirstChildElement ( "remoteIP2" )->Attribute ( "ip" );
    const char* ip3_p = docXml.FirstChildElement ( "scada" )->FirstChildElement ( "remoteIP3" )->Attribute ( "ip" );
    const char* ip4_p = docXml.FirstChildElement ( "scada" )->FirstChildElement ( "remoteIP4" )->Attribute ( "ip" );
    memcpy ( chnlCfg_p[0].szStnIp, ip1_p, strlen ( ip1_p ) );
    memcpy ( chnlCfg_p[1].szStnIp, ip2_p, strlen ( ip2_p ) );
    memcpy ( chnlCfg_p[2].szStnIp, ip3_p, strlen ( ip3_p ) );
    memcpy ( chnlCfg_p[3].szStnIp, ip4_p, strlen ( ip4_p ) );
    //取端口号
    int tPortNum1 = 0;
    int tPortNum2 = 0;
    int tPortNum3 = 0;
    int tPortNum4 = 0;
    docXml.FirstChildElement ( "scada" )->FirstChildElement ( "remoteIP1" )->QueryIntAttribute ( "port", &tPortNum1 );
    docXml.FirstChildElement ( "scada" )->FirstChildElement ( "remoteIP2" )->QueryIntAttribute ( "port", &tPortNum2 );
    docXml.FirstChildElement ( "scada" )->FirstChildElement ( "remoteIP3" )->QueryIntAttribute ( "port", &tPortNum3 );
    docXml.FirstChildElement ( "scada" )->FirstChildElement ( "remoteIP4" )->QueryIntAttribute ( "port", &tPortNum4 );
    //{"192.168.121.210","192.168.120.210","192.120.3.150"}; // 远程站 IP 地址
    //szStnIp[0] =  *ip1_p;
    chnlCfg_p[0].szStnPort = tPortNum1;
    chnlCfg_p[1].szStnPort = tPortNum2;
    chnlCfg_p[2].szStnPort = tPortNum3;
    chnlCfg_p[3].szStnPort = tPortNum4;
    //int szStnPort[MC_DEF_STN_NUM] = { tPortNum1 , tPortNum2 ,tPortNum3}; // 远程站服务端口

    int tlocalPortNum1 = 0;
    int tlocalPortNum2 = 0;
    int tlocalPortNum3 = 0;
    int tlocalPortNum4 = 0;
    docXml.FirstChildElement ( "scada" )->FirstChildElement ( "remoteIP1" )->QueryIntAttribute ( "localport", &tlocalPortNum1 );
    docXml.FirstChildElement ( "scada" )->FirstChildElement ( "remoteIP2" )->QueryIntAttribute ( "localport", &tlocalPortNum2 );
    docXml.FirstChildElement ( "scada" )->FirstChildElement ( "remoteIP3" )->QueryIntAttribute ( "localport", &tlocalPortNum3 );
    docXml.FirstChildElement ( "scada" )->FirstChildElement ( "remoteIP4" )->QueryIntAttribute ( "localport", &tlocalPortNum4 );
    chnlCfg_p[0].localPort = tlocalPortNum1;
    chnlCfg_p[1].localPort = tlocalPortNum2;
    chnlCfg_p[2].localPort = tlocalPortNum3;
    chnlCfg_p[3].localPort = tlocalPortNum4;
    const char* localip1_p = docXml.FirstChildElement ( "scada" )->FirstChildElement ( "remoteIP1" )->Attribute ( "localip" );
    const char* localip2_p = docXml.FirstChildElement ( "scada" )->FirstChildElement ( "remoteIP2" )->Attribute ( "localip" );
    const char* localip3_p = docXml.FirstChildElement ( "scada" )->FirstChildElement ( "remoteIP3" )->Attribute ( "localip" );
    const char* localip4_p = docXml.FirstChildElement ( "scada" )->FirstChildElement ( "remoteIP4" )->Attribute ( "localip" );
    memcpy ( chnlCfg_p[0].localszStnIp, localip1_p, strlen ( localip1_p ) );
    memcpy ( chnlCfg_p[1].localszStnIp, localip2_p, strlen ( localip2_p ) );
    memcpy ( chnlCfg_p[2].localszStnIp, localip3_p, strlen ( localip3_p ) );
    memcpy ( chnlCfg_p[3].localszStnIp, localip4_p, strlen ( localip4_p ) );
}



//现在的版本废弃此接口
XMLDocument* cfgManage::getCfgFile()
{
    return &docXml;
}
#endif


//取得从xfm读的配置结构chnlObjCfg
chnlObjCfg* cfgManage::getchnlObjCfg()
{
    if ( chnlCfg_p[1].tmpApduDscr.ASDU_CMTBL[0] == 0 )
    {
        printf ( "chnlCfg_p[1].tmpApduDscr.ASDU_CMTBL[0] = %d\r\n", chnlCfg_p[1].tmpApduDscr.ASDU_CMTBL[0] );
    }

    return chnlCfg_p;
}


//取得从xfm读的配置结构T_lcalIps
T_lcalIps* cfgManage::getDataBaseIP()
{
    return &outLcalIps;
}



//modbus  会话:规则步及相关配置数据组装
void cfgManage::setModbsQueDataCfg ( int ssnNo, T_snnCfg* snnCfg, T_ptclFunDsc* funDsc, const int& index, u_short offset )
{
    T_stnAcsFlagM tmpStnAcsFlag = {0, 0, 1, 0};
    u_char ssnid = ssnNo;
    u_char actFlig = 0;//主被动型,由规约设置
    u_char infoType = GINFOTYPE;//
    u_short cmnAddr = 1;//公共地址
    u_char stepNum = 0;//步数

    if ( ssnNo == MC_MBFC_0xFE )
    {
        stepNum = 1;
    }
    else
    {
        stepNum = 2;
    }

    /*******************************************设置MODBUS回话**********************************************/
    setCfg ( & ( snnCfg[index] ), ssnid, actFlig, infoType, cmnAddr, stepNum );

    /*******************************************第一步**********************************************/
    u_char reqAPDUNum = 1;//请求帧个数
    u_char respAPDUNum = 0;//响应帧个数
    u_char body_setNum = 1;//一帧数据中，信息对象或信息元素的个数
    //对第一步
    //[1,2] 位1 = 1存储子系统执行数据值的保存；
    //位2 = 1存储子系统执行数据值的加载
    tmpStnAcsFlag.ctlType = 0;
    snnCfg[index].stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
    setModbsStepCfg ( & ( snnCfg[index].stepCfg_p[0] ), * ( u_short* ) &tmpStnAcsFlag, reqAPDUNum, respAPDUNum );
    u_char tmpData_p[256] = {}; //用于存储PDU中数据域的数据

    switch ( ssnNo )
    {
        case MC_MBFC_0xFE:
            snnCfg[index].stepCfg_p[0].reqBody_setNum_p[0] = body_setNum;
            snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.mbapHeader.trans_id = 0x01;
            snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.mbapHeader.pctl_id = 0x0;
            snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.mbapHeader.length = 0x06;
            snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.mbapHeader.unit_id = 0x01;
            snnCfg[index].offset = offset;
            break;

        case MC_MBFC_0x01:
            snnCfg[index].stepCfg_p[0].reqBody_setNum_p[0] = body_setNum;
            snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.mbapHeader.trans_id = 0x01;
            snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.mbapHeader.pctl_id = 0x0;
            snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.mbapHeader.length = 0x06;
            snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.mbapHeader.unit_id = 0x01;
            snnCfg[index].offset = offset;
            break;

        case MC_MBFC_0x02:
            snnCfg[index].stepCfg_p[0].reqBody_setNum_p[0] = body_setNum;
            snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.mbapHeader.trans_id = 0x01;
            snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.mbapHeader.pctl_id = 0x0;
            snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.mbapHeader.length = 0x06;
            snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.mbapHeader.unit_id = 0x01;
            snnCfg[index].offset = 0;
            snnCfg[index].offset = offset;
            break;

        case MC_MBFC_0x03:
            snnCfg[index].stepCfg_p[0].reqBody_setNum_p[0] = body_setNum;
            snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.mbapHeader.trans_id = 0x01;
            snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.mbapHeader.pctl_id = 0x0;
            snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.mbapHeader.length = 0x06;
            snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.mbapHeader.unit_id = 0x01;
            snnCfg[index].offset = offset;
            break;

        case MC_MBFC_0x04:
            snnCfg[index].stepCfg_p[0].reqBody_setNum_p[0] = body_setNum;
            snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.mbapHeader.trans_id = 0x01;
            snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.mbapHeader.pctl_id = 0x0;
            snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.mbapHeader.length = 0x06;
            snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.mbapHeader.unit_id = 0x01;
            snnCfg[index].offset = offset;
            break;

        case MC_MBFC_0x05:
            snnCfg[index].stepCfg_p[0].reqBody_setNum_p[0] = body_setNum;
            snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.mbapHeader.trans_id = 0x01;
            snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.mbapHeader.pctl_id = 0x0;
            snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.mbapHeader.length = 0x06;
            snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.mbapHeader.unit_id = 0x01;
            snnCfg[index].offset = offset;
            break;

        case MC_MBFC_0x06:
            snnCfg[index].stepCfg_p[0].reqBody_setNum_p[0] = body_setNum;
            snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.mbapHeader.trans_id = 0x01;
            snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.mbapHeader.pctl_id = 0x0;
            snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.mbapHeader.length = 0x06;
            snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.mbapHeader.unit_id = 0x01;
            snnCfg[index].offset = offset;
            break;

        default:
            break;
    }

    u_char memVal[ADU_CMTBL_NUM] = {};
    ELE_TYPE eleVal[ADU_CMTBL_NUM] =
    {
        //mbap报文头里的“单元标识符”1字节 配置到 ADU_ADDR_INDEX 域
        {ADU_ADDR_INDEX, 0, &memVal[1]},
        {PDU_FC_INDEX, 1, &memVal[2]},
        {PDU_DATA_INDEX, 4, tmpData_p},
        {ADU_ECHK_INDEX, 0, &memVal[4]}
    };
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.aduAddr ), eleVal[0] );
    snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.pduFc = ssnNo;
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.pduData ), eleVal[2] );
    setEleVal ( & ( snnCfg[index].stepCfg_p[0].comReqData_p[0].modbus.aduEchk ), eleVal[3] );

    /*******************************************第二步**********************************************/
    if ( ssnNo != MC_MBFC_0xFE )
    {

        reqAPDUNum = 0;//请求帧个数
        respAPDUNum = 1;//响应帧个数
        body_setNum = 1;//一帧数据中，信息对象或信息元素的个数
        tmpStnAcsFlag.ctlType = 1;
        snnCfg[index].stepCfg_p[1].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
        setModbsStepCfg ( & ( snnCfg[index].stepCfg_p[1] ), * ( u_short* ) &tmpStnAcsFlag, reqAPDUNum, respAPDUNum );
        u_char usingMode = 0;
        setFunDsc ( & ( funDsc[index] ), ssnNo, ACTIVE_TYPE, usingMode, stepNum );

        setPtclStep ( & ( funDsc[index].remoteSet_p[0] ), MODBUS_COT_ACT, MAX104_ASDU_LNG, 1 );
        //funDsc[index].remoteSet_p[0].TIDLng_p[0] = tid_lng;// 这个数组的大小取决于remoteSet_p->setNum的值
        funDsc[index].remoteSet_p[0].TID_p[0] = ssnNo;
    }
    else
    {
        u_char usingMode = 0;
        setFunDsc ( & ( funDsc[index] ), ssnNo, ACTIVE_TYPE, usingMode, stepNum );

        setPtclStep ( & ( funDsc[index].remoteSet_p[0] ), MODBUS_COT_ACT, MAX104_ASDU_LNG, 1 );
        //funDsc[index].remoteSet_p[0].TIDLng_p[0] = tid_lng;// 这个数组的大小取决于remoteSet_p->setNum的值
        funDsc[index].remoteSet_p[0].TID_p[0] = ssnNo;
    }

    ;

    /*
    cot = 7;
    setPtclStep ( & ( funDsc[index].remoteSet_p[1] ), cot, 249, 1 );
    funDsc[index].remoteSet_p[0].TIDLng_p[0] = tid_lng;// 这个数组的大小取决于remoteSet_p->setNum的值
    funDsc[index].remoteSet_p[0].TID_p[0] = tid;*/

}


//取得从xml读取的初始化共用内存的配置数据
T_initChnUnit* cfgManage::getChnUnit()
{
    return outChnUnit_p;
}


//给ssnReqInfo成员开空间
void cfgManage::crtSsnRInfoCfg ( ssnReqInfo* ssnRInfo, int ssnRInfoNum )
{
    ssnRInfo->sRCnt_p = new ssnReqCnt[ssnRInfoNum]();
    ssnRInfo->cntNum = ssnRInfoNum;
}

//给ssnReqInfo成员赋值
void cfgManage::setSsnReqInfoCfg ( int chnlID, u_char ssnNo, u_char ptclNo,  int ssnRInfoIndex, u_short addrStart,
                                   u_short accumValue, int snnMaxRetryNum,  bool collMethods, ssnReqInfo* ssnRInfo )
{
    ssnRInfo->tReqInfo.chnlID = chnlID;
    ssnRInfo->tReqInfo.snnMaxRetryNum = snnMaxRetryNum;
    ssnRInfo->sRCnt_p[ssnRInfoIndex].collMethods = collMethods;
    ssnRInfo->sRCnt_p[ssnRInfoIndex].ssn.ptclNo = ptclNo;
    ssnRInfo->sRCnt_p[ssnRInfoIndex].ssn.snnDscNo = ssnNo;
    ssnRInfo->sRCnt_p[ssnRInfoIndex].accumValue = accumValue;
    ssnRInfo->sRCnt_p[ssnRInfoIndex].addrStart = addrStart;
}
/*
ssnReqInfo* cfgManage::getSsnRInfoCfg ( int chnlID )
{
    return & ( ssnRInfo[chnlID] );
}

ssnReqInfo* cfgManage::getSsnRInfoCfg ( )
{
    return ssnRInfo;
}*/

//给T_phyValCfg结构赋值
void cfgManage::setPhyObjValCfg ( T_phyValCfg*  phyObjVal,   PHY_UNIT_TYPE  phyType, PHY_CALC_METHD calcMethd,
                                  u_char phyValLng,  u_int numOfAdd, PNT_ADDR_TYPE* pntAddr_p, void* dataId_p  )
{
    phyObjVal->phyType   =  phyType;
    phyObjVal->calcMethd =  calcMethd;
    phyObjVal->phyValLng =  phyValLng;
    phyObjVal->numOfAdd  =  numOfAdd;
    phyObjVal->pntAddr_p = new PNT_ADDR_TYPE[numOfAdd]();
    memcpy ( phyObjVal->pntAddr_p, pntAddr_p, ( sizeof ( PNT_ADDR_TYPE ) * numOfAdd ) );
    phyObjVal->dataId_p  =  dataId_p;
}


//此函数现在的版本不用
void cfgManage::setInParCfg ( T_initChnUnit* inPar, int chnlID )
{
    DID did;
    /*************************************通道0***************************************/
    inPar->chnNum = 0;       //通道号
    inPar->pntObjLng = sizeof ( T_snnUsrData ); //点地址关联通用数据对象的长度
    inPar->dataIdLng = sizeof ( DATA_ID ); //数据id长度
    inPar->pntAddrNum = NOT_SURE;   //点地址数量
    inPar->phyUnitNum  = NOT_SURE;   //物理量个数
    inPar->pntAddrCfg.num = chnlCfg_p[0].cfgLen;   //点地址段表头
    inPar->pntAddrCfg.contPntAdd_p = new T_contPntAdd[inPar->pntAddrCfg.num]();   //点地址段表头

    for ( u_int i = 0; i < inPar->pntAddrCfg.num; i++ )
    {
        inPar->pntAddrCfg.contPntAdd_p[i].beginAddrVal = chnlCfg_p[chnlID].cfgAddr_p[i].begAddr;
        inPar->pntAddrCfg.contPntAdd_p[i].endAddrVal = chnlCfg_p[chnlID].cfgAddr_p[i].endAddr;
    }

    inPar->phyObjVal_p = new T_phyValCfg[inPar->phyUnitNum]();   //物理量表头

    u_int pntAddr[6] = {1, 2, 3, 4, 5, 6};
    DATA_ID dataId;
    memset ( &dataId, 0, sizeof ( DATA_ID ) );
    dataId = * ( DATA_ID* ) did.MakeDate ( DATACATALOG_DD, DATAKIND_INVALID, 1 );
    setPhyObjValCfg ( & ( inPar->phyObjVal_p[0] ), 1, PHY_CALC_CCNT,
                      sizeof ( DATAFORMAT_DATE7B1 ), 6, pntAddr, &dataId ); //........

}







/*****************************************************************************
     以下为       傅世凯 合并解析xml配置文件的接口
******************************************************************************/

//生成data id 需要的参数变量
typedef struct
{
    int funcType;
    int ordNum;
    int dcatalog;
    int dkind;
    int dtype;
    int dlen;
    int amount;
    int ectype;
    int ecsn;
    int srctype;
    int srcsn;
    int htype;
    float hvalue;
    int itype;
    int ivalue;
} T_DIDfaceVal;



typedef struct
{
    const char* enKey_p;
    int enVal;
} T_enumKeyVal;

//const char* getEnumStr ( const char* name_p )
//{
//    return name_p;
//}

#define enToStr(val) (#val)

/************************************************
          定义物理量的计算方法枚举 key,value结构
          要使用新的枚举需要下相应结构中添加
*************************************************/
//PHY_CALC_METHD key value
const u_int g_phyCalcNum = 7;
const T_enumKeyVal g_phyCalcKV[g_phyCalcNum] ={
    {enToStr ( PHY_CALC_INVALID     ), PHY_CALC_INVALID     },
    {enToStr ( PHY_CALC_SAVE        ), PHY_CALC_SAVE        },
    {enToStr ( PHY_CALC_CCNT        ), PHY_CALC_CCNT        },
    {enToStr ( PHY_CALC_SUM         ), PHY_CALC_SUM         },
    {enToStr ( PHY_CALC_CCNT_AND_SUM ), PHY_CALC_CCNT_AND_SUM},
    {enToStr ( PHY_CALC_CCNT_AND_BIT ), PHY_CALC_CCNT_AND_BIT},
    {enToStr ( PHY_CALC_LAST        ), PHY_CALC_LAST        }
};


/************************************************
          定义生成数据id需要的枚举 key,value结构
          要使用新的枚举需要下相应结构中添加
*************************************************/
//DATA_CATALOG key value
const u_int g_dcatalogNum = 51;
const T_enumKeyVal g_dcatalogKV[g_dcatalogNum] ={
    {enToStr ( DATACATALOG_INVALID  ), DATACATALOG_INVALID},
    {enToStr ( DATACATALOG_WS       ), DATACATALOG_WS     },
    {enToStr ( DATACATALOG_WD       ), DATACATALOG_WD     },
    {enToStr ( DATACATALOG_AT       ), DATACATALOG_AT     },
    {enToStr ( DATACATALOG_AH       ), DATACATALOG_AH     },
    {enToStr ( DATACATALOG_AD       ), DATACATALOG_AD     },
    {enToStr ( DATACATALOG_AP       ), DATACATALOG_AP     },
    {enToStr ( DATACATALOG_BL       ), DATACATALOG_BL     },
    {enToStr ( DATACATALOG_CT       ), DATACATALOG_CT     },
    {enToStr ( DATACATALOG_RF       ), DATACATALOG_RF     },
    {enToStr ( DATACATALOG_TA       ), DATACATALOG_TA     },
    {enToStr ( DATACATALOG_EC       ), DATACATALOG_EC     },
    {enToStr ( DATACATALOG_HT       ), DATACATALOG_HT     },
    {enToStr ( DATACATALOG_SD       ), DATACATALOG_SD     },
    {enToStr ( DATACATALOG_DD       ), DATACATALOG_DD     },
    {enToStr ( DATACATALOG_FD       ), DATACATALOG_FD     },
    {enToStr ( DATACATALOG_PD       ), DATACATALOG_PD     },
    {enToStr ( DATACATALOG_DS       ), DATACATALOG_DS     },
    {enToStr ( DATACATALOG_DT       ), DATACATALOG_DT     },
    {enToStr ( DATACATALOG_NT       ), DATACATALOG_NT     },
    {enToStr ( DATACATALOG_ET       ), DATACATALOG_ET     },
    {enToStr ( DATACATALOG_SL       ), DATACATALOG_SL     },
    {enToStr ( DATACATALOG_PP       ), DATACATALOG_PP     },
    {enToStr ( DATACATALOG_PQ       ), DATACATALOG_PQ     },
    {enToStr ( DATACATALOG_CP       ), DATACATALOG_CP     },
    {enToStr ( DATACATALOG_GE       ), DATACATALOG_GE     },
    {enToStr ( DATACATALOG_TP       ), DATACATALOG_TP     },
    {enToStr ( DATACATALOG_TE       ), DATACATALOG_TE     },
    {enToStr ( DATACATALOG_SF       ), DATACATALOG_SF     },
    {enToStr ( DATACATALOG_PF       ), DATACATALOG_PF     },
    {enToStr ( DATACATALOG_FQ       ), DATACATALOG_FQ     },
    {enToStr ( DATACATALOG_RS       ), DATACATALOG_RS     },
    {enToStr ( DATACATALOG_PA       ), DATACATALOG_PA     },
    {enToStr ( DATACATALOG_YA       ), DATACATALOG_YA     },
    {enToStr ( DATACATALOG_PV       ), DATACATALOG_PV     },
    {enToStr ( DATACATALOG_PI       ), DATACATALOG_PI     },
    {enToStr ( DATACATALOG_TS       ), DATACATALOG_TS     },
    {enToStr ( DATACATALOG_FN       ), DATACATALOG_FN     },
    {enToStr ( DATACATALOG_TN       ), DATACATALOG_TN     },
    {enToStr ( DATACATALOG_PM       ), DATACATALOG_PM     },
    {enToStr ( DATACATALOG_SN       ), DATACATALOG_SN     },
    {enToStr ( DATACATALOG_NM       ), DATACATALOG_NM     },
    {enToStr ( DATACATALOG_ID       ), DATACATALOG_ID     },
    {enToStr ( DATACATALOG_SC       ), DATACATALOG_SC     },
    {enToStr ( DATACATALOG_IS       ), DATACATALOG_IS     },
    {enToStr ( DATACATALOG_VP       ), DATACATALOG_VP     },
    {enToStr ( DATACATALOG_BP       ), DATACATALOG_BP     },
    {enToStr ( DATACATALOG_CM       ), DATACATALOG_CM     },
    {enToStr ( DATACATALOG_LAST     ), DATACATALOG_LAST   },
    {enToStr ( DATACATALOG_ALL      ), DATACATALOG_ALL    },
    {enToStr ( DATACATALOG_VALID    ), DATACATALOG_VALID  }

};

//DATA_KIND key value
const u_int g_dkindNum = 83;
const T_enumKeyVal g_dkindKV[g_dkindNum] ={
    {enToStr ( DATAKIND_INVALID ), DATAKIND_INVALID},
    {enToStr ( DATAKIND_RTV    ), DATAKIND_RTV    },
    {enToStr ( DATAKIND_AVG    ), DATAKIND_AVG    },
    {enToStr ( DATAKIND_MAX    ), DATAKIND_MAX    },
    {enToStr ( DATAKIND_MIN    ), DATAKIND_MIN    },
    {enToStr ( DATAKIND_AVG2   ), DATAKIND_AVG2   },
    {enToStr ( DATAKIND_PLV    ), DATAKIND_PLV    },
    {enToStr ( DATAKIND_EXV    ), DATAKIND_EXV    },
    {enToStr ( DATAKIND_THV    ), DATAKIND_THV    },
    {enToStr ( DATAKIND_UTV    ), DATAKIND_UTV    },
    {enToStr ( DATAKIND_LTV    ), DATAKIND_LTV    },
    {enToStr ( DATAKIND_MDV    ), DATAKIND_MDV    },
    {enToStr ( DATAKIND_SDV    ), DATAKIND_SDV    },
    {enToStr ( DATAKIND_3SAVG  ), DATAKIND_3SAVG  },
    {enToStr ( DATAKIND_OGNPLV ), DATAKIND_OGNPLV },
    {enToStr ( DATAKIND_ILD    ), DATAKIND_ILD    },
    {enToStr ( DATAKIND_PGG    ), DATAKIND_PGG    },
    {enToStr ( DATAKIND_WWG    ), DATAKIND_WWG    },
    {enToStr ( DATAKIND_SPG    ), DATAKIND_SPG    },
    {enToStr ( DATAKIND_CBG    ), DATAKIND_CBG    },
    {enToStr ( DATAKIND_TGV    ), DATAKIND_TGV    },
    {enToStr ( DATAKIND_UGV    ), DATAKIND_UGV    },
    {enToStr ( DATAKIND_IBV    ), DATAKIND_IBV    },
    {enToStr ( DATAKIND_OBV    ), DATAKIND_OBV    },
    {enToStr ( DATAKIND_CWSCLV ), DATAKIND_CWSCLV },
    {enToStr ( DATAKIND_PTBCLV ), DATAKIND_PTBCLV },
    {enToStr ( DATAKIND_OGN    ), DATAKIND_OGN    },
    {enToStr ( DATAKIND_STD    ), DATAKIND_STD    },
    {enToStr ( DATAKIND_DPH    ), DATAKIND_DPH    },
    {enToStr ( DATAKIND_NRV    ), DATAKIND_NRV    },
    {enToStr ( DATAKIND_AMV    ), DATAKIND_AMV    },
    {enToStr ( DATAKIND_CLV    ), DATAKIND_CLV    },
    {enToStr ( DATAKIND_AGV    ), DATAKIND_AGV    },
    {enToStr ( DATAKIND_WAV    ), DATAKIND_WAV    },
    {enToStr ( DATAKIND_PGGTHV ), DATAKIND_PGGTHV },
    {enToStr ( DATAKIND_PGGAMV ), DATAKIND_PGGAMV },
    {enToStr ( DATAKIND_OVH    ), DATAKIND_OVH    },
    {enToStr ( DATAKIND_CTF    ), DATAKIND_CTF    },
    {enToStr ( DATAKIND_BLD    ), DATAKIND_BLD    },
    {enToStr ( DATAKIND_PSA    ), DATAKIND_PSA    },
    {enToStr ( DATAKIND_PSB    ), DATAKIND_PSB    },
    {enToStr ( DATAKIND_PSC    ), DATAKIND_PSC    },
    {enToStr ( DATAKIND_MAXCT  ), DATAKIND_MAXCT  },
    {enToStr ( DATAKIND_MINCT  ), DATAKIND_MINCT  },
    {enToStr ( DATAKIND_MAXBL  ), DATAKIND_MAXBL  },
    {enToStr ( DATAKIND_MINBL  ), DATAKIND_MINBL  },
    {enToStr ( DATAKIND_MAXPP  ), DATAKIND_MAXPP  },
    {enToStr ( DATAKIND_MINPP  ), DATAKIND_MINPP  },
    {enToStr ( DATAKIND_MAXWS  ), DATAKIND_MAXWS  },
    {enToStr ( DATAKIND_MINWS  ), DATAKIND_MINWS  },
    {enToStr ( DATAKIND_MAXWD  ), DATAKIND_MAXWD  },
    {enToStr ( DATAKIND_MINWD  ), DATAKIND_MINWD  },
    {enToStr ( DATAKIND_MAXAT  ), DATAKIND_MAXAT  },
    {enToStr ( DATAKIND_MINAT  ), DATAKIND_MINAT  },
    {enToStr ( DATAKIND_MAXAH  ), DATAKIND_MAXAH  },
    {enToStr ( DATAKIND_MINAH  ), DATAKIND_MINAH  },
    {enToStr ( DATAKIND_MAXAD  ), DATAKIND_MAXAD  },
    {enToStr ( DATAKIND_MINAD  ), DATAKIND_MINAD  },
    {enToStr ( DATAKIND_MAXAP  ), DATAKIND_MAXAP  },
    {enToStr ( DATAKIND_MINAP  ), DATAKIND_MINAP  },
    {enToStr ( DATAKIND_3SAVG1 ), DATAKIND_3SAVG1 },
    {enToStr ( DATAKIND_3SAVG2 ), DATAKIND_3SAVG2 },
    {enToStr ( DATAKIND_3SAVG3 ), DATAKIND_3SAVG3 },
    {enToStr ( DATAKIND_3SAVG4 ), DATAKIND_3SAVG4 },
    {enToStr ( DATAKIND_3SAVG5 ), DATAKIND_3SAVG5 },
    {enToStr ( DATAKIND_3SAVG6 ), DATAKIND_3SAVG6 },
    {enToStr ( DATAKIND_3SAVG7 ), DATAKIND_3SAVG7 },
    {enToStr ( DATAKIND_3SAVG8 ), DATAKIND_3SAVG8 },
    {enToStr ( DATAKIND_3SAVG9 ), DATAKIND_3SAVG9 },
    {enToStr ( DATAKIND_3SAVG10 ), DATAKIND_3SAVG10},
    {enToStr ( DATAKIND_3SAVG11 ), DATAKIND_3SAVG11},
    {enToStr ( DATAKIND_3SAVG12 ), DATAKIND_3SAVG12},
    {enToStr ( DATAKIND_3SAVG13 ), DATAKIND_3SAVG13},
    {enToStr ( DATAKIND_3SAVG14 ), DATAKIND_3SAVG14},
    {enToStr ( DATAKIND_3SAVG15 ), DATAKIND_3SAVG15},
    {enToStr ( DATAKIND_3SAVG16 ), DATAKIND_3SAVG16},
    {enToStr ( DATAKIND_3SAVG17 ), DATAKIND_3SAVG17},
    {enToStr ( DATAKIND_3SAVG18 ), DATAKIND_3SAVG18},
    {enToStr ( DATAKIND_3SAVG19 ), DATAKIND_3SAVG19},
    {enToStr ( DATAKIND_3SAVG20 ), DATAKIND_3SAVG20},
    {enToStr ( DATAKIND_LAST   ), DATAKIND_LAST   },
    {enToStr ( DATAKIND_ALL    ), DATAKIND_ALL    },
    {enToStr ( DATAKIND_VALID  ), DATAKIND_VALID  }
};

//DATA_TYPE key value
const u_int g_dtypeNum = 20;
const T_enumKeyVal g_dtypeKV[g_dtypeNum] ={
    {enToStr ( DATATYPE_BOOL   ), DATATYPE_BOOL   },
    {enToStr ( DATATYPE_UINT8  ), DATATYPE_UINT8  },
    {enToStr ( DATATYPE_SINT8  ), DATATYPE_SINT8  },
    {enToStr ( DATATYPE_UINT16 ), DATATYPE_UINT16 },
    {enToStr ( DATATYPE_SINT16 ), DATATYPE_SINT16 },
    {enToStr ( DATATYPE_UINT32 ), DATATYPE_UINT32 },
    {enToStr ( DATATYPE_SINT32 ), DATATYPE_SINT32 },
    {enToStr ( DATATYPE_FLOAT16 ), DATATYPE_FLOAT16},
    {enToStr ( DATATYPE_FLOAT32 ), DATATYPE_FLOAT32},
    {enToStr ( DATATYPE_DATE7B ), DATATYPE_DATE7B },
    {enToStr ( DATATYPE_UINT64 ), DATATYPE_UINT64 },
    {enToStr ( DATATYPE_SINT64 ), DATATYPE_SINT64 },
    {enToStr ( DATATYPE_FLOAT64 ), DATATYPE_FLOAT64},
    {enToStr ( DATATYPE_ASCII  ), DATATYPE_ASCII  },
    {enToStr ( DATATYPE_GBK    ), DATATYPE_GBK    },
    {enToStr ( DATATYPE_UTF8   ), DATATYPE_UTF8   },
    {enToStr ( DATATYPE_CUSTOM1 ), DATATYPE_CUSTOM1},
    {enToStr ( DATATYPE_INVALID ), DATATYPE_INVALID},
    {enToStr ( DATATYPE_ALL    ), DATATYPE_ALL    },
    {enToStr ( DATATYPE_VALID  ), DATATYPE_VALID  }
};

//DATA_ENCODETYPE key value
const u_int g_ectypeNum = 14;
const T_enumKeyVal g_ectypeKV[g_ectypeNum] ={
    {enToStr ( ENCODETYPE_INVALID ), ENCODETYPE_INVALID},
    {enToStr ( ENCODETYPE_FARM   ), ENCODETYPE_FARM   },
    {enToStr ( ENCODETYPE_MODEL  ), ENCODETYPE_MODEL  },
    {enToStr ( ENCODETYPE_STAGE  ), ENCODETYPE_STAGE  },
    {enToStr ( ENCODETYPE_TURBINE ), ENCODETYPE_TURBINE},
    {enToStr ( ENCODETYPE_AMT    ), ENCODETYPE_AMT    },
    {enToStr ( ENCODETYPE_NWP    ), ENCODETYPE_NWP    },
    {enToStr ( ENCODETYPE_TSS    ), ENCODETYPE_TSS    },
    {enToStr ( ENCODETYPE_TPS    ), ENCODETYPE_TPS    },
    {enToStr ( ENCODETYPE_EMS    ), ENCODETYPE_EMS    },
    {enToStr ( ENCODETYPE_IAS    ), ENCODETYPE_IAS    },
    {enToStr ( ENCODETYPE_AGC    ), ENCODETYPE_AGC    },
    {enToStr ( ENCODETYPE_ALL    ), ENCODETYPE_ALL    },
    {enToStr ( ENCODETYPE_VALID  ), ENCODETYPE_VALID  }
};

//DATA_SOURCETYPE key value
const u_int g_srctypeNum = 17;
const T_enumKeyVal g_srctypeKV[g_srctypeNum] ={
    {enToStr ( SOURCETYPE_INVALID ), SOURCETYPE_INVALID},
    {enToStr ( SOURCETYPE_AMT    ), SOURCETYPE_AMT    },
    {enToStr ( SOURCETYPE_TSS    ), SOURCETYPE_TSS    },
    {enToStr ( SOURCETYPE_IAS    ), SOURCETYPE_IAS    },
    {enToStr ( SOURCETYPE_NWP    ), SOURCETYPE_NWP    },
    {enToStr ( SOURCETYPE_STF    ), SOURCETYPE_STF    },
    {enToStr ( SOURCETYPE_UTF    ), SOURCETYPE_UTF    },
    {enToStr ( SOURCETYPE_TPS    ), SOURCETYPE_TPS    },
    {enToStr ( SOURCETYPE_HND    ), SOURCETYPE_HND    },
    {enToStr ( SOURCETYPE_EMS    ), SOURCETYPE_EMS    },
    {enToStr ( SOURCETYPE_AGC    ), SOURCETYPE_AGC    },
    {enToStr ( SOURCETYPE_PLN    ), SOURCETYPE_PLN    },
    {enToStr ( SOURCETYPE_SBI    ), SOURCETYPE_SBI    },
    {enToStr ( SOURCETYPE_CFG    ), SOURCETYPE_CFG    },
    {enToStr ( SOURCETYPE_LAST   ), SOURCETYPE_LAST   },
    {enToStr ( SOURCETYPE_ALL    ), SOURCETYPE_ALL    },
    {enToStr ( SOURCETYPE_VALID  ), SOURCETYPE_VALID  }
};

//DATA_HEIGHTTYPE key value
const u_int g_htypeNum = 6;
const T_enumKeyVal g_htypeKV[g_htypeNum] ={
    {enToStr ( HEIGHTTYPE_HUB    ), HEIGHTTYPE_HUB    },
    {enToStr ( HEIGHTTYPE_SL     ), HEIGHTTYPE_SL     },
    {enToStr ( HEIGHTTYPE_ES     ), HEIGHTTYPE_ES     },
    {enToStr ( HEIGHTTYPE_INVALID ), HEIGHTTYPE_INVALID},
    {enToStr ( HEIGHTTYPE_ALL    ), HEIGHTTYPE_ALL    },
    {enToStr ( HEIGHTTYPE_VALID  ), HEIGHTTYPE_VALID  }
};

//DATA_INTERVALUNIT key value
const u_int g_itypeNum = 12;
const T_enumKeyVal g_itypeKV[g_itypeNum] ={
    {enToStr ( INTERVALUNIT_INVALID ), INTERVALUNIT_INVALID },
    {enToStr ( INTERVALUNIT_UNDEFINE ), INTERVALUNIT_UNDEFINE},
    {enToStr ( INTERVALUNIT_SECOND  ), INTERVALUNIT_SECOND  },
    {enToStr ( INTERVALUNIT_3SECOND ), INTERVALUNIT_3SECOND },
    {enToStr ( INTERVALUNIT_MINUTE  ), INTERVALUNIT_MINUTE  },
    {enToStr ( INTERVALUNIT_5MINUTE ), INTERVALUNIT_5MINUTE },
    {enToStr ( INTERVALUNIT_HOUR    ), INTERVALUNIT_HOUR    },
    {enToStr ( INTERVALUNIT_5HOUR   ), INTERVALUNIT_5HOUR   },
    {enToStr ( INTERVALUNIT_DAY     ), INTERVALUNIT_DAY     },
    {enToStr ( INTERVALUNIT_LAST    ), INTERVALUNIT_LAST    },
    {enToStr ( INTERVALUNIT_ALL     ), INTERVALUNIT_ALL     },
    {enToStr ( INTERVALUNIT_VALID   ), INTERVALUNIT_VALID   }
};


/*
获得只读字符串中的头和尾去掉空格 TAB 回车
后可见字符的开始与结尾索引
*/
void trimConst ( const char* s_p,
                 int& lowIndex,
                 int& highIndex
               )
{
    lowIndex = 0;
    highIndex = 0;

    if ( s_p == NULL )
    {
        return;
    }

    highIndex = strlen ( s_p ) - 1;

    while ( ( lowIndex <= highIndex )
            && ( s_p[lowIndex] <= 32 )
            && ( s_p[lowIndex] > 0 )
          )
    {
        lowIndex++;
    }

    if ( lowIndex > highIndex )
    {
        lowIndex = 0;
        highIndex = 0;
    }
    else
    {
        while ( ( s_p[highIndex] <= 32 )
                && ( s_p[highIndex] > 0 )
              )
        {
            highIndex--;
        }
    }

    return;
}

//获得parentE_p结点下子结点eleName_p的个数
u_int getXmlChildEleNum ( const XMLElement* parentE_p,
                          const char* eleName_p
                        )
{
    if ( parentE_p == NULL
         || eleName_p == NULL
       )
    {
        return 0;
    }

    u_int num = 0;

    const XMLElement* childE_p = parentE_p->FirstChildElement ( eleName_p );

    if ( childE_p != NULL )
    {
        num++;

        while ( ( childE_p = childE_p->NextSiblingElement ( eleName_p )
                ) != NULL
              )
        {
            num++;
        }
    }

    return num;
}


//获得parentE_p结点下子结点eleName_p属性值
u_int getXmlChldEleAttr ( const XMLElement* parentE_p,
                          const char* eleName_p,
                          const char* attrName_p,
                          PHY_UNIT_TYPE* data_p
                        )
{
    if ( parentE_p == NULL
         || eleName_p == NULL
         || attrName_p == NULL
         || data_p == NULL
       )
    {
        return 0;
    }

    u_int num = 0;

    const XMLElement* childE_p = parentE_p->FirstChildElement ( eleName_p );

    if ( childE_p != NULL )
    {
        int tIntVal;
        childE_p->QueryIntAttribute ( attrName_p, &tIntVal );
        data_p[num] = ( PHY_UNIT_TYPE ) tIntVal;
        num++;

        while ( ( childE_p = childE_p->NextSiblingElement ( eleName_p )
                ) != NULL
              )
        {
            childE_p->QueryIntAttribute ( attrName_p, &tIntVal );
            data_p[num] = ( PHY_UNIT_TYPE ) tIntVal;
            num++;
        }
    }

    return num;
}


int getOneDidValS ( T_DIDfaceVal* didVal_p,
                    const XMLElement* dataIdE_p
                  )
{
    memset ( didVal_p, 0, sizeof ( T_DIDfaceVal ) );

    dataIdE_p->QueryIntAttribute ( "funcType", & ( didVal_p->funcType ) );
    dataIdE_p->QueryIntAttribute ( "ordNum", & ( didVal_p->ordNum   ) );
    //dataIdE_p->QueryIntAttribute ( "dcatalog", & ( didVal_p->dcatalog ) );
    const char* dcatalog_p = dataIdE_p->Attribute ( "dcatalog" );
    int lwIdx = 0;
    int hgIdx = 0;

    char tmpStr[MC_TMPSTR_LEN];
    memset ( tmpStr, 0, sizeof ( tmpStr ) );

    if ( dcatalog_p != NULL )
    {
        //去掉前后的空格
        trimConst ( dcatalog_p, lwIdx, hgIdx );
        memset ( tmpStr, 0, sizeof ( tmpStr ) );

        if ( ( hgIdx + 1 - lwIdx ) > MC_TMPSTR_LEN )
        {
            return MC_ERROR_STRLEN;
        }

        strncpy ( tmpStr,
                  dcatalog_p + lwIdx,
                  hgIdx + 1 - lwIdx
                );

        for ( u_int i = 0; i < g_dcatalogNum; i++ )
        {
            if ( 0 == strcmp ( g_dcatalogKV[i].enKey_p,
                               tmpStr
                             )
               )
            {
                didVal_p->dcatalog = g_dcatalogKV[i].enVal;
                break;
            }
        }
    }

    //dataIdE_p->QueryIntAttribute ( "dkind", & ( didVal_p->dkind    ) );
    const char* dkind_p = dataIdE_p->Attribute ( "dkind" );

    if ( dkind_p != NULL )
    {
        //去掉前后的空格
        trimConst ( dkind_p, lwIdx, hgIdx );
        memset ( tmpStr, 0, sizeof ( tmpStr ) );

        if ( ( hgIdx + 1 - lwIdx ) > MC_TMPSTR_LEN )
        {
            return MC_ERROR_STRLEN;
        }

        strncpy ( tmpStr,
                  dkind_p + lwIdx,
                  hgIdx + 1 - lwIdx
                );

        for ( u_int i = 0; i < g_dkindNum; i++ )
        {
            if ( 0 == strcmp (  g_dkindKV[i].enKey_p,
                                tmpStr
                             )
               )
            {
                didVal_p->dkind = g_dkindKV[i].enVal;
                break;
            }
        }
    }

    //dataIdE_p->QueryIntAttribute ( "dtype", & ( didVal_p->dtype    ) );
    const char* dtype_p = dataIdE_p->Attribute ( "dtype" );

    if ( dtype_p != NULL )
    {
        //去掉前后的空格
        trimConst ( dtype_p, lwIdx, hgIdx );
        memset ( tmpStr, 0, sizeof ( tmpStr ) );

        if ( ( hgIdx + 1 - lwIdx ) > MC_TMPSTR_LEN )
        {
            return MC_ERROR_STRLEN;
        }

        strncpy ( tmpStr,
                  dtype_p + lwIdx,
                  hgIdx + 1 - lwIdx
                );

        for ( u_int i = 0; i < g_dtypeNum; i++ )
        {
            if ( 0 == strcmp (  g_dtypeKV[i].enKey_p,
                                tmpStr
                             )
               )
            {
                didVal_p->dtype = g_dtypeKV[i].enVal;
                break;
            }
        }
    }

    //dataIdE_p->QueryIntAttribute ( "dlen", & ( didVal_p->dlen     ) );
    didVal_p->dlen = didVal_p->dtype;

    dataIdE_p->QueryIntAttribute ( "amount", & ( didVal_p->amount   ) );

    //dataIdE_p->QueryIntAttribute ( "ectype", & ( didVal_p->ectype   ) );
    const char* ectype_p = dataIdE_p->Attribute ( "ectype" );

    if ( ectype_p != NULL )
    {
        //去掉前后的空格
        trimConst ( ectype_p, lwIdx, hgIdx );
        memset ( tmpStr, 0, sizeof ( tmpStr ) );

        if ( ( hgIdx + 1 - lwIdx ) > MC_TMPSTR_LEN )
        {
            return MC_ERROR_STRLEN;
        }

        strncpy ( tmpStr,
                  ectype_p + lwIdx,
                  hgIdx + 1 - lwIdx
                );

        for ( u_int i = 0; i < g_ectypeNum; i++ )
        {
            if ( 0 == strcmp (  g_ectypeKV[i].enKey_p,
                                tmpStr
                             )
               )
            {
                didVal_p->ectype = g_ectypeKV[i].enVal;
                break;
            }
        }
    }

    dataIdE_p->QueryIntAttribute ( "ecsn", & ( didVal_p->ecsn     ) );

    //dataIdE_p->QueryIntAttribute ( "srctype", & ( didVal_p->srctype  ) );
    const char* srctype_p = dataIdE_p->Attribute ( "srctype" );

    if ( srctype_p != NULL )
    {
        //去掉前后的空格
        trimConst ( srctype_p, lwIdx, hgIdx );
        memset ( tmpStr, 0, sizeof ( tmpStr ) );

        if ( ( hgIdx + 1 - lwIdx ) > MC_TMPSTR_LEN )
        {
            return MC_ERROR_STRLEN;
        }

        strncpy ( tmpStr,
                  srctype_p + lwIdx,
                  hgIdx + 1 - lwIdx
                );

        for ( u_int i = 0; i < g_srctypeNum; i++ )
        {
            if ( 0 == strcmp (  g_srctypeKV[i].enKey_p,
                                tmpStr
                             )
               )
            {
                didVal_p->srctype = g_srctypeKV[i].enVal;
                break;
            }
        }
    }

    dataIdE_p->QueryIntAttribute ( "srcsn", & ( didVal_p->srcsn    ) );

    //dataIdE_p->QueryIntAttribute ( "htype", & ( didVal_p->htype    ) );
    const char* htype_p = dataIdE_p->Attribute ( "htype" );

    if ( htype_p != NULL )
    {
        //去掉前后的空格
        trimConst ( htype_p, lwIdx, hgIdx );
        memset ( tmpStr, 0, sizeof ( tmpStr ) );

        if ( ( hgIdx + 1 - lwIdx ) > MC_TMPSTR_LEN )
        {
            return MC_ERROR_STRLEN;
        }

        strncpy ( tmpStr,
                  htype_p + lwIdx,
                  hgIdx + 1 - lwIdx
                );

        for ( u_int i = 0; i < g_htypeNum; i++ )
        {
            if ( 0 == strcmp (  g_htypeKV[i].enKey_p,
                                tmpStr
                             )
               )
            {
                didVal_p->htype = g_htypeKV[i].enVal;
                break;
            }
        }
    }

    dataIdE_p->QueryFloatAttribute ( "hvalue", & ( didVal_p->hvalue   ) );

    //dataIdE_p->QueryIntAttribute ( "itype", & ( didVal_p->itype    ) );
    const char* itype_p = dataIdE_p->Attribute ( "itype" );

    if ( itype_p != NULL )
    {
        //去掉前后的空格
        trimConst ( itype_p, lwIdx, hgIdx );
        memset ( tmpStr, 0, sizeof ( tmpStr ) );

        if ( ( hgIdx + 1 - lwIdx ) > MC_TMPSTR_LEN )
        {
            return MC_ERROR_STRLEN;
        }

        strncpy ( tmpStr,
                  itype_p + lwIdx,
                  hgIdx + 1 - lwIdx
                );

        for ( u_int i = 0; i < g_itypeNum; i++ )
        {
            if ( 0 == strcmp (  g_itypeKV[i].enKey_p,
                                tmpStr
                             )
               )
            {
                didVal_p->itype = g_itypeKV[i].enVal;
                break;
            }
        }
    }

    dataIdE_p->QueryIntAttribute ( "ivalue", & ( didVal_p->ivalue   ) );

    return 0;
}




int generateOneDid ( char* dataId_p,
                     const T_DIDfaceVal* didVal_p,
                     DID* didObj_p
                   )
{
    if ( didVal_p == NULL
         || didObj_p == NULL
         /*|| dataId_p == NULL*/
       )
    {
        return MC_ERROR_INPAR;
    }

    //int tRet = 0;

    //DATA_ID* tDataId_p = NULL;

    //1:调用MakeMeteorSingle；2:调用MakePeriodSingle;3:调用MakeDate
    if ( didVal_p->funcType == 1 )
    {
        //20180604094528@modify MakeMeteorSingle to MakeNormal
        didObj_p->MakeNormal (
        didVal_p->dcatalog,
        didVal_p->dkind,
        didVal_p->dtype,
        didObj_p->TypeSize ( didVal_p->dtype ),
        didVal_p->amount,
        didVal_p->ectype,
        didVal_p->ecsn,
        didVal_p->srctype,
        didVal_p->srcsn,
        didVal_p->itype,
        didVal_p->ivalue,
        didVal_p->htype,
        didVal_p->hvalue
        );
        /*tDataId_p = ( DATA_ID* ) didObj.MakeMeteorSingle (
            didVal_p->dcatalog,
            didVal_p->dkind,
            didVal_p->dtype,
            didObj.TypeSize ( didVal_p->dtype ),
            didVal_p->amount,
            didVal_p->ectype,
            didVal_p->ecsn,
            didVal_p->srctype,
            didVal_p->srcsn,
            didVal_p->htype,
            didVal_p->hvalue,
            didVal_p->itype,
            didVal_p->ivalue
            );*/


    }
    else if ( didVal_p->funcType == 2 )
    {
        //20180604094248@modify MakePeriodSingle to MakeNormal
        didObj_p->MakeNormal (
        didVal_p->dcatalog,
        didVal_p->dkind,
        didVal_p->dtype,
        didObj_p->TypeSize ( didVal_p->dtype ),
        didVal_p->amount,
        didVal_p->ectype,
        didVal_p->ecsn,
        didVal_p->srctype,
        didVal_p->srcsn,
        didVal_p->itype,
        didVal_p->ivalue
        );
        //tDataId_p = ( DATA_ID* ) didObj.MakePeriodSingle (
        //            didVal_p->dcatalog,
        //            didVal_p->dkind,
        //            didVal_p->dtype,
        //            didObj.TypeSize ( didVal_p->dtype ),
        //            didVal_p->amount,
        //            didVal_p->ectype,
        //            didVal_p->ecsn,
        //            didVal_p->srctype,
        //            didVal_p->srcsn,
        //            /*didVal_p->htype,
        //            didVal_p->hvalue,*/
        //            didVal_p->itype,
        //            didVal_p->ivalue
        //            );
    }
    else if ( didVal_p->funcType == 3 )
    {
        /*tDataId_p = ( DATA_ID* ) didObj.MakeDate (
                    didVal_p->dcatalog,
                    didVal_p->dkind,
                    didVal_p->amount
                    );*/
        didObj_p->MakeNormal (
        didVal_p->dcatalog,
        didVal_p->dkind,
        DATATYPE_DATE7B,
        didObj_p->TypeSize ( DATATYPE_DATE7B ),
        didVal_p->amount
        );
    }

    if ( dataId_p != NULL )
    {
        didObj_p->Export ( dataId_p );

        //memcpy ( &didObj_p, tDataId_p, sizeof ( DATA_ID ) );
    }

    didObj_p->Clear();

    return 0;
}


//to be data id from xml documents
//int toBeDaIdFxml ( const char* xmlFilePath_p,
//                   DID* didObjArr_p,
//                   const int& maxNum,
//                   int& curNum
//                 )
//{
//    if ( xmlFilePath_p == NULL
//         || didObjArr_p == NULL
//         || maxNum == 0
//       )
//    {
//        return MC_ERROR_INPAR;
//    }
//
//    XMLDocument doc;
//    //DID didObj;
//    int tRet = 0;
//
//    tRet = doc.LoadFile ( xmlFilePath_p );
//
//    if ( tRet != 0 )
//    {
//        return -2;
//    }
//
//    const XMLElement* rootE_p = doc.FirstChildElement ( "root" );
//
//    if ( rootE_p == NULL )
//    {
//        return MC_ERROR_XMLELE;
//    }
//
//    int idNum = 0;
//    rootE_p->QueryIntAttribute ( "idNum", &idNum );
//
//    if ( idNum > maxNum )
//    {
//        return -4;
//    }
//
//    const XMLElement* dataIdE_p = rootE_p->FirstChildElement ( "dataId" );
//
//    if ( dataIdE_p == NULL )
//    {
//        return MC_ERROR_XMLELE;
//    }
//
//    curNum = 0;
//
//    T_DIDfaceVal didValS;
//    //取一个dataId的属性值
//    getOneDidValS ( &didValS, dataIdE_p );
//    generateOneDid ( & ( didObjArr_p[curNum] ), &didValS/*, didObj*/ );
//    curNum++;
//
//    while ( ( dataIdE_p = dataIdE_p->NextSiblingElement ( "dataId" )
//            ) != NULL
//          )
//    {
//        //取一个dataId的属性值
//        getOneDidValS ( &didValS, dataIdE_p );
//        generateOneDid ( & ( didObjArr_p[curNum] ), &didValS/*, didObj*/ );
//        curNum++;
//
//        if ( curNum >= maxNum )
//        {
//            break;
//        }
//    }
//
//
//    return 0;
//}

/******************************************************************************/


//释放T_phyValCfg结构的指针成员
void freeOnePhyValCfgM ( T_phyValCfg* PhyValCfg_p )
{
    if ( PhyValCfg_p == NULL )
    {
        return;
    }

    if ( PhyValCfg_p->pntAddr_p != NULL )
    {
        free ( PhyValCfg_p->pntAddr_p );
        PhyValCfg_p->pntAddr_p = NULL;
    }

    if ( PhyValCfg_p->dataId_p != NULL )
    {
        free ( PhyValCfg_p->dataId_p );
        PhyValCfg_p->dataId_p = NULL;
    }

    return;
}


//释放initChnUnit_p的指针成员(如成员下面还有指针成员也进行释放)
void freeOneInitChnUnitM ( T_initChnUnit* initChnUnit_p )
{
    if ( initChnUnit_p == NULL )
    {
        return;
    }

    if ( initChnUnit_p->pntAddrCfg.contPntAdd_p != NULL )
    {
        free ( initChnUnit_p->pntAddrCfg.contPntAdd_p );
        initChnUnit_p->pntAddrCfg.contPntAdd_p = NULL;
    }

    u_int i = 0;

    while ( i < initChnUnit_p->phyUnitNum
            && initChnUnit_p->phyObjVal_p != NULL
          )
    {
        T_phyValCfg* tPhyValCfg_p = & ( initChnUnit_p->phyObjVal_p[i] );
        freeOnePhyValCfgM ( tPhyValCfg_p );
        i++;
    }

    if ( initChnUnit_p->phyObjVal_p != NULL )

    {
        free ( initChnUnit_p->phyObjVal_p );
        initChnUnit_p->phyObjVal_p = NULL;
    }

    i = 0;

    while ( i < initChnUnit_p->numPhyCat
            && initChnUnit_p->phyCat_p != NULL
          )
    {
        T_phyCat* tPhyCat_p = & ( initChnUnit_p->phyCat_p[i] );

        if ( tPhyCat_p->head_p != NULL )
        {
            free ( tPhyCat_p->head_p );
            tPhyCat_p->head_p = NULL;
        }

        i++;
    }

    if ( initChnUnit_p->phyCat_p != NULL )
    {
        free ( initChnUnit_p->phyCat_p );
        initChnUnit_p->phyCat_p = NULL;
    }

    //untChnTmStmp
    if ( initChnUnit_p->untChnTmStmp.dataId_p != NULL )
    {
        free ( initChnUnit_p->untChnTmStmp.dataId_p );
        initChnUnit_p->untChnTmStmp.dataId_p = NULL;
    }

    if ( initChnUnit_p->untChnTmStmp.tmStmp_p != NULL )
    {
        free ( initChnUnit_p->untChnTmStmp.tmStmp_p );
        initChnUnit_p->untChnTmStmp.tmStmp_p = NULL;
    }

    return;
}



void freeOneChnlObjCfg ( chnlObjCfg* freData_p )
{
    if ( freData_p == NULL )
    {
        return;
    }

    if ( freData_p->cfgAddr_p != NULL )
    {
        free ( freData_p->cfgAddr_p );
        freData_p->cfgAddr_p = NULL;
    }

    if ( freData_p->phyType_p != NULL )
    {
        free ( freData_p->phyType_p );
        freData_p->phyType_p = NULL;
    }

    if ( freData_p->ssnRInfo.sRCnt_p != NULL )
    {
        free ( freData_p->ssnRInfo.sRCnt_p );
        freData_p->ssnRInfo.sRCnt_p = NULL;
    }

    return;
}





//释放T_initChnUnit[]
void freeNChnUnit ( const u_int& num,
                    T_initChnUnit** initChnUnit_pp
                  )
{
    if ( initChnUnit_pp == NULL
         || num == 0
       )
    {
        return;
    }

    T_initChnUnit* tArry_p = *initChnUnit_pp;

    if ( tArry_p == NULL )
    {
        return;
    }

    for ( u_int i = 0; i < num; i++ )
    {
        T_initChnUnit* tUnit_p = & ( tArry_p[i] );
        freeOneInitChnUnitM ( tUnit_p );
    }

    free ( tArry_p );
    *initChnUnit_pp = NULL;

    return;
}


//释放chnlObjCfg[]
void freeNChnlObjCfg ( const u_int& num,
                       chnlObjCfg** freData_pp
                     )
{
    if ( freData_pp == NULL
         || num == 0
       )
    {
        return;
    }

    chnlObjCfg* tArry_p = *freData_pp;

    if ( tArry_p == NULL )
    {
        return;
    }

    for ( u_int i = 0; i < num; i++ )
    {
        chnlObjCfg* tChnOjb_p = & ( tArry_p[i] );
        freeOneChnlObjCfg ( tChnOjb_p );
    }

    free ( tArry_p );
    *freData_pp = NULL;

    return;
}




//从一个通道取一个物理量的配置
int getOnePhyCfg ( const XMLElement* phyObjValE_p,
                   DID* didObj_p,
                   T_phyValCfg* PhyValCfg_p,
                   const int& didLng
                 )
{
    if ( phyObjValE_p == NULL
         || PhyValCfg_p == NULL
         || didObj_p == NULL
       )
    {
        return MC_ERROR_INPAR;
    }

    //int tRet = 0;
    /*T_phyValCfg* tPhyValCfg_p = ( T_phyValCfg* ) calloc (
                                1,
                                sizeof ( T_phyValCfg )
                                );*/
    T_phyValCfg* tPhyValCfg_p = PhyValCfg_p;

    phyObjValE_p->QueryIntAttribute ( "phyType",
                                      ( int* ) & ( tPhyValCfg_p->phyType ) );
    /*phyObjValE_p->QueryIntAttribute ( "calcMethd",
                                      ( int* ) & ( tPhyValCfg_p->calcMethd ) );*/
    const char* calcMethd_p = phyObjValE_p->Attribute ( "calcMethd" );

    if ( calcMethd_p != NULL )
    {
        int lwIdx = 0;
        int hgIdx = 0;
        char tmpStr[MC_TMPSTR_LEN];
        //去掉前后的空格
        trimConst ( calcMethd_p, lwIdx, hgIdx );
        memset ( tmpStr, 0, sizeof ( tmpStr ) );

        if ( ( hgIdx + 1 - lwIdx ) > MC_TMPSTR_LEN )
        {
            return MC_ERROR_STRLEN;
        }

        strncpy ( tmpStr,
                  calcMethd_p + lwIdx,
                  hgIdx + 1 - lwIdx
                );

        for ( u_int i = 0; i < g_phyCalcNum; i++ )
        {
            if ( 0 == strcmp (  g_phyCalcKV[i].enKey_p,
                                tmpStr
                             )
               )
            {
                tPhyValCfg_p->calcMethd = ( PHY_CALC_METHD ) g_phyCalcKV[i].enVal;
                break;
            }
        }
    }

    phyObjValE_p->QueryFloatAttribute ( "multFactor",
                                        & ( tPhyValCfg_p->multFactor ) );
    int phyValLng = 0;
    phyObjValE_p->QueryIntAttribute ( "phyValLng", & phyValLng );
    tPhyValCfg_p->phyValLng = phyValLng;

    const XMLElement* pntAddrsE_p = phyObjValE_p->FirstChildElement (
                                    "pntAddrs" );

    if ( pntAddrsE_p != NULL )
    {
        int numOfAdd = 0;
        //pntAddrsE_p->QueryIntAttribute ( "numOfAdd", & numOfAdd );
        numOfAdd = getXmlChildEleNum ( pntAddrsE_p, "pntAddr" );
        tPhyValCfg_p->numOfAdd = numOfAdd;

        if ( numOfAdd > 0 )
        {
            tPhyValCfg_p->pntAddr_p = ( PNT_ADDR_TYPE* ) calloc (
                                      numOfAdd,
                                      sizeof ( PNT_ADDR_TYPE )
                                      );

            if ( tPhyValCfg_p->pntAddr_p == NULL )
            {
                freeOnePhyValCfgM ( tPhyValCfg_p );
                return MC_ERROR_CALLOC;
            }

            const XMLElement* pntAddrE_p = pntAddrsE_p->FirstChildElement (
                                           "pntAddr" );

            if ( pntAddrE_p == NULL )
            {
                freeOnePhyValCfgM ( tPhyValCfg_p );
                return -2;
            }

            int pntAddIdx = 0;
            pntAddrE_p->QueryIntText ( ( int* ) & (
                                       tPhyValCfg_p->pntAddr_p[pntAddIdx]
                                       )
                                     );
            pntAddIdx++;

            while ( ( pntAddrE_p = pntAddrE_p->NextSiblingElement (
                                   "pntAddr" ) ) != NULL
                    && pntAddIdx < numOfAdd
                  )
            {
                pntAddrE_p->QueryIntText ( ( int* ) & (
                                           tPhyValCfg_p->pntAddr_p[pntAddIdx]
                                           )
                                         );
                pntAddIdx++;
            }

        }
        else
        {
            tPhyValCfg_p->pntAddr_p = NULL;
        }
    }
    else
    {
        tPhyValCfg_p->numOfAdd = 0;
        tPhyValCfg_p->pntAddr_p = NULL;
    }


    const XMLElement* dataIdE_p = phyObjValE_p->FirstChildElement (
                                  "dataId" );

    if ( dataIdE_p != NULL && didLng > 0  )
    {
        tPhyValCfg_p->dataId_p = calloc ( 1, didLng );

        if ( tPhyValCfg_p->dataId_p == NULL )
        {
            freeOnePhyValCfgM ( tPhyValCfg_p );
            return MC_ERROR_CALLOC;
        }

        T_DIDfaceVal didValS;

        //取一个dataId的属性值
        getOneDidValS ( &didValS, dataIdE_p );
        //调用接口生成dataId
        generateOneDid ( ( char* ) tPhyValCfg_p->dataId_p,
                         &didValS,
                         didObj_p
                       );
    }
    else
    {
        tPhyValCfg_p->dataId_p = NULL;
    }

    return 0;
}


/*取物理量的特定累计类别phyCatName的个数输出到outNum
如果phyType_p不为空则把匹配的物理量phyType配置值赋于其相应地址
*/
int getPhyCatNameNum ( const XMLElement* channelE_p,
                       const char* idAttName_p,
                       const int& phyCatName,
                       u_int* outNum,
                       PHY_UNIT_TYPE* phyType_p
                     )
{
    if ( channelE_p == NULL
         || idAttName_p == NULL
       )
    {
        return MC_ERROR_INPAR;
    }

    u_int tOutNum = 0;
    const XMLElement* phyObjValE_p = channelE_p->FirstChildElement (
                                     "phyObjVal" );

    if ( phyObjValE_p != NULL )
    {
        const XMLElement* dataIdE_p = phyObjValE_p->FirstChildElement (
                                      "dataId" );

        if ( dataIdE_p != NULL )
        {
            int tIntVal = 0;

            dataIdE_p->QueryIntAttribute ( idAttName_p, &tIntVal );

            if ( tIntVal == phyCatName )
            {
                if ( phyType_p != NULL )
                {
                    int tPhyType = 0;
                    phyObjValE_p->QueryIntAttribute ( "phyType", &tPhyType );
                    phyType_p[tOutNum] = ( PHY_UNIT_TYPE ) tPhyType;
                }

                tOutNum++;
            }
        }

        while ( ( phyObjValE_p = phyObjValE_p->NextSiblingElement (
                                 "phyObjVal" ) ) != NULL
              )
        {
            dataIdE_p = phyObjValE_p->FirstChildElement (
                        "dataId" );

            if ( dataIdE_p != NULL )
            {
                int tIntVal = 0;

                dataIdE_p->QueryIntAttribute ( idAttName_p, &tIntVal );

                if ( tIntVal == phyCatName )
                {
                    if ( phyType_p != NULL )
                    {
                        int tPhyType = 0;
                        phyObjValE_p->QueryIntAttribute ( "phyType", &tPhyType );
                        phyType_p[tOutNum] = ( PHY_UNIT_TYPE ) tPhyType;
                    }

                    tOutNum++;
                }
            }
        }
    }

    if ( outNum != NULL )
    {
        *outNum = tOutNum;
    }

    return 0;
}


//校验一个通道中物理量类型是否有重复的
int chkDuplicatePhyType ( const XMLElement* channelE_p,
                          u_int& numI,
                          u_int& numJ
                        )
{

    if ( channelE_p == NULL )
    {
        return MC_ERROR_INPAR;
    }

    u_int numPhy = getXmlChildEleNum ( channelE_p, "phyObjVal" );
    PHY_UNIT_TYPE* tPhyTypeAll_p = NULL;

    if ( numPhy > 0 )
    {
        tPhyTypeAll_p = ( PHY_UNIT_TYPE* ) calloc (
                        numPhy,
                        sizeof ( PHY_UNIT_TYPE ) );

        if ( tPhyTypeAll_p == NULL )
        {
            return MC_ERROR_CALLOC;
        }

        u_int tRet = getXmlChldEleAttr ( channelE_p,
                                         "phyObjVal",
                                         "phyType",
                                         tPhyTypeAll_p
                                       );

        //对tPhyTypeAll_p进行重复判断
        for ( u_int i = 0; i < numPhy - 1; i++ )
        {
            for ( u_int j = i + 1; j < numPhy; j++ )
            {
                if ( tPhyTypeAll_p[i] == tPhyTypeAll_p[j] )
                {
                    numI = i;
                    numJ = j;

                    if ( tPhyTypeAll_p != NULL )
                    {
                        free ( tPhyTypeAll_p );
                        tPhyTypeAll_p = NULL;
                    }

                    return 1;
                }
            }
        }
    }

    if ( tPhyTypeAll_p != NULL )
    {
        free ( tPhyTypeAll_p );
        tPhyTypeAll_p = NULL;
    }

    return 0;
}

//从一个通道取所有物理量的配置
int getOneChnCfgFrmXml ( const XMLElement* channelE_p,
                         T_initChnUnit* initChnUnit_p,
                         chnlObjCfg* otherChnlCfg_p
                       )
{
    if ( channelE_p == NULL )
    {
        return MC_ERROR_INPAR;
    }

    int tRet = 0;


    u_int numI = 0;
    u_int numJ = 0;

    //判断xml中是否有重复的phyType值
    tRet = chkDuplicatePhyType ( channelE_p, numI, numJ );

    if ( tRet < 0 ) //错误
    {
        return tRet;
    }
    else if ( tRet > 0 ) //有重复的值
    {
        return MC_ERROR_XMLPHY_DUP;
    }

    /*T_initChnUnit* initChnUnit_p = ( T_initChnUnit* ) calloc (
                                   1,
                                   sizeof ( T_initChnUnit )
                                   );*/
    int chnNum     = 0; //通道号
    int pntObjLng  = 0; //点地址关联通用数据对象的长度
    int dataIdLng  = 0; //数据id长度
    int tmStmpLng  = 0; //时标长度
    int pntAddrNum = 0; //通道中包括的点地址总数
    int phyUnitNum = 0; //通道中包括的物理量总数
    int maxPhyType = 0; //通道中最大的物理量类型值

    DID didObj;

    channelE_p->QueryIntAttribute ( "chnNum", &chnNum );
    channelE_p->QueryIntAttribute ( "pntObjLng", &pntObjLng );
    //channelE_p->QueryIntAttribute ( "dataIdLng", &dataIdLng );
    dataIdLng = didObj.IdSize();
    channelE_p->QueryIntAttribute ( "tmStmpLng", &tmStmpLng );
    channelE_p->QueryIntAttribute ( "pntAddrNum", &pntAddrNum );
    //channelE_p->QueryIntAttribute ( "phyUnitNum", &phyUnitNum );
    channelE_p->QueryIntAttribute ( "maxPhyType", &maxPhyType );
    phyUnitNum = getXmlChildEleNum ( channelE_p, "phyObjVal" );

    if ( dataIdLng < 0 )
    {
        dataIdLng = 0;
    }

    /**********通道时标特殊配置**********/
    const XMLElement* chnTmStmpE_p = channelE_p->FirstChildElement (
                                     "chnTmStmp" );

    if ( chnTmStmpE_p != NULL )
    {
        int tchnTmStmpLng = 0 ;
        chnTmStmpE_p->QueryIntAttribute ( "lng", &tchnTmStmpLng );
        initChnUnit_p->untChnTmStmp.tmStmpLng = tchnTmStmpLng;
        const XMLElement* dataIdE_p =  chnTmStmpE_p->FirstChildElement (
                                       "dataId" );

        if ( dataIdE_p != NULL && dataIdLng > 0 )
        {
            T_DIDfaceVal didValS;
            initChnUnit_p->untChnTmStmp.dataId_p = calloc ( 1,
                                                   dataIdLng
                                                          );

            if ( initChnUnit_p->untChnTmStmp.dataId_p == NULL )
            {
                freeOneInitChnUnitM ( initChnUnit_p );
                freeOneChnlObjCfg ( otherChnlCfg_p );
                return MC_ERROR_CALLOC;
            }

            //取一个dataId的属性值
            getOneDidValS ( &didValS, dataIdE_p );
            //调用接口生成dataId
            generateOneDid ( ( char* ) initChnUnit_p->untChnTmStmp.dataId_p,
                             &didValS,
                             &didObj
                           );
        }
    }

    /***********otherChnProp通道其他属性配置 begine**********/
    const XMLElement* otherChnPropE_p = channelE_p->FirstChildElement (
                                        "otherChnProp" );

    if ( otherChnPropE_p == NULL )
    {
        /*freeOneInitChnUnitM ( initChnUnit_p );
        freeOneChnlObjCfg ( otherChnlCfg_p );*/
        return MC_ERROR_XMLELE;
    }

    int stage            =  0; //是否需要激活。=0：需要激活，=1：不需要激活
    int putStagFlag      =  0; //是否入库
    int role             =  0; // 本站角色
    int stnRole          =  0; // 远程站角色
    //char szStnIp[16]      ;   //远程站IP地址
    //char localszStnIp[16] ;   //需要绑定的本地IP地址
    int szStnPort        =  0; //远程站端口号
    int localPort        =  0; //需要绑定胡本地IP地址
    int ptclMdlNum       =  0; //协议号
    int bufLng           =  0; //数据缓冲区的最大长度
    int cfgLen           =  0; //配置的点表个数,cfgAddr_p数组的长度
    int usrType          =  0; // 数据类型，0--元素集，1--文件

    /*memset ( szStnIp, 0, sizeof ( szStnIp ) );
    memset ( localszStnIp, 0, sizeof ( localszStnIp ) );*/

    const XMLElement* stageE_p = otherChnPropE_p->FirstChildElement (
                                 "stage" );

    if ( stageE_p != NULL )
    {
        stageE_p->QueryIntText ( &stage );
    }

    const XMLElement* putStagFlagE_p = otherChnPropE_p->FirstChildElement (
                                       "putStagFlag" );

    if ( putStagFlagE_p != NULL )
    {
        putStagFlagE_p->QueryIntText ( &putStagFlag );
    }

    const XMLElement* roleOthE_p = otherChnPropE_p->FirstChildElement (
                                   "role" );

    if ( roleOthE_p != NULL )
    {
        roleOthE_p->QueryIntText ( &role );
    }

    const XMLElement* stnRoleE_p = otherChnPropE_p->FirstChildElement (
                                   "stnRole" );

    if ( stnRoleE_p != NULL )
    {
        stnRoleE_p->QueryIntText ( &stnRole );
    }

    const XMLElement* ptclMdlNumE_p = otherChnPropE_p->FirstChildElement (
                                      "ptclMdlNum" );

    if ( ptclMdlNumE_p != NULL )
    {
        ptclMdlNumE_p->QueryIntText ( &ptclMdlNum );
    }

    const XMLElement* bufLngE_p = otherChnPropE_p->FirstChildElement (
                                  "bufLng" );

    if ( bufLngE_p != NULL )
    {
        bufLngE_p->QueryIntText ( &bufLng );
    }

    const XMLElement* usrTypeE_p = otherChnPropE_p->FirstChildElement (
                                   "usrType" );

    if ( usrTypeE_p != NULL )
    {
        usrTypeE_p->QueryIntText ( &usrType );
    }

    const XMLElement* remoteIPE_p = otherChnPropE_p->FirstChildElement (
                                    "remoteIP" );

    if ( remoteIPE_p != NULL )
    {
        const char* rIP_p = remoteIPE_p->Attribute ( "ip" );
        const char* lIP_p = remoteIPE_p->Attribute ( "localip" );

        remoteIPE_p->QueryIntAttribute ( "port", &szStnPort );
        remoteIPE_p->QueryIntAttribute ( "localport", &localPort );

        if ( rIP_p != NULL )
        {
            memcpy ( otherChnlCfg_p->szStnIp,
                     rIP_p,
                     strlen ( rIP_p )
                   );
        }

        if ( lIP_p != NULL )
        {
            memcpy ( otherChnlCfg_p->localszStnIp,
                     lIP_p,
                     strlen ( lIP_p )
                   );
        }

        otherChnlCfg_p->szStnPort = szStnPort;
        otherChnlCfg_p->localPort = localPort;
    }

    otherChnlCfg_p->stage       = stage;
    otherChnlCfg_p->putStagFlag = putStagFlag;
    otherChnlCfg_p->role        = role;
    otherChnlCfg_p->stnRole     = stnRole;
    otherChnlCfg_p->ptclMdlNum  = ptclMdlNum;
    otherChnlCfg_p->bufLng      = bufLng;
    otherChnlCfg_p->usrType     = usrType;
    int tIntVal = 0;
    const XMLElement* APDU_DSCRE_p = otherChnPropE_p->FirstChildElement (
                                     "APDU_DSCR" );

    if ( APDU_DSCRE_p != NULL )
    {
        APDU_DSCRE_p->QueryIntAttribute ( "ASDU_TID", &tIntVal );
        otherChnlCfg_p->tmpApduDscr.ASDU_TID = tIntVal;
        APDU_DSCRE_p->QueryIntAttribute ( "InfoEleLng", &tIntVal );
        otherChnlCfg_p->tmpApduDscr.InfoEleLng = tIntVal;

        int tAsduCmtbNum = 0;
        APDU_DSCRE_p->QueryIntAttribute ( "asduCmtbNum", &tAsduCmtbNum );
        const XMLElement* ASDU_CMTBLE_p = APDU_DSCRE_p->FirstChildElement (
                                          "ASDU_CMTBL" );
        int tAsduCmIdx = 0;

        if ( ASDU_CMTBLE_p != NULL  && tAsduCmtbNum > 0 )
        {
            ASDU_CMTBLE_p->QueryIntText ( &tIntVal );
            otherChnlCfg_p->tmpApduDscr.ASDU_CMTBL[tAsduCmIdx] = tIntVal;
            tAsduCmIdx++;

            while ( ( ASDU_CMTBLE_p = ASDU_CMTBLE_p->NextSiblingElement (
                                      "ASDU_CMTBL" ) ) != NULL
                    && tAsduCmIdx < tAsduCmtbNum
                  )
            {
                ASDU_CMTBLE_p->QueryIntText ( &tIntVal );
                otherChnlCfg_p->tmpApduDscr.ASDU_CMTBL[tAsduCmIdx] = tIntVal;
                tAsduCmIdx++;
            }
        }

    }

    const XMLElement* ssnReqInfoE_p = otherChnPropE_p->FirstChildElement (
                                      "ssnReqInfo" );

    if ( ssnReqInfoE_p != NULL )
    {
        const XMLElement* reqInfoE_p = ssnReqInfoE_p->FirstChildElement (
                                       "reqInfo" );

        if ( reqInfoE_p != NULL )
        {
            tIntVal = 0;
            const XMLElement* rIfRoleE_p = reqInfoE_p->FirstChildElement (
                                           "role" );

            if ( rIfRoleE_p != NULL )
            {
                rIfRoleE_p->QueryIntText ( &tIntVal );
            }

            otherChnlCfg_p->ssnRInfo.tReqInfo.role = tIntVal;

            tIntVal = 0;
            const XMLElement* rIfChnlIDE_p = reqInfoE_p->FirstChildElement (
                                             "chnlID" );

            if ( rIfChnlIDE_p != NULL )
            {
                rIfChnlIDE_p->QueryIntText ( &tIntVal );
            }

            otherChnlCfg_p->ssnRInfo.tReqInfo.chnlID = tIntVal;

            tIntVal = 0;
            const XMLElement* rIfSmrtnE_p = reqInfoE_p->FirstChildElement (
                                            "snnMaxRetryNum" );

            if ( rIfSmrtnE_p != NULL )
            {
                rIfSmrtnE_p->QueryIntText ( &tIntVal );
            }

            otherChnlCfg_p->ssnRInfo.tReqInfo.snnMaxRetryNum = tIntVal;
            /*此值暂不在配置中赋值
            otherChnlCfg_p->ssnRInfo.tReqInfo.infoObjchkStd_p
            */
        }//end reqInfoE_p != NULL

        const XMLElement* ssnReqCntSE_p = ssnReqInfoE_p->FirstChildElement (
                                          "ssnReqCntS" );

        if ( ssnReqCntSE_p != NULL )
        {
            tIntVal = 0;
            int tCntNum = 0;
            //ssnReqCntSE_p->QueryIntAttribute ( "cntNum", &tCntNum );
            tCntNum = getXmlChildEleNum ( ssnReqCntSE_p, "ssnReqCnt" );
            otherChnlCfg_p->ssnRInfo.cntNum = tCntNum;

            const XMLElement* ssnReqCntE_p = ssnReqCntSE_p->FirstChildElement (
                                             "ssnReqCnt" );
            int srcntIdx = 0;

            if ( ssnReqCntE_p != NULL && tCntNum > 0 )
            {
                otherChnlCfg_p->ssnRInfo.sRCnt_p = ( ssnReqCnt* ) calloc (
                                                   tCntNum,
                                                   sizeof ( ssnReqCnt )
                                                   );

                if ( otherChnlCfg_p->ssnRInfo.sRCnt_p == NULL )
                {
                    freeOneInitChnUnitM ( initChnUnit_p );
                    freeOneChnlObjCfg ( otherChnlCfg_p );
                    return MC_ERROR_CALLOC;
                }

                tIntVal = 0;
                ssnReqCnt* tSsnReqCnt_p = & (
                                          otherChnlCfg_p->ssnRInfo.sRCnt_p[srcntIdx] );
                srcntIdx++;
                ssnReqCntE_p->QueryIntAttribute ( "collMethods", &tIntVal );

                if ( tIntVal > 0 )
                {
                    tSsnReqCnt_p->collMethods = true;
                }
                else
                {
                    tSsnReqCnt_p->collMethods = false;
                }

                ssnReqCntE_p->QueryIntAttribute ( "cycTm", &tIntVal );
                tSsnReqCnt_p->cycTm = tIntVal;

                ssnReqCntE_p->QueryIntAttribute ( "addrStart", &tIntVal );
                tSsnReqCnt_p->addrStart = tIntVal;
                ssnReqCntE_p->QueryIntAttribute ( "accumValue", &tIntVal );
                tSsnReqCnt_p->accumValue = tIntVal;
                const XMLElement* ptclSnnKeyE_p = ssnReqCntE_p->FirstChildElement (
                                                  "ptclSnnKey" );

                if ( ptclSnnKeyE_p != NULL )
                {
                    tIntVal = 0;
                    ptclSnnKeyE_p->QueryIntAttribute ( "ptclNo", &tIntVal );
                    tSsnReqCnt_p->ssn.ptclNo = tIntVal;
                    tIntVal = 0;
                    ptclSnnKeyE_p->QueryIntAttribute ( "snnDscNo", &tIntVal );
                    tSsnReqCnt_p->ssn.snnDscNo = tIntVal;
                }

                while ( ( ssnReqCntE_p = ssnReqCntE_p->NextSiblingElement (
                                         "ssnReqCnt" ) ) != NULL
                        && srcntIdx < tCntNum
                      )
                {
                    tIntVal = 0;
                    tSsnReqCnt_p = & (
                                   otherChnlCfg_p->ssnRInfo.sRCnt_p[srcntIdx] );
                    srcntIdx++;
                    ssnReqCntE_p->QueryIntAttribute ( "collMethods", &tIntVal );

                    if ( tIntVal > 0 )
                    {
                        tSsnReqCnt_p->collMethods = true;
                    }
                    else
                    {
                        tSsnReqCnt_p->collMethods = false;
                    }

                    ssnReqCntE_p->QueryIntAttribute ( "cycTm", &tIntVal );
                    tSsnReqCnt_p->cycTm = tIntVal;

                    ssnReqCntE_p->QueryIntAttribute ( "addrStart", &tIntVal );
                    tSsnReqCnt_p->addrStart = tIntVal;
                    ssnReqCntE_p->QueryIntAttribute ( "accumValue", &tIntVal );
                    tSsnReqCnt_p->accumValue = tIntVal;
                    ptclSnnKeyE_p = ssnReqCntE_p->FirstChildElement (
                                    "ptclSnnKey" );

                    if ( ptclSnnKeyE_p != NULL )
                    {
                        tIntVal = 0;
                        ptclSnnKeyE_p->QueryIntAttribute ( "ptclNo", &tIntVal );
                        tSsnReqCnt_p->ssn.ptclNo = tIntVal;
                        tIntVal = 0;
                        ptclSnnKeyE_p->QueryIntAttribute ( "snnDscNo", &tIntVal );
                        tSsnReqCnt_p->ssn.snnDscNo = tIntVal;
                    }
                }
            }
        }

    }

    /*以下值在此逻辑块后面赋值
      otherChnlCfg_p->cfgLen
      otherChnlCfg_p->cfgAddr_p
      otherChnlCfg_p->phyTypeNum
      otherChnlCfg_p->phyType_p
    */
    /***********otherChnProp通道其他属性配置 end**********/


    //phyQuaCfg <!--物理量类别配置-->
    const XMLElement* phyQuaCfgE_p = channelE_p->FirstChildElement (
                                     "phyQuaCfg" );


    if ( phyQuaCfgE_p != NULL )
    {
        u_int categoryNum = getXmlChildEleNum ( phyQuaCfgE_p, "phyCat" );

        initChnUnit_p->numPhyCat = categoryNum;
        initChnUnit_p->phyCat_p = ( T_phyCat* ) calloc (
                                  categoryNum,
                                  sizeof ( T_phyCat )
                                  );

        if ( initChnUnit_p->phyCat_p == NULL )
        {
            freeOneInitChnUnitM ( initChnUnit_p );
            freeOneChnlObjCfg ( otherChnlCfg_p );
            return MC_ERROR_CALLOC;
        }

        u_int i = 0;
        const XMLElement* phyCatE_p = phyQuaCfgE_p->FirstChildElement (
                                      "phyCat" );

        if ( phyCatE_p != NULL )
        {
            tIntVal = 0;
            phyCatE_p->QueryIntAttribute ( "name", &tIntVal );
            initChnUnit_p->phyCat_p[i].categoryName = tIntVal;
            //phyCatE_p->QueryIntAttribute ( "num", &tIntVal );
            u_int tOutNum = 0;
            //取categoryName数量
            tRet = getPhyCatNameNum ( channelE_p,
                                      "ivalue",
                                      initChnUnit_p->phyCat_p[i].categoryName,
                                      &tOutNum,
                                      NULL
                                    );
            initChnUnit_p->phyCat_p[i].curNum = tOutNum;

            if ( tOutNum > 0 )
            {
                initChnUnit_p->phyCat_p[i].head_p = ( PHY_UNIT_TYPE* ) calloc ( tOutNum,
                                                    sizeof ( PHY_UNIT_TYPE ) );

                if ( initChnUnit_p->phyCat_p[i].head_p == NULL )
                {
                    freeOneInitChnUnitM ( initChnUnit_p );
                    freeOneChnlObjCfg ( otherChnlCfg_p );
                    return MC_ERROR_CALLOC;
                }

                if ( initChnUnit_p->phyCat_p[i].head_p != NULL )
                {
                    //取categoryName的值对应的phyType到head_p
                    tRet = getPhyCatNameNum ( channelE_p,
                                              "ivalue",
                                              initChnUnit_p->phyCat_p[i].categoryName,
                                              NULL,
                                              initChnUnit_p->phyCat_p[i].head_p
                                            );
                }
            }

            i++;

            while ( ( phyCatE_p = phyCatE_p->NextSiblingElement ( "phyCat" )
                    ) != NULL
                    && i < categoryNum
                  )
            {
                tIntVal = 0;
                phyCatE_p->QueryIntAttribute ( "name", &tIntVal );
                initChnUnit_p->phyCat_p[i].categoryName = tIntVal;
                //phyCatE_p->QueryIntAttribute ( "num", &tIntVal );
                tOutNum = 0;
                //取categoryName数量
                tRet = getPhyCatNameNum ( channelE_p,
                                          "ivalue",
                                          initChnUnit_p->phyCat_p[i].categoryName,
                                          &tOutNum,
                                          NULL
                                        );
                initChnUnit_p->phyCat_p[i].curNum = tOutNum;

                if ( tOutNum > 0 )
                {
                    initChnUnit_p->phyCat_p[i].head_p = ( PHY_UNIT_TYPE* ) calloc ( tOutNum,
                                                        sizeof ( PHY_UNIT_TYPE ) );

                    if ( initChnUnit_p->phyCat_p[i].head_p == NULL )
                    {
                        freeOneInitChnUnitM ( initChnUnit_p );
                        freeOneChnlObjCfg ( otherChnlCfg_p );
                        return MC_ERROR_CALLOC;
                    }

                    if ( initChnUnit_p->phyCat_p[i].head_p != NULL )
                    {
                        //取categoryName的值对应的phyType到head_p
                        tRet = getPhyCatNameNum ( channelE_p,
                                                  "ivalue",
                                                  initChnUnit_p->phyCat_p[i].categoryName,
                                                  NULL,
                                                  initChnUnit_p->phyCat_p[i].head_p
                                                );
                    }
                }

                i++;
            }
        }
    }


    //<pntAddrCfg num="3"><!--点地址段配置,num:包括不连续地址段的段数 -->
    const XMLElement* pntAddrCfgE_p = channelE_p->FirstChildElement (
                                      "pntAddrCfg" );

    if ( pntAddrCfgE_p == NULL )
    {
        freeOneInitChnUnitM ( initChnUnit_p );
        freeOneChnlObjCfg ( otherChnlCfg_p );
        return MC_ERROR_XMLELE;
    }

    initChnUnit_p->chnNum        = chnNum     ;
    initChnUnit_p->pntObjLng     = pntObjLng  ;
    initChnUnit_p->dataIdLng     = dataIdLng  ;
    initChnUnit_p->tmStmpLng     = tmStmpLng  ;
    initChnUnit_p->pntAddrNum    = pntAddrNum ;
    initChnUnit_p->maxPhyType    = maxPhyType ;
    initChnUnit_p->phyUnitNum    = phyUnitNum ;
    otherChnlCfg_p->pntTmStmpLng = tmStmpLng;

    int addSegNum = 0;//地址段数量
    //pntAddrCfgE_p->QueryIntAttribute ( "num", &addSegNum );
    addSegNum = getXmlChildEleNum ( pntAddrCfgE_p, "contPntAdd" );

    if ( addSegNum == 0 )
    {
        freeOneInitChnUnitM ( initChnUnit_p );
        freeOneChnlObjCfg ( otherChnlCfg_p );
        return -4;

    }

    initChnUnit_p->pntAddrCfg.num = addSegNum;

    //连续地址段表头
    initChnUnit_p->pntAddrCfg.contPntAdd_p = ( T_contPntAdd* ) calloc (
            addSegNum,
            sizeof ( T_contPntAdd )
            );

    if ( initChnUnit_p->pntAddrCfg.contPntAdd_p == NULL )
    {
        freeOneInitChnUnitM ( initChnUnit_p );
        freeOneChnlObjCfg ( otherChnlCfg_p );
        return MC_ERROR_CALLOC;
    }

    cfgLen = addSegNum;
    otherChnlCfg_p->cfgLen = cfgLen;
    otherChnlCfg_p->cfgAddr_p = ( addrRange* ) calloc (
                                addSegNum,
                                sizeof ( addrRange )
                                );

    if ( otherChnlCfg_p->cfgAddr_p == NULL )
    {
        freeOneInitChnUnitM ( initChnUnit_p );
        freeOneChnlObjCfg ( otherChnlCfg_p );
        return MC_ERROR_CALLOC;
    }

    //int i = 0;
    const XMLElement* contPntAddE_p = pntAddrCfgE_p->FirstChildElement (
                                      "contPntAdd" );

    if ( contPntAddE_p == NULL )
    {
        freeOneInitChnUnitM ( initChnUnit_p );
        freeOneChnlObjCfg ( otherChnlCfg_p );
        return MC_ERROR_XMLELE;
    }

    T_contPntAdd* tContPntAdd_p = & ( initChnUnit_p->pntAddrCfg.contPntAdd_p[0] );
    contPntAddE_p->QueryIntAttribute ( "beginAddrVal",
                                       ( int* ) & ( tContPntAdd_p->beginAddrVal )
                                     );
    contPntAddE_p->QueryIntAttribute ( "endAddrVal",
                                       ( int* ) & ( tContPntAdd_p->endAddrVal )
                                     );

    otherChnlCfg_p->cfgAddr_p[0].begAddr = tContPntAdd_p->beginAddrVal;
    otherChnlCfg_p->cfgAddr_p[0].endAddr = tContPntAdd_p->endAddrVal;
    otherChnlCfg_p->cfgAddr_p[0].useNum = (
                                          tContPntAdd_p->endAddrVal
                                          - tContPntAdd_p->beginAddrVal
                                          + 1
                                          );

    int contAddIdx = 1;

    while ( ( contPntAddE_p = contPntAddE_p->NextSiblingElement ( "contPntAdd" )
            ) != NULL
            && contAddIdx < addSegNum
          )
    {
        tContPntAdd_p = & ( initChnUnit_p->pntAddrCfg.contPntAdd_p[contAddIdx] );
        contPntAddE_p->QueryIntAttribute ( "beginAddrVal",
                                           ( int* ) & ( tContPntAdd_p->beginAddrVal )
                                         );
        contPntAddE_p->QueryIntAttribute ( "endAddrVal",
                                           ( int* ) & ( tContPntAdd_p->endAddrVal )
                                         );

        otherChnlCfg_p->cfgAddr_p[contAddIdx].begAddr = tContPntAdd_p->beginAddrVal;
        otherChnlCfg_p->cfgAddr_p[contAddIdx].endAddr = tContPntAdd_p->endAddrVal;
        otherChnlCfg_p->cfgAddr_p[contAddIdx].useNum = (
                tContPntAdd_p->endAddrVal
                - tContPntAdd_p->beginAddrVal
                + 1
                );

        contAddIdx++;
    }

    int phyObjIdx = 0;

    //对每一个物理量的配置进行解析
    if ( phyUnitNum > 0 )
    {
        //调用接口进行处理
        const XMLElement* phyObjValE_p = channelE_p->FirstChildElement (
                                         "phyObjVal" );

        if ( phyObjValE_p == NULL )
        {
            freeOneInitChnUnitM ( initChnUnit_p );
            freeOneChnlObjCfg ( otherChnlCfg_p );
            return MC_ERROR_XMLELE;
        }


        //物理量配置结构表头
        initChnUnit_p->phyObjVal_p = ( T_phyValCfg* ) calloc (
                                     phyUnitNum,
                                     sizeof ( T_phyValCfg )
                                     );

        if ( initChnUnit_p->phyObjVal_p == NULL )
        {
            freeOneInitChnUnitM ( initChnUnit_p );
            freeOneChnlObjCfg ( otherChnlCfg_p );
            return MC_ERROR_CALLOC;
        }

        otherChnlCfg_p->phyType_p = ( PHY_UNIT_TYPE* ) calloc (
                                    phyUnitNum,
                                    sizeof ( PHY_UNIT_TYPE )
                                    );

        if ( otherChnlCfg_p->phyType_p == NULL )
        {
            freeOneInitChnUnitM ( initChnUnit_p );
            freeOneChnlObjCfg ( otherChnlCfg_p );
            return MC_ERROR_CALLOC;
        }

        tRet = getOnePhyCfg ( phyObjValE_p,
                              &didObj,
                              & ( initChnUnit_p->phyObjVal_p[phyObjIdx] ),
                              dataIdLng
                            );

        if ( tRet != 0 )
        {
            freeOneInitChnUnitM ( initChnUnit_p );
            freeOneChnlObjCfg ( otherChnlCfg_p );
            return tRet;
        }

        otherChnlCfg_p->phyType_p[phyObjIdx] = initChnUnit_p->phyObjVal_p[phyObjIdx].phyType;
        phyObjIdx++;

        while ( ( phyObjValE_p = phyObjValE_p->NextSiblingElement (
                                 "phyObjVal" ) ) != NULL
                && phyObjIdx < phyUnitNum
              )
        {
            tRet = getOnePhyCfg ( phyObjValE_p,
                                  &didObj,
                                  & ( initChnUnit_p->phyObjVal_p[phyObjIdx] ),
                                  dataIdLng
                                );

            if ( tRet != 0 )
            {
                freeOneInitChnUnitM ( initChnUnit_p );
                freeOneChnlObjCfg ( otherChnlCfg_p );
                return tRet;
            }

            otherChnlCfg_p->phyType_p[phyObjIdx] = initChnUnit_p->phyObjVal_p[phyObjIdx].phyType;
            phyObjIdx++;
        }

        otherChnlCfg_p->phyTypeNum = phyObjIdx;
    }

    return 0;
}



//取配置文件的localIpTable配置
int getLcalIpTab ( const XMLElement* lcalIpTabE_p,
                   T_lcalIps* lcalIps_p
                 )
{
    if ( lcalIpTabE_p == NULL
         || lcalIps_p == NULL
       )
    {
        return MC_ERROR_INPAR;
    }

    const XMLElement* dInLocalIPE_p  =  lcalIpTabE_p->FirstChildElement (
                                        "dataInIP" );

    if ( dInLocalIPE_p != NULL )
    {
        //local
        const char* lcalIp_p = dInLocalIPE_p->Attribute ( "lcalIp" );

        if ( lcalIp_p != NULL )
        {
            memcpy ( lcalIps_p->lcalDataInIp.ipAddr,
                     lcalIp_p,
                     strlen ( lcalIp_p )
                   );
        }

        dInLocalIPE_p->QueryIntAttribute ( "lcalPort",
                                           & ( lcalIps_p->lcalDataInIp.ipPort )
                                         ) ;
        //remote
        const char* rmtIp_p = dInLocalIPE_p->Attribute ( "rmtIp" );

        if ( rmtIp_p != NULL )
        {
            memcpy ( lcalIps_p->rmtDataInIp.ipAddr,
                     rmtIp_p,
                     strlen ( rmtIp_p )
                   );
        }

        dInLocalIPE_p->QueryIntAttribute ( "rmtPort",
                                           & ( lcalIps_p->rmtDataInIp.ipPort )
                                         ) ;
    }

    const XMLElement* dOutLocalIPE_p  =  lcalIpTabE_p->FirstChildElement (
                                         "dataOutIP" );

    if ( dOutLocalIPE_p != NULL )
    {
        //local
        const char* lcalIp_p = dOutLocalIPE_p->Attribute ( "lcalIp" );

        if ( lcalIp_p != NULL )
        {
            memcpy ( lcalIps_p->lcalDataOutIp.ipAddr,
                     lcalIp_p,
                     strlen ( lcalIp_p )
                   );
        }

        dOutLocalIPE_p->QueryIntAttribute ( "lcalPort",
                                            & ( lcalIps_p->lcalDataOutIp.ipPort )
                                          ) ;
        //remote
        const char* rmtIp_p = dOutLocalIPE_p->Attribute ( "rmtIp" );

        if ( rmtIp_p != NULL )
        {
            memcpy ( lcalIps_p->rmtDataOutIp.ipAddr,
                     rmtIp_p,
                     strlen ( rmtIp_p )
                   );
        }

        dOutLocalIPE_p->QueryIntAttribute ( "rmtPort",
                                            & ( lcalIps_p->rmtDataOutIp.ipPort )
                                          ) ;
    }

    return 0;
}

//从xml文件获取应用配置
int getScadaAppCfgFxml ( const char* xmlFilePath_p,     /*xml配置文件(带路径)*/
                         u_int& outChnNumS,             /*出参:通道总数*/
                         chnlObjCfg** outChnlObjCfg_pp,/*出参:返回非共用内存的配置*/
                         T_initChnUnit** outChnUnit_pp,/*出参:返回共用内存的配置*/
                         T_lcalIps* outLcalIps_p       /*出参:返回本地ip*/
                       )
{
    if ( xmlFilePath_p == NULL
         || outChnlObjCfg_pp == NULL
         || outChnUnit_pp == NULL
       )
    {
        return MC_ERROR_INPAR;
    }

    XMLDocument doc;
    int tRet = 0;
    int chnNumS = 0;//通道总数
    T_initChnUnit* initChnUnitS_p = NULL;//共用内存初始化结构T_initChnUnit[]
    chnlObjCfg* tChnlObjCfg_p = NULL;
    int chnIdx = 0;

    tRet = doc.LoadFile ( xmlFilePath_p );

    if ( tRet != 0 )
    {
        return MC_ERROR_XMLLOAD;
    }

    const XMLElement* rootE_p = doc.FirstChildElement ( "root" );

    if ( rootE_p == NULL )
    {
        return MC_ERROR_XMLELE;
    }

    const XMLElement* channelsE_p = rootE_p->FirstChildElement ( "channels" );

    if ( channelsE_p == NULL )
    {
        return MC_ERROR_XMLELE;
    }

    channelsE_p->QueryIntAttribute ( "num", &chnNumS );

    if ( chnNumS <= 0 )
    {
        return MC_ERROR_XMLELE;
    }

    tChnlObjCfg_p = ( chnlObjCfg* ) calloc (
                    chnNumS,
                    sizeof ( chnlObjCfg )
                    );

    if ( tChnlObjCfg_p == NULL )
    {
        return MC_ERROR_CALLOC;
    }

    initChnUnitS_p = ( T_initChnUnit* ) calloc (
                     chnNumS,
                     sizeof ( T_initChnUnit )
                     );

    if ( initChnUnitS_p == NULL )
    {
        freeNChnlObjCfg ( chnNumS, &tChnlObjCfg_p );
        return MC_ERROR_CALLOC;
    }

    const XMLElement* channelE_p = channelsE_p->FirstChildElement ( "channel" );

    if ( channelE_p == NULL )
    {
        freeNChnUnit ( chnNumS, &initChnUnitS_p );
        freeNChnlObjCfg ( chnNumS, &tChnlObjCfg_p );
        return MC_ERROR_XMLELE;
    }



    //生成一个通道配置值
    tRet = getOneChnCfgFrmXml ( channelE_p,
                                & ( initChnUnitS_p[chnIdx] ),
                                & ( tChnlObjCfg_p[chnIdx] )
                              );

    if ( tRet != 0 )
    {
        freeNChnUnit ( chnNumS, &initChnUnitS_p );
        freeNChnlObjCfg ( chnNumS, &tChnlObjCfg_p );
        return tRet;
    }

    chnIdx++;

    //循环取多个通道的配置
    while ( ( channelE_p = channelE_p->NextSiblingElement (
                           "channel" ) ) != NULL
            && chnIdx < chnNumS
          )
    {
        //生成一个通道配置值
        tRet = getOneChnCfgFrmXml ( channelE_p,
                                    & ( initChnUnitS_p[chnIdx] ),
                                    & ( tChnlObjCfg_p[chnIdx] )
                                  );

        if ( tRet != 0 )
        {
            freeNChnUnit ( chnNumS, &initChnUnitS_p );
            freeNChnlObjCfg ( chnNumS, &tChnlObjCfg_p );
            return tRet;
        }

        chnIdx++;
    }

    if ( outLcalIps_p != NULL )
    {
        const XMLElement* lcIpTabE_p = rootE_p->FirstChildElement ( "localIpTable" );

        if ( lcIpTabE_p == NULL )
        {
            freeNChnUnit ( chnNumS, &initChnUnitS_p );
            freeNChnlObjCfg ( chnNumS, &tChnlObjCfg_p );
            return MC_ERROR_XMLELE;
        }

        tRet = getLcalIpTab ( lcIpTabE_p, outLcalIps_p );

        if ( tRet != 0 )
        {
            freeNChnUnit ( chnNumS, &initChnUnitS_p );
            freeNChnlObjCfg ( chnNumS, &tChnlObjCfg_p );
            return tRet;
        }
    }

    *outChnlObjCfg_pp = tChnlObjCfg_p;
    *outChnUnit_pp = initChnUnitS_p;
    outChnNumS = chnIdx;

    return 0;
}

