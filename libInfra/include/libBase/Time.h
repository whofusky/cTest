/***********************************************************
 *
 * @file    Time.h
 *
 * @brief   时间处理相关自定义函数头文件
 *
 * @author  
 *
 * @date    2021-07-03
 *
 * @version V10.010.000
 *
 ***********************************************************/

#ifndef _TIME_H_
#define _TIME_H_

#include <time.h>

#include "Base.h"

/******************************
 * @brief     convert string to 'time_t'
 * @param[in] szTime:"%Y-%m-%d:%H-%M-%S"
 * @return    time_t:seconds num
******************************/
time_t ConvertStrToTime( const char* szTime );

/******************************
 * @brief     convert string B to 'time_t'
 * @param[in] szTime:"%Y-%m-%d_%H-%M-%S"
 * @return    time_t:seconds num
******************************/
time_t ConvertStrBtoTime( const char* szTime );

/******************************
 * @brief     convert string C to 'time_t'
 * @param[in] szTime:"%Y-%m-%d %H-%M-%S"
 * @return    time_t:seconds num
******************************/
time_t ConvertStrCtoTime( const char* szTime );

bool ConvertTimeToStr( char* buf, int bufLen, struct tm time );

bool ConvertTimeToStr2( char* buf, int bufLen, struct tm time );


typedef struct day_time
{
	int mHour;
	int mMinute;
	int mSecond;
}DAY_TIME;


bool ConvertDayTimeStrToStruct(const char* dTime,DAY_TIME* dstTime );

int timeStructCompare( DAY_TIME left, DAY_TIME right );




#endif//_TIME_H_
