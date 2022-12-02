#include <stdio.h>
#include <string.h>
#include "CustomOutLog.h"

typedef unsigned char  u_char;
typedef unsigned short u_short;
typedef unsigned int   u_int;
typedef unsigned long  u_long;


int subsum(int a,int b,int& sum);

typedef struct
{
    u_char Mode;
    u_short K;
    u_short W;
}realTimeThreshold;
    

int main()
{
    //short tUcVal = 140;
    //u_long ttul = 12345679;

    /////home/fusky/mygit/cTest
    //WriteLogInit( 
    //    LOGDEBUG,
    //    "/home/fusky/mygit/cTest/log",      /*不设置时默认日志路径为程序运行路径下LOG/$YYYY/$MM*/
    //    "tt" /*当设置dirName_p时才设置此值：日志名为logPrefixName_$YYYYMMDD.log*/
    //    );

    char tmpStr[1024] = {0};
    int  sizeofStr = sizeof(tmpStr);
    c_write_log( _DEBUG, "sizeof(tmpStr[1024])=[%d]", sizeofStr );
    //c_write_log(_DEBUG,"tUcVal=[0x%x],ttul=[%lu]",tUcVal,ttul);
    c_write_log( _DEBUG, "hahha" );
    c_write_log( _DEBUG, "hahha21" );
    c_write_log( _DEBUG, "hahha2,[%d],[%s]",1,"fushikai" );
    

    return 0;
}
