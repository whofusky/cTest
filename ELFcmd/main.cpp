#include <stdio.h>
#include <string.h>

#include "ELFcmd.h"
#include "CustomOutLog.h"



int main()
{


    int ret = 0;
    //char tmpStr[1024] = {0};

    ////求文件的md5值
    //snprintf( tmpStr, sizeof(tmpStr),"/home/fusky/tmp/t/t.sh");
    //char omd5val[33];
    //ret = ELFcmd::md5sum( tmpStr, omd5val);
    //c_write_log(_DEBUG,"[%s],ret=[%d],\nmd5sum[%s]",tmpStr,ret, omd5val );
    //c_write_log(_DEBUG,"path [%s]",ELFcmd::get_self_exe_path() );

    ////求字符串的md5值
    //snprintf( tmpStr, sizeof(tmpStr),"homefuskyDownloadsqt-opensource-linux-x64-5.12.0.run");
    //ret = ELFcmd::md5str( tmpStr, omd5val);
    //c_write_log(_DEBUG,"[%s],ret=[%d],\nmd5str[%s]",tmpStr,ret, omd5val );

    pid_t pid       = 2489982;
    int killAllFlag = 1;
    ELFcmd::kill9Pbypid( killAllFlag, pid, ret );
    c_write_log(_DEBUG,"ELFcmd::kill9Pbypid(%d,%d) return[%d]",killAllFlag, pid, ret );


    return 0;
}
