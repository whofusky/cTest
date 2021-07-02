#define DLL_IMPLEMENT

#include "protocol104.h"
#include "InternalFunction.h"



//ע���˱����ռ��body_setֵ��׼���ô˱�����Ҫ����body_setǰ��Ŀռ�
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
        case STN_INIT://վ��ʼ��
        {
            break;
        }

        case QUE_DATA: //��ѯ����
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

            if ( usingMode != 0xFF ) //�����Ϊ׼
            {
                if ( usingMode != RD_DATA )
                {
                    freeRl_P ( Fun_p, NULL, NULL );
                    return NULL;
                }
            }
            else//����ѡһ�����Ե�ֵ
            {
                usingMode = RD_DATA;
            }

            ptclFunDsc->usingMode = usingMode;

            u_char stepNum = 2;

            //1���Բ�����Fun_p��ֵ
            Fun_p->srvNo              = QUE_DATA;
            Fun_p->usingMode          = RD_DATA;
            Fun_p->cmLvl[POLL_EVT]    = UNKNOWN;
            Fun_p->cmLvl[POLL_DATA]   = UNKNOWN;
            Fun_p->cmLvl[RD_DATA]     = APP_LVL; // 104��ԼȡAPP_LVL
            Fun_p->stepNum[POLL_EVT]  = UNKNOWN;
            Fun_p->stepNum[POLL_DATA] = UNKNOWN;
            Fun_p->stepNum[RD_DATA]   = stepNum;
            //��һ��
            ONE_STEP* one_step_p        = new ONE_STEP();
            vrfItem* tsVerifyBlock1_p   = new vrfItem();
            tsVerifyBlock1_p->chkRslt_p = new int();
            setONE_STEP ( one_step_p, 1, I_F, 1, NULL, I_F, 0, NULL,
                          tsVerifyBlock1_p );
            //�ڶ���
            ONE_STEP* two_step_p        = new ONE_STEP();
            vrfItem* tsVerifyBlock2_p   = new vrfItem();
            tsVerifyBlock2_p->chkRslt_p = new int();
            setONE_STEP ( two_step_p, 2, I_F, 0, NULL, I_F, 1, NULL,
                          tsVerifyBlock2_p );
            two_step_p->nextStep_p = NULL;
            addNode ( one_step_p, two_step_p );

            Fun_p->steps_p[RD_DATA] = one_step_p;


            //2���ԻỰ����Ĭ��֡���� T_snnCfg* stnSnnCfg_p
            u_short cmnAddr = 1;
            BODY_ADDR_VAL bodyAddrVal = {};
            bodyAddrVal.shortAddr = 4;
            u_char defSetNum = 2;//Ĭ����ϢԪ�ظ���

            snnCfg_p->ssnID = srvNo;
            //snnCfg_p->actFlag
            snnCfg_p->infoType = GINFOTYPE;
            snnCfg_p->cmnAddr = cmnAddr;
            snnCfg_p->stepNum = stepNum;
            snnCfg_p->stepCfg_p = new T_ssnStepCfg[stepNum]();
            //�Ե�һ��
            //[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
            tmpStnAcsFlag.ctlType = 0;
            snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
            //��һ������֡
            u_char oneReqAPDUNum = 1;//��һ��������֡����
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
            //���ýӿڸ�snnCfg_p��һ������֡�ĵ�1֡��ֵ
            setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
            //��һ����Ӧ
            snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

            //�ڶ����������ݵ�Ĭ������
            //[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
            tmpStnAcsFlag.ctlType = 2;
            tmpStnAcsFlag.setValid = 1;
            snnCfg_p->stepCfg_p[1].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
            //�ڶ���������֡
            snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
            //�ڶ�������Ӧ֡
            u_char twoRespAPDUNum = 1;//�ڶ�������Ӧ֡����
            snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
            snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
            snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
            defSetNum;//�ڶ�����Ӧ��1֡Ԫ�ؼ�����
            //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
            snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
            * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_22;
            * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = 1 << 7 | MAX104_VSQ_N;
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_5;
            eleVal[BODY_SET_INDEX].lng = 1;
            * ( eleVal[BODY_SET_INDEX].p_Mem ) = 121;
            //���ýӿڸ�snnCfg_p�ڶ�����Ӧ�ĵ�1֡��ֵ
            setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

            //3������Ĭ�ϵĲ�����tid,cot�Ķ�Ӧ��ϵ
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

        case CYC_TRNS: //���ڴ���
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

            if ( usingMode != 0xFF ) //�����Ϊ׼
            {
                if ( usingMode != ONLY_ONE )
                {
                    freeRl_P ( Fun_p, NULL, NULL );
                    return NULL;
                }
            }
            else//����ѡһ�����Ե�ֵ
            {
                usingMode = ONLY_ONE;
            }

            ptclFunDsc->usingMode = usingMode;
            u_char stepNum = 1;

            //1���Բ�����Fun_p��ֵ
            Fun_p->srvNo             = CYC_TRNS;
            Fun_p->usingMode         = ONLY_ONE;
            Fun_p->cmLvl[ONLY_ONE]   = APP_LVL; //104��ԼȡAPP_LVL
            Fun_p->stepNum[ONLY_ONE] = stepNum;
            ONE_STEP* one_step_p        = new ONE_STEP();
            vrfItem* tsVerifyBlock1_p   = new vrfItem();
            tsVerifyBlock1_p->chkRslt_p = new int();
            setONE_STEP ( one_step_p, 1, I_F, 0, NULL, I_F, 1, NULL,
                          tsVerifyBlock1_p );

            Fun_p->steps_p[ONLY_ONE] = one_step_p;


            //2���ԻỰ����Ĭ��֡���� T_snnCfg* stnSnnCfg_p
            u_short cmnAddr = 1;
            BODY_ADDR_VAL bodyAddrVal = {};
            bodyAddrVal.shortAddr = 4;
            u_char defSetNum = 1;//Ĭ����ϢԪ�ظ���

            snnCfg_p->ssnID = srvNo;
            //snnCfg_p->actFlag
            snnCfg_p->infoType = GINFOTYPE;
            snnCfg_p->cmnAddr = cmnAddr;
            snnCfg_p->stepNum = stepNum;
            snnCfg_p->stepCfg_p = new T_ssnStepCfg[stepNum]();

            //��һ��
            //[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
            tmpStnAcsFlag.ctlType = 2;
            tmpStnAcsFlag.setValid = 1;
            snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
            //��һ��������֡
            u_char oneReqAPDUNum = 0;
            snnCfg_p->stepCfg_p[0].reqAPDUNum = oneReqAPDUNum;
            //��һ����Ӧ��֡
            u_char oneRespAPDUNum = 1;//��һ������Ӧ֡����
            snnCfg_p->stepCfg_p[0].respAPDUNum = oneRespAPDUNum;
            snnCfg_p->stepCfg_p[0].respBody_setNum_p = new u_char[oneRespAPDUNum]();
            snnCfg_p->stepCfg_p[0].respBody_setNum_p[0] =
            defSetNum;//��һ����Ӧ��1֡Ԫ�ؼ�����
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
            //���ýӿڸ�snnCfg_p��һ����Ӧ֡�ĵ�1֡��ֵ
            setOneFrmCfg ( PASSIVE_TYPE, 1, 1, eleVal, snnCfg_p );

            //3������Ĭ�ϵĲ�����tid,cot�Ķ�Ӧ��ϵ
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

        case ACQ_EVT: //�¼��ռ�
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

            if ( usingMode != 0xFF ) //�����Ϊ׼
            {
                if ( usingMode != ONLY_ONE )
                {
                    freeRl_P ( Fun_p, NULL, NULL );
                    return NULL;
                }
            }
            else//����ѡһ�����Ե�ֵ
            {
                usingMode = ONLY_ONE;
            }

            ptclFunDsc->usingMode = usingMode;

            u_char stepNum = 1;

            //1���Բ�����Fun_p��ֵ
            Fun_p->srvNo             = ACQ_EVT;
            Fun_p->usingMode         = ONLY_ONE;
            Fun_p->cmLvl[ONLY_ONE]   = APP_LVL; //104��ԼȡAPP_LVL
            Fun_p->stepNum[ONLY_ONE] = stepNum;
            ONE_STEP* one_step_p        = new ONE_STEP();
            vrfItem* tsVerifyBlock1_p   = new vrfItem();
            tsVerifyBlock1_p->chkRslt_p = new int();
            setONE_STEP ( one_step_p, 1, I_F, 0, NULL, I_F, 1, NULL,
                          tsVerifyBlock1_p );

            Fun_p->steps_p[ONLY_ONE] = one_step_p;

            //2���ԻỰ����Ĭ��֡���� T_snnCfg* stnSnnCfg_p
            u_short cmnAddr = 1;
            BODY_ADDR_VAL bodyAddrVal = {};
            bodyAddrVal.shortAddr = 4;
            u_char defSetNum = 1;//Ĭ����ϢԪ�ظ���

            snnCfg_p->ssnID = srvNo;
            //snnCfg_p->actFlag
            snnCfg_p->infoType = GINFOTYPE;
            snnCfg_p->cmnAddr = cmnAddr;
            snnCfg_p->stepNum = stepNum;
            snnCfg_p->stepCfg_p = new T_ssnStepCfg[stepNum]();

            //��һ��
            //[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
            tmpStnAcsFlag.ctlType = 2;
            tmpStnAcsFlag.setValid = 1;
            snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
            //��һ��������֡
            u_char oneReqAPDUNum = 0;
            snnCfg_p->stepCfg_p[0].reqAPDUNum = oneReqAPDUNum;
            //��һ����Ӧ��֡
            u_char oneRespAPDUNum = 1;//��һ������Ӧ֡����
            snnCfg_p->stepCfg_p[0].respAPDUNum = oneRespAPDUNum;
            snnCfg_p->stepCfg_p[0].respBody_setNum_p = new u_char[oneRespAPDUNum]();
            snnCfg_p->stepCfg_p[0].respBody_setNum_p[0] =
            defSetNum;//��һ����Ӧ��1֡Ԫ�ؼ�����
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
            //���ýӿڸ�snnCfg_p��һ����Ӧ֡�ĵ�1֡��ֵ
            setOneFrmCfg ( PASSIVE_TYPE, 1, 1, eleVal, snnCfg_p );

            //3������Ĭ�ϵĲ�����tid,cot�Ķ�Ӧ��ϵ
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

        case QUKCHK_EVT://����-����(quick-check)���̲ɼ��¼�
        {
            //��ʹ��
            freeRl_P ( Fun_p, snnCfg_p, ptclFunDsc );
            break;
        }

        case STN_INRGTN: //վ�ٻ�
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

            if ( usingMode != 0xFF ) //�����Ϊ׼
            {
                if ( usingMode != ONLY_ONE )
                {
                    freeRl_P ( Fun_p, NULL, NULL );
                    return NULL;
                }
            }
            else//����ѡһ�����Ե�ֵ
            {
                usingMode = ONLY_ONE;
            }

            ptclFunDsc->usingMode = usingMode;

            u_char stepNum = 4;
            u_char stepTwoRespFrmNum = 13;

            //1���Բ�����Fun_p��ֵ
            Fun_p->srvNo             = STN_INRGTN;
            Fun_p->usingMode         = ONLY_ONE;
            Fun_p->cmLvl[ONLY_ONE]   = APP_LVL;//104��ԼȡAPP_LVL
            Fun_p->stepNum[ONLY_ONE] = stepNum;//��ѡ�õ�����Э����������
            //��һ��
            ONE_STEP* one_step_p        = new ONE_STEP();
            vrfItem* tsVerifyBlock1_p   = new vrfItem();
            tsVerifyBlock1_p->chkRslt_p = new int();
            setONE_STEP ( one_step_p, 1, I_F, 1, NULL, I_F, 0, NULL,
                          tsVerifyBlock1_p );
            //�ڶ���
            ONE_STEP* one_step_resp_p        = new ONE_STEP();
            vrfItem* tsVerifyBlock11_p   = new vrfItem();
            tsVerifyBlock11_p->chkRslt_p = new int();
            setONE_STEP ( one_step_resp_p, 2, I_F, 0, NULL, I_F, 1, NULL,
                          tsVerifyBlock11_p );
            addNode ( one_step_p, one_step_resp_p );
            //������
            ONE_STEP* two_step_p        = new ONE_STEP();
            vrfItem* tsVerifyBlock2_p   = new vrfItem();
            tsVerifyBlock2_p->chkRslt_p = new int();
            setONE_STEP ( two_step_p, 3, I_F, 0, NULL, I_F,
                          stepTwoRespFrmNum, NULL, tsVerifyBlock2_p );
            two_step_p->nextStep_p = NULL;
            addNode ( one_step_p, two_step_p );

            if ( 1 == 1 ) //ͨ�������ж�
            {
                //���Ĳ�
                ONE_STEP* three_step_p      = new ONE_STEP();
                vrfItem* tsVerifyBlock3_p   = new vrfItem();
                tsVerifyBlock3_p->chkRslt_p = new int();
                setONE_STEP ( three_step_p, 4, I_F, 0, NULL, I_F, 1,
                              NULL, tsVerifyBlock3_p );
                three_step_p->nextStep_p = NULL;
                addNode ( one_step_p, three_step_p );
            }

            Fun_p->steps_p[ONLY_ONE] = one_step_p;

            //2���ԻỰ����Ĭ��֡���� T_snnCfg* stnSnnCfg_p
            //fusk test add
            /*struct T_zzfVal{
                u_int BSI;
                u_char QDS;
            } zzfVal = {};*/
            u_short cmnAddr = 2;
            BODY_ADDR_VAL bodyAddrVal = {};
            u_char defSetNum = 2;//Ĭ����ϢԪ�ظ���

            snnCfg_p->ssnID = srvNo;
            //snnCfg_p->actFlag
            snnCfg_p->infoType = GINFOTYPE;
            snnCfg_p->cmnAddr = cmnAddr;
            snnCfg_p->stepNum = stepNum;
            snnCfg_p->stepCfg_p = new T_ssnStepCfg[stepNum]();
            //��һ�����ٻ�����
            //[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
            tmpStnAcsFlag.ctlType = 0;
            snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
            //��һ������֡
            u_char oneReqAPDUNum = 1;//��һ��������֡����
            snnCfg_p->stepCfg_p[0].reqAPDUNum = oneReqAPDUNum;
            snnCfg_p->stepCfg_p[0].reqBody_setNum_p = new u_char[oneReqAPDUNum]();
            snnCfg_p->stepCfg_p[0].reqBody_setNum_p[0] = 1;//�����1֡Ԫ�ؼ�ΪQCC
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
            * ( eleVal[BODY_SET_INDEX].p_Mem ) = 20; //QCC վ�ٻ�
            //���ýӿڸ�snnCfg_p��һ������֡�ĵ�1֡��ֵ
            setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
            //��һ����Ӧ֡
            snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

            //�ڶ���ȷ��վ�ٻ�����
            //[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
            tmpStnAcsFlag.ctlType = 0;
            snnCfg_p->stepCfg_p[1].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
            //�ڶ�������֡
            snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
            //�ڶ�����Ӧ֡
            snnCfg_p->stepCfg_p[1].respAPDUNum = 1;
            snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[1 + 1]();
            snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
            1;//�ڶ�����Ӧ��1֡Ԫ�ؼ�����
            //_ASSERTE(_CrtCheckMemory());
            //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[1][11]();
            snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[1]();
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_7;
            //���ýӿڸ�snnCfg_p�ڶ�����Ӧ֡�ĵ�1֡��ֵ
            setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

            //���������ٻ�����Ϣ
            //[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
            tmpStnAcsFlag.ctlType = 2;
            tmpStnAcsFlag.setValid = 1;
            snnCfg_p->stepCfg_p[2].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
            //����������֡
            snnCfg_p->stepCfg_p[2].reqAPDUNum = 0;
            //��������Ӧ֡
            //u_char twoRespAPDUNum = stepTwoRespFrmNum;//�ڶ�������Ӧ֡����
            snnCfg_p->stepCfg_p[2].respAPDUNum = stepTwoRespFrmNum;
            snnCfg_p->stepCfg_p[2].respBody_setNum_p = new u_char[stepTwoRespFrmNum]();
            snnCfg_p->stepCfg_p[2].respBody_setNum_p[0] =
            defSetNum;//�ڶ�����Ӧ��1֡Ԫ�ؼ�����
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
            //���ýӿڸ�snnCfg_p��������Ӧ֡�ĵ�1֡��ֵ
            setOneFrmCfg ( PASSIVE_TYPE, 3, 1, eleVal, snnCfg_p );

            ////���ýӿڸ�snnCfg_p��������Ӧ֡�ĵ�2֡��ֵ
            bodyAddrVal.shortAddr = 6;
            snnCfg_p->stepCfg_p[2].respBody_setNum_p[1] =
            defSetNum;//��������Ӧ��2֡Ԫ�ؼ�����
            setOneFrmCfg ( PASSIVE_TYPE, 3, 2, eleVal, snnCfg_p );

            //���ýӿڸ�snnCfg_p��������Ӧ֡�ĵ�3֡��ֵ
            bodyAddrVal.shortAddr = 8;
            snnCfg_p->stepCfg_p[2].respBody_setNum_p[2] =
            defSetNum;//��������Ӧ��3֡Ԫ�ؼ�����
            setOneFrmCfg ( PASSIVE_TYPE, 3, 3, eleVal, snnCfg_p );

            //���ýӿڸ�snnCfg_p��������Ӧ֡�ĵ�4֡��ֵ
            bodyAddrVal.shortAddr = 10;
            snnCfg_p->stepCfg_p[2].respBody_setNum_p[3] =
            defSetNum;//��������Ӧ��4֡Ԫ�ؼ�����
            setOneFrmCfg ( PASSIVE_TYPE, 3, 4, eleVal, snnCfg_p );

            //���ýӿڸ�snnCfg_p��������Ӧ֡�ĵ�5֡��ֵ
            bodyAddrVal.shortAddr = 12;
            snnCfg_p->stepCfg_p[2].respBody_setNum_p[4] =
            defSetNum;//��������Ӧ��5֡Ԫ�ؼ�����
            setOneFrmCfg ( PASSIVE_TYPE, 3, 5, eleVal, snnCfg_p );

            //���ýӿڸ�snnCfg_p��������Ӧ֡�ĵ�6֡��ֵ
            bodyAddrVal.shortAddr = 14;
            snnCfg_p->stepCfg_p[2].respBody_setNum_p[5] =
            defSetNum;//��������Ӧ��6֡Ԫ�ؼ�����
            setOneFrmCfg ( PASSIVE_TYPE, 3, 6, eleVal, snnCfg_p );

            //���ýӿڸ�snnCfg_p��������Ӧ֡�ĵ�7֡��ֵ
            bodyAddrVal.shortAddr = 16;
            snnCfg_p->stepCfg_p[2].respBody_setNum_p[6] =
            defSetNum;//��������Ӧ��7֡Ԫ�ؼ�����
            setOneFrmCfg ( PASSIVE_TYPE, 3, 7, eleVal, snnCfg_p );

            //���ýӿڸ�snnCfg_p��������Ӧ֡�ĵ�8֡��ֵ
            bodyAddrVal.shortAddr = 18;
            snnCfg_p->stepCfg_p[2].respBody_setNum_p[7] =
            defSetNum;//��������Ӧ��8֡Ԫ�ؼ�����
            setOneFrmCfg ( PASSIVE_TYPE, 3, 8, eleVal, snnCfg_p );

            //���ýӿڸ�snnCfg_p��������Ӧ֡�ĵ�9֡��ֵ
            bodyAddrVal.shortAddr = 20;
            snnCfg_p->stepCfg_p[2].respBody_setNum_p[8] =
            defSetNum;//��������Ӧ��9֡Ԫ�ؼ�����
            setOneFrmCfg ( PASSIVE_TYPE, 3, 9, eleVal, snnCfg_p );

            //���ýӿڸ�snnCfg_p��������Ӧ֡�ĵ�10֡��ֵ
            bodyAddrVal.shortAddr = 22;
            snnCfg_p->stepCfg_p[2].respBody_setNum_p[9] =
            defSetNum;//��������Ӧ��10֡Ԫ�ؼ�����
            setOneFrmCfg ( PASSIVE_TYPE, 3, 10, eleVal, snnCfg_p );

            //���ýӿڸ�snnCfg_p��������Ӧ֡�ĵ�11֡��ֵ
            bodyAddrVal.shortAddr = 24;
            snnCfg_p->stepCfg_p[2].respBody_setNum_p[10] =
            defSetNum;//��������Ӧ��11֡Ԫ�ؼ�����
            setOneFrmCfg ( PASSIVE_TYPE, 3, 11, eleVal, snnCfg_p );

            //���ýӿڸ�snnCfg_p��������Ӧ֡�ĵ�12֡��ֵ
            bodyAddrVal.shortAddr = 26;
            snnCfg_p->stepCfg_p[2].respBody_setNum_p[11] =
            defSetNum;//��������Ӧ��12֡Ԫ�ؼ�����
            setOneFrmCfg ( PASSIVE_TYPE, 3, 12, eleVal, snnCfg_p );

            //���ýӿڸ�snnCfg_p��������Ӧ֡�ĵ�13֡��ֵ
            bodyAddrVal.shortAddr = 28;
            snnCfg_p->stepCfg_p[2].respBody_setNum_p[12] =
            1;//��������Ӧ��13֡Ԫ�ؼ�����
            setOneFrmCfg ( PASSIVE_TYPE, 3, 13, eleVal, snnCfg_p );

            //���Ĳ�վ�ٻ�����
            tmpStnAcsFlag.ctlType =
            0;//[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
            snnCfg_p->stepCfg_p[3].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
            //���Ĳ�����֡
            snnCfg_p->stepCfg_p[3].reqAPDUNum = 0;
            //���Ĳ���Ӧ֡
            snnCfg_p->stepCfg_p[3].respAPDUNum = 1;
            snnCfg_p->stepCfg_p[3].respBody_setNum_p = new u_char[1]();
            snnCfg_p->stepCfg_p[3].respBody_setNum_p[0] =
            1;//���Ĳ���Ӧ��1֡Ԫ�ؼ�����
            //snnCfg_p->stepCfg_p[3].respEleVal_pp = new ELE_TYPE[1][11]();
            snnCfg_p->stepCfg_p[3].respData_p = new dataFrame[1]();
            * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_100;
            * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_10;
            //���ýӿڸ�snnCfg_p���Ĳ���Ӧ֡�ĵ�1֡��ֵ
            setOneFrmCfg ( PASSIVE_TYPE, 4, 1, eleVal, snnCfg_p );

            //3������Ĭ�ϵĲ�����tid,cot�Ķ�Ӧ��ϵ
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

        case CLK_SYN://ʱ��ͬ��
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

            if ( usingMode != 0xFF ) //�����Ϊ׼
            {
                if ( usingMode != CRCT_REQ )
                {
                    freeRl_P ( Fun_p, NULL, NULL );
                    return NULL;
                }
            }
            else//����ѡһ�����Ե�ֵ
            {
                usingMode = CRCT_REQ;
            }

            ptclFunDsc->usingMode = usingMode;

            u_char stepNum = 1;

            //1���Բ�����Fun_p��ֵ
            Fun_p->srvNo             = CLK_SYN;
            Fun_p->usingMode         =
            CRCT_REQ;//��ѡ�õ�����Э������ CRCT_REQ CRCT_BC
            Fun_p->cmLvl[CRCT_REQ]   = APP_LVL; //104��ԼȡAPP_LVL
            Fun_p->cmLvl[CRCT_BC]    = LNK_LVL;
            Fun_p->stepNum[CRCT_REQ] = stepNum;
            Fun_p->stepNum[CRCT_BC]  = 1;
            //��һ��
            ONE_STEP* one_CRCT_REQ_p      = new ONE_STEP();
            vrfItem* tsVerifyCRCTREQ1_p   = new vrfItem();
            tsVerifyCRCTREQ1_p->chkRslt_p = new int();
            setONE_STEP ( one_CRCT_REQ_p, 1, I_F, 1, NULL, I_F, 0, NULL,
                          tsVerifyCRCTREQ1_p );
            ////�ڶ���
            //ONE_STEP* two_CRCT_REQ_p      = new ONE_STEP();
            //vrfItem* tsVerifyCRCTREQ2_p   = new vrfItem();
            //tsVerifyCRCTREQ2_p->chkRslt_p = new int();
            //setONE_STEP(two_CRCT_REQ_p , 2 , I_F , 0 , NULL , I_F , 1, NULL , tsVerifyCRCTREQ2_p);
            //addNode(one_CRCT_REQ_p , two_CRCT_REQ_p );

            Fun_p->steps_p[CRCT_REQ] = one_CRCT_REQ_p;

            //2���ԻỰ����Ĭ��֡���� T_snnCfg* stnSnnCfg_p
            u_short cmnAddr = 1;

            snnCfg_p->ssnID = srvNo;
            //snnCfg_p->actFlag
            snnCfg_p->infoType = GINFOTYPE;
            snnCfg_p->cmnAddr = cmnAddr;
            snnCfg_p->stepNum = stepNum;
            snnCfg_p->stepCfg_p = new T_ssnStepCfg[stepNum]();
            //��һ��ʱ��ͬ��
            tmpStnAcsFlag.ctlType =
            2;//[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
            tmpStnAcsFlag.tspValidB = 1;
            snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
            //��һ������֡
            u_char oneReqAPDUNum = 1;//��һ��������֡����
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
            //���ýӿڸ�snnCfg_p��һ������֡�ĵ�1֡��ֵ
            setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
            //��һ����Ӧ
            snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

            ////�ڶ�������ʱ��ƫ��
            //tmpStnAcsFlag.ctlType = 2;//[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
            //tmpStnAcsFlag.tspValidB = 1;
            //snnCfg_p->stepCfg_p[1].stnAcsFlag = *((u_short*)&tmpStnAcsFlag);
            ////�ڶ���������֡
            //snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
            ////�ڶ�������Ӧ֡
            //u_char twoRespAPDUNum = 1;//�ڶ�������Ӧ֡����
            //snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
            //snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
            //snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] = 0;//�ڶ�����Ӧ��1֡Ԫ�ؼ�����
            //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
            //snnCfg_p->stepCfg_p[1].respData = new dataFrame[twoRespAPDUNum]();
            //*(eleVal[ASDU_TID_INDEX].p_Mem) = MC_TID_103;
            //*(eleVal[ASDU_VSQ_INDEX].p_Mem) = MAX104_VSQ_N;
            //*(eleVal[ASDU_COT_INDEX].p_Mem) = MC_COT_7;
            //*(eleVal[BODY_ADDR_INDEX].p_Mem) = 0;
            //tmStmp.mScnd = 2;
            //eleVal[BODY_TSP_INDEX].p_Mem = (u_char*)&tmStmp;
            ////���ýӿڸ�snnCfg_p�ڶ�����Ӧ�ĵ�1֡��ֵ
            //setOneFrmCfg(PASSIVE_TYPE,2,1,eleVal,snnCfg_p);

            //3������Ĭ�ϵĲ�����tid,cot�Ķ�Ӧ��ϵ
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

        case CMD_TRNS://�����
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

            if ( usingMode != 0xFF ) //�����Ϊ׼
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
            else//����ѡһ�����Ե�ֵ
            {
                usingMode = CMD_DE;
            }

            ptclFunDsc->usingMode = usingMode;

            u_char seStepNum = 2;
            u_char deStepNum = 4;
            u_char cnclStepNum = 2;
            u_char defSetNum = 1;//Ĭ����ϢԪ�ظ���

            Fun_p->srvNo             = CMD_TRNS;
            Fun_p->usingMode         =
            usingMode;//������ģʽ����վ˫�����룬Э������֧�����ֶ�̬����
            Fun_p->cmLvl[CMD_SE]     = APP_LVL; //104��ԼȡAPP_LVL
            Fun_p->cmLvl[CMD_DE]     = APP_LVL; //104��ԼȡAPP_LVL
            Fun_p->cmLvl[CMD_CNCL]   = APP_LVL;
            Fun_p->stepNum[CMD_SE]   = seStepNum;//��ѡ�õ�����Э����������
            Fun_p->stepNum[CMD_DE]   = deStepNum;//��ѡ�õ�����Э����������
            Fun_p->stepNum[CMD_CNCL] = cnclStepNum;

            if ( usingMode == CMD_SE ) //��װCMD_SEģʽ
            {
                //1���Բ�����Fun_p��ֵ
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

                //2���ԻỰ����Ĭ��֡���� T_snnCfg* stnSnnCfg_p
                u_short cmnAddr = 1;
                BODY_ADDR_VAL bodyAddrVal = {};
                bodyAddrVal.shortAddr = 4;

                snnCfg_p->ssnID = srvNo;
                //snnCfg_p->actFlag
                snnCfg_p->infoType = GINFOTYPE;
                snnCfg_p->cmnAddr = cmnAddr;
                snnCfg_p->stepNum = seStepNum;
                snnCfg_p->stepCfg_p = new T_ssnStepCfg[seStepNum]();
                //�Ե�һ��
                tmpStnAcsFlag.ctlType =
                0;//[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
                snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //��һ������֡
                u_char oneReqAPDUNum = 1;//��һ��������֡����
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
                //���ýӿڸ�snnCfg_p��һ������֡�ĵ�1֡��ֵ
                setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
                //��һ����Ӧ
                snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

                //�ڶ���ѡ��ȷ��
                tmpStnAcsFlag.ctlType =
                0;//[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
                snnCfg_p->stepCfg_p[1].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //�ڶ���������֡
                snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
                //�ڶ�������Ӧ֡
                u_char twoRespAPDUNum = 1;//�ڶ�������Ӧ֡����
                snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
                snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
                snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
                defSetNum;//�ڶ�����Ӧ��1֡Ԫ�ؼ�����
                //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
                snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_45;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_7;
                //���ýӿڸ�snnCfg_p�ڶ�����Ӧ�ĵ�1֡��ֵ
                setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

                //3������Ĭ�ϵĲ�����tid,cot�Ķ�Ӧ��ϵ
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
            else if ( usingMode == CMD_DE ) //��װCMD_DEģʽ
            {
                //1���Բ�����Fun_p��ֵ
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

                //2���ԻỰ����Ĭ��֡���� T_snnCfg* stnSnnCfg_p
                u_short cmnAddr = 1;
                BODY_ADDR_VAL bodyAddrVal = {};
                bodyAddrVal.shortAddr = 4;

                snnCfg_p->ssnID = srvNo;
                //snnCfg_p->actFlag
                snnCfg_p->infoType = GINFOTYPE;
                snnCfg_p->cmnAddr = cmnAddr;
                snnCfg_p->stepNum = deStepNum;
                snnCfg_p->stepCfg_p = new T_ssnStepCfg[deStepNum]();
                //��һ��ִ������
                tmpStnAcsFlag.ctlType =
                0;//[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
                snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //��һ������֡
                u_char oneReqAPDUNum = 1;//��һ��������֡����
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
                //���ýӿڸ�snnCfg_p��һ������֡�ĵ�1֡��ֵ
                setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
                //��һ����Ӧ
                snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

                //�ڶ���ֱ������ȷ��
                tmpStnAcsFlag.ctlType =
                0;//[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
                snnCfg_p->stepCfg_p[1].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //�ڶ���������֡
                snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
                //�ڶ�������Ӧ֡
                u_char twoRespAPDUNum = 1;//�ڶ�������Ӧ֡����
                snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
                snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
                snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
                defSetNum;//�ڶ�����Ӧ��1֡Ԫ�ؼ�����
                //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
                snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_45;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_7;
                //���ýӿڸ�snnCfg_p�ڶ�����Ӧ�ĵ�1֡��ֵ
                setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

                //���������Ʋ������
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
                2;//[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
                tmpStnAcsFlag.setValid = 1;
                snnCfg_p->stepCfg_p[2].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //������������֡
                snnCfg_p->stepCfg_p[2].reqAPDUNum = 0;
                //����������Ӧ֡
                u_char threeRespAPDUNum = 1;//����������Ӧ֡����
                snnCfg_p->stepCfg_p[2].respAPDUNum = threeRespAPDUNum;
                snnCfg_p->stepCfg_p[2].respBody_setNum_p = new u_char[threeRespAPDUNum]();
                snnCfg_p->stepCfg_p[2].respBody_setNum_p[0] =
                defSetNum;//��������Ӧ��1֡Ԫ�ؼ�����
                //snnCfg_p->stepCfg_p[2].respEleVal_pp = new ELE_TYPE[threeRespAPDUNum][11]();
                snnCfg_p->stepCfg_p[2].respData_p = new dataFrame[threeRespAPDUNum]();
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_1;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_11;
                eleVal[BODY_SET_INDEX].p_Mem    = ( u_char* ) &siq;
                //���ýӿڸ�snnCfg_p���������Ʋ�����ɵĵ�1֡��ֵ
                setOneFrmCfg ( PASSIVE_TYPE, 3, 1, eleVal, snnCfg_p );

                //���Ĳ����Ʋ�������
                tmpStnAcsFlag.ctlType =
                0;//[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
                snnCfg_p->stepCfg_p[3].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //���Ĳ�������֡
                snnCfg_p->stepCfg_p[3].reqAPDUNum = 0;
                //���Ĳ�����Ӧ֡
                u_char fourRespAPDUNum = 1;//���Ĳ�����Ӧ֡����
                snnCfg_p->stepCfg_p[3].respAPDUNum = fourRespAPDUNum;
                snnCfg_p->stepCfg_p[3].respBody_setNum_p = new u_char[fourRespAPDUNum]();
                snnCfg_p->stepCfg_p[3].respBody_setNum_p[0] =
                defSetNum;//���Ĳ���Ӧ��1֡Ԫ�ؼ�����
                //snnCfg_p->stepCfg_p[3].respEleVal_pp = new ELE_TYPE[fourRespAPDUNum][11]();
                snnCfg_p->stepCfg_p[3].respData_p = new dataFrame[fourRespAPDUNum]();
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_45;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_10;
                eleVal[BODY_SET_INDEX].p_Mem    = ( u_char* ) &sco;
                //���ýӿڸ�snnCfg_p���Ĳ����Ʋ��������ĵ�1֡��ֵ
                setOneFrmCfg ( PASSIVE_TYPE, 4, 1, eleVal, snnCfg_p );

                //3������Ĭ�ϵĲ�����tid,cot�Ķ�Ӧ��ϵ
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
            else if ( usingMode == CMD_CNCL ) //��װCMD_CNCLģʽ
            {
                //1���Բ�����Fun_p��ֵ
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

                //2���ԻỰ����Ĭ��֡���� T_snnCfg* stnSnnCfg_p
                u_short cmnAddr = 1;
                BODY_ADDR_VAL bodyAddrVal = {};
                bodyAddrVal.shortAddr = 4;

                snnCfg_p->ssnID = srvNo;
                //snnCfg_p->actFlag
                snnCfg_p->infoType = GINFOTYPE;
                snnCfg_p->cmnAddr = cmnAddr;
                snnCfg_p->stepNum = cnclStepNum;
                snnCfg_p->stepCfg_p = new T_ssnStepCfg[cnclStepNum]();
                //��һ����������
                tmpStnAcsFlag.ctlType =
                0;//[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
                snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //��һ������֡
                u_char oneReqAPDUNum = 1;//��һ��������֡����
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
                //���ýӿڸ�snnCfg_p��һ������֡�ĵ�1֡��ֵ
                setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
                //��һ����Ӧ
                snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

                //�ڶ�������ȷ��
                tmpStnAcsFlag.ctlType =
                0;//[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
                snnCfg_p->stepCfg_p[1].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //�ڶ���������֡
                snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
                //�ڶ�������Ӧ֡
                u_char twoRespAPDUNum = 1;//�ڶ�������Ӧ֡����
                snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
                snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
                snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
                defSetNum;//�ڶ�����Ӧ��1֡Ԫ�ؼ�����
                //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
                snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_45;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_9;
                //���ýӿڸ�snnCfg_p�ڶ�����Ӧ�ĵ�1֡��ֵ
                setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

                //3������Ĭ�ϵĲ�����tid,cot�Ķ�Ӧ��ϵ
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

        case ACMU_FRZ: //�ۻ�������ǰ�Ķ��� �˹����Ǵ�ACMU_TRNS���ܵ�ģʽ2������վ�ȶ�������������������
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

            if ( usingMode != 0xFF ) //�����Ϊ׼
            {
                if ( usingMode != ONLY_ONE )
                {
                    freeRl_P ( Fun_p, NULL, NULL );
                    return NULL;
                }
            }
            else//����ѡһ�����Ե�ֵ
            {
                usingMode = ONLY_ONE;
            }

            ptclFunDsc->usingMode = usingMode;

            u_char stepNum = 2;

            //1���Բ�����Fun_p��ֵ
            Fun_p->srvNo             = ACMU_FRZ;
            Fun_p->usingMode         = ONLY_ONE;
            Fun_p->cmLvl[ONLY_ONE]   = APP_LVL; //104��ԼȡAPP_LVL
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

            //2���ԻỰ����Ĭ��֡���� T_snnCfg* stnSnnCfg_p
            u_short cmnAddr = 1;
            BODY_ADDR_VAL bodyAddrVal = {};
            bodyAddrVal.shortAddr = 0;
            u_char defSetNum = 1;//Ĭ����ϢԪ�ظ���

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
            //��һ���洢����������
            tmpStnAcsFlag.ctlType =
            0;//[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
            snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
            //��һ������֡
            u_char oneReqAPDUNum = 1;//��һ��������֡����
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
            //���ýӿڸ�snnCfg_p��һ������֡�ĵ�1֡��ֵ
            setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
            //��һ����Ӧ
            snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

            //�ڶ�������ȷ��
            tmpStnAcsFlag.ctlType =
            0;//[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
            snnCfg_p->stepCfg_p[1].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
            //�ڶ���������֡
            snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
            //�ڶ�������Ӧ֡
            u_char twoRespAPDUNum = 1;//�ڶ�������Ӧ֡����
            snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
            snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
            snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
            defSetNum;//�ڶ�����Ӧ��1֡Ԫ�ؼ�����
            //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
            snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
            * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_101;
            * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_7;
            //���ýӿڸ�snnCfg_p�ڶ�����Ӧ�ĵ�1֡��ֵ
            setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

            //3������Ĭ�ϵĲ�����tid,cot�Ķ�Ӧ��ϵ
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

        case ACMU_TRNS://�ۻ�������
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

            if ( usingMode != 0xFF ) //�����Ϊ׼
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
            else//����ѡһ�����Ե�ֵ
            {
                usingMode = CTL_DRCTREQ_TRNS;
            }

            ptclFunDsc->usingMode = usingMode;

            u_char sonptStepNum = 1;
            u_char drReqStepNum = 4;
            u_char ctlFrzSonptNum = 3;
            //1���Բ�����Fun_p��ֵ
            Fun_p->srvNo                            = ACMU_TRNS;
            Fun_p->usingMode                        = UNKNOWN;
            Fun_p->cmLvl[CTLD_DRCTSONPT_TRNS]       =
            APP_LVL;// 101��ԼȡAPP_LVL��104��Լ����������²�֧���ۼ�����ͻ������
            Fun_p->cmLvl[CTL_DRCTREQ_TRNS]          = APP_LVL;
            Fun_p->cmLvl[CTL_FRZREQ_TRNS]           = APP_LVL; // 104��ԼȡAPP_LVL
            Fun_p->cmLvl[CTL_FRZ_CTLD_SONPT_TRNS]   = APP_LVL;
            Fun_p->stepNum[CTLD_DRCTSONPT_TRNS]     = sonptStepNum;
            Fun_p->stepNum[CTL_DRCTREQ_TRNS]        = drReqStepNum;
            Fun_p->stepNum[CTL_FRZREQ_TRNS]         =
            0;//�˸�ģʽ�������ˣ������ģʽ=�µĻỰACMU_FRZ + CTL_DRCTREQ_TRNS ģʽ�����
            Fun_p->stepNum[CTL_FRZ_CTLD_SONPT_TRNS] = ctlFrzSonptNum;

            u_char defSetNum = 1;//Ĭ����ϢԪ�ظ���

            if ( usingMode == CTLD_DRCTSONPT_TRNS )
            {
                //��װCTLD_DRCTSONPT_TRNSģʽ
                ONE_STEP* one_DRCTSONPT_p       = new ONE_STEP();
                vrfItem* tsVerifyDRCTSONPT1_p   = new vrfItem();
                tsVerifyDRCTSONPT1_p->chkRslt_p = new int();
                setONE_STEP ( one_DRCTSONPT_p, 1, I_F, 0, NULL, I_F, 1, NULL,
                              tsVerifyDRCTSONPT1_p );
                Fun_p->steps_p[CTLD_DRCTSONPT_TRNS] = one_DRCTSONPT_p;

                //2���ԻỰ����Ĭ��֡���� T_snnCfg* stnSnnCfg_p
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
                //�Ե�һ��ͻ������
                tmpStnAcsFlag.ctlType =
                2;//[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
                tmpStnAcsFlag.setValid = 1;
                snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //��һ��������֡
                snnCfg_p->stepCfg_p[0].reqAPDUNum = 0;
                //��һ������Ӧ֡
                u_char oneRespAPDUNum = 1;//��һ������Ӧ֡����
                snnCfg_p->stepCfg_p[0].respAPDUNum = oneRespAPDUNum;
                snnCfg_p->stepCfg_p[0].respBody_setNum_p = new u_char[oneRespAPDUNum]();
                snnCfg_p->stepCfg_p[0].respBody_setNum_p[0] =
                defSetNum;//��һ����Ӧ��1֡Ԫ�ؼ�����
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
                //���ýӿڸ�snnCfg_p��һ��ͻ���ĵ�1֡��ֵ
                setOneFrmCfg ( PASSIVE_TYPE, 1, 1, eleVal, snnCfg_p );

                //3������Ĭ�ϵĲ�����tid,cot�Ķ�Ӧ��ϵ
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
                //1���Բ�����Fun_p��ֵ
                //��װCTL_DRCTREQ_TRNSģʽ
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

                //2���ԻỰ����Ĭ��֡���� T_snnCfg* stnSnnCfg_p
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
                //�Ե�һ���������������
                tmpStnAcsFlag.ctlType =
                0;//[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
                snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //��һ������֡
                u_char oneReqAPDUNum = 1;//��һ��������֡����
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
                //���ýӿڸ�snnCfg_p��һ������֡�ĵ�1֡��ֵ
                setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
                //��һ����Ӧ
                snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

                //�ڶ����������������ȷ��
                tmpStnAcsFlag.ctlType =
                0;//[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
                snnCfg_p->stepCfg_p[1].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //�ڶ���������֡
                snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
                //�ڶ�������Ӧ֡
                u_char twoRespAPDUNum = 1;//�ڶ�������Ӧ֡����
                snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
                snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
                snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
                defSetNum;//�ڶ�����Ӧ��1֡Ԫ�ؼ�����
                //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
                snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_101;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_7;
                //���ýӿڸ�snnCfg_p�ڶ�����Ӧ�ĵ�1֡��ֵ
                setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

                //�������ɼ��ļ�����
                tmpStnAcsFlag.ctlType =
                2;//[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
                tmpStnAcsFlag.setValid = 1;
                snnCfg_p->stepCfg_p[2].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //������������֡
                snnCfg_p->stepCfg_p[2].reqAPDUNum = 0;
                //����������Ӧ֡
                u_char oneRespAPDUNum = 1;//����������Ӧ֡����
                snnCfg_p->stepCfg_p[2].respAPDUNum = oneRespAPDUNum;
                snnCfg_p->stepCfg_p[2].respBody_setNum_p = new u_char[oneRespAPDUNum]();
                snnCfg_p->stepCfg_p[2].respBody_setNum_p[0] =
                1;//��������Ӧ��1֡Ԫ�ؼ�����
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
                //���ýӿڸ�snnCfg_p������ͻ���ĵ�1֡��ֵ
                setOneFrmCfg ( PASSIVE_TYPE, 3, 1, eleVal, snnCfg_p );

                //���Ĳ��������������
                tmpStnAcsFlag.ctlType =
                0;//[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
                snnCfg_p->stepCfg_p[3].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //���Ĳ�������֡
                snnCfg_p->stepCfg_p[3].reqAPDUNum = 0;
                //���Ĳ�����Ӧ֡
                u_char fourRespAPDUNum = 1;//���Ĳ�����Ӧ֡����
                snnCfg_p->stepCfg_p[3].respAPDUNum = fourRespAPDUNum;
                snnCfg_p->stepCfg_p[3].respBody_setNum_p = new u_char[fourRespAPDUNum]();
                snnCfg_p->stepCfg_p[3].respBody_setNum_p[0] =
                defSetNum;//���Ĳ���Ӧ��1֡Ԫ�ؼ�����
                //snnCfg_p->stepCfg_p[3].respEleVal_pp = new ELE_TYPE[fourRespAPDUNum][11]();
                snnCfg_p->stepCfg_p[3].respData_p = new dataFrame[fourRespAPDUNum]();
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_101;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_10;
                eleVal[BODY_ADDR_INDEX].lng = 1;
                * ( eleVal[BODY_ADDR_INDEX].p_Mem ) = 0;
                eleVal[BODY_SET_INDEX].lng = sizeof ( qcc );
                eleVal[BODY_SET_INDEX].p_Mem = ( u_char* ) &qcc;
                //���ýӿڸ�snnCfg_p���Ĳ���Ӧ�ĵ�1֡��ֵ
                setOneFrmCfg ( PASSIVE_TYPE, 4, 1, eleVal, snnCfg_p );

                //3������Ĭ�ϵĲ�����tid,cot�Ķ�Ӧ��ϵ
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
                //��װCTL_FRZ_CTLD_SONPT_TRNSģʽ
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

                //2���ԻỰ����Ĭ��֡���� T_snnCfg* stnSnnCfg_p
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
                //�Ե�һ���洢����������
                tmpStnAcsFlag.ctlType =
                0;//[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
                snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //��һ������֡
                u_char oneReqAPDUNum = 1;//��һ��������֡����
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
                //���ýӿڸ�snnCfg_p��һ������֡�ĵ�1֡��ֵ
                setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
                //��һ����Ӧ
                snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

                //�ڶ������������洢ȷ��
                tmpStnAcsFlag.ctlType =
                0;//[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
                snnCfg_p->stepCfg_p[1].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //�ڶ���������֡
                snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
                //�ڶ�������Ӧ֡
                u_char twoRespAPDUNum = 1;//�ڶ�������Ӧ֡����
                snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
                snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
                snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
                defSetNum;//�ڶ�����Ӧ��1֡Ԫ�ؼ�����
                //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
                snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_101;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_7;
                //���ýӿڸ�snnCfg_p�ڶ�����Ӧ�ĵ�1֡��ֵ
                setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

                //������������ͻ��
                tmpStnAcsFlag.ctlType =
                2;//[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
                tmpStnAcsFlag.setValid = 1;
                snnCfg_p->stepCfg_p[2].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //������������֡
                snnCfg_p->stepCfg_p[2].reqAPDUNum = 0;
                //����������Ӧ֡
                u_char oneRespAPDUNum = 1;//����������Ӧ֡����
                snnCfg_p->stepCfg_p[2].respAPDUNum = oneRespAPDUNum;
                snnCfg_p->stepCfg_p[2].respBody_setNum_p = new u_char[oneRespAPDUNum]();
                snnCfg_p->stepCfg_p[2].respBody_setNum_p[0] =
                defSetNum;//��������Ӧ��1֡Ԫ�ؼ�����
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
                //���ýӿڸ�snnCfg_p������ͻ���ĵ�1֡��ֵ
                setOneFrmCfg ( PASSIVE_TYPE, 3, 1, eleVal, snnCfg_p );

                //3������Ĭ�ϵĲ�����tid,cot�Ķ�Ӧ��ϵ
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

        case PARA_LD://��������
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

            if ( usingMode != 0xFF ) //�����Ϊ׼
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
            else//����ѡһ�����Ե�ֵ
            {
                usingMode = AUTOACT_LD;
            }

            ptclFunDsc->usingMode = usingMode;

            u_char onlyStepNum = 2;
            u_char onlyActStepNum = 2;
            u_char autoActStepNum = 2;

            Fun_p->srvNo               = PARA_LD;
            Fun_p->usingMode           = AUTO;
            Fun_p->cmLvl[ONLY_LD]      = APP_LVL; //104��ԼȡAPP_LVL
            Fun_p->cmLvl[ONLYACT_LD]   = APP_LVL; //104��ԼȡAPP_LVL
            Fun_p->cmLvl[AUTOACT_LD]   = APP_LVL; //104��ԼȡAPP_LVL
            Fun_p->stepNum[ONLY_LD]    = onlyStepNum;
            Fun_p->stepNum[ONLYACT_LD] = onlyActStepNum;
            Fun_p->stepNum[AUTOACT_LD] = autoActStepNum;


            u_short cmnAddr = 1;
            BODY_ADDR_VAL bodyAddrVal = {};
            bodyAddrVal.shortAddr = 4;
            u_char defSetNum = 1;//Ĭ����ϢԪ�ظ���

            if ( usingMode == ONLY_LD )
            {
                //1���Բ�����Fun_p��ֵ
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

                //2���ԻỰ����Ĭ��֡���� T_snnCfg* stnSnnCfg_p
                snnCfg_p->ssnID = srvNo;
                //snnCfg_p->actFlag
                snnCfg_p->infoType = GINFOTYPE;
                snnCfg_p->cmnAddr = cmnAddr;
                snnCfg_p->stepNum = onlyStepNum;
                snnCfg_p->stepCfg_p = new T_ssnStepCfg[onlyStepNum]();
                //�Ե�һ������װ������
                tmpStnAcsFlag.ctlType =
                2;//[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
                tmpStnAcsFlag.setValid = 1;
                snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //��һ������֡
                u_char oneReqAPDUNum = 1;//��һ��������֡����
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
                MC_TID_110; //����ֵ������һ��ֵ��MC_TID_111��Ȼ�ֵ MC_TID_112������
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_6;
                //���ýӿڸ�snnCfg_p��һ������֡�ĵ�1֡��ֵ
                setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
                //��һ����Ӧ
                snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

                //�ڶ�������װ������ȷ��
                tmpStnAcsFlag.ctlType =
                2;//[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
                tmpStnAcsFlag.setValid = 1;
                snnCfg_p->stepCfg_p[1].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //�ڶ���������֡
                snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
                //�ڶ�������Ӧ֡
                u_char twoRespAPDUNum = 1;//�ڶ�������Ӧ֡����
                snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
                snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
                snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
                defSetNum;//�ڶ�����Ӧ��1֡Ԫ�ؼ�����
                //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
                snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_110;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_7;
                //���ýӿڸ�snnCfg_p�ڶ�����Ӧ�ĵ�1֡��ֵ
                setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

                //3������Ĭ�ϵĲ�����tid,cot�Ķ�Ӧ��ϵ
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
                //1���Բ�����Fun_p��ֵ
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

                //2���ԻỰ����Ĭ��֡���� T_snnCfg* stnSnnCfg_p
                snnCfg_p->ssnID = srvNo;
                //snnCfg_p->actFlag
                snnCfg_p->infoType = GINFOTYPE;
                snnCfg_p->cmnAddr = cmnAddr;
                snnCfg_p->stepNum = onlyActStepNum;
                snnCfg_p->stepCfg_p = new T_ssnStepCfg[onlyActStepNum]();
                //��һ��������������
                tmpStnAcsFlag.ctlType =
                0;//[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
                snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //��һ������֡
                u_char oneReqAPDUNum = 1;//��һ��������֡����
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
                * ( eleVal[BODY_SET_INDEX].p_Mem ) = 3; //��ģʽ��QPA��������3
                //���ýӿڸ�snnCfg_p��һ������֡�ĵ�1֡��ֵ
                setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
                //��һ����Ӧ
                snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

                //�ڶ��������Ѽ���ȷ��
                tmpStnAcsFlag.ctlType =
                0;//[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
                snnCfg_p->stepCfg_p[1].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //�ڶ���������֡
                snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
                //�ڶ�������Ӧ֡
                u_char twoRespAPDUNum = 1;//�ڶ�������Ӧ֡����
                snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
                snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
                snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
                defSetNum;//�ڶ�����Ӧ��1֡Ԫ�ؼ�����
                //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
                snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_113;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_7;
                * ( eleVal[BODY_SET_INDEX].p_Mem ) = 3; //��ģʽ��QPA��������3
                //���ýӿڸ�snnCfg_p�ڶ�����Ӧ�ĵ�1֡��ֵ
                setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

                //3������Ĭ�ϵĲ�����tid,cot�Ķ�Ӧ��ϵ
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
                //1���Բ�����Fun_p��ֵ
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


                //2���ԻỰ����Ĭ��֡���� T_snnCfg* stnSnnCfg_p
                snnCfg_p->ssnID = srvNo;
                //snnCfg_p->actFlag
                snnCfg_p->infoType = GINFOTYPE;
                snnCfg_p->cmnAddr = cmnAddr;
                snnCfg_p->stepNum = autoActStepNum;
                snnCfg_p->stepCfg_p = new T_ssnStepCfg[autoActStepNum]();
                //��һ������װ�ز���������
                tmpStnAcsFlag.ctlType =
                2;//[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
                tmpStnAcsFlag.setValid = 1;
                snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //��һ������֡
                u_char oneReqAPDUNum = 1;//��һ��������֡����
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
                MC_TID_110; //����ֵ������һ��ֵ��MC_TID_111��Ȼ�ֵ MC_TID_112������
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_6;
                //���ýӿڸ�snnCfg_p��һ������֡�ĵ�1֡��ֵ
                setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
                //��һ����Ӧ
                snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

                //�ڶ�������װ�ز���������ȷ��
                tmpStnAcsFlag.ctlType =
                2;//[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
                tmpStnAcsFlag.setValid = 1;
                snnCfg_p->stepCfg_p[1].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
                //�ڶ���������֡
                snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
                //�ڶ�������Ӧ֡
                u_char twoRespAPDUNum = 1;//�ڶ�������Ӧ֡����
                snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
                snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
                snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
                defSetNum;//�ڶ�����Ӧ��1֡Ԫ�ؼ�����
                //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
                snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
                * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_110;
                * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
                * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_7;
                //���ýӿڸ�snnCfg_p�ڶ�����Ӧ�ĵ�1֡��ֵ
                setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

                //3������Ĭ�ϵĲ�����tid,cot�Ķ�Ӧ��ϵ
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

        case TST_PRCD://���Թ���
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

            if ( usingMode != 0xFF ) //�����Ϊ׼
            {
                if ( usingMode != ONLY_LD )
                {
                    freeRl_P ( Fun_p, NULL, NULL );
                    return NULL;
                }
            }
            else//����ѡһ�����Ե�ֵ
            {
                usingMode = ONLY_LD;
            }

            ptclFunDsc->usingMode = usingMode;

            u_char stepNum = 2;

            //1���Բ�����Fun_p��ֵ
            Fun_p->srvNo             = TST_PRCD;
            Fun_p->usingMode         = ONLY_ONE;
            Fun_p->cmLvl[ONLY_ONE]   = APP_LVL;//104��ԼȡAPP_LVL
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

            //2���ԻỰ����Ĭ��֡���� T_snnCfg* stnSnnCfg_p
            u_short cmnAddr = 11;
            u_short bodySet = 0x55AA;//FBP=�̶�������
            u_char defSetNum = 1;//Ĭ����ϢԪ�ظ���

            snnCfg_p->ssnID = srvNo;
            //snnCfg_p->actFlag
            snnCfg_p->infoType = GINFOTYPE;
            snnCfg_p->cmnAddr = cmnAddr;
            snnCfg_p->stepNum = stepNum;
            snnCfg_p->stepCfg_p = new T_ssnStepCfg[stepNum]();
            //�Ե�һ��
            tmpStnAcsFlag.ctlType =
            2;//[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
            tmpStnAcsFlag.setValid = 1;
            tmpStnAcsFlag.tspValidB = 1;
            snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
            //��һ������֡
            u_char oneReqAPDUNum = 1;//��һ��������֡����
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
            //���ýӿڸ�snnCfg_p��һ������֡�ĵ�1֡��ֵ
            setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
            //��һ����Ӧ
            snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

            //�ڶ����������ݵ�Ĭ������
            tmpStnAcsFlag.ctlType =
            2;//[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻λ2 = 1�洢��ϵͳִ������ֵ�ļ���
            tmpStnAcsFlag.setValid = 1;
            tmpStnAcsFlag.tspValidB = 1;
            snnCfg_p->stepCfg_p[1].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
            //�ڶ���������֡
            snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
            //�ڶ�������Ӧ֡
            u_char twoRespAPDUNum = 1;//�ڶ�������Ӧ֡����
            snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
            snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
            snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
            defSetNum;//�ڶ�����Ӧ��1֡Ԫ�ؼ�����
            //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
            snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
            * ( eleVal[ASDU_TID_INDEX].p_Mem ) = MC_TID_107;
            * ( eleVal[ASDU_VSQ_INDEX].p_Mem ) = MAX104_VSQ_N;
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_7;
            * ( eleVal[BODY_ADDR_INDEX].p_Mem ) = 0;
            //���ýӿڸ�snnCfg_p�ڶ�����Ӧ�ĵ�1֡��ֵ
            setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

            //3������Ĭ�ϵĲ�����tid,cot�Ķ�Ӧ��ϵ
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

        case FILE_TRNS://�ļ�����
        {
            //�ݲ�ʵ��
            break;
        }

        case ACQ_TRNSDLY://�ռ������ӳ�ֵ
        {
            //��ʹ��
            freeRl_P ( Fun_p, snnCfg_p, ptclFunDsc );
            break;
        }

        case U_STARTDT://104Э��U֡STARTDT ����ĻỰ
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

            if ( usingMode != 0xFF ) //�����Ϊ׼
            {
                if ( usingMode != ONLY_LD )
                {
                    freeRl_P ( Fun_p, NULL, NULL );
                    return NULL;
                }
            }
            else//����ѡһ�����Ե�ֵ
            {
                usingMode = ONLY_LD;
            }

            ptclFunDsc->usingMode = usingMode;

            u_char stepNum = 2;

            //1���Բ�����Fun_p��ֵ
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

            //2���ԻỰ����Ĭ��֡���� T_snnCfg* stnSnnCfg_p
            u_short cmnAddr = 0;
            snnCfg_p->ssnID = srvNo;
            //snnCfg_p->actFlag
            snnCfg_p->infoType = GINFOTYPE;
            snnCfg_p->cmnAddr = cmnAddr;
            snnCfg_p->stepNum = stepNum;
            snnCfg_p->stepCfg_p = new T_ssnStepCfg[stepNum]();
            //��һ������
            snnCfg_p->stepCfg_p[0].stnAcsFlag = 0;
            //��һ������֡
            u_char oneReqAPDUNum = 1;//��һ��������֡����
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
            //���ýӿڸ�snnCfg_p��һ������֡�ĵ�1֡��ֵ
            setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
            //��һ����Ӧ
            snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

            //�ڶ�������ȷ��
            snnCfg_p->stepCfg_p[1].stnAcsFlag = 0;
            //�ڶ���������֡
            snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
            //�ڶ�������Ӧ֡
            u_char twoRespAPDUNum = 1;//�ڶ�������Ӧ֡����
            snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
            snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
            snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
            0;//�ڶ�����Ӧ��1֡Ԫ�ؼ�����
            //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
            snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_7;
            //���ýӿڸ�snnCfg_p�ڶ�����Ӧ�ĵ�1֡��ֵ
            setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

            //3������Ĭ�ϵĲ�����tid,cot�Ķ�Ӧ��ϵ
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

        case U_STOPDT://104Э��U֡STOPDT ����ĻỰ
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

            if ( usingMode != 0xFF ) //�����Ϊ׼
            {
                if ( usingMode != ONLY_LD )
                {
                    freeRl_P ( Fun_p, NULL, NULL );
                    return NULL;
                }
            }
            else//����ѡһ�����Ե�ֵ
            {
                usingMode = ONLY_LD;
            }

            ptclFunDsc->usingMode = usingMode;

            u_char stepNum = 2;

            //1���Բ�����Fun_p��ֵ
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

            //2���ԻỰ����Ĭ��֡���� T_snnCfg* stnSnnCfg_p
            u_short cmnAddr = 0;
            snnCfg_p->ssnID = srvNo;
            //snnCfg_p->actFlag
            snnCfg_p->infoType = GINFOTYPE;
            snnCfg_p->cmnAddr = cmnAddr;
            snnCfg_p->stepNum = stepNum;
            snnCfg_p->stepCfg_p = new T_ssnStepCfg[stepNum]();
            //��һ������
            snnCfg_p->stepCfg_p[0].stnAcsFlag = 0;
            //��һ������֡
            u_char oneReqAPDUNum = 1;//��һ��������֡����
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
            //���ýӿڸ�snnCfg_p��һ������֡�ĵ�1֡��ֵ
            setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
            //��һ����Ӧ
            snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

            //�ڶ�������ȷ��
            snnCfg_p->stepCfg_p[1].stnAcsFlag = 0;
            //�ڶ���������֡
            snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
            //�ڶ�������Ӧ֡
            u_char twoRespAPDUNum = 1;//�ڶ�������Ӧ֡����
            snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
            snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
            snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
            0;//�ڶ�����Ӧ��1֡Ԫ�ؼ�����
            //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
            snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_7;
            //���ýӿڸ�snnCfg_p�ڶ�����Ӧ�ĵ�1֡��ֵ
            setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

            //3������Ĭ�ϵĲ�����tid,cot�Ķ�Ӧ��ϵ
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

        case U_TESTFR://104Э��U֡TESTFR ����ĻỰ
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

            if ( usingMode != 0xFF ) //�����Ϊ׼
            {
                if ( usingMode != ONLY_LD )
                {
                    freeRl_P ( Fun_p, NULL, NULL );
                    return NULL;
                }
            }
            else//����ѡһ�����Ե�ֵ
            {
                usingMode = ONLY_LD;
            }

            ptclFunDsc->usingMode = usingMode;

            u_char stepNum = 2;

            //1���Բ�����Fun_p��ֵ
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

            //2���ԻỰ����Ĭ��֡���� T_snnCfg* stnSnnCfg_p
            u_short cmnAddr = 0;
            snnCfg_p->ssnID = srvNo;
            //snnCfg_p->actFlag
            snnCfg_p->infoType = GINFOTYPE;
            snnCfg_p->cmnAddr = cmnAddr;
            snnCfg_p->stepNum = stepNum;
            snnCfg_p->stepCfg_p = new T_ssnStepCfg[stepNum]();
            //��һ������
            snnCfg_p->stepCfg_p[0].stnAcsFlag = 0;
            //��һ������֡
            u_char oneReqAPDUNum = 1;//��һ��������֡����
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
            //���ýӿڸ�snnCfg_p��һ������֡�ĵ�1֡��ֵ
            setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
            //��һ����Ӧ
            snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

            //�ڶ�������ȷ��
            snnCfg_p->stepCfg_p[1].stnAcsFlag = 0;
            //�ڶ���������֡
            snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
            //�ڶ�������Ӧ֡
            u_char twoRespAPDUNum = 1;//�ڶ�������Ӧ֡����
            snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
            snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
            snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
            0;//�ڶ�����Ӧ��1֡Ԫ�ؼ�����
            //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
            snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_7;
            //���ýӿڸ�snnCfg_p�ڶ�����Ӧ�ĵ�1֡��ֵ
            setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

            //3������Ĭ�ϵĲ�����tid,cot�Ķ�Ӧ��ϵ
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

        case U_NEWHOLD://��U֡��STARTDT STOPDT TESTFR ȫΪ0 ��ʾ���µĻỰ������һ��ͨ��
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

            if ( usingMode != 0xFF ) //�����Ϊ׼
            {
                if ( usingMode != ONLY_LD )
                {
                    freeRl_P ( Fun_p, NULL, NULL );
                    return NULL;
                }
            }
            else//����ѡһ�����Ե�ֵ
            {
                usingMode = ONLY_LD;
            }

            ptclFunDsc->usingMode = usingMode;

            u_char stepNum = 2;

            //1���Բ�����Fun_p��ֵ
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

            //2���ԻỰ����Ĭ��֡���� T_snnCfg* stnSnnCfg_p
            u_short cmnAddr = 0;
            snnCfg_p->ssnID = srvNo;
            //snnCfg_p->actFlag
            snnCfg_p->infoType = GINFOTYPE;
            snnCfg_p->cmnAddr = cmnAddr;
            snnCfg_p->stepNum = stepNum;
            snnCfg_p->stepCfg_p = new T_ssnStepCfg[stepNum]();
            //��һ������
            snnCfg_p->stepCfg_p[0].stnAcsFlag = 0;
            //��һ������֡
            u_char oneReqAPDUNum = 1;//��һ��������֡����
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
            //���ýӿڸ�snnCfg_p��һ������֡�ĵ�1֡��ֵ
            setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
            //��һ����Ӧ
            snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

            //�ڶ�������ȷ��
            snnCfg_p->stepCfg_p[1].stnAcsFlag = 0;
            //�ڶ���������֡
            snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
            //�ڶ�������Ӧ֡
            u_char twoRespAPDUNum = 1;//�ڶ�������Ӧ֡����
            snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
            snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
            snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
            0;//�ڶ�����Ӧ��1֡Ԫ�ؼ�����
            //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
            snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_7;
            //���ýӿڸ�snnCfg_p�ڶ�����Ӧ�ĵ�1֡��ֵ
            setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

            //3������Ĭ�ϵĲ�����tid,cot�Ķ�Ӧ��ϵ
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

        case U_DELHOLD://��U֡��STARTDT STOPDT TESTFR ȫΪ1 ��ʾ���µĻỰ����һ��ͨ��
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

            if ( usingMode != 0xFF ) //�����Ϊ׼
            {
                if ( usingMode != ONLY_LD )
                {
                    freeRl_P ( Fun_p, NULL, NULL );
                    return NULL;
                }
            }
            else//����ѡһ�����Ե�ֵ
            {
                usingMode = ONLY_LD;
            }

            ptclFunDsc->usingMode = usingMode;

            u_char stepNum = 2;

            //1���Բ�����Fun_p��ֵ
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

            //2���ԻỰ����Ĭ��֡���� T_snnCfg* stnSnnCfg_p
            u_short cmnAddr = 0;
            snnCfg_p->ssnID = srvNo;
            //snnCfg_p->actFlag
            snnCfg_p->infoType = GINFOTYPE;
            snnCfg_p->cmnAddr = cmnAddr;
            snnCfg_p->stepNum = stepNum;
            snnCfg_p->stepCfg_p = new T_ssnStepCfg[stepNum]();
            //��һ������
            snnCfg_p->stepCfg_p[0].stnAcsFlag = 0;
            //��һ������֡
            u_char oneReqAPDUNum = 1;//��һ��������֡����
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
            //���ýӿڸ�snnCfg_p��һ������֡�ĵ�1֡��ֵ
            setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
            //��һ����Ӧ
            snnCfg_p->stepCfg_p[0].respAPDUNum = 0;

            //�ڶ�������ȷ��
            snnCfg_p->stepCfg_p[1].stnAcsFlag = 0;
            //�ڶ���������֡
            snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
            //�ڶ�������Ӧ֡
            u_char twoRespAPDUNum = 1;//�ڶ�������Ӧ֡����
            snnCfg_p->stepCfg_p[1].respAPDUNum = twoRespAPDUNum;
            snnCfg_p->stepCfg_p[1].respBody_setNum_p = new u_char[twoRespAPDUNum]();
            snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] =
            0;//�ڶ�����Ӧ��1֡Ԫ�ؼ�����
            //snnCfg_p->stepCfg_p[1].respEleVal_pp = new ELE_TYPE[twoRespAPDUNum][11]();
            snnCfg_p->stepCfg_p[1].respData_p = new dataFrame[twoRespAPDUNum]();
            * ( eleVal[ASDU_COT_INDEX].p_Mem ) = MC_COT_7;
            //���ýӿڸ�snnCfg_p�ڶ�����Ӧ�ĵ�1֡��ֵ
            setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

            //3������Ĭ�ϵĲ�����tid,cot�Ķ�Ӧ��ϵ
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
 *���� : �ͷ�ONE_STEP����ռ�
 *���� : ONE_STEP����ָ��
 *��� : ��
 *���� : 0 �ɹ�
 *�޸���ʷ
 *
*******************************************************************************/
EXPDLL int freeOneStep ( ONE_STEP* pSpace )
{
    //û��ָ��ռ�ֱ�ӷ���
    if ( pSpace == NULL )
    {
        return 0;
    }

    //����ONE_STEP�г�ONE_STEP*֮��ĳ�Աָ��
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

    //�����ONE_STEP* nextStep��Ա��ݹ���һONE_STEP��Ա�ڵ�
    if ( pSpace->nextStep_p != NULL )
    {
        freeOneStep ( pSpace->nextStep_p );
    }

    delete  pSpace;
    pSpace = NULL;

    return 0;
}




/******************************************************************************
 *���� : �ͷ�FUN_RL����ռ�
 *���� : FUN_RL����ָ��
         T_snnCfg ����ָ��
         T_ptclFunDsc ����ָ��
 *��� : ��
 *���� : 0���ɹ�
 *�޸���ʷ
 *
******************************************************************************/
EXPDLL int  freeRl_P ( FUN_RL* pSpace,
                       T_snnCfg* pSnnCfg,
                       T_ptclFunDsc* ptclFunDsc
                     )
{
    u_int vRet = 0;

    if ( pSpace != NULL ) //FUN_RL�ͷ�
    {
        //steps[i],i���Ϊ MAX_MODE_N
        for ( u_int i = 0; i < MAX_MODE_N; i++ )
        {
            if ( pSpace->steps_p[i] != NULL )
            {
                //����freeOneStep�ݹ�ɾ�� ONE_STEP
                vRet = freeOneStep ( pSpace->steps_p[i] );
            }
        }

        delete  pSpace;
        pSpace = NULL;
    }

#ifdef INTER_CREATERL_VAL

    if ( pSnnCfg != NULL ) //T_snnCfg�ͷ�
    {
        if ( pSnnCfg->stepCfg_p != NULL )
        {
            for ( u_int j = 0; j < pSnnCfg->stepNum; j++ )
            {

                //�ͷ�����֡�ռ�
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
                              && ��k1 < pSnnCfg->stepCfg_p[j].reqBody_setNum_p[k]
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


                //�ͷ���Ӧ֡�ռ�
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

        //delete [] pSnnCfg;//pSnnCfg ���� createRl_P �д��������Բ��ڴ��ͷ�
    }

    if ( ptclFunDsc != NULL ) //T_ptclFunDsc �ͷ�
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

        //delete [] ptclFunDsc;//ptclFunDsc���� createRl_P �д��������Բ��ڴ��ͷ�
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

    //ͳ������֡����Ϣ��ĸ���
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
            ) //������������֡
        {
            if ( tStepCfg_p->reqData_p == NULL
                 || tStepCfg_p->reqBody_setNum_p == NULL
                 //|| tStepCfg_p->reqEleVal_pp[i] ==NULL
               )
            {
                return ERR_POINTER_NULL;
            }

            //���ͱ�ʶ
            ASDU_TID* tid_p = & (
                              tStepCfg_p->reqData_p[i].asduTid );
            memcpy ( &stnSnnPara_p->ASDUID_p[i],
                     tid_p->p_Mem,
                     tid_p->lng
                   );
            u_int tmpTID = stnSnnPara_p->ASDUID_p[i];

            //�ɱ�ṹ���޶���
            ASDU_VSQ* vsq_p = & (
                              tStepCfg_p->reqData_p[i].asduVsq );
            memcpy ( &stnSnnPara_p->acsMthd,
                     vsq_p->p_Mem,
                     vsq_p->lng
                   );

            ASDU_VSQ_VAL* tmpVsqVal_p = ( ASDU_VSQ_VAL* ) & (
                                        stnSnnPara_p->acsMthd );
            //�ɱ�ṹ�޶����е��������õĵ���������򱨴�
            /*if (tmpVsqVal_p->N != tStepCfg_p->reqBody_setNum_p[i])
            {
                return -2;
            }*/

            //����ԭ��
            ASDU_COT* cot_p = & (
                              tStepCfg_p->reqData_p[i].asduCot );
            memcpy ( &stnSnnPara_p->COT,
                     cot_p->p_Mem,
                     cot_p->lng
                   );

            //ASDU������ַ
            ASDU_ADDR* addr_p = & (
                                tStepCfg_p->reqData_p[i].asduAddr );
            memcpy ( &stnSnnPara_p->cmnAddr,
                     addr_p->p_Mem,
                     addr_p->lng
                   );

            //��Ϣ����
            BODY_OBJ* bodyObj_p =  tStepCfg_p->reqData_p[i].bodyObj_p;

            //tStepCfg_p->reqBody_setNum_p[i] ��Ϣ�������
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
                //��Ϣ���ַ
                BODY_ADDR* body_addr_p = NULL;
                //��ϢԪ�ؼ�
                BODY_SET* body_set_p = NULL;
                //��Ϣ��ʱ��
                BODY_TSP* body_tsp_p = NULL;
                body_addr_p = & (
                              tStepCfg_p->reqData_p[i].bodyObj_p[j].body_Addr );
                body_set_p = & (
                             tStepCfg_p->reqData_p[i].bodyObj_p[j].body_Set );
                body_tsp_p = & (
                             tStepCfg_p->reqData_p[i].bodyObj_p[j].body_Tsp );

                //�ж����õ�Ԫ�س����Ƿ񳬹�ϵͳ֧�ֵ���󳤶�
                u_short pntVal1lng = 0;

                if ( tmpVsqVal_p->N > 0
                     && body_set_p != NULL
                     && body_set_p->lng > 0
                   )
                {
                    /*ע��body_set_p->lngΪbody_set_p->p_Mem��
                        ����Ԫ�صĳ��ȶ���������Ԫ�ؼ��ĳ���
                        Ԫ�ؼ��ĸ����ɿɱ�ṹ�޶��ʵ� N ����
                    */
                    pntVal1lng = sizeof (
                                 tUsrData_p->gData.val_chkStd.pntVal1 );

                    if ( body_set_p->lng > pntVal1lng * 2 )
                    {
                        //Ҫ���صĵ�ֵ���ȴ��ڽ��ձ���pntVal1 ���쳣
                        return -3;
                    }
                }

                //ȡ��Ϣ���ַ��ֵ
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

                //ȡ��Ϣ��ʱ���ֵ
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

                    //��Ϣ����ʱ������
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


                //ȡ��ֵ
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
                            /*�ӵڶ����㿪ʼ��ֵ����һ����ĵ�ַ��for֮ǰ��ֵ
                            SQ:=1 ������ϢԪ�ص���������Ϣ�����ַ��Ѱַ����Ϣ
                            �����ַ��˳�򵥸�Ԫ�صĵ�һ����ϢԪ�ص�ַ����̵���
                            ��ϢԪ���Ǵ������ַ˳���1
                            */
                            tUsrData_p->gData.val_chkStd.pntAddr = pntAddr + k;
                        }
                        else//tmpVsqVal_p->SQ == 0
                        {
                            /*SQ:=0 �����Ϣ����ṹ��ÿ����Ϣ�������Լ������ĵ�ַ��
                            ÿ�������Ԫ�ؼ���ֻ��һ��Ԫ�أ����Լ���������Ϣ��ʱ��
                            */
                            tUsrData_p->gData.val_chkStd.pntAddr = pntAddr;
                        }

                    }



                    /*stnAcsFlag ��2λΪ1ʱ ��ֵ���ڴ˼���
                        stnAcsFlag ��2λΪ0ʱ ��Ҫ���صĲ���
                                    ��ֵ��Ҫ�ڴ˼�����
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
                            //�Ѷ��ֵ�ŵ���4�ֽ�
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

                        //Ʒ��������
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

                    /*����TID�ж϶�ӦԪ��ֵ��
                        ���Ƿ����4�ֽڣ������
                        ��4�ֽ���Ҫ�Ѷ��ֵ�ŵ�
                        ��4�ֽ�
                    */
                    /*if ( OVER4TID ( tmpTID ) )
                    {
                    tUsrData_p->gData.pntType = PNTTYPE_HIGH_SET;
                    }*/
                    tUsrData_p->gData.pntType = tTidObjLng.tidValLng;

                    /*��ʱ�����ʱ�긳���Ӧ��usrData_p�ṹ��
                        ��һ�����ʱ����for֮ǰ��ֵ
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

                        //��Ϣ����ʱ������
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

                if ( tmpVsqVal_p->N == 0 ) //��Ϣ��û��Ԫ�ؼ�
                {
                    tUsrData_p->gData.pntType = PNTTYPE_NULL_SET;
                    stnSnnPara_p->pntNum++;
                    setNum ++;
                    tUsrData_p = stnSnnPara_p->usrData_p + setNum;
                }
            }//end for ����Ϣ��ĸ�ֵ
        }//end for ��������֡
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
            ) //����������Ӧ֡
        {
            if ( tStepCfg_p->respData_p == NULL
                 || tStepCfg_p->respBody_setNum_p == NULL
               )
            {
                return ERR_POINTER_NULL;
            }

            //���ͱ�ʶ
            ASDU_TID* tid_p = & (
                              tStepCfg_p->respData_p[i].asduTid );
            memcpy ( &stnSnnPara_p->ASDUID_p[i],
                     tid_p->p_Mem,
                     tid_p->lng
                   );
            u_int tmpTID = stnSnnPara_p->ASDUID_p[i];

            //�ɱ�ṹ���޶���
            ASDU_VSQ* vsq_p = & (
                              tStepCfg_p->respData_p[i].asduVsq );
            memcpy ( &stnSnnPara_p->acsMthd,
                     vsq_p->p_Mem,
                     vsq_p->lng
                   );

            ASDU_VSQ_VAL* tmpVsqVal_p = ( ASDU_VSQ_VAL* ) & (
                                        stnSnnPara_p->acsMthd );
            //�ɱ�ṹ�޶����е��������õĵ���������򱨴�
            /*if (tmpVsqVal_p->N != tStepCfg_p->respBody_setNum_p[i])
            {
                return -2;
            }*/

            //����ԭ��
            ASDU_COT* cot_p = & (
                              tStepCfg_p->respData_p[i].asduCot );
            memcpy ( &stnSnnPara_p->COT,
                     cot_p->p_Mem,
                     cot_p->lng
                   );

            //ASDU������ַ
            ASDU_ADDR* addr_p = & (
                                tStepCfg_p->respData_p[i].asduAddr );
            memcpy ( &stnSnnPara_p->cmnAddr,
                     addr_p->p_Mem,
                     addr_p->lng
                   );

            //��Ϣ����
            BODY_OBJ* bodyObj_p =  tStepCfg_p->respData_p[i].bodyObj_p;

            //tStepCfg_p->respBody_setNum_p[i] ��Ϣ�������
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
                //��Ϣ���ַ
                BODY_ADDR* body_addr_p = NULL;
                //��ϢԪ�ؼ�
                BODY_SET* body_set_p = NULL;
                //��Ϣ��ʱ��
                BODY_TSP* body_tsp_p = NULL;
                body_addr_p = & (
                              tStepCfg_p->respData_p[i].bodyObj_p[j].body_Addr );
                body_set_p = & (
                             tStepCfg_p->respData_p[i].bodyObj_p[j].body_Set );
                body_tsp_p = & (
                             tStepCfg_p->respData_p[i].bodyObj_p[j].body_Tsp );

                //�ж����õ�Ԫ�س����Ƿ񳬹�ϵͳ֧�ֵ���󳤶�
                u_short pntVal1lng = 0;

                if ( tmpVsqVal_p->N > 0
                     && body_set_p != NULL
                     && body_set_p->lng > 0
                   )
                {
                    /*ע��body_set_p->lngΪbody_set_p->p_Mem��
                        ����Ԫ�صĳ��ȶ���������Ԫ�ؼ��ĳ���
                        Ԫ�ؼ��ĸ����ɿɱ�ṹ�޶��ʵ� N ����
                    */
                    pntVal1lng = sizeof (
                                 tUsrData_p->gData.val_chkStd.pntVal1 );

                    if ( body_set_p->lng > pntVal1lng * 2 )
                    {
                        //Ҫ���صĵ�ֵ���ȴ��ڽ��ձ���pntVal1 ���쳣
                        return -3;
                    }
                }

                //ȡ��Ϣ���ַ��ֵ
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

                //ȡ��Ϣ��ʱ���ֵ
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

                    //��Ϣ����ʱ������
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


                //ȡ��ֵ
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
                            /*�ӵڶ����㿪ʼ��ֵ����һ����ĵ�ַ��for֮ǰ��ֵ
                            SQ:=1 ������ϢԪ�ص���������Ϣ�����ַ��Ѱַ����Ϣ
                            �����ַ��˳�򵥸�Ԫ�صĵ�һ����ϢԪ�ص�ַ����̵���
                            ��ϢԪ���Ǵ������ַ˳���1
                            */
                            tUsrData_p->gData.val_chkStd.pntAddr = pntAddr + k;
                        }
                        else//tmpVsqVal_p->SQ == 0
                        {
                            /*SQ:=0 �����Ϣ����ṹ��ÿ����Ϣ�������Լ������ĵ�ַ��
                            ÿ�������Ԫ�ؼ���ֻ��һ��Ԫ�أ����Լ���������Ϣ��ʱ��
                            */
                            tUsrData_p->gData.val_chkStd.pntAddr = pntAddr;
                        }

                    }



                    /*stnAcsFlag ��2λΪ1ʱ ��ֵ���ڴ˼���
                        stnAcsFlag ��2λΪ0ʱ ��Ҫ���صĲ���
                                    ��ֵ��Ҫ�ڴ˼�����
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
                            //�Ѷ��ֵ�ŵ���4�ֽ�
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

                        //Ʒ��������
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

                    /*����TID�ж϶�ӦԪ��ֵ��
                        ���Ƿ����4�ֽڣ������
                        ��4�ֽ���Ҫ�Ѷ��ֵ�ŵ�
                        ��4�ֽ�
                    */
                    /*if ( OVER4TID ( tmpTID ) )
                    {
                        tUsrData_p->gData.pntType = PNTTYPE_HIGH_SET;
                    }*/
                    tUsrData_p->gData.pntType = tTidObjLng.tidValLng;

                    /*��ʱ�����ʱ�긳���Ӧ��usrData_p�ṹ��
                        ��һ�����ʱ����for֮ǰ��ֵ
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

                        //��Ϣ����ʱ������
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

                if ( tmpVsqVal_p->N == 0 ) //��Ϣ��û��Ԫ�ؼ�
                {
                    tUsrData_p->gData.pntType = PNTTYPE_NULL_SET;
                    stnSnnPara_p->pntNum++;
                    setNum ++;
                    tUsrData_p = stnSnnPara_p->usrData_p + setNum;
                }
            }//end for ����Ϣ��ĸ�ֵ

        }//end for ������Ӧ֡
    }//end PASSIVE_TYPE

    return 0;

}




/******************************************************************************
  *���ܣ���������������ʼ֡��Ӧ�Ĳ�
  *���룺
  *      srvNo            ��������
  *      usingMode        :ģʽ��
  *      preStepRul_p     ����ǰ���ܲ�������reqPDU_NumΪ��ǰ���ܲ�����ɵ�֡���
  *      statrtStepStnD_p :�Ự����Ҫ��������Ϣ�����������˵����
  *�����
  *      preStepRul_p     :���������ʼ֡���Ӧ�Ĳ���Ϣ
  *���أ�
  *      =0               :�ӿ�û���߼�ֱ�ӷ���
  *      >0               :�Ự״̬
  *      0xffff           :�ù��ܲ�֧��
  *      <0               :�ӿ��쳣
  *�޸���ʷ
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

    int tmpRet               = 0; //�²㺯������ֵ
    int nxtStepStat          = 0; //��һ֡�Ự״̬
    u_char nxtFrmStepNo      = 1; //��һ֡����
    ONE_STEP tmpNowRlStep    = {};//��ʱ���򲽱���
    T_ptclFunDsc tmpFuncDsc  = {};//��ʱ�Ự���������Ա���

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

    switch ( ssnNo ) //�жϹ���;
    {
        case STN_INIT://վ��ʼ��;
        {
            break;
        }

        case QUE_DATA://��ѯ����;
        {
            //���ýӿ�asgmtNxtFrmPar����װ֡����
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

            //���͵ȴ�����;
            nxtStepStat = MC_SEND_WAIT_CHECK;
            break;
        }

        case CYC_TRNS://���ڴ���;
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

            //���ͻỰ���;
            nxtStepStat = MC_SEND_SSN_FNSH;
            break;
        }

        case ACQ_EVT://�¼��ռ�;
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

            //���ͻỰ���;
            nxtStepStat = MC_SEND_SSN_FNSH;
            break;
        }

        case QUKCHK_EVT: //����¼�;
        {
            //��ʹ��
            nxtStepStat = 0xffff;
            break;
        }

        case STN_INRGTN: //վ�ٻ�
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
            ////ע����ֵ���ϲ��н��иı䲻�ڴ˸ı�
            ////preStepRul_p->reqPDU_Num    = 1;
            //preStepRul_p->sVerifyBlock_p = NULL;

            //tmpTID = MC_TID_100;
            //vsqVal.SQ = 0;
            //vsqVal.N = 1;
            //cot1Val.CAUSE = MC_COT_6;
            //addrVal.shortAddr = 1;
            //bodAddVal.shortAddr = 0;
            //bodSet = 20;//QCC վ�ٻ�
            //bodSetLng = 1;

            ////����ֵ��װASDU
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

            //���͵ȴ�����
            nxtStepStat = MC_SEND_WAIT_CHECK;
            break;
        }

        case CLK_SYN:       //ʱ��ͬ��;
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

            //���͵ȴ�����
            nxtStepStat = MC_SEND_WAIT_CHECK;
            break;
        }

        case CMD_TRNS://�����
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

            //���͵ȴ�����
            nxtStepStat = MC_SEND_WAIT_CHECK;
            break;
        }

        case ACMU_FRZ:
        {
            /*�ۻ�������ǰ�Ķ���    �˹����Ǵ�ACMU_TRNS
              ���ܵ�ģʽ2������վ�ȶ�������������������*/

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

            //���͵ȴ�����
            nxtStepStat = MC_SEND_WAIT_CHECK;
            break;
        }

        case ACMU_TRNS://�ۻ�������
        {
            if ( usingNode == CTLD_DRCTSONPT_TRNS )
            {
                tmpFuncDsc.act_prio_chnlID = PASSIVE_TYPE;
                tmpNowRlStep.respPDU_FCode = I_F;
                //���ͻỰ���
                nxtStepStat = MC_SEND_SSN_FNSH;
            }
            else//CTL_DRCTREQ_TRNS/CTL_FRZREQ_TRNS/CTL_FRZ_CTLD_SONPT_TRNS
            {
                tmpFuncDsc.act_prio_chnlID = ACTIVE_TYPE;
                tmpNowRlStep.reqPDU_FCode = I_F;
                //���͵ȴ�����
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

        case PARA_LD://��������
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

            //���͵ȴ�����
            nxtStepStat = MC_SEND_WAIT_CHECK;
            break;
        }

        case TST_PRCD:      //���Թ���;
        {
            //���ýӿ�asgmtNxtFrmPar����װ֡����
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

            //���͵ȴ�����
            nxtStepStat = MC_SEND_WAIT_CHECK;
            break;
        }

        case FILE_TRNS://�ļ�����
        {
            //�ݲ�ʵ��
            break;
        }

        case ACQ_TRNSDLY://�ռ������ӳ�ֵ
        {
            //��ʹ��
            nxtStepStat = 0xffff;
            break;
        }

        case U_STARTDT://104Э��U֡STARTDT ����ĻỰ
        {
            //���ýӿ�asgmtNxtFrmPar����װ֡����
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

            //���͵ȴ�����;
            nxtStepStat = MC_SEND_WAIT_CHECK;

            break;
        }

        case U_STOPDT://104Э��U֡STOPDT ����ĻỰ
        {
            //���ýӿ�asgmtNxtFrmPar����װ֡����
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

            //���͵ȴ�����;
            nxtStepStat = MC_SEND_WAIT_CHECK;
            break;
        }

        case U_TESTFR://104Э��U֡TESTFR ����ĻỰ
        {
            //���ýӿ�asgmtNxtFrmPar����װ֡����
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

            //���͵ȴ�����;
            nxtStepStat = MC_SEND_WAIT_CHECK;
            break;
        }

        case U_NEWHOLD:
        {
            //��U֡��STARTDT STOPDT TESTFR ȫΪ0 ��ʾ���µĻỰ������һ��ͨ��
            //���ýӿ�asgmtNxtFrmPar����װ֡����
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

            //���͵ȴ�����;
            nxtStepStat = MC_SEND_WAIT_CHECK;
            break;
        }

        case U_DELHOLD:
        {
            //��U֡��STARTDT STOPDT TESTFR ȫΪ1 ��ʾ���µĻỰ����һ��ͨ��
            //���ýӿ�asgmtNxtFrmPar����װ֡����
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

            //���͵ȴ�����;
            nxtStepStat = MC_SEND_WAIT_CHECK;
            break;
        }

        default:
            break;

    }

    return nxtStepStat;

}




/******************************************************************************
 *���ܣ���������������һ֡��Ӧ�Ĳ�
 * ���룺
 *      Fun_p        ��������
 *      stepRl_p     ����ǰ֡��Ӧ�Ĳ�
 *      funDsc_p     ���Ự����������
 *      ssnPar_p     ���Ự����Ҫ��������Ϣ�����������˵����
 *�����
 *      stepRl_p     ����һ֡��Ӧ�Ĳ�
 *      nxtStepStat_p���Ự״̬
 *���أ�
 *      >0           ������nxtStep������һ֡���ݵ���һ֡��Ӧ�Ĳ���(����Ѿ�����
                       ��һ�����һ֡���ֵΪ��ǰ֡��Ӧ�Ĳ��ţ�
 *      0xffff       ���ù��ܲ�֧��
 *      <0           ���ӿ��쳣
 *�޸���ʷ
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

    int               tmpRet   = 0;   //�����²�ӿڷ���ֵ
    ONE_STEP* nowRuleStep_p    = NULL; //��ǰ���Ĺ���ָ��
    ONE_STEP* nxtFrmRlStep_p   = NULL;//��һ֡��Ӧ�Ĺ���
    // u_char      allFrameNum  = 0;   //������֡�������޶�ֵ;
    // u_char      nowFrameNum  = 0;   //��ǰֵ֡���ǲ��ĵڼ�֡��;
    u_char     nxtFrmStepNo    = 0;

    *nxtStepStat_p = 0xffff;       //��ʼ��Ϊ�ǻỰ״ֵ̬

    ONE_STEP* lastRuleStep_p = NULL;
    nxtFrmStepNo = stepRl_p->stepNo;

    //�������U֡�����������Ự
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

        //�ҵ��������һ��
        lastRuleStep_p = Fun_p->steps_p[Fun_p->usingMode];

        while ( lastRuleStep_p->nextStep_p != NULL )
        {
            lastRuleStep_p    = lastRuleStep_p->nextStep_p;
        }
    }

    switch ( funDsc_p->ssnDscNo ) //�жϹ���;
    {
        case STN_INIT: //վ��ʼ��;
        {
            break;
        }

        case QUE_DATA: //��ѯ����;
        {
            if ( stepRl_p->stepNo > lastRuleStep_p->stepNo )
            {
                *nxtStepStat_p = MC_NO_SEND_SSN_STOP; //�����ͻỰ��ֹ
                return nxtFrmStepNo;
            }

            break;
        }

        case CYC_TRNS: //���ڴ���;�޵ڶ�֡;
        {
            if ( stepRl_p->stepNo > lastRuleStep_p->stepNo
                 || lastRuleStep_p->stepNo > C_STEP_NO_ONE
               )
            {
                *nxtStepStat_p = MC_NO_SEND_SSN_STOP; //�����ͻỰ��ֹ
                return nxtFrmStepNo;
            }

            break;
        }

        case ACQ_EVT: //�¼��ռ�;�޵ڶ�֡;
        {
            if ( stepRl_p->stepNo > lastRuleStep_p->stepNo
                 || lastRuleStep_p->stepNo > C_STEP_NO_ONE
               )
            {
                *nxtStepStat_p = MC_NO_SEND_SSN_STOP; //�����ͻỰ��ֹ
                return nxtFrmStepNo;
            }

            break;
        }

        case QUKCHK_EVT: //����¼�;
        {
            //�ݲ�ʵ��;
            return nxtFrmStepNo;
            break;
        }

        case STN_INRGTN: //վ�ٻ�;
        {
            if ( stepRl_p->stepNo > lastRuleStep_p->stepNo )
            {
                *nxtStepStat_p = MC_NO_SEND_SSN_STOP; //�����ͻỰ��ֹ
                return nxtFrmStepNo;
            }

            break;
        }

        case CLK_SYN: //ʱ��ͬ��;
        {
            if ( stepRl_p->stepNo > lastRuleStep_p->stepNo )
            {
                *nxtStepStat_p = MC_NO_SEND_SSN_STOP; //�����ͻỰ��ֹ
                return nxtFrmStepNo;
            }

            break;
        }

        case CMD_TRNS: //�����
        {
            if ( stepRl_p->stepNo > lastRuleStep_p->stepNo )
            {
                *nxtStepStat_p = MC_NO_SEND_SSN_STOP; //�����ͻỰ��ֹ
                return nxtFrmStepNo;
            }

            break;
        }

        case ACMU_FRZ:
        {
            /*�ۻ�������ǰ�Ķ���    �˹����Ǵ�ACMU_TRNS���ܵ�ģʽ2��
              ����վ�ȶ�������������������*/

            if ( stepRl_p->stepNo > lastRuleStep_p->stepNo )
            {
                *nxtStepStat_p = MC_NO_SEND_SSN_STOP; //�����ͻỰ��ֹ
                return nxtFrmStepNo;
            }

            break;
        }

        case ACMU_TRNS: //�ۻ�������
        {
            if ( Fun_p->usingMode == CTLD_DRCTSONPT_TRNS )
            {
                if ( stepRl_p->stepNo > lastRuleStep_p->stepNo )
                {
                    *nxtStepStat_p = MC_NO_SEND_SSN_STOP; //�����ͻỰ��ֹ
                    return *nxtStepStat_p;
                }
            }
            else if ( Fun_p->usingMode == CTL_DRCTREQ_TRNS )
            {
                if ( stepRl_p->stepNo > lastRuleStep_p->stepNo )
                {
                    *nxtStepStat_p = MC_NO_SEND_SSN_STOP; //�����ͻỰ��ֹ
                    return nxtFrmStepNo;
                }
            }
            else if ( Fun_p->usingMode == CTL_FRZREQ_TRNS )
            {
                if ( stepRl_p->stepNo > lastRuleStep_p->stepNo )
                {
                    *nxtStepStat_p = MC_NO_SEND_SSN_STOP; //�����ͻỰ��ֹ
                    return *nxtStepStat_p;
                }
            }
            else //CTL_FRZ_CTLD_SONPT_TRNS
            {
                if ( stepRl_p->stepNo > lastRuleStep_p->stepNo )
                {
                    *nxtStepStat_p = MC_NO_SEND_SSN_STOP; //�����ͻỰ��ֹ
                    return nxtFrmStepNo;
                }
            }

            break;
        }

        case PARA_LD: //��������
        {
            if ( stepRl_p->stepNo > lastRuleStep_p->stepNo )
            {
                *nxtStepStat_p = MC_NO_SEND_SSN_STOP; //�����ͻỰ��ֹ
                return nxtFrmStepNo;
            }

            break;
        }

        case TST_PRCD: //���Թ���;
        {
            if ( stepRl_p->stepNo > lastRuleStep_p->stepNo )
            {
                *nxtStepStat_p = MC_NO_SEND_SSN_STOP; //�����ͻỰ��ֹ
                return nxtFrmStepNo;
            }

            break;
        }

        case FILE_TRNS: //�ļ�����
        {
            //�ݲ�ʵ��
            break;
        }

        case ACQ_TRNSDLY: //�ռ������ӳ�ֵ
        {
            //��ʹ��
            return nxtFrmStepNo;
            break;
        }

        case U_STARTDT://104Э��U֡STARTDT ����ĻỰ
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

        case U_STOPDT://104Э��U֡STOPDT ����ĻỰ
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

        case U_TESTFR://104Э��U֡TESTFR ����ĻỰ
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
            //��U֡��STARTDT STOPDT TESTFR ȫΪ0 ��ʾ���µĻỰ������һ��ͨ��
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
            //��U֡��STARTDT STOPDT TESTFR ȫΪ1 ��ʾ���µĻỰ����һ��ͨ��
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
        //��ǰ���Ĺ���ָ��
        nowRuleStep_p = displayNextNode (
                        Fun_p->steps_p[Fun_p->usingMode],
                        stepRl_p
                        );

        if ( nowRuleStep_p == NULL )
        {
            return ERR_POINTER_NULL;
        }

        //�����һ֡�Ự״̬ nxtStepStat_p,��һ֡����nxtFrmStepNo
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

        //�з���֡�ĻỰ״̬��Ҫ����asgmtNxtFrmPar����װ֡
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
  *���ܣ���ASDU
  *���룺
  *     APDU_p  ��APDU��ַ
  *     APDU_lng: APDU����
  *�����
  *     TID     �����ͱ�ʶ��
  *     COT     ������ԭ��
  *���أ�
  *     0       ����ȷ
  *     <0      ������
  *�޸���ʷ��
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

    if ( APDU_lng == 0 ) //APDU_p ��ֻ��apci
    {
        //getFrameType(APDU_p,APDU_lng);
        APCI_104* apci_p = ( APCI_104* ) APDU_p ;

        if ( ( apci_p->ctlField[0] & U_F ) == U_F )
        {
            u_char data_type = apci_p->ctlField[0]; // �������λλ��1

            switch ( data_type )
            {
                case IEC104_STARTDT_ACT://STARTDT����
                    *TID_p = MC_TID_136;
                    *COT_p = MC_COT_6;
                    break;

                case IEC104_STARTDT_ACK://STARTDTȷ��
                    *TID_p = MC_TID_136;
                    *COT_p = MC_COT_7;
                    break;

                case IEC104_STOPDT_ACT://STOPDT����
                    *TID_p = MC_TID_137;
                    *COT_p = MC_COT_6;
                    break;

                case IEC104_STOPDT_ACK://STOPDTȷ��
                    *TID_p = MC_TID_137;
                    *COT_p = MC_COT_7;
                    break;

                case IEC104_TESTFR_ACT:// TESTFR����
                    *TID_p = MC_TID_138;
                    *COT_p = MC_COT_6;
                    break;

                case IEC104_TESTFR_ACK://TESTFRȷ��
                    *TID_p = MC_TID_138;
                    *COT_p = MC_COT_7;
                    break;

                case IEC104_NEWHOLD_ACT://NEWHOLD ����һ��ͨ�� ����
                    *TID_p = MC_TID_139;
                    *COT_p = MC_COT_6;
                    break;

                case IEC104_NEWHOLD_ACK://NEWHOLD ����һ��ͨ�� ȷ��
                    *TID_p = MC_TID_139;
                    *COT_p = MC_COT_7;
                    break;

                case IEC104_DELHOLD_ACT://DELHOLD ��һ��ͨ�� ����
                    *TID_p = MC_TID_140;
                    *COT_p = MC_COT_6;
                    break;

                case IEC104_DELHOLD_ACK://DELHOLD ��һ��ͨ�� ȷ��
                    *TID_p = MC_TID_140;
                    *COT_p = MC_COT_7;
                    break;

                default: //��֧�ֵ�U֡����
                    return -2;
            }
        }
    }
    else
    {
        APDU_104_p->bindToMem ( APDU_p, 1, APDU_lng ); //��APDU_104
        APDU_DSCR* apdu_dscr_p = APDU_104_p->getDU_Info();
        apdu_dscr_p->ASDU_CMTBL[APDU_APCI_INDEX] = 0;
        u_short asdu104 = 0;
        APDU_104_p->getDU ( asdu104, 1 );
        //ȡTID���
        ASDU_TID_VAL* tid = APDU_104_p->getASDU_TID_VAL (
                            &APDU_104_p->getDU_Info()->ASDU_CMTBL[ASDU_TID_INDEX]
                            );
        memcpy ( TID_p, tid, sizeof ( ASDU_TID_VAL ) );

        //ȡCOT���
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
        else //�޶����COT����
        {
            return -1;
        }
    }

    return 0 ;
}





/******************************************************************************
  *���ܣ���ȡ���ܵĹ��ܺ�
  *���룺
  *     TID��     ���ͱ�ʶ��
  *     COT��     ����ԭ��
  *�����
  *     funDsc_p���Ự�������ݵķ���(�����������������)
  *     funNum_p���������������funDsc_p����ʵ���±�ֵ��
  *���أ�
  *     Э���ţ���101��104
  *�޸���ʷ
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

    T_ptclFunDsc function[26] = {}; //����T_ptclFunDsc��a����
    *funNum_p = 0;

    //����Ӧ�ã�ר�÷�Χ��
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

            case MC_TID_139://NEWHOLD ����һ��ͨ��
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

            case MC_TID_140://DELHOLD ��һ��ͨ��
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
        //��function�����ֵ��������funDsc_p����
        memcpy ( funDsc_p, function, sizeof ( T_ptclFunDsc ) * ( *funNum_p ) );

        return 104;
    }

    if ( TID >= MC_TID_22
         && TID <= MC_TID_29 ) //22...29 Ϊ�������ݶ��屣��
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

    //ͨ��TID��Χ��COT��Χȷ��<��ѯ����>������
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

    //ͨ��TID��Χ��COT��Χȷ��<���ڴ���>������
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

    //ͨ��TID��Χ��COT��Χȷ��<�ռ��¼�>������
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

    //ͨ��TID��Χ��COT��Χȷ��<վ�ٻ�>������
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

    //ͨ��TID��Χ��COT��Χȷ��<ʱ��ͬ��>������
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

    //ͨ��TID��Χ��COT��Χȷ��<�����>������
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
            ) //fusk add:���ﲻ̫ȷ���Ƿ��� MC_COT_12 ��ֵ �����
    {
        function[*funNum_p].ssnDscNo = CMD_TRNS;
        function[*funNum_p].act_prio_chnlID = MC_MONITOR_DIRECTION;
        ( *funNum_p ) ++;
    }

    //ͨ��TID��Χ��COT��Χȷ��<�ۻ�������>������
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
            /*�ۻ�������ǰ�Ķ���    �˹����Ǵ�ACMU_TRNS
             ���ܵ�ģʽ2������վ�ȶ�������������������*/
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

    //ͨ��TID��Χ��COT��Χȷ��<��������>������
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

    //ͨ��TID��Χ��COT��Χȷ��<���Թ���>������
    if ( TID == MC_TID_107 ) //fusk add:��ȷ��104�Ƿ���tid=104�����
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

    //ͨ��TID��Χ��COT��Χȷ��<�ļ�����>������
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

    //��function�����ֵ��������funDsc_p����
    memcpy ( funDsc_p, function, sizeof ( T_ptclFunDsc ) * ( *funNum_p ) );

    return 104;

}




/******************************************************************************
  *���ܣ���ȡ�Ự����
  *���룺
  *     funDsc_p  :�Ự����������
  *     TID       :���ͱ�ʶ��
  *     COT       :����ԭ��
  *     PDUObj_p  :APDU104����
  *     ASDU_p    :ASDU��ַ
  *     ASDUlng   :ASDU����
  *�����
  *     ��
  *���أ�
  *     < 0       :ϵͳ�쳣
  *     0xffff    :�ù��ܲ�֧��
  *     > 0       :stepNo����
  *�޸���ʷ:
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

    //�������U֡�����������Ự
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

        //4¥�����������ͻ�����
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

    //   u_int tmpFlag = 0;//�Ƿ�Զ��վ���ý��з���,1:�����÷��أ�0:�������÷���
    //   u_char usingMode = funDsc_p->usingMode;
    //   u_char act_prio_chnlID = (u_char) funDsc_p->act_prio_chnlID;
    //
    //
    //APDU104* apdu_104_p =(APDU104*) PDUObj_p;
    //u_short  apdu_lng   = 0;
    //   u_char*  asdu104_p  = apdu_104_p->getDU(apdu_lng ,ASDU_TYPE);
    //   if (apdu_lng != ASDUlng)//�ж�ASDU��ַ�ĳ����Ƿ����;
    //   {
    //     return -1;
    //   }
    //   if (asdu104_p !=  ASDU_p) //Ҫ�󶨵�ASDU��ַ��ԭ����ASDU��ַ���ȲŽ��а�
    //   {
    //  APDU_DSCR* apdu_dscr_p = apdu_104_p->getDU_Info();
    //  apdu_dscr_p->ASDU_CMTBL[APDU_APCI_INDEX] = 0;
    //       //��ASDU_p��APDU104����
    //       apdu_104_p->bindToMem((ASDU_p), ASDU_TYPE , ASDUlng);
    //   }

    //   //ȡVSQ
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
    //           switch(funDsc_p->ssnDscNo)    //ͨ���ػ���ȷ������;
    //           {
    //           case STN_INIT: //վ��ʼ��
    //               {
    //                   break;
    //               }

    //           case QUE_DATA: //��ѯ����
    //               {
    //                   if (act_prio_chnlID == PASSIVE_TYPE) //����վ�Ự
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
    //                   else if (act_prio_chnlID == PASSIVE_TYPE)//����վ�Ự
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

    //           case CYC_TRNS: //���ڴ���
    //               {
    //                   if (act_prio_chnlID == ACTIVE_TYPE) //����վ�Ự
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

    //           case ACQ_EVT: //�¼��ռ�
    //               {
    //                   if (act_prio_chnlID == ACTIVE_TYPE) //����վ�Ự
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

    //           case QUKCHK_EVT: //����¼�
    //               {
    //                   //��ʹ��
    //                   stepNo = 0xffff;
    //                   break;
    //               }

    //           case STN_INRGTN: //վ�ٻ�
    //               {
    //                   if (act_prio_chnlID == PASSIVE_TYPE)  //����վ�Ự
    //                   {
    //                       if (TID == C_IC_NA
    //                           && VSQ_p->N == C_N_1
    //                           && VSQ_p->SQ == C_SQ_0
    //                           && (COT == MC_COT_6 || COT == MC_COT_8)
    //                           ) //�����ٻ�;
    //                       {
    //                           stepNo = C_STEP_NO_ONE;
    //                       }

    //                   }
    //                   else if (act_prio_chnlID == ACTIVE_TYPE) //����վ�Ự
    //                   {
    //                       if (TID == C_IC_NA
    //                           && VSQ_p->N == C_N_1
    //                           && VSQ_p->SQ == C_SQ_0
    //                           && COT == MC_COT_7
    //                           ) //ȷ�ϼ����ٻ�;
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
    //                           ) //��Ӧվ�ٻ�;
    //                       {
    //                           stepNo =  C_STEP_NO_TWO;
    //                       }
    //                       else if (TID == C_IC_NA
    //                           && VSQ_p->N == C_N_1
    //                           && VSQ_p->SQ == C_SQ_0
    //                           && COT == MC_COT_10
    //                           ) //�ٻ���Ϣ����֪ͨ
    //                       {
    //                           stepNo =  C_STEP_NO_THREE;
    //                       }
    //                   }

    //                   break;
    //               }//end case STN_INRGTN

    //           case CLK_SYN: //ʱ��ͬ��
    //               {
    //                   if (act_prio_chnlID == PASSIVE_TYPE) //����վ�Ự
    //                   {
    //                       if (TID == C_CS_NA
    //                           && VSQ_p ->N == C_N_1
    //                           && VSQ_p ->SQ == C_SQ_0
    //                           && COT == MC_COT_6
    //                           ) //����ʱ��ͬ��;
    //                       {
    //                           stepNo = C_STEP_NO_ONE;
    //                       }
    //                   }
    //                   else if (act_prio_chnlID == ACTIVE_TYPE) //����վ�Ự
    //                   {
    //                       if (TID == C_CS_NA
    //                           && VSQ_p ->N == C_N_1
    //                           && VSQ_p ->SQ == C_SQ_0
    //                           && COT == MC_COT_7
    //                           ) //ȷ�ϼ���ʱ��ͬ��;
    //                       {
    //                           stepNo = C_STEP_NO_ONE;
    //                       }
    //                   }
    //                   break;
    //               }
    //           case CMD_TRNS: //�����
    //               {
    //                   if (act_prio_chnlID == PASSIVE_TYPE) //����վ�Ự
    //                   {
    //                       if(  (TID >= MC_TID_45 && TID <= MC_TID_51
    //                           || TID >= MC_TID_58 && TID <= MC_TID_64)
    //                           &&(COT == MC_COT_6 || COT == MC_COT_8)
    //                           )
    //                       {
    //                           stepNo = C_STEP_NO_ONE;
    //                       }
    //                   }
    //                   else if(act_prio_chnlID == ACTIVE_TYPE) //����վ�Ự
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
    //               {//�ۻ�������ǰ�Ķ���    �˹����Ǵ�ACMU_TRNS���ܵ�ģʽ2������վ
    //                //�ȶ�������������������
    //                   if (act_prio_chnlID == PASSIVE_TYPE) //����վ�Ự
    //                   {
    //                       if(TID == MC_TID_101
    //                           && VSQ_p->N == C_N_1
    //                           && VSQ_p->SQ == C_SQ_0
    //                           && COT == MC_COT_6)
    //                       {
    //                           stepNo = C_STEP_NO_ONE;
    //                       }
    //                   }
    //                   else if(act_prio_chnlID == ACTIVE_TYPE) //����վ�Ự
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
    //           case ACMU_TRNS: //�ۻ�������
    //               {

    //                   if (act_prio_chnlID == PASSIVE_TYPE) //����վ�Ự
    //                   {
    //                       if(TID == MC_TID_101
    //                           && VSQ_p->N == C_N_1
    //                           && VSQ_p->SQ == C_SQ_0
    //                           && COT == MC_COT_6)
    //                       {
    //                           stepNo = C_STEP_NO_ONE;
    //                       }
    //                   }
    //                   else if(act_prio_chnlID == ACTIVE_TYPE) //����վ�Ự
    //                   {
    //                       if (  (TID == MC_TID_15
    //                              || TID == MC_TID_16
    //                              || TID == MC_TID_37)
    //                           && COT == MC_COT_3
    //                           )
    //                       {
    //                           if (usingMode == CTLD_DRCTSONPT_TRNS)
    //                           {//ģʽ0������վֱ��ͻ������
    //                               stepNo = C_STEP_NO_ONE;
    //                           }
    //                           else if (usingMode == CTL_FRZ_CTLD_SONPT_TRNS)
    //                           {//ģʽ3������վ����󱻿�վͻ������
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
    //           case PARA_LD: //��������
    //               {
    //                   if (act_prio_chnlID == PASSIVE_TYPE) //����վ�Ự
    //                   {
    //if(TID >= MC_TID_110
    //    && TID <= MC_TID_113
    //    &&COT == MC_COT_6
    //  )
    //{
    //    stepNo = C_STEP_NO_ONE;
    //}
    //                   }
    //                   else if(act_prio_chnlID == ACTIVE_TYPE) //����վ�Ự
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
    //           case TST_PRCD: //���Թ���
    //               {
    //                   if (act_prio_chnlID == PASSIVE_TYPE) //����վ�Ự
    //                   {
    //                       if ((TID == MC_TID_104 || TID == MC_TID_107)
    //                           && VSQ_p ->N == C_N_1
    //                           && VSQ_p ->SQ == C_SQ_0
    //                           && COT == MC_COT_6
    //                           ) //�������;
    //                       {
    //                           stepNo = C_STEP_NO_ONE;
    //                       }
    //                   }
    //                   else if (act_prio_chnlID == ACTIVE_TYPE) //����վ�Ự
    //                   {
    //                       if ((TID == MC_TID_104 || TID == MC_TID_107)
    //                           && VSQ_p ->N == C_N_1
    //                           && VSQ_p ->SQ == C_SQ_0
    //                           && COT == MC_COT_7
    //                           ) //ȷ�ϼ������;
    //                       {
    //                           stepNo = C_STEP_NO_ONE;
    //                       }
    //                   }
    //                   break;

    //               }
    //           case FILE_TRNS: //�ļ�����
    //               {
    //                   //�ݲ�ʵ��
    //                   break;
    //               }
    //           case ACQ_TRNSDLY: //�ռ������ӳ�ֵ
    //               {
    //                   //��ʹ��
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
  *���� ���ԻỰ���е�֡��������
  * ���룺
  *     Fun_p        :������
  *     stepRl_p     :��ǰ��
  *     funDsc_p     :�Ự����������
  *     nxtStepNo_p  :��һ֡�Ĳ���
  *�����
  *     ssnPar_p     :���������˵��
  *     nxtStepNo_p  :��һ֡�Ĳ���
  *���أ�
  *  0               :�ɹ���
  *  0xffff          :�ù��ܲ�֧��
  *  ����            :ʧ��
  *                -1 :��ǰ�Ự������
  *                -2 :��ǰ�Ự���е�֡��������
  *                -3 :����ԭ�����
  *                -4 : �������Բ�������
  *                -5 :�޶�����ʵ��Ԫ�ظ�������
  *               -21 :���ֵ����ʧ��
  *               -22 :��Сֵ����ʧ��
  *               -23 :�������С����ֵ����
  *               ������ֵΪ�²㺯������
  *�޸���ʷ
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



    //�������ԻỰ��ʶ
    u_char act_prio_chnlID     = ( u_char ) funDsc_p->act_prio_chnlID;

    if ( act_prio_chnlID != ACTIVE_TYPE
         && act_prio_chnlID != PASSIVE_TYPE
       )
    {
        return ERR_ACT_PAS_VAL;
    }

    //�����U֡�����������Ự
    if ( funDsc_p->ssnDscNo == U_STARTDT
         || funDsc_p->ssnDscNo == U_STOPDT
         || funDsc_p->ssnDscNo == U_TESTFR
         || funDsc_p->ssnDscNo == U_NEWHOLD
         || funDsc_p->ssnDscNo == U_DELHOLD
       )
    {
        switch ( funDsc_p->ssnDscNo ) //�жϹ��ܺ�;
        {

            case U_STARTDT://104Э��U֡STARTDT ����ĻỰ
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

            case U_STOPDT://104Э��U֡STOPDT ����ĻỰ
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

            case U_TESTFR://104Э��U֡TESTFR ����ĻỰ
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
                //��U֡��STARTDT STOPDT TESTFR ȫΪ0 ��ʾ���µĻỰ������һ��ͨ��
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
                //��U֡��STARTDT STOPDT TESTFR ȫΪ1 ��ʾ���µĻỰ����һ��ͨ��
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
    else//��U֡�����������Ự
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

        //ģʽ��
        u_char usingMode            = funDsc_p->usingMode;

        if ( usingMode != Fun_p->usingMode )
        {
            return ERR_CONFIG;
        }

        //��ȡ�����е�ǰ����ָ��;
        ONE_STEP* nowRuleStep_p = displayNextNode ( Fun_p->steps_p[usingMode],
                                  stepRl_p
                                                  );

        if ( nowRuleStep_p == NULL )
        {
            return ERR_POINTER_NULL;
        }

        //�ҵ��������һ��
        ONE_STEP* lastRuleStep_p = Fun_p->steps_p[usingMode];

        while ( lastRuleStep_p->nextStep_p != NULL )
        {
            lastRuleStep_p    = lastRuleStep_p->nextStep_p;
        }

        //ȡ������Ӧ�Ķ���
        APDU104* apdu104_p = NULL;

        if ( act_prio_chnlID == PASSIVE_TYPE )
        {
            if ( stepRl_p->respAPDU_Obj_p == NULL )
            {
                return ERR_POINTER_NULL;
            }

            apdu104_p = ( APDU104* ) stepRl_p->respAPDU_Obj_p;
            ////��һ֡�뵱ǰ֡����һ����Ҫ�Բ���ĳЩֵ���г�ʼ��
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
            ////��һ֡�뵱ǰ֡����һ����Ҫ�Բ���ĳЩֵ���г�ʼ��
            //if (*nxtStepNo_p != stepRl_p->stepNo)
            //{
            //    stepRl_p->reqPDU_Num = 0;
            //    stepRl_p->reqPDU_FCode = 0;
            //    stepRl_p->respPDU_Num = 1;
            //}
        }

        int vRet    = 0;    //�����²㺯������ֵ
        //����chkAsduDataID����TID��COT��У��
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

        //����chkVerifyBlock���д�СֵУ��:����Ự֡����ҪУ��
        //  ����ֱ�ӷ���0
        vRet = chkVerifyBlock ( nowRuleStep_p->sVerifyBlock_p,
                                apdu104_p
                              );

        if ( vRet != 0 )
        {
            return vRet;
        }

        /*����ֵ�ļ��ء������ʶ:
            λ0 = 1�洢��ϵͳִ������ֵ�ı��棻
            λ1 = 1�洢��ϵͳִ������ֵ�ļ��أ�
                ȫ = 0, do nothing��ȫ = 1�������ұ���
            λ3 - 10 �ֱ��ʾAPDUԪ��3 -10��
                =1��ʾ��λ0��λ1�Ĳ����ڴ�Ԫ������Ч��=0Ϊ��Ч��
            */
        T_stnAcsFlag tmpStnAcsFlag = {};
        //���ղ�����һ֡����,�洢�����ر�ʶ
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

        //����stepRl_p��apdu104����Ӧ���ݸ�ssnPar_p��ֵ
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

    if ( act_prio_chnlID == PASSIVE_TYPE ) //����Ӧ֡
    {
        apdu104_p = ( APDU104* ) stepRl_p->respAPDU_Obj_p;
    }
    else //������֡
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
        /*fusk@20180510 add �˴���apdu_dscr_p->InfoEleLng�ÿ��Կ���ͨ��
          Э���Լ����õ�g_TID104�õ�
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
        return -1;//apdu���ȴ���
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
//*���ܣ���ȡ����ͷ����
// *����ֵ��
// *        ����ͷ����
// */
EXPDLL u_char getHeaderlng()
{
    return 2;//����104��ͷ�̶�����
}




/******************************************************************************
*���ܣ���װAPDU�����ö�ʱ�������Լ���������
*���룺
*      rtmParam_p   ��ʵʱ�����������˷�����ţ�������� �� �Լ�K��W��ֵ
*      APDU104_p    ��APDU104����
*      ASDUFrm_Lng  : ASDU���ݳ���
*�����
*      APDU104_p    : ��װ���APDU104����
*���أ�
*      void
*�޸���ʷ��
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
    //����APDU�ĵ�ַ
    //APDU_104->bindToMem(ASDUFrm_p - 6 ,ASDU_TYPE);
    APDU104* tApdu104_p  = ( APDU104* ) APDU104_p;
    u_char* APDUFrm_p    = tApdu104_p->getDU ( ASDUFrm_Lng, 1 );
    realTimeValue* rtm_p = ( realTimeValue* ) rtmParam_p; //ʵʱ����

    if ( APDUFrm_p == NULL )
    {
        return ;
    }

    APCI_104* apci_p = ( APCI_104* ) APDU_buf_p;
    apci_p->startChar = SRTCHAR_F1_2_V;

    if ( ASDUFrm_Lng == 0 ) //U֡
    {
        //����APCI֡ͷ
        /*U_FRM u_frm_p     = {0};
        u_frm_p.frameType = 3;
        memcpy(apci_p->ctlField ,&u_frm_p , sizeof(U_FRM));*/

        memcpy ( apci_p, ( APCI_104* ) APDUFrm_p, sizeof ( APCI_104 ) );
    }
    else//I֡
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
    realTimeValue* rtm_p = ( realTimeValue* ) rtmParam_p; //ʵʱ����

    frameType frmType = {0};

    if ( apci_p->startChar != SRTCHAR_F1_2_V )
    {
        //�������յ��޷�ʶ���֡
        frmType.fTypeL = MC_APDU_UNKNOWN;
        frmType.fTypeH = CLOSE_CONNECT;
        return * ( u_short* ) &frmType;
    }

    if ( apci_p->APDULng != revDataFrm->APDU_Buf.APDU_Lng - 2 )
    {
        //�ж��յ���֡����
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
            //�������յ�APDU�Ƿ�ʧ��
            frmType.fTypeL = MC_APDU_DISORDER;
            frmType.fTypeH = CLOSE_CONNECT;
        }
        else
        {
            revDataFrm->APDU_Buf.APDU_Data_p += sizeof ( APCI_104 );
            revDataFrm->APDU_Buf.APDU_Lng    -= sizeof ( APCI_104 );
            frmType.fTypeL                    = NO_OPERATION;
            frmType.fTypeH                    = I_F;//0��ʾ����֡
            rtm_p->kValue = ( rtm_p->sendnum % 0x7fff ) - ( i_frm_p->RNo % 0x7fff );
        }
    }
    else if ( frmeType_p == S_F )
    {
        S_FRM* S_frm_p                 = ( S_FRM* ) apci_p->ctlField;
        revDataFrm->APDU_Buf.APDU_Lng  = 0;
        frmType.fTypeL                 = NO_OPERATION;
        frmType.fTypeH                 = S_F;//1��ʾ����֡
        rtm_p->kValue = ( rtm_p->sendnum % 0x7fff ) - ( S_frm_p->hRNo  % 0x7fff );

    }
    else  if ( frmeType_p == U_F )
    {
        revDataFrm->APDU_Buf.APDU_Lng  = 0;
        frmType.fTypeL                 = NO_OPERATION;
        frmType.fTypeH                 = U_F;////1��ʾ����֡
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
    // ��ȡ��ǰͨ���ķ���״̬
    char curStage = ( ( realTimeValue* ) rtmParam_p )->stage; //rtmParam.stateStage;
    // ��ȡ��ǰͨ���������ݵ�����
    u_char frmType = getFrameType ( data_p, length );

    // U֡ͬʱ���ڶ��ֹ���
    if ( frmType == -1 )
    {
        return SEND_FRAME_ERROR;
    }

    switch ( role )
    {
        // ����վ
        case MASTER_STATION :
            switch ( curStage )
            {
                // STOPPED����
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

                // ����δȷ��STOPPED����
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

                // STARTED����
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

                // ����STARTED����
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

                // ����STOPPED����
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

                // �Ƿ���״̬��״̬���в����ڵ�״̬��
                default :
                    result.errType = SEND_STATUS_ERROR;
                    result.motion  = CLOSE_CONNECT;

            }

            break;

        // ������վ
        case SLAVE_STATION:
            switch ( curStage )
            {
                // STOPPED����
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

                // ����δȷ��STOPPED����
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

                // STARTED����
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

                // �Ƿ���״̬��״̬���в����ڵ�״̬��
                default:
                    result.errType = SEND_STATUS_ERROR;
                    result.motion  = CLOSE_CONNECT;
            }

            break;

        // �Ƿ�վ���ɫ
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
    // ��ȡ��ǰͨ���ķ���״̬
    char curStage = ( ( realTimeValue* ) rtmParam_p )->stage; //rtmParam.stateStage;
    // ��ȡ��ǰͨ���������ݵ�����
    u_char frmType = getFrameType ( data_p, length );

    // U֡ͬʱ���ڶ��ֹ���
    if ( frmType == -1 )
    {
        return SEND_FRAME_ERROR;
    }

    switch ( role )
    {
        // ����վ
        case MASTER_STATION :
            switch ( curStage )
            {
                // STOPPED����
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

                // ����δȷ��STOPPED����
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

                // STARTED����
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

                // ����STARTED����
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

                // ����STOPPED����
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

                // �Ƿ���״̬��״̬���в����ڵ�״̬��
                default :
                    result.errType = SEND_STATUS_ERROR;
                    result.motion  = CLOSE_CONNECT;

            }

            break;

        // ������վ
        case SLAVE_STATION:
            switch ( curStage )
            {
                // STOPPED����
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

                // ����δȷ��STOPPED����
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

                // STARTED����
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

                // �Ƿ���״̬��״̬���в����ڵ�״̬��
                default:
                    result.errType = SEND_STATUS_ERROR;
                    result.motion  = CLOSE_CONNECT;
            }

            break;

        // �Ƿ�վ���ɫ
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
    // ���Э�����Ʊ���
    u_short result = 0;
    realTimeValue* rtm_p = ( realTimeValue* ) rtmParam_p;

    // ��ȡ��ǰͨ���ķ���״̬
    char curStage = rtm_p->stage;  //rtmParam.stateStage;
    //  ������һ��״̬
    //      char nxtStage;

    // ��ȡ��ǰͨ����������֡������
    u_char frmType = getFrameType ( data_p, length );


    // 1.���·���״̬
    switch ( role )
    {
        // ����վ
        case MASTER_STATION :
            switch ( curStage )
            {
                // STOPPED����
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

                // ����STARTED����
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

                // STARTED����
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

                // ����δȷ��STOPPED����
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

                // ����STOPPED����
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

                // �Ƿ���״̬��״̬���в����ڵ�״̬��
                default :
                    break;
            }

            break;

        // ������վ
        case SLAVE_STATION:
            switch ( curStage )
            {
                // STOPPED����
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

                // STARTED����
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

                // ����δȷ��STOPPED����
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

                // �Ƿ���״̬��״̬���в����ڵ�״̬��
                default:
                    result = 2;
            }

            break;

        default:
            break;
    }

    // 2. ������������ʵʱ����
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

    // 3.���¶�ʱ���Ŀ��Ʋ��������ö�ʱ������֤�롣
    if ( ( frmType == I_F ) |
         ( frmType == IEC104_STOPDT_ACT ) |
         ( frmType == IEC104_STARTDT_ACT ) |
         ( frmType == IEC104_TESTFR_ACT ) )
    {
        ( ( timerType* ) tmrCtrlCode_p )->T1 = TIMER_PAUSE; //T1����
    }

    ( ( timerType* ) tmrCtrlCode_p )->T3 = TIMER_PAUSE; //T3����
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
    // ���Э�����Ʊ���
    ctrl_Value result = {0};

    realTimeValue* rtm_p = ( realTimeValue* ) rtmParam_p;
    // ��ȡ��ǰͨ���ķ���״̬
    //char curStage = rtm_p->stage;  //rtmParam.stateStage;
    // ������һ��״̬
    //char nxtStage;

    // ��ȡ��ǰͨ����������֡������
    u_char frmType = getFrameType ( data_p, length );

    //
    switch ( frmType )
    {
        case IEC104_TESTFR_ACK:
            result.fType = IEC104_TESTFR_ACK;
            result.bit3  = 1;
            ( ( timerType* ) tmrCtrlCode_p )->T1 = TIMER_STOP; //T1ֹͣ
            break;

        case IEC104_TESTFR_ACT:
            result.fType = IEC104_TESTFR_ACT;
            result.bit3  = 1;
            break;

        case IEC104_STOPDT_ACK:
            result.fType = IEC104_STOPDT_ACK;
            rtm_p->stage = MASTER_STOPPED;
            result.bit3  = 1;
            ( ( timerType* ) tmrCtrlCode_p )->T1 = TIMER_STOP; //T1ֹͣ
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
            ( ( timerType* ) tmrCtrlCode_p )->T1 = TIMER_STOP; //T1ֹͣ
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
            ( ( timerType* ) tmrCtrlCode_p )->T2 = TIMER_PAUSE; //T2��λ

            if ( rtm_p->wValue == ( ( realTimeThreshold* ) cfgParam_p )->W )
            {
                result.bit1 = 1;
                result.bit2  = 1;
            }

            ( ( timerType* ) tmrCtrlCode_p )->T1 = TIMER_STOP ; //T1ֹͣ
            //rtm_p->kValue =  (rtm_p->sendnum  % 0x7fff) - (rtm_p->receivenum  % 0x7fff);
            break;

        case S_F:
            result.fType = S_F;
            result.bit1  =
            1; //�յ�S֡�����������1.t2��ʱ����ʱ ��Wֵ�ѵ����ֵ

            //rtm_p->kValue = (rtm_p->sendnum  % 0x7fff) - (rtm_p->receivenum  % 0x7fff);
            if ( rtm_p->wValue == 0 )
            {
                ( ( timerType* ) tmrCtrlCode_p )->T1 = TIMER_STOP ; //T1ֹͣ
            }

            break;

        default:
            break;
    }

    ( ( timerType* ) tmrCtrlCode_p )->T3 = TIMER_PAUSE ; //T3����
    result.bit0 = 1;//������ʱ��
    return * ( u_short* ) &result;
}




EXPDLL int getFrameType ( u_char* data_p, u_short length )
{
    /****************    IEC104Э���Ӧ�����ݸ�ʽ    ***************************/
    /*|-----------------------|*/
    /*|�����ַ�68H (һ���ֽ�) |*/
    /*|-----------------------|*/
    /*|APDU����    (һ���ֽ�) |*/
    /*|-----------------------|*/
    /*|������      (�ĸ��ֽ�) |*/
    /*|-----------------------|*/
    /*|ASDU����    (�䳤�ֽ�) |*/
    /*|-----------------------|*/

    /****************    �������I֡���ݸ�ʽ    ********************************/
    /*|----------------------------------------------------|                 */
    /*| bit8 | bit7| bit6| bit5 | bit4 | bit3| bit2 | bit1 |                 */
    /*|---------------------------------------------|------|                 */
    /*|                 �������к�N(S)           LSB|   0  |  ��λλ��1       */
    /*|---------------------------------------------|------|                 */
    /*| MSB             �������к�N(S)                     |  ��λλ��2       */
    /*|----------------------------------------------------|                 */
    /*|                 �������к�R(S)           LSB|  0   |  ��λλ��3       */
    /*|----------------------------------------------------|                 */
    /*| MSB             �������к�N(S)                     |  ��λλ��4       */
    /*|----------------------------------------------------|                 */

    /****************    �������S֡���ݸ�ʽ    ********************************/
    /*|----------------------------------------------------|                 */
    /*| bit8 | bit7| bit6| bit5 | bit4 | bit3| bit2 | bit1 |                 */
    /*|--------------------------------------|------|------|                 */
    /*|                  0                   |   0  |   1  |  ��λλ��1       */
    /*|--------------------------------------|------|------|                 */
    /*|                         0                          |  ��λλ��2       */
    /*|----------------------------------------------------|                 */
    /*|                 �������к�R(S)           LSB |   0 |  ��λλ��3       */
    /*|----------------------------------------------------|                 */
    /*| MSB             �������к�N(S)                     |  ��λλ��4       */
    /*|----------------------------------------------------|                 */

    /****************    �������U֡���ݸ�ʽ    ********************************/
    /*|----------------------------------------------------|                 */
    /*| bit8 | bit7| bit6| bit5 | bit4 | bit3| bit2 | bit1 |                 */
    /*|------------|------------|------------|------|------|                 */
    /*|   TESTFR   |   STOPDT   |   STARTDT  |      |      |                 */
    /*|------------|------------|------------|  1   |   1  |  ��λλ��1      */
    /*| ȷ�� | ����| ȷ�� | ����| ȷ�� | ����|      |      |                  */
    /*|------------|------------|------------|-------------|                 */
    /*|                         0                          |  ��λλ��2       */
    /*|----------------------------------------------------|                 */
    /*|                         0                          |  ��λλ��3       */
    /*|----------------------------------------------------|                 */
    /*|                         0                          |  ��λλ��4       */
    /*|----------------------------------------------------|                 */
    if ( data_p == NULL )
    {
        return ERR_POINTER_NULL;
    }

    //����APDU�ĵ�ַ
    APCI_104* apci_p = ( APCI_104* ) data_p ;

    if ( ( apci_p->ctlField[0] & U_F ) == U_F )
    {
        //char data_type = apci_p->ctlField[0]; // �������λλ��1
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

            default: // ͬʱ���ڶ��ֹ��ܻ���Ϊ��U֡����
                return -1;
        }
    }
    else if ( ( apci_p->ctlField[0] & S_F ) == S_F )
    {
        return S_F;
    }

    else//I֡
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
    //�˺�������104��Լ�Ĵ����ǶϿ�����
    return CLOSE_CONNECT;
}




/*******************************************************************************
* ���ܣ����촫����Ƶ�Ӧ��֡����
* ����������IEC104Э���׼��������Ӧ��������������Ĺ��ܣ�
*     1.�����������Ϊ����֡���ͣ�������Ӧ��Ӧ��֡��������յ�U֡�ġ�����֡������Ҫ
*       �����Ӧ�ġ�ȷ��֡����
*     2.�����������Ϊ����֡���ͣ��ҽ��������ﵽ�������޷�ֵWʱ����Ҫ����һ������ȷ
*       ��֡����S֡��
* �ӿڲ�����
*

* ����ֵ������Э����ܿ��ƣ�����ֵ�������£�
*     �������ֵΪ1�����ʾ��Ҫ��ܶԶ�ʱ���������ã�������Ҫ����
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

        default: // ͬʱ���ڶ��ֹ��ܻ���Ϊ��U֡����
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

        default: // ͬʱ���ڶ��ֹ��ܻ���Ϊ��U֡����
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
        return -1;//apdu���ȴ���
    }

    return 0;
}




/******************************************************************************
  *���ܣ��ӻ��������APDU�Ĵ洢����
  * ���룺
  *      bufferAdd   ���������׵�ַ
  *      bufferLng   : ����������
  *�����
  *      ��
  *���أ�
  *    >0 �ɹ�����APDU����
  *    ��1 ���������ȴ���
  *    ��2 �׵�ַ���������ַ�
  *    <0  �ӿ�ִ���쳣
  *�޸���ʷ
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
  *���ܣ���ʱ��T0��T1��T2��T3��ʱ������
  * ���룺
  *      parameter_p   ��Ϊ����������ʱ����
  *���أ�
  *   �͵�1λ��λ0������1Ϊ�رն�ʱ��
  *   �͵�2λ��λ1������1Ϊ�ر�����
  *   �͵�3λ��λ2������1Ϊ����S֡
  *   �͵�4λ��λ3������1Ϊ���Ͳ��Լ���
  *�޸���ʷ
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






