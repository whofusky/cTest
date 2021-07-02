
#include "PDU.h"
#include <string.h>




/************************************************************************/
/*C_ModbusPDU ʵ��                                                             */
/************************************************************************/




C_ModbusPDU::C_ModbusPDU ( ADU_DSCR* cstmInfo_p,
                           u_char duType,
                           u_char allocFlag
                         )
{
    u_short length = 0;            //modbus ADU��ռ�ռ��ֽ���
    this->DU_Info = *cstmInfo_p;  //����ADU�����ṹ����
    this->DUType    = duType;
    this->allocFlag = allocFlag;

    //��������ADUϵͳ��ţ������ݸ�ϵͳ��ŵ�ADU���������㵱ǰԪ�صĳ���
    for ( int i = ADU_MBAPH_INDEX; i <= ADU_ECHK_INDEX; i++ )
    {
        if ( i == PDU_DATA_INDEX )
        {
            //�жϵ�ǰ�����Ƿ�ΪADU����������
            length += this->DU_Info.ADU_CMTBL[i] * this->DU_Info.InfoEleLng;
        }
        else
        {
            length += this->DU_Info.ADU_CMTBL[i];
        }
    }

    if ( allocFlag != 0 )
    {
        switch ( duType ) //����duType����ASDU���Ƿ���Ч
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

    switch ( this->DUType ) //����duType����ADU���Ƿ���Ч
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

        //ADU��Ԫ�ص�ƫ����
        u_int offset =  0;

        //����emt֮ǰ������Ԫ�أ�������ЩԪ����ռ�ռ����ƫ����
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

    switch ( this->DUType ) //����duType����ASDU���Ƿ���Ч
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

        //ADU��Ԫ�ص�ƫ����
        u_int offset = 0;

        //����emt֮ǰ������Ԫ�أ�������ЩԪ����ռ�ռ����ƫ����
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

        //���ADU�����еĳ��ȴ���ʵ��Ҫ���õĳ��ȣ�����ֵ���� 2��ʾ����С
        if ( adu_TSPlng > lng )
        {
            memcpy ( du_p + offset, p_Mem, lng );
            return 2;
        }
        //���ADU�����еĳ���С��ʵ��Ҫ���õĳ��ȣ�����ֵ���� 1��ʾ���ȹ�
        //�����ᱻ�ض�
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
        return -10;  //���ò��ɹ������� -10
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

    //��������ADUϵͳ��ţ������ݸ�ϵͳ��ŵ�ADU���������㵱ǰԪ�صĳ���
    for ( int i = ADU_MBAPH_INDEX; i <= ADU_ECHK_INDEX; i++ )
    {
        if ( i == PDU_DATA_INDEX )
        {
            //�жϵ�ǰ�����Ƿ�ΪADU����������
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
    u_short length = 0; //ADU��ռ�ռ��ֽ���
    int pduLngType = 0;
    u_char* du_p = NULL;

    if ( duType == ADU_TYPE ) //��ȡADUʵ�ʳ���
    {
        pduLngType = ADU_MBAPH_INDEX;
        du_p = this->ADU_p;
    }
    else if ( duType == PDU_TYPE ) //��ȡPDUʵ�ʳ���
    {
        pduLngType = ADU_ADDR_INDEX;
        du_p = this->PDU_p;
    }

    //��������ADUϵͳ��ţ������ݸ�ϵͳ��ŵ�ADU���������㵱ǰԪ�صĳ���
    for ( int i = pduLngType; i <= ADU_ECHK_INDEX; i++ )
    {
        if ( i == PDU_DATA_INDEX )
        {
            //�жϵ�ǰ�����Ƿ�ΪPDU����������
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

    //ADU��ռ�ռ��ֽ���
    u_short length = 0;

    //��������ADUϵͳ��ţ������ݸ�ϵͳ��ŵ�ADU���������㵱ǰԪ�صĳ���
    for ( int i = ADU_MBAPH_INDEX; i <= ADU_ECHK_INDEX; i++ )
    {
        if ( i == PDU_DATA_INDEX )
        {
            //�жϵ�ǰ�����Ƿ�ΪPDU����������
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



