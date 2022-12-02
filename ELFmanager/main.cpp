#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

#include "ELFmanager.h"
#include "InfraBase.h"
#include "CustomOutLog.h"

void print_some( std::string &cfg )
{
    int ret = 0;
    std::string fullName;
    std::string::size_type head = 0;
    std::string::size_type tail = cfg.find( "|", head );
    std::string::size_type rpos = cfg.rfind( "|");
    if ( rpos == cfg.npos )
    {
        fullName = cfg;
    }
    else
    {
        fullName = cfg.substr( head, tail - head );
    }

    p_c_info cinfo;
    ret = ELFM_getPCInfo( fullName, cinfo );
    c_write_log(_DEBUG,"ELFM_getPCInfo(%s) return[%d]"
            "cfgstat[%d],stdlogflag[%d],cfginter[%d]"
            "path=[%s],fname[%s],runprefix[%s],runpar[%s]",
            fullName.c_str(), ret,
           cinfo.cfgstat, cinfo.stdlogflag, cinfo.cfginter, cinfo.path.c_str(),
          cinfo.fname.c_str(), cinfo.runprefix.c_str(), cinfo.runpar.c_str() );


    p_r_info rinfo;
    ret = ELFM_getPRInfo( fullName, rinfo );
    c_write_log(_DEBUG,"ELFM_getPRInfo(%s) return[%d],"
            "runstat[%d],stardate[%s],timestamp[%ld],"
            "startnum[%d],stopnum[%d],pidnum[%d]",
            fullName.c_str(), ret ,
            rinfo.runstat, rinfo.stardate, rinfo.timestamp,
            rinfo.startnum, rinfo.stopnum, rinfo.pidnum );

    for(int i = 0; i < rinfo.pidnum; i++ )
    {
        p_s_info *tpf = &(rinfo.pf[i]);
        c_write_log(_DEBUG,"[%d]:pid[%d],name[%s],state[%s],ppid[%d],vmsize[%s],"
                "vmpeak[%s],vmrss[%s],vmhwm[%s]",
                i,rinfo.pid[i], tpf->name, tpf->state, tpf->ppid, tpf->vmsize,
               tpf->vmpeak, tpf->vmrss, tpf->vmhwm ); 

    }

    return;
}

int main()
{

    pid_t tid;
    tid = syscall(SYS_gettid);
    c_write_log(_INFO,"thread id[%d]", tid);

    int ret = 0;
    std::vector<std::string> cfgInfo;

    //  runname|cfgstat|stdlogflag|cfginter|runprefix|runpar
    std::string tpstr = "/zfmd/pvfs20/gaolongshan/CDUS/CDUS|0|1|2||0";
    cfgInfo.push_back( tpstr );
    tpstr = "/zfmd/pvfs20/gaolongshan/01/STF/STF|0|1|2";
    cfgInfo.push_back( tpstr );
    ELFM_addCfgInfo( cfgInfo );

    //ELFM_start( int timerFixSeconds = 2, int starHold = 0, int stopHold = 0, const int &pre_val = -1 );
    ELFM_start( 2, 4, 4 );
    std::string tpath = "/zfmd/pvfs20/gaolongshan/CDUS/CDUS";
    //std::string tpath = "/zfmd/pvfs20/gaolongshan";

    //ret = ELFM_openAutoStart( tpath );
    ret = ELFM_startExe( tpath );
    //ret = ELFM_startExeByPrefix( tpath );
    //ret = ELFM_startExeAll();
    //ret = ELFM_stopExe( tpath );
    c_write_log(_DEBUG,"ELFM_startExe(%s) return[%d]", 
            tpath.c_str(), ret);


    int j = 0;
    while(1)
    {
        PauseThreadSleep( 2, 0 );
        for( std::string::size_type i = 0; i < cfgInfo.size(); i++ )
        {
            if ( 0 == i )
                print_some( cfgInfo[i] );
        }
        //if ( 2 == j )
        //{
        //    std::string tst = "/zfmd/pvfs20/gaolongshan";
        //    //ret = ELFM_stopExe( tst );
        //    //ret = ELFM_stopExeByPrefix( tst );
        //    ret = ELFM_stopExeAll();
        //    c_write_log(_DEBUG,"ELFM_stopExeAll(%s) return[%d]",
        //            tst.c_str(), ret );
        //}

        //if ( 8 == j )
        //{
        //    //std::string tst = "/zfmd/pvfs20/gaolongshan/CDUS/CDUS";
        //    //std::string tst = "/zfmd/pvfs20/gaolongshan";
        //    //ret = ELFM_openPrefixAutoStart( tst );
        //    //ret = ELFM_closeAllAutoStart();
        //    ret = ELFM_startExe( tpath );
        //    c_write_log(_DEBUG,"ELFM_startExe(%s) return[%d]",
        //            tpath.c_str(), ret );
        //}
        j++;

    }

    return 0;
}
