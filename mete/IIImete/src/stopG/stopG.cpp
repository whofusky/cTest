/**********************************************************
* name:       stopG.cpp
*
* desc:      kill程序
*
* date:      20170804171728
*
***********************************************************/

#include "funcLib.h"
//配置getPidNum.sh路径 的环境变量名
#define MC_GETPIDNUM                "METEGETPIDNUM"



int main ( int argc, const char** argv )
{

    int vRet = 0;
    int vFlag = 0;

    if ( argc != 2 )
    {
        printf ( "input parameter is incorrect;argc=[%d],\n\
            the correct input methods such as \"stopG <stop_program_name>\"\n\n",
                 argc );
        return -1;
    }

    char tmpStr[200];
    memset ( tmpStr, 0, sizeof ( tmpStr ) );

    snprintf ( tmpStr, sizeof ( tmpStr ),
               "exit $(pidof -x %s|awk 'BEGIN {num=0;}{ if(NF>0){num=NF;}} END{print num}')",
               argv[1] );
    pid_t status = system ( tmpStr );
    int num = WEXITSTATUS ( status );

    if ( num < 0 )
    {
        printf ( "%s|%s|%d:executeShell [%s] return error vRet=[%d]\n",
                 __FILE__, __func__, __LINE__, tmpStr, num );
        return -3;

    }
    else if ( num > 0 )
    {
        t_printf ( "start stop [%s]!\n", argv[1] );
        snprintf ( tmpStr, sizeof ( tmpStr ), "kill $(pidof -x \"%s\")", argv[1] );
        vRet = executeShell ( tmpStr, NULL );

        if ( vRet != 0 )
        {
            t_printf ( "%s|%s|%d:executeShell [%s] return error vRet=[%d]\n",
                       __FILE__, __func__, __LINE__, tmpStr, vRet );
            return -4;
        }

        vFlag = 1;
    }

    if ( vFlag == 1 )
    {
        t_printf ( "[%s] stop success!\n", argv[1] );
    }
    else
    {
        printf ( "there is no procedure PID for[%s]\n", argv[1] );
    }


    return 0;

}

