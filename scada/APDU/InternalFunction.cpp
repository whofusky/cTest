
#include "InternalFunction.h"




const T_TIDVALLNG  g_TID104[MC_MAXTID_IDS] =
{
    /*tid:0  */ {0,  0, 0, 0},  /*未定义*/
    /*tid:1  */ {1,  1, 1, 0},
    /*tid:2  */ {1,  1, 1, 3},
    /*tid:3  */ {1,  2, 1, 0},
    /*tid:4  */ {1,  2, 1, 3},
    /*tid:5  */ {2,  7, 1, 0},   /*VTI带瞬变状态指示值*/
    /*tid:6  */ {2,  7, 1, 3},
    /*tid:7  */ {5, 32, 1, 0},
    /*tid:8  */ {5, 32, 1, 3},
    /*tid:9  */ {3, 16, 1, 0},
    /*tid:10 */ {3, 16, 1, 3},
    /*tid:11 */ {3, 16, 1, 0},
    /*tid:12 */ {3, 16, 1, 3},
    /*tid:13 */ {5, 32, 1, 0},
    /*tid:14 */ {5, 32, 1, 3},
    /*tid:15 */ {5, 32, 1, 0},  /*BCR二进制计数器读数*/
    /*tid:16 */ {5, 32, 1, 3},
    /*tid:17 */ {1,  2, 1, 5},  /*SEP(继电保护设备事件)+CP16Time2a+CP24Time2a*/
    /*tid:18 */ {2,  6, 1, 5},  /*SPE+QDP+CP16Time2a+CP24Time2a*/
    /*tid:19 */ {2,  4, 1, 5},  /*OCI+QDP+CP16Time2a+CP24Time2a*/
    /*tid:20 */ {5, 32, 1, 0},
    /*tid:21 */ {2, 16, 0, 0},
    /*tid:22 */ {0,  0, 0, 0}, /*22...29 为将来兼容定义保留*/
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
    /*tid:41 */ {0,  0, 0, 0}, /*41...44 为将来兼容定义保留*/
    /*tid:42 */ {0,  0, 0, 0},
    /*tid:43 */ {0,  0, 0, 0},
    /*tid:44 */ {0,  0, 0, 0},
    /*tid:45 */ {1,  0, 1, 0},/*SCO*//*45...69为控制方向的过程信息*/
    /*tid:46 */ {1,  0, 1, 0},/*DCO*/
    /*tid:47 */ {1,  0, 1, 0},/*RCO*/
    /*tid:48 */ {3, 16, 1, 0},
    /*tid:49 */ {3, 16, 1, 0},
    /*tid:50 */ {5, 32, 1, 0},
    /*tid:51 */ {4, 32, 0, 0},/*BSI*/
    /*tid:52 */ {0,  0, 0, 0},/*52...69为将来兼容定义保留*/
    /*tid:53 */ {0,  0, 0, 0},
    /*tid:54 */ {0,  0, 0, 0},
    /*tid:55 */ {0,  0, 0, 0},
    /*tid:56 */ {0,  0, 0, 0},
    /*tid:57 */ {0,  0, 0, 0},
    /*tid:58 */ {1,  0, 1, 7},/*SCO+CP56Time2a;58...64是104协议较101新增的*/
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
    /*tid:70 */ {1,  0, 1, 0},/*70...99 监视方向的系统命令*/
    /*tid:71 */ {0,  0, 0, 0},/*71...99为将来兼容定义保留*/
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
    /*tid:100*/ {1,  0, 1, 0},/*100...109控制方向的系统命令*/
    /*tid:101*/ {1,  0, 1, 0},
    /*tid:102*/ {0,  0, 0, 0},/*读命令只有信息对象地址*/
    /*tid:103*/ {0,  0, 0, 7},/*CP56Time2a*/
    /*tid:104*/ {2, 16, 0, 0},
    /*tid:105*/ {1,  0, 1, 0},
    /*tid:106*/ {0,  0, 0, 7},/*CP56Time2a*//*107...109为将来兼容定义保留*/
    /*tid:107*/ {1,  8, 0, 7},/*104协议增加tid107:TSC+CP56Time2a*/
    /*tid:108*/ {0,  0, 0, 0},
    /*tid:109*/ {0,  0, 0, 0},
    /*tid:110*/ {3, 16, 1, 0},/*110...119在控制方向的参数命令*/
    /*tid:111*/ {3, 16, 1, 0},
    /*tid:112*/ {5, 32, 1, 0},
    /*tid:113*/ {1,  0, 1, 0},
    /*tid:114*/ {0,  0, 0, 0},/*114...119为将来兼容定义保留*/
    /*tid:115*/ {0,  0, 0, 0},
    /*tid:116*/ {0,  0, 0, 0},
    /*tid:117*/ {0,  0, 0, 0},
    /*tid:118*/ {0,  0, 0, 0},
    /*tid:119*/ {0,  0, 0, 0},
    /*tid:120*/ {3, 16, 1, 0},/*120...127 文件传输：暂不处理*/
    /*tid:121*/ {4, 24, 1, 0},
    /*tid:122*/ {3, 16, 1, 0},
    /*tid:123*/ {5, 40, 0, 0},
    /*tid:124*/ {4, 24, 1, 0},
    /*tid:125*/ {0,  0, 0, 0},
    /*tid:126*/ {3, 24, 0, 7},
    /*tid:127*/ {1,  8, 0, 14},/*104协议增加tid127: +CP56Time2a+CP56Time2a*/
    /*tid:128 */ {0, 0, 0, 0},
    /*tid:129 */ {0, 0, 0, 0},
    /*tid:130 */ {0, 0, 0, 0},
    /*tid:131 */ {0,  0, 0, 0},
    /*tid:132 */ {0,  0, 0, 0},
    /*tid:133 */ {0,  0, 0, 0},
    /*tid:134 */ {0,  0, 0, 0},
    /*tid:135 */ {0,  0, 0, 0},
    /*tid:136 */ {0,  0, 0, 0}, /*U帧抽象会话 STARTDT             */
    /*tid:137 */ {0,  0, 0, 0}, /*U帧抽象会话 STOPDT              */
    /*tid:138 */ {0,  0, 0, 0}, /*U帧抽象会话 TESTFR              */
    /*tid:139 */ {0,  0, 0, 0}, /*U帧抽象会话 NEWHOLD 新联一个通道 */
    /*tid:140 */ {0,  0, 0, 0}  /*U帧抽象会话 DELHOLD 关一个通道   */
};


/**************************内部使用函数非接口***********************************/

/*******************************************************************************
  *功能：组装ASDU
  * 输入：
  *      apdu_p      ：APDU104指针
  *      tid_p       : 类型标识
  *      vsq_p       : 可变结构体限定词
  *      cot_p       : ASDU_COT1_VAL类型传送原因
  *      cot2_p      : ASDU_COT2_VAL类型传送原因
  *      addr_p      : ASDU公共地址
  *      bodyAddr_p  : 信息体地址
  *      bodySet_p   : 信息元素集
  *      bodySetLng  : 信息元素集长度
  *      tsp_p       : 信息体时标
  *      bodyObjNum  : 信息对象个数
  *输出：
  *     apdu_p       : 赋值后的APDU104
  *返回：
  *    =0 接口执行成功
  *    <0 接口执行异常
  *修改历史
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
  *功能：
  *      查找startStep_p中步号为targetStep_p->stepNo的步
  *输入：
  *      startStep_p ：规则步
  *      targetStep_p：要查找的步
  *输出：
  *      无
  *返回：
  *      ONE_STEP*
  *              1、异常情况返回 NULL
  *              2、正常情况返回startStep_p中步号为targetStep_p->stepNo的步;
  *修改历史
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
  *功能：在链表ONE_STEP末尾插入一个节点
  * 输入：
  *      one_step_p ：链表头指针
  *      tail_step_p: 要添加的节点指针
  *输出：
  *      one_step_p ：添加节点指针之后的链表头指针
  *返回：
  *      void
  *修改历史：
  *      20170221092324 修改原来的逻辑错误
  *      20170314084211 添加step_p为null的情况
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



//因版本变更此接口暂时无用
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
  *功能：
  *    会话步中帧数据验收后给T_snnPara赋值
  * 输入：
  *     stepRl_p  ：验收后的当前步
  *     funDsc_p  ：会话号与主动性
  *     apdu104_p ：APDU104对象指针
  *     stnAcsFlag：存储、读取标识
  *输出：
  *     ssnPar_p：  详见此类型说明
  *返回：
  *     0 成功
  *    <0 失败
  *修改历史
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

    /*******************************从APDU104对象中取值*************************/
    //类型标识
    ASDU_TID_VAL* ssnTid_p = apdu104_p->getASDU_TID_VAL (
                             & ( tApduDscr_p->ASDU_CMTBL[ASDU_TID_INDEX] )
                             );
    //可变结构体限定词
    ASDU_VSQ_VAL* ssnVSQ_p = apdu104_p->getASDU_VSQ_VAL (
                             & ( tApduDscr_p->ASDU_CMTBL[ASDU_VSQ_INDEX] )
                             );

    if ( ssnVSQ_p == NULL )
    {
        return ERR_POINTER_NULL;
    }

    //传送原因的长度
    u_short* ssnCotLen_p = & (
                           tApduDscr_p->ASDU_CMTBL[ASDU_COT_INDEX] );
    //传送原因
    u_short* ssnCOT_p = ( u_short* ) apdu104_p->getASDU_COT_VAL (
                        ssnCotLen_p );
    //ASDU公共地址
    ASDU_ADDR_VAL* ssnAddr_p = apdu104_p->getASDU_ADDR_VAL (
                               & ( tApduDscr_p->ASDU_CMTBL[ASDU_ADDR_INDEX] )
                               );



    /*******************************赋值T_snnPara**************************/
    ssnPar_p->stepNo      = stepRl_p->stepNo;
    ssnPar_p->infoType    = GINFOTYPE;

    /*单向总帧数 （此值不在某一帧中修改）
    ssnPar_p->ASDUIDNum = 1;
    */
    //u_char actFlag = ( u_char )funDsc_p->act_prio_chnlID;
    char reqFlag = getRcvReqFlag( funDsc_p );

    u_char ASDUID_index = 0;

    if ( reqFlag == 1 )
    {
        //验收请求的数据
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
        //验收响应的数据;
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

    u_char acsMthd        = ( ssnPar_p->acsMthd & 0x7f ); //取低7位
    u_short usrData_index = ASDUID_index * acsMthd;
    ssnPar_p->pntPos      = usrData_index;
    ssnPar_p->pntNum      = usrData_index + ssnVSQ_p->N;

    //因每个ASDU的信息体与信息体时标是不确定的，需要根据具体的会话来处理
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
        case STN_INIT: //站初始化;
        {
            break;
        }

        case QUE_DATA: //查询数据;
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
        }          //end 查询数据

        case CYC_TRNS: //周期传输
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
        }         //end 周期传输

        case ACQ_EVT: //事件收集
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
        }            //end 事件采集

        case QUKCHK_EVT: //快检事件
        {
            //不使用
            break;
        }

        case STN_INRGTN: //站召唤
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
        }//end 站召唤

        case CLK_SYN: //时钟同步
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

        case CMD_TRNS: //命令传输
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
            //累积量传输前的冻结    此功能是从ACMU_TRNS功能的模式2，控制站先冻结再请求传输分离出来的
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

        case ACMU_TRNS: //累积量传输
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

        case PARA_LD: //参数加载
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

        case TST_PRCD: //测试过程
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

        case FILE_TRNS: //文件传输
        {
            //暂不实现
            break;
        }

        case ACQ_TRNSDLY: //收集传输延迟值
        {
            //不使用
            break;
        }

        default:
            break;
    }//end 按应用功能的不同分别对元素集与时标的解包处理

    ssnPar_p->stnAcsFlag = * ( ( u_short* ) stnAcsFlag );

    return 0;
}




/*******************************************************************************
  *功能：
  *    专用步验证块校验
  *
  * 输入：
  *     iVerifyBlock_p  ：专用步验证块
  *     apdu104_p       ：APDU104对象
  *输出：
  *     iVerifyBlock_p->chkRslt_p：验收状态码//位0 = 1 表示按最大值验收成功，
                                   //位 1 = 1时按最小值验收成功
  *返回：
  *     0  成功
  *     -5  限定词与实际元素个数不等
  *     -21 最大值验收失败
  *     -22 最小值验收失败
  *     -23 按最大最小区间值验收
  *修改历史
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

    u_int  tmpUint         = 0;    //点值
    u_short tmpBodSetLng   = 0;    //元素集长度
    u_char* ssnBodSet_p    = NULL; //元素集首地址
    u_int tmpOneLng        = 0;    //每个元素的长度
    u_short tmpBodSetNum   = 0;    //元素个数
    ASDU_VSQ_VAL* ssnVSQ_p = NULL;//可变限定词

    ssnBodSet_p   = apdu104_p->getAPDU_ELEMENT ( &tmpBodSetLng, BODY_SET_INDEX );
    tmpOneLng     = apdu104_p->getDU_Info()->InfoEleLng;
    tmpBodSetNum  = apdu104_p->getDU_Info()->ASDU_CMTBL[BODY_SET_INDEX];
    ssnVSQ_p      = apdu104_p->getASDU_VSQ_VAL (
                    &apdu104_p->getDU_Info()->ASDU_CMTBL[ASDU_VSQ_INDEX]
                    );

    if ( tmpBodSetNum > 0
         && tmpBodSetNum != ssnVSQ_p->N
       ) //限定词与实际元素个数不等
    {
        return ERR_VSQN_NO;
    }

    for ( int i = 0; i < tmpBodSetNum; i++ ) //对每个点值进行大小值校验
    {
        memcpy ( &tmpUint,
                 ssnBodSet_p + i * tmpOneLng,
                 tmpOneLng
               );

        if ( iVerifyBlock_p->vrfCd == 1 )   //按最大值验收;
        {
            if ( iVerifyBlock_p->chkStd.gChkStd.pntVal2 < tmpUint )
            {
                return ERR_MAX_VAL_CH;
            }

            * ( iVerifyBlock_p->chkRslt_p ) = 1;
        }
        else if ( iVerifyBlock_p->vrfCd == 2 ) //按最小值验收;
        {
            if ( ( u_char ) iVerifyBlock_p->chkStd.gChkStd.pntVal1 > tmpUint )
            {
                return ERR_MIN_VAL_CH;
            }

            * ( iVerifyBlock_p->chkRslt_p ) = 2;
        }
        else if ( iVerifyBlock_p->vrfCd == 3 ) //按最大最小区间值验收;
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

    u_char act_prio_chnlID = ( u_char ) funDsc_p->act_prio_chnlID;

    //对参数进行校验
    if (   act_prio_chnlID != ACTIVE_TYPE
           && act_prio_chnlID != PASSIVE_TYPE
       )
    {
        return ERR_ACT_PAS_VAL;
    }

    char reqFlag = getSndReqFlag( funDsc_p );

    APDU104* apdu104_p = NULL;

    //取需要装帧的apdu对象
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

    //取tid
    ASDU_TID_VAL* TID_p  = NULL;

    //下一帧是重新开始的一步
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

    //传送原因
    ASDU_COT1_VAL* cot1Val_p = NULL;
    ASDU_COT2_VAL* cot2Val_p = NULL;
    //取cot
    cot1Val_p   = ( ASDU_COT1_VAL* ) & ( ssnPar_p->COT );

    //136...255 特殊应用（专用范围）
    //U帧变化而来的功能会话
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

        APCI_104 apci104  = {};//104协议的APCI
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

        //长度置为0
        APDU_DSCR* apdu_dscr_p = apdu104_p->getDU_Info();

        for ( int i = ASDU_TID_INDEX; i <= ASDU_TSP_INDEX; i++ )
        {
            apdu_dscr_p->ASDU_CMTBL[i] = 0;
        }

        if ( reqFlag == 1 ) //主动
        {
            //nxtFrmStep_p->reqAPDU_Obj_p   = (u_char*)apdu104_p;
            nxtFrmStep_p->reqPDU_FCode    = U_F;
        }
        else // (act_prio_chnlID == PASSIVE_TYPE) //被动
        {
            //nxtFrmStep_p->respAPDU_Obj_p = (u_char*)apdu104_p;
            nxtFrmStep_p->respPDU_FCode  = U_F;
        }
    }
    else//I帧
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

        //某步的所有点数
        //u_short pntNum  = ssnPar_p->pntNum;
        //某步的当前点位置
        //u_short pntPos  = ssnPar_p->pntPos;
        //一帧中最大的点个数:取ssnPar_p->acsMthd低7位的值
        //u_char  acsMthd = ( ssnPar_p->acsMthd ) & 0x7f;

        //可变结构体限定词
        ASDU_VSQ_VAL vsqVal = {};
        //取ssnPar_p->acsMthd最高位的值
        vsqVal.SQ = ( ssnPar_p->acsMthd ) >> 7;
        //取ssnPar_p->acsMthd低7位的值
        vsqVal.N = ( ssnPar_p->acsMthd & 0x7f );

        //找出vsqVal->N
        //u_short tmpNum = pntNum - pntPos;
        //if (acsMthd > 0 && tmpNum > 0)
        //{
        //    //找出要装的这一帧中实际的VSQ_N
        //    //（因为最后一帧或第一帧就有可能
        //    //小于每一帧可装的最大点数）
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

        //vsqVal.SQ 为1 时支持vsqVal.N大于0
        //vsqVal.SQ 为0 时只支持vsqVal.N等于1
        /*if( !((vsqVal.SQ == 0 && vsqVal.N ==1)
              ||(vsqVal.SQ == 1 && vsqVal.N >0)
             )
           )
        {
            return ERR_VSQN_SQ;
        }*/

        T_snnUsrData* tUsrData_p = NULL;
        tUsrData_p = & ( ssnPar_p->usrData_p[ssnPar_p->pntPos] );

        //根据实际的点值个数更新装帧对象中apdu描述信息之元素集个数
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

        //信息元素集中每个信息元素的字节长度
        u_int InfoEleLng = apdu_dscr_p->InfoEleLng;

        //限制 暂时不加此限制
        /*if(  InfoEleLng != tTidObjLng.cfgLng.tidQdsLng )
        {
            return ERR_CONFIG;
        }*/
        if ( InfoEleLng != tTidObjLng.cfgLng.tidQdsLng )
        {
            apdu_dscr_p->InfoEleLng = tTidObjLng.cfgLng.tidQdsLng;
            InfoEleLng = tTidObjLng.cfgLng.tidQdsLng;
        }

        //ASDU公共地址
        ASDU_ADDR_VAL* addrVal_p   = ( ASDU_ADDR_VAL* ) & ( ssnPar_p->cmnAddr );

        //取信息体地址
        /*u_char* bodAddVal_p = (u_char*)
            &(tUsrData_p->gData.val_chkStd.pntAddr);*/

        //拼接vsqVal.N个点值成为一块连续的元素集空间
        //pntVal1值的长度
        u_int pntVal1Lng = sizeof ( tUsrData_p->gData.val_chkStd.pntVal1 );
        //20170601 测试发现此处不需要限制而注释掉
        /*if (InfoEleLng < pntVal1Lng)
        {
        return ERR_INFOELELNG_CFG;
        }*/

        //u_char*    tspVal_p = NULL;//时标首地址指针
        u_char* tspVal_p[MAX104_APDU_LNG] = {0};
        //信息元素集临时变量空间
        u_char  bodSet_p[MAX104_APDU_LNG] = {0};
        u_char  bodAddVal_p[MAX104_APDU_LNG] = {0};
        u_char*    p = NULL;

        //单信息体对象结构的ASDU只有一个信息对象地址值
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

            //多信息对象结构的ASDU取信息对象地址
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
                //品质描述词与值混合在一起:如 带品质描述词的单点信息
                tPntVal1 = ( tPntVal1 | tQdsVal );
            }

            //单信息体对象结构的ASDU拷贝每一个点值到元素集空间
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
                    //配置的信息元素集中每个信息元素的字
                    //节长度 大于 2个 siseof(u_int)
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

                //品质描述词
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
                //多信息对象结构的ASDU拷贝每一个信息对象元素


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
                    //配置的信息元素集中每个信息元素的字节
                    //长度 大于 2个 siseof(u_int)
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

                //品质描述词
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

            //单个信息体对象结构的ASDU取时标（只存放在一帧中最
            //后一个点对应tUsrData_p->gData.tmStmp_p
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
                    return ERR_TIME_TYPE;//未知的时标类型
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
                //单个信息体对象结构的ASDU取时标每一个对象有一个时标
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
                    return ERR_TIME_TYPE;//未知的时标类型
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
            //元素集长度
            bodSetLng = p - bodSet_p;
        }
        else
        {
            //多对象结构中，一个对象只有一个信息元素
            bodSetLng = InfoEleLng;
        }

        //检查帧长度是否超过协议规定最大值
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

        //根据值组装ASDU
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

        //给出参下一帧对应步赋值
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
            //注：此值在上层中进行改变不在此改变
            //nxtFrmStep_p->reqPDU_Num     = nxtFrmStep_p->reqPDU_Num + 1;

        }
        else // (act_prio_chnlID == PASSIVE_TYPE) 
        {
            //nxtFrmStep_p->respAPDU_Obj_p = (u_char*)apdu104_p;
            nxtFrmStep_p->respPDU_FCode  = nxtFrmRlStep_p->respPDU_FCode;
            //注：此值在上层中进行改变不在此改变
            //nxtFrmStep_p->respPDU_Num    = nxtFrmStep_p->respPDU_Num + 1;
        }
    }//end I帧

    return 0;
}


//根据reqFlag标识与规则步比较请求或响应帧
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
    if ( reqFlag == 1 ) //比较请求帧
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
            //响应帧还未达到限定值
            if ( nowFrameNum_resp < allFrameNum_resp )
            {
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
    }
    else ////////////////////////////////////////////////////比较响应帧
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
            //请求帧还未达到限定值
            if ( nowFrameNum_req < allFrameNum_req )
            {
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
                        if ( nxtRuleStep_p->reqPDU_Num > 0 )
                        {
                            //下一步请求帧数据也大于0
                            //对于被动站来说则暂定规则为：先等待验
                            //  收请求帧再发送自己的响应帧
                            //发送等待验收  但需要跳步！！！！！
                            *nxtStepStat_p = MC_SEND_WAIT_CHECK;
                        }
                        else
                        {
                            //发送连续执行  但需要跳步！！！！！
                            *nxtStepStat_p = MC_SEND_CNUE_EXEC;
                        }
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
                        if ( nxtRuleStep_p->reqPDU_Num > 0 )
                        {
                            //下一步请求帧数据也大于0
                            //对于被动站来说则暂定规则为：
                            //  先等待验收请求帧再发送自己的响应帧
                            //不发送等待验收  但需要跳步！！！！！
                            *nxtStepStat_p = MC_NO_SEND_WAIT_CHECK;
                        }
                        else
                        {
                            //不发送连续执行  但需要跳步！！！！！
                            *nxtStepStat_p = MC_NO_SEND_CNUE_EXEC;
                        }
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
    }

    return 0;
}


/******************************************************************************
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

    char reqFlag = getSndReqFlag( funDsc_p );

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
    //        //主动
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
    //        else //如果是突发回话
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

    //    case PASSIVE_TYPE://被动
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
    //        else //如果是突发回话
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

    //获取规则步中当前步的指针;
    ONE_STEP* nowRuleStep_p = displayNextNode (
                              Fun_p->steps_p[Fun_p->usingMode],
                              stepRl_p );

    if ( nowRuleStep_p == NULL )
    {
        return ERR_POINTER_NULL;
    }

    //当前步指针
    const ONE_STEP* nowStep_p     = stepRl_p;

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

    T_stnAcsFlag tmpStnAcsFlag = {0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0};
    *stnAcsFlag_p = tmpStnAcsFlag;

    *nxtStepNo_p  = nowStep_p->stepNo;

    //主被动会话标识
    //u_char actFlag = ( u_char ) funDsc_p->act_prio_chnlID;

    char reqFlag = getRcvReqFlag( funDsc_p );

    switch ( reqFlag )
    {
        case 1://验收请求
        {
            /*if ( funDsc_p->ssnDscNo != ACQ_EVT )
            {*/
            if ( nowFrameNum_req == allFrameNum_req )
            {
                //请求帧达到限定值
                if ( nowFrameNum_resp == allFrameNum_resp )
                {
                    //响应帧也达到限定值
                    if ( nxtRuleStep_p != NULL )
                    {
                        //如果当前步不是最后一步
                        *nxtStepNo_p = nxtRuleStep_p->stepNo;
                        //下一步系统执行数据值的加载
                        stnAcsFlag_p->ctlType = 2;
                    }
                }
                else if ( nowFrameNum_resp > allFrameNum_resp )
                {
                    //响应帧错误
                    return ERR_FRAME_NO;
                }
            }
            else if ( nowFrameNum_req > allFrameNum_req )
            {
                //请求帧错误
                return ERR_FRAME_NO;
            }
            //}
            //else
            //{
            //    if ( nowFrameNum_resp == allFrameNum_resp )
            //    {
            //        //响应帧达到限定值
            //        if ( nowFrameNum_req == allFrameNum_req )
            //        {
            //            //请求帧也达到限定值
            //            if ( nxtRuleStep_p != NULL )
            //            {
            //                //如果当前步不是最后一步
            //                *nxtStepNo_p = nxtRuleStep_p->stepNo;
            //                //下一步系统执行数据值的加载
            //                stnAcsFlag_p->ctlType = 2;
            //            }
            //        }
            //        else if ( nowFrameNum_req > allFrameNum_req )
            //        {
            //            //请求帧错误
            //            return ERR_FRAME_NO;
            //        }
            //    }
            //}

            break;
        }

        case 0: //验收响应
        {
            //主动会话接收被动站数据
            /*if (funDsc_p->ssnDscNo!= ACQ_EVT)
            {*/
            if ( nowFrameNum_resp == allFrameNum_resp )
            {
                //响应帧达到限定值
                if ( nowFrameNum_req == allFrameNum_req )
                {
                    //请求帧也达到限定值
                    if ( nxtRuleStep_p != NULL )
                    {
                        //如果当前步不是最后一步
                        *nxtStepNo_p = nxtRuleStep_p->stepNo;
                        //下一步系统执行数据值的加载
                        stnAcsFlag_p->ctlType = 2;
                    }
                }
                else if ( nowFrameNum_req > allFrameNum_req )
                {
                    //请求帧错误
                    return ERR_FRAME_NO;
                }
            }
            else if ( nowFrameNum_resp > allFrameNum_resp )
            {
                //响应帧错误
                return ERR_FRAME_NO;
            }

            /*}
            else
            {
                if (nowFrameNum_resp == allFrameNum_resp)
                {//响应帧达到限定值
                    if (nowFrameNum_req == allFrameNum_req)
                    {//请求帧也达到限定值
                        if (nxtRuleStep_p != NULL)
                        {//如果当前步不是最后一步
                            *nxtStepNo_p = nxtRuleStep_p->stepNo;
                            //下一步系统执行数据值的加载
                            stnAcsFlag_p->ctlType=2;
                        }
                    }
                    else if (nowFrameNum_req > allFrameNum_req)
                    {//请求帧错误
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
 *    20180223151731因T_snnCfg结构体变化，此内部接口无实际用途，所以注释内部逻辑
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
  *功能：
  *   在nxtStep里U帧会话参数处理
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
            //不发送等待验收
            *nxtStepStat_p = MC_NO_SEND_WAIT_CHECK;
        }
        else//C_STEP_NO_TWO
        {
            if ( nowRespFrmNum < 1 )
            {
                //不发送等待验收
                *nxtStepStat_p = MC_NO_SEND_WAIT_CHECK;
            }
            else
            {
                //不发送会话完成
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
                //不发送等待验收
                *nxtStepStat_p = MC_NO_SEND_WAIT_CHECK;
            }
            else
            {
                //不发送连续执行  但需要跳步！！！！！
                *nxtStepStat_p = MC_NO_SEND_CNUE_EXEC;
                nxtFrmStepNo   = C_STEP_NO_TWO;
            }
        }
        else//C_STEP_NO_TWO
        {
            if ( nowRespFrmNum < 1 )
            {
                //发送会话完成
                *nxtStepStat_p = MC_SEND_SSN_FNSH;
            }
            else
            {
                //不发送会话完成
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
                //104协议U帧STARTDT 抽象的会话
                ssnPar_p->ASDUID_p[0] = MC_TID_136;
                break;

            case U_STOPDT:
                //104协议U帧STOPDT 抽象的会话
                ssnPar_p->ASDUID_p[0] = MC_TID_137;
                break;

            case U_TESTFR:
                //104协议U帧TESTFR 抽象的会话
                ssnPar_p->ASDUID_p[0] = MC_TID_138;
                break;

            case U_NEWHOLD:
                /*用U帧的STARTDT STOPDT TESTFR
                  全为0 表示出新的会话：新联一个通道
                */
                ssnPar_p->ASDUID_p[0] = MC_TID_139;
                break;

            case U_DELHOLD:
                /*用U帧的STARTDT STOPDT TESTFR 全
                 为1 表示出新的会话：关一个通道
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
  *功能：
  *    校验数据单元标识符
  *
  * 输入：
  *     nowRuleStep_p :当前步规则
  *     lastRuleStep_p:规则步的最后一步
  *     stepNo        :当前步号
  *     funDsc_p     :会话号与主动性
  *     apdu104_p    :需要验证数据的对应APDU104对象
  *输出：
  *     无
  *返回：
  *     0    成功
  *     <0   错误
  *修改历史
  *
*******************************************************************************/
int chkAsduDataID ( const ONE_STEP* nowRuleStep_p,
                    const ONE_STEP* lastRuleStep_p,
                    const u_char& stepNo,
                    const T_ptclFunDsc* funDsc_p,
                    APDU104* apdu104_p
                  )
{


    //主被动性会话标识
    //u_char act_prio_chnlID  = ( u_char ) funDsc_p->act_prio_chnlID;
    //模式码
    u_char usingMode        = funDsc_p->usingMode;

    //规则步中请求帧数量的限定值
    u_char allFrameNum_req  = nowRuleStep_p->reqPDU_Num;

    //当前请求帧值（是步的第几帧）
    //u_char nowFrameNum_req  = stepRl_p->reqPDU_Num;

    //规则步中响应帧数量的限定值
    u_char allFrameNum_resp = nowRuleStep_p->respPDU_Num;

    //当前响应帧值（是步的第几帧）
    //u_char nowFrameNum_resp = stepRl_p->respPDU_Num;

    

    //传送原因
    ASDU_COT1_VAL* COT_p = ( ( ASDU_COT1_VAL* )
                             apdu104_p->getASDU_COT_VAL (
                             &apdu104_p->getDU_Info()->ASDU_CMTBL[ASDU_COT_INDEX] ) );
    //类型标识
    ASDU_TID_VAL* TID_p = apdu104_p->getASDU_TID_VAL (
                          &apdu104_p->getDU_Info()->ASDU_CMTBL[ASDU_TID_INDEX] );
    u_short      tmpCOT = 0;  //传送原因
    u_char       tmpTID = 0;  //类型标识

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
        case STN_INIT: //站初始化;
        {
            break;
        }

        case QUE_DATA: //查询数据;
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
                //被控站会话验收主控站ASDU
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
                //主站接收的是被动站的数据
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
        }          //end 查询数据

        case CYC_TRNS: //周期传输
        {
            if ( stepNo != C_STEP_NO_ONE )
            {
                //只有一步、一帧;
                return ERR_STEP_NO;
            }

            //if ( act_prio_chnlID == PASSIVE_TYPE )
            if ( funDsc_p->chnlRole == SLAVE_STATION )
            {
                //被控站接收的是控制站的数据
                if ( allFrameNum_resp != 0 )
                {
                    return ERR_FRAME_NO;
                }

                //周期传输没有控制站到被控站的模式
                return ERR_ACT_PAS_VAL;
            }
            //else if ( act_prio_chnlID == ACTIVE_TYPE )
            else if ( funDsc_p->chnlRole == MASTER_STATION )
            {
                //控制站接收的是被控站的数据
                //被动会话验收请求帧数
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
        }         //end 周期传输

        case ACQ_EVT: //事件收集
        {
            if ( stepNo != C_STEP_NO_ONE )
            {
                //只有一步、一帧;
                return ERR_STEP_NO;
            }

            //if ( act_prio_chnlID == ACTIVE_TYPE )
            if ( funDsc_p->chnlRole == SLAVE_STATION  )
            {
                //事件收集 被控站没有验收的数据
                if ( allFrameNum_resp != 0 )
                {
                    return ERR_FRAME_NO;
                }

                //事件采集没有控制站到被控站的模式
                return ERR_ACT_PAS_VAL;
            }
            //else if ( act_prio_chnlID == PASSIVE_TYPE )
            else if ( funDsc_p->chnlRole == MASTER_STATION )
            {
                //控制站接收的是被控站的数据，被动会话验收请求帧数
                // fusk@20180517 add 但事件采集为被动会话,
                //      修改allFrameNum_resp -> allFrameNum_req
                //fusk@20180626 add 但事件采集为被动会话,
                //      修改allFrameNum_req -> allFrameNum_resp
                //fusk@20180626 add 但事件采集为被动会话,
                //      修改allFrameNum_resp -> allFrameNum_req      
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
        }            //end 事件采集

        case QUKCHK_EVT: //快检事件
        {
            //不使用
            return 0xffff;
            break;
        }

        case STN_INRGTN: //站召唤
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
                //被控站的验收主控站的数据
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
                //主控站验收被控站的数据

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

            }//end 主动站验收

            break;
        }//end 站召唤

        case CLK_SYN: //时钟同步
        {
            if ( funDsc_p->chnlRole == SLAVE_STATION ) //被控站的验收主控站的数据
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
            else if ( funDsc_p->chnlRole == MASTER_STATION ) //主控站验收被控站的数据
            {
                if ( allFrameNum_resp != 1 )
                {
                    return ERR_FRAME_NO;
                }

                if ( tmpCOT != MC_COT_7
                     || tmpCOT != tRemoteSet_p->COT
                   )   //传送原因
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

        case CMD_TRNS: //命令传输
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

            if ( funDsc_p->chnlRole == SLAVE_STATION ) //被控站的验收主控站的数据
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
            else if ( funDsc_p->chnlRole == MASTER_STATION ) //主控站验收被控站的数据
            {
                //tid校验

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

                //cot校验
                if ( ( tmpTID >= MC_TID_45 && tmpTID <= MC_TID_51 )
                     || ( tmpTID >= MC_TID_58 && tmpTID <= MC_TID_64 ) )
                {
                    if ( stepNo < lastRuleStep_p->stepNo ) //不是最后一步
                    {
                        if ( ( tmpCOT != MC_COT_7 && tmpCOT != MC_COT_9 )
                             || tmpCOT != tRemoteSet_p->COT
                           )
                        {
                            return ERR_COT_VAL;
                        }
                    }
                    else if ( stepNo == lastRuleStep_p->stepNo ) //是最后一步
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

            }//end 主动站验收

            //待实现
            break;
        }

        case ACMU_FRZ:
        {
            /*累积量传输前的冻结
              此功能是从ACMU_TRNS功能的模式2，
              控制站先冻结再请求传输分离出来的
             */
            if ( funDsc_p->chnlRole == SLAVE_STATION ) //被动站接收的是主动站的数据
            {
                if ( tmpCOT != MC_COT_6
                     || tmpCOT != tRemoteSet_p->COT
                   )
                {
                    return ERR_COT_VAL;
                }
            }
            else if ( funDsc_p->chnlRole == MASTER_STATION ) //主站接收的是被动站的数据
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

        case ACMU_TRNS: //累积量传输
        {
            switch ( usingMode )
            {
                case CTLD_DRCTSONPT_TRNS://模式0，被控站直接突发传输
                {
                    if ( stepNo != C_STEP_NO_ONE )  //只有一步、一帧;
                    {
                        return ERR_STEP_NO;
                    }

                    if ( funDsc_p->chnlRole == SLAVE_STATION ) //被动站
                    {
                        if ( allFrameNum_resp != 0 )
                        {
                            return ERR_FRAME_NO;
                        }

                        return ERR_ACT_PAS_VAL;//突发没有控制站到被控站的模式
                    }
                    else if ( funDsc_p->chnlRole == MASTER_STATION )  //控制站
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

                case CTL_DRCTREQ_TRNS://模式1，控制站直接请求传输
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
                        //被动站接收的是主动站的数据
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
                        //控制站接收的是被动站的数据
                        //tid校验
                        if ( ! ( ( tmpTID == MC_TID_101
                                   || tmpTID == MC_TID_15
                                   || tmpTID == MC_TID_37
                                 ) && vFlag == 1
                               )
                           )
                        {
                            return ERR_TID_VAL;
                        }

                        //cot校验
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
                                //M_IT传输不应是最后一步
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

                case CTL_FRZ_CTLD_SONPT_TRNS://模式3，控制站冻结后被控站突发传输
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
                        //被动站接收的是主动站的数据
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
                        //控制站接收的是被动站的数据
                        //tid校验
                        if ( ! ( ( tmpTID == MC_TID_101
                                   || tmpTID == MC_TID_15
                                   || tmpTID == MC_TID_37
                                 ) && vFlag == 1
                               )
                           )
                        {
                            return ERR_TID_VAL;
                        }

                        //cot校验
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
                }//end 模式3，控制站冻结后被控站突发传输

                default:
                    break;
            }//end switch (usingMode)

            break;
        }

        case PARA_LD: //参数加载
        {
            switch ( usingMode )
            {
                case ONLY_LD://只加载，模式0
                {
                    if ( funDsc_p->chnlRole == SLAVE_STATION ) //被控站会话
                    {
                        if ( tmpCOT != MC_COT_6
                             || tmpCOT != tRemoteSet_p->COT
                           )
                        {
                            return ERR_COT_VAL;
                        }

                    }
                    else if ( funDsc_p->chnlRole == MASTER_STATION ) //主控站会话
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

                case ONLYACT_LD://只激活，模式1
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

                    if ( funDsc_p->chnlRole == SLAVE_STATION ) //被控站会话
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
                    else if ( funDsc_p->chnlRole == MASTER_STATION ) //主控站会话
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
                    }//end 主控站会话

                    break;
                }

                case AUTOACT_LD://自动激活，模式2
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

                    if ( funDsc_p->chnlRole == SLAVE_STATION ) //被控站会话
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
                    else if ( funDsc_p->chnlRole == MASTER_STATION ) //主控站会话
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
                    }//end 主控站会话

                    break;
                }

                default:
                    break;
            }

            break;
        }

        case TST_PRCD: //测试过程
        {
            if ( funDsc_p->chnlRole == SLAVE_STATION ) //被控站接收的是控制站的数据
            {
                if ( tmpCOT != MC_COT_6
                     || tmpCOT != tRemoteSet_p->COT
                   )   //传送原因
                {
                    return ERR_COT_VAL;
                }
            }
            else if ( funDsc_p->chnlRole == MASTER_STATION ) //控制站接收的是被控站的数据
            {
                if ( tmpCOT != MC_COT_7
                     || tmpCOT != tRemoteSet_p->COT
                   )   //传送原因
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

        case FILE_TRNS: //文件传输
        {
            //暂不实现
            return 0xffff;
            break;
        }

        case ACQ_TRNSDLY: //收集传输延迟值
        {
            //不使用
            return 0xffff;
            break;
        }

        default:
            break;
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
    fun_p->cmLvl[usingMode]    = APP_LVL; // 104规约取APP_LVL
    fun_p->stepNum[usingMode]  = stepNum;

    //2、创建步规则
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

    //3、把步规则挂到Fun_p上
    fun_p->steps_p[usingMode] = tmpHead_p;

    return;
}


/*******************************************************************************
  *功能：(此接口暂时未用到)
  *    根据tid的值相应配置的长度（根据规约规定得到）
  *
  *
  * 输入：
  *     TID      ：tid的值
  *     flag
  *        0:tid值的长度（以字节为单位）
  *        1:tid值的长度（为位为单位）
  *        2:品质描述的长度（以字节为单位）
  *        3:时标的长度（以字节为单位）
  *        4:判断值与QDS是否参杂在一起的:即QDS不单独占字节
  *        5:tid值+QDS的长度
  *输出：
  *     无
  *
  *返回：
  *     tid所配置相应值的长度
  *修改历史
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

    if ( flag == 0 ) /*tid值的长度*/
    {
        if ( g_TID104[TID].tidBitLng == 0 )
        {
            tRet = 0;
        }
        //else if ( ( g_TID104[TID].tidQdsLng - g_TID104[TID].qdsLng ) == 0 )
        //{
        //    /*值与QDS共用一个字节，如：带品质描述词的单点信息*/
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
    else if ( flag == 1 ) /*tid值的bit长度*/
    {
        tRet = g_TID104[TID].tidBitLng;
    }
    else if ( flag == 2 ) /*QDS的长度*/
    {
        tRet = g_TID104[TID].qdsLng;
    }
    else if ( flag == 3 ) /*时标的长度*/
    {
        tRet = g_TID104[TID].tmStmpLng;
    }
    else if ( flag == 4 ) /*判断值与QDS是否参杂在一起的:即QDS不单独占字节*/
    {
        if ( g_TID104[TID].tidQdsLng == g_TID104[TID].qdsLng
             && g_TID104[TID].tidBitLng > 0
           )
        {
            tRet = 1;/*参杂在一起的*/
        }
        else
        {
            tRet = 0;/*没参杂*/
        }
    }
    else if ( flag == 5 ) /*tid值+QDS的长度*/
    {
        tRet = g_TID104[TID].tidQdsLng;
    }

    return tRet;
}

/*******************************************************************************
  *功能：
  *    根据tid的值相应配置的长度（根据规约规定得到）
  *
  *
  * 输入：
  *     TID      ：tid的值
  *
  *输出：
  *     tidObjLng_p tid相关的长度
  *
  *返回：
  *     0   成功
  *     非0 失败
  *修改历史
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

    /*tid值的字节长度*/
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

    /*判断值与QDS是否参杂在一起的:即QDS不单独占字节*/
    if ( g_TID104[TID].tidQdsLng == g_TID104[TID].qdsLng
         && g_TID104[TID].tidBitLng > 0
       )
    {
        /*参杂在一起的*/
        tidObjLng_p->qdsMixFlag = 1;
    }
    else
    {
        /*没参杂*/
        tidObjLng_p->qdsMixFlag = 0;
    }

    /*tid值的bit长度*/
    tidObjLng_p->cfgLng.tidBitLng = g_TID104[TID].tidBitLng;
    /*QDS的长度*/
    tidObjLng_p->cfgLng.qdsLng = g_TID104[TID].qdsLng;
    /*时标的长度*/
    tidObjLng_p->cfgLng.tmStmpLng = g_TID104[TID].tmStmpLng;
    /*tid值+QDS的长度*/
    tidObjLng_p->cfgLng.tidQdsLng = g_TID104[TID].tidQdsLng;

    return 0;
}


/*******************************************************************************
  *功能：
  *    取数据中的某些位作为值输出
  *
  *
  * 输入：
  *     val_p      :输入输出参数,需要取值的地址
  *     sizeV      :val_p中数据类型的大小
  *     bgBit      :取值的开始位(从1开始)
  *     edBit      :取值的结束位
  *
  *输出：
  *     val_p
  *
  *返回：
  *     0   成功
  *     非0 失败
  *注意:
  *    1.val_p的数据类型只支持 u_char,u_short,u_int
  *    1.函数成功后将覆盖原来的输入值val_p
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
  *功能：
  *    帧数据给T_snnPara赋值
  *
  *
  * 输入：
  *     tTidObjLng     :tid对应协议配置长度
  *     usrData_index  :此帧数据在步中开始点
  *     tUsrData_p     :点的存取数据
  *     apdu104_p      :APDU104对应
  *     stnAcsFlag     :数据存取控制结构
  *
  *输出：
  *     tUsrData_p     :点的存取数据
  *     apdu104_p      :APDU104对应
  *     stnAcsFlag     :数据存取控制结构
  *
  *返回：
  *     0   成功
  *     非0 失败
  *注意:
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
    u_short tmpBodSetLng = 0;   //元素集长度
    u_char* ssnBodSet_p  = NULL;//元素集首地址
    u_short tmpBodSetNum = 0;   //元素个数

    APDU_DSCR* tApduDscr_p   = apdu104_p->getDU_Info();

    if ( tApduDscr_p == NULL )
    {
        return ERR_POINTER_NULL;
    }

    //可变结构体限定词
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

    //信息体地址
    BODY_ADDR_VAL* ssnBodAddr_p = NULL;
    u_short tOneBdAddLng        = tApduDscr_p->ASDU_CMTBL[BODY_ADDR_INDEX];

    if ( tOneBdAddLng > 0 )
    {
        ssnBodAddr_p = apdu104_p->getBODY_ADDR_VAL (
                       & ( tOneBdAddLng )
                       );
    }

    //将信息体地址存入当前帧首个点值对应的点地址中
    if ( ssnBodAddr_p != NULL )
    {
        memcpy ( & ( tUsrData_p[usrData_index].gData.val_chkStd.pntAddr ),
                 ssnBodAddr_p,
                 tOneBdAddLng
               );
    }

    //每个元素的长度
    //u_int tmpOneLng = tApduDscr_p->InfoEleLng;
    u_int tmpPntLng = sizeof (
                      tUsrData_p[usrData_index].gData.val_chkStd.pntVal1 );

    //if ( tmpOneLng > tmpPntLng * 2 )
    if ( tTidObjLng.cfgLng.tidQdsLng > tmpPntLng * 2 )
    {
        return ERR_INFOELELNG_CFG;
    }

    //根据tid判断是否有时标
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


    tmpBodSetNum = ssnVSQ_p->N;      //元素个数

    if ( tTidObjLng.cfgLng.tidQdsLng > 0 )
    {
        //点值或品质描述
        ssnBodSet_p  = apdu104_p->getAPDU_ELEMENT (
                       &tmpBodSetLng,
                       BODY_SET_INDEX ); //元素集首地址
    }

    for ( u_short i = usrData_index;
          i < usrData_index + tmpBodSetNum;
          i++
        ) //对每个点地址和点值
    {
        //点地址
        if ( ssnVSQ_p->SQ == 1
             && ssnBodAddr_p != NULL
             && i > usrData_index )
        {
            /*SQ:=1 单个信息元素的序列由信息对象地址来寻址：信息
            对象地址是顺序单个元素的第一个信息元素地址，后继单个
            信息元素是从这个地址顺序加1*/
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
            //点值或品质描述
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

            if ( ssnVSQ_p->SQ == 1 ) //单对象结构
            {
                addLng = i * tTidObjLng.cfgLng.tidQdsLng;
            }
            else//多对象结构
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
                //品质描述与值在一起时需要分离出值
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
                //品质描述与值分离的
                memcpy ( & ( tUsrData_p[i].gData.dDsc ),
                         ssnBodSet_p + addLng + tTidObjLng.tidValLng,
                         tTidObjLng.cfgLng.qdsLng
                       );
            }

            tUsrData_p[i].gData.val_chkStd.pntVal1 = tPntVal1;

            if ( tTidObjLng.tidValLng > 0 )
            {
                stnAcsFlag->ctlType = ( stnAcsFlag->ctlType | 0x01 ); //存储数据
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
            //如果有时标，则把时标赋予当前帧最后一个点对应的usrData_p结构中
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
  *功能：
  *    接收侧取得标识reqFlag（是否处理请求或响应）
  *
  *
  * 输入：
  *     funDsc_p     :T_ptclFunDsc结构变量
  *
  *输出：
  *     无
  *
  *返回：
  *     0   请求
  *     1   响应
  *     非0 失败
  *注意:
  *
*******************************************************************************/
inline char getRcvReqFlag ( const T_ptclFunDsc* funDsc_p )
{
    char reqFlag = -1;

    u_char actFlag = ( u_char ) funDsc_p->act_prio_chnlID;

    if ( funDsc_p->chnlRole == MASTER_STATION )//主控站的接收
    {
        if ( actFlag == PASSIVE_TYPE )
        {
            reqFlag = 1; //接收侧处理请求
        }
        else //ACTIVE_TYPE
        {
            reqFlag = 0; //响应
        }
    }
    else //SLAVE_STATION 被控站的接收
    {
        if ( actFlag == PASSIVE_TYPE )
        {
            reqFlag = 1; //请求
        }
        else //ACTIVE_TYPE
        {
            reqFlag = 0; //响应
        }
    }

    return reqFlag;
}

/*******************************************************************************
  *功能：
  *    发送侧取得标识reqFlag（是否处理请求或响应）
  *
  *
  * 输入：
  *     funDsc_p     :T_ptclFunDsc结构变量
  *
  *输出：
  *     无
  *
  *返回：
  *     0   请求
  *     1   响应
  *     非0 失败
  *注意:
  *
*******************************************************************************/
inline char getSndReqFlag ( const T_ptclFunDsc* funDsc_p )
{
    char reqFlag = -1;

    u_char actFlag = ( u_char ) funDsc_p->act_prio_chnlID;

    if ( funDsc_p->chnlRole == MASTER_STATION )//主控站的发送
    {
        if ( actFlag == PASSIVE_TYPE )
        {
            //逻辑错误
            reqFlag = 0; 
        }
        else //ACTIVE_TYPE
        {
            reqFlag = 1; //请求
        }
    }
    else //SLAVE_STATION 被控站的发送
    {
        if ( actFlag == PASSIVE_TYPE )
        {
            reqFlag = 0; //响应
        }
        else //ACTIVE_TYPE
        {
            reqFlag = 1; //请求
        }
    }

    return reqFlag;
}

