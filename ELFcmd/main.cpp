#include <stdio.h>
#include <string.h>

#include "ELFcmd.h"
#include "CustomOutLog.h"



int main()
{


    int ret = 0;
    char tmpStr[1024] = {0};

    //snprintf( tmpStr, sizeof(tmpStr),"/home/fusky/tmp/t/t.sh");
    snprintf( tmpStr, sizeof(tmpStr),"homefuskyDownloadsqt-opensource-linux-x64-5.12.0.run");
    char omd5val[33];
    ret = ELFcmd::md5sum( tmpStr, omd5val);
    c_write_log(_DEBUG,"[%s],ret=[%d],\nmd5sum[%s]",tmpStr,ret, omd5val );



    return 0;
}
