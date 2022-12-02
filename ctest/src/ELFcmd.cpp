/***********************************************************
 *
 * @file    ELFcmd.cpp
 *
 * @brief   ELFcmd源文件
 *
 * @author  fu.sky
 *
 * @date    2022-11-17_09:19:45
 *
 * @version V10.010.000
 *
 ***********************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <signal.h>
#include <string.h>
#include <dirent.h>



#include "ELFcmd.h"
#include "CustomOutLog.h"
#include "InfraBase.h"

#define MC_CBUFFER_SIZE 1024


// 取/proc/pid/cmdline文件的内容
int ELFcmd::getCmdlinePar( const char* path, std::string str[], int &inoutnum )
{
    char tmpStr[1024] = {0};
    int tfixnum = inoutnum;
    FILE *fp;

    if ( ( fp = fopen(path,"rb") ) == NULL )
    {
        inoutnum = 0;
        return errno;
    }
    if ( fgets(tmpStr,1023,fp) == NULL )
    {
        inoutnum = 0;
        return 1;
    }
    fclose(fp);

    char *tp = tmpStr;
    int i = 0;
    while ( *tp != '\0' && i < tfixnum )
    {
        //b_write_log(_DEBUG,"[%s]",tp);
        str[i++] = tp;
        tp = tp + strlen(tp) +1;
    }

    inoutnum = i;

    return 0;
}




// 取/proc/pid/status文件的内容
//  str[0] Name   进程名
//  str[1] State  进程的状态
//  str[2] PPid   进程的父进程
//  str[3] VmSize 进程现在正在占用的内存
//  str[4] VmPeak 当前进程运行过程中占用内存的峰值
//  str[5] VmRSS  是程序现在使用的物理内存
//  str[6] VmHWM  是程序得到分配到物理内存的峰值
int ELFcmd::getPidStatus( const char* path, std::string str[], int &inoutnum )
{
    char tmpStr[1024] = {0};
    char name[512]    = {0};
    char key[512]     = {0};
    int tfixnum = inoutnum;

    FILE *fp = NULL;
    if ( ( fp = fopen(path,"rb") ) == NULL )
    {
        inoutnum = 0;
        return errno;
    }

    int i = 0;
    while( fgets(tmpStr,1023,fp) != NULL && i < tfixnum )
    {
        sscanf( tmpStr,"%[^:] : %511[^\n]",name,key);

        if ( strcmp(name,"Name") == 0 )
        {
            str[0] = key;
            i++;
        }
        else if ( strcmp(name,"State") == 0 )
        {
            str[1] = key;
            i++;
        }
        else if ( strcmp(name,"PPid") == 0 )
        {
            str[2] = key;
            i++;
        }
        else if ( strcmp(name,"VmSize") == 0 )
        {
            str[3] = key;
            i++;
        }
        else if ( strcmp(name,"VmPeak") == 0 )
        {
            str[4] = key;
            i++;
        }
        else if ( strcmp(name,"VmRSS") == 0 )
        {
            str[5] = key;
            i++;
        }
        else if ( strcmp(name,"VmHWM") == 0 )
        {
            str[6] = key;
            i++;
        }

        //b_write_log( _DEBUG,"name[%s],key[%s]",name,key);
    }
    fclose(fp);

    inoutnum = i;

    return 0;

}




// ret:
//    0  成功
//    -1 opendir "/proc" error
//    -2 没找到进程
//    1  同程序名同时运行的pid超过系统最大限制
int ELFcmd::getPidByName( const std::string &exe_name, PNAME_RET_INFO &retinfo )
{
    retinfo.curnum = 0;

    DIR*   dirStream_p    = NULL;
    struct dirent* dir_fp = NULL;

    std::string rpdir="/proc";

    dirStream_p = opendir( rpdir.c_str() );
    if ( dirStream_p == NULL )
    {
        return -1;
    }

    int tpid = 0;
    int ret  = 0;

    char texepath[PV_DIR_MAX_LEN] = {0};
    char tcwdpath[PV_DIR_MAX_LEN] = {0};
    std::string strPathRun;
    std::string tPathFile;
    std::string tstrName[3];
    int  inoutnum = 1;

    while ( ( dir_fp = readdir(dirStream_p) ) != NULL)
    {
        if( strcmp(dir_fp->d_name,".") == 0 
                || strcmp(dir_fp->d_name,"..")==0
          )    //current dir OR parrent dir
        {
            continue;
        }

        if ( 0 == (tpid = atoi( dir_fp->d_name )) ) continue;

        //file:/proc/pid/status  取进程名
        tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
        tPathFile = tPathFile + "/" +std::string("status");
        inoutnum = 1;
        getPidStatus( tPathFile.c_str(), tstrName, inoutnum  );
        if ( inoutnum != 1 ) continue;

        //进程名与要查找的是否相等
        ret = strcmp( exe_name.c_str(), tstrName[0].c_str() );
        if ( ret != 0 ) continue;


        //file:/proc/pid/exe
        tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
        tPathFile = tPathFile + "/" +std::string("exe");
        
        //memset( texepath, 0 , PV_DIR_MAX_LEN );
        
        int r = readlink( tPathFile.c_str(), texepath, PV_DIR_MAX_LEN );
        if ( r < 0 || r >= PV_DIR_MAX_LEN )
        {
            b_write_log( _ERROR,"readlink [%s] fail,errno[%d],errMsg[%s]", 
                    tPathFile.c_str(),errno, strerror(errno) );

            continue;
        }
        texepath[r] = '\0';
        char *s = strrchr(texepath, '/');
        if ( NULL == s ) continue;

        *s = '\0';
        s++;
        ret = strcmp( exe_name.c_str(), s );
        if ( 0 == ret )
        {
            strPathRun = texepath;
        }
        else // ( ret != 0 ) 
        {
            //如果不相等:说明当前进程不是ELF文件，是类似shell脚本进程,这种情况
            // exe的路径并不是脚本的程序，需要拼音cwd及运行执行路径来得到脚本的路径

            // 取file:/proc/pid/cmdline 第二个参数(运行脚本 带相对路径)
            tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
            tPathFile = tPathFile + "/" +std::string("cmdline");
            inoutnum = 2;
            getCmdlinePar( tPathFile.c_str(), tstrName, inoutnum  );
            if (  inoutnum != 2 ) continue;


            if ( tstrName[1].compare(0,1,"/") == 0 )
            {
                //运行脚本是绝对路径直接取路径即可
                strPathRun = tstrName[1].erase(tstrName[1].find_last_of('/'));
                if ( strPathRun.empty() ) strPathRun = "/";
            }
            else
            {
                //运行脚本是相对路径则需要添加cwd

                if ( tstrName[1].compare(0,2,"./") == 0 )
                { 
                    tstrName[1].erase(0, 2 );
                }

                std::string::size_type tpos;
                if ( (tpos = tstrName[1].find_last_of("/")) != std::string::npos )
                {
                    tstrName[1].erase(tpos);
                }

                //memset( tcwdpath, 0 , PV_DIR_MAX_LEN );
                
                //file:/proc/pid/cwd
                tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
                tPathFile = tPathFile + "/" +std::string("cwd");

                r = readlink( tPathFile.c_str(), tcwdpath, PV_DIR_MAX_LEN );
                if ( r < 0 || r >= PV_DIR_MAX_LEN )
                {
                    b_write_log( _ERROR,"readlink [%s] fail,errno[%d],errMsg[%s]", 
                            tPathFile.c_str(),errno, strerror(errno) );

                    continue;
                }
                tcwdpath[r] = '\0';
                if ( r > 1 && tcwdpath[r -1] == '/' )
                {
                    //去掉路径值最后的'/' 
                    tcwdpath[r-1] = '\0';
                }

                //b_write_log(_DEBUG,"tcwdpath=[%s],tstrName[1]=[%s]",
                //        tcwdpath, tstrName[1].c_str() );

                tpos = tstrName[1].find("/");
                if ( tpos == std::string::npos )
                {
                    strPathRun = tcwdpath;
                }
                else
                {
                    strPathRun = tcwdpath;
                    strPathRun += "/" + tstrName[1];
                }
            }// end cwd + sbudir

        }// end run p is script, to get path


        //b_write_log(_DEBUG,"exe_name=[%s],tpid=[%d],texepath=[%s],strPathRun=[%s]", 
        //        exe_name.c_str(),tpid, texepath,strPathRun.c_str() );
        if ( retinfo.curnum >= SAMEEXE_MAX_PIDS  )
        {
            closedir( dirStream_p );
            return 1;
        }

        retinfo.pt[ retinfo.curnum ].pid  = tpid;
        retinfo.pt[ retinfo.curnum ].path = strPathRun;
        retinfo.curnum++;
    }

    closedir( dirStream_p );

    if ( 0 == retinfo.curnum )  
    {
        return -2;
    }

    return 0;
}




// ret:
//    0  成功
//    -1 opendir "/proc" error
//    -2 没找到进程
//    1  同程序名同时运行的pid超过系统最大限制
int ELFcmd::getPidByName( const char* exe_name, PNAME_RET_INFO &retinfo )
{
    retinfo.curnum = 0;

    DIR*   dirStream_p    = NULL;
    struct dirent* dir_fp = NULL;

    std::string rpdir="/proc";

    dirStream_p = opendir( rpdir.c_str() );
    if ( dirStream_p == NULL )
    {
        return -1;
    }

    int tpid = 0;
    int ret  = 0;

    char texepath[PV_DIR_MAX_LEN] = {0};
    char tcwdpath[PV_DIR_MAX_LEN] = {0};
    std::string strPathRun;
    std::string tPathFile;
    std::string tstrName[3];
    int  inoutnum = 1;

    while ( ( dir_fp = readdir(dirStream_p) ) != NULL)
    {
        if( strcmp(dir_fp->d_name,".") == 0 
                || strcmp(dir_fp->d_name,"..")==0
          )    //current dir OR parrent dir
        {
            continue;
        }

        if ( 0 == (tpid = atoi( dir_fp->d_name )) ) continue;

        //file:/proc/pid/status  取进程名
        tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
        tPathFile = tPathFile + "/" +std::string("status");
        inoutnum = 1;
        getPidStatus( tPathFile.c_str(), tstrName, inoutnum  );
        if ( inoutnum != 1 ) continue;

        //进程名与要查找的是否相等
        ret = strcmp( exe_name, tstrName[0].c_str() );
        if ( ret != 0 ) continue;


        //file:/proc/pid/exe
        tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
        tPathFile = tPathFile + "/" +std::string("exe");
        
        //memset( texepath, 0 , PV_DIR_MAX_LEN );
        
        int r = readlink( tPathFile.c_str(), texepath, PV_DIR_MAX_LEN );
        if ( r < 0 || r >= PV_DIR_MAX_LEN )
        {
            b_write_log( _ERROR,"readlink [%s] fail,errno[%d],errMsg[%s]", 
                    tPathFile.c_str(),errno, strerror(errno) );

            continue;
        }
        texepath[r] = '\0';
        char *s = strrchr(texepath, '/');
        if ( NULL == s ) continue;

        *s = '\0';
        s++;
        ret = strcmp( exe_name, s );
        if ( 0 == ret )
        {
            strPathRun = texepath;
        }
        else // ( ret != 0 ) 
        {
            //如果不相等:说明当前进程不是ELF文件，是类似shell脚本进程,这种情况
            // exe的路径并不是脚本的程序，需要拼音cwd及运行执行路径来得到脚本的路径

            // 取file:/proc/pid/cmdline 第二个参数(运行脚本 带相对路径)
            tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
            tPathFile = tPathFile + "/" +std::string("cmdline");
            inoutnum = 2;
            getCmdlinePar( tPathFile.c_str(), tstrName, inoutnum  );
            if (  inoutnum != 2 ) continue;


            if ( tstrName[1].compare(0,1,"/") == 0 )
            {
                //运行脚本是绝对路径直接取路径即可
                strPathRun = tstrName[1].erase(tstrName[1].find_last_of('/'));
                if ( strPathRun.empty() ) strPathRun = "/";
            }
            else
            {
                //运行脚本是相对路径则需要添加cwd

                if ( tstrName[1].compare(0,2,"./") == 0 )
                { 
                    tstrName[1].erase(0, 2 );
                }

                std::string::size_type tpos;
                if ( (tpos = tstrName[1].find_last_of("/")) != std::string::npos )
                {
                    tstrName[1].erase(tpos);
                }

                //memset( tcwdpath, 0 , PV_DIR_MAX_LEN );
                
                //file:/proc/pid/cwd
                tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
                tPathFile = tPathFile + "/" +std::string("cwd");

                r = readlink( tPathFile.c_str(), tcwdpath, PV_DIR_MAX_LEN );
                if ( r < 0 || r >= PV_DIR_MAX_LEN )
                {
                    b_write_log( _ERROR,"readlink [%s] fail,errno[%d],errMsg[%s]", 
                            tPathFile.c_str(),errno, strerror(errno) );

                    continue;
                }
                tcwdpath[r] = '\0';
                if ( r > 1 && tcwdpath[r -1] == '/' )
                {
                    //去掉路径值最后的'/' 
                    tcwdpath[r-1] = '\0';
                }

                //b_write_log(_DEBUG,"tcwdpath=[%s],tstrName[1]=[%s]",
                //        tcwdpath, tstrName[1].c_str() );

                tpos = tstrName[1].find("/");
                if ( tpos == std::string::npos )
                {
                    strPathRun = tcwdpath;
                }
                else
                {
                    strPathRun = tcwdpath;
                    strPathRun += "/" + tstrName[1];
                }
            }// end cwd + sbudir

        }// end run p is script, to get path


        //b_write_log(_DEBUG,"exe_name=[%s],tpid=[%d],texepath=[%s],strPathRun=[%s]", 
        //        exe_name,tpid, texepath,strPathRun.c_str() );
        if ( retinfo.curnum >= SAMEEXE_MAX_PIDS  )
        {
            closedir( dirStream_p );
            return 1;
        }

        retinfo.pt[ retinfo.curnum ].pid  = tpid;
        retinfo.pt[ retinfo.curnum ].path = strPathRun;
        retinfo.curnum++;
    }

    closedir( dirStream_p );

    if ( 0 == retinfo.curnum )  
    {
        return -2;
    }

    return 0;
}




// ret:
//    0  成功
//    -1 opendir "/proc" error
//    -2 没找到进程
//    1  同程序名同时运行的pid超过系统最大限制
int ELFcmd::getPidByFullName( const char* full_exe_name, PNAME_RET_INFO &retinfo )
{
    retinfo.curnum = 0;

    DIR*   dirStream_p    = NULL;
    struct dirent* dir_fp = NULL;

    std::string rpdir="/proc";

    dirStream_p = opendir( rpdir.c_str() );
    if ( dirStream_p == NULL )
    {
        return -1;
    }

    int tpid = 0;
    int ret  = 0;

    char texepath[PV_DIR_MAX_LEN] = {0};
    char tcwdpath[PV_DIR_MAX_LEN] = {0};
    std::string strPathRun;
    std::string tPathFile;
    std::string tstrName[3];

    std::string fullexename = full_exe_name;
    std::string exe_name ;
    std::string exe_path;
    get_path_fname( fullexename, exe_path, exe_name);

    int  inoutnum = 1;

    while ( ( dir_fp = readdir(dirStream_p) ) != NULL)
    {
        if( strcmp(dir_fp->d_name,".") == 0 
                || strcmp(dir_fp->d_name,"..")==0
          )    //current dir OR parrent dir
        {
            continue;
        }

        if ( 0 == (tpid = atoi( dir_fp->d_name )) ) continue;

        //file:/proc/pid/status  取进程名
        tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
        tPathFile = tPathFile + "/" +std::string("status");
        inoutnum = 1;
        getPidStatus( tPathFile.c_str(), tstrName, inoutnum  );
        if ( inoutnum != 1 ) continue;

        //进程名与要查找的是否相等
        ret = strcmp( exe_name.c_str(), tstrName[0].c_str() );
        if ( ret != 0 ) continue;


        //file:/proc/pid/exe
        tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
        tPathFile = tPathFile + "/" +std::string("exe");
        
        //memset( texepath, 0 , PV_DIR_MAX_LEN );
        
        int r = readlink( tPathFile.c_str(), texepath, PV_DIR_MAX_LEN );
        if ( r < 0 || r >= PV_DIR_MAX_LEN )
        {
            b_write_log( _ERROR,"readlink [%s] fail,errno[%d],errMsg[%s]", 
                    tPathFile.c_str(),errno, strerror(errno) );

            continue;
        }
        texepath[r] = '\0';
        char *s = strrchr(texepath, '/');
        if ( NULL == s ) continue;

        *s = '\0';
        s++;
        ret = strcmp( exe_name.c_str(), s );
        if ( 0 == ret )
        {
            strPathRun = texepath;
        }
        else // ( ret != 0 ) 
        {
            //如果不相等:说明当前进程不是ELF文件，是类似shell脚本进程,这种情况
            // exe的路径并不是脚本的程序，需要拼音cwd及运行执行路径来得到脚本的路径

            // 取file:/proc/pid/cmdline 第二个参数(运行脚本 带相对路径)
            tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
            tPathFile = tPathFile + "/" +std::string("cmdline");
            inoutnum = 2;
            getCmdlinePar( tPathFile.c_str(), tstrName, inoutnum  );
            if (  inoutnum != 2 ) continue;


            if ( tstrName[1].compare(0,1,"/") == 0 )
            {
                //运行脚本是绝对路径直接取路径即可
                strPathRun = tstrName[1].erase(tstrName[1].find_last_of('/'));
                if ( strPathRun.empty() ) strPathRun = "/";
            }
            else
            {
                //运行脚本是相对路径则需要添加cwd

                if ( tstrName[1].compare(0,2,"./") == 0 )
                { 
                    tstrName[1].erase(0, 2 );
                }

                std::string::size_type tpos;
                if ( (tpos = tstrName[1].find_last_of("/")) != std::string::npos )
                {
                    tstrName[1].erase(tpos);
                }

                //memset( tcwdpath, 0 , PV_DIR_MAX_LEN );
                
                //file:/proc/pid/cwd
                tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
                tPathFile = tPathFile + "/" +std::string("cwd");

                r = readlink( tPathFile.c_str(), tcwdpath, PV_DIR_MAX_LEN );
                if ( r < 0 || r >= PV_DIR_MAX_LEN )
                {
                    b_write_log( _ERROR,"readlink [%s] fail,errno[%d],errMsg[%s]", 
                            tPathFile.c_str(),errno, strerror(errno) );

                    continue;
                }
                tcwdpath[r] = '\0';
                if ( r > 1 && tcwdpath[r -1] == '/' )
                {
                    //去掉路径值最后的'/' 
                    tcwdpath[r-1] = '\0';
                }

                //b_write_log(_DEBUG,"tcwdpath=[%s],tstrName[1]=[%s]",
                //        tcwdpath, tstrName[1].c_str() );

                tpos = tstrName[1].find("/");
                if ( tpos == std::string::npos )
                {
                    strPathRun = tcwdpath;
                }
                else
                {
                    strPathRun = tcwdpath;
                    strPathRun += "/" + tstrName[1];
                }
            }// end cwd + sbudir

        }// end run p is script, to get path

        if ( strPathRun != exe_path )
        {
            continue;
        }

        //b_write_log(_DEBUG,"exe_name=[%s],tpid=[%d],texepath=[%s],strPathRun=[%s]", 
        //        exe_name.c_str(),tpid, texepath,strPathRun.c_str() );
        if ( retinfo.curnum >= SAMEEXE_MAX_PIDS  )
        {
            closedir( dirStream_p );
            return 1;
        }

        retinfo.pt[ retinfo.curnum ].pid  = tpid;
        retinfo.pt[ retinfo.curnum ].path = strPathRun;
        retinfo.curnum++;
    }

    closedir( dirStream_p );

    if ( 0 == retinfo.curnum )  
    {
        return -2;
    }

    return 0;
}




// ret:
//    0  成功
//    -1 opendir "/proc" error
//    -2 没找到进程
//    1  同程序名同时运行的pid超过系统最大限制
int ELFcmd::getPidByFullName( const std::string &full_exe_name, PNAME_RET_INFO &retinfo )
{
    retinfo.curnum = 0;

    DIR*   dirStream_p    = NULL;
    struct dirent* dir_fp = NULL;

    std::string rpdir="/proc";

    dirStream_p = opendir( rpdir.c_str() );
    if ( dirStream_p == NULL )
    {
        return -1;
    }

    int tpid = 0;
    int ret  = 0;

    char texepath[PV_DIR_MAX_LEN] = {0};
    char tcwdpath[PV_DIR_MAX_LEN] = {0};
    std::string strPathRun;
    std::string tPathFile;
    std::string tstrName[3];

    std::string fullexename = full_exe_name;
    std::string exe_name ;
    std::string exe_path;
    get_path_fname( fullexename, exe_path, exe_name);

    //b_write_log(_DEBUG,"fusktest:fullexename[%s],exe_path[%s],exe_name[%s]",
    //        fullexename.c_str(), exe_path.c_str(), exe_name.c_str() );

    int  inoutnum = 1;

    while ( ( dir_fp = readdir(dirStream_p) ) != NULL)
    {
        if( strcmp(dir_fp->d_name,".") == 0 
                || strcmp(dir_fp->d_name,"..")==0
          )    //current dir OR parrent dir
        {
            continue;
        }

        if ( 0 == (tpid = atoi( dir_fp->d_name )) ) continue;

        //file:/proc/pid/status  取进程名
        tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
        tPathFile = tPathFile + "/" +std::string("status");
        inoutnum = 1;
        getPidStatus( tPathFile.c_str(), tstrName, inoutnum  );
        if ( inoutnum != 1 ) continue;

        //进程名与要查找的是否相等
        ret = strcmp( exe_name.c_str(), tstrName[0].c_str() );
        if ( ret != 0 ) continue;


        //file:/proc/pid/exe
        tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
        tPathFile = tPathFile + "/" +std::string("exe");
        
        //memset( texepath, 0 , PV_DIR_MAX_LEN );
        
        int r = readlink( tPathFile.c_str(), texepath, PV_DIR_MAX_LEN );
        if ( r < 0 || r >= PV_DIR_MAX_LEN )
        {
            b_write_log( _ERROR,"readlink [%s] fail,errno[%d],errMsg[%s]", 
                    tPathFile.c_str(),errno, strerror(errno) );

            continue;
        }
        texepath[r] = '\0';
        char *s = strrchr(texepath, '/');
        if ( NULL == s ) continue;

        *s = '\0';
        s++;
        ret = strcmp( exe_name.c_str(), s );
        if ( 0 == ret )
        {
            strPathRun = texepath;
        }
        else // ( ret != 0 ) 
        {
            //如果不相等:说明当前进程不是ELF文件，是类似shell脚本进程,这种情况
            // exe的路径并不是脚本的程序，需要拼音cwd及运行执行路径来得到脚本的路径

            // 取file:/proc/pid/cmdline 第二个参数(运行脚本 带相对路径)
            tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
            tPathFile = tPathFile + "/" +std::string("cmdline");
            inoutnum = 2;
            getCmdlinePar( tPathFile.c_str(), tstrName, inoutnum  );
            if (  inoutnum != 2 ) continue;


            if ( tstrName[1].compare(0,1,"/") == 0 )
            {
                //运行脚本是绝对路径直接取路径即可
                strPathRun = tstrName[1].erase(tstrName[1].find_last_of('/'));
                if ( strPathRun.empty() ) strPathRun = "/";
            }
            else
            {
                //运行脚本是相对路径则需要添加cwd

                if ( tstrName[1].compare(0,2,"./") == 0 )
                { 
                    tstrName[1].erase(0, 2 );
                }

                std::string::size_type tpos;
                if ( (tpos = tstrName[1].find_last_of("/")) != std::string::npos )
                {
                    tstrName[1].erase(tpos);
                }

                //memset( tcwdpath, 0 , PV_DIR_MAX_LEN );
                
                //file:/proc/pid/cwd
                tPathFile = rpdir + "/" + std::string( dir_fp->d_name );
                tPathFile = tPathFile + "/" +std::string("cwd");

                r = readlink( tPathFile.c_str(), tcwdpath, PV_DIR_MAX_LEN );
                if ( r < 0 || r >= PV_DIR_MAX_LEN )
                {
                    b_write_log( _ERROR,"readlink [%s] fail,errno[%d],errMsg[%s]", 
                            tPathFile.c_str(),errno, strerror(errno) );

                    continue;
                }
                tcwdpath[r] = '\0';
                if ( r > 1 && tcwdpath[r -1] == '/' )
                {
                    //去掉路径值最后的'/' 
                    tcwdpath[r-1] = '\0';
                }

                //b_write_log(_DEBUG,"tcwdpath=[%s],tstrName[1]=[%s]",
                //        tcwdpath, tstrName[1].c_str() );

                tpos = tstrName[1].find("/");
                if ( tpos == std::string::npos )
                {
                    strPathRun = tcwdpath;
                }
                else
                {
                    strPathRun = tcwdpath;
                    strPathRun += "/" + tstrName[1];
                }
            }// end cwd + sbudir

        }// end run p is script, to get path

        if ( strPathRun != exe_path )
        {
            continue;
        }

        //b_write_log(_DEBUG,"exe_name=[%s],tpid=[%d],texepath=[%s],strPathRun=[%s]", 
        //        exe_name.c_str(),tpid, texepath,strPathRun.c_str() );
        if ( retinfo.curnum >= SAMEEXE_MAX_PIDS  )
        {
            closedir( dirStream_p );
            return 1;
        }

        retinfo.pt[ retinfo.curnum ].pid  = tpid;
        retinfo.pt[ retinfo.curnum ].path = strPathRun;
        retinfo.curnum++;
    }

    closedir( dirStream_p );

    if ( 0 == retinfo.curnum )  
    {
        return -2;
    }

    return 0;
}




/*************************************
  *功能 ：执行shell程序
  * 输入：shell名、shell需要的参数
  * 输出：无
  *返回：
  *     　0         :成功，
  *       >0        :shell程序抛出的异常值
  *       <0        :失败
*************************************/
int ELFcmd::executeShell ( const char* shName_p, const char* szFormat_p, ... )
{
    if ( shName_p == NULL )
    {
        b_write_log( _ERROR, "input parm shName_p is NULL");
        return -1;
    }

    char  szParam[256]     = {0};
    char  szCommand[256 * 2] = {0};
    int   actLen           = 0;
    pid_t status           = 0;
    int   vRet             = 0;

    if ( szFormat_p != NULL )
    {
        va_list pvList;
        va_start ( pvList, szFormat_p );

        //把所有参数都存入szParam字符串
        actLen = vsprintf ( szParam, szFormat_p, pvList ); 


        if ( ( actLen < 0 ) || (  actLen >= 256 ) )
        {
            b_write_log( _ERROR,"actLen=[%d],par fix max len=[256]", actLen);

            return -2;
        }

        va_end ( pvList );

        sprintf ( szCommand, "%s %s", shName_p, szParam );
    }
    else
    {
        sprintf ( szCommand, "%s", shName_p );
    }

    sighandler_t old_handler;
    old_handler = signal ( SIGCHLD, SIG_DFL );

    status = system ( szCommand );

    signal ( SIGCHLD, old_handler );

    if ( status == -1 )
    {
        //system 在创建子进程等准备工作 失败
        b_write_log( _ERROR,"system Failed to create child processes and other preparations");
        return -3;
    }
    else
    {
        if ( ! ( WIFEXITED ( status ) ) )
        {
            //system 执行shell异常结束(如被强制kill掉)
            b_write_log( _ERROR,"system The execution shell ends abnormally (such as being forcibly killed)");
            return -4;
        }
        else
        {
            //vRet 一般为0 表示shell执行正常；其他情况则为shell中抛出的异常值
            vRet = WEXITSTATUS ( status );
            return vRet;
        }

    }

    return 0;
}





void ELFcmd::kill9Pbypid( const int &killAllFlag, const pid_t &pid, int &status )
{
    if ( killAllFlag == 1 )
    {
        char tPidFile[30] = {0};
        char tStr[256] = {0};
        strcpy( tPidFile, ".plusTmp_XXXXXX" );

        int fd = mkstemp( tPidFile );           
        if ( fd < 0 )
        {
            b_write_log(_ERROR,"mkstemp(%s),errno[%d],errMsg=[%s]",
                    tPidFile, errno,strerror ( errno ) );

            status = -1;
            return;
            //break;

        }

        snprintf ( tStr, sizeof ( tStr ),
                   "pstree -np %d|awk -F'(' '{i = 2; while "
                   "(i <= NF) {print $i;i++}}'|awk -F')' '"
                   "{print $1}'|sort -r >%s",
                   pid, tPidFile
                 );

        //b_write_log(_DEBUG,"tPidFile=[%s",tPidFile);
        system ( tStr );

        FILE* fp  = fdopen ( fd, "rb" );

        if ( fp == NULL )
        {
            b_write_log(_ERROR,"fdopen(%d,rb) error,errno[%d],errMsg=[%s]",
                    fd, errno,strerror ( errno )  );

            status = -1;
            return;
            //break;
        }

        int tPid = 0;
        int tNum = 0;
        int tFNum = 0;
        char tStrErr[100];
        memset ( tStrErr, 0, sizeof ( tStrErr ) );

        while ( fscanf ( fp, "%d", &tPid ) > 0 )
        {
            tNum++;

            if ( kill ( tPid, SIGKILL ) == 0 )
            {
                waitpid ( tPid, NULL, 0 );
            }
            else
            {
                tFNum++;
                snprintf ( tStrErr, sizeof ( tStrErr ),
                           "errno[%d],errMsg=[%s]", errno,
                           strerror ( errno ) );
            }
        }

        if ( fp != NULL )
        {
            fclose ( fp );
            fp = NULL;
        }

        unlink( tPidFile );

        if ( tNum > 0 && tNum == tFNum )
        {
            status = -3;

            b_write_log(_ERROR,"After the shell script execution time-out,the kill process is abnormal,%s",tStrErr);
        }
        else
        {
            status = -2;
        }

    }
    else
    {
        if ( kill ( pid, SIGKILL ) == 0 )
        {
            waitpid ( pid, NULL, 0 );
            status = -2;
            //b_write_log(_DEBUG,"kill[%d]",pid);
        }
        else
        {
            b_write_log(_ERROR,"After the shell script execution time-out,the kill process is abnormal,errno[%d],errMsg[%s]",errno, strerror ( errno ) );

            status = -3;
        }
    }

    return;
}





/*******************************************************************
  *功能 ：执行shell程序
  * 输入：
        *cmdStr_p,    需要执行的shell命令或shell脚本
        &timeout,     大于0表示超时时间(单位秒)等于0为不超时
        &times,       超时后需要重新执行的的次数
        &killAllFlag, 1:超时后kill脚本派生的所有进程;0只kill脚本进程
  *返回：
  *     　0           成功，
  *       >0 且!=127  shell程序抛出的异常值
  *       -1          失败
  *       -2          shell执行超时
  *       -3          shell执行超时后kill进程错误
  *注意:
  *     要想利用脚本回返值来判断脚本的执行逻辑,需要在脚本中显示的用
        exit value (value的值是需要小于127的正数)
********************************************************************/
int ELFcmd::system_plus ( const char* cmdStr_p, 
        const int& timeout, 
        const int& times, 
        const int& killAllFlag
        )
{
    if ( NULL == cmdStr_p )
    {
        b_write_log( _ERROR, "input parm cmdStr_p is NULL");
        return -1;
    }

    int tTimes = ( times > 0 ) ? times : 0;

    errno = 0;

    pid_t pid       = 0;
    int   status    = 0;
    bool  needRetry = true;

    struct sigaction ignore;
    struct sigaction saveintr;
    struct sigaction savequit;
    sigset_t chldmask;
    sigset_t savemask;

    memset ( &ignore,   0, sizeof ( ignore ) );
    memset ( &saveintr, 0, sizeof ( saveintr ) );
    memset ( &savequit, 0, sizeof ( savequit ) );
    memset ( &chldmask, 0, sizeof ( chldmask ) );
    memset ( &chldmask, 0, sizeof ( chldmask ) );


    ignore.sa_handler = SIG_IGN;
    sigemptyset ( &ignore.sa_mask );
    ignore.sa_flags = 0;

    if ( sigaction ( SIGINT, &ignore, &saveintr ) < 0 )
    {
        b_write_log( _ERROR,
                "sigaction SIGINT,errno[%d],errmsg[%s]",
                errno, strerror ( errno ) );

        return -1;
    }

    if ( sigaction ( SIGQUIT, &ignore, &savequit ) < 0 )
    {
        b_write_log( _ERROR,
                "sigaction SIGQUIT,errno[%d],errmsg[%s]",
                errno, strerror ( errno ) );

        return -1;
    }

    sigemptyset ( &chldmask );
    sigaddset ( &chldmask, SIGCHLD );

    if ( sigprocmask ( SIG_BLOCK, &chldmask, &savemask ) < 0 )
    {
        b_write_log( _ERROR,
                "sigprocmask SIG_BLOCK,errno[%d],errmsg[%s]",
                errno, strerror ( errno ) );

        return -1;
    }

    for ( int i = -1; i < tTimes && true == needRetry; i++ )
    {
        needRetry = false;

        if ( ( pid = fork() ) < 0 )
        {
            b_write_log( _ERROR,
                    "fork child process,errno[%d],errmsg[%s]", 
                    errno, strerror ( errno ) );

            status = -1;
        }
        else if ( 0 == pid )
        {
            sigaction ( SIGINT, &saveintr, NULL );
            sigaction ( SIGQUIT, &savequit, NULL );
            sigprocmask ( SIG_SETMASK, &savemask, NULL );

            execlp ( "sh", "sh", "-c", cmdStr_p, ( char* ) 0 );

            _exit ( 127 );

        }
        else
        {
            int leftTime = timeout;
            int rv;
            int options = ( timeout > 0 ) ? WNOHANG : 0 ;

            while ( ( rv = waitpid ( pid, &status, options ) ) <= 0 )
            {
                if ( -1 == rv && EINTR != errno )
                {
                    b_write_log(_ERROR,
                            "waitpid [%d] returns error,errno[%d],errmsg[%s]",
                            pid, errno, strerror ( errno ) );

                    status = -1;
                    break;
                }
                if ( timeout <= 0 ) continue;

                //sleep ( 1 );
                PauseThreadSleep( 1, 0 );

                leftTime--;

                if ( leftTime > 0 ) continue;

                kill9Pbypid( killAllFlag, pid, status );

                needRetry = true;
                break;
            }
        }//end 父进程
    }//end 重试

    if ( sigaction ( SIGINT, &saveintr, NULL ) < 0 )
    {
        b_write_log(_ERROR,
                "sigaction SIGINT ignore:errno[%d],errmsg[%s]",
                errno, strerror ( errno ) );

        return -1;
    }

    if ( sigaction ( SIGQUIT, &savequit, NULL ) < 0 )
    {
        b_write_log(_ERROR,
                "sigaction SIGQUIT ignore:errno[%d],errmsg[%s]",
                errno, strerror ( errno ) );

        return -1;
    }

    if ( sigprocmask ( SIG_SETMASK, &savemask, NULL ) < 0 )
    {
        b_write_log(_ERROR,
                "sigprocmask SIG_SETMASK savemask:errno[%d],errmsg[%s]",
                errno, strerror ( errno ) );

        return -1;
    }

    int tRetValue = ( WIFEXITED ( status ) ) ? ( WEXITSTATUS ( status ) ) : status;

    if ( tRetValue == 127 )
    {
        b_write_log(_ERROR,
                "child process execlp returns returns error");
    }

    return tRetValue;

}


//  mod null  判断文件或目录是否存在
//  mod "rwx" 判断文件或目录是否有相应权限
bool ELFcmd::fd_exist( const char* f_p ,const char* mode )
{
    if ( NULL == f_p ) return false;

    int tmd = F_OK;

    //b_write_log(_DEBUG,"[%s][%s][%d]",f_p,mode,tmd);

    if ( mode != NULL )
    {
        const char* l = NULL; const char* u = NULL;

        l = strchr(mode,'r'); u = strchr(mode, 'R');
        if ( l != NULL || u != NULL ) tmd |= R_OK;

        l = strchr(mode,'w'); u = strchr(mode, 'W');
        if ( l != NULL || u != NULL ) tmd |= W_OK;

        l = strchr(mode,'x'); u = strchr(mode, 'X');
        if ( l != NULL || u != NULL ) tmd |= X_OK;
    }

    //b_write_log(_DEBUG,"[%s][%s][%d]",f_p,mode,tmd);
    
    if ( 0 == ( access(f_p,tmd) ) ) return true;

    return false;
}

bool ELFcmd::isFile( const char* f_p )
{
    if ( NULL == f_p ) return false;

    struct stat sb;

    if ( stat(f_p, &sb) == -1) return false;

    if ( (sb.st_mode & S_IFMT) != S_IFREG ) return false;

    return true;
}


bool ELFcmd::isDir( const char* f_p )
{
    if ( NULL == f_p ) return false;

    struct stat sb;

    if ( stat(f_p, &sb) == -1) return false;

    if ( (sb.st_mode & S_IFMT) != S_IFDIR ) return false;

    return true;
}


//mkdir -p dir
int ELFcmd::mkdirs( const std::string& dir )
{


    if ( isDir(dir.c_str()) ) return 0;

    std::size_t iIndex = 0;
    std::size_t iPos   = 0;
    std::string sub;

    mode_t mkmode = 0755;

    while ( ( iPos = dir.find('/', iIndex) ) != std::string::npos )
    {
        sub = dir.substr( 0, iPos+1 );
        if ( (!sub.empty()) && (!isDir(sub.c_str())) ) 
        {
            if ( 0 != (mkdir( sub.c_str(), mkmode )) )
            {
                b_write_log(_ERROR,"mkdir dir[%s]fail,errno[%d],errMsg[%s]",
                        sub.c_str(), errno, strerror(errno) );

                return -1;
            }
        }
        iIndex = iPos + 1;
    }

    //does not end with '/'
    if ( sub != dir )
    {
        if ( 0 != (mkdir( dir.c_str(), mkmode )) )
        {
            b_write_log(_ERROR,"mkdir dir[%s]fail,errno[%d],errMsg[%s]",
                    dir.c_str(), errno, strerror(errno) );

            return -1;
        }
    }

    return 0;
}

//rm -rf fds
int ELFcmd::rm( const char* fds )
{
    if ( NULL == fds ) return 0;

    if ( ! fd_exist( fds ) ) return 0;

    if ( ! isDir( fds ) ) 
    {
        //b_write_log(_DEBUG," remove [%s]", fds);
        return remove( fds );
    }

    DIR*   dirStream_p    = NULL;
    struct dirent* dir_fp = NULL;

    dirStream_p = opendir( fds );
    if ( dirStream_p == NULL ) return 1;

    while ( ( dir_fp = readdir(dirStream_p) ) != NULL)
    {
        if( strcmp(dir_fp->d_name,".") == 0 
                || strcmp(dir_fp->d_name,"..")==0
          )    ///current dir OR parrent dir
        {
            continue;
        }

        std::string tPathFile;
        if ( '/' == fds[strlen(fds)-1] )
        {
            tPathFile = std::string(fds) + std::string( dir_fp->d_name );
        }
        else
        {
            tPathFile = std::string(fds) + "/" + std::string( dir_fp->d_name );
        }

        //b_write_log(_DEBUG," into [%s]", tPathFile.c_str());

        rm( tPathFile.c_str() );
    }

    closedir( dirStream_p );

    //b_write_log(_DEBUG," remove [%s]", fds);
    return remove( fds );
}


//获得全路径文件名 中的 文件名 和 路径值
//  eg: /home/fusky/1.txt  ->   path=/home/fusky/   fname=1.txt
//获得路径值       中的 最后一级目录 和最后一级目录所在目录
//  eg: /home/fusky/   ->  path=/home   fname=fusky
//
int ELFcmd::get_path_fname(
        const std::string &src, 
        std::string &path, 
        std::string &fname
        )
{
    if ( src.empty() )
    {
        path  = ""; fname = "";
        return 0;
    }

    std::size_t found = src.rfind("/");
    if ( found != std::string::npos )
    {
        path  = src.substr(0,found);

        //b_write_log(_DEBUG,"found=[%ld],src.length=[%ld],path=[%s]",
        //        found,src.length(), path.c_str() );

        if ( found != (src.length() -1) ) 
        {
            fname = src.substr(found +1);
        }
        else
        {
            std::string tsrc = path;
            //fname = "";
            get_path_fname(tsrc, path, fname);
        }

        return 0;
    }

    path  = "./"; fname = src; 
    return 0;
}


int ELFcmd::mv( const char* src, const char* dst)
{
    if ( NULL == src || NULL == dst ) return -1;

    if ( ! fd_exist(src) ) return -2;

    bool dstIsDir = isDir(dst);
    if ( fd_exist(dst) && (!dstIsDir) ) return -3;
    int ret = 0;

    if ( dstIsDir )
    {
        std::string tpath; std::string tfname;
        get_path_fname( src, tpath, tfname );

        std::string tdst;
        if ( dst[strlen(dst)-1] == '/' )
        {
            tdst = std::string(dst) + tfname;
        }
        else
        {
            tdst = std::string(dst) + "/" + tfname;
        }

        ret =  rename ( src, tdst.c_str() );
        if ( ret != 0 )
        {
            b_write_log(_ERROR,"rename(%s,%s) return[%d],errno[%d],errMsg[%s]",
                    src, tdst.c_str(), ret, errno, strerror(errno) );
            return -4;
        }

        return 0;

    }

    ret =  rename ( src, dst );
    if ( ret != 0 )
    {
        b_write_log(_ERROR,"rename(%s,%s) return[%d],errno[%d],errMsg[%s]",
                src, dst, ret, errno, strerror(errno) );
    }
    
    return 0;
}


//cp -r src  dst
int ELFcmd::cp( const char* src, const char* dst)
{
    if ( NULL == src || NULL == dst ) return -1;

    if ( ! fd_exist(src) ) return -2;

    if ( ! fd_exist(src,"r") ) return -3;

    bool dstExist = fd_exist(dst); 
    bool dstIsDir = isDir(dst);
    bool srcIsDir = isDir(src);

    //源为目录,目标必须为存在的目录
    if ( srcIsDir && ( ! dstExist ) ) return -4;
    if ( srcIsDir && ( ! dstIsDir ) ) return -5;

    std::string tsrc = src;
    std::string tdst = dst;

    //目标文件的上级目录必须存在
    if( ! dstExist )
    {
        std::string dstPath;  std::string dstFname;
        get_path_fname( tdst, dstPath, dstFname );
        if ( ! isDir(dstPath.c_str()) ) return -6;
    }


    //源是文件
    if ( ! srcIsDir )
    {
        char tmpbuf[MC_CBUFFER_SIZE] = {0};
        int lenR = 0;
        int lenW = 0;

        if ( dstIsDir )
        {
            std::string srcPath;  std::string srcFname;
            get_path_fname( tsrc, srcPath, srcFname );
            if ( dst[strlen(dst)-1] == '/' )
            {
                tdst += srcFname;
            }
            else
            {
                tdst += "/"; tdst += srcFname;
            }
        }

        //b_write_log(_DEBUG,": cp [%s] [%s]",src, tdst.c_str());
        
        FILE* fpR_p = NULL;
        FILE* fpW_p = NULL;
        if ( ( fpR_p = fopen ( src, "r" ) ) == NULL )
        {
            b_write_log(_ERROR," fopen(%s),errno[%d],errMsg[%s]",
                    src, errno, strerror ( errno ) );
            return -7;
        }

        if ( ( fpW_p = fopen ( tdst.c_str(), "w" ) ) == NULL )
        {
            fclose ( fpR_p );
            b_write_log(_ERROR," fopen(%s),errno[%d],errMsg[%s]",
                    tdst.c_str(), errno, strerror ( errno ) );
            return -8;
        }

        memset ( tmpbuf, 0, MC_CBUFFER_SIZE );

        while ( ( lenR = fread ( tmpbuf, 1, MC_CBUFFER_SIZE, fpR_p ) ) > 0 )
        {
            if ( ( lenW = fwrite ( tmpbuf, 1, lenR, fpW_p ) ) != lenR )
            {
                fclose ( fpR_p );
                fclose ( fpW_p );
                b_write_log(_ERROR,"fwrite[%s] error lenW=[%d],lenR=[%d]",
                        tdst.c_str(), lenW, lenR);

                return -9;
            }

            memset ( tmpbuf, 0, MC_CBUFFER_SIZE );
        }

        fclose ( fpR_p );
        fclose ( fpW_p );

        return 0;
    }


    //srcIsDir  dstIsDir
    tdst = dst;
    tsrc = src;
    std::string srcPath;  std::string srcFname;
    get_path_fname( tsrc, srcPath, srcFname );
    if ( dst[strlen(dst)-1] == '/' )
    {
        tdst += srcFname;
    }
    else
    {
        tdst += "/"; tdst += srcFname;
    }

    //建立目标目录
    int ret = mkdirs( tdst );
    if ( ret != 0 )
    {
        b_write_log(_ERROR,"mkdirs [%s] return[%d]", tdst.c_str(), ret);
        return -10;
    }

    //b_write_log(_DEBUG,": opendir [%s]",src);

    //遍历源目录进行拷贝
    DIR*   dirStream_p    = NULL;
    struct dirent* dir_fp = NULL;

    dirStream_p = opendir( src );
    if ( dirStream_p == NULL )
    {
        b_write_log(_ERROR,"opendir [%s] errno[%d],errMsg[%s]", 
                src, errno, strerror ( errno ) );
        return -11;
    }

    while ( ( dir_fp = readdir(dirStream_p) ) != NULL)
    {
        if( strcmp(dir_fp->d_name,".") == 0 
                || strcmp(dir_fp->d_name,"..")==0
          )    ///current dir OR parrent dir
        {
            continue;
        }

        tsrc = src;
        if ( src[strlen(src)-1] == '/' )
        {
            tsrc += std::string( dir_fp->d_name );
        }
        else
        {
            tsrc += "/"; 
            tsrc += std::string( dir_fp->d_name );
        }

        //b_write_log(_DEBUG,": will into cp [%s] [%s]",
        //        tsrc.c_str(), tdst.c_str());

        cp( tsrc.c_str(), tdst.c_str() );

    }

    closedir( dirStream_p );
    
    return 0;
}








ELFcmd::ELFcmd()
{

}




ELFcmd::~ELFcmd()
{

}




