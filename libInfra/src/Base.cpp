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

#include <time.h>

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

