/***********************************************************
 *
 * @file    parseGPRMC.h
 *
 * @brief   parseGPRMC头文件
 *
 * @author  fu.sky
 *
 * @date    2022-11-17_09:19:45
 *
 * @version V10.010.000
 *
 ***********************************************************/

#ifndef _PARSE_GPRMC_H_
#define _PARSE_GPRMC_H_

/*
 GPRMC is a specific sentence format used in the NMEA 0183 standard for communicating GPS data. The sentence includes information about the current time, location, speed, and course of a vehicle or other moving object. The sentence consists of a series of comma-separated fields, each of which contains a specific piece of information.

Here is an example of a GPRMC sentence:

$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A
<0>    <1>   <2> <3>    <4> <5>     <6> <7>  <8>    <9>   <10><11><12>

The fields in this sentence are as follows:

<0> $GPRMC: This is the sentence identifier, which indicates that the sentence is a GPRMC sentence.

<1> 123519: This is the current time in UTC (Coordinated Universal Time). The format is hhmmss, where hh is the hour, mm is the minute, and ss is the second.

<2> A: This field indicates the status of the GPS fix. A value of "A" indicates that the GPS fix is valid, while a value of "V" indicates that the fix is invalid.

<3> <4> 4807.038,N: This field indicates the latitude of the current location. The first part (4807.038) is the latitude in degrees, and the second part (N) indicates whether the latitude is north or south of the equator.

<5> <6> 01131.000,E: This field indicates the longitude of the current location. The first part (01131.000) is the longitude in degrees, and the second part (E) indicates whether the longitude is east or west of the prime meridian.

<7> 022.4: This field indicates the ground speed of the vehicle in knots.

<8> 084.4: This field indicates the course of the vehicle, in degrees, relative to true north.

<9> 230394: This field indicates the date, in the format ddmmyy.

<10> <11> 003.1,W: This field indicates the magnetic variation, or the difference between true north and magnetic north. The first part (003.1) is the variation in degrees, and the second part (W) indicates whether the variation is east or west.

<12> 6A: This is a checksum field, which is used to verify the integrity of the sentence.

*/

/*
GPRMC是Global Positioning System (GPS)中的一个报文类型，
    用于提供GPS接收器的位置、速度和时间信息。GPRMC报文的格式如下：

$GPRMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,ddmmyy,x.x,a,m*hh
<0>    <1>      <2> <3>   <4> <5>    <6><7> <8> <9>    <10><11><12><13>

其中：

<0> $GPRMC：报文类型。
<1> hhmmss.ss：UTC时间，小时，分钟，秒，小数秒。
<2> A：定位状态，A=有效，V=无效。
<3> llll.ll：纬度，ddmm.mmmm（度、分）格式，其中dd=纬度，mm.mmmm=分。
<4> a：纬度半球，N=北半球，S=南半球。
<5> yyyyy.yy：经度，dddmm.mmmm（度、分）格式，其中ddd=经度，mm.mmmm=分。
<6> a：经度半球，E=东半球，W=西半球。
<7> x.x：在UTC时间下的地面速率（以节为单位）。
<8> x.x：在UTC时间下的地面航向（以度为单位）。
<9> ddmmyy：UTC日期，日、月、年。
<10> x.x：地面航迹角度，以度为单位。
<11> a：方向，T=真北、M=磁北。
<12> m：模式指示，A=自主定位，D=差分，E=估算，N=数据无效。
<13> hh：校验和。
*/






#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MC_FLOAT_LEN 32  //定义float数位长度




typedef struct {
    char status;                      // 状态，A表示有效，V表示无效
    char latiudeFlag;                 // 纬度标识
    char longitudeFlag;               // 经度标识
    char date[7];                     // 日期，格式是ddmmyy
    char timestamp[11];               // 时间戳，格式是hhmmss.ss
    char latitude[MC_FLOAT_LEN];      // 纬度
    char longitude[MC_FLOAT_LEN];     // 经度
    char speed[MC_FLOAT_LEN];         // 地面速率
    char heading[MC_FLOAT_LEN];       // 地面航向
} T_GPRMC;


// 从字符串中解析出GPRMC报文
int parse_gprmc(const char *str, T_GPRMC &retRMC ) ;






#endif//_PARSE_GPRMC_H_
