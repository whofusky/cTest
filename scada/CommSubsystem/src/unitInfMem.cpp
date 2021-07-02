/******************************************************************************
** �ļ�����unitInfMem.cpp
** ���ߣ�
** ����ʱ�䣺2018-04-21
** ���ܣ�
** �޸ģ�
** ������
*******************************************************************************/

#include "unitInfMem.h"
#include <stdlib.h>
//#include "DataType.h"



//�������Ƿ���Ч
inline bool isPhyTypeAvailable ( const PHY_UNIT_TYPE& val,
                                 const u_int& maxVal
                               )
{
    if ( val ==  0
         || val > maxVal
       )
    {
        //�����������ʹ���
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



//��ʼ����ķ���
int C_chnUnitData::initPntStrgObj ( const T_pntAddrCfg* inPar_p,
                                    const u_int&  pntObjLng,
                                    const u_int&  pntAddrNum,
                                    const u_char& tmStmpLng
                                  )
{
    if ( inPar_p == NULL
         || inPar_p->num == 0 /*���������ӵ�ַ�εĶ���*/
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

    m_tmStmpLng    = tmStmpLng;  //ʱ�곤��

    //����ÿ�����ַ�μ�����������
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

    //������ȡ�����
    if ( tNum > pntAddrNum )
    {
        m_pntAddrNum = tNum;
    }
    else
    {
        m_pntAddrNum = pntAddrNum;
    }

    //������ַ��Ӧ����ռ�
    m_pntStrgObj.pntMemObj_p = ( T_pntMemObj* ) calloc ( m_pntAddrNum,
                               sizeof ( T_pntMemObj ) );

    if ( m_pntStrgObj.pntMemObj_p == NULL )
    {
        return MC_ERROR_CALLOC;
    }

    //����Э������ʱ��ռ�
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

    //�����ַ���ƶ�������ַ��ÿ�ο�ʼ,�������ַ��ֵ
    //����¼ÿ�ο�ʼ��ַ��������
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

        //��ͨ�ö���ĵ��ַ��ֵ
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


//ͨ�����ַ�ҵ��ַ�������ṹ�е�������
int C_chnUnitData::findPntIndx ( const PNT_ADDR_TYPE& value,
                                 u_int& retIndx
                               ) const
{
    u_char flag = 0;

    /*�ҵ��ַ�����е��ַ�����е��±�
    m_pntStrgObj.pntAddrCtl.num �����ж��ٶ������ĵ�ַ��
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
    m_phyUnitNum   = phyUnitNum;//����������
    m_dataIdLng    = dataIdLng; //����id����


    //������������һ��ȫ��,�����õ��ĸ�������,������������ֵΪȫ��������±�����
    m_phyToPnt_p = ( T_phyValStrg* ) calloc ( m_maxPhyType,
                   sizeof ( T_phyValStrg )
                                            );

    if ( m_phyToPnt_p == NULL )
    {
        return MC_ERROR_CALLOC;
    }

    //��ÿһ����������ֵ
    for ( u_int i = 0; i < m_phyUnitNum; i++ )
    {
        const T_phyValCfg* tInPhyCfgI_p = & ( tInPhyCfg_p[i] );

        if ( ! ( isPhyTypeAvailable ( tInPhyCfgI_p->phyType, m_maxPhyType )
               )
           )
        {
            //�����������ʹ���
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
            tPhyToPnt_p->multFactor = 1;//Ĭ��Ϊ1
        }
        else
        {
            tPhyToPnt_p->multFactor = tInPhyCfgI_p->multFactor;
        }

        //����洢������ֵ�Ŀռ�
        if ( tPhyToPnt_p->phyValLng > 0 )
        {
            tPhyToPnt_p->phyObjVal_p  = calloc (
                                        1,
                                        tPhyToPnt_p->phyValLng
                                        );

            if ( tPhyToPnt_p->phyObjVal_p == NULL )
            {
                //����洢������ֵ�Ŀռ� ����
                return MC_ERROR_CALLOC;
            }
        }
        else
        {
            tPhyToPnt_p->phyObjVal_p = NULL;
        }

        //����洢����id�Ŀռ�
        if ( m_dataIdLng > 0 )
        {
            tPhyToPnt_p->dataId_p  = calloc (
                                     1,
                                     m_dataIdLng
                                     );

            if ( tPhyToPnt_p->dataId_p == NULL )
            {
                //����洢����id�Ŀռ� ����
                return MC_ERROR_CALLOC;
            }

            //copy����id��ֵ
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
        //�����������������ַ�Ŀռ�
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

        //�����ַ��ֵ
        /*memcpy ( tPhyToPnt_p->pntAddr_p,
        tInPhyToPnt_p->pntAddr_p,
        tInPhyToPnt_p->numOfAdd * sizeof ( PNT_ADDR_TYPE )
        );*/

        //�����������������ַ���ܵĵ�ַַ��������ռ�
        /*tPhyToPnt_p->addIndex_p = ( u_int* ) calloc (
        tInPhyToPnt_p->numOfAdd,
        sizeof ( u_int )
        );

        if ( tPhyToPnt_p->addIndex_p == NULL )
        {
        tRet = MC_ERROR_CALLOC;
        goto end_initM;
        }*/

        //��ֵ���ַ���Ե��ַ��Ӧ�Ķ�����������й���(�γ�ָ�뻷)����
        for ( u_int j = 0; j < tInPhyCfgI_p->numOfAdd; j++ )
        {
            tPhyToPnt_p->pntAddr_p[j] = tInPhyCfgI_p->pntAddr_p[j];

            //u_char flag = 0;

            //�������ַ��Ӧ������������
            u_int tInDx = 0;
            tRet = findPntIndx ( tPhyToPnt_p->pntAddr_p[j], tInDx );

            if ( tRet != 0 )
            {
                //�����������ĵ�ֵ���ܵĵ����û��,���ش���
                return MC_ERROR_PNTAD;
            }

            //��ǰ���ַ��δ��Ӧ�κ�������
            if ( m_pntStrgObj.pntMemObj_p[tInDx].pntUType == 0 )
            {
                m_pntStrgObj.pntMemObj_p[tInDx].pntUType = tInPhyCfgI_p->phyType;
            }

            //��ǰ���ַ��Ӧ���׸�����������
            PHY_UNIT_TYPE tPhyType = m_pntStrgObj.pntMemObj_p[tInDx].pntUType;

            T_phyValStrg* tPhyValStrgHead_p = & ( m_phyToPnt_p[tPhyType - 1 ] );
            T_phyValStrg* tPhyValStrg_p = & ( m_phyToPnt_p[tPhyType - 1 ] );

            if ( tPhyValStrg_p->next_p == NULL )
            {
                //�����һָ��Ϊ������һָ��ָ���Լ�
                tPhyValStrg_p->next_p = tPhyValStrg_p;
            }

            //�ҵ���Ҫ����Ļ�λ��
            while ( tPhyValStrg_p->next_p != tPhyValStrgHead_p
                    && tPhyType != tInPhyCfgI_p->phyType
                  )
            {
                tPhyValStrg_p = tPhyValStrg_p->next_p;
                tPhyType      = tPhyValStrg_p->phyType;
            }

            if ( tPhyType == tInPhyCfgI_p->phyType )
            {
                /*tInPhyCfgI_p->phyType�����������Ѿ���ͬ��ַָ�뻷��,
                ����Ҫ�ٴ���*/
                //break;
            }
            else
            {
                /*tInPhyCfgI_p->phyType����������Ҫ��ӵ�ͬ��ַָ�뻷��,
                */
                tPhyValStrg_p->next_p = tPhyToPnt_p;
                tPhyToPnt_p->next_p   = tPhyValStrgHead_p;
            }

        }

    }//end ��ÿһ����������ֵ

    return 0;
}


//��ʼ�������ۼ����������ṹ
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



//��ʼ��
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
    m_chnNum       = tInPar_p->chnNum;    //ͨ����
    m_maxPhyType   = tInPar_p->maxPhyType;//һ��ͨ������������������ֵ

    //m_pntAddrNum   = tInPar_p->pntAddrNum;//���ַ����
    //m_phyUnitNum   = tInPar_p->phyUnitNum;//����������
    //m_pntObjLng    = tInPar_p->pntObjLng; //���ַ����ͨ�����ݶ���ĳ���
    //m_dataIdLng    = tInPar_p->dataIdLng; //����id����
    //m_tmStmpLng    = inPar_p->tmStmpLng;  //ʱ�곤��

    m_valLock.Init();
    m_tmStmpLock.Init();

    int tRet = 0;

    //��ʼ���������
    tRet = initPntStrgObj ( & ( tInPar_p->pntAddrCfg ),
                            tInPar_p->pntObjLng,
                            tInPar_p->pntAddrNum,
                            tInPar_p->tmStmpLng
                          );

    if ( tRet != 0 )
    {
        goto end_initM;
    }


    //��ʼ������������ַ�Ķ�Ӧ��ϵ
    tRet = initPhyToPnt ( tInPar_p->phyObjVal_p,
                          tInPar_p->phyUnitNum,
                          tInPar_p->dataIdLng
                        );

    if ( tRet != 0 )
    {
        goto end_initM;
    }

    //��ʼ�������ۼ����������ṹ
    tRet = initPhyCat ( tInPar_p->numPhyCat,
                        tInPar_p->phyCat_p
                      );

    if ( tRet != 0 )
    {
        goto end_initM;
    }

    //��ʼ��ͨ��ʱ��
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

            //copy����id��ֵ
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

    if ( tRet != 0 ) //�ͷ��Ѿ�����ɹ����ڴ�
    {
        freeM();
    }


    return tRet;
}





/*����outData_p->phyTypeȡ�ض�������������ֵ
ע��:�˽ӿ�û��ȡ��������Ӧ��ֵ,ֻ��ȡ������ֵ
    ���:outData_p->phyType ��Ҫ���ýӿ�ǰ��ֵ
    ����:outData_p������ֵ
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
        //��������Ч
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
        //����id
        memcpy ( outData_p->dataId_p,
                 m_phyToPnt_p[tPhyIndx].dataId_p,
                 m_dataIdLng
               );
    }

    //tPhyIndx���������������е��ַ
    for ( u_int i = 0;
          i < outData_p->numOfAdd && outData_p->pntAddr_p != NULL;
          i++
        )
    {
        outData_p->pntAddr_p[i] = m_phyToPnt_p[tPhyIndx].pntAddr_p[i];
    }

    return 0;
}




/*���ݵ��ַȡ������������ֵ
    ���:pntAddVal_p
            outInNum ����outData_p�Ĵ�С
    ����:outData_p
            outInNum ������ʵ�ʸ�ֵ�ĸ���
    ����ֵ:
        0: �ɹ�
        <0:ʧ��
        >0:ʵ��������������������������
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
    //�ҵ����ַ������
    tRet = findPntIndx ( *tInAddVal_p, tPntIndx );

    if ( tRet != 0 )
    {
        //δ�ҵ���Ӧ�ĵ��ַ����
        return MC_ERROR_PNTAD;
    }

    u_int num = 1;

    //���ݵ��ַ���� ȡ ���ַ���������������(����������)
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

    if ( num > outInNum ) //ʵ��������������������������
    {
        return 1;
    }

    //�������������͵õ�����������ֵ
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

    //ȡ���������������
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



/*���ݶ�����ַȡ������������(����ͬ�������Ѿ�ȥ����:
    ������ͬһ��������ֻ����һ��)
    ���:pntAddVal_p
    ����:outNum_p   ������������
        outPhyType_p (��ѡ)���˲�����Ϊ��ʱ���������������
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

    //const int tPhyArrNum = 10000;//������������������

    PHY_UNIT_TYPE tPhyType[MC_MAX_PHYNUM];
    memset ( tPhyType, 0, sizeof ( tPhyType ) );

    //��һ����㲻������ͬ���ַ��������
    for ( u_int i = 0; i < numPnt; i++ )
    {
        //const PNT_ADDR_TYPE* tInAddVal2_p = &(tInAddVal_p[i]);
        //�ҵ����ַ������
        tRet = findPntIndx ( pntAddVal_pp[i]->gData.val_chkStd.pntAddr,
                             tPntIndx
                           );

        if ( tRet != 0 )
        {
            //δ�ҵ���Ӧ�ĵ��ַ����
            return MC_ERROR_PNTAD;
        }

        u_char oldFlag = 0 ;

        //����ͬ������������ȥ��
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
            /*�������Ѿ�����*/
            continue;
        }

        //���ݵ��ַ���� ȡ ���ַ���������������(����������)
        if ( m_pntStrgObj.pntMemObj_p[tPntIndx].pntUType != MC_INVALID_PHTYPE )
        {
            tPhyType[curNum] = m_pntStrgObj.pntMemObj_p[tPntIndx].pntUType;
            curNum++;
        }
    }

    //�ڶ�������������ͬ���ַ��������
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

            //��ѭ�����з�ֹ��ѭ������
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
                /*�������Ѿ�����*/
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




/*���ݶ�����ַȡ������������ֵ(����ͬ�������Ѿ�ȥ����:
          ������ͬһ��������ֻ����һ��)
            ���:pntAddVal_pp
            ����:outData_p
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
        //�������������͵õ�����������ֵ
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
    //�ͷŵ��ַ���������
    if ( m_pntStrgObj.pntMemObj_p != NULL )
    {
        free ( m_pntStrgObj.pntMemObj_p );
        m_pntStrgObj.pntMemObj_p = NULL;
    }

    //�ͷ�ʱ��ռ�
    if ( m_pntStrgObj.tmStmp_p != NULL )
    {
        free ( m_pntStrgObj.tmStmp_p );
        m_pntStrgObj.tmStmp_p = NULL;
    }

    //�ͷŵ��ַ���ƽṹ
    if ( m_pntStrgObj.pntAddrCtl.conAddDex_p != NULL )
    {
        free ( m_pntStrgObj.pntAddrCtl.conAddDex_p );
        m_pntStrgObj.pntAddrCtl.conAddDex_p = NULL;
    }


    //�ͷ�����������ַ�Ķ�Ӧ��ϵ
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

    //�ͷ�m_phyCat_p �������������
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

    //�ͷ�ͨ��ʱ��
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




//�������������ֵ
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

    //�����ж�
    for ( u_int i = 0;
          tInPhy_p != NULL && i < tInPar_p->phyUnitNum;
          i++
        )
    {
        if ( ! ( isPhyTypeAvailable ( tInPhy_p[i].phyType, m_maxPhyType )
               )
           )
        {
            //��������Ч
            return MC_ERROR_PHYTY;
        }

        //��Ҫ���ݳ�ʼ���ĳ����ٽ��������ж�
        if ( limitFlag == 1 )
        {
            u_int phyIndx = tInPhy_p[i].phyType - 1;
            T_phyValStrg* tPhyValS_p = & ( m_phyToPnt_p[phyIndx] );

            if ( tPhyValS_p->phyValLng != tInPhy_p[i].phyValLng )
            {
                //�������ĳ������ʼ��ʱ�ĳ��Ȳ�һ��
                return -4;
            }

            if ( m_pntObjLng != tInPhy_p[i].pntObjLng )
            {
                //���ַ��Ӧͨ�ö���ĳ������ʼ����һ��
                return -5;
            }

            if ( tPhyValS_p->numOfAdd != tInPhy_p[i].numOfAdd )
            {
                //��������Ӧ�ĵ�������ʼ�����ò�һ��
                return -6;
            }

            //ʱ�곤������
            //...������Ϊ׼
            if ( m_tmStmpLng != tInPhy_p[i].tmStmpLng )
            {
                //ʱ�곤�����ʼ�����ò�һ��
                return -7;
            }

            //if ( tPhyValS_p->calcMethd != tInPhy_p[i].calcMethd )
            //{
            //    //������ֵ���㷨���ʼ��ʱ�ĳ��Ȳ�һ��
            //    return -4;
            //}
        }
    }

    m_valLock.Lock();

    //�Զ������������ֵ����
    for ( u_int i = 0;
          tInPhy_p != NULL && i < tInPar_p->phyUnitNum;
          i++
        )
    {
        u_int phyIndx = tInPhy_p[i].phyType - 1;
        T_phyValStrg* tPhyValS_p = & ( m_phyToPnt_p[phyIndx] );

        //��¼�ۼӴ���
        tPhyValS_p->acmTimes  = tInPhy_p[i].acmTimes;

        //�㷨 ������ֵ�ݶ����ڴ�����
        //tPhyValS_p->calcMethd = tInPhy_p[i].calcMethd;

        //copy ��������ֵ
        if ( tInPhy_p[i].phyObjVal_p != NULL )
        {
            memcpy ( tPhyValS_p->phyObjVal_p,
                     tInPhy_p[i].phyObjVal_p,
                     tPhyValS_p->phyValLng
                   );
            tPhyValS_p->freshFlag = true;
        }

        //copy���ַ��Ӧ��ͨ�ö���
        for ( u_int j = 0;
              j < tPhyValS_p->numOfAdd && tInPhy_p[i].pntObj_p != NULL;
              j++
            )
        {
            /*���������������ҵ����õĵ��ַ
            Ȼ����ݵ��ַ�ҵ��ַ����,�����µ��ַ��Ӧ�Ķ���
            */
            u_int pntIndx = 0;
            //PNT_ADDR_TYPE tPntAdd = tPhyValS_p->pntAddr_p[j];
            tRet = findPntIndx ( tPhyValS_p->pntAddr_p[j], pntIndx );

            if ( tRet != 0 )
            {
                //δ�ҵ����ַ������
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

            //copyʱ��
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


//����ͨ��ʱ��
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

/*��ȡ���������ֵ
   ���:inPar_p->phyUnitNum                 Ҫ��ȡ����������
           inPar_p->phyObjVal_p[i].phyType  Ҫ��ȡ������������
   ����:inPar_p������ֵ
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

    //����
    for ( u_int i = 0;
          tInPhyVal_p != NULL && i < tInPhyNVal_p->phyUnitNum;
          i++
        )
    {
        if ( ! ( isPhyTypeAvailable ( tInPhyVal_p[i].phyType, m_maxPhyType )
               )
           )
        {
            //��������Ч
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

        //copy ��������ֵ
        if ( tInPhyVal_p[i].phyObjVal_p != NULL )
        {
            memcpy ( tInPhyVal_p[i].phyObjVal_p,
                     tPhyStrg_p->phyObjVal_p,
                     tPhyStrg_p->phyValLng
                   );
        }

        //copy���ַ��Ӧ��ͨ�ö���
        for ( u_int j = 0;
              j < tPhyStrg_p->numOfAdd && tInPhyVal_p[i].pntObj_p != NULL;
              j++
            )
        {
            /*���ݵ��ַ�ҵ��ַ����,���get���ַ��Ӧ�Ķ���
            */
            u_int pntIndx = 0;
            //PNT_ADDR_TYPE tPntAdd = tPhyValS_p->pntAddr_p[j];
            tRet = findPntIndx ( tPhyStrg_p->pntAddr_p[j], pntIndx );

            if ( tRet != 0 )
            {
                //δ�ҵ����ַ������
                tPhyStrg_p->freshFlag = tInPhyVal_p[i].freshFlag;
                m_valLock.UnLock();
                return MC_ERROR_PNTAD;
            }

            COM_OBJ_TYPE* tRelateObj_p = & (
                                         m_pntStrgObj.pntMemObj_p[pntIndx].relateObj
                                         );
            char* tObj_p = ( ( char* ) tInPhyVal_p[i].pntObj_p ) + ( j * m_pntObjLng );
            //copyͨ�ö����з�ָ�����ֵ
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

            //copyʱ��
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


//��ȡͨ��ʱ��
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


//���г���,����Ҫ��,ֱ�Ӹ����
int C_chnUnitData::getPhyValADid ( const PHY_UNIT_TYPE phyType[],/*����������*/
                                   const u_char& num,             /*����������*/
                                   const u_char& onePhyVsize,
                                   void*  oPhyObjVal_p,/*phyObjVal[],��������ֵ*/
                                   u_char oPhyValLng[],/*ÿ��������ֵ�ĳ���*/
                                   void*  oDataId_p,   /*dataId[],��������Ӧ������id*/
                                   u_int* oDataIdLng_p   /*����id�ĳ���*/
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

    //����
    for ( u_int i = 0; i < num; i++ )
    {
        if ( ! ( isPhyTypeAvailable ( phyType[i], m_maxPhyType )
               )
           )
        {
            //��������Ч
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
            //copy��������ֵ
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

            //������ֵ�ĳ���
            if ( oPhyValLng != NULL )
            {
                oPhyValLng[i] = tPhyStrg_p->phyValLng;
            }

            //copy����id
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



int C_chnUnitData::getPhyValLng ( const PHY_UNIT_TYPE phyType[], /*����������*/
                                  const u_char& num,
                                  u_char oPhyValLng[]/*ÿ��������ֵ�ĳ���*/
                                ) const
{
    if ( phyType == NULL
         || num == 0
         || oPhyValLng == NULL
       )
    {
        return MC_ERROR_INPAR;
    }

    //����
    for ( u_int i = 0; i < num; i++ )
    {
        if ( ! ( isPhyTypeAvailable ( phyType[i], m_maxPhyType )
               )
           )
        {
            //��������Ч
            return MC_ERROR_PHYTY;
        }
    }

    for ( u_int i = 0; i < num; i++ )
    {
        u_int phyIndx = phyType[i] - 1;
        const T_phyValStrg* tPhyStrg_p = & ( m_phyToPnt_p[phyIndx] );
        //������ֵ�ĳ���
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

C_unitInfData::C_unitInfData ( const u_int& chnNumS /*ͨ������*/
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




//��ʼ��
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



//��ʼ��
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



/*����outData_p->phyTypeȡĳ��ͨ�����ض�������������ֵ
    ���:chnNum ͨ�����
        outData_p->phyType ��Ҫ���ýӿ�ǰ��ֵ
    ����:outData_p������ֵ
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
        //û�ҵ���Ӧͨ��������
        return MC_ERROR_CHNN0;
    }

    int tRet = m_chnUnitHead_p[curIndex].getPhyCfgByType ( outData_p );

    if ( tRet != 0 )
    {
        return tRet;
    }

    return 0;
}



/*���ݵ��ַȡĳ��ͨ��������������ֵ
    ���:chnNum ͨ�����
        pntAddVal_p ���ַ
       outInNum ����outData_p�Ĵ�С
    ����:outData_p
         outInNum ������ʵ�ʸ�ֵ�ĸ���
    ����ֵ:
        0: �ɹ�
        <0:ʧ��
        >0:ʵ��������������������������
    ����:outData_p
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
        //û�ҵ���Ӧͨ��������
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



/*���ݶ�����ַȡ������������(����ͬ�������Ѿ�ȥ����:
                  ������ͬһ��������ֻ����һ��)
                  ���:chnNum ͨ�����
                       pntAddVal_pp
                  ����:outNum_p   ������������
                       outPhyType_p (��ѡ)���˲�����Ϊ��ʱ���������������
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
        //û�ҵ���Ӧͨ��������
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


/*���ݶ�����ַȡ������������ֵ(����ͬ�������Ѿ�ȥ����:
        ������ͬһ��������ֻ����һ��)
        ���:chnNum ͨ�����
             pntAddVal_pp
        ����:outData_p
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
        //û�ҵ���Ӧͨ��������
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

void C_unitInfData::getChnNum ( u_int* chnNumS_p, /*ͨ������*/
                                u_int* curNumS_p  /*�Ѿ���ʼ��������*/
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




//�������������ֵ
int C_unitInfData::setM ( const u_int& chnNum,       /*��Ҫ��ֵ��ͨ����*/
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
        //û�ҵ���Ӧͨ��������
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




/*��ȡ���������ֵ
    ���:valChnNum
            cfgChnNum
            valNData_p->phyUnitNum              Ҫ��ȡ����������
            valNData_p->phyObjVal_p[i].phyType  Ҫ��ȡ������������
            cfgNPhyCfg_p->phyUnitNum             Ҫ��ȡ����������
            cfgNPhyCfg_p->phyValCfg_p[i].phyType Ҫ��ȡ������������
    ����:valNData_p������ֵ
            cfgNPhyCfg_p������ֵ
*/
int C_unitInfData::getM ( const u_int& valChnNum, /*Ҫȡֵ��ͨ����*/
                          const u_int& cfgChnNum,  /*Ҫȡ���õ�ͨ����*/
                          T_phyNVal*   valNData_p,/*������ֵ�ṹ*/
                          T_phyNCfg*   cfgNPhyCfg_p/*���������ýṹ*/
                        ) const
{
    //ֵ�����ò���ͬʱΪ��
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


    //ȡ�����������ֵ
    if ( tInValPar_p != NULL )
    {
        valIndex = findChnIndx ( valChnNum );

        if ( valIndex < 0 )
        {
            //û�ҵ���Ӧͨ��������
            return MC_ERROR_CHNN0;
        }

        tRet =  m_chnUnitHead_p[valIndex].getM ( tInValPar_p ) ;

        if ( tRet != 0 )
        {
            return tRet;
        }
    }

    //ȡ���������������
    if ( tInCfgPhyCfg_p != NULL )
    {
        cfgIndex = findChnIndx ( cfgChnNum );

        if ( cfgIndex < 0 )
        {
            //û�ҵ���Ӧͨ��������
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




//����ͨ��ʱ��
int C_unitInfData::setChnTmStmp ( const u_int& chnNum, /*��Ҫ��ֵ��ͨ����*/
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
        //û�ҵ���Ӧͨ��������
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



//��ȡͨ��ʱ��
int C_unitInfData::getChnTmStmp ( const u_int& fromChnNum, /*Ҫ���ĸ�ͨ����ȡֵ*/
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
        //û�ҵ���Ӧͨ��������
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



/*��ȡ���������ֵ
���г���,����Ҫ��,ֱ�Ӹ����
    ���:fromChnNum
            curChnNum
            fromNData_p->phyUnitNum              Ҫ��ȡ����������
            fromNData_p->phyObjVal_p[i].phyType  Ҫ��ȡ������������
            curNPhyCfg_p->phyUnitNum             Ҫ��ȡ����������
            curNPhyCfg_p->phyValCfg_p[i].phyType Ҫ��ȡ������������
    ����:fromNData_p������ֵ
            curNPhyCfg_p������ֵ
*/
int C_unitInfData:: getPhyValADid ( const u_int& chnNum,          /*ͨ����*/
                                    const PHY_UNIT_TYPE phyType[],/*����������*/
                                    const u_char& num,             /*����������*/
                                    const u_char& onePhyVsize,
                                    void*  oPhyObjVal_p,/*phyObjVal[],��������ֵ*/
                                    u_char oPhyValLng[],/*ÿ��������ֵ�ĳ���*/
                                    void*  oDataId_p,   /*dataId[],��������Ӧ������id*/
                                    u_int* oDataIdLng_p   /*����id�ĳ���*/
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
        //û�ҵ���Ӧͨ��������
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




int C_unitInfData::getPhyValLng ( const u_int& chnNum,           /*ͨ����*/
                                  const PHY_UNIT_TYPE phyType[], /*����������*/
                                  const u_char& num,
                                  u_char oPhyValLng[]/*ÿ��������ֵ�ĳ���*/
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
        //û�ҵ���Ӧͨ��������
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



const  PHY_UNIT_TYPE* C_unitInfData::getPhyCatBuf ( const u_int& chnNum,    /*ͨ����*/
        const int& phyCatName, /*�����������*/
        u_int& bufLng          /*����������buf����*/
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