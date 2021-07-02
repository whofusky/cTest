#ifndef _BASEFUNC_H_
#define _BASEFUNC_H_

//#include <stdio.h>
#include <sys/types.h>
//#include <unistd.h>
#include <sys/wait.h>
//#include <string.h>
#include<time.h>
#include<sys/time.h>
//#include<stdlib.h>

#include<unistd.h>
#include<fcntl.h>

#include<stdarg.h>
//#include<dirent.h>
#include<sys/stat.h>
#include<errno.h>
#include<regex.h>
#include <signal.h>


//#include "baseMacro.h"
#include "errorMacro.h"


//环境变量名
#define MC_PDEBUG_XML                "METEDELOG"    //配置mete_debug.xml路径 的环境变量名
#define MC_PLOCAL_XML                "METELOCACFG"  //配置locaCfg.xml路径 的环境变量名
#define MC_GETPIDNUM                "METEGETPIDNUM" //获取公共getPidNum.sh的环境变量名


//定义文件名相关长度
#define MC_PATH_LEN               100    //文件路径长度
#define MC_NAME_LEN               50     //文件名长度
#define MC_SUFNAME_LEN            20     //文件后缀名长度
#define MC_FULLN_LEN              170    //全文件名长度 100+50+20

#define MC_ONECE_FNUM             20     //ftp一次支持处理最大文件个数


#define MC_ERRMSG_MAX             512    //错误信息临时变量大小

#define MC_RETCODE_LEN            12    //返回代码长度
#define MC_RETMSG_LEN             512    //返回信息长度

//其他相关长度
#define MC_TIMES_LEN              25    //时间字符串最大长度
#define MC_XMLELE_LEM             30    //xml元素名最大长度
#define MC_CBUFFER_SIZE           200   //考贝函数buffer大小
#define MC_IP_LEN                 20    //ip地址最大长度
#define MC_USERNAME_LEN           20    //用户名最大长度
#define MC_USERPWD_LEN            20    //用户密码最大长度

//定义xml配置文件元素名
#define MC_L_TOPRINT            "toPrint"         //打印标识元素节点


typedef void ( *sighandler_t ) ( int );

/* 统一错误信息处理*/
#define outPutErrM(oErrMsg_p,errMsg)\
    {\
        if((oErrMsg_p)!=NULL)\
        {\
            snprintf((char*)(oErrMsg_p),MC_ERRMSG_MAX-1,\
                     "%s|%s|%d:%s\n",\
                     __FILE__,__func__,__LINE__,(errMsg));\
        }\
        else\
        {\
            printf("%s|%s|%d:%s\n",\
                   __FILE__,__func__,__LINE__,(errMsg));\
        }\
    }

//thead-safe
int nanoSleep ( const time_t& tv_sec,
                const long&   tv_nsec
              );
int sleepSecond ( const size_t second );
int sleepMilliSecond ( const long millisecond );
int sleepMicroSecond ( const long microsecond );




/*************************************
  *功能 ：字符串的头和尾去掉空格 TAB 回车
  * 输入：源字符串
  *
  * 输出：去掉空格TAB回车后的字符串
  *返回：
  *     去掉空格TAB回车后的字符串
  *
  *修改：
  *    　20170705180921 创建函数
*************************************/
char* Trim ( char* S_p );

/*************************************
  *功能 :用正则表达式格式匹配字符串
  * 输入：
  *       bufStr_p  :要匹配的字符串
  *       pattern_p :正则表达式格式字符串(POSIX正则表达式)
  *
  * 输出：
  *       oErrMsg_p  如果此变量为NULL则错误信息直接printf
  *                  如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *返回：
  *     0  成功
  *    非0 不成功
  *
  *修改：
  *    　20170807150552 创建函数
  *注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int regexMatch ( const char* bufStr_p,
                 const char* pattern_p,
                 const char* oErrMsg_p = NULL
               );


/*************************************
  *功能 ：把指定字符串按指定分隔符，分别存放到后面的字符串指针数组中
  * 输入：源字符串
  *       分隔符
  *
  * 输出：ptrs_pp
  *返回：
  *     <0 失败
  *     >=0 src_p中包含域的数量
  *
  *修改：
  *    　20170717152447 创建函数
  *注意:
  *    经过此函数后 src_p 中分隔符的已经替换成字符串结束符了。
*************************************/
int strToPrts ( char* src_p,
                const char* separateStr_p,
                char** ptrs_pp
              );


/*************************************
  *功能 ：获得时间值
  * 输入：tmtype    :需要获得的时间类型
  *
  * 输出：tmLocal_p ：得到的时间值
  *       tmUsec_p  : 得到微秒时间值
  *                   tmtype=0:
  *                       tmLocal_p 有值:%Y-%m-%d %H:%M:%S.微秒
  *                       tmUsec_p  无值
  *                   tmtype=1:
  *                       tmLocal_p 无值
  *                       tmUsec_p  有值:系统时间到 1970/1/1 0:0:0 所经过的微秒数
  *                   tmtype=2:
  *                       tmLocal_p 有值:%Y-%m-%d %H:%M:%S.微秒
  *                       tmUsec_p  有值:系统时间到 1970/1/1 0:0:0 所经过的微秒数
  *                   tmtype=3:
  *                       tmLocal_p 有值:获得系统时间:%Y%m%d
  *                       tmUsec_p  无值
  *                   tmtype=4:
  *                       tmLocal_p 有值:获得系统时间:%Y-%m-%d:%H-%M-%S
  *                       tmUsec_p  无值
  *       oErrMsg_p  如果此变量为NULL则错误信息直接printf
  *                  如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *返回：
  *     　0         :成功，
  *     　负数      :失败
  *修改：
  *    　20170628173607 创建函数
  *注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int getTime ( const int& tmtype,
              char* tmLocal_p,
              char* tmUsec_p,
              const char* oErrMsg_p = NULL
            );

/*************************************
  *功能 ：文件是否存在
  * 输入：
  *      fileName_p:需要判断的文件名
  *
  * 输出：
  *      oErrMsg_p  如果此变量为NULL则错误信息直接printf
  *                 如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *返回：
  *     0  文件存在
  *     1  文件不存在
  *     -1 错误
  *     　
  *修改：
  *    　20170707100216 创建函数
  *注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int fileExists ( const char* fileName_p,
                 const char* oErrMsg_p = NULL
               );

/*************************************
  *功能 ：文件状态是否正确
  * 输入：
  *      fileName_p:需要判断的文件名
  *
  * 输出：
  *      oErrMsg_p  如果此变量为NULL则错误信息直接printf
  *                 如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *
  *返回：
  *     0  文件状态正常
  *     -1 错误
  *     　
  *修改：
  *    　20170707102111 创建函数
  *注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int checkFileStat ( const char* fileName_p,
                    const char* oErrMsg_p = NULL
                  );

/*************************************
  *功能 ：获取文件状态时间与当前时间的差值(单位秒) 及 状态时间
  * 输入：fileName_p 带路径的文件名
  *       flag       标识:0 求访问时间差值 ;1 求修改时间的差值; 2求改变时间的差值
  *
  * 输出：
  *       subSecs    当前时间与文件状态时间的差值
  *       statTime_p 文件状态时间 (具体的时间格式可以根据timeFlag扩展)
  *       oErrMsg_p  如果此变量为NULL则错误信息直接printf
  *                  如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *返回：
  *     0  正确
  *     <0 错值
  *     　
  *修改：
  *    　20170707102111 创建函数
  *注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int getSubFSec ( const char* fileName_p,
                 const int& flag,
                 long& subSecs,
                 char* statTime_p,
                 const char* oErrMsg_p = NULL,
                 const int& timeFlag = 0
               );


/*************************************
  *功能 ：判断是否为目录
  * 输入：
  *     dirName_p :目录名
  *
  *
  * 输出：
  *      oErrMsg_p  如果此变量为NULL则错误信息直接printf
  *                 如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *返回：
  *     0 是目录
  *     1 不是
  *     -1 错误
  *
  *修改：
  *    　20170707134628 创建函数
  *注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int isDir ( const char* dirName_p,
            const char* oErrMsg_p = NULL
          );

/*************************************
* 功能：将全路径名解析成路径名和文件名
* 输入：
*       fullFileName_p: 全路径文件名
*       sptFlag       :0 取路径和文件名，1 取路径名，2 取文件名
*
* 输出：
*       dirName_p     :路径名
*       fileName_p    :文件名
*       oErrMsg_p     如果此变量为NULL则错误信息直接printf
*                     如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
* 返回：
*     0    成功
*     <0   输入错误
*     >0   未找到路径名
*
*修改：
*    　20170707134628 创建函数
*注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int sptDirName ( const char* fullFileName_p,
                 char* dirName_p,
                 char* fileName_p,
                 const char* oErrMsg_p = NULL,
                 const int& sptFlag = 0
               );


/*************************************
* 功能：将文件路径和文件名拼接成全路径名
*         如果文件路径是带文件名，则替换原有文件名
*         如果文件名有路径，只取文件名
* 输入：
*       dirName_p     :路径名(如果是文件夹则需要在后面加/)
*       fileName_p    :文件名(可以带路径/)

* 输出：
*       fullFileName_p: 全路径文件名
*       oErrMsg_p       如果此变量为NULL则错误信息直接printf
*                       如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
* 返回：
*     0    成功
*     <0   输入错误
*     >0   未找到路径名或文件名
*
*修改：
*    　20170731200432 创建函数
*注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int conDirName ( char* fullFileName_p,
                 const char* dirName_p,
                 const char* fileName_p,
                 const char* oErrMsg_p = NULL
               );


/*************************************
  *功能 ：重定向日志到文件
  * 输入：ym_p    :日志后缀（如年月)
  *       path_p  :日志路径
  *       logName_p:日志名(不带扩展.log的名)(可空)
  *
  * 输出：
  *      oErrMsg_p  如果此变量为NULL则错误信息直接printf
  *                 如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *返回：
  *     　0         :成功，
  *       >0        :失败
  *修改：
  *    　20170628173607 创建函数
  *注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int redirect ( const char* path_p,
               const char* logName_p,
               const char* ym_p,
               const char* oErrMsg_p = NULL
             );

/*************************************
  *功能 ：根据配置打印输出
  * 输入：
  *
  * 输出：无
  *返回：
  *     　0         :成功，
  *       >0        :失败
  *修改：
  *    　20170705135530 创建函数
*************************************/
int d_printf ( const char* format_p, ... );

/*************************************
  *功能 ：在输出的内容前面加时间
  *         [格式为:%Y-%m-%d %H:%M:%S.微秒]
  * 输入：
  *
  * 输出：无
  *返回：
  *     　0         :成功，
  *       <0        :失败
  *修改：
  *    　20170803155433 创建函数
*************************************/
int t_printf ( const char* format_p, ... );


/*************************************
  *功能 ：输出日志到日志文件
  * 输入：fullLogName 带路径的日志名
  *       szFormat    格式字符串
  * 输出：无
  *返回：
  *     　0         :成功，
  *       <0        :失败
  *修改：
  *    　20170630135308 创建函数
*************************************/
int writeLog ( const char* fullLogName_p, const char* szFormat_p, ... );

/*************************************
  *功能 ：复制文件
  * 输入：
  *     srcFileName_p:源文件
  *     dstFileName_p:目标文件
  *     flag         :标识符  0 调用C语言函数fread及fwrite,1 调用shell命令cp处理
  *
  *
  * 输出：
  *      oErrMsg_p  如果此变量为NULL则错误信息直接printf
  *                 如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *返回：
  *     　
  *
  *
  *修改：
  *    　20170629160441 创建函数
  *注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int copyFile ( const char* srcFileName_p,
               const char* dstFileName_p,
               const char* oErrMsg_p = NULL,
               const int& flag = 0
             );


/*************************************
* 功能：重命名文件
* 输入：
*     oldpath_p :源文件名
*     newpath_p :目标文件名
*     flag      :标识符  0 调用C语言函数rename,1 调用shell命令mv处理
*
*
* 输出：
*      oErrMsg_p  如果此变量为NULL则错误信息直接printf
  *               如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
*
* 返回：
*     0    成功
*     <0   错误
*
* 注意:
*
*修改：
*    　20170707134628 创建函数
*注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int mrename ( const char* oldpath_p,
              const char* newpath_p,
              const char* oErrMsg_p = NULL,
              const int& flag = 0
            );

/*************************************
* 功能：删除文件
* 输入：
*     fileName_p:要删除的文件名
*     flag      :标识符  0 调用C语言函数remove,1 调用shell命令rm处理
*
* 输出：
*    oErrMsg_p  如果此变量为NULL则错误信息直接printf
*               如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
*
* 返回：
*     0    成功
*     <0   错误
*
*
*修改：
*    　20170710175639 创建函数
*注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int mdelete ( const char* fileName_p,
              const char* oErrMsg_p = NULL,
              const int& flag = 0
            );


/*************************************
  *功能 ：执行shell程序
  * 输入：shell名、shell需要的参数
  * 输出：无
  *返回：
  *     　0         :成功，
  *       >0        :shell程序抛出的异常值
  *       <0        :失败
  *修改：
  *    　20170629160441 创建函数
*************************************/
int executeShell ( const char* shName_p, const char* szFormat_p, ... );



/*******************************************************************
  *功能 ：执行shell程序
  * 输入：
        *cmdStr_p,    需要执行的shell命令或shell脚本
        &timeout,     大于0表示超时时间(单位秒)等于0为不超时
        &times,       超时后需要重新执行的的次数
        &killAllFlag, 1:超时后kill脚本派生的所有进程;0只kill脚本进程
        &sizeRetMsg   retMsg_p的空间大小即:sizeof(retMsg_p)
  * 输出：
        *retMsg_p,    错误信息
  *返回：
  *     　0           成功，
  *       >0 且!=127  shell程序抛出的异常值
  *       -1          失败
  *       -2          shell执行超时
  *       -3          shell执行超时后kill进程错误
  *注意:
  *     要想利用脚本回返值来判断脚本的执行逻辑,需要在脚本中显示的用
        exit value (value的值是需要小于127的正数)
  *修改：
  *    　20180308085542 创建函数
********************************************************************/

int system_plus ( const char* cmdStr_p,
                  const int& timeout = 0,
                  const int& times = 0,
                  const int& killAllFlag = 0,
                  char*  retMsg_p = NULL,
                  const int& sizeRetMsg = 0
                );

/*************************************
* 功能：删除指定目录下同时满足如下条件的文件
*       (1)文件名匹配正则表达式parttern_p(POSIX正则表达式)
*       (2)文件的时间超过overTimeSecs秒(具体是什么时间看timeFlag而定)
* 输入：
*     dirName_p   :要删除的文件对应的上级目录
*     parttern_p  :正则表达式格式字符串(POSIX正则表达式)
*     overTimeSecs:超时时间值（单位秒）
*     timeFlag    :标识符  0 time of last access ;
*                          1 time of last modification; 
*                          2 time of last status change
*
* 输出：
*    oErrMsg_p  如果此变量为NULL则错误信息直接printf
*               如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
*
* 返回：
*     0    成功
*     <0   错误
*
*
*修改：
*    　20190306 创建函数
*注：
*      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int mDelByRegAndTm ( const char* dirName_p,
              const char* parttern_p,
              const long&  overTimeSecs = 0,
              const char* oErrMsg_p = NULL,
              const int& timeFlag = 1
            );



#endif // _BASEFUNC_H_
