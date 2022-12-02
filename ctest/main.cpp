#include <stdio.h>
#include <string.h>
#include "WriteLog.h"

#include <stdio.h>
#include <string.h>
#include <algorithm>

#include <unistd.h>
#include <string>
#include <map>
#include <stdlib.h>
#include <sys/syscall.h>
#include "json/json.h"


#include "ELFcmd.h"

#include "InfraBase.h"
#include "CustomOutLog.h"




#define PID_STATU_LEN 20    /*取进程可读信息字符串最大长度*/
//此处的结构是存储/proc/pid/status中信息,
//   进程状态也是带解读模式的如: S (sleeping)
//   其中内存信息是带单位的如: 184 kB
typedef struct
{
       char name[PID_STATU_LEN];   /*进程名*/
       char state[PID_STATU_LEN];  /*进程的状态*/
       int  ppid;                  /*进程的父进程*/
       char vmsize[PID_STATU_LEN]; /*进程现在正在占用的内存*/
       char vmpeak[PID_STATU_LEN]; /*当前进程运行过程中占用内存的峰值*/
       char vmrss[PID_STATU_LEN];  /*是程序现在使用的物理内存*/
       char vmhwm[PID_STATU_LEN];  /*是程序得到分配到物理内存的峰值*/
}p_s_info;

// get hunan readable progress stauts info
int getHRPSInfo( int pid, p_s_info &retInfo )
{
    char tmpc[100] = {0};
    memset( &retInfo, 0, sizeof(p_s_info) );
    snprintf( tmpc,sizeof(tmpc),"%d",pid);

    std::string pfname = "/proc/";
    pfname += tmpc + std::string("/status");

    std::string strVal[8];
    int inoutnum = 7;

    // 取/proc/pid/status文件的内容
    //  str[0] Name   进程名
    //  str[1] State  进程的状态
    //  str[2] PPid   进程的父进程
    //  str[3] VmSize 进程现在正在占用的内存
    //  str[4] VmPeak 当前进程运行过程中占用内存的峰值
    //  str[5] VmRSS  是程序现在使用的物理内存
    //  str[6] VmHWM  是程序得到分配到物理内存的峰值
    int ret = ELFcmd::getPidStatus( pfname.c_str(), strVal, inoutnum );
    if ( ret != 0 )
    {
        b_write_log(_ERROR,"ELFcmd::getPidStatus return[%d]", ret);
        return ret;
    }
    snprintf( retInfo.name, sizeof(retInfo.name),
            "%s", strVal[0].c_str() );
    snprintf( retInfo.state, sizeof(retInfo.state),
            "%s", strVal[1].c_str() );
    retInfo.ppid = atoi( strVal[2].c_str() );
    snprintf( retInfo.vmsize, sizeof(retInfo.vmsize),
            "%s", strVal[3].c_str() );
    snprintf( retInfo.vmpeak, sizeof(retInfo.vmpeak),
            "%s", strVal[4].c_str() );
    snprintf( retInfo.vmrss, sizeof(retInfo.vmrss),
            "%s", strVal[5].c_str() );
    snprintf( retInfo.vmhwm, sizeof(retInfo.vmhwm),
            "%s", strVal[6].c_str() );

    return 0;
}

int main()
{
    pid_t tid;
    tid = syscall(SYS_gettid);
    c_write_log(_INFO,"thread id[%d]", tid);


    int ret = 0;
    char tmpStr[1024] = {0};

    //snprintf( tmpStr, sizeof(tmpStr),"nohup /home/fusky/tmp/t/t.sh >/dev/null 2>&1 &");
    snprintf( tmpStr, sizeof(tmpStr),"/home/fusky/tmp/t/t.sh");
    //snprintf( tmpStr, sizeof(tmpStr),"/home/fusky/tmp/t");

    std::string tss;
    std::string ts1 = "";
    Json::Value tjval;
    tjval["id"] = 1;
    tjval["ss1"] = "";
    tjval["ss2"] = tss;
    tjval["ss3"] = ts1;
    tjval["ss4"] = "haha";

    //int aa = tjval["id"].asInt();

    int tpid = 1467306;
    p_s_info retinfo;
    ret = getHRPSInfo( tpid, retinfo );

    c_write_log(_DEBUG,"getHRPSInfo ret[%d],tpid[%d],"
            "name[%s],state[%s],ppid[%d],vmsize[%s],"
            "vmpeak[%s],vmrss[%s],vmhwm[%s]", 
            ret, tpid, retinfo.name, retinfo.state,
           retinfo.ppid, retinfo.vmsize, retinfo.vmpeak,
          retinfo.vmrss, retinfo.vmrss );

    PNAME_RET_INFO retpinfo;
    std::string exe_name = "t.sh";
    std::string full_exe_name = "/home/fusky/tmp/t/t.sh";
    ret = ELFcmd::getPidByName( exe_name.c_str(), retpinfo );
    c_write_log(_DEBUG, "getPidByName ret[%d]", ret );
    for(int i = 0 ;i < retpinfo.curnum; i++)
    {
        c_write_log(_DEBUG,"[%d] pid[%d] runpath[%s]",
                i, retpinfo.pt[i].pid, retpinfo.pt[i].path.c_str() );
    }
    ret = ELFcmd::getPidByFullName( full_exe_name.c_str(), retpinfo );
    c_write_log(_DEBUG, "getPidByFullName ret[%d]", ret );
    for(int i = 0 ;i < retpinfo.curnum; i++)
    {
        c_write_log(_DEBUG,"[%d] pid[%d] runpath[%s]",
                i, retpinfo.pt[i].pid, retpinfo.pt[i].path.c_str() );
    }

    //return 0;

    //std::string src = "/home/fusky/2";
    //std::string dst = "/home/fusky/1";
    //ret = ELFcmd::cp(src.c_str(), dst.c_str());
    //c_write_log(_DEBUG,"cp [%s] [%s] return[%d]", 
    //        src.c_str(), dst.c_str(),ret );


    //return 0;

    ////ret = executeShell(tmpStr,NULL);
    //ret = ELFcmd::system_plus ( tmpStr,
    //              30,
    //              0,
    //              1
    //            );
    //c_write_log(_DEBUG, "system_plus ret[%d]", ret );

    PNAME_RET_INFO tRetInfo;
    //getPidByName( "t.sh" );
    //ret = getPidByName( "sunloginclient", tRetInfo );
    ret = ELFcmd::getPidByName( "t.sh", tRetInfo );
    c_write_log(_DEBUG, "getPidByName ret[%d]", ret );
    for(int i = 0 ;i < tRetInfo.curnum; i++)
    {
        c_write_log(_DEBUG,"[%d] pid[%d] runpath[%s]",
                i, tRetInfo.pt[i].pid, tRetInfo.pt[i].path.c_str() );
    }


    ret = strcmp("abcde","abcda");
    c_write_log( _DEBUG, "ret=[%d]", ret);
    getcwd( tmpStr, 1024);
    c_write_log( _DEBUG, "getcwd=[%s]", tmpStr);

    c_write_log(_DEBUG,"%s" ",%lu" ,tmpStr,sizeof(tmpStr));
    c_write_log( _INFO,"tmpStr=[%s]111", tmpStr);

    while ( 1 )
    {
        PauseThreadSleep( 2, 0 );
        c_write_log(_DEBUG,"in while loop");
    }


    return 0;
}
