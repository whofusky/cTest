#include <stdio.h>


#include "parseGPRMC.h"
#include "CustomOutLog.h"



int main()
{

    char str[] = "$GPRMC,083559.00,A,4717.11437,N,00833.91522,E,0.004,77.52,091202,,,A57";

    T_GPRMC gprmc ;
    int ret = parse_gprmc(str, gprmc);

    c_write_log(_DEBUG,"str：[%s]", str);
    c_write_log(_DEBUG,"call parse_gprmc ret=[%d]\n", ret);

    c_write_log(_DEBUG,"时间戳(hhmmss.ss)       :[%s]", gprmc.timestamp);
    c_write_log(_DEBUG,"状态(A:valid V:invalid) :[%c]", gprmc.status);
    c_write_log(_DEBUG,"纬度                    :[%c %s]", gprmc.latiudeFlag,gprmc.latitude);
    c_write_log(_DEBUG,"经度                    :[%c %s]", gprmc.longitudeFlag,gprmc.longitude);
    c_write_log(_DEBUG,"地面速率                :[%s]", gprmc.speed);
    c_write_log(_DEBUG,"地面航向                :[%s]", gprmc.heading);
    c_write_log(_DEBUG,"日期(ddmmyy)            :[%s]", gprmc.date);
    //c_write_log(_DEBUG,"str：[%s]", str);

    return 0;
}
