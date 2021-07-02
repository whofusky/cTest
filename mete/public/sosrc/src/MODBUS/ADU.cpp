
#include "ADU.h"
#include <string.h>




C_ModbusADU::C_ModbusADU ( ADU_DSCR* cstmInfo_p,
                           u_char duType,
                           u_char  allocFlag
                         ) : C_ModbusPDU ( cstmInfo_p, duType, allocFlag )
{

}




C_ModbusADU:: ~C_ModbusADU()
{

}




ADU_ADDR_VAL* C_ModbusADU::getADU_ADDR_VAL ( u_short& lng_p )
{
    return ( ADU_ADDR_VAL* ) this->getADU_ELEMENT ( lng_p,
            ADU_ADDR_INDEX
                                                  );
}




int C_ModbusADU::setADU_ADDR_VAL ( ADU_ADDR_VAL* p_Mem )
{
    return this->setADU_ELEMENT ( ( u_char* ) p_Mem,
                                  this->getDU_Info()->ADU_CMTBL[ADU_ADDR_INDEX],
                                  ADU_ADDR_INDEX
                                );
}




ADU_ECHK_VAL* C_ModbusADU::getADU_ECHK_VAL ( u_short& lng_p )
{
    return ( ADU_ECHK_VAL* ) this->getADU_ELEMENT ( lng_p,
            ADU_ECHK_INDEX
                                                  );
}




int C_ModbusADU::setADU_ECHK_VAL ( ADU_ECHK_VAL* p_Mem )
{
    return this->setADU_ELEMENT ( ( u_char* ) p_Mem,
                                  this->getDU_Info()->ADU_CMTBL[ADU_ECHK_INDEX],
                                  ADU_ECHK_INDEX
                                );
}




ADU_MBAP_VAL* C_ModbusADU::getADU_MBAPH_VAL ( u_short& lng_p )
{
    return ( ADU_MBAP_VAL* ) this->getADU_ELEMENT ( lng_p,
            ADU_MBAPH_INDEX
                                                  );
}




int C_ModbusADU::setADU_MBAPH_VAL ( ADU_MBAP_VAL* p_Mem )
{
    return this->setADU_ELEMENT ( ( u_char* ) p_Mem,
                                  this->getDU_Info()->ADU_CMTBL[ADU_MBAPH_INDEX],
                                  ADU_MBAPH_INDEX
                                );
}



