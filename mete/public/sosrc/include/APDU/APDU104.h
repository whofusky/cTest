
#pragma once

#ifndef APDU104_H
#define APDU104_H
#include "APDU101.h"




typedef u_char GEN_FRM[4];

typedef struct
{
    u_short frameType: 1;
    u_short SNo: 15;
    u_short pad: 1;
    u_short RNo: 15;
} I_FRM;


typedef struct
{
    u_short frameType: 2;
    u_short hZero : 6;
    u_short pad: 8;
    u_short lRNo: 1;
    u_short hRNo: 15;
} S_FRM;


typedef struct
{
    u_char  frameType: 2;
    u_char  SRTDT_ACT: 1;
    u_char  SRTDT_CON: 1;
    u_char  STPDT_ACT: 1;
    u_char  STPDT_CON: 1;
    u_char  TSTFR_ACT: 1;
    u_char  TSTFR_CON: 1;
    u_char  lZero;
    u_short hZero;
} U_FRM;


typedef struct
{
    u_char startChar;//启动字符：= SRTCHAR_F1_2_V，见本文的2.2.5部分
    u_char APDULng;//<= APDUMAX - 2
    GEN_FRM ctlField;
} APCI_104;


u_int getFrmType ( GEN_FRM ctlField, u_char* frmeType_p );

u_int getSNoFromGenFrm ( GEN_FRM ctlField, u_short* I_SNo_p );

u_int getRNoFromGenFrm ( GEN_FRM ctlField, u_short* I_RNo_p );

u_int getU_FunFromGenFrm ( GEN_FRM ctlField, u_char* U_FunNo_p );




class APDU104 :
    public APDU101
{
    public:
        APDU104 ( APDU_DSCR* cstmInfo_p, u_char duType, u_char  allocFlag );
        ~APDU104();

};




#endif //  APDU104_H