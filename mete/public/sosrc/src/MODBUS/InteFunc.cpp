
#include "InteFunc.h"

/**************************************�ڲ�ʹ�ú����ǽӿ�***********************/

const T_MDPNTLNG  g_FCMDBUSLNG[MC_MAXFC_NUMS] =
{
    /*function code:01 */ {1, 1}, /*����Ȧ*/
    /*function code:02 */ {1, 1}, /*����ɢ������*/
    /*function code:03 */ {2, 16}, /*�����ּĴ���*/
    /*function code:04 */ {2, 16}, /*������Ĵ���*/
    /*function code:05 */ {2, 16}, /*д������Ȧ*/
    /*function code:06 */ {2, 16}, /*д�����Ĵ���*/
    /*function code:07 */ {0, 0},
    /*function code:08 */ {0, 0},
    /*function code:09 */ {0, 0},
    /*function code:10 */ {0, 0},
    /*function code:11 */ {0, 0},
    /*function code:12 */ {0, 0},
    /*function code:13 */ {0, 0},
    /*function code:14 */ {0, 0},
    /*function code:15 */ {0, 0}, /*д�����Ȧ*/
    /*function code:16 */ {0, 0}, /*д����Ĵ���*/
    /*function code:17 */ {0, 0},
    /*function code:18 */ {0, 0},
    /*function code:19 */ {0, 0},
    /*function code:20 */ {0, 0}, /*���ļ���¼:��ʱ����*/
    /*function code:21 */ {0, 0}, /*д�ļ���¼:��ʱ����*/
    /*function code:22 */ {8, 64}, /*����д�Ĵ���*/
    /*function code:23 */ {2, 16}, /*��д����Ĵ���*/
    /*function code:24 */ {0, 0},
    /*function code:25 */ {0, 0},
    /*function code:26 */ {0, 0},
    /*function code:27 */ {0, 0},
    /*function code:28 */ {0, 0},
    /*function code:29 */ {0, 0},
    /*function code:30 */ {0, 0},
    /*function code:31 */ {0, 0},
    /*function code:32 */ {0, 0},
    /*function code:33 */ {0, 0},
    /*function code:34 */ {0, 0},
    /*function code:35 */ {0, 0},
    /*function code:36 */ {0, 0},
    /*function code:37 */ {0, 0},
    /*function code:38 */ {0, 0},
    /*function code:39 */ {0, 0},
    /*function code:40 */ {0, 0},
    /*function code:41 */ {0, 0},
    /*function code:42 */ {0, 0},
    /*function code:43 */ {0, 0} /*���豸ʶ����:��ʱ����*/

};



/*******************************************************************************
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
    step_p->reqAPDU_Obj_p    = reqAPDU_Obj_p;
    step_p->reqPDU_FCode     = reqPDU_FCode;
    step_p->reqPDU_Num       = reqPDU_Num;
    step_p->respAPDU_Obj_p   = respAPDU_Obj_p;
    step_p->respPDU_FCode    = respPDU_FCode;
    step_p->respPDU_Num      = respPDU_Num;
    step_p->stepNo           = stepNo;
    step_p->sVerifyBlock_p   = sVerifyBlock_p;
}




/*******************************************************************************
  *���ܣ�����startStep_p�в���ΪtargetStep_p->stepNo�Ĳ�
  *���룺
  *      startStep_p ������
  *      targetStep_p��Ҫ���ҵĲ�
  *�������
  *���أ�
  *      ONE_STEP*
  *              1���쳣������� NULL
  *              2�������������startStep_p�в���ΪtargetStep_p->stepNo�Ĳ�;
  *�޸�:
  *
*******************************************************************************/
ONE_STEP* displayNextNode ( ONE_STEP* startStep_p,
                            ONE_STEP* targetStep_p
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
  *���أ�void
  *�޸ģ�
  *
*******************************************************************************/
void addNode ( ONE_STEP* step_p,
               ONE_STEP* tail_step_p
             )
{
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



/*******************************************************************************
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
 *    20180411145002��T_snnCfg�ṹ��仯�����ڲ��ӿ���ʵ����;������ע���ڲ��߼�
*******************************************************************************/
int setOneFrmCfg ( u_char actChnlID,
                   u_char stepNum,
                   u_char frmNum,
                   ELE_TYPE* eleVal_p,
                   T_snnCfg* snnCfg_p
                 )
{
    //if ( actChnlID != ACTIVE_TYPE
    //     && actChnlID != PASSIVE_TYPE
    //   )
    //{
    //    return ERR_ACT_PAS_VAL;
    //}

    //if ( stepNum < 1
    //    || frmNum < 1
    //    )
    //{
    //    return ERR_STEP_NO;
    //}

    //if ( eleVal_p == NULL
    //     || snnCfg_p == NULL
    //     || snnCfg_p->stepCfg_p == NULL
    //    )
    //{
    //    return ERR_POINTER_NULL;
    //}
    //
    //T_ssnStepCfg* tStepCfg_p = &( snnCfg_p->stepCfg_p[stepNum-1] );
    //if (actChnlID == ACTIVE_TYPE)
    //{
    //    if ( tStepCfg_p->reqEleVal_pp == NULL
    //        || tStepCfg_p->reqEleVal_pp[frmNum-1] == NULL
    //        )
    //    {
    //        return ERR_POINTER_NULL;
    //    }
    //    for (int i = 0;i <= ADU_ECHK_INDEX;i++)
    //    {
    //        if ( eleVal_p[i].lng > 0 )
    //        {
    //            ELE_TYPE* tEleType_p = &(
    //                 tStepCfg_p->reqEleVal_pp[frmNum-1][i] );
    //            tEleType_p->sysEleID = eleVal_p[i].sysEleID;
    //            tEleType_p->lng = eleVal_p[i].lng;
    //            tEleType_p->p_Mem = new u_char[eleVal_p[i].lng]();
    //            memcpy(tEleType_p->p_Mem,
    //                   eleVal_p[i].p_Mem,
    //                   eleVal_p[i].lng
    //                   );
    //        }
    //    }
    //}
    //else //if (actChnlID ==PASSIVE_TYPE)
    //{
    //    if (tStepCfg_p->respEleVal_pp == NULL
    //        || tStepCfg_p->respEleVal_pp[frmNum-1] == NULL
    //        )
    //    {
    //        return ERR_POINTER_NULL;
    //    }
    //    for (int i = 0;i <= ADU_ECHK_INDEX;i++)
    //    {
    //        if ( eleVal_p[i].lng > 0 )
    //        {
    //            ELE_TYPE* tEleType_p = &(
    //                tStepCfg_p->respEleVal_pp[frmNum-1][i] );
    //            tEleType_p->sysEleID = eleVal_p[i].sysEleID;
    //            tEleType_p->lng = eleVal_p[i].lng;
    //            tEleType_p->p_Mem = new u_char[eleVal_p[i].lng]();
    //            memcpy( tEleType_p->p_Mem,
    //                    eleVal_p[i].p_Mem,
    //                    eleVal_p[i].lng
    //                  );
    //        }
    //    }
    //}

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

    u_char actFlag = ( u_char ) funDsc_p->act_prio_chnlID;

    //�Բ�������У��
    if (   actFlag != ACTIVE_TYPE
           && actFlag != PASSIVE_TYPE
       )
    {
        return ERR_ACT_PAS_VAL;
    }

    C_ModbusADU* stepObj_p = NULL;

    //ȡ��Ҫװ֡��apdu����
    if ( actFlag == ACTIVE_TYPE )
    {
        if ( nxtFrmStep_p->reqAPDU_Obj_p == NULL )
        {
            return ERR_POINTER_NULL;
        }

        stepObj_p = ( C_ModbusADU* ) nxtFrmStep_p->reqAPDU_Obj_p;
    }
    else
    {
        if ( nxtFrmStep_p->respAPDU_Obj_p == NULL )
        {
            return ERR_POINTER_NULL;
        }

        stepObj_p = ( C_ModbusADU* ) nxtFrmStep_p->respAPDU_Obj_p;
    }

    //ȡFunction code �� Unit Identifier
    PDU_FC_VAL* tmpFC_p = NULL;
    u_char tmpFCode     = 0;
    ADU_MBAP_VAL tMbapVal;//MBAP����ͷ 7�ֽ�
    T_mbsID       tMbsId;
    memset ( &tMbapVal, 0, sizeof ( tMbapVal ) );
    memset ( &tMbsId, 0, sizeof ( tMbsId ) );

    //��һ֡�����¿�ʼ��һ��
    if ( *nxtFrmStepNo_p != nxtFrmStep_p->stepNo )
    {
        tMbsId = * ( ( T_mbsID* ) & ( ssnPar_p->ASDUID_p[0] ) );
    }
    else
    {
        if ( actFlag == ACTIVE_TYPE )
        {
            tMbsId = * ( ( T_mbsID* ) & (
                         ssnPar_p->ASDUID_p[nxtFrmStep_p->reqPDU_Num] ) );
            //tmpFCode = ssnPar_p->ASDUID_p[nxtFrmStep_p->reqPDU_Num];
        }
        else
        {
            tMbsId = * ( ( T_mbsID* ) & (
                         ssnPar_p->ASDUID_p[nxtFrmStep_p->respPDU_Num] ) );
            //tmpFCode = ssnPar_p->ASDUID_p[nxtFrmStep_p->respPDU_Num];
        }
    }

    tMbapVal.trans_id = tMbsId.trans_id;
    tMbapVal.pctl_id  = 0x0;
    tMbapVal.length   = 0x0;
    tMbapVal.unit_id  = tMbsId.unit_id;
    tmpFCode          = tMbsId.func_id;
    tmpFC_p           = ( PDU_FC_VAL* ) &tmpFCode;

    ////ȡ��8λΪ����Ԫ��ʶ����
    //unitID = (tmpFCode>>(sizeof(u_int)-1)*8);
    ////��3�ֽڼ�¼����֡�����еġ������롱
    //tmpFCode = tmpFCode - (unitID<<(sizeof(u_int)-1)*8 );
    //u_char tmpFCode = *((u_char*)tmpFC_p);

    u_char tmpData[MAX_ARRAY_INDEX];
    u_char* tmpData_p        = tmpData;
    u_char tmpDataLng        = 0;
    u_short setNum           = 0;
    T_snnUsrData* tUsrData_p = & ( ssnPar_p->usrData_p[setNum] );
    memset ( tmpData, 0, sizeof ( tmpData ) );

    if ( ACTIVE_TYPE == actFlag ) //��վ
    {
        if (    tmpFCode == MC_MBFC_0x00     /*����һ��ͨ��*/
                || tmpFCode == MC_MBFC_0xFF     /*��һ��ͨ��*/
           )
        {
            /*û��������*/
            tmpDataLng = 0;
        }
        else if ( ( tmpFCode >= 1 ) && ( tmpFCode <= 6 ) )
        {
            /*
             * PDU�����������2�ֽ���ʼ��ַ+2�ֽڶ���д��������ֵ
             */
            u_short reqNum  = 0;
            u_short reqAddr = 0;

            reqAddr = tUsrData_p->gData.val_chkStd.pntAddr;
            reqNum  = tUsrData_p->gData.val_chkStd.pntVal1;
            setNum++;
            tUsrData_p = & ( ssnPar_p->usrData_p[setNum] );

            //��С��ת��
            reversEndian ( ( u_char* ) &reqAddr, sizeof ( u_short ) );
            reversEndian ( ( u_char* ) &reqNum, sizeof ( u_short ) );

            memcpy ( tmpData_p, &reqAddr, 2 );
            memcpy ( tmpData_p + 2, &reqNum, 2 );
            tmpDataLng = 4;

        }
        else if (   15 == tmpFCode  //д�����Ȧ
                    || 16 == tmpFCode  //д����Ĵ���
                )
        {
            /*
             * PDU�����������2�ֽ���ʼ��ַ+2�ֽ����������Ĵ�������
                               +1�ֽ��ֽ���+����ֵN�ֽ�
             */
            u_short reqNum  = 0;
            u_short reqAddr = 0;
            u_char byteNum  = 0;

            reqAddr = tUsrData_p->gData.val_chkStd.pntAddr;
            reqNum  = tUsrData_p->gData.val_chkStd.pntVal1;
            byteNum = tUsrData_p->gData.val_chkStd.pntVal2;
            setNum++;
            tUsrData_p = & ( ssnPar_p->usrData_p[setNum] );
            reversEndian ( ( u_char* ) &reqAddr, sizeof ( u_short ) );
            reversEndian ( ( u_char* ) &reqNum, sizeof ( u_short ) );

            memcpy ( tmpData_p,    &reqAddr, 2 );
            memcpy ( tmpData_p + 2,  &reqNum, 2 );
            memcpy ( tmpData_p + 2 + 2, &byteNum, 1 );
            tmpDataLng = 5;

            for ( u_int j = 0; j < byteNum; j++ )
            {
                memcpy ( tmpData_p + 2 + 2 + 1 + j,
                         &tUsrData_p->gData.val_chkStd.pntVal1,
                         1
                       );
                setNum++;
                tUsrData_p = & ( ssnPar_p->usrData_p[setNum] );
                tmpDataLng++;
            }

        }
        else if (  20 == tmpFCode //���ļ���¼
                   || 21 == tmpFCode //д�ļ���¼
                )
        {
            /*�Ӻ�ʵ��*/
        }
        else if ( 22 == tmpFCode ) //����д�Ĵ���
        {
            /*
             * PDU������������ο���ַ��2�ֽڣ�+ And_Mask(2�ֽ�)
                               + Or_Mask(2�ֽ�)
             */
            u_short reqAddr = 0;
            u_short reqAnd  = 0;
            u_short reqOr   = 0;
            reqAddr = ( u_short ) tUsrData_p->gData.val_chkStd.pntAddr;
            reqAnd  = ( u_short ) tUsrData_p->gData.val_chkStd.pntVal1;
            reqOr   = ( u_short ) tUsrData_p->gData.val_chkStd.pntVal2;
            reversEndian ( ( u_char* ) &reqAddr, sizeof ( u_short ) );
            reversEndian ( ( u_char* ) &reqAnd, sizeof ( u_short ) );
            reversEndian ( ( u_char* ) &reqOr,  sizeof ( u_short ) );


            memcpy ( tmpData_p,      &reqAddr, 2 );
            memcpy ( tmpData_p + 2,  &reqAnd,  2 );
            memcpy ( tmpData_p + 2 + 2, &reqOr,   2 );

            tmpDataLng = 6;

            setNum++;
            tUsrData_p = & ( ssnPar_p->usrData_p[setNum] );
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

            reqRAddr = tUsrData_p->gData.val_chkStd.pntAddr;
            reqRNum = tUsrData_p->gData.val_chkStd.pntVal1;
            setNum++;
            tUsrData_p = & ( ssnPar_p->usrData_p[setNum] );

            reqWAddr = tUsrData_p->gData.val_chkStd.pntAddr;
            reqWNum = tUsrData_p->gData.val_chkStd.pntVal1;
            reqWByte = tUsrData_p->gData.val_chkStd.pntVal2;
            setNum++;
            tUsrData_p = & ( ssnPar_p->usrData_p[setNum] );

            reversEndian ( ( u_char* ) &reqRAddr, sizeof ( u_short ) );
            reversEndian ( ( u_char* ) &reqRNum, sizeof ( u_short ) );
            reversEndian ( ( u_char* ) &reqWAddr, sizeof ( u_short ) );
            reversEndian ( ( u_char* ) &reqWNum, sizeof ( u_short ) );

            memcpy ( tmpData_p,        &reqRAddr, 2 );
            memcpy ( tmpData_p + 2,      &reqRNum,  2 );
            memcpy ( tmpData_p + 2 + 2,    &reqWAddr, 2 );
            memcpy ( tmpData_p + 2 + 2 + 2,  &reqWNum,  2 );
            memcpy ( tmpData_p + 2 + 2 + 2 + 2, &reqWByte, 1 );
            tmpDataLng = 9;

            for ( u_int j = 0; j < reqWByte; j++ )
            {
                memcpy ( tmpData_p + 2 + 2 + 2 + 2 + 1 + j,
                         &tUsrData_p->gData.val_chkStd.pntVal1,
                         1
                       );
                setNum++;
                tUsrData_p = & ( ssnPar_p->usrData_p[setNum] );
                tmpDataLng++;
            }

        }
        else if ( 43 == tmpFCode ) //���豸ʶ����
        {
            /*�ݲ�ʵ��*/
        }
    }
    else////////////////////////////����վ���վ ��Ӧ����////////////////////////
    {
        u_short offSetNum = 0;

        if (   tmpFCode == MC_MBFC_0x00  /*����һ��ͨ��*/
               || tmpFCode == MC_MBFC_0xFF /*��һ��ͨ��*/
           )
        {
            /*û��������*/
            tmpDataLng = 0;
        }
        else if ( tmpFCode >= 1 && tmpFCode <= 4 )
        {
            /*
             * ��ӦPDU������������ֽ�����1�ֽ�)+״̬��ֵ��N�ֽڣ�
             */
            setNum             = 0;
            u_char respByteNum = 0;
            u_char tData       = 0;
            u_int  tInt        = 0;

            if ( tmpFCode == 1
                 || tmpFCode == 2
               )
            {
                /*NΪ��Ȧ����������ֽ�����N/8,�������������0��
                                    ��ô �ֽ�����N/8 + 1
                 */
                if ( ssnPar_p->pntNum % 8 != 0 )
                {
                    respByteNum = ssnPar_p->pntNum / 8 + 1;
                }
                else
                {
                    respByteNum = ssnPar_p->pntNum / 8;
                }

                //���ֽ�������pdu�ĵ�һ���ֽ�
                memcpy ( tmpData_p, &respByteNum, 1 );

                //��ÿһ����Ȧ��״�ϳ�ֵ
                for ( int i = 0; i < ssnPar_p->pntNum; i++ )
                {
                    tInt  = tUsrData_p->gData.val_chkStd.pntVal1;
                    tData = tData + ( ( ( u_char ) tInt ) << ( i % 8 ) );

                    if ( ( i + 1 ) % 8 == 0 )
                    {
                        memcpy ( tmpData_p + 1 + ( i + 1 ) / 8, &tData, 1 );
                        tData = 0;
                    }
                    //����8λ����Ҳ����һ���ֽڵ�ֵ
                    else if ( i + 1 == ssnPar_p->pntNum )
                    {
                        memcpy ( tmpData_p + 1 + ( i + 1 ) / 8 + 1, &tData, 1 );
                        tData = 0;
                    }

                    setNum++;
                    tUsrData_p = & ( ssnPar_p->usrData_p[setNum] );
                }

                tmpDataLng = respByteNum + 1;
            }
            else // 3 4
            {
                //NΪ�Ĵ��������� �ֽ�����2*N
                respByteNum = ssnPar_p->pntNum * 2;
                //���ֽ�������pdu�ĵ�һ���ֽ�
                memcpy ( tmpData_p, &respByteNum, 1 );

                u_short tRgtVal = 0;

                //���Ĵ�����ֵ���
                for ( int i = 0; i < ssnPar_p->pntNum; i++ )
                {
                    tRgtVal = ( u_short ) tUsrData_p->gData.val_chkStd.pntVal1;
                    //x86ΪС�˴洢����ֵת����mobbus�Ĵ��
                    reversEndian ( ( u_char* ) &tRgtVal, sizeof ( tRgtVal ) );
                    memcpy ( tmpData_p + 1 + ( i * 2 ), &tRgtVal, 2 );
                    setNum++;
                    tUsrData_p = & ( ssnPar_p->usrData_p[setNum] );
                }

                tmpDataLng = respByteNum + 1;
            }
        }
        else if ( 5 == tmpFCode  //д������Ȧ
                  || 6 == tmpFCode //д�����Ĵ���
                )
        {
            /*
             * ��ӦPDU�������������ַ��2�ֽ�)+ֵ��2�ֽڣ�
             */
            u_short uSh = 0;
            uSh = tUsrData_p->gData.val_chkStd.pntAddr;
            reversEndian ( ( u_char* ) &uSh, sizeof ( u_short ) );
            memcpy ( tmpData_p, &uSh, 2 );

            uSh = tUsrData_p->gData.val_chkStd.pntVal1;
            reversEndian ( ( u_char* ) &uSh, sizeof ( u_short ) );
            memcpy ( tmpData_p + 2, &uSh, 2 );

            tmpDataLng = 4;
            setNum++;
            tUsrData_p = & ( ssnPar_p->usrData_p[setNum] );
        }
        else if (   15 == tmpFCode  //д�����Ȧ
                    || 16 == tmpFCode  //д����Ĵ���
                )
        {
            /*
             * ��ӦPDU�������������ʼ��ַ��2�ֽ�)+������2�ֽڣ�
             */
            u_short uSh = 0;
            uSh = tUsrData_p->gData.val_chkStd.pntAddr;
            reversEndian ( ( u_char* ) &uSh, sizeof ( u_short ) );
            memcpy ( tmpData_p, &uSh, 2 );

            if ( 15 == tmpFCode ) //����ֽ���
            {
                if ( ssnPar_p->pntNum % 8 == 0 )
                {
                    uSh = ssnPar_p->pntNum / 8;
                }
                else
                {
                    uSh = ssnPar_p->pntNum / 8 + 1;
                }

            }
            else// 16
            {
                uSh = ssnPar_p->pntNum * 2;
            }

            //uSh = tUsrData_p->gData.val_chkStd.pntVal1;
            reversEndian ( ( u_char* ) &uSh, sizeof ( u_short ) );
            memcpy ( tmpData_p + 2, &uSh, 2 );

            tmpDataLng = 4;
            setNum++;
            tUsrData_p = & ( ssnPar_p->usrData_p[setNum] );

        }
        else if (   20 == tmpFCode  //���ļ���¼
                    || 21 == tmpFCode  //д�ļ���¼
                )
        {
            /*�Ӻ�ʵ��*/
        }
        else if ( 22 == tmpFCode ) //����д�Ĵ���
        {
            /*
             * ��ӦPDU������������ο���ַ��2�ֽڣ�+ And_Mask(2�ֽ�)
                                   + Or_Mask(2�ֽ�)
             */
            u_short tUshort = 0;
            tUshort = ( u_short ) tUsrData_p->gData.val_chkStd.pntAddr;
            reversEndian ( ( u_char* ) &tUshort, sizeof ( u_short ) );
            memcpy ( tmpData_p, &tUshort, 2 );

            tUshort = ( u_short ) tUsrData_p->gData.val_chkStd.pntVal1;
            reversEndian ( ( u_char* ) &tUshort, sizeof ( u_short ) );
            memcpy ( tmpData_p + 2, &tUshort, 2 );

            tUshort = ( u_short ) tUsrData_p->gData.val_chkStd.pntVal2;
            reversEndian ( ( u_char* ) &tUshort, sizeof ( u_short ) );
            memcpy ( tmpData_p + 2 + 2, &tUshort, 2 );

            tmpDataLng = 6;
            setNum++;
            tUsrData_p = & ( ssnPar_p->usrData_p[setNum] );

        }
        else if ( 23 == tmpFCode ) //��д����Ĵ���
        {
            u_short tUshort = 0;
            /*
             * ��ӦPDU������������ֽ�����1�ֽڣ�+ ���Ĵ���ֵ(N�ֽ�)
             */
            u_char respByteNum = 0;
            respByteNum = ssnPar_p->pntLoadNum * 2;

            //respByteNum = tUsrData_p->gData.val_chkStd.pntVal1;
            memcpy ( tmpData_p, &respByteNum, 1 );
            tmpDataLng = 1;
            offSetNum = 1;
            //setNum++;

            for ( u_int i = 0; i < respByteNum / 2; i++ )
            {
                tUshort = ( u_short ) tUsrData_p->gData.val_chkStd.pntVal1;
                reversEndian ( ( u_char* ) &tUshort, sizeof ( u_short ) );

                memcpy ( tmpData_p + offSetNum, &tUshort, 2 );
                setNum++;
                tUsrData_p = & ( ssnPar_p->usrData_p[setNum] );
                offSetNum = offSetNum + 2;
            }

            tmpDataLng = respByteNum + 1;
        }
        else if ( 43 == tmpFCode ) //���豸ʶ����
        {
            /*�ݲ�ʵ��*/
        }
    }



    //���¶�����������ƽṹ�ĳ���Ϊʵ��ֵ
    stepObj_p->getDU_Info()->InfoEleLng = 1;
    stepObj_p->getDU_Info()->ADU_CMTBL[PDU_DATA_INDEX] = tmpDataLng;
    //����MBAP����ͷ�ĳ�����ֵ
    tMbapVal.length = tmpDataLng + 2;
    //ƴװ����ͷ��ĳһ���Ķ�����
    stepObj_p->setADU_MBAPH_VAL ( &tMbapVal );
    //��ƴװ��PDU�������빦���븳��ĳһ���Ķ�����
    stepObj_p->setPDU_FC_VAL ( tmpFC_p );

    if ( tmpDataLng > 0 )
    {
        stepObj_p->setADU_ELEMENT ( tmpData_p,
                                    tmpDataLng,
                                    PDU_DATA_INDEX
                                  );
    }

    //��������һ֡��Ӧ����ֵ
    nxtFrmStep_p->stepNo = nxtFrmRlStep_p->stepNo;
    //nxtFrmStep_p->sVerifyBlock_p = NULL;

    if ( actFlag == ACTIVE_TYPE ) //����
    {
        nxtFrmStep_p->reqPDU_FCode = nxtFrmRlStep_p->reqPDU_FCode;

    }
    else // (actFlag == PASSIVE_TYPE) //����
    {
        nxtFrmStep_p->respPDU_FCode = nxtFrmRlStep_p->respPDU_FCode;
    }

    return 0;
}




/*******************************************************************************
  *���ܣ�modbus���ղ�У�鹦��
  *
  * ���룺
  *     nowRuleStep_p :��ǰ������
  *     lastRuleStep_p:���򲽵����һ��
  *     stepNo        :��ǰ����
  *     funDsc_p      :�Ự����������
  *     mbap_p        :��Ҫ��֤���ݵĶ�ӦC_MBAP����
  *�����
  *     ��
  *���أ�
  *     0    �ɹ�
  *     <0   ����
  *�޸���ʷ
  *
*******************************************************************************/
int chkRecData (  ONE_STEP* nowRuleStep_p,
                  ONE_STEP* lastRuleStep_p,
                  const u_char& stepNo,
                  T_ptclFunDsc* funDsc_p,
                  C_ModbusADU* mbap_p
               )
{


    //�������ԻỰ��ʶ
    u_char act_prio_chnlID   = ( u_char ) funDsc_p->act_prio_chnlID;
    //ģʽ��
    u_char usingMode         = funDsc_p->usingMode;
    //����������֡�������޶�ֵ
    u_char allFrameNum_req  = nowRuleStep_p->reqPDU_Num;
    //��������Ӧ֡�������޶�ֵ
    u_char allFrameNum_resp = nowRuleStep_p->respPDU_Num;

    if ( ! ( ASSERTMBFC ( funDsc_p->ssnDscNo ) ) )
    {
        return ERR_FC_VAL_RAN;
    }

    if ( lastRuleStep_p->stepNo > C_STEP_NO_TWO )
    {
        //��󲽺�Ϊ2
        return ERR_CONFIG;
    }

    //ȡFunction code
    PDU_FC_VAL* tmpFC_p = mbap_p->getPDU_FC_VAL (
                          mbap_p->getDU_Info()->ADU_CMTBL[PDU_FC_INDEX]
                          );
    u_char tmpFCode   = * ( ( u_char* ) tmpFC_p );
    u_char tmpFCode_c = tmpFCode;

    if ( act_prio_chnlID == ACTIVE_TYPE
         && tmpFCode > 0x80 //��Ӧ���쳣��
       )
    {
        tmpFCode_c = ( tmpFCode & 0x7F );
    }

    if ( ! ( ASSERTMBFC ( tmpFCode_c ) ) )
    {
        return ERR_FC_VAL_RAN;
    }

    if ( act_prio_chnlID == PASSIVE_TYPE )
    {
        //����վ�Ự��������վASDU
        //if (allFrameNum_req != 1)
        //{
        //    return ERR_FRAME_NO;
        //}
        //if( stepNo != 1 )//����վ������һ�����ڵ�һ��
        //{
        //    return ERR_STEP_NO;
        //}

    }
    else if ( act_prio_chnlID == ACTIVE_TYPE )
    {
        //��վ���յ��Ǳ���վ������
        /*if (allFrameNum_resp != 1)
        {
        return ERR_FRAME_NO;
        }*/

    }


    //   switch( funDsc_p->ssnDscNo )
    //{
    //   case STN_INIT: //վ��ʼ��;
    //       {
    //           break;
    //       }
    //   case QUE_DATA: //modbusĬ�ϻỰ
    //       {
    //           if (lastRuleStep_p->stepNo > C_STEP_NO_TWO)
    //           {//��󲽺�Ϊ2
    //               return ERR_CONFIG;
    //           }

    //           //ȡFunction code
    //           PDU_FC_VAL *tmpFC_p = mbap_p->getPDU_FC_VAL(
    //               mbap_p->getDU_Info()->ADU_CMTBL[PDU_FC_INDEX]
    //           );
    //           u_char tmpFCode   = *((u_char*)tmpFC_p);
    //           u_char tmpFCode_c = tmpFCode;

    //           if (act_prio_chnlID == ACTIVE_TYPE
    //               && tmpFCode >0x80 //��Ӧ���쳣��
    //               )
    //           {
    //               tmpFCode_c = (tmpFCode &0x7F);
    //           }

    //           if ( !((tmpFCode_c>=1 && tmpFCode_c<=6)
    //                 || 15==tmpFCode_c
    //                 || 16==tmpFCode_c
    //                 || (tmpFCode_c>=20 && tmpFCode_c<=23)
    //                 || 43==tmpFCode_c
    //                )
    //               )
    //           {
    //               return ERR_FC_VAL_RAN;
    //           }

    //           if (act_prio_chnlID == PASSIVE_TYPE )
    //           {//����վ�Ự��������վASDU
    //               //if (allFrameNum_req != 1)
    //               //{
    //               //    return ERR_FRAME_NO;
    //               //}
    //               //if( stepNo != 1 )//����վ������һ�����ڵ�һ��
    //               //{
    //               //    return ERR_STEP_NO;
    //               //}
    //
    //           }
    //           else if (act_prio_chnlID == ACTIVE_TYPE)
    //           {//��վ���յ��Ǳ���վ������
    //               /*if (allFrameNum_resp != 1)
    //               {
    //               return ERR_FRAME_NO;
    //               }*/
    //
    //           }
    //           break;
    //       }
    //   default:
    //           break;
    //}


    return 0;
}




/*******************************************************************************
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
int getChkNxtStepNo ( FUN_RL* Fun_p,
                      ONE_STEP* stepRl_p,
                      T_ptclFunDsc* funDsc_p,
                      T_stnAcsFlagM* stnAcsFlag_p,
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
    ONE_STEP* nowStep_p     = stepRl_p;

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

    T_stnAcsFlagM tmpStnAcsFlag = {0, 0, 1, 0};
    *stnAcsFlag_p = tmpStnAcsFlag;

    *nxtStepNo_p  = nowStep_p->stepNo;

    //�������Ự��ʶ
    u_char actFlag = ( u_char ) funDsc_p->act_prio_chnlID;

    switch ( actFlag )
    {
        case PASSIVE_TYPE://�����Ự������վ����
        {
            if ( nowFrameNum_req == allFrameNum_req ) //����֡�ﵽ�޶�ֵ
            {
                if ( nowFrameNum_resp == allFrameNum_resp ) //��Ӧ֡Ҳ�ﵽ�޶�ֵ
                {
                    if ( nxtRuleStep_p != NULL ) //�����ǰ���������һ��
                    {
                        *nxtStepNo_p = nxtRuleStep_p->stepNo;
                        stnAcsFlag_p->ctlType = 2; //��һ��ϵͳִ������ֵ�ļ���
                    }
                }
                else if ( nowFrameNum_resp > allFrameNum_resp ) //��Ӧ֡����
                {
                    return ERR_FRAME_NO;
                }
            }
            else if ( nowFrameNum_req > allFrameNum_req ) //����֡����
            {
                return ERR_FRAME_NO;
            }

            break;
        }

        case ACTIVE_TYPE://�����Ự���ձ���վ����
        {
            if ( nowFrameNum_resp == allFrameNum_resp ) //��Ӧ֡�ﵽ�޶�ֵ
            {
                if ( nowFrameNum_req == allFrameNum_req ) //����֡Ҳ�ﵽ�޶�ֵ
                {
                    if ( nxtRuleStep_p != NULL ) //�����ǰ���������һ��
                    {
                        *nxtStepNo_p = nxtRuleStep_p->stepNo;
                        stnAcsFlag_p->ctlType = 2; //��һ��ϵͳִ������ֵ�ļ���
                    }
                }
                else if ( nowFrameNum_req > allFrameNum_req ) //����֡����
                {
                    return ERR_FRAME_NO;
                }
            }
            else if ( nowFrameNum_resp > allFrameNum_resp ) //��Ӧ֡����
            {
                return ERR_FRAME_NO;
            }

            break;
        }

        default:
            break;
    }

    return 0;
}




/*******************************************************************************
  *���ܣ�
  *    �Ự����֡�������պ��T_snnPara��ֵ
  * ���룺
  *     stepRl_p  �����պ�ĵ�ǰ��
  *     funDsc_p  ���Ự����������
  *     mbap_p    ��C_MBAP����ָ��
  *     stnAcsFlag���洢����ȡ��ʶ
  *�����
  *     ssnPar_p��  ���������˵��
  *���أ�
  *     0 �ɹ�
  *    <0 ʧ��
  *�޸���ʷ
  *
*******************************************************************************/
int asgmtSnnPar ( ONE_STEP* stepRl_p,
                  T_ptclFunDsc* funDsc_p,
                  C_ModbusADU* mbap_p,
                  T_stnAcsFlagM* stnAcsFlag,
                  T_snnPara* ssnPar_p
                )
{

    if ( stepRl_p == NULL
         || funDsc_p == NULL
         || mbap_p == NULL
         || ssnPar_p == NULL
         || ssnPar_p->ASDUID_p == NULL
         || ssnPar_p->usrData_p == NULL
       )
    {
        return ERR_POINTER_NULL;
    }

    /****************************��C_MBAP������ȡֵ**************************/
    ADU_MBAP_VAL* tMbapVal_p = NULL; //MBAP����ͷ 7�ֽ�
    T_mbsID       tMbsId;
    memset ( &tMbsId, 0, sizeof ( tMbsId ) );

    tMbapVal_p = mbap_p->getADU_MBAPH_VAL (
                 mbap_p->getDU_Info()->ADU_CMTBL[ADU_MBAPH_INDEX]
                 );
    //ȡFunction code
    PDU_FC_VAL* tmpFC_p = mbap_p->getPDU_FC_VAL (
                          mbap_p->getDU_Info()->ADU_CMTBL[PDU_FC_INDEX]
                          );
    u_char tmpFCode = * ( ( u_char* ) tmpFC_p );

    tMbsId.trans_id = tMbapVal_p->trans_id;
    tMbsId.unit_id  = tMbapVal_p->unit_id;
    tMbsId.func_id  = tmpFCode;

    //ȡPDU������ָ��
    u_short tDataLng = 0;
    u_char* tmpData_p = mbap_p->getADU_ELEMENT (
                        tDataLng,
                        PDU_DATA_INDEX
                        );

    /*******************************��ֵT_snnPara**************************/
    ssnPar_p->stepNo      = stepRl_p->stepNo;
    ssnPar_p->infoType    = GINFOTYPE;
    ssnPar_p->ASDUID_p[0] = * ( ( u_int* ) &tMbsId );

    u_char tActFlag = ( u_char ) funDsc_p->act_prio_chnlID;

    /*������֡�� ����ֵ����ĳһ֡���޸ģ�
    ssnPar_p->ASDUIDNum = 1;
    */
    if ( tActFlag == PASSIVE_TYPE )
    {
        //����վ��������վ������
        if ( 0 == stepRl_p->reqPDU_Num )
        {
            return ERR_FRAME_NO;
        }

        ssnPar_p->actFlag = ACTIVE_TYPE;
    }
    else if ( tActFlag == ACTIVE_TYPE )
    {
        //����վ���ձ���վ������;
        if ( 0 == stepRl_p->respPDU_Num )
        {
            return ERR_FRAME_NO;
        }

        ssnPar_p->actFlag = PASSIVE_TYPE;

    }

    u_short setNum             = 0;
    T_snnUsrData* tUsrData_p   = ssnPar_p->usrData_p + setNum;
    T_gVal_ChkStd* valChkStd_p = & ( tUsrData_p->gData.val_chkStd );

    if ( ! ( ASSERTMBFC ( funDsc_p->ssnDscNo ) ) )
    {
        //�Ự�ţ������룩����
        return ERR_FC_VAL_RAN;
    }

    //��Ӧ�ù��ܵĲ�ͬ�ֱ��Ԫ�صĽ������
    //ssnPar_p->pntPos = 0;
    if ( tActFlag == PASSIVE_TYPE )
    {
        stnAcsFlag->ctlType = 2;

        if (   tmpFCode == MC_MBFC_0x00 /*����һ��ͨ��*/
               || tmpFCode == MC_MBFC_0xFF /*��һ��ͨ��*/
           )
        {
            stnAcsFlag->ctlType       = 0;
            tUsrData_p->gData.pntType = g_FCMDBUSLNG[tmpFCode].byteLng;
            valChkStd_p->pntAddr      = 0;
            valChkStd_p->pntVal1      = 0;
            setNum++;
            tUsrData_p  = ssnPar_p->usrData_p + setNum;
            valChkStd_p = & ( tUsrData_p->gData.val_chkStd );
        }
        else if ( tmpFCode >= 1 && tmpFCode <= 6 )
        {
            /*
                * PDU�����������2�ֽ���ʼ��ַ+2�ֽڶ���д��������ֵ
                */
            u_short reqNum  = 0;
            u_short reqAddr = 0;
            memcpy ( &reqAddr, tmpData_p, 2 );
            memcpy ( &reqNum, tmpData_p + 2, 2 );

            //modbus�Ǵ�ˣ�x86��С����Ҫת��
            reversEndian ( ( u_char* ) &reqAddr, sizeof ( u_short ) );
            reversEndian ( ( u_char* ) &reqNum, sizeof ( u_short ) );

            //������ĵ�ַ������ssnPar_p�е�pntAddr ��ȥ
            for ( int i = 0; i < reqNum; i++ )
            {
                tUsrData_p->gData.pntType = g_FCMDBUSLNG[tmpFCode].byteLng;
                valChkStd_p->pntAddr      = reqAddr + setNum;
                setNum++;
                tUsrData_p  = ssnPar_p->usrData_p + setNum;
                valChkStd_p = & ( tUsrData_p->gData.val_chkStd );
            }

            /*valChkStd_p->pntAddr = reqAddr;
            valChkStd_p->pntVal1 = reqNum;
            setNum++;
            tUsrData_p = ssnPar_p->usrData_p+setNum;
            valChkStd_p = &( tUsrData_p->gData.val_chkStd );
            */
        }
        else if ( 15 == tmpFCode     //д�����Ȧ
                  || 16 == tmpFCode  //д����Ĵ���
                )
        {
            stnAcsFlag->ctlType = 1;
            /*
                * PDU�����������2�ֽ���ʼ��ַ+2�ֽ����������Ĵ�������
                                +1�ֽ��ֽ���+����ֵN�ֽ�
                */
            u_short reqNum   = 0;
            u_short reqAddr  = 0;
            u_char byteNum   = 0;
            u_char* reqVal_p = NULL;
            u_short reqVal   = 0;

            memcpy ( &reqAddr, tmpData_p,         2 );
            memcpy ( &reqNum,  tmpData_p + 2,    2 );
            memcpy ( &byteNum, tmpData_p + 2 + 2, 1 );
            reqVal_p = tmpData_p + 2 + 2 + 1;

            reversEndian ( ( u_char* ) &reqAddr, sizeof ( u_short ) );
            reversEndian ( ( u_char* ) &reqNum,  sizeof ( u_short ) );

            u_int vFlag = 0;

            if ( 15 == tmpFCode )
            {
                for ( int i = 0; i < reqNum; i++ )
                {
                    tUsrData_p->gData.pntType = g_FCMDBUSLNG[tmpFCode].byteLng;
                    valChkStd_p->pntAddr = reqAddr + i;

                    if ( byteNum >= 2 && i % 16 == 0 )
                    {
                        memcpy ( &reqVal, reqVal_p + ( i + 1 ) / 8, 2 );
                        reversEndian ( ( u_char* ) &reqVal, sizeof ( u_short ) );
                        byteNum = byteNum - 2;
                        //reqVal_p = reqVal_p +2;
                        vFlag = 1;
                    }
                    else if ( i % 8 == 0 ) //byteNum =1
                    {
                        memcpy ( &reqVal, reqVal_p + ( i + 1 ) / 8, 1 );
                        byteNum = 0;
                        //reqVal_p = reqVal_p +1;
                        vFlag = 0;
                    }

                    if ( vFlag == 1 )
                    {
                        valChkStd_p->pntVal1 = (
                                               ( reqVal >> ( i % 16 ) ) & 0x01 );
                    }
                    else
                    {
                        valChkStd_p->pntVal1 = (
                                               ( reqVal >> ( i % 8 ) ) & 0x01 );
                    }

                    setNum++;
                    tUsrData_p = ssnPar_p->usrData_p + setNum;
                    valChkStd_p = & ( tUsrData_p->gData.val_chkStd );
                }
            }
            else//16
            {
                for ( int i = 0; i < reqNum; i++ )
                {
                    tUsrData_p->gData.pntType = g_FCMDBUSLNG[tmpFCode].byteLng;
                    valChkStd_p->pntAddr = reqAddr + i;
                    memcpy ( &reqVal, reqVal_p + i * 2, 2 );
                    reversEndian ( ( u_char* ) &reqVal, sizeof ( u_short ) );
                    valChkStd_p->pntVal1 = reqVal;

                    setNum++;
                    tUsrData_p = ssnPar_p->usrData_p + setNum;
                    valChkStd_p = & ( tUsrData_p->gData.val_chkStd );
                }
            }

        }
        else if ( 20 == tmpFCode  //���ļ���¼
                  || 21 == tmpFCode //д�ļ���¼
                )
        {
            /*�Ӻ�ʵ��*/
        }
        else if ( 22 == tmpFCode ) //����д�Ĵ���
        {
            stnAcsFlag->ctlType = 1;
            /*
                * PDU������������ο���ַ��2�ֽڣ�+ And_Mask(2�ֽ�)
                                + Or_Mask(2�ֽ�)
                */
            tUsrData_p->gData.pntType = g_FCMDBUSLNG[tmpFCode].byteLng;
            u_short tUshort           = 0;
            memcpy ( &tUshort, tmpData_p, 2 );
            reversEndian ( ( u_char* ) &tUshort, sizeof ( u_short ) );
            valChkStd_p->pntAddr = tUshort;

            memcpy ( &tUshort, tmpData_p + 2, 2 );
            reversEndian ( ( u_char* ) &tUshort, sizeof ( u_short ) );
            valChkStd_p->pntVal1 = tUshort;

            memcpy ( &tUshort, tmpData_p + 2 + 2, 2 );
            reversEndian ( ( u_char* ) &tUshort, sizeof ( u_short ) );
            valChkStd_p->pntVal2 = tUshort;

            setNum++;
            tUsrData_p = ssnPar_p->usrData_p + setNum;
            valChkStd_p = & ( tUsrData_p->gData.val_chkStd );

        }
        else if ( 23 == tmpFCode ) //��д����Ĵ���
        {
            ssnPar_p->pntStorNum = 0;
            ssnPar_p->pntLoadNum = 0;
            u_short tUshort = 0;
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

            memcpy ( &reqRAddr, tmpData_p, 2 );
            offsetNum += 2;
            memcpy ( &reqRNum, tmpData_p, 2 );
            offsetNum += 2;

            memcpy ( &reqWAddr, tmpData_p + offsetNum, 2 );
            offsetNum += 2;
            memcpy ( &reqWNum, tmpData_p + offsetNum, 2 );
            offsetNum += 2;
            memcpy ( &reqWByte, tmpData_p + offsetNum, 1 );
            offsetNum += 1;
            reqWVal_p = tmpData_p + offsetNum;

            //��С��ת��
            reversEndian ( ( u_char* ) &reqRAddr, sizeof ( u_short ) );
            reversEndian ( ( u_char* ) &reqRNum, sizeof ( u_short ) );
            reversEndian ( ( u_char* ) &reqWAddr, sizeof ( u_short ) );
            reversEndian ( ( u_char* ) &reqWNum, sizeof ( u_short ) );


            for ( int i = 0; i < reqRNum; i++ ) //�����������ݵ�ssnPar_p�ṹ��ȥ
            {
                tUsrData_p->gData.pntType = g_FCMDBUSLNG[tmpFCode].byteLng;
                valChkStd_p->pntAddr      = reqRAddr + i;

                setNum++;
                tUsrData_p  = ssnPar_p->usrData_p + setNum;
                valChkStd_p = & ( tUsrData_p->gData.val_chkStd );

                ssnPar_p->pntLoadNum++;
            }

            ssnPar_p->pntPos = setNum;//д�Ŀ�ʼ�±�λ��

            //����д�����ݵ�ssnPar_p�ṹ��ȥ
            for ( int j = 0; j < reqWNum; j++ )
            {
                tUsrData_p->gData.pntType = g_FCMDBUSLNG[tmpFCode].byteLng;
                valChkStd_p->pntAddr      = reqWAddr + j;

                memcpy ( &tUshort, reqWVal_p + j * 2, 2 );
                //��С��ת��
                reversEndian ( ( u_char* ) &tUshort, sizeof ( u_short ) );
                valChkStd_p->pntVal1 = tUshort;

                setNum++;
                tUsrData_p  = ssnPar_p->usrData_p + setNum;
                valChkStd_p = & ( tUsrData_p->gData.val_chkStd );
                ssnPar_p->pntStorNum++;
            }

            stnAcsFlag->ctlType = 1;

        }
        else if ( 43 == tmpFCode ) //���豸ʶ����
        {
            /*�ݲ�ʵ��*/
        }

        ssnPar_p->pntNum = setNum;
    }//end ����վ��������
    else if ( tActFlag == ACTIVE_TYPE ) //����վ
    {
        if ( tmpFCode > 0x80 ) //�����
        {
            tUsrData_p->gData.pntType = g_FCMDBUSLNG[tmpFCode].byteLng;
            //�쳣��ӦPDU���ݣ�����루1�ֽڣ�+ �쳣�루1�ֽڣ�
            //valChkStd_p->pntVal1;
            memcpy ( & ( valChkStd_p->pntVal1 ),
                     tmpData_p,
                     1
                   );
            setNum++;
            tUsrData_p  = ssnPar_p->usrData_p + setNum;
            valChkStd_p = & ( tUsrData_p->gData.val_chkStd );
        }
        else//������Ӧ������
        {
            stnAcsFlag->ctlType = 3;
            stnAcsFlag->pad     = 4095;
            u_short offSetNum   = 0;

            if (   tmpFCode == MC_MBFC_0x00    /*����һ��ͨ��*/
                   || tmpFCode == MC_MBFC_0xFF    /*��һ��ͨ��*/
               )
            {
                stnAcsFlag->ctlType       = 0;
                tUsrData_p->gData.pntType = g_FCMDBUSLNG[tmpFCode].byteLng;
                valChkStd_p->pntAddr      = 0;
                valChkStd_p->pntVal1      = 0;
                setNum++;
                tUsrData_p  = ssnPar_p->usrData_p + setNum;
                valChkStd_p = & (
                              tUsrData_p->gData.val_chkStd );
            }
            else if ( tmpFCode >= 1 && tmpFCode <= 4 )
            {
                u_short reqNum  = 0;
                u_short reqAddr = 0;
                u_char  tmpC    = 0;

                /*
                    * ��ӦPDU������������ֽ�����1�ֽ�)+״̬��ֵ��N�ֽڣ�
                    */
                u_char respByteNum = 0;

                if ( tmpData_p != NULL )
                {
                    memcpy ( &respByteNum, tmpData_p, 1 );
                }

                if ( tmpFCode == 1
                     || tmpFCode == 2
                   )
                {
                    /*ssnPar_p��ʱ��ֵΪ����վ ��������֡������*/

                    //��ʼ��ַ
                    reqAddr = valChkStd_p->pntAddr;
                    //��Ȧ����
                    reqNum = valChkStd_p->pntVal1;

                    //�Ա���վ��Ӧ���ֽ�������У��
                    if ( reqNum > respByteNum * 8 )
                    {
                        //��Ӧ�������������������һ��������
                        return ERR_RESP_NUM;
                    }

                    //��tmpData_p�е�ֵ������ssnPar_p�ṹ��ȥ
                    for ( int i = 0; i < reqNum; i++ )
                    {
                        if ( i % 8 == 0 ) //ÿ��ȡһ���ֽڵ���Ȧ״ֵ̬
                        {
                            memcpy ( &tmpC, tmpData_p + 1 + ( i + 1 ) / 8, 1 );
                        }

                        tUsrData_p->gData.pntType = g_FCMDBUSLNG[tmpFCode].byteLng;
                        valChkStd_p->pntAddr = (
                                               reqAddr + setNum );

                        //fusk modify@20171211112607
                        //tUsrData_p->gData.val_chkStd.pntVal1 = (
                        //    tmpC&(i%8 + 1) );
                        if ( ( tmpC & ( i % 8 + 1 ) ) > 0 )
                        {
                            //�� (i%8 + 1) λ��ֵ
                            valChkStd_p->pntVal1 = MC_COIL_STATUS_ON;
                        }
                        else
                        {
                            valChkStd_p->pntVal1 = MC_COIL_STATUS_OFF;
                        }

                        setNum++;
                        tUsrData_p = ssnPar_p->usrData_p + setNum;
                        valChkStd_p = & (
                                      tUsrData_p->gData.val_chkStd );
                    }

                }
                else //3 4
                {
                    u_short tUs = 0;
                    /*ssnPar_p��ʱ��ֵΪ����վ ��������֡������*/
                    //��ʼ��ַ
                    reqAddr = valChkStd_p->pntAddr;
                    //�Ĵ�������
                    reqNum = valChkStd_p->pntVal1;

                    //�Ա���վ��Ӧ���ֽ�������У��
                    if ( reqNum * 2 != respByteNum )
                    {
                        //��Ӧ�������������������һ��������
                        return ERR_RESP_NUM;
                    }

                    //��tmpData_p�е�ֵ������ssnPar_p�ṹ��ȥ
                    for ( int i = 0; i < reqNum; i++ )
                    {
                        //һ���Ĵ�����ֵ��Ӧ2���ֽ�
                        memcpy ( &tUs, tmpData_p + 1 + i * 2, 2 );

                        tUsrData_p->gData.pntType = g_FCMDBUSLNG[tmpFCode].byteLng;
                        //��С��ת��
                        reversEndian ( ( u_char* ) &tUs, sizeof ( u_short ) );

                        valChkStd_p->pntAddr = reqAddr + setNum;
                        valChkStd_p->pntVal1 = tUs;

                        setNum++;
                        tUsrData_p = ssnPar_p->usrData_p + setNum;
                        valChkStd_p = & (
                                      tUsrData_p->gData.val_chkStd );
                    }

                }

            }
            else if ( 5 == tmpFCode //д������Ȧ
                      || 6 == tmpFCode //д�����Ĵ���
                    )
            {
                tUsrData_p->gData.pntType = g_FCMDBUSLNG[tmpFCode].byteLng;
                /*
                    * ��ӦPDU�������������ַ��2�ֽ�)+ֵ��2�ֽڣ�
                    */
                memcpy ( & ( valChkStd_p->pntAddr ),
                         tmpData_p,
                         2
                       );
                memcpy ( & ( valChkStd_p->pntVal1 ),
                         tmpData_p + 2,
                         2
                       );
                setNum++;
                tUsrData_p = ssnPar_p->usrData_p + setNum;
                valChkStd_p = & ( tUsrData_p->gData.val_chkStd );
            }
            else if (   15 == tmpFCode  //д�����Ȧ
                        || 16 == tmpFCode  //д����Ĵ���
                    )
            {
                /*ssnPar_p��ʱ��ֵΪ����վ ��������֡������*/
                u_short reqAddr  = 0;
                u_short reqNum   = 0;
                u_short respAddr = 0;
                u_short respNum  = 0;

                //��ʼ��ַ
                reqAddr = valChkStd_p->pntAddr;
                //��Ȧ���������Ĵ�������
                reqNum = ( u_short ) ssnPar_p->pntNum;

                /*
                    * ��ӦPDU�������������ʼ��ַ��2�ֽ�)+������2�ֽڣ�
                    */
                memcpy ( &respAddr, tmpData_p, 2 );
                reversEndian ( ( u_char* ) &respAddr, sizeof ( u_short ) );
                memcpy ( &respNum, tmpData_p + 2, 2 );
                reversEndian ( ( u_char* ) &respNum, sizeof ( u_short ) );

                if ( reqAddr != respAddr )
                {
                    return ERR_ADDR_VAL;
                }

                if ( ( 15 == tmpFCode && reqNum > respNum * 8 )
                     || ( 16 == tmpFCode && 2 * reqNum != respNum )
                   )
                {
                    return ERR_RESP_NUM;
                }

                tUsrData_p->gData.pntType = g_FCMDBUSLNG[tmpFCode].byteLng;
                valChkStd_p->pntVal1      = reqNum;

                setNum++;
                tUsrData_p  = ssnPar_p->usrData_p + setNum;
                valChkStd_p = & ( tUsrData_p->gData.val_chkStd );

            }
            else if (   20 == tmpFCode //���ļ���¼
                        || 21 == tmpFCode //д�ļ���¼
                    )
            {
                /*�Ӻ�ʵ��*/
            }
            else if ( 22 == tmpFCode ) //����д�Ĵ���
            {
                u_short reqAddr  = 0;
                u_short reqAnd   = 0;
                u_short reqOr    = 0;

                u_short respAddr = 0;
                u_short respAnd  = 0;
                u_short respOr   = 0;

                tUsrData_p->gData.pntType = g_FCMDBUSLNG[tmpFCode].byteLng;
                /*
                    * ��ӦPDU������������ο���ַ��2�ֽڣ�
                        + And_Mask(2�ֽ�) + Or_Mask(2�ֽ�)
                    */
                /*ssnPar_p��ʱ��ֵΪ����վ ��������֡������*/
                reqAddr = ( u_char ) valChkStd_p->pntAddr;
                reqAnd  = ( u_char ) valChkStd_p->pntVal1;
                reqOr   = ( u_char ) valChkStd_p->pntVal2;


                memcpy ( &respAddr, tmpData_p,       2 );
                memcpy ( &respAnd,  tmpData_p + 2,   2 );
                memcpy ( &respOr,   tmpData_p + 2 + 2, 2 );

                reversEndian ( ( u_char* ) &respAddr, sizeof ( u_short ) );
                reversEndian ( ( u_char* ) &respAnd,  sizeof ( u_short ) );
                reversEndian ( ( u_char* ) &respOr,   sizeof ( u_short ) );

                if ( reqAddr != respAddr
                     || reqAnd != respAnd
                     || reqOr != respOr
                   )
                {
                    return ERR_RESP_NUM;
                }

                setNum++;
                tUsrData_p  = ssnPar_p->usrData_p + setNum;
                valChkStd_p = & ( tUsrData_p->gData.val_chkStd );

            }
            else if ( 23 == tmpFCode ) //��д����Ĵ���
            {
                u_short tUshort = 0;
                /*ssnPar_p��ʱ��ֵΪ����վ ��������֡������*/
                u_short reqRAddr  = 0;
                u_short reqRNum   = 0;
                reqRAddr          = ( u_short ) valChkStd_p->pntAddr;
                reqRNum           = ssnPar_p->pntLoadNum;

                /*
                    * ��ӦPDU������������ֽ�����1�ֽڣ�+ ���Ĵ���ֵ(N�ֽ�)
                    */
                u_char respByteNum = 0;
                memcpy ( &respByteNum, tmpData_p, 1 );

                //reversEndian((u_char*)&respAddr,sizeof(u_short));
                if ( reqRNum * 2 != respByteNum )
                {
                    return ERR_RESP_NUM;
                }

                for ( int i = 0; i < reqRNum; i++ )
                {
                    tUsrData_p->gData.pntType = g_FCMDBUSLNG[tmpFCode].byteLng;
                    memcpy ( &tUshort, tmpData_p + 1 + i * 2, 2 );
                    reversEndian ( ( u_char* ) &tUshort, sizeof ( u_short ) );
                    valChkStd_p->pntAddr = reqRAddr + i;
                    valChkStd_p->pntVal1 = tUshort;

                    setNum++;
                    tUsrData_p  = ssnPar_p->usrData_p + setNum;
                    valChkStd_p = & ( tUsrData_p->gData.val_chkStd );
                }
            }
            else if ( 43 == tmpFCode ) //���豸ʶ����
            {
                /*�ݲ�ʵ��*/
            }

            ssnPar_p->pntNum = setNum;
        }

    }//end ����վ��������

    //switch ( funDsc_p->ssnDscNo )
    //{
    //case STN_INIT: //վ��ʼ��;
    //    {
    //        break;
    //    }
    //case QUE_DATA: //modbusĬ�ϻỰ
    //    {
    //
    //        break;
    //    }
    //default:
    //        break;
    //}//end ��Ӧ�ù��ܵĲ�ͬ�ֱ��Ԫ�صĽ������

    ssnPar_p->stnAcsFlag = * ( ( u_short* ) stnAcsFlag );

    return 0;
}




/*******************************************************************************
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

    switch ( actFlag )
    {
        case ACTIVE_TYPE://����
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
                if ( nowFrameNum_resp < allFrameNum_resp )
                {
                    //��Ӧ֡��δ�ﵽ�޶�ֵ
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

            break;
        }

        case PASSIVE_TYPE://����
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
                if ( nowFrameNum_req < allFrameNum_req )
                {
                    //����֡��δ�ﵽ�޶�ֵ
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
                            //��������ִ��  ����Ҫ��������������
                            *nxtStepStat_p = MC_SEND_CNUE_EXEC;
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
                            //����������ִ��  ����Ҫ��������������
                            *nxtStepStat_p = MC_NO_SEND_CNUE_EXEC;
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

            break;
        }

        default:
            break;
    }

    return 0;
}




/*******************************************************************************
 *���ܣ���preAddData_p�������һ��one_p�ڵ�
 * ��1�����preAddData_p���м���������Ч�ڵ����one_p�滻��Щ��Ч�ڵ�
 * ��2�����preAddData_p���м���������Ч�ڵ㣬����preAddData_pĩβ���one_p�ڵ�ֵ
 *
 * ���룺
 *      preAddData_p����Ҫ��ֵ�����ݻ�����
 *      one_p       ����Ҫ��ӵĽڵ�

 *�����
 *      preAddData_p���ӽڵ������ݻ�����
 *���أ�
 *      0  �ɹ�
 *      <0 �ӿ��쳣
 *        -1:Դ�����߼�����
 *        -2:�Ѿ�����������ֵ���ƣ�������add������
 *�޸���ʷ
 *
*******************************************************************************/
int addSearData ( void* preAddData_p,
                  void* one_p
                )
{
    /*T_mbsChkData�����ݽṹ˵�������one_node_p[]
     *max_index: ���������±�ֵ����ֵ
     *nex_index: �������ʱ����ֱ��ʹ�õ�����ֵ
     *cur_index: ��ǰ������������������ֵ����С��
                 cur_index�������п��ܴ�����Ч������
     */

    u_char tFlag               = 0;
    T_mbsChkData* tmpSData_p  = ( T_mbsChkData* ) preAddData_p;
    T_mbsChkNode*   tmpOne_p   = ( T_mbsChkNode* ) one_p;

    if ( tmpSData_p->nex_index > ( tmpSData_p->cur_index + 1 ) )
    {
        return -1; //Դ�����߼�����
    }

    if (   tmpSData_p->nex_index > tmpSData_p->max_index
           || tmpSData_p->cur_index > tmpSData_p->max_index
       )
    {
        return -2; //�Ѿ�����������ֵ���ƣ�������add������
    }

    if ( ( tmpSData_p->cur_index + 1 ) == tmpSData_p->nex_index )
    {
        //��������<=cur_index�����ݶ�����Ч������
        memcpy ( & ( tmpSData_p->one_node_p[tmpSData_p->nex_index] ),
                 tmpOne_p,
                 sizeof ( T_mbsChkNode )
               );
        tmpSData_p->cur_index++;
        tmpSData_p->nex_index++;
    }
    //nex_index<=cur_index : <=cur_index ��Ӧ��������Ч���ݣ����԰�
    //one_p�����ݷŴ˴�
    else
    {
        memcpy ( & ( tmpSData_p->one_node_p[tmpSData_p->nex_index] ),
                 tmpOne_p,
                 sizeof ( T_mbsChkNode )
               );

        //�� nex_index ָ����һ����ӽڵ�Ŀ���������
        T_mbsChkNode tmpNode = {};

        //memset(&tmpNode,0,sizeof(T_mbsChkNode));
        for ( int i = 0; i <= tmpSData_p->cur_index; i++ )
        {
            if ( 0 == memcmp ( & ( tmpSData_p->one_node_p[i] ),
                               &tmpNode,
                               sizeof ( T_mbsChkNode )
                             )
               )
            {
                tmpSData_p->nex_index = i;
                tFlag = 1;
                break;
            }
        }

        if ( tFlag != 1 )
        {
            tmpSData_p->nex_index = tmpSData_p->cur_index + 1;
        }

    }

    return 0;
}




/*******************************************************************************
 *���ܣ���preAddData_p��ɾ��one_p�ڵ�
 *         ��1����preAddData_p�в���one_p��Ӧ�Ľڵ�
 *         ��2���ɹ��ҵ�ƥ��ڵ��ֻ��ӦpreAddData_p�ж�Ӧ�ڵ�ֵΪ0(��Ч��״̬
 *
 * ���룺
 *      preAddData_p����Ҫ���ҵ����ݻ�����
 *      one_p       ����Ҫɾ���Ľڵ�

 *�����
 *      preAddData_p��ɾ���ڵ������ݻ�����
 *���أ�
 *      0  �ɹ�
 *      <0 �ӿ��쳣
 *        -1:Դ�����߼�����
 *        -2:Դ�����߼�����
 *        -3:preAddData_p ��û������
 *        -4:preAddData_p��û��ƥ�䵽����
 *�޸���ʷ
 *
*******************************************************************************/
int delSearData ( void* preAddData_p,
                  void* one_p
                )
{
    /*T_mbsChkData�����ݽṹ˵�������one_node_p[]
     *max_index: ���������±�ֵ����ֵ
     *nex_index: �������ʱ����ֱ��ʹ�õ�����ֵ
     *cur_index: ��ǰ������������������ֵ����С��
                 cur_index�������п��ܴ�����Ч������
     */

    u_char tFlag               = 0;
    T_mbsChkData* tmpSData_p  = ( T_mbsChkData* ) preAddData_p;
    T_mbsChkNode*   tmpOne_p   = ( T_mbsChkNode* ) one_p;

    if ( ( tmpSData_p->cur_index + 1 ) < tmpSData_p->nex_index )
    {
        return -1; //Դ�����߼�����
    }

    if (   tmpSData_p->nex_index > tmpSData_p->max_index + 1
           || tmpSData_p->cur_index > tmpSData_p->max_index
       )
    {
        return -2; //Դ�����߼�����
    }

    if ( tmpSData_p->cur_index == tmpSData_p->nex_index )
    {
        return -3; //preAddData_p ��û������
    }

    for ( int i = 0; i <= tmpSData_p->cur_index; i++ )
    {
        if (   tmpSData_p->one_node_p[i].trans_id == tmpOne_p->trans_id
               && tmpSData_p->one_node_p[i].func_id == tmpOne_p->func_id
           )
        {
            memset ( & ( tmpSData_p->one_node_p[i] ),
                     0,
                     sizeof ( T_mbsChkNode )
                   );
            //��һ�����õĽڵ�����ָ��ǰ����Ϊ��Ч�Ľڵ�
            tmpSData_p->nex_index = i;
            tFlag = 1;//ƥ��ɹ�
            break;
        }
    }

    if ( tFlag != 1 )
    {
        return -4; //preAddData_p��û��ƥ�䵽����
    }

    return 0;
}




/*******************************************************************************
*���ܣ���data_p�е����ݰ��ֽڽ��д�С�˷�ת��
*      (������ĺ�REVERS_FLAGΪ0ʱת��������ת����
*    1������Ǵ�˾����˺�����ΪС��
*    2�������С�˾����˺�����Ϊ���
*
*���룺data_p ����Ҫ���з�ת������
*      dataLng�����ݴ�С
*
*�����data_p ��ת�������
*
*����
*    0  �ɹ�
*    <0 ����
*
*******************************************************************************/
int reversEndian ( u_char* data_p,
                   int dataLng
                 )
{
    if ( REVERS_FLAG == 0 )
    {
        u_char tCh = 0;

        for ( int i = 0,        j = dataLng - 1 ;
              i < dataLng / 2 && j >= dataLng / 2 ;
              i++,            j--
            )
        {
            tCh = data_p[j];
            data_p[j] = data_p[i];
            data_p[i] = tCh;
            /*for(int j=dataLng-1;j>=dataLng/2;j--)
            {
                tCh = data_p[j];
                data_p[j]=data_p[i];
                data_p[i]= tch;
            }*/
        }
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
    fun_p->cmLvl[usingMode]    = APP_LVL; // MODBUS��ԼȡAPP_LVL
    fun_p->stepNum[usingMode]  = stepNum;

    //2������������
    ONE_STEP* tmpHead_p = NULL;
    T_ssnStepCfg* tStepCfg_p = NULL;

    for ( u_char i = 0; i < stepNum; i++ )
    {
        tStepCfg_p = ( snnCfg_p->stepCfg_p + i );

        ONE_STEP* one_step_p  = new ONE_STEP();
        vrfItem* tsVerifyBlock1_p = new vrfItem();
        //tsVerifyBlock1_p->chkRslt_p = new int();

        setONE_STEP ( one_step_p,
                      i + 1,
                      PDU_FCode,
                      tStepCfg_p->reqAPDUNum,
                      NULL,
                      PDU_FCode,
                      tStepCfg_p->respAPDUNum,
                      NULL,
                      NULL/*tsVerifyBlock1_p*/
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
  *���ܣ�
  *    ����ELE_TYPE.lngֵ��Ϊ��Ϊ�����ĳ��Ȱ�ELE_TYPE.p_Mem�����ݿ�����dst_p
  *
  * ���룺
  *     dst_p      ����Ҫ��������Ŀ���ַ
  *     eleType_p  : ����Դ�ṹ
  *�����
  *     dst_p      ����Ҫ��������Ŀ���ַ
  *���أ�
  *     void
  *�޸���ʷ
  *
*******************************************************************************/
void*& memcpyEleType ( void* dst_p,
                       const ELE_TYPE* eleType_p
                     )
{
    if (  dst_p == NULL
          || eleType_p == NULL
          || eleType_p->p_Mem == NULL
          || eleType_p->lng == 0
       )
    {
        return dst_p;
    }

    memcpy ( dst_p,
             eleType_p->p_Mem,
             eleType_p->lng
           );

    return dst_p;
}


