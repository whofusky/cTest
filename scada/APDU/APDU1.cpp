#include "APDU1.h"
#include <string.h>




/************************************************************************/
/* BS8s_Ss30 类实现                                                     */
/************************************************************************/

BS8s_Ss30::BS8s_Ss30 ( u_char num )
{
    this->signs = 0;

    if ( num < 0 || num > 7 )
    {
        this->values_p = NULL;
    }

    this->num     = num;
    this->values_p  = new BS8[num - 1]();
}


BS8s_Ss30::~BS8s_Ss30()
{
    delete [] values_p;
}

BS8_S BS8s_Ss30::getValues ( u_char bitsNo )
{
    BS8_S bs8_s = {0, 0, 0};

    if ( bitsNo >= num )
    {
        return bs8_s;
    }

    bs8_s.value         = * ( this->values_p + bitsNo ) ;
    u_char signValue    = this->signs & ( 1 << bitsNo );
    bs8_s.sign          = signValue >> bitsNo;
    return bs8_s;
}

u_int BS8s_Ss30::setValues ( u_char bitsNo, BS8_S* n9BitsValue_p )
{

    if ( bitsNo > num )
    {
        return -1;
    }

    if ( bitsNo == 0 )
    {
        for ( int i = 0; i < num; i++ )
        {
            * ( this->values_p + i ) = n9BitsValue_p->value;

            SetBit ( this->signs, i, n9BitsValue_p->sign );
        }
    }
    else
    {
        * ( this->values_p + ( bitsNo - 1 ) ) = n9BitsValue_p->value;

        SetBit ( this->signs, ( bitsNo - 1 ), n9BitsValue_p->sign );
    }

    return 0;
}

u_char BS8s_Ss30::getNumber()
{
    return num;
}





/************************************************************************/
/*带品质描述词的信息体                                                  */
/************************************************************************/


INFO_QUAL1::INFO_QUAL1()
{
    this->value_p = NULL;
    this->qual_p  = NULL;
}

INFO_QUAL1::~ INFO_QUAL1 ()
{

}

// bodyWidth为输出参数，表示信息体字节数；
//返回值：信息体指针。
BS8* INFO_QUAL1::getValue ( u_char* bodyWidth_p )
{
    bodyWidth_p = &num;
    return this->value_p;
}

QUAL1* INFO_QUAL1::getQual()
{
    return qual_p;
}

u_int INFO_QUAL1::setValue ( BS8* infoValue_p, u_char* bodyWidth_p )
{
    this->value_p   = infoValue_p;
    memcpy ( this->value_p, infoValue_p, *bodyWidth_p );
    this->num   = *bodyWidth_p;
    return 0;
}

u_int INFO_QUAL1::setQual ( QUAL1* infoQual_p )
{
    memcpy ( ( this->value_p + this->num - 1 ), infoQual_p, sizeof ( QUAL1 ) );
    qual_p = infoQual_p;
    return 0;
}






/************************************************************************/
/* 二进制计数器读数1、双极性二进制计数器读数1、BCD计数器读数1                                                                     */
/************************************************************************/

COUNTER1::COUNTER1 ( u_char cntrType, u_char num )
{
    this->cntrType = cntrType;
    this->num       = num;
}


COUNTER1::~COUNTER1()
{

}

u_int COUNTER1::getValue ( u_char* cntrType_p, u_char* cntrWidth_p )
{
    cntrWidth_p = &this->num;
    return this->value;
}

CNTR_STATE*  COUNTER1::getSeq ( u_char* cntrType_p )
{
    cntrType_p = &this->cntrType;
    return &this->cntrSeq;
}

u_int COUNTER1::setValue ( u_char cntrType, u_int intcntrValue,
                           u_char cntrWidth )
{
    this->cntrType  = cntrType;
    this->value     = intcntrValue;
    this->num       = cntrWidth;
    return 0;

}

u_int COUNTER1::setSeq ( CNTR_STATE cntrSeq )
{
    this->cntrSeq = cntrSeq;
    return 0;
}





/************************************************************************/
/*APDU 实现                                                             */
/************************************************************************/

APDU1::APDU1 ( APDU_DSCR* cstmInfo_p, u_char duType, u_char allocFlag )
{
    u_short length  = 0;             //APDU所占空间字节数
    this->DU_Info    = *cstmInfo_p; //保存APDU描述结构定义
    this->DUType     = duType;
    this->allocFlag = allocFlag;
    this->asduVsqVal = 0;

    /*遍历所有APDU系统编号，并根据该系统编号的APDU描述来
     计算当前元素的长度
    */
    for ( int i = APDU_APCI_INDEX; i <= ASDU_TSP_INDEX; i++ )
    {
        if ( i == BODY_SET_INDEX )
        {
            //判断当前描述是否为信息元素集描述
            length += ( this->DU_Info.ASDU_CMTBL[i]
                        * this->DU_Info.InfoEleLng );
        }
        else
        {
            /*if (i > ASDU_ADDR_INDEX
                && i < ASDU_TSP_INDEX
                && this->APDU_Info.ASDU_CMTBL[BODY_SET_INDEX] > 1
                )
            { 判断当前描述是否为信息体（对象）描述
                length += this->APDU_Info.ASDU_CMTBL[i]
                            * this->APDU_Info.ASDU_CMTBL[BODY_SET_INDEX];
            }
                else
            {*/
            length += this->DU_Info.ASDU_CMTBL[i];
            /*  }*/
        }
    }

    if ( allocFlag != 0 )
    {
        //this->APDU_p = new u_char[length](); //根据计算结果申请内存
        //this->APDU_Lng = length;             //保存内存长度
        switch ( duType )                      //根据duType设置ASDU域是否有效
        {
            case APDU_TYPE:
                this->APDU_p = new u_char[length]();
                this->ASDU_p = NULL;
                break;

            case ASDU_TYPE:
                this->APDU_p = NULL;
                this->ASDU_p = new u_char[length]();
                break;

            default:
                break;
        }
    }
    else
    {
        this->APDU_p = NULL;
        this->ASDU_p = NULL;
    }

    this->DU_Lng = length;
}



APDU1::~APDU1()
{
    if ( this->allocFlag != 0 )
    {
        if ( this->APDU_p != NULL )
        {
            delete[] this->APDU_p;
            this->APDU_p = NULL;
        }

        if ( this->ASDU_p != NULL )
        {
            delete[] this->ASDU_p;
            this->ASDU_p = NULL;
        }
    }

    //delete[] this;
}





APDU_DSCR* APDU1:: getDU_Info()
{
    return &this->DU_Info;
}




u_char* APDU1::getAPDU_ELEMENT ( u_short* lng_p,
                                 u_short emt,
                                 u_char index
                               )
{
    u_char* du_p = NULL;

    //根据duType设置ASDU域是否有效
    switch ( this->DUType )
    {
        case APDU_TYPE:
            du_p = this->APDU_p ;
            break;

        case ASDU_TYPE:
            du_p = this->ASDU_p ;
            break;

        default:
            break;
    }

    u_char* asduTSP = NULL;

    if ( lng_p == NULL )
    {
        return asduTSP;
    }

    //index 表示对象的索引下标(0开始)
    //u_char vObjNum = index +1;

    //取可变结构体限定词的SQ值
    u_char tSQVal = ( asduVsqVal >> 7 );
    //取可变结构体限定词的N值
    u_char tNVal = ( asduVsqVal & 0x7F );

    /*多对象结构时，  查询的索引
      不能大于ASDU中可变结构限定
      词的N值
    */
    if (  tSQVal == C_SQ_0
          && ( ( tNVal == 0 && index != 0 )
               || ( tNVal > 0 && index > tNVal - 1 )
             )
       )
    {
        return asduTSP;
    }

    if ( du_p != NULL &&  this->DU_Info.ASDU_CMTBL[emt] > 0 )
    {
        u_short asdu_ele_lng = 0 ;

        if ( emt == BODY_SET_INDEX )
        {
            if ( tSQVal == C_SQ_1 )
            {
                /*ASDU中信息体结构为单对象
                    :DU_Info.ASDU_CMTBL[emt] 表示单对象中元素的个数
                    :DU_Info.InfoEleLng 表示每个元素的长度
                */
                asdu_ele_lng = ( this->DU_Info.ASDU_CMTBL[emt]
                                 * this->DU_Info.InfoEleLng ) ;
            }
            else
            {
                /*ASDU中信息体结构为多对象
                    :DU_Info.ASDU_CMTBL[emt] 表示对象的个数
                    :DU_Info.InfoEleLng 表示每个对象中元素的长度
                */
                asdu_ele_lng = this->DU_Info.InfoEleLng;
            }
        }
        else
        {
            asdu_ele_lng = this->DU_Info.ASDU_CMTBL[emt] ;
        }

        //根据emt保存元素长度
        memcpy ( lng_p, &asdu_ele_lng, sizeof ( short ) );
        //APDU各元素的偏移量
        u_int offset =  0;

        //遍历emt之前的所有元素，根据这些元素所占空间计算偏移量
        for ( int i = APDU_APCI_INDEX; i < emt ; i ++ )
        {
            if ( i == BODY_SET_INDEX )
            {
                if ( tSQVal == C_SQ_1 )
                {
                    //单对象结构
                    offset += ( this->DU_Info.ASDU_CMTBL[i]
                                * this->DU_Info.InfoEleLng );
                }
                else
                {
                    //多对象结构
                    offset += this->DU_Info.InfoEleLng;
                }
            }
            else
            {
                offset += this->DU_Info.ASDU_CMTBL[i];
            }
        }

        /*多对象结构中计算第n个信息对象中的偏移
          = DUID的固定长度 + (n-1)*1个对象的长度 + 对象内的偏移
        */
        if ( emt > ASDU_ADDR_INDEX
             && tSQVal == C_SQ_0
             && index > 0
           )
        {
            //多对象结构
            //一个信息对象的长度
            u_char oneObjLen = ( this->DU_Info.ASDU_CMTBL[BODY_TID_INDEX]
                                 + this->DU_Info.ASDU_CMTBL[BODY_ADDR_INDEX]
                                 + this->DU_Info.InfoEleLng
                                 + this->DU_Info.ASDU_CMTBL[BODY_TSP_INDEX]
                                 //+ this->DU_Info.ASDU_CMTBL[ASDU_TSP_INDEX]
                               );
            offset += oneObjLen * index;
        }

        asduTSP = du_p + offset;
    }

    return asduTSP;
}




int APDU1::setAPDU_ELEMENT ( u_char* p_Mem,
                             u_short lng,
                             u_short emt,
                             u_char index
                           )
{
    u_char* du_p = NULL;

    //根据duType设置ASDU域是否有效
    switch ( this->DUType )
    {
        case APDU_TYPE:
            du_p = this->APDU_p ;
            break;

        case ASDU_TYPE:
            du_p = this->ASDU_p ;
            break;

        default:
            break;
    }

    /*如果set的是可变结构限定词VSQ则需要
      更新APDU1对象的中asduVsqVal
    */
    if ( emt == ASDU_VSQ_INDEX )
    {
        memcpy ( &asduVsqVal, p_Mem, sizeof ( asduVsqVal ) );
    }

    //index 表示对象的索引下标(0开始)
    //u_char vObjNum = index +1;

    //取可变结构体限定词的SQ值
    u_char tSQVal = ( asduVsqVal >> 7 );
    //取可变结构体限定词的N值
    u_char tNVal = ( asduVsqVal & 0x7F );

    /*多对象结构时，  查询的索引
      不能大于ASDU中可变结构限定
      词的N值
    */
    if (  tSQVal == C_SQ_0
          && ( ( tNVal == 0 && index != 0 )
               || ( tNVal > 0 && index > tNVal - 1 )
             )
       )
    {
        return -10;
    }

    if ( du_p != NULL && this->DU_Info.ASDU_CMTBL[emt] > 0 )
    {
        int asdu_TSPlng = 0;

        if ( emt != BODY_SET_INDEX )
        {
            asdu_TSPlng = this->DU_Info.ASDU_CMTBL[emt];
        }
        else
        {
            if ( tSQVal == C_SQ_1 )
            {
                //单对象结构
                asdu_TSPlng = ( this->DU_Info.ASDU_CMTBL[emt]
                                * this->DU_Info.InfoEleLng );
            }
            else
            {
                //多对象结构
                asdu_TSPlng = this->DU_Info.InfoEleLng;
            }
        }

        //APDU各元素的偏移量
        u_int offset = 0;

        //遍历emt之前的所有元素，根据这些元素所占空间计算偏移量
        for ( int i = 0; i < emt ; i ++ )
        {
            if ( i == BODY_SET_INDEX )
            {
                if ( tSQVal == C_SQ_1 )
                {
                    offset += ( this->DU_Info.ASDU_CMTBL[i]
                                * this->DU_Info.InfoEleLng );
                }
                else
                {
                    offset += this->DU_Info.InfoEleLng;
                }
            }
            else
            {
                offset += this->DU_Info.ASDU_CMTBL[i];
            }
        }

        /*多对象结构中计算第n个信息对象中的偏移
          = DUID的固定长度 + (n-1)*1个对象的长度 + 对象内的偏移
        */
        if ( emt > ASDU_ADDR_INDEX
             && tSQVal == C_SQ_0
             && index > 0
           )
        {
            //多对象结构
            //一个信息对象的长度
            u_char oneObjLen = ( this->DU_Info.ASDU_CMTBL[BODY_TID_INDEX]
                                 + this->DU_Info.ASDU_CMTBL[BODY_ADDR_INDEX]
                                 + this->DU_Info.InfoEleLng
                                 + this->DU_Info.ASDU_CMTBL[BODY_TSP_INDEX]
                                 //+ this->DU_Info.ASDU_CMTBL[ASDU_TSP_INDEX]
                               );
            offset += oneObjLen * index;
        }

        if ( asdu_TSPlng > lng )
        {
            /*如果APDU描述中的长度大于实际要设置
            的长度，拷贝值返回 2表示长度小
            */
            memcpy ( du_p + offset, p_Mem, lng );
            return 2;
        }
        else if ( asdu_TSPlng < lng )
        {
            /*如果APDU描述中的长度小于实际要设置的长度，
            拷贝值返回 1表示长度过长，会被截断
            */
            memcpy ( du_p + offset, p_Mem, asdu_TSPlng );
            return 1;
        }
        else
        {
            memcpy ( du_p + offset, p_Mem, asdu_TSPlng );

            return 0;
        }
    }
    else
    {
        return -10; //设置不成功，返回 -10
    }

    return 0;
}




u_short APDU1::getDU_Lng ()
{
    return this->DU_Lng;
}




u_char APDU1::getDUType()
{
    return this->DUType;
}




u_char* APDU1::getDU ( char duType )
{
    u_char* du_p = NULL;

    if ( duType == APDU_TYPE )
    {
        //获取APDU实际长度
        du_p = this->APDU_p;
    }
    else if ( duType == ASDU_TYPE )
    {
        //获取ASDU实际长度
        du_p = this->ASDU_p;
    }

    return du_p;
}




u_char* APDU1::getDU ( u_short& apduLng, char duType )
{
    //APDU所占空间字节数
    u_short length = 0;

    int pduLngType = 0;
    u_char* du_p = NULL;

    if ( duType == APDU_TYPE )
    {
        //获取APDU

        pduLngType = APDU_APCI_INDEX;
        du_p = this->APDU_p;
    }
    else if ( duType == ASDU_TYPE )
    {
        //获取ASDU

        pduLngType = ASDU_TID_INDEX;
        du_p = this->ASDU_p;
    }

    //取可变结构体限定词的SQ值
    u_char tSQVal = ( asduVsqVal >> 7 );
    //取可变结构体限定词的N值
    u_char tNVal = ( asduVsqVal & 0x7F );

    /*遍历所有APDU系统编号，并根据该系统编号的APDU
    描述来计算当前元素的长度
    */
    if ( tSQVal == C_SQ_1 )
    {
        //单信息对象结构
        for ( int i = pduLngType; i <= ASDU_TSP_INDEX; i++ )
        {
            //判断当前描述是否为信息元素集描述
            if ( i == BODY_SET_INDEX )
            {
                length += ( this->DU_Info.ASDU_CMTBL[i]
                            * this->DU_Info.InfoEleLng );
            }
            else
            {
                length += this->DU_Info.ASDU_CMTBL[i];
            }
        }
    }
    else
    {
        //多信息对象结构

        /*长度 = DUID固定长度 + N*1个信息对象的长度 + ASDU时标长度
        */

        //DUID固定长度
        u_char vDUIDlen = 0;

        for ( int i = pduLngType; i <= ASDU_ADDR_INDEX; i++ )
        {
            vDUIDlen += this->DU_Info.ASDU_CMTBL[i];
        }

        //DUID固定长度 + ASDU时标长度
        vDUIDlen += this->DU_Info.ASDU_CMTBL[ASDU_TSP_INDEX];

        //一个信息对象的长度
        u_char oneObjLen = ( this->DU_Info.ASDU_CMTBL[BODY_TID_INDEX]
                             + this->DU_Info.ASDU_CMTBL[BODY_ADDR_INDEX]
                             + this->DU_Info.InfoEleLng
                             + this->DU_Info.ASDU_CMTBL[BODY_TSP_INDEX]
                             //+ this->DU_Info.ASDU_CMTBL[ASDU_TSP_INDEX]
                           );
        length = vDUIDlen + ( oneObjLen * tNVal );
    }


    apduLng = length;
    //memcpy(apduLng ,&length , sizeof(u_short));

    return du_p;

}





ASDU_DUT* APDU1::getASDU_DUT()
{
    u_char* du_p = NULL;

    //根据duType设置ASDU域是否有效
    switch ( this->DUType )
    {
        case APDU_TYPE:
            du_p = this->APDU_p ;
            break;

        case ASDU_TYPE:
            du_p = this->ASDU_p ;
            break;

        default:
            break;
    }

    ASDU_DUT* asdu_DUT_p = new ASDU_DUT();

    asdu_DUT_p->SYS_ELENum = 0;
    // 保存APDU的系统编号
    int eleID [3];

    /*遍历数据单元类型的所有元素描述，如果描述的元素被启用
    则保存它的系统编号
    */
    for ( int i = ASDU_TID_INDEX; i <= ASDU_VSQ_INDEX; i++ )
    {
        //判断编号中的元素是否被启用
        if ( this->DU_Info.ASDU_CMTBL[i] > 0 )
        {
            eleID[asdu_DUT_p->SYS_ELENum] = i;
            asdu_DUT_p->SYS_ELENum += 1;
        }
    }

    //根据数据单元类型中被启用的元素个数申请内存
    asdu_DUT_p->SYS_TYPE_p = new ELE_TYPE[asdu_DUT_p->SYS_ELENum]();
    //数据单元类型的首地址
    u_char* memory_p = du_p + this->DU_Info.ASDU_CMTBL[APDU_APCI_INDEX] ;

    /*遍历数据单元类型的所有元素，将这些元素中的系统编
    号、大小以及内存地址保存到asdu_DUT中
    */
    for ( int j = 0; j < asdu_DUT_p->SYS_ELENum; j++ )
    {
        asdu_DUT_p->SYS_TYPE_p[j].sysEleID = eleID[j];
        asdu_DUT_p->SYS_TYPE_p[j].lng = this->DU_Info.ASDU_CMTBL[eleID[j]];
        asdu_DUT_p->SYS_TYPE_p[j].p_Mem = memory_p;
        memory_p += asdu_DUT_p->SYS_TYPE_p[j].lng;
    }

    return asdu_DUT_p;

}




u_int APDU1::freeASDU_DUT ( ASDU_DUT* pSpace )
{
    if ( pSpace != NULL )
    {
        if ( pSpace->SYS_TYPE_p != NULL )
        {
            delete [] pSpace->SYS_TYPE_p;
        }

        delete [] pSpace;
        pSpace->SYS_TYPE_p = NULL;
        pSpace = NULL;
    }

    return 0;
}





ASDU_DUID* APDU1::getASDU_DUID()
{
    u_char* du_p = NULL;

    //根据duType设置ASDU域是否有效
    switch ( this->DUType )
    {
        case APDU_TYPE:
            du_p = this->APDU_p ;
            break;

        case ASDU_TYPE:
            du_p = this->ASDU_p ;
            break;

        default:
            break;
    }

    ASDU_DUT* asdu_DUID_p = new ASDU_DUT();

    asdu_DUID_p->SYS_ELENum = 0;
    // 保存APDU的系统编号
    int eleID [5];

    /*遍历数据单元标识符的所有元素描述，如果描述的元素被
    启用则保存它的系统编号
    */
    for ( int i = ASDU_TID_INDEX; i <= ASDU_ADDR_INDEX; i++ )
    {
        //判断编号中的元素是否被启用
        if ( this->DU_Info.ASDU_CMTBL[i] > 0 )
        {
            eleID[asdu_DUID_p->SYS_ELENum] = i;
            asdu_DUID_p->SYS_ELENum += 1;
        }
    }

    //根据数据单元标识符中被启用的元素个数申请内存
    asdu_DUID_p->SYS_TYPE_p = new ELE_TYPE[asdu_DUID_p->SYS_ELENum]();
    u_char* memory_p = du_p + this->DU_Info.ASDU_CMTBL[APDU_APCI_INDEX];

    /*遍历数据单元标识符的所有元素，将这些元素中的系统
    编号、大小以及内存地址保存到asdu_DUID中
    */
    for ( int j = 0; j < asdu_DUID_p->SYS_ELENum; j++ )
    {
        asdu_DUID_p->SYS_TYPE_p[j].sysEleID = eleID[j];
        asdu_DUID_p->SYS_TYPE_p[j].lng = this->DU_Info.ASDU_CMTBL[eleID[j]];
        asdu_DUID_p->SYS_TYPE_p[j].p_Mem = memory_p;
        memory_p += asdu_DUID_p->SYS_TYPE_p[j].lng;
    }

    return asdu_DUID_p;

}




u_int APDU1::freeASDU_DUID ( ASDU_DUID* pSpace )
{
    if ( pSpace != NULL )
    {
        if ( pSpace->SYS_TYPE_p != NULL )
        {
            delete [] pSpace->SYS_TYPE_p;
        }

        delete [] pSpace;
        pSpace->SYS_TYPE_p = NULL;
        pSpace = NULL;
    }

    return 0;
}




INF_BODY* APDU1::getINF_BODY ( u_char index )
{
    u_char* du_p = NULL;

    //根据duType设置ASDU域是否有效
    switch ( this->DUType )
    {
        case APDU_TYPE:
            du_p = this->APDU_p ;
            break;

        case ASDU_TYPE:
            du_p = this->ASDU_p ;
            break;

        default:
            break;
    }

    u_short offset = 0;
    ASDU_DUT* asdu_BODY_p = NULL;

    //取可变结构体限定词的SQ值
    u_char tSQVal = ( asduVsqVal >> 7 );
    //取可变结构体限定词的N值
    u_char tNVal = ( asduVsqVal & 0x7F );

    /*多对象结构时，  查询的索引
      不能大于ASDU中可变结构限定
      词的N值
    */
    if (  tSQVal == C_SQ_0
          && ( ( tNVal == 0 && index != 0 )
               || ( tNVal > 0 && index > tNVal - 1 )
             )
       )
    {
        return asdu_BODY_p;
    }

    asdu_BODY_p = new ASDU_DUT();

    asdu_BODY_p->SYS_ELENum = 0;
    // 保存APDU的系统编号
    int eleID [4];

    /*遍历信息体（对象）的所有元素描述，如果描述的元素被
      启用则保存它的系统编号
    */
    for ( int i = BODY_TID_INDEX; i <= BODY_TSP_INDEX; i++ )
    {
        if ( this->DU_Info.ASDU_CMTBL[i] > 0 )
        {
            //判断编号中的元素是否被启用
            eleID[asdu_BODY_p->SYS_ELENum] = i;
            asdu_BODY_p->SYS_ELENum += 1;
        }
    }

    //根据信息体（对象）中被启用的元素个数申请内存
    asdu_BODY_p->SYS_TYPE_p = new ELE_TYPE[asdu_BODY_p->SYS_ELENum]();

    u_char* memory_p = NULL;

    //计算信息对象偏移量
    for ( int j = APDU_APCI_INDEX; j <= ASDU_ADDR_INDEX; j++ )
    {
        offset += this->DU_Info.ASDU_CMTBL[j];
    }

    if ( tSQVal == C_SQ_0
         && index > 0
       )
    {
        //多对象结构
        //一个信息对象的长度
        u_char oneObjLen = ( this->DU_Info.ASDU_CMTBL[BODY_TID_INDEX]
                             + this->DU_Info.ASDU_CMTBL[BODY_ADDR_INDEX]
                             + this->DU_Info.InfoEleLng
                             + this->DU_Info.ASDU_CMTBL[BODY_TSP_INDEX]
                             //+ this->DU_Info.ASDU_CMTBL[ASDU_TSP_INDEX]
                           );
        offset += oneObjLen * index;
    }

    memory_p  =  du_p + offset ;

    /*遍历信息体（对象）的所有元素，将这些元素中的系
      统编号、大小以及内存地址保存到asdu_DUID中
    */
    for ( int k = 0; k < asdu_BODY_p->SYS_ELENum; k++ )
    {
        asdu_BODY_p->SYS_TYPE_p[k].sysEleID = eleID[k];
        asdu_BODY_p->SYS_TYPE_p[k].p_Mem = memory_p;

        if ( k != BODY_SET_INDEX )
        {
            asdu_BODY_p->SYS_TYPE_p[k].lng = (
                                             this->DU_Info.ASDU_CMTBL[eleID[k]] );
            memory_p += asdu_BODY_p->SYS_TYPE_p[k].lng;
        }
        else
        {
            if ( tSQVal == C_SQ_0 )
            {
                /*多对象结构中一个信息体只有一个元素
                  DU_Info.InfoEleLng表示一个元素的长度
                */
                asdu_BODY_p->SYS_TYPE_p[k].lng = (
                                                 this->DU_Info.InfoEleLng );
            }
            else
            {
                asdu_BODY_p->SYS_TYPE_p[k].lng = (
                                                 this->DU_Info.ASDU_CMTBL[eleID[k]]
                                                 * this->DU_Info.InfoEleLng );
            }

            memory_p += asdu_BODY_p->SYS_TYPE_p[k].lng;
        }
    }

    return asdu_BODY_p;

}




u_int APDU1::freeINF_BODY ( INF_BODY* pSpace )
{
    if ( pSpace != NULL )
    {
        if ( pSpace->SYS_TYPE_p != NULL )
        {
            delete [] pSpace->SYS_TYPE_p;
        }

        delete [] pSpace;
        pSpace->SYS_TYPE_p = NULL;
        pSpace = NULL;
    }

    return 0;
}




u_int APDU1::freeAPDU ( INF_BODY* pSpace )
{
    if ( this->APDU_p != NULL )
    {
        delete []this->APDU_p;
    }

    if ( this->ASDU_p != NULL )
    {
        delete []this->APDU_p;
    }

    return 0;
}




u_int APDU1::bindToMem ( u_char* memBlock_p, u_char duType, u_short APDU_Lng )
{
    //APDU所占空间字节数
    u_short length = 0;

    //取可变结构体限定词的SQ值
    u_char tSQVal = ( asduVsqVal >> 7 );

    /*遍历所有APDU系统编号，并根据该系统编号的APDU描述来
      计算当前元素的长度
    */
    for ( int i = APDU_APCI_INDEX; i <= ASDU_TSP_INDEX; i++ )
    {
        if ( i == BODY_SET_INDEX )
        {
            //判断当前描述是否为信息元素集描述
            if ( tSQVal == C_SQ_0 )
            {
                /*多对象结构中一个信息体只有一个元素
                  DU_Info.InfoEleLng表示一个元素的长度
                */
                length += this->DU_Info.InfoEleLng;
            }
            else
            {
                length += ( this->DU_Info.ASDU_CMTBL[i]
                            * this->DU_Info.InfoEleLng );
            }
        }
        else
        {
            length += this->DU_Info.ASDU_CMTBL[i];
        }
    }

    if ( length == 0 )
    {
        return -1;
    }

    if ( duType == APDU_TYPE && this->APDU_p == memBlock_p )
    {
        return 0 ;
    }

    if ( duType == ASDU_TYPE && this->ASDU_p == memBlock_p )
    {
        return 0 ;
    }

    this->DUType = duType;

    switch ( this->DUType )
    {
        case APDU_TYPE:
            this->APDU_p = memBlock_p;
            this->ASDU_p = NULL;
            break;

        case ASDU_TYPE:
            this->APDU_p = NULL;
            this->ASDU_p = memBlock_p ;
            break;

        default:
            break;
    }

    return 0;
}


