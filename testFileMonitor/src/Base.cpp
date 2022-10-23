/***********************************************************
 *
 * @file    Base.cpp
 *
 * @brief   Base源文件
 *
 * @author  
 *
 * @date    2022-06-02
 *
 * @version V10.010.000
 *
 ***********************************************************/

#include <sys/time.h>
#include <time.h>
#include <string.h>

#include "Base.h"

void PauseThreadSleep(const int& sec, const int& us)
{
    struct timespec tDly;
    if ( sec < 0 || us < 0 ) {
        tDly.tv_sec  = 0;
        tDly.tv_nsec = 1;
    }
    else {
        tDly.tv_sec  = sec;
        tDly.tv_nsec = us*1000;
    }
    nanosleep( &tDly, NULL );

    return;
}


void getFormatTime( char* buf, int bufLen, const char* timeFormat, int addSeconds)
{
    memset( buf, 0, bufLen );

    struct timeval tv;
    time_t nowTimeSec;
    struct tm curTime;

    gettimeofday( &tv, NULL );

    nowTimeSec = tv.tv_sec;
    nowTimeSec += addSeconds;

    localtime_r( &nowTimeSec, &curTime );

    strftime( buf, bufLen, timeFormat, &curTime );

    return;
}

