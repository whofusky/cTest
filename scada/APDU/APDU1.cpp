#include "APDU1.h"
#include <string.h>




/************************************************************************/
/* BS8s_Ss30 ��ʵ��                                                     */
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
/*��Ʒ�������ʵ���Ϣ��                                                  */
/************************************************************************/


INFO_QUAL1::INFO_QUAL1()
{
    this->value_p = NULL;
    this->qual_p  = NULL;
}

INFO_QUAL1::~ INFO_QUAL1 ()
{

}

// bodyWidthΪ�����������ʾ��Ϣ���ֽ�����
//����ֵ����Ϣ��ָ�롣
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
/* �����Ƽ���������1��˫���Զ����Ƽ���������1��BCD����������1                                                                     */
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
/*APDU ʵ��                                                             */
/************************************************************************/

APDU1::APDU1 ( APDU_DSCR* cstmInfo_p, u_char duType, u_char allocFlag )
{
    u_short length  = 0;             //APDU��ռ�ռ��ֽ���
    this->DU_Info    = *cstmInfo_p; //����APDU�����ṹ����
    this->DUType     = duType;
    this->allocFlag = allocFlag;
    this->asduVsqVal = 0;

    /*��������APDUϵͳ��ţ������ݸ�ϵͳ��ŵ�APDU������
     ���㵱ǰԪ�صĳ���
    */
    for ( int i = APDU_APCI_INDEX; i <= ASDU_TSP_INDEX; i++ )
    {
        if ( i == BODY_SET_INDEX )
        {
            //�жϵ�ǰ�����Ƿ�Ϊ��ϢԪ�ؼ�����
            length += ( this->DU_Info.ASDU_CMTBL[i]
                        * this->DU_Info.InfoEleLng );
        }
        else
        {
            /*if (i > ASDU_ADDR_INDEX
                && i < ASDU_TSP_INDEX
                && this->APDU_Info.ASDU_CMTBL[BODY_SET_INDEX] > 1
                )
            { �жϵ�ǰ�����Ƿ�Ϊ��Ϣ�壨��������
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
        //this->APDU_p = new u_char[length](); //���ݼ����������ڴ�
        //this->APDU_Lng = length;             //�����ڴ泤��
        switch ( duType )                      //����duType����ASDU���Ƿ���Ч
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

    //����duType����ASDU���Ƿ���Ч
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

    //index ��ʾ����������±�(0��ʼ)
    //u_char vObjNum = index +1;

    //ȡ�ɱ�ṹ���޶��ʵ�SQֵ
    u_char tSQVal = ( asduVsqVal >> 7 );
    //ȡ�ɱ�ṹ���޶��ʵ�Nֵ
    u_char tNVal = ( asduVsqVal & 0x7F );

    /*�����ṹʱ��  ��ѯ������
      ���ܴ���ASDU�пɱ�ṹ�޶�
      �ʵ�Nֵ
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
                /*ASDU����Ϣ��ṹΪ������
                    :DU_Info.ASDU_CMTBL[emt] ��ʾ��������Ԫ�صĸ���
                    :DU_Info.InfoEleLng ��ʾÿ��Ԫ�صĳ���
                */
                asdu_ele_lng = ( this->DU_Info.ASDU_CMTBL[emt]
                                 * this->DU_Info.InfoEleLng ) ;
            }
            else
            {
                /*ASDU����Ϣ��ṹΪ�����
                    :DU_Info.ASDU_CMTBL[emt] ��ʾ����ĸ���
                    :DU_Info.InfoEleLng ��ʾÿ��������Ԫ�صĳ���
                */
                asdu_ele_lng = this->DU_Info.InfoEleLng;
            }
        }
        else
        {
            asdu_ele_lng = this->DU_Info.ASDU_CMTBL[emt] ;
        }

        //����emt����Ԫ�س���
        memcpy ( lng_p, &asdu_ele_lng, sizeof ( short ) );
        //APDU��Ԫ�ص�ƫ����
        u_int offset =  0;

        //����emt֮ǰ������Ԫ�أ�������ЩԪ����ռ�ռ����ƫ����
        for ( int i = APDU_APCI_INDEX; i < emt ; i ++ )
        {
            if ( i == BODY_SET_INDEX )
            {
                if ( tSQVal == C_SQ_1 )
                {
                    //������ṹ
                    offset += ( this->DU_Info.ASDU_CMTBL[i]
                                * this->DU_Info.InfoEleLng );
                }
                else
                {
                    //�����ṹ
                    offset += this->DU_Info.InfoEleLng;
                }
            }
            else
            {
                offset += this->DU_Info.ASDU_CMTBL[i];
            }
        }

        /*�����ṹ�м����n����Ϣ�����е�ƫ��
          = DUID�Ĺ̶����� + (n-1)*1������ĳ��� + �����ڵ�ƫ��
        */
        if ( emt > ASDU_ADDR_INDEX
             && tSQVal == C_SQ_0
             && index > 0
           )
        {
            //�����ṹ
            //һ����Ϣ����ĳ���
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

    //����duType����ASDU���Ƿ���Ч
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

    /*���set���ǿɱ�ṹ�޶���VSQ����Ҫ
      ����APDU1�������asduVsqVal
    */
    if ( emt == ASDU_VSQ_INDEX )
    {
        memcpy ( &asduVsqVal, p_Mem, sizeof ( asduVsqVal ) );
    }

    //index ��ʾ����������±�(0��ʼ)
    //u_char vObjNum = index +1;

    //ȡ�ɱ�ṹ���޶��ʵ�SQֵ
    u_char tSQVal = ( asduVsqVal >> 7 );
    //ȡ�ɱ�ṹ���޶��ʵ�Nֵ
    u_char tNVal = ( asduVsqVal & 0x7F );

    /*�����ṹʱ��  ��ѯ������
      ���ܴ���ASDU�пɱ�ṹ�޶�
      �ʵ�Nֵ
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
                //������ṹ
                asdu_TSPlng = ( this->DU_Info.ASDU_CMTBL[emt]
                                * this->DU_Info.InfoEleLng );
            }
            else
            {
                //�����ṹ
                asdu_TSPlng = this->DU_Info.InfoEleLng;
            }
        }

        //APDU��Ԫ�ص�ƫ����
        u_int offset = 0;

        //����emt֮ǰ������Ԫ�أ�������ЩԪ����ռ�ռ����ƫ����
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

        /*�����ṹ�м����n����Ϣ�����е�ƫ��
          = DUID�Ĺ̶����� + (n-1)*1������ĳ��� + �����ڵ�ƫ��
        */
        if ( emt > ASDU_ADDR_INDEX
             && tSQVal == C_SQ_0
             && index > 0
           )
        {
            //�����ṹ
            //һ����Ϣ����ĳ���
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
            /*���APDU�����еĳ��ȴ���ʵ��Ҫ����
            �ĳ��ȣ�����ֵ���� 2��ʾ����С
            */
            memcpy ( du_p + offset, p_Mem, lng );
            return 2;
        }
        else if ( asdu_TSPlng < lng )
        {
            /*���APDU�����еĳ���С��ʵ��Ҫ���õĳ��ȣ�
            ����ֵ���� 1��ʾ���ȹ������ᱻ�ض�
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
        return -10; //���ò��ɹ������� -10
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
        //��ȡAPDUʵ�ʳ���
        du_p = this->APDU_p;
    }
    else if ( duType == ASDU_TYPE )
    {
        //��ȡASDUʵ�ʳ���
        du_p = this->ASDU_p;
    }

    return du_p;
}




u_char* APDU1::getDU ( u_short& apduLng, char duType )
{
    //APDU��ռ�ռ��ֽ���
    u_short length = 0;

    int pduLngType = 0;
    u_char* du_p = NULL;

    if ( duType == APDU_TYPE )
    {
        //��ȡAPDU

        pduLngType = APDU_APCI_INDEX;
        du_p = this->APDU_p;
    }
    else if ( duType == ASDU_TYPE )
    {
        //��ȡASDU

        pduLngType = ASDU_TID_INDEX;
        du_p = this->ASDU_p;
    }

    //ȡ�ɱ�ṹ���޶��ʵ�SQֵ
    u_char tSQVal = ( asduVsqVal >> 7 );
    //ȡ�ɱ�ṹ���޶��ʵ�Nֵ
    u_char tNVal = ( asduVsqVal & 0x7F );

    /*��������APDUϵͳ��ţ������ݸ�ϵͳ��ŵ�APDU
    ���������㵱ǰԪ�صĳ���
    */
    if ( tSQVal == C_SQ_1 )
    {
        //����Ϣ����ṹ
        for ( int i = pduLngType; i <= ASDU_TSP_INDEX; i++ )
        {
            //�жϵ�ǰ�����Ƿ�Ϊ��ϢԪ�ؼ�����
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
        //����Ϣ����ṹ

        /*���� = DUID�̶����� + N*1����Ϣ����ĳ��� + ASDUʱ�곤��
        */

        //DUID�̶�����
        u_char vDUIDlen = 0;

        for ( int i = pduLngType; i <= ASDU_ADDR_INDEX; i++ )
        {
            vDUIDlen += this->DU_Info.ASDU_CMTBL[i];
        }

        //DUID�̶����� + ASDUʱ�곤��
        vDUIDlen += this->DU_Info.ASDU_CMTBL[ASDU_TSP_INDEX];

        //һ����Ϣ����ĳ���
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

    //����duType����ASDU���Ƿ���Ч
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
    // ����APDU��ϵͳ���
    int eleID [3];

    /*�������ݵ�Ԫ���͵�����Ԫ�����������������Ԫ�ر�����
    �򱣴�����ϵͳ���
    */
    for ( int i = ASDU_TID_INDEX; i <= ASDU_VSQ_INDEX; i++ )
    {
        //�жϱ���е�Ԫ���Ƿ�����
        if ( this->DU_Info.ASDU_CMTBL[i] > 0 )
        {
            eleID[asdu_DUT_p->SYS_ELENum] = i;
            asdu_DUT_p->SYS_ELENum += 1;
        }
    }

    //�������ݵ�Ԫ�����б����õ�Ԫ�ظ��������ڴ�
    asdu_DUT_p->SYS_TYPE_p = new ELE_TYPE[asdu_DUT_p->SYS_ELENum]();
    //���ݵ�Ԫ���͵��׵�ַ
    u_char* memory_p = du_p + this->DU_Info.ASDU_CMTBL[APDU_APCI_INDEX] ;

    /*�������ݵ�Ԫ���͵�����Ԫ�أ�����ЩԪ���е�ϵͳ��
    �š���С�Լ��ڴ��ַ���浽asdu_DUT��
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

    //����duType����ASDU���Ƿ���Ч
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
    // ����APDU��ϵͳ���
    int eleID [5];

    /*�������ݵ�Ԫ��ʶ��������Ԫ�����������������Ԫ�ر�
    �����򱣴�����ϵͳ���
    */
    for ( int i = ASDU_TID_INDEX; i <= ASDU_ADDR_INDEX; i++ )
    {
        //�жϱ���е�Ԫ���Ƿ�����
        if ( this->DU_Info.ASDU_CMTBL[i] > 0 )
        {
            eleID[asdu_DUID_p->SYS_ELENum] = i;
            asdu_DUID_p->SYS_ELENum += 1;
        }
    }

    //�������ݵ�Ԫ��ʶ���б����õ�Ԫ�ظ��������ڴ�
    asdu_DUID_p->SYS_TYPE_p = new ELE_TYPE[asdu_DUID_p->SYS_ELENum]();
    u_char* memory_p = du_p + this->DU_Info.ASDU_CMTBL[APDU_APCI_INDEX];

    /*�������ݵ�Ԫ��ʶ��������Ԫ�أ�����ЩԪ���е�ϵͳ
    ��š���С�Լ��ڴ��ַ���浽asdu_DUID��
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

    //����duType����ASDU���Ƿ���Ч
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

    //ȡ�ɱ�ṹ���޶��ʵ�SQֵ
    u_char tSQVal = ( asduVsqVal >> 7 );
    //ȡ�ɱ�ṹ���޶��ʵ�Nֵ
    u_char tNVal = ( asduVsqVal & 0x7F );

    /*�����ṹʱ��  ��ѯ������
      ���ܴ���ASDU�пɱ�ṹ�޶�
      �ʵ�Nֵ
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
    // ����APDU��ϵͳ���
    int eleID [4];

    /*������Ϣ�壨���󣩵�����Ԫ�����������������Ԫ�ر�
      �����򱣴�����ϵͳ���
    */
    for ( int i = BODY_TID_INDEX; i <= BODY_TSP_INDEX; i++ )
    {
        if ( this->DU_Info.ASDU_CMTBL[i] > 0 )
        {
            //�жϱ���е�Ԫ���Ƿ�����
            eleID[asdu_BODY_p->SYS_ELENum] = i;
            asdu_BODY_p->SYS_ELENum += 1;
        }
    }

    //������Ϣ�壨�����б����õ�Ԫ�ظ��������ڴ�
    asdu_BODY_p->SYS_TYPE_p = new ELE_TYPE[asdu_BODY_p->SYS_ELENum]();

    u_char* memory_p = NULL;

    //������Ϣ����ƫ����
    for ( int j = APDU_APCI_INDEX; j <= ASDU_ADDR_INDEX; j++ )
    {
        offset += this->DU_Info.ASDU_CMTBL[j];
    }

    if ( tSQVal == C_SQ_0
         && index > 0
       )
    {
        //�����ṹ
        //һ����Ϣ����ĳ���
        u_char oneObjLen = ( this->DU_Info.ASDU_CMTBL[BODY_TID_INDEX]
                             + this->DU_Info.ASDU_CMTBL[BODY_ADDR_INDEX]
                             + this->DU_Info.InfoEleLng
                             + this->DU_Info.ASDU_CMTBL[BODY_TSP_INDEX]
                             //+ this->DU_Info.ASDU_CMTBL[ASDU_TSP_INDEX]
                           );
        offset += oneObjLen * index;
    }

    memory_p  =  du_p + offset ;

    /*������Ϣ�壨���󣩵�����Ԫ�أ�����ЩԪ���е�ϵ
      ͳ��š���С�Լ��ڴ��ַ���浽asdu_DUID��
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
                /*�����ṹ��һ����Ϣ��ֻ��һ��Ԫ��
                  DU_Info.InfoEleLng��ʾһ��Ԫ�صĳ���
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
    //APDU��ռ�ռ��ֽ���
    u_short length = 0;

    //ȡ�ɱ�ṹ���޶��ʵ�SQֵ
    u_char tSQVal = ( asduVsqVal >> 7 );

    /*��������APDUϵͳ��ţ������ݸ�ϵͳ��ŵ�APDU������
      ���㵱ǰԪ�صĳ���
    */
    for ( int i = APDU_APCI_INDEX; i <= ASDU_TSP_INDEX; i++ )
    {
        if ( i == BODY_SET_INDEX )
        {
            //�жϵ�ǰ�����Ƿ�Ϊ��ϢԪ�ؼ�����
            if ( tSQVal == C_SQ_0 )
            {
                /*�����ṹ��һ����Ϣ��ֻ��һ��Ԫ��
                  DU_Info.InfoEleLng��ʾһ��Ԫ�صĳ���
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


