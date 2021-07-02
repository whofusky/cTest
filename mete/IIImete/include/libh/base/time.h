/***********************************************************
*
*
*  time.h
*
*  Create by muxd
*
*  2017/07/22
***********************************************************/
#ifndef _TIME_H_
#define _TIME_H_
#include <time.h>

#include "base/Base.h"

time_t ConvertStrToTime( const char* szTime );
time_t ConvertStrBToTime( const char* szTime );


bool ConvertTimeToStr( 
    char* buf, 
    int bufLen, 
    struct tm time );

typedef struct day_time
{
    int mHour;
    int mMinute;
    int mSecond;
}DAY_TIME;

bool ConvertDayTimeStrToStruct( 
    const char* dTime, 
    DAY_TIME* dstTime );

int timeStructCompare( 
    DAY_TIME left, 
    DAY_TIME right );

#endif

