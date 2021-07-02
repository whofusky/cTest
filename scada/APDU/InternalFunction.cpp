
#include "InternalFunction.h"




const T_TIDVALLNG  g_TID104[MC_MAXTID_IDS] =
{
    /*tid:0  */ {0,  0, 0, 0},  /*δ����*/
    /*tid:1  */ {1,  1, 1, 0},
    /*tid:2  */ {1,  1, 1, 3},
    /*tid:3  */ {1,  2, 1, 0},
    /*tid:4  */ {1,  2, 1, 3},
    /*tid:5  */ {2,  7, 1, 0},   /*VTI��˲��״ָ̬ʾֵ*/
    /*tid:6  */ {2,  7, 1, 3},
    /*tid:7  */ {5, 32, 1, 0},
    /*tid:8  */ {5, 32, 1, 3},
    /*tid:9  */ {3, 16, 1, 0},
    /*tid:10 */ {3, 16, 1, 3},
    /*tid:11 */ {3, 16, 1, 0},
    /*tid:12 */ {3, 16, 1, 3},
    /*tid:13 */ {5, 32, 1, 0},
    /*tid:14 */ {5, 32, 1, 3},
    /*tid:15 */ {5, 32, 1, 0},  /*BCR�����Ƽ���������*/
    /*tid:16 */ {5, 32, 1, 3},
    /*tid:17 */ {1,  2, 1, 5},  /*SEP(�̵籣���豸�¼�)+CP16Time2a+CP24Time2a*/
    /*tid:18 */ {2,  6, 1, 5},  /*SPE+QDP+CP16Time2a+CP24Time2a*/
    /*tid:19 */ {2,  4, 1, 5},  /*OCI+QDP+CP16Time2a+CP24Time2a*/
    /*tid:20 */ {5, 32, 1, 0},
    /*tid:21 */ {2, 16, 0, 0},
    /*tid:22 */ {0,  0, 0, 0}, /*22...29 Ϊ�������ݶ��屣��*/
    /*tid:23 */ {0,  0, 0, 0},
    /*tid:24 */ {0,  0, 0, 0},
    /*tid:25 */ {0,  0, 0, 0},
    /*tid:26 */ {0,  0, 0, 0},
    /*tid:27 */ {0,  0, 0, 0},
    /*tid:28 */ {0,  0, 0, 0},
    /*tid:29 */ {0,  0, 0, 0},
    /*tid:30 */ {1,  1, 1, 7}, /*SIQ+CP56Time2a*/
    /*tid:31 */ {1,  2, 1, 7}, /*DIQ+CP56Time2a*/
    /*tid:32 */ {2,  7, 1, 7}, /*VTI+QDS+CP56Time2a*/
    /*tid:33 */ {5, 32, 1, 7},/*BSI+QDS+CP56Time2a*/
    /*tid:34 */ {3, 16, 1, 7},/*NVA+QDS+CP56Time2a*/
    /*tid:35 */ {3, 16, 1, 7},/*SVA+QDS+CP56Time2a*/
    /*tid:36 */ {5, 32, 1, 7},/*STD+QDS+CP56Time2a*/
    /*tid:37 */ {5, 32, 1, 7},/*BCR+CP56Time2a*/
    /*tid:38 */ {1,  2, 1, 9}, /*SEP+CP16Time2a+CP56Time2a*/
    /*tid:39 */ {2,  6, 1, 9}, /*SPE+QDP+CP16Time2a+CP56Time2a*/
    /*tid:40 */ {2,  4, 1, 9}, /*OCI+QDP+CP16Time2a+CP56Time2a*/
    /*tid:41 */ {0,  0, 0, 0}, /*41...44 Ϊ�������ݶ��屣��*/
    /*tid:42 */ {0,  0, 0, 0},
    /*tid:43 */ {0,  0, 0, 0},
    /*tid:44 */ {0,  0, 0, 0},
    /*tid:45 */ {1,  0, 1, 0},/*SCO*//*45...69Ϊ���Ʒ���Ĺ�����Ϣ*/
    /*tid:46 */ {1,  0, 1, 0},/*DCO*/
    /*tid:47 */ {1,  0, 1, 0},/*RCO*/
    /*tid:48 */ {3, 16, 1, 0},
    /*tid:49 */ {3, 16, 1, 0},
    /*tid:50 */ {5, 32, 1, 0},
    /*tid:51 */ {4, 32, 0, 0},/*BSI*/
    /*tid:52 */ {0,  0, 0, 0},/*52...69Ϊ�������ݶ��屣��*/
    /*tid:53 */ {0,  0, 0, 0},
    /*tid:54 */ {0,  0, 0, 0},
    /*tid:55 */ {0,  0, 0, 0},
    /*tid:56 */ {0,  0, 0, 0},
    /*tid:57 */ {0,  0, 0, 0},
    /*tid:58 */ {1,  0, 1, 7},/*SCO+CP56Time2a;58...64��104Э���101������*/
    /*tid:59 */ {1,  0, 1, 7},/*DCO+CP56Time2a*/
    /*tid:60 */ {1,  0, 1, 7},/*RCO+CP56Time2a*/
    /*tid:61 */ {3, 16, 1, 7},/*NVA+QOS+CP56Time2a*/
    /*tid:62 */ {3, 16, 1, 7},/*SVA+QOS+CP56Time2a*/
    /*tid:63 */ {5, 32, 1, 7},/*STD+QOS+CP56Time2a*/
    /*tid:64 */ {4, 32, 0, 7},/*BSI+CP56Time2a*/
    /*tid:65 */ {0,  0, 0, 0},
    /*tid:66 */ {0,  0, 0, 0},
    /*tid:67 */ {0,  0, 0, 0},
    /*tid:68 */ {0,  0, 0, 0},
    /*tid:69 */ {0,  0, 0, 0},
    /*tid:70 */ {1,  0, 1, 0},/*70...99 ���ӷ����ϵͳ����*/
    /*tid:71 */ {0,  0, 0, 0},/*71...99Ϊ�������ݶ��屣��*/
    /*tid:72 */ {0,  0, 0, 0},
    /*tid:73 */ {0,  0, 0, 0},
    /*tid:74 */ {0,  0, 0, 0},
    /*tid:75 */ {0,  0, 0, 0},
    /*tid:76 */ {0,  0, 0, 0},
    /*tid:77 */ {0,  0, 0, 0},
    /*tid:78 */ {0,  0, 0, 0},
    /*tid:79 */ {0,  0, 0, 0},
    /*tid:80 */ {0,  0, 0, 0},
    /*tid:81 */ {0,  0, 0, 0},
    /*tid:82 */ {0,  0, 0, 0},
    /*tid:83 */ {0,  0, 0, 0},
    /*tid:84 */ {0,  0, 0, 0},
    /*tid:85 */ {0,  0, 0, 0},
    /*tid:86 */ {0,  0, 0, 0},
    /*tid:87 */ {0,  0, 0, 0},
    /*tid:88 */ {0,  0, 0, 0},
    /*tid:89 */ {0,  0, 0, 0},
    /*tid:90 */ {0,  0, 0, 0},
    /*tid:91 */ {0,  0, 0, 0},
    /*tid:92 */ {0,  0, 0, 0},
    /*tid:93 */ {0,  0, 0, 0},
    /*tid:94 */ {0,  0, 0, 0},
    /*tid:95 */ {0,  0, 0, 0},
    /*tid:96 */ {0,  0, 0, 0},
    /*tid:97 */ {0,  0, 0, 0},
    /*tid:98 */ {0,  0, 0, 0},
    /*tid:99 */ {0,  0, 0, 0},
    /*tid:100*/ {1,  0, 1, 0},/*100...109���Ʒ����ϵͳ����*/
    /*tid:101*/ {1,  0, 1, 0},
    /*tid:102*/ {0,  0, 0, 0},/*������ֻ����Ϣ�����ַ*/
    /*tid:103*/ {0,  0, 0, 7},/*CP56Time2a*/
    /*tid:104*/ {2, 16, 0, 0},
    /*tid:105*/ {1,  0, 1, 0},
    /*tid:106*/ {0,  0, 0, 7},/*CP56Time2a*//*107...109Ϊ�������ݶ��屣��*/
    /*tid:107*/ {1,  8, 0, 7},/*104Э������tid107:TSC+CP56Time2a*/
    /*tid:108*/ {0,  0, 0, 0},
    /*tid:109*/ {0,  0, 0, 0},
    /*tid:110*/ {3, 16, 1, 0},/*110...119�ڿ��Ʒ���Ĳ�������*/
    /*tid:111*/ {3, 16, 1, 0},
    /*tid:112*/ {5, 32, 1, 0},
    /*tid:113*/ {1,  0, 1, 0},
    /*tid:114*/ {0,  0, 0, 0},/*114...119Ϊ�������ݶ��屣��*/
    /*tid:115*/ {0,  0, 0, 0},
    /*tid:116*/ {0,  0, 0, 0},
    /*tid:117*/ {0,  0, 0, 0},
    /*tid:118*/ {0,  0, 0, 0},
    /*tid:119*/ {0,  0, 0, 0},
    /*tid:120*/ {3, 16, 1, 0},/*120...127 �ļ����䣺�ݲ�����*/
    /*tid:121*/ {4, 24, 1, 0},
    /*tid:122*/ {3, 16, 1, 0},
    /*tid:123*/ {5, 40, 0, 0},
    /*tid:124*/ {4, 24, 1, 0},
    /*tid:125*/ {0,  0, 0, 0},
    /*tid:126*/ {3, 24, 0, 7},
    /*tid:127*/ {1,  8, 0, 14},/*104Э������tid127: +CP56Time2a+CP56Time2a*/
    /*tid:128 */ {0, 0, 0, 0},
    /*tid:129 */ {0, 0, 0, 0},
    /*tid:130 */ {0, 0, 0, 0},
    /*tid:131 */ {0,  0, 0, 0},
    /*tid:132 */ {0,  0, 0, 0},
    /*tid:133 */ {0,  0, 0, 0},
    /*tid:134 */ {0,  0, 0, 0},
    /*tid:135 */ {0,  0, 0, 0},
    /*tid:136 */ {0,  0, 0, 0}, /*U֡����Ự STARTDT             */
    /*tid:137 */ {0,  0, 0, 0}, /*U֡����Ự STOPDT              */
    /*tid:138 */ {0,  0, 0, 0}, /*U֡����Ự TESTFR              */
    /*tid:139 */ {0,  0, 0, 0}, /*U֡����Ự NEWHOLD ����һ��ͨ�� */
    /*tid:140 */ {0,  0, 0, 0}  /*U֡����Ự DELHOLD ��һ��ͨ��   */
};


/**************************�ڲ�ʹ�ú����ǽӿ�***********************************/

/*******************************************************************************
  *���ܣ���װASDU
  * ���룺
  *      apdu_p      ��APDU104ָ��
  *      tid_p       : ���ͱ�ʶ
  *      vsq_p       : �ɱ�ṹ���޶���
  *      cot_p       : ASDU_COT1_VAL���ʹ���ԭ��
  *      cot2_p      : ASDU_COT2_VAL���ʹ���ԭ��
  *      addr_p      : ASDU������ַ
  *      bodyAddr_p  : ��Ϣ���ַ
  *      bodySet_p   : ��ϢԪ�ؼ�
  *      bodySetLng  : ��ϢԪ�ؼ�����
  *      tsp_p       : ��Ϣ��ʱ��
  *      bodyObjNum  : ��Ϣ�������
  *�����
  *     apdu_p       : ��ֵ���APDU104
  *���أ�
  *    =0 �ӿ�ִ�гɹ�
  *    <0 �ӿ�ִ���쳣
  *�޸���ʷ
  *
********************************************************************************/
int packASDU ( APDU104* apdu_p,
               ASDU_TID_VAL* tid_p,
               ASDU_VSQ_VAL* vsq_p,
               ASDU_COT1_VAL* cot_p,
               ASDU_COT2_VAL* cot2_p,
               ASDU_ADDR_VAL* addr_p,
               u_char* bodyAddr_p,
               u_char* bodySet_p,
               u_short bodySetLng,
               void* tsp_p,
               u_char bodyObjNum
             )
{
    int tmpRet = 0;

    if ( apdu_p == NULL
         || tid_p == NULL
         || vsq_p == NULL
         || ( cot_p == NULL && cot2_p == NULL )
         || addr_p == NULL
       )
    {
        return ERR_POINTER_NULL;
    }

    tmpRet = apdu_p->setASDU_TID_VAL ( tid_p );

    if ( tmpRet < 0 )
    {
        return tmpRet;
    }

    tmpRet = apdu_p->setASDU_VSQ_VAL ( vsq_p );

    if ( tmpRet < 0 )
    {
        return tmpRet;
    }

    if ( cot_p != NULL )
    {
        tmpRet = apdu_p->setASDU_COT_VAL ( cot_p );
    }
    else
    {
        tmpRet = apdu_p->setASDU_COT_VAL ( cot2_p );
    }

    if ( tmpRet < 0 )
    {
        return tmpRet;
    }

    tmpRet = apdu_p->setASDU_ADDR_VAL ( addr_p );

    if ( tmpRet < 0 )
    {
        return tmpRet;
    }

    APDU_DSCR* apdu_dscr_p = apdu_p->getDU_Info();
    u_short tOneBdAddLng   = apdu_dscr_p->ASDU_CMTBL[BODY_ADDR_INDEX];
    u_short tOneTspLng     = apdu_dscr_p->ASDU_CMTBL[BODY_TSP_INDEX];

    BODY_ADDR_VAL* tBodyAddr_p = NULL;
    u_char* tmpSetUc_p = NULL; 
    u_char* tmpTspUc_p = NULL; 
    for ( u_char i = 0; i < bodyObjNum; i++ )
    {
        if ( bodyAddr_p != NULL )
        {
            tBodyAddr_p = ( BODY_ADDR_VAL* ) ( bodyAddr_p + ( i * tOneBdAddLng ) );
            tmpRet = apdu_p->setBODY_ADDR_VAL ( tBodyAddr_p, i );

            if ( tmpRet < 0 )
            {
                return tmpRet;
            }
        }

        if ( bodySet_p != NULL && bodySetLng > 0 )
        {
            tmpSetUc_p = bodySet_p + ( i * bodySetLng );
            tmpRet = apdu_p->setAPDU_ELEMENT ( tmpSetUc_p, bodySetLng,
                                               BODY_SET_INDEX,
                                               i );

            if ( tmpRet < 0 )
            {
                return tmpRet;
            }
        }

        if ( tsp_p != NULL && tOneTspLng > 0 )
        {
            tmpTspUc_p = ( ( u_char* ) tsp_p ) + ( i * tOneTspLng );
            tmpRet = apdu_p->setBODY_TSP_VAL ( tmpTspUc_p, i );

            if ( tmpRet < 0 )
            {
                return tmpRet;
            }
        }
    }

    return tmpRet;
}




/******************************************************************************
  *���ܣ�
  *     ��װONE_STEP�г�nextStep_p֮���ֵ
  * ���룺
  *      step_p         ��ONE_STEP�ռ�ָ��
  *      stepNo         : Ҫ��װ��ֵ ����
  *      reqPDU_FCode   : Ҫ��װ��ֵ ����֡ʹ�õ�֡��ʽ����
  *      reqPDU_Num     : Ҫ��װ��ֵ ����֡��PDU������
  *      reqAPDU_Obj_p  : Ҫ��װ��ֵ ����֡APDU����ָ���
  *      respPDU_FCode  : Ҫ��װ��ֵ ��Ӧ֡ʹ�õ�֡��ʽ����
  *      respPDU_Num    : Ҫ��װ��ֵ ��ӦPDU����
  *      respAPDU_Obj_p : Ҫ��װ��ֵ ��Ӧ֡APDU����ָ���
  *      sVerifyBlock_p : Ҫ��װ��ֵ ר�ò���֤��
  *�����
  *      step_p         : ��ֵ���ONE_STEPָ��
  *���أ�
  *      void
  *�޸���ʷ
  *
*******************************************************************************/
void setONE_STEP ( ONE_STEP* step_p,
                   u_char stepNo,
                   u_char reqPDU_FCode,
                   u_char reqPDU_Num,
                   u_char* reqAPDU_Obj_p,
                   u_char respPDU_FCode,
                   u_char respPDU_Num,
                   u_char* respAPDU_Obj_p,
                   vrfItem* sVerifyBlock_p
                 )
{
    step_p->reqAPDU_Obj_p     = reqAPDU_Obj_p;
    step_p->reqPDU_FCode      = reqPDU_FCode;
    step_p->reqPDU_Num        = reqPDU_Num;
    step_p->respAPDU_Obj_p    = respAPDU_Obj_p;
    step_p->respPDU_FCode     = respPDU_FCode;
    step_p->respPDU_Num       = respPDU_Num;
    step_p->stepNo            = stepNo;
    step_p->sVerifyBlock_p    = sVerifyBlock_p;
}




/******************************************************************************
  *���ܣ�
  *      ����startStep_p�в���ΪtargetStep_p->stepNo�Ĳ�
  *���룺
  *      startStep_p ������
  *      targetStep_p��Ҫ���ҵĲ�
  *�����
  *      ��
  *���أ�
  *      ONE_STEP*
  *              1���쳣������� NULL
  *              2�������������startStep_p�в���ΪtargetStep_p->stepNo�Ĳ�;
  *�޸���ʷ
  *
******************************************************************************/
ONE_STEP* displayNextNode ( ONE_STEP* startStep_p,
                            const ONE_STEP* targetStep_p
                          )
{
    ONE_STEP* step_p = startStep_p;

    while ( step_p->stepNo != targetStep_p->stepNo )
    {
        if ( step_p->nextStep_p != NULL )
        {
            step_p = step_p->nextStep_p;
        }
        else
        {
            return NULL;
        }
    }

    return step_p;
}




/*******************************************************************************
  *���ܣ�������ONE_STEPĩβ����һ���ڵ�
  * ���룺
  *      one_step_p ������ͷָ��
  *      tail_step_p: Ҫ��ӵĽڵ�ָ��
  *�����
  *      one_step_p ����ӽڵ�ָ��֮�������ͷָ��
  *���أ�
  *      void
  *�޸���ʷ��
  *      20170221092324 �޸�ԭ�����߼�����
  *      20170314084211 ���step_pΪnull�����
*******************************************************************************/
void addNode ( ONE_STEP* step_p,
               ONE_STEP* tail_step_p
             )
{
    /*ONE_STEP* one_step_p = step_p;
    do
    {
        one_step_p = one_step_p->nextStep;
    }
    while (one_step_p !=NULL);
    one_step_p->nextStep = tail_step_p;*/

    ONE_STEP* one_step_p = step_p;

    if ( one_step_p == NULL )
    {
        step_p = tail_step_p;
    }
    else
    {
        while ( one_step_p->nextStep_p != NULL )
        {
            one_step_p = one_step_p->nextStep_p;
        }

        one_step_p->nextStep_p = tail_step_p;
    }

}



//��汾����˽ӿ���ʱ����
void calcSetLng ( u_char VSQ_SQ,
                  int i,
                  int usrData_index,
                  u_int tmpOneLng,
                  u_int tmpPntLng,
                  u_int& lowOffset,
                  u_int& lowLen,
                  u_int& hghOvrOffset,
                  u_int& hghOvrLen,
                  u_int& highOffset,
                  u_int& highLen
                )
{
    lowOffset    = 0;
    lowLen       = 0;
    hghOvrOffset = 0;
    hghOvrLen    = 0;
    highOffset   = 0;
    highLen      = 0;

    if ( VSQ_SQ == 1 )
    {
        lowOffset = ( i - usrData_index ) * tmpOneLng;
        lowLen = tmpPntLng;
    }
    else if ( VSQ_SQ == 0 )
    {
        lowOffset = 0;

        if ( tmpOneLng > tmpPntLng )
        {
            lowLen = tmpPntLng;
        }
        else
        {
            lowLen = tmpOneLng;
        }
    }

    if ( tmpOneLng > tmpPntLng * 2 )
    {

        if ( VSQ_SQ == 1 )
        {
            hghOvrOffset = ( i - usrData_index ) * tmpOneLng + tmpPntLng;
            hghOvrLen    = tmpPntLng;
        }
        else if ( VSQ_SQ == 0 )
        {
            hghOvrOffset = tmpPntLng;
            hghOvrLen    = tmpPntLng;
        }

    }
    else
    {
        if ( VSQ_SQ == 1 )
        {
            highOffset = ( i - usrData_index ) * tmpOneLng + tmpPntLng;
            highLen    = tmpOneLng - tmpPntLng;
        }
        else if ( VSQ_SQ == 0 )
        {
            highOffset = tmpPntLng;
            highLen    = tmpOneLng - tmpPntLng;
        }
    }

    return;
}


/******************************************************************************
  *���ܣ�
  *    �Ự����֡�������պ��T_snnPara��ֵ
  * ���룺
  *     stepRl_p  �����պ�ĵ�ǰ��
  *     funDsc_p  ���Ự����������
  *     apdu104_p ��APDU104����ָ��
  *     stnAcsFlag���洢����ȡ��ʶ
  *�����
  *     ssnPar_p��  ���������˵��
  *���أ�
  *     0 �ɹ�
  *    <0 ʧ��
  *�޸���ʷ
  *
*******************************************************************************/
int asgmtSnnPar ( const ONE_STEP* stepRl_p,
                  const T_ptclFunDsc* funDsc_p,
                  APDU104* apdu104_p,
                  T_stnAcsFlag* stnAcsFlag,
                  T_snnPara* ssnPar_p
                )
{

    if ( stepRl_p == NULL
         || funDsc_p == NULL
         || apdu104_p == NULL
         || ssnPar_p == NULL
         || ssnPar_p->ASDUID_p == NULL
         || ssnPar_p->usrData_p == NULL
       )
    {
        return ERR_POINTER_NULL;
    }

    T_snnUsrData* tUsrData_p = ssnPar_p->usrData_p;
    APDU_DSCR* tApduDscr_p   = apdu104_p->getDU_Info();

    if ( tApduDscr_p == NULL )
    {
        return ERR_POINTER_NULL;
    }

    /*******************************��APDU104������ȡֵ*************************/
    //���ͱ�ʶ
    ASDU_TID_VAL* ssnTid_p = apdu104_p->getASDU_TID_VAL (
                             & ( tApduDscr_p->ASDU_CMTBL[ASDU_TID_INDEX] )
                             );
    //�ɱ�ṹ���޶���
    ASDU_VSQ_VAL* ssnVSQ_p = apdu104_p->getASDU_VSQ_VAL (
                             & ( tApduDscr_p->ASDU_CMTBL[ASDU_VSQ_INDEX] )
                             );

    if ( ssnVSQ_p == NULL )
    {
        return ERR_POINTER_NULL;
    }

    //����ԭ��ĳ���
    u_short* ssnCotLen_p = & (
                           tApduDscr_p->ASDU_CMTBL[ASDU_COT_INDEX] );
    //����ԭ��
    u_short* ssnCOT_p = ( u_short* ) apdu104_p->getASDU_COT_VAL (
                        ssnCotLen_p );
    //ASDU������ַ
    ASDU_ADDR_VAL* ssnAddr_p = apdu104_p->getASDU_ADDR_VAL (
                               & ( tApduDscr_p->ASDU_CMTBL[ASDU_ADDR_INDEX] )
                               );



    /*******************************��ֵT_snnPara**************************/
    ssnPar_p->stepNo      = stepRl_p->stepNo;
    ssnPar_p->infoType    = GINFOTYPE;

    /*������֡�� ����ֵ����ĳһ֡���޸ģ�
    ssnPar_p->ASDUIDNum = 1;
    */
    //u_char actFlag = ( u_char )funDsc_p->act_prio_chnlID;
    char reqFlag = getRcvReqFlag( funDsc_p );

    u_char ASDUID_index = 0;

    if ( reqFlag == 1 )
    {
        //�������������
        if ( 0 == stepRl_p->reqPDU_Num )
        {
            return ERR_SYSTEM_LOGIC;
        }

        ssnPar_p->actFlag = ACTIVE_TYPE;
        ASDUID_index      = stepRl_p->reqPDU_Num - 1;
        //ASDUID_index = stepRl_p->respPDU_Num ;
    }
    else if ( reqFlag == 0 )
    {
        //������Ӧ������;
        if ( 0 == stepRl_p->respPDU_Num )
        {
            return ERR_SYSTEM_LOGIC;
        }

        ssnPar_p->actFlag = PASSIVE_TYPE;
        ASDUID_index      = stepRl_p->respPDU_Num - 1;
        //ASDUID_index = stepRl_p->reqPDU_Num - 1;

    }

    memcpy ( & ( ssnPar_p->ASDUID_p[ASDUID_index] ),
             ssnTid_p,
             sizeof ( ASDU_TID_VAL )
           );
    memcpy ( &ssnPar_p->COT, ssnCOT_p, *ssnCotLen_p );
    memcpy ( &ssnPar_p->cmnAddr, ssnAddr_p, sizeof ( ASDU_ADDR_VAL ) );
    //memcpy(&ssnPar_p->acsMthd,ssnVSQ_p ,sizeof(ASDU_VSQ_VAL));

    u_char acsMthd        = ( ssnPar_p->acsMthd & 0x7f ); //ȡ��7λ
    u_short usrData_index = ASDUID_index * acsMthd;
    ssnPar_p->pntPos      = usrData_index;
    ssnPar_p->pntNum      = usrData_index + ssnVSQ_p->N;

    //��ÿ��ASDU����Ϣ������Ϣ��ʱ���ǲ�ȷ���ģ���Ҫ���ݾ���ĻỰ������
    u_char  tmpTID       = * ( ( u_char* ) ssnTid_p );

    T_tidObjLng tTidObjLng;
    memset ( &tTidObjLng, 0, sizeof ( tTidObjLng ) );
    int tRet = getTidObjLng ( tmpTID,
                              &tTidObjLng
                            );

    if ( tRet != 0 )
    {
        return tRet;
    }

    switch ( funDsc_p->ssnDscNo )
    {
        case STN_INIT: //վ��ʼ��;
        {
            break;
        }

        case QUE_DATA: //��ѯ����;
        {
            if ( tmpTID == MC_TID_102 )
            {
                if ( ssnVSQ_p->SQ != 0 )
                {
                    return ERR_VSQN_SQ;
                }
            }

            tRet = setPntSnnpar ( tTidObjLng,
                                  usrData_index,
                                  tUsrData_p,
                                  apdu104_p,
                                  stnAcsFlag
                                );

            if ( tRet != 0 )
            {
                return tRet;
            }

            break;
        }          //end ��ѯ����

        case CYC_TRNS: //���ڴ���
        {

            tRet = setPntSnnpar ( tTidObjLng,
                                  usrData_index,
                                  tUsrData_p,
                                  apdu104_p,
                                  stnAcsFlag
                                );

            if ( tRet != 0 )
            {
                return tRet;
            }

            break;
        }         //end ���ڴ���

        case ACQ_EVT: //�¼��ռ�
        {
            tRet = setPntSnnpar ( tTidObjLng,
                                  usrData_index,
                                  tUsrData_p,
                                  apdu104_p,
                                  stnAcsFlag
                                );

            if ( tRet != 0 )
            {
                return tRet;
            }

            break;
        }            //end �¼��ɼ�

        case QUKCHK_EVT: //����¼�
        {
            //��ʹ��
            break;
        }

        case STN_INRGTN: //վ�ٻ�
        {

            if ( tmpTID == MC_TID_100 )
            {
                if ( ssnVSQ_p->N != 1 )
                {
                    return ERR_VSQN_N;
                }

            }

            tRet = setPntSnnpar ( tTidObjLng,
                                  usrData_index,
                                  tUsrData_p,
                                  apdu104_p,
                                  stnAcsFlag
                                );

            if ( tRet != 0 )
            {
                return tRet;
            }

            break;
        }//end վ�ٻ�

        case CLK_SYN: //ʱ��ͬ��
        {
            if ( tmpTID == MC_TID_103 )
            {
                if ( ssnVSQ_p->SQ != 0 )
                {
                    return ERR_VSQN_SQ;
                }

                tRet = setPntSnnpar ( tTidObjLng,
                                      usrData_index,
                                      tUsrData_p,
                                      apdu104_p,
                                      stnAcsFlag
                                    );

                if ( tRet != 0 )
                {
                    return tRet;
                }
            }

            break;
        }

        case CMD_TRNS: //�����
        {

            tRet = setPntSnnpar ( tTidObjLng,
                                  usrData_index,
                                  tUsrData_p,
                                  apdu104_p,
                                  stnAcsFlag
                                );

            if ( tRet != 0 )
            {
                return tRet;
            }

            break;
        }

        case ACMU_FRZ:
        {
            //�ۻ�������ǰ�Ķ���    �˹����Ǵ�ACMU_TRNS���ܵ�ģʽ2������վ�ȶ�������������������
            if ( tmpTID == MC_TID_101 )
            {
                if ( ssnVSQ_p->SQ != 0 )
                {
                    return ERR_VSQN_SQ;
                }

                tRet = setPntSnnpar ( tTidObjLng,
                                      usrData_index,
                                      tUsrData_p,
                                      apdu104_p,
                                      stnAcsFlag
                                    );

                if ( tRet != 0 )
                {
                    return tRet;
                }
            }

            break;
        }

        case ACMU_TRNS: //�ۻ�������
        {
            tRet = setPntSnnpar ( tTidObjLng,
                                  usrData_index,
                                  tUsrData_p,
                                  apdu104_p,
                                  stnAcsFlag
                                );

            if ( tRet != 0 )
            {
                return tRet;
            }

            break;
        }

        case PARA_LD: //��������
        {
            if ( ssnVSQ_p->SQ != 0 )
            {
                return ERR_VSQN_SQ;
            }

            tRet = setPntSnnpar ( tTidObjLng,
                                  usrData_index,
                                  tUsrData_p,
                                  apdu104_p,
                                  stnAcsFlag
                                );

            if ( tRet != 0 )
            {
                return tRet;
            }

            break;
        }

        case TST_PRCD: //���Թ���
        {
            if ( ssnVSQ_p->SQ != 0 )
            {
                return ERR_VSQN_SQ;
            }

            tRet = setPntSnnpar ( tTidObjLng,
                                  usrData_index,
                                  tUsrData_p,
                                  apdu104_p,
                                  stnAcsFlag
                                );

            if ( tRet != 0 )
            {
                return tRet;
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
            break;
        }

        default:
            break;
    }//end ��Ӧ�ù��ܵĲ�ͬ�ֱ��Ԫ�ؼ���ʱ��Ľ������

    ssnPar_p->stnAcsFlag = * ( ( u_short* ) stnAcsFlag );

    return 0;
}




/*******************************************************************************
  *���ܣ�
  *    ר�ò���֤��У��
  *
  * ���룺
  *     iVerifyBlock_p  ��ר�ò���֤��
  *     apdu104_p       ��APDU104����
  *�����
  *     iVerifyBlock_p->chkRslt_p������״̬��//λ0 = 1 ��ʾ�����ֵ���ճɹ���
                                   //λ 1 = 1ʱ����Сֵ���ճɹ�
  *���أ�
  *     0  �ɹ�
  *     -5  �޶�����ʵ��Ԫ�ظ�������
  *     -21 ���ֵ����ʧ��
  *     -22 ��Сֵ����ʧ��
  *     -23 �������С����ֵ����
  *�޸���ʷ
  *
*******************************************************************************/
int chkVerifyBlock ( const vrfItem* iVerifyBlock_p,
                     APDU104* apdu104_p
                   )
{
    if ( iVerifyBlock_p == NULL
         || ( iVerifyBlock_p->vrfCd & 3 ) == 0
       )
    {
        return 0;
    }

    u_int  tmpUint         = 0;    //��ֵ
    u_short tmpBodSetLng   = 0;    //Ԫ�ؼ�����
    u_char* ssnBodSet_p    = NULL; //Ԫ�ؼ��׵�ַ
    u_int tmpOneLng        = 0;    //ÿ��Ԫ�صĳ���
    u_short tmpBodSetNum   = 0;    //Ԫ�ظ���
    ASDU_VSQ_VAL* ssnVSQ_p = NULL;//�ɱ��޶���

    ssnBodSet_p   = apdu104_p->getAPDU_ELEMENT ( &tmpBodSetLng, BODY_SET_INDEX );
    tmpOneLng     = apdu104_p->getDU_Info()->InfoEleLng;
    tmpBodSetNum  = apdu104_p->getDU_Info()->ASDU_CMTBL[BODY_SET_INDEX];
    ssnVSQ_p      = apdu104_p->getASDU_VSQ_VAL (
                    &apdu104_p->getDU_Info()->ASDU_CMTBL[ASDU_VSQ_INDEX]
                    );

    if ( tmpBodSetNum > 0
         && tmpBodSetNum != ssnVSQ_p->N
       ) //�޶�����ʵ��Ԫ�ظ�������
    {
        return ERR_VSQN_NO;
    }

    for ( int i = 0; i < tmpBodSetNum; i++ ) //��ÿ����ֵ���д�СֵУ��
    {
        memcpy ( &tmpUint,
                 ssnBodSet_p + i * tmpOneLng,
                 tmpOneLng
               );

        if ( iVerifyBlock_p->vrfCd == 1 )   //�����ֵ����;
        {
            if ( iVerifyBlock_p->chkStd.gChkStd.pntVal2 < tmpUint )
            {
                return ERR_MAX_VAL_CH;
            }

            * ( iVerifyBlock_p->chkRslt_p ) = 1;
        }
        else if ( iVerifyBlock_p->vrfCd == 2 ) //����Сֵ����;
        {
            if ( ( u_char ) iVerifyBlock_p->chkStd.gChkStd.pntVal1 > tmpUint )
            {
                return ERR_MIN_VAL_CH;
            }

            * ( iVerifyBlock_p->chkRslt_p ) = 2;
        }
        else if ( iVerifyBlock_p->vrfCd == 3 ) //�������С����ֵ����;
        {
            if ( ( u_char ) iVerifyBlock_p->chkStd.gChkStd.pntVal2 < tmpUint
                 || ( u_char ) iVerifyBlock_p->chkStd.gChkStd.pntVal1 > tmpUint
               )
            {
                return ERR_INTE_VAL_CH;
            }

            * ( iVerifyBlock_p->chkRslt_p ) = 3;
        }

    }

    return 0;
}




/*******************************************************************************
  *���ܣ�
  *   ��������һ֡��ֵ
  *
  * ���룺
  *     nxtFrmRlStep_p����һ֡��Ӧ�Ĺ���
  *     nxtFrmStep_p  ����ǰ��
  *     nxtFrmStepNo_p: ��һ֡�Ĳ���
  *     funDsc_p      ���Ự����������
  *     ssnPar_p      �����������˵��
  *�����
  *     nxtFrmStep_p  ����һ֡��Ӧ�Ĳ�
  *���أ�
  *     0    �ɹ�
  *     ��ֵ �ӿ��쳣
  *�޸���ʷ
  *
*******************************************************************************/
int asgmtNxtFrmPar ( ONE_STEP* nxtFrmRlStep_p,
                     ONE_STEP* nxtFrmStep_p,
                     u_char* nxtFrmStepNo_p,
                     T_ptclFunDsc* funDsc_p,
                     T_snnPara* ssnPar_p
                   )
{
    if ( nxtFrmRlStep_p == NULL
         || nxtFrmStep_p == NULL
         || nxtFrmStepNo_p == NULL
         || funDsc_p == NULL
         || ssnPar_p == NULL
         || ssnPar_p->ASDUID_p == NULL
         || ssnPar_p->usrData_p == NULL
       )
    {
        return ERR_POINTER_NULL;
    }

    u_char act_prio_chnlID = ( u_char ) funDsc_p->act_prio_chnlID;

    //�Բ�������У��
    if (   act_prio_chnlID != ACTIVE_TYPE
           && act_prio_chnlID != PASSIVE_TYPE
       )
    {
        return ERR_ACT_PAS_VAL;
    }

    char reqFlag = getSndReqFlag( funDsc_p );

    APDU104* apdu104_p = NULL;

    //ȡ��Ҫװ֡��apdu����
    //if ( act_prio_chnlID == ACTIVE_TYPE )
    if ( reqFlag == 1 )
    {
        if ( nxtFrmStep_p->reqAPDU_Obj_p == NULL )
        {
            return ERR_POINTER_NULL;
        }

        apdu104_p = ( APDU104* ) nxtFrmStep_p->reqAPDU_Obj_p;
    }
    else
    {
        if ( nxtFrmStep_p->respAPDU_Obj_p == NULL )
        {
            return ERR_POINTER_NULL;
        }

        apdu104_p = ( APDU104* ) nxtFrmStep_p->respAPDU_Obj_p;
    }

    //ȡtid
    ASDU_TID_VAL* TID_p  = NULL;

    //��һ֡�����¿�ʼ��һ��
    //if ( *nxtFrmStepNo_p != nxtFrmStep_p->stepNo)
    //{
    //    TID_p = (ASDU_TID_VAL*) &(ssnPar_p->ASDUID_p[0]);
    //}
    //else
    //{
    //if ( act_prio_chnlID == ACTIVE_TYPE )
    if ( reqFlag == 1 )
    {
        TID_p = ( ASDU_TID_VAL* ) & (
                ssnPar_p->ASDUID_p[nxtFrmStep_p->reqPDU_Num] );
    }
    else
    {
        TID_p = ( ASDU_TID_VAL* ) & (
                ssnPar_p->ASDUID_p[nxtFrmStep_p->respPDU_Num] );
    }

    /* }*/
    u_char tmpTID = * ( ( u_char* ) TID_p );

    //����ԭ��
    ASDU_COT1_VAL* cot1Val_p = NULL;
    ASDU_COT2_VAL* cot2Val_p = NULL;
    //ȡcot
    cot1Val_p   = ( ASDU_COT1_VAL* ) & ( ssnPar_p->COT );

    //136...255 ����Ӧ�ã�ר�÷�Χ��
    //U֡�仯�����Ĺ��ܻỰ
    if ( tmpTID >= MC_TID_136 && tmpTID <= MC_TID_140 )
    {
        if ( cot1Val_p->CAUSE == MC_COT_6 )
        {
            nxtFrmStep_p->stepNo = C_STEP_NO_ONE;
        }
        else if ( cot1Val_p->CAUSE == MC_COT_7 )
        {
            nxtFrmStep_p->stepNo = C_STEP_NO_TWO;
        }
        else
        {
            return ERR_COT_VAL;
        }

        APCI_104 apci104  = {};//104Э���APCI
        apci104.startChar = SRTCHAR_F1_2_V;
        apci104.APDULng   = 4;

        switch ( tmpTID )
        {
            case MC_TID_136:
            {
                if ( cot1Val_p->CAUSE == MC_COT_6 )
                {
                    apci104.ctlField[0] = IEC104_STARTDT_ACT;
                }
                else if ( cot1Val_p->CAUSE == MC_COT_7 )
                {
                    apci104.ctlField[0] = IEC104_STARTDT_ACK;
                }

                break;
            }

            case MC_TID_137:
            {
                if ( cot1Val_p->CAUSE == MC_COT_6 )
                {
                    apci104.ctlField[0] = IEC104_STOPDT_ACT;
                }
                else if ( cot1Val_p->CAUSE == MC_COT_7 )
                {
                    apci104.ctlField[0] = IEC104_STOPDT_ACK;
                }

                break;
            }

            case MC_TID_138:
            {
                if ( cot1Val_p->CAUSE == MC_COT_6 )
                {
                    apci104.ctlField[0] = IEC104_TESTFR_ACT;
                }
                else if ( cot1Val_p->CAUSE == MC_COT_7 )
                {
                    apci104.ctlField[0] = IEC104_TESTFR_ACK;
                }

                break;
            }

            case MC_TID_139:
            {
                if ( cot1Val_p->CAUSE == MC_COT_6 )
                {
                    apci104.ctlField[0] = IEC104_NEWHOLD_ACT;
                }
                else if ( cot1Val_p->CAUSE == MC_COT_7 )
                {
                    apci104.ctlField[0] = IEC104_NEWHOLD_ACK;
                }

                break;
            }

            case MC_TID_140:
            {
                if ( cot1Val_p->CAUSE == MC_COT_6 )
                {
                    apci104.ctlField[0] = IEC104_DELHOLD_ACT;
                }
                else if ( cot1Val_p->CAUSE == MC_COT_7 )
                {
                    apci104.ctlField[0] = IEC104_DELHOLD_ACK;
                }

                break;
            }

            default:
                break;
        }

        u_short duLng = 0;
        u_char* DU_p  = apdu104_p->getDU ( duLng, apdu104_p->getDUType() );
        memcpy ( DU_p, &apci104, sizeof ( APCI_104 ) );

        //������Ϊ0
        APDU_DSCR* apdu_dscr_p = apdu104_p->getDU_Info();

        for ( int i = ASDU_TID_INDEX; i <= ASDU_TSP_INDEX; i++ )
        {
            apdu_dscr_p->ASDU_CMTBL[i] = 0;
        }

        if ( reqFlag == 1 ) //����
        {
            //nxtFrmStep_p->reqAPDU_Obj_p   = (u_char*)apdu104_p;
            nxtFrmStep_p->reqPDU_FCode    = U_F;
        }
        else // (act_prio_chnlID == PASSIVE_TYPE) //����
        {
            //nxtFrmStep_p->respAPDU_Obj_p = (u_char*)apdu104_p;
            nxtFrmStep_p->respPDU_FCode  = U_F;
        }
    }
    else//I֡
    {
        int tRet = 0;
        T_tidObjLng tTidObjLng;
        memset ( &tTidObjLng, 0, sizeof ( tTidObjLng ) );
        tRet = getTidObjLng ( tmpTID,
                              &tTidObjLng
                            );

        if ( tRet != 0 )
        {
            return tRet;
        }

        //ĳ�������е���
        //u_short pntNum  = ssnPar_p->pntNum;
        //ĳ���ĵ�ǰ��λ��
        //u_short pntPos  = ssnPar_p->pntPos;
        //һ֡�����ĵ����:ȡssnPar_p->acsMthd��7λ��ֵ
        //u_char  acsMthd = ( ssnPar_p->acsMthd ) & 0x7f;

        //�ɱ�ṹ���޶���
        ASDU_VSQ_VAL vsqVal = {};
        //ȡssnPar_p->acsMthd���λ��ֵ
        vsqVal.SQ = ( ssnPar_p->acsMthd ) >> 7;
        //ȡssnPar_p->acsMthd��7λ��ֵ
        vsqVal.N = ( ssnPar_p->acsMthd & 0x7f );

        //�ҳ�vsqVal->N
        //u_short tmpNum = pntNum - pntPos;
        //if (acsMthd > 0 && tmpNum > 0)
        //{
        //    //�ҳ�Ҫװ����һ֡��ʵ�ʵ�VSQ_N
        //    //����Ϊ���һ֡���һ֡���п���
        //    //С��ÿһ֡��װ����������
        //    if (tmpNum < acsMthd)
        //    {
        //        vsqVal.N = (u_char)tmpNum;
        //    }
        //    else
        //    {
        //        vsqVal.N = acsMthd;
        //    }
        //}
        //else if (acsMthd == 0 && tmpNum > 0)
        //{
        //    return ERR_ACS_MTHD;
        //}
        //else
        //{
        //    vsqVal.N = 0;
        //}

        //vsqVal.SQ Ϊ1 ʱ֧��vsqVal.N����0
        //vsqVal.SQ Ϊ0 ʱֻ֧��vsqVal.N����1
        /*if( !((vsqVal.SQ == 0 && vsqVal.N ==1)
              ||(vsqVal.SQ == 1 && vsqVal.N >0)
             )
           )
        {
            return ERR_VSQN_SQ;
        }*/

        T_snnUsrData* tUsrData_p = NULL;
        tUsrData_p = & ( ssnPar_p->usrData_p[ssnPar_p->pntPos] );

        //����ʵ�ʵĵ�ֵ��������װ֡������apdu������Ϣ֮Ԫ�ؼ�����
        APDU_DSCR* apdu_dscr_p = apdu104_p->getDU_Info();

        //if ( tUsrData_p->gData.pntType == PNTTYPE_NULL_SET )
        if ( tTidObjLng.cfgLng.tidQdsLng == 0 )
        {
            apdu_dscr_p->ASDU_CMTBL[BODY_SET_INDEX] = 0;
        }
        else
        {
            apdu_dscr_p->ASDU_CMTBL[BODY_SET_INDEX] = vsqVal.N;
        }

        if ( apdu_dscr_p->ASDU_CMTBL[ASDU_COT_INDEX] == 2 )
        {
            cot1Val_p   = NULL;
            cot2Val_p   = ( ASDU_COT2_VAL* ) & ( ssnPar_p->COT );
        }

        //��ϢԪ�ؼ���ÿ����ϢԪ�ص��ֽڳ���
        u_int InfoEleLng = apdu_dscr_p->InfoEleLng;

        //���� ��ʱ���Ӵ�����
        /*if(  InfoEleLng != tTidObjLng.cfgLng.tidQdsLng )
        {
            return ERR_CONFIG;
        }*/
        if ( InfoEleLng != tTidObjLng.cfgLng.tidQdsLng )
        {
            apdu_dscr_p->InfoEleLng = tTidObjLng.cfgLng.tidQdsLng;
            InfoEleLng = tTidObjLng.cfgLng.tidQdsLng;
        }

        //ASDU������ַ
        ASDU_ADDR_VAL* addrVal_p   = ( ASDU_ADDR_VAL* ) & ( ssnPar_p->cmnAddr );

        //ȡ��Ϣ���ַ
        /*u_char* bodAddVal_p = (u_char*)
            &(tUsrData_p->gData.val_chkStd.pntAddr);*/

        //ƴ��vsqVal.N����ֵ��Ϊһ��������Ԫ�ؼ��ռ�
        //pntVal1ֵ�ĳ���
        u_int pntVal1Lng = sizeof ( tUsrData_p->gData.val_chkStd.pntVal1 );
        //20170601 ���Է��ִ˴�����Ҫ���ƶ�ע�͵�
        /*if (InfoEleLng < pntVal1Lng)
        {
        return ERR_INFOELELNG_CFG;
        }*/

        //u_char*    tspVal_p = NULL;//ʱ���׵�ַָ��
        u_char* tspVal_p[MAX104_APDU_LNG] = {0};
        //��ϢԪ�ؼ���ʱ�����ռ�
        u_char  bodSet_p[MAX104_APDU_LNG] = {0};
        u_char  bodAddVal_p[MAX104_APDU_LNG] = {0};
        u_char*    p = NULL;

        //����Ϣ�����ṹ��ASDUֻ��һ����Ϣ�����ֵַ
        if ( vsqVal.SQ == 1 )
        {
            memcpy ( bodAddVal_p,
                     & ( tUsrData_p->gData.val_chkStd.pntAddr ),
                     apdu_dscr_p->ASDU_CMTBL[BODY_ADDR_INDEX]
                   );
        }

        p = bodSet_p;

        for ( int i = 0; i < vsqVal.N; i++ )
        {
            tUsrData_p = & ( ssnPar_p->usrData_p[ssnPar_p->pntPos] );

            //����Ϣ����ṹ��ASDUȡ��Ϣ�����ַ
            if ( vsqVal.SQ == 0 )
            {
                u_short tOneBdAddLng = apdu_dscr_p->ASDU_CMTBL[BODY_ADDR_INDEX];
                memcpy ( bodAddVal_p + ( i * tOneBdAddLng ),
                         & ( tUsrData_p->gData.val_chkStd.pntAddr ),
                         tOneBdAddLng
                       );
            }

            u_int tPntVal1 = tUsrData_p->gData.val_chkStd.pntVal1;
            u_char tQdsVal = * ( ( u_char* ) ( & ( tUsrData_p->gData.dDsc ) ) );

            if ( tTidObjLng.cfgLng.qdsLng > 0
                 && tTidObjLng.qdsMixFlag == 1
               )
            {
                //Ʒ����������ֵ�����һ��:�� ��Ʒ�������ʵĵ�����Ϣ
                tPntVal1 = ( tPntVal1 | tQdsVal );
            }

            //����Ϣ�����ṹ��ASDU����ÿһ����ֵ��Ԫ�ؼ��ռ�
            if ( vsqVal.SQ == 1
                 && tTidObjLng.cfgLng.tidQdsLng > 0
               )
            {

                if ( tTidObjLng.tidValLng <= pntVal1Lng
                     && tTidObjLng.tidValLng > 0
                   )
                {
                    memcpy ( p,
                             & ( tPntVal1 ),
                             pntVal1Lng
                           );
                    p = p + tTidObjLng.tidValLng;
                }
                else if ( tUsrData_p->gData.pntType > pntVal1Lng
                          && tTidObjLng.tidValLng > pntVal1Lng
                        )
                {
                    //���õ���ϢԪ�ؼ���ÿ����ϢԪ�ص���
                    //�ڳ��� ���� 2�� siseof(u_int)
                    //if ( InfoEleLng > pntVal1Lng * 2 )
                    if ( tTidObjLng.tidValLng > pntVal1Lng * 2 )
                    {
                        memcpy ( p,
                                 & ( tUsrData_p->gData.val_chkStd.pntVal2 ),
                                 pntVal1Lng
                               );
                        p = p + pntVal1Lng;
                    }
                    else
                    {
                        memcpy ( p,
                                 & ( tUsrData_p->gData.val_chkStd.pntVal2 ),
                                 tTidObjLng.tidValLng - pntVal1Lng
                               );
                        p = p + tTidObjLng.tidValLng - pntVal1Lng;
                    }
                }

                //Ʒ��������
                if ( tTidObjLng.cfgLng.qdsLng > 0
                     && tTidObjLng.qdsMixFlag == 0
                   )
                {
                    memcpy ( p,
                             & ( tQdsVal ),
                             tTidObjLng.cfgLng.qdsLng
                           );
                    p = p + tTidObjLng.cfgLng.qdsLng;
                }
            }
            else if ( vsqVal.SQ == 0
                      && tTidObjLng.cfgLng.tidQdsLng > 0
                    )
            {
                //����Ϣ����ṹ��ASDU����ÿһ����Ϣ����Ԫ��


                //p = p + pntVal1Lng;
                if ( tTidObjLng.tidValLng <= pntVal1Lng
                     && tTidObjLng.tidValLng > 0
                   )
                {
                    memcpy ( p,
                             & ( tPntVal1 ),
                             pntVal1Lng
                           );
                    p = p + tTidObjLng.tidValLng;
                }
                else if ( tUsrData_p->gData.pntType > pntVal1Lng
                          && tTidObjLng.tidValLng > pntVal1Lng
                        )
                {
                    //���õ���ϢԪ�ؼ���ÿ����ϢԪ�ص��ֽ�
                    //���� ���� 2�� siseof(u_int)
                    if ( tTidObjLng.tidValLng > pntVal1Lng * 2 )
                    {
                        memcpy ( p + pntVal1Lng,
                                 & ( tUsrData_p->gData.val_chkStd.pntVal2 ),
                                 pntVal1Lng
                               );
                        p = p + pntVal1Lng;
                    }
                    else
                    {
                        memcpy ( p + pntVal1Lng,
                                 & ( tUsrData_p->gData.val_chkStd.pntVal2 ),
                                 tTidObjLng.tidValLng - pntVal1Lng
                               );
                        p = p + tTidObjLng.tidValLng - pntVal1Lng;
                    }
                }

                //Ʒ��������
                if ( tTidObjLng.cfgLng.qdsLng > 0
                     && tTidObjLng.qdsMixFlag == 0
                   )
                {
                    memcpy ( p,
                             & ( tQdsVal ),
                             tTidObjLng.cfgLng.qdsLng
                           );
                    p = p + tTidObjLng.cfgLng.qdsLng;
                }

                //p = p + InfoEleLng;
            }

            //������Ϣ�����ṹ��ASDUȡʱ�ֻ꣨�����һ֡����
            //��һ�����ӦtUsrData_p->gData.tmStmp_p
            if ( vsqVal.SQ == 1
                 && i == ( vsqVal.N - 1 )
                 && tUsrData_p->gData.tmStmp_p != NULL
                 && tUsrData_p->gData.tmStmpType != 0
               )
            {
                if ( tUsrData_p->gData.tmStmpType == TIME_TYPE_56_2A )
                {
                    apdu_dscr_p->ASDU_CMTBL[BODY_TSP_INDEX] = sizeof (
                            CP56TIME2a );
                }
                else if ( tUsrData_p->gData.tmStmpType == TIME_TYPE_24_2A )
                {
                    apdu_dscr_p->ASDU_CMTBL[BODY_TSP_INDEX] = sizeof (
                            CP24TIME2a );
                }
                else if ( tUsrData_p->gData.tmStmpType > 0 )
                {
                    return ERR_TIME_TYPE;//δ֪��ʱ������
                }

                /*tspVal_p = (u_char*)
                    tUsrData_p->gData.tmStmp_p;*/
                memcpy ( tspVal_p,
                         ( u_char* ) & ( tUsrData_p->gData.tmStmp_p ),
                         apdu_dscr_p->ASDU_CMTBL[BODY_TSP_INDEX]
                       );
            }
            else if ( vsqVal.SQ == 0
                      && tUsrData_p->gData.tmStmp_p != NULL
                      && tUsrData_p->gData.tmStmpType != 0
                    )
            {
                //������Ϣ�����ṹ��ASDUȡʱ��ÿһ��������һ��ʱ��
                if ( tUsrData_p->gData.tmStmpType == TIME_TYPE_56_2A )
                {
                    apdu_dscr_p->ASDU_CMTBL[BODY_TSP_INDEX] = sizeof (
                            CP56TIME2a );
                }
                else if ( tUsrData_p->gData.tmStmpType == TIME_TYPE_24_2A )
                {
                    apdu_dscr_p->ASDU_CMTBL[BODY_TSP_INDEX] = sizeof (
                            CP24TIME2a );
                }
                else if ( tUsrData_p->gData.tmStmpType > 0 )
                {
                    return ERR_TIME_TYPE;//δ֪��ʱ������
                }

                u_short tTspLng = apdu_dscr_p->ASDU_CMTBL[BODY_TSP_INDEX];
                memcpy ( tspVal_p + ( i * tTspLng ),
                         ( u_char* ) & ( tUsrData_p->gData.tmStmp_p ),
                         tTspLng
                       );
            }

            ssnPar_p->pntPos ++;
        }

        u_short bodSetLng = 0;

        if ( vsqVal.SQ == 1 )
        {
            //Ԫ�ؼ�����
            bodSetLng = p - bodSet_p;
        }
        else
        {
            //�����ṹ�У�һ������ֻ��һ����ϢԪ��
            bodSetLng = InfoEleLng;
        }

        //���֡�����Ƿ񳬹�Э��涨���ֵ
        u_char duType = apdu104_p->getDUType();
        u_short duLng = 0;
        apdu104_p->getDU ( duLng, duType );

        if ( ( duType == APDU_TYPE && duLng > MAX104_APDU_LNG )
             || ( duType == ASDU_TYPE && duLng > MAX104_ASDU_LNG )
           )
        {
            return ERR_FRAME_LNG;
        }

        u_char tBodyObjNum = 1;

        if ( vsqVal.SQ == 0 /*&& vsqVal.N > tBodyObjNum*/ )
        {
            tBodyObjNum = vsqVal.N;
        }

        //����ֵ��װASDU
        tRet = packASDU ( apdu104_p,
                          TID_p,
                          &vsqVal,
                          cot1Val_p,
                          cot2Val_p,
                          addrVal_p,
                          bodAddVal_p,
                          bodSet_p,
                          bodSetLng,
                          tspVal_p,
                          tBodyObjNum
                        );

        if ( tRet < 0 )
        {
            return tRet;
        }

        //��������һ֡��Ӧ����ֵ
        nxtFrmStep_p->stepNo = nxtFrmRlStep_p->stepNo;

        if (   nxtFrmStep_p->sVerifyBlock_p != NULL
               && nxtFrmRlStep_p->sVerifyBlock_p != NULL
           )
        {
            * ( nxtFrmStep_p->sVerifyBlock_p ) = * (
                                                 nxtFrmRlStep_p->sVerifyBlock_p );
        }

        if ( reqFlag == 1 ) 
        {
            //nxtFrmStep_p->reqAPDU_Obj_p = (u_char*)apdu104_p;
            nxtFrmStep_p->reqPDU_FCode   = nxtFrmRlStep_p->reqPDU_FCode;
            //ע����ֵ���ϲ��н��иı䲻�ڴ˸ı�
            //nxtFrmStep_p->reqPDU_Num     = nxtFrmStep_p->reqPDU_Num + 1;

        }
        else // (act_prio_chnlID == PASSIVE_TYPE) 
        {
            //nxtFrmStep_p->respAPDU_Obj_p = (u_char*)apdu104_p;
            nxtFrmStep_p->respPDU_FCode  = nxtFrmRlStep_p->respPDU_FCode;
            //ע����ֵ���ϲ��н��иı䲻�ڴ˸ı�
            //nxtFrmStep_p->respPDU_Num    = nxtFrmStep_p->respPDU_Num + 1;
        }
    }//end I֡

    return 0;
}


//����reqFlag��ʶ����򲽱Ƚ��������Ӧ֡
int compFrmStep ( const u_char& reqFlag,
                  ONE_STEP*& lastRuleStep_p,
                  ONE_STEP*& nxtRuleStep_p,
                  ONE_STEP*& nowStep_p,
                  const u_char& allFrameNum_req,
                  const u_char& allFrameNum_resp,
                  const u_char& nowFrameNum_req,
                  const u_char& nowFrameNum_resp,
                  u_char*& nxtStepNo_p,
                  u_int*& nxtStepStat_p
                )
{
    if ( reqFlag == 1 ) //�Ƚ�����֡
    {
        if ( nowFrameNum_req < allFrameNum_req - 1 )
        {
            //����֡δ�ﵽ�޶�ֵ
            //��������ִ��
            *nxtStepStat_p = MC_SEND_CNUE_EXEC;
        }
        else if ( nowFrameNum_req == allFrameNum_req - 1 )
        {
            //��Ҫװ������֡�ﵽ�޶�ֵ
            //��Ӧ֡��δ�ﵽ�޶�ֵ
            if ( nowFrameNum_resp < allFrameNum_resp )
            {
                //���͵ȴ�����
                *nxtStepStat_p = MC_SEND_WAIT_CHECK;
            }
            else if ( nowFrameNum_resp == allFrameNum_resp
                      || nowFrameNum_resp == allFrameNum_resp + 1
                    ) //��Ӧ֡�ﵽ���޶�ֵ
            {
                //ͨ�������ж��Ƿ�Ϊ���һ��
                if ( nowStep_p->stepNo == lastRuleStep_p->stepNo )
                {
                    //�����һ��
                    //���ͻỰ���
                    *nxtStepStat_p = MC_SEND_SSN_FNSH;
                }
                else//�������һ��
                {
                    *nxtStepNo_p = nxtRuleStep_p->stepNo;

                    if ( nxtRuleStep_p->reqPDU_Num > 0 )
                    {
                        //��һ������֡������0
                        //��������ִ��  ����Ҫ��������������
                        *nxtStepStat_p = MC_SEND_CNUE_EXEC;
                    }
                    else//��һ������֡����0����Ӧ֡������0
                    {
                        //���͵ȴ�����
                        *nxtStepStat_p = MC_SEND_WAIT_CHECK;
                    }
                }
            }
            else//��Ӧ֡����
            {
                //�����ͻỰ��ֹ
                *nxtStepStat_p = MC_NO_SEND_SSN_STOP;
            }
        }
        else if ( nowFrameNum_req == allFrameNum_req )
        {
            //�Ѿ����޶�ֵ�˲���Ҫ����֡��
            if ( nowFrameNum_resp < allFrameNum_resp )
            {
                //��Ӧ֡��δ�ﵽ�޶�ֵ
                //�����͵ȴ�����
                *nxtStepStat_p = MC_NO_SEND_WAIT_CHECK;
            }
            else if ( nowFrameNum_resp == allFrameNum_resp
                      || nowFrameNum_resp == allFrameNum_resp + 1
                    ) //��Ӧ֡�ﵽ���޶�ֵ
            {
                //ͨ�������ж��Ƿ�Ϊ���һ��
                if ( nowStep_p->stepNo == lastRuleStep_p->stepNo )
                {
                    //�����һ��
                    //�����ͻỰ���
                    *nxtStepStat_p = MC_NO_SEND_SSN_FNSH;
                }
                else//�������һ��
                {
                    *nxtStepNo_p = nxtRuleStep_p->stepNo;

                    if ( nxtRuleStep_p->reqPDU_Num > 0 )
                    {
                        //��һ������֡������0
                        //����������ִ��  ����Ҫ��������������
                        *nxtStepStat_p = MC_NO_SEND_CNUE_EXEC;
                    }
                    else//��һ������֡����0����Ӧ֡������0
                    {
                        //�����͵ȴ�����
                        *nxtStepStat_p = MC_NO_SEND_WAIT_CHECK;
                    }
                }
            }
            else//��Ӧ֡����
            {
                //�����ͻỰ��ֹ
                *nxtStepStat_p = MC_NO_SEND_SSN_STOP;
            }
        }
        else//����֡����nowFrameNum_req>allFrameNum_req
        {
            //�����ͻỰ��ֹ
            *nxtStepStat_p = MC_NO_SEND_SSN_STOP;
        }
    }
    else ////////////////////////////////////////////////////�Ƚ���Ӧ֡
    {
        if ( nowFrameNum_resp < allFrameNum_resp - 1 )
        {
            //��Ӧ֡δ�ﵽ�޶�ֵ
            //��������ִ��
            *nxtStepStat_p = MC_SEND_CNUE_EXEC;
        }
        else if ( nowFrameNum_resp == allFrameNum_resp - 1 )
        {
            //��Ҫװ����Ӧ֡�ﵽ�޶�ֵ
            //����֡��δ�ﵽ�޶�ֵ
            if ( nowFrameNum_req < allFrameNum_req )
            {
                //���͵ȴ�����
                *nxtStepStat_p = MC_SEND_WAIT_CHECK;
            }
            else if ( nowFrameNum_req == allFrameNum_req
                      || nowFrameNum_req == allFrameNum_req + 1
                    ) //����֡�ﵽ���޶�ֵ
            {
                //ͨ�������ж��Ƿ�Ϊ���һ��
                if ( nowStep_p->stepNo == lastRuleStep_p->stepNo )
                {
                    //�����һ��
                    //���ͻỰ���
                    *nxtStepStat_p = MC_SEND_SSN_FNSH;
                }
                else//�������һ��
                {
                    *nxtStepNo_p = nxtRuleStep_p->stepNo;

                    if ( nxtRuleStep_p->respPDU_Num > 0 )
                    {
                        //��һ����Ӧ֡������0
                        if ( nxtRuleStep_p->reqPDU_Num > 0 )
                        {
                            //��һ������֡����Ҳ����0
                            //���ڱ���վ��˵���ݶ�����Ϊ���ȵȴ���
                            //  ������֡�ٷ����Լ�����Ӧ֡
                            //���͵ȴ�����  ����Ҫ��������������
                            *nxtStepStat_p = MC_SEND_WAIT_CHECK;
                        }
                        else
                        {
                            //��������ִ��  ����Ҫ��������������
                            *nxtStepStat_p = MC_SEND_CNUE_EXEC;
                        }
                    }
                    else//��һ����Ӧ֡����0������֡������0
                    {
                        //���͵ȴ�����
                        *nxtStepStat_p = MC_SEND_WAIT_CHECK;
                    }
                }
            }
            else//����֡����
            {
                //�����ͻỰ��ֹ
                *nxtStepStat_p = MC_NO_SEND_SSN_STOP;
            }
        }
        else if ( nowFrameNum_resp == allFrameNum_resp )
        {
            //�Ѿ����޶�ֵ�˲���Ҫ����֡��
            if ( nowFrameNum_req < allFrameNum_req )
            {
                //����֡��δ�ﵽ�޶�ֵ
                //�����͵ȴ�����
                *nxtStepStat_p = MC_NO_SEND_WAIT_CHECK;
            }
            else if ( nowFrameNum_req == allFrameNum_req
                      || nowFrameNum_req == allFrameNum_req + 1
                    ) //����֡�ﵽ���޶�ֵ
            {
                //ͨ�������ж��Ƿ�Ϊ���һ��
                if ( nowStep_p->stepNo == lastRuleStep_p->stepNo )
                {
                    //�����һ��
                    //�����ͻỰ���
                    *nxtStepStat_p = MC_NO_SEND_SSN_FNSH;
                }
                else//�������һ��
                {
                    *nxtStepNo_p = nxtRuleStep_p->stepNo;

                    if ( nxtRuleStep_p->respPDU_Num > 0 )
                    {
                        //��һ����Ӧ֡������0
                        if ( nxtRuleStep_p->reqPDU_Num > 0 )
                        {
                            //��һ������֡����Ҳ����0
                            //���ڱ���վ��˵���ݶ�����Ϊ��
                            //  �ȵȴ���������֡�ٷ����Լ�����Ӧ֡
                            //�����͵ȴ�����  ����Ҫ��������������
                            *nxtStepStat_p = MC_NO_SEND_WAIT_CHECK;
                        }
                        else
                        {
                            //����������ִ��  ����Ҫ��������������
                            *nxtStepStat_p = MC_NO_SEND_CNUE_EXEC;
                        }
                    }
                    else//��һ����Ӧ֡����0������֡������0
                    {
                        //�����͵ȴ�����
                        *nxtStepStat_p = MC_NO_SEND_WAIT_CHECK;
                    }
                }
            }
            else//����֡����
            {
                //�����ͻỰ��ֹ
                *nxtStepStat_p = MC_NO_SEND_SSN_STOP;
            }
        }
        else//��Ӧ֡����nowFrameNum_resp>allFrameNum_resp
        {
            //�����ͻỰ��ֹ
            *nxtStepStat_p = MC_NO_SEND_SSN_STOP;
        }
    }

    return 0;
}


/******************************************************************************
 *���ܣ������һ֡�Ự״̬,�������һ֡����
 * ���룺
 *      Fun_p        ��������
 *      stepRl_p     ����ǰ֡��Ӧ�Ĳ�
 *      funDsc_p     ���Ự����������
 *�����
 *      nxtStepStat_p����һ֡�Ự״̬
 *      nxtStepNo_p  ����һ֡����
 *���أ�
 *      >0 �Ự״̬
 *      <0 �ӿ��쳣
 *�޸���ʷ
 *
*******************************************************************************/
int getNxtFrmSnnStatus ( FUN_RL* Fun_p,
                         ONE_STEP* stepRl_p,
                         T_ptclFunDsc* funDsc_p,
                         u_int* nxtStepStat_p,
                         u_char* nxtStepNo_p
                       )
{

    if ( Fun_p == NULL
         || Fun_p->steps_p[Fun_p->usingMode] == NULL
         || stepRl_p == NULL
         || funDsc_p == NULL
         || nxtStepStat_p == NULL
       )
    {
        return ERR_POINTER_NULL;
    }

    //��ǰ��ָ��
    ONE_STEP* nowStep_p     = stepRl_p;
    //��ǰ���Ĺ���ָ��
    ONE_STEP* nowRuleStep_p = displayNextNode (
                              Fun_p->steps_p[Fun_p->usingMode],
                              nowStep_p
                              );

    if ( nowRuleStep_p == NULL )
    {
        return ERR_POINTER_NULL;
    }

    //�ҵ��������һ��
    ONE_STEP* lastRuleStep_p = Fun_p->steps_p[Fun_p->usingMode];

    while ( lastRuleStep_p->nextStep_p != NULL )
    {
        lastRuleStep_p    = lastRuleStep_p->nextStep_p;
    }

    char reqFlag = getSndReqFlag( funDsc_p );

    //��ǰ���򲽵���һ��
    ONE_STEP* nxtRuleStep_p = nowRuleStep_p->nextStep_p;
    //�������Ա�ʶ
    u_char actFlag = ( u_char ) funDsc_p->act_prio_chnlID;

    //����������֡�������޶�ֵ
    u_char allFrameNum_req  = nowRuleStep_p->reqPDU_Num;
    //��������Ӧ֡�������޶�ֵ
    u_char allFrameNum_resp = nowRuleStep_p->respPDU_Num;
    //��ǰ����ֵ֡���ǲ��ĵڼ�֡��
    u_char nowFrameNum_req  = nowStep_p->reqPDU_Num;
    //��ǰ��Ӧֵ֡���ǲ��ĵڼ�֡��
    u_char nowFrameNum_resp = nowStep_p->respPDU_Num;

    //��һ֡���ݵĲ��ų�ʼ��Ϊ��Ĭ�ϵ�ǰ���Ĳ���
    *nxtStepNo_p = nowStep_p->stepNo;

    compFrmStep ( reqFlag,
        lastRuleStep_p,
        nxtRuleStep_p,
        nowStep_p,
        allFrameNum_req,
        allFrameNum_resp,
        nowFrameNum_req,
        nowFrameNum_resp,
        nxtStepNo_p,
        nxtStepStat_p
        );

    if( funDsc_p->chnlRole == MASTER_STATION 
        && actFlag == PASSIVE_TYPE
      )
    {
        if ( *nxtStepStat_p == MC_SEND_SSN_FNSH )
        {
            *nxtStepStat_p = MC_NO_SEND_SSN_FNSH;
        }
    }

    //switch ( actFlag )
    //{
    //    case ACTIVE_TYPE:
    //    {
    //        //����
    //        if ( funDsc_p->ssnDscNo != ACQ_EVT )
    //        {
    //            compFrmStep ( 1,
    //                          lastRuleStep_p,
    //                          nxtRuleStep_p,
    //                          nowStep_p,
    //                          allFrameNum_req,
    //                          allFrameNum_resp,
    //                          nowFrameNum_req,
    //                          nowFrameNum_resp,
    //                          nxtStepNo_p,
    //                          nxtStepStat_p
    //                        );


    //        }
    //        else //�����ͻ���ػ�
    //        {
    //            compFrmStep ( 1,
    //                          lastRuleStep_p,
    //                          nxtRuleStep_p,
    //                          nowStep_p,
    //                          allFrameNum_req,
    //                          allFrameNum_resp,
    //                          nowFrameNum_req,
    //                          nowFrameNum_resp,
    //                          nxtStepNo_p,
    //                          nxtStepStat_p
    //                        );
    //        }

    //        break;
    //    }

    //    case PASSIVE_TYPE://����
    //    {
    //        if ( funDsc_p->ssnDscNo != ACQ_EVT )
    //        {
    //            compFrmStep ( 0,
    //                lastRuleStep_p,
    //                nxtRuleStep_p,
    //                nowStep_p,
    //                allFrameNum_req,
    //                allFrameNum_resp,
    //                nowFrameNum_req,
    //                nowFrameNum_resp,
    //                nxtStepNo_p,
    //                nxtStepStat_p
    //                );
    //        }
    //        else //�����ͻ���ػ�
    //        {
    //            compFrmStep ( 1,
    //                lastRuleStep_p,
    //                nxtRuleStep_p,
    //                nowStep_p,
    //                allFrameNum_req,
    //                allFrameNum_resp,
    //                nowFrameNum_req,
    //                nowFrameNum_resp,
    //                nxtStepNo_p,
    //                nxtStepStat_p
    //                );

    //            if ( *nxtStepStat_p == MC_SEND_SSN_FNSH )
    //            {
    //                *nxtStepStat_p = MC_NO_SEND_SSN_FNSH;
    //            }
    //        }

    //        break;
    //    }

    //    default:
    //        break;
    //}

    return 0;
}




/******************************************************************************
 *���ܣ����ղ�����һ֡����,�洢�����ر�ʶ
 * ���룺
 *      Fun_p        ��������
 *      stepRl_p     ����ǰ֡��Ӧ�Ĳ�
 *      funDsc_p     ���Ự����������
 *�����
 *      stnAcsFlag_p ���洢�����ر�ʶ
 *      nxtStepNo_p  ����һ֡����
 *���أ�
 *      0  �ɹ�
 *      <0 �ӿ��쳣
 *�޸���ʷ
 *
*******************************************************************************/
int getChkNxtStepNo ( const FUN_RL* Fun_p,
                      const ONE_STEP* stepRl_p,
                      const T_ptclFunDsc* funDsc_p,
                      T_stnAcsFlag* stnAcsFlag_p,
                      u_int* nxtStepNo_p
                    )
{
    if ( Fun_p == NULL
         || Fun_p->steps_p == NULL
         || Fun_p->steps_p[Fun_p->usingMode] == NULL
         || stepRl_p == NULL
         || funDsc_p == NULL
         || stnAcsFlag_p == NULL
         || nxtStepNo_p == NULL
       )
    {
        return ERR_POINTER_NULL;
    }

    //��ȡ�����е�ǰ����ָ��;
    ONE_STEP* nowRuleStep_p = displayNextNode (
                              Fun_p->steps_p[Fun_p->usingMode],
                              stepRl_p );

    if ( nowRuleStep_p == NULL )
    {
        return ERR_POINTER_NULL;
    }

    //��ǰ��ָ��
    const ONE_STEP* nowStep_p     = stepRl_p;

    //�ҵ��������һ��
    /*ONE_STEP* lastRuleStep_p = Fun_p->steps_p[Fun_p->usingMode];
    while(lastRuleStep_p->nextStep_p != NULL)
    {
        lastRuleStep_p    = lastRuleStep_p->nextStep_p;
    }*/

    //��ǰ���Ĳ��űȹ��򲽵Ĵ����쳣
    if ( nowStep_p->stepNo > nowRuleStep_p->stepNo )
    {
        return ERR_STEP_NO;
    }

    //��ǰ���򲽵���һ��
    ONE_STEP* nxtRuleStep_p = nowRuleStep_p->nextStep_p;

    //����������֡�������޶�ֵ
    u_char allFrameNum_req  = nowRuleStep_p->reqPDU_Num;

    //��ǰ����ֵ֡���ǲ��ĵڼ�֡��
    u_char nowFrameNum_req  = nowStep_p->reqPDU_Num;

    //��������Ӧ֡�������޶�ֵ
    u_char allFrameNum_resp = nowRuleStep_p->respPDU_Num;

    //��ǰ��Ӧֵ֡���ǲ��ĵڼ�֡��
    u_char nowFrameNum_resp = nowStep_p->respPDU_Num;

    T_stnAcsFlag tmpStnAcsFlag = {0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0};
    *stnAcsFlag_p = tmpStnAcsFlag;

    *nxtStepNo_p  = nowStep_p->stepNo;

    //�������Ự��ʶ
    //u_char actFlag = ( u_char ) funDsc_p->act_prio_chnlID;

    char reqFlag = getRcvReqFlag( funDsc_p );

    switch ( reqFlag )
    {
        case 1://��������
        {
            /*if ( funDsc_p->ssnDscNo != ACQ_EVT )
            {*/
            if ( nowFrameNum_req == allFrameNum_req )
            {
                //����֡�ﵽ�޶�ֵ
                if ( nowFrameNum_resp == allFrameNum_resp )
                {
                    //��Ӧ֡Ҳ�ﵽ�޶�ֵ
                    if ( nxtRuleStep_p != NULL )
                    {
                        //�����ǰ���������һ��
                        *nxtStepNo_p = nxtRuleStep_p->stepNo;
                        //��һ��ϵͳִ������ֵ�ļ���
                        stnAcsFlag_p->ctlType = 2;
                    }
                }
                else if ( nowFrameNum_resp > allFrameNum_resp )
                {
                    //��Ӧ֡����
                    return ERR_FRAME_NO;
                }
            }
            else if ( nowFrameNum_req > allFrameNum_req )
            {
                //����֡����
                return ERR_FRAME_NO;
            }
            //}
            //else
            //{
            //    if ( nowFrameNum_resp == allFrameNum_resp )
            //    {
            //        //��Ӧ֡�ﵽ�޶�ֵ
            //        if ( nowFrameNum_req == allFrameNum_req )
            //        {
            //            //����֡Ҳ�ﵽ�޶�ֵ
            //            if ( nxtRuleStep_p != NULL )
            //            {
            //                //�����ǰ���������һ��
            //                *nxtStepNo_p = nxtRuleStep_p->stepNo;
            //                //��һ��ϵͳִ������ֵ�ļ���
            //                stnAcsFlag_p->ctlType = 2;
            //            }
            //        }
            //        else if ( nowFrameNum_req > allFrameNum_req )
            //        {
            //            //����֡����
            //            return ERR_FRAME_NO;
            //        }
            //    }
            //}

            break;
        }

        case 0: //������Ӧ
        {
            //�����Ự���ձ���վ����
            /*if (funDsc_p->ssnDscNo!= ACQ_EVT)
            {*/
            if ( nowFrameNum_resp == allFrameNum_resp )
            {
                //��Ӧ֡�ﵽ�޶�ֵ
                if ( nowFrameNum_req == allFrameNum_req )
                {
                    //����֡Ҳ�ﵽ�޶�ֵ
                    if ( nxtRuleStep_p != NULL )
                    {
                        //�����ǰ���������һ��
                        *nxtStepNo_p = nxtRuleStep_p->stepNo;
                        //��һ��ϵͳִ������ֵ�ļ���
                        stnAcsFlag_p->ctlType = 2;
                    }
                }
                else if ( nowFrameNum_req > allFrameNum_req )
                {
                    //����֡����
                    return ERR_FRAME_NO;
                }
            }
            else if ( nowFrameNum_resp > allFrameNum_resp )
            {
                //��Ӧ֡����
                return ERR_FRAME_NO;
            }

            /*}
            else
            {
                if (nowFrameNum_resp == allFrameNum_resp)
                {//��Ӧ֡�ﵽ�޶�ֵ
                    if (nowFrameNum_req == allFrameNum_req)
                    {//����֡Ҳ�ﵽ�޶�ֵ
                        if (nxtRuleStep_p != NULL)
                        {//�����ǰ���������һ��
                            *nxtStepNo_p = nxtRuleStep_p->stepNo;
                            //��һ��ϵͳִ������ֵ�ļ���
                            stnAcsFlag_p->ctlType=2;
                        }
                    }
                    else if (nowFrameNum_req > allFrameNum_req)
                    {//����֡����
                        return ERR_FRAME_NO;
                    }
                }
            }*/
            break;
        }

        default:
            break;
    }

    return 0;
}




/******************************************************************************
 *���ܣ�����eleVal_p��ֵ��snnCfg_p��ֵ��snnCfg_p.p_Mem�ڽӿ�������new�ռ䣩
 * ���룺
 *      actChnlID     ����������ʶ
 *      stepNum       ������stepNum����ֵ
 *      frmNum        ������frmNum֡��ֵ
 *      eleVal_p      ��ELE_TYPE [11]���ͱ�����ȡֵ����
 *      T_snnCfg      ����Ҫ��ֵ�Ľṹ

 *�����
 *      T_snnCfg      ����ֵ��Ľṹ
 *���أ�
 *      0  �ɹ�
 *      <0 �ӿ��쳣
 *�޸���ʷ
 *    20180223151731��T_snnCfg�ṹ��仯�����ڲ��ӿ���ʵ����;������ע���ڲ��߼�
*******************************************************************************/
int setOneFrmCfg ( u_char actChnlID,
                   u_char stepNum,
                   u_char frmNum,
                   ELE_TYPE* eleVal_p,
                   T_snnCfg* snnCfg_p
                 )
{
    //if (actChnlID != ACTIVE_TYPE
    //    && actChnlID != PASSIVE_TYPE
    //    )
    //{
    //    return ERR_ACT_PAS_VAL;
    //}

    //if ( stepNum < 1
    //    || frmNum < 1
    //    )
    //{
    //    return ERR_STEP_NO;
    //}

    //if (eleVal_p == NULL
    //    || snnCfg_p == NULL
    //    || snnCfg_p->stepCfg_p == NULL
    //    )
    //{
    //    return ERR_POINTER_NULL;
    //}

    //if (actChnlID == ACTIVE_TYPE)
    //{
    //    if (snnCfg_p->stepCfg_p[stepNum-1].reqEleVal_pp == NULL
    //        || snnCfg_p->stepCfg_p[stepNum-1].reqEleVal_pp[frmNum-1] == NULL
    //        )
    //    {
    //        return ERR_POINTER_NULL;
    //    }
    //    for (int i = 0;i <= ASDU_TSP_INDEX;i++)
    //    {
    //        if (eleVal_p[i].lng > 0)
    //        {
    //            ELE_TYPE* tmp_p = &(
    //            snnCfg_p->stepCfg_p[stepNum-1].reqEleVal_pp[frmNum-1][i]);

    //            tmp_p->sysEleID = eleVal_p[i].sysEleID;
    //            tmp_p->lng = eleVal_p[i].lng;
    //            tmp_p->p_Mem = new u_char[eleVal_p[i].lng]();
    //            memcpy(tmp_p->p_Mem,
    //                   eleVal_p[i].p_Mem,
    //                   eleVal_p[i].lng
    //                   );
    //        }
    //    }
    //}
    //else //if (actChnlID ==PASSIVE_TYPE)
    //{
    //    if (snnCfg_p->stepCfg_p[stepNum-1].respEleVal_pp == NULL
    //        || snnCfg_p->stepCfg_p[stepNum-1].respEleVal_pp[frmNum-1] == NULL
    //        )
    //    {
    //        return ERR_POINTER_NULL;
    //    }
    //    for (int i = 0;i <= ASDU_TSP_INDEX;i++)
    //    {
    //        if (eleVal_p[i].lng > 0)
    //        {
    //            ELE_TYPE* tmp_p = &(
    //               snnCfg_p->stepCfg_p[stepNum-1].respEleVal_pp[frmNum-1][i]);
    //            tmp_p->sysEleID = eleVal_p[i].sysEleID;
    //            tmp_p->lng = eleVal_p[i].lng;
    //            tmp_p->p_Mem = new u_char[eleVal_p[i].lng]();
    //            memcpy(tmp_p->p_Mem,
    //                eleVal_p[i].p_Mem,
    //                eleVal_p[i].lng
    //                );
    //        }
    //    }
    //
    //}

    return 0;
}




/*******************************************************************************
  *���ܣ�
  *   ��nxtStep��U֡�Ự��������
*******************************************************************************/
int asgmtNxtUPar ( u_char srvNo,
                   ONE_STEP* stepRl_p,
                   T_ptclFunDsc* funDsc_p,
                   T_snnPara* ssnPar_p,
                   u_char& nxtFrmStepNo,
                   u_int* nxtStepStat_p
                 )
{
    if ( stepRl_p == NULL
         || funDsc_p == NULL
         || nxtStepStat_p == NULL
         || ssnPar_p == NULL
         || ssnPar_p->ASDUID_p == NULL
       )
    {
        return ERR_POINTER_NULL;
    }

    u_char nowStepNum    = stepRl_p->stepNo;
    u_char nowRespFrmNum = stepRl_p->respPDU_Num;
    u_char nowReqFrmNum  = stepRl_p->reqPDU_Num;

    nxtFrmStepNo = stepRl_p->stepNo;

    char reqFlag = getSndReqFlag( funDsc_p );

    //if ( ( u_char ) funDsc_p->act_prio_chnlID == ACTIVE_TYPE )
    
    if ( reqFlag == 1 )
    {
        if ( nowStepNum == C_STEP_NO_ONE )
        {
            //�����͵ȴ�����
            *nxtStepStat_p = MC_NO_SEND_WAIT_CHECK;
        }
        else//C_STEP_NO_TWO
        {
            if ( nowRespFrmNum < 1 )
            {
                //�����͵ȴ�����
                *nxtStepStat_p = MC_NO_SEND_WAIT_CHECK;
            }
            else
            {
                //�����ͻỰ���
                *nxtStepStat_p = MC_NO_SEND_SSN_FNSH;
            }
        }
    }
    else //SLAVE_STATION
    {
        if ( nowStepNum == C_STEP_NO_ONE )
        {
            if ( nowReqFrmNum < 1 )
            {
                //�����͵ȴ�����
                *nxtStepStat_p = MC_NO_SEND_WAIT_CHECK;
            }
            else
            {
                //����������ִ��  ����Ҫ��������������
                *nxtStepStat_p = MC_NO_SEND_CNUE_EXEC;
                nxtFrmStepNo   = C_STEP_NO_TWO;
            }
        }
        else//C_STEP_NO_TWO
        {
            if ( nowRespFrmNum < 1 )
            {
                //���ͻỰ���
                *nxtStepStat_p = MC_SEND_SSN_FNSH;
            }
            else
            {
                //�����ͻỰ���
                *nxtStepStat_p = MC_NO_SEND_SSN_FNSH;
            }
        }
    }//end PASSIVE_TYPE

    if ( *nxtStepStat_p == MC_SEND_SSN_FNSH )
    {
        ssnPar_p->COT = MC_COT_7;

        switch ( srvNo )
        {
            case U_STARTDT:
                //104Э��U֡STARTDT ����ĻỰ
                ssnPar_p->ASDUID_p[0] = MC_TID_136;
                break;

            case U_STOPDT:
                //104Э��U֡STOPDT ����ĻỰ
                ssnPar_p->ASDUID_p[0] = MC_TID_137;
                break;

            case U_TESTFR:
                //104Э��U֡TESTFR ����ĻỰ
                ssnPar_p->ASDUID_p[0] = MC_TID_138;
                break;

            case U_NEWHOLD:
                /*��U֡��STARTDT STOPDT TESTFR
                  ȫΪ0 ��ʾ���µĻỰ������һ��ͨ��
                */
                ssnPar_p->ASDUID_p[0] = MC_TID_139;
                break;

            case U_DELHOLD:
                /*��U֡��STARTDT STOPDT TESTFR ȫ
                 Ϊ1 ��ʾ���µĻỰ����һ��ͨ��
                */
                ssnPar_p->ASDUID_p[0] = MC_TID_140;
                break;

            default:
                break;
        }

        int tmpRet = asgmtNxtFrmPar (
                     stepRl_p,
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

    return 0;
}




/*******************************************************************************
  *���ܣ�
  *    У�����ݵ�Ԫ��ʶ��
  *
  * ���룺
  *     nowRuleStep_p :��ǰ������
  *     lastRuleStep_p:���򲽵����һ��
  *     stepNo        :��ǰ����
  *     funDsc_p     :�Ự����������
  *     apdu104_p    :��Ҫ��֤���ݵĶ�ӦAPDU104����
  *�����
  *     ��
  *���أ�
  *     0    �ɹ�
  *     <0   ����
  *�޸���ʷ
  *
*******************************************************************************/
int chkAsduDataID ( const ONE_STEP* nowRuleStep_p,
                    const ONE_STEP* lastRuleStep_p,
                    const u_char& stepNo,
                    const T_ptclFunDsc* funDsc_p,
                    APDU104* apdu104_p
                  )
{


    //�������ԻỰ��ʶ
    //u_char act_prio_chnlID  = ( u_char ) funDsc_p->act_prio_chnlID;
    //ģʽ��
    u_char usingMode        = funDsc_p->usingMode;

    //����������֡�������޶�ֵ
    u_char allFrameNum_req  = nowRuleStep_p->reqPDU_Num;

    //��ǰ����ֵ֡���ǲ��ĵڼ�֡��
    //u_char nowFrameNum_req  = stepRl_p->reqPDU_Num;

    //��������Ӧ֡�������޶�ֵ
    u_char allFrameNum_resp = nowRuleStep_p->respPDU_Num;

    //��ǰ��Ӧֵ֡���ǲ��ĵڼ�֡��
    //u_char nowFrameNum_resp = stepRl_p->respPDU_Num;

    

    //����ԭ��
    ASDU_COT1_VAL* COT_p = ( ( ASDU_COT1_VAL* )
                             apdu104_p->getASDU_COT_VAL (
                             &apdu104_p->getDU_Info()->ASDU_CMTBL[ASDU_COT_INDEX] ) );
    //���ͱ�ʶ
    ASDU_TID_VAL* TID_p = apdu104_p->getASDU_TID_VAL (
                          &apdu104_p->getDU_Info()->ASDU_CMTBL[ASDU_TID_INDEX] );
    u_short      tmpCOT = 0;  //����ԭ��
    u_char       tmpTID = 0;  //���ͱ�ʶ

    if ( TID_p != NULL )
    {
        tmpTID = * ( ( u_char* ) TID_p );
    }

    if ( COT_p != NULL )
    {
        tmpCOT = * ( ( u_char* ) ( COT_p ) );
    }

    ptclStep* tRemoteSet_p = & ( funDsc_p->remoteSet_p[stepNo - 1] );

    u_char vFlag = 0;

    switch ( funDsc_p->ssnDscNo )
    {
        case STN_INIT: //վ��ʼ��;
        {
            break;
        }

        case QUE_DATA: //��ѯ����;
        {
            vFlag = 0;

            for ( int i = 0; i < tRemoteSet_p->TidNum; i++ )
            {
                if ( tmpTID == tRemoteSet_p->TID_p[i] )
                {
                    vFlag = 1;
                    break;
                }
            }

            //if ( act_prio_chnlID == PASSIVE_TYPE )
            if ( funDsc_p->chnlRole == SLAVE_STATION  )
            {
                //����վ�Ự��������վASDU
                if ( allFrameNum_req != 1 )
                {
                    return ERR_FRAME_NO;
                }

                if ( tmpTID != MC_TID_102
                     || vFlag != 1
                   )
                {
                    return ERR_TID_VAL;
                }

                if ( tmpCOT != MC_COT_5
                     || tmpCOT != tRemoteSet_p->COT
                   )
                {
                    return ERR_COT_VAL;
                }
            }
            //else if ( act_prio_chnlID == ACTIVE_TYPE )
            else if ( funDsc_p->chnlRole == MASTER_STATION )
            {
                //��վ���յ��Ǳ���վ������
                if ( allFrameNum_resp != 1 )
                {
                    return ERR_FRAME_NO;
                }

                if ( ! ( ( ( tmpTID >= MC_TID_1
                             && tmpTID <= MC_TID_14
                             && tmpTID % 2 == 1
                           )
                           || ( tmpTID == MC_TID_20 )
                           || ( tmpTID == MC_TID_21 )
                           || ( tmpTID >= MC_TID_30 && tmpTID <= MC_TID_36 )
                           || ( CCIFTID ( tmpTID ) )
                         ) && ( vFlag == 1 )
                       )
                   )
                {
                    return ERR_TID_VAL;
                }

                if ( tmpCOT != MC_COT_5
                     || tmpCOT != tRemoteSet_p->COT
                   )
                {
                    return ERR_COT_VAL;
                }
            }

            break;
        }          //end ��ѯ����

        case CYC_TRNS: //���ڴ���
        {
            if ( stepNo != C_STEP_NO_ONE )
            {
                //ֻ��һ����һ֡;
                return ERR_STEP_NO;
            }

            //if ( act_prio_chnlID == PASSIVE_TYPE )
            if ( funDsc_p->chnlRole == SLAVE_STATION )
            {
                //����վ���յ��ǿ���վ������
                if ( allFrameNum_resp != 0 )
                {
                    return ERR_FRAME_NO;
                }

                //���ڴ���û�п���վ������վ��ģʽ
                return ERR_ACT_PAS_VAL;
            }
            //else if ( act_prio_chnlID == ACTIVE_TYPE )
            else if ( funDsc_p->chnlRole == MASTER_STATION )
            {
                //����վ���յ��Ǳ���վ������
                //�����Ự��������֡��
                if ( allFrameNum_req != 1 )
                {
                    return ERR_FRAME_NO;
                }

                vFlag = 0;

                for ( int i = 0; i < tRemoteSet_p->TidNum; i++ )
                {
                    if ( tmpTID == tRemoteSet_p->TID_p[i] )
                    {
                        vFlag = 1;
                        break;
                    }
                }

                if ( ! ( ( tmpTID == MC_TID_9
                           || tmpTID == MC_TID_11
                           || tmpTID == MC_TID_13
                           || tmpTID == MC_TID_21
                           || CCIFTID ( tmpTID )
                         ) && vFlag == 1
                       )
                   )
                {
                    return ERR_TID_VAL;
                }

                if ( tmpCOT != MC_COT_1
                     || tmpCOT != tRemoteSet_p->COT
                   )
                {
                    return ERR_COT_VAL;
                }
            }

            break;
        }         //end ���ڴ���

        case ACQ_EVT: //�¼��ռ�
        {
            if ( stepNo != C_STEP_NO_ONE )
            {
                //ֻ��һ����һ֡;
                return ERR_STEP_NO;
            }

            //if ( act_prio_chnlID == ACTIVE_TYPE )
            if ( funDsc_p->chnlRole == SLAVE_STATION  )
            {
                //�¼��ռ� ����վû�����յ�����
                if ( allFrameNum_resp != 0 )
                {
                    return ERR_FRAME_NO;
                }

                //�¼��ɼ�û�п���վ������վ��ģʽ
                return ERR_ACT_PAS_VAL;
            }
            //else if ( act_prio_chnlID == PASSIVE_TYPE )
            else if ( funDsc_p->chnlRole == MASTER_STATION )
            {
                //����վ���յ��Ǳ���վ�����ݣ������Ự��������֡��
                // fusk@20180517 add ���¼��ɼ�Ϊ�����Ự,
                //      �޸�allFrameNum_resp -> allFrameNum_req
                //fusk@20180626 add ���¼��ɼ�Ϊ�����Ự,
                //      �޸�allFrameNum_req -> allFrameNum_resp
                //fusk@20180626 add ���¼��ɼ�Ϊ�����Ự,
                //      �޸�allFrameNum_resp -> allFrameNum_req      
                if ( allFrameNum_req != 1 )
                {
                    return ERR_FRAME_NO;
                }

                vFlag = 0;

                for ( int i = 0; i < tRemoteSet_p->TidNum; i++ )
                {
                    if ( tmpTID == tRemoteSet_p->TID_p[i] )
                    {
                        vFlag = 1;
                        break;
                    }
                }

                if ( ! ( ( ( tmpTID >= MC_TID_1
                             && tmpTID <= MC_TID_15
                             && tmpTID % 2 == 1
                           )
                           || ( tmpTID == MC_TID_20 || tmpTID == MC_TID_21 )
                           || ( tmpTID >= MC_TID_30 && tmpTID <= MC_TID_40 )
                           || ( CCIFTID ( tmpTID ) )
                         ) && ( vFlag == 1 )
                       )
                   )
                {
                    return ERR_TID_VAL;
                }

                if ( tmpCOT != MC_COT_3
                     || tmpCOT != tRemoteSet_p->COT
                   )
                {
                    return ERR_COT_VAL;
                }
            }

            break;
        }            //end �¼��ɼ�

        case QUKCHK_EVT: //����¼�
        {
            //��ʹ��
            return 0xffff;
            break;
        }

        case STN_INRGTN: //վ�ٻ�
        {
            vFlag = 0;

            for ( int i = 0; i < tRemoteSet_p->TidNum; i++ )
            {
                if ( tmpTID == tRemoteSet_p->TID_p[i] )
                {
                    vFlag = 1;
                    break;
                }
            }

            //if ( act_prio_chnlID == PASSIVE_TYPE )
            if ( funDsc_p->chnlRole == SLAVE_STATION )
            {
                //����վ����������վ������
                if ( stepNo != C_STEP_NO_ONE )
                {
                    return ERR_STEP_NO;
                }

                if ( allFrameNum_req != 1 )
                {
                    return ERR_FRAME_NO;
                }

                if ( tmpTID != MC_TID_100
                     || vFlag != 1
                   )
                {
                    return ERR_TID_VAL;
                }

                if ( tmpCOT != MC_COT_6
                     || tmpCOT != tRemoteSet_p->COT
                   )
                {
                    return ERR_COT_VAL;
                }
            }
            //else if ( act_prio_chnlID == ACTIVE_TYPE )
            else if ( funDsc_p->chnlRole == MASTER_STATION )
            {
                //����վ���ձ���վ������

                if ( ! ( ( ( tmpTID == MC_TID_100 )
                           || ( tmpTID >= MC_TID_1
                                && tmpTID <= MC_TID_14
                                && tmpTID % 2 == 1
                              )
                           || ( tmpTID == MC_TID_20 )
                           || ( tmpTID == MC_TID_21 )
                           || ( CCIFTID ( tmpTID ) )
                         ) && ( vFlag == 1 )
                       )
                   )
                {
                    return ERR_TID_VAL;
                }

                if ( tmpTID == MC_TID_100 )
                {
                    if ( ( tmpCOT != MC_COT_7 && tmpCOT != MC_COT_10 )
                         || tmpCOT != tRemoteSet_p->COT
                       )
                    {
                        return ERR_COT_VAL;
                    }
                }
                else if ( ( tmpTID >= MC_TID_1
                            && tmpTID <= MC_TID_14
                            && tmpTID % 2 == 1
                          )
                          || ( tmpTID == MC_TID_20 )
                          || ( tmpTID == MC_TID_21 )
                          || ( CCIFTID ( tmpTID ) )
                        )
                {
                    if ( ! ( ( tmpCOT >= MC_COT_20 && tmpCOT <= MC_COT_36 )
                             && tmpCOT == tRemoteSet_p->COT
                           )
                       )
                    {
                        return ERR_COT_VAL;
                    }
                }

            }//end ����վ����

            break;
        }//end վ�ٻ�

        case CLK_SYN: //ʱ��ͬ��
        {
            if ( funDsc_p->chnlRole == SLAVE_STATION ) //����վ����������վ������
            {
                if ( allFrameNum_req != 1 )
                {
                    return ERR_FRAME_NO;
                }

                if ( tmpCOT != MC_COT_6
                     || tmpCOT != tRemoteSet_p->COT
                   )
                {
                    return ERR_COT_VAL;
                }
            }
            else if ( funDsc_p->chnlRole == MASTER_STATION ) //����վ���ձ���վ������
            {
                if ( allFrameNum_resp != 1 )
                {
                    return ERR_FRAME_NO;
                }

                if ( tmpCOT != MC_COT_7
                     || tmpCOT != tRemoteSet_p->COT
                   )   //����ԭ��
                {
                    return ERR_COT_VAL;
                }
            }

            vFlag = 0;

            for ( int i = 0; i < tRemoteSet_p->TidNum; i++ )
            {
                if ( tmpTID == tRemoteSet_p->TID_p[i] )
                {
                    vFlag = 1;
                    break;
                }
            }

            if ( tmpTID != MC_TID_103
                 || vFlag != 1
               )
            {
                return ERR_TID_VAL;
            }

            break;
        }

        case CMD_TRNS: //�����
        {
            vFlag = 0;

            for ( int i = 0; i < tRemoteSet_p->TidNum; i++ )
            {
                if ( tmpTID == tRemoteSet_p->TID_p[i] )
                {
                    vFlag = 1;
                    break;
                }
            }

            if ( funDsc_p->chnlRole == SLAVE_STATION ) //����վ����������վ������
            {

                if ( allFrameNum_req != 1 )
                {
                    return ERR_FRAME_NO;
                }

                if ( ! ( ( ( tmpTID >= MC_TID_45 && tmpTID <= MC_TID_51 )
                           || ( tmpTID >= MC_TID_58 && tmpTID <= MC_TID_64 )
                         ) && ( vFlag == 1 )
                       )
                   )
                {
                    return ERR_TID_VAL;
                }

                if ( ( tmpCOT != MC_COT_6 && tmpCOT != MC_COT_8 )
                     || tmpCOT != tRemoteSet_p->COT
                   )
                {
                    return ERR_COT_VAL;
                }
            }
            else if ( funDsc_p->chnlRole == MASTER_STATION ) //����վ���ձ���վ������
            {
                //tidУ��

                if ( ! ( ( ( tmpTID >= MC_TID_45 && tmpTID <= MC_TID_51 )
                           || ( tmpTID >= MC_TID_58 && tmpTID <= MC_TID_64 )
                           || ( tmpTID >= MC_TID_1 && tmpTID <= MC_TID_7
                                && tmpTID % 2 == 1 )
                           || ( tmpTID == MC_TID_20 )
                           || ( tmpTID >= MC_TID_30 && tmpTID <= MC_TID_32 )
                         ) && ( vFlag == 1 )
                       )
                   )
                {
                    return ERR_TID_VAL;
                }

                //cotУ��
                if ( ( tmpTID >= MC_TID_45 && tmpTID <= MC_TID_51 )
                     || ( tmpTID >= MC_TID_58 && tmpTID <= MC_TID_64 ) )
                {
                    if ( stepNo < lastRuleStep_p->stepNo ) //�������һ��
                    {
                        if ( ( tmpCOT != MC_COT_7 && tmpCOT != MC_COT_9 )
                             || tmpCOT != tRemoteSet_p->COT
                           )
                        {
                            return ERR_COT_VAL;
                        }
                    }
                    else if ( stepNo == lastRuleStep_p->stepNo ) //�����һ��
                    {
                        if ( tmpCOT != MC_COT_10
                             || tmpCOT != tRemoteSet_p->COT
                           )
                        {
                            return ERR_COT_VAL;
                        }
                    }
                }

                if ( ( tmpTID >= MC_TID_1 && tmpTID <= MC_TID_7 )
                     || ( tmpTID == MC_TID_20 )
                     || ( tmpTID >= MC_TID_30 && tmpTID <= MC_TID_32 )
                   )
                {
                    if ( ! ( ( tmpCOT == MC_COT_11 || tmpCOT == MC_COT_12 )
                             && tmpCOT == tRemoteSet_p->COT
                           )
                       )
                    {
                        return ERR_COT_VAL;
                    }
                }

            }//end ����վ����

            //��ʵ��
            break;
        }

        case ACMU_FRZ:
        {
            /*�ۻ�������ǰ�Ķ���
              �˹����Ǵ�ACMU_TRNS���ܵ�ģʽ2��
              ����վ�ȶ�������������������
             */
            if ( funDsc_p->chnlRole == SLAVE_STATION ) //����վ���յ�������վ������
            {
                if ( tmpCOT != MC_COT_6
                     || tmpCOT != tRemoteSet_p->COT
                   )
                {
                    return ERR_COT_VAL;
                }
            }
            else if ( funDsc_p->chnlRole == MASTER_STATION ) //��վ���յ��Ǳ���վ������
            {
                if ( tmpCOT != MC_COT_7
                     || tmpCOT != tRemoteSet_p->COT
                   )
                {
                    return ERR_COT_VAL;
                }
            }

            vFlag = 0;

            for ( int i = 0; i < tRemoteSet_p->TidNum; i++ )
            {
                if ( tmpTID == tRemoteSet_p->TID_p[i] )
                {
                    vFlag = 1;
                    break;
                }
            }

            if ( tmpTID != MC_TID_101
                 || vFlag != 1
               )
            {
                return ERR_TID_VAL;
            }

            break;
        }

        case ACMU_TRNS: //�ۻ�������
        {
            switch ( usingMode )
            {
                case CTLD_DRCTSONPT_TRNS://ģʽ0������վֱ��ͻ������
                {
                    if ( stepNo != C_STEP_NO_ONE )  //ֻ��һ����һ֡;
                    {
                        return ERR_STEP_NO;
                    }

                    if ( funDsc_p->chnlRole == SLAVE_STATION ) //����վ
                    {
                        if ( allFrameNum_resp != 0 )
                        {
                            return ERR_FRAME_NO;
                        }

                        return ERR_ACT_PAS_VAL;//ͻ��û�п���վ������վ��ģʽ
                    }
                    else if ( funDsc_p->chnlRole == MASTER_STATION )  //����վ
                    {
                        if ( allFrameNum_req != 1 )
                        {
                            return ERR_FRAME_NO;
                        }

                        if ( tmpCOT != MC_COT_3
                             || tmpCOT != tRemoteSet_p->COT
                           )
                        {
                            return ERR_COT_VAL;
                        }

                        vFlag = 0;

                        for ( int i = 0;
                              i < tRemoteSet_p->TidNum;
                              i++
                            )
                        {
                            if ( tmpTID == tRemoteSet_p->TID_p[i] )
                            {
                                vFlag = 1;
                                break;
                            }
                        }

                        if ( ! ( ( tmpTID == MC_TID_15
                                   || tmpTID == MC_TID_37
                                   || CCIFTID ( tmpTID )
                                 ) && vFlag == 1
                               )
                           )
                        {
                            return ERR_TID_VAL;
                        }
                    }

                    break;
                }

                case CTL_DRCTREQ_TRNS://ģʽ1������վֱ��������
                {
                    vFlag = 0;

                    for ( int i = 0; i < tRemoteSet_p->TidNum; i++ )
                    {
                        if ( tmpTID == tRemoteSet_p->TID_p[i] )
                        {
                            vFlag = 1;
                            break;
                        }
                    }

                    if ( funDsc_p->chnlRole == SLAVE_STATION )
                    {
                        //����վ���յ�������վ������
                        if ( tmpCOT != MC_COT_6
                             || tmpCOT != tRemoteSet_p->COT
                           )
                        {
                            return ERR_COT_VAL;
                        }

                        if ( tmpTID != MC_TID_101
                             || vFlag != 1
                           )
                        {
                            return ERR_TID_VAL;
                        }

                    }
                    else if ( funDsc_p->chnlRole == MASTER_STATION )
                    {
                        //����վ���յ��Ǳ���վ������
                        //tidУ��
                        if ( ! ( ( tmpTID == MC_TID_101
                                   || tmpTID == MC_TID_15
                                   || tmpTID == MC_TID_37
                                 ) && vFlag == 1
                               )
                           )
                        {
                            return ERR_TID_VAL;
                        }

                        //cotУ��
                        if ( tmpTID == MC_TID_101 )
                        {
                            if ( stepNo < lastRuleStep_p->stepNo )
                            {
                                if ( tmpCOT != MC_COT_7
                                     || tmpCOT != tRemoteSet_p->COT
                                   )
                                {
                                    return ERR_COT_VAL;
                                }
                            }
                            else if ( stepNo == lastRuleStep_p->stepNo )
                            {
                                if ( tmpCOT != MC_COT_10
                                     || tmpCOT != tRemoteSet_p->COT
                                   )
                                {
                                    return ERR_COT_VAL;
                                }
                            }
                        }
                        else
                        {
                            if ( stepNo >= lastRuleStep_p->stepNo )
                            {
                                //M_IT���䲻Ӧ�����һ��
                                return ERR_STEP_NO;
                            }

                            if ( ! ( tmpCOT >= MC_COT_37 && tmpCOT <= MC_COT_41
                                     && tmpCOT == tRemoteSet_p->COT
                                   )
                               )
                            {
                                return ERR_COT_VAL;
                            }
                        }
                    }

                    break;
                }

                case CTL_FRZ_CTLD_SONPT_TRNS://ģʽ3������վ����󱻿�վͻ������
                {
                    vFlag = 0;

                    for ( int i = 0; i < tRemoteSet_p->TidNum; i++ )
                    {
                        if ( tmpTID == tRemoteSet_p->TID_p[i] )
                        {
                            vFlag = 1;
                            break;
                        }
                    }

                    if ( funDsc_p->chnlRole == SLAVE_STATION )
                    {
                        //����վ���յ�������վ������
                        if ( tmpCOT != MC_COT_6
                             || tmpCOT != tRemoteSet_p->COT
                           )
                        {
                            return ERR_COT_VAL;
                        }

                        if ( tmpTID != MC_TID_101
                             || vFlag != 1
                           )
                        {
                            return ERR_TID_VAL;
                        }

                    }
                    else if ( funDsc_p->chnlRole == MASTER_STATION )
                    {
                        //����վ���յ��Ǳ���վ������
                        //tidУ��
                        if ( ! ( ( tmpTID == MC_TID_101
                                   || tmpTID == MC_TID_15
                                   || tmpTID == MC_TID_37
                                 ) && vFlag == 1
                               )
                           )
                        {
                            return ERR_TID_VAL;
                        }

                        //cotУ��
                        if ( tmpTID == MC_TID_101 )
                        {
                            if ( stepNo < lastRuleStep_p->stepNo )
                            {
                                if ( tmpCOT != MC_COT_7
                                     || tmpCOT != tRemoteSet_p->COT
                                   )
                                {
                                    return ERR_COT_VAL;
                                }
                            }
                            else if ( stepNo == lastRuleStep_p->stepNo )
                            {
                                return ERR_STEP_NO;
                            }
                        }
                        else
                        {
                            if ( tmpCOT != MC_COT_3
                                 || tmpCOT != tRemoteSet_p->COT
                               )
                            {
                                return ERR_COT_VAL;
                            }

                        }
                    }//end act_prio_chnlID == ACTIVE_TYPE

                    break;
                }//end ģʽ3������վ����󱻿�վͻ������

                default:
                    break;
            }//end switch (usingMode)

            break;
        }

        case PARA_LD: //��������
        {
            switch ( usingMode )
            {
                case ONLY_LD://ֻ���أ�ģʽ0
                {
                    if ( funDsc_p->chnlRole == SLAVE_STATION ) //����վ�Ự
                    {
                        if ( tmpCOT != MC_COT_6
                             || tmpCOT != tRemoteSet_p->COT
                           )
                        {
                            return ERR_COT_VAL;
                        }

                    }
                    else if ( funDsc_p->chnlRole == MASTER_STATION ) //����վ�Ự
                    {
                        if ( tmpCOT != MC_COT_7
                             || tmpCOT != MC_COT_3
                             || tmpCOT != tRemoteSet_p->COT
                           )
                        {
                            return ERR_COT_VAL;
                        }
                    }

                    vFlag = 0;

                    for ( int i = 0; i < tRemoteSet_p->TidNum; i++ )
                    {
                        if ( tmpTID == tRemoteSet_p->TID_p[i] )
                        {
                            vFlag = 1;
                            break;
                        }
                    }

                    if ( ! ( ( tmpTID == MC_TID_110
                               || tmpTID == MC_TID_111
                               || tmpTID == MC_TID_112
                             ) && vFlag == 1
                           )
                       )
                    {
                        return ERR_TID_VAL;
                    }

                    break;
                }

                case ONLYACT_LD://ֻ���ģʽ1
                {
                    vFlag = 0;

                    for ( int i = 0; i < tRemoteSet_p->TidNum; i++ )
                    {
                        if ( tmpTID == tRemoteSet_p->TID_p[i] )
                        {
                            vFlag = 1;
                            break;
                        }
                    }

                    if ( funDsc_p->chnlRole == SLAVE_STATION ) //����վ�Ự
                    {
                        if ( tmpCOT != MC_COT_6
                             || vFlag != 1
                           )
                        {
                            return ERR_COT_VAL;
                        }

                        if ( tmpTID != MC_TID_113
                             || vFlag != 1
                           )
                        {
                            return ERR_TID_VAL;
                        }
                    }
                    else if ( funDsc_p->chnlRole == MASTER_STATION ) //����վ�Ự
                    {
                        if ( ! ( tmpTID == MC_TID_113
                                 && vFlag == 1
                               )
                           )
                        {
                            return ERR_TID_VAL;
                        }

                        if ( tmpTID == MC_TID_113 )
                        {
                            if ( tmpCOT != MC_COT_7
                                 || tmpCOT != tRemoteSet_p->COT
                               )
                            {
                                return ERR_COT_VAL;
                            }
                        }

                        /*else if (  tmpTID >= MC_TID_110 && tmpTID <= MC_TID_112 )
                        {
                            if (tmpCOT != MC_COT_3)
                            {
                                return ERR_COT_VAL;
                            }
                        }*/
                    }//end ����վ�Ự

                    break;
                }

                case AUTOACT_LD://�Զ����ģʽ2
                {
                    vFlag = 0;

                    for ( int i = 0; i < tRemoteSet_p->TidNum; i++ )
                    {
                        if ( tmpTID == tRemoteSet_p->TID_p[i] )
                        {
                            vFlag = 1;
                            break;
                        }
                    }

                    if ( funDsc_p->chnlRole == SLAVE_STATION ) //����վ�Ự
                    {
                        if ( tmpCOT != MC_COT_6
                             || tmpCOT != tRemoteSet_p->COT
                           )
                        {
                            return ERR_COT_VAL;
                        }

                        if ( ! ( ( tmpTID == MC_TID_110
                                   || tmpTID == MC_TID_111
                                   || tmpTID == MC_TID_112
                                 ) && vFlag == 1
                               )
                           )
                        {
                            return ERR_TID_VAL;
                        }
                    }
                    else if ( funDsc_p->chnlRole == MASTER_STATION ) //����վ�Ự
                    {
                        if ( ! ( ( tmpTID == MC_TID_110
                                   || tmpTID == MC_TID_111
                                   || tmpTID == MC_TID_112
                                 ) && vFlag == 1
                               )
                           )
                        {
                            return ERR_TID_VAL;
                        }

                        if ( tmpCOT != MC_COT_7
                             || tmpCOT != MC_COT_3
                             || tmpCOT != tRemoteSet_p->COT
                             //&& tmpCOT != MC_COT_3
                           )
                        {
                            return ERR_COT_VAL;
                        }
                    }//end ����վ�Ự

                    break;
                }

                default:
                    break;
            }

            break;
        }

        case TST_PRCD: //���Թ���
        {
            if ( funDsc_p->chnlRole == SLAVE_STATION ) //����վ���յ��ǿ���վ������
            {
                if ( tmpCOT != MC_COT_6
                     || tmpCOT != tRemoteSet_p->COT
                   )   //����ԭ��
                {
                    return ERR_COT_VAL;
                }
            }
            else if ( funDsc_p->chnlRole == MASTER_STATION ) //����վ���յ��Ǳ���վ������
            {
                if ( tmpCOT != MC_COT_7
                     || tmpCOT != tRemoteSet_p->COT
                   )   //����ԭ��
                {
                    return ERR_COT_VAL;
                }
            }

            vFlag = 0;

            for ( int i = 0; i < tRemoteSet_p->TidNum; i++ )
            {
                if ( tmpTID == tRemoteSet_p->TID_p[i] )
                {
                    vFlag = 1;
                    break;
                }
            }

            if ( ! ( ( tmpTID == MC_TID_107
                     ) && vFlag == 1
                   )
               )
            {
                return ERR_TID_VAL;
            }

            break;
        }

        case FILE_TRNS: //�ļ�����
        {
            //�ݲ�ʵ��
            return 0xffff;
            break;
        }

        case ACQ_TRNSDLY: //�ռ������ӳ�ֵ
        {
            //��ʹ��
            return 0xffff;
            break;
        }

        default:
            break;
    }


    return 0;
}



/*******************************************************************************
  *���ܣ�
  *    ������θ�FUN_RL�ṹ��ֵ
  *
  * ���룺
  *     srvNo      �����ܺ�
  *     usingMode  : ģʽ��
  *     PDU_FCode  : ֡ʹ�õ�֡��ʽ����
  *     stnSnnCfg_p: �Ự����֡������Ϣ
  *�����
  *     fun_p     �� Ĭ�ϲ�����
  *���أ�
  *     0    �ɹ�
  *     <0   ����
  *�޸���ʷ
  *
*******************************************************************************/
void setFunRl ( const u_char& srvNo,
                const u_char& usingMode,
                const u_char& PDU_FCode,
                T_snnCfg*& snnCfg_p,
                FUN_RL*& fun_p
              )
{
    u_char stepNum = snnCfg_p->stepNum;

    //1���Բ�����fun_p��ֵ
    fun_p->srvNo               = srvNo;
    fun_p->usingMode           = usingMode;
    fun_p->cmLvl[usingMode]    = APP_LVL; // 104��ԼȡAPP_LVL
    fun_p->stepNum[usingMode]  = stepNum;

    //2������������
    ONE_STEP* tmpHead_p = NULL;
    T_ssnStepCfg* tStepCfg_p = NULL;

    for ( u_char i = 0; i < stepNum; i++ )
    {
        tStepCfg_p = ( snnCfg_p->stepCfg_p + i );

        ONE_STEP* one_step_p        = new ONE_STEP();
        vrfItem* tsVerifyBlock1_p   = new vrfItem();
        tsVerifyBlock1_p->chkRslt_p = new int();

        setONE_STEP ( one_step_p,
                      i + 1,
                      PDU_FCode,
                      tStepCfg_p->reqAPDUNum,
                      NULL,
                      PDU_FCode,
                      tStepCfg_p->respAPDUNum,
                      NULL,
                      tsVerifyBlock1_p
                    );
        one_step_p->nextStep_p = NULL;

        if ( i == 0 )
        {
            tmpHead_p = one_step_p;
        }
        else
        {
            addNode ( tmpHead_p, one_step_p );
        }
    }

    //3���Ѳ�����ҵ�Fun_p��
    fun_p->steps_p[usingMode] = tmpHead_p;

    return;
}


/*******************************************************************************
  *���ܣ�(�˽ӿ���ʱδ�õ�)
  *    ����tid��ֵ��Ӧ���õĳ��ȣ����ݹ�Լ�涨�õ���
  *
  *
  * ���룺
  *     TID      ��tid��ֵ
  *     flag
  *        0:tidֵ�ĳ��ȣ����ֽ�Ϊ��λ��
  *        1:tidֵ�ĳ��ȣ�ΪλΪ��λ��
  *        2:Ʒ�������ĳ��ȣ����ֽ�Ϊ��λ��
  *        3:ʱ��ĳ��ȣ����ֽ�Ϊ��λ��
  *        4:�ж�ֵ��QDS�Ƿ������һ���:��QDS������ռ�ֽ�
  *        5:tidֵ+QDS�ĳ���
  *�����
  *     ��
  *
  *���أ�
  *     tid��������Ӧֵ�ĳ���
  *�޸���ʷ
  *
*******************************************************************************/
u_char getTidOneLng ( const u_short& TID,
                      const u_char& flag
                    )
{
    if ( TID >= MC_MAXTID_IDS )
    {
        return 0;
    }

    u_char tRet = 0;

    if ( flag == 0 ) /*tidֵ�ĳ���*/
    {
        if ( g_TID104[TID].tidBitLng == 0 )
        {
            tRet = 0;
        }
        //else if ( ( g_TID104[TID].tidQdsLng - g_TID104[TID].qdsLng ) == 0 )
        //{
        //    /*ֵ��QDS����һ���ֽڣ��磺��Ʒ�������ʵĵ�����Ϣ*/
        //    oTidValLng = 1;
        //}
        else
        {
            u_char integ = g_TID104[TID].tidBitLng / 8;
            u_char remder = g_TID104[TID].tidBitLng % 8;
            u_char round = remder > 0 ? 1 : 0;
            tRet = integ + round;
        }
    }
    else if ( flag == 1 ) /*tidֵ��bit����*/
    {
        tRet = g_TID104[TID].tidBitLng;
    }
    else if ( flag == 2 ) /*QDS�ĳ���*/
    {
        tRet = g_TID104[TID].qdsLng;
    }
    else if ( flag == 3 ) /*ʱ��ĳ���*/
    {
        tRet = g_TID104[TID].tmStmpLng;
    }
    else if ( flag == 4 ) /*�ж�ֵ��QDS�Ƿ������һ���:��QDS������ռ�ֽ�*/
    {
        if ( g_TID104[TID].tidQdsLng == g_TID104[TID].qdsLng
             && g_TID104[TID].tidBitLng > 0
           )
        {
            tRet = 1;/*������һ���*/
        }
        else
        {
            tRet = 0;/*û����*/
        }
    }
    else if ( flag == 5 ) /*tidֵ+QDS�ĳ���*/
    {
        tRet = g_TID104[TID].tidQdsLng;
    }

    return tRet;
}

/*******************************************************************************
  *���ܣ�
  *    ����tid��ֵ��Ӧ���õĳ��ȣ����ݹ�Լ�涨�õ���
  *
  *
  * ���룺
  *     TID      ��tid��ֵ
  *
  *�����
  *     tidObjLng_p tid��صĳ���
  *
  *���أ�
  *     0   �ɹ�
  *     ��0 ʧ��
  *�޸���ʷ
  *
*******************************************************************************/
int getTidObjLng ( const u_short& TID,
                   T_tidObjLng* tidObjLng_p
                 )
{
    if ( tidObjLng_p == NULL )
    {
        return ERR_POINTER_NULL;
    }
    else if ( TID >= MC_MAXTID_IDS )
    {
        return ERR_CONFIG;
    }

    /*tidֵ���ֽڳ���*/
    if ( g_TID104[TID].tidBitLng == 0 )
    {
        tidObjLng_p->tidValLng = 0;
    }
    else
    {
        u_char integ = g_TID104[TID].tidBitLng / 8;
        u_char remder = g_TID104[TID].tidBitLng % 8;
        u_char round = remder > 0 ? 1 : 0;
        tidObjLng_p->tidValLng = integ + round;
    }

    /*�ж�ֵ��QDS�Ƿ������һ���:��QDS������ռ�ֽ�*/
    if ( g_TID104[TID].tidQdsLng == g_TID104[TID].qdsLng
         && g_TID104[TID].tidBitLng > 0
       )
    {
        /*������һ���*/
        tidObjLng_p->qdsMixFlag = 1;
    }
    else
    {
        /*û����*/
        tidObjLng_p->qdsMixFlag = 0;
    }

    /*tidֵ��bit����*/
    tidObjLng_p->cfgLng.tidBitLng = g_TID104[TID].tidBitLng;
    /*QDS�ĳ���*/
    tidObjLng_p->cfgLng.qdsLng = g_TID104[TID].qdsLng;
    /*ʱ��ĳ���*/
    tidObjLng_p->cfgLng.tmStmpLng = g_TID104[TID].tmStmpLng;
    /*tidֵ+QDS�ĳ���*/
    tidObjLng_p->cfgLng.tidQdsLng = g_TID104[TID].tidQdsLng;

    return 0;
}


/*******************************************************************************
  *���ܣ�
  *    ȡ�����е�ĳЩλ��Ϊֵ���
  *
  *
  * ���룺
  *     val_p      :�����������,��Ҫȡֵ�ĵ�ַ
  *     sizeV      :val_p���������͵Ĵ�С
  *     bgBit      :ȡֵ�Ŀ�ʼλ(��1��ʼ)
  *     edBit      :ȡֵ�Ľ���λ
  *
  *�����
  *     val_p
  *
  *���أ�
  *     0   �ɹ�
  *     ��0 ʧ��
  *ע��:
  *    1.val_p����������ֻ֧�� u_char,u_short,u_int
  *    1.�����ɹ��󽫸���ԭ��������ֵval_p
  *
*******************************************************************************/
int getBitsVal ( void* val_p,
                 const u_char& sizeV,
                 const u_char& bgBit,
                 const u_char& edBit
               )
{
    if ( ! ( val_p != NULL
             && ( bgBit <= edBit && bgBit > 0 )
             && edBit <= ( sizeV * 8 )
             && ( edBit - bgBit + 1 ) <= 32
             && ( sizeV == 1
                  || sizeV == 2
                  || sizeV == 4
                )
           )
       )
    {
        return ERR_INPUT_VAL;
    }

    u_char mvFlag = 0;

    if ( sizeV == 1 )
    {
        u_char uVc = * ( u_char* ) val_p;

        if ( 8 - edBit > 0 )
        {
            uVc <<= ( 8 - edBit );
            uVc >>= ( 8 - edBit );
            mvFlag = 1;
        }

        if ( bgBit - 1 > 0 )
        {
            uVc >>= ( bgBit - 1 );
            //uVc <<= ( bgBit - 1 );
            mvFlag = 1;
        }

        if ( mvFlag == 1 )
        {
            * ( u_char* ) val_p = uVc;
        }
    }
    else if ( sizeV == 2 )
    {
        u_short uVs = * ( u_short* ) val_p;

        if ( 16 - edBit > 0 )
        {
            uVs <<= ( 16 - edBit );
            uVs >>= ( 16 - edBit );
            mvFlag = 1;
        }

        if ( bgBit - 1 > 0 )
        {
            uVs >>= ( bgBit - 1 );
            //uVs <<= ( bgBit - 1 );
            mvFlag = 1;
        }

        if ( mvFlag == 1 )
        {
            * ( u_short* ) val_p = uVs;
        }
    }
    else if ( sizeV == 4 )
    {
        u_int uVi = * ( u_int* ) val_p;

        if ( 32 - edBit > 0 )
        {
            uVi <<= ( 32 - edBit );
            uVi >>= ( 32 - edBit );
            mvFlag = 1;
        }

        if ( bgBit - 1 > 0 )
        {
            uVi >>= ( bgBit - 1 );
            //uVi <<= ( bgBit - 1 );
            mvFlag = 1;
        }

        if ( mvFlag == 1 )
        {
            * ( u_int* ) val_p = uVi;
        }
    }

    return 0;
}



/*******************************************************************************
  *���ܣ�
  *    ֡���ݸ�T_snnPara��ֵ
  *
  *
  * ���룺
  *     tTidObjLng     :tid��ӦЭ�����ó���
  *     usrData_index  :��֡�����ڲ��п�ʼ��
  *     tUsrData_p     :��Ĵ�ȡ����
  *     apdu104_p      :APDU104��Ӧ
  *     stnAcsFlag     :���ݴ�ȡ���ƽṹ
  *
  *�����
  *     tUsrData_p     :��Ĵ�ȡ����
  *     apdu104_p      :APDU104��Ӧ
  *     stnAcsFlag     :���ݴ�ȡ���ƽṹ
  *
  *���أ�
  *     0   �ɹ�
  *     ��0 ʧ��
  *ע��:
  *
*******************************************************************************/
int  setPntSnnpar ( const T_tidObjLng& tTidObjLng,
                    const u_short& usrData_index,
                    T_snnUsrData* tUsrData_p,
                    APDU104* apdu104_p,
                    T_stnAcsFlag* stnAcsFlag
                  )
{
    int tRet = 0;
    u_char* tmpTSP_p = NULL;
    u_short tmpBodSetLng = 0;   //Ԫ�ؼ�����
    u_char* ssnBodSet_p  = NULL;//Ԫ�ؼ��׵�ַ
    u_short tmpBodSetNum = 0;   //Ԫ�ظ���

    APDU_DSCR* tApduDscr_p   = apdu104_p->getDU_Info();

    if ( tApduDscr_p == NULL )
    {
        return ERR_POINTER_NULL;
    }

    //�ɱ�ṹ���޶���
    ASDU_VSQ_VAL* ssnVSQ_p = apdu104_p->getASDU_VSQ_VAL (
                             & ( tApduDscr_p->ASDU_CMTBL[ASDU_VSQ_INDEX] )
                             );

    if ( ssnVSQ_p == NULL )
    {
        return ERR_POINTER_NULL;
    }

    tRet = apdu104_p->setASDU_VSQ_VAL(ssnVSQ_p);
    if ( tRet != 0 )
    {
        return tRet;
    }

    if ( tTidObjLng.cfgLng.tidQdsLng > 0 )
    {
        tApduDscr_p->ASDU_CMTBL[BODY_SET_INDEX] = ssnVSQ_p->N ;
    }
    else
    {
        tApduDscr_p->ASDU_CMTBL[BODY_SET_INDEX] = 0;
    }

    //��Ϣ���ַ
    BODY_ADDR_VAL* ssnBodAddr_p = NULL;
    u_short tOneBdAddLng        = tApduDscr_p->ASDU_CMTBL[BODY_ADDR_INDEX];

    if ( tOneBdAddLng > 0 )
    {
        ssnBodAddr_p = apdu104_p->getBODY_ADDR_VAL (
                       & ( tOneBdAddLng )
                       );
    }

    //����Ϣ���ַ���뵱ǰ֡�׸���ֵ��Ӧ�ĵ��ַ��
    if ( ssnBodAddr_p != NULL )
    {
        memcpy ( & ( tUsrData_p[usrData_index].gData.val_chkStd.pntAddr ),
                 ssnBodAddr_p,
                 tOneBdAddLng
               );
    }

    //ÿ��Ԫ�صĳ���
    //u_int tmpOneLng = tApduDscr_p->InfoEleLng;
    u_int tmpPntLng = sizeof (
                      tUsrData_p[usrData_index].gData.val_chkStd.pntVal1 );

    //if ( tmpOneLng > tmpPntLng * 2 )
    if ( tTidObjLng.cfgLng.tidQdsLng > tmpPntLng * 2 )
    {
        return ERR_INFOELELNG_CFG;
    }

    //����tid�ж��Ƿ���ʱ��
    if ( tTidObjLng.cfgLng.tmStmpLng > 0 )
    {
        tApduDscr_p->ASDU_CMTBL[BODY_TSP_INDEX] = sizeof ( CP56TIME2a );
        tmpTSP_p = ( u_char* ) apdu104_p->getBODY_TSP_VAL_CP56 (
                   &tApduDscr_p->ASDU_CMTBL[BODY_TSP_INDEX]
                   );
    }
    else
    {
        tApduDscr_p->ASDU_CMTBL[BODY_TSP_INDEX] = 0;
        tmpTSP_p = NULL;
    }


    tmpBodSetNum = ssnVSQ_p->N;      //Ԫ�ظ���

    if ( tTidObjLng.cfgLng.tidQdsLng > 0 )
    {
        //��ֵ��Ʒ������
        ssnBodSet_p  = apdu104_p->getAPDU_ELEMENT (
                       &tmpBodSetLng,
                       BODY_SET_INDEX ); //Ԫ�ؼ��׵�ַ
    }

    for ( u_short i = usrData_index;
          i < usrData_index + tmpBodSetNum;
          i++
        ) //��ÿ�����ַ�͵�ֵ
    {
        //���ַ
        if ( ssnVSQ_p->SQ == 1
             && ssnBodAddr_p != NULL
             && i > usrData_index )
        {
            /*SQ:=1 ������ϢԪ�ص���������Ϣ�����ַ��Ѱַ����Ϣ
            �����ַ��˳�򵥸�Ԫ�صĵ�һ����ϢԪ�ص�ַ����̵���
            ��ϢԪ���Ǵ������ַ˳���1*/
            tUsrData_p[i].gData.val_chkStd.pntAddr = (
                    tUsrData_p[usrData_index].gData.val_chkStd.pntAddr
                    + ( i - usrData_index ) );
        }
        else if ( ssnVSQ_p->SQ == 0
                  && tOneBdAddLng > 0
                )
        {

            ssnBodAddr_p = apdu104_p->getBODY_ADDR_VAL (
                           &tOneBdAddLng,
                           ( u_char ) i
                           );

            if ( ssnBodAddr_p != NULL )
            {
                memcpy ( & ( tUsrData_p[i].gData.val_chkStd.pntAddr ),
                         ssnBodAddr_p,
                         tOneBdAddLng
                       );
            }
        }

        if ( ssnVSQ_p->SQ == 0 &&
             tTidObjLng.cfgLng.tidQdsLng > 0
           )
        {
            //��ֵ��Ʒ������
            ssnBodSet_p  = apdu104_p->getAPDU_ELEMENT (
                           &tmpBodSetLng,
                           BODY_SET_INDEX,
                           ( u_char ) i
                           );
        }

        tUsrData_p[i].gData.pntType = tTidObjLng.tidValLng;

        if ( ssnBodSet_p != NULL )
        {
            u_int tPntVal1 = 0;
            u_char addLng = 0;

            if ( ssnVSQ_p->SQ == 1 ) //������ṹ
            {
                addLng = i * tTidObjLng.cfgLng.tidQdsLng;
            }
            else//�����ṹ
            {
                addLng = 0;
            }

            if ( tTidObjLng.tidValLng <= tmpPntLng
                 && tTidObjLng.tidValLng > 0
               )
            {
                memcpy ( & ( tPntVal1 ),
                         ssnBodSet_p + addLng,
                         tTidObjLng.tidValLng
                       );
            }
            else if ( tTidObjLng.tidValLng > tmpPntLng )
            {
                memcpy ( & ( tPntVal1 ),
                         ssnBodSet_p + addLng,
                         tmpPntLng
                       );
                memcpy ( & ( tUsrData_p[i].gData.val_chkStd.pntVal2 ),
                         ssnBodSet_p + addLng + tmpPntLng,
                         tTidObjLng.tidValLng - tmpPntLng
                       );
            }

            u_char qdsVal = 0;

            if ( tTidObjLng.cfgLng.qdsLng > 0
                 && tTidObjLng.qdsMixFlag == 1
               )
            {
                qdsVal = ( u_char ) tPntVal1;
                //Ʒ��������ֵ��һ��ʱ��Ҫ�����ֵ
                tRet = getBitsVal ( &tPntVal1,
                                    sizeof ( tPntVal1 ),
                                    1,
                                    tTidObjLng.cfgLng.qdsLng
                                  );

                if ( tRet != 0 )
                {
                    return tRet;
                }

                memcpy ( & ( tUsrData_p[i].gData.dDsc ),
                         &qdsVal,
                         tTidObjLng.cfgLng.qdsLng
                       );
            }
            else if ( tTidObjLng.cfgLng.qdsLng > 0
                      && tTidObjLng.qdsMixFlag == 0
                    )
            {
                //Ʒ��������ֵ�����
                memcpy ( & ( tUsrData_p[i].gData.dDsc ),
                         ssnBodSet_p + addLng + tTidObjLng.tidValLng,
                         tTidObjLng.cfgLng.qdsLng
                       );
            }

            tUsrData_p[i].gData.val_chkStd.pntVal1 = tPntVal1;

            if ( tTidObjLng.tidValLng > 0 )
            {
                stnAcsFlag->ctlType = ( stnAcsFlag->ctlType | 0x01 ); //�洢����
                stnAcsFlag->setValid = 1;
            }
        }

        if ( tmpTSP_p != NULL
             && ssnVSQ_p->SQ == 0
           )
        {
            tmpTSP_p = ( u_char* ) apdu104_p->getBODY_TSP_VAL_CP56 (
                       & ( tApduDscr_p->ASDU_CMTBL[BODY_TSP_INDEX] ),
                       ( u_char ) i );
            memcpy ( tUsrData_p[i].gData.tmStmp_p,
                     tmpTSP_p,
                     sizeof ( CP56TIME2a )
                   );
            tUsrData_p[i].gData.tmStmpType = TIME_TYPE_56_2A;
            stnAcsFlag->tspValidB = 1;
        }

        if ( ssnVSQ_p->SQ == 1
             && tmpTSP_p != NULL
             && i == ( usrData_index + tmpBodSetNum - 1 )
           )
        {
            //�����ʱ�꣬���ʱ�긳�赱ǰ֡���һ�����Ӧ��usrData_p�ṹ��
            memcpy ( tUsrData_p[i].gData.tmStmp_p,
                     tmpTSP_p,
                     sizeof ( CP56TIME2a )
                   );
            tUsrData_p[i].gData.tmStmpType = TIME_TYPE_56_2A;
            stnAcsFlag->tspValidB = 1;
        }
    }

    return 0;
}


/*******************************************************************************
  *���ܣ�
  *    ���ղ�ȡ�ñ�ʶreqFlag���Ƿ����������Ӧ��
  *
  *
  * ���룺
  *     funDsc_p     :T_ptclFunDsc�ṹ����
  *
  *�����
  *     ��
  *
  *���أ�
  *     0   ����
  *     1   ��Ӧ
  *     ��0 ʧ��
  *ע��:
  *
*******************************************************************************/
inline char getRcvReqFlag ( const T_ptclFunDsc* funDsc_p )
{
    char reqFlag = -1;

    u_char actFlag = ( u_char ) funDsc_p->act_prio_chnlID;

    if ( funDsc_p->chnlRole == MASTER_STATION )//����վ�Ľ���
    {
        if ( actFlag == PASSIVE_TYPE )
        {
            reqFlag = 1; //���ղദ������
        }
        else //ACTIVE_TYPE
        {
            reqFlag = 0; //��Ӧ
        }
    }
    else //SLAVE_STATION ����վ�Ľ���
    {
        if ( actFlag == PASSIVE_TYPE )
        {
            reqFlag = 1; //����
        }
        else //ACTIVE_TYPE
        {
            reqFlag = 0; //��Ӧ
        }
    }

    return reqFlag;
}

/*******************************************************************************
  *���ܣ�
  *    ���Ͳ�ȡ�ñ�ʶreqFlag���Ƿ����������Ӧ��
  *
  *
  * ���룺
  *     funDsc_p     :T_ptclFunDsc�ṹ����
  *
  *�����
  *     ��
  *
  *���أ�
  *     0   ����
  *     1   ��Ӧ
  *     ��0 ʧ��
  *ע��:
  *
*******************************************************************************/
inline char getSndReqFlag ( const T_ptclFunDsc* funDsc_p )
{
    char reqFlag = -1;

    u_char actFlag = ( u_char ) funDsc_p->act_prio_chnlID;

    if ( funDsc_p->chnlRole == MASTER_STATION )//����վ�ķ���
    {
        if ( actFlag == PASSIVE_TYPE )
        {
            //�߼�����
            reqFlag = 0; 
        }
        else //ACTIVE_TYPE
        {
            reqFlag = 1; //����
        }
    }
    else //SLAVE_STATION ����վ�ķ���
    {
        if ( actFlag == PASSIVE_TYPE )
        {
            reqFlag = 0; //��Ӧ
        }
        else //ACTIVE_TYPE
        {
            reqFlag = 1; //����
        }
    }

    return reqFlag;
}

