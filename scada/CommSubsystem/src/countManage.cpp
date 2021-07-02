#include "countManage.h"

typedef struct
{
    u_int  byteL: 16;
    u_int byteH: 16;
} Bitint;


typedef struct
{
    u_int  byteL;
    u_int  byteH;
} Bitdouble;





countManage::countManage ( void )
{

}


countManage::~countManage ( void )
{

}


/*����ͨ�ú���(ͨ����ֵ�������������ֵ)
 * ���룺usrData_p:ͨ�����ݶ�������
 *���룺dtCfg_p�������������ýṹ
 *�����dtValue_p��������������
*/
int countManage::calcFuncAddToVal ( T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dtValue_p  )
{
    switch ( dtCfg_p->calcMethd )
    {
        case PHY_CALC_SAVE: //ֱ�ӱ���
            if ( dtCfg_p->numOfAdd > 1 )
            {
                return -1;
            }

            return calcSave ( usrData_p, dtCfg_p, dtValue_p );
            break;

        case PHY_CALC_CCNT: //����ֵ�����õ�˳������
            return calcCcnt ( usrData_p, dtCfg_p, dtValue_p );
            break;

        case PHY_CALC_SUM: //�ۼ�������
            return calcSum  ( usrData_p, dtCfg_p, dtValue_p );
            break;

        case PHY_CALC_CCNT_AND_SUM: //���㰴���õ�˳��ƴ�Ӳ��ۼ�������
            return calcCcntAndSum ( usrData_p, dtCfg_p, dtValue_p );
            break;

        case PHY_CALC_CCNT_AND_BIT: //���㰴����˳�򣨰�λƴ�ӣ�
            return calcCcntAndSumBit ( usrData_p, dtCfg_p, dtValue_p );
            break;

        default:
            break;
    }

    return 0;
}


/*����ͨ�ú���(�����������ֵ�����ַ�ĵ�ֵ)
 * ���룺usrData_p:ͨ�����ݶ�������
 *���룺dtCfg_p�������������ýṹ
 *�����dtValue_p��������������
*/
int countManage::calcFuncValToAdd ( T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dtValue_p )
{
    switch ( dtCfg_p->calcMethd )
    {
        case PHY_CALC_SAVE: //ֱ�ӱ���
            if ( dtCfg_p->numOfAdd > 1 )
            {

                return -1;
            }

            return calcTake ( usrData_p, dtCfg_p, dtValue_p );
            break;

        default:
            return -1;
            break;
    }

    return 0;
}


/*�������id�Ķ�Ӧֵ�������볤��
 * ���룺did_p:����id�Ķ����ƴ�
 *���룺dType������id����������
 *����: �������͵ĳ���
*/
int countManage::getDataType ( void* did_p, DATA_TYPE& dType )
{
    dType = DATATYPE_INVALID;

    if( did_p != NULL )
    {
        DID tDidObj;
        tDidObj.Import ( ( char* ) did_p );

        dType = ( DATA_TYPE ) tDidObj.ValueType();

        return tDidObj.ValueLength();
    }
    
    dType = DATATYPE_FLOAT64;
    return 8;
}


//�����������ֵ�����ַ�ĵ�ֵ--ֱ��ȡֵ
int countManage::calcTake (  T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dataVal_p)
{

    int tOneSize = sizeof ( usrData_p->gData.val_chkStd.pntVal1 );

    if ( dtCfg_p->phyValLng <= tOneSize )
    {
        memcpy ( & ( usrData_p->gData.val_chkStd.pntVal1 ), dataVal_p, dtCfg_p->phyValLng );
    }
    else if ( dtCfg_p->phyValLng <= ( 2 * tOneSize ) )
    {
        double dataValue = 0;
        memcpy ( & dataValue, dataVal_p->phyObjVal_p,  dataVal_p->phyValLng );
        float dtV = (float)dataValue;
        memcpy ( & ( usrData_p->gData.val_chkStd.pntVal1 ), &dtV, sizeof ( float ) );

        //memcpy ( & ( usrData_p->gData.val_chkStd.pntVal1 ), dataVal_p, tOneSize );
        //memcpy ( & ( usrData_p->gData.val_chkStd.pntVal2 ),
        //         ( char* ) dataVal_p + tOneSize,
        //         dtCfg_p->phyValLng - tOneSize
        //       );
    }
    else
    {
        return -1;
    }

    return 0;

}

//ͨ����ֵ�������������ֵ--ֱ�ӱ���
int countManage::calcSave ( T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dtValue_p )
{

    T_pntTmpVal tVal;
    memset ( &tVal, 0, sizeof ( tVal ) );
    DATA_TYPE tDType;
    //ȡ������id���õ���������
    int tDLng = getDataType ( dtCfg_p->dataId_p, tDType );

    if ( tDType == DATATYPE_UINT16 )
    {
        tVal.uInt16 = ( unsigned short ) usrData_p->gData.val_chkStd.pntVal1;
        tVal.uInt16 = tVal.uInt16 * dtCfg_p->multFactor;
    }
    else if ( tDType == DATATYPE_SINT16 )
    {
        tVal.sInt16 = ( short ) usrData_p->gData.val_chkStd.pntVal1;
        tVal.sInt16 = tVal.sInt16 * dtCfg_p->multFactor;
    }
    else if ( tDType == DATATYPE_UINT32 )
    {
        tVal.uInt32 = ( unsigned int ) usrData_p->gData.val_chkStd.pntVal1;
        tVal.uInt32 = tVal.uInt32 * dtCfg_p->multFactor;
    }
    else if ( tDType == DATATYPE_SINT32 )
    {
        tVal.sInt32 = ( int ) usrData_p->gData.val_chkStd.pntVal1;
        tVal.sInt32 = tVal.sInt32 * dtCfg_p->multFactor;
    }
    else if ( tDType == DATATYPE_FLOAT32 )
    {
        //tVal.sFlt32 = ( float ) usrData_p->gData.val_chkStd.pntVal1;
        float vFval = 0;
        memcpy ( &vFval,
            & ( usrData_p->gData.val_chkStd.pntVal1 ),
            sizeof ( usrData_p->gData.val_chkStd.pntVal1 )
            );
        tVal.sFlt32 = vFval;
        tVal.sFlt32 = tVal.sFlt32 * dtCfg_p->multFactor;
    }
    else if ( tDType == DATATYPE_FLOAT64 )
    {
        /*Bitdouble tDb;
        tDb.byteL = usrData_p->gData.val_chkStd.pntVal1;
        tDb.byteH = usrData_p->gData.val_chkStd.pntVal2;
        tVal.sFlt64 = * ( ( double* ) &tDb );*/
        float vFval = 0;
        memcpy ( &vFval,
                 & ( usrData_p->gData.val_chkStd.pntVal1 ),
                 sizeof ( usrData_p->gData.val_chkStd.pntVal1 )
               );
        tVal.sFlt64 = vFval;
        tVal.sFlt64 = tVal.sFlt64 * dtCfg_p->multFactor;
    }

    memset ( dtValue_p->phyObjVal_p, 0, dtCfg_p->phyValLng );
    memcpy ( dtValue_p->phyObjVal_p, &tVal, tDLng );
    dtValue_p->acmTimes = 1;

    return 0;

}


/*
ͨ����ֵ�������������ֵ--����ֵ�����õ�˳������
(�˽ӿ�һ������modbus�ӵ���������ƴ��)

ע:1.Ŀǰֻ֧�� ������ƴ��һ���������͵�ֵ,
   ������������,�����������ӿ���:calcSave
   2.ĿǰЭ��ֻ֧�ִ������ȵĸ�����,����˫����ʱ,��Ҫ�޸Ĵ˽ӿ��е�������߼�
     (�������id���ó���˫������:ʵ�ʴ洢ʱ�ǰѽ��յ��ĵ�����ֵת��˫���ȴ洢��,
      ������ʾ���յ��ľ���˫���ȵ�ֵ)
*/
int countManage::calcCcnt ( T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dataVal_p)
{
    //char data[4];
    Bitint data        = {0};
    int dataIndex      = 0;
    float dataValue    = 0;
    double dtV         = 0;

    //Ŀǰֻ֧�� ������ƴ��һ������(����)���͵�ֵ,������������,��Ҫ�޸Ĵ˺����߼�
    if ( dtCfg_p->numOfAdd > 2 )
    {
        return -3;
    }

    DATA_TYPE tDType;
    //ȡ������id���õ���������
    int tDLng = getDataType ( dtCfg_p->dataId_p, tDType );

    if ( tDType != DATATYPE_FLOAT32
         && tDType != DATATYPE_FLOAT64
       )
    {
        //�˽ӿ�ֻ���������͵���,����ֵ����������ʵ��,��:calcSave
        return -4;
    }



    for ( u_int i = 0; i < dtCfg_p->numOfAdd; i++ )
    {
        /*if ( dataIndex > 4 )
        {
            return -3;
        }*/
        if ( usrData_p[i].gData.pntType != 2 )
        {
            return -4;
        }

        if ( i == 0 )
        {
            u_short val =  usrData_p[i].gData.val_chkStd.pntVal1;
            data.byteL = val;
        }

        if ( i == 1 )
        {
            u_short val =  usrData_p[i].gData.val_chkStd.pntVal1;
            data.byteH = val;
        }

        //dataIndex += usrData_p[i].gData.pntType;
    }

    memset ( dataVal_p->phyObjVal_p, 0, dtCfg_p->phyValLng );

    if ( tDType == DATATYPE_FLOAT32 )
    {
        dataValue = * ( float* ) &data;
        dataValue = dataValue * dtCfg_p->multFactor;
        memcpy ( dataVal_p->phyObjVal_p, &dataValue, sizeof ( dataValue ) );
    }
    else // if ( tDType == DATATYPE_FLOAT64 )
    {
        dataValue = * ( float* ) &data;
        dtV = dataValue;
        dtV = dtV * dtCfg_p->multFactor;
        memcpy ( dataVal_p->phyObjVal_p, &dtV, sizeof ( dtV ) );
    }

    dataVal_p->acmTimes = 1;

    return 0 ;
}



//ͨ����ֵ�������������ֵ--�ۼ�������
int countManage::calcSum ( T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dtValue_p )
{
    T_pntTmpVal tVal;
    memset ( &tVal, 0, sizeof ( tVal ) );
    DATA_TYPE tDType;
    //ȡ������id���õ���������
    int tDLng = getDataType ( dtCfg_p->dataId_p, tDType );

    if ( tDType == DATATYPE_UINT16 )
    {
        tVal.uInt16 = ( unsigned short ) usrData_p->gData.val_chkStd.pntVal1;
        tVal.uInt16 = tVal.uInt16 * dtCfg_p->multFactor;

        if ( dtValue_p->acmTimes != 0 )
        {
            tVal.uInt16 = tVal.uInt16 + ( * ( unsigned short* ) ( dtValue_p->phyObjVal_p ) );
        }
    }
    else if ( tDType == DATATYPE_SINT16 )
    {
        tVal.sInt16 = ( short ) usrData_p->gData.val_chkStd.pntVal1;
        tVal.sInt16 = tVal.sInt16 * dtCfg_p->multFactor;

        if ( dtValue_p->acmTimes != 0 )
        {
            tVal.sInt16 = tVal.sInt16 + ( * ( short* ) ( dtValue_p->phyObjVal_p ) );
        }
    }
    else if ( tDType == DATATYPE_UINT32 )
    {
        tVal.uInt32 = ( unsigned int ) usrData_p->gData.val_chkStd.pntVal1;
        tVal.uInt32 = tVal.uInt32 * dtCfg_p->multFactor;

        if ( dtValue_p->acmTimes != 0 )
        {
            tVal.uInt32 = tVal.uInt32 + ( * ( unsigned int* ) ( dtValue_p->phyObjVal_p ) );
        }
    }
    else if ( tDType == DATATYPE_SINT32 )
    {
        tVal.sInt32 = ( int ) usrData_p->gData.val_chkStd.pntVal1;
        tVal.sInt32 = tVal.sInt32 * dtCfg_p->multFactor;

        if ( dtValue_p->acmTimes != 0 )
        {
            tVal.sInt32 = tVal.sInt32 + ( * ( int* ) ( dtValue_p->phyObjVal_p ) );
        }
    }
    else if ( tDType == DATATYPE_FLOAT32 )
    {
        float vFval = 0;
        memcpy ( &vFval,
            & ( usrData_p->gData.val_chkStd.pntVal1 ),
            sizeof ( usrData_p->gData.val_chkStd.pntVal1 )
            );
        //tVal.sFlt32 = ( float ) usrData_p->gData.val_chkStd.pntVal1;
        tVal.sFlt32 = vFval;
        tVal.sFlt32 = tVal.sFlt32 * dtCfg_p->multFactor;

        if ( dtValue_p->acmTimes != 0 )
        {
            tVal.sFlt32 = tVal.sFlt32 + ( * ( float* ) ( dtValue_p->phyObjVal_p ) );
        }
    }
    else if ( tDType == DATATYPE_FLOAT64 )
    {
        /*Bitdouble tDb = {0};
        tDb.byteL = usrData_p->gData.val_chkStd.pntVal1;
        tDb.byteH = usrData_p->gData.val_chkStd.pntVal2;
        tVal.sFlt64 = * ( ( double* ) &tDb );*/
        //tVal.sFlt64 = ( double ) usrData_p->gData.val_chkStd.pntVal1 ;
        float vFval = 0;
        memcpy ( &vFval,
            & ( usrData_p->gData.val_chkStd.pntVal1 ),
            sizeof ( usrData_p->gData.val_chkStd.pntVal1 )
            );
        tVal.sFlt64 = vFval;
        tVal.sFlt64 = tVal.sFlt64 * dtCfg_p->multFactor;

        if ( dtValue_p->acmTimes != 0 )
        {
            tVal.sFlt64 = tVal.sFlt64 + ( * ( double* ) ( dtValue_p->phyObjVal_p ) );
        }
    }
    else
    {
        return -4;
    }

    /* double dataValue = ( double ) usrData_p->gData.val_chkStd.pntVal1 ;

     if ( dtValue_p->acmTimes != 0 )
     {
         dataValue = ( dataValue * dtCfg_p->multFactor ) + ( * ( double* ) ( dtValue_p->phyObjVal_p ) );
     }
     else
     {
         dataValue = ( dataValue * dtCfg_p->multFactor );
     }*/

    memset ( dtValue_p->phyObjVal_p, 0, dtCfg_p->phyValLng );
    memcpy ( dtValue_p->phyObjVal_p, &tVal, tDLng );
    dtValue_p->acmTimes ++;

    return 0;
}


/*
ͨ����ֵ�������������ֵ--���㰴���õ�˳��ƴ�Ӳ��ۼ�������
(�˽ӿ�һ������modbus�ӵ���������ƴ��)

ע:1.Ŀǰֻ֧�� ������ƴ��һ���������͵�ֵ,
   ������������,�����������ӿ���:calcSave
   2.ĿǰЭ��ֻ֧�ִ������ȵĸ�����,����˫����ʱ,��Ҫ�޸Ĵ˽ӿ��е�������߼�
     (�������id���ó���˫������:ʵ�ʴ洢ʱ�ǰѽ��յ��ĵ�����ֵת��˫���ȴ洢��,
      ������ʾ���յ��ľ���˫���ȵ�ֵ)
*/
int countManage::calcCcntAndSum ( T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dtValue_p )
{
    Bitint data = {0};
    int dataIndex = 0;
    float dataValue = 0;
    double dtV = 0;

    //Ŀǰֻ֧�� ������ƴ��һ������(����)���͵�ֵ,������������,��Ҫ�޸Ĵ˺����߼�
    if ( dtCfg_p->numOfAdd > 2 )
    {
        return -3;
    }

    DATA_TYPE tDType;
    //ȡ������id���õ���������
    int tDLng = getDataType ( dtCfg_p->dataId_p, tDType );

    if ( tDType != DATATYPE_FLOAT32
         && tDType != DATATYPE_FLOAT64
       )
    {
        //�˽ӿ�ֻ���������͵���,����ֵ����������ʵ��,��:calcSave
        return -4;
    }

    for ( u_int i = 0; i < dtCfg_p->numOfAdd; i++ )
    {
        /*if ( dataIndex > 4 )
        {
            return -3;
        }*/
        if ( usrData_p[i].gData.pntType != 2 )
        {
            return -4;
        }

        if ( i == 0 )
        {
            u_short val = usrData_p[i].gData.val_chkStd.pntVal1;
            data.byteL = val;
        }

        if ( i == 1 )
        {
            u_short val =  usrData_p[i].gData.val_chkStd.pntVal1;
            data.byteH = val;
        }

        //dataIndex += usrData_p[i].gData.pntType;
    }

    if ( tDType == DATATYPE_FLOAT32 )
    {
        dataValue = * ( float* ) &data;
        dataValue = dataValue * dtCfg_p->multFactor;

        if ( dtValue_p->acmTimes != 0 )
        {
            dataValue = dataValue + ( * ( float* ) dtValue_p->phyObjVal_p );
        }

        memset ( dtValue_p->phyObjVal_p, 0, dtCfg_p->phyValLng );
        memcpy ( dtValue_p->phyObjVal_p, &dataValue, tDLng );
    }
    else // if ( tDType == DATATYPE_FLOAT64 )
    {
        dataValue = * ( float* ) &data;
        dtV = dataValue;
        dtV = dtV * dtCfg_p->multFactor;

        if ( dtValue_p->acmTimes != 0 )
        {
            dtV = dtV + ( * ( double* ) dtValue_p->phyObjVal_p );
        }

        memset ( dtValue_p->phyObjVal_p, 0, dtCfg_p->phyValLng );
        memcpy ( dtValue_p->phyObjVal_p, &dtV, tDLng );
    }

    /*dataValue = * ( float* ) &data;
    dtV = dataValue;

    if ( dtValue_p->acmTimes != 0 )
    {
        dtV = ( dtV * dtCfg_p->multFactor ) + ( * ( double* ) dtValue_p->phyObjVal_p );
    }
    else
    {
        dtV = ( dtV * dtCfg_p->multFactor ) ;
    }
    memcpy ( dtValue_p->phyObjVal_p, &dtV, dtCfg_p->phyValLng );*/


    dtValue_p->acmTimes ++;

    return 0;
}


//ͨ����ֵ�������������ֵ--���㰴����˳�򣨰�λƴ�ӣ�
//ע:���ֻ֧��32�����糬����ֵ��Ҫ�����޸Ĵ˽ӿ�
int countManage::calcCcntAndSumBit ( T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dtValue_p )
{
    u_int data = 0;
    //int dataIndex = 0;

    if ( dtCfg_p->numOfAdd > 32 )
    {
        //���֧��32����
        return -3;
    }

    for ( u_int i = 0; i < dtCfg_p->numOfAdd; i++ )
    {
        if ( usrData_p[i].gData.pntType > 1 ) //ע:�����1��ʾ1���ֽڶ�����1λ
        {
            return -4;
        }

        /*if ( dataIndex > 32 )
        {
            return -3;
        }*/

        data =  data | ( ( usrData_p[i].gData.val_chkStd.pntVal1 & 0x01 ) << i );

    }

    memcpy ( dtValue_p->phyObjVal_p, &data, dtCfg_p->phyValLng );

    return 0 ;
}
