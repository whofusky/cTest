/****************************************
 * 
 *    CustomOutLog.h
 * 
 *    Create by fushikai
 * 
 *    2022-11-02_16:33:30
 * 
 ****************************************/
#ifndef __CUSTOM_OUT_LOG_H__
#define __CUSTOM_OUT_LOG_H__

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <libgen.h>


/******************************************************************************
 *
 *    此头文件是为了兼容程序中已经写好的日志输出函数,
 *
 *    如果需要修改程序中的日志输出则需要修改下面的宏定义及相关逻辑,
 *    但参数形式不要变 
 *      b_write_log     //基础函数或类中用到此接口
 *      c_write_log     //除基础函数或类中之外的代码中用此接口
 *
 *
 *
******************************************************************************/








enum{
    _NOLOG,
    _ERROR,
    _WARN,
    _INFO,
    _DEBUG
};








#define enumToStr(enumpar)  #enumpar

/*******注：控制输出日志等级需要修改 control_level然后再重新编译*********/

#define b_write_log(logLevel, str, arg... )  do{ \
    int control_level = _DEBUG; \
    if( logLevel > control_level ) break; \
    char localTmpBuf_a[32] = {0}; \
    MyGetFormatTime( localTmpBuf_a, sizeof(localTmpBuf_a),"%Y-%m-%d_%H:%M:%S",0,1); \
    char f_path_p[]= __FILE__; \
    printf("%s|%s|%s|%d|%s|" str "\n",localTmpBuf_a, basename(f_path_p),__FUNCTION__, __LINE__,enumToStr(logLevel), ##arg); \
}while(0)


#define c_write_log(logLevel, str, arg... )  do{ \
    int control_level = _DEBUG; \
    if( logLevel > control_level ) break; \
    char localTmpBuf_a[32] = {0}; \
    MyGetFormatTime( localTmpBuf_a, sizeof(localTmpBuf_a),"%Y-%m-%d_%H:%M:%S",0,1); \
    char f_path_p[]= __FILE__; \
    printf("%s|%s|%s|%d|%s|" str "\n",localTmpBuf_a, basename(f_path_p),__FUNCTION__, __LINE__,enumToStr(logLevel), ##arg); \
}while(0)








static void MyGetFormatTime( char* buf, const int bufLen, const char* timeFormat="%Y-%m-%d_%H:%M:%S", int addSeconds=0, char microsecFlag=0 )
{
    //memset( buf, 0, bufLen );

    struct timeval tv;
    time_t nowTimeSec;
    struct tm curTime;

    char ttBuf[32] = {0};

    gettimeofday( &tv, NULL );

    nowTimeSec = tv.tv_sec;
    nowTimeSec += addSeconds;

    localtime_r( &nowTimeSec, &curTime );

    if ( microsecFlag == 0 )
    {
        strftime( buf, bufLen, timeFormat, &curTime );
    }
    else
    {
        strftime( ttBuf, 32, timeFormat, &curTime );
        snprintf( buf, bufLen,"%s.%06ld",ttBuf,tv.tv_usec );
    }

    return;
}





#endif //__CUSTOM_OUT_LOG_H__
