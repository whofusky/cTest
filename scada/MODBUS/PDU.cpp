
#include "PDU.h"
#include <string.h>




/************************************************************************/
/*C_ModbusPDU 实现                                                             */
/************************************************************************/




C_ModbusPDU::C_ModbusPDU ( ADU_DSCR* cstmInfo_p,
                           u_char duType,
                           u_char allocFlag
                         )
{
    u_short length = 0;            //modbus ADU所占空间字节数
    this->DU_Info = *cstmInfo_p;  //保存ADU描述结构定义
    this->DUType    = duType;
    this->allocFlag = allocFlag;

    //遍历所有ADU系统编号，并根据该系统编号的ADU描述来计算当前元素的长度
    for ( int i = ADU_MBAPH_INDEX; i <= ADU_ECHK_INDEX; i++ )
    {
        if ( i == PDU_DATA_INDEX )
        {
            //判断当前描述是否为ADU数据域描述
            length += this->DU_Info.ADU_CMTBL[i] * this->DU_Info.InfoEleLng;
        }
        else
        {
            length += this->DU_Info.ADU_CMTBL[i];
        }
    }

    if ( allocFlag != 0 )
    {
        switch ( duType ) //根据duType设置ASDU域是否有效
        {
            case ADU_TYPE:
                this->ADU_p = new u_char[length]();
                this->PDU_p = NULL;
                break;

            case PDU_TYPE:
                this->ADU_p = NULL;
                this->PDU_p = new u_char[length]();
                break;

            default:
                break;
        }
    }
    else
    {
        this->ADU_p = NULL;
        this->PDU_p = NULL;
    }

    this->DU_Lng     = length;
    this->DU_Ini_Lng = length;
}




C_ModbusPDU::~C_ModbusPDU()
{
    if ( this->allocFlag != 0 )
    {
        if ( this->ADU_p != NULL )
        {
            delete[] this->ADU_p;
            this->ADU_p = NULL;
        }

        if ( this->PDU_p != NULL )
        {
            delete[] this->PDU_p;
            this->PDU_p = NULL;
        }
    }

    //delete[] this;
}




ADU_DSCR* C_ModbusPDU:: getDU_Info()
{
    return &this->DU_Info;
}




u_char* C_ModbusPDU::getADU_ELEMENT ( u_short& lng_p,
                                      u_short emt
                                    )
{
    u_char* du_p = NULL;

    switch ( this->DUType ) //根据duType设置ADU域是否有效
    {
        case ADU_TYPE:
            du_p = this->ADU_p ;
            break;

        case PDU_TYPE:
            du_p = this->PDU_p ;
            break;

        default:
            break;
    }

    u_char* asduTSP = NULL;

    if (   du_p != NULL
           && this->DU_Info.ADU_CMTBL[emt] > 0
       )
    {
        //u_short asdu_ele_lng = 0 ;

        if ( emt == PDU_DATA_INDEX )
        {
            lng_p = this->DU_Info.ADU_CMTBL[emt] * this->DU_Info.InfoEleLng ;
        }
        else
        {
            lng_p = this->DU_Info.ADU_CMTBL[emt] ;
        }

        //ADU各元素的偏移量
        u_int offset =  0;

        //遍历emt之前的所有元素，根据这些元素所占空间计算偏移量
        for ( int i = ADU_MBAPH_INDEX; i < emt ; i ++ )
        {
            if ( i == PDU_DATA_INDEX )
            {
                offset += this->DU_Info.ADU_CMTBL[i] * this->DU_Info.InfoEleLng;
            }
            else
            {
                offset += this->DU_Info.ADU_CMTBL[i];
            }
        }

        asduTSP = du_p + offset;
    }

    return asduTSP;
}




int C_ModbusPDU::setADU_ELEMENT ( u_char* p_Mem,
                                  u_short lng,
                                  u_short emt
                                )
{
    u_char* du_p = NULL;

    switch ( this->DUType ) //根据duType设置ASDU域是否有效
    {
        case ADU_TYPE:
            du_p = this->ADU_p;
            break;

        case PDU_TYPE:
            du_p = this->PDU_p;
            break;

        default:
            break;
    }

    if ( du_p != NULL && this->DU_Info.ADU_CMTBL[emt] > 0 )
    {
        int adu_TSPlng = 0;

        if ( emt != PDU_DATA_INDEX )
        {
            adu_TSPlng = this->DU_Info.ADU_CMTBL[emt];
        }
        else
        {
            adu_TSPlng = this->DU_Info.ADU_CMTBL[emt] * this->DU_Info.InfoEleLng;
        }

        //ADU各元素的偏移量
        u_int offset = 0;

        //遍历emt之前的所有元素，根据这些元素所占空间计算偏移量
        for ( int i = 0; i < emt ; i ++ )
        {
            if ( i == PDU_DATA_INDEX )
            {
                offset += this->DU_Info.ADU_CMTBL[i] * this->DU_Info.InfoEleLng;
            }
            else
            {
                offset += this->DU_Info.ADU_CMTBL[i];
            }
        }

        //如果ADU描述中的长度大于实际要设置的长度，拷贝值返回 2表示长度小
        if ( adu_TSPlng > lng )
        {
            memcpy ( du_p + offset, p_Mem, lng );
            return 2;
        }
        //如果ADU描述中的长度小于实际要设置的长度，拷贝值返回 1表示长度过
        //长，会被截断
        else if ( adu_TSPlng < lng )
        {
            memcpy ( du_p + offset, p_Mem, adu_TSPlng );
            return 1;
        }
        //fusk 20180413104656@modify 'else' to 'else if( adu_TSPlng > 0 )'
        else if ( adu_TSPlng > 0 )
        {
            memcpy ( du_p + offset, p_Mem, adu_TSPlng );
            return 0;
        }

    }
    else
    {
        return -10;  //设置不成功，返回 -10
    }

    return 0;
}




PDU_FC_VAL* C_ModbusPDU::getPDU_FC_VAL ( u_short& lng_p )
{
    return ( PDU_FC_VAL* ) getADU_ELEMENT ( lng_p,
                                            PDU_FC_INDEX
                                          );
}




int C_ModbusPDU::setPDU_FC_VAL ( PDU_FC_VAL* p_Mem )
{
    return this->setADU_ELEMENT ( ( u_char* ) p_Mem,
                                  this->getDU_Info()->ADU_CMTBL[PDU_FC_INDEX],
                                  PDU_FC_INDEX
                                );
}




u_short C_ModbusPDU::getDU_Lng ()
{
    u_short length = 0;

    //遍历所有ADU系统编号，并根据该系统编号的ADU描述来计算当前元素的长度
    for ( int i = ADU_MBAPH_INDEX; i <= ADU_ECHK_INDEX; i++ )
    {
        if ( i == PDU_DATA_INDEX )
        {
            //判断当前描述是否为ADU数据域描述
            length += this->DU_Info.ADU_CMTBL[i] * this->DU_Info.InfoEleLng;
        }
        else
        {
            length += this->DU_Info.ADU_CMTBL[i];
        }
    }

    this->DU_Lng = length;

    return this->DU_Lng;
}




u_short C_ModbusPDU::getDU_Ini_Lng()
{
    return this->DU_Ini_Lng;
}




u_char C_ModbusPDU::getDUType()
{
    return this->DUType;
}




u_char* C_ModbusPDU::getDU ( char duType )
{
    u_char* du_p = NULL;

    if ( duType == ADU_TYPE )
    {
        du_p = this->ADU_p;
    }
    else if ( duType == PDU_TYPE )
    {
        du_p = this->PDU_p;
    }

    return du_p;
}




u_char* C_ModbusPDU::getDU ( u_short& aduLng,
                             char duType
                           )
{
    u_short length = 0; //ADU所占空间字节数
    int pduLngType = 0;
    u_char* du_p = NULL;

    if ( duType == ADU_TYPE ) //获取ADU实际长度
    {
        pduLngType = ADU_MBAPH_INDEX;
        du_p = this->ADU_p;
    }
    else if ( duType == PDU_TYPE ) //获取PDU实际长度
    {
        pduLngType = ADU_ADDR_INDEX;
        du_p = this->PDU_p;
    }

    //遍历所有ADU系统编号，并根据该系统编号的ADU描述来计算当前元素的长度
    for ( int i = pduLngType; i <= ADU_ECHK_INDEX; i++ )
    {
        if ( i == PDU_DATA_INDEX )
        {
            //判断当前描述是否为PDU数据域描述
            length += this->DU_Info.ADU_CMTBL[i] * this->DU_Info.InfoEleLng;
        }
        else
        {
            length += this->DU_Info.ADU_CMTBL[i];
        }
    }

    aduLng      = length;
    this->DU_Lng = length;

    return du_p;
}




u_int C_ModbusPDU::freeADU ( void )
{
    if ( this->ADU_p != NULL )
    {
        delete []this->ADU_p;
        this->ADU_p = NULL;
    }

    if ( this->PDU_p != NULL )
    {
        delete []this->PDU_p;
        this->PDU_p = NULL;
    }

    return 0;
}




int C_ModbusPDU::bindToMem ( u_char* memBlock_p,
                             u_char duType,
                             u_short ADU_Lng
                           )
{
    if ( duType != ADU_TYPE && duType != PDU_TYPE )
    {
        return -2;
    }

    //ADU所占空间字节数
    u_short length = 0;

    //遍历所有ADU系统编号，并根据该系统编号的ADU描述来计算当前元素的长度
    for ( int i = ADU_MBAPH_INDEX; i <= ADU_ECHK_INDEX; i++ )
    {
        if ( i == PDU_DATA_INDEX )
        {
            //判断当前描述是否为PDU数据域描述
            length += this->DU_Info.ADU_CMTBL[i] * this->DU_Info.InfoEleLng;
        }
        else
        {
            length += this->DU_Info.ADU_CMTBL[i];
        }
    }

    if ( length == 0 )
    {
        return -1;
    }

    this->DU_Lng = length;

    if ( duType == ADU_TYPE && this->ADU_p == memBlock_p )
    {
        return 0 ;
    }

    if ( duType == PDU_TYPE && this->PDU_p == memBlock_p )
    {
        return 0 ;
    }

    this->DUType = duType;

    switch ( this->DUType )
    {
        case ADU_TYPE:
            this->ADU_p = memBlock_p;
            this->PDU_p = NULL;
            break;

        case PDU_TYPE:
            this->PDU_p = NULL;
            this->ADU_p = memBlock_p ;
            break;

        default:
            break;
    }

    return 0;

}



