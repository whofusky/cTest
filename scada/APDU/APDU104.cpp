#include "APDU104.h"




u_int getFrmType ( GEN_FRM ctlField, u_char* frmeType_p )
{
    if ( ! ( ctlField[0] & 0x1 ) )
    {
        *frmeType_p = 0;
        return 0;//OK
    }
    else
    {
        *frmeType_p = ctlField[0] & 0x3;

        if ( *frmeType_p == UNKNOW_F )
        {
            return 1;    //无法识别帧类型
        }
    }

    return 0;//OK
}




u_int getSNoFromGenFrm ( GEN_FRM ctlField, u_short* I_SNo_p )
{
    if ( ! ( ctlField[0] & 0x1 ) )
    {
        I_FRM* ctl_p = ( I_FRM* ) ctlField;
        //*I_SNo = ctl->lSNo| (ctl->hSNo<<7);
        *I_SNo_p = ctl_p->SNo;
    }
    else
    {
        *I_SNo_p = 0;
        return 1;//无发送序号的帧类型
    }

    return 0;//ok
}




u_int getRNoFromGenFrm ( GEN_FRM ctlField, u_short* I_RNo_p )
{
    if ( ( ctlField[0] & 0x3 ) !=
         3 ) //I帧或S帧，它们的接收序号存储位置相同
    {
        I_FRM* ctl_p = ( I_FRM* ) ctlField;
        //*I_RNo = ctl->lRNo| (ctl->hRNo<<7);
        *I_RNo_p = ctl_p->RNo;
    }
    else
    {
        *I_RNo_p = 0;
        return 1;//无接收序号的帧类型
    }

    return 0;//ok
}




u_int getU_FunFromGenFrm ( GEN_FRM ctlField, u_char* U_FunNo_p )
{
    if ( ( ctlField[0] & 0x3 ) == 3 )
    {
        *U_FunNo_p = ctlField[0] & 0xFC;

        switch ( *U_FunNo_p )
        {
            case U_SRTDT_ACT:
                break;

            case U_SRTDT_CON:
                break;

            case U_STPDT_ACT:
                break;

            case U_STPDT_CON:
                break;

            case U_TST_ACT:
                break;

            case U_TST_CON:
                break;

            default://不允许多个功能同时置位
            {
                *U_FunNo_p = 0x0;
                return 1;//err
            }
        }
    }
    else//非U帧
    {
        *U_FunNo_p = 0x0;
        return 1;//err
    }

    return 0;//ok
}




APDU104::APDU104 ( APDU_DSCR* cstmInfo_p,
                   u_char duType,
                   u_char  allocFlag
                 ) : APDU101 ( cstmInfo_p, duType, allocFlag )
{

}




APDU104::~APDU104()
{

}



