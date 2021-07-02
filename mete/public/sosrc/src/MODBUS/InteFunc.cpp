
#include "InteFunc.h"

/**************************************内部使用函数非接口***********************/

const T_MDPNTLNG  g_FCMDBUSLNG[MC_MAXFC_NUMS] =
{
    /*function code:01 */ {1, 1}, /*读线圈*/
    /*function code:02 */ {1, 1}, /*读离散输入量*/
    /*function code:03 */ {2, 16}, /*读保持寄存器*/
    /*function code:04 */ {2, 16}, /*读输入寄存器*/
    /*function code:05 */ {2, 16}, /*写单个线圈*/
    /*function code:06 */ {2, 16}, /*写单个寄存器*/
    /*function code:07 */ {0, 0},
    /*function code:08 */ {0, 0},
    /*function code:09 */ {0, 0},
    /*function code:10 */ {0, 0},
    /*function code:11 */ {0, 0},
    /*function code:12 */ {0, 0},
    /*function code:13 */ {0, 0},
    /*function code:14 */ {0, 0},
    /*function code:15 */ {0, 0}, /*写多个线圈*/
    /*function code:16 */ {0, 0}, /*写多个寄存器*/
    /*function code:17 */ {0, 0},
    /*function code:18 */ {0, 0},
    /*function code:19 */ {0, 0},
    /*function code:20 */ {0, 0}, /*读文件记录:暂时不做*/
    /*function code:21 */ {0, 0}, /*写文件记录:暂时不做*/
    /*function code:22 */ {8, 64}, /*屏蔽写寄存器*/
    /*function code:23 */ {2, 16}, /*读写多个寄存器*/
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
    /*function code:43 */ {0, 0} /*读设备识别码:暂时不做*/

};



/*******************************************************************************
  *功能：
  *     组装ONE_STEP中除nextStep_p之外的值
  * 输入：
  *      step_p         ：ONE_STEP空间指针
  *      stepNo         : 要组装的值 步号
  *      reqPDU_FCode   : 要组装的值 请求帧使用的帧格式编码
  *      reqPDU_Num     : 要组装的值 请求帧（PDU）个数
  *      reqAPDU_Obj_p  : 要组装的值 请求帧APDU对象指针表
  *      respPDU_FCode  : 要组装的值 响应帧使用的帧格式编码
  *      respPDU_Num    : 要组装的值 响应PDU个数
  *      respAPDU_Obj_p : 要组装的值 响应帧APDU对象指针表
  *      sVerifyBlock_p : 要组装的值 专用步验证块
  *输出：
  *      step_p         : 赋值后的ONE_STEP指针
  *返回：
  *      void
  *修改历史
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
  *功能：查找startStep_p中步号为targetStep_p->stepNo的步
  *输入：
  *      startStep_p ：规则步
  *      targetStep_p：要查找的步
  *输出：无
  *返回：
  *      ONE_STEP*
  *              1、异常情况返回 NULL
  *              2、正常情况返回startStep_p中步号为targetStep_p->stepNo的步;
  *修改:
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
  *功能：在链表ONE_STEP末尾插入一个节点
  * 输入：
  *      one_step_p ：链表头指针
  *      tail_step_p: 要添加的节点指针
  *输出：
  *      one_step_p ：添加节点指针之后的链表头指针
  *返回：void
  *修改：
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
 *功能：根据eleVal_p的值给snnCfg_p赋值（snnCfg_p.p_Mem在接口中重新new空间）
 * 输入：
 *      actChnlID     ：主被动标识
 *      stepNum       ：给第stepNum步赋值
 *      frmNum        ：给第frmNum帧赋值
 *      eleVal_p      ：ELE_TYPE [11]类型变量，取值参数
 *      T_snnCfg      ：将要赋值的结构

 *输出：
 *      T_snnCfg      ：赋值后的结构
 *返回：
 *      0  成功
 *      <0 接口异常
 *修改历史
 *    20180411145002因T_snnCfg结构体变化，此内部接口无实际用途，所以注释内部逻辑
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
  *功能：
  *   给步的下一帧赋值
  *
  * 输入：
  *     nxtFrmRlStep_p：下一帧对应的规则步
  *     nxtFrmStep_p  ：当前步
  *     nxtFrmStepNo_p: 下一帧的步号
  *     funDsc_p      ：会话号与主动性
  *     ssnPar_p      ：详见此类型说明
  *输出：
  *     nxtFrmStep_p  ：下一帧对应的步
  *返回：
  *     0    成功
  *     负值 接口异常
  *修改历史
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

    //对参数进行校验
    if (   actFlag != ACTIVE_TYPE
           && actFlag != PASSIVE_TYPE
       )
    {
        return ERR_ACT_PAS_VAL;
    }

    C_ModbusADU* stepObj_p = NULL;

    //取需要装帧的apdu对象
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

    //取Function code 及 Unit Identifier
    PDU_FC_VAL* tmpFC_p = NULL;
    u_char tmpFCode     = 0;
    ADU_MBAP_VAL tMbapVal;//MBAP报文头 7字节
    T_mbsID       tMbsId;
    memset ( &tMbapVal, 0, sizeof ( tMbapVal ) );
    memset ( &tMbsId, 0, sizeof ( tMbsId ) );

    //下一帧是重新开始的一步
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

    ////取高8位为“单元标识符”
    //unitID = (tmpFCode>>(sizeof(u_int)-1)*8);
    ////低3字节记录的是帧数据中的“功能码”
    //tmpFCode = tmpFCode - (unitID<<(sizeof(u_int)-1)*8 );
    //u_char tmpFCode = *((u_char*)tmpFC_p);

    u_char tmpData[MAX_ARRAY_INDEX];
    u_char* tmpData_p        = tmpData;
    u_char tmpDataLng        = 0;
    u_short setNum           = 0;
    T_snnUsrData* tUsrData_p = & ( ssnPar_p->usrData_p[setNum] );
    memset ( tmpData, 0, sizeof ( tmpData ) );

    if ( ACTIVE_TYPE == actFlag ) //主站
    {
        if (    tmpFCode == MC_MBFC_0x00     /*新联一个通道*/
                || tmpFCode == MC_MBFC_0xFF     /*关一个通道*/
           )
        {
            /*没有数据域*/
            tmpDataLng = 0;
        }
        else if ( ( tmpFCode >= 1 ) && ( tmpFCode <= 6 ) )
        {
            /*
             * PDU数据数据域里：2字节起始地址+2字节读或写的数量或值
             */
            u_short reqNum  = 0;
            u_short reqAddr = 0;

            reqAddr = tUsrData_p->gData.val_chkStd.pntAddr;
            reqNum  = tUsrData_p->gData.val_chkStd.pntVal1;
            setNum++;
            tUsrData_p = & ( ssnPar_p->usrData_p[setNum] );

            //大小端转换
            reversEndian ( ( u_char* ) &reqAddr, sizeof ( u_short ) );
            reversEndian ( ( u_char* ) &reqNum, sizeof ( u_short ) );

            memcpy ( tmpData_p, &reqAddr, 2 );
            memcpy ( tmpData_p + 2, &reqNum, 2 );
            tmpDataLng = 4;

        }
        else if (   15 == tmpFCode  //写多个线圈
                    || 16 == tmpFCode  //写多个寄存器
                )
        {
            /*
             * PDU数据数据域里：2字节起始地址+2字节输出数量或寄存器数量
                               +1字节字节数+数出值N字节
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
        else if (  20 == tmpFCode //读文件记录
                   || 21 == tmpFCode //写文件记录
                )
        {
            /*延后实现*/
        }
        else if ( 22 == tmpFCode ) //屏蔽写寄存器
        {
            /*
             * PDU数据数据域里：参考地址（2字节）+ And_Mask(2字节)
                               + Or_Mask(2字节)
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
        else if ( 23 == tmpFCode ) //读写多个寄存器
        {
            /*
             * PDU数据数据域里：读起始地址（2字节）+ 读的数量(2字节)
                               + 写的起始地址(2字节) + 写的数量（2字节）
                               +写字节数（1字节）+ 写寄存器值 （N个字节）
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
        else if ( 43 == tmpFCode ) //读设备识别码
        {
            /*暂不实现*/
        }
    }
    else////////////////////////////被动站或从站 响应数据////////////////////////
    {
        u_short offSetNum = 0;

        if (   tmpFCode == MC_MBFC_0x00  /*新联一个通道*/
               || tmpFCode == MC_MBFC_0xFF /*关一个通道*/
           )
        {
            /*没有数据域*/
            tmpDataLng = 0;
        }
        else if ( tmpFCode >= 1 && tmpFCode <= 4 )
        {
            /*
             * 响应PDU数据数据域里：字节数（1字节)+状态或值（N字节）
             */
            setNum             = 0;
            u_char respByteNum = 0;
            u_char tData       = 0;
            u_int  tInt        = 0;

            if ( tmpFCode == 1
                 || tmpFCode == 2
               )
            {
                /*N为线圈输出数量则：字节数＝N/8,如果余数不等于0，
                                    那么 字节数＝N/8 + 1
                 */
                if ( ssnPar_p->pntNum % 8 != 0 )
                {
                    respByteNum = ssnPar_p->pntNum / 8 + 1;
                }
                else
                {
                    respByteNum = ssnPar_p->pntNum / 8;
                }

                //将字节数放入pdu的第一个字节
                memcpy ( tmpData_p, &respByteNum, 1 );

                //将每一个线圈的状合成值
                for ( int i = 0; i < ssnPar_p->pntNum; i++ )
                {
                    tInt  = tUsrData_p->gData.val_chkStd.pntVal1;
                    tData = tData + ( ( ( u_char ) tInt ) << ( i % 8 ) );

                    if ( ( i + 1 ) % 8 == 0 )
                    {
                        memcpy ( tmpData_p + 1 + ( i + 1 ) / 8, &tData, 1 );
                        tData = 0;
                    }
                    //不够8位的则也复制一个字节的值
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
                //N为寄存器数量则 字节数＝2*N
                respByteNum = ssnPar_p->pntNum * 2;
                //将字节数放入pdu的第一个字节
                memcpy ( tmpData_p, &respByteNum, 1 );

                u_short tRgtVal = 0;

                //将寄存器的值输出
                for ( int i = 0; i < ssnPar_p->pntNum; i++ )
                {
                    tRgtVal = ( u_short ) tUsrData_p->gData.val_chkStd.pntVal1;
                    //x86为小端存储，将值转换成mobbus的大端
                    reversEndian ( ( u_char* ) &tRgtVal, sizeof ( tRgtVal ) );
                    memcpy ( tmpData_p + 1 + ( i * 2 ), &tRgtVal, 2 );
                    setNum++;
                    tUsrData_p = & ( ssnPar_p->usrData_p[setNum] );
                }

                tmpDataLng = respByteNum + 1;
            }
        }
        else if ( 5 == tmpFCode  //写单个线圈
                  || 6 == tmpFCode //写单个寄存器
                )
        {
            /*
             * 响应PDU数据数据域里：地址（2字节)+值（2字节）
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
        else if (   15 == tmpFCode  //写多个线圈
                    || 16 == tmpFCode  //写多个寄存器
                )
        {
            /*
             * 响应PDU数据数据域里：起始地址（2字节)+数量（2字节）
             */
            u_short uSh = 0;
            uSh = tUsrData_p->gData.val_chkStd.pntAddr;
            reversEndian ( ( u_char* ) &uSh, sizeof ( u_short ) );
            memcpy ( tmpData_p, &uSh, 2 );

            if ( 15 == tmpFCode ) //获得字节数
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
        else if (   20 == tmpFCode  //读文件记录
                    || 21 == tmpFCode  //写文件记录
                )
        {
            /*延后实现*/
        }
        else if ( 22 == tmpFCode ) //屏蔽写寄存器
        {
            /*
             * 响应PDU数据数据域里：参考地址（2字节）+ And_Mask(2字节)
                                   + Or_Mask(2字节)
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
        else if ( 23 == tmpFCode ) //读写多个寄存器
        {
            u_short tUshort = 0;
            /*
             * 响应PDU数据数据域里：字节数（1字节）+ 读寄存器值(N字节)
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
        else if ( 43 == tmpFCode ) //读设备识别码
        {
            /*暂不实现*/
        }
    }



    //更新对象中数域控制结构的长度为实际值
    stepObj_p->getDU_Info()->InfoEleLng = 1;
    stepObj_p->getDU_Info()->ADU_CMTBL[PDU_DATA_INDEX] = tmpDataLng;
    //更新MBAP报文头的长度域值
    tMbapVal.length = tmpDataLng + 2;
    //拼装报文头到某一步的对象中
    stepObj_p->setADU_MBAPH_VAL ( &tMbapVal );
    //将拼装的PDU数据域与功能码赋于某一步的对象中
    stepObj_p->setPDU_FC_VAL ( tmpFC_p );

    if ( tmpDataLng > 0 )
    {
        stepObj_p->setADU_ELEMENT ( tmpData_p,
                                    tmpDataLng,
                                    PDU_DATA_INDEX
                                  );
    }

    //给出参下一帧对应步赋值
    nxtFrmStep_p->stepNo = nxtFrmRlStep_p->stepNo;
    //nxtFrmStep_p->sVerifyBlock_p = NULL;

    if ( actFlag == ACTIVE_TYPE ) //主动
    {
        nxtFrmStep_p->reqPDU_FCode = nxtFrmRlStep_p->reqPDU_FCode;

    }
    else // (actFlag == PASSIVE_TYPE) //被动
    {
        nxtFrmStep_p->respPDU_FCode = nxtFrmRlStep_p->respPDU_FCode;
    }

    return 0;
}




/*******************************************************************************
  *功能：modbus接收侧校验功能
  *
  * 输入：
  *     nowRuleStep_p :当前步规则
  *     lastRuleStep_p:规则步的最后一步
  *     stepNo        :当前步号
  *     funDsc_p      :会话号与主动性
  *     mbap_p        :需要验证数据的对应C_MBAP对象
  *输出：
  *     无
  *返回：
  *     0    成功
  *     <0   错误
  *修改历史
  *
*******************************************************************************/
int chkRecData (  ONE_STEP* nowRuleStep_p,
                  ONE_STEP* lastRuleStep_p,
                  const u_char& stepNo,
                  T_ptclFunDsc* funDsc_p,
                  C_ModbusADU* mbap_p
               )
{


    //主被动性会话标识
    u_char act_prio_chnlID   = ( u_char ) funDsc_p->act_prio_chnlID;
    //模式码
    u_char usingMode         = funDsc_p->usingMode;
    //规则步中请求帧数量的限定值
    u_char allFrameNum_req  = nowRuleStep_p->reqPDU_Num;
    //规则步中响应帧数量的限定值
    u_char allFrameNum_resp = nowRuleStep_p->respPDU_Num;

    if ( ! ( ASSERTMBFC ( funDsc_p->ssnDscNo ) ) )
    {
        return ERR_FC_VAL_RAN;
    }

    if ( lastRuleStep_p->stepNo > C_STEP_NO_TWO )
    {
        //最大步号为2
        return ERR_CONFIG;
    }

    //取Function code
    PDU_FC_VAL* tmpFC_p = mbap_p->getPDU_FC_VAL (
                          mbap_p->getDU_Info()->ADU_CMTBL[PDU_FC_INDEX]
                          );
    u_char tmpFCode   = * ( ( u_char* ) tmpFC_p );
    u_char tmpFCode_c = tmpFCode;

    if ( act_prio_chnlID == ACTIVE_TYPE
         && tmpFCode > 0x80 //响应的异常码
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
        //被控站会话验收主控站ASDU
        //if (allFrameNum_req != 1)
        //{
        //    return ERR_FRAME_NO;
        //}
        //if( stepNo != 1 )//主动站的数据一定是在第一步
        //{
        //    return ERR_STEP_NO;
        //}

    }
    else if ( act_prio_chnlID == ACTIVE_TYPE )
    {
        //主站接收的是被动站的数据
        /*if (allFrameNum_resp != 1)
        {
        return ERR_FRAME_NO;
        }*/

    }


    //   switch( funDsc_p->ssnDscNo )
    //{
    //   case STN_INIT: //站初始化;
    //       {
    //           break;
    //       }
    //   case QUE_DATA: //modbus默认会话
    //       {
    //           if (lastRuleStep_p->stepNo > C_STEP_NO_TWO)
    //           {//最大步号为2
    //               return ERR_CONFIG;
    //           }

    //           //取Function code
    //           PDU_FC_VAL *tmpFC_p = mbap_p->getPDU_FC_VAL(
    //               mbap_p->getDU_Info()->ADU_CMTBL[PDU_FC_INDEX]
    //           );
    //           u_char tmpFCode   = *((u_char*)tmpFC_p);
    //           u_char tmpFCode_c = tmpFCode;

    //           if (act_prio_chnlID == ACTIVE_TYPE
    //               && tmpFCode >0x80 //响应的异常码
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
    //           {//被控站会话验收主控站ASDU
    //               //if (allFrameNum_req != 1)
    //               //{
    //               //    return ERR_FRAME_NO;
    //               //}
    //               //if( stepNo != 1 )//主动站的数据一定是在第一步
    //               //{
    //               //    return ERR_STEP_NO;
    //               //}
    //
    //           }
    //           else if (act_prio_chnlID == ACTIVE_TYPE)
    //           {//主站接收的是被动站的数据
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
 *功能：接收侧获得下一帧步号,存储、加载标识
 * 输入：
 *      Fun_p        ：步规则
 *      stepRl_p     ：当前帧对应的步
 *      funDsc_p     ：会话号与主动性
 *输出：
 *      stnAcsFlag_p ：存储、加载标识
 *      nxtStepNo_p  ：下一帧步号
 *返回：
 *      0  成功
 *      <0 接口异常
 *修改历史
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

    //获取规则步中当前步的指针;
    ONE_STEP* nowRuleStep_p = displayNextNode (
                              Fun_p->steps_p[Fun_p->usingMode],
                              stepRl_p );

    if ( nowRuleStep_p == NULL )
    {
        return ERR_POINTER_NULL;
    }

    //当前步指针
    ONE_STEP* nowStep_p     = stepRl_p;

    //找到规则步最后一步
    /*ONE_STEP* lastRuleStep_p = Fun_p->steps_p[Fun_p->usingMode];
    while(lastRuleStep_p->nextStep_p != NULL)
    {
        lastRuleStep_p    = lastRuleStep_p->nextStep_p;
    }*/
    //当前步的步号比规则步的大则异常
    if ( nowStep_p->stepNo > nowRuleStep_p->stepNo )
    {
        return ERR_STEP_NO;
    }

    //当前规则步的下一步
    ONE_STEP* nxtRuleStep_p = nowRuleStep_p->nextStep_p;

    //规则步中请求帧数量的限定值
    u_char allFrameNum_req  = nowRuleStep_p->reqPDU_Num;
    //当前请求帧值（是步的第几帧）
    u_char nowFrameNum_req  = nowStep_p->reqPDU_Num;
    //规则步中响应帧数量的限定值
    u_char allFrameNum_resp = nowRuleStep_p->respPDU_Num;
    //当前响应帧值（是步的第几帧）
    u_char nowFrameNum_resp = nowStep_p->respPDU_Num;

    T_stnAcsFlagM tmpStnAcsFlag = {0, 0, 1, 0};
    *stnAcsFlag_p = tmpStnAcsFlag;

    *nxtStepNo_p  = nowStep_p->stepNo;

    //主被动会话标识
    u_char actFlag = ( u_char ) funDsc_p->act_prio_chnlID;

    switch ( actFlag )
    {
        case PASSIVE_TYPE://被动会话接收主站数据
        {
            if ( nowFrameNum_req == allFrameNum_req ) //请求帧达到限定值
            {
                if ( nowFrameNum_resp == allFrameNum_resp ) //响应帧也达到限定值
                {
                    if ( nxtRuleStep_p != NULL ) //如果当前步不是最后一步
                    {
                        *nxtStepNo_p = nxtRuleStep_p->stepNo;
                        stnAcsFlag_p->ctlType = 2; //下一步系统执行数据值的加载
                    }
                }
                else if ( nowFrameNum_resp > allFrameNum_resp ) //响应帧错误
                {
                    return ERR_FRAME_NO;
                }
            }
            else if ( nowFrameNum_req > allFrameNum_req ) //请求帧错误
            {
                return ERR_FRAME_NO;
            }

            break;
        }

        case ACTIVE_TYPE://主动会话接收被动站数据
        {
            if ( nowFrameNum_resp == allFrameNum_resp ) //响应帧达到限定值
            {
                if ( nowFrameNum_req == allFrameNum_req ) //请求帧也达到限定值
                {
                    if ( nxtRuleStep_p != NULL ) //如果当前步不是最后一步
                    {
                        *nxtStepNo_p = nxtRuleStep_p->stepNo;
                        stnAcsFlag_p->ctlType = 2; //下一步系统执行数据值的加载
                    }
                }
                else if ( nowFrameNum_req > allFrameNum_req ) //请求帧错误
                {
                    return ERR_FRAME_NO;
                }
            }
            else if ( nowFrameNum_resp > allFrameNum_resp ) //响应帧错误
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
  *功能：
  *    会话步中帧数据验收后给T_snnPara赋值
  * 输入：
  *     stepRl_p  ：验收后的当前步
  *     funDsc_p  ：会话号与主动性
  *     mbap_p    ：C_MBAP对象指针
  *     stnAcsFlag：存储、读取标识
  *输出：
  *     ssnPar_p：  详见此类型说明
  *返回：
  *     0 成功
  *    <0 失败
  *修改历史
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

    /****************************从C_MBAP对象中取值**************************/
    ADU_MBAP_VAL* tMbapVal_p = NULL; //MBAP报文头 7字节
    T_mbsID       tMbsId;
    memset ( &tMbsId, 0, sizeof ( tMbsId ) );

    tMbapVal_p = mbap_p->getADU_MBAPH_VAL (
                 mbap_p->getDU_Info()->ADU_CMTBL[ADU_MBAPH_INDEX]
                 );
    //取Function code
    PDU_FC_VAL* tmpFC_p = mbap_p->getPDU_FC_VAL (
                          mbap_p->getDU_Info()->ADU_CMTBL[PDU_FC_INDEX]
                          );
    u_char tmpFCode = * ( ( u_char* ) tmpFC_p );

    tMbsId.trans_id = tMbapVal_p->trans_id;
    tMbsId.unit_id  = tMbapVal_p->unit_id;
    tMbsId.func_id  = tmpFCode;

    //取PDU数据域指针
    u_short tDataLng = 0;
    u_char* tmpData_p = mbap_p->getADU_ELEMENT (
                        tDataLng,
                        PDU_DATA_INDEX
                        );

    /*******************************赋值T_snnPara**************************/
    ssnPar_p->stepNo      = stepRl_p->stepNo;
    ssnPar_p->infoType    = GINFOTYPE;
    ssnPar_p->ASDUID_p[0] = * ( ( u_int* ) &tMbsId );

    u_char tActFlag = ( u_char ) funDsc_p->act_prio_chnlID;

    /*单向总帧数 （此值不在某一帧中修改）
    ssnPar_p->ASDUIDNum = 1;
    */
    if ( tActFlag == PASSIVE_TYPE )
    {
        //被动站验收主动站的数据
        if ( 0 == stepRl_p->reqPDU_Num )
        {
            return ERR_FRAME_NO;
        }

        ssnPar_p->actFlag = ACTIVE_TYPE;
    }
    else if ( tActFlag == ACTIVE_TYPE )
    {
        //主动站验收被动站的数据;
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
        //会话号（功能码）错误
        return ERR_FC_VAL_RAN;
    }

    //按应用功能的不同分别对元素的解包处理
    //ssnPar_p->pntPos = 0;
    if ( tActFlag == PASSIVE_TYPE )
    {
        stnAcsFlag->ctlType = 2;

        if (   tmpFCode == MC_MBFC_0x00 /*新联一个通道*/
               || tmpFCode == MC_MBFC_0xFF /*关一个通道*/
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
                * PDU数据数据域里：2字节起始地址+2字节读或写的数量或值
                */
            u_short reqNum  = 0;
            u_short reqAddr = 0;
            memcpy ( &reqAddr, tmpData_p, 2 );
            memcpy ( &reqNum, tmpData_p + 2, 2 );

            //modbus是大端，x86是小端需要转换
            reversEndian ( ( u_char* ) &reqAddr, sizeof ( u_short ) );
            reversEndian ( ( u_char* ) &reqNum, sizeof ( u_short ) );

            //把请求的地址解析到ssnPar_p中的pntAddr 中去
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
        else if ( 15 == tmpFCode     //写多个线圈
                  || 16 == tmpFCode  //写多个寄存器
                )
        {
            stnAcsFlag->ctlType = 1;
            /*
                * PDU数据数据域里：2字节起始地址+2字节输出数量或寄存器数量
                                +1字节字节数+数出值N字节
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
        else if ( 20 == tmpFCode  //读文件记录
                  || 21 == tmpFCode //写文件记录
                )
        {
            /*延后实现*/
        }
        else if ( 22 == tmpFCode ) //屏蔽写寄存器
        {
            stnAcsFlag->ctlType = 1;
            /*
                * PDU数据数据域里：参考地址（2字节）+ And_Mask(2字节)
                                + Or_Mask(2字节)
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
        else if ( 23 == tmpFCode ) //读写多个寄存器
        {
            ssnPar_p->pntStorNum = 0;
            ssnPar_p->pntLoadNum = 0;
            u_short tUshort = 0;
            /*
                * PDU数据数据域里：读起始地址（2字节）+ 读的数量(2字节)
                                + 写的起始地址(2字节) + 写的数量（2字节）
                                +写字节数（1字节）+ 写寄存器值 （N个字节）
                */

            u_short reqRAddr  = 0;
            u_short reqRNum   = 0;
            u_short reqWAddr  = 0;
            u_short reqWNum   = 0;
            u_char  reqWByte  = 0;
            u_char* reqWVal_p = NULL;

            u_char offsetNum = 0;//偏移量

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

            //大小端转换
            reversEndian ( ( u_char* ) &reqRAddr, sizeof ( u_short ) );
            reversEndian ( ( u_char* ) &reqRNum, sizeof ( u_short ) );
            reversEndian ( ( u_char* ) &reqWAddr, sizeof ( u_short ) );
            reversEndian ( ( u_char* ) &reqWNum, sizeof ( u_short ) );


            for ( int i = 0; i < reqRNum; i++ ) //解析读的数据到ssnPar_p结构中去
            {
                tUsrData_p->gData.pntType = g_FCMDBUSLNG[tmpFCode].byteLng;
                valChkStd_p->pntAddr      = reqRAddr + i;

                setNum++;
                tUsrData_p  = ssnPar_p->usrData_p + setNum;
                valChkStd_p = & ( tUsrData_p->gData.val_chkStd );

                ssnPar_p->pntLoadNum++;
            }

            ssnPar_p->pntPos = setNum;//写的开始下标位置

            //解析写的数据到ssnPar_p结构中去
            for ( int j = 0; j < reqWNum; j++ )
            {
                tUsrData_p->gData.pntType = g_FCMDBUSLNG[tmpFCode].byteLng;
                valChkStd_p->pntAddr      = reqWAddr + j;

                memcpy ( &tUshort, reqWVal_p + j * 2, 2 );
                //大小端转换
                reversEndian ( ( u_char* ) &tUshort, sizeof ( u_short ) );
                valChkStd_p->pntVal1 = tUshort;

                setNum++;
                tUsrData_p  = ssnPar_p->usrData_p + setNum;
                valChkStd_p = & ( tUsrData_p->gData.val_chkStd );
                ssnPar_p->pntStorNum++;
            }

            stnAcsFlag->ctlType = 1;

        }
        else if ( 43 == tmpFCode ) //读设备识别码
        {
            /*暂不实现*/
        }

        ssnPar_p->pntNum = setNum;
    }//end 被动站解析数据
    else if ( tActFlag == ACTIVE_TYPE ) //主动站
    {
        if ( tmpFCode > 0x80 ) //差错码
        {
            tUsrData_p->gData.pntType = g_FCMDBUSLNG[tmpFCode].byteLng;
            //异常响应PDU数据：差错码（1字节）+ 异常码（1字节）
            //valChkStd_p->pntVal1;
            memcpy ( & ( valChkStd_p->pntVal1 ),
                     tmpData_p,
                     1
                   );
            setNum++;
            tUsrData_p  = ssnPar_p->usrData_p + setNum;
            valChkStd_p = & ( tUsrData_p->gData.val_chkStd );
        }
        else//正常响应的数据
        {
            stnAcsFlag->ctlType = 3;
            stnAcsFlag->pad     = 4095;
            u_short offSetNum   = 0;

            if (   tmpFCode == MC_MBFC_0x00    /*新联一个通道*/
                   || tmpFCode == MC_MBFC_0xFF    /*关一个通道*/
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
                    * 响应PDU数据数据域里：字节数（1字节)+状态或值（N字节）
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
                    /*ssnPar_p此时的值为主动站 发送申请帧的数据*/

                    //起始地址
                    reqAddr = valChkStd_p->pntAddr;
                    //线圈数量
                    reqNum = valChkStd_p->pntVal1;

                    //对被动站响应的字节数进行校验
                    if ( reqNum > respByteNum * 8 )
                    {
                        //响应的数量与请求的数量不一至，报错
                        return ERR_RESP_NUM;
                    }

                    //将tmpData_p中的值解析到ssnPar_p结构中去
                    for ( int i = 0; i < reqNum; i++ )
                    {
                        if ( i % 8 == 0 ) //每次取一个字节的线圈状态值
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
                            //第 (i%8 + 1) 位有值
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
                    /*ssnPar_p此时的值为主动站 发送申请帧的数据*/
                    //起始地址
                    reqAddr = valChkStd_p->pntAddr;
                    //寄存器数量
                    reqNum = valChkStd_p->pntVal1;

                    //对被动站响应的字节数进行校验
                    if ( reqNum * 2 != respByteNum )
                    {
                        //响应的数量与请求的数量不一至，报错
                        return ERR_RESP_NUM;
                    }

                    //将tmpData_p中的值解析到ssnPar_p结构中去
                    for ( int i = 0; i < reqNum; i++ )
                    {
                        //一个寄存器的值对应2个字节
                        memcpy ( &tUs, tmpData_p + 1 + i * 2, 2 );

                        tUsrData_p->gData.pntType = g_FCMDBUSLNG[tmpFCode].byteLng;
                        //大小端转换
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
            else if ( 5 == tmpFCode //写单个线圈
                      || 6 == tmpFCode //写单个寄存器
                    )
            {
                tUsrData_p->gData.pntType = g_FCMDBUSLNG[tmpFCode].byteLng;
                /*
                    * 响应PDU数据数据域里：地址（2字节)+值（2字节）
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
            else if (   15 == tmpFCode  //写多个线圈
                        || 16 == tmpFCode  //写多个寄存器
                    )
            {
                /*ssnPar_p此时的值为主动站 发送申请帧的数据*/
                u_short reqAddr  = 0;
                u_short reqNum   = 0;
                u_short respAddr = 0;
                u_short respNum  = 0;

                //起始地址
                reqAddr = valChkStd_p->pntAddr;
                //线圈输出数量或寄存器数量
                reqNum = ( u_short ) ssnPar_p->pntNum;

                /*
                    * 响应PDU数据数据域里：起始地址（2字节)+数量（2字节）
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
            else if (   20 == tmpFCode //读文件记录
                        || 21 == tmpFCode //写文件记录
                    )
            {
                /*延后实现*/
            }
            else if ( 22 == tmpFCode ) //屏蔽写寄存器
            {
                u_short reqAddr  = 0;
                u_short reqAnd   = 0;
                u_short reqOr    = 0;

                u_short respAddr = 0;
                u_short respAnd  = 0;
                u_short respOr   = 0;

                tUsrData_p->gData.pntType = g_FCMDBUSLNG[tmpFCode].byteLng;
                /*
                    * 响应PDU数据数据域里：参考地址（2字节）
                        + And_Mask(2字节) + Or_Mask(2字节)
                    */
                /*ssnPar_p此时的值为主动站 发送申请帧的数据*/
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
            else if ( 23 == tmpFCode ) //读写多个寄存器
            {
                u_short tUshort = 0;
                /*ssnPar_p此时的值为主动站 发送申请帧的数据*/
                u_short reqRAddr  = 0;
                u_short reqRNum   = 0;
                reqRAddr          = ( u_short ) valChkStd_p->pntAddr;
                reqRNum           = ssnPar_p->pntLoadNum;

                /*
                    * 响应PDU数据数据域里：字节数（1字节）+ 读寄存器值(N字节)
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
            else if ( 43 == tmpFCode ) //读设备识别码
            {
                /*暂不实现*/
            }

            ssnPar_p->pntNum = setNum;
        }

    }//end 被动站解析数据

    //switch ( funDsc_p->ssnDscNo )
    //{
    //case STN_INIT: //站初始化;
    //    {
    //        break;
    //    }
    //case QUE_DATA: //modbus默认会话
    //    {
    //
    //        break;
    //    }
    //default:
    //        break;
    //}//end 按应用功能的不同分别对元素的解包处理

    ssnPar_p->stnAcsFlag = * ( ( u_short* ) stnAcsFlag );

    return 0;
}




/*******************************************************************************
 *功能：获得下一帧会话状态,并输出下一帧步号
 * 输入：
 *      Fun_p        ：步规则
 *      stepRl_p     ：当前帧对应的步
 *      funDsc_p     ：会话号与主动性
 *输出：
 *      nxtStepStat_p：下一帧会话状态
 *      nxtStepNo_p  ：下一帧步号
 *返回：
 *      >0 会话状态
 *      <0 接口异常
 *修改历史
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

    //当前步指针
    ONE_STEP* nowStep_p     = stepRl_p;
    //当前步的规则步指针
    ONE_STEP* nowRuleStep_p = displayNextNode (
                              Fun_p->steps_p[Fun_p->usingMode],
                              nowStep_p
                              );

    if ( nowRuleStep_p == NULL )
    {
        return ERR_POINTER_NULL;
    }

    //找到规则步最后一步
    ONE_STEP* lastRuleStep_p = Fun_p->steps_p[Fun_p->usingMode];

    while ( lastRuleStep_p->nextStep_p != NULL )
    {
        lastRuleStep_p    = lastRuleStep_p->nextStep_p;
    }

    //当前规则步的下一步
    ONE_STEP* nxtRuleStep_p = nowRuleStep_p->nextStep_p;
    //主被动性标识
    u_char actFlag = ( u_char ) funDsc_p->act_prio_chnlID;

    //规则步中请求帧数量的限定值
    u_char allFrameNum_req  = nowRuleStep_p->reqPDU_Num;
    //规则步中响应帧数量的限定值
    u_char allFrameNum_resp = nowRuleStep_p->respPDU_Num;
    //当前请求帧值（是步的第几帧）
    u_char nowFrameNum_req  = nowStep_p->reqPDU_Num;
    //当前响应帧值（是步的第几帧）
    u_char nowFrameNum_resp = nowStep_p->respPDU_Num;

    //下一帧数据的步号初始化为：默认当前步的步号
    *nxtStepNo_p = nowStep_p->stepNo;

    switch ( actFlag )
    {
        case ACTIVE_TYPE://主动
        {
            if ( nowFrameNum_req < allFrameNum_req - 1 )
            {
                //请求帧未达到限定值
                //发送连续执行
                *nxtStepStat_p = MC_SEND_CNUE_EXEC;
            }
            else if ( nowFrameNum_req == allFrameNum_req - 1 )
            {
                //将要装的请求帧达到限定值
                if ( nowFrameNum_resp < allFrameNum_resp )
                {
                    //响应帧还未达到限定值
                    //发送等待验收
                    *nxtStepStat_p = MC_SEND_WAIT_CHECK;
                }
                else if ( nowFrameNum_resp == allFrameNum_resp
                          || nowFrameNum_resp == allFrameNum_resp + 1
                        ) //响应帧达到了限定值
                {
                    //通过规则步判断是否为最后一步
                    if ( nowStep_p->stepNo == lastRuleStep_p->stepNo )
                    {
                        //是最后一步
                        //发送会话完成
                        *nxtStepStat_p = MC_SEND_SSN_FNSH;
                    }
                    else//不是最后一步
                    {
                        *nxtStepNo_p = nxtRuleStep_p->stepNo;

                        if ( nxtRuleStep_p->reqPDU_Num > 0 )
                        {
                            //下一步请求帧数大于0
                            //发送连续执行  但需要跳步！！！！！
                            *nxtStepStat_p = MC_SEND_CNUE_EXEC;
                        }
                        else//下一步请求帧等于0，响应帧数大于0
                        {
                            //发送等待验收
                            *nxtStepStat_p = MC_SEND_WAIT_CHECK;
                        }
                    }
                }
                else//响应帧错误
                {
                    //不发送会话中止
                    *nxtStepStat_p = MC_NO_SEND_SSN_STOP;
                }
            }
            else if ( nowFrameNum_req == allFrameNum_req )
            {
                //已经是限定值了不需要发送帧了
                if ( nowFrameNum_resp < allFrameNum_resp )
                {
                    //响应帧还未达到限定值
                    //不发送等待验收
                    *nxtStepStat_p = MC_NO_SEND_WAIT_CHECK;
                }
                else if ( nowFrameNum_resp == allFrameNum_resp
                          || nowFrameNum_resp == allFrameNum_resp + 1
                        ) //响应帧达到了限定值
                {
                    //通过规则步判断是否为最后一步
                    if ( nowStep_p->stepNo == lastRuleStep_p->stepNo )
                    {
                        //是最后一步
                        //不发送会话完成
                        *nxtStepStat_p = MC_NO_SEND_SSN_FNSH;
                    }
                    else//不是最后一步
                    {
                        *nxtStepNo_p = nxtRuleStep_p->stepNo;

                        if ( nxtRuleStep_p->reqPDU_Num > 0 )
                        {
                            //下一步请求帧数大于0
                            //不发送连续执行  但需要跳步！！！！！
                            *nxtStepStat_p = MC_NO_SEND_CNUE_EXEC;
                        }
                        else//下一步请求帧等于0，响应帧数大于0
                        {
                            //不发送等待验收
                            *nxtStepStat_p = MC_NO_SEND_WAIT_CHECK;
                        }
                    }
                }
                else//响应帧错误
                {
                    //不发送会话中止
                    *nxtStepStat_p = MC_NO_SEND_SSN_STOP;
                }
            }
            else//请求帧错误：nowFrameNum_req>allFrameNum_req
            {
                //不发送会话中止
                *nxtStepStat_p = MC_NO_SEND_SSN_STOP;
            }

            break;
        }

        case PASSIVE_TYPE://被动
        {
            if ( nowFrameNum_resp < allFrameNum_resp - 1 )
            {
                //响应帧未达到限定值
                //发送连续执行
                *nxtStepStat_p = MC_SEND_CNUE_EXEC;
            }
            else if ( nowFrameNum_resp == allFrameNum_resp - 1 )
            {
                //将要装的响应帧达到限定值
                if ( nowFrameNum_req < allFrameNum_req )
                {
                    //请求帧还未达到限定值
                    //发送等待验收
                    *nxtStepStat_p = MC_SEND_WAIT_CHECK;
                }
                else if ( nowFrameNum_req == allFrameNum_req
                          || nowFrameNum_req == allFrameNum_req + 1
                        ) //请求帧达到了限定值
                {
                    //通过规则步判断是否为最后一步
                    if ( nowStep_p->stepNo == lastRuleStep_p->stepNo )
                    {
                        //是最后一步
                        //发送会话完成
                        *nxtStepStat_p = MC_SEND_SSN_FNSH;
                    }
                    else//不是最后一步
                    {
                        *nxtStepNo_p = nxtRuleStep_p->stepNo;

                        if ( nxtRuleStep_p->respPDU_Num > 0 )
                        {
                            //下一步响应帧数大于0
                            //发送连续执行  但需要跳步！！！！！
                            *nxtStepStat_p = MC_SEND_CNUE_EXEC;
                        }
                        else//下一步响应帧等于0，请求帧数大于0
                        {
                            //发送等待验收
                            *nxtStepStat_p = MC_SEND_WAIT_CHECK;
                        }
                    }
                }
                else//请求帧错误
                {
                    //不发送会话中止
                    *nxtStepStat_p = MC_NO_SEND_SSN_STOP;
                }
            }
            else if ( nowFrameNum_resp == allFrameNum_resp )
            {
                //已经是限定值了不需要发送帧了
                if ( nowFrameNum_req < allFrameNum_req )
                {
                    //请求帧还未达到限定值
                    //不发送等待验收
                    *nxtStepStat_p = MC_NO_SEND_WAIT_CHECK;
                }
                else if ( nowFrameNum_req == allFrameNum_req
                          || nowFrameNum_req == allFrameNum_req + 1
                        ) //请求帧达到了限定值
                {
                    //通过规则步判断是否为最后一步
                    if ( nowStep_p->stepNo == lastRuleStep_p->stepNo )
                    {
                        //是最后一步
                        //不发送会话完成
                        *nxtStepStat_p = MC_NO_SEND_SSN_FNSH;
                    }
                    else//不是最后一步
                    {
                        *nxtStepNo_p = nxtRuleStep_p->stepNo;

                        if ( nxtRuleStep_p->respPDU_Num > 0 )
                        {
                            //下一步响应帧数大于0
                            //不发送连续执行  但需要跳步！！！！！
                            *nxtStepStat_p = MC_NO_SEND_CNUE_EXEC;
                        }
                        else//下一步响应帧等于0，请求帧数大于0
                        {
                            //不发送等待验收
                            *nxtStepStat_p = MC_NO_SEND_WAIT_CHECK;
                        }
                    }
                }
                else//请求帧错误
                {
                    //不发送会话中止
                    *nxtStepStat_p = MC_NO_SEND_SSN_STOP;
                }
            }
            else//响应帧错误：nowFrameNum_resp>allFrameNum_resp
            {
                //不发送会话中止
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
 *功能：向preAddData_p的中添加一个one_p节点
 * （1）如果preAddData_p的中间区域有无效节点就用one_p替换这些无效节点
 * （2）如果preAddData_p的中间区域都是有效节点，则在preAddData_p末尾添加one_p节点值
 *
 * 输入：
 *      preAddData_p：需要加值的数据缓冲区
 *      one_p       ：将要添加的节点

 *输出：
 *      preAddData_p：加节点后的数据缓冲区
 *返回：
 *      0  成功
 *      <0 接口异常
 *        -1:源数据逻辑错误
 *        -2:已经超过最大添加值限制，不能再add数据了
 *修改历史
 *
*******************************************************************************/
int addSearData ( void* preAddData_p,
                  void* one_p
                )
{
    /*T_mbsChkData中数据结构说明：针对one_node_p[]
     *max_index: 允许的最大下标值索引值
     *nex_index: 添加数据时可以直接使用的索引值
     *cur_index: 当前在用数据中最大的索引值，在小于
                 cur_index的数据中可能存在无效的数据
     */

    u_char tFlag               = 0;
    T_mbsChkData* tmpSData_p  = ( T_mbsChkData* ) preAddData_p;
    T_mbsChkNode*   tmpOne_p   = ( T_mbsChkNode* ) one_p;

    if ( tmpSData_p->nex_index > ( tmpSData_p->cur_index + 1 ) )
    {
        return -1; //源数据逻辑错误
    }

    if (   tmpSData_p->nex_index > tmpSData_p->max_index
           || tmpSData_p->cur_index > tmpSData_p->max_index
       )
    {
        return -2; //已经超过最大添加值限制，不能再add数据了
    }

    if ( ( tmpSData_p->cur_index + 1 ) == tmpSData_p->nex_index )
    {
        //数据区中<=cur_index的数据都是有效的数据
        memcpy ( & ( tmpSData_p->one_node_p[tmpSData_p->nex_index] ),
                 tmpOne_p,
                 sizeof ( T_mbsChkNode )
               );
        tmpSData_p->cur_index++;
        tmpSData_p->nex_index++;
    }
    //nex_index<=cur_index : <=cur_index 对应的区有无效数据，可以把
    //one_p的数据放此处
    else
    {
        memcpy ( & ( tmpSData_p->one_node_p[tmpSData_p->nex_index] ),
                 tmpOne_p,
                 sizeof ( T_mbsChkNode )
               );

        //把 nex_index 指向下一次添加节点的可用索引处
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
 *功能：从preAddData_p中删除one_p节点
 *         （1）在preAddData_p中查找one_p对应的节点
 *         （2）成功找到匹配节点后只是应preAddData_p中对应节点值为0(无效）状态
 *
 * 输入：
 *      preAddData_p：需要查找的数据缓冲区
 *      one_p       ：将要删除的节点

 *输出：
 *      preAddData_p：删除节点后的数据缓冲区
 *返回：
 *      0  成功
 *      <0 接口异常
 *        -1:源数据逻辑错误
 *        -2:源数据逻辑错误
 *        -3:preAddData_p 中没有数据
 *        -4:preAddData_p中没有匹配到数据
 *修改历史
 *
*******************************************************************************/
int delSearData ( void* preAddData_p,
                  void* one_p
                )
{
    /*T_mbsChkData中数据结构说明：针对one_node_p[]
     *max_index: 允许的最大下标值索引值
     *nex_index: 添加数据时可以直接使用的索引值
     *cur_index: 当前在用数据中最大的索引值，在小于
                 cur_index的数据中可能存在无效的数据
     */

    u_char tFlag               = 0;
    T_mbsChkData* tmpSData_p  = ( T_mbsChkData* ) preAddData_p;
    T_mbsChkNode*   tmpOne_p   = ( T_mbsChkNode* ) one_p;

    if ( ( tmpSData_p->cur_index + 1 ) < tmpSData_p->nex_index )
    {
        return -1; //源数据逻辑错误
    }

    if (   tmpSData_p->nex_index > tmpSData_p->max_index + 1
           || tmpSData_p->cur_index > tmpSData_p->max_index
       )
    {
        return -2; //源数据逻辑错误
    }

    if ( tmpSData_p->cur_index == tmpSData_p->nex_index )
    {
        return -3; //preAddData_p 中没有数据
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
            //下一个可用的节点索引指向当前刚置为无效的节点
            tmpSData_p->nex_index = i;
            tFlag = 1;//匹配成功
            break;
        }
    }

    if ( tFlag != 1 )
    {
        return -4; //preAddData_p中没有匹配到数据
    }

    return 0;
}




/*******************************************************************************
*功能：对data_p中的数据按字节进行大小端反转：
*      (当定义的宏REVERS_FLAG为0时转换，否则不转换）
*    1、如果是大端经过此函数变为小端
*    2、如果是小端经过此函数变为大端
*
*输入：data_p ：需要进行反转的数据
*      dataLng：数据大小
*
*输出：data_p 反转后的数据
*
*返回
*    0  成功
*    <0 错误
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
  *功能：
  *    根据入参给FUN_RL结构赋值
  *
  * 输入：
  *     srvNo      ：功能号
  *     usingMode  : 模式码
  *     PDU_FCode  : 帧使用的帧格式编码
  *     stnSnnCfg_p: 会话步、帧配置信息
  *输出：
  *     fun_p     ： 默认步规则
  *返回：
  *     0    成功
  *     <0   错误
  *修改历史
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

    //1、对步规则fun_p赋值
    fun_p->srvNo               = srvNo;
    fun_p->usingMode           = usingMode;
    fun_p->cmLvl[usingMode]    = APP_LVL; // MODBUS规约取APP_LVL
    fun_p->stepNum[usingMode]  = stepNum;

    //2、创建步规则
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

    //3、把步规则挂到Fun_p上
    fun_p->steps_p[usingMode] = tmpHead_p;

    return;
}




/*******************************************************************************
  *功能：
  *    根据ELE_TYPE.lng值作为作为拷贝的长度把ELE_TYPE.p_Mem的内容拷贝到dst_p
  *
  * 输入：
  *     dst_p      ：需要拷贝到的目标地址
  *     eleType_p  : 拷贝源结构
  *输出：
  *     dst_p      ：需要拷贝到的目标地址
  *返回：
  *     void
  *修改历史
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


