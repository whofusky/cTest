
#define DLL_IMPLEMENT

#include "ModbusTcpIp.h"
#include "InteFunc.h"




/*******************************************************************************
*���ܣ�����һ��ADU����
*����������C_ModbusADU��Ĺ��캯������һ��ADU����
*���룺
*      cstmInfo_p   �� ADU����֡�Ľṹ������
*      uType        �� ����֡�����ͣ�PDU��ADU��
*      allocFlag    �� �Ƿ����洢�ռ�
*���أ�
*      u_char*      : ADU����
*
*******************************************************************************/
EXPDLL u_char* createAPDU ( APDU_DSCR* cstmInfo_p,
                            u_char duType,
                            u_char  allocFlag
                          )
{
    /*ADU_DSCR            ��   APDU_DSCR �Ķ�Ӧ��ϵ
    ADU_DSCR.ADU_TID;    ��Ӧ  APDU_DSCR.ASDU_TID;
    ADU_DSCR.InfoEleLng; ��Ӧ  APDU_DSCR.InfoEleLng;
    ADU_DSCR.ADU_CMTBL[ADU_CMTBL_NUM]; ��Ӧ��
                   APDU_DSCR.ASDU_CMTBL [11];��ADU_CMTBLֻ�����£���Ԫ��
    0   //ADU MBAP����ͷ
    1   //ADU ��ַ��
    2   //PDU������
    3   //PDU����
    4   //ADU���У��
    */
    ADU_DSCR* aduInfo_p = ( ADU_DSCR* ) cstmInfo_p;
    C_ModbusADU* apdu_p = new C_ModbusADU ( aduInfo_p, duType, allocFlag );
    return ( u_char* ) apdu_p;
}




/*******************************************************************************
*���ܣ��ͷ�һ��ADU����Ŀռ䡣
*����������C_ModbusADU������������ͷ�һ��ADU����Ŀռ䡣
*���룺
*      APDU_Obj_p   �� ADU����
*���أ�
*      ��
*
*******************************************************************************/
EXPDLL void freeAPDU ( u_char* APDU_Obj_p )
{
    if ( APDU_Obj_p != NULL )
    {
        C_ModbusADU* apdu_p = ( C_ModbusADU* ) APDU_Obj_p;
        delete apdu_p;
        APDU_Obj_p = NULL;
    }
}




/*******************************************************************************
*���ܣ���ȡMODBUS��Э��š�
*������Э��������жϴ�����������ڻ��Ǵ���,Э���Ϊ����ʱ��
       ��ʾʹ�����ڣ���Э���Ϊż��ʱ����ʾʹ�ô��ڡ�
*���룺
*      �գ�
*���أ�
*      �̶�ֵ��0x69
*
*******************************************************************************/
EXPDLL int getPtclID()
{
    return P_MODBUS_TCP;
}




/******************************************************************************
*���ܣ�����������
*��������������������Ĭ�ϵĻỰ���á�
*���룺
*     srvNo      ���Ự�ţ�MODBUSֻ֧��һ�ֻỰ��
*     usingMode  : ģʽ�룬�̶�ΪONLY_ONE����0
*��� :
*      stnSnnCfg_p: Ĭ�ϻỰ����֡������Ϣ
*      ptclFunDsc : Ĭ�ϻỰ����Ӧ��cot,tid����
*���أ�
*      FUN_RL     �� Ĭ�ϲ�����
*
******************************************************************************/
EXPDLL FUN_RL* createRl_P ( u_char srvNo,
                            u_char usingMode,
                            T_snnCfg* snnCfg_p,
                            T_ptclFunDsc* ptclFunDsc_p
                          )
{
    if (   snnCfg_p == NULL
           || ptclFunDsc_p == NULL
       )
    {
        return NULL;
    }

    if ( ! ( ASSERTMBFC ( srvNo ) ) )
    {
        //���ڶ���ĻỰ�������룩��Χ��
        return NULL;
    }

    //���������
    FUN_RL* Fun_p = new FUN_RL();

    if (   srvNo == MC_MBFC_0xFE /*����һ��ͨ��*/
           || srvNo == MC_MBFC_0xFF /*��һ��ͨ��*/
       )
    {
#ifndef INTER_CREATERL_VAL

        setFunRl ( srvNo,
                   usingMode,
                   MB_REQ_F,
                   snnCfg_p,
                   Fun_p
                 );
#endif//#ifndef INTER_CREATERL_VAL

#ifdef INTER_CREATERL_VAL

        //�Թ������ģʽ�����У��
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

        ptclFunDsc_p->usingMode = usingMode;

        u_char stepNum = 2;//����

        //1������Ĭ�ϵĹ���
        Fun_p->srvNo              = srvNo;
        Fun_p->usingMode          = ONLY_ONE;
        Fun_p->cmLvl[ONLY_ONE]    = APP_LVL; // MODBUS��ԼȡAPP_LVL
        Fun_p->stepNum[ONLY_ONE]  = stepNum;
        //��һ��
        ONE_STEP* one_step_p     = new ONE_STEP();
        setONE_STEP ( one_step_p,
                      1,
                      MB_REQ_F,
                      1,
                      NULL,
                      MB_REQ_F,
                      0,
                      NULL,
                      NULL
                    );
        ONE_STEP* two_step_p    = new ONE_STEP();
        setONE_STEP ( two_step_p,
                      2,
                      MB_REQ_F,
                      0,
                      NULL,
                      MB_REQ_F,
                      1,
                      NULL,
                      NULL
                    );
        two_step_p->nextStep_p = NULL;
        addNode ( one_step_p, two_step_p );

        Fun_p->steps_p[ONLY_ONE] = one_step_p;

        //2���ԻỰ����Ĭ��֡���� T_snnCfg* stnSnnCfg_p
        u_short cmnAddr = 1;
        u_char bodySetNum = 0;
        //����MBAP����ͷ 7�ֽ�
        ADU_MBAP_VAL tMbapVal = {};
        tMbapVal.trans_id    = 0x01;
        tMbapVal.pctl_id     = 0x0;
        tMbapVal.length      = 0x3;
        tMbapVal.unit_id     = 0x01;

        snnCfg_p->ssnID     = srvNo;
        snnCfg_p->infoType  = GINFOTYPE;
        snnCfg_p->cmnAddr   = cmnAddr;
        snnCfg_p->stepNum   = stepNum;
        snnCfg_p->stepCfg_p = new T_ssnStepCfg[stepNum]();

        T_stnAcsFlagM tmpStnAcsFlag = {0, 0, 0, 0};
        //�Ե�һ��
        //[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻
        //λ2 = 1�洢��ϵͳִ������ֵ�ļ���
        tmpStnAcsFlag.ctlType = 0;
        snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
        //��һ������֡
        u_char oneReqAPDUNum = 1;//��һ��������֡����
        snnCfg_p->stepCfg_p[0].reqAPDUNum = oneReqAPDUNum;
        snnCfg_p->stepCfg_p[0].reqBody_setNum_p = (
                new u_char[oneReqAPDUNum]() );
        snnCfg_p->stepCfg_p[0].reqBody_setNum_p[0] = bodySetNum;
        snnCfg_p->stepCfg_p[0].reqEleVal_pp =  (
                                               new ELE_TYPE[oneReqAPDUNum][11]() );

        u_char memVal[ADU_CMTBL_NUM] = {};
        u_char tmpData_p[256] = {}; //���ڴ洢PDU�������������
        ELE_TYPE eleVal[ADU_CMTBL_NUM] =
        {
            {ADU_MBAPH_INDEX, 7, ( u_char* )& tMbapVal},
            //mbap����ͷ��ġ���Ԫ��ʶ����1�ֽ� ���õ� ADU_ADDR_INDEX ��
            {ADU_ADDR_INDEX, 0, &memVal[1]},
            {PDU_FC_INDEX, 1, &memVal[2]},
            {PDU_DATA_INDEX, 0, tmpData_p},
            {ADU_ECHK_INDEX, 0, &memVal[4]}
        };

        //���ýӿڸ�snnCfg_p��һ������֡�ĵ�1֡��ֵ
        //�̶���������Ϊ
        * ( eleVal[PDU_FC_INDEX].p_Mem ) = srvNo;
        //PDU������
        tmpData_p[0] = 0;
        tmpData_p[1] = 0;
        tmpData_p[2] = 0;
        tmpData_p[3] = 0;
        // ����ADU�С������������ݵ��ֽ�����
        eleVal[PDU_DATA_INDEX].lng = 0;
        // ����ADU�С������򡱵���ֵ�������ڴ�վ��ַ��
        // ����������ݵ��ֽ���֮��
        tMbapVal.length = 3;
        setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
        snnCfg_p->stepCfg_p[0].respAPDUNum = 0;
        //[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻
        //λ2 = 1�洢��ϵͳִ������ֵ�ļ���
        tmpStnAcsFlag.ctlType = 0;
        snnCfg_p->stepCfg_p[1].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
        //�ڶ���������֡
        snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
        //�ڶ�����Ӧ
        u_char oneRespAPDUNum = 1;  //��һ������Ӧ֡����
        snnCfg_p->stepCfg_p[1].respAPDUNum = oneRespAPDUNum;
        snnCfg_p->stepCfg_p[1].respBody_setNum_p = (
                new u_char[oneRespAPDUNum]() );
        snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] = bodySetNum;
        snnCfg_p->stepCfg_p[1].respEleVal_pp =  (
                                                new ELE_TYPE[oneRespAPDUNum][11]() );
        //���ýӿڸ�snnCfg_p��һ������Ӧ�ĵ�1֡��ֵ
        * ( eleVal[PDU_FC_INDEX].p_Mem ) = srvNo; //������̶�
        memset ( tmpData_p, 0, sizeof ( tmpData_p ) );
        eleVal[PDU_DATA_INDEX].lng = 0;
        tMbapVal.length = 3;
        setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

        //3������Ĭ�ϵĲ�����tid,cot�Ķ�Ӧ��ϵ
        ptclFunDsc_p->ssnDscNo                = srvNo;
        ptclFunDsc_p->act_prio_chnlID         = ACTIVE_TYPE;
        ptclFunDsc_p->usingMode               = usingMode;
        ptclFunDsc_p->setNum                  = stepNum;//stepNum=2
        ptclFunDsc_p->remoteSet_p             = new ptclStep[stepNum]();
        ptclFunDsc_p->remoteSet_p[0].TID_p    = new u_short();
        ptclFunDsc_p->remoteSet_p[0].TID_p[0] = srvNo;
        ptclFunDsc_p->remoteSet_p[0].COT      = MODBUS_COT_ACT;
        ptclFunDsc_p->remoteSet_p[0].MaxLen   = MAX104_ASDU_LNG;
        ptclFunDsc_p->remoteSet_p[0].TidNum   = 1;
#endif//#ifdef INTER_CREATERL_VAL

    }
    else //������Ự�������룩
    {
#ifndef INTER_CREATERL_VAL

        setFunRl ( srvNo,
                   usingMode,
                   MB_REQ_F,
                   snnCfg_p,
                   Fun_p
                 );
#endif//#ifndef INTER_CREATERL_VAL 

#ifdef INTER_CREATERL_VAL

        //�Թ������ģʽ�����У��
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

        ptclFunDsc_p->usingMode = usingMode;

        u_char stepNum = 2;//����

        //1������Ĭ�ϵĹ���
        Fun_p->srvNo              = srvNo;
        Fun_p->usingMode          = ONLY_ONE;
        Fun_p->cmLvl[ONLY_ONE]    = APP_LVL; // MODBUS��ԼȡAPP_LVL
        Fun_p->stepNum[ONLY_ONE]  = stepNum;
        //��һ��
        ONE_STEP* one_step_p     = new ONE_STEP();
        setONE_STEP ( one_step_p,
                      1,
                      MB_REQ_F,
                      1,
                      NULL,
                      MB_REQ_F,
                      0,
                      NULL,
                      NULL
                    );
        ONE_STEP* two_step_p    = new ONE_STEP();
        setONE_STEP ( two_step_p,
                      2,
                      MB_REQ_F,
                      0,
                      NULL,
                      MB_REQ_F,
                      1,
                      NULL,
                      NULL
                    );
        two_step_p->nextStep_p = NULL;
        addNode ( one_step_p, two_step_p );

        Fun_p->steps_p[ONLY_ONE] = one_step_p;

        //2���ԻỰ����Ĭ��֡���� T_snnCfg* stnSnnCfg_p
        u_short cmnAddr = 1;
        u_char bodySetNum = 1;
        //����MBAP����ͷ 7�ֽ�
        ADU_MBAP_VAL tMbapVal = {};
        tMbapVal.trans_id    = 0x01;
        tMbapVal.pctl_id     = 0x0;
        tMbapVal.length      = 0x4;
        tMbapVal.unit_id     = 0x01;

        snnCfg_p->ssnID     = srvNo;
        snnCfg_p->infoType  = GINFOTYPE;
        snnCfg_p->cmnAddr   = cmnAddr;
        snnCfg_p->stepNum   = stepNum;
        snnCfg_p->stepCfg_p = new T_ssnStepCfg[stepNum]();

        T_stnAcsFlagM tmpStnAcsFlag = {0, 0, 1, 0};
        //�Ե�һ��
        //[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻
        //λ2 = 1�洢��ϵͳִ������ֵ�ļ���
        tmpStnAcsFlag.ctlType = 0;
        snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
        //��һ������֡
        u_char oneReqAPDUNum = 1;//��һ��������֡����
        snnCfg_p->stepCfg_p[0].reqAPDUNum = oneReqAPDUNum;
        snnCfg_p->stepCfg_p[0].reqBody_setNum_p = (
                new u_char[oneReqAPDUNum]() );
        snnCfg_p->stepCfg_p[0].reqBody_setNum_p[0] = bodySetNum;
        snnCfg_p->stepCfg_p[0].reqEleVal_pp =  (
                                               new ELE_TYPE[oneReqAPDUNum][11]() );

        u_char memVal[ADU_CMTBL_NUM] = {};
        u_char tmpData_p[256] = {}; //���ڴ洢PDU�������������
        ELE_TYPE eleVal[ADU_CMTBL_NUM] =
        {
            {ADU_MBAPH_INDEX, 7, ( u_char* )& tMbapVal},
            //mbap����ͷ��ġ���Ԫ��ʶ����1�ֽ� ���õ� ADU_ADDR_INDEX ��
            {ADU_ADDR_INDEX, 0, &memVal[1]},
            {PDU_FC_INDEX, 1, &memVal[2]},
            {PDU_DATA_INDEX, 1, tmpData_p},
            {ADU_ECHK_INDEX, 0, &memVal[4]}
        };

        //���ýӿڸ�snnCfg_p��һ������֡�ĵ�1֡��ֵ
        //����һ��Ĭ�ϵĹ���֡���ݣ���������Ϊ01
        * ( eleVal[PDU_FC_INDEX].p_Mem ) = 0x03;
        tmpData_p[0] = 1; //��ʼ��ַ2���ֽ� tmpData_p[0] tmpData_p[1]
        tmpData_p[1] = 0;
        tmpData_p[2] = 20; //��Ȧ����2���ֽ� tmpData_p[2] tmpData_p[3]
        tmpData_p[3] = 0;
        // ����ADU�С������������ݵ��ֽ�����
        eleVal[PDU_DATA_INDEX].lng = 4;
        // ����ADU�С������򡱵���ֵ�������ڴ�վ��ַ��
        // ����������ݵ��ֽ���֮��
        tMbapVal.length = 6;
        setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
        snnCfg_p->stepCfg_p[0].respAPDUNum = 0;
        //[1,2] λ1 = 1�洢��ϵͳִ������ֵ�ı��棻
        //λ2 = 1�洢��ϵͳִ������ֵ�ļ���
        tmpStnAcsFlag.ctlType = 2;
        snnCfg_p->stepCfg_p[1].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
        //�ڶ���������֡
        snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
        //�ڶ�����Ӧ
        u_char oneRespAPDUNum = 1;  //��һ������Ӧ֡����
        snnCfg_p->stepCfg_p[1].respAPDUNum = oneRespAPDUNum;
        snnCfg_p->stepCfg_p[1].respBody_setNum_p = (
                new u_char[oneRespAPDUNum]() );
        snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] = bodySetNum;
        snnCfg_p->stepCfg_p[1].respEleVal_pp =  (
                                                new ELE_TYPE[oneRespAPDUNum][11]() );
        //���ýӿڸ�snnCfg_p��һ������Ӧ�ĵ�1֡��ֵ
        * ( eleVal[PDU_FC_INDEX].p_Mem ) = 3; //��������Ϊ01
        memset ( tmpData_p, 0, sizeof ( tmpData_p ) );
        tmpData_p[0] = 2; //�ֽ���1�����ֽ� tmpData_p[0]
        tmpData_p[1] = 1; //��Ȧ״̬N���ֽڣ�����N��ֵΪ tmpData_p[0]��ֵ
        tmpData_p[2] = 1;
        tmpData_p[3] = 1;
        tmpData_p[4] = 1;
        tmpData_p[5] = 15;
        tmpData_p[6] = 16;
        tmpData_p[7] = 17;
        tmpData_p[8] = 18;
        tmpData_p[9] = 19;
        tmpData_p[10] = 20;
        eleVal[PDU_DATA_INDEX].lng = 20;
        tMbapVal.length = 5;
        setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

        //3������Ĭ�ϵĲ�����tid,cot�Ķ�Ӧ��ϵ
        ptclFunDsc_p->ssnDscNo                = srvNo;
        ptclFunDsc_p->act_prio_chnlID         = ACTIVE_TYPE;
        ptclFunDsc_p->usingMode               = usingMode;
        ptclFunDsc_p->setNum                  = stepNum;//stepNum=2
        ptclFunDsc_p->remoteSet_p             = new ptclStep[stepNum]();
        ptclFunDsc_p->remoteSet_p[0].TID_p    = new u_short();
        ptclFunDsc_p->remoteSet_p[0].TID_p[0] = MODBUS_TID;
        ptclFunDsc_p->remoteSet_p[0].COT      = MODBUS_COT_ACT;
        ptclFunDsc_p->remoteSet_p[0].MaxLen   = MAX104_ASDU_LNG;
        ptclFunDsc_p->remoteSet_p[0].TidNum   = 1;
#endif//INTER_CREATERL_VAL

    }//end ������Ự�������룩

    return Fun_p ;
}




/*******************************************************************************
 *���� : �ͷ�ONE_STEP����ռ�
 *���� : ONE_STEP����ָ��
 *��� : ��
 *���� : 0 �ɹ�
 *�޸���ʷ
 *     ����104��Լ����������Ķ�
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
        pSpace->reqAPDU_Obj_p = NULL;
    }

    if ( pSpace->respAPDU_Obj_p != NULL )
    {
        delete [] pSpace->respAPDU_Obj_p;
        pSpace->respAPDU_Obj_p = NULL;
    }

    if ( pSpace->sVerifyBlock_p != NULL )
    {
        if ( pSpace->sVerifyBlock_p->chkRslt_p != NULL )
        {
            delete pSpace->sVerifyBlock_p->chkRslt_p;
            pSpace->sVerifyBlock_p->chkRslt_p = NULL;
        }

        delete pSpace->sVerifyBlock_p;
        pSpace->sVerifyBlock_p = NULL;
    }

    //�����ONE_STEP* nextStep��Ա��ݹ���һONE_STEP��Ա�ڵ�
    if ( pSpace->nextStep_p != NULL )
    {
        freeOneStep ( pSpace->nextStep_p );
    }

    delete pSpace;
    pSpace = NULL;

    return 0;
}




/*******************************************************************************
 *���� : �ͷ�FUN_RL����ռ�
 *���� : FUN_RL����ָ��
         T_snnCfg ����ָ��
         T_ptclFunDsc ����ָ��
 *��� : ��
 *���� : 0���ɹ�
 *�޸���ʷ
 *      ����104��Լ����������Ķ�
*******************************************************************************/
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

        delete pSpace;
        pSpace = NULL;
    }

#ifdef INTER_CREATERL_VAL

    if ( pSnnCfg != NULL ) //T_snnCfg�ͷ�
    {
        T_ssnStepCfg* tStepCfg_p = pSnnCfg->stepCfg_p;

        if ( tStepCfg_p != NULL )
        {
            for ( u_int j = 0; j < pSnnCfg->stepNum; j++ )
            {
                if ( tStepCfg_p[j].reqBody_setNum_p != NULL )
                {
                    delete [] tStepCfg_p[j].reqBody_setNum_p;
                }

                //�ͷ�����֡�ռ�
                if ( tStepCfg_p[j].reqEleVal_pp != NULL )
                {
                    for ( u_int k = 0; k < tStepCfg_p[j].reqAPDUNum; k++ )
                    {
                        for ( u_int k1 = APDU_APCI_INDEX;
                              k1 <= ASDU_TSP_INDEX;
                              k1++
                            )
                        {
                            u_char* p_Mem = (
                                            tStepCfg_p[j].reqEleVal_pp[k][k1].p_Mem );

                            if ( p_Mem != NULL )
                            {
                                delete [] p_Mem;
                            }
                        }
                    }

                    delete [] tStepCfg_p[j].reqEleVal_pp;
                }

                if ( tStepCfg_p[j].respBody_setNum_p != NULL )
                {
                    delete [] tStepCfg_p[j].respBody_setNum_p;
                }

                //�ͷ���Ӧ֡�ռ�
                if ( tStepCfg_p[j].respEleVal_pp != NULL )
                {
                    for ( u_int t = 0; t < tStepCfg_p[j].respAPDUNum; t++ )
                    {
                        for ( u_int t1 = APDU_APCI_INDEX;
                              t1 <= ASDU_TSP_INDEX;
                              t1++
                            )
                        {
                            u_char* p_Mem = (
                                            tStepCfg_p[j].respEleVal_pp[t][t1].p_Mem );

                            if ( p_Mem != NULL )
                            {
                                delete [] p_Mem;
                            }
                        }
                    }

                    delete [] tStepCfg_p[j].respEleVal_pp;
                }
            }//end for stepNum

            delete [] tStepCfg_p;
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

        //delete [] ptclFunDsc;
        //ptclFunDsc ���� createRl_P �д��������Բ��ڴ��ͷ�
    }

#endif//#ifdef INTER_CREATERL_VAL

    return vRet;
}




/*******************************************************************************
 *���� : ͨ���Ự�Ĳ�������Ϣ�����칦�����������
 *���룺
 *       stnSnnCfg_p��վ�Ự���ã��Ự��ͨ����Ϣ���б����洢��ַ��
 *       stepNo:����
 *�����
 *       stnSnnPara_p: ����������Ҫ���صĵ��ַ��ʱ���Э��Ҫ��
                       �����ɴ洢�����ADU��������Ԫ��ֵ
 *
 *����ֵ��ִ��״̬��=0�ɹ���<0 ʧ��
 *�޸���ʷ
 *
*******************************************************************************/
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

    int setNum = 0;//ͳ������֡����Ϣ��ĸ���
    //ADU_MBAP_VAL tMbapVal = {}; //MBAP����ͷ 7�ֽ�
    T_mbsID       tMbsId = {};
    T_ssnStepCfg* tStepCfg_p = & ( stnSnnCfg_p->stepCfg_p[stepNo - 1] );
    T_snnUsrData* tUsrData_p = stnSnnPara_p->usrData_p + setNum;

    if ( stnSnnPara_p->actFlag == ACTIVE_TYPE )
    {
        stnSnnPara_p->pntNum     = 0;
        stnSnnPara_p->pntPos     = 0;
        stnSnnPara_p->stepNo     = stepNo;
        stnSnnPara_p->infoType   = stnSnnCfg_p->infoType;
        stnSnnPara_p->cmnAddr    = stnSnnCfg_p->cmnAddr;
        stnSnnPara_p->stnAcsFlag = tStepCfg_p->stnAcsFlag;
        stnSnnPara_p->ASDUIDNum  = tStepCfg_p->reqAPDUNum;

        //������������֡
        for ( int i = 0; i < tStepCfg_p->reqAPDUNum; i++ )
        {
            if ( tStepCfg_p->comReqData_p == NULL
                 /*|| tStepCfg_p->reqEleVal_pp[i] ==NULL */
               )
            {
                return ERR_POINTER_NULL;
            }

            T_modbusFrame* tModbusFrame_p = & (
                                            tStepCfg_p->comReqData_p->modbus );

            /*��mbap����ͷ��������򣨹�4�ֽڣ�:
                ������:      1�ֽ�
                ��Ԫ��ʶ��:  1�ֽ�
                ����Ԫ��ʶ��:2�ֽ�
            ����T_snnPara�ṹ��ASDUID_p[i]����
            */
            u_char tmpFCode = tModbusFrame_p->pduFc;
            tMbsId.func_id  = tmpFCode;
            tMbsId.trans_id = tModbusFrame_p->mbapHeader.trans_id;
            tMbsId.unit_id  = tModbusFrame_p->mbapHeader.unit_id;

            stnSnnPara_p->ASDUID_p[i] = * ( ( u_int* ) &tMbsId );

            //��stnSnnCfg_p�еõ�PDU����
            PDU_DATA* pduData_p = & ( tModbusFrame_p->pduData );

            if ( tmpFCode != MC_MBFC_0xFE
                 && tmpFCode != MC_MBFC_0xFF
                 && pduData_p->lng == 0 )
            {
                return ERR_CONFIG;
            }

            if ( tmpFCode == MC_MBFC_0xFE    /*����һ��ͨ��*/
                 || tmpFCode == MC_MBFC_0xFF  /*��һ��ͨ��*/
               )
            {
                tUsrData_p->gData.pntType            = g_FCMDBUSLNG[tmpFCode].byteLng;
                tUsrData_p->gData.val_chkStd.pntAddr = 0;
                tUsrData_p->gData.val_chkStd.pntVal1 = 0;
                setNum++;
                tUsrData_p = stnSnnPara_p->usrData_p + setNum;
            }
            else if ( tmpFCode >= 1 && tmpFCode <= 6 )
            {
                /*
                 * PDU�����������2�ֽ���ʼ��ַ+2�ֽڶ���д��������ֵ
                 */
                u_short reqNum  = 0;
                u_short reqAddr = 0;
                memcpy ( &reqAddr, pduData_p->p_Mem, 2 );
                memcpy ( &reqNum, pduData_p->p_Mem + 2, 2 );

                tUsrData_p->gData.pntType            = g_FCMDBUSLNG[tmpFCode].byteLng;
                tUsrData_p->gData.val_chkStd.pntAddr = reqAddr;
                tUsrData_p->gData.val_chkStd.pntVal1 = reqNum;
                setNum++;
                tUsrData_p = stnSnnPara_p->usrData_p + setNum;
            }
            else if ( 15 == tmpFCode    //д�����Ȧ
                      || 16 == tmpFCode  //д����Ĵ���
                    )
            {
                /*
                 * PDU�����������2�ֽ���ʼ��ַ+2�ֽ����������Ĵ�������
                                   +1�ֽ��ֽ���+����ֵN�ֽ�
                 */
                u_short reqNum   = 0;
                u_short reqAddr  = 0;
                u_char byteNum   = 0;
                u_char* reqVal_p = NULL;

                memcpy ( &reqAddr, pduData_p->p_Mem, 2 );
                memcpy ( &reqNum, pduData_p->p_Mem + 2, 2 );
                memcpy ( &byteNum, pduData_p->p_Mem + 2 + 2, 1 );
                reqVal_p = pduData_p->p_Mem + 2 + 2 + 1;

                tUsrData_p->gData.pntType            = g_FCMDBUSLNG[tmpFCode].byteLng;
                tUsrData_p->gData.val_chkStd.pntAddr = reqAddr;
                tUsrData_p->gData.val_chkStd.pntVal1 = reqNum;
                tUsrData_p->gData.val_chkStd.pntVal2 = byteNum;
                setNum++;
                tUsrData_p = stnSnnPara_p->usrData_p + setNum;

                for ( u_int j = 0; j < byteNum; j++ )
                {
                    tUsrData_p->gData.pntType = g_FCMDBUSLNG[tmpFCode].byteLng;
                    memcpy ( & ( tUsrData_p->gData.val_chkStd.pntVal1 ),
                             reqVal_p + j,
                             1
                           );
                    setNum++;
                }
            }
            else if ( 20 == tmpFCode  //���ļ���¼
                      || 21 == tmpFCode //д�ļ���¼
                    )
            {
                //�ƻ���ʵ��
            }
            else if ( 22 == tmpFCode ) //����д�Ĵ���
            {
                tUsrData_p->gData.pntType = g_FCMDBUSLNG[tmpFCode].byteLng;
                /*
                 * PDU������������ο���ַ��2�ֽڣ�+ And_Mask(2�ֽ�)
                                   + Or_Mask(2�ֽ�)
                 */
                memcpy ( & ( tUsrData_p->gData.val_chkStd.pntAddr ),
                         pduData_p->p_Mem,
                         2
                       );

                memcpy ( & ( tUsrData_p->gData.val_chkStd.pntVal1 ),
                         pduData_p->p_Mem + 2,
                         2
                       );
                memcpy ( & ( tUsrData_p->gData.val_chkStd.pntVal2 ),
                         pduData_p->p_Mem + 2 + 2,
                         2
                       );
                setNum++;
                tUsrData_p = stnSnnPara_p->usrData_p + setNum;
            }
            else if ( 23 == tmpFCode ) //��д����Ĵ���
            {
                /*
                 * PDU���������������ʼ��ַ��2�ֽڣ�+ ��������(2�ֽ�)
                                   + д����ʼ��ַ(2�ֽ�) + д��������2�ֽڣ�
                                   +д�ֽ�����1�ֽڣ�+ д�Ĵ���ֵ ��N���ֽڣ�
                 */

                u_short reqRAddr  = 0;
                u_short reqRNum   = 0;
                u_short reqWAddr  = 0;
                u_short reqWNum   = 0;
                u_char  reqWByte  = 0;
                u_char* reqWVal_p = NULL;

                u_char offsetNum = 0;//ƫ����

                memcpy ( &reqRAddr, pduData_p->p_Mem, 2 );
                offsetNum += 2;
                memcpy ( &reqRNum, pduData_p->p_Mem, 2 );
                offsetNum += 2;

                memcpy ( &reqWAddr, pduData_p->p_Mem + offsetNum, 2 );
                offsetNum += 2;
                memcpy ( &reqWNum, pduData_p->p_Mem + offsetNum, 2 );
                offsetNum += 2;
                memcpy ( &reqWByte, pduData_p->p_Mem + offsetNum, 1 );
                offsetNum += 1;
                reqWVal_p = pduData_p->p_Mem + offsetNum;

                tUsrData_p->gData.pntType            = g_FCMDBUSLNG[tmpFCode].byteLng;
                tUsrData_p->gData.val_chkStd.pntAddr = reqRAddr;
                tUsrData_p->gData.val_chkStd.pntVal1 = reqRNum;
                setNum++;
                tUsrData_p = stnSnnPara_p->usrData_p + setNum;

                tUsrData_p->gData.pntType            = g_FCMDBUSLNG[tmpFCode].byteLng;
                tUsrData_p->gData.val_chkStd.pntAddr = reqWAddr;
                tUsrData_p->gData.val_chkStd.pntVal1 = reqWNum;
                tUsrData_p->gData.val_chkStd.pntVal2 = reqWByte;
                setNum++;
                tUsrData_p = stnSnnPara_p->usrData_p + setNum;

                for ( u_int j = 0; j < reqWByte; j++ )
                {
                    tUsrData_p->gData.pntType = g_FCMDBUSLNG[tmpFCode].byteLng;
                    memcpy ( & ( tUsrData_p->gData.val_chkStd.pntVal1 ),
                             reqWVal_p + j,
                             1
                           );
                    setNum++;
                    tUsrData_p = stnSnnPara_p->usrData_p + setNum;
                }
            }
            else if ( 43 == tmpFCode ) //���豸ʶ����
            {
                /*�ݲ�ʵ��*/
            }

            /*if( setNum != tStepCfg_p->reqBody_setNum_p[i] )
            {
                return ERR_CONFIG;
            }*/

            stnSnnPara_p->pntNum += setNum;
        }
    }
    else//����վ
    {
        //modbus �����ҵķ����ݲ����ڱ���վ��Ҫ����getGLoadPnt�����
        /*modbus ��Ƚϼ򵥣�һ�ʣ�һ�𣩣�����ֻ���ֵ�һ����ʵ�֣�
             ������վ��������ʱ����getGLoadPnt����
         �������ݵ��û����ݽṹT_snnPara�У�����վ���յ�����վ������
             ��֡���ݣ��������ݺϷ���
         ֱ�Ӹ�������֡�����ݣ������ַ�������ȣ�������Ӧ֡����
        */
        T_stnAcsFlagM tmpStnAcsFlag = {3, 0, 1, 4095};
        stnSnnPara_p->stnAcsFlag    = * ( u_short* ) &tmpStnAcsFlag;
        u_char tmpFCode             = 0;
        T_mbsID       tMbsId        = {};
        tMbsId   = * ( ( T_mbsID* ) & ( stnSnnPara_p->ASDUID_p[0] ) );
        tmpFCode = tMbsId.func_id;

        if ( 23 == tmpFCode )
        {
            stnSnnPara_p->pntPos = 0;
            stnSnnPara_p->pntNum = stnSnnPara_p->pntLoadNum;
        }

        //      stnSnnPara_p->pntNum    = 0;
        //      stnSnnPara_p->pntPos    = 0;
        //stnSnnPara_p->stepNo   = stepNo;
        //stnSnnPara_p->infoType  = stnSnnCfg_p->infoType;
        //stnSnnPara_p->cmnAddr   = stnSnnCfg_p->cmnAddr;
        //stnSnnPara_p->stnAcsFlag   = tStepCfg_p->stnAcsFlag;
        //stnSnnPara_p->ASDUIDNum = tStepCfg_p->respAPDUNum;

        ////������������֡
        //for (int i = 0; i < tStepCfg_p->respAPDUNum; i++)
        //{
        //          if ( tStepCfg_p->comRespData_p ==NULL
        //              /*|| tStepCfg_p->respEleVal_pp[i] ==NULL */
        //              )
        //          {
        //              return ERR_POINTER_NULL;
        //          }
        //
        //T_modbusFrame* tModbusFrame_p = &(
        //    tStepCfg_p->comRespData_p->modbus );

        ///*��mbap����ͷ��������򣨹�4�ֽڣ�:
        //    ������:      1�ֽ�
        //    ��Ԫ��ʶ��:  1�ֽ�
        //    ����Ԫ��ʶ��:2�ֽ�
        //����T_snnPara�ṹ��ASDUID_p[i]����
        //*/
        //u_char tmpFCode = tModbusFrame_p->pduFc;
        //tMbsId.func_id  = tmpFCode;
        //tMbsId.trans_id = tModbusFrame_p->mbapHeader.trans_id;
        //tMbsId.unit_id  = tModbusFrame_p->mbapHeader.unit_id;

        //stnSnnPara_p->ASDUID_p[i] = *((u_int*)&tMbsId);

        ////��stnSnnCfg_p�еõ�PDU����
        //         PDU_DATA * pduData_p = &( tModbusFrame_p->pduData );
        /*if( tmpFCode != MC_MBFC_0x00
        && tmpFCode != MC_MBFC_0xFF
        && pduData_p->lng == 0 )
        {
        return ERR_CONFIG;
        }*/
        //if ( tmpFCode == MC_MBFC_0x00    /*����һ��ͨ��*/
        //    || tmpFCode == MC_MBFC_0xFF  /*��һ��ͨ��*/
        //    )
        //{
        //    tUsrData_p->gData.pntType = g_FCMDBUSLNG[tmpFCode].byteLng;
        //    tUsrData_p->gData.val_chkStd.pntAddr = 0;
        //    tUsrData_p->gData.val_chkStd.pntVal1 = 0;
        //    setNum++;
        //    tUsrData_p = stnSnnPara_p->usrData_p + setNum;
        //}
        //else if (tmpFCode>=1 && tmpFCode<=4)
        //          {
        //              /*
        //               * ��ӦPDU������������ֽ�����1�ֽ�)+״̬��ֵ��N�ֽڣ�
        //               */
        //              u_char respByteNum = 0;
        //              memcpy(&respByteNum ,pduData_p->p_Mem,1);
        //              tUsrData_p->gData.val_chkStd.pntVal1 = respByteNum;
        //              setNum++;
        //              tUsrData_p = stnSnnPara_p->usrData_p+setNum;

        //              for( u_int i=0;i<respByteNum; i++)
        //              {
        //                  memcpy( &(tUsrData_p->gData.val_chkStd.pntVal1),
        //                      pduData_p->p_Mem + setNum,
        //                      1
        //                      );
        //                  setNum++;
        //                  tUsrData_p = stnSnnPara_p->usrData_p+setNum;
        //              }
        //
        //          }
        //          else if ( 5==tmpFCode   //д������Ȧ
        //              ||6== tmpFCode  //д�����Ĵ���
        //              )
        //          {
        //              /*
        //               * ��ӦPDU�������������ַ��2�ֽ�)+ֵ��2�ֽڣ�
        //               */
        //              memcpy( &(tUsrData_p->gData.val_chkStd.pntAddr),
        //                  pduData_p->p_Mem,
        //                  2
        //                  );
        //              memcpy( &(tUsrData_p->gData.val_chkStd.pntVal1),
        //                  pduData_p->p_Mem + 2,
        //                  2
        //                  );
        //              setNum++;
        //              tUsrData_p = stnSnnPara_p->usrData_p+setNum;
        //          }
        //          else if (15==tmpFCode       //д�����Ȧ
        //                   || 16 == tmpFCode  //д����Ĵ���
        //                   )
        //          {
        //              /*
        //               * ��ӦPDU�������������ʼ��ַ��2�ֽ�)+������2�ֽڣ�
        //               */
        //              memcpy( &(tUsrData_p->gData.val_chkStd.pntAddr),
        //                  pduData_p->p_Mem,
        //                  2
        //                  );
        //              memcpy( &(tUsrData_p->gData.val_chkStd.pntVal1),
        //                  pduData_p->p_Mem + 2,
        //                  2
        //                  );
        //              setNum++;
        //              tUsrData_p = stnSnnPara_p->usrData_p+setNum;
        //
        //          }
        //          else if (20 == tmpFCode   //���ļ���¼
        //                   || 21== tmpFCode //д�ļ���¼
        //                  )
        //          {
        //              //�ƻ���ʵ��
        //          }
        //          else if ( 22==tmpFCode )//����д�Ĵ���
        //          {
        //              /*
        //               * ��ӦPDU������������ο���ַ��2�ֽڣ�+ And_Mask(2�ֽ�)
        //                                     + Or_Mask(2�ֽ�)
        //               */
        //              memcpy( &(tUsrData_p->gData.val_chkStd.pntAddr),
        //                  pduData_p->p_Mem,
        //                  2
        //                  );
        //              memcpy( &(tUsrData_p->gData.val_chkStd.pntVal1),
        //                  pduData_p->p_Mem + 2,
        //                  4
        //                  );
        //              setNum++;
        //              tUsrData_p = stnSnnPara_p->usrData_p+setNum;
        //          }
        //          else if ( 23==tmpFCode )//��д����Ĵ���
        //          {
        //              /*
        //               * ��ӦPDU������������ֽ�����1�ֽڣ�+ ���Ĵ���ֵ(N�ֽ�)
        //               */
        //              u_char respByteNum = 0;
        //              memcpy(&respByteNum ,pduData_p->p_Mem,1);
        //              tUsrData_p->gData.val_chkStd.pntVal1 = respByteNum;
        //              setNum++;
        //              tUsrData_p = stnSnnPara_p->usrData_p+setNum;

        //              for( u_int i=0;i<respByteNum; i++)
        //              {
        //                  memcpy( &(tUsrData_p->gData.val_chkStd.pntVal1),
        //                      pduData_p->p_Mem + setNum,
        //                      1
        //                      );
        //                  setNum++;
        //                  tUsrData_p = stnSnnPara_p->usrData_p+setNum;
        //              }
        //
        //          }
        //          else if ( 43==tmpFCode )//���豸ʶ����
        //          {
        //              /*�ݲ�ʵ��*/
        //          }

        //          /*if( setNum != tStepCfg_p->reqBody_setNum_p[i] )
        //          {
        //              return ERR_CONFIG;
        //          }*/
        //
        //          stnSnnPara_p->pntNum += setNum;
        //}


    }

    return 0;
}




/*******************************************************************************
  *���ܣ���������������ʼ֡��Ӧ�Ĳ����Ự�еĵ�һ֡�ŵ��ã�
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
         || statrtStepStnD_p->ASDUID_p == NULL
       )
    {
        return ERR_POINTER_NULL;
    }

    int tmpRet              = 0; //�²㺯������ֵ
    int nxtStepStat         = 0; //��һ֡�Ự״̬
    u_char nxtFrmStepNo     = 1; //��һ֡����
    ONE_STEP tmpNowRlStep   = {0};//��ʱ���򲽱���
    T_ptclFunDsc tmpFuncDsc = {0};//��ʱ�Ự���������Ա���

    preStepRul_p->stepNo = 0;
    tmpNowRlStep.stepNo  = 1;
    tmpFuncDsc.ssnDscNo  = ssnNo;

    u_char actFlag = statrtStepStnD_p->actFlag;

    if (  actFlag != ACTIVE_TYPE
          && actFlag != PASSIVE_TYPE
       )
    {
        return ERR_ACT_PAS_VAL;
    }

    if ( ! ( ASSERTMBFC ( ssnNo ) ) )
    {
        //���ڶ���ĻỰ�������룩��Χ��
        return ERR_FC_VAL_RAN;
    }

    //���ýӿ�asgmtNxtFrmPar����װ֡����
    tmpNowRlStep.reqPDU_FCode     = MB_REQ_F;
    tmpFuncDsc.act_prio_chnlID  = ACTIVE_TYPE;
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

    //��������ִ��
    nxtStepStat = MC_SEND_WAIT_CHECK;

    return nxtStepStat;

}




/*******************************************************************************
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
 *      >0           ������nxtStep������һ֡���ݵ���һ֡��Ӧ
                       �Ĳ���(����Ѿ������һ�����һ֡���
                       ֵΪ��ǰ֡��Ӧ�Ĳ��ţ�
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
    if (   NULL == funDsc_p
           || NULL == stepRl_p
       )
    {
        return ERR_POINTER_NULL;
    }

    u_char actFlag = ( u_char ) funDsc_p->act_prio_chnlID;

    if (   actFlag != ACTIVE_TYPE
           && actFlag != PASSIVE_TYPE
       )
    {
        return ERR_ACT_PAS_VAL;
    }

    if ( ! ( ASSERTMBFC ( funDsc_p->ssnDscNo ) ) )
    {
        //���ڶ���ĻỰ�������룩��Χ��
        return ERR_FC_VAL_RAN;
    }

    *nxtStepStat_p = 0xffff;       //��ʼ��Ϊ�ǻỰ״ֵ̬

    int                tmpRet = 0;
    u_char     nxtFrmStepNo   = 0;
    ONE_STEP* lastRuleStep_p  = NULL;
    ONE_STEP* nowRuleStep_p   = NULL; //��ǰ���Ĺ���ָ��
    ONE_STEP* nxtFrmRlStep_p  = NULL;//��һ֡��Ӧ�Ĺ���
    u_char tUsingMode         = Fun_p->usingMode;

    nxtFrmStepNo = stepRl_p->stepNo;

    //�ҵ��������һ��
    lastRuleStep_p = Fun_p->steps_p[tUsingMode];

    while ( lastRuleStep_p->nextStep_p != NULL )
    {
        lastRuleStep_p    = lastRuleStep_p->nextStep_p;
    }

    if ( stepRl_p->stepNo > lastRuleStep_p->stepNo )
    {
        *nxtStepStat_p = MC_NO_SEND_SSN_STOP; //�����ͻỰ��ֹ
        return nxtFrmStepNo;
    }

    //��ǰ���Ĺ���ָ��
    nowRuleStep_p = displayNextNode (
                    Fun_p->steps_p[tUsingMode],
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

    if ( stepRl_p->stepNo != nxtFrmStepNo )
    {
        nxtFrmRlStep_p = nowRuleStep_p->nextStep_p;
    }
    else
    {
        nxtFrmRlStep_p = nowRuleStep_p;
    }

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


    return nxtFrmStepNo;
}




/*******************************************************************************
  *���ܣ���ASDU
  *     ��104Э���У��˽ӿ������Ϊ�˽�����TID_p��COT_p,����Ľӿڸ���TID_p��COT_p
  *         �õ��Ự����Ự���������ԡ��Ự���ţ�
  *��modbusЭ���У��˽ӿ�û��ʵ�ʵ����壬��Ϊ��ֻ��֡�����������޷������������Ͳ��ţ�
  *
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
                     u_int* TID_p,
                     u_int* COT_p
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
        //104Э���д˴����߼��ǽ���U֡����ĻỰ,��Щ�Ự����I֡����֮ǰ��״̬����
        //modbus�����ڴ��߼�
        return ERR_LOGIC_RUN;
    }
    else
    {
        /*104Э���д˴����߼����Ȱ�APDU_p�󶨵�ȫ��104����
                Ȼ����������е�COT��TID���������ӿ� �жϵ�
                ǰ���ݴ���ʲô�Ự���ܺţ����Ự���ţ�*/

        //modbusЭ�� û�д˽ӿ��е��߼�����ʱ��һ��Ĭ�ϵĶ�ֵ
        //ȡ������
        u_char tFuncCode = APDU_p[7];
        //*TID_p = MODBUS_TID;
        *TID_p = tFuncCode;
        *COT_p = MODBUS_COT_ACT;

    }

    return 0 ;
}




/*******************************************************************************
  *���ܣ���ȡ���ܵĹ��ܺ�
  *       (104Э���У��˽ӿ������Ϊ�õ��Ự����Ự����������)
  *��modbusЭ���У��˽ӿ���modbus��û��ʵ�ʵ�����,��Ϊ��ֻ��֡
  *        �����������޷����������� ��������վ�Ŀ�ʼʱ��ȷ������104������)
  *
  *���룺
  *     TID��     ���ͱ�ʶ��(��modbus�д�ֵ�����ǹ�����)
  *     COT��     ����ԭ��
  *�����
  *     funDsc_p���Ự��������������
  *     funNum_p���������������funDsc_p����ʵ���±�ֵ��
  *���أ�
  *     Э���ţ���P_MODBUS_TCP
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

    //T_ptclFunDsc function[3] = {}; //����T_ptclFunDsc��a����
    *funNum_p = 0;

    //if ( TID == MODBUS_TID )
    if ( ASSERTMBFC ( TID ) ) //�жϹ������Ƿ��ڶ���ķ�Χ��
    {
        //funDsc_p[*funNum_p].ssnDscNo = QUE_DATA;
        funDsc_p[*funNum_p].ssnDscNo = ( u_short ) TID;
        //function[*funNum_p].act_prio_chnlID �����������޷�ȷ��
        ( *funNum_p ) ++;
    }

    //��function�����ֵ��������funDsc_p����
    //memcpy(funDsc_p,function,sizeof(T_ptclFunDsc)*(*funNum_p));

    return P_MODBUS_TCP;

}




/*******************************************************************************
  *���ܣ���ȡ�Ự���ţ������µ�ASDU����)
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


    if ( funDsc_p == NULL
         || funDsc_p->remoteSet_p == NULL
       )
    {
        return ERR_POINTER_NULL;
    }

    u_int stepNo = 0xffff;

    //�������U֡�����������Ự
    //if ( funDsc_p->ssnDscNo == U_STARTDT
    //    && funDsc_p->ssnDscNo == U_STOPDT
    //    && funDsc_p->ssnDscNo == U_TESTFR
    //    && funDsc_p->ssnDscNo == U_NEWHOLD
    //    && funDsc_p->ssnDscNo == U_DELHOLD
    //    )
    //{
    //    /*�˴��߼���104Э���������壬modbus���޴˴����߼�*/
    //}
    //else
    //{
    //    //modbus����Ϊֻ��һ��
    //    /*ptclStep* tmpPtclStep = funDsc_p->remoteSet_p;
    //    for (int i = 0;i < funDsc_p->setNum; i++)
    //    {
    //        if ( TID == (tmpPtclStep + i)->TID
    //            && COT == (tmpPtclStep + i)->COT
    //            )
    //        {
    //            stepNo = i + 1;
    //            break;
    //        }
    //    }*/
    //}
    if ( ( u_char ) funDsc_p->act_prio_chnlID == ACTIVE_TYPE ) //����վ
    {
        stepNo = 2;
    }
    else//����վ
    {
        stepNo = 1;
    }

    return stepNo;

}




/*******************************************************************************
  *���� ���ԻỰ��stepRl_p�е�֡�������գ����պ��֡���ݸ���ssnPar_p
  * ���룺
  *     Fun_p        :������
  *     stepRl_p     :��ǰ��
  *     funDsc_p     :�Ự����������
  *�����
  *     ssnPar_p     :���������˵��
  *     nxtStepNo_p  :��һ֡�Ĳ��ţ����һ�������һ֡ʱ���Ų��䣩
  *���أ�
  *  0               :�ɹ���
  *  0xffff          :�ù��ܲ�֧��
  *  ����            :ʧ��
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
    C_ModbusADU* mbap_p = NULL;

    if ( act_prio_chnlID == PASSIVE_TYPE )
    {
        if ( stepRl_p->respAPDU_Obj_p == NULL )
        {
            return ERR_POINTER_NULL;
        }

        mbap_p = ( C_ModbusADU* ) stepRl_p->respAPDU_Obj_p;

    }
    else//ACTIVE_TYPE
    {
        if ( stepRl_p->reqAPDU_Obj_p == NULL )
        {
            return ERR_POINTER_NULL;
        }

        mbap_p = ( C_ModbusADU* ) stepRl_p->reqAPDU_Obj_p;

    }

    int vRet    = 0;    //�����²㺯������ֵ
    //����chkRecData������ع��ܵ�У��
    vRet =  chkRecData ( nowRuleStep_p,
                         lastRuleStep_p,
                         stepRl_p->stepNo,
                         funDsc_p,
                         mbap_p
                       );

    if ( vRet != 0 )
    {
        return vRet;
    }

    /*����ֵ�ļ��ء������ʶ:
        λ0 = 1�洢��ϵͳִ������ֵ�ı��棻
        λ1 = 1�洢��ϵͳִ������ֵ�ļ��أ�
            ȫ = 0, do nothing��ȫ = 1�������ұ���
        λ3��ʾADUԪ��3��=1��ʾ��λ0��λ1�Ĳ����ڴ�
            Ԫ������Ч��=0Ϊ��Ч��
        */
    T_stnAcsFlagM tmpStnAcsFlag = {0, 0, 0, 4095};
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

    //����stepRl_p��C_MBAP����Ӧ���ݸ�ssnPar_p��ֵ
    vRet = asgmtSnnPar ( stepRl_p,
                         funDsc_p,
                         mbap_p,
                         &tmpStnAcsFlag,
                         ssnPar_p
                       );

    if ( vRet < 0 )
    {
        return vRet;
    }

    return 0;
}




/*******************************************************************************
  *���� ����PDU_pָ��󶨵�ONE_STEP������
  * ���룺
  *     stepRl_p  :�󶨵���Ŀ��ONE_STEP
  *     funDsc_p  :�������� ����
  *     PDU_p     :���󶨵ĵ�ַ
  *     PDUlng    :���ڴ�ĳ���
  *     duType    :�󶨵�ַ������
  *�����
  *     ��
  *���أ�
  *     0         :�ɹ���
  *     ����      :ʧ��
  *�޸ģ�
  *
 ******************************************************************************/
EXPDLL int bindToMem ( ONE_STEP* stepRl_p,
                       T_ptclFunDsc* funDsc_p,
                       u_char* PDU_p,
                       u_short PDUlng,
                       u_char duType
                     )
{
    C_ModbusADU* tMbap_p = NULL;
    //int asdu_BodySet = 0;
    u_char act_prio_chnlID = ( u_char ) funDsc_p->act_prio_chnlID;

    if ( act_prio_chnlID == PASSIVE_TYPE ) //����Ӧ֡
    {
        tMbap_p = ( C_ModbusADU* ) stepRl_p->respAPDU_Obj_p;
    }
    else //������֡
    {
        tMbap_p = ( C_ModbusADU* ) stepRl_p->reqAPDU_Obj_p;
    }

    ADU_DSCR* apdu_dscr_p = tMbap_p->getDU_Info();

    if ( duType == PDU_TYPE )
    {
        apdu_dscr_p->ADU_CMTBL[ADU_MBAPH_INDEX] = 0;
        apdu_dscr_p->ADU_CMTBL[ADU_ADDR_INDEX] = 0;
        //apdu_dscr_p->ADU_CMTBL[ADU_ECHK_INDEX] = 0;
    }

    //u_short tLng = *((u_short*)(PDU_p + 4));
    //u_short tPDULng = tLng - 1;
    //apdu_dscr_p->ADU_CMTBL[PDU_DATA_INDEX] = tPDULng;

    tMbap_p->bindToMem ( PDU_p, duType, PDUlng );

    if ( tMbap_p->getDU_Ini_Lng() < PDUlng )
    {
        return -1;//apdu���ȴ���
    }

    return 0;
}




/*******************************************************************************
  *���� �����C_ModbusADU�����ADU_p��PDU_p��ַ
  * ���룺
  *     apduObj_p :C_ModbusADU����
  *     duType    :��Ҫ��ȡ�����ͣ�adu��pdu)
  *�����
  *     apduLng   :ADU_p��PDU_p�ڴ������ݵĳ���
  *���أ�
  *     ADU_p��PDU_p��ַ
  *�޸ģ�
  *
 ******************************************************************************/
EXPDLL u_char* getDU ( u_char* apduObj_p,
                       u_short& apduLng,
                       char duType
                     )
{
    u_char* tDu_p = ( ( C_ModbusADU* ) apduObj_p )->getDU ( apduLng, duType );

    if ( tDu_p != NULL
         && ( tDu_p[7] == MC_MBFC_0xFE    /*����һ��ͨ��*/
              || tDu_p[7] == MC_MBFC_0xFF     /*��һ��ͨ��*/
            )
       )
    {
        //��104Э������u֡�ĳ��ȷ���0
        apduLng = 0;
    }

    return tDu_p;
}




/*******************************************************************************
*���ܣ�MODBUS�����е�ADU_p��PDU_p�ڴ������ݵĳ��ȡ�
*���룺
*      apduObj_p ����ָ��
*�����
*      ��
*���أ�
*      ADU_p��PDU_p�ڴ������ݵĳ���
*�޸���ʷ��
*
*******************************************************************************/
EXPDLL u_short getDULng ( u_char* apduObj_p )
{
    return ( ( C_ModbusADU* ) apduObj_p )->getDU_Lng();
}




/*******************************************************************************
*���ܣ����� ADU����֡�Ľṹ����
*���룺
*      apduObj_p ��C_ModbusADU����ָ��
*      apduDSCR  ��ADU����֡�Ľṹ����
*�����
*      ��
*���أ�
*      void
*�޸���ʷ��
*
*******************************************************************************/
EXPDLL void setAPDU_DSCR ( u_char* apduObj_p,
                           ADU_DSCR apduDSCR
                         )
{
    memcpy ( ( ( C_ModbusADU* ) apduObj_p )->getDU_Info(),
             &apduDSCR,
             sizeof ( ADU_DSCR )
           );
}




/*******************************************************************************
*���ܣ�MODBUS�����е�֡ͷ���ȣ���MBAP�ĳ��ȡ�
*���룺
*      void��
*�����
*      void��
*���أ�
*      u_char
*�޸���ʷ��
*
*******************************************************************************/
EXPDLL u_char getHeaderlng()
{
    return 7;
}




/*******************************************************************************
*���ܣ�ֻ��MODBUS��ADU����ת�����ֽ���,������MBAP�С����ȡ����ֵ��
*��������MODBUS�У���ͬ��ɫ��վ�㣬����֡ͷ�ķ�ʽ�ǲ�ͬ�ġ���վ��
*       Ҫ����֡ͷ�������򣬶���վֻ��Ҫ���ó�����
*       ����������������֡�е���Ӧ�Ķ���
*���룺
*      rtmParam_p   �� �գ�
*      cfgParam_p   �� �գ�
*      APDU104_p    �� MODBUSЭ���ADU����
*      ASDUFrm_Lng  :  PDU���ݵĳ���
*�����
*      APDU_buf_p   : ADU���ݵ��ֽ���
*      APDU_bufLng  : �ֽ����ĳ���
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
    // ��ȡMODBUS����-ADU
    C_ModbusADU* adu_p = ( C_ModbusADU* ) APDU104_p;
    u_char* PDUFrm_p = adu_p->getDU ( ASDUFrm_Lng, ADU_TYPE );

    if ( PDUFrm_p == NULL )
    {
        return ;
    }

    // ����ADU�����ݳ���
    APDU_bufLng = ( u_char ) ASDUFrm_Lng /*+ 7*/;

    // ����ADU����
    memcpy ( APDU_buf_p, PDUFrm_p, APDU_bufLng );

    //�޸�ADU֡ͷMBAP�С����ȡ���ʶ��ֵ
    u_short length = ASDUFrm_Lng - 6;

    //��С��ת��
    reversEndian ( ( u_char* ) &length, sizeof ( u_short ) );
    //MBAP=2�ֽ�����Ԫ��ʶ+2�ֽ�Э���ʶ+2�ֽڳ���+1�ֽڵ�Ԫ��ʶ
    memcpy ( APDU_buf_p + 4, &length, 2 );
    /*APDU_buf_p[4] = (length >> 8) & 0xFF;
    APDU_buf_p[5] = (length >> 0) & 0xFF;*/
}




/*******************************************************************************
*���ܣ������յ�����֡�Ƿ����MODBUS����֡�ĸ�ʽ�淶����ʱ
*      ֻʵ��TCP֡������֡��ʽδ���ǡ�
*���룺
*      rtmParam_p   �� �գ�
*      cfgParam_p   �� ��ʱΪ�գ�����ָ��MODBUS֡�ĸ�ʽ��
*      revDataFrm   �� ��������
*�����
*      APDU_buf_p   : ADU���ݵ��ֽ���
*      APDU_bufLng  : �ֽ����ĳ���
*���أ�
*      void    ��ADU���ݸ�ʽ��ȷʱ������ֵ�ĸ�8λ��ʾ֡���ͣ���8λΪ0��
*                ADU���ݸ�ʽ��ȷʱ������ֵ�ĸ�8λ��ʾ��Ҫ����Ķ�����
*                 ��8λΪ������룻
*�޸���ʷ��
*
*******************************************************************************/
EXPDLL u_short chkAPDUData ( revBuf* revDataFrm,
                             void* rtmParam_p,
                             void* cfgParam_p
                           )
{
    /*
    //���ò���cfgParam_pָ����MODBUS���Ľṹ
    u_char frame_mode = ((thresholdCfgPara*)cfgParam_p)->mode;
    //����ֵ�����ݽṹ
    frameType frmType = {0};

    if( frame_mode == MODBUS_ASCII )
    {
        //ʵ����ʱΪ��
        return *(u_short*)&frmType;
    }
    else if( frame_mode == MODBUS_RTU )
    {
        //ʵ����ʱΪ��
        return *(u_short*)&frmType;
    }
    else if( frame_mode == MODBUS_TCP )
    {
        //ADU����
        ADU_TCP_VAL* adu_p = (ADU_TCP_VAL*)revDataFrm->APDU_Buf.APDU_Data_p;

        //���Modbus����֡�е�Э���ʶ��
        if ( adu_p->pctl_id != PROTOCOL_MODBUS)
        {
            frmType.fTypeL = MC_UNKNOWN_MODBUS; // ��MODBUSЭ��
            frmType.fTypeH = NO_OPERATION;
            return *(u_short*)&frmType;
        }

    }
    else
    {
        frmType.fTypeL = MC_UNKNOWN_MODE; // δ����MODBUSЭ���֡��ʽ
        frmType.fTypeH = NO_OPERATION;
        return *(u_short*)&frmType;
    }

    */

    //����ֵ�����ݽṹ
    frameType frmType;
    frmType.fTypeL = MC_APDU_OK;
    frmType.fTypeH = I_F;

    //ADU����
    ADU_TCP_VAL* adu_p = ( ADU_TCP_VAL* ) revDataFrm->APDU_Buf.APDU_Data_p;

    //���Modbus����֡�е�Э���ʶ��
    if ( adu_p->pctl_id != PROTOCOL_MODBUS )
    {
        frmType.fTypeL = MC_UNKNOWN_MODBUS; // ��MODBUSЭ��
        frmType.fTypeH = NO_OPERATION;
        return * ( u_short* ) &frmType;
    }

    //���Modbus����֡�еĳ���
    u_short length = 0;
    //X86����С��ģʽ,��MODBUS���ݲ��ô��ģʽ
    //length =( ( adu_p->length & 0xFF ) << 8 )
    //        + ( ( adu_p->length >> 8 ) & 0xFF ) + 6;
    //if ( adu_p->length != revDataFrm->APDU_Buf.APDU_Lng )
    length = adu_p->length;
    reversEndian ( ( u_char* ) &length, sizeof ( u_short ) );
    length = length + 6;

    if ( length != revDataFrm->APDU_Buf.APDU_Lng )
    {
        frmType.fTypeL = MC_APDU_CHECKSUM;
        frmType.fTypeH = NO_OPERATION;
        return * ( u_short* ) &frmType;
    }

    return * ( u_short* ) &frmType;
}




/*******************************************************************************
*���ܣ�����֡ͷMBAP�еġ����ȡ���,����鷢�����ݵ������Ƿ������
*      ��MODBUSЭ���У�����ǿͻ��ˣ��������ݶˣ���
*      ��Ҫ���书�ܣ��Ա�˶Է���˷��ص�Ӧ��֡����������ǰ��MODBUS
*      ����Ƿ���Խ����ݼ��뵽�洢�����С�
*      ����Ƿ���ˣ���Ӧ���ݣ����Ͳ���Ҫ���书�ܣ������ݷ������ɡ�
*���룺
*      role        �� վ���ɫ��
*      rtmParam_p  �� ʵʱ����������MBAP֡ͷ�е����ﴦ���ʶ��
*      data_p      :  ADU����
*      length      :  ADU���ݳ���
*�����
*      void
*���أ�
*      u_int       :  ��16λ���غ���ִ�еĽ����Ϣ��������Ϣ����
*                     ��16λ���ؽ����Ϣ��������Ϣ���Ľ������
*ע�����
*
*******************************************************************************/
EXPDLL u_int CheckSendState ( short role,
                              void* rtmParam_p,
                              u_char* data_p,
                              u_short length
                            )
{
    //����ֵ
    sandMotion result = {0};

    // ����վ, �ͻ��ˣ���������֡��
    if ( role == MASTER_STATION )
    {
        //��¼����֡�еĹ���������ﴦ���ʶ���Ա�˶Է���˷��ص���Ӧ֡
        T_mbsChkNode nodeData;
        T_mbsChkData* chkData_p = NULL;
        //ͨ�������ڴ洢���ݵĿռ�
        chkData_p = ( T_mbsChkData* ) & (
                    ( ( realTimeValue* ) rtmParam_p )->chkData );
        // ���ﴦ���ʶ
        nodeData.trans_id = ( ( realTimeValue* ) rtmParam_p )->sendnum;
        nodeData.func_id  = data_p[7];   // ������

        // ����MBAP�е����ﴦ���ʶ, MODBS����Big-Endianģʽ
        /*data_p[0] = ( nodeData.trans_id >> 8 ) & 0xFF;
        data_p[1] = ( nodeData.trans_id >> 0 ) & 0xFF;*/
        memcpy ( data_p, & ( nodeData.trans_id ), 2 );
        reversEndian ( data_p, 2 );


        // ����MBAP�е�Э���ʶ
        data_p[2] = 0x00;
        data_p[3] = 0x00;

        /*
        fusk add@20171218130936 chkData_p->nex_index = 0
        ע��˲���ǰ�������modbusЭ���һ��һ���������ʽ��
        ���ģ�����ϲ���û�еȵ���վ��Ӧ��û�г�ʱ�ٴ��ط���
        ��֡��������ǲ������
        ��Ҫ�ϲ���Ʋ�����
        */
        chkData_p->nex_index = 0;

        // ���ȴ���Ӧ֡�������Ƿ��пռ䡣��������֡ʱ���������
        //���յ���Ӧ����Ӧ֡ʱ���Ӹ�������ȥ����
        if ( addSearData ( ( void* ) chkData_p, ( void* ) &nodeData ) != 0 )
        {
            result.errType = SEND_STATUS_ERROR;
            result.motion  = NO_OPERATION;
        }
        else
        {
            result.errType = SEND_READY_STATUS;
            result.motion  = WRITE_CONNECT;
        }
    }
    // ����վ������ˣ�������Ӧ֡��
    else if ( role == SLAVE_STATION )
    {
        // ��Ӧ֡MBAP�С����ȡ�����packAPDU(��
        //���ã��������ɿ���ϲ���ղ��Լ������
        result.errType = SEND_READY_STATUS;
        result.motion  = WRITE_CONNECT;
    }
    // �Ƿ���״̬��״̬���в����ڵ�״̬��
    else
    {
        result.errType = SEND_ROLE_ERROR;
        result.motion  = NO_OPERATION;
    }

    return * ( u_int* ) &result;
}




/*******************************************************************************
*���ܣ�����������֡�Ĺ���������ﴦ���ʶ�Ƿ�ͱ�վ����������
*      ֡һ�¡���MODBUSЭ���У�ֻ�пͻ��ˣ��������ݶˣ���
*     ����Ҫ���������ݣ�������ˣ���Ӧ���ݣ��Ͳ���Ҫ������ݡ�
*     �ϲ㹹������ʱ��ֱ�Ӹ���֡ͷ���ɡ�
*���룺
*      role        �� վ���ɫ��
*      rtmParam_p  �� ʵʱ����������MBAP֡ͷ�е����ﴦ���ʶ��
*      data_p      :  ADU����
*      length      :  ADU���ݳ���
*�����
*      void
*���أ�
*      u_int       :  ��16λ���غ���ִ�еĽ����Ϣ��������Ϣ����
*                     ��16λ���ؽ����Ϣ��������Ϣ���Ľ������
*ע�⣺
*
*******************************************************************************/
EXPDLL u_int CheckRecvState ( short role,
                              void* rtmParam_p,
                              u_char* data_p,
                              u_short length
                            )

{
    //����ֵ
    sandMotion result = {0};

    // ����վ, �ͻ��ˣ����յ���Ӧ֡��
    if ( role == MASTER_STATION )
    {
        //��¼����֡�еĹ���������ﴦ���ʶ��
        //�Ա�˶Է���˷��ص���Ӧ֡
        T_mbsChkData* chkData_p = NULL;
        //ͨ�������ڴ洢���ݵĿռ�
        chkData_p = ( T_mbsChkData* ) & (
                    ( ( realTimeValue* ) rtmParam_p )->chkData );

        // ����������������ﴦ���ʶ
        T_mbsChkNode nodeData;
        // ���ﴦ���ʶ, MODBS����Big-Endianģʽ��X86����С��ģʽ
        //nodeData.trans_id = ( data_p[0] << 8 ) + ( data_p[1] );
        memcpy ( &nodeData.trans_id, data_p, 2 );
        reversEndian ( ( u_char* ) & ( nodeData.trans_id ), sizeof ( u_short ) );
        // ������
        nodeData.func_id  = data_p[7];

        if ( nodeData.func_id > 0x80 ) //˵����Ӧ���������쳣��Ӧ
        {
            //�õ��쳣��Ӧ��Ӧ�Ĺ�����
            nodeData.func_id = nodeData.func_id - 0x80;
        }


        // ����MBAP�е�Э���ʶ
        data_p[2] = 0x00;
        data_p[3] = 0x00;

        // ������֡�Ƿ�Ϊ��վ��������֡����Ӧ�������MBAP��
        // �����ﴦ���ʶ�͹�����
        // ����Ǳ�վ��������֡����Ӧ���ʹӸ�������ȥ������֡��
        if ( delSearData ( ( void* ) chkData_p, ( void* ) &nodeData ) != 0 )
        {
            result.errType = MC_APDU_CHECKSUM;
            result.motion  = NO_OPERATION;
        }
        else
        {
            result.errType = MC_APDU_OK;
            result.motion  = READ_CONNECT;
            // �������б���ɾ��������
            //delSearData((void*)chkData_p, (void*)&nodeData);
        }
    }
    // ����վ������ˣ����յ�����֡��
    else if ( role == SLAVE_STATION )
    {
        // ��Ӧ֡MBAP�С����ȡ�����packAPDU(��
        // ���ã��������ɿ���ϲ���ղ��Լ������
        result.errType = MC_APDU_OK;
        result.motion  = READ_CONNECT;
    }
    // �Ƿ���״̬��״̬���в����ڵ�״̬��
    else
    {
        result.errType = MC_APDU_CHECKSUM;
        result.motion  = NO_OPERATION;
    }

    return * ( u_int* ) &result;
}




/*******************************************************************************
*���ܣ����·��Ͷ˵�ʵʱ����������MBAP֡ͷ�е����ﴦ���ʶ, ��������š�
*���룺
*      role           �� վ���ɫ��
*      rtmParam_p     �� ʵʱ����������MBAP֡ͷ�е����ﴦ���ʶ��
*      cfgParam_p     �� ��
*      tmrCtrlCode_p  �� ��
*      data_p         :  ADU����
*      length         :  ADU���ݳ���
*�����
*      rtmParam_p
*���أ�
*      u_int       :  �Ƿ���Ҫ���Ͳ�Э����MODBUS����ҪЭ������˶�����0
*�޸���ʷ��
*
*******************************************************************************/
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

    // ����վ, �ͻ��ˣ���������֡��
    if ( role == MASTER_STATION )
    {
        // �������ﴦ���ʶ, ��������š�
        ( ( realTimeValue* ) rtmParam_p )->sendnum = (
                ( ( realTimeValue* ) rtmParam_p )->sendnum  % 0x7fff );
        ( ( realTimeValue* ) rtmParam_p )->sendnum++;

        result = 0;
    }
    // ����վ������ˣ�������Ӧ֡��
    // MODBUSЭ���У���Ӧ֡�����ﴦ��
    //��ʶ���ԶԷ�������֡����˲�����Ҫ���¡�
    else if ( role == SLAVE_STATION )
    {
        result = 0;
    }
    else
    {
        result = 0;
    }

    return result;
}




/*******************************************************************************
* ���ܣ���MODBUS�У�������Ҫ���ý��ղ�Ĳ�������˸ú���û��ʹ�á�
* ��������
* �ӿڲ�����
*     ��
* ����ֵ����
*******************************************************************************/
EXPDLL u_short UpdateRecvState ( short role,
                                 void* rtmParam_p,
                                 void* cfgParam_p,
                                 u_char* tmrCtrlCode_p,
                                 u_char* data_p,
                                 u_short length
                               )
{
    if ( role == MASTER_STATION )
    {
        ( ( realTimeValue* ) rtmParam_p )->receivenum = (
                ( ( realTimeValue* ) rtmParam_p )->receivenum  % 0x7fff );
        ( ( realTimeValue* ) rtmParam_p )->receivenum++;
    }

    // ���Э�����Ʊ���
    ctrl_Value result = {0};

    return * ( u_short* ) &result;
}




/*******************************************************************************
* ���ܣ���MODBUS�У��ú���û��ʹ�á�
* ��������
* �ӿڲ�����
*     ��
* ����ֵ����
*******************************************************************************/
EXPDLL int getFrameType ( u_char* data_p,
                          u_short length
                        )
{
    int tRet = I_F;

    if ( data_p != NULL && length == 0 )
    {
        if ( data_p[7] == MC_MBFC_0xFE ) //����һ��ͨ��
        {
            tRet = 0x03;
        }
        else if ( data_p[7] == MC_MBFC_0xFF ) //��һ��ͨ��
        {
            tRet = 0xff;
        }
    }

    return tRet;

}




/*******************************************************************************
* ���ܣ���MODBUS�У�������Ҫ�ײ����ϲ�֮����쳣�����Э����
* ��������
* �ӿڲ�����
*     ��
* ����ֵ����
*******************************************************************************/
EXPDLL int HandleRecvStateError ( transCtrlFrmDsc* ctrlFrm,
                                  void* rtmParam_p,
                                  int error
                                )

{
    // ������������Ҫ�������Ʋ���з���Э�����
    //Ҳ����Ҫ���ϲ�ASDU���з���Э�����

    return 0;
}




/*******************************************************************************
* ���ܣ���MODBUS�У�������Ҫ�ײ����֡Ӧ�������ݡ�
* ��������
* �ӿڲ�����
*     ��
* ����ֵ����
*******************************************************************************/
EXPDLL int RespCtrlFrame ( transCtrlFrmDsc* ctrlFrm,
                           void* rtmParam_p,
                           int result
                         )
{

    return 0;
}




/*******************************************************************************
* ���ܣ���MODBUS�У�������Ҫ�ײ����֡Э���������ݡ�
* ��������
* �ӿڲ�����
*     ��
* ����ֵ����
*******************************************************************************/
EXPDLL int CtrlFrame ( transCtrlFrmDsc* ctrlFrm,
                       void* rtmParam_p,
                       int result
                     )

{
    return 0;
}




EXPDLL int APDUbindToMem ( u_char* apduObj_p,
                           u_char* APDU_p,
                           u_short ASDUlng,
                           u_char duType
                         )

{
    C_ModbusADU* apdu104_p = NULL;
    //int asdu_BodySet = 0;
    apdu104_p = ( C_ModbusADU* ) apduObj_p;

    //ADU_DSCR* apdu_dscr_p = apdu104_p->getDU_Info();
    apdu104_p->bindToMem ( APDU_p, duType, ASDUlng );

    if ( apdu104_p->getDU_Lng() < ASDUlng )
    {
        return -1;//apdu���ȴ���
    }

    return 0;
}




/*******************************************************************************
  *���ܣ���ȡMODBUS����֡�ĳ��ȡ����㷽�����£�
  *     ��Socket���ջ����У���ȡMBAP�С����ȡ����ֵ��
  *     Ȼ���ٸ���MBAP�ĳ��ȣ������ADU���ȡ�
  * ���룺
  *      bufferAdd   ��MBAP�׵�ַ
  *      bufferLng   : MBAP����
  *�����
  *      ��
  *���أ�
  *    >0 �ɹ�����ADU����(MBAP+PDU)
  *    ��1 ���������ȴ���
  *    ��2 �׵�ַ���������ַ�
  *    <0  �ӿ�ִ���쳣
  *�޸���ʷ
  *
*******************************************************************************/
EXPDLL int getApduLngFrmBuf ( char*  bufferAdd,
                              int bufferLng
                            )
{
    if ( bufferAdd == NULL )
    {
        return ERR_POINTER_NULL;
    }

    if ( bufferLng <= 0 )
    {
        return -1;
    }

    //��ȡ�����ȡ����ֵ��MODBUS���ô��ģʽ
    //int aduLng  = (bufferAdd[4] << 8) + bufferAdd[5];
    u_short aduLng = 0;
    memcpy ( &aduLng, bufferAdd + 4, 2 );
    reversEndian ( ( u_char* ) &aduLng, sizeof ( u_short ) );

    //����ADU���ݳ���
    // 6 = 2�ֽڣ����ȣ�+ 2�ֽڣ�Э���ʶ����
    //     +2�ֽڣ�����Ԫ��ʶ����
    aduLng = aduLng + 6;

    return aduLng;
}



