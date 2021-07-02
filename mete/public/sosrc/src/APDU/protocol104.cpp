#define DLL_IMPLEMENT

#include "protocol104.h"
#include "InternalFunction.h"



//注：此变量空间的body_set值不准，用此变量主要是用body_set前面的空间
APDU104* APDU_104_p = NULL;



EXPDLL u_char* createAPDU ( APDU_DSCR* cstmInfo_p,
                            u_char duType,
                            u_char  allocFlag
                          )
{
    if ( APDU_104_p == NULL )
    {
        APDU_104_p = new APDU104 ( cstmInfo_p,
                                   duType,
                                   allocFlag );
    }

    APDU104* apdu_p = new APDU104 ( cstmInfo_p,
                                    duType,
                                    allocFlag );

    return ( u_char* ) apdu_p;
}




EXPDLL void freeAPDU ( u_char* APDU_Obj_p )
{
    if ( APDU_Obj_p != NULL )
    {
        APDU104* apdu_p = ( APDU104* ) APDU_Obj_p;
        delete  apdu_p;
        APDU_Obj_p = NULL;
    }
}




EXPDLL int getPtclID()
{
    return 104;
}




EXPDLL FUN_RL* createRl_P ( u_char srvNo,
                            u_char usingMode,
                            T_snnCfg* snnCfg_p,
                            T_ptclFunDsc* ptclFunDsc
                          )
{
    if ( snnCfg_p == NULL
         || ptclFunDsc == NULL
       )
    {
        return NULL;
    }

    FUN_RL* Fun_p = new FUN_RL();
    T_stnAcsFlag tmpStnAcsFlag = {0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0};

    switch ( srvNo )
    {
        case STN_INIT://站初始化
        {
            break;
        }

        case QUE_DATA: //查询数据
        {
#ifndef INTER_CREATERL_VAL

            setFunRl ( srvNo,
                       usingMode,
                       I_F,
                       snnCfg_p,
                       Fun_p
                     );
#endif//#ifndef INTER_CREATERL_VAL 

#ifdef INTER_CREATERL_VAL

            if ( usingMode != 0xFF ) //以入参为准
            {
                if ( usingMode != RD_DATA )
                {
                    freeRl_P ( Fun_p, NULL, NULL );
                    return NULL;
                }
            }
            else//配置选一个可以的值
            {
                usingMode = RD_DATA;
            }

            ptclFunDsc->usingMode = usingMode;

            u_char stepNum = 2;

            //1、对步规则Fun_p赋值
            Fun_p->srvNo              = QUE_DATA;
            Fun_p->usingMode          = RD_DATA;
            Fun_p->cmLvl[POLL_EVT]    = UNKNOWN;
            Fun_p->cmLvl[POLL_DATA]   = UNKNOWN;
            Fun_p->cmLvl[RD_DATA]     = APP_LVL; // 104规约取APP_LVL
            Fun_p->stepNum[POLL_EVT]  = UNKNOWN;
            Fun_p->stepNum[POLL_DATA] = UNKNOWN;
            Fun_p->stepNum[RD_DATA]   = stepNum;
            //第一步
            ONE_STEP* one_step_p        = new ONE_STEP();
            vrfItem* tsVerifyBlock1_p   = new vrfItem();
            tsVerifyBlock1_p->chkRslt_p = new int();
            setONE_STEP ( one_step_p, 1, I_F, 1, NULL, I_F, 0, NULL,
                          tsVerifyBlock1_p );
            //第二步
            ONE_STEP* two_step_p        = new ONE_STEP();
            vrfItem* tsVerifyBlock2_p   = new vrfItem();
            tsVerifyBlock2_p->chkRslt_p = new int();
            setONE_STEP ( two_step_p, 2, I_F, 0, NULL, I_F, 1, NULL,
                          tsVerifyBlock2_p );
            two_step_p->nextStep_p = NULL;
            addNode ( one_step_p, two_step_p );

            Fun_p->steps_p[RD_DATA] = one_step_p;


            //2、对会话配置默认帧数据 T_snnCfg* stnSnnCfg_p
            u_short cmnAddr = 1;
            BODY_ADDR_VAL bodyAddrVal = {};
            bodyAddrVal.shortAddr = 4;
            u_char defSetNum = 2;//默认信息元素个数

            snnCfg_p->ssnID = srvNo;
            //snnCfg_p->actFlag
            snnCfg_p->infoType = GINFOTYPE;
            snnCfg_p->cmnAddr = cmnAddr;
            snnCfg_p->stepNum = stepNum;
            snnCfg_p->stepCfg_p = new T_ssnStepCfg[stepNum]();
            //对第一步
            //[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
            tmpStnAcsFlag.ctlType = 0;
            snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
            //第一步请求帧
            u_char oneReqAPDUNum = 1;//第一步的请求帧数量
            snnCfg_p->stepCfg_p[0].reqAPDUNum = oneReqAPDUNum;
            snnCfg_p->stepCfg_p[0].reqBody_setNum_p = new u_char[oneReqAPDUNum]();
            snnCfg_p->stepCfg_p[0].reqBody_setNum_p[0] = 0;
            //snnCfg_p->stepCfg_p[0].reqEleVal_pp =  new ELE_TYPE[oneReqAPDUNum][11]();
            snnCfg_p->stepCfg_p[0].reqData_p =  new dataFrame[oneReqAPDUNum]();
            u_char memVal[11] = {};
            ELE_TYPE eleVal[11] =
            {
                {APDU_APCI_INDEX, 0, &memVal[0]},
                {ASDU_TID_INDEX, 1, &memVal[1]},
                {ASDU_LNG_INDEX, 0, &memVal[2]},
                {ASDU_VSQ_INDEX, 1, &memVal[3]},
                {ASDU_COT_INDEX, 1, &memVal[4]},
                {ASDU_ADDR_INDEX, sizeof ( u_short ), ( u_char* )& cmnAddr},
                {BODY_TID_INDEX, 0, &memVal[6]},
                {BODY_ADDR_INDEX, sizeof ( BODY_ADDR_VAL ), ( u_char* )& bodyAddrVal},
                {BODY_SET_INDEX, 0, &memVal[8]},
                {BODY_TSP_INDEX, 0, &memVal[9]},
                {ASDU_TSP_INDEX, 0, &memVal[10]}
            };
            * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_102;
            * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_5;
            //调用接口给snnCfg_p第一步请求帧的第1帧赋值
            setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
            //第一步响应
            snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

            //第二步被读数据的默认配置
            //[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
            tmpStnAcsFlag.ctlType = 2;
            tmpStnAcsFlag.setValid = 1;
            snnCfg_p->stepCfg_p[1].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
            //第二步的请求帧
            snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
            //第二步的响应帧
            u_char twoRespAPDUNum = 1;//第二步的响应帧数量
            snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
            snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
            snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
            defSetNum;//第二步响应第1帧元素集个数
            //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
            snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
            * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_22;
            * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = 1 << 7 | MAX104_VSQ_N;
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_5;
            eleVal[BODY_SET_INDEX].lng = 1;
            * ( eleVal[BODY_SET_INDEX].p_Mem ) = 121;
            //调用接口给snnCfg_p第二步响应的第1帧赋值
            setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

            //3、配置默认的步号与tid,cot的对应关系
            ptclFunDsc->ssnDscNo = srvNo;
            ptclFunDsc->act_prio_chnlID = ACTIVE_TYPE;
            ptclFunDsc->usingMode = usingMode;
            ptclFunDsc->setNum = stepNum;//stepNum=2
            ptclFunDsc->remoteSet_p = new ptclStep[stepNum]();
            ptclFunDsc->remoteSet_p[0].TID_p = new u_short();
            ptclFunDsc->remoteSet_p[0].TID_p[0] = MC_TID_102;
            ptclFunDsc->remoteSet_p[0].COT = MC_COT_5;
            ptclFunDsc->remoteSet_p[0].MaxLen = MAX104_ASDU_LNG;
            ptclFunDsc->remoteSet_p[0].TidNum = 1;

            ptclFunDsc->remoteSet_p[1].TID_p = new u_short();
            ptclFunDsc->remoteSet_p[1].TID_p[0] = MC_TID_22;
            ptclFunDsc->remoteSet_p[1].COT = MC_COT_5;
            ptclFunDsc->remoteSet_p[1].MaxLen = MAX104_ASDU_LNG;
            ptclFunDsc->remoteSet_p[1].TidNum = 1;
#endif//INTER_CREATERL_VAL

            break;
        }

        case CYC_TRNS: //周期传输
        {
#ifndef INTER_CREATERL_VAL

            setFunRl ( srvNo,
                       usingMode,
                       I_F,
                       snnCfg_p,
                       Fun_p
                     );
#endif//#ifndef INTER_CREATERL_VAL 


#ifdef INTER_CREATERL_VAL

            if ( usingMode != 0xFF ) //以入参为准
            {
                if ( usingMode != ONLY_ONE )
                {
                    freeRl_P ( Fun_p, NULL, NULL );
                    return NULL;
                }
            }
            else//配置选一个可以的值
            {
                usingMode = ONLY_ONE;
            }

            ptclFunDsc->usingMode = usingMode;
            u_char stepNum = 1;

            //1、对步规则Fun_p赋值
            Fun_p->srvNo             = CYC_TRNS;
            Fun_p->usingMode         = ONLY_ONE;
            Fun_p->cmLvl[ONLY_ONE]   = APP_LVL; //104规约取APP_LVL
            Fun_p->stepNum[ONLY_ONE] = stepNum;
            ONE_STEP* one_step_p        = new ONE_STEP();
            vrfItem* tsVerifyBlock1_p   = new vrfItem();
            tsVerifyBlock1_p->chkRslt_p = new int();
            setONE_STEP ( one_step_p, 1, I_F, 0, NULL, I_F, 1, NULL,
                          tsVerifyBlock1_p );

            Fun_p->steps_p[ONLY_ONE] = one_step_p;


            //2、对会话配置默认帧数据 T_snnCfg* stnSnnCfg_p
            u_short cmnAddr = 1;
            BODY_ADDR_VAL bodyAddrVal = {};
            bodyAddrVal.shortAddr = 4;
            u_char defSetNum = 1;//默认信息元素个数

            snnCfg_p->ssnID = srvNo;
            //snnCfg_p->actFlag
            snnCfg_p->infoType = GINFOTYPE;
            snnCfg_p->cmnAddr = cmnAddr;
            snnCfg_p->stepNum = stepNum;
            snnCfg_p->stepCfg_p = new T_ssnStepCfg[stepNum]();

            //第一步
            //[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
            tmpStnAcsFlag.ctlType = 2;
            tmpStnAcsFlag.setValid = 1;
            snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
            //第一步的请求帧
            u_char oneReqAPDUNum = 0;
            snnCfg_p->stepCfg_p[0].reqAPDUNum = oneReqAPDUNum;
            //第一步响应的帧
            u_char oneRespAPDUNum = 1;//第一步的响应帧数量
            snnCfg_p->stepCfg_p[0].respAPDUNum = oneRespAPDUNum;
            snnCfg_p->stepCfg_p[0].respBody_setNum_p = new u_char[oneRespAPDUNum]();
            snnCfg_p->stepCfg_p[0].respBody_setNum_p[0] =
            defSetNum;//第一步响应第1帧元素集个数
            //snnCfg_p->stepCfg_p[0].respEleVal_pp = new ELE_TYPE[oneRespAPDUNum][11]();
            snnCfg_p->stepCfg_p[0].respData_p = new dataFrame[oneRespAPDUNum]();
            u_char memVal[11] = {};
            ELE_TYPE eleVal[11] =
            {
                {APDU_APCI_INDEX, 0, &memVal[0]},
                {ASDU_TID_INDEX, 1, &memVal[1]},
                {ASDU_LNG_INDEX, 0, &memVal[2]},
                {ASDU_VSQ_INDEX, 1, &memVal[3]},
                {ASDU_COT_INDEX, 1, &memVal[4]},
                {ASDU_ADDR_INDEX, sizeof ( u_short ), ( u_char* )& cmnAddr},
                {BODY_TID_INDEX, 0, &memVal[6]},
                {BODY_ADDR_INDEX, sizeof ( BODY_ADDR_VAL ), ( u_char* )& bodyAddrVal},
                {BODY_SET_INDEX, 1, &memVal[8]},
                {BODY_TSP_INDEX, 0, &memVal[9]},
                {ASDU_TSP_INDEX, 0, &memVal[10]}
            };
            * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_22;
            * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = 1 << 7 | MAX104_VSQ_N;
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_1;
            * ( eleVal[BODY_SET_INDEX].p_Mem ) = 0;
            //调用接口给snnCfg_p第一步响应帧的第1帧赋值
            setOneFrmCfg ( PASSIVE_TYPE, 1, 1, eleVal, snnCfg_p );

            //3、配置默认的步号与tid,cot的对应关系
            ptclFunDsc->ssnDscNo = srvNo;
            ptclFunDsc->act_prio_chnlID = PASSIVE_TYPE;
            ptclFunDsc->usingMode = usingMode;
            ptclFunDsc->setNum = stepNum;//stepNum=1
            ptclFunDsc->remoteSet_p = new ptclStep[stepNum]();
            ptclFunDsc->remoteSet_p[0].TID_p = new u_short();
            ptclFunDsc->remoteSet_p[0].TID_p[0] = MC_TID_22;
            ptclFunDsc->remoteSet_p[0].COT = MC_COT_1;
            ptclFunDsc->remoteSet_p[0].MaxLen = MAX104_ASDU_LNG;
            ptclFunDsc->remoteSet_p[0].TidNum = 1;
#endif//INTER_CREATERL_VAL

            break;
        }

        case ACQ_EVT: //事件收集
        {
#ifndef INTER_CREATERL_VAL

            setFunRl ( srvNo,
                       usingMode,
                       I_F,
                       snnCfg_p,
                       Fun_p
                     );
#endif//#ifndef INTER_CREATERL_VAL

#ifdef INTER_CREATERL_VAL

            if ( usingMode != 0xFF ) //以入参为准
            {
                if ( usingMode != ONLY_ONE )
                {
                    freeRl_P ( Fun_p, NULL, NULL );
                    return NULL;
                }
            }
            else//配置选一个可以的值
            {
                usingMode = ONLY_ONE;
            }

            ptclFunDsc->usingMode = usingMode;

            u_char stepNum = 1;

            //1、对步规则Fun_p赋值
            Fun_p->srvNo             = ACQ_EVT;
            Fun_p->usingMode         = ONLY_ONE;
            Fun_p->cmLvl[ONLY_ONE]   = APP_LVL; //104规约取APP_LVL
            Fun_p->stepNum[ONLY_ONE] = stepNum;
            ONE_STEP* one_step_p        = new ONE_STEP();
            vrfItem* tsVerifyBlock1_p   = new vrfItem();
            tsVerifyBlock1_p->chkRslt_p = new int();
            setONE_STEP ( one_step_p, 1, I_F, 0, NULL, I_F, 1, NULL,
                          tsVerifyBlock1_p );

            Fun_p->steps_p[ONLY_ONE] = one_step_p;

            //2、对会话配置默认帧数据 T_snnCfg* stnSnnCfg_p
            u_short cmnAddr = 1;
            BODY_ADDR_VAL bodyAddrVal = {};
            bodyAddrVal.shortAddr = 4;
            u_char defSetNum = 1;//默认信息元素个数

            snnCfg_p->ssnID = srvNo;
            //snnCfg_p->actFlag
            snnCfg_p->infoType = GINFOTYPE;
            snnCfg_p->cmnAddr = cmnAddr;
            snnCfg_p->stepNum = stepNum;
            snnCfg_p->stepCfg_p = new T_ssnStepCfg[stepNum]();

            //第一步
            //[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
            tmpStnAcsFlag.ctlType = 2;
            tmpStnAcsFlag.setValid = 1;
            snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
            //第一步的请求帧
            u_char oneReqAPDUNum = 0;
            snnCfg_p->stepCfg_p[0].reqAPDUNum = oneReqAPDUNum;
            //第一步响应的帧
            u_char oneRespAPDUNum = 1;//第一步的响应帧数量
            snnCfg_p->stepCfg_p[0].respAPDUNum = oneRespAPDUNum;
            snnCfg_p->stepCfg_p[0].respBody_setNum_p = new u_char[oneRespAPDUNum]();
            snnCfg_p->stepCfg_p[0].respBody_setNum_p[0] =
            defSetNum;//第一步响应第1帧元素集个数
            //snnCfg_p->stepCfg_p[0].respEleVal_pp = new ELE_TYPE[oneRespAPDUNum][11]();
            snnCfg_p->stepCfg_p[0].respData_p = new dataFrame[oneRespAPDUNum]();
            u_char memVal[11] = {};
            ELE_TYPE eleVal[11] =
            {
                {APDU_APCI_INDEX, 0, &memVal[0]},
                {ASDU_TID_INDEX, 1, &memVal[1]},
                {ASDU_LNG_INDEX, 0, &memVal[2]},
                {ASDU_VSQ_INDEX, 1, &memVal[3]},
                {ASDU_COT_INDEX, 1, &memVal[4]},
                {ASDU_ADDR_INDEX, sizeof ( u_short ), ( u_char* )& cmnAddr},
                {BODY_TID_INDEX, 0, &memVal[6]},
                {BODY_ADDR_INDEX, sizeof ( BODY_ADDR_VAL ), ( u_char* )& bodyAddrVal},
                {BODY_SET_INDEX, 1, &memVal[8]},
                {BODY_TSP_INDEX, 0, &memVal[9]},
                {ASDU_TSP_INDEX, 0, &memVal[10]}
            };
            * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_1;
            * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = 0xf0;
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_3;
            * ( eleVal[BODY_SET_INDEX].p_Mem ) = 0;
            //调用接口给snnCfg_p第一步响应帧的第1帧赋值
            setOneFrmCfg ( PASSIVE_TYPE, 1, 1, eleVal, snnCfg_p );

            //3、配置默认的步号与tid,cot的对应关系
            ptclFunDsc->ssnDscNo = srvNo;
            ptclFunDsc->act_prio_chnlID = PASSIVE_TYPE;
            ptclFunDsc->usingMode = usingMode;
            ptclFunDsc->setNum = stepNum;//stepNum=1
            ptclFunDsc->remoteSet_p = new ptclStep[stepNum]();
            ptclFunDsc->remoteSet_p[0].TID_p = new u_short[2]();
            ptclFunDsc->remoteSet_p[0].TIDLng_p = new u_char[2]();
            ptclFunDsc->remoteSet_p[0].TID_p[0] = MC_TID_1;
            ptclFunDsc->remoteSet_p[0].TIDLng_p[0] = 1;
            ptclFunDsc->remoteSet_p[0].TID_p[1] = MC_TID_13;
            ptclFunDsc->remoteSet_p[0].TIDLng_p[1] = 5;
            ptclFunDsc->remoteSet_p[0].COT = MC_COT_3;
            ptclFunDsc->remoteSet_p[0].MaxLen = MAX104_ASDU_LNG;
            ptclFunDsc->remoteSet_p[0].TidNum = 2;
#endif//INTER_CREATERL_VAL

            break;
        }

        case QUKCHK_EVT://快速-检验(quick-check)过程采集事件
        {
            //不使用
            freeRl_P ( Fun_p, snnCfg_p, ptclFunDsc );
            break;
        }

        case STN_INRGTN: //站召唤
        {
#ifndef INTER_CREATERL_VAL

            setFunRl ( srvNo,
                       usingMode,
                       I_F,
                       snnCfg_p,
                       Fun_p
                     );
#endif//#ifndef INTER_CREATERL_VAL

#ifdef INTER_CREATERL_VAL

            if ( usingMode != 0xFF ) //以入参为准
            {
                if ( usingMode != ONLY_ONE )
                {
                    freeRl_P ( Fun_p, NULL, NULL );
                    return NULL;
                }
            }
            else//配置选一个可以的值
            {
                usingMode = ONLY_ONE;
            }

            ptclFunDsc->usingMode = usingMode;

            u_char stepNum = 4;
            u_char stepTwoRespFrmNum = 13;

            //1、对步规则Fun_p赋值
            Fun_p->srvNo             = STN_INRGTN;
            Fun_p->usingMode         = ONLY_ONE;
            Fun_p->cmLvl[ONLY_ONE]   = APP_LVL;//104规约取APP_LVL
            Fun_p->stepNum[ONLY_ONE] = stepNum;//由选用的配套协议重新设置
            //第一步
            ONE_STEP* one_step_p        = new ONE_STEP();
            vrfItem* tsVerifyBlock1_p   = new vrfItem();
            tsVerifyBlock1_p->chkRslt_p = new int();
            setONE_STEP ( one_step_p, 1, I_F, 1, NULL, I_F, 0, NULL,
                          tsVerifyBlock1_p );
            //第二步
            ONE_STEP* one_step_resp_p        = new ONE_STEP();
            vrfItem* tsVerifyBlock11_p   = new vrfItem();
            tsVerifyBlock11_p->chkRslt_p = new int();
            setONE_STEP ( one_step_resp_p, 2, I_F, 0, NULL, I_F, 1, NULL,
                          tsVerifyBlock11_p );
            addNode ( one_step_p, one_step_resp_p );
            //第三步
            ONE_STEP* two_step_p        = new ONE_STEP();
            vrfItem* tsVerifyBlock2_p   = new vrfItem();
            tsVerifyBlock2_p->chkRslt_p = new int();
            setONE_STEP ( two_step_p, 3, I_F, 0, NULL, I_F,
                          stepTwoRespFrmNum, NULL, tsVerifyBlock2_p );
            two_step_p->nextStep_p = NULL;
            addNode ( one_step_p, two_step_p );

            if ( 1 == 1 ) //通过配置判断
            {
                //第四步
                ONE_STEP* three_step_p      = new ONE_STEP();
                vrfItem* tsVerifyBlock3_p   = new vrfItem();
                tsVerifyBlock3_p->chkRslt_p = new int();
                setONE_STEP ( three_step_p, 4, I_F, 0, NULL, I_F, 1,
                              NULL, tsVerifyBlock3_p );
                three_step_p->nextStep_p = NULL;
                addNode ( one_step_p, three_step_p );
            }

            Fun_p->steps_p[ONLY_ONE] = one_step_p;

            //2、对会话配置默认帧数据 T_snnCfg* stnSnnCfg_p
            //fusk test add
            /*struct T_zzfVal{
                u_int BSI;
                u_char QDS;
            } zzfVal = {};*/
            u_short cmnAddr = 2;
            BODY_ADDR_VAL bodyAddrVal = {};
            u_char defSetNum = 2;//默认信息元素个数

            snnCfg_p->ssnID = srvNo;
            //snnCfg_p->actFlag
            snnCfg_p->infoType = GINFOTYPE;
            snnCfg_p->cmnAddr = cmnAddr;
            snnCfg_p->stepNum = stepNum;
            snnCfg_p->stepCfg_p = new T_ssnStepCfg[stepNum]();
            //第一步总召唤命令
            //[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
            tmpStnAcsFlag.ctlType = 0;
            snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
            //第一步请求帧
            u_char oneReqAPDUNum = 1;//第一步的请求帧数量
            snnCfg_p->stepCfg_p[0].reqAPDUNum = oneReqAPDUNum;
            snnCfg_p->stepCfg_p[0].reqBody_setNum_p = new u_char[oneReqAPDUNum]();
            snnCfg_p->stepCfg_p[0].reqBody_setNum_p[0] = 1;//请求第1帧元素集为QCC
            //snnCfg_p->stepCfg_p[0].reqEleVal_pp =  new ELE_TYPE[oneReqAPDUNum][11]();
            snnCfg_p->stepCfg_p[0].reqData_p =  new dataFrame[oneReqAPDUNum]();
            u_char memVal[11] = {};
            ELE_TYPE eleVal[11] =
            {
                {APDU_APCI_INDEX, 0, &memVal[0]},
                {ASDU_TID_INDEX, 1, &memVal[1]},
                {ASDU_LNG_INDEX, 0, &memVal[2]},
                {ASDU_VSQ_INDEX, 1, &memVal[3]},
                {ASDU_COT_INDEX, 1, &memVal[4]},
                {ASDU_ADDR_INDEX, sizeof ( u_short ), ( u_char* )& cmnAddr},
                {BODY_TID_INDEX, 0, &memVal[6]},
                {BODY_ADDR_INDEX, sizeof ( BODY_ADDR_VAL ), ( u_char* )& bodyAddrVal},
                {BODY_SET_INDEX, 1, &memVal[8]},
                //{BODY_SET_INDEX   ,sizeof(zzfVal),(u_char*)&zzfVal},
                {BODY_TSP_INDEX, 0, &memVal[9]},
                {ASDU_TSP_INDEX, 0, &memVal[10]}
            };
            * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_100;
            * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_6;
            eleVal[BODY_SET_INDEX].lng = sizeof ( u_char );
            * ( eleVal[BODY_SET_INDEX].p_Mem ) = 20; //QCC 站召唤
            //调用接口给snnCfg_p第一步请求帧的第1帧赋值
            setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
            //第一步响应帧
            snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

            //第二步确认站召唤命令
            //[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
            tmpStnAcsFlag.ctlType = 0;
            snnCfg_p->stepCfg_p[1].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
            //第二步请求帧
            snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
            //第二步响应帧
            snnCfg_p->stepCfg_p[1].respAPDUNum = 1;
            snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[1 + 1]();
            snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
            1;//第二步响应第1帧元素集个数
            //_ASSERTE(_CrtCheckMemory());
            //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[1][11]();
            snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[1]();
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_7;
            //调用接口给snnCfg_p第二步响应帧的第1帧赋值
            setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

            //第三步被召唤的信息
            //[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
            tmpStnAcsFlag.ctlType = 2;
            tmpStnAcsFlag.setValid = 1;
            snnCfg_p->stepCfg_p[2].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
            //第三步请求帧
            snnCfg_p->stepCfg_p[2].reqAPDUNum = 0;
            //第三步响应帧
            //u_char twoRespAPDUNum = stepTwoRespFrmNum;//第二步的响应帧数量
            snnCfg_p->stepCfg_p[2].respAPDUNum = stepTwoRespFrmNum;
            snnCfg_p->stepCfg_p[2].respBody_setNum_p = new u_char[stepTwoRespFrmNum]();
            snnCfg_p->stepCfg_p[2].respBody_setNum_p[0] =
            defSetNum;//第二步响应第1帧元素集个数
            //snnCfg_p->stepCfg_p[2].respEleVal_pp = new ELE_TYPE[stepTwoRespFrmNum][11]();
            snnCfg_p->stepCfg_p[2].respData_p = new dataFrame[stepTwoRespFrmNum]();
            //*(eleVal[ASDU_TID_INDEX].p_Mem) = MC_TID_7;
            //*(eleVal[ASDU_TID_INDEX].p_Mem) = MC_TID_13;
            * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_1;
            * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = 1 << 7 | MAX104_VSQ_N;
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_20;
            bodyAddrVal.shortAddr = 4;
            * ( eleVal[BODY_SET_INDEX].p_Mem ) = 0;
            //eleVal[BODY_SET_INDEX].lng = sizeof(zzfVal);
            //zzfVal.BSI = 1234;
            //zzfVal.QDS = 66;
            //调用接口给snnCfg_p第三步响应帧的第1帧赋值
            setOneFrmCfg ( PASSIVE_TYPE, 3, 1, eleVal, snnCfg_p );

            ////调用接口给snnCfg_p第三步响应帧的第2帧赋值
            bodyAddrVal.shortAddr = 6;
            snnCfg_p->stepCfg_p[2].respBody_setNum_p[1] =
            defSetNum;//第三步响应第2帧元素集个数
            setOneFrmCfg ( PASSIVE_TYPE, 3, 2, eleVal, snnCfg_p );

            //调用接口给snnCfg_p第三步响应帧的第3帧赋值
            bodyAddrVal.shortAddr = 8;
            snnCfg_p->stepCfg_p[2].respBody_setNum_p[2] =
            defSetNum;//第三步响应第3帧元素集个数
            setOneFrmCfg ( PASSIVE_TYPE, 3, 3, eleVal, snnCfg_p );

            //调用接口给snnCfg_p第三步响应帧的第4帧赋值
            bodyAddrVal.shortAddr = 10;
            snnCfg_p->stepCfg_p[2].respBody_setNum_p[3] =
            defSetNum;//第三步响应第4帧元素集个数
            setOneFrmCfg ( PASSIVE_TYPE, 3, 4, eleVal, snnCfg_p );

            //调用接口给snnCfg_p第三步响应帧的第5帧赋值
            bodyAddrVal.shortAddr = 12;
            snnCfg_p->stepCfg_p[2].respBody_setNum_p[4] =
            defSetNum;//第三步响应第5帧元素集个数
            setOneFrmCfg ( PASSIVE_TYPE, 3, 5, eleVal, snnCfg_p );

            //调用接口给snnCfg_p第三步响应帧的第6帧赋值
            bodyAddrVal.shortAddr = 14;
            snnCfg_p->stepCfg_p[2].respBody_setNum_p[5] =
            defSetNum;//第三步响应第6帧元素集个数
            setOneFrmCfg ( PASSIVE_TYPE, 3, 6, eleVal, snnCfg_p );

            //调用接口给snnCfg_p第三步响应帧的第7帧赋值
            bodyAddrVal.shortAddr = 16;
            snnCfg_p->stepCfg_p[2].respBody_setNum_p[6] =
            defSetNum;//第三步响应第7帧元素集个数
            setOneFrmCfg ( PASSIVE_TYPE, 3, 7, eleVal, snnCfg_p );

            //调用接口给snnCfg_p第三步响应帧的第8帧赋值
            bodyAddrVal.shortAddr = 18;
            snnCfg_p->stepCfg_p[2].respBody_setNum_p[7] =
            defSetNum;//第三步响应第8帧元素集个数
            setOneFrmCfg ( PASSIVE_TYPE, 3, 8, eleVal, snnCfg_p );

            //调用接口给snnCfg_p第三步响应帧的第9帧赋值
            bodyAddrVal.shortAddr = 20;
            snnCfg_p->stepCfg_p[2].respBody_setNum_p[8] =
            defSetNum;//第三步响应第9帧元素集个数
            setOneFrmCfg ( PASSIVE_TYPE, 3, 9, eleVal, snnCfg_p );

            //调用接口给snnCfg_p第三步响应帧的第10帧赋值
            bodyAddrVal.shortAddr = 22;
            snnCfg_p->stepCfg_p[2].respBody_setNum_p[9] =
            defSetNum;//第三步响应第10帧元素集个数
            setOneFrmCfg ( PASSIVE_TYPE, 3, 10, eleVal, snnCfg_p );

            //调用接口给snnCfg_p第三步响应帧的第11帧赋值
            bodyAddrVal.shortAddr = 24;
            snnCfg_p->stepCfg_p[2].respBody_setNum_p[10] =
            defSetNum;//第三步响应第11帧元素集个数
            setOneFrmCfg ( PASSIVE_TYPE, 3, 11, eleVal, snnCfg_p );

            //调用接口给snnCfg_p第三步响应帧的第12帧赋值
            bodyAddrVal.shortAddr = 26;
            snnCfg_p->stepCfg_p[2].respBody_setNum_p[11] =
            defSetNum;//第三步响应第12帧元素集个数
            setOneFrmCfg ( PASSIVE_TYPE, 3, 12, eleVal, snnCfg_p );

            //调用接口给snnCfg_p第三步响应帧的第13帧赋值
            bodyAddrVal.shortAddr = 28;
            snnCfg_p->stepCfg_p[2].respBody_setNum_p[12] =
            1;//第三步响应第13帧元素集个数
            setOneFrmCfg ( PASSIVE_TYPE, 3, 13, eleVal, snnCfg_p );

            //第四步站召唤结束
            tmpStnAcsFlag.ctlType =
            0;//[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
            snnCfg_p->stepCfg_p[3].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
            //第四步请求帧
            snnCfg_p->stepCfg_p[3].reqAPDUNum = 0;
            //第四步响应帧
            snnCfg_p->stepCfg_p[3].respAPDUNum = 1;
            snnCfg_p->stepCfg_p[3].respBody_setNum_p = new u_char[1]();
            snnCfg_p->stepCfg_p[3].respBody_setNum_p[0] =
            1;//第四步响应第1帧元素集个数
            //snnCfg_p->stepCfg_p[3].respEleVal_pp = new ELE_TYPE[1][11]();
            snnCfg_p->stepCfg_p[3].respData_p = new dataFrame[1]();
            * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_100;
            * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_10;
            //调用接口给snnCfg_p第四步响应帧的第1帧赋值
            setOneFrmCfg ( PASSIVE_TYPE, 4, 1, eleVal, snnCfg_p );

            //3、配置默认的步号与tid,cot的对应关系
            ptclFunDsc->ssnDscNo = srvNo;
            ptclFunDsc->act_prio_chnlID = ACTIVE_TYPE;
            ptclFunDsc->usingMode = usingMode;
            ptclFunDsc->setNum = stepNum;//stepNum=2
            ptclFunDsc->remoteSet_p = new ptclStep[stepNum]();
            ptclFunDsc->remoteSet_p[0].TID_p = new u_short();
            ptclFunDsc->remoteSet_p[0].TIDLng_p = new u_char();
            ptclFunDsc->remoteSet_p[0].TID_p[0] = MC_TID_100;
            ptclFunDsc->remoteSet_p[0].TIDLng_p[0] = 1;
            ptclFunDsc->remoteSet_p[0].COT = MC_COT_6;
            ptclFunDsc->remoteSet_p[0].MaxLen = MAX104_ASDU_LNG;
            ptclFunDsc->remoteSet_p[0].TidNum = 1;

            ptclFunDsc->remoteSet_p[1].TID_p = new u_short();
            ptclFunDsc->remoteSet_p[1].TIDLng_p = new u_char();
            ptclFunDsc->remoteSet_p[1].TID_p[0] = MC_TID_100;
            ptclFunDsc->remoteSet_p[1].TIDLng_p[0] = 1;
            ptclFunDsc->remoteSet_p[1].COT = MC_COT_7;
            ptclFunDsc->remoteSet_p[1].MaxLen = MAX104_ASDU_LNG;
            ptclFunDsc->remoteSet_p[1].TidNum = 1;
            //ptclFunDsc->remoteSet_p[1].TID = MC_TID_7;

            ptclFunDsc->remoteSet_p[2].TID_p = new u_short[2]();
            ptclFunDsc->remoteSet_p[2].TIDLng_p = new u_char[2]();
            ptclFunDsc->remoteSet_p[2].TID_p[0] = MC_TID_1;
            ptclFunDsc->remoteSet_p[2].TIDLng_p[0] = 1;
            ptclFunDsc->remoteSet_p[2].TID_p[1] = MC_TID_13;
            ptclFunDsc->remoteSet_p[2].TIDLng_p[1] = 5;
            ptclFunDsc->remoteSet_p[2].COT = MC_COT_20;
            ptclFunDsc->remoteSet_p[2].MaxLen = MAX104_ASDU_LNG;
            ptclFunDsc->remoteSet_p[2].TidNum = 2;

            ptclFunDsc->remoteSet_p[3].TID_p = new u_short();
            ptclFunDsc->remoteSet_p[3].TIDLng_p = new u_char();
            ptclFunDsc->remoteSet_p[3].TID_p[0] = MC_TID_100;
            ptclFunDsc->remoteSet_p[3].TIDLng_p[0] = 1;
            ptclFunDsc->remoteSet_p[3].COT = MC_COT_10;
            ptclFunDsc->remoteSet_p[3].MaxLen = MAX104_ASDU_LNG;
            ptclFunDsc->remoteSet_p[3].TidNum = 1;
#endif//#ifdef INTER_CREATERL_VAL

            break;
        }

        case CLK_SYN://时钟同步
        {
#ifndef INTER_CREATERL_VAL

            setFunRl ( srvNo,
                       usingMode,
                       I_F,
                       snnCfg_p,
                       Fun_p
                     );
#endif//#ifndef INTER_CREATERL_VAL

#ifdef INTER_CREATERL_VAL

            if ( usingMode != 0xFF ) //以入参为准
            {
                if ( usingMode != CRCT_REQ )
                {
                    freeRl_P ( Fun_p, NULL, NULL );
                    return NULL;
                }
            }
            else//配置选一个可以的值
            {
                usingMode = CRCT_REQ;
            }

            ptclFunDsc->usingMode = usingMode;

            u_char stepNum = 1;

            //1、对步规则Fun_p赋值
            Fun_p->srvNo             = CLK_SYN;
            Fun_p->usingMode         =
            CRCT_REQ;//由选用的配套协议设置 CRCT_REQ CRCT_BC
            Fun_p->cmLvl[CRCT_REQ]   = APP_LVL; //104规约取APP_LVL
            Fun_p->cmLvl[CRCT_BC]    = LNK_LVL;
            Fun_p->stepNum[CRCT_REQ] = stepNum;
            Fun_p->stepNum[CRCT_BC]  = 1;
            //第一步
            ONE_STEP* one_CRCT_REQ_p      = new ONE_STEP();
            vrfItem* tsVerifyCRCTREQ1_p   = new vrfItem();
            tsVerifyCRCTREQ1_p->chkRslt_p = new int();
            setONE_STEP ( one_CRCT_REQ_p, 1, I_F, 1, NULL, I_F, 0, NULL,
                          tsVerifyCRCTREQ1_p );
            ////第二步
            //ONE_STEP* two_CRCT_REQ_p      = new ONE_STEP();
            //vrfItem* tsVerifyCRCTREQ2_p   = new vrfItem();
            //tsVerifyCRCTREQ2_p->chkRslt_p = new int();
            //setONE_STEP(two_CRCT_REQ_p , 2 , I_F , 0 , NULL , I_F , 1, NULL , tsVerifyCRCTREQ2_p);
            //addNode(one_CRCT_REQ_p , two_CRCT_REQ_p );

            Fun_p->steps_p[CRCT_REQ] = one_CRCT_REQ_p;

            //2、对会话配置默认帧数据 T_snnCfg* stnSnnCfg_p
            u_short cmnAddr = 1;

            snnCfg_p->ssnID = srvNo;
            //snnCfg_p->actFlag
            snnCfg_p->infoType = GINFOTYPE;
            snnCfg_p->cmnAddr = cmnAddr;
            snnCfg_p->stepNum = stepNum;
            snnCfg_p->stepCfg_p = new T_ssnStepCfg[stepNum]();
            //第一步时间同步
            tmpStnAcsFlag.ctlType =
            2;//[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
            tmpStnAcsFlag.tspValidB = 1;
            snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
            //第一步请求帧
            u_char oneReqAPDUNum = 1;//第一步的请求帧数量
            snnCfg_p->stepCfg_p[0].reqAPDUNum = oneReqAPDUNum;
            snnCfg_p->stepCfg_p[0].reqBody_setNum_p = new u_char[oneReqAPDUNum]();
            snnCfg_p->stepCfg_p[0].reqBody_setNum_p[0] = 0;
            //snnCfg_p->stepCfg_p[0].reqEleVal_pp =  new ELE_TYPE[oneReqAPDUNum][11]();
            snnCfg_p->stepCfg_p[0].reqData_p =  new dataFrame[oneReqAPDUNum]();
            u_char memVal[11] = {};
            CP56TIME2a tmStmp = {1};
            ELE_TYPE eleVal[11] =
            {
                {APDU_APCI_INDEX, 0, &memVal[0]},
                {ASDU_TID_INDEX, 1, &memVal[1]},
                {ASDU_LNG_INDEX, 0, &memVal[2]},
                {ASDU_VSQ_INDEX, 1, &memVal[3]},
                {ASDU_COT_INDEX, 1, &memVal[4]},
                {ASDU_ADDR_INDEX, sizeof ( u_short ), ( u_char* )& cmnAddr},
                {BODY_TID_INDEX, 0, &memVal[6]},
                {BODY_ADDR_INDEX, 1, &memVal[7]},
                {BODY_SET_INDEX, 0, &memVal[8]},
                {BODY_TSP_INDEX, sizeof ( CP56TIME2a ), ( u_char* )& tmStmp},
                {ASDU_TSP_INDEX, 0, &memVal[10]}
            };
            * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_103;
            * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_6;
            * ( eleVal[BODY_ADDR_INDEX].p_Mem ) = 0;
            //调用接口给snnCfg_p第一步请求帧的第1帧赋值
            setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
            //第一步响应
            snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

            ////第二步本地时间偏差
            //tmpStnAcsFlag.ctlType = 2;//[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
            //tmpStnAcsFlag.tspValidB = 1;
            //snnCfg_p->stepCfg_p[1].stnAcsFlag = *((u_short*)&tmpStnAcsFlag);
            ////第二步的请求帧
            //snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
            ////第二步的响应帧
            //u_char twoRespAPDUNum = 1;//第二步的响应帧数量
            //snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
            //snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
            //snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] = 0;//第二步响应第1帧元素集个数
            //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
            //snnCfg_p->stepCfg_p[1].respData = new dataFrame[twoRespAPDUNum]();
            //*(eleVal[ASDU_TID_INDEX].p_Mem) = MC_TID_103;
            //*(eleVal[ASDU_VSQ_INDEX].p_Mem) = MAX104_VSQ_N;
            //*(eleVal[ASDU_COT_INDEX].p_Mem) = MC_COT_7;
            //*(eleVal[BODY_ADDR_INDEX].p_Mem) = 0;
            //tmStmp.mScnd = 2;
            //eleVal[BODY_TSP_INDEX].p_Mem = (u_char*)&tmStmp;
            ////调用接口给snnCfg_p第二步响应的第1帧赋值
            //setOneFrmCfg(PASSIVE_TYPE,2,1,eleVal,snnCfg_p);

            //3、配置默认的步号与tid,cot的对应关系
            ptclFunDsc->ssnDscNo = srvNo;
            ptclFunDsc->act_prio_chnlID = ACTIVE_TYPE;
            ptclFunDsc->usingMode = usingMode;
            ptclFunDsc->setNum = stepNum;//stepNum=2
            ptclFunDsc->remoteSet_p = new ptclStep[stepNum]();
            ptclFunDsc->remoteSet_p[0].TID_p = new u_short();
            ptclFunDsc->remoteSet_p[0].TID_p[0] = MC_TID_103;
            ptclFunDsc->remoteSet_p[0].COT = MC_COT_6;
            ptclFunDsc->remoteSet_p[0].MaxLen = MAX104_ASDU_LNG;
            ptclFunDsc->remoteSet_p[0].TidNum = 1;
            /*ptclFunDsc->remoteSet_p[1].TID = MC_TID_103;
            ptclFunDsc->remoteSet_p[1].COT = MC_COT_7;
            ptclFunDsc->remoteSet_p[1].MaxLen = MAX104_ASDU_LNG;*/

            /*ONE_STEP* one_CRCT_BC_p      = new ONE_STEP();
            vrfItem* tsVerifyCRCTBC1_p   = new vrfItem();
            tsVerifyCRCTBC1_p->chkRslt_p = new int();
            setONE_STEP(one_CRCT_BC_p , 1 , I_F , 1 , NULL , I_F , 1 , NULL , tsVerifyCRCTBC1_p);
            Fun_p->steps_p[CRCT_BC] = one_CRCT_BC_p;*/
#endif//#ifdef INTER_CREATERL_VAL            

            break;
        }

        case CMD_TRNS://命令传输
        {
#ifndef INTER_CREATERL_VAL

            setFunRl ( srvNo,
                       usingMode,
                       I_F,
                       snnCfg_p,
                       Fun_p
                     );
#endif//#ifndef INTER_CREATERL_VAL

#ifdef INTER_CREATERL_VAL

            if ( usingMode != 0xFF ) //以入参为准
            {
                if ( usingMode != CMD_SE
                     && usingMode != CMD_DE
                     && usingMode != CMD_CNCL
                   )
                {
                    freeRl_P ( Fun_p, NULL, NULL );
                    return NULL;
                }
            }
            else//配置选一个可以的值
            {
                usingMode = CMD_DE;
            }

            ptclFunDsc->usingMode = usingMode;

            u_char seStepNum = 2;
            u_char deStepNum = 4;
            u_char cnclStepNum = 2;
            u_char defSetNum = 1;//默认信息元素个数

            Fun_p->srvNo             = CMD_TRNS;
            Fun_p->usingMode         =
            usingMode;//用哪种模式属两站双方范畴，协议自身支持这种动态特性
            Fun_p->cmLvl[CMD_SE]     = APP_LVL; //104规约取APP_LVL
            Fun_p->cmLvl[CMD_DE]     = APP_LVL; //104规约取APP_LVL
            Fun_p->cmLvl[CMD_CNCL]   = APP_LVL;
            Fun_p->stepNum[CMD_SE]   = seStepNum;//由选用的配套协议重新设置
            Fun_p->stepNum[CMD_DE]   = deStepNum;//由选用的配套协议重新设置
            Fun_p->stepNum[CMD_CNCL] = cnclStepNum;

            if ( usingMode == CMD_SE ) //组装CMD_SE模式
            {
                //1、对步规则Fun_p赋值
                ONE_STEP* one_CMD_SE_p      = new ONE_STEP();
                vrfItem* tsVerifyCMDSE1_p   = new vrfItem();
                tsVerifyCMDSE1_p->chkRslt_p = new int();
                setONE_STEP ( one_CMD_SE_p, 1, I_F, 1, NULL, I_F, 0, NULL, tsVerifyCMDSE1_p );
                ONE_STEP* two_CMD_SE_p      = new ONE_STEP();
                vrfItem* tsVerifyCMDSE2_p   = new vrfItem();
                tsVerifyCMDSE2_p->chkRslt_p = new int();
                setONE_STEP ( two_CMD_SE_p, 2, I_F, 0, NULL, I_F, 1, NULL, tsVerifyCMDSE2_p );
                addNode ( one_CMD_SE_p, two_CMD_SE_p );
                Fun_p->steps_p[CMD_SE] = one_CMD_SE_p;

                //2、对会话配置默认帧数据 T_snnCfg* stnSnnCfg_p
                u_short cmnAddr = 1;
                BODY_ADDR_VAL bodyAddrVal = {};
                bodyAddrVal.shortAddr = 4;

                snnCfg_p->ssnID = srvNo;
                //snnCfg_p->actFlag
                snnCfg_p->infoType = GINFOTYPE;
                snnCfg_p->cmnAddr = cmnAddr;
                snnCfg_p->stepNum = seStepNum;
                snnCfg_p->stepCfg_p = new T_ssnStepCfg[seStepNum]();
                //对第一步
                tmpStnAcsFlag.ctlType =
                0;//[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
                snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //第一步请求帧
                u_char oneReqAPDUNum = 1;//第一步的请求帧数量
                snnCfg_p->stepCfg_p[0].reqAPDUNum = oneReqAPDUNum;
                snnCfg_p->stepCfg_p[0].reqBody_setNum_p = new u_char[oneReqAPDUNum]();
                snnCfg_p->stepCfg_p[0].reqBody_setNum_p[0] = 0;
                //snnCfg_p->stepCfg_p[0].reqEleVal_pp =  new ELE_TYPE[oneReqAPDUNum][11]();
                snnCfg_p->stepCfg_p[0].reqData_p =  new dataFrame[oneReqAPDUNum]();
                u_char memVal[11] = {};
                struct T_SCO
                {
                    u_char SCS: 1;
                    u_char BS1: 1;
                    u_char QOC: 6;
                } sco;
                sco.SCS = 1;
                ELE_TYPE eleVal[11] =
                {
                    {APDU_APCI_INDEX, 0, &memVal[0]},
                    {ASDU_TID_INDEX, 1, &memVal[1]},
                    {ASDU_LNG_INDEX, 0, &memVal[2]},
                    {ASDU_VSQ_INDEX, 1, &memVal[3]},
                    {ASDU_COT_INDEX, 1, &memVal[4]},
                    {ASDU_ADDR_INDEX, sizeof ( u_short ), ( u_char* )& cmnAddr},
                    {BODY_TID_INDEX, 0, &memVal[6]},
                    {BODY_ADDR_INDEX, sizeof ( BODY_ADDR_VAL ), ( u_char* )& bodyAddrVal},
                    {BODY_SET_INDEX, 1, ( u_char* )& sco},
                    {BODY_TSP_INDEX, 0, &memVal[9]},
                    {ASDU_TSP_INDEX, 0, &memVal[10]}
                };
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_45;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_6;
                //调用接口给snnCfg_p第一步请求帧的第1帧赋值
                setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
                //第一步响应
                snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

                //第二步选择确认
                tmpStnAcsFlag.ctlType =
                0;//[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
                snnCfg_p->stepCfg_p[1].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //第二步的请求帧
                snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
                //第二步的响应帧
                u_char twoRespAPDUNum = 1;//第二步的响应帧数量
                snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
                snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
                snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
                defSetNum;//第二步响应第1帧元素集个数
                //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
                snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_45;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_7;
                //调用接口给snnCfg_p第二步响应的第1帧赋值
                setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

                //3、配置默认的步号与tid,cot的对应关系
                ptclFunDsc->ssnDscNo = srvNo;
                ptclFunDsc->act_prio_chnlID = ACTIVE_TYPE;
                ptclFunDsc->usingMode = usingMode;
                ptclFunDsc->setNum = seStepNum;//stepNum=2
                ptclFunDsc->remoteSet_p = new ptclStep[seStepNum]();
                ptclFunDsc->remoteSet_p[0].TID_p = new u_short();
                ptclFunDsc->remoteSet_p[0].TID_p[0] = MC_TID_45;
                ptclFunDsc->remoteSet_p[0].COT = MC_COT_6;
                ptclFunDsc->remoteSet_p[0].MaxLen = MAX104_ASDU_LNG;
                ptclFunDsc->remoteSet_p[0].TidNum = 1;

                ptclFunDsc->remoteSet_p[1].TID_p = new u_short();
                ptclFunDsc->remoteSet_p[1].TID_p[0] = MC_TID_45;
                ptclFunDsc->remoteSet_p[1].COT = MC_COT_7;
                ptclFunDsc->remoteSet_p[1].MaxLen = MAX104_ASDU_LNG;
                ptclFunDsc->remoteSet_p[1].TidNum = 1;
            }
            else if ( usingMode == CMD_DE ) //组装CMD_DE模式
            {
                //1、对步规则Fun_p赋值
                ONE_STEP* one_CMD_DE_p      = new ONE_STEP();
                vrfItem* tsVerifyCMDDE1_p   = new vrfItem();
                tsVerifyCMDDE1_p->chkRslt_p = new int();
                setONE_STEP ( one_CMD_DE_p, 1, I_F, 1, NULL, I_F, 0, NULL, tsVerifyCMDDE1_p );

                ONE_STEP* two_CMD_DE_con_p      = new ONE_STEP();
                vrfItem* tsVerifyCMDDE2_p   = new vrfItem();
                tsVerifyCMDDE2_p->chkRslt_p = new int();
                setONE_STEP ( two_CMD_DE_con_p, 2, I_F, 0, NULL, I_F, 1, NULL,
                              tsVerifyCMDDE2_p );
                addNode ( one_CMD_DE_p, two_CMD_DE_con_p );

                ONE_STEP* three_CMD_DE_p      = new ONE_STEP();
                vrfItem* tsVerifyCMDDE3_p   = new vrfItem();
                tsVerifyCMDDE3_p->chkRslt_p = new int();
                setONE_STEP ( three_CMD_DE_p, 3, I_F, 0, NULL, I_F, 1, NULL, tsVerifyCMDDE3_p );
                addNode ( one_CMD_DE_p, three_CMD_DE_p );

                ONE_STEP* four_CMD_DE_p      = new ONE_STEP();
                vrfItem* tsVerifyCMDDE4_p   = new vrfItem();
                tsVerifyCMDDE4_p->chkRslt_p = new int();
                setONE_STEP ( four_CMD_DE_p, 4, I_F, 0, NULL, I_F, 1, NULL, tsVerifyCMDDE4_p );
                addNode ( one_CMD_DE_p, four_CMD_DE_p );

                Fun_p->steps_p[CMD_DE] = one_CMD_DE_p;

                //2、对会话配置默认帧数据 T_snnCfg* stnSnnCfg_p
                u_short cmnAddr = 1;
                BODY_ADDR_VAL bodyAddrVal = {};
                bodyAddrVal.shortAddr = 4;

                snnCfg_p->ssnID = srvNo;
                //snnCfg_p->actFlag
                snnCfg_p->infoType = GINFOTYPE;
                snnCfg_p->cmnAddr = cmnAddr;
                snnCfg_p->stepNum = deStepNum;
                snnCfg_p->stepCfg_p = new T_ssnStepCfg[deStepNum]();
                //第一步执行命令
                tmpStnAcsFlag.ctlType =
                0;//[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
                snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //第一步请求帧
                u_char oneReqAPDUNum = 1;//第一步的请求帧数量
                snnCfg_p->stepCfg_p[0].reqAPDUNum = oneReqAPDUNum;
                snnCfg_p->stepCfg_p[0].reqBody_setNum_p = new u_char[oneReqAPDUNum]();
                snnCfg_p->stepCfg_p[0].reqBody_setNum_p[0] = 0;
                //snnCfg_p->stepCfg_p[0].reqEleVal_pp =  new ELE_TYPE[oneReqAPDUNum][11]();
                snnCfg_p->stepCfg_p[0].reqData_p =  new dataFrame[oneReqAPDUNum]();
                u_char memVal[11] = {};
                struct T_SCO
                {
                    u_char SCS: 1;
                    u_char BS1: 1;
                    u_char QOC: 6;
                } sco;
                sco.SCS = 1;
                ELE_TYPE eleVal[11] =
                {
                    {APDU_APCI_INDEX, 0, &memVal[0]},
                    {ASDU_TID_INDEX, 1, &memVal[1]},
                    {ASDU_LNG_INDEX, 0, &memVal[2]},
                    {ASDU_VSQ_INDEX, 1, &memVal[3]},
                    {ASDU_COT_INDEX, 1, &memVal[4]},
                    {ASDU_ADDR_INDEX, sizeof ( u_short ), ( u_char* )& cmnAddr},
                    {BODY_TID_INDEX, 0, &memVal[6]},
                    {BODY_ADDR_INDEX, sizeof ( BODY_ADDR_VAL ), ( u_char* )& bodyAddrVal},
                    {BODY_SET_INDEX, 1, ( u_char* )& sco},
                    {BODY_TSP_INDEX, 0, &memVal[9]},
                    {ASDU_TSP_INDEX, 0, &memVal[10]}
                };
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_45;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_6;
                //调用接口给snnCfg_p第一步请求帧的第1帧赋值
                setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
                //第一步响应
                snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

                //第二步直接命令确认
                tmpStnAcsFlag.ctlType =
                0;//[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
                snnCfg_p->stepCfg_p[1].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //第二步的请求帧
                snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
                //第二步的响应帧
                u_char twoRespAPDUNum = 1;//第二步的响应帧数量
                snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
                snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
                snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
                defSetNum;//第二步响应第1帧元素集个数
                //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
                snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_45;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_7;
                //调用接口给snnCfg_p第二步响应的第1帧赋值
                setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

                //第三步控制操作完成
                struct T_SIQ
                {
                    u_char SPI: 1;
                    u_char RES: 3;
                    u_char BL: 1;
                    u_char SB: 1;
                    u_char NT: 1;
                    u_char IV: 1;
                } siq = {1, 0, 0};
                tmpStnAcsFlag.ctlType =
                2;//[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
                tmpStnAcsFlag.setValid = 1;
                snnCfg_p->stepCfg_p[2].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //第三步的请求帧
                snnCfg_p->stepCfg_p[2].reqAPDUNum = 0;
                //第三步的响应帧
                u_char threeRespAPDUNum = 1;//第三步的响应帧数量
                snnCfg_p->stepCfg_p[2].respAPDUNum = threeRespAPDUNum;
                snnCfg_p->stepCfg_p[2].respBody_setNum_p = new u_char[threeRespAPDUNum]();
                snnCfg_p->stepCfg_p[2].respBody_setNum_p[0] =
                defSetNum;//第三步响应第1帧元素集个数
                //snnCfg_p->stepCfg_p[2].respEleVal_pp = new ELE_TYPE[threeRespAPDUNum][11]();
                snnCfg_p->stepCfg_p[2].respData_p = new dataFrame[threeRespAPDUNum]();
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_1;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_11;
                eleVal[BODY_SET_INDEX].p_Mem    = ( u_char* ) &siq;
                //调用接口给snnCfg_p第三步控制操作完成的第1帧赋值
                setOneFrmCfg ( PASSIVE_TYPE, 3, 1, eleVal, snnCfg_p );

                //第四步控制操作结束
                tmpStnAcsFlag.ctlType =
                0;//[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
                snnCfg_p->stepCfg_p[3].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //第四步的请求帧
                snnCfg_p->stepCfg_p[3].reqAPDUNum = 0;
                //第四步的响应帧
                u_char fourRespAPDUNum = 1;//第四步的响应帧数量
                snnCfg_p->stepCfg_p[3].respAPDUNum = fourRespAPDUNum;
                snnCfg_p->stepCfg_p[3].respBody_setNum_p = new u_char[fourRespAPDUNum]();
                snnCfg_p->stepCfg_p[3].respBody_setNum_p[0] =
                defSetNum;//第四步响应第1帧元素集个数
                //snnCfg_p->stepCfg_p[3].respEleVal_pp = new ELE_TYPE[fourRespAPDUNum][11]();
                snnCfg_p->stepCfg_p[3].respData_p = new dataFrame[fourRespAPDUNum]();
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_45;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_10;
                eleVal[BODY_SET_INDEX].p_Mem    = ( u_char* ) &sco;
                //调用接口给snnCfg_p第四步控制操作结束的第1帧赋值
                setOneFrmCfg ( PASSIVE_TYPE, 4, 1, eleVal, snnCfg_p );

                //3、配置默认的步号与tid,cot的对应关系
                ptclFunDsc->ssnDscNo = srvNo;
                ptclFunDsc->act_prio_chnlID = ACTIVE_TYPE;
                ptclFunDsc->usingMode = usingMode;
                ptclFunDsc->setNum = deStepNum;//stepNum=4
                ptclFunDsc->remoteSet_p = new ptclStep[deStepNum]();
                ptclFunDsc->remoteSet_p[0].TID_p = new u_short();
                ptclFunDsc->remoteSet_p[0].TID_p[0] = MC_TID_45;
                ptclFunDsc->remoteSet_p[0].COT = MC_COT_6;
                ptclFunDsc->remoteSet_p[0].MaxLen = MAX104_ASDU_LNG;
                ptclFunDsc->remoteSet_p[0].TidNum = 1;

                ptclFunDsc->remoteSet_p[1].TID_p = new u_short();
                ptclFunDsc->remoteSet_p[1].TID_p[0] = MC_TID_45;
                ptclFunDsc->remoteSet_p[1].COT = MC_COT_7;
                ptclFunDsc->remoteSet_p[1].MaxLen = MAX104_ASDU_LNG;
                ptclFunDsc->remoteSet_p[1].TidNum = 1;

                ptclFunDsc->remoteSet_p[2].TID_p = new u_short();
                ptclFunDsc->remoteSet_p[2].TID_p[0] = MC_TID_1;
                ptclFunDsc->remoteSet_p[2].COT = MC_COT_11;
                ptclFunDsc->remoteSet_p[2].MaxLen = MAX104_ASDU_LNG;
                ptclFunDsc->remoteSet_p[2].TidNum = 1;

                ptclFunDsc->remoteSet_p[3].TID_p = new u_short();
                ptclFunDsc->remoteSet_p[3].TID_p[0] = MC_TID_45;
                ptclFunDsc->remoteSet_p[3].COT = MC_COT_10;
                ptclFunDsc->remoteSet_p[3].MaxLen = MAX104_ASDU_LNG;
                ptclFunDsc->remoteSet_p[3].TidNum = 1;
            }
            else if ( usingMode == CMD_CNCL ) //组装CMD_CNCL模式
            {
                //1、对步规则Fun_p赋值
                ONE_STEP* one_CMD_CNCL_p     = new ONE_STEP();
                vrfItem* tsVerifyCMDCNCL_p   = new vrfItem();
                tsVerifyCMDCNCL_p->chkRslt_p = new int();
                setONE_STEP ( one_CMD_CNCL_p, 1, I_F, 1, NULL, I_F, 0, NULL,
                              tsVerifyCMDCNCL_p );
                ONE_STEP* two_CMD_CNCL_p     = new ONE_STEP();
                vrfItem* tsVerifyCMDCNCL2_p   = new vrfItem();
                tsVerifyCMDCNCL2_p->chkRslt_p = new int();
                setONE_STEP ( two_CMD_CNCL_p, 2, I_F, 0, NULL, I_F, 1, NULL,
                              tsVerifyCMDCNCL2_p );
                addNode ( one_CMD_CNCL_p, two_CMD_CNCL_p );
                Fun_p->steps_p[CMD_CNCL] = one_CMD_CNCL_p;

                //2、对会话配置默认帧数据 T_snnCfg* stnSnnCfg_p
                u_short cmnAddr = 1;
                BODY_ADDR_VAL bodyAddrVal = {};
                bodyAddrVal.shortAddr = 4;

                snnCfg_p->ssnID = srvNo;
                //snnCfg_p->actFlag
                snnCfg_p->infoType = GINFOTYPE;
                snnCfg_p->cmnAddr = cmnAddr;
                snnCfg_p->stepNum = cnclStepNum;
                snnCfg_p->stepCfg_p = new T_ssnStepCfg[cnclStepNum]();
                //第一步撤消命令
                tmpStnAcsFlag.ctlType =
                0;//[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
                snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //第一步请求帧
                u_char oneReqAPDUNum = 1;//第一步的请求帧数量
                snnCfg_p->stepCfg_p[0].reqAPDUNum = oneReqAPDUNum;
                snnCfg_p->stepCfg_p[0].reqBody_setNum_p = new u_char[oneReqAPDUNum]();
                snnCfg_p->stepCfg_p[0].reqBody_setNum_p[0] = 0;
                //snnCfg_p->stepCfg_p[0].reqEleVal_pp =  new ELE_TYPE[oneReqAPDUNum][11]();
                snnCfg_p->stepCfg_p[0].reqData_p =  new dataFrame[oneReqAPDUNum]();
                u_char memVal[11] = {};
                struct T_SCO
                {
                    u_char SCS: 1;
                    u_char BS1: 1;
                    u_char QOC: 6;
                } sco;
                sco.SCS = 1;
                ELE_TYPE eleVal[11] =
                {
                    {APDU_APCI_INDEX, 0, &memVal[0]},
                    {ASDU_TID_INDEX, 1, &memVal[1]},
                    {ASDU_LNG_INDEX, 0, &memVal[2]},
                    {ASDU_VSQ_INDEX, 1, &memVal[3]},
                    {ASDU_COT_INDEX, 1, &memVal[4]},
                    {ASDU_ADDR_INDEX, sizeof ( u_short ), ( u_char* )& cmnAddr},
                    {BODY_TID_INDEX, 0, &memVal[6]},
                    {BODY_ADDR_INDEX, sizeof ( BODY_ADDR_VAL ), ( u_char* )& bodyAddrVal},
                    {BODY_SET_INDEX, 1, ( u_char* )& sco},
                    {BODY_TSP_INDEX, 0, &memVal[9]},
                    {ASDU_TSP_INDEX, 0, &memVal[10]}
                };
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_45;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_8;
                //调用接口给snnCfg_p第一步请求帧的第1帧赋值
                setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
                //第一步响应
                snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

                //第二步撤消确认
                tmpStnAcsFlag.ctlType =
                0;//[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
                snnCfg_p->stepCfg_p[1].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //第二步的请求帧
                snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
                //第二步的响应帧
                u_char twoRespAPDUNum = 1;//第二步的响应帧数量
                snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
                snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
                snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
                defSetNum;//第二步响应第1帧元素集个数
                //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
                snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_45;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_9;
                //调用接口给snnCfg_p第二步响应的第1帧赋值
                setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

                //3、配置默认的步号与tid,cot的对应关系
                ptclFunDsc->ssnDscNo = srvNo;
                ptclFunDsc->act_prio_chnlID = ACTIVE_TYPE;
                ptclFunDsc->usingMode = usingMode;
                ptclFunDsc->setNum = cnclStepNum;//stepNum=2
                ptclFunDsc->remoteSet_p = new ptclStep[cnclStepNum]();
                ptclFunDsc->remoteSet_p[0].TID_p = new u_short();
                ptclFunDsc->remoteSet_p[0].TID_p[0] = MC_TID_45;
                ptclFunDsc->remoteSet_p[0].COT = MC_COT_8;
                ptclFunDsc->remoteSet_p[0].MaxLen = MAX104_ASDU_LNG;
                ptclFunDsc->remoteSet_p[0].TidNum = 1;

                ptclFunDsc->remoteSet_p[1].TID_p = new u_short();
                ptclFunDsc->remoteSet_p[1].TID_p[0] = MC_TID_45;
                ptclFunDsc->remoteSet_p[1].COT = MC_COT_9;
                ptclFunDsc->remoteSet_p[1].MaxLen = MAX104_ASDU_LNG;
                ptclFunDsc->remoteSet_p[1].TidNum = 1;


            }

#endif//#ifdef INTER_CREATERL_VAL

            break;
        }

        case ACMU_FRZ: //累积量传输前的冻结 此功能是从ACMU_TRNS功能的模式2，控制站先冻结再请求传输分离出来的
        {
#ifndef INTER_CREATERL_VAL

            setFunRl ( srvNo,
                       usingMode,
                       I_F,
                       snnCfg_p,
                       Fun_p
                     );
#endif//#ifndef INTER_CREATERL_VAL

#ifdef INTER_CREATERL_VAL

            if ( usingMode != 0xFF ) //以入参为准
            {
                if ( usingMode != ONLY_ONE )
                {
                    freeRl_P ( Fun_p, NULL, NULL );
                    return NULL;
                }
            }
            else//配置选一个可以的值
            {
                usingMode = ONLY_ONE;
            }

            ptclFunDsc->usingMode = usingMode;

            u_char stepNum = 2;

            //1、对步规则Fun_p赋值
            Fun_p->srvNo             = ACMU_FRZ;
            Fun_p->usingMode         = ONLY_ONE;
            Fun_p->cmLvl[ONLY_ONE]   = APP_LVL; //104规约取APP_LVL
            Fun_p->stepNum[ONLY_ONE] = stepNum;

            ONE_STEP* one_step_p        = new ONE_STEP();
            vrfItem* tsVerifyBlock1_p   = new vrfItem();
            tsVerifyBlock1_p->chkRslt_p = new int();
            setONE_STEP ( one_step_p, 1, I_F, 1, NULL, I_F, 0, NULL, tsVerifyBlock1_p );

            ONE_STEP* two_step_p        = new ONE_STEP();
            vrfItem* tsVerifyBlock2_p   = new vrfItem();
            tsVerifyBlock2_p->chkRslt_p = new int();
            setONE_STEP ( two_step_p, 2, I_F, 0, NULL, I_F, 1, NULL, tsVerifyBlock2_p );
            addNode ( one_step_p, two_step_p );

            Fun_p->steps_p[ONLY_ONE] = one_step_p;

            //2、对会话配置默认帧数据 T_snnCfg* stnSnnCfg_p
            u_short cmnAddr = 1;
            BODY_ADDR_VAL bodyAddrVal = {};
            bodyAddrVal.shortAddr = 0;
            u_char defSetNum = 1;//默认信息元素个数

            struct T_QCC
            {
                u_char RQT: 6;
                u_char FRZ: 2;
            } qcc = {0, 1};

            snnCfg_p->ssnID = srvNo;
            //snnCfg_p->actFlag
            snnCfg_p->infoType = GINFOTYPE;
            snnCfg_p->cmnAddr = cmnAddr;
            snnCfg_p->stepNum = stepNum;
            snnCfg_p->stepCfg_p = new T_ssnStepCfg[stepNum]();
            //第一步存储计数量命令
            tmpStnAcsFlag.ctlType =
            0;//[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
            snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
            //第一步请求帧
            u_char oneReqAPDUNum = 1;//第一步的请求帧数量
            snnCfg_p->stepCfg_p[0].reqAPDUNum = oneReqAPDUNum;
            snnCfg_p->stepCfg_p[0].reqBody_setNum_p = new u_char[oneReqAPDUNum]();
            snnCfg_p->stepCfg_p[0].reqBody_setNum_p[0] = 0;
            //snnCfg_p->stepCfg_p[0].reqEleVal_pp =  new ELE_TYPE[oneReqAPDUNum][11]();
            snnCfg_p->stepCfg_p[0].reqData_p =  new dataFrame[oneReqAPDUNum]();
            u_char memVal[11] = {};
            ELE_TYPE eleVal[11] =
            {
                {APDU_APCI_INDEX, 0, &memVal[0]},
                {ASDU_TID_INDEX, 1, &memVal[1]},
                {ASDU_LNG_INDEX, 0, &memVal[2]},
                {ASDU_VSQ_INDEX, 1, &memVal[3]},
                {ASDU_COT_INDEX, 1, &memVal[4]},
                {ASDU_ADDR_INDEX, sizeof ( u_short ), ( u_char* )& cmnAddr},
                {BODY_TID_INDEX, 0, &memVal[6]},
                {BODY_ADDR_INDEX, 1, &memVal[7]},
                {BODY_SET_INDEX, sizeof ( qcc ), ( u_char* )& qcc},
                {BODY_TSP_INDEX, 0, &memVal[9]},
                {ASDU_TSP_INDEX, 0, &memVal[10]}
            };
            * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_101;
            * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_6;
            * ( eleVal[BODY_ADDR_INDEX].p_Mem ) = 0;
            //调用接口给snnCfg_p第一步请求帧的第1帧赋值
            setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
            //第一步响应
            snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

            //第二步命令确认
            tmpStnAcsFlag.ctlType =
            0;//[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
            snnCfg_p->stepCfg_p[1].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
            //第二步的请求帧
            snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
            //第二步的响应帧
            u_char twoRespAPDUNum = 1;//第二步的响应帧数量
            snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
            snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
            snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
            defSetNum;//第二步响应第1帧元素集个数
            //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
            snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
            * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_101;
            * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_7;
            //调用接口给snnCfg_p第二步响应的第1帧赋值
            setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

            //3、配置默认的步号与tid,cot的对应关系
            ptclFunDsc->ssnDscNo = srvNo;
            ptclFunDsc->act_prio_chnlID = ACTIVE_TYPE;
            ptclFunDsc->usingMode = usingMode;
            ptclFunDsc->setNum = stepNum;//stepNum=2
            ptclFunDsc->remoteSet_p = new ptclStep[stepNum]();
            ptclFunDsc->remoteSet_p[0].TID_p = new u_short();
            ptclFunDsc->remoteSet_p[0].TID_p[0] = MC_TID_101;
            ptclFunDsc->remoteSet_p[0].COT = MC_COT_6;
            ptclFunDsc->remoteSet_p[0].MaxLen = MAX104_ASDU_LNG;
            ptclFunDsc->remoteSet_p[0].TidNum = 1;

            ptclFunDsc->remoteSet_p[1].TID_p = new u_short();
            ptclFunDsc->remoteSet_p[1].TID_p[0] = MC_TID_101;
            ptclFunDsc->remoteSet_p[1].COT = MC_COT_7;
            ptclFunDsc->remoteSet_p[1].MaxLen = MAX104_ASDU_LNG;
            ptclFunDsc->remoteSet_p[1].TidNum = 1;
#endif//#ifdef INTER_CREATERL_VAL

            break;
        }

        case ACMU_TRNS://累积量传输
        {
#ifndef INTER_CREATERL_VAL

            setFunRl ( srvNo,
                       usingMode,
                       I_F,
                       snnCfg_p,
                       Fun_p
                     );
#endif//#ifndef INTER_CREATERL_VAL

#ifdef INTER_CREATERL_VAL

            if ( usingMode != 0xFF ) //以入参为准
            {
                if ( usingMode != CTLD_DRCTSONPT_TRNS
                     && usingMode != CTL_DRCTREQ_TRNS
                     && usingMode != CTL_FRZ_CTLD_SONPT_TRNS
                   )
                {
                    freeRl_P ( Fun_p, NULL, NULL );
                    return NULL;
                }
            }
            else//配置选一个可以的值
            {
                usingMode = CTL_DRCTREQ_TRNS;
            }

            ptclFunDsc->usingMode = usingMode;

            u_char sonptStepNum = 1;
            u_char drReqStepNum = 4;
            u_char ctlFrzSonptNum = 3;
            //1、对步规则Fun_p赋值
            Fun_p->srvNo                            = ACMU_TRNS;
            Fun_p->usingMode                        = UNKNOWN;
            Fun_p->cmLvl[CTLD_DRCTSONPT_TRNS]       =
            APP_LVL;// 101规约取APP_LVL，104规约无配置情况下不支持累计量的突发传输
            Fun_p->cmLvl[CTL_DRCTREQ_TRNS]          = APP_LVL;
            Fun_p->cmLvl[CTL_FRZREQ_TRNS]           = APP_LVL; // 104规约取APP_LVL
            Fun_p->cmLvl[CTL_FRZ_CTLD_SONPT_TRNS]   = APP_LVL;
            Fun_p->stepNum[CTLD_DRCTSONPT_TRNS]     = sonptStepNum;
            Fun_p->stepNum[CTL_DRCTREQ_TRNS]        = drReqStepNum;
            Fun_p->stepNum[CTL_FRZREQ_TRNS]         =
            0;//此各模式不存在了，因此种模式=新的会话ACMU_FRZ + CTL_DRCTREQ_TRNS 模式替代了
            Fun_p->stepNum[CTL_FRZ_CTLD_SONPT_TRNS] = ctlFrzSonptNum;

            u_char defSetNum = 1;//默认信息元素个数

            if ( usingMode == CTLD_DRCTSONPT_TRNS )
            {
                //组装CTLD_DRCTSONPT_TRNS模式
                ONE_STEP* one_DRCTSONPT_p       = new ONE_STEP();
                vrfItem* tsVerifyDRCTSONPT1_p   = new vrfItem();
                tsVerifyDRCTSONPT1_p->chkRslt_p = new int();
                setONE_STEP ( one_DRCTSONPT_p, 1, I_F, 0, NULL, I_F, 1, NULL,
                              tsVerifyDRCTSONPT1_p );
                Fun_p->steps_p[CTLD_DRCTSONPT_TRNS] = one_DRCTSONPT_p;

                //2、对会话配置默认帧数据 T_snnCfg* stnSnnCfg_p
                u_short cmnAddr = 1;
                BODY_ADDR_VAL bodyAddrVal = {};
                bodyAddrVal.shortAddr = 4;
                int bdSet = 23;

                snnCfg_p->ssnID = srvNo;
                //snnCfg_p->actFlag
                snnCfg_p->infoType = GINFOTYPE;
                snnCfg_p->cmnAddr = cmnAddr;
                snnCfg_p->stepNum = sonptStepNum;
                snnCfg_p->stepCfg_p = new T_ssnStepCfg[sonptStepNum]();
                //对第一步突发传输
                tmpStnAcsFlag.ctlType =
                2;//[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
                tmpStnAcsFlag.setValid = 1;
                snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //第一步的请求帧
                snnCfg_p->stepCfg_p[0].reqAPDUNum = 0;
                //第一步的响应帧
                u_char oneRespAPDUNum = 1;//第一步的响应帧数量
                snnCfg_p->stepCfg_p[0].respAPDUNum = oneRespAPDUNum;
                snnCfg_p->stepCfg_p[0].respBody_setNum_p = new u_char[oneRespAPDUNum]();
                snnCfg_p->stepCfg_p[0].respBody_setNum_p[0] =
                defSetNum;//第一步响应第1帧元素集个数
                //snnCfg_p->stepCfg_p[0].respEleVal_pp = new ELE_TYPE[oneRespAPDUNum][11]();
                snnCfg_p->stepCfg_p[0].respData_p = new dataFrame[oneRespAPDUNum]();
                u_char memVal[11] = {};
                ELE_TYPE eleVal[11] =
                {
                    {APDU_APCI_INDEX, 0, &memVal[0]},
                    {ASDU_TID_INDEX, 1, &memVal[1]},
                    {ASDU_LNG_INDEX, 0, &memVal[2]},
                    {ASDU_VSQ_INDEX, 1, &memVal[3]},
                    {ASDU_COT_INDEX, 1, &memVal[4]},
                    {ASDU_ADDR_INDEX, sizeof ( u_short ), ( u_char* )& cmnAddr},
                    {BODY_TID_INDEX, 0, &memVal[6]},
                    {BODY_ADDR_INDEX, sizeof ( BODY_ADDR_VAL ), ( u_char* )& bodyAddrVal},
                    {BODY_SET_INDEX, sizeof ( int ), ( u_char* )& bdSet},
                    {BODY_TSP_INDEX, 0, &memVal[9]},
                    {ASDU_TSP_INDEX, 0, &memVal[10]}
                };
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_37;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_3;
                //调用接口给snnCfg_p第一步突发的第1帧赋值
                setOneFrmCfg ( PASSIVE_TYPE, 1, 1, eleVal, snnCfg_p );

                //3、配置默认的步号与tid,cot的对应关系
                ptclFunDsc->ssnDscNo = srvNo;
                ptclFunDsc->act_prio_chnlID = PASSIVE_TYPE;
                ptclFunDsc->usingMode = usingMode;
                ptclFunDsc->setNum = sonptStepNum;//stepNum=1
                ptclFunDsc->remoteSet_p = new ptclStep[sonptStepNum]();
                ptclFunDsc->remoteSet_p[0].TID_p = new u_short();
                ptclFunDsc->remoteSet_p[0].TID_p[0] = MC_TID_37;
                ptclFunDsc->remoteSet_p[0].COT = MC_COT_3;
                ptclFunDsc->remoteSet_p[0].MaxLen = MAX104_ASDU_LNG;
                ptclFunDsc->remoteSet_p[0].TidNum = 1;

            }
            else if ( usingMode == CTL_DRCTREQ_TRNS )
            {
                //1、对步规则Fun_p赋值
                //组装CTL_DRCTREQ_TRNS模式
                ONE_STEP* one_DRCTREQ_p       = new ONE_STEP();
                vrfItem* tsVerifyDRCTREQ1_p   = new vrfItem();
                tsVerifyDRCTREQ1_p->chkRslt_p = new int();
                setONE_STEP ( one_DRCTREQ_p, 1, I_F, 1, NULL, I_F, 0, NULL,
                              tsVerifyDRCTREQ1_p );

                ONE_STEP* two_DRCTREQ_p       = new ONE_STEP();
                vrfItem* tsVerifyDRCTREQ2_p   = new vrfItem();
                tsVerifyDRCTREQ2_p->chkRslt_p = new int();
                setONE_STEP ( two_DRCTREQ_p, 2, I_F, 0, NULL, I_F, 1, NULL,
                              tsVerifyDRCTREQ2_p );
                addNode ( one_DRCTREQ_p, two_DRCTREQ_p );

                ONE_STEP* three_DRCTREQ_p       = new ONE_STEP();
                vrfItem* tsVerifyDRCTREQ3_p   = new vrfItem();
                tsVerifyDRCTREQ3_p->chkRslt_p = new int();
                setONE_STEP ( three_DRCTREQ_p, 3, I_F, 0, NULL, I_F, 1, NULL,
                              tsVerifyDRCTREQ3_p );
                addNode ( one_DRCTREQ_p, three_DRCTREQ_p );

                ONE_STEP* four_DRCTREQ_p       = new ONE_STEP();
                vrfItem* tsVerifyDRCTREQ4_p   = new vrfItem();
                tsVerifyDRCTREQ4_p->chkRslt_p = new int();
                setONE_STEP ( four_DRCTREQ_p, 4, I_F, 0, NULL, I_F, 1, NULL,
                              tsVerifyDRCTREQ4_p );
                addNode ( one_DRCTREQ_p, four_DRCTREQ_p );

                Fun_p->steps_p[CTL_DRCTREQ_TRNS] = one_DRCTREQ_p;

                //2、对会话配置默认帧数据 T_snnCfg* stnSnnCfg_p
                u_short cmnAddr = 1;
                BODY_ADDR_VAL bodyAddrVal = {};
                bodyAddrVal.shortAddr = 4;

                struct T_QCC
                {
                    u_char RQT: 6;
                    u_char FRZ: 2;
                } qcc = {5, 0};

                snnCfg_p->ssnID = srvNo;
                //snnCfg_p->actFlag
                snnCfg_p->infoType = GINFOTYPE;
                snnCfg_p->cmnAddr = cmnAddr;
                snnCfg_p->stepNum = drReqStepNum;
                snnCfg_p->stepCfg_p = new T_ssnStepCfg[drReqStepNum]();
                //对第一步请求计数量命令
                tmpStnAcsFlag.ctlType =
                0;//[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
                snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //第一步请求帧
                u_char oneReqAPDUNum = 1;//第一步的请求帧数量
                snnCfg_p->stepCfg_p[0].reqAPDUNum = oneReqAPDUNum;
                snnCfg_p->stepCfg_p[0].reqBody_setNum_p = new u_char[oneReqAPDUNum]();
                snnCfg_p->stepCfg_p[0].reqBody_setNum_p[0] = 0;
                //snnCfg_p->stepCfg_p[0].reqEleVal_pp =  new ELE_TYPE[oneReqAPDUNum][11]();
                snnCfg_p->stepCfg_p[0].reqData_p =  new dataFrame[oneReqAPDUNum]();
                u_char memVal[11] = {};
                ELE_TYPE eleVal[11] =
                {
                    {APDU_APCI_INDEX, 0, &memVal[0]},
                    {ASDU_TID_INDEX, 1, &memVal[1]},
                    {ASDU_LNG_INDEX, 0, &memVal[2]},
                    {ASDU_VSQ_INDEX, 1, &memVal[3]},
                    {ASDU_COT_INDEX, 1, &memVal[4]},
                    {ASDU_ADDR_INDEX, sizeof ( u_short ), ( u_char* )& cmnAddr},
                    {BODY_TID_INDEX, 0, &memVal[6]},
                    {BODY_ADDR_INDEX, 1, &memVal[7]},
                    {BODY_SET_INDEX, sizeof ( qcc ), ( u_char* )& qcc},
                    {BODY_TSP_INDEX, 0, &memVal[9]},
                    {ASDU_TSP_INDEX, 0, &memVal[10]}
                };
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_101;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_6;
                * ( eleVal[BODY_ADDR_INDEX].p_Mem ) = 0;
                //调用接口给snnCfg_p第一步请求帧的第1帧赋值
                setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
                //第一步响应
                snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

                //第二步请求计数量命令确认
                tmpStnAcsFlag.ctlType =
                0;//[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
                snnCfg_p->stepCfg_p[1].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //第二步的请求帧
                snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
                //第二步的响应帧
                u_char twoRespAPDUNum = 1;//第二步的响应帧数量
                snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
                snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
                snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
                defSetNum;//第二步响应第1帧元素集个数
                //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
                snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_101;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_7;
                //调用接口给snnCfg_p第二步响应的第1帧赋值
                setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

                //第三步采集的计数量
                tmpStnAcsFlag.ctlType =
                2;//[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
                tmpStnAcsFlag.setValid = 1;
                snnCfg_p->stepCfg_p[2].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //第三步的请求帧
                snnCfg_p->stepCfg_p[2].reqAPDUNum = 0;
                //第三步的响应帧
                u_char oneRespAPDUNum = 1;//第三步的响应帧数量
                snnCfg_p->stepCfg_p[2].respAPDUNum = oneRespAPDUNum;
                snnCfg_p->stepCfg_p[2].respBody_setNum_p = new u_char[oneRespAPDUNum]();
                snnCfg_p->stepCfg_p[2].respBody_setNum_p[0] =
                1;//第三步响应第1帧元素集个数
                //snnCfg_p->stepCfg_p[2].respEleVal_pp = new ELE_TYPE[oneRespAPDUNum][11]();
                snnCfg_p->stepCfg_p[2].respData_p = new dataFrame[oneRespAPDUNum]();
                int bdSet = 22;

                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_15;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = 1 << 7 | MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_37;
                eleVal[BODY_ADDR_INDEX].lng = sizeof ( BODY_ADDR_VAL );
                eleVal[BODY_ADDR_INDEX].p_Mem = ( u_char* ) &bodyAddrVal;
                eleVal[BODY_SET_INDEX].lng = sizeof ( int );
                eleVal[BODY_SET_INDEX].p_Mem = ( u_char* ) &bdSet;
                //调用接口给snnCfg_p第三步突发的第1帧赋值
                setOneFrmCfg ( PASSIVE_TYPE, 3, 1, eleVal, snnCfg_p );

                //第四步请求计数量结束
                tmpStnAcsFlag.ctlType =
                0;//[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
                snnCfg_p->stepCfg_p[3].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //第四步的请求帧
                snnCfg_p->stepCfg_p[3].reqAPDUNum = 0;
                //第四步的响应帧
                u_char fourRespAPDUNum = 1;//第四步的响应帧数量
                snnCfg_p->stepCfg_p[3].respAPDUNum = fourRespAPDUNum;
                snnCfg_p->stepCfg_p[3].respBody_setNum_p = new u_char[fourRespAPDUNum]();
                snnCfg_p->stepCfg_p[3].respBody_setNum_p[0] =
                defSetNum;//第四步响应第1帧元素集个数
                //snnCfg_p->stepCfg_p[3].respEleVal_pp = new ELE_TYPE[fourRespAPDUNum][11]();
                snnCfg_p->stepCfg_p[3].respData_p = new dataFrame[fourRespAPDUNum]();
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_101;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_10;
                eleVal[BODY_ADDR_INDEX].lng = 1;
                * ( eleVal[BODY_ADDR_INDEX].p_Mem ) = 0;
                eleVal[BODY_SET_INDEX].lng = sizeof ( qcc );
                eleVal[BODY_SET_INDEX].p_Mem = ( u_char* ) &qcc;
                //调用接口给snnCfg_p第四步响应的第1帧赋值
                setOneFrmCfg ( PASSIVE_TYPE, 4, 1, eleVal, snnCfg_p );

                //3、配置默认的步号与tid,cot的对应关系
                ptclFunDsc->ssnDscNo = srvNo;
                ptclFunDsc->act_prio_chnlID = ACTIVE_TYPE;
                ptclFunDsc->usingMode = usingMode;
                ptclFunDsc->setNum = drReqStepNum;//stepNum=3
                ptclFunDsc->remoteSet_p = new ptclStep[drReqStepNum]();
                ptclFunDsc->remoteSet_p[0].TID_p = new u_short();
                ptclFunDsc->remoteSet_p[0].TID_p[0] = MC_TID_101;
                ptclFunDsc->remoteSet_p[0].COT = MC_COT_6;
                ptclFunDsc->remoteSet_p[0].MaxLen = MAX104_ASDU_LNG;
                ptclFunDsc->remoteSet_p[0].TidNum = 1;

                ptclFunDsc->remoteSet_p[1].TID_p = new u_short();
                ptclFunDsc->remoteSet_p[1].TID_p[0] = MC_TID_101;
                ptclFunDsc->remoteSet_p[1].COT = MC_COT_7;
                ptclFunDsc->remoteSet_p[1].MaxLen = MAX104_ASDU_LNG;
                ptclFunDsc->remoteSet_p[1].TidNum = 1;

                ptclFunDsc->remoteSet_p[2].TID_p = new u_short();
                ptclFunDsc->remoteSet_p[2].TID_p[0] = MC_TID_15;
                ptclFunDsc->remoteSet_p[2].COT = MC_COT_37;
                ptclFunDsc->remoteSet_p[2].MaxLen = MAX104_ASDU_LNG;
                ptclFunDsc->remoteSet_p[2].TidNum = 1;

                ptclFunDsc->remoteSet_p[3].TID_p = new u_short();
                ptclFunDsc->remoteSet_p[3].TID_p[0] = MC_TID_101;
                ptclFunDsc->remoteSet_p[3].COT = MC_COT_10;
                ptclFunDsc->remoteSet_p[3].MaxLen = MAX104_ASDU_LNG;
                ptclFunDsc->remoteSet_p[3].TidNum = 1;


            }
            else if ( usingMode == CTL_FRZ_CTLD_SONPT_TRNS )
            {
                //组装CTL_FRZ_CTLD_SONPT_TRNS模式
                ONE_STEP* one_FRZ_CTLD_p      = new ONE_STEP();
                vrfItem* tsVerifyFRZCTLD1_p   = new vrfItem();
                tsVerifyFRZCTLD1_p->chkRslt_p = new int();
                setONE_STEP ( one_FRZ_CTLD_p, 1, I_F, 1, NULL, I_F, 0, NULL,
                              tsVerifyFRZCTLD1_p );

                ONE_STEP* two_FRZ_CTLD_p      = new ONE_STEP();
                vrfItem* tsVerifyFRZCTLD2_p   = new vrfItem();
                tsVerifyFRZCTLD2_p->chkRslt_p = new int();
                setONE_STEP ( two_FRZ_CTLD_p, 2, I_F, 0, NULL, I_F, 1, NULL,
                              tsVerifyFRZCTLD2_p );
                addNode ( one_FRZ_CTLD_p, two_FRZ_CTLD_p );

                ONE_STEP* three_FRZ_CTLD_p      = new ONE_STEP();
                vrfItem* tsVerifyFRZCTLD3_p   = new vrfItem();
                tsVerifyFRZCTLD3_p->chkRslt_p = new int();
                setONE_STEP ( three_FRZ_CTLD_p, 3, I_F, 0, NULL, I_F, 1, NULL,
                              tsVerifyFRZCTLD3_p );
                addNode ( one_FRZ_CTLD_p, three_FRZ_CTLD_p );

                Fun_p->steps_p[CTL_FRZ_CTLD_SONPT_TRNS] = one_FRZ_CTLD_p;

                //2、对会话配置默认帧数据 T_snnCfg* stnSnnCfg_p
                u_short cmnAddr = 1;
                BODY_ADDR_VAL bodyAddrVal = {};
                bodyAddrVal.shortAddr = 4;

                struct T_QCC
                {
                    u_char RQT: 6;
                    u_char FRZ: 2;
                } qcc = {0, 1};

                snnCfg_p->ssnID = srvNo;
                //snnCfg_p->actFlag
                snnCfg_p->infoType = GINFOTYPE;
                snnCfg_p->cmnAddr = cmnAddr;
                snnCfg_p->stepNum = ctlFrzSonptNum;
                snnCfg_p->stepCfg_p = new T_ssnStepCfg[ctlFrzSonptNum]();
                //对第一步存储计数量命令
                tmpStnAcsFlag.ctlType =
                0;//[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
                snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //第一步请求帧
                u_char oneReqAPDUNum = 1;//第一步的请求帧数量
                snnCfg_p->stepCfg_p[0].reqAPDUNum = oneReqAPDUNum;
                snnCfg_p->stepCfg_p[0].reqBody_setNum_p = new u_char[oneReqAPDUNum]();
                snnCfg_p->stepCfg_p[0].reqBody_setNum_p[0] = 0;
                //snnCfg_p->stepCfg_p[0].reqEleVal_pp =  new ELE_TYPE[oneReqAPDUNum][11]();
                snnCfg_p->stepCfg_p[0].reqData_p =  new dataFrame[oneReqAPDUNum]();
                u_char memVal[11] = {};
                ELE_TYPE eleVal[11] =
                {
                    {APDU_APCI_INDEX, 0, &memVal[0]},
                    {ASDU_TID_INDEX, 1, &memVal[1]},
                    {ASDU_LNG_INDEX, 0, &memVal[2]},
                    {ASDU_VSQ_INDEX, 1, &memVal[3]},
                    {ASDU_COT_INDEX, 1, &memVal[4]},
                    {ASDU_ADDR_INDEX, sizeof ( u_short ), ( u_char* )& cmnAddr},
                    {BODY_TID_INDEX, 0, &memVal[6]},
                    {BODY_ADDR_INDEX, 1, &memVal[7]},
                    {BODY_SET_INDEX, sizeof ( qcc ), ( u_char* )& qcc},
                    {BODY_TSP_INDEX, 0, &memVal[9]},
                    {ASDU_TSP_INDEX, 0, &memVal[10]}
                };
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_101;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_6;
                //*(eleVal[ASDU_ADDR_INDEX].p_Mem) = cmnAddr;
                * ( eleVal[BODY_ADDR_INDEX].p_Mem ) = 0;
                //调用接口给snnCfg_p第一步请求帧的第1帧赋值
                setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
                //第一步响应
                snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

                //第二步计数量被存储确认
                tmpStnAcsFlag.ctlType =
                0;//[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
                snnCfg_p->stepCfg_p[1].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //第二步的请求帧
                snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
                //第二步的响应帧
                u_char twoRespAPDUNum = 1;//第二步的响应帧数量
                snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
                snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
                snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
                defSetNum;//第二步响应第1帧元素集个数
                //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
                snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_101;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_7;
                //调用接口给snnCfg_p第二步响应的第1帧赋值
                setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

                //第三步计数量突发
                tmpStnAcsFlag.ctlType =
                2;//[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
                tmpStnAcsFlag.setValid = 1;
                snnCfg_p->stepCfg_p[2].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //第三步的请求帧
                snnCfg_p->stepCfg_p[2].reqAPDUNum = 0;
                //第三步的响应帧
                u_char oneRespAPDUNum = 1;//第三步的响应帧数量
                snnCfg_p->stepCfg_p[2].respAPDUNum = oneRespAPDUNum;
                snnCfg_p->stepCfg_p[2].respBody_setNum_p = new u_char[oneRespAPDUNum]();
                snnCfg_p->stepCfg_p[2].respBody_setNum_p[0] =
                defSetNum;//第三步响应第1帧元素集个数
                //snnCfg_p->stepCfg_p[2].respEleVal_pp = new ELE_TYPE[oneRespAPDUNum][11]();
                snnCfg_p->stepCfg_p[2].respData_p = new dataFrame[oneRespAPDUNum]();
                int bdSet = 22;

                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_37;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_3;
                eleVal[BODY_ADDR_INDEX].lng = sizeof ( BODY_ADDR_VAL );
                eleVal[BODY_ADDR_INDEX].p_Mem = ( u_char* ) &bodyAddrVal;
                eleVal[BODY_SET_INDEX].lng = sizeof ( int );
                eleVal[BODY_SET_INDEX].p_Mem = ( u_char* ) &bdSet;
                //调用接口给snnCfg_p第三步突发的第1帧赋值
                setOneFrmCfg ( PASSIVE_TYPE, 3, 1, eleVal, snnCfg_p );

                //3、配置默认的步号与tid,cot的对应关系
                ptclFunDsc->ssnDscNo = srvNo;
                ptclFunDsc->act_prio_chnlID = ACTIVE_TYPE;
                ptclFunDsc->usingMode = usingMode;
                ptclFunDsc->setNum = ctlFrzSonptNum;//stepNum=3
                ptclFunDsc->remoteSet_p = new ptclStep[ctlFrzSonptNum]();
                ptclFunDsc->remoteSet_p[0].TID_p = new u_short();
                ptclFunDsc->remoteSet_p[0].TID_p[0] = MC_TID_101;
                ptclFunDsc->remoteSet_p[0].COT = MC_COT_6;
                ptclFunDsc->remoteSet_p[0].MaxLen = MAX104_ASDU_LNG;
                ptclFunDsc->remoteSet_p[0].TidNum = 1;

                ptclFunDsc->remoteSet_p[1].TID_p = new u_short();
                ptclFunDsc->remoteSet_p[1].TID_p[0] = MC_TID_101;
                ptclFunDsc->remoteSet_p[1].COT = MC_COT_7;
                ptclFunDsc->remoteSet_p[1].MaxLen = MAX104_ASDU_LNG;
                ptclFunDsc->remoteSet_p[1].TidNum = 1;

                ptclFunDsc->remoteSet_p[2].TID_p = new u_short();
                ptclFunDsc->remoteSet_p[2].TID_p[0] = MC_TID_37;
                ptclFunDsc->remoteSet_p[2].COT = MC_COT_3;
                ptclFunDsc->remoteSet_p[2].MaxLen = MAX104_ASDU_LNG;
                ptclFunDsc->remoteSet_p[2].TidNum = 1;

            }

#endif//#ifdef INTER_CREATERL_VAL

            break;
        }

        case PARA_LD://参数加载
        {
#ifndef INTER_CREATERL_VAL

            setFunRl ( srvNo,
                       usingMode,
                       I_F,
                       snnCfg_p,
                       Fun_p
                     );
#endif//#ifndef INTER_CREATERL_VAL

#ifdef INTER_CREATERL_VAL

            if ( usingMode != 0xFF ) //以入参为准
            {
                if ( usingMode != ONLY_LD
                     && usingMode != ONLYACT_LD
                     && usingMode != AUTOACT_LD
                   )
                {
                    freeRl_P ( Fun_p, NULL, NULL );
                    return NULL;
                }
            }
            else//配置选一个可以的值
            {
                usingMode = AUTOACT_LD;
            }

            ptclFunDsc->usingMode = usingMode;

            u_char onlyStepNum = 2;
            u_char onlyActStepNum = 2;
            u_char autoActStepNum = 2;

            Fun_p->srvNo               = PARA_LD;
            Fun_p->usingMode           = AUTO;
            Fun_p->cmLvl[ONLY_LD]      = APP_LVL; //104规约取APP_LVL
            Fun_p->cmLvl[ONLYACT_LD]   = APP_LVL; //104规约取APP_LVL
            Fun_p->cmLvl[AUTOACT_LD]   = APP_LVL; //104规约取APP_LVL
            Fun_p->stepNum[ONLY_LD]    = onlyStepNum;
            Fun_p->stepNum[ONLYACT_LD] = onlyActStepNum;
            Fun_p->stepNum[AUTOACT_LD] = autoActStepNum;


            u_short cmnAddr = 1;
            BODY_ADDR_VAL bodyAddrVal = {};
            bodyAddrVal.shortAddr = 4;
            u_char defSetNum = 1;//默认信息元素个数

            if ( usingMode == ONLY_LD )
            {
                //1、对步规则Fun_p赋值
                ONE_STEP* one_ONLY_p       = new ONE_STEP();
                vrfItem* tsVerifyONLY1_p   = new vrfItem();
                tsVerifyONLY1_p->chkRslt_p = new int();
                setONE_STEP ( one_ONLY_p, 1, I_F, 1, NULL, I_F, 0, NULL, tsVerifyONLY1_p );
                ONE_STEP* two_ONLY_p       = new ONE_STEP();
                vrfItem* tsVerifyONLY2_p   = new vrfItem();
                tsVerifyONLY2_p->chkRslt_p = new int();
                setONE_STEP ( two_ONLY_p, 2, I_F, 0, NULL, I_F, 1, NULL, tsVerifyONLY2_p );
                addNode ( one_ONLY_p, two_ONLY_p );

                Fun_p->steps_p[ONLY_LD] = one_ONLY_p;

                //2、对会话配置默认帧数据 T_snnCfg* stnSnnCfg_p
                snnCfg_p->ssnID = srvNo;
                //snnCfg_p->actFlag
                snnCfg_p->infoType = GINFOTYPE;
                snnCfg_p->cmnAddr = cmnAddr;
                snnCfg_p->stepNum = onlyStepNum;
                snnCfg_p->stepCfg_p = new T_ssnStepCfg[onlyStepNum]();
                //对第一步参数装载命令
                tmpStnAcsFlag.ctlType =
                2;//[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
                tmpStnAcsFlag.setValid = 1;
                snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //第一步请求帧
                u_char oneReqAPDUNum = 1;//第一步的请求帧数量
                snnCfg_p->stepCfg_p[0].reqAPDUNum = oneReqAPDUNum;
                snnCfg_p->stepCfg_p[0].reqBody_setNum_p = new u_char[oneReqAPDUNum]();
                snnCfg_p->stepCfg_p[0].reqBody_setNum_p[0] = 0;
                //snnCfg_p->stepCfg_p[0].reqEleVal_pp =  new ELE_TYPE[oneReqAPDUNum][11]();
                snnCfg_p->stepCfg_p[0].reqData_p =  new dataFrame[oneReqAPDUNum]();
                u_char memVal[11] = {};
                struct pMENA
                {
                    F16 NVA;
                    T_infoQDS QPM_inf;
                };
                struct pMENA pmena = {};
                pmena.QPM_inf.QPM.KPA = 3;
                pmena.QPM_inf.QPM.POP = 1;
                ELE_TYPE eleVal[11] =
                {
                    {APDU_APCI_INDEX, 0, &memVal[0]},
                    {ASDU_TID_INDEX, 1, &memVal[1]},
                    {ASDU_LNG_INDEX, 0, &memVal[2]},
                    {ASDU_VSQ_INDEX, 1, &memVal[3]},
                    {ASDU_COT_INDEX, 1, &memVal[4]},
                    {ASDU_ADDR_INDEX, sizeof ( u_short ), ( u_char* )& cmnAddr},
                    {BODY_TID_INDEX, 0, &memVal[6]},
                    {BODY_ADDR_INDEX, sizeof ( BODY_ADDR_VAL ), ( u_char* )& bodyAddrVal},
                    {BODY_SET_INDEX, sizeof ( pmena ), ( u_char* )& pmena},
                    {BODY_TSP_INDEX, 0, &memVal[9]},
                    {ASDU_TSP_INDEX, 0, &memVal[10]}
                };
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) =
                MC_TID_110; //测量值参数规一化值，MC_TID_111标度化值 MC_TID_112浮点数
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_6;
                //调用接口给snnCfg_p第一步请求帧的第1帧赋值
                setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
                //第一步响应
                snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

                //第二步参数装载命令确认
                tmpStnAcsFlag.ctlType =
                2;//[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
                tmpStnAcsFlag.setValid = 1;
                snnCfg_p->stepCfg_p[1].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //第二步的请求帧
                snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
                //第二步的响应帧
                u_char twoRespAPDUNum = 1;//第二步的响应帧数量
                snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
                snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
                snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
                defSetNum;//第二步响应第1帧元素集个数
                //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
                snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_110;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_7;
                //调用接口给snnCfg_p第二步响应的第1帧赋值
                setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

                //3、配置默认的步号与tid,cot的对应关系
                ptclFunDsc->ssnDscNo = srvNo;
                ptclFunDsc->act_prio_chnlID = ACTIVE_TYPE;
                ptclFunDsc->usingMode = usingMode;
                ptclFunDsc->setNum = onlyStepNum;//stepNum=2
                ptclFunDsc->remoteSet_p = new ptclStep[onlyStepNum]();
                ptclFunDsc->remoteSet_p[0].TID_p = new u_short();
                ptclFunDsc->remoteSet_p[0].TID_p[0] = MC_TID_110;
                ptclFunDsc->remoteSet_p[0].COT = MC_COT_6;
                ptclFunDsc->remoteSet_p[0].MaxLen = MAX104_ASDU_LNG;
                ptclFunDsc->remoteSet_p[0].TidNum = 1;

                ptclFunDsc->remoteSet_p[1].TID_p = new u_short();
                ptclFunDsc->remoteSet_p[1].TID_p[0] = MC_TID_110;
                ptclFunDsc->remoteSet_p[1].COT = MC_COT_7;
                ptclFunDsc->remoteSet_p[1].MaxLen = MAX104_ASDU_LNG;
                ptclFunDsc->remoteSet_p[1].TidNum = 1;
            }
            else if ( usingMode == ONLYACT_LD )
            {
                //1、对步规则Fun_p赋值
                ONE_STEP* one_ONLYACT_p       = new ONE_STEP();
                vrfItem* tsVerifyONLYACT1_p   = new vrfItem();
                tsVerifyONLYACT1_p->chkRslt_p = new int();
                setONE_STEP ( one_ONLYACT_p, 1, I_F, 1, NULL, I_F, 0, NULL,
                              tsVerifyONLYACT1_p );
                ONE_STEP* two_ONLYACT_p       = new ONE_STEP();
                vrfItem* tsVerifyONLYACT2_p   = new vrfItem();
                tsVerifyONLYACT2_p->chkRslt_p = new int();
                setONE_STEP ( two_ONLYACT_p, 2, I_F, 0, NULL, I_F, 1, NULL,
                              tsVerifyONLYACT2_p );
                addNode ( one_ONLYACT_p, two_ONLYACT_p );
                Fun_p->steps_p[ONLYACT_LD] = one_ONLYACT_p;

                //2、对会话配置默认帧数据 T_snnCfg* stnSnnCfg_p
                snnCfg_p->ssnID = srvNo;
                //snnCfg_p->actFlag
                snnCfg_p->infoType = GINFOTYPE;
                snnCfg_p->cmnAddr = cmnAddr;
                snnCfg_p->stepNum = onlyActStepNum;
                snnCfg_p->stepCfg_p = new T_ssnStepCfg[onlyActStepNum]();
                //第一步参数激活命令
                tmpStnAcsFlag.ctlType =
                0;//[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
                snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //第一步请求帧
                u_char oneReqAPDUNum = 1;//第一步的请求帧数量
                snnCfg_p->stepCfg_p[0].reqAPDUNum = oneReqAPDUNum;
                snnCfg_p->stepCfg_p[0].reqBody_setNum_p = new u_char[oneReqAPDUNum]();
                snnCfg_p->stepCfg_p[0].reqBody_setNum_p[0] = 0;
                //snnCfg_p->stepCfg_p[0].reqEleVal_pp =  new ELE_TYPE[oneReqAPDUNum][11]();
                snnCfg_p->stepCfg_p[0].reqData_p =  new dataFrame[oneReqAPDUNum]();
                u_char memVal[11] = {};

                ELE_TYPE eleVal[11] =
                {
                    {APDU_APCI_INDEX, 0, &memVal[0]},
                    {ASDU_TID_INDEX, 1, &memVal[1]},
                    {ASDU_LNG_INDEX, 0, &memVal[2]},
                    {ASDU_VSQ_INDEX, 1, &memVal[3]},
                    {ASDU_COT_INDEX, 1, &memVal[4]},
                    {ASDU_ADDR_INDEX, sizeof ( u_short ), ( u_char* )& cmnAddr},
                    {BODY_TID_INDEX, 0, &memVal[6]},
                    {BODY_ADDR_INDEX, sizeof ( BODY_ADDR_VAL ), ( u_char* )& bodyAddrVal},
                    {BODY_SET_INDEX, 1, &memVal[8]},
                    {BODY_TSP_INDEX, 0, &memVal[9]},
                    {ASDU_TSP_INDEX, 0, &memVal[10]}
                };
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_113;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_6;
                * ( eleVal[BODY_SET_INDEX].p_Mem ) = 3; //此模式的QPA是允许是3
                //调用接口给snnCfg_p第一步请求帧的第1帧赋值
                setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
                //第一步响应
                snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

                //第二步参数已激活确认
                tmpStnAcsFlag.ctlType =
                0;//[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
                snnCfg_p->stepCfg_p[1].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //第二步的请求帧
                snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
                //第二步的响应帧
                u_char twoRespAPDUNum = 1;//第二步的响应帧数量
                snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
                snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
                snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
                defSetNum;//第二步响应第1帧元素集个数
                //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
                snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_113;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_7;
                * ( eleVal[BODY_SET_INDEX].p_Mem ) = 3; //此模式的QPA是允许是3
                //调用接口给snnCfg_p第二步响应的第1帧赋值
                setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

                //3、配置默认的步号与tid,cot的对应关系
                ptclFunDsc->ssnDscNo = srvNo;
                ptclFunDsc->act_prio_chnlID = ACTIVE_TYPE;
                ptclFunDsc->usingMode = usingMode;
                ptclFunDsc->setNum = onlyActStepNum;//stepNum=2
                ptclFunDsc->remoteSet_p = new ptclStep[onlyActStepNum]();
                ptclFunDsc->remoteSet_p[0].TID_p = new u_short();
                ptclFunDsc->remoteSet_p[0].TID_p[0] = MC_TID_113;
                ptclFunDsc->remoteSet_p[0].COT = MC_COT_6;
                ptclFunDsc->remoteSet_p[0].MaxLen = MAX104_ASDU_LNG;
                ptclFunDsc->remoteSet_p[0].TidNum = 1;

                ptclFunDsc->remoteSet_p[1].TID_p = new u_short();
                ptclFunDsc->remoteSet_p[1].TID_p[0] = MC_TID_113;
                ptclFunDsc->remoteSet_p[1].COT = MC_COT_7;
                ptclFunDsc->remoteSet_p[1].MaxLen = MAX104_ASDU_LNG;
                ptclFunDsc->remoteSet_p[1].TidNum = 1;
            }
            else if ( usingMode == AUTOACT_LD )
            {
                //1、对步规则Fun_p赋值
                ONE_STEP* one_AUTOACT_p       = new ONE_STEP();
                vrfItem* tsVerifyAUTOACT1_p   = new vrfItem();
                tsVerifyAUTOACT1_p->chkRslt_p = new int();
                setONE_STEP ( one_AUTOACT_p, 1, I_F, 1, NULL, I_F, 0, NULL,
                              tsVerifyAUTOACT1_p );
                ONE_STEP* two_AUTOACT_p       = new ONE_STEP();
                vrfItem* tsVerifyAUTOACT2_p   = new vrfItem();
                tsVerifyAUTOACT2_p->chkRslt_p = new int();
                setONE_STEP ( two_AUTOACT_p, 2, I_F, 0, NULL, I_F, 1, NULL,
                              tsVerifyAUTOACT2_p );
                addNode ( one_AUTOACT_p, two_AUTOACT_p );
                Fun_p->steps_p[AUTOACT_LD] = one_AUTOACT_p;


                //2、对会话配置默认帧数据 T_snnCfg* stnSnnCfg_p
                snnCfg_p->ssnID = srvNo;
                //snnCfg_p->actFlag
                snnCfg_p->infoType = GINFOTYPE;
                snnCfg_p->cmnAddr = cmnAddr;
                snnCfg_p->stepNum = autoActStepNum;
                snnCfg_p->stepCfg_p = new T_ssnStepCfg[autoActStepNum]();
                //第一步参数装载并激活命令
                tmpStnAcsFlag.ctlType =
                2;//[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
                tmpStnAcsFlag.setValid = 1;
                snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //第一步请求帧
                u_char oneReqAPDUNum = 1;//第一步的请求帧数量
                snnCfg_p->stepCfg_p[0].reqAPDUNum = oneReqAPDUNum;
                snnCfg_p->stepCfg_p[0].reqBody_setNum_p = new u_char[oneReqAPDUNum]();
                snnCfg_p->stepCfg_p[0].reqBody_setNum_p[0] = 0;
                //snnCfg_p->stepCfg_p[0].reqEleVal_pp =  new ELE_TYPE[oneReqAPDUNum][11]();
                snnCfg_p->stepCfg_p[0].reqData_p =  new dataFrame[oneReqAPDUNum]();
                u_char memVal[11] = {};
                struct pMENA
                {
                    F16 NVA;
                    T_infoQDS QPM_inf;
                };
                struct pMENA pmena = {};
                pmena.QPM_inf.QPM.KPA = 3;
                ELE_TYPE eleVal[11] =
                {
                    {APDU_APCI_INDEX, 0, &memVal[0]},
                    {ASDU_TID_INDEX, 1, &memVal[1]},
                    {ASDU_LNG_INDEX, 0, &memVal[2]},
                    {ASDU_VSQ_INDEX, 1, &memVal[3]},
                    {ASDU_COT_INDEX, 1, &memVal[4]},
                    {ASDU_ADDR_INDEX, sizeof ( u_short ), ( u_char* )& cmnAddr},
                    {BODY_TID_INDEX, 0, &memVal[6]},
                    {BODY_ADDR_INDEX, sizeof ( BODY_ADDR_VAL ), ( u_char* )& bodyAddrVal},
                    {BODY_SET_INDEX, sizeof ( pmena ), ( u_char* )& pmena},
                    {BODY_TSP_INDEX, 0, &memVal[9]},
                    {ASDU_TSP_INDEX, 0, &memVal[10]}
                };
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) =
                MC_TID_110; //测量值参数规一化值，MC_TID_111标度化值 MC_TID_112浮点数
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_6;
                //调用接口给snnCfg_p第一步请求帧的第1帧赋值
                setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
                //第一步响应
                snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

                //第二步参数装载并激活命令确认
                tmpStnAcsFlag.ctlType =
                2;//[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
                tmpStnAcsFlag.setValid = 1;
                snnCfg_p->stepCfg_p[1].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //第二步的请求帧
                snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
                //第二步的响应帧
                u_char twoRespAPDUNum = 1;//第二步的响应帧数量
                snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
                snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
                snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
                defSetNum;//第二步响应第1帧元素集个数
                //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
                snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_110;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_7;
                //调用接口给snnCfg_p第二步响应的第1帧赋值
                setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

                //3、配置默认的步号与tid,cot的对应关系
                ptclFunDsc->ssnDscNo = srvNo;
                ptclFunDsc->act_prio_chnlID = ACTIVE_TYPE;
                ptclFunDsc->usingMode = usingMode;
                ptclFunDsc->setNum = autoActStepNum;//stepNum=2
                ptclFunDsc->remoteSet_p = new ptclStep[autoActStepNum]();
                ptclFunDsc->remoteSet_p[0].TID_p = new u_short();
                ptclFunDsc->remoteSet_p[0].TID_p[0] = MC_TID_110;
                ptclFunDsc->remoteSet_p[0].COT = MC_COT_6;
                ptclFunDsc->remoteSet_p[0].MaxLen = MAX104_ASDU_LNG;
                ptclFunDsc->remoteSet_p[0].TidNum = 1;

                ptclFunDsc->remoteSet_p[1].TID_p = new u_short();
                ptclFunDsc->remoteSet_p[1].TID_p[0] = MC_TID_110;
                ptclFunDsc->remoteSet_p[1].COT = MC_COT_7;
                ptclFunDsc->remoteSet_p[1].MaxLen = MAX104_ASDU_LNG;
                ptclFunDsc->remoteSet_p[1].TidNum = 1;
            }

#endif//#ifdef INTER_CREATERL_VAL

            break;
        }

        case TST_PRCD://测试过程
        {
#ifndef INTER_CREATERL_VAL

            setFunRl ( srvNo,
                       usingMode,
                       I_F,
                       snnCfg_p,
                       Fun_p
                     );
#endif//#ifndef INTER_CREATERL_VAL

#ifdef INTER_CREATERL_VAL

            if ( usingMode != 0xFF ) //以入参为准
            {
                if ( usingMode != ONLY_LD )
                {
                    freeRl_P ( Fun_p, NULL, NULL );
                    return NULL;
                }
            }
            else//配置选一个可以的值
            {
                usingMode = ONLY_LD;
            }

            ptclFunDsc->usingMode = usingMode;

            u_char stepNum = 2;

            //1、对步规则Fun_p赋值
            Fun_p->srvNo             = TST_PRCD;
            Fun_p->usingMode         = ONLY_ONE;
            Fun_p->cmLvl[ONLY_ONE]   = APP_LVL;//104规约取APP_LVL
            Fun_p->stepNum[ONLY_ONE] = stepNum;

            ONE_STEP* one_ONLY_p       = new ONE_STEP();
            vrfItem* tsVerifyONLY1_p   = new vrfItem();
            tsVerifyONLY1_p->chkRslt_p = new int();
            setONE_STEP ( one_ONLY_p, 1, I_F, 1, NULL, I_F, 0, NULL, tsVerifyONLY1_p );
            ONE_STEP* two_ONLY_p       = new ONE_STEP();
            vrfItem* tsVerifyONLY2_p   = new vrfItem();
            tsVerifyONLY2_p->chkRslt_p = new int();
            setONE_STEP ( two_ONLY_p, 2, I_F, 0, NULL, I_F, 1, NULL, tsVerifyONLY2_p );
            addNode ( one_ONLY_p, two_ONLY_p );
            Fun_p->steps_p[ONLY_ONE] = one_ONLY_p;

            //2、对会话配置默认帧数据 T_snnCfg* stnSnnCfg_p
            u_short cmnAddr = 11;
            u_short bodySet = 0x55AA;//FBP=固定测试字
            u_char defSetNum = 1;//默认信息元素个数

            snnCfg_p->ssnID = srvNo;
            //snnCfg_p->actFlag
            snnCfg_p->infoType = GINFOTYPE;
            snnCfg_p->cmnAddr = cmnAddr;
            snnCfg_p->stepNum = stepNum;
            snnCfg_p->stepCfg_p = new T_ssnStepCfg[stepNum]();
            //对第一步
            tmpStnAcsFlag.ctlType =
            2;//[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
            tmpStnAcsFlag.setValid = 1;
            tmpStnAcsFlag.tspValidB = 1;
            snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
            //第一步请求帧
            u_char oneReqAPDUNum = 1;//第一步的请求帧数量
            snnCfg_p->stepCfg_p[0].reqAPDUNum = oneReqAPDUNum;
            snnCfg_p->stepCfg_p[0].reqBody_setNum_p = new u_char[oneReqAPDUNum]();
            snnCfg_p->stepCfg_p[0].reqBody_setNum_p[0] = 0;
            //snnCfg_p->stepCfg_p[0].reqEleVal_pp =  new ELE_TYPE[oneReqAPDUNum][11]();
            snnCfg_p->stepCfg_p[0].reqData_p =  new dataFrame[oneReqAPDUNum]();
            u_char memVal[11] = {};
            CP56TIME2a tmStmp = {1};
            ELE_TYPE eleVal[11] =
            {
                {APDU_APCI_INDEX, 0, &memVal[0]},
                {ASDU_TID_INDEX, 1, &memVal[1]},
                {ASDU_LNG_INDEX, 0, &memVal[2]},
                {ASDU_VSQ_INDEX, 1, &memVal[3]},
                {ASDU_COT_INDEX, 1, &memVal[4]},
                {ASDU_ADDR_INDEX, sizeof ( u_short ), ( u_char* )& cmnAddr},
                {BODY_TID_INDEX, 0, &memVal[6]},
                {BODY_ADDR_INDEX, 1, &memVal[7]},
                {BODY_SET_INDEX, sizeof ( u_short ), ( u_char* )& bodySet},
                {BODY_TSP_INDEX, sizeof ( CP56TIME2a ), ( u_char* )& tmStmp},
                {ASDU_TSP_INDEX, 0, &memVal[10]}
            };
            * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_107;
            * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_6;
            * ( eleVal[BODY_ADDR_INDEX].p_Mem ) = 0;
            //调用接口给snnCfg_p第一步请求帧的第1帧赋值
            setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
            //第一步响应
            snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

            //第二步被读数据的默认配置
            tmpStnAcsFlag.ctlType =
            2;//[1,2] 位1 = 1存储子系统执行数据值的保存；位2 = 1存储子系统执行数据值的加载
            tmpStnAcsFlag.setValid = 1;
            tmpStnAcsFlag.tspValidB = 1;
            snnCfg_p->stepCfg_p[1].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
            //第二步的请求帧
            snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
            //第二步的响应帧
            u_char twoRespAPDUNum = 1;//第二步的响应帧数量
            snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
            snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
            snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
            defSetNum;//第二步响应第1帧元素集个数
            //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
            snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
            * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_107;
            * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_7;
            * ( eleVal[BODY_ADDR_INDEX].p_Mem ) = 0;
            //调用接口给snnCfg_p第二步响应的第1帧赋值
            setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

            //3、配置默认的步号与tid,cot的对应关系
            ptclFunDsc->ssnDscNo = srvNo;
            ptclFunDsc->act_prio_chnlID = ACTIVE_TYPE;
            ptclFunDsc->usingMode = usingMode;
            ptclFunDsc->setNum = stepNum;//stepNum=2
            ptclFunDsc->remoteSet_p = new ptclStep[stepNum]();
            ptclFunDsc->remoteSet_p[0].TID_p = new u_short();
            ptclFunDsc->remoteSet_p[0].TID_p[0] = MC_TID_107;
            ptclFunDsc->remoteSet_p[0].COT = MC_COT_6;
            ptclFunDsc->remoteSet_p[0].MaxLen = MAX104_ASDU_LNG;
            ptclFunDsc->remoteSet_p[0].TidNum = 1;

            ptclFunDsc->remoteSet_p[1].TID_p = new u_short();
            ptclFunDsc->remoteSet_p[1].TID_p[0] = MC_TID_107;
            ptclFunDsc->remoteSet_p[1].COT = MC_COT_7;
            ptclFunDsc->remoteSet_p[1].MaxLen = MAX104_ASDU_LNG;
            ptclFunDsc->remoteSet_p[1].TidNum = 1;
#endif//#ifdef INTER_CREATERL_VAL

            break;
        }

        case FILE_TRNS://文件传输
        {
            //暂不实现
            break;
        }

        case ACQ_TRNSDLY://收集传输延迟值
        {
            //不使用
            freeRl_P ( Fun_p, snnCfg_p, ptclFunDsc );
            break;
        }

        case U_STARTDT://104协议U帧STARTDT 抽象的会话
        {
#ifndef INTER_CREATERL_VAL

            setFunRl ( srvNo,
                       usingMode,
                       U_F,
                       snnCfg_p,
                       Fun_p
                     );
#endif//#ifndef INTER_CREATERL_VAL

#ifdef INTER_CREATERL_VAL

            if ( usingMode != 0xFF ) //以入参为准
            {
                if ( usingMode != ONLY_LD )
                {
                    freeRl_P ( Fun_p, NULL, NULL );
                    return NULL;
                }
            }
            else//配置选一个可以的值
            {
                usingMode = ONLY_LD;
            }

            ptclFunDsc->usingMode = usingMode;

            u_char stepNum = 2;

            //1、对步规则Fun_p赋值
            Fun_p->srvNo             = U_STARTDT;
            Fun_p->usingMode         = ONLY_ONE;
            Fun_p->cmLvl[ONLY_ONE]   = APP_LVL;
            Fun_p->stepNum[ONLY_ONE] = stepNum;

            ONE_STEP* one_ONLY_p       = new ONE_STEP();
            setONE_STEP ( one_ONLY_p, 1, U_F, 1, NULL, U_F, 0, NULL, NULL );
            ONE_STEP* two_ONLY_p       = new ONE_STEP();
            setONE_STEP ( two_ONLY_p, 2, U_F, 0, NULL, U_F, 1, NULL, NULL );
            addNode ( one_ONLY_p, two_ONLY_p );
            Fun_p->steps_p[ONLY_ONE] = one_ONLY_p;

            //2、对会话配置默认帧数据 T_snnCfg* stnSnnCfg_p
            u_short cmnAddr = 0;
            snnCfg_p->ssnID = srvNo;
            //snnCfg_p->actFlag
            snnCfg_p->infoType = GINFOTYPE;
            snnCfg_p->cmnAddr = cmnAddr;
            snnCfg_p->stepNum = stepNum;
            snnCfg_p->stepCfg_p = new T_ssnStepCfg[stepNum]();
            //第一步激活
            snnCfg_p->stepCfg_p[0].stnAcsFlag = 0;
            //第一步请求帧
            u_char oneReqAPDUNum = 1;//第一步的请求帧数量
            snnCfg_p->stepCfg_p[0].reqAPDUNum = oneReqAPDUNum;
            snnCfg_p->stepCfg_p[0].reqBody_setNum_p = new u_char[oneReqAPDUNum]();
            snnCfg_p->stepCfg_p[0].reqBody_setNum_p[0] = 0;
            //snnCfg_p->stepCfg_p[0].reqEleVal_pp =  new ELE_TYPE[oneReqAPDUNum][11]();
            snnCfg_p->stepCfg_p[0].reqData_p =  new dataFrame[oneReqAPDUNum]();
            u_char memVal[11] = {};
            ELE_TYPE eleVal[11] =
            {
                {APDU_APCI_INDEX, 0, &memVal[0]},
                {ASDU_TID_INDEX, 1, &memVal[1]},
                {ASDU_LNG_INDEX, 0, &memVal[2]},
                {ASDU_VSQ_INDEX, 1, &memVal[3]},
                {ASDU_COT_INDEX, 1, &memVal[4]},
                {ASDU_ADDR_INDEX, 1, &memVal[5]},
                {BODY_TID_INDEX, 0, &memVal[6]},
                {BODY_ADDR_INDEX, 0, &memVal[7]},
                {BODY_SET_INDEX, 0, &memVal[8]},
                {BODY_TSP_INDEX, 0, &memVal[9]},
                {ASDU_TSP_INDEX, 0, &memVal[10]}
            };
            * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_136;
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_6;
            //调用接口给snnCfg_p第一步请求帧的第1帧赋值
            setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
            //第一步响应
            snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

            //第二步激活确认
            snnCfg_p->stepCfg_p[1].stnAcsFlag = 0;
            //第二步的请求帧
            snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
            //第二步的响应帧
            u_char twoRespAPDUNum = 1;//第二步的响应帧数量
            snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
            snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
            snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
            0;//第二步响应第1帧元素集个数
            //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
            snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_7;
            //调用接口给snnCfg_p第二步响应的第1帧赋值
            setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

            //3、配置默认的步号与tid,cot的对应关系
            ptclFunDsc->ssnDscNo = srvNo;
            ptclFunDsc->act_prio_chnlID = ACTIVE_TYPE;
            ptclFunDsc->usingMode = usingMode;
            ptclFunDsc->setNum = stepNum;//stepNum=2
            ptclFunDsc->remoteSet_p = new ptclStep[stepNum]();
            ptclFunDsc->remoteSet_p[0].TID_p = new u_short();
            ptclFunDsc->remoteSet_p[0].TID_p[0] = MC_TID_136;
            ptclFunDsc->remoteSet_p[0].COT = MC_COT_6;
            ptclFunDsc->remoteSet_p[0].MaxLen = MAX104_ASDU_LNG;
            ptclFunDsc->remoteSet_p[0].TidNum = 1;

            ptclFunDsc->remoteSet_p[1].TID_p = new u_short();
            ptclFunDsc->remoteSet_p[1].TID_p[0] = MC_TID_136;
            ptclFunDsc->remoteSet_p[1].COT = MC_COT_7;
            ptclFunDsc->remoteSet_p[1].MaxLen = MAX104_ASDU_LNG;
            ptclFunDsc->remoteSet_p[1].TidNum = 1;
#endif//#ifdef INTER_CREATERL_VAL

            break;
        }

        case U_STOPDT://104协议U帧STOPDT 抽象的会话
        {
#ifndef INTER_CREATERL_VAL

            setFunRl ( srvNo,
                       usingMode,
                       U_F,
                       snnCfg_p,
                       Fun_p
                     );
#endif//#ifndef INTER_CREATERL_VAL

#ifdef INTER_CREATERL_VAL

            if ( usingMode != 0xFF ) //以入参为准
            {
                if ( usingMode != ONLY_LD )
                {
                    freeRl_P ( Fun_p, NULL, NULL );
                    return NULL;
                }
            }
            else//配置选一个可以的值
            {
                usingMode = ONLY_LD;
            }

            ptclFunDsc->usingMode = usingMode;

            u_char stepNum = 2;

            //1、对步规则Fun_p赋值
            Fun_p->srvNo             = U_STOPDT;
            Fun_p->usingMode         = ONLY_ONE;
            Fun_p->cmLvl[ONLY_ONE]   = APP_LVL;
            Fun_p->stepNum[ONLY_ONE] = stepNum;

            ONE_STEP* one_ONLY_p       = new ONE_STEP();
            setONE_STEP ( one_ONLY_p, 1, U_F, 1, NULL, U_F, 0, NULL, NULL );
            ONE_STEP* two_ONLY_p       = new ONE_STEP();
            setONE_STEP ( two_ONLY_p, 2, U_F, 0, NULL, U_F, 1, NULL, NULL );
            addNode ( one_ONLY_p, two_ONLY_p );
            Fun_p->steps_p[ONLY_ONE] = one_ONLY_p;

            //2、对会话配置默认帧数据 T_snnCfg* stnSnnCfg_p
            u_short cmnAddr = 0;
            snnCfg_p->ssnID = srvNo;
            //snnCfg_p->actFlag
            snnCfg_p->infoType = GINFOTYPE;
            snnCfg_p->cmnAddr = cmnAddr;
            snnCfg_p->stepNum = stepNum;
            snnCfg_p->stepCfg_p = new T_ssnStepCfg[stepNum]();
            //第一步激活
            snnCfg_p->stepCfg_p[0].stnAcsFlag = 0;
            //第一步请求帧
            u_char oneReqAPDUNum = 1;//第一步的请求帧数量
            snnCfg_p->stepCfg_p[0].reqAPDUNum = oneReqAPDUNum;
            snnCfg_p->stepCfg_p[0].reqBody_setNum_p = new u_char[oneReqAPDUNum]();
            snnCfg_p->stepCfg_p[0].reqBody_setNum_p[0] = 0;
            //snnCfg_p->stepCfg_p[0].reqEleVal_pp =  new ELE_TYPE[oneReqAPDUNum][11]();
            snnCfg_p->stepCfg_p[0].reqData_p =  new dataFrame[oneReqAPDUNum]();
            u_char memVal[11] = {};
            ELE_TYPE eleVal[11] =
            {
                {APDU_APCI_INDEX, 0, &memVal[0]},
                {ASDU_TID_INDEX, 1, &memVal[1]},
                {ASDU_LNG_INDEX, 0, &memVal[2]},
                {ASDU_VSQ_INDEX, 1, &memVal[3]},
                {ASDU_COT_INDEX, 1, &memVal[4]},
                {ASDU_ADDR_INDEX, 1, &memVal[5]},
                {BODY_TID_INDEX, 0, &memVal[6]},
                {BODY_ADDR_INDEX, 0, &memVal[7]},
                {BODY_SET_INDEX, 0, &memVal[8]},
                {BODY_TSP_INDEX, 0, &memVal[9]},
                {ASDU_TSP_INDEX, 0, &memVal[10]}
            };
            * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_137;
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_6;
            //调用接口给snnCfg_p第一步请求帧的第1帧赋值
            setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
            //第一步响应
            snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

            //第二步激活确认
            snnCfg_p->stepCfg_p[1].stnAcsFlag = 0;
            //第二步的请求帧
            snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
            //第二步的响应帧
            u_char twoRespAPDUNum = 1;//第二步的响应帧数量
            snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
            snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
            snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
            0;//第二步响应第1帧元素集个数
            //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
            snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_7;
            //调用接口给snnCfg_p第二步响应的第1帧赋值
            setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

            //3、配置默认的步号与tid,cot的对应关系
            ptclFunDsc->ssnDscNo = srvNo;
            ptclFunDsc->act_prio_chnlID = ACTIVE_TYPE;
            ptclFunDsc->usingMode = usingMode;
            ptclFunDsc->setNum = stepNum;//stepNum=2
            ptclFunDsc->remoteSet_p = new ptclStep[stepNum]();
            ptclFunDsc->remoteSet_p[0].TID_p = new u_short();
            ptclFunDsc->remoteSet_p[0].TID_p[0] = MC_TID_137;
            ptclFunDsc->remoteSet_p[0].COT = MC_COT_6;
            ptclFunDsc->remoteSet_p[0].MaxLen = MAX104_ASDU_LNG;
            ptclFunDsc->remoteSet_p[0].TidNum = 1;

            ptclFunDsc->remoteSet_p[1].TID_p = new u_short();
            ptclFunDsc->remoteSet_p[1].TID_p[0] = MC_TID_137;
            ptclFunDsc->remoteSet_p[1].COT = MC_COT_7;
            ptclFunDsc->remoteSet_p[1].MaxLen = MAX104_ASDU_LNG;
            ptclFunDsc->remoteSet_p[1].TidNum = 1;
#endif//#ifdef INTER_CREATERL_VAL
            break;
        }

        case U_TESTFR://104协议U帧TESTFR 抽象的会话
        {
#ifndef INTER_CREATERL_VAL

            setFunRl ( srvNo,
                       usingMode,
                       U_F,
                       snnCfg_p,
                       Fun_p
                     );
#endif//#ifndef INTER_CREATERL_VAL

#ifdef INTER_CREATERL_VAL

            if ( usingMode != 0xFF ) //以入参为准
            {
                if ( usingMode != ONLY_LD )
                {
                    freeRl_P ( Fun_p, NULL, NULL );
                    return NULL;
                }
            }
            else//配置选一个可以的值
            {
                usingMode = ONLY_LD;
            }

            ptclFunDsc->usingMode = usingMode;

            u_char stepNum = 2;

            //1、对步规则Fun_p赋值
            Fun_p->srvNo             = U_TESTFR;
            Fun_p->usingMode         = ONLY_ONE;
            Fun_p->cmLvl[ONLY_ONE]   = APP_LVL;
            Fun_p->stepNum[ONLY_ONE] = stepNum;

            ONE_STEP* one_ONLY_p       = new ONE_STEP();
            setONE_STEP ( one_ONLY_p, 1, U_F, 1, NULL, U_F, 0, NULL, NULL );
            ONE_STEP* two_ONLY_p       = new ONE_STEP();
            setONE_STEP ( two_ONLY_p, 2, U_F, 0, NULL, U_F, 1, NULL, NULL );
            addNode ( one_ONLY_p, two_ONLY_p );
            Fun_p->steps_p[ONLY_ONE] = one_ONLY_p;

            //2、对会话配置默认帧数据 T_snnCfg* stnSnnCfg_p
            u_short cmnAddr = 0;
            snnCfg_p->ssnID = srvNo;
            //snnCfg_p->actFlag
            snnCfg_p->infoType = GINFOTYPE;
            snnCfg_p->cmnAddr = cmnAddr;
            snnCfg_p->stepNum = stepNum;
            snnCfg_p->stepCfg_p = new T_ssnStepCfg[stepNum]();
            //第一步激活
            snnCfg_p->stepCfg_p[0].stnAcsFlag = 0;
            //第一步请求帧
            u_char oneReqAPDUNum = 1;//第一步的请求帧数量
            snnCfg_p->stepCfg_p[0].reqAPDUNum = oneReqAPDUNum;
            snnCfg_p->stepCfg_p[0].reqBody_setNum_p = new u_char[oneReqAPDUNum]();
            snnCfg_p->stepCfg_p[0].reqBody_setNum_p[0] = 0;
            //snnCfg_p->stepCfg_p[0].reqEleVal_pp =  new ELE_TYPE[oneReqAPDUNum][11]();
            snnCfg_p->stepCfg_p[0].reqData_p =  new dataFrame[oneReqAPDUNum]();
            u_char memVal[11] = {};
            ELE_TYPE eleVal[11] =
            {
                {APDU_APCI_INDEX, 0, &memVal[0]},
                {ASDU_TID_INDEX, 1, &memVal[1]},
                {ASDU_LNG_INDEX, 0, &memVal[2]},
                {ASDU_VSQ_INDEX, 1, &memVal[3]},
                {ASDU_COT_INDEX, 1, &memVal[4]},
                {ASDU_ADDR_INDEX, 1, &memVal[5]},
                {BODY_TID_INDEX, 0, &memVal[6]},
                {BODY_ADDR_INDEX, 0, &memVal[7]},
                {BODY_SET_INDEX, 0, &memVal[8]},
                {BODY_TSP_INDEX, 0, &memVal[9]},
                {ASDU_TSP_INDEX, 0, &memVal[10]}
            };
            * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_138;
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_6;
            //调用接口给snnCfg_p第一步请求帧的第1帧赋值
            setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
            //第一步响应
            snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

            //第二步激活确认
            snnCfg_p->stepCfg_p[1].stnAcsFlag = 0;
            //第二步的请求帧
            snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
            //第二步的响应帧
            u_char twoRespAPDUNum = 1;//第二步的响应帧数量
            snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
            snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
            snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
            0;//第二步响应第1帧元素集个数
            //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
            snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_7;
            //调用接口给snnCfg_p第二步响应的第1帧赋值
            setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

            //3、配置默认的步号与tid,cot的对应关系
            ptclFunDsc->ssnDscNo = srvNo;
            ptclFunDsc->act_prio_chnlID = ACTIVE_TYPE;
            ptclFunDsc->usingMode = usingMode;
            ptclFunDsc->setNum = stepNum;//stepNum=2
            ptclFunDsc->remoteSet_p = new ptclStep[stepNum]();
            ptclFunDsc->remoteSet_p[0].TID_p = new u_short();
            ptclFunDsc->remoteSet_p[0].TID_p[0] = MC_TID_138;
            ptclFunDsc->remoteSet_p[0].COT = MC_COT_6;
            ptclFunDsc->remoteSet_p[0].MaxLen = MAX104_ASDU_LNG;
            ptclFunDsc->remoteSet_p[0].TidNum = 1;

            ptclFunDsc->remoteSet_p[1].TID_p = new u_short();
            ptclFunDsc->remoteSet_p[1].TID_p[0] = MC_TID_138;
            ptclFunDsc->remoteSet_p[1].COT = MC_COT_7;
            ptclFunDsc->remoteSet_p[1].MaxLen = MAX104_ASDU_LNG;
            ptclFunDsc->remoteSet_p[1].TidNum = 1;
#endif//#ifdef INTER_CREATERL_VAL

            break;
        }

        case U_NEWHOLD://用U帧的STARTDT STOPDT TESTFR 全为0 表示出新的会话：新联一个通道
        {
#ifndef INTER_CREATERL_VAL

            setFunRl ( srvNo,
                       usingMode,
                       U_F,
                       snnCfg_p,
                       Fun_p
                     );
#endif//#ifndef INTER_CREATERL_VAL

#ifdef INTER_CREATERL_VAL

            if ( usingMode != 0xFF ) //以入参为准
            {
                if ( usingMode != ONLY_LD )
                {
                    freeRl_P ( Fun_p, NULL, NULL );
                    return NULL;
                }
            }
            else//配置选一个可以的值
            {
                usingMode = ONLY_LD;
            }

            ptclFunDsc->usingMode = usingMode;

            u_char stepNum = 2;

            //1、对步规则Fun_p赋值
            Fun_p->srvNo             = U_NEWHOLD;
            Fun_p->usingMode         = ONLY_ONE;
            Fun_p->cmLvl[ONLY_ONE]   = APP_LVL;
            Fun_p->stepNum[ONLY_ONE] = stepNum;

            ONE_STEP* one_ONLY_p       = new ONE_STEP();
            setONE_STEP ( one_ONLY_p, 1, U_F, 1, NULL, U_F, 0, NULL, NULL );
            ONE_STEP* two_ONLY_p       = new ONE_STEP();
            setONE_STEP ( two_ONLY_p, 2, U_F, 0, NULL, U_F, 1, NULL, NULL );
            addNode ( one_ONLY_p, two_ONLY_p );
            Fun_p->steps_p[ONLY_ONE] = one_ONLY_p;

            //2、对会话配置默认帧数据 T_snnCfg* stnSnnCfg_p
            u_short cmnAddr = 0;
            snnCfg_p->ssnID = srvNo;
            //snnCfg_p->actFlag
            snnCfg_p->infoType = GINFOTYPE;
            snnCfg_p->cmnAddr = cmnAddr;
            snnCfg_p->stepNum = stepNum;
            snnCfg_p->stepCfg_p = new T_ssnStepCfg[stepNum]();
            //第一步激活
            snnCfg_p->stepCfg_p[0].stnAcsFlag = 0;
            //第一步请求帧
            u_char oneReqAPDUNum = 1;//第一步的请求帧数量
            snnCfg_p->stepCfg_p[0].reqAPDUNum = oneReqAPDUNum;
            snnCfg_p->stepCfg_p[0].reqBody_setNum_p = new u_char[oneReqAPDUNum]();
            snnCfg_p->stepCfg_p[0].reqBody_setNum_p[0] = 0;
            //snnCfg_p->stepCfg_p[0].reqEleVal_pp =  new ELE_TYPE[oneReqAPDUNum][11]();
            snnCfg_p->stepCfg_p[0].reqData_p =  new dataFrame[oneReqAPDUNum]();
            u_char memVal[11] = {};
            ELE_TYPE eleVal[11] =
            {
                {APDU_APCI_INDEX, 0, &memVal[0]},
                {ASDU_TID_INDEX, 1, &memVal[1]},
                {ASDU_LNG_INDEX, 0, &memVal[2]},
                {ASDU_VSQ_INDEX, 1, &memVal[3]},
                {ASDU_COT_INDEX, 1, &memVal[4]},
                {ASDU_ADDR_INDEX, 1, &memVal[5]},
                {BODY_TID_INDEX, 0, &memVal[6]},
                {BODY_ADDR_INDEX, 0, &memVal[7]},
                {BODY_SET_INDEX, 0, &memVal[8]},
                {BODY_TSP_INDEX, 0, &memVal[9]},
                {ASDU_TSP_INDEX, 0, &memVal[10]}
            };
            * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_139;
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_6;
            //调用接口给snnCfg_p第一步请求帧的第1帧赋值
            setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
            //第一步响应
            snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

            //第二步激活确认
            snnCfg_p->stepCfg_p[1].stnAcsFlag = 0;
            //第二步的请求帧
            snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
            //第二步的响应帧
            u_char twoRespAPDUNum = 1;//第二步的响应帧数量
            snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
            snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
            snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
            0;//第二步响应第1帧元素集个数
            //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
            snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_7;
            //调用接口给snnCfg_p第二步响应的第1帧赋值
            setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

            //3、配置默认的步号与tid,cot的对应关系
            ptclFunDsc->ssnDscNo = srvNo;
            ptclFunDsc->act_prio_chnlID = ACTIVE_TYPE;
            ptclFunDsc->usingMode = usingMode;
            ptclFunDsc->setNum = stepNum;//stepNum=2
            ptclFunDsc->remoteSet_p = new ptclStep[stepNum]();
            ptclFunDsc->remoteSet_p[0].TID_p = new u_short();
            ptclFunDsc->remoteSet_p[0].TID_p[0] = MC_TID_139;
            ptclFunDsc->remoteSet_p[0].COT = MC_COT_6;
            ptclFunDsc->remoteSet_p[0].MaxLen = MAX104_ASDU_LNG;
            ptclFunDsc->remoteSet_p[0].TidNum = 1;

            ptclFunDsc->remoteSet_p[1].TID_p = new u_short();
            ptclFunDsc->remoteSet_p[1].TID_p[0] = MC_TID_139;
            ptclFunDsc->remoteSet_p[1].COT = MC_COT_7;
            ptclFunDsc->remoteSet_p[1].MaxLen = MAX104_ASDU_LNG;
            ptclFunDsc->remoteSet_p[1].TidNum = 1;
#endif//#ifdef INTER_CREATERL_VAL

            break;
        }

        case U_DELHOLD://用U帧的STARTDT STOPDT TESTFR 全为1 表示出新的会话：关一个通道
        {
#ifndef INTER_CREATERL_VAL

            setFunRl ( srvNo,
                       usingMode,
                       U_F,
                       snnCfg_p,
                       Fun_p
                     );
#endif//#ifndef INTER_CREATERL_VAL

#ifdef INTER_CREATERL_VAL

            if ( usingMode != 0xFF ) //以入参为准
            {
                if ( usingMode != ONLY_LD )
                {
                    freeRl_P ( Fun_p, NULL, NULL );
                    return NULL;
                }
            }
            else//配置选一个可以的值
            {
                usingMode = ONLY_LD;
            }

            ptclFunDsc->usingMode = usingMode;

            u_char stepNum = 2;

            //1、对步规则Fun_p赋值
            Fun_p->srvNo             = U_DELHOLD;
            Fun_p->usingMode         = ONLY_ONE;
            Fun_p->cmLvl[ONLY_ONE]   = APP_LVL;
            Fun_p->stepNum[ONLY_ONE] = stepNum;

            ONE_STEP* one_ONLY_p       = new ONE_STEP();
            setONE_STEP ( one_ONLY_p, 1, U_F, 1, NULL, U_F, 0, NULL, NULL );
            ONE_STEP* two_ONLY_p       = new ONE_STEP();
            setONE_STEP ( two_ONLY_p, 2, U_F, 0, NULL, U_F, 1, NULL, NULL );
            addNode ( one_ONLY_p, two_ONLY_p );
            Fun_p->steps_p[ONLY_ONE] = one_ONLY_p;

            //2、对会话配置默认帧数据 T_snnCfg* stnSnnCfg_p
            u_short cmnAddr = 0;
            snnCfg_p->ssnID = srvNo;
            //snnCfg_p->actFlag
            snnCfg_p->infoType = GINFOTYPE;
            snnCfg_p->cmnAddr = cmnAddr;
            snnCfg_p->stepNum = stepNum;
            snnCfg_p->stepCfg_p = new T_ssnStepCfg[stepNum]();
            //第一步激活
            snnCfg_p->stepCfg_p[0].stnAcsFlag = 0;
            //第一步请求帧
            u_char oneReqAPDUNum = 1;//第一步的请求帧数量
            snnCfg_p->stepCfg_p[0].reqAPDUNum = oneReqAPDUNum;
            snnCfg_p->stepCfg_p[0].reqBody_setNum_p = new u_char[oneReqAPDUNum]();
            snnCfg_p->stepCfg_p[0].reqBody_setNum_p[0] = 0;
            //snnCfg_p->stepCfg_p[0].reqEleVal_pp =  new ELE_TYPE[oneReqAPDUNum][11]();
            snnCfg_p->stepCfg_p[0].reqData_p =  new dataFrame[oneReqAPDUNum]();
            u_char memVal[11] = {};
            ELE_TYPE eleVal[11] =
            {
                {APDU_APCI_INDEX, 0, &memVal[0]},
                {ASDU_TID_INDEX, 1, &memVal[1]},
                {ASDU_LNG_INDEX, 0, &memVal[2]},
                {ASDU_VSQ_INDEX, 1, &memVal[3]},
                {ASDU_COT_INDEX, 1, &memVal[4]},
                {ASDU_ADDR_INDEX, 1, &memVal[5]},
                {BODY_TID_INDEX, 0, &memVal[6]},
                {BODY_ADDR_INDEX, 0, &memVal[7]},
                {BODY_SET_INDEX, 0, &memVal[8]},
                {BODY_TSP_INDEX, 0, &memVal[9]},
                {ASDU_TSP_INDEX, 0, &memVal[10]}
            };
            * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_140;
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_6;
            //调用接口给snnCfg_p第一步请求帧的第1帧赋值
            setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
            //第一步响应
            snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

            //第二步激活确认
            snnCfg_p->stepCfg_p[1].stnAcsFlag = 0;
            //第二步的请求帧
            snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
            //第二步的响应帧
            u_char twoRespAPDUNum = 1;//第二步的响应帧数量
            snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
            snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
            snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
            0;//第二步响应第1帧元素集个数
            //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
            snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_7;
            //调用接口给snnCfg_p第二步响应的第1帧赋值
            setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

            //3、配置默认的步号与tid,cot的对应关系
            ptclFunDsc->ssnDscNo = srvNo;
            ptclFunDsc->act_prio_chnlID = ACTIVE_TYPE;
            ptclFunDsc->usingMode = usingMode;
            ptclFunDsc->setNum = stepNum;//stepNum=2
            ptclFunDsc->remoteSet_p = new ptclStep[stepNum]();
            ptclFunDsc->remoteSet_p[0].TID_p = new u_short();
            ptclFunDsc->remoteSet_p[0].TID_p[0] = MC_TID_140;
            ptclFunDsc->remoteSet_p[0].COT = MC_COT_6;
            ptclFunDsc->remoteSet_p[0].MaxLen = MAX104_ASDU_LNG;
            ptclFunDsc->remoteSet_p[0].TidNum = 1;

            ptclFunDsc->remoteSet_p[1].TID_p = new u_short();
            ptclFunDsc->remoteSet_p[1].TID_p[0] = MC_TID_140;
            ptclFunDsc->remoteSet_p[1].COT = MC_COT_7;
            ptclFunDsc->remoteSet_p[1].MaxLen = MAX104_ASDU_LNG;
            ptclFunDsc->remoteSet_p[1].TidNum = 1;
#endif//#ifdef INTER_CREATERL_VAL

            break;
        }

        default:
            break;
    }

    return Fun_p ;

}




/******************************************************************************
 *功能 : 释放ONE_STEP对象空间
 *输入 : ONE_STEP对象指针
 *输出 : 无
 *返回 : 0 成功
 *修改历史
 *
*******************************************************************************/
EXPDLL int freeOneStep ( ONE_STEP* pSpace )
{
    //没有指针空间直接返回
    if ( pSpace == NULL )
    {
        return 0;
    }

    //处理ONE_STEP中除ONE_STEP*之外的成员指针
    if ( pSpace->reqAPDU_Obj_p != NULL )
    {
        delete [] pSpace->reqAPDU_Obj_p;
    }

    if ( pSpace->respAPDU_Obj_p != NULL )
    {
        delete [] pSpace->respAPDU_Obj_p;
    }

    if ( pSpace->sVerifyBlock_p != NULL )
    {
        if ( pSpace->sVerifyBlock_p->chkRslt_p != NULL )
        {
            delete  pSpace->sVerifyBlock_p->chkRslt_p;
        }

        delete  pSpace->sVerifyBlock_p;
    }

    //如果有ONE_STEP* nextStep成员则递归下一ONE_STEP成员节点
    if ( pSpace->nextStep_p != NULL )
    {
        freeOneStep ( pSpace->nextStep_p );
    }

    delete  pSpace;
    pSpace = NULL;

    return 0;
}




/******************************************************************************
 *功能 : 释放FUN_RL对象空间
 *输入 : FUN_RL对象指针
         T_snnCfg 对象指针
         T_ptclFunDsc 对象指针
 *输出 : 无
 *返回 : 0，成功
 *修改历史
 *
******************************************************************************/
EXPDLL int  freeRl_P ( FUN_RL* pSpace,
                       T_snnCfg* pSnnCfg,
                       T_ptclFunDsc* ptclFunDsc
                     )
{
    u_int vRet = 0;

    if ( pSpace != NULL ) //FUN_RL释放
    {
        //steps[i],i最大为 MAX_MODE_N
        for ( u_int i = 0; i < MAX_MODE_N; i++ )
        {
            if ( pSpace->steps_p[i] != NULL )
            {
                //调用freeOneStep递归删除 ONE_STEP
                vRet = freeOneStep ( pSpace->steps_p[i] );
            }
        }

        delete  pSpace;
        pSpace = NULL;
    }

#ifdef INTER_CREATERL_VAL

    if ( pSnnCfg != NULL ) //T_snnCfg释放
    {
        if ( pSnnCfg->stepCfg_p != NULL )
        {
            for ( u_int j = 0; j < pSnnCfg->stepNum; j++ )
            {

                //释放请求帧空间
                /*if ( pSnnCfg->stepCfg_p[j].reqEleVal_pp != NULL )
                {
                    for ( u_int k = 0;
                          k < pSnnCfg->stepCfg_p[j].reqAPDUNum;
                          k++
                        )
                    {
                        for ( u_int k1 = APDU_APCI_INDEX;
                              k1<=ASDU_TSP_INDEX;
                              k1++
                            )
                        {
                            ELE_TYPE* tmp_p = &(
                            pSnnCfg->stepCfg_p[j].reqEleVal_pp[k][k1] );
                            if ( tmp_p->p_Mem != NULL)
                            {
                                delete [] tmp_p->p_Mem;
                                tmp_p->p_Mem = NULL;
                            }
                        }
                    }
                    delete [] pSnnCfg->stepCfg_p[j].reqEleVal_pp;
                    pSnnCfg->stepCfg_p[j].reqEleVal_pp = NULL;
                }*/
                if ( pSnnCfg->stepCfg_p[j].reqData_p != NULL )
                {
                    for ( u_int k = 0;
                          k < pSnnCfg->stepCfg_p[j].reqAPDUNum;
                          k++
                        )
                    {
                        ELE_TYPE* tmp_p = NULL;
                        tmp_p = & (
                                pSnnCfg->stepCfg_p[j].reqData_p[k].asduTid );

                        if ( tmp_p != NULL && tmp_p->p_Mem != NULL )
                        {
                            delete [] tmp_p->p_Mem;
                            tmp_p->p_Mem = NULL;
                        }

                        tmp_p = & (
                                pSnnCfg->stepCfg_p[j].reqData_p[k].asduLng );

                        if ( tmp_p != NULL && tmp_p->p_Mem != NULL )
                        {
                            delete [] tmp_p->p_Mem;
                            tmp_p->p_Mem = NULL;
                        }

                        tmp_p = & (
                                pSnnCfg->stepCfg_p[j].reqData_p[k].asduVsq );

                        if ( tmp_p != NULL && tmp_p->p_Mem != NULL )
                        {
                            delete [] tmp_p->p_Mem;
                            tmp_p->p_Mem = NULL;
                        }

                        tmp_p = & (
                                pSnnCfg->stepCfg_p[j].reqData_p[k].asduCot );

                        if ( tmp_p != NULL && tmp_p->p_Mem != NULL )
                        {
                            delete [] tmp_p->p_Mem;
                            tmp_p->p_Mem = NULL;
                        }

                        tmp_p = & (
                                pSnnCfg->stepCfg_p[j].reqData_p[k].asduAddr );

                        if ( tmp_p != NULL && tmp_p->p_Mem != NULL )
                        {
                            delete [] tmp_p->p_Mem;
                            tmp_p->p_Mem = NULL;
                        }

                        for ( u_char k1 = 0;
                              pSnnCfg->stepCfg_p[j].reqBody_setNum_p != NULL
                              && 　k1 < pSnnCfg->stepCfg_p[j].reqBody_setNum_p[k]
                              && pSnnCfg->stepCfg_p[j].reqData_p[k].bodyObj_p != NULL;
                              k1++
                            )
                        {
                            tmp_p = & (
                                    pSnnCfg->stepCfg_p[j].reqData_p[k].bodyObj_p[k1] );

                            if ( tmp_p != NULL && tmp_p->p_Mem != NULL )
                            {
                                delete [] tmp_p->p_Mem;
                                tmp_p->p_Mem = NULL;
                            }
                        }

                        if ( pSnnCfg->stepCfg_p[j].reqData_p[k].bodyObj_p != NULL )
                        {
                            delete [] pSnnCfg->stepCfg_p[j].reqData_p[k].bodyObj_p;
                            pSnnCfg->stepCfg_p[j].reqData_p[k].bodyObj_p = NULL;
                        }

                        tmp_p = & (
                                pSnnCfg->stepCfg_p[j].reqData_p[k].asduTsp );

                        if ( tmp_p != NULL && tmp_p->p_Mem != NULL )
                        {
                            delete [] tmp_p->p_Mem;
                            tmp_p->p_Mem = NULL;
                        }
                    }

                    delete [] pSnnCfg->stepCfg_p[j].reqData_p;
                    pSnnCfg->stepCfg_p[j].reqData_p = NULL;
                }

                if ( pSnnCfg->stepCfg_p[j].reqBody_setNum_p != NULL )
                {
                    delete [] pSnnCfg->stepCfg_p[j].reqBody_setNum_p;
                    pSnnCfg->stepCfg_p[j].reqBody_setNum_p = NULL;
                }


                //释放响应帧空间
                /*if ( pSnnCfg->stepCfg_p[j].respEleVal_pp != NULL )
                {
                    for ( u_int t = 0;
                          t < pSnnCfg->stepCfg_p[j].respAPDUNum;
                          t++
                        )
                    {
                        for ( u_int t1 = APDU_APCI_INDEX;
                              t1<=ASDU_TSP_INDEX;
                              t1++
                            )
                        {
                            ELE_TYPE* tmp_p = &(
                                pSnnCfg->stepCfg_p[j].respEleVal_pp[t][t1] );
                            if ( tmp_p->p_Mem != NULL)
                            {
                                delete [] tmp_p->p_Mem;
                                tmp_p->p_Mem = NULL;
                            }
                        }
                    }
                    delete [] pSnnCfg->stepCfg_p[j].respEleVal_pp;
                    pSnnCfg->stepCfg_p[j].respEleVal_pp = NULL;
                }*/

                if ( pSnnCfg->stepCfg_p[j].respData_p != NULL )
                {
                    for ( u_int t = 0;
                          t < pSnnCfg->stepCfg_p[j].respAPDUNum;
                          t++
                        )
                    {
                        ELE_TYPE* tmp_p = NULL;
                        tmp_p = & (
                                pSnnCfg->stepCfg_p[j].respData_p[k].asduTid );

                        if ( tmp_p != NULL && tmp_p->p_Mem != NULL )
                        {
                            delete [] tmp_p->p_Mem;
                            tmp_p->p_Mem = NULL;
                        }

                        tmp_p = & (
                                pSnnCfg->stepCfg_p[j].respData_p[k].asduLng );

                        if ( tmp_p != NULL && tmp_p->p_Mem != NULL )
                        {
                            delete [] tmp_p->p_Mem;
                            tmp_p->p_Mem = NULL;
                        }

                        tmp_p = & (
                                pSnnCfg->stepCfg_p[j].respData_p[k].asduVsq );

                        if ( tmp_p != NULL && tmp_p->p_Mem != NULL )
                        {
                            delete [] tmp_p->p_Mem;
                            tmp_p->p_Mem = NULL;
                        }

                        tmp_p = & (
                                pSnnCfg->stepCfg_p[j].respData_p[k].asduCot );

                        if ( tmp_p != NULL && tmp_p->p_Mem != NULL )
                        {
                            delete [] tmp_p->p_Mem;
                            tmp_p->p_Mem = NULL;
                        }

                        tmp_p = & (
                                pSnnCfg->stepCfg_p[j].respData_p[k].asduAddr );

                        if ( tmp_p != NULL && tmp_p->p_Mem != NULL )
                        {
                            delete [] tmp_p->p_Mem;
                            tmp_p->p_Mem = NULL;
                        }

                        for ( u_int t1 = APDU_APCI_INDEX;
                              pSnnCfg->stepCfg_p[j].respBody_setNum_p != NULL
                              && t1 < pSnnCfg->stepCfg_p[j].respBody_setNum_p[k]
                              && pSnnCfg->stepCfg_p[j].respData_p[t].bodyObj_p != NULL;
                              t1++
                            )
                        {
                            tmp_p = & (
                                    pSnnCfg->stepCfg_p[j].respData_p[t].bodyObj_p[t1] );

                            if ( tmp_p != NULL && tmp_p->p_Mem != NULL )
                            {
                                delete [] tmp_p->p_Mem;
                                tmp_p->p_Mem = NULL;
                            }
                        }
                    }

                    delete [] pSnnCfg->stepCfg_p[j].respData_p;
                    pSnnCfg->stepCfg_p[j].respData_p = NULL;
                }

                if ( pSnnCfg->stepCfg_p[j].respBody_setNum_p != NULL )
                {
                    delete [] pSnnCfg->stepCfg_p[j].respBody_setNum_p;
                    pSnnCfg->stepCfg_p[j].respBody_setNum_p = NULL;
                }
            }//end for stepNum

            delete [] pSnnCfg->stepCfg_p;
            pSnnCfg->stepCfg_p = NULL;
            pSnnCfg->stepNum = 0;

        }//end pSnnCfg->stepCfg_p

        //delete [] pSnnCfg;//pSnnCfg 不在 createRl_P 中创建，所以不在此释放
    }

    if ( ptclFunDsc != NULL ) //T_ptclFunDsc 释放
    {
        if ( ptclFunDsc->remoteSet_p != NULL )
        {
            for ( int i = 0; i < ptclFunDsc->setNum; i++ )
            {
                if ( ptclFunDsc->remoteSet_p[i].TID_p != NULL )
                {
                    delete [] ptclFunDsc->remoteSet_p[i].TID_p;
                    ptclFunDsc->remoteSet_p[i].TID_p = NULL;
                }

                if ( ptclFunDsc->remoteSet_p[i].TIDLng_p != NULL )
                {
                    delete [] ptclFunDsc->remoteSet_p[i].TIDLng_p;
                    ptclFunDsc->remoteSet_p[i].TIDLng_p = NULL;
                }
            }

            delete [] ptclFunDsc->remoteSet_p;
            ptclFunDsc->remoteSet_p = NULL;
            ptclFunDsc->setNum = 0;
        }

        //delete [] ptclFunDsc;//ptclFunDsc不在 createRl_P 中创建，所以不在此释放
    }

#endif//#ifdef INTER_CREATERL_VAL

    return vRet;
}




EXPDLL int getGLoadPnt ( T_snnCfg* stnSnnCfg_p,
                         u_char stepNo,
                         T_snnPara* stnSnnPara_p
                       )
{
    if ( stnSnnCfg_p == NULL
         || stnSnnCfg_p->stepCfg_p == NULL
         || stnSnnPara_p == NULL
         || stnSnnPara_p->ASDUID_p == NULL
         || stnSnnPara_p->usrData_p == NULL
       )
    {
        return ERR_POINTER_NULL;
    }

    if ( stnSnnCfg_p->stepNum < stepNo )
    {
        return -1;
    }

    int tRet = 0;
    T_ssnStepCfg* tStepCfg_p = & ( stnSnnCfg_p->stepCfg_p[stepNo - 1] );

    //统计所有帧中信息体的个数
    int setNum = 0;
    u_int pntAddr = 0;
    T_snnUsrData* tUsrData_p = stnSnnPara_p->usrData_p + setNum;

    if ( stnSnnPara_p->actFlag == ACTIVE_TYPE )
    {
        stnSnnPara_p->pntNum     = 0;
        stnSnnPara_p->pntPos     = 0;
        stnSnnPara_p->stepNo     = stepNo;
        stnSnnPara_p->infoType   = stnSnnCfg_p->infoType;
        stnSnnPara_p->cmnAddr    = stnSnnCfg_p->cmnAddr;
        stnSnnPara_p->stnAcsFlag = tStepCfg_p->stnAcsFlag;
        stnSnnPara_p->ASDUIDNum  = tStepCfg_p->reqAPDUNum ;

        for ( int i = 0;
              i < tStepCfg_p->reqAPDUNum;
              i++
            ) //遍历所有请求帧
        {
            if ( tStepCfg_p->reqData_p == NULL
                 || tStepCfg_p->reqBody_setNum_p == NULL
                 //|| tStepCfg_p->reqEleVal_pp[i] ==NULL
               )
            {
                return ERR_POINTER_NULL;
            }

            //类型标识
            ASDU_TID* tid_p = & (
                              tStepCfg_p->reqData_p[i].asduTid );
            memcpy ( &stnSnnPara_p->ASDUID_p[i],
                     tid_p->p_Mem,
                     tid_p->lng
                   );
            u_int tmpTID = stnSnnPara_p->ASDUID_p[i];

            //可变结构体限定词
            ASDU_VSQ* vsq_p = & (
                              tStepCfg_p->reqData_p[i].asduVsq );
            memcpy ( &stnSnnPara_p->acsMthd,
                     vsq_p->p_Mem,
                     vsq_p->lng
                   );

            ASDU_VSQ_VAL* tmpVsqVal_p = ( ASDU_VSQ_VAL* ) & (
                                        stnSnnPara_p->acsMthd );
            //可变结构限定词中点数与配置的点数不相等则报错
            /*if (tmpVsqVal_p->N != tStepCfg_p->reqBody_setNum_p[i])
            {
                return -2;
            }*/

            //传送原因
            ASDU_COT* cot_p = & (
                              tStepCfg_p->reqData_p[i].asduCot );
            memcpy ( &stnSnnPara_p->COT,
                     cot_p->p_Mem,
                     cot_p->lng
                   );

            //ASDU公共地址
            ASDU_ADDR* addr_p = & (
                                tStepCfg_p->reqData_p[i].asduAddr );
            memcpy ( &stnSnnPara_p->cmnAddr,
                     addr_p->p_Mem,
                     addr_p->lng
                   );

            //信息对象
            BODY_OBJ* bodyObj_p =  tStepCfg_p->reqData_p[i].bodyObj_p;

            //tStepCfg_p->reqBody_setNum_p[i] 信息对象个数
            if ( tStepCfg_p->reqBody_setNum_p[i] > 0
                 && bodyObj_p == NULL
               )
            {
                return ERR_POINTER_NULL;
            }

            T_tidObjLng tTidObjLng;
            memset ( &tTidObjLng, 0, sizeof ( tTidObjLng ) );
            tRet = getTidObjLng ( ( u_short ) tmpTID,
                                  &tTidObjLng
                                );

            if ( tRet != 0 )
            {
                return tRet;
            }

            for ( u_char j = 0;
                  j < tStepCfg_p->reqBody_setNum_p[i];
                  j++
                )
            {
                //信息体地址
                BODY_ADDR* body_addr_p = NULL;
                //信息元素集
                BODY_SET* body_set_p = NULL;
                //信息体时标
                BODY_TSP* body_tsp_p = NULL;
                body_addr_p = & (
                              tStepCfg_p->reqData_p[i].bodyObj_p[j].body_Addr );
                body_set_p = & (
                             tStepCfg_p->reqData_p[i].bodyObj_p[j].body_Set );
                body_tsp_p = & (
                             tStepCfg_p->reqData_p[i].bodyObj_p[j].body_Tsp );

                //判断配置的元素长度是否超过系统支持的最大长度
                u_short pntVal1lng = 0;

                if ( tmpVsqVal_p->N > 0
                     && body_set_p != NULL
                     && body_set_p->lng > 0
                   )
                {
                    /*注：body_set_p->lng为body_set_p->p_Mem中
                        单个元素的长度而不是整个元素集的长度
                        元素集的个数由可变结构限定词的 N 决定
                    */
                    pntVal1lng = sizeof (
                                 tUsrData_p->gData.val_chkStd.pntVal1 );

                    if ( body_set_p->lng > pntVal1lng * 2 )
                    {
                        //要加载的点值长度大于接收变量pntVal1 则异常
                        return -3;
                    }
                }

                //取信息体地址的值
                pntAddr = 0;

                if ( body_addr_p != NULL
                     && body_addr_p->lng > 0
                   )
                {
                    memcpy ( & ( tUsrData_p->gData.val_chkStd.pntAddr ),
                             body_addr_p->p_Mem,
                             body_addr_p->lng
                           );
                    pntAddr = tUsrData_p->gData.val_chkStd.pntAddr;
                }

                //取信息体时标的值
                if ( body_tsp_p != NULL
                     && body_tsp_p->p_Mem != NULL
                     && body_tsp_p->lng > 0
                   )
                {
                    if ( tUsrData_p->gData.tmStmp_p == NULL )
                    {
                        return ERR_POINTER_NULL;
                    }

                    memcpy ( tUsrData_p->gData.tmStmp_p,
                             body_tsp_p->p_Mem,
                             body_tsp_p->lng
                           );

                    //信息对象时标类型
                    if ( body_tsp_p->lng == sizeof ( CP56TIME2a ) )
                    {
                        tUsrData_p->gData.tmStmpType = TIME_TYPE_56_2A;
                    }
                    else if ( body_tsp_p->lng == sizeof ( CP24TIME2a ) )
                    {
                        tUsrData_p->gData.tmStmpType = TIME_TYPE_24_2A;
                    }
                    else
                    {
                        return ERR_TIME_TYPE;
                    }
                }


                //取点值
                for ( u_char k = 0;
                      k < tmpVsqVal_p->N;
                      k++
                    )
                {
                    tUsrData_p->gData.pntType = 0;

                    if ( k > 0 )
                    {
                        if ( tmpVsqVal_p->SQ == 1 )
                        {
                            /*从第二个点开始赋值，第一个点的地址在for之前赋值
                            SQ:=1 单个信息元素的序列由信息对象地址来寻址：信息
                            对象地址是顺序单个元素的第一个信息元素地址，后继单个
                            信息元素是从这个地址顺序加1
                            */
                            tUsrData_p->gData.val_chkStd.pntAddr = pntAddr + k;
                        }
                        else//tmpVsqVal_p->SQ == 0
                        {
                            /*SQ:=0 多个信息对象结构，每个信息对象有自己单独的地址，
                            每个对象的元素集中只有一个元素，有自己单独的信息体时标
                            */
                            tUsrData_p->gData.val_chkStd.pntAddr = pntAddr;
                        }

                    }



                    /*stnAcsFlag 第2位为1时 点值不在此加载
                        stnAcsFlag 第2位为0时 需要加载的不是
                                    点值需要在此加载如
                    */
                    if ( 0 == ( ( stnSnnPara_p->stnAcsFlag ) & 0x2 )
                         && body_set_p != NULL
                         && body_set_p->lng > 0
                       )
                    {
                        //if ( body_set_p->lng > tTidObjLng.tidValLng )
                        if ( tTidObjLng.tidValLng > pntVal1lng )
                        {
                            memcpy ( & ( tUsrData_p->gData.val_chkStd.pntVal1 ),
                                     body_set_p->p_Mem + ( k * body_set_p->lng ),
                                     pntVal1lng
                                   );
                            //把多的值放到高4字节
                            memcpy ( & ( tUsrData_p->gData.val_chkStd.pntVal2 ),
                                     ( body_set_p->p_Mem
                                       + ( k * body_set_p->lng ) + pntVal1lng ),
                                     tTidObjLng.tidValLng - pntVal1lng
                                   );
                            //tUsrData_p->gData.pntType = (
                            //                            ( u_char ) body_set_p->lng /*PNTTYPE_HIGH_SET*/
                            //                            );
                        }
                        else
                        {
                            memcpy ( & ( tUsrData_p->gData.val_chkStd.pntVal1 ),
                                     body_set_p->p_Mem + ( k * body_set_p->lng ),
                                     tTidObjLng.tidValLng
                                   );
                        }

                        //品质描述词
                        if ( tTidObjLng.cfgLng.qdsLng > 0
                             && tTidObjLng.qdsMixFlag == 0
                             && body_set_p->lng > tTidObjLng.tidValLng
                           )
                        {
                            memcpy ( & ( tUsrData_p->gData.dDsc ),
                                     ( body_set_p->p_Mem
                                       + ( k * body_set_p->lng ) 
                                       + tTidObjLng.tidValLng ),
                                     tTidObjLng.cfgLng.qdsLng
                                   );
                        }
                        else if ( tTidObjLng.cfgLng.qdsLng > 0
                                  && tTidObjLng.qdsMixFlag == 1
                                  && body_set_p->lng > 0
                                )
                        {
                            memcpy ( & ( tUsrData_p->gData.dDsc ),
                                     ( body_set_p->p_Mem
                                       + ( k * body_set_p->lng )  ),
                                     tTidObjLng.cfgLng.qdsLng
                                   );
                        }

                    }

                    /*根据TID判断对应元素值长
                        度是否大于4字节，如果大
                        于4字节需要把多的值放到
                        高4字节
                    */
                    /*if ( OVER4TID ( tmpTID ) )
                    {
                    tUsrData_p->gData.pntType = PNTTYPE_HIGH_SET;
                    }*/
                    tUsrData_p->gData.pntType = tTidObjLng.tidValLng;

                    /*有时标则把时标赋予对应的usrData_p结构中
                        第一个点的时标在for之前赋值
                    */
                    if ( body_tsp_p != NULL
                         && body_tsp_p->p_Mem != NULL
                         && body_tsp_p->lng > 0
                         && k > 0
                       )
                    {
                        if ( tUsrData_p->gData.tmStmp_p == NULL )
                        {
                            return ERR_POINTER_NULL;
                        }

                        memcpy ( tUsrData_p->gData.tmStmp_p,
                                 body_tsp_p->p_Mem,
                                 body_tsp_p->lng
                               );

                        //信息对象时标类型
                        if ( body_tsp_p->lng == sizeof ( CP56TIME2a ) )
                        {
                            tUsrData_p->gData.tmStmpType = TIME_TYPE_56_2A;
                        }
                        else if ( body_tsp_p->lng == sizeof ( CP24TIME2a ) )
                        {
                            tUsrData_p->gData.tmStmpType = TIME_TYPE_24_2A;
                        }
                        else
                        {
                            return ERR_TIME_TYPE;
                        }
                    }

                    stnSnnPara_p->pntNum++;
                    setNum ++;
                    tUsrData_p = stnSnnPara_p->usrData_p + setNum;
                }

                if ( tmpVsqVal_p->N == 0 ) //信息体没有元素集
                {
                    tUsrData_p->gData.pntType = PNTTYPE_NULL_SET;
                    stnSnnPara_p->pntNum++;
                    setNum ++;
                    tUsrData_p = stnSnnPara_p->usrData_p + setNum;
                }
            }//end for 对信息体的赋值
        }//end for 所有请求帧
    }//end ACTIVE_TYPE
    else //PASSIVE_TYPE
    {
        stnSnnPara_p->pntNum       = 0;
        stnSnnPara_p->pntPos       = 0;
        stnSnnPara_p->stepNo       = stepNo;
        stnSnnPara_p->infoType     = stnSnnCfg_p->infoType;
        stnSnnPara_p->cmnAddr      = stnSnnCfg_p->cmnAddr;
        stnSnnPara_p->stnAcsFlag   = tStepCfg_p->stnAcsFlag;
        stnSnnPara_p->ASDUIDNum    = tStepCfg_p->respAPDUNum;

        for ( int i = 0;
              i < tStepCfg_p->respAPDUNum;
              i++
            ) //遍历所有响应帧
        {
            if ( tStepCfg_p->respData_p == NULL
                 || tStepCfg_p->respBody_setNum_p == NULL
               )
            {
                return ERR_POINTER_NULL;
            }

            //类型标识
            ASDU_TID* tid_p = & (
                              tStepCfg_p->respData_p[i].asduTid );
            memcpy ( &stnSnnPara_p->ASDUID_p[i],
                     tid_p->p_Mem,
                     tid_p->lng
                   );
            u_int tmpTID = stnSnnPara_p->ASDUID_p[i];

            //可变结构体限定词
            ASDU_VSQ* vsq_p = & (
                              tStepCfg_p->respData_p[i].asduVsq );
            memcpy ( &stnSnnPara_p->acsMthd,
                     vsq_p->p_Mem,
                     vsq_p->lng
                   );

            ASDU_VSQ_VAL* tmpVsqVal_p = ( ASDU_VSQ_VAL* ) & (
                                        stnSnnPara_p->acsMthd );
            //可变结构限定词中点数与配置的点数不相等则报错
            /*if (tmpVsqVal_p->N != tStepCfg_p->respBody_setNum_p[i])
            {
                return -2;
            }*/

            //传送原因
            ASDU_COT* cot_p = & (
                              tStepCfg_p->respData_p[i].asduCot );
            memcpy ( &stnSnnPara_p->COT,
                     cot_p->p_Mem,
                     cot_p->lng
                   );

            //ASDU公共地址
            ASDU_ADDR* addr_p = & (
                                tStepCfg_p->respData_p[i].asduAddr );
            memcpy ( &stnSnnPara_p->cmnAddr,
                     addr_p->p_Mem,
                     addr_p->lng
                   );

            //信息对象
            BODY_OBJ* bodyObj_p =  tStepCfg_p->respData_p[i].bodyObj_p;

            //tStepCfg_p->respBody_setNum_p[i] 信息对象个数
            if ( tStepCfg_p->respBody_setNum_p[i] > 0
                 && bodyObj_p == NULL
               )
            {
                return ERR_POINTER_NULL;
            }

            T_tidObjLng tTidObjLng;
            memset ( &tTidObjLng, 0, sizeof ( tTidObjLng ) );
            tRet = getTidObjLng ( ( u_short ) tmpTID,
                &tTidObjLng
                );

            if ( tRet != 0 )
            {
                return tRet;
            }

            for ( u_char j = 0;
                  j < tStepCfg_p->respBody_setNum_p[i];
                  j++
                )
            {
                //信息体地址
                BODY_ADDR* body_addr_p = NULL;
                //信息元素集
                BODY_SET* body_set_p = NULL;
                //信息体时标
                BODY_TSP* body_tsp_p = NULL;
                body_addr_p = & (
                              tStepCfg_p->respData_p[i].bodyObj_p[j].body_Addr );
                body_set_p = & (
                             tStepCfg_p->respData_p[i].bodyObj_p[j].body_Set );
                body_tsp_p = & (
                             tStepCfg_p->respData_p[i].bodyObj_p[j].body_Tsp );

                //判断配置的元素长度是否超过系统支持的最大长度
                u_short pntVal1lng = 0;

                if ( tmpVsqVal_p->N > 0
                     && body_set_p != NULL
                     && body_set_p->lng > 0
                   )
                {
                    /*注：body_set_p->lng为body_set_p->p_Mem中
                        单个元素的长度而不是整个元素集的长度
                        元素集的个数由可变结构限定词的 N 决定
                    */
                    pntVal1lng = sizeof (
                                 tUsrData_p->gData.val_chkStd.pntVal1 );

                    if ( body_set_p->lng > pntVal1lng * 2 )
                    {
                        //要加载的点值长度大于接收变量pntVal1 则异常
                        return -3;
                    }
                }

                //取信息体地址的值
                pntAddr = 0;

                if ( body_addr_p != NULL
                     && body_addr_p->lng > 0
                   )
                {
                    memcpy ( & ( tUsrData_p->gData.val_chkStd.pntAddr ),
                             body_addr_p->p_Mem,
                             body_addr_p->lng
                           );
                    pntAddr = tUsrData_p->gData.val_chkStd.pntAddr;
                }

                //取信息体时标的值
                if ( body_tsp_p != NULL
                     && body_tsp_p->p_Mem != NULL
                     && body_tsp_p->lng > 0
                   )
                {
                    if ( tUsrData_p->gData.tmStmp_p == NULL )
                    {
                        return ERR_POINTER_NULL;
                    }

                    memcpy ( tUsrData_p->gData.tmStmp_p,
                             body_tsp_p->p_Mem,
                             body_tsp_p->lng
                           );

                    //信息对象时标类型
                    if ( body_tsp_p->lng == sizeof ( CP56TIME2a ) )
                    {
                        tUsrData_p->gData.tmStmpType = TIME_TYPE_56_2A;
                    }
                    else if ( body_tsp_p->lng == sizeof ( CP24TIME2a ) )
                    {
                        tUsrData_p->gData.tmStmpType = TIME_TYPE_24_2A;
                    }
                    else
                    {
                        return ERR_TIME_TYPE;
                    }
                }


                //取点值
                for ( u_char k = 0;
                      k < tmpVsqVal_p->N;
                      k++
                    )
                {
                    tUsrData_p->gData.pntType = 0;

                    if ( k > 0 )
                    {
                        if ( tmpVsqVal_p->SQ == 1 )
                        {
                            /*从第二个点开始赋值，第一个点的地址在for之前赋值
                            SQ:=1 单个信息元素的序列由信息对象地址来寻址：信息
                            对象地址是顺序单个元素的第一个信息元素地址，后继单个
                            信息元素是从这个地址顺序加1
                            */
                            tUsrData_p->gData.val_chkStd.pntAddr = pntAddr + k;
                        }
                        else//tmpVsqVal_p->SQ == 0
                        {
                            /*SQ:=0 多个信息对象结构，每个信息对象有自己单独的地址，
                            每个对象的元素集中只有一个元素，有自己单独的信息体时标
                            */
                            tUsrData_p->gData.val_chkStd.pntAddr = pntAddr;
                        }

                    }



                    /*stnAcsFlag 第2位为1时 点值不在此加载
                        stnAcsFlag 第2位为0时 需要加载的不是
                                    点值需要在此加载如
                    */
                    if ( 0 == ( ( stnSnnPara_p->stnAcsFlag ) & 0x2 )
                         && body_set_p != NULL
                         && body_set_p->lng > 0
                       )
                    {
                        //if ( body_set_p->lng > tTidObjLng.tidValLng )
                        if ( tTidObjLng.tidValLng > pntVal1lng )
                        {
                            memcpy ( & ( tUsrData_p->gData.val_chkStd.pntVal1 ),
                                     body_set_p->p_Mem + ( k * body_set_p->lng ),
                                     pntVal1lng
                                   );
                            //把多的值放到高4字节
                            memcpy ( & ( tUsrData_p->gData.val_chkStd.pntVal2 ),
                                     ( body_set_p->p_Mem
                                       + ( k * body_set_p->lng ) + pntVal1lng ),
                                     tTidObjLng.tidValLng - pntVal1lng
                                   );
                            //tUsrData_p->gData.pntType = PNTTYPE_HIGH_SET;
                        }
                        else
                        {
                            memcpy ( & ( tUsrData_p->gData.val_chkStd.pntVal1 ),
                                     body_set_p->p_Mem + ( k * body_set_p->lng ),
                                     tTidObjLng.tidValLng
                                   );
                        }

                        //品质描述词
                        if ( tTidObjLng.cfgLng.qdsLng > 0
                             && tTidObjLng.qdsMixFlag == 0
                             && body_set_p->lng > tTidObjLng.tidValLng
                           )
                        {
                            memcpy ( & ( tUsrData_p->gData.dDsc ),
                                     ( body_set_p->p_Mem
                                       + ( k * body_set_p->lng ) 
                                       + tTidObjLng.tidValLng ),
                                     tTidObjLng.cfgLng.qdsLng
                                   );
                        }
                        else if ( tTidObjLng.cfgLng.qdsLng > 0
                                  && tTidObjLng.qdsMixFlag == 1
                                  && body_set_p->lng > 0
                                )
                        {
                            memcpy ( & ( tUsrData_p->gData.dDsc ),
                                     ( body_set_p->p_Mem
                                       + ( k * body_set_p->lng )  ),
                                     tTidObjLng.cfgLng.qdsLng
                                   );
                        }
                    }

                    /*根据TID判断对应元素值长
                        度是否大于4字节，如果大
                        于4字节需要把多的值放到
                        高4字节
                    */
                    /*if ( OVER4TID ( tmpTID ) )
                    {
                        tUsrData_p->gData.pntType = PNTTYPE_HIGH_SET;
                    }*/
                    tUsrData_p->gData.pntType = tTidObjLng.tidValLng;

                    /*有时标则把时标赋予对应的usrData_p结构中
                        第一个点的时标在for之前赋值
                    */
                    if ( body_tsp_p != NULL
                         && body_tsp_p->p_Mem != NULL
                         && body_tsp_p->lng > 0
                         && k > 0
                       )
                    {
                        if ( tUsrData_p->gData.tmStmp_p == NULL )
                        {
                            return ERR_POINTER_NULL;
                        }

                        memcpy ( tUsrData_p->gData.tmStmp_p,
                                 body_tsp_p->p_Mem,
                                 body_tsp_p->lng
                               );

                        //信息对象时标类型
                        if ( body_tsp_p->lng == sizeof ( CP56TIME2a ) )
                        {
                            tUsrData_p->gData.tmStmpType = TIME_TYPE_56_2A;
                        }
                        else if ( body_tsp_p->lng == sizeof ( CP24TIME2a ) )
                        {
                            tUsrData_p->gData.tmStmpType = TIME_TYPE_24_2A;
                        }
                        else
                        {
                            return ERR_TIME_TYPE;
                        }
                    }

                    stnSnnPara_p->pntNum++;
                    setNum ++;
                    tUsrData_p = stnSnnPara_p->usrData_p + setNum;
                }

                if ( tmpVsqVal_p->N == 0 ) //信息体没有元素集
                {
                    tUsrData_p->gData.pntType = PNTTYPE_NULL_SET;
                    stnSnnPara_p->pntNum++;
                    setNum ++;
                    tUsrData_p = stnSnnPara_p->usrData_p + setNum;
                }
            }//end for 对信息体的赋值

        }//end for 所有响应帧
    }//end PASSIVE_TYPE

    return 0;

}




/******************************************************************************
  *功能：步驱动，设置起始帧对应的步
  *输入：
  *      srvNo            ：功能码
  *      usingMode        :模式码
  *      preStepRul_p     ：当前功能步，其中reqPDU_Num为当前功能步已完成的帧编号
  *      statrtStepStnD_p :会话步需要的数据信息（详见此类型说明）
  *输出：
  *      preStepRul_p     :设置完成起始帧后对应的步信息
  *返回：
  *      =0               :接口没有逻辑直接返回
  *      >0               :会话状态
  *      0xffff           :该功能不支持
  *      <0               :接口异常
  *修改历史
  *
*******************************************************************************/

EXPDLL int startStep ( u_short ssnNo,
                       u_char usingNode,
                       ONE_STEP* preStepRul_p,
                       T_snnPara* statrtStepStnD_p
                     )
{
    if ( preStepRul_p == NULL
         || statrtStepStnD_p == NULL
       )
    {
        return ERR_POINTER_NULL;
    }

    int tmpRet               = 0; //下层函数返回值
    int nxtStepStat          = 0; //下一帧会话状态
    u_char nxtFrmStepNo      = 1; //下一帧步号
    ONE_STEP tmpNowRlStep    = {};//临时规则步变量
    T_ptclFunDsc tmpFuncDsc  = {};//临时会话号与主动性变量

    /*APDU104*         apdu104_p = NULL;
    ASDU_TID_VAL     tmpTID = 0;
    ASDU_VSQ_VAL     vsqVal ={};
    ASDU_COT1_VAL   cot1Val = {};
    ASDU_ADDR_VAL   addrVal = {};
    BODY_ADDR_VAL bodAddVal = {};
    u_char           bodSet = 0;
    u_short      bodSetLng = 0;*/

    //preStepRul_p->stepNo = 0;
    tmpNowRlStep.stepNo = 1;
    tmpFuncDsc.ssnDscNo = ssnNo;

    switch ( ssnNo ) //判断功能;
    {
        case STN_INIT://站初始化;
        {
            break;
        }

        case QUE_DATA://查询数据;
        {
            //调用接口asgmtNxtFrmPar进行装帧操作
            tmpNowRlStep.reqPDU_FCode     = I_F;
            tmpFuncDsc.act_prio_chnlID     = ACTIVE_TYPE;
            tmpRet = asgmtNxtFrmPar ( &tmpNowRlStep,
                                      preStepRul_p,
                                      &nxtFrmStepNo,
                                      &tmpFuncDsc,
                                      statrtStepStnD_p
                                    );

            if ( tmpRet < 0 )
            {
                return tmpRet;
            }

            //发送等待验收;
            nxtStepStat = MC_SEND_WAIT_CHECK;
            break;
        }

        case CYC_TRNS://周期传输;
        {
            tmpNowRlStep.respPDU_FCode     = I_F;
            tmpFuncDsc.act_prio_chnlID     = PASSIVE_TYPE;
            tmpRet = asgmtNxtFrmPar ( &tmpNowRlStep,
                                      preStepRul_p,
                                      &nxtFrmStepNo,
                                      &tmpFuncDsc,
                                      statrtStepStnD_p
                                    );

            if ( tmpRet < 0 )
            {
                return tmpRet;
            }

            //发送会话完成;
            nxtStepStat = MC_SEND_SSN_FNSH;
            break;
        }

        case ACQ_EVT://事件收集;
        {
            tmpNowRlStep.respPDU_FCode     = I_F;
            tmpFuncDsc.act_prio_chnlID     = PASSIVE_TYPE;
            tmpRet = asgmtNxtFrmPar ( &tmpNowRlStep,
                                      preStepRul_p,
                                      &nxtFrmStepNo,
                                      &tmpFuncDsc,
                                      statrtStepStnD_p
                                    );

            if ( tmpRet < 0 )
            {
                return tmpRet;
            }

            //发送会话完成;
            nxtStepStat = MC_SEND_SSN_FNSH;
            break;
        }

        case QUKCHK_EVT: //快检事件;
        {
            //不使用
            nxtStepStat = 0xffff;
            break;
        }

        case STN_INRGTN: //站召唤
        {
            tmpNowRlStep.reqPDU_FCode     = I_F;
            tmpFuncDsc.act_prio_chnlID     = ACTIVE_TYPE;
            tmpRet = asgmtNxtFrmPar ( &tmpNowRlStep,
                                      preStepRul_p,
                                      &nxtFrmStepNo,
                                      &tmpFuncDsc,
                                      statrtStepStnD_p
                                    );

            if ( tmpRet < 0 )
            {
                return tmpRet;
            }

            //preStepRul_p->stepNo        = 1;
            //preStepRul_p->reqPDU_FCode  = I_F;
            ////注：此值在上层中进行改变不在此改变
            ////preStepRul_p->reqPDU_Num    = 1;
            //preStepRul_p->sVerifyBlock_p = NULL;

            //tmpTID = MC_TID_100;
            //vsqVal.SQ = 0;
            //vsqVal.N = 1;
            //cot1Val.CAUSE = MC_COT_6;
            //addrVal.shortAddr = 1;
            //bodAddVal.shortAddr = 0;
            //bodSet = 20;//QCC 站召唤
            //bodSetLng = 1;

            ////根据值组装ASDU
            //apdu104_p = (APDU104*)preStepRul_p->reqAPDU_Obj_p;
            //tmpRet    = packASDU(apdu104_p,
            //                      &tmpTID,
            //                      &vsqVal,
            //                      &cot1Val,
            //                      NULL,
            //                      &addrVal,
            //                      &bodAddVal,
            //                      &bodSet,
            //                      bodSetLng,
            //                      NULL
            //                      );
            //if (tmpRet < 0)
            //{
            //    return tmpRet;
            //}

            //发送等待验收
            nxtStepStat = MC_SEND_WAIT_CHECK;
            break;
        }

        case CLK_SYN:       //时钟同步;
        {
            tmpNowRlStep.reqPDU_FCode     = I_F;
            tmpFuncDsc.act_prio_chnlID     = ACTIVE_TYPE;
            tmpRet = asgmtNxtFrmPar ( &tmpNowRlStep,
                                      preStepRul_p,
                                      &nxtFrmStepNo,
                                      &tmpFuncDsc,
                                      statrtStepStnD_p
                                    );

            if ( tmpRet < 0 )
            {
                return tmpRet;
            }

            //发送等待验收
            nxtStepStat = MC_SEND_WAIT_CHECK;
            break;
        }

        case CMD_TRNS://命令传输
        {
            tmpNowRlStep.respPDU_FCode     = I_F;
            tmpFuncDsc.act_prio_chnlID     = ACTIVE_TYPE;
            tmpRet = asgmtNxtFrmPar ( &tmpNowRlStep,
                                      preStepRul_p,
                                      &nxtFrmStepNo,
                                      &tmpFuncDsc,
                                      statrtStepStnD_p
                                    );

            if ( tmpRet < 0 )
            {
                return tmpRet;
            }

            //发送等待验收
            nxtStepStat = MC_SEND_WAIT_CHECK;
            break;
        }

        case ACMU_FRZ:
        {
            /*累积量传输前的冻结    此功能是从ACMU_TRNS
              功能的模式2，控制站先冻结再请求传输分离出来的*/

            tmpNowRlStep.respPDU_FCode     = I_F;
            tmpFuncDsc.act_prio_chnlID     = ACTIVE_TYPE;
            tmpRet = asgmtNxtFrmPar ( &tmpNowRlStep,
                                      preStepRul_p,
                                      &nxtFrmStepNo,
                                      &tmpFuncDsc,
                                      statrtStepStnD_p
                                    );

            if ( tmpRet < 0 )
            {
                return tmpRet;
            }

            //发送等待验收
            nxtStepStat = MC_SEND_WAIT_CHECK;
            break;
        }

        case ACMU_TRNS://累积量传输
        {
            if ( usingNode == CTLD_DRCTSONPT_TRNS )
            {
                tmpFuncDsc.act_prio_chnlID = PASSIVE_TYPE;
                tmpNowRlStep.respPDU_FCode = I_F;
                //发送会话完成
                nxtStepStat = MC_SEND_SSN_FNSH;
            }
            else//CTL_DRCTREQ_TRNS/CTL_FRZREQ_TRNS/CTL_FRZ_CTLD_SONPT_TRNS
            {
                tmpFuncDsc.act_prio_chnlID = ACTIVE_TYPE;
                tmpNowRlStep.reqPDU_FCode = I_F;
                //发送等待验收
                nxtStepStat = MC_SEND_WAIT_CHECK;
            }

            tmpRet = asgmtNxtFrmPar ( &tmpNowRlStep,
                                      preStepRul_p,
                                      &nxtFrmStepNo,
                                      &tmpFuncDsc,
                                      statrtStepStnD_p
                                    );

            if ( tmpRet < 0 )
            {
                return tmpRet;
            }

            break;
        }

        case PARA_LD://参数加载
        {
            tmpNowRlStep.reqPDU_FCode      = I_F;
            tmpFuncDsc.act_prio_chnlID     = ACTIVE_TYPE;
            tmpRet = asgmtNxtFrmPar ( &tmpNowRlStep,
                                      preStepRul_p,
                                      &nxtFrmStepNo,
                                      &tmpFuncDsc,
                                      statrtStepStnD_p
                                    );

            if ( tmpRet < 0 )
            {
                return tmpRet;
            }

            //发送等待验收
            nxtStepStat = MC_SEND_WAIT_CHECK;
            break;
        }

        case TST_PRCD:      //测试过程;
        {
            //调用接口asgmtNxtFrmPar进行装帧操作
            tmpNowRlStep.reqPDU_FCode      = I_F;
            tmpFuncDsc.act_prio_chnlID     = ACTIVE_TYPE;
            tmpRet = asgmtNxtFrmPar ( &tmpNowRlStep,
                                      preStepRul_p,
                                      &nxtFrmStepNo,
                                      &tmpFuncDsc,
                                      statrtStepStnD_p
                                    );

            if ( tmpRet < 0 )
            {
                return tmpRet;
            }

            //发送等待验收
            nxtStepStat = MC_SEND_WAIT_CHECK;
            break;
        }

        case FILE_TRNS://文件传输
        {
            //暂不实现
            break;
        }

        case ACQ_TRNSDLY://收集传输延迟值
        {
            //不使用
            nxtStepStat = 0xffff;
            break;
        }

        case U_STARTDT://104协议U帧STARTDT 抽象的会话
        {
            //调用接口asgmtNxtFrmPar进行装帧操作
            tmpNowRlStep.reqPDU_FCode      = U_F;
            tmpFuncDsc.act_prio_chnlID     = ACTIVE_TYPE;
            statrtStepStnD_p->ASDUID_p[0]  = MC_TID_136;
            statrtStepStnD_p->COT          = MC_COT_6;
            tmpRet = asgmtNxtFrmPar ( &tmpNowRlStep,
                                      preStepRul_p,
                                      &nxtFrmStepNo,
                                      &tmpFuncDsc,
                                      statrtStepStnD_p
                                    );

            if ( tmpRet < 0 )
            {
                return tmpRet;
            }

            //发送等待验收;
            nxtStepStat = MC_SEND_WAIT_CHECK;

            break;
        }

        case U_STOPDT://104协议U帧STOPDT 抽象的会话
        {
            //调用接口asgmtNxtFrmPar进行装帧操作
            tmpNowRlStep.reqPDU_FCode      = U_F;
            tmpFuncDsc.act_prio_chnlID     = ACTIVE_TYPE;
            statrtStepStnD_p->ASDUID_p[0]  = MC_TID_137;
            statrtStepStnD_p->COT          = MC_COT_6;
            tmpRet = asgmtNxtFrmPar ( &tmpNowRlStep,
                                      preStepRul_p,
                                      &nxtFrmStepNo,
                                      &tmpFuncDsc,
                                      statrtStepStnD_p
                                    );

            if ( tmpRet < 0 )
            {
                return tmpRet;
            }

            //发送等待验收;
            nxtStepStat = MC_SEND_WAIT_CHECK;
            break;
        }

        case U_TESTFR://104协议U帧TESTFR 抽象的会话
        {
            //调用接口asgmtNxtFrmPar进行装帧操作
            tmpNowRlStep.reqPDU_FCode      = U_F;
            tmpFuncDsc.act_prio_chnlID     = ACTIVE_TYPE;
            statrtStepStnD_p->ASDUID_p[0]  = MC_TID_138;
            statrtStepStnD_p->COT          = MC_COT_6;
            tmpRet = asgmtNxtFrmPar ( &tmpNowRlStep,
                                      preStepRul_p,
                                      &nxtFrmStepNo,
                                      &tmpFuncDsc,
                                      statrtStepStnD_p
                                    );

            if ( tmpRet < 0 )
            {
                return tmpRet;
            }

            //发送等待验收;
            nxtStepStat = MC_SEND_WAIT_CHECK;
            break;
        }

        case U_NEWHOLD:
        {
            //用U帧的STARTDT STOPDT TESTFR 全为0 表示出新的会话：新联一个通道
            //调用接口asgmtNxtFrmPar进行装帧操作
            tmpNowRlStep.reqPDU_FCode      = U_F;
            tmpFuncDsc.act_prio_chnlID     = ACTIVE_TYPE;
            statrtStepStnD_p->ASDUID_p[0]  = MC_TID_139;
            statrtStepStnD_p->COT          = MC_COT_6;
            tmpRet = asgmtNxtFrmPar ( &tmpNowRlStep,
                                      preStepRul_p,
                                      &nxtFrmStepNo,
                                      &tmpFuncDsc,
                                      statrtStepStnD_p
                                    );

            if ( tmpRet < 0 )
            {
                return tmpRet;
            }

            //发送等待验收;
            nxtStepStat = MC_SEND_WAIT_CHECK;
            break;
        }

        case U_DELHOLD:
        {
            //用U帧的STARTDT STOPDT TESTFR 全为1 表示出新的会话：关一个通道
            //调用接口asgmtNxtFrmPar进行装帧操作
            tmpNowRlStep.reqPDU_FCode      = U_F;
            tmpFuncDsc.act_prio_chnlID     = ACTIVE_TYPE;
            statrtStepStnD_p->ASDUID_p[0]  = MC_TID_140;
            statrtStepStnD_p->COT          = MC_COT_6;
            tmpRet = asgmtNxtFrmPar ( &tmpNowRlStep,
                                      preStepRul_p,
                                      &nxtFrmStepNo,
                                      &tmpFuncDsc,
                                      statrtStepStnD_p
                                    );

            if ( tmpRet < 0 )
            {
                return tmpRet;
            }

            //发送等待验收;
            nxtStepStat = MC_SEND_WAIT_CHECK;
            break;
        }

        default:
            break;

    }

    return nxtStepStat;

}




/******************************************************************************
 *功能：步驱动，设置下一帧对应的步
 * 输入：
 *      Fun_p        ：步规则
 *      stepRl_p     ：当前帧对应的步
 *      funDsc_p     ：会话号与主动性
 *      ssnPar_p     ：会话步需要的数据信息（详见此类型说明）
 *输出：
 *      stepRl_p     ：下一帧对应的步
 *      nxtStepStat_p：会话状态
 *返回：
 *      >0           ：调用nxtStep发的这一帧数据的下一帧对应的步号(如果已经是最
                       后一步最后一帧则此值为当前帧对应的步号）
 *      0xffff       ：该功能不支持
 *      <0           ：接口异常
 *修改历史
 *
*******************************************************************************/
EXPDLL int nxtStep ( FUN_RL* Fun_p,
                     ONE_STEP* stepRl_p,
                     T_ptclFunDsc* funDsc_p,
                     T_snnPara* ssnPar_p,
                     u_int* nxtStepStat_p
                   )
{
    if ( NULL == funDsc_p
         || NULL == stepRl_p
       )
    {
        return ERR_POINTER_NULL;
    }

    if ( ( u_char ) funDsc_p->act_prio_chnlID != ACTIVE_TYPE
         && ( u_char ) funDsc_p->act_prio_chnlID != PASSIVE_TYPE
       )
    {
        return ERR_ACT_PAS_VAL;
    }

    int               tmpRet   = 0;   //调用下层接口返回值
    ONE_STEP* nowRuleStep_p    = NULL; //当前步的规则步指针
    ONE_STEP* nxtFrmRlStep_p   = NULL;//下一帧对应的规则步
    // u_char      allFrameNum  = 0;   //规则步中帧数量的限定值;
    // u_char      nowFrameNum  = 0;   //当前帧值（是步的第几帧）;
    u_char     nxtFrmStepNo    = 0;

    *nxtStepStat_p = 0xffff;       //初始化为非会话状态值

    ONE_STEP* lastRuleStep_p = NULL;
    nxtFrmStepNo = stepRl_p->stepNo;

    //如果不是U帧抽象出的特殊会话
    if ( funDsc_p->ssnDscNo != U_STARTDT
         && funDsc_p->ssnDscNo != U_STOPDT
         && funDsc_p->ssnDscNo != U_TESTFR
         && funDsc_p->ssnDscNo != U_NEWHOLD
         && funDsc_p->ssnDscNo != U_DELHOLD
       )
    {
        if ( NULL == Fun_p
             || NULL == Fun_p->steps_p[Fun_p->usingMode]
           )
        {
            return ERR_POINTER_NULL;
        }

        if ( Fun_p->srvNo != funDsc_p->ssnDscNo )
        {
            return ERR_CONFIG;
        }

        //找到规则步最后一步
        lastRuleStep_p = Fun_p->steps_p[Fun_p->usingMode];

        while ( lastRuleStep_p->nextStep_p != NULL )
        {
            lastRuleStep_p    = lastRuleStep_p->nextStep_p;
        }
    }

    switch ( funDsc_p->ssnDscNo ) //判断功能;
    {
        case STN_INIT: //站初始化;
        {
            break;
        }

        case QUE_DATA: //查询数据;
        {
            if ( stepRl_p->stepNo > lastRuleStep_p->stepNo )
            {
                *nxtStepStat_p = MC_NO_SEND_SSN_STOP; //不发送会话中止
                return nxtFrmStepNo;
            }

            break;
        }

        case CYC_TRNS: //周期传输;无第二帧;
        {
            if ( stepRl_p->stepNo > lastRuleStep_p->stepNo
                 || lastRuleStep_p->stepNo > C_STEP_NO_ONE
               )
            {
                *nxtStepStat_p = MC_NO_SEND_SSN_STOP; //不发送会话中止
                return nxtFrmStepNo;
            }

            break;
        }

        case ACQ_EVT: //事件收集;无第二帧;
        {
            if ( stepRl_p->stepNo > lastRuleStep_p->stepNo
                 || lastRuleStep_p->stepNo > C_STEP_NO_ONE
               )
            {
                *nxtStepStat_p = MC_NO_SEND_SSN_STOP; //不发送会话中止
                return nxtFrmStepNo;
            }

            break;
        }

        case QUKCHK_EVT: //快检事件;
        {
            //暂不实现;
            return nxtFrmStepNo;
            break;
        }

        case STN_INRGTN: //站召唤;
        {
            if ( stepRl_p->stepNo > lastRuleStep_p->stepNo )
            {
                *nxtStepStat_p = MC_NO_SEND_SSN_STOP; //不发送会话中止
                return nxtFrmStepNo;
            }

            break;
        }

        case CLK_SYN: //时钟同步;
        {
            if ( stepRl_p->stepNo > lastRuleStep_p->stepNo )
            {
                *nxtStepStat_p = MC_NO_SEND_SSN_STOP; //不发送会话中止
                return nxtFrmStepNo;
            }

            break;
        }

        case CMD_TRNS: //命令传输
        {
            if ( stepRl_p->stepNo > lastRuleStep_p->stepNo )
            {
                *nxtStepStat_p = MC_NO_SEND_SSN_STOP; //不发送会话中止
                return nxtFrmStepNo;
            }

            break;
        }

        case ACMU_FRZ:
        {
            /*累积量传输前的冻结    此功能是从ACMU_TRNS功能的模式2，
              控制站先冻结再请求传输分离出来的*/

            if ( stepRl_p->stepNo > lastRuleStep_p->stepNo )
            {
                *nxtStepStat_p = MC_NO_SEND_SSN_STOP; //不发送会话中止
                return nxtFrmStepNo;
            }

            break;
        }

        case ACMU_TRNS: //累积量传输
        {
            if ( Fun_p->usingMode == CTLD_DRCTSONPT_TRNS )
            {
                if ( stepRl_p->stepNo > lastRuleStep_p->stepNo )
                {
                    *nxtStepStat_p = MC_NO_SEND_SSN_STOP; //不发送会话中止
                    return *nxtStepStat_p;
                }
            }
            else if ( Fun_p->usingMode == CTL_DRCTREQ_TRNS )
            {
                if ( stepRl_p->stepNo > lastRuleStep_p->stepNo )
                {
                    *nxtStepStat_p = MC_NO_SEND_SSN_STOP; //不发送会话中止
                    return nxtFrmStepNo;
                }
            }
            else if ( Fun_p->usingMode == CTL_FRZREQ_TRNS )
            {
                if ( stepRl_p->stepNo > lastRuleStep_p->stepNo )
                {
                    *nxtStepStat_p = MC_NO_SEND_SSN_STOP; //不发送会话中止
                    return *nxtStepStat_p;
                }
            }
            else //CTL_FRZ_CTLD_SONPT_TRNS
            {
                if ( stepRl_p->stepNo > lastRuleStep_p->stepNo )
                {
                    *nxtStepStat_p = MC_NO_SEND_SSN_STOP; //不发送会话中止
                    return nxtFrmStepNo;
                }
            }

            break;
        }

        case PARA_LD: //参数加载
        {
            if ( stepRl_p->stepNo > lastRuleStep_p->stepNo )
            {
                *nxtStepStat_p = MC_NO_SEND_SSN_STOP; //不发送会话中止
                return nxtFrmStepNo;
            }

            break;
        }

        case TST_PRCD: //测试过程;
        {
            if ( stepRl_p->stepNo > lastRuleStep_p->stepNo )
            {
                *nxtStepStat_p = MC_NO_SEND_SSN_STOP; //不发送会话中止
                return nxtFrmStepNo;
            }

            break;
        }

        case FILE_TRNS: //文件传输
        {
            //暂不实现
            break;
        }

        case ACQ_TRNSDLY: //收集传输延迟值
        {
            //不使用
            return nxtFrmStepNo;
            break;
        }

        case U_STARTDT://104协议U帧STARTDT 抽象的会话
        {
            tmpRet = asgmtNxtUPar (
                     U_STARTDT,
                     stepRl_p,
                     funDsc_p,
                     ssnPar_p,
                     nxtFrmStepNo,
                     nxtStepStat_p
                     );

            if ( tmpRet < 0 )
            {
                return tmpRet;
            }

            break;
        }

        case U_STOPDT://104协议U帧STOPDT 抽象的会话
        {
            tmpRet = asgmtNxtUPar (
                     U_STOPDT,
                     stepRl_p,
                     funDsc_p,
                     ssnPar_p,
                     nxtFrmStepNo,
                     nxtStepStat_p
                     );

            if ( tmpRet < 0 )
            {
                return tmpRet;
            }

            break;
        }

        case U_TESTFR://104协议U帧TESTFR 抽象的会话
        {
            tmpRet = asgmtNxtUPar (
                     U_TESTFR,
                     stepRl_p,
                     funDsc_p,
                     ssnPar_p,
                     nxtFrmStepNo,
                     nxtStepStat_p
                     );

            if ( tmpRet < 0 )
            {
                return tmpRet;
            }

            break;
        }

        case U_NEWHOLD:
        {
            //用U帧的STARTDT STOPDT TESTFR 全为0 表示出新的会话：新联一个通道
            tmpRet = asgmtNxtUPar (
                     U_NEWHOLD,
                     stepRl_p,
                     funDsc_p,
                     ssnPar_p,
                     nxtFrmStepNo,
                     nxtStepStat_p
                     );

            if ( tmpRet < 0 )
            {
                return tmpRet;
            }

            break;
        }

        case U_DELHOLD:
        {
            //用U帧的STARTDT STOPDT TESTFR 全为1 表示出新的会话：关一个通道
            tmpRet = asgmtNxtUPar (
                     U_DELHOLD,
                     stepRl_p,
                     funDsc_p,
                     ssnPar_p,
                     nxtFrmStepNo,
                     nxtStepStat_p
                     );

            if ( tmpRet < 0 )
            {
                return tmpRet;
            }

            break;
        }

        default:
            break;

    }

    if ( funDsc_p->ssnDscNo != U_STARTDT
         && funDsc_p->ssnDscNo != U_STOPDT
         && funDsc_p->ssnDscNo != U_TESTFR
         && funDsc_p->ssnDscNo != U_NEWHOLD
         && funDsc_p->ssnDscNo != U_DELHOLD
       )
    {
        //当前步的规则步指针
        nowRuleStep_p = displayNextNode (
                        Fun_p->steps_p[Fun_p->usingMode],
                        stepRl_p
                        );

        if ( nowRuleStep_p == NULL )
        {
            return ERR_POINTER_NULL;
        }

        //获得下一帧会话状态 nxtStepStat_p,下一帧步号nxtFrmStepNo
        tmpRet = getNxtFrmSnnStatus ( Fun_p,
                                      stepRl_p,
                                      funDsc_p,
                                      nxtStepStat_p,
                                      &nxtFrmStepNo
                                    );

        if ( tmpRet < 0 )
        {
            return tmpRet;
        }

        /*if (stepRl_p->stepNo != nxtFrmStepNo)
        {
            nxtFrmRlStep_p = nowRuleStep_p->nextStep_p;
        }
        else
        {*/
        nxtFrmRlStep_p = nowRuleStep_p;
        /*}*/

        //有发送帧的会话状态需要调用asgmtNxtFrmPar进行装帧
        if (   *nxtStepStat_p != MC_NO_SEND_WAIT_CHECK
               && *nxtStepStat_p != MC_NO_SEND_CNUE_EXEC
               && *nxtStepStat_p != MC_NO_SEND_SSN_STOP
               && *nxtStepStat_p != MC_NO_SEND_SSN_FNSH
               && *nxtStepStat_p != 0xffff
           )
        {
            tmpRet = asgmtNxtFrmPar (
                     nxtFrmRlStep_p,
                     stepRl_p,
                     &nxtFrmStepNo,
                     funDsc_p,
                     ssnPar_p
                     );

            if ( tmpRet < 0 )
            {
                return tmpRet;
            }
        }
    }


    return nxtFrmStepNo;
}





/******************************************************************************
  *功能：解ASDU
  *输入：
  *     APDU_p  ：APDU地址
  *     APDU_lng: APDU长度
  *输出：
  *     TID     ：类型标识符
  *     COT     ：传输原因
  *返回：
  *     0       ：正确
  *     <0      ：错误
  *修改历史：
  *
*******************************************************************************/
EXPDLL int chkASDU ( u_char* APDU_p,
                     u_short APDU_lng,
                     u_int* TID_p, u_int* COT_p
                   )
{
    if ( APDU_p == NULL
         || TID_p == NULL
         || COT_p == NULL
       )
    {
        return ERR_POINTER_NULL;
    }

    if ( APDU_lng == 0 ) //APDU_p 里只有apci
    {
        //getFrameType(APDU_p,APDU_lng);
        APCI_104* apci_p = ( APCI_104* ) APDU_p ;

        if ( ( apci_p->ctlField[0] & U_F ) == U_F )
        {
            u_char data_type = apci_p->ctlField[0]; // 控制域八位位组1

            switch ( data_type )
            {
                case IEC104_STARTDT_ACT://STARTDT激活
                    *TID_p = MC_TID_136;
                    *COT_p = MC_COT_6;
                    break;

                case IEC104_STARTDT_ACK://STARTDT确认
                    *TID_p = MC_TID_136;
                    *COT_p = MC_COT_7;
                    break;

                case IEC104_STOPDT_ACT://STOPDT激活
                    *TID_p = MC_TID_137;
                    *COT_p = MC_COT_6;
                    break;

                case IEC104_STOPDT_ACK://STOPDT确认
                    *TID_p = MC_TID_137;
                    *COT_p = MC_COT_7;
                    break;

                case IEC104_TESTFR_ACT:// TESTFR激活
                    *TID_p = MC_TID_138;
                    *COT_p = MC_COT_6;
                    break;

                case IEC104_TESTFR_ACK://TESTFR确认
                    *TID_p = MC_TID_138;
                    *COT_p = MC_COT_7;
                    break;

                case IEC104_NEWHOLD_ACT://NEWHOLD 新联一个通道 激活
                    *TID_p = MC_TID_139;
                    *COT_p = MC_COT_6;
                    break;

                case IEC104_NEWHOLD_ACK://NEWHOLD 新联一个通道 确认
                    *TID_p = MC_TID_139;
                    *COT_p = MC_COT_7;
                    break;

                case IEC104_DELHOLD_ACT://DELHOLD 关一个通道 激活
                    *TID_p = MC_TID_140;
                    *COT_p = MC_COT_6;
                    break;

                case IEC104_DELHOLD_ACK://DELHOLD 关一个通道 确认
                    *TID_p = MC_TID_140;
                    *COT_p = MC_COT_7;
                    break;

                default: //不支持的U帧功能
                    return -2;
            }
        }
    }
    else
    {
        APDU_104_p->bindToMem ( APDU_p, 1, APDU_lng ); //绑定APDU_104
        APDU_DSCR* apdu_dscr_p = APDU_104_p->getDU_Info();
        apdu_dscr_p->ASDU_CMTBL[APDU_APCI_INDEX] = 0;
        u_short asdu104 = 0;
        APDU_104_p->getDU ( asdu104, 1 );
        //取TID输出
        ASDU_TID_VAL* tid = APDU_104_p->getASDU_TID_VAL (
                            &APDU_104_p->getDU_Info()->ASDU_CMTBL[ASDU_TID_INDEX]
                            );
        memcpy ( TID_p, tid, sizeof ( ASDU_TID_VAL ) );

        //取COT输出
        u_short* cot_len = &APDU_104_p->getDU_Info()->ASDU_CMTBL[ASDU_COT_INDEX];

        if ( *cot_len == sizeof ( ASDU_COT1_VAL ) )
        {
            ASDU_COT1_VAL* cot = ( ASDU_COT1_VAL* )
                                 APDU_104_p->getASDU_COT_VAL ( cot_len );
            memcpy ( COT_p, cot, *cot_len );
        }
        else if ( *cot_len == sizeof ( ASDU_COT2_VAL ) )
        {
            ASDU_COT2_VAL* cot = ( ASDU_COT2_VAL* )
                                 APDU_104_p->getASDU_COT_VAL ( cot_len );
            memcpy ( COT_p, cot, *cot_len );
        }
        else //无定义的COT类型
        {
            return -1;
        }
    }

    return 0 ;
}





/******************************************************************************
  *功能：获取可能的功能号
  *输入：
  *     TID：     类型标识符
  *     COT：     传输原因
  *输出：
  *     funDsc_p：会话号与数据的方向(控制与监视两个方向)
  *     funNum_p：功能码个数（即funDsc_p数组实际下标值）
  *返回：
  *     协议编号：如101、104
  *修改历史
  *
*******************************************************************************/
EXPDLL int getSsnSet ( u_int TID,
                       u_int COT,
                       T_ptclFunDsc* funDsc_p,
                       u_char* funNum_p
                     )
{
    if ( funDsc_p == NULL
         || funNum_p == NULL
       )
    {
        return ERR_POINTER_NULL;
    }

    T_ptclFunDsc function[26] = {}; //建立T_ptclFunDsc型a数组
    *funNum_p = 0;

    //特殊应用（专用范围）
    if ( TID >= MC_TID_136 && TID <= MC_TID_140 )
    {
        switch ( TID )
        {
            case MC_TID_136://STARTDT
                function[*funNum_p].ssnDscNo = U_STARTDT;

                if ( COT == MC_COT_6 )
                {
                    function[*funNum_p].act_prio_chnlID = MC_CONTROL_DIRECTION;
                }
                else if ( COT == MC_COT_7 )
                {
                    function[*funNum_p].act_prio_chnlID = MC_MONITOR_DIRECTION;
                }

                break;

            case MC_TID_137://STOPDT
                function[*funNum_p].ssnDscNo = U_STOPDT;

                if ( COT == MC_COT_6 )
                {
                    function[*funNum_p].act_prio_chnlID = MC_CONTROL_DIRECTION;
                }
                else if ( COT == MC_COT_7 )
                {
                    function[*funNum_p].act_prio_chnlID = MC_MONITOR_DIRECTION;
                }

                break;

            case MC_TID_138:// TESTFR
                function[*funNum_p].ssnDscNo = U_TESTFR;

                if ( COT == MC_COT_6 )
                {
                    function[*funNum_p].act_prio_chnlID = MC_CONTROL_DIRECTION;
                }
                else if ( COT == MC_COT_7 )
                {
                    function[*funNum_p].act_prio_chnlID = MC_MONITOR_DIRECTION;
                }

                break;

            case MC_TID_139://NEWHOLD 新联一个通道
                function[*funNum_p].ssnDscNo = U_NEWHOLD;

                if ( COT == MC_COT_6 )
                {
                    function[*funNum_p].act_prio_chnlID = MC_CONTROL_DIRECTION;
                }
                else if ( COT == MC_COT_7 )
                {
                    function[*funNum_p].act_prio_chnlID = MC_MONITOR_DIRECTION;
                }

                break;

            case MC_TID_140://DELHOLD 关一个通道
                function[*funNum_p].ssnDscNo = U_DELHOLD;

                if ( COT == MC_COT_6 )
                {
                    function[*funNum_p].act_prio_chnlID = MC_CONTROL_DIRECTION;
                }
                else if ( COT == MC_COT_7 )
                {
                    function[*funNum_p].act_prio_chnlID = MC_MONITOR_DIRECTION;
                }

                break;

            default:
                break;
        }

        ( *funNum_p ) ++;
        //将function数组的值赋给出参funDsc_p数组
        memcpy ( funDsc_p, function, sizeof ( T_ptclFunDsc ) * ( *funNum_p ) );

        return 104;
    }

    if ( TID >= MC_TID_22
         && TID <= MC_TID_29 ) //22...29 为将来兼容定义保留
    {
        function[0].ssnDscNo        = QUE_DATA;
        function[0].act_prio_chnlID = MC_MONITOR_DIRECTION;

        function[1].ssnDscNo        = CYC_TRNS;
        function[1].act_prio_chnlID = MC_MONITOR_DIRECTION;

        function[2].ssnDscNo          = ACQ_EVT;
        // function[2].act_prio_chnlID = ACTIVE_TYPE;
        function[2].act_prio_chnlID   = MC_MONITOR_DIRECTION;

        function[3].ssnDscNo        = STN_INRGTN;
        function[3].act_prio_chnlID = MC_MONITOR_DIRECTION;

        function[4].ssnDscNo        = CMD_TRNS;
        function[4].act_prio_chnlID = MC_MONITOR_DIRECTION;

        function[5].ssnDscNo        = ACMU_TRNS;
        function[5].act_prio_chnlID = MC_MONITOR_DIRECTION;

        function[6].ssnDscNo        = PARA_LD;
        function[6].act_prio_chnlID = MC_MONITOR_DIRECTION;

        function[7].ssnDscNo        = TST_PRCD;
        function[7].act_prio_chnlID = MC_MONITOR_DIRECTION;

        *funNum_p = 8;
    }

    //通过TID范围和COT范围确定<查询数据>功能码
    if ( ( ( TID >= MC_TID_1 && TID <= MC_TID_14 )
           || ( TID == MC_TID_20 )
           || ( TID == MC_TID_21 )
           || ( TID >= MC_TID_30 && TID <= MC_TID_36 )
           || ( TID == MC_TID_126 ) )
         && ( COT == MC_COT_5 )
       )
    {
        function[*funNum_p].ssnDscNo        = QUE_DATA;
        function[*funNum_p].act_prio_chnlID = MC_MONITOR_DIRECTION;
        ( *funNum_p ) ++;
    }
    else if ( TID == MC_TID_102 )
    {
        function[*funNum_p].ssnDscNo = QUE_DATA;

        if ( COT == MC_COT_5 )
        {
            function[*funNum_p].act_prio_chnlID = MC_CONTROL_DIRECTION;
        }
        else if ( COT >= MC_COT_44 && COT <= MC_COT_47 )
        {
            function[*funNum_p].act_prio_chnlID = 0xffff;
        }

        ( *funNum_p ) ++;
    }

    //通过TID范围和COT范围确定<周期传输>功能码
    if ( ( ( TID == MC_TID_9 )
           || ( TID == MC_TID_11 )
           || ( TID == MC_TID_13 )
           || ( TID == MC_TID_21 ) )
         && ( COT == MC_COT_1 )
       )
    {
        function[*funNum_p].ssnDscNo        = CYC_TRNS;
        function[*funNum_p].act_prio_chnlID = MC_MONITOR_DIRECTION;
        ( *funNum_p ) ++;
    }

    //通过TID范围和COT范围确定<收集事件>功能码
    if (  ( ( TID >= MC_TID_1 && TID <= MC_TID_21 )
            || ( TID >= MC_TID_30 && TID <= MC_TID_40 ) )
          && ( COT == MC_COT_3 )
       )
    {
        function[*funNum_p].ssnDscNo = ACQ_EVT;
        //function[*funNum_p].act_prio_chnlID = ACTIVE_TYPE;
        function[*funNum_p].act_prio_chnlID = MC_MONITOR_DIRECTION;
        ( *funNum_p ) ++;
    }

    //通过TID范围和COT范围确定<站召唤>功能码
    if (  ( ( TID >= MC_TID_1 && TID <= MC_TID_14 && TID % 2 == 1 )
            || ( TID == MC_TID_20 )
            || ( TID == MC_TID_21 )
          )
          && ( COT >= MC_COT_20 && COT <= MC_COT_36 )
       )
    {
        function[*funNum_p].ssnDscNo        = STN_INRGTN;
        function[*funNum_p].act_prio_chnlID = MC_MONITOR_DIRECTION;
        ( *funNum_p ) ++;
    }
    else if ( TID == MC_TID_100 )
    {
        function[*funNum_p].ssnDscNo = STN_INRGTN;

        if ( COT == MC_COT_6 )
        {
            function[*funNum_p].act_prio_chnlID = MC_CONTROL_DIRECTION;
        }
        else if ( COT == MC_COT_7 || COT == MC_COT_10 )
        {
            function[*funNum_p].act_prio_chnlID = MC_MONITOR_DIRECTION;
        }
        else if ( COT >= MC_COT_44 && COT <= MC_COT_47 )
        {
            function[*funNum_p].act_prio_chnlID = 0xffff;
        }

        ( *funNum_p ) ++;
    }

    //通过TID范围和COT范围确定<时钟同步>功能码
    if ( TID == MC_TID_103 )
    {
        function[*funNum_p].ssnDscNo = CLK_SYN;

        if ( COT == MC_COT_6 )
        {
            function[*funNum_p].act_prio_chnlID = MC_CONTROL_DIRECTION;
        }
        else if ( COT == MC_COT_7 )
        {
            function[*funNum_p].act_prio_chnlID = MC_MONITOR_DIRECTION;
        }
        else if ( COT >= MC_COT_44 && COT <= MC_COT_47 )
        {
            function[*funNum_p].act_prio_chnlID = 0xffff;
        }

        ( *funNum_p ) ++;
    }

    //通过TID范围和COT范围确定<命令传输>功能码
    if (  ( ( TID >= MC_TID_45 && TID <= MC_TID_51 )
            || ( TID >= MC_TID_58 && TID <= MC_TID_64 ) )
          && ( ( COT == MC_COT_6 )
               || ( COT == MC_COT_7 )
               || ( COT == MC_COT_8 )
               || ( COT == MC_COT_9 )
               || ( COT == MC_COT_10 )
               || ( COT >= MC_COT_44 && COT <= MC_COT_47 )
             )
       )
    {
        function[*funNum_p].ssnDscNo = CMD_TRNS;

        if ( COT == MC_COT_6 || COT == MC_COT_8 )
        {
            function[*funNum_p].act_prio_chnlID = MC_CONTROL_DIRECTION;
        }
        else if ( COT == MC_COT_7 || COT == MC_COT_9 || COT == MC_COT_10 )
        {
            function[*funNum_p].act_prio_chnlID = MC_MONITOR_DIRECTION;
        }
        else if ( COT >= MC_COT_44 && COT <= MC_COT_47 )
        {
            function[*funNum_p].act_prio_chnlID = 0xffff;
        }

        ( *funNum_p ) ++;
    }
    else if ( ( ( TID >= MC_TID_1 && TID <= MC_TID_7 )
                || ( TID == MC_TID_20 )
                || ( TID >= MC_TID_30 && TID <= MC_TID_32 )
              )
              && ( COT == MC_COT_11 || COT == MC_COT_12 )
            ) //fusk add:这里不太确定是否有 MC_COT_12 此值 的情况
    {
        function[*funNum_p].ssnDscNo = CMD_TRNS;
        function[*funNum_p].act_prio_chnlID = MC_MONITOR_DIRECTION;
        ( *funNum_p ) ++;
    }

    //通过TID范围和COT范围确定<累积量传输>功能码
    if (  ( TID == MC_TID_15 || TID == MC_TID_16 || TID == MC_TID_37 )
          && ( COT == MC_COT_3
               || ( COT >= MC_COT_37 && COT <= MC_COT_41 )
             )
       )
    {
        function[*funNum_p].ssnDscNo = ACMU_TRNS;
        function[*funNum_p].act_prio_chnlID = MC_MONITOR_DIRECTION;
        ( *funNum_p ) ++;
    }
    else if ( TID == MC_TID_101 )
    {
        function[*funNum_p].ssnDscNo = ACMU_TRNS;

        if ( COT == MC_COT_6 )
        {
            function[*funNum_p].act_prio_chnlID = MC_CONTROL_DIRECTION;
        }
        else if ( COT == MC_COT_7 || COT == MC_COT_10 )
        {
            function[*funNum_p].act_prio_chnlID = MC_MONITOR_DIRECTION;
        }
        else if ( COT >= MC_COT_44 && COT <= MC_COT_47 )
        {
            function[*funNum_p].act_prio_chnlID = 0xffff;
        }

        ( *funNum_p ) ++;

        if ( COT == MC_COT_6 || COT == MC_COT_7 )
        {
            /*累积量传输前的冻结    此功能是从ACMU_TRNS
             功能的模式2，控制站先冻结再请求传输分离出来的*/
            function[*funNum_p].ssnDscNo = ACMU_FRZ;

            if ( COT == MC_COT_6 )
            {
                function[*funNum_p].act_prio_chnlID = MC_CONTROL_DIRECTION;
            }
            else if ( COT == MC_COT_7 || COT == MC_COT_10 )
            {
                function[*funNum_p].act_prio_chnlID = MC_MONITOR_DIRECTION;
            }

            ( *funNum_p ) ++;
        }
    }

    //通过TID范围和COT范围确定<参数加载>功能码
    if ( TID >= MC_TID_110 && TID <= MC_TID_112 )
    {
        function[*funNum_p].ssnDscNo = PARA_LD;

        if ( COT == MC_COT_6 )
        {
            function[*funNum_p].act_prio_chnlID = MC_CONTROL_DIRECTION;
        }
        else if ( COT == MC_COT_7 || COT == MC_COT_3 )
        {
            function[*funNum_p].act_prio_chnlID = MC_MONITOR_DIRECTION;
        }
        else if ( COT >= MC_COT_44 && COT <= MC_COT_47 )
        {
            function[*funNum_p].act_prio_chnlID = 0xffff;
        }

        ( *funNum_p ) ++;
    }
    else if ( TID == MC_TID_113 )
    {
        function[*funNum_p].ssnDscNo = PARA_LD;

        if ( COT == MC_COT_6 )
        {
            function[*funNum_p].act_prio_chnlID = MC_CONTROL_DIRECTION;
        }
        else if ( COT == MC_COT_7 )
        {
            function[*funNum_p].act_prio_chnlID = MC_MONITOR_DIRECTION;
        }
        else if ( COT >= MC_COT_44 && COT <= MC_COT_47 )
        {
            function[*funNum_p].act_prio_chnlID = 0xffff;
        }

        ( *funNum_p ) ++;
    }

    //通过TID范围和COT范围确定<测试过程>功能码
    if ( TID == MC_TID_107 ) //fusk add:不确定104是否有tid=104的情况
    {
        function[*funNum_p].ssnDscNo = TST_PRCD;

        if ( COT == MC_COT_6 )
        {
            function[*funNum_p].act_prio_chnlID = MC_CONTROL_DIRECTION;
        }
        else if ( COT == MC_COT_7 )
        {
            function[*funNum_p].act_prio_chnlID = MC_MONITOR_DIRECTION;
        }
        else if ( COT >= MC_COT_44 && COT <= MC_COT_47 )
        {
            function[*funNum_p].act_prio_chnlID = 0xffff;
        }

        ( *funNum_p ) ++;
    }

    //通过TID范围和COT范围确定<文件传输>功能码
    if ( TID == MC_TID_120
         || TID == MC_TID_121
         || ( TID >= MC_TID_123 && TID <= MC_TID_125 )
         || TID == MC_TID_127
       )
    {
        function[*funNum_p].ssnDscNo = FILE_TRNS;

        if ( COT == MC_COT_13 )
        {
            function[*funNum_p].act_prio_chnlID = MC_MONITOR_DIRECTION;
        }
        else if ( COT >= MC_COT_44 && COT <= MC_COT_47 )
        {
            function[*funNum_p].act_prio_chnlID = 0xffff;
        }

        ( *funNum_p ) ++;
    }
    else if ( TID == MC_TID_122 )
    {
        function[*funNum_p].ssnDscNo = FILE_TRNS;

        if ( COT == MC_COT_13 || COT == MC_COT_5 )
        {
            function[*funNum_p].act_prio_chnlID = MC_MONITOR_DIRECTION;
        }
        else if ( COT >= MC_COT_44 && COT <= MC_COT_47 )
        {
            function[*funNum_p].act_prio_chnlID = 0xffff;
        }

        ( *funNum_p ) ++;
    }
    else if ( TID == MC_TID_126 )
    {
        function[*funNum_p].ssnDscNo = FILE_TRNS;

        if ( COT == MC_COT_13 || COT == MC_COT_5 )
        {
            function[*funNum_p].act_prio_chnlID = MC_MONITOR_DIRECTION;
        }
    }

    //将function数组的值赋给出参funDsc_p数组
    memcpy ( funDsc_p, function, sizeof ( T_ptclFunDsc ) * ( *funNum_p ) );

    return 104;

}




/******************************************************************************
  *功能：获取会话步号
  *输入：
  *     funDsc_p  :会话号与主动性
  *     TID       :类型标识符
  *     COT       :传输原因
  *     PDUObj_p  :APDU104对象
  *     ASDU_p    :ASDU地址
  *     ASDUlng   :ASDU长度
  *输出：
  *     无
  *返回：
  *     < 0       :系统异常
  *     0xffff    :该功能不支持
  *     > 0       :stepNo步号
  *修改历史:
  *
*******************************************************************************/
EXPDLL int getFunStep ( T_ptclFunDsc* funDsc_p,
                        u_int TID,
                        u_int COT,
                        u_char* PDUObj_p,
                        u_char* ASDU_p,
                        u_short ASDUlng
                      )
{
    /*if ( funDsc_p == NULL
        || PDUObj_p == NULL
        || ASDU_p == NULL
        )
    {
        return ERR_POINTER_NULL;
    }*/

    if ( funDsc_p == NULL
         || funDsc_p->remoteSet_p == NULL
         || funDsc_p->remoteSet_p->TID_p == NULL
       )
    {
        return ERR_POINTER_NULL;
    }

    u_int stepNo = 0xffff;

    //如果不是U帧抽象出的特殊会话
    if ( funDsc_p->ssnDscNo == U_STARTDT
         && funDsc_p->ssnDscNo == U_STOPDT
         && funDsc_p->ssnDscNo == U_TESTFR
         && funDsc_p->ssnDscNo == U_NEWHOLD
         && funDsc_p->ssnDscNo == U_DELHOLD
       )
    {
        if ( ( TID == MC_TID_136
               || TID == MC_TID_137
               || TID == MC_TID_138
               || TID == MC_TID_139
               || TID == MC_TID_140 )
             && COT == MC_COT_6
           )
        {
            stepNo = 1;
        }
        else if ( ( TID == MC_TID_136
                    || TID == MC_TID_137
                    || TID == MC_TID_138
                    || TID == MC_TID_139
                    || TID == MC_TID_140 )
                  && COT == MC_COT_7
                )
        {
            stepNo = 2;
        }
    }
    else
    {
        u_char tFlag = 0;

        //4楼联调测试针对突发添加
        if ( funDsc_p->ssnDscNo == ACQ_EVT )
        {
            stepNo = 1;
            return stepNo;
        }
        else
        {
            ptclStep* tmpPtclStep = funDsc_p->remoteSet_p;

            for ( int i = 0; i < funDsc_p->setNum; i++ )
            {
                tFlag = 0;

                for ( int j = 0; j < ( ( tmpPtclStep + i )->TidNum ) ; j++ )
                {
                    if ( TID == ( tmpPtclStep + i )->TID_p[j]
                         && COT == ( tmpPtclStep + i )->COT
                       )
                    {
                        stepNo = i + 1;
                        tFlag = 1;
                        break;
                    }
                }

                if ( tFlag == 1 )
                {
                    break;
                }

            }
        }
    }


    return stepNo;

    //   u_int tmpFlag = 0;//是否按远动站配置进行返回,1:按配置返回，0:不按配置返回
    //   u_char usingMode = funDsc_p->usingMode;
    //   u_char act_prio_chnlID = (u_char) funDsc_p->act_prio_chnlID;
    //
    //
    //APDU104* apdu_104_p =(APDU104*) PDUObj_p;
    //u_short  apdu_lng   = 0;
    //   u_char*  asdu104_p  = apdu_104_p->getDU(apdu_lng ,ASDU_TYPE);
    //   if (apdu_lng != ASDUlng)//判断ASDU地址的长度是否符合;
    //   {
    //     return -1;
    //   }
    //   if (asdu104_p !=  ASDU_p) //要绑定的ASDU地址与原对象ASDU地址不等才进行绑定
    //   {
    //  APDU_DSCR* apdu_dscr_p = apdu_104_p->getDU_Info();
    //  apdu_dscr_p->ASDU_CMTBL[APDU_APCI_INDEX] = 0;
    //       //绑定ASDU_p到APDU104对象
    //       apdu_104_p->bindToMem((ASDU_p), ASDU_TYPE , ASDUlng);
    //   }

    //   //取VSQ
    //   ASDU_VSQ_VAL* VSQ_p = apdu_104_p->getASDU_VSQ_VAL(
    //                        &apdu_104_p->getDU_Info()->ASDU_CMTBL[ASDU_VSQ_INDEX]
    //                         );
    //

    //   if (funDsc_p->remoteSet != NULL)
    //   {
    //       tmpFlag = 1;
    //   }

    //   switch (tmpFlag)
    //   {
    //   case 0:
    //       {
    //           switch(funDsc_p->ssnDscNo)    //通过回话号确定功能;
    //           {
    //           case STN_INIT: //站初始化
    //               {
    //                   break;
    //               }

    //           case QUE_DATA: //查询数据
    //               {
    //                   if (act_prio_chnlID == PASSIVE_TYPE) //被控站会话
    //                   {
    //                       if (TID == C_RD_NA
    //                           && VSQ_p->N == C_N_1
    //                           && VSQ_p->SQ == C_SQ_0
    //                           && COT == MC_COT_5
    //                           )
    //                       {
    //                           stepNo = C_STEP_NO_ONE;
    //                       }
    //                   }
    //                   else if (act_prio_chnlID == PASSIVE_TYPE)//控制站会话
    //                   {
    //                       if ((TID >= MC_TID_1 && TID <= MC_TID_14
    //                           || TID == MC_TID_20
    //                           || TID == MC_TID_21
    //                           || TID >= MC_TID_30 && TID <= MC_TID_36
    //                           || TID == MC_TID_126)
    //                           && COT == MC_COT_5)
    //                       {
    //                           stepNo = C_STEP_NO_ONE;
    //                       }
    //                   }

    //                   break;
    //               }

    //           case CYC_TRNS: //周期传输
    //               {
    //                   if (act_prio_chnlID == ACTIVE_TYPE) //控制站会话
    //                   {
    //                       if (  (TID == MC_TID_9
    //                           || TID == MC_TID_11
    //                           || TID == MC_TID_13
    //                           || TID == MC_TID_21)
    //                           && COT == MC_COT_1
    //                           )
    //                       {
    //                           stepNo = C_STEP_NO_ONE;
    //                       }
    //                   }
    //                   break;
    //               }

    //           case ACQ_EVT: //事件收集
    //               {
    //                   if (act_prio_chnlID == ACTIVE_TYPE) //控制站会话
    //                   {
    //                       if ( (TID >= MC_TID_1 && TID <= MC_TID_21
    //                           || TID >= MC_TID_30 && TID <= MC_TID_40)
    //                           && COT == MC_COT_3
    //                           )
    //                       {
    //                           stepNo = C_STEP_NO_ONE;
    //                       }
    //                   }
    //                   break;
    //               }

    //           case QUKCHK_EVT: //快检事件
    //               {
    //                   //不使用
    //                   stepNo = 0xffff;
    //                   break;
    //               }

    //           case STN_INRGTN: //站召唤
    //               {
    //                   if (act_prio_chnlID == PASSIVE_TYPE)  //被控站会话
    //                   {
    //                       if (TID == C_IC_NA
    //                           && VSQ_p->N == C_N_1
    //                           && VSQ_p->SQ == C_SQ_0
    //                           && (COT == MC_COT_6 || COT == MC_COT_8)
    //                           ) //激活召唤;
    //                       {
    //                           stepNo = C_STEP_NO_ONE;
    //                       }

    //                   }
    //                   else if (act_prio_chnlID == ACTIVE_TYPE) //控制站会话
    //                   {
    //                       if (TID == C_IC_NA
    //                           && VSQ_p->N == C_N_1
    //                           && VSQ_p->SQ == C_SQ_0
    //                           && COT == MC_COT_7
    //                           ) //确认激活召唤;
    //                       {
    //                           stepNo = C_STEP_NO_ONE;
    //                       }
    //                       else if ( ( TID >= MC_TID_1
    //&& TID <= MC_TID_14
    //&& TID % 2 == 1
    //)
    //                           || TID == MC_TID_20
    //                           || TID == MC_TID_21
    //                           )
    //                           && COT >= MC_COT_20
    //                           && COT <= MC_COT_36
    //                           ) //响应站召唤;
    //                       {
    //                           stepNo =  C_STEP_NO_TWO;
    //                       }
    //                       else if (TID == C_IC_NA
    //                           && VSQ_p->N == C_N_1
    //                           && VSQ_p->SQ == C_SQ_0
    //                           && COT == MC_COT_10
    //                           ) //召唤信息结束通知
    //                       {
    //                           stepNo =  C_STEP_NO_THREE;
    //                       }
    //                   }

    //                   break;
    //               }//end case STN_INRGTN

    //           case CLK_SYN: //时钟同步
    //               {
    //                   if (act_prio_chnlID == PASSIVE_TYPE) //被控站会话
    //                   {
    //                       if (TID == C_CS_NA
    //                           && VSQ_p ->N == C_N_1
    //                           && VSQ_p ->SQ == C_SQ_0
    //                           && COT == MC_COT_6
    //                           ) //激活时钟同步;
    //                       {
    //                           stepNo = C_STEP_NO_ONE;
    //                       }
    //                   }
    //                   else if (act_prio_chnlID == ACTIVE_TYPE) //控制站会话
    //                   {
    //                       if (TID == C_CS_NA
    //                           && VSQ_p ->N == C_N_1
    //                           && VSQ_p ->SQ == C_SQ_0
    //                           && COT == MC_COT_7
    //                           ) //确认激活时钟同步;
    //                       {
    //                           stepNo = C_STEP_NO_ONE;
    //                       }
    //                   }
    //                   break;
    //               }
    //           case CMD_TRNS: //命令传输
    //               {
    //                   if (act_prio_chnlID == PASSIVE_TYPE) //被控站会话
    //                   {
    //                       if(  (TID >= MC_TID_45 && TID <= MC_TID_51
    //                           || TID >= MC_TID_58 && TID <= MC_TID_64)
    //                           &&(COT == MC_COT_6 || COT == MC_COT_8)
    //                           )
    //                       {
    //                           stepNo = C_STEP_NO_ONE;
    //                       }
    //                   }
    //                   else if(act_prio_chnlID == ACTIVE_TYPE) //主控站会话
    //                   {
    //                       if(  (TID >= MC_TID_45 && TID <= MC_TID_51
    //                           || TID >= MC_TID_58 && TID <= MC_TID_64)
    //                           &&(COT == MC_COT_7 || COT == MC_COT_9)
    //                           )
    //                       {
    //                           stepNo = C_STEP_NO_ONE;
    //                       }
    //                       else if(   (TID >= MC_TID_1 && TID <= MC_TID_64
    //                           || TID == MC_TID_20
    //                           || TID >= MC_TID_30 && TID <= MC_TID_32)
    //                           && (COT == MC_COT_11 || COT == MC_COT_12)
    //                           )
    //                       {
    //                           stepNo = C_STEP_NO_TWO;
    //                       }
    //                       else if((TID >= MC_TID_45 && TID <= MC_TID_51
    //                           || TID >= MC_TID_58 && TID <= MC_TID_64
    //                           )
    //                           && COT == MC_COT_10
    //                           )
    //                       {
    //                           stepNo = C_STEP_NO_THREE;
    //                       }
    //                   }

    //                   break;
    //               }
    //           case ACMU_FRZ:
    //               {//累积量传输前的冻结    此功能是从ACMU_TRNS功能的模式2，控制站
    //                //先冻结再请求传输分离出来的
    //                   if (act_prio_chnlID == PASSIVE_TYPE) //被控站会话
    //                   {
    //                       if(TID == MC_TID_101
    //                           && VSQ_p->N == C_N_1
    //                           && VSQ_p->SQ == C_SQ_0
    //                           && COT == MC_COT_6)
    //                       {
    //                           stepNo = C_STEP_NO_ONE;
    //                       }
    //                   }
    //                   else if(act_prio_chnlID == ACTIVE_TYPE) //主控站会话
    //                   {
    //                       if(TID == MC_TID_101
    //                           && VSQ_p->N == C_N_1
    //                           && VSQ_p->SQ == C_SQ_0
    //                           && COT == MC_COT_7)
    //                       {
    //                           stepNo = C_STEP_NO_ONE;
    //                       }
    //                   }
    //                   break;
    //               }
    //           case ACMU_TRNS: //累积量传输
    //               {

    //                   if (act_prio_chnlID == PASSIVE_TYPE) //被控站会话
    //                   {
    //                       if(TID == MC_TID_101
    //                           && VSQ_p->N == C_N_1
    //                           && VSQ_p->SQ == C_SQ_0
    //                           && COT == MC_COT_6)
    //                       {
    //                           stepNo = C_STEP_NO_ONE;
    //                       }
    //                   }
    //                   else if(act_prio_chnlID == ACTIVE_TYPE) //主控站会话
    //                   {
    //                       if (  (TID == MC_TID_15
    //                              || TID == MC_TID_16
    //                              || TID == MC_TID_37)
    //                           && COT == MC_COT_3
    //                           )
    //                       {
    //                           if (usingMode == CTLD_DRCTSONPT_TRNS)
    //                           {//模式0，被控站直接突发传输
    //                               stepNo = C_STEP_NO_ONE;
    //                           }
    //                           else if (usingMode == CTL_FRZ_CTLD_SONPT_TRNS)
    //                           {//模式3，控制站冻结后被控站突发传输
    //                               stepNo = C_STEP_NO_TWO;
    //                           }
    //
    //                       }
    //                       else if(TID == MC_TID_101
    //                           && VSQ_p->N == C_N_1
    //                           && VSQ_p->SQ == C_SQ_0
    //                           && COT == MC_COT_7)
    //                       {
    //                           stepNo = C_STEP_NO_ONE;
    //                       }
    //                       else if (  (TID == MC_TID_15
    //                                   || TID == MC_TID_16
    //                                   || TID == MC_TID_37)
    //                                && (COT >= MC_COT_37
    //                                    && COT <=MC_COT_41)
    //                               )
    //                       {
    //                           stepNo = C_STEP_NO_TWO;
    //                       }
    //                       else if(TID == MC_TID_101 && COT == MC_COT_10)
    //                       {
    //                           stepNo = C_STEP_NO_THREE;
    //                       }
    //                   }
    //                   break;
    //               }
    //           case PARA_LD: //参数加载
    //               {
    //                   if (act_prio_chnlID == PASSIVE_TYPE) //被控站会话
    //                   {
    //if(TID >= MC_TID_110
    //    && TID <= MC_TID_113
    //    &&COT == MC_COT_6
    //  )
    //{
    //    stepNo = C_STEP_NO_ONE;
    //}
    //                   }
    //                   else if(act_prio_chnlID == ACTIVE_TYPE) //主控站会话
    //                   {
    //                       if(TID >= MC_TID_110 && TID <= MC_TID_113
    //                           &&(COT == MC_COT_7 || COT == MC_COT_3)
    //                           )
    //                       {
    //                           stepNo = C_STEP_NO_ONE;
    //                       }
    //                   }
    //                   break;
    //               }
    //           case TST_PRCD: //测试过程
    //               {
    //                   if (act_prio_chnlID == PASSIVE_TYPE) //被控站会话
    //                   {
    //                       if ((TID == MC_TID_104 || TID == MC_TID_107)
    //                           && VSQ_p ->N == C_N_1
    //                           && VSQ_p ->SQ == C_SQ_0
    //                           && COT == MC_COT_6
    //                           ) //激活测试;
    //                       {
    //                           stepNo = C_STEP_NO_ONE;
    //                       }
    //                   }
    //                   else if (act_prio_chnlID == ACTIVE_TYPE) //控制站会话
    //                   {
    //                       if ((TID == MC_TID_104 || TID == MC_TID_107)
    //                           && VSQ_p ->N == C_N_1
    //                           && VSQ_p ->SQ == C_SQ_0
    //                           && COT == MC_COT_7
    //                           ) //确认激活测试;
    //                       {
    //                           stepNo = C_STEP_NO_ONE;
    //                       }
    //                   }
    //                   break;

    //               }
    //           case FILE_TRNS: //文件传输
    //               {
    //                   //暂不实现
    //                   break;
    //               }
    //           case ACQ_TRNSDLY: //收集传输延迟值
    //               {
    //                   //不使用
    //                   stepNo = 0xffff;
    //                   break;
    //               }

    //           default:
    //               break;
    //           }//end switch(funDsc_p->ssnDscNo

    //           break;
    //       }//end switch (tmpFlag) case 0:
    //   case 1:
    //       {
    //           ptclStep* tmpPtclStep = funDsc_p->remoteSet;
    //           for (int i = 0;i < funDsc_p->setNum; i++)
    //           {
    //               if ( TID == (tmpPtclStep + i)->TID
    //                   && COT == (tmpPtclStep + i)->COT
    //                   )
    //               {
    //                   stepNo = i + 1;
    //                   break;
    //               }
    //           }

    //           break;
    //       }//end switch (tmpFlag) case 1:
    //   default:
    //       break;
    //   }
    //

    //return stepNo;

}




/******************************************************************************
  *功能 ：对会话步中的帧进行验收
  * 输入：
  *     Fun_p        :步规则
  *     stepRl_p     :当前步
  *     funDsc_p     :会话号与主动性
  *     nxtStepNo_p  :上一帧的步号
  *输出：
  *     ssnPar_p     :详见此类型说明
  *     nxtStepNo_p  :下一帧的步号
  *返回：
  *  0               :成功，
  *  0xffff          :该功能不支持
  *  负数            :失败
  *                -1 :当前会话步错误
  *                -2 :当前会话步中的帧数量错误
  *                -3 :传送原因错误
  *                -4 : 主被动性参数错误
  *                -5 :限定词与实际元素个数不等
  *               -21 :最大值验收失败
  *               -22 :最小值验收失败
  *               -23 :按最大最小区间值验收
  *               其他负值为下层函数返回
  *修改历史
  *
 ******************************************************************************/
EXPDLL int chkFun ( FUN_RL* Fun_p,
                    ONE_STEP* stepRl_p,
                    T_ptclFunDsc* funDsc_p,
                    T_snnPara* ssnPar_p,
                    u_int* nxtStepNo_p
                  )
{
    if ( stepRl_p == NULL
         || funDsc_p == NULL
         || ssnPar_p == NULL
         || nxtStepNo_p == NULL
       )
    {
        return ERR_POINTER_NULL;
    }



    //主被动性会话标识
    u_char act_prio_chnlID     = ( u_char ) funDsc_p->act_prio_chnlID;

    if ( act_prio_chnlID != ACTIVE_TYPE
         && act_prio_chnlID != PASSIVE_TYPE
       )
    {
        return ERR_ACT_PAS_VAL;
    }

    //如果是U帧抽象出的特殊会话
    if ( funDsc_p->ssnDscNo == U_STARTDT
         || funDsc_p->ssnDscNo == U_STOPDT
         || funDsc_p->ssnDscNo == U_TESTFR
         || funDsc_p->ssnDscNo == U_NEWHOLD
         || funDsc_p->ssnDscNo == U_DELHOLD
       )
    {
        switch ( funDsc_p->ssnDscNo ) //判断功能号;
        {

            case U_STARTDT://104协议U帧STARTDT 抽象的会话
            {
                *nxtStepNo_p          = 2;
                ssnPar_p->ASDUID_p[0] = MC_TID_136;
                ssnPar_p->infoType    = GINFOTYPE;
                ssnPar_p->stnAcsFlag  = 0;
                ssnPar_p->cmnAddr     = 0;

                if ( act_prio_chnlID == PASSIVE_TYPE )
                {
                    ssnPar_p->stepNo     = 1;
                    ssnPar_p->actFlag    = ACTIVE_TYPE;
                    ssnPar_p->COT        = MC_COT_6;
                }
                else if ( act_prio_chnlID == ACTIVE_TYPE )
                {
                    ssnPar_p->stepNo     = 2;
                    ssnPar_p->actFlag    = PASSIVE_TYPE;
                    ssnPar_p->COT        = MC_COT_7;
                }

                break;
            }

            case U_STOPDT://104协议U帧STOPDT 抽象的会话
            {
                *nxtStepNo_p          = 2;
                ssnPar_p->ASDUID_p[0] = MC_TID_137;
                ssnPar_p->infoType    = GINFOTYPE;
                ssnPar_p->stnAcsFlag  = 0;
                ssnPar_p->cmnAddr     = 0;

                if ( act_prio_chnlID == PASSIVE_TYPE )
                {
                    ssnPar_p->stepNo     = 1;
                    ssnPar_p->actFlag    = ACTIVE_TYPE;
                    ssnPar_p->COT        = MC_COT_6;
                }
                else if ( act_prio_chnlID == ACTIVE_TYPE )
                {
                    ssnPar_p->stepNo     = 2;
                    ssnPar_p->actFlag    = PASSIVE_TYPE;
                    ssnPar_p->COT        = MC_COT_7;
                }

                break;
            }

            case U_TESTFR://104协议U帧TESTFR 抽象的会话
            {
                *nxtStepNo_p          = 2;
                ssnPar_p->ASDUID_p[0] = MC_TID_138;
                ssnPar_p->infoType    = GINFOTYPE;
                ssnPar_p->stnAcsFlag  = 0;
                ssnPar_p->cmnAddr     = 0;

                if ( act_prio_chnlID == PASSIVE_TYPE )
                {
                    ssnPar_p->stepNo     = 1;
                    ssnPar_p->actFlag    = ACTIVE_TYPE;
                    ssnPar_p->COT        = MC_COT_6;
                }
                else if ( act_prio_chnlID == ACTIVE_TYPE )
                {
                    ssnPar_p->stepNo     = 2;
                    ssnPar_p->actFlag    = PASSIVE_TYPE;
                    ssnPar_p->COT        = MC_COT_7;
                }

                break;
            }

            case U_NEWHOLD:
            {
                //用U帧的STARTDT STOPDT TESTFR 全为0 表示出新的会话：新联一个通道
                *nxtStepNo_p          = 2;
                ssnPar_p->ASDUID_p[0] = MC_TID_139;
                ssnPar_p->infoType    = GINFOTYPE;
                ssnPar_p->stnAcsFlag  = 0;
                ssnPar_p->cmnAddr     = 0;

                if ( act_prio_chnlID == PASSIVE_TYPE )
                {
                    ssnPar_p->stepNo     = 1;
                    ssnPar_p->actFlag    = ACTIVE_TYPE;
                    ssnPar_p->COT        = MC_COT_6;
                }
                else if ( act_prio_chnlID == ACTIVE_TYPE )
                {
                    ssnPar_p->stepNo     = 2;
                    ssnPar_p->actFlag    = PASSIVE_TYPE;
                    ssnPar_p->COT        = MC_COT_7;
                }

                break;
            }

            case U_DELHOLD:
            {
                //用U帧的STARTDT STOPDT TESTFR 全为1 表示出新的会话：关一个通道
                *nxtStepNo_p          = 2;
                ssnPar_p->ASDUID_p[0] = MC_TID_140;
                ssnPar_p->infoType    = GINFOTYPE;
                ssnPar_p->stnAcsFlag  = 0;
                ssnPar_p->cmnAddr     = 0;

                if ( act_prio_chnlID == PASSIVE_TYPE )
                {
                    ssnPar_p->stepNo     = 1;
                    ssnPar_p->actFlag    = ACTIVE_TYPE;
                    ssnPar_p->COT        = MC_COT_6;
                }
                else if ( act_prio_chnlID == ACTIVE_TYPE )
                {
                    ssnPar_p->stepNo     = 2;
                    ssnPar_p->actFlag    = PASSIVE_TYPE;
                    ssnPar_p->COT        = MC_COT_7;
                }

                break;
            }

            default:
                break;
        }
    }
    else//非U帧抽象出的特殊会话
    {
        if ( Fun_p == NULL
             || Fun_p->steps_p == NULL
             || Fun_p->steps_p[Fun_p->usingMode] == NULL
             || funDsc_p->remoteSet_p == NULL
           )
        {
            return ERR_POINTER_NULL;
        }

        if ( Fun_p->srvNo != funDsc_p->ssnDscNo )
        {
            return ERR_CONFIG;
        }

        //模式码
        u_char usingMode            = funDsc_p->usingMode;

        if ( usingMode != Fun_p->usingMode )
        {
            return ERR_CONFIG;
        }

        //获取规则步中当前步的指针;
        ONE_STEP* nowRuleStep_p = displayNextNode ( Fun_p->steps_p[usingMode],
                                  stepRl_p
                                                  );

        if ( nowRuleStep_p == NULL )
        {
            return ERR_POINTER_NULL;
        }

        //找到规则步最后一步
        ONE_STEP* lastRuleStep_p = Fun_p->steps_p[usingMode];

        while ( lastRuleStep_p->nextStep_p != NULL )
        {
            lastRuleStep_p    = lastRuleStep_p->nextStep_p;
        }

        //取步中相应的对象
        APDU104* apdu104_p = NULL;

        if ( act_prio_chnlID == PASSIVE_TYPE )
        {
            if ( stepRl_p->respAPDU_Obj_p == NULL )
            {
                return ERR_POINTER_NULL;
            }

            apdu104_p = ( APDU104* ) stepRl_p->respAPDU_Obj_p;
            ////上一帧与当前帧不是一步需要对步的某些值进行初始化
            //if (*nxtStepNo_p != stepRl_p->stepNo)
            //{
            //    stepRl_p->respPDU_Num = 0;
            //    stepRl_p->respPDU_FCode = 0;
            //    stepRl_p->reqPDU_Num = 1;
            //}
        }
        else//ACTIVE_TYPE
        {
            if ( stepRl_p->reqAPDU_Obj_p == NULL )
            {
                return ERR_POINTER_NULL;
            }

            apdu104_p = ( APDU104* ) stepRl_p->reqAPDU_Obj_p;
            ////上一帧与当前帧不是一步需要对步的某些值进行初始化
            //if (*nxtStepNo_p != stepRl_p->stepNo)
            //{
            //    stepRl_p->reqPDU_Num = 0;
            //    stepRl_p->reqPDU_FCode = 0;
            //    stepRl_p->respPDU_Num = 1;
            //}
        }

        int vRet    = 0;    //调用下层函数返回值
        //调用chkAsduDataID进行TID与COT的校验
        vRet =  chkAsduDataID ( nowRuleStep_p,
                                lastRuleStep_p,
                                stepRl_p->stepNo,
                                funDsc_p,
                                apdu104_p
                              );

        if ( vRet != 0 )
        {
            return vRet;
        }

        //调用chkVerifyBlock进行大小值校验:如果会话帧不需要校验
        //  则函数直接返回0
        vRet = chkVerifyBlock ( nowRuleStep_p->sVerifyBlock_p,
                                apdu104_p
                              );

        if ( vRet != 0 )
        {
            return vRet;
        }

        /*数据值的加载、保存标识:
            位0 = 1存储子系统执行数据值的保存；
            位1 = 1存储子系统执行数据值的加载；
                全 = 0, do nothing；全 = 1，加载且保存
            位3 - 10 分别表示APDU元素3 -10，
                =1表示，位0与位1的操作在此元素中有效，=0为无效；
            */
        T_stnAcsFlag tmpStnAcsFlag = {};
        //接收侧获得下一帧步号,存储、加载标识
        vRet = getChkNxtStepNo ( Fun_p,
                                 stepRl_p,
                                 funDsc_p,
                                 &tmpStnAcsFlag,
                                 nxtStepNo_p
                               );

        if ( vRet < 0 )
        {
            return vRet;
        }

        //根据stepRl_p及apdu104的相应内容给ssnPar_p赋值
        vRet = asgmtSnnPar ( stepRl_p,
                             funDsc_p,
                             apdu104_p,
                             &tmpStnAcsFlag,
                             ssnPar_p
                           );

        if ( vRet < 0 )
        {
            return vRet;
        }
    }

    return 0;
}




EXPDLL int bindToMem ( ONE_STEP* stepRl_p,
                       T_ptclFunDsc* funDsc_p,
                       u_char* ASDU_p,
                       u_short ASDUlng,
                       u_char duType
                     )
{
    APDU104* apdu104_p = NULL;
    //int asdu_BodySet = 0;
    u_char act_prio_chnlID = ( u_char ) funDsc_p->act_prio_chnlID;

    if ( act_prio_chnlID == PASSIVE_TYPE ) //是响应帧
    {
        apdu104_p = ( APDU104* ) stepRl_p->respAPDU_Obj_p;
    }
    else //是请求帧
    {
        apdu104_p = ( APDU104* ) stepRl_p->reqAPDU_Obj_p;
    }

    APDU_DSCR* apdu_dscr_p = apdu104_p->getDU_Info();

    if ( duType == ASDU_TYPE )
    {
        apdu_dscr_p->ASDU_CMTBL[APDU_APCI_INDEX] = 0;
    }

    apdu104_p->bindToMem ( ASDU_p, duType, ASDUlng );

    //u_short tidLng = 0;
    u_char TID = funDsc_p->realTID;

    //ASDU_TID_VAL *tmp_p = apdu104_p->getASDU_TID_VAL(&tidLng);
    if ( TID != 0 )
    {
        /*fusk@20180510 add 此处获apdu_dscr_p->InfoEleLng得可以考虑通过
          协议自己配置的g_TID104得到
          apdu_dscr_p->InfoEleLng = getTidLng ( TID );
        */

        //TID = *(u_char*)tmp_p;
        ptclStep*    tRemoteSet_p = & (
                                    funDsc_p->remoteSet_p[stepRl_p->stepNo - 1] );

        for ( int i = 0;
              i < tRemoteSet_p->TidNum;
              i++
            )
        {
            if ( tRemoteSet_p->TID_p != NULL
                 && tRemoteSet_p->TIDLng_p != NULL
                 && tRemoteSet_p->TID_p[i] == TID
               )
            {
                apdu_dscr_p->InfoEleLng = tRemoteSet_p->TIDLng_p[i];
                break;
            }
        }
    }

    if ( apdu104_p->getDU_Lng() < ASDUlng )
    {
        return -1;//apdu长度错误
    }

    return 0;
}




EXPDLL u_char* getDU ( u_char* apduObj_p,
                       u_short& apduLng,
                       char duType
                     )
{
    return ( ( APDU104* ) apduObj_p )->getDU ( apduLng, duType );
}




EXPDLL u_short getDULng ( u_char* apduObj_p )
{
    return ( ( APDU104* ) apduObj_p )->getDU_Lng();
}




EXPDLL void setAPDU_DSCR ( u_char* apduObj_p, APDU_DSCR apduDSCR )
{
    memcpy ( ( ( APDU104* ) apduObj_p )->getDU_Info(),
             &apduDSCR,
             sizeof ( APDU_DSCR ) );
}




//
//*功能：获取报文头长度
// *返回值：
// *        报文头长度
// */
EXPDLL u_char getHeaderlng()
{
    return 2;//返回104报头固定长度
}




/******************************************************************************
*功能：组装APDU，设置定时器对象，以及流量控制
*输入：
*      rtmParam_p   ：实时参数，包含了发送序号，接受序号 ， 以及K，W等值
*      APDU104_p    ：APDU104对象
*      ASDUFrm_Lng  : ASDU数据长度
*输出：
*      APDU104_p    : 封装后的APDU104对象
*返回：
*      void
*修改历史：
*
*******************************************************************************/
EXPDLL void packAPDU ( void* rtmParam_p,
                       void* cfgParam_p,
                       u_char* APDU104_p,
                       u_short ASDUFrm_Lng,
                       u_char* APDU_buf_p,
                       u_char& APDU_bufLng
                     )
{
    //计算APDU的地址
    //APDU_104->bindToMem(ASDUFrm_p - 6 ,ASDU_TYPE);
    APDU104* tApdu104_p  = ( APDU104* ) APDU104_p;
    u_char* APDUFrm_p    = tApdu104_p->getDU ( ASDUFrm_Lng, 1 );
    realTimeValue* rtm_p = ( realTimeValue* ) rtmParam_p; //实时参数

    if ( APDUFrm_p == NULL )
    {
        return ;
    }

    APCI_104* apci_p = ( APCI_104* ) APDU_buf_p;
    apci_p->startChar = SRTCHAR_F1_2_V;

    if ( ASDUFrm_Lng == 0 ) //U帧
    {
        //构造APCI帧头
        /*U_FRM u_frm_p     = {0};
        u_frm_p.frameType = 3;
        memcpy(apci_p->ctlField ,&u_frm_p , sizeof(U_FRM));*/

        memcpy ( apci_p, ( APCI_104* ) APDUFrm_p, sizeof ( APCI_104 ) );
    }
    else//I帧
    {
        I_FRM i_frm     = {0};
        i_frm.frameType = 0;
        i_frm.SNo       = rtm_p->sendnum  % 0x7fff;
        i_frm.pad       = 0;
        i_frm.RNo       = rtm_p->receivenum  % 0x7fff;
        memcpy ( apci_p->ctlField, &i_frm, sizeof ( I_FRM ) );
        apci_p->APDULng = ( u_char ) ASDUFrm_Lng +  sizeof ( I_FRM );
        rtm_p->sendnum  = ( rtm_p->sendnum  % 0x7fff );
        rtm_p->sendnum ++;
        rtm_p->kValue ++;
    }

    memcpy ( ( APDU_buf_p + sizeof ( APCI_104 ) ), APDUFrm_p, ASDUFrm_Lng );
    APDU_bufLng = ASDUFrm_Lng + sizeof ( APCI_104 );
}




EXPDLL u_short chkAPDUData ( revBuf* revDataFrm,
                             void* rtmParam_p,
                             void* cfgParam_p
                           )
{
    APCI_104* apci_p     = ( APCI_104* ) revDataFrm->APDU_Buf.APDU_Data_p;
    realTimeValue* rtm_p = ( realTimeValue* ) rtmParam_p; //实时参数

    frameType frmType = {0};

    if ( apci_p->startChar != SRTCHAR_F1_2_V )
    {
        //传输层接收到无法识别的帧
        frmType.fTypeL = MC_APDU_UNKNOWN;
        frmType.fTypeH = CLOSE_CONNECT;
        return * ( u_short* ) &frmType;
    }

    if ( apci_p->APDULng != revDataFrm->APDU_Buf.APDU_Lng - 2 )
    {
        //判断收到的帧长度
        frmType.fTypeL = MC_APDU_UNKNOWN;
        frmType.fTypeH = CLOSE_CONNECT;
        return * ( u_short* ) &frmType;
    }

    u_char frmeType_p = 0;
    getFrmType ( apci_p->ctlField, &frmeType_p );

    if ( frmeType_p == I_F )
    {
        I_FRM* i_frm_p = ( I_FRM* ) apci_p->ctlField;

        if ( ( i_frm_p->SNo % 0x7fff ) != ( rtm_p->receivenum % 0x7fff ) )
        {
            //传输层接收的APDU是否失序
            frmType.fTypeL = MC_APDU_DISORDER;
            frmType.fTypeH = CLOSE_CONNECT;
        }
        else
        {
            revDataFrm->APDU_Buf.APDU_Data_p += sizeof ( APCI_104 );
            revDataFrm->APDU_Buf.APDU_Lng    -= sizeof ( APCI_104 );
            frmType.fTypeL                    = NO_OPERATION;
            frmType.fTypeH                    = I_F;//0表示数据帧
            rtm_p->kValue = ( rtm_p->sendnum % 0x7fff ) - ( i_frm_p->RNo % 0x7fff );
        }
    }
    else if ( frmeType_p == S_F )
    {
        S_FRM* S_frm_p                 = ( S_FRM* ) apci_p->ctlField;
        revDataFrm->APDU_Buf.APDU_Lng  = 0;
        frmType.fTypeL                 = NO_OPERATION;
        frmType.fTypeH                 = S_F;//1表示控制帧
        rtm_p->kValue = ( rtm_p->sendnum % 0x7fff ) - ( S_frm_p->hRNo  % 0x7fff );

    }
    else  if ( frmeType_p == U_F )
    {
        revDataFrm->APDU_Buf.APDU_Lng  = 0;
        frmType.fTypeL                 = NO_OPERATION;
        frmType.fTypeH                 = U_F;////1表示控制帧
    }

    return * ( u_short* ) &frmType;
}




EXPDLL u_int CheckSendState ( short role,
                              void* rtmParam_p,
                              u_char* data_p,
                              u_short length
                            )
{
    sandMotion result = {0};
    // 获取当前通道的发送状态
    char curStage = ( ( realTimeValue* ) rtmParam_p )->stage; //rtmParam.stateStage;
    // 获取当前通道发送数据的类型
    u_char frmType = getFrameType ( data_p, length );

    // U帧同时存在多种功能
    if ( frmType == -1 )
    {
        return SEND_FRAME_ERROR;
    }

    switch ( role )
    {
        // 控制站
        case MASTER_STATION :
            switch ( curStage )
            {
                // STOPPED连接
                case MASTER_STOPPED:
                    if ( ( frmType & U_F ) == U_F )
                    {
                        result.errType = SEND_READY_STATUS;
                        result.motion  = WRITE_CONNECT;
                    }
                    else
                    {
                        result.errType = SEND_TRANSITION_ERROR;
                        result.motion  = CLOSE_CONNECT;
                    }

                    break;

                // 挂起未确认STOPPED连接
                case MASTER_UNCONFIRMED_STOPPED:
                    if ( ( ( frmType & U_F ) == U_F )
                         || ( frmType == I_F )
                         || ( frmType == S_F )
                       )
                    {
                        result.errType = SEND_READY_STATUS;
                        result.motion  = WRITE_CONNECT;
                    }
                    else
                    {
                        result.errType = SEND_TRANSITION_ERROR;
                        result.motion  = CLOSE_CONNECT;
                    }

                    break;

                // STARTED连接
                case MASTER_STARTED:
                    if ( ( ( frmType & U_F ) == U_F )
                         || ( frmType == I_F )
                         || ( frmType == S_F )
                       )
                    {
                        result.errType = SEND_READY_STATUS;
                        result.motion  = WRITE_CONNECT;
                    }
                    else
                    {
                        result.errType = SEND_TRANSITION_ERROR;
                        result.motion  = CLOSE_CONNECT;
                    }

                    break;

                // 挂起STARTED连接
                case MASTER_PEND_STARTED:
                    if ( ( frmType & U_F ) == U_F )
                    {
                        result.errType = SEND_READY_STATUS;
                        result.motion  = WRITE_CONNECT;
                    }
                    else
                    {
                        result.errType = SEND_TRANSITION_ERROR;
                        result.motion  = CLOSE_CONNECT;
                    }

                    break;

                // 挂起STOPPED连接
                case MASTER_PEND_STOPPED:
                    if ( ( ( frmType & U_F ) == U_F )
                         || ( frmType == I_F )
                         || ( frmType == S_F )
                       )
                    {
                        result.errType = SEND_READY_STATUS;
                        result.motion  = WRITE_CONNECT;
                    }
                    else
                    {
                        result.errType = SEND_TRANSITION_ERROR;
                        result.motion  = CLOSE_CONNECT;
                    }

                    break;

                // 非法的状态（状态机中不存在的状态）
                default :
                    result.errType = SEND_STATUS_ERROR;
                    result.motion  = CLOSE_CONNECT;

            }

            break;

        // 被控制站
        case SLAVE_STATION:
            switch ( curStage )
            {
                // STOPPED连接
                case SLAVE_STOPPED:
                    if ( ( frmType & U_F ) == U_F )
                    {
                        result.errType = SEND_READY_STATUS;
                        result.motion  = WRITE_CONNECT;
                    }
                    else
                    {
                        result.errType = SEND_TRANSITION_ERROR;
                        result.motion  = CLOSE_CONNECT;
                    }

                    break;

                // 挂起未确认STOPPED连接
                case SLAVE_UNCONFIRMED_STOPPED:
                    if ( ( ( frmType & U_F ) == U_F )
                         || ( frmType == S_F )
                       )
                    {
                        result.errType = SEND_READY_STATUS;
                        result.motion  = WRITE_CONNECT;
                    }
                    else
                    {
                        result.errType = SEND_TRANSITION_ERROR;
                        result.motion  = CLOSE_CONNECT;
                    }

                    break;

                // STARTED连接
                case SLAVE_STARTED:
                    if ( ( ( frmType & U_F ) == U_F )
                         || ( frmType == I_F )
                         || ( frmType == S_F )
                       )
                    {
                        result.errType = SEND_READY_STATUS;
                        result.motion  = WRITE_CONNECT;
                    }
                    else
                    {
                        result.errType = SEND_TRANSITION_ERROR;
                        result.motion  = CLOSE_CONNECT;
                    }

                    break;

                // 非法的状态（状态机中不存在的状态）
                default:
                    result.errType = SEND_STATUS_ERROR;
                    result.motion  = CLOSE_CONNECT;
            }

            break;

        // 非法站点角色
        default:
            result.errType = SEND_ROLE_ERROR;
            result.motion  = CLOSE_CONNECT;
    }

    return * ( u_int* ) &result;
}




EXPDLL u_int CheckRecvState ( short role,
                              void* rtmParam_p,
                              u_char* data_p,
                              u_short length
                            )
{
    sandMotion result = {0};
    // 获取当前通道的发送状态
    char curStage = ( ( realTimeValue* ) rtmParam_p )->stage; //rtmParam.stateStage;
    // 获取当前通道发送数据的类型
    u_char frmType = getFrameType ( data_p, length );

    // U帧同时存在多种功能
    if ( frmType == -1 )
    {
        return SEND_FRAME_ERROR;
    }

    switch ( role )
    {
        // 控制站
        case MASTER_STATION :
            switch ( curStage )
            {
                // STOPPED连接
                case MASTER_STOPPED:
                    if ( ( frmType & U_F ) == U_F )
                    {
                        result.errType = SEND_READY_STATUS;
                        result.motion  = READ_CONNECT;
                    }
                    else
                    {
                        result.errType = SEND_TRANSITION_ERROR;
                        result.motion  = CLOSE_CONNECT;
                    }

                    break;

                // 挂起未确认STOPPED连接
                case MASTER_UNCONFIRMED_STOPPED:
                    if ( ( ( frmType & U_F ) == U_F )
                         || ( frmType == I_F )
                         || ( frmType == S_F )
                       )
                    {
                        result.errType = SEND_READY_STATUS;
                        result.motion  = READ_CONNECT;
                    }
                    else
                    {
                        result.errType = SEND_TRANSITION_ERROR;
                        result.motion  = CLOSE_CONNECT;
                    }

                    break;

                // STARTED连接
                case MASTER_STARTED:
                    if ( ( ( frmType & U_F ) == U_F )
                         || ( frmType == I_F )
                         || ( frmType == S_F )
                       )
                    {
                        result.errType = SEND_READY_STATUS;
                        result.motion  = READ_CONNECT;
                    }
                    else
                    {
                        result.errType = SEND_TRANSITION_ERROR;
                        result.motion  = CLOSE_CONNECT;
                    }

                    break;

                // 挂起STARTED连接
                case MASTER_PEND_STARTED:
                    if ( ( frmType & U_F ) == U_F )
                    {
                        result.errType = SEND_READY_STATUS;
                        result.motion  = READ_CONNECT;
                    }

                    else
                    {
                        result.errType = SEND_TRANSITION_ERROR;
                        result.motion  = CLOSE_CONNECT;
                    }

                    break;

                // 挂起STOPPED连接
                case MASTER_PEND_STOPPED:
                    if ( ( ( frmType & U_F ) == U_F )
                         || ( frmType == I_F )
                         || ( frmType == S_F )
                       )
                    {
                        result.errType = SEND_READY_STATUS;
                        result.motion  = READ_CONNECT;
                    }
                    else
                    {
                        result.errType = SEND_TRANSITION_ERROR;
                        result.motion  = CLOSE_CONNECT;
                    }

                    break;

                // 非法的状态（状态机中不存在的状态）
                default :
                    result.errType = SEND_STATUS_ERROR;
                    result.motion  = CLOSE_CONNECT;

            }

            break;

        // 被控制站
        case SLAVE_STATION:
            switch ( curStage )
            {
                // STOPPED连接
                case SLAVE_STOPPED:
                    if ( ( frmType & U_F ) == U_F )
                    {
                        result.errType = SEND_READY_STATUS;
                        result.motion  = READ_CONNECT;
                    }
                    else
                    {
                        result.errType = SEND_TRANSITION_ERROR;
                        result.motion  = CLOSE_CONNECT;
                    }

                    break;

                // 挂起未确认STOPPED连接
                case SLAVE_UNCONFIRMED_STOPPED:
                    if ( ( ( frmType & U_F ) == U_F )
                         || ( frmType == S_F )
                       )
                    {
                        result.errType = SEND_READY_STATUS;
                        result.motion  = READ_CONNECT;
                    }
                    else
                    {
                        result.errType = SEND_TRANSITION_ERROR;
                        result.motion  = CLOSE_CONNECT;
                    }

                    break;

                // STARTED连接
                case SLAVE_STARTED:
                    if ( ( ( frmType & U_F ) == U_F )
                         || ( frmType == I_F )
                         || ( frmType == S_F )
                       )
                    {
                        result.errType = SEND_READY_STATUS;
                        result.motion  = READ_CONNECT;
                    }
                    else
                    {
                        result.errType = SEND_TRANSITION_ERROR;
                        result.motion  = CLOSE_CONNECT;
                    }

                    break;

                // 非法的状态（状态机中不存在的状态）
                default:
                    result.errType = SEND_STATUS_ERROR;
                    result.motion  = CLOSE_CONNECT;
            }

            break;

        // 非法站点角色
        default:
            result.errType = SEND_ROLE_ERROR;
            result.motion  = CLOSE_CONNECT;
    }

    return * ( u_int* ) &result;
}




EXPDLL u_short UpdateSendState ( short role,
                                 void* rtmParam_p,
                                 void* cfgParam_p,
                                 u_char* tmrCtrlCode_p,
                                 u_char* data_p,
                                 u_short length
                               )
{
    // 框架协调控制变量
    u_short result = 0;
    realTimeValue* rtm_p = ( realTimeValue* ) rtmParam_p;

    // 获取当前通道的发送状态
    char curStage = rtm_p->stage;  //rtmParam.stateStage;
    //  声明下一个状态
    //      char nxtStage;

    // 获取当前通道发送数据帧的类型
    u_char frmType = getFrameType ( data_p, length );


    // 1.更新发送状态
    switch ( role )
    {
        // 控制站
        case MASTER_STATION :
            switch ( curStage )
            {
                // STOPPED连接
                case MASTER_STOPPED:
                    if ( ( frmType == IEC104_STARTDT_ACK )
                         || ( frmType == IEC104_STARTDT_ACT )
                       )
                    {
                        ( ( realTimeValue* ) rtmParam_p )->stage = MASTER_PEND_STARTED;
                    }
                    else
                    {
                        ( ( realTimeValue* ) rtmParam_p )->stage = curStage;
                    }

                    break;

                // 挂起STARTED连接
                case MASTER_PEND_STARTED:
                    if ( ( frmType == IEC104_STARTDT_ACK ) )
                    {
                        ( ( realTimeValue* ) rtmParam_p )->stage = MASTER_STARTED;
                    }
                    else
                    {
                        ( ( realTimeValue* ) rtmParam_p )->stage = curStage;
                    }

                    break;

                // STARTED连接
                case MASTER_STARTED:
                    if ( ( frmType == IEC104_STARTDT_ACT ) )
                    {
                        ( ( realTimeValue* ) rtmParam_p )->stage = MASTER_STARTED;
                    }
                    else
                    {
                        ( ( realTimeValue* ) rtmParam_p )->stage = curStage;
                    }

                    break;

                // 挂起未确认STOPPED连接
                case MASTER_UNCONFIRMED_STOPPED:
                    if ( ( frmType == IEC104_STOPDT_ACK ) )
                    {
                        ( ( realTimeValue* ) rtmParam_p )->stage = MASTER_STOPPED;
                    }
                    else
                    {
                        ( ( realTimeValue* ) rtmParam_p )->stage = curStage;
                    }

                    break;

                // 挂起STOPPED连接
                case MASTER_PEND_STOPPED:
                    if ( ( frmType == IEC104_STOPDT_ACK ) )
                    {
                        ( ( realTimeValue* ) rtmParam_p )->stage = MASTER_STOPPED;
                    }
                    else
                    {
                        ( ( realTimeValue* ) rtmParam_p )->stage = curStage;
                    }

                    break;

                // 非法的状态（状态机中不存在的状态）
                default :
                    break;
            }

            break;

        // 被控制站
        case SLAVE_STATION:
            switch ( curStage )
            {
                // STOPPED连接
                case SLAVE_STOPPED:
                    if ( ( frmType == IEC104_STARTDT_ACK )
                         || ( frmType == IEC104_STARTDT_ACT )
                       )
                    {
                        ( ( realTimeValue* ) rtmParam_p )->stage = SLAVE_STARTED;
                    }
                    else
                    {
                        ( ( realTimeValue* ) rtmParam_p )->stage = curStage;
                    }

                    break;

                // STARTED连接
                case SLAVE_STARTED:
                    if ( ( frmType == IEC104_STOPDT_ACT ) )
                    {
                        ( ( realTimeValue* ) rtmParam_p )->stage = SLAVE_UNCONFIRMED_STOPPED;
                    }
                    else
                    {
                        ( ( realTimeValue* ) rtmParam_p )->stage = curStage;
                    }

                    break;

                // 挂起未确认STOPPED连接
                case SLAVE_UNCONFIRMED_STOPPED:
                    if ( ( frmType == U_F ) || ( frmType == S_F ) )
                    {
                        result = 0;
                    }
                    else
                    {
                        result = 5;
                    }

                    break;

                // 非法的状态（状态机中不存在的状态）
                default:
                    result = 2;
            }

            break;

        default:
            break;
    }

    // 2. 更新流量控制实时参数
    if ( frmType == I_F )
    {
        //rtm_p->sendnum++;
        //rtm_p->kValue = (rtm_p->sendnum  % 0x7fff) - (rtm_p->receivenum  % 0x7fff);
        //      rtm_p->sendnum ++;
        //      rtm_p->sendnum = (rtm_p->sendnum  % 0x7fff) ;
        //      rtm_p->kValue ++;
        if ( rtm_p->kValue >= ( ( realTimeThreshold* ) cfgParam_p )->K )
        {
            result |= 0x2;
        }

        rtm_p->wValue = 0 ;
        ( ( timerType* ) tmrCtrlCode_p )->T2 = TIMER_STOP;
    }

    // 3.更新定时器的控制参数，设置定时器的验证码。
    if ( ( frmType == I_F ) |
         ( frmType == IEC104_STOPDT_ACT ) |
         ( frmType == IEC104_STARTDT_ACT ) |
         ( frmType == IEC104_TESTFR_ACT ) )
    {
        ( ( timerType* ) tmrCtrlCode_p )->T1 = TIMER_PAUSE; //T1启动
    }

    ( ( timerType* ) tmrCtrlCode_p )->T3 = TIMER_PAUSE; //T3启动
    result |= 0x1;
    return result;
}




EXPDLL u_short UpdateRecvState ( short role,
                                 void* rtmParam_p,
                                 void* cfgParam_p,
                                 u_char* tmrCtrlCode_p,
                                 u_char* data_p,
                                 u_short length
                               )
{
    // 框架协调控制变量
    ctrl_Value result = {0};

    realTimeValue* rtm_p = ( realTimeValue* ) rtmParam_p;
    // 获取当前通道的发送状态
    //char curStage = rtm_p->stage;  //rtmParam.stateStage;
    // 声明下一个状态
    //char nxtStage;

    // 获取当前通道发送数据帧的类型
    u_char frmType = getFrameType ( data_p, length );

    //
    switch ( frmType )
    {
        case IEC104_TESTFR_ACK:
            result.fType = IEC104_TESTFR_ACK;
            result.bit3  = 1;
            ( ( timerType* ) tmrCtrlCode_p )->T1 = TIMER_STOP; //T1停止
            break;

        case IEC104_TESTFR_ACT:
            result.fType = IEC104_TESTFR_ACT;
            result.bit3  = 1;
            break;

        case IEC104_STOPDT_ACK:
            result.fType = IEC104_STOPDT_ACK;
            rtm_p->stage = MASTER_STOPPED;
            result.bit3  = 1;
            ( ( timerType* ) tmrCtrlCode_p )->T1 = TIMER_STOP; //T1停止
            break;

        case IEC104_STOPDT_ACT:
            result.fType = IEC104_STOPDT_ACT;

            if ( rtm_p->wValue != 0 )
            {
                rtm_p->stage = SLAVE_UNCONFIRMED_STOPPED;
            }
            else
            {
                rtm_p->stage = SLAVE_STOPPED;
            }

            result.bit3  = 1;
            break;

        case IEC104_STARTDT_ACK:
            result.fType = IEC104_STARTDT_ACK;
            rtm_p->stage = MASTER_STARTED;
            result.bit3  = 1;
            ( ( timerType* ) tmrCtrlCode_p )->T1 = TIMER_STOP; //T1停止
            break;

        case IEC104_STARTDT_ACT:
            result.fType = IEC104_STARTDT_ACT;
            rtm_p->stage = MASTER_STARTED;
            result.bit3  = 1;
            break;

        case I_F:
            result.fType = I_F;

            if ( rtm_p->receivenum == 0x7fff )
            {
                rtm_p->receivenum = ( rtm_p->receivenum  % 0x7fff );
            }
            else
            {
                rtm_p->receivenum = ( rtm_p->receivenum  % 0x7fff );
                rtm_p->receivenum ++ ;
            }

            rtm_p->wValue ++;
            ( ( timerType* ) tmrCtrlCode_p )->T2 = TIMER_PAUSE; //T2复位

            if ( rtm_p->wValue == ( ( realTimeThreshold* ) cfgParam_p )->W )
            {
                result.bit1 = 1;
                result.bit2  = 1;
            }

            ( ( timerType* ) tmrCtrlCode_p )->T1 = TIMER_STOP ; //T1停止
            //rtm_p->kValue =  (rtm_p->sendnum  % 0x7fff) - (rtm_p->receivenum  % 0x7fff);
            break;

        case S_F:
            result.fType = S_F;
            result.bit1  =
            1; //收到S帧有两种情况，1.t2定时器超时 ，W值已到最大值

            //rtm_p->kValue = (rtm_p->sendnum  % 0x7fff) - (rtm_p->receivenum  % 0x7fff);
            if ( rtm_p->wValue == 0 )
            {
                ( ( timerType* ) tmrCtrlCode_p )->T1 = TIMER_STOP ; //T1停止
            }

            break;

        default:
            break;
    }

    ( ( timerType* ) tmrCtrlCode_p )->T3 = TIMER_PAUSE ; //T3启动
    result.bit0 = 1;//操作定时器
    return * ( u_short* ) &result;
}




EXPDLL int getFrameType ( u_char* data_p, u_short length )
{
    /****************    IEC104协议的应用数据格式    ***************************/
    /*|-----------------------|*/
    /*|启动字符68H (一个字节) |*/
    /*|-----------------------|*/
    /*|APDU长度    (一个字节) |*/
    /*|-----------------------|*/
    /*|控制域      (四个字节) |*/
    /*|-----------------------|*/
    /*|ASDU数据    (变长字节) |*/
    /*|-----------------------|*/

    /****************    控制域的I帧数据格式    ********************************/
    /*|----------------------------------------------------|                 */
    /*| bit8 | bit7| bit6| bit5 | bit4 | bit3| bit2 | bit1 |                 */
    /*|---------------------------------------------|------|                 */
    /*|                 发送序列号N(S)           LSB|   0  |  八位位组1       */
    /*|---------------------------------------------|------|                 */
    /*| MSB             发送序列号N(S)                     |  八位位组2       */
    /*|----------------------------------------------------|                 */
    /*|                 接收序列号R(S)           LSB|  0   |  八位位组3       */
    /*|----------------------------------------------------|                 */
    /*| MSB             接收序列号N(S)                     |  八位位组4       */
    /*|----------------------------------------------------|                 */

    /****************    控制域的S帧数据格式    ********************************/
    /*|----------------------------------------------------|                 */
    /*| bit8 | bit7| bit6| bit5 | bit4 | bit3| bit2 | bit1 |                 */
    /*|--------------------------------------|------|------|                 */
    /*|                  0                   |   0  |   1  |  八位位组1       */
    /*|--------------------------------------|------|------|                 */
    /*|                         0                          |  八位位组2       */
    /*|----------------------------------------------------|                 */
    /*|                 接收序列号R(S)           LSB |   0 |  八位位组3       */
    /*|----------------------------------------------------|                 */
    /*| MSB             接收序列号N(S)                     |  八位位组4       */
    /*|----------------------------------------------------|                 */

    /****************    控制域的U帧数据格式    ********************************/
    /*|----------------------------------------------------|                 */
    /*| bit8 | bit7| bit6| bit5 | bit4 | bit3| bit2 | bit1 |                 */
    /*|------------|------------|------------|------|------|                 */
    /*|   TESTFR   |   STOPDT   |   STARTDT  |      |      |                 */
    /*|------------|------------|------------|  1   |   1  |  八位位组1      */
    /*| 确认 | 激活| 确认 | 激活| 确认 | 激活|      |      |                  */
    /*|------------|------------|------------|-------------|                 */
    /*|                         0                          |  八位位组2       */
    /*|----------------------------------------------------|                 */
    /*|                         0                          |  八位位组3       */
    /*|----------------------------------------------------|                 */
    /*|                         0                          |  八位位组4       */
    /*|----------------------------------------------------|                 */
    if ( data_p == NULL )
    {
        return ERR_POINTER_NULL;
    }

    //计算APDU的地址
    APCI_104* apci_p = ( APCI_104* ) data_p ;

    if ( ( apci_p->ctlField[0] & U_F ) == U_F )
    {
        //char data_type = apci_p->ctlField[0]; // 控制域八位位组1
        switch ( apci_p->ctlField[0] )
        {
            case IEC104_TESTFR_ACK:
                return IEC104_TESTFR_ACK;
                break;

            case IEC104_TESTFR_ACT:
                return IEC104_TESTFR_ACT;
                break;

            case IEC104_STOPDT_ACK:
                return IEC104_STOPDT_ACK;
                break;

            case IEC104_STOPDT_ACT:
                return IEC104_STOPDT_ACT;
                break;

            case IEC104_STARTDT_ACK:
                return IEC104_STARTDT_ACK;
                break;

            case IEC104_STARTDT_ACT:
                return IEC104_STARTDT_ACT;
                break;

            case IEC104_NEWHOLD_ACT:
                return IEC104_NEWHOLD_ACT;
                break;

            case IEC104_DELHOLD_ACT:
                return IEC104_DELHOLD_ACT;
                break;

            case IEC104_NEWHOLD_ACK:
                return IEC104_NEWHOLD_ACK;
                break;

            case IEC104_DELHOLD_ACK:
                return IEC104_DELHOLD_ACK;
                break;

            default: // 同时存在多种功能或者为非U帧数据
                return -1;
        }
    }
    else if ( ( apci_p->ctlField[0] & S_F ) == S_F )
    {
        return S_F;
    }

    else//I帧
    {
        return I_F;
    }

    return -1;
}




EXPDLL int HandleRecvStateError ( transCtrlFrmDsc* ctrlFrm,
                                  void* rtmParam_p,
                                  int error
                                )
{
    //此函数对于104规约的处理都是断开连接
    return CLOSE_CONNECT;
}




/*******************************************************************************
* 功能：构造传输控制的应答帧数据
* 描述：按照IEC104协议标准，本函数应当具有以下情况的功能：
*     1.如果接收数据为控制帧类型，构造相应的应答帧；比如接收到U帧的“激活帧”，需要
*       构造对应的“确认帧”；
*     2.如果接收数据为数据帧类型，且接收数量达到接收上限阀值W时，需要构造一个数据确
*       认帧，即S帧。
* 接口参数：
*

* 返回值：用于协调框架控制，返回值含义如下：
*     如果返回值为1，则表示需要框架对定时器进行设置，否则不需要设置
*******************************************************************************/
EXPDLL int RespCtrlFrame ( transCtrlFrmDsc* ctrlFrm,
                           void* rtmParam_p,
                           int result
                         )
{
    if ( ctrlFrm == NULL
         || ctrlFrm->ctrlFrm_p == NULL
       )
    {
        return ERR_POINTER_NULL;
    }

    realTimeValue* rtm_p = ( realTimeValue* ) rtmParam_p;
    APCI_104* apci = ( APCI_104* ) ( ctrlFrm->ctrlFrm_p );
    ctrlFrm->frmLng = sizeof ( APCI_104 );
    apci->startChar = SRTCHAR_F1_2_V;
    apci->APDULng = sizeof ( GEN_FRM );

    switch ( result )
    {
        case IEC104_TESTFR_ACK:
            return -1;
            break;

        case IEC104_TESTFR_ACT:
        {
            U_FRM u_f = {0};
            u_f.frameType = U_F;
            u_f.TSTFR_CON = 1;
            memcpy ( ( char* ) apci->ctlField, &u_f, sizeof ( U_FRM ) );
            ctrlFrm->ctrlType = U_F;
        }
        break;

        case IEC104_STOPDT_ACK:
            return -1;
            break;

        case IEC104_STOPDT_ACT:
        {
            U_FRM u_f = {0};
            u_f.frameType = U_F;
            u_f.STPDT_CON = 1;
            memcpy ( ( char* ) apci->ctlField, &u_f, sizeof ( U_FRM ) );
            ctrlFrm->ctrlType = U_F;
        }
        break;

        case IEC104_STARTDT_ACK:
            return -1;
            break;

        case IEC104_STARTDT_ACT:
        {
            U_FRM u_f = {0};
            u_f.frameType = U_F;
            u_f.SRTDT_CON = 1;
            memcpy ( ( char* ) apci->ctlField, &u_f, sizeof ( U_FRM ) );
            ctrlFrm->ctrlType = U_F;
        }
        break;

        case S_F:
        {
            if ( rtm_p == NULL )
            {
                return ERR_POINTER_NULL;
            }

            S_FRM u_s = {0};
            u_s.frameType = S_F;
            u_s.lRNo = rtm_p->receivenum;
            memcpy ( ( char* ) apci->ctlField, &u_s, sizeof ( S_FRM ) );
            ctrlFrm->ctrlType = S_F;
            rtm_p->wValue = 0;
        }
        break;

        case I_F:
        {
            if ( rtmParam_p == NULL )
            {
                return ERR_POINTER_NULL;
            }

            S_FRM u_s = {0};
            u_s.frameType = S_F;
            u_s.lRNo = 0;
            u_s.hRNo = ( rtm_p->receivenum );
            memcpy ( ( char* ) apci->ctlField, &u_s, sizeof ( S_FRM ) );
            ctrlFrm->ctrlType = S_F;
            rtm_p->wValue = 0;
        }
        break;

        default: // 同时存在多种功能或者为非U帧数据
            return -1;
    }

    return 0;
}




EXPDLL int CtrlFrame ( transCtrlFrmDsc* ctrlFrm,
                       void* rtmParam_p,
                       int result
                     )
{
    if ( ctrlFrm == NULL
         || ctrlFrm->ctrlFrm_p == NULL
       )
    {
        return ERR_POINTER_NULL;
    }

    realTimeValue* rtm_p = ( realTimeValue* ) rtmParam_p;
    APCI_104* apci = ( APCI_104* ) ( ctrlFrm->ctrlFrm_p );
    ctrlFrm->frmLng = sizeof ( APCI_104 );
    apci->startChar = SRTCHAR_F1_2_V;
    apci->APDULng = sizeof ( GEN_FRM );

    switch ( result )
    {
        case IEC104_TESTFR_ACK:
        {
            U_FRM u_f = {0};
            u_f.frameType = U_F;
            u_f.TSTFR_CON = 1;
            memcpy ( ( char* ) apci->ctlField, &u_f, sizeof ( U_FRM ) );
            ctrlFrm->ctrlType = U_F;
        }
        break;

        case IEC104_TESTFR_ACT:
        {
            U_FRM u_f = {0};
            u_f.frameType = U_F;
            u_f.TSTFR_ACT = 1;
            memcpy ( ( char* ) apci->ctlField, &u_f, sizeof ( U_FRM ) );
            ctrlFrm->ctrlType = U_F;
        }
        break;

        case IEC104_STOPDT_ACK:
        {
            U_FRM u_f = {0};
            u_f.frameType = U_F;
            u_f.STPDT_CON = 1;
            memcpy ( ( char* ) apci->ctlField, &u_f, sizeof ( U_FRM ) );
            ctrlFrm->ctrlType = U_F;
        }
        break;

        case IEC104_STOPDT_ACT:
        {
            U_FRM u_f = {0};
            u_f.frameType = U_F;
            u_f.STPDT_ACT = 1;
            memcpy ( ( char* ) apci->ctlField, &u_f, sizeof ( U_FRM ) );
            ctrlFrm->ctrlType = U_F;
        }
        break;

        case IEC104_STARTDT_ACK:
        {
            U_FRM u_f = {0};
            u_f.frameType = U_F;
            u_f.SRTDT_CON = 1;
            memcpy ( ( char* ) apci->ctlField, &u_f, sizeof ( U_FRM ) );
            ctrlFrm->ctrlType = U_F;
        }
        break;

        case IEC104_STARTDT_ACT:
        {
            U_FRM u_f = {0};
            u_f.frameType = U_F;
            u_f.SRTDT_ACT = 1;
            memcpy ( ( char* ) apci->ctlField, &u_f, sizeof ( U_FRM ) );
            ctrlFrm->ctrlType = U_F;
        }
        break;

        case S_F:
        {
            if ( rtm_p == NULL )
            {
                return ERR_POINTER_NULL;
            }

            if ( rtm_p->wValue > 0 )
            {
                S_FRM u_s = {0};
                u_s.frameType = S_F;
                u_s.lRNo = 0;
                u_s.hRNo = ( rtm_p->receivenum );
                memcpy ( ( char* ) apci->ctlField, &u_s, sizeof ( S_FRM ) );
                ctrlFrm->ctrlType = S_F;
                rtm_p->wValue = 0;
            }
            else
            {
                ctrlFrm->ctrlFrm_p = 0;
                ctrlFrm->frmLng = 0;
            }
        }
        break;

        case I_F:
            return -1;
            break;

        default: // 同时存在多种功能或者为非U帧数据
            return -1;
    }

    return 0;
}




EXPDLL int APDUbindToMem ( u_char* apduObj_p,
                           u_char* APDU_p,
                           u_short ASDUlng,
                           u_char duType
                         )
{
    APDU104* apdu104_p = NULL;
    //int asdu_BodySet = 0;
    apdu104_p = ( APDU104* ) apduObj_p;

    //APDU_DSCR* apdu_dscr_p = apdu104_p->getDU_Info();
    apdu104_p->bindToMem ( APDU_p, duType, ASDUlng );

    if ( apdu104_p->getDU_Lng() < ASDUlng )
    {
        return -1;//apdu长度错误
    }

    return 0;
}




/******************************************************************************
  *功能：从缓冲区获得APDU的存储长度
  * 输入：
  *      bufferAdd   ：缓冲区首地址
  *      bufferLng   : 缓冲区长度
  *输出：
  *      无
  *返回：
  *    >0 成功返回APDU长度
  *    －1 缓冲区长度错误
  *    －2 首地址不是启动字符
  *    <0  接口执行异常
  *修改历史
  *
*******************************************************************************/
EXPDLL int getApduLngFrmBuf ( char*  bufferAdd, int bufferLng )
{
    if ( bufferAdd == NULL )
    {
        return ERR_POINTER_NULL;
    }

    if ( bufferLng <= 0 )
    {
        return -1;
    }

    if ( *bufferAdd != SRTCHAR_F1_2_V )
    {
        return -2;
    }

    int apduPLng = * ( u_char* ) ( bufferAdd + 1 ) + 2;


    return apduPLng;
}




/******************************************************************************
  *功能：定时器T0，T1，T2，T3超时处理函数
  * 输入：
  *      parameter_p   ：为保留参数暂时无用
  *返回：
  *   低第1位（位0）：至1为关闭定时器
  *   低第2位（位1）：至1为关闭链接
  *   低第3位（位2）：至1为发送S帧
  *   低第4位（位3）：至1为发送测试激活
  *修改历史
  *
*******************************************************************************/
EXPDLL int timerRingT0 ( void* parameter_p )
{
    int timeOut = 0;
    timeOut = timeOut | 0x3;
    return timeOut;
}




EXPDLL int timerRingT1 ( void* parameter_p )
{
    int timeOut = 0;
    timeOut = timeOut | 0x3;
    return timeOut;
}




EXPDLL int timerRingT2 ( void* parameter_p )
{
    int timeOut = 0;
    timeOut = timeOut | 0x4;
    return timeOut;
}




EXPDLL int timerRingT3 ( void* parameter_p )
{
    int timeOut = 0;
    timeOut = timeOut | 0x8;
    return timeOut;

}




EXPDLL int timerRingAdd ( u_char timerId )
{
    int parameter_p;

    switch ( timerId )
    {
        case 0:
            return timerRingT0 ( &parameter_p );
            break;

        case 1:
            return timerRingT1 ( &parameter_p );
            break;

        case 2:
            return timerRingT2 ( &parameter_p );
            break;

        case 3:
            return timerRingT3 ( &parameter_p );
            break;

        default:
            return NULL;
            break;
    }
}




EXPDLL int getSaveCom ( T_ptclFunDsc* funDsc_p )
{
    if ( funDsc_p->realTID > MC_TID_41
         && funDsc_p->realTID < MC_TID_140
       )
    {
        return -1;
    }
    else
    {
        return 0;
    }
}






