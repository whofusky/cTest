/******************************************************************************
** 文件名：unitInfMem.cpp
** 作者：
** 创建时间：2018-04-21
** 功能：
** 修改：
** 其它：
*******************************************************************************/

#include "unitInfMem.h"
#include <stdlib.h>
//#include "DataType.h"



//物理量是否有效
inline bool isPhyTypeAvailable ( const PHY_UNIT_TYPE& val,
                                 const u_int& maxVal
                               )
{
    if ( val ==  0
         || val > maxVal
       )
    {
        //物理量的类型错误
        return false;
    }
    else
    {
        return true;
    }
}




C_chnUnitData::C_chnUnitData()
{
    m_chnNum         = 0;
    m_pntAddrNum     = 0;
    m_maxPhyType     = 0;
    m_phyUnitNum     = 0;
    m_pntObjLng      = 0;
    m_dataIdLng      = 0;
    m_tmStmpLng      = 0;
    m_numPhyCat      = 0;

    //m_pntStrgObj     = NULL;
    m_phyToPnt_p     = NULL;
    m_phyCat_p       = NULL;
    memset ( &m_pntStrgObj, 0, sizeof ( m_pntStrgObj ) );
    memset ( &m_untChnTmStmp, 0, sizeof ( m_untChnTmStmp ) );

}



C_chnUnitData::~C_chnUnitData()
{
    freeM();
}



//初始化点的分配
int C_chnUnitData::initPntStrgObj ( const T_pntAddrCfg* inPar_p,
                                    const u_int&  pntObjLng,
                                    const u_int&  pntAddrNum,
                                    const u_char& tmStmpLng
                                  )
{
    if ( inPar_p == NULL
         || inPar_p->num == 0 /*包括不连接地址段的段数*/
       )
    {
        return MC_ERROR_INPAR;
    }

    const T_pntAddrCfg* tInAddrCfg_p = inPar_p;

    if ( pntObjLng == 0 )
    {
        m_pntObjLng = sizeof ( COM_OBJ_TYPE );
    }
    else
    {
        m_pntObjLng = pntObjLng;
    }

    m_tmStmpLng    = tmStmpLng;  //时标长度

    //计算每个点地址段加起来的总数
    u_int tNum = 0;

    for ( u_int i = 0; i < tInAddrCfg_p->num; i++ )
    {
        T_contPntAdd* tConAdd_p = & (
                                  tInAddrCfg_p->contPntAdd_p[i] );

        if ( tConAdd_p->endAddrVal < tConAdd_p->beginAddrVal )
        {
            return -2;
        }

        u_int tDiff = ( tConAdd_p->endAddrVal - tConAdd_p->beginAddrVal + 1 );
        tNum = tNum + tDiff;
    }

    //点总数取最大者
    if ( tNum > pntAddrNum )
    {
        m_pntAddrNum = tNum;
    }
    else
    {
        m_pntAddrNum = pntAddrNum;
    }

    //分配点地址对应对象空间
    m_pntStrgObj.pntMemObj_p = ( T_pntMemObj* ) calloc ( m_pntAddrNum,
                               sizeof ( T_pntMemObj ) );

    if ( m_pntStrgObj.pntMemObj_p == NULL )
    {
        return MC_ERROR_CALLOC;
    }

    //分配协议对象的时标空间
    if ( m_tmStmpLng > 0 )
    {
        m_pntStrgObj.tmStmp_p = calloc ( m_pntAddrNum,
                                         m_tmStmpLng );

        if ( m_pntStrgObj.tmStmp_p == NULL )
        {
            return MC_ERROR_CALLOC;
        }
    }

    m_pntStrgObj.pntAddrCtl.num = tInAddrCfg_p->num;
    m_pntStrgObj.pntAddrCtl.conAddDex_p = ( T_contAddDex* ) calloc (
                                          tInAddrCfg_p->num,
                                          sizeof ( T_contAddDex ) );

    if ( m_pntStrgObj.pntAddrCtl.conAddDex_p == NULL )
    {
        return MC_ERROR_CALLOC;
    }

    u_int pntMemDx = 0;
    u_int tmpMemDx = 0;

    //给点地址控制段连续地址的每段开始,结束点地址赋值
    //并记录每段开始地址的索引号
    for ( u_int i = 0; i < tInAddrCfg_p->num; i++ )
    {
        T_contPntAdd* tInConAdd_p = & (
                                    tInAddrCfg_p->contPntAdd_p[i] );
        T_contAddDex* tContAddDex_p = & (
                                      m_pntStrgObj.pntAddrCtl.conAddDex_p[i] );
        tContAddDex_p->contPntAdd.beginAddrVal = tInConAdd_p->beginAddrVal;
        tContAddDex_p->contPntAdd.endAddrVal   = tInConAdd_p->endAddrVal;
        tContAddDex_p->begineIndex = pntMemDx;

        u_int tDiff = tInConAdd_p->endAddrVal - tInConAdd_p->beginAddrVal;
        tmpMemDx = pntMemDx + tDiff + 1;

        //给通用对象的点地址赋值
        for ( u_int j = pntMemDx; j < tmpMemDx; j++ )
        {
            COM_OBJ_TYPE* relateObj_p = &(
                m_pntStrgObj.pntMemObj_p[j].relateObj );
            relateObj_p->gData.val_chkStd.pntAddr = (
                tInConAdd_p->beginAddrVal + ( j - pntMemDx ) );
        }   

        pntMemDx = tmpMemDx;
        //for ( u_int j = tInConAdd_p->beginAddrVal;
        //      j <= tInConAdd_p->endAddrVal;
        //      j++
        //    )
        //{
        //    //m_pntStrgObj.pntMemObj_p[pntMemDx].pntAddr = j;
        //    pntMemDx++;
        //}

    }

    return  0;
}


//通过点地址找点地址有整个结构中的索引号
int C_chnUnitData::findPntIndx ( const PNT_ADDR_TYPE& value,
                                 u_int& retIndx
                               ) const
{
    u_char flag = 0;

    /*找点地址在已有点地址数组中的下标
    m_pntStrgObj.pntAddrCtl.num 代表有多少段连续的地址段
    */
    for ( u_int k = 0;
          k < m_pntStrgObj.pntAddrCtl.num;
          k++
        )
    {
        T_contAddDex* tConAddDex_p = & (
                                     m_pntStrgObj.pntAddrCtl.conAddDex_p[k]
                                     );
        T_contPntAdd* tConPntAdd_p = & (
                                     tConAddDex_p->contPntAdd
                                     );

        if ( value >= tConPntAdd_p->beginAddrVal
             && value <= tConPntAdd_p->endAddrVal
           )
        {
            retIndx = ( tConAddDex_p->begineIndex
                        + ( value - tConPntAdd_p->beginAddrVal )
                      );
            //m_pntStrgObj.pntMemObj_p[tInDx].pntUType = tInPhyCfgI_p->phyType;
            flag = 1;
            break;
        }
    }

    if ( flag == 1 )
    {
        return 0;
    }
    else
    {
        return -1;
    }

}



int C_chnUnitData::initPhyToPnt ( const T_phyValCfg*  inPar_p,
                                  const  u_int&       phyUnitNum,
                                  const  u_int&       dataIdLng
                                )
{
    if ( inPar_p == NULL
         || phyUnitNum == 0
         || m_maxPhyType == 0
       )
    {
        return MC_ERROR_INPAR;
    }

    int tRet = 0;

    const T_phyValCfg* tInPhyCfg_p = inPar_p;
    m_phyUnitNum   = phyUnitNum;//物理量个数
    m_dataIdLng    = dataIdLng; //数据id长度


    //物理量个数是一个全集,具体用到哪个物理量,其物理量类型值为全集数组的下标索引
    m_phyToPnt_p = ( T_phyValStrg* ) calloc ( m_maxPhyType,
                   sizeof ( T_phyValStrg )
                                            );

    if ( m_phyToPnt_p == NULL )
    {
        return MC_ERROR_CALLOC;
    }

    //给每一个物理量赋值
    for ( u_int i = 0; i < m_phyUnitNum; i++ )
    {
        const T_phyValCfg* tInPhyCfgI_p = & ( tInPhyCfg_p[i] );

        if ( ! ( isPhyTypeAvailable ( tInPhyCfgI_p->phyType, m_maxPhyType )
               )
           )
        {
            //物理量的类型错误
            return MC_ERROR_PHYTY;
        }

        T_phyValStrg* tPhyToPnt_p   = & (
                                      m_phyToPnt_p[tInPhyCfgI_p->phyType - 1 ]
                                      );
        tPhyToPnt_p->calcMethd     = tInPhyCfgI_p->calcMethd;
        tPhyToPnt_p->phyValLng     = tInPhyCfgI_p->phyValLng;
        //tPhyToPnt_p->phyCatName    = tInPhyCfgI_p->phyCatName;
        tPhyToPnt_p->acmTimes      = 0;
        tPhyToPnt_p->freshFlag     = false;

        if ( tInPhyCfgI_p->multFactor == 0 )
        {
            tPhyToPnt_p->multFactor = 1;//默认为1
        }
        else
        {
            tPhyToPnt_p->multFactor = tInPhyCfgI_p->multFactor;
        }

        //分配存储物理量值的空间
        if ( tPhyToPnt_p->phyValLng > 0 )
        {
            tPhyToPnt_p->phyObjVal_p  = calloc (
                                        1,
                                        tPhyToPnt_p->phyValLng
                                        );

            if ( tPhyToPnt_p->phyObjVal_p == NULL )
            {
                //分配存储物理量值的空间 错误
                return MC_ERROR_CALLOC;
            }
        }
        else
        {
            tPhyToPnt_p->phyObjVal_p = NULL;
        }

        //分配存储数据id的空间
        if ( m_dataIdLng > 0 )
        {
            tPhyToPnt_p->dataId_p  = calloc (
                                     1,
                                     m_dataIdLng
                                     );

            if ( tPhyToPnt_p->dataId_p == NULL )
            {
                //分配存储数据id的空间 错误
                return MC_ERROR_CALLOC;
            }

            //copy数据id的值
            memcpy ( tPhyToPnt_p->dataId_p,
                     tInPhyCfgI_p->dataId_p,
                     m_dataIdLng
                   );
        }
        else
        {
            tPhyToPnt_p->dataId_p = NULL;
        }

        tPhyToPnt_p->phyType       = tInPhyCfgI_p->phyType;
        tPhyToPnt_p->numOfAdd      = tInPhyCfgI_p->numOfAdd;
        //分配物理量包括点地址的空间
        tPhyToPnt_p->pntAddr_p    = (
                                    ( PNT_ADDR_TYPE* ) calloc (
                                    tInPhyCfgI_p->numOfAdd,
                                    sizeof ( PNT_ADDR_TYPE )
                                    )
                                    );

        if ( tPhyToPnt_p->pntAddr_p == NULL )
        {
            return MC_ERROR_CALLOC;
        }

        //给点地址赋值
        /*memcpy ( tPhyToPnt_p->pntAddr_p,
        tInPhyToPnt_p->pntAddr_p,
        tInPhyToPnt_p->numOfAdd * sizeof ( PNT_ADDR_TYPE )
        );*/

        //分配物理量包括点地址在总的点址址表的索引空间
        /*tPhyToPnt_p->addIndex_p = ( u_int* ) calloc (
        tInPhyToPnt_p->numOfAdd,
        sizeof ( u_int )
        );

        if ( tPhyToPnt_p->addIndex_p == NULL )
        {
        tRet = MC_ERROR_CALLOC;
        goto end_initM;
        }*/

        //赋值点地址并对点地址对应的多个物理量进行关联(形成指针环)处理
        for ( u_int j = 0; j < tInPhyCfgI_p->numOfAdd; j++ )
        {
            tPhyToPnt_p->pntAddr_p[j] = tInPhyCfgI_p->pntAddr_p[j];

            //u_char flag = 0;

            //建立点地址对应物理量的索引
            u_int tInDx = 0;
            tRet = findPntIndx ( tPhyToPnt_p->pntAddr_p[j], tInDx );

            if ( tRet != 0 )
            {
                //物理量包含的点值在总的点表中没有,返回错误
                return MC_ERROR_PNTAD;
            }

            //当前点地址还未对应任何物理量
            if ( m_pntStrgObj.pntMemObj_p[tInDx].pntUType == 0 )
            {
                m_pntStrgObj.pntMemObj_p[tInDx].pntUType = tInPhyCfgI_p->phyType;
            }

            //当前点地址对应的首个物理量类型
            PHY_UNIT_TYPE tPhyType = m_pntStrgObj.pntMemObj_p[tInDx].pntUType;

            T_phyValStrg* tPhyValStrgHead_p = & ( m_phyToPnt_p[tPhyType - 1 ] );
            T_phyValStrg* tPhyValStrg_p = & ( m_phyToPnt_p[tPhyType - 1 ] );

            if ( tPhyValStrg_p->next_p == NULL )
            {
                //如果下一指针为空则将下一指针指向自己
                tPhyValStrg_p->next_p = tPhyValStrg_p;
            }

            //找到需要插入的环位置
            while ( tPhyValStrg_p->next_p != tPhyValStrgHead_p
                    && tPhyType != tInPhyCfgI_p->phyType
                  )
            {
                tPhyValStrg_p = tPhyValStrg_p->next_p;
                tPhyType      = tPhyValStrg_p->phyType;
            }

            if ( tPhyType == tInPhyCfgI_p->phyType )
            {
                /*tInPhyCfgI_p->phyType此物理量的已经在同地址指针环中,
                不需要再处理*/
                //break;
            }
            else
            {
                /*tInPhyCfgI_p->phyType此物理量需要添加到同地址指针环中,
                */
                tPhyValStrg_p->next_p = tPhyToPnt_p;
                tPhyToPnt_p->next_p   = tPhyValStrgHead_p;
            }

        }

    }//end 给每一个物理量赋值

    return 0;
}


//初始化物理累加量的索引结构
int C_chnUnitData::initPhyCat ( const u_int& numPhyCat,
                                const T_phyCat*     phyCat_p
                              )
{
    if ( numPhyCat == 0
         || phyCat_p == NULL
       )
    {
        return MC_ERROR_INPAR;
    }

    m_numPhyCat = numPhyCat;
    m_phyCat_p = ( T_phyCat* ) calloc ( numPhyCat, sizeof ( T_phyCat ) );

    if ( m_phyCat_p == NULL )
    {
        return MC_ERROR_CALLOC;
    }

    for ( u_int i = 0; i < numPhyCat; i++ )
    {
        m_phyCat_p[i].categoryName = phyCat_p[i].categoryName;
        //m_phyCat_p[i].maxNum       = phyCat_p[i].curNum;
        m_phyCat_p[i].curNum       = phyCat_p[i].curNum;
        m_phyCat_p[i].head_p = ( PHY_UNIT_TYPE* ) calloc (
                               phyCat_p[i].curNum,
                               sizeof ( PHY_UNIT_TYPE )
                               );

        if ( m_phyCat_p[i].head_p == NULL )
        {
            return MC_ERROR_CALLOC;
        }

        for ( u_int j = 0; j < phyCat_p[i].curNum; j++ )
        {
            m_phyCat_p[i].head_p[j] = phyCat_p[i].head_p[j];
        }
    }

    return 0;
}



//初始化
int C_chnUnitData::initM ( const T_initChnUnit* inPar_p )
{
    if ( inPar_p == NULL
         /*|| inPar_p->pntAddrCfg == NULL*/
         || inPar_p->phyObjVal_p == NULL
         /*|| inPar_p->pntAddrNum == 0*/
         || inPar_p->phyUnitNum == 0
         /*|| inPar_p->pntObjLng == 0*/
         || inPar_p->pntAddrCfg.num == 0
       )
    {
        return MC_ERROR_INPAR;
    }

    const T_initChnUnit* tInPar_p = inPar_p;
    m_chnNum       = tInPar_p->chnNum;    //通道号
    m_maxPhyType   = tInPar_p->maxPhyType;//一个通道中最大的物理量类型值

    //m_pntAddrNum   = tInPar_p->pntAddrNum;//点地址数量
    //m_phyUnitNum   = tInPar_p->phyUnitNum;//物理量个数
    //m_pntObjLng    = tInPar_p->pntObjLng; //点地址关联通用数据对象的长度
    //m_dataIdLng    = tInPar_p->dataIdLng; //数据id长度
    //m_tmStmpLng    = inPar_p->tmStmpLng;  //时标长度

    m_valLock.Init();
    m_tmStmpLock.Init();

    int tRet = 0;

    //初始化点的设置
    tRet = initPntStrgObj ( & ( tInPar_p->pntAddrCfg ),
                            tInPar_p->pntObjLng,
                            tInPar_p->pntAddrNum,
                            tInPar_p->tmStmpLng
                          );

    if ( tRet != 0 )
    {
        goto end_initM;
    }


    //初始化物理量与点地址的对应关系
    tRet = initPhyToPnt ( tInPar_p->phyObjVal_p,
                          tInPar_p->phyUnitNum,
                          tInPar_p->dataIdLng
                        );

    if ( tRet != 0 )
    {
        goto end_initM;
    }

    //初始化物理累加量的索引结构
    tRet = initPhyCat ( tInPar_p->numPhyCat,
                        tInPar_p->phyCat_p
                      );

    if ( tRet != 0 )
    {
        goto end_initM;
    }

    //初始化通道时标
    if ( tInPar_p->untChnTmStmp.tmStmpLng > 0 )
    {
        m_untChnTmStmp.tmStmpLng = tInPar_p->untChnTmStmp.tmStmpLng;
        m_untChnTmStmp.tmStmp_p = calloc ( 1,
                                           m_untChnTmStmp.tmStmpLng
                                         );

        if ( m_untChnTmStmp.tmStmp_p == NULL )
        {
            tRet = MC_ERROR_CALLOC;
            goto end_initM;
        }

        if ( m_dataIdLng > 0 )
        {
            m_untChnTmStmp.dataId_p = calloc ( 1,
                                               m_dataIdLng
                                             );

            if ( m_untChnTmStmp.dataId_p == NULL )
            {
                tRet = MC_ERROR_CALLOC;
                goto end_initM;
            }

            //copy数据id的值
            if ( tInPar_p->untChnTmStmp.dataId_p != NULL )
            {
                memcpy ( m_untChnTmStmp.dataId_p,
                         tInPar_p->untChnTmStmp.dataId_p,
                         m_dataIdLng
                       );
            }
        }

    }
    else
    {
        m_untChnTmStmp.tmStmpLng = 0;
        m_untChnTmStmp.tmStmp_p  = NULL;
        m_untChnTmStmp.dataId_p  = NULL;
    }

end_initM:

    if ( tRet != 0 ) //释放已经分配成功的内存
    {
        freeM();
    }


    return tRet;
}





/*根据outData_p->phyType取特定物理量的配置值
注意:此接口没有取物理量对应的值,只是取了配置值
    入参:outData_p->phyType 需要调用接口前赋值
    出参:outData_p中其他值
*/
int C_chnUnitData::getPhyCfgByType ( T_phyValCfg* outData_p ) const
{
    if ( outData_p == NULL )
    {
        return MC_ERROR_INPAR;
    }

    if ( ! ( isPhyTypeAvailable ( outData_p->phyType, m_maxPhyType )
           )
       )
    {
        //物理量无效
        return MC_ERROR_PHYTY;
    }

    u_int tPhyIndx        = outData_p->phyType - 1 ;
    outData_p->calcMethd  = m_phyToPnt_p[tPhyIndx].calcMethd;
    outData_p->multFactor = m_phyToPnt_p[tPhyIndx].multFactor;
    outData_p->phyValLng  = m_phyToPnt_p[tPhyIndx].phyValLng;
    //outData_p->phyCatName = m_phyToPnt_p[tPhyIndx].phyCatName;
    outData_p->numOfAdd   = m_phyToPnt_p[tPhyIndx].numOfAdd;

    if ( outData_p->dataId_p != NULL )
    {
        //数据id
        memcpy ( outData_p->dataId_p,
                 m_phyToPnt_p[tPhyIndx].dataId_p,
                 m_dataIdLng
               );
    }

    //tPhyIndx物理量包括的所有点地址
    for ( u_int i = 0;
          i < outData_p->numOfAdd && outData_p->pntAddr_p != NULL;
          i++
        )
    {
        outData_p->pntAddr_p[i] = m_phyToPnt_p[tPhyIndx].pntAddr_p[i];
    }

    return 0;
}




/*根据点地址取物理量的配置值
    入参:pntAddVal_p
            outInNum 数组outData_p的大小
    出参:outData_p
            outInNum 数组中实际赋值的个数
    返回值:
        0: 成功
        <0:失败
        >0:实际物理量个数大于入参最大容量
*/
int C_chnUnitData::getPhyCfgByPnt ( const PNT_ADDR_TYPE* pntAddVal_p,
                                    T_phyValCfg outData_p[],
                                    u_int&  outInNum
                                  ) const
{
    if ( pntAddVal_p == NULL
         || outData_p == NULL
         || outInNum == 0
       )
    {
        return MC_ERROR_INPAR;
    }

    const PNT_ADDR_TYPE* tInAddVal_p = pntAddVal_p;
    int tRet = 0;
    u_int tPntIndx = 0;
    //找到点地址的索引
    tRet = findPntIndx ( *tInAddVal_p, tPntIndx );

    if ( tRet != 0 )
    {
        //未找到相应的点地址配置
        return MC_ERROR_PNTAD;
    }

    u_int num = 1;

    //根据点地址索引 取 点地址对象的物理量类型(物理量索引)
    PHY_UNIT_TYPE tPhyType = m_pntStrgObj.pntMemObj_p[tPntIndx].pntUType;

    T_phyValStrg* tPhyValStrgHead_p = & ( m_phyToPnt_p[tPhyType - 1] );
    T_phyValStrg* tPhyValStrg_p = tPhyValStrgHead_p;

    while ( tPhyValStrg_p->next_p != tPhyValStrgHead_p )
    {
        tPhyValStrg_p = tPhyValStrg_p->next_p;
        num++;

        if ( num > MC_MAX_CYCLES )
        {
            break;
        }
    }

    if ( num > MC_MAX_CYCLES )
    {
        return MC_ERROR_CYCLES;
    }

    if ( num > outInNum ) //实际物理量个数大于入参最大容量
    {
        return 1;
    }

    //根据物理量类型得到物理量配置值
    u_int i = 0;
    T_phyValCfg* tPhyValCfg_p = & ( outData_p[i] );
    tPhyValStrg_p         = tPhyValStrgHead_p;
    tPhyValCfg_p->phyType = tPhyValStrg_p->phyType;
    tRet = getPhyCfgByType ( tPhyValCfg_p );

    if ( tRet != 0 )
    {
        return tRet;
    }

    i++;

    //取多个物理量的配置
    while ( tPhyValStrg_p->next_p != tPhyValStrgHead_p )
    {
        tPhyValStrg_p         = tPhyValStrg_p->next_p;
        tPhyValCfg_p          = & ( outData_p[i] );
        tPhyValCfg_p->phyType = tPhyValStrg_p->phyType;

        tRet = getPhyCfgByType ( tPhyValCfg_p );

        if ( tRet != 0 )
        {
            return tRet;
        }

        i++;
    }

    outInNum = i;

    return 0;
}



/*根据多个点地址取物理量的数量(对相同物理量已经去重了:
    多个点对同一个物理量只返回一个)
    入参:pntAddVal_p
    出参:outNum_p   物理量的数量
        outPhyType_p (可选)当此参数不为空时输出物理量的类型
*/
int C_chnUnitData::getPhyNumByNPnt ( COM_OBJ_TYPE* pntAddVal_pp[],
                                     const u_int& numPnt,
                                     u_int*  outNum_p,
                                     PHY_UNIT_TYPE outPhyType_p[]
                                   ) const
{
    if ( pntAddVal_pp == NULL
         || numPnt == 0
         || outNum_p == NULL
       )
    {
        return MC_ERROR_INPAR;
    }

    //const COM_OBJ_TYPE** tInAddVal_p = pntAddVal_pp;
    int tRet = 0;
    u_int tPntIndx = 0;
    u_int curNum = 0;

    //const int tPhyArrNum = 10000;//定义最大的物理量个数

    PHY_UNIT_TYPE tPhyType[MC_MAX_PHYNUM];
    memset ( tPhyType, 0, sizeof ( tPhyType ) );

    //第一遍计算不算有相同点地址的物理量
    for ( u_int i = 0; i < numPnt; i++ )
    {
        //const PNT_ADDR_TYPE* tInAddVal2_p = &(tInAddVal_p[i]);
        //找到点地址的索引
        tRet = findPntIndx ( pntAddVal_pp[i]->gData.val_chkStd.pntAddr,
                             tPntIndx
                           );

        if ( tRet != 0 )
        {
            //未找到相应的点地址配置
            return MC_ERROR_PNTAD;
        }

        u_char oldFlag = 0 ;

        //对相同的物理量进行去重
        for ( u_int j = 0; j < curNum; j++ )
        {
            if ( tPhyType[j] == m_pntStrgObj.pntMemObj_p[tPntIndx].pntUType )
            {
                oldFlag = 1;
                break;
            }
        }

        if ( oldFlag == 1 )
        {
            /*物理量已经有了*/
            continue;
        }

        //根据点地址索引 取 点地址对象的物理量类型(物理量索引)
        if ( m_pntStrgObj.pntMemObj_p[tPntIndx].pntUType != MC_INVALID_PHTYPE )
        {
            tPhyType[curNum] = m_pntStrgObj.pntMemObj_p[tPntIndx].pntUType;
            curNum++;
        }
    }

    //第二遍计算加上有相同点地址的物理量
    PHY_UNIT_TYPE tType = 0;
    u_char oldFlag = 0 ;
    u_int tFNum = curNum;

    for ( u_int i = 0; i < tFNum; i++ )
    {
        T_phyValStrg* tPhyValHead_p = & ( m_phyToPnt_p[tPhyType[i] - 1] );
        T_phyValStrg* tPhyVal_p = tPhyValHead_p;
        u_int unusualNum = 0;

        while ( tPhyVal_p->next_p != tPhyValHead_p )
        {
            unusualNum++;

            //对循环进行防止死循环处理
            if ( unusualNum > MC_MAX_CYCLES )
            {
                return MC_ERROR_CYCLES;
            }

            tPhyVal_p = tPhyVal_p->next_p;
            tType = tPhyVal_p->phyType;
            oldFlag = 0;

            for ( u_int j = 0; j < curNum; j++ )
            {
                if ( tType == tPhyType[j] )
                {
                    oldFlag = 1;
                    break;
                }
            }

            if ( oldFlag == 1 )
            {
                /*物理量已经有了*/
                continue;
            }

            tPhyType[curNum] = tType;
            curNum++;

        }
    }

    if ( outPhyType_p != NULL
         && *outNum_p > curNum
       )
    {
        memcpy ( outPhyType_p,
                 tPhyType,
                 sizeof ( PHY_UNIT_TYPE ) *curNum
               );
    }

    *outNum_p = curNum;
    return 0;
}




/*根据多个点地址取物理量的配置值(对相同物理量已经去重了:
          多个点对同一个物理量只返回一个)
            入参:pntAddVal_pp
            出参:outData_p
        */
int C_chnUnitData::getPhyCfgByNPnt ( COM_OBJ_TYPE* pntAddVal_pp[],
                                     const u_int& numPnt,
                                     T_phyValCfg outData_p[],
                                     u_int*  outNum_p
                                   ) const
{
    if ( pntAddVal_pp == NULL
         || outData_p == NULL
         || numPnt == 0
         || outNum_p == NULL
       )
    {
        return MC_ERROR_INPAR;
    }

    //const COM_OBJ_TYPE* tInAddVal_p = pntAddVal_pp;
    int tRet = 0;
    //u_int tPntIndx = 0;
    //u_int curNum = 0;

    PHY_UNIT_TYPE tPhyType[MC_MAX_PHYNUM];
    memset ( tPhyType, 0, sizeof ( tPhyType ) );
    u_int tMaxPhyNum = MC_MAX_PHYNUM;
    tRet = getPhyNumByNPnt ( pntAddVal_pp,
                             numPnt,
                             &tMaxPhyNum,
                             tPhyType
                           );

    if ( tRet != 0 )
    {
        return tRet;
    }

    for ( u_int i = 0; i < tMaxPhyNum; i++ )
    {
        outData_p[i].phyType = tPhyType[i];
        //根据物理量类型得到物理量配置值
        tRet = getPhyCfgByType ( & ( outData_p[i] ) );

        if ( tRet != 0 )
        {
            return tRet;
        }
    }

    *outNum_p = tMaxPhyNum;

    return 0;
}



void C_chnUnitData::freeM ( void )
{
    //释放点地址与关联对象
    if ( m_pntStrgObj.pntMemObj_p != NULL )
    {
        free ( m_pntStrgObj.pntMemObj_p );
        m_pntStrgObj.pntMemObj_p = NULL;
    }

    //释放时标空间
    if ( m_pntStrgObj.tmStmp_p != NULL )
    {
        free ( m_pntStrgObj.tmStmp_p );
        m_pntStrgObj.tmStmp_p = NULL;
    }

    //释放点地址控制结构
    if ( m_pntStrgObj.pntAddrCtl.conAddDex_p != NULL )
    {
        free ( m_pntStrgObj.pntAddrCtl.conAddDex_p );
        m_pntStrgObj.pntAddrCtl.conAddDex_p = NULL;
    }


    //释放物理量与点地址的对应关系
    if ( m_phyToPnt_p != NULL )
    {
        for ( u_int i = 0;
              i < m_maxPhyType;
              i++
            )
        {
            T_phyValStrg* tPhyToPnt_p   = & ( m_phyToPnt_p[i] );

            if ( tPhyToPnt_p->phyObjVal_p != NULL )
            {
                free ( tPhyToPnt_p->phyObjVal_p );
                tPhyToPnt_p->phyObjVal_p = NULL;
            }

            if ( tPhyToPnt_p->dataId_p != NULL )
            {
                free ( tPhyToPnt_p->dataId_p );
                tPhyToPnt_p->dataId_p = NULL;
            }

            if ( tPhyToPnt_p->pntAddr_p != NULL )
            {
                free ( tPhyToPnt_p->pntAddr_p );
                tPhyToPnt_p->pntAddr_p = NULL;
            }
        }

        free ( m_phyToPnt_p );
        m_phyToPnt_p = NULL;
    }

    //释放m_phyCat_p 物理量类别索引
    if ( m_phyCat_p != NULL )
    {
        for ( u_int i = 0; i < m_numPhyCat; i++ )
        {
            if ( m_phyCat_p[i].head_p != NULL )
            {
                free ( m_phyCat_p[i].head_p );
                m_phyCat_p[i].head_p = NULL;
            }
        }

        free ( m_phyCat_p );
        m_phyCat_p = NULL;
    }

    //释放通道时标
    if ( m_untChnTmStmp.dataId_p != NULL )
    {
        free ( m_untChnTmStmp.dataId_p  );
        m_untChnTmStmp.dataId_p = NULL;
    }

    if ( m_untChnTmStmp.tmStmp_p != NULL )
    {
        free ( m_untChnTmStmp.tmStmp_p  );
        m_untChnTmStmp.tmStmp_p = NULL;
    }

    m_untChnTmStmp.tmStmpLng = 0;

    m_chnNum     = 0;
    m_pntAddrNum = 0;
    m_phyUnitNum = 0;
    m_pntObjLng  = 0;
    m_dataIdLng  = 0;
    m_tmStmpLng  = 0;
    m_numPhyCat  = 0;
    m_maxPhyType = 0;

    memset ( &m_pntStrgObj, 0, sizeof ( m_pntStrgObj ) );
    memset ( &m_untChnTmStmp, 0, sizeof ( m_untChnTmStmp ) );

    return;
}




//设置物理量相关值
int C_chnUnitData::setM ( const T_phyNVal* inPar_p,
                          const u_char limitFlag
                        )
{
    if ( inPar_p == NULL )
    {
        return MC_ERROR_INPAR;
    }

    if ( inPar_p->phyUnitNum == 0
         || inPar_p->phyObjVal_p == NULL
       )
    {
        return MC_ERROR_INPAR;
    }

    const T_phyNVal* tInPar_p = inPar_p;
    T_phyObjVal* tInPhy_p     = inPar_p->phyObjVal_p;
    int tRet                  = 0;

    //限制判断
    for ( u_int i = 0;
          tInPhy_p != NULL && i < tInPar_p->phyUnitNum;
          i++
        )
    {
        if ( ! ( isPhyTypeAvailable ( tInPhy_p[i].phyType, m_maxPhyType )
               )
           )
        {
            //物理量无效
            return MC_ERROR_PHYTY;
        }

        //需要根据初始化的长度再进行限制判断
        if ( limitFlag == 1 )
        {
            u_int phyIndx = tInPhy_p[i].phyType - 1;
            T_phyValStrg* tPhyValS_p = & ( m_phyToPnt_p[phyIndx] );

            if ( tPhyValS_p->phyValLng != tInPhy_p[i].phyValLng )
            {
                //物理量的长度与初始化时的长度不一致
                return -4;
            }

            if ( m_pntObjLng != tInPhy_p[i].pntObjLng )
            {
                //点地址对应通用对象的长度与初始化不一致
                return -5;
            }

            if ( tPhyValS_p->numOfAdd != tInPhy_p[i].numOfAdd )
            {
                //物理量对应的点个数与初始化配置不一致
                return -6;
            }

            //时标长度限制
            //...以配置为准
            if ( m_tmStmpLng != tInPhy_p[i].tmStmpLng )
            {
                //时标长度与初始化配置不一致
                return -7;
            }

            //if ( tPhyValS_p->calcMethd != tInPhy_p[i].calcMethd )
            //{
            //    //物理量值的算法与初始化时的长度不一致
            //    return -4;
            //}
        }
    }

    m_valLock.Lock();

    //对多个物理量进行值设置
    for ( u_int i = 0;
          tInPhy_p != NULL && i < tInPar_p->phyUnitNum;
          i++
        )
    {
        u_int phyIndx = tInPhy_p[i].phyType - 1;
        T_phyValStrg* tPhyValS_p = & ( m_phyToPnt_p[phyIndx] );

        //记录累加次数
        tPhyValS_p->acmTimes  = tInPhy_p[i].acmTimes;

        //算法 是配置值暂定不在此设置
        //tPhyValS_p->calcMethd = tInPhy_p[i].calcMethd;

        //copy 物理量的值
        if ( tInPhy_p[i].phyObjVal_p != NULL )
        {
            memcpy ( tPhyValS_p->phyObjVal_p,
                     tInPhy_p[i].phyObjVal_p,
                     tPhyValS_p->phyValLng
                   );
            tPhyValS_p->freshFlag = true;
        }

        //copy点地址对应的通用对象
        for ( u_int j = 0;
              j < tPhyValS_p->numOfAdd && tInPhy_p[i].pntObj_p != NULL;
              j++
            )
        {
            /*根据物理量类型找到配置的点地址
            然后根据点地址找点地址索引,最后更新点地址对应的对象
            */
            u_int pntIndx = 0;
            //PNT_ADDR_TYPE tPntAdd = tPhyValS_p->pntAddr_p[j];
            tRet = findPntIndx ( tPhyValS_p->pntAddr_p[j], pntIndx );

            if ( tRet != 0 )
            {
                //未找到点地址的配置
                m_valLock.UnLock();
                return MC_ERROR_PNTAD;
            }

            COM_OBJ_TYPE* tRelateObj_p = & (
                                         m_pntStrgObj.pntMemObj_p[pntIndx].relateObj
                                         );
            memcpy ( tRelateObj_p,
                     ( ( char* ) tInPhy_p[i].pntObj_p ) + ( j * m_pntObjLng ),
                     m_pntObjLng
                   );

            //copy时标
            if ( m_tmStmpLng > 0
                 && tRelateObj_p->gData.tmStmp_p != NULL
               )
            {
                memcpy ( ( ( char* ) m_pntStrgObj.tmStmp_p ) + ( pntIndx * m_tmStmpLng ),
                         /*( ( COM_OBJ_TYPE* ) tInPhy_p[i].pntObj_p )->gData.tmStmp_p,*/
                         tRelateObj_p->gData.tmStmp_p,
                         m_tmStmpLng
                       );
            }
        }
    }

    m_valLock.UnLock();

    return 0;
}


//设置通道时标
int C_chnUnitData::setChnTmStmp ( const void* tmStmp_p )
{
    if ( tmStmp_p == NULL )
    {
        return MC_ERROR_INPAR;
    }

    if ( m_untChnTmStmp.tmStmp_p == NULL
         || m_untChnTmStmp.tmStmpLng == 0
       )
    {
        return MC_ERROR_CAUS;
    }

    m_tmStmpLock.Lock();
    memcpy ( m_untChnTmStmp.tmStmp_p,
             tmStmp_p,
             m_untChnTmStmp.tmStmpLng
           );
    m_untChnTmStmp.freshFlag = true;
    m_tmStmpLock.UnLock();

    return 0;
}

/*获取物理量相关值
   入参:inPar_p->phyUnitNum                 要获取物理量个数
           inPar_p->phyObjVal_p[i].phyType  要获取物理量的类型
   出参:inPar_p的其他值
*/
int C_chnUnitData::getM ( T_phyNVal* inPar_p )
{
    if ( inPar_p == NULL )
    {
        return MC_ERROR_INPAR;
    }

    if ( inPar_p->phyUnitNum == 0 )
    {
        return 0;
    }

    T_phyNVal* tInPhyNVal_p  = inPar_p;
    T_phyObjVal* tInPhyVal_p = tInPhyNVal_p->phyObjVal_p;
    int tRet                 = 0;

    //限制
    for ( u_int i = 0;
          tInPhyVal_p != NULL && i < tInPhyNVal_p->phyUnitNum;
          i++
        )
    {
        if ( ! ( isPhyTypeAvailable ( tInPhyVal_p[i].phyType, m_maxPhyType )
               )
           )
        {
            //物理量无效
            return MC_ERROR_PHYTY;
        }
    }

    m_valLock.Lock();

    for ( u_int i = 0;
          tInPhyVal_p != NULL && i < tInPhyNVal_p->phyUnitNum;
          i++
        )
    {
        u_int phyDex = tInPhyVal_p[i].phyType - 1;

        T_phyValStrg* tPhyStrg_p       = & ( m_phyToPnt_p[phyDex] );
        tInPhyVal_p[i].calcMethd       = tPhyStrg_p->calcMethd;
        tInPhyVal_p[i].multFactor      = tPhyStrg_p->multFactor;
        tInPhyVal_p[i].phyValLng       = tPhyStrg_p->phyValLng;
        //tInPhyVal_p[i].phyCatName      = tPhyStrg_p->phyCatName;
        tInPhyVal_p[i].acmTimes        = tPhyStrg_p->acmTimes;
        tInPhyVal_p[i].freshFlag       = tPhyStrg_p->freshFlag;
        tInPhyVal_p[i].numOfAdd        = tPhyStrg_p->numOfAdd;
        tInPhyVal_p[i].pntObjLng       = m_pntObjLng;
        tInPhyVal_p[i].tmStmpLng       = m_tmStmpLng;

        tPhyStrg_p->freshFlag = false;

        //copy 物理量的值
        if ( tInPhyVal_p[i].phyObjVal_p != NULL )
        {
            memcpy ( tInPhyVal_p[i].phyObjVal_p,
                     tPhyStrg_p->phyObjVal_p,
                     tPhyStrg_p->phyValLng
                   );
        }

        //copy点地址对应的通用对象
        for ( u_int j = 0;
              j < tPhyStrg_p->numOfAdd && tInPhyVal_p[i].pntObj_p != NULL;
              j++
            )
        {
            /*根据点地址找点地址索引,最后get点地址对应的对象
            */
            u_int pntIndx = 0;
            //PNT_ADDR_TYPE tPntAdd = tPhyValS_p->pntAddr_p[j];
            tRet = findPntIndx ( tPhyStrg_p->pntAddr_p[j], pntIndx );

            if ( tRet != 0 )
            {
                //未找到点地址的配置
                tPhyStrg_p->freshFlag = tInPhyVal_p[i].freshFlag;
                m_valLock.UnLock();
                return MC_ERROR_PNTAD;
            }

            COM_OBJ_TYPE* tRelateObj_p = & (
                                         m_pntStrgObj.pntMemObj_p[pntIndx].relateObj
                                         );
            char* tObj_p = ( ( char* ) tInPhyVal_p[i].pntObj_p ) + ( j * m_pntObjLng );
            //copy通用对象中非指错变量值
            ( ( COM_OBJ_TYPE* ) tObj_p )->gData.pntType = (
                    tRelateObj_p->gData.pntType
                    );
            ( ( COM_OBJ_TYPE* ) tObj_p )->gData.tmStmpType = (
                    tRelateObj_p->gData.tmStmpType
                    );
            memcpy ( & ( ( ( COM_OBJ_TYPE* ) tObj_p )->gData.val_chkStd ),
                     & ( tRelateObj_p->gData.val_chkStd ),
                     sizeof ( tRelateObj_p->gData.val_chkStd )
                   );
            memcpy ( & ( ( ( COM_OBJ_TYPE* ) tObj_p )->gData.dDsc ),
                     & ( tRelateObj_p->gData.dDsc ),
                     sizeof ( tRelateObj_p->gData.dDsc )
                   );

            /*memcpy ( tObj_p,
                     tRelateObj_p,
                     m_pntObjLng
                   );*/

            //copy时标
            char* tTmStmp_p = NULL;

            if ( m_tmStmpLng > 0 )
            {
                tTmStmp_p = ( ( char* ) m_pntStrgObj.tmStmp_p ) + ( pntIndx * m_tmStmpLng );
            }


            if ( m_tmStmpLng > 0
                 && ( ( COM_OBJ_TYPE* ) tObj_p )->gData.tmStmp_p != NULL
                 && tTmStmp_p != NULL
               )
            {
                memcpy ( ( ( COM_OBJ_TYPE* ) tObj_p )->gData.tmStmp_p,
                         tTmStmp_p,
                         m_tmStmpLng
                       );
            }
        }

    }

    m_valLock.UnLock();

    return 0;
}


//获取通道时标
int C_chnUnitData::getChnTmStmp ( T_untChnTmStmp* chnTmStmp_p )
{
    if ( chnTmStmp_p == NULL )
    {
        return MC_ERROR_INPAR;
    }

    if ( m_untChnTmStmp.tmStmp_p == NULL
         || m_untChnTmStmp.tmStmpLng == 0
       )
    {
        return MC_ERROR_CAUS;
    }

    chnTmStmp_p->tmStmpLng = m_untChnTmStmp.tmStmpLng;

    if ( chnTmStmp_p->tmStmp_p != NULL
         && m_untChnTmStmp.tmStmpLng > 0
       )
    {
        m_tmStmpLock.Lock();
        memcpy ( chnTmStmp_p->tmStmp_p,
                 m_untChnTmStmp.tmStmp_p,
                 m_untChnTmStmp.tmStmpLng
               );
        chnTmStmp_p->freshFlag   = m_untChnTmStmp.freshFlag;
        m_untChnTmStmp.freshFlag = false;
        m_tmStmpLock.UnLock();
    }

    if ( chnTmStmp_p->dataId_p != NULL
         && m_untChnTmStmp.dataId_p != NULL
         && m_dataIdLng > 0
       )
    {
        memcpy ( chnTmStmp_p->dataId_p,
                 m_untChnTmStmp.dataId_p,
                 m_dataIdLng
               );
    }

    return 0;
}


//所有出参,不需要的,直接赋予空
int C_chnUnitData::getPhyValADid ( const PHY_UNIT_TYPE phyType[],/*物理量类型*/
                                   const u_char& num,             /*物理量个数*/
                                   const u_char& onePhyVsize,
                                   void*  oPhyObjVal_p,/*phyObjVal[],物理量的值*/
                                   u_char oPhyValLng[],/*每个物理量值的长度*/
                                   void*  oDataId_p,   /*dataId[],物理量对应的数据id*/
                                   u_int* oDataIdLng_p   /*数据id的长度*/
                                 )
{
    if ( phyType == NULL
         || num == 0
         || ( oPhyObjVal_p != NULL && onePhyVsize == 0 )
         || ( oPhyObjVal_p == NULL
              && oPhyValLng == NULL
              && oDataId_p == NULL
              && oDataIdLng_p == NULL
            )
       )
    {
        return MC_ERROR_INPAR;
    }

    //限制
    for ( u_int i = 0; i < num; i++ )
    {
        if ( ! ( isPhyTypeAvailable ( phyType[i], m_maxPhyType )
               )
           )
        {
            //物理量无效
            return MC_ERROR_PHYTY;
        }
    }

    //u_int addLen = 0;
    if ( oPhyObjVal_p != NULL )
    {
        m_valLock.Lock();

        for ( u_int i = 0; i < num; i++ )
        {
            u_int phyIndx = phyType[i] - 1;
            const T_phyValStrg* tPhyStrg_p = & ( m_phyToPnt_p[phyIndx] );
            //copy物理量的值
            memcpy ( ( ( char* ) oPhyObjVal_p ) + ( i * onePhyVsize ),
                     tPhyStrg_p->phyObjVal_p,
                     tPhyStrg_p->phyValLng
                   ) ;
            //addLen = addLen + tPhyStrg_p->phyValLng;
        }

        m_valLock.UnLock();
    }


    if ( oPhyValLng != NULL
         || oDataId_p != NULL
       )
    {
        for ( u_int i = 0; i < num; i++ )
        {
            u_int phyIndx = phyType[i] - 1;
            const T_phyValStrg* tPhyStrg_p = & ( m_phyToPnt_p[phyIndx] );

            //物理量值的长度
            if ( oPhyValLng != NULL )
            {
                oPhyValLng[i] = tPhyStrg_p->phyValLng;
            }

            //copy数据id
            memcpy ( ( ( char* ) oDataId_p ) + ( i * m_dataIdLng ),
                     tPhyStrg_p->dataId_p,
                     m_dataIdLng
                   ) ;
        }
    }

    if ( oDataIdLng_p != NULL )
    {
        *oDataIdLng_p = m_dataIdLng;
    }

    return 0;
}



int C_chnUnitData::getPhyValLng ( const PHY_UNIT_TYPE phyType[], /*物理量类型*/
                                  const u_char& num,
                                  u_char oPhyValLng[]/*每个物理量值的长度*/
                                ) const
{
    if ( phyType == NULL
         || num == 0
         || oPhyValLng == NULL
       )
    {
        return MC_ERROR_INPAR;
    }

    //限制
    for ( u_int i = 0; i < num; i++ )
    {
        if ( ! ( isPhyTypeAvailable ( phyType[i], m_maxPhyType )
               )
           )
        {
            //物理量无效
            return MC_ERROR_PHYTY;
        }
    }

    for ( u_int i = 0; i < num; i++ )
    {
        u_int phyIndx = phyType[i] - 1;
        const T_phyValStrg* tPhyStrg_p = & ( m_phyToPnt_p[phyIndx] );
        //物理量值的长度
        oPhyValLng[i] = tPhyStrg_p->phyValLng;
    }

    return 0;
}



const  PHY_UNIT_TYPE* C_chnUnitData::getPhyCatBuf ( const int& phyCatName,
        u_int& bufLng
                                                  ) const
{
    if ( m_phyCat_p == NULL
         || m_numPhyCat == 0
       )
    {
        bufLng = 0;
        return NULL;
    }

    u_int i = 0;

    for ( i = 0; i < m_numPhyCat; i++ )
    {
        if ( m_phyCat_p[i].categoryName == phyCatName )
        {
            break;
        }
    }

    if ( i < m_numPhyCat )
    {
        bufLng = m_phyCat_p[i].curNum;
        return m_phyCat_p[i].head_p;
    }

    bufLng = 0;
    return NULL;
}



/***********************************************************************/
/**************************class C_unitInfData**************************/
C_unitInfData::C_unitInfData()
{
    m_chnNumS       = 0;
    m_curNumS       = 0;
    m_chnUnitHead_p = NULL;
}

C_unitInfData::C_unitInfData ( const u_int& chnNumS /*通道总数*/
                             )
{
    if ( chnNumS > 0 )
    {
        m_chnNumS = chnNumS;
        m_chnUnitHead_p = new C_chnUnitData[m_chnNumS]();
    }
    else
    {
        m_chnNumS       = 0;
        m_chnUnitHead_p = NULL;
    }

    m_curNumS = 0;

}

C_unitInfData::~C_unitInfData()
{
    freeM();
}




//初始化
int C_unitInfData::initM ( const u_int& chnNumS,
                           const T_initChnUnit* inPar_p
                         )
{
    if ( inPar_p == NULL
         /*|| chnNumS == 0*/
       )
    {
        return MC_ERROR_INPAR;
    }

    const T_initChnUnit* tInPar_p = inPar_p;

    int tRet = 0;

    if ( chnNumS > m_chnNumS  )
    {
        freeM();
        m_chnNumS = chnNumS;
        m_chnUnitHead_p = new C_chnUnitData[m_chnNumS]();
    }

    if ( m_chnUnitHead_p == NULL )
    {
        return MC_ERROR_CAUS;
    }

    for ( u_int i = 0; i < chnNumS; i++ )
    {
        tRet = m_chnUnitHead_p[i].initM ( & ( tInPar_p[i] ) );

        if ( tRet != 0 )
        {
            freeM();
            return tRet;
        }
    }

    m_curNumS = chnNumS;

    return 0;
}



//初始化
int C_unitInfData::initM ( const T_initChnUnit* inPar_p )
{
    if ( inPar_p == NULL )
    {
        return MC_ERROR_INPAR;
    }
    else if ( m_chnUnitHead_p == NULL )
    {
        return MC_ERROR_CAUS;
    }
    else if ( m_curNumS >= m_chnNumS )
    {
        return MC_ERROR_OVMAX;
    }


    const T_initChnUnit* tInPar_p = inPar_p;

    int tRet = 0;

    tRet = m_chnUnitHead_p[m_curNumS].initM ( tInPar_p );

    if ( tRet != 0 )
    {
        //freeM();
        return tRet;
    }

    m_curNumS = m_curNumS + 1;

    return 0;
}



void C_unitInfData::freeM ( void )
{
    if ( m_chnUnitHead_p != NULL )
    {
        delete [] m_chnUnitHead_p;
        m_chnUnitHead_p = NULL;
    }

    return;
}



/*根据outData_p->phyType取某个通道的特定物理量的配置值
    入参:chnNum 通道编号
        outData_p->phyType 需要调用接口前赋值
    出参:outData_p中其他值
*/
int C_unitInfData::getPhyCfgByType ( const u_int& chnNum,
                                     T_phyValCfg* outData_p ) const
{
    if ( outData_p == NULL )
    {
        return MC_ERROR_INPAR;
    }

    int curIndex = findChnIndx ( chnNum );

    if ( curIndex < 0 )
    {
        //没找到对应通道的设置
        return MC_ERROR_CHNN0;
    }

    int tRet = m_chnUnitHead_p[curIndex].getPhyCfgByType ( outData_p );

    if ( tRet != 0 )
    {
        return tRet;
    }

    return 0;
}



/*根据点地址取某个通道物理量的配置值
    入参:chnNum 通道编号
        pntAddVal_p 点地址
       outInNum 数组outData_p的大小
    出参:outData_p
         outInNum 数组中实际赋值的个数
    返回值:
        0: 成功
        <0:失败
        >0:实际物理量个数大于入参最大容量
    出参:outData_p
*/
int C_unitInfData::getPhyCfgByPnt ( const u_int& chnNum,
                                    const PNT_ADDR_TYPE* pntAddVal_p,
                                    T_phyValCfg outData_p[],
                                    u_int&  outInNum
                                  ) const
{
    if ( pntAddVal_p == NULL
         || outData_p == NULL
       )
    {
        return MC_ERROR_INPAR;
    }

    int curIndex = findChnIndx ( chnNum );

    if ( curIndex < 0 )
    {
        //没找到对应通道的设置
        return MC_ERROR_CHNN0;
    }

    int tRet = m_chnUnitHead_p[curIndex].getPhyCfgByPnt (
               pntAddVal_p,
               outData_p,
               outInNum
               );

    if ( tRet != 0 )
    {
        return tRet;
    }

    return 0;
}



/*根据多个点地址取物理量的数量(对相同物理量已经去重了:
                  多个点对同一个物理量只返回一个)
                  入参:chnNum 通道编号
                       pntAddVal_pp
                  出参:outNum_p   物理量的数量
                       outPhyType_p (可选)当此参数不为空时输出物理量的类型
*/
int C_unitInfData::getPhyNumByNPnt (  const u_int& chnNum,
                                      COM_OBJ_TYPE* pntAddVal_pp[],
                                      const u_int& numPnt,
                                      u_int*  outNum_p,
                                      PHY_UNIT_TYPE outPhyType_p[]
                                   ) const
{
    if ( pntAddVal_pp == NULL
         || numPnt == 0
         || outNum_p == NULL
       )
    {
        return MC_ERROR_INPAR;
    }

    int curIndex = findChnIndx ( chnNum );

    if ( curIndex < 0 )
    {
        //没找到对应通道的设置
        return MC_ERROR_CHNN0;
    }

    int tRet = m_chnUnitHead_p[curIndex].getPhyNumByNPnt (
               pntAddVal_pp,
               numPnt,
               outNum_p,
               outPhyType_p
               );

    if ( tRet != 0 )
    {
        return tRet;
    }

    return 0;
}


/*根据多个点地址取物理量的配置值(对相同物理量已经去重了:
        多个点对同一个物理量只返回一个)
        入参:chnNum 通道编号
             pntAddVal_pp
        出参:outData_p
*/
int C_unitInfData::getPhyCfgByNPnt ( const u_int& chnNum,
                                     COM_OBJ_TYPE* pntAddVal_pp[],
                                     const u_int& numPnt,
                                     T_phyValCfg outData_p[],
                                     u_int*  outNum_p
                                   ) const
{
    if ( pntAddVal_pp == NULL
         || outData_p == NULL
         || numPnt == 0
         || outNum_p == NULL
       )
    {
        return MC_ERROR_INPAR;
    }

    int curIndex = findChnIndx ( chnNum );

    if ( curIndex < 0 )
    {
        //没找到对应通道的设置
        return MC_ERROR_CHNN0;
    }

    int tRet = m_chnUnitHead_p[curIndex].getPhyCfgByNPnt ( pntAddVal_pp,
               numPnt,
               outData_p,
               outNum_p );

    if ( tRet != 0 )
    {
        return tRet;
    }

    return 0;
}

void C_unitInfData::getChnNum ( u_int* chnNumS_p, /*通道总数*/
                                u_int* curNumS_p  /*已经初始化的数量*/
                              ) const
{
    if ( chnNumS_p == NULL
         || curNumS_p == NULL
       )
    {
        return;
    }

    *chnNumS_p = m_chnNumS;
    *curNumS_p = m_curNumS;
    return;
}


int C_unitInfData::findChnIndx ( const u_int& chnNum ) const
{
    int curIndex = -1;

    for ( u_int i = 0; i < m_chnNumS; i++ )
    {
        if ( m_chnUnitHead_p[i].m_chnNum == chnNum )
        {
            curIndex = i;
            break;
        }
    }

    return curIndex;
}




//设置物理量相关值
int C_unitInfData::setM ( const u_int& chnNum,       /*需要赋值的通道号*/
                          const T_phyNVal* inPar_p
                        )
{
    if ( inPar_p == NULL )
    {
        return MC_ERROR_INPAR;
    }

    int curIndex = findChnIndx ( chnNum );

    if ( curIndex < 0 )
    {
        //没找到对应通道的设置
        return MC_ERROR_CHNN0;
    }

    const T_phyNVal* tInPar_p = inPar_p;
    //const u_int tChnNum      = chnNum;

    int tRet =  m_chnUnitHead_p[curIndex].setM ( tInPar_p );

    if ( tRet != 0 )
    {
        return tRet;
    }

    return 0;
}




/*获取物理量相关值
    入参:valChnNum
            cfgChnNum
            valNData_p->phyUnitNum              要获取物理量个数
            valNData_p->phyObjVal_p[i].phyType  要获取物理量的类型
            cfgNPhyCfg_p->phyUnitNum             要获取物理量个数
            cfgNPhyCfg_p->phyValCfg_p[i].phyType 要获取物理量的类型
    出参:valNData_p的其他值
            cfgNPhyCfg_p的其他值
*/
int C_unitInfData::getM ( const u_int& valChnNum, /*要取值的通道号*/
                          const u_int& cfgChnNum,  /*要取配置的通道号*/
                          T_phyNVal*   valNData_p,/*物理量值结构*/
                          T_phyNCfg*   cfgNPhyCfg_p/*物理量配置结构*/
                        ) const
{
    //值或配置不能同时为空
    if ( valNData_p == NULL
         && cfgNPhyCfg_p == NULL
       )
    {
        return MC_ERROR_INPAR;
    }

    int cfgIndex              = 0;
    int valIndex              = 0;
    T_phyNVal* tInValPar_p    = valNData_p;
    T_phyNCfg* tInCfgPhyCfg_p = cfgNPhyCfg_p;
    int tRet                  = 0;


    //取物理量的相关值
    if ( tInValPar_p != NULL )
    {
        valIndex = findChnIndx ( valChnNum );

        if ( valIndex < 0 )
        {
            //没找到对应通道的设置
            return MC_ERROR_CHNN0;
        }

        tRet =  m_chnUnitHead_p[valIndex].getM ( tInValPar_p ) ;

        if ( tRet != 0 )
        {
            return tRet;
        }
    }

    //取物理量的相关配置
    if ( tInCfgPhyCfg_p != NULL )
    {
        cfgIndex = findChnIndx ( cfgChnNum );

        if ( cfgIndex < 0 )
        {
            //没找到对应通道的设置
            return MC_ERROR_CHNN0;
        }

        for ( u_int i = 0; i < tInCfgPhyCfg_p->phyUnitNum; i++ )
        {
            T_phyValCfg* tPhyValCfg_p = & ( tInCfgPhyCfg_p->phyValCfg_p[i] );
            tRet = m_chnUnitHead_p[cfgIndex].getPhyCfgByType ( tPhyValCfg_p );

            if ( tRet != 0 )
            {
                return tRet;
            }
        }
    }

    return 0;
}




//设置通道时标
int C_unitInfData::setChnTmStmp ( const u_int& chnNum, /*需要赋值的通道号*/
                                  const void* tmStmp_p
                                )
{
    if ( tmStmp_p == NULL )
    {
        return MC_ERROR_INPAR;
    }

    int curIndex = findChnIndx ( chnNum );

    if ( curIndex < 0 )
    {
        //没找到对应通道的设置
        return MC_ERROR_CHNN0;
    }

    int tRet =  m_chnUnitHead_p[curIndex].setChnTmStmp (
                tmStmp_p
                ) ;

    if ( tRet != 0 )
    {
        return tRet;
    }

    return 0;
}



//获取通道时标
int C_unitInfData::getChnTmStmp ( const u_int& fromChnNum, /*要从哪个通道号取值*/
                                  T_untChnTmStmp* chnTmStmp_p
                                )
{
    if ( chnTmStmp_p == NULL )
    {
        return MC_ERROR_INPAR;
    }

    int curIndex = findChnIndx ( fromChnNum );

    if ( curIndex < 0 )
    {
        //没找到对应通道的设置
        return MC_ERROR_CHNN0;
    }

    int tRet =  m_chnUnitHead_p[curIndex].getChnTmStmp (
                chnTmStmp_p
                ) ;

    if ( tRet != 0 )
    {
        return tRet;
    }

    return 0;
}



/*获取物理量相关值
所有出参,不需要的,直接赋予空
    入参:fromChnNum
            curChnNum
            fromNData_p->phyUnitNum              要获取物理量个数
            fromNData_p->phyObjVal_p[i].phyType  要获取物理量的类型
            curNPhyCfg_p->phyUnitNum             要获取物理量个数
            curNPhyCfg_p->phyValCfg_p[i].phyType 要获取物理量的类型
    出参:fromNData_p的其他值
            curNPhyCfg_p的其他值
*/
int C_unitInfData:: getPhyValADid ( const u_int& chnNum,          /*通道号*/
                                    const PHY_UNIT_TYPE phyType[],/*物理量类型*/
                                    const u_char& num,             /*物理量个数*/
                                    const u_char& onePhyVsize,
                                    void*  oPhyObjVal_p,/*phyObjVal[],物理量的值*/
                                    u_char oPhyValLng[],/*每个物理量值的长度*/
                                    void*  oDataId_p,   /*dataId[],物理量对应的数据id*/
                                    u_int* oDataIdLng_p   /*数据id的长度*/
                                  ) const
{

    if ( phyType == NULL
         || num == 0
         || ( oPhyObjVal_p != NULL && onePhyVsize == 0 )
         || ( oPhyObjVal_p == NULL
              && oPhyValLng == NULL
              && oDataId_p == NULL
              && oDataIdLng_p == NULL
            )
       )
    {
        return MC_ERROR_INPAR;
    }

    int curIndex = findChnIndx ( chnNum );

    if ( curIndex < 0 )
    {
        //没找到对应通道的设置
        return MC_ERROR_CHNN0;
    }

    int tRet =  m_chnUnitHead_p[curIndex].getPhyValADid (
                phyType,
                num,
                onePhyVsize,
                oPhyObjVal_p,
                oPhyValLng,
                oDataId_p,
                oDataIdLng_p
                ) ;

    if ( tRet != 0 )
    {
        return tRet;
    }

    return 0;
}




int C_unitInfData::getPhyValLng ( const u_int& chnNum,           /*通道号*/
                                  const PHY_UNIT_TYPE phyType[], /*物理量类型*/
                                  const u_char& num,
                                  u_char oPhyValLng[]/*每个物理量值的长度*/
                                ) const
{
    if ( phyType == NULL
         || num == 0
         || oPhyValLng == NULL
       )
    {
        return MC_ERROR_INPAR;
    }

    int curIndex = findChnIndx ( chnNum );

    if ( curIndex < 0 )
    {
        //没找到对应通道的设置
        return MC_ERROR_CHNN0;
    }

    int tRet =  m_chnUnitHead_p[curIndex].getPhyValLng (
                phyType,
                num,
                oPhyValLng
                ) ;

    if ( tRet != 0 )
    {
        return tRet;
    }

    return 0;
}



const  PHY_UNIT_TYPE* C_unitInfData::getPhyCatBuf ( const u_int& chnNum,    /*通道号*/
        const int& phyCatName, /*物理量类别名*/
        u_int& bufLng          /*物理量类型buf长度*/
                                                  ) const
{
    int curIndex = findChnIndx ( chnNum );

    if ( curIndex < 0 )
    {
        bufLng = 0;
        return NULL;
    }

    return ( m_chnUnitHead_p[curIndex].getPhyCatBuf (
             phyCatName,
             bufLng
             )
           );

}