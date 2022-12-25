/***********************************************************
 *
 * @file    ELFcmd.h
 *
 * @brief   ELFcmd头文件
 *
 * @author  fu.sky
 *
 * @date    2022-11-17_09:19:45
 *
 * @version V10.010.000
 *
 ***********************************************************/

#ifndef _ELF_CMD_H_
#define _ELF_CMD_H_

#include <string>
#include <errno.h>




#ifndef SAMEEXE_MAX_PIDS
    #define SAMEEXE_MAX_PIDS 10   //同程序名的程序可同时运行的pid的最大个数
#endif
#ifndef PV_DIR_MAX_LEN
    #define PV_DIR_MAX_LEN   1024
#endif


//根据程序名查找进程返回信息结构
typedef struct
{
    int curnum;
    struct{
        std::string path;    /*程序路径*/
        int pid;             /*程序进程号*/
    }pt[SAMEEXE_MAX_PIDS];
}PNAME_RET_INFO;




class ELFcmd
{

private:
    ELFcmd();
    ~ELFcmd();

public:

    // cnt : 文件路径 或 要加密的字符串
    // omd5: 文件或输入字符串的md5值
    // return:
    //       0  成功
    //       !0 失败
    static int md5sum( char* cnt, char omd5[32+1] );

    // 取/proc/pid/cmdline文件的内容
    static int getCmdlinePar( const char* path, std::string str[],int &inoutnum );

    // 取/proc/pid/status文件的内容
    //  str[0] Name   进程名
    //  str[1] State  进程的状态
    //  str[2] PPid   进程的父进程
    //  str[3] VmSize 进程现在正在占用的内存
    //  str[4] VmPeak 当前进程运行过程中占用内存的峰值
    //  str[5] VmRSS  是程序现在使用的物理内存
    //  str[6] VmHWM  是程序得到分配到物理内存的峰值
    static int getPidStatus( const char* path, std::string str[],int &inoutnum  );
    
    //根据程序名获得程序路径和运行pid
    // ret:
    //    0  成功
    //   -1 opendir "/proc" error
    //    -2 没找到进程
    //    1  同程序名同时运行的pid超过系统最大限制
    static int getPidByName( const char* exe_name, PNAME_RET_INFO &retinfo );
    static int getPidByName( const std::string &exe_name, PNAME_RET_INFO &retinfo );

    //根据程序全名(带绝对路径值)获得程序路径和运行pid
    // ret:
    //    0  成功
    //   -1 opendir "/proc" error
    //    -2 没找到进程
    //    1  同程序名同时运行的pid超过系统最大限制
    static int getPidByFullName( const char* full_exe_name, PNAME_RET_INFO &retinfo );
    static int getPidByFullName( const std::string &full_exe_name, PNAME_RET_INFO &retinfo );

    /*************************************
      *功能 ：执行shell程序
      * 输入：shell名、shell需要的参数
      * 输出：无
      *返回：
      *     　0         :成功，
      *       >0        :shell程序抛出的异常值
      *       <0        :失败
    *************************************/
    static int executeShell ( 
            const char* shName_p, 
            const char* szFormat_p, ... 
            );

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
    static int system_plus ( const char* cmdStr_p,
                      const int& timeout,
                      const int& times,
                      const int& killAllFlag
                    );

    //  mod null  判断文件或目录是否存在
    //  mod "rwx" 判断文件或目录是否有相应权限
    static bool fd_exist( const char* f_p ,const char* mode = NULL );

    static bool isFile( const char* f_p );
    static bool isDir( const char* f_p );

    //mkdir -p dir
    static int mkdirs( const std::string& dir );

    //rm -rf  fds
    static int rm( const char* fds );

    static int mv( const char* src, const char* dst);

    //cp -r src dst
    static int cp( const char* src, const char* dst);
    
    //获得全路径文件名 中的 文件名 和 路径值
    //  eg: /home/fusky/1.txt  ->   path=/home/fusky/   fname=1.txt
    //获得路径值       中的 最后一级目录 和最后一级目录所在目录
    //  eg: /home/fusky/   ->  path=/home   fname=fusky
    //
    static int get_path_fname(
            const std::string &src, 
            std::string &path, 
            std::string &fname
            );

private:
    static void kill9Pbypid( const int &killAllFlag, 
            const pid_t &pid, 
            int &status 
            );

};



#endif//_ELF_CMD_H_
