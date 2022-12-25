/*********************************************************** * * @file    parseGPRMC.cpp
 *
 * @brief   parseGPRMC源文件
 *
 * @author  fu.sky
 *
 * @date    2022-12-23_21:07:24
 *
 * @version V10.010.000
 *
 ***********************************************************/

#include "parseGPRMC.h"
//#include "CustomOutLog.h"

#define MC_TMP_LEN 100 //临时字符串长度






// 从字符串中解析出GPRMC报文
int parse_gprmc(const char *str, T_GPRMC &retRMC ) 
{
    //$GPRMC,083559.00,A,4717.11437,N,00833.91522,E,0.004,77.52,091202,,,A57
    //

    T_GPRMC gprmc;
    memset( &gprmc, 0, sizeof( gprmc ) );
    memset( &retRMC, 0, sizeof( retRMC ) );
    char tmpStr[MC_TMP_LEN];

    const char * k = str;
    const char *p = strchr(k, ',');
    if ( NULL == p ) return -1;

    memset( tmpStr, 0, MC_TMP_LEN );
    size_t sz = p -k;
    if ( sz >= MC_TMP_LEN ) sz = MC_TMP_LEN -1;
    strncpy( tmpStr, k, sz );

    if ( strcmp( tmpStr, "$GPRMC" ) != 0 ) 
    {
        return -2; //字符串头不符合要求
    }


    int i = 0;

    while ( p != NULL ) 
    {
        memset( tmpStr, 0, MC_TMP_LEN );
        sz = p -k;
        if ( sz >= MC_TMP_LEN ) sz = MC_TMP_LEN -1;
        strncpy( tmpStr, k, sz );

        switch ( i ) 
        {
            case 1: // 时间戳
            strncpy( gprmc.timestamp, tmpStr, sizeof(gprmc.timestamp) );
            break;

            case 2: // 状态
            gprmc.status = tmpStr[0];
            break;

            case 3: // 纬度
            strncpy( gprmc.latitude, tmpStr, sizeof(gprmc.latitude) );
            break;

            case 4: // 纬度半球
            gprmc.latiudeFlag = tmpStr[0];
            break;

            case 5: // 经度
            strncpy( gprmc.longitude, tmpStr, sizeof(gprmc.longitude) );
            break;

            case 6: // 经度半球
            gprmc.longitudeFlag = tmpStr[0];
            break;

            case 7: // 地面速率
            strncpy( gprmc.speed, tmpStr, sizeof(gprmc.speed) );
            break;

            case 8: // 地面航向
            strncpy( gprmc.heading, tmpStr, sizeof(gprmc.heading) );
            break;

            case 9: // 日期
            strncpy( gprmc.date, tmpStr, sizeof(gprmc.date) );
            break;
        }

        k = p +1;
        p = strchr(k, ',');
        i++;
    }

    //b_write_log(_DEBUG,"i=[%d]",i);
    if ( i <10 )
    {
        return -3; //字符串格式不符合要求
    }
    memcpy( &retRMC, &gprmc, sizeof(gprmc) );

    return 0;

}


