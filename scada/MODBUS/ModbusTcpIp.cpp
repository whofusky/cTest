
#define DLL_IMPLEMENT

#include "ModbusTcpIp.h"
#include "InteFunc.h"




/*******************************************************************************
*功能：创建一个ADU对象。
*描述：调用C_ModbusADU类的构造函数声明一个ADU对象。
*输入：
*      cstmInfo_p   ： ADU数据帧的结构描述；
*      uType        ： 构造帧的类型，PDU或ADU；
*      allocFlag    ： 是否分配存储空间
*返回：
*      u_char*      : ADU对象
*
*******************************************************************************/
EXPDLL u_char* createAPDU ( APDU_DSCR* cstmInfo_p,
                            u_char duType,
                            u_char  allocFlag
                          )
{
    /*ADU_DSCR            与   APDU_DSCR 的对应关系
    ADU_DSCR.ADU_TID;    对应  APDU_DSCR.ASDU_TID;
    ADU_DSCR.InfoEleLng; 对应  APDU_DSCR.InfoEleLng;
    ADU_DSCR.ADU_CMTBL[ADU_CMTBL_NUM]; 对应　
                   APDU_DSCR.ASDU_CMTBL [11];但ADU_CMTBL只有如下５个元素
    0   //ADU MBAP报文头
    1   //ADU 地址域
    2   //PDU功能码
    3   //PDU数据
    4   //ADU差错校验
    */
    ADU_DSCR* aduInfo_p = ( ADU_DSCR* ) cstmInfo_p;
    C_ModbusADU* apdu_p = new C_ModbusADU ( aduInfo_p, duType, allocFlag );
    return ( u_char* ) apdu_p;
}




/*******************************************************************************
*功能：释放一个ADU对象的空间。
*描述：调用C_ModbusADU类的析构函数释放一个ADU对象的空间。
*输入：
*      APDU_Obj_p   ： ADU对象；
*返回：
*      空
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
*功能：获取MODBUS的协议号。
*描述：协议号用来判断传输介质是网口还是串口,协议号为奇数时，
       表示使用网口；当协议号为偶数时，表示使用串口。
*输入：
*      空；
*返回：
*      固定值，0x69
*
*******************************************************************************/
EXPDLL int getPtclID()
{
    return P_MODBUS_TCP;
}




/******************************************************************************
*功能：创建步规则。
*描述：创建步规则、设置默认的会话设置。
*输入：
*     srvNo      ：会话号，MODBUS只支持一种会话。
*     usingMode  : 模式码，固定为ONLY_ONE，即0
*输出 :
*      stnSnnCfg_p: 默认会话步、帧配置信息
*      ptclFunDsc : 默认会话步对应的cot,tid配置
*返回：
*      FUN_RL     ： 默认步规则
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
        //不在定义的会话（功能码）范围内
        return NULL;
    }

    //步规则变量
    FUN_RL* Fun_p = new FUN_RL();

    if (   srvNo == MC_MBFC_0xFE /*新联一个通道*/
           || srvNo == MC_MBFC_0xFF /*关一个通道*/
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

        //对规则步里的模式码进行校验
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

        ptclFunDsc_p->usingMode = usingMode;

        u_char stepNum = 2;//步数

        //1、创建默认的规则步
        Fun_p->srvNo              = srvNo;
        Fun_p->usingMode          = ONLY_ONE;
        Fun_p->cmLvl[ONLY_ONE]    = APP_LVL; // MODBUS规约取APP_LVL
        Fun_p->stepNum[ONLY_ONE]  = stepNum;
        //第一步
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

        //2、对会话配置默认帧数据 T_snnCfg* stnSnnCfg_p
        u_short cmnAddr = 1;
        u_char bodySetNum = 0;
        //构造MBAP报文头 7字节
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
        //对第一步
        //[1,2] 位1 = 1存储子系统执行数据值的保存；
        //位2 = 1存储子系统执行数据值的加载
        tmpStnAcsFlag.ctlType = 0;
        snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
        //第一步请求帧
        u_char oneReqAPDUNum = 1;//第一步的请求帧数量
        snnCfg_p->stepCfg_p[0].reqAPDUNum = oneReqAPDUNum;
        snnCfg_p->stepCfg_p[0].reqBody_setNum_p = (
                new u_char[oneReqAPDUNum]() );
        snnCfg_p->stepCfg_p[0].reqBody_setNum_p[0] = bodySetNum;
        snnCfg_p->stepCfg_p[0].reqEleVal_pp =  (
                                               new ELE_TYPE[oneReqAPDUNum][11]() );

        u_char memVal[ADU_CMTBL_NUM] = {};
        u_char tmpData_p[256] = {}; //用于存储PDU中数据域的数据
        ELE_TYPE eleVal[ADU_CMTBL_NUM] =
        {
            {ADU_MBAPH_INDEX, 7, ( u_char* )& tMbapVal},
            //mbap报文头里的“单元标识符”1字节 配置到 ADU_ADDR_INDEX 域
            {ADU_ADDR_INDEX, 0, &memVal[1]},
            {PDU_FC_INDEX, 1, &memVal[2]},
            {PDU_DATA_INDEX, 0, tmpData_p},
            {ADU_ECHK_INDEX, 0, &memVal[4]}
        };

        //调用接口给snnCfg_p第一步请求帧的第1帧赋值
        //固定功能码配为
        * ( eleVal[PDU_FC_INDEX].p_Mem ) = srvNo;
        //PDU数据无
        tmpData_p[0] = 0;
        tmpData_p[1] = 0;
        tmpData_p[2] = 0;
        tmpData_p[3] = 0;
        // 设置ADU中“数据域”中数据的字节数量
        eleVal[PDU_DATA_INDEX].lng = 0;
        // 设置ADU中“长度域”的数值，它等于从站地址、
        // 功能码和数据的字节数之和
        tMbapVal.length = 3;
        setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
        snnCfg_p->stepCfg_p[0].respAPDUNum = 0;
        //[1,2] 位1 = 1存储子系统执行数据值的保存；
        //位2 = 1存储子系统执行数据值的加载
        tmpStnAcsFlag.ctlType = 0;
        snnCfg_p->stepCfg_p[1].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
        //第二步的请求帧
        snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
        //第二步响应
        u_char oneRespAPDUNum = 1;  //第一步的响应帧数量
        snnCfg_p->stepCfg_p[1].respAPDUNum = oneRespAPDUNum;
        snnCfg_p->stepCfg_p[1].respBody_setNum_p = (
                new u_char[oneRespAPDUNum]() );
        snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] = bodySetNum;
        snnCfg_p->stepCfg_p[1].respEleVal_pp =  (
                                                new ELE_TYPE[oneRespAPDUNum][11]() );
        //调用接口给snnCfg_p第一步请响应的第1帧赋值
        * ( eleVal[PDU_FC_INDEX].p_Mem ) = srvNo; //功能码固定
        memset ( tmpData_p, 0, sizeof ( tmpData_p ) );
        eleVal[PDU_DATA_INDEX].lng = 0;
        tMbapVal.length = 3;
        setOneFrmCfg ( PASSIVE_TYPE, 2, 1, eleVal, snnCfg_p );

        //3、配置默认的步号与tid,cot的对应关系
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
    else //非特殊会话（或功能码）
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

        //对规则步里的模式码进行校验
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

        ptclFunDsc_p->usingMode = usingMode;

        u_char stepNum = 2;//步数

        //1、创建默认的规则步
        Fun_p->srvNo              = srvNo;
        Fun_p->usingMode          = ONLY_ONE;
        Fun_p->cmLvl[ONLY_ONE]    = APP_LVL; // MODBUS规约取APP_LVL
        Fun_p->stepNum[ONLY_ONE]  = stepNum;
        //第一步
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

        //2、对会话配置默认帧数据 T_snnCfg* stnSnnCfg_p
        u_short cmnAddr = 1;
        u_char bodySetNum = 1;
        //构造MBAP报文头 7字节
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
        //对第一步
        //[1,2] 位1 = 1存储子系统执行数据值的保存；
        //位2 = 1存储子系统执行数据值的加载
        tmpStnAcsFlag.ctlType = 0;
        snnCfg_p->stepCfg_p[0].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
        //第一步请求帧
        u_char oneReqAPDUNum = 1;//第一步的请求帧数量
        snnCfg_p->stepCfg_p[0].reqAPDUNum = oneReqAPDUNum;
        snnCfg_p->stepCfg_p[0].reqBody_setNum_p = (
                new u_char[oneReqAPDUNum]() );
        snnCfg_p->stepCfg_p[0].reqBody_setNum_p[0] = bodySetNum;
        snnCfg_p->stepCfg_p[0].reqEleVal_pp =  (
                                               new ELE_TYPE[oneReqAPDUNum][11]() );

        u_char memVal[ADU_CMTBL_NUM] = {};
        u_char tmpData_p[256] = {}; //用于存储PDU中数据域的数据
        ELE_TYPE eleVal[ADU_CMTBL_NUM] =
        {
            {ADU_MBAPH_INDEX, 7, ( u_char* )& tMbapVal},
            //mbap报文头里的“单元标识符”1字节 配置到 ADU_ADDR_INDEX 域
            {ADU_ADDR_INDEX, 0, &memVal[1]},
            {PDU_FC_INDEX, 1, &memVal[2]},
            {PDU_DATA_INDEX, 1, tmpData_p},
            {ADU_ECHK_INDEX, 0, &memVal[4]}
        };

        //调用接口给snnCfg_p第一步请求帧的第1帧赋值
        //配置一个默认的功能帧数据，功能码配为01
        * ( eleVal[PDU_FC_INDEX].p_Mem ) = 0x03;
        tmpData_p[0] = 1; //起始地址2个字节 tmpData_p[0] tmpData_p[1]
        tmpData_p[1] = 0;
        tmpData_p[2] = 20; //线圈数量2个字节 tmpData_p[2] tmpData_p[3]
        tmpData_p[3] = 0;
        // 设置ADU中“数据域”中数据的字节数量
        eleVal[PDU_DATA_INDEX].lng = 4;
        // 设置ADU中“长度域”的数值，它等于从站地址、
        // 功能码和数据的字节数之和
        tMbapVal.length = 6;
        setOneFrmCfg ( ACTIVE_TYPE, 1, 1, eleVal, snnCfg_p );
        snnCfg_p->stepCfg_p[0].respAPDUNum = 0;
        //[1,2] 位1 = 1存储子系统执行数据值的保存；
        //位2 = 1存储子系统执行数据值的加载
        tmpStnAcsFlag.ctlType = 2;
        snnCfg_p->stepCfg_p[1].stnAcsFlag = * ( ( u_short* ) &tmpStnAcsFlag );
        //第二步的请求帧
        snnCfg_p->stepCfg_p[1].reqAPDUNum = 0;
        //第二步响应
        u_char oneRespAPDUNum = 1;  //第一步的响应帧数量
        snnCfg_p->stepCfg_p[1].respAPDUNum = oneRespAPDUNum;
        snnCfg_p->stepCfg_p[1].respBody_setNum_p = (
                new u_char[oneRespAPDUNum]() );
        snnCfg_p->stepCfg_p[1].respBody_setNum_p[0] = bodySetNum;
        snnCfg_p->stepCfg_p[1].respEleVal_pp =  (
                                                new ELE_TYPE[oneRespAPDUNum][11]() );
        //调用接口给snnCfg_p第一步请响应的第1帧赋值
        * ( eleVal[PDU_FC_INDEX].p_Mem ) = 3; //功能码配为01
        memset ( tmpData_p, 0, sizeof ( tmpData_p ) );
        tmpData_p[0] = 2; //字节数1个地字节 tmpData_p[0]
        tmpData_p[1] = 1; //线圈状态N个字节，这里N的值为 tmpData_p[0]的值
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

        //3、配置默认的步号与tid,cot的对应关系
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

    }//end 非特殊会话（或功能码）

    return Fun_p ;
}




/*******************************************************************************
 *功能 : 释放ONE_STEP对象空间
 *输入 : ONE_STEP对象指针
 *输出 : 无
 *返回 : 0 成功
 *修改历史
 *     采用104规约函数，无需改动
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

    //如果有ONE_STEP* nextStep成员则递归下一ONE_STEP成员节点
    if ( pSpace->nextStep_p != NULL )
    {
        freeOneStep ( pSpace->nextStep_p );
    }

    delete pSpace;
    pSpace = NULL;

    return 0;
}




/*******************************************************************************
 *功能 : 释放FUN_RL对象空间
 *输入 : FUN_RL对象指针
         T_snnCfg 对象指针
         T_ptclFunDsc 对象指针
 *输出 : 无
 *返回 : 0，成功
 *修改历史
 *      采用104规约函数，无需改动
*******************************************************************************/
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

        delete pSpace;
        pSpace = NULL;
    }

#ifdef INTER_CREATERL_VAL

    if ( pSnnCfg != NULL ) //T_snnCfg释放
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

                //释放请求帧空间
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

                //释放响应帧空间
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

        //delete [] ptclFunDsc;
        //ptclFunDsc 不在 createRl_P 中创建，所以不在此释放
    }

#endif//#ifdef INTER_CREATERL_VAL

    return vRet;
}




/*******************************************************************************
 *功能 : 通过会话的步规则信息，构造功能码和数据域。
 *输入：
 *       stnSnnCfg_p：站会话配置，会话的通道信息块中保留存储地址；
 *       stepNo:步号
 *输出：
 *       stnSnnPara_p: 给定步号需要加载的点地址、时标和协议要求传
                       输且由存储保存的ADU其它类型元素值
 *
 *返回值：执行状态，=0成功，<0 失败
 *修改历史
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

    int setNum = 0;//统计所有帧中信息体的个数
    //ADU_MBAP_VAL tMbapVal = {}; //MBAP报文头 7字节
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

        //遍历所有请求帧
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

            /*将mbap报文头里的如下域（共4字节）:
                功能码:      1字节
                单元标识符:  1字节
                事务元标识符:2字节
            记入T_snnPara结构的ASDUID_p[i]变量
            */
            u_char tmpFCode = tModbusFrame_p->pduFc;
            tMbsId.func_id  = tmpFCode;
            tMbsId.trans_id = tModbusFrame_p->mbapHeader.trans_id;
            tMbsId.unit_id  = tModbusFrame_p->mbapHeader.unit_id;

            stnSnnPara_p->ASDUID_p[i] = * ( ( u_int* ) &tMbsId );

            //从stnSnnCfg_p中得到PDU数据
            PDU_DATA* pduData_p = & ( tModbusFrame_p->pduData );

            if ( tmpFCode != MC_MBFC_0xFE
                 && tmpFCode != MC_MBFC_0xFF
                 && pduData_p->lng == 0 )
            {
                return ERR_CONFIG;
            }

            if ( tmpFCode == MC_MBFC_0xFE    /*新联一个通道*/
                 || tmpFCode == MC_MBFC_0xFF  /*关一个通道*/
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
                 * PDU数据数据域里：2字节起始地址+2字节读或写的数量或值
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
            else if ( 15 == tmpFCode    //写多个线圈
                      || 16 == tmpFCode  //写多个寄存器
                    )
            {
                /*
                 * PDU数据数据域里：2字节起始地址+2字节输出数量或寄存器数量
                                   +1字节字节数+数出值N字节
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
            else if ( 20 == tmpFCode  //读文件记录
                      || 21 == tmpFCode //写文件记录
                    )
            {
                //计划不实现
            }
            else if ( 22 == tmpFCode ) //屏蔽写寄存器
            {
                tUsrData_p->gData.pntType = g_FCMDBUSLNG[tmpFCode].byteLng;
                /*
                 * PDU数据数据域里：参考地址（2字节）+ And_Mask(2字节)
                                   + Or_Mask(2字节)
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
                u_char* reqWVal_p = NULL;

                u_char offsetNum = 0;//偏移量

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
            else if ( 43 == tmpFCode ) //读设备识别码
            {
                /*暂不实现*/
            }

            /*if( setNum != tStepCfg_p->reqBody_setNum_p[i] )
            {
                return ERR_CONFIG;
            }*/

            stnSnnPara_p->pntNum += setNum;
        }
    }
    else//被控站
    {
        //modbus 根据我的分析暂不存在被动站需要调用getGLoadPnt的情况
        /*modbus 因比较简单（一问，一答），规则步只划分到一步中实现，
             在主动站发送请求时调用getGLoadPnt加载
         配置数据到用户数据结构T_snnPara中；被控站侧收到主控站发的请
             求帧数据，检验数据合法后，
         直接根据请求帧的数据（请求地址、数量等）生成响应帧数据
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

        ////遍历所有请求帧
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

        ///*将mbap报文头里的如下域（共4字节）:
        //    功能码:      1字节
        //    单元标识符:  1字节
        //    事务元标识符:2字节
        //记入T_snnPara结构的ASDUID_p[i]变量
        //*/
        //u_char tmpFCode = tModbusFrame_p->pduFc;
        //tMbsId.func_id  = tmpFCode;
        //tMbsId.trans_id = tModbusFrame_p->mbapHeader.trans_id;
        //tMbsId.unit_id  = tModbusFrame_p->mbapHeader.unit_id;

        //stnSnnPara_p->ASDUID_p[i] = *((u_int*)&tMbsId);

        ////从stnSnnCfg_p中得到PDU数据
        //         PDU_DATA * pduData_p = &( tModbusFrame_p->pduData );
        /*if( tmpFCode != MC_MBFC_0x00
        && tmpFCode != MC_MBFC_0xFF
        && pduData_p->lng == 0 )
        {
        return ERR_CONFIG;
        }*/
        //if ( tmpFCode == MC_MBFC_0x00    /*新联一个通道*/
        //    || tmpFCode == MC_MBFC_0xFF  /*关一个通道*/
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
        //               * 响应PDU数据数据域里：字节数（1字节)+状态或值（N字节）
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
        //          else if ( 5==tmpFCode   //写单个线圈
        //              ||6== tmpFCode  //写单个寄存器
        //              )
        //          {
        //              /*
        //               * 响应PDU数据数据域里：地址（2字节)+值（2字节）
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
        //          else if (15==tmpFCode       //写多个线圈
        //                   || 16 == tmpFCode  //写多个寄存器
        //                   )
        //          {
        //              /*
        //               * 响应PDU数据数据域里：起始地址（2字节)+数量（2字节）
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
        //          else if (20 == tmpFCode   //读文件记录
        //                   || 21== tmpFCode //写文件记录
        //                  )
        //          {
        //              //计划不实现
        //          }
        //          else if ( 22==tmpFCode )//屏蔽写寄存器
        //          {
        //              /*
        //               * 响应PDU数据数据域里：参考地址（2字节）+ And_Mask(2字节)
        //                                     + Or_Mask(2字节)
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
        //          else if ( 23==tmpFCode )//读写多个寄存器
        //          {
        //              /*
        //               * 响应PDU数据数据域里：字节数（1字节）+ 读寄存器值(N字节)
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
        //          else if ( 43==tmpFCode )//读设备识别码
        //          {
        //              /*暂不实现*/
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
  *功能：步驱动，设置起始帧对应的步（会话中的第一帧才调用）
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
         || statrtStepStnD_p->ASDUID_p == NULL
       )
    {
        return ERR_POINTER_NULL;
    }

    int tmpRet              = 0; //下层函数返回值
    int nxtStepStat         = 0; //下一帧会话状态
    u_char nxtFrmStepNo     = 1; //下一帧步号
    ONE_STEP tmpNowRlStep   = {0};//临时规则步变量
    T_ptclFunDsc tmpFuncDsc = {0};//临时会话号与主动性变量

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
        //不在定义的会话（功能码）范围内
        return ERR_FC_VAL_RAN;
    }

    //调用接口asgmtNxtFrmPar进行装帧操作
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

    //发送连续执行
    nxtStepStat = MC_SEND_WAIT_CHECK;

    return nxtStepStat;

}




/*******************************************************************************
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
 *      >0           ：调用nxtStep发的这一帧数据的下一帧对应
                       的步号(如果已经是最后一步最后一帧则此
                       值为当前帧对应的步号）
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
        //不在定义的会话（功能码）范围内
        return ERR_FC_VAL_RAN;
    }

    *nxtStepStat_p = 0xffff;       //初始化为非会话状态值

    int                tmpRet = 0;
    u_char     nxtFrmStepNo   = 0;
    ONE_STEP* lastRuleStep_p  = NULL;
    ONE_STEP* nowRuleStep_p   = NULL; //当前步的规则步指针
    ONE_STEP* nxtFrmRlStep_p  = NULL;//下一帧对应的规则步
    u_char tUsingMode         = Fun_p->usingMode;

    nxtFrmStepNo = stepRl_p->stepNo;

    //找到规则步最后一步
    lastRuleStep_p = Fun_p->steps_p[tUsingMode];

    while ( lastRuleStep_p->nextStep_p != NULL )
    {
        lastRuleStep_p    = lastRuleStep_p->nextStep_p;
    }

    if ( stepRl_p->stepNo > lastRuleStep_p->stepNo )
    {
        *nxtStepStat_p = MC_NO_SEND_SSN_STOP; //不发送会话中止
        return nxtFrmStepNo;
    }

    //当前步的规则步指针
    nowRuleStep_p = displayNextNode (
                    Fun_p->steps_p[tUsingMode],
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

    if ( stepRl_p->stepNo != nxtFrmStepNo )
    {
        nxtFrmRlStep_p = nowRuleStep_p->nextStep_p;
    }
    else
    {
        nxtFrmRlStep_p = nowRuleStep_p;
    }

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


    return nxtFrmStepNo;
}




/*******************************************************************************
  *功能：解ASDU
  *     （104协议中：此接口设计是为了解析出TID_p和COT_p,后面的接口根据TID_p和COT_p
  *         得到会话号与会话的主被动性、会话步号）
  *（modbus协议中：此接口没有实际的意义，因为：只从帧的数据来看无法区分主被动和步号）
  *
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

    if ( APDU_lng == 0 ) //APDU_p 里只有apci
    {
        //104协议中此处的逻辑是进行U帧抽象的会话,这些会话用于I帧传输之前的状态处理
        //modbus不存在此逻辑
        return ERR_LOGIC_RUN;
    }
    else
    {
        /*104协议中此处的逻辑是先把APDU_p绑定到全局104对象，
                然后解析出其中的COT与TID用于其他接口 判断当
                前数据处于什么会话功能号？及会话步号？*/

        //modbus协议 没有此接口中的逻辑，暂时给一个默认的定值
        //取功能码
        u_char tFuncCode = APDU_p[7];
        //*TID_p = MODBUS_TID;
        *TID_p = tFuncCode;
        *COT_p = MODBUS_COT_ACT;

    }

    return 0 ;
}




/*******************************************************************************
  *功能：获取可能的功能号
  *       (104协议中：此接口设计是为得到会话号与会话的主被动性)
  *（modbus协议中：此接口在modbus中没有实际的意义,因为：只从帧
  *        的数据来看无法区分主被动 主动被在站的开始时就确定了与104有区别)
  *
  *输入：
  *     TID：     类型标识符(在modbus中此值传的是功能码)
  *     COT：     传输原因
  *输出：
  *     funDsc_p：会话号与主动性数组
  *     funNum_p：功能码个数（即funDsc_p数组实际下标值）
  *返回：
  *     协议编号：宏P_MODBUS_TCP
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

    //T_ptclFunDsc function[3] = {}; //建立T_ptclFunDsc型a数组
    *funNum_p = 0;

    //if ( TID == MODBUS_TID )
    if ( ASSERTMBFC ( TID ) ) //判断功能码是否在定义的范围内
    {
        //funDsc_p[*funNum_p].ssnDscNo = QUE_DATA;
        funDsc_p[*funNum_p].ssnDscNo = ( u_short ) TID;
        //function[*funNum_p].act_prio_chnlID 的主被动性无法确定
        ( *funNum_p ) ++;
    }

    //将function数组的值赋给出参funDsc_p数组
    //memcpy(funDsc_p,function,sizeof(T_ptclFunDsc)*(*funNum_p));

    return P_MODBUS_TCP;

}




/*******************************************************************************
  *功能：获取会话步号（并绑定新的ASDU对象)
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


    if ( funDsc_p == NULL
         || funDsc_p->remoteSet_p == NULL
       )
    {
        return ERR_POINTER_NULL;
    }

    u_int stepNo = 0xffff;

    //如果不是U帧抽象出的特殊会话
    //if ( funDsc_p->ssnDscNo == U_STARTDT
    //    && funDsc_p->ssnDscNo == U_STOPDT
    //    && funDsc_p->ssnDscNo == U_TESTFR
    //    && funDsc_p->ssnDscNo == U_NEWHOLD
    //    && funDsc_p->ssnDscNo == U_DELHOLD
    //    )
    //{
    //    /*此处逻辑在104协议中有意义，modbus中无此处的逻辑*/
    //}
    //else
    //{
    //    //modbus都定为只有一步
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
    if ( ( u_char ) funDsc_p->act_prio_chnlID == ACTIVE_TYPE ) //主动站
    {
        stepNo = 2;
    }
    else//被动站
    {
        stepNo = 1;
    }

    return stepNo;

}




/*******************************************************************************
  *功能 ：对会话步stepRl_p中的帧进行验收，验收后把帧数据赋于ssnPar_p
  * 输入：
  *     Fun_p        :步规则
  *     stepRl_p     :当前步
  *     funDsc_p     :会话号与主动性
  *输出：
  *     ssnPar_p     :详见此类型说明
  *     nxtStepNo_p  :下一帧的步号（最后一步的最后一帧时步号不变）
  *返回：
  *  0               :成功，
  *  0xffff          :该功能不支持
  *  负数            :失败
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

    int vRet    = 0;    //调用下层函数返回值
    //调用chkRecData进行相关功能的校验
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

    /*数据值的加载、保存标识:
        位0 = 1存储子系统执行数据值的保存；
        位1 = 1存储子系统执行数据值的加载；
            全 = 0, do nothing；全 = 1，加载且保存
        位3表示ADU元素3，=1表示，位0与位1的操作在此
            元素中有效，=0为无效；
        */
    T_stnAcsFlagM tmpStnAcsFlag = {0, 0, 0, 4095};
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

    //根据stepRl_p及C_MBAP的相应内容给ssnPar_p赋值
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
  *功能 ：把PDU_p指针绑定到ONE_STEP对象上
  * 输入：
  *     stepRl_p  :绑定到的目标ONE_STEP
  *     funDsc_p  :主、被动 描述
  *     PDU_p     :被绑定的地址
  *     PDUlng    :绑定内存的长度
  *     duType    :绑定地址的类型
  *输出：
  *     无
  *返回：
  *     0         :成功，
  *     负数      :失败
  *修改：
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

    if ( act_prio_chnlID == PASSIVE_TYPE ) //是响应帧
    {
        tMbap_p = ( C_ModbusADU* ) stepRl_p->respAPDU_Obj_p;
    }
    else //是请求帧
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
        return -1;//apdu长度错误
    }

    return 0;
}




/*******************************************************************************
  *功能 ：获得C_ModbusADU对象的ADU_p或PDU_p地址
  * 输入：
  *     apduObj_p :C_ModbusADU对象
  *     duType    :需要获取的类型（adu或pdu)
  *输出：
  *     apduLng   :ADU_p或PDU_p内存中内容的长度
  *返回：
  *     ADU_p或PDU_p地址
  *修改：
  *
 ******************************************************************************/
EXPDLL u_char* getDU ( u_char* apduObj_p,
                       u_short& apduLng,
                       char duType
                     )
{
    u_char* tDu_p = ( ( C_ModbusADU* ) apduObj_p )->getDU ( apduLng, duType );

    if ( tDu_p != NULL
         && ( tDu_p[7] == MC_MBFC_0xFE    /*新联一个通道*/
              || tDu_p[7] == MC_MBFC_0xFF     /*关一个通道*/
            )
       )
    {
        //与104协议类似u帧的长度返回0
        apduLng = 0;
    }

    return tDu_p;
}




/*******************************************************************************
*功能：MODBUS数据中的ADU_p或PDU_p内存中内容的长度。
*输入：
*      apduObj_p 对象指针
*输出：
*      无
*返回：
*      ADU_p或PDU_p内存中内容的长度
*修改历史：
*
*******************************************************************************/
EXPDLL u_short getDULng ( u_char* apduObj_p )
{
    return ( ( C_ModbusADU* ) apduObj_p )->getDU_Lng();
}




/*******************************************************************************
*功能：设置 ADU数据帧的结构描述
*输入：
*      apduObj_p ：C_ModbusADU对象指针
*      apduDSCR  ：ADU数据帧的结构描述
*输出：
*      无
*返回：
*      void
*修改历史：
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
*功能：MODBUS数据中的帧头长度，即MBAP的长度。
*输入：
*      void；
*输出：
*      void；
*返回：
*      u_char
*修改历史：
*
*******************************************************************************/
EXPDLL u_char getHeaderlng()
{
    return 7;
}




/*******************************************************************************
*功能：只将MODBUS的ADU对象转换成字节流,并更新MBAP中“长度”域的值。
*描述：在MODBUS中，不同角色的站点，构造帧头的方式是不同的。主站需
*       要设置帧头的所有域，而子站只需要设置长度域，
*       而其他域则复制请求帧中的响应的段域。
*输入：
*      rtmParam_p   ： 空；
*      cfgParam_p   ： 空；
*      APDU104_p    ： MODBUS协议的ADU对象
*      ASDUFrm_Lng  :  PDU数据的长度
*输出：
*      APDU_buf_p   : ADU数据的字节流
*      APDU_bufLng  : 字节流的长度
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
    // 获取MODBUS对象-ADU
    C_ModbusADU* adu_p = ( C_ModbusADU* ) APDU104_p;
    u_char* PDUFrm_p = adu_p->getDU ( ASDUFrm_Lng, ADU_TYPE );

    if ( PDUFrm_p == NULL )
    {
        return ;
    }

    // 计算ADU的数据长度
    APDU_bufLng = ( u_char ) ASDUFrm_Lng /*+ 7*/;

    // 复制ADU数据
    memcpy ( APDU_buf_p, PDUFrm_p, APDU_bufLng );

    //修改ADU帧头MBAP中“长度”标识的值
    u_short length = ASDUFrm_Lng - 6;

    //大小端转换
    reversEndian ( ( u_char* ) &length, sizeof ( u_short ) );
    //MBAP=2字节事务元标识+2字节协议标识+2字节长度+1字节单元标识
    memcpy ( APDU_buf_p + 4, &length, 2 );
    /*APDU_buf_p[4] = (length >> 8) & 0xFF;
    APDU_buf_p[5] = (length >> 0) & 0xFF;*/
}




/*******************************************************************************
*功能：检查接收的数据帧是否符合MODBUS数据帧的格式规范，暂时
*      只实现TCP帧，其他帧格式未考虑。
*输入：
*      rtmParam_p   ： 空；
*      cfgParam_p   ： 暂时为空；用于指定MODBUS帧的格式。
*      revDataFrm   ： 接收数据
*输出：
*      APDU_buf_p   : ADU数据的字节流
*      APDU_bufLng  : 字节流的长度
*返回：
*      void    ：ADU数据格式正确时，返回值的高8位表示帧类型，低8位为0；
*                ADU数据格式正确时，返回值的高8位表示将要处理的动作，
*                 低8位为错误代码；
*修改历史：
*
*******************************************************************************/
EXPDLL u_short chkAPDUData ( revBuf* revDataFrm,
                             void* rtmParam_p,
                             void* cfgParam_p
                           )
{
    /*
    //配置参数cfgParam_p指明了MODBUS包的结构
    u_char frame_mode = ((thresholdCfgPara*)cfgParam_p)->mode;
    //返回值的数据结构
    frameType frmType = {0};

    if( frame_mode == MODBUS_ASCII )
    {
        //实现暂时为空
        return *(u_short*)&frmType;
    }
    else if( frame_mode == MODBUS_RTU )
    {
        //实现暂时为空
        return *(u_short*)&frmType;
    }
    else if( frame_mode == MODBUS_TCP )
    {
        //ADU数据
        ADU_TCP_VAL* adu_p = (ADU_TCP_VAL*)revDataFrm->APDU_Buf.APDU_Data_p;

        //检查Modbus数据帧中的协议标识符
        if ( adu_p->pctl_id != PROTOCOL_MODBUS)
        {
            frmType.fTypeL = MC_UNKNOWN_MODBUS; // 非MODBUS协议
            frmType.fTypeH = NO_OPERATION;
            return *(u_short*)&frmType;
        }

    }
    else
    {
        frmType.fTypeL = MC_UNKNOWN_MODE; // 未配置MODBUS协议的帧格式
        frmType.fTypeH = NO_OPERATION;
        return *(u_short*)&frmType;
    }

    */

    //返回值的数据结构
    frameType frmType;
    frmType.fTypeL = MC_APDU_OK;
    frmType.fTypeH = I_F;

    //ADU数据
    ADU_TCP_VAL* adu_p = ( ADU_TCP_VAL* ) revDataFrm->APDU_Buf.APDU_Data_p;

    //检查Modbus数据帧中的协议标识符
    if ( adu_p->pctl_id != PROTOCOL_MODBUS )
    {
        frmType.fTypeL = MC_UNKNOWN_MODBUS; // 非MODBUS协议
        frmType.fTypeH = NO_OPERATION;
        return * ( u_short* ) &frmType;
    }

    //检查Modbus数据帧中的长度
    u_short length = 0;
    //X86采用小端模式,而MODBUS数据采用大端模式
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
*功能：设置帧头MBAP中的“长度”域,并检查发送数据的条件是否就绪。
*      在MODBUS协议中，如果是客户端（请求数据端），
*      需要记忆功能，以便核对服务端发回的应答帧。发送数据前，MODBUS
*      检查是否可以将数据加入到存储链表中。
*      如果是服务端（响应数据），就不需要记忆功能，将数据发出即可。
*输入：
*      role        ： 站点角色；
*      rtmParam_p  ： 实时参数，比如MBAP帧头中的事物处理标识；
*      data_p      :  ADU数据
*      length      :  ADU数据长度
*输出：
*      void
*返回：
*      u_int       :  高16位返回函数执行的结果信息（错误信息）；
*                     高16位返回结果信息（错误信息）的解决方法
*注意事项：
*
*******************************************************************************/
EXPDLL u_int CheckSendState ( short role,
                              void* rtmParam_p,
                              u_char* data_p,
                              u_short length
                            )
{
    //返回值
    sandMotion result = {0};

    // 控制站, 客户端（发送请求帧）
    if ( role == MASTER_STATION )
    {
        //记录请求帧中的功能码和事物处理标识，以便核对服务端发回的响应帧
        T_mbsChkNode nodeData;
        T_mbsChkData* chkData_p = NULL;
        //通道中用于存储数据的空间
        chkData_p = ( T_mbsChkData* ) & (
                    ( ( realTimeValue* ) rtmParam_p )->chkData );
        // 事物处理标识
        nodeData.trans_id = ( ( realTimeValue* ) rtmParam_p )->sendnum;
        nodeData.func_id  = data_p[7];   // 功能码

        // 设置MBAP中的事物处理标识, MODBS采用Big-Endian模式
        /*data_p[0] = ( nodeData.trans_id >> 8 ) & 0xFF;
        data_p[1] = ( nodeData.trans_id >> 0 ) & 0xFF;*/
        memcpy ( data_p, & ( nodeData.trans_id ), 2 );
        reversEndian ( data_p, 2 );


        // 设置MBAP中的协议标识
        data_p[2] = 0x00;
        data_p[3] = 0x00;

        /*
        fusk add@20171218130936 chkData_p->nex_index = 0
        注意此操作前提是针对modbus协议的一问一答的特殊形式而
        定的；如果上层在没有等到子站响应且没有超时再次重发请
        求帧这种情况是不允许的
        需要上层控制并避免
        */
        chkData_p->nex_index = 0;

        // 检查等待响应帧的链表是否还有空间。发送请求帧时进入该链表，
        //接收到对应的响应帧时，从该链表中去除。
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
    // 被控站，服务端（发送响应帧）
    else if ( role == SLAVE_STATION )
    {
        // 响应帧MBAP中“长度”域由packAPDU(）
        //设置，其他域由框架上层接收侧规约负责复制
        result.errType = SEND_READY_STATUS;
        result.motion  = WRITE_CONNECT;
    }
    // 非法的状态（状态机中不存在的状态）
    else
    {
        result.errType = SEND_ROLE_ERROR;
        result.motion  = NO_OPERATION;
    }

    return * ( u_int* ) &result;
}




/*******************************************************************************
*功能：检查接收数据帧的功能码和事物处理标识是否和本站发出的请求
*      帧一致。在MODBUS协议中，只有客户端（请求数据端），
*     才需要检查接收数据，而服务端（响应数据）就不需要检查数据。
*     上层构造数据时，直接复制帧头即可。
*输入：
*      role        ： 站点角色；
*      rtmParam_p  ： 实时参数，比如MBAP帧头中的事物处理标识；
*      data_p      :  ADU数据
*      length      :  ADU数据长度
*输出：
*      void
*返回：
*      u_int       :  高16位返回函数执行的结果信息（错误信息）；
*                     高16位返回结果信息（错误信息）的解决方法
*注意：
*
*******************************************************************************/
EXPDLL u_int CheckRecvState ( short role,
                              void* rtmParam_p,
                              u_char* data_p,
                              u_short length
                            )

{
    //返回值
    sandMotion result = {0};

    // 控制站, 客户端（接收到响应帧）
    if ( role == MASTER_STATION )
    {
        //记录请求帧中的功能码和事物处理标识，
        //以便核对服务端发回的响应帧
        T_mbsChkData* chkData_p = NULL;
        //通道中用于存储数据的空间
        chkData_p = ( T_mbsChkData* ) & (
                    ( ( realTimeValue* ) rtmParam_p )->chkData );

        // 解析出功能码和事物处理标识
        T_mbsChkNode nodeData;
        // 事物处理标识, MODBS采用Big-Endian模式，X86采用小端模式
        //nodeData.trans_id = ( data_p[0] << 8 ) + ( data_p[1] );
        memcpy ( &nodeData.trans_id, data_p, 2 );
        reversEndian ( ( u_char* ) & ( nodeData.trans_id ), sizeof ( u_short ) );
        // 功能码
        nodeData.func_id  = data_p[7];

        if ( nodeData.func_id > 0x80 ) //说明响应的数据是异常响应
        {
            //得到异常响应对应的功能码
            nodeData.func_id = nodeData.func_id - 0x80;
        }


        // 设置MBAP中的协议标识
        data_p[2] = 0x00;
        data_p[3] = 0x00;

        // 检查接收帧是否为本站发出请求帧的响应，即检查MBAP中
        // 的事物处理标识和功能码
        // 如果是本站发出请求帧的响应，就从该链表中去除请求帧。
        if ( delSearData ( ( void* ) chkData_p, ( void* ) &nodeData ) != 0 )
        {
            result.errType = MC_APDU_CHECKSUM;
            result.motion  = NO_OPERATION;
        }
        else
        {
            result.errType = MC_APDU_OK;
            result.motion  = READ_CONNECT;
            // 从请求列表中删除该请求
            //delSearData((void*)chkData_p, (void*)&nodeData);
        }
    }
    // 被控站，服务端（接收到请求帧）
    else if ( role == SLAVE_STATION )
    {
        // 响应帧MBAP中“长度”域由packAPDU(）
        // 设置，其他域由框架上层接收侧规约负责复制
        result.errType = MC_APDU_OK;
        result.motion  = READ_CONNECT;
    }
    // 非法的状态（状态机中不存在的状态）
    else
    {
        result.errType = MC_APDU_CHECKSUM;
        result.motion  = NO_OPERATION;
    }

    return * ( u_int* ) &result;
}




/*******************************************************************************
*功能：更新发送端的实时参数，比如MBAP帧头中的事物处理标识, 即发送序号。
*输入：
*      role           ： 站点角色；
*      rtmParam_p     ： 实时参数，比如MBAP帧头中的事物处理标识；
*      cfgParam_p     ： 空
*      tmrCtrlCode_p  ： 空
*      data_p         :  ADU数据
*      length         :  ADU数据长度
*输出：
*      rtmParam_p
*返回：
*      u_int       :  是否需要发送侧协调，MODBUS不需要协调，因此都返回0
*修改历史：
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
    // 框架协调控制变量
    u_short result = 0;

    // 控制站, 客户端（发送请求帧）
    if ( role == MASTER_STATION )
    {
        // 更新事物处理标识, 即发送序号。
        ( ( realTimeValue* ) rtmParam_p )->sendnum = (
                ( ( realTimeValue* ) rtmParam_p )->sendnum  % 0x7fff );
        ( ( realTimeValue* ) rtmParam_p )->sendnum++;

        result = 0;
    }
    // 被控站，服务端（发送响应帧）
    // MODBUS协议中，响应帧的事物处理
    //标识来自对方的请求帧，因此并不需要更新。
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
* 功能：在MODBUS中，并不需要设置接收侧的参数，因此该函数没有使用。
* 描述：无
* 接口参数：
*     无
* 返回值：无
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

    // 框架协调控制变量
    ctrl_Value result = {0};

    return * ( u_short* ) &result;
}




/*******************************************************************************
* 功能：在MODBUS中，该函数没有使用。
* 描述：无
* 接口参数：
*     无
* 返回值：无
*******************************************************************************/
EXPDLL int getFrameType ( u_char* data_p,
                          u_short length
                        )
{
    int tRet = I_F;

    if ( data_p != NULL && length == 0 )
    {
        if ( data_p[7] == MC_MBFC_0xFE ) //新联一个通道
        {
            tRet = 0x03;
        }
        else if ( data_p[7] == MC_MBFC_0xFF ) //关一个通道
        {
            tRet = 0xff;
        }
    }

    return tRet;

}




/*******************************************************************************
* 功能：在MODBUS中，并不需要底层与上层之间的异常情况的协调。
* 描述：无
* 接口参数：
*     无
* 返回值：无
*******************************************************************************/
EXPDLL int HandleRecvStateError ( transCtrlFrmDsc* ctrlFrm,
                                  void* rtmParam_p,
                                  int error
                                )

{
    // 本函数及不需要向左侧控制层对列发送协调命令，
    //也不需要向上侧ASDU对列发送协调命令。

    return 0;
}




/*******************************************************************************
* 功能：在MODBUS中，并不需要底层控制帧应答发送数据。
* 描述：无
* 接口参数：
*     无
* 返回值：无
*******************************************************************************/
EXPDLL int RespCtrlFrame ( transCtrlFrmDsc* ctrlFrm,
                           void* rtmParam_p,
                           int result
                         )
{

    return 0;
}




/*******************************************************************************
* 功能：在MODBUS中，并不需要底层控制帧协调发送数据。
* 描述：无
* 接口参数：
*     无
* 返回值：无
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
        return -1;//apdu长度错误
    }

    return 0;
}




/*******************************************************************************
  *功能：获取MODBUS数据帧的长度。计算方法如下：
  *     从Socket接收缓冲中，获取MBAP中“长度”域的值，
  *     然后再根据MBAP的长度，计算出ADU长度。
  * 输入：
  *      bufferAdd   ：MBAP首地址
  *      bufferLng   : MBAP长度
  *输出：
  *      无
  *返回：
  *    >0 成功返回ADU长度(MBAP+PDU)
  *    －1 缓冲区长度错误
  *    －2 首地址不是启动字符
  *    <0  接口执行异常
  *修改历史
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

    //获取“长度”域的值，MODBUS采用大端模式
    //int aduLng  = (bufferAdd[4] << 8) + bufferAdd[5];
    u_short aduLng = 0;
    memcpy ( &aduLng, bufferAdd + 4, 2 );
    reversEndian ( ( u_char* ) &aduLng, sizeof ( u_short ) );

    //计算ADU数据长度
    // 6 = 2字节（长度）+ 2字节（协议标识符）
    //     +2字节（事务元标识符）
    aduLng = aduLng + 6;

    return aduLng;
}



