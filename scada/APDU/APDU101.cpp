
#include "APDU101.h"
#include <string.h>




APDU101::APDU101 ( APDU_DSCR* cstmInfo_p,
                   u_char duType,
                   u_char  allocFlag
                 ) : APDU1 ( cstmInfo_p, duType, allocFlag )
{

}


APDU101:: ~APDU101()
{

}




ASDU_TID_VAL* APDU101::getASDU_TID_VAL ( u_short* lng_p )
{
    ASDU_TID_VAL* asdu_vsq_val = ( ASDU_TID_VAL* ) this->getAPDU_ELEMENT ( lng_p,
                                 ASDU_TID_INDEX
                                                                         );
    return asdu_vsq_val;
}


u_int APDU101::setASDU_TID_VAL ( ASDU_TID_VAL* p_Mem )
{
    return this->setAPDU_ELEMENT ( ( u_char* ) p_Mem,
                                   this->getDU_Info()->ASDU_CMTBL[ASDU_TID_INDEX],
                                   ASDU_TID_INDEX
                                 );
}




ASDU_VSQ_VAL* APDU101::getASDU_VSQ_VAL ( u_short* lng_p )
{
    return ( ASDU_VSQ_VAL* ) this->getAPDU_ELEMENT ( lng_p,
            ASDU_VSQ_INDEX
                                                   );
}


u_int  APDU101::setASDU_VSQ_VAL ( ASDU_VSQ_VAL* p_Mem )
{
    return this->setAPDU_ELEMENT ( ( u_char* ) p_Mem,
                                   this->getDU_Info()->ASDU_CMTBL[ASDU_VSQ_INDEX],
                                   ASDU_VSQ_INDEX
                                 );
}




void* APDU101:: getASDU_COT_VAL ( u_short* lng_p )
{
    this->getAPDU_ELEMENT ( lng_p, ASDU_COT_INDEX );

    if ( *lng_p == 1 )
    {
        ASDU_COT1_VAL* asdu_cot1_val;
        asdu_cot1_val = ( ASDU_COT1_VAL* ) this->getAPDU_ELEMENT ( lng_p,
                        ASDU_COT_INDEX
                                                                 );
        return asdu_cot1_val;
    }
    else
    {
        ASDU_COT2_VAL* asdu_cot2_val;
        asdu_cot2_val = ( ASDU_COT2_VAL* ) this->getAPDU_ELEMENT ( lng_p,
                        ASDU_COT_INDEX
                                                                 );
        return asdu_cot2_val;
    }
}


u_int APDU101::setASDU_COT_VAL ( ASDU_COT2_VAL* p_Mem )
{
    return this->setAPDU_ELEMENT ( ( u_char* ) p_Mem,
                                   this->getDU_Info()->ASDU_CMTBL[ASDU_COT_INDEX],
                                   ASDU_COT_INDEX
                                 );
}


u_int APDU101::setASDU_COT_VAL ( ASDU_COT1_VAL* p_Mem )
{
    return this->setAPDU_ELEMENT ( ( u_char* ) p_Mem,
                                   this->getDU_Info()->ASDU_CMTBL[ASDU_COT_INDEX],
                                   ASDU_COT_INDEX
                                 );
}




ASDU_ADDR_VAL* APDU101::getASDU_ADDR_VAL ( u_short* lng_p )
{
    return ( ASDU_ADDR_VAL* ) this->getAPDU_ELEMENT ( lng_p,
            ASDU_ADDR_INDEX
                                                    );
}


u_int   APDU101::setASDU_ADDR_VAL ( ASDU_ADDR_VAL* p_Mem )
{
    return this->setAPDU_ELEMENT ( ( u_char* ) p_Mem,
                                   this->getDU_Info()->ASDU_CMTBL[ASDU_ADDR_INDEX],
                                   ASDU_ADDR_INDEX
                                 );
}




BODY_ADDR_VAL*  APDU101::getBODY_ADDR_VAL ( u_short* lng_p, u_char index )
{
    return ( BODY_ADDR_VAL* ) this->getAPDU_ELEMENT ( lng_p,
            BODY_ADDR_INDEX,
            index
                                                    );
}


u_int APDU101::setBODY_ADDR_VAL ( BODY_ADDR_VAL* p_Mem, u_char index )
{
    return this->setAPDU_ELEMENT ( ( u_char* ) p_Mem,
                                   this->getDU_Info()->ASDU_CMTBL[BODY_ADDR_INDEX],
                                   BODY_ADDR_INDEX,
                                   index
                                 );
}




CP24TIME2a* APDU101::getBODY_TSP_VAL_CP24 ( u_short* lng_p, u_char index )
{
    return ( CP24TIME2a* ) this->getAPDU_ELEMENT ( lng_p,
            BODY_TSP_INDEX,
            index
                                                 );
}


CP56TIME2a* APDU101::getBODY_TSP_VAL_CP56 ( u_short* lng_p, u_char index )
{
    return ( CP56TIME2a* ) this->getAPDU_ELEMENT ( lng_p,
            BODY_TSP_INDEX,
            index
                                                 );
}


u_int APDU101::setBODY_TSP_VAL ( void* p_Mem, u_char index )
{
    return this->setAPDU_ELEMENT ( ( u_char* ) p_Mem,
                                   this->getDU_Info()->ASDU_CMTBL[BODY_TSP_INDEX],
                                   BODY_TSP_INDEX,
                                   index
                                 );
}



