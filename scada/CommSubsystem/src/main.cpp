

#include "cmSubSys.h"
//#include "FrameGlobal.h"
//extern nsCPSocket::CTcpServer g_locServer; // 用于初始化g_evtArr_pp
extern cfgManage* cfgMng ;


int end = 0;

#ifdef WINDOWS_PLATFORM     //是WINDOWS平台
#else

void signal_fun ( int sig_num )
{
    switch ( sig_num )
    {
        case SIGKILL:
            cfgMng->tryLog.LPrintf ( DEBUG,
                                     DEBUG_LEVEL_0,
                                     "SIGKILL recerved \r\n"
                                   );
            break;

        case SIGINT:
            cfgMng->tryLog.LPrintf ( DEBUG,
                                     DEBUG_LEVEL_0,
                                     "SIGINT recerved \r\n"
                                   );
            break;

        case SIGTSTP:
            cfgMng->tryLog.LPrintf ( DEBUG,
                                     DEBUG_LEVEL_0,
                                     "SIGTSTP recerved \r\n"
                                   );
            break;

        case SIGQUIT:
            cfgMng->tryLog.LPrintf ( DEBUG,
                                     DEBUG_LEVEL_0,
                                     "SIGQUIT recerved \r\n"
                                   );
            break;

        case SIGTERM:
            cfgMng->tryLog.LPrintf ( DEBUG,
                                     DEBUG_LEVEL_0,
                                     "SIGTERM recerved \r\n"
                                   );
            break;

        case SIGSEGV:
            cfgMng->tryLog.LPrintf ( DEBUG,
                                     DEBUG_LEVEL_0,
                                     "SIGTERM recerved \r\n"
                                   );

            break;

        default:
            cfgMng->tryLog.LPrintf ( DEBUG,
                                     DEBUG_LEVEL_0,
                                     "sig_num%d recerved \r\n", sig_num
                                   );
            break;
    }

    sysClear();
    exit ( 0 );

}
#endif
int main()
{
    int iRet = 0;

    u_int iTid = nsCPThread::GetThreadID();

    // init system
    sysInit();


    cfgMng->tryLog.LPrintf ( DEBUG,
                             DEBUG_LEVEL_0,
                             "start reqSnn tid=%u \r\n", iTid
                           );
    //  int pid =   msgget((key_t)1234 , IPC_CREAT|0666);
    //  if (pid == -1)
    //  {
    //      printf("%s/n",strerror(errno));
    //      return pid;
    //  }


#ifdef WINDOWS_PLATFORM     //是WINDOWS平台
#else
    struct sigaction act;
    act.sa_flags = 0;
    act.sa_handler = signal_fun;
    sigaction ( SIGKILL, &act, NULL );
    sigaction ( SIGINT, &act, NULL );
    sigaction ( SIGTSTP, &act, NULL );
    sigaction ( SIGQUIT, &act, NULL );
    sigaction ( SIGTERM, &act, NULL );
    sigaction ( SIGSEGV, &act, NULL );
#endif

    cfgMng->tryLog.LPrintf ( DEBUG,
                             DEBUG_LEVEL_0,
                             "main !!\r\n"
                           );

    while ( !end )
    {
        nsCPPub::SleepSecond ( 1000 );
    }

    sysClear();

    return 0;
}
