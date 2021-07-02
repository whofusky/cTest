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


/*计算通用函数(通过点值计算出物理量的值)
 * 输入：usrData_p:通用数据对象数组
 *输入：dtCfg_p：物理量的配置结构
 *输出：dtValue_p：计算后的物理量
*/
int countManage::calcFuncAddToVal ( T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dtValue_p  )
{
    switch ( dtCfg_p->calcMethd )
    {
        case PHY_CALC_SAVE: //直接保存
            if ( dtCfg_p->numOfAdd > 1 )
            {
                return -1;
            }

            return calcSave ( usrData_p, dtCfg_p, dtValue_p );
            break;

        case PHY_CALC_CCNT: //将点值按配置的顺序连接
            return calcCcnt ( usrData_p, dtCfg_p, dtValue_p );
            break;

        case PHY_CALC_SUM: //累加做除法
            return calcSum  ( usrData_p, dtCfg_p, dtValue_p );
            break;

        case PHY_CALC_CCNT_AND_SUM: //将点按配置的顺序拼接并累加做除法
            return calcCcntAndSum ( usrData_p, dtCfg_p, dtValue_p );
            break;

        case PHY_CALC_CCNT_AND_BIT: //将点按配置顺序（按位拼接）
            return calcCcntAndSumBit ( usrData_p, dtCfg_p, dtValue_p );
            break;

        default:
            break;
    }

    return 0;
}


/*计算通用函数(拆分物理量的值到点地址的点值)
 * 输入：usrData_p:通用数据对象数组
 *输入：dtCfg_p：物理量的配置结构
 *输出：dtValue_p：计算后的物理量
*/
int countManage::calcFuncValToAdd ( T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dtValue_p )
{
    switch ( dtCfg_p->calcMethd )
    {
        case PHY_CALC_SAVE: //直接保存
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


/*获得数据id的对应值的类型与长度
 * 输入：did_p:数据id的二进制串
 *输入：dType：数据id的数据类型
 *返回: 数据类型的长度
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


//拆分物理量的值到点地址的点值--直接取值
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

//通过点值计算出物理量的值--直接保存
int countManage::calcSave ( T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dtValue_p )
{

    T_pntTmpVal tVal;
    memset ( &tVal, 0, sizeof ( tVal ) );
    DATA_TYPE tDType;
    //取得数据id配置的数据类型
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
通过点值计算出物理量的值--将点值按配置的顺序连接
(此接口一般用于modbus接到的数进行拼接)

注:1.目前只支持 两个点拼成一个浮点类型的值,
   如有其他需求,考虑用其他接口如:calcSave
   2.目前协议只支持处理单精度的浮点数,如有双精度时,需要修改此接口中点个数的逻辑
     (如果数据id配置成了双精度则:实际存储时是把接收到的单精度值转成双精度存储的,
      而不表示接收到的就是双精度的值)
*/
int countManage::calcCcnt ( T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dataVal_p)
{
    //char data[4];
    Bitint data        = {0};
    int dataIndex      = 0;
    float dataValue    = 0;
    double dtV         = 0;

    //目前只支持 两个点拼成一个浮点(单精)类型的值,如有其他需求,需要修改此函数逻辑
    if ( dtCfg_p->numOfAdd > 2 )
    {
        return -3;
    }

    DATA_TYPE tDType;
    //取得数据id配置的数据类型
    int tDLng = getDataType ( dtCfg_p->dataId_p, tDType );

    if ( tDType != DATATYPE_FLOAT32
         && tDType != DATATYPE_FLOAT64
       )
    {
        //此接口只处理浮点类型的数,其他值用其他方法实现,如:calcSave
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



//通过点值计算出物理量的值--累加做除法
int countManage::calcSum ( T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dtValue_p )
{
    T_pntTmpVal tVal;
    memset ( &tVal, 0, sizeof ( tVal ) );
    DATA_TYPE tDType;
    //取得数据id配置的数据类型
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
通过点值计算出物理量的值--将点按配置的顺序拼接并累加做除法
(此接口一般用于modbus接到的数进行拼接)

注:1.目前只支持 两个点拼成一个浮点类型的值,
   如有其他需求,考虑用其他接口如:calcSave
   2.目前协议只支持处理单精度的浮点数,如有双精度时,需要修改此接口中点个数的逻辑
     (如果数据id配置成了双精度则:实际存储时是把接收到的单精度值转成双精度存储的,
      而不表示接收到的就是双精度的值)
*/
int countManage::calcCcntAndSum ( T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dtValue_p )
{
    Bitint data = {0};
    int dataIndex = 0;
    float dataValue = 0;
    double dtV = 0;

    //目前只支持 两个点拼成一个浮点(单精)类型的值,如有其他需求,需要修改此函数逻辑
    if ( dtCfg_p->numOfAdd > 2 )
    {
        return -3;
    }

    DATA_TYPE tDType;
    //取得数据id配置的数据类型
    int tDLng = getDataType ( dtCfg_p->dataId_p, tDType );

    if ( tDType != DATATYPE_FLOAT32
         && tDType != DATATYPE_FLOAT64
       )
    {
        //此接口只处理浮点类型的数,其他值用其他方法实现,如:calcSave
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


//通过点值计算出物理量的值--将点按配置顺序（按位拼接）
//注:最大只支持32个点如超过此值需要考虑修改此接口
int countManage::calcCcntAndSumBit ( T_snnUsrData* usrData_p, T_phyValCfg* dtCfg_p, T_phyObjVal* dtValue_p )
{
    u_int data = 0;
    //int dataIndex = 0;

    if ( dtCfg_p->numOfAdd > 32 )
    {
        //最大支持32个点
        return -3;
    }

    for ( u_int i = 0; i < dtCfg_p->numOfAdd; i++ )
    {
        if ( usrData_p[i].gData.pntType > 1 ) //注:这里的1表示1个字节而不是1位
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
