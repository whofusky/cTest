#include "baseFunc.h"
#include "tinyxml2.h"
#include <dirent.h>

#if defined(TINYXML2_INCLUDED)
    using namespace tinyxml2;
#endif




int g_dug_flag = 0;   //是否打印日志标识
int g_load_onece = 0; //加载一次标识


int nanoSleep ( const time_t& tv_sec,
    const long&   tv_nsec
    )
{
    int tRet = 0;
    struct timespec req;
    req.tv_sec  = tv_sec;
    req.tv_nsec = tv_nsec;
    struct timespec rem;
    rem.tv_sec  = 0;
    rem.tv_nsec = 0;
    tRet = nanosleep ( &req, &rem );
    while ( ( tRet < 0 ) && ( errno == EINTR ) )
    {
        req.tv_sec  = rem.tv_sec;
        req.tv_nsec = rem.tv_nsec;
        rem.tv_sec  = 0;
        rem.tv_nsec = 0;
        tRet = nanosleep ( &req, &rem );
    }

    return tRet;
}

//sleep ? seconds (thead-safe)
int sleepSecond ( const size_t second )
{
    return ( nanoSleep ( second, 0 ) );
}

//sleep ? milliseconds (thead-safe)
int sleepMilliSecond ( const long millisecond )
{

    time_t tv_sec  = 0;
    long   tv_nsec = 0;

    if ( millisecond > 1000 )
    {
        tv_sec  = millisecond / 1000;
        tv_nsec = ( millisecond % 1000 ) * 1000000;
    }
    else
    {
        tv_nsec = millisecond * 1000000;
    }

    return ( nanoSleep ( tv_sec, tv_nsec ) );

}

//sleep ? microseconds (thead-safe)
int sleepMicroSecond ( const long microsecond )
{

    time_t tv_sec  = 0;
    long   tv_nsec = 0;

    if ( microsecond > 1000000 )
    {
        tv_sec  = microsecond / 1000000;
        tv_nsec = ( microsecond % 1000000 ) * 1000;
    }
    else
    {
        tv_nsec = microsecond * 1000;
    }

    return ( nanoSleep ( tv_sec, tv_nsec ) );

}




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
char* Trim ( char* S_p )
{
    int I = 0;
    int L = 0;

    if ( S_p == NULL )
    {
        return S_p;
    }

    L = strlen ( S_p ) - 1;
    I = 0;

    while ( ( I <= L ) && ( S_p[I] <= ' ' ) && ( S_p[I] > 0 ) )
    {
        I++;
    }

    if ( I > L )
    {
        S_p[0] = '\0';
    }
    else
    {
        while ( ( S_p[L] <= ' ' ) && ( S_p[L] > 0 ) )
        {
            L--;
        }

        strncpy ( S_p, S_p + I, L + 1 );
        S_p[L + 1 - I] = '\0';
    }

    return S_p;

}

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
                 const char* oErrMsg_p
               )
{
    char errMsg[MC_ERRMSG_MAX];
    memset ( errMsg, 0, sizeof ( errMsg ) );


    if ( bufStr_p == NULL || pattern_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input params bufStr_p or pattern_p is NULL" );
        outPutErrM ( oErrMsg_p, errMsg );
        return ERR_POINTER_NULL;
    }

    int vRet = 0;
    regex_t preg;
    int  cflags = REG_EXTENDED;
    const size_t nmatch = 1;
    //size_t imatch=0;
    regmatch_t pm[nmatch];

    //memset(pm,0,sizeof(regmatch_t)*nmatch);

    vRet = regcomp ( &preg, pattern_p, cflags );

    if ( vRet != 0 ) //编译正则表达式失败
    {
        regerror ( vRet, &preg, errMsg, sizeof ( errMsg ) );
        outPutErrM ( oErrMsg_p, errMsg );
        return -1000;
    }

    vRet = regexec ( &preg, bufStr_p, nmatch, pm, 0 );

    //    t_printf("nmatch=[%d],pm[0].rm_so=[%d],pm[0].rm_eo=[%d],strlen(bufStr_p)=[%d]\n",
    //        nmatch,pm[0].rm_so,pm[0].rm_eo,strlen(bufStr_p));
    if ( vRet == REG_NOMATCH ) //无匹配项
    {
        regfree ( &preg );
        return 1;
    }



    if ( pm[0].rm_so == 0
         && pm[0].rm_eo == ( int ) strlen ( bufStr_p )
       ) //整个字符串匹配
    {
        regfree ( &preg );
        return 0;
    }


    regfree ( &preg );

    return 2;//不是整个字符串匹配

}


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
              )
{
    int i = 0;
    int sepLen = 0;
    char* p = NULL;

    if ( separateStr_p == NULL )
    {
        return ERR_POINTER_NULL;
    }

    sepLen = strlen ( separateStr_p );

    while ( src_p )
    {
        ptrs_pp[i++] = src_p;
        p = strstr ( src_p, separateStr_p );

        if ( p != ( char* ) NULL )
        {
            *p = 0x0;
            src_p = p + sepLen;
        }
        else
        {
            break;
        }
    }

    ptrs_pp[i] = NULL;

    return i;
}



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
              const char* oErrMsg_p
            )
{

    struct timeval tv;
    time_t nowTimeSec;
    struct tm nowTime;
    char tmbuf[64], buf[64];

    memset ( &tv, 0, sizeof ( tv ) );
    memset ( &nowTimeSec, 0, sizeof ( nowTimeSec ) );
    memset ( &nowTime, 0, sizeof ( nowTime ) );
    memset ( tmbuf, 0, sizeof ( tmbuf ) );
    memset ( buf, 0, sizeof ( buf ) );

    gettimeofday ( &tv, NULL );

    char errMsg[MC_ERRMSG_MAX];
    memset ( errMsg, 0, sizeof ( errMsg ) );

    if ( 0 == tmtype ) //获得的系统时间格式为:%Y-%m-%d %H:%M:%S.微秒
    {
        if ( tmLocal_p == NULL )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "error[tmLocal_p is NULL]" );
            outPutErrM ( oErrMsg_p, errMsg );
            return ERR_POINTER_NULL;
        }

        nowTimeSec = tv.tv_sec;
        localtime_r ( &nowTimeSec, &nowTime );
        strftime ( tmbuf, sizeof ( tmbuf ), "%Y-%m-%d %H:%M:%S", &nowTime );
        snprintf ( buf, sizeof ( buf ), "%s.%06ld", tmbuf, tv.tv_usec );

        strcpy ( tmLocal_p, buf );
    }
    else if ( 1 == tmtype ) //获得系统时间到 1970/1/1 0:0:0 所经过的微秒数
    {
        if ( tmUsec_p == NULL )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "error[tmUsec_p is NULL]" );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_POINTER_NULL;
        }

        snprintf ( buf, sizeof ( buf ), "%ld", tv.tv_sec * 1000000 + tv.tv_usec );

        strcpy ( tmUsec_p, buf );
    }
    else if ( 2 == tmtype ) //获得系统时间到 1970/1/1 0:0:0 所经过的微秒数 及 获得的系统时间格式为:%Y-%m-%d %H:%M:%S.微秒
    {
        if ( tmLocal_p == NULL )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "error[tmLocal_p is NULL]" );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_POINTER_NULL;
        }

        if ( tmUsec_p == NULL )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "error[tmUsec_p is NULL]" );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_POINTER_NULL;
        }

        nowTimeSec = tv.tv_sec;
        localtime_r ( &nowTimeSec, &nowTime );
        strftime ( tmbuf, sizeof ( tmbuf ), "%Y-%m-%d %H:%M:%S", &nowTime );
        snprintf ( buf, sizeof ( buf ), "%s.%06ld", tmbuf, tv.tv_usec );

        strcpy ( tmLocal_p, buf );

        snprintf ( buf, sizeof ( buf ), "%ld", tv.tv_sec * 1000000 + tv.tv_usec );

        strcpy ( tmUsec_p, buf );
    }
    else if ( 3 == tmtype ) //获得系统时间:%Y%m%d
    {
        if ( tmLocal_p == NULL )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "error[tmLocal_p is NULL]" );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_POINTER_NULL;
        }

        nowTimeSec = tv.tv_sec;
        localtime_r ( &nowTimeSec, &nowTime );
        strftime ( tmbuf, sizeof ( tmbuf ), "%Y%m%d", &nowTime );
        snprintf ( buf, sizeof ( buf ), "%s", tmbuf );

        strcpy ( tmLocal_p, buf );
    }
    else if ( 4 == tmtype ) //获得系统时间:%Y-%m-%d:%H-%M-%S"
    {
        if ( tmLocal_p == NULL )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "error[tmLocal_p is NULL]" );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_POINTER_NULL;
        }

        nowTimeSec = tv.tv_sec;
        localtime_r ( &nowTimeSec, &nowTime );
        strftime ( tmbuf, sizeof ( tmbuf ), "%Y-%m-%d:%H-%M-%S", &nowTime );
        snprintf ( buf, sizeof ( buf ), "%s", tmbuf );

        strcpy ( tmLocal_p, buf );
    }
    else
    {
        return ERR_INPUT_VALUE;
    }

    return 0;


}

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
                 const char* oErrMsg_p
               )
{
    char errMsg[MC_ERRMSG_MAX];
    memset ( errMsg, 0, sizeof ( errMsg ) );

    if ( fileName_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "file name is NULL\n" );
        outPutErrM ( oErrMsg_p, errMsg );
        return ERR_POINTER_NULL;
    }

    int vRet = 0;

    vRet = access ( fileName_p, F_OK );

    if ( vRet != 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "access %s error[%s]\n",
                   fileName_p, strerror ( errno ) );
        outPutErrM ( oErrMsg_p, errMsg );

        return 1;
    }

    return 0;

}

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
                    const char* oErrMsg_p
                  )
{
    int vRet = 0;
    struct stat fstat ;
    char errMsg[MC_ERRMSG_MAX];
    memset ( errMsg, 0, sizeof ( errMsg ) );
    memset ( &fstat, 0, sizeof ( struct stat ) );

    if ( fileName_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "file name is NULL" );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_POINTER_NULL;
    }

    vRet = access ( fileName_p, F_OK );

    if ( vRet < 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "access %s error[%s]",
                   fileName_p, strerror ( errno ) );
        outPutErrM ( oErrMsg_p, errMsg );

        return -1;
    }

    vRet = stat ( fileName_p, &fstat ); //vRet 0 成功,－1失败

    if ( vRet < 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "stat %s error[%s]",
                   fileName_p, strerror ( errno ) );
        outPutErrM ( oErrMsg_p, errMsg );

        return -1;
    }

    return 0;

}

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
            const char* oErrMsg_p
          )
{
    struct stat bufStat;
    char errMsg[MC_ERRMSG_MAX];
    memset ( errMsg, 0, sizeof ( errMsg ) );
    memset ( &bufStat, 0, sizeof ( struct stat ) );

    if ( stat ( dirName_p, &bufStat ) != 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "stat %s error[%s]",
                   dirName_p, strerror ( errno ) );
        outPutErrM ( oErrMsg_p, errMsg );

        return -1;
    }

    if ( ( bufStat.st_mode & S_IFDIR ) != 0 )
    {
        return 0;
    }
    else
    {
        return 1;
    }
}


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
                 const char* oErrMsg_p,
                 const int& sptFlag
               )
{
    int errFlag = 0;
    char errMsg[MC_ERRMSG_MAX];
    memset ( errMsg, 0, sizeof ( errMsg ) );

    if ( sptFlag == 0 )
    {
        if ( fullFileName_p == NULL
             || dirName_p == NULL
             || fileName_p == NULL
           )
        {
            errFlag = 1;
        }
    }
    else if ( sptFlag == 1 )
    {
        if ( fullFileName_p == NULL
             || dirName_p == NULL
           )
        {
            errFlag = 1;
        }
    }
    else if ( sptFlag == 2 )
    {
        if ( fullFileName_p == NULL
             || fileName_p == NULL
           )
        {
            errFlag = 1;
        }
    }
    else
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "error[input params sptFlag value]" );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_INPUT_VALUE;
    }

    if ( errFlag == 1 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "error[input params is NULL ]" );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_POINTER_NULL;
    }

    int len = strlen ( fullFileName_p );

    if ( len <= 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "error[input params fullFileName_p len=[%d] ]",
                   len );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_INPUT_VALUE;
    }

    int pos = 0;

    for ( int i = len - 1; i > 0; i-- )
    {
        if ( fullFileName_p[i] == '/' )
        {
            pos = i + 1;
            break;
        }
    }

    if ( pos == 0 )
    {
        for ( int i = len - 1; i > 0; i-- )
        {
            if ( fullFileName_p[i] == '\\' )
            {
                pos = i + 1;
                break;
            }
        }
    }

    if ( sptFlag != 2 && pos == 0 )
    {
        return 1;
    }

    if ( sptFlag == 0 )
    {
        strncpy ( dirName_p, fullFileName_p, pos );
        * ( dirName_p + pos ) = 0;
        strncpy ( fileName_p, fullFileName_p + pos, len - pos );
        * ( fileName_p + ( len - pos ) ) = 0;
    }
    else if ( sptFlag == 1 )
    {
        strncpy ( dirName_p, fullFileName_p, pos );
        * ( dirName_p + pos ) = 0;
    }
    else if ( sptFlag == 2 )
    {
        strncpy ( fileName_p, fullFileName_p + pos, len - pos );
        * ( fileName_p + ( len - pos ) ) = 0;
    }

    //d_printf("%s|%s|%d:  fullFileName_p=[%s]\n",__FILE__,__func__,__LINE__, fullFileName_p);
    //d_printf("%s|%s|%d:  len=[%d],pos=[%d]\n",__FILE__,__func__,__LINE__, len,pos);
    //d_printf("%s|%s|%d:  dirName_p=[%s]\n",__FILE__,__func__,__LINE__, dirName_p);
    //d_printf("%s|%s|%d:  fileName_p=[%s]\n",__FILE__,__func__,__LINE__, fileName_p);

    return 0;
}

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
                 const char* oErrMsg_p
               )
{
    char errMsg[MC_ERRMSG_MAX];
    memset ( errMsg, 0, sizeof ( errMsg ) );

    if ( fullFileName_p == NULL
         || dirName_p == NULL
         || fileName_p == NULL
       )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "error[input params is NULL ]" );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_POINTER_NULL;
    }

    int len1 = strlen ( dirName_p );
    int len2 = strlen ( fileName_p );

    if ( len1 == 0 || len2 == 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "error[input params dirName_p len=[%d],fileName_p len=[%d]",
                   len1, len2 );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_INPUT_VALUE;
    }

    int posD = 0;

    for ( int i = len1 - 1; i > 0; i-- )
    {
        if ( dirName_p[i] == '/' )
        {
            posD = i + 1;
            break;
        }
    }

    if ( posD == 0 )
    {
        return 1;
    }

    char tmpStr[MC_FULLN_LEN] = {};
    strncpy ( tmpStr, dirName_p, posD );

    int posF = -1;

    for ( int j = len2 - 1; j >= 0; j-- )
    {
        if ( fileName_p[j] == '/' )
        {
            posF = j;
            break;
        }
    }

    if ( posF == len2 - 1 ) //有/符号但没有文件名
    {
        return 2;
    }
    else if ( posF == -1 ) //没有/符号
    {
        strcat ( tmpStr, fileName_p );
    }
    else if ( posF >= 0 ) //有/符号并有文件名
    {
        strcat ( tmpStr, fileName_p + posF + 1 );
    }

    strcpy ( fullFileName_p, tmpStr );

    return 0;
}



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
                 const char* oErrMsg_p,
                 const int& timeFlag
               )
{
    char errMsg[MC_ERRMSG_MAX];
    memset ( errMsg, 0, sizeof ( errMsg ) );

    int vRet = 0;
    struct tm gTime;
    char tmbuf[64];
    memset ( &gTime, 0, sizeof ( struct tm ) );
    memset ( tmbuf, 0, sizeof ( tmbuf ) );

    vRet = fileExists ( fileName_p, oErrMsg_p );

    if ( vRet != 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "[%s] fileExists error vRet=[%d]\n%s",
                   fileName_p, vRet, oErrMsg_p );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_DIY_FUNCRET;
    }

    struct stat fstat = {};

    if ( stat ( fileName_p, &fstat ) < 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "stat [%s] error[%s]",
                   fileName_p, strerror ( errno ) );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_SYS_FUNCRET;
    }

    time_t tmNow = 0;
    tmNow = time ( NULL );

    if ( flag == 0 )
    {
        subSecs = tmNow - fstat.st_atime;
        localtime_r ( & ( fstat.st_atime ), &gTime );
    }
    else if ( flag == 1 )
    {
        subSecs = tmNow - fstat.st_mtime;
        localtime_r ( & ( fstat.st_mtime ), &gTime );
    }
    else if ( flag == 2 )
    {
        subSecs = tmNow - fstat.st_ctime;
        localtime_r ( & ( fstat.st_ctime ), &gTime );
    }
    else
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input flag values error  flag=[%d]",
                   flag );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_INPUT_VALUE;
    }

    if ( timeFlag == 0 )
    {
        strftime ( tmbuf, sizeof ( tmbuf ),
                   "%Y-%m-%d:%H-%M-%S", &gTime );
    }

    if ( statTime_p != NULL )
    {
        strcpy ( statTime_p, tmbuf );
    }


    return 0;
}


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
               const char* oErrMsg_p
             )
{
    int vRet = 0;
    int fd;
    mode_t fd_mod = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    char fileName[MC_FULLN_LEN];
    char errMsg[MC_ERRMSG_MAX] ;

    memset ( fileName, 0, sizeof ( fileName ) );
    memset ( errMsg, 0, sizeof ( errMsg ) );

    if ( ym_p == NULL )
    {
        char buf_p[20] = {};
        vRet = getTime ( 3, buf_p, NULL, NULL ); //取时间YYYYMMDD

        if ( vRet != 0 )
        {
            //            d_printf("%s|%s|%d:getTime 3 return[%d]\n",
            //                __FILE__,__func__,__LINE__, vRet);
            return ERR_DIY_FUNCRET;
        }

        if ( logName_p == NULL )
        {
            sprintf ( fileName, "%s%s.log",
                      path_p, buf_p );
        }
        else
        {
            sprintf ( fileName, "%s/%s%s.log",
                      path_p, logName_p, buf_p );
        }
    }
    else
    {
        if ( logName_p == NULL )
        {
            sprintf ( fileName, "%s%s.log",
                      path_p, ym_p );
        }
        else
        {
            sprintf ( fileName, "%s/%s%s.log",
                      path_p, logName_p, ym_p );
        }
    }

    //    printf("日志文件为%s_%s.log",logName_p,ym_p);
    if ( ( fd = open ( fileName, O_WRONLY ) ) < 0 )
    {
        fd = open ( fileName, O_WRONLY | O_CREAT, fd_mod );

        if ( fd < 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "创建日志文件失败 error[%s]",
                       strerror ( errno ) );
            outPutErrM ( oErrMsg_p, errMsg );

            return 1;
        }
    }
    else
    {
        fd = open ( fileName, O_WRONLY | O_APPEND, fd_mod );

        if ( fd < 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "创建日志文件失败 error[%s]",
                       strerror ( errno ) );
            outPutErrM ( oErrMsg_p, errMsg );

            return 2;
        }
    }

    if ( dup2 ( fd, STDOUT_FILENO ) < 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "日志文件重定向失败" );
        outPutErrM ( oErrMsg_p, errMsg );

        return 3;
    }

    return 0;
}

/*************************************
  *功能 ：根据mete_debug.xml的配置打印输出
  * 输入：
  *
  * 输出：无
  *返回：
  *     　0         :成功，
  *       <0        :失败
  *修改：
  *    　20170705144425 创建函数
*************************************/
int d_printf ( const char* format_p, ... )
{
    int result = 0;
    //int flag = 0;

    if ( g_load_onece == 0 )
    {
        XMLDocument doc;
        char* cfgDebug_p = NULL;
        cfgDebug_p = getenv ( MC_PDEBUG_XML ); ////取得配置mete_debug.xml路径 的环境变量值

        if ( cfgDebug_p == NULL )
        {
            printf ( "%s|%s|%d:  getenv [%s] return error\n",
                     __FILE__, __func__, __LINE__, MC_PDEBUG_XML );
            return ERR_POINTER_NULL;
        }

        result = doc.LoadFile ( cfgDebug_p );

        if ( result != 0 )
        {
            printf ( "%s|%s|%d:load xml file [%s] failed [%s]\n",
                     __FILE__, __func__, __LINE__, cfgDebug_p, doc.ErrorName() );

        }
        else
        {
            result = doc.FirstChildElement ( MC_L_TOPRINT )->QueryIntText ( &g_dug_flag );

            if ( result != 0 )
            {
                printf ( "%s|%s|%d:QueryIntText [%s] failed  result=[%d][%s]\n",
                         __FILE__, __func__, __LINE__, MC_L_TOPRINT, result,
                         doc.ErrorIDToName ( ( XMLError ) result ) );
            }
        }

        doc.Clear();
        g_load_onece = 1;
    }

    if ( g_dug_flag == 1 )
    {
        va_list vp;
        va_start ( vp, format_p );

        char tmpTime[30] = {};
        getTime ( 0, tmpTime, NULL, NULL );
        printf ( "[%s]|", tmpTime );

        result = vprintf ( format_p, vp );

        va_end ( vp );
    }

    //    fflush(stdout);

    return result;

}

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
int t_printf ( const char* format_p, ... )
{
    int result = 0;

    va_list vp;
    va_start ( vp, format_p );

    char tmpTime[30] = {};
    getTime ( 0, tmpTime, NULL, NULL );
    result = printf ( "[%s]|", tmpTime );

    if ( result < 0 )
    {
        return result;
    }

    result = vprintf ( format_p, vp );
    va_end ( vp );

    //    fflush(stdout);

    if ( result < 0 )
    {
        return result;
    }

    return 0;

}




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
int writeLog ( const char* fullLogName_p, const char* szFormat_p, ... )
{
    int vRet   = 0;
    FILE* f_p  = NULL;
    int actLen = 0;

    if ( fullLogName_p == NULL )
    {
        return ERR_POINTER_NULL;
    }

    if ( szFormat_p == NULL )
    {
        return ERR_POINTER_NULL;
    }

    char buf_p[20] = {};
    vRet = getTime ( 3, buf_p, NULL, NULL ); //取时间YYYYMMDD

    if ( vRet != 0 )
    {
        //        d_printf("%s|%s|%d:getTime 3 return [%d]\n"
        //            ,__FILE__,__func__,__LINE__, vRet);
        return ERR_DIY_FUNCRET;
    }

    strcat ( buf_p, ".log" );

    char tmpFullName[MC_FULLN_LEN] = {};
    snprintf ( tmpFullName, sizeof ( tmpFullName ),
               "%s%s", fullLogName_p, buf_p );

    f_p = fopen ( tmpFullName, "a+" );

    if ( f_p == NULL )
    {
        return ERR_RETURN_NULL;
    }

    char tmpTime[30] = {};
    vRet = getTime ( 0, tmpTime, NULL, NULL );

    if ( vRet != 0 )
    {
        //        d_printf("%s|%s|%d:getTime 0 return [%d]\n",
        //            __FILE__,__func__,__LINE__, vRet);
        return ERR_DIY_FUNCRET;
    }

    fprintf ( f_p, "[%s]|", tmpTime );
    va_list marker;
    va_start ( marker, szFormat_p );
    actLen = vfprintf ( f_p, szFormat_p, marker );

    //printf("\n actLen = [%d] \n",actLen);
    if ( actLen < 0 )
    {
        return ERR_SYS_FUNCRET;
    }

    va_end ( marker );

    //    fflush(f_p);

    if ( f_p )
    {
        fclose ( f_p );
    }

    return 0;
}
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
               const char* oErrMsg_p,
               const int& flag
             )
{
    int vRet = 0;
    FILE* fpR_p = NULL;
    FILE* fpW_p = NULL;
    char tmpbuf[MC_CBUFFER_SIZE] ;
    int lenR = 0;
    int lenW = 0;
    char errMsg[MC_ERRMSG_MAX];
    memset ( errMsg, 0, sizeof ( errMsg ) );
    memset ( tmpbuf, 0, sizeof ( tmpbuf ) );

    vRet = fileExists ( srcFileName_p, oErrMsg_p );

    if ( vRet != 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "fileExists vRet[%d]\n%s",
                   vRet, oErrMsg_p );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_INPUT_VALUE;
    }

    if ( dstFileName_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "dstFileName_p is NULL error" );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_POINTER_NULL;
    }

    if ( flag == 0 )
    {
        if ( ( fpR_p = fopen ( srcFileName_p, "r" ) ) == NULL )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "fopen [%s] error[%s]",
                       srcFileName_p, strerror ( errno ) );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_SYS_FUNCRET;
        }

        char tFileNmae[MC_NAME_LEN] = {};
        char dstFileNmae[MC_FULLN_LEN] = {};

        if ( isDir ( dstFileName_p, oErrMsg_p ) == 0 ) //目标文件是一个目录则 在目录后加上文件名
        {
            char tDirName[MC_PATH_LEN] = {};
            vRet = sptDirName ( srcFileName_p,
                                tDirName,
                                tFileNmae,
                                oErrMsg_p,
                                2
                              );

            if ( vRet != 0 )
            {
                snprintf ( errMsg,
                           sizeof ( errMsg ),
                           "sptDirName [%s] return error vRet=[%d]\n%s",
                           srcFileName_p, vRet, oErrMsg_p );
                outPutErrM ( oErrMsg_p, errMsg );

                return ERR_DIY_FUNCRET;
            }

            snprintf ( dstFileNmae,
                       sizeof ( dstFileNmae ),
                       "%s/%s",
                       dstFileName_p, tFileNmae );

        }
        else
        {
            snprintf ( dstFileNmae,
                       sizeof ( dstFileNmae ),
                       dstFileName_p );
        }

        if ( ( fpW_p = fopen ( dstFileNmae, "w" ) ) == NULL )
        {
            fclose ( fpR_p );
            snprintf ( errMsg, sizeof ( errMsg ),
                       "fopen [%s] error[%s]",
                       dstFileNmae, strerror ( errno ) );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_SYS_FUNCRET;
        }

        memset ( tmpbuf, 0, MC_CBUFFER_SIZE );

        while ( ( lenR = fread ( tmpbuf, 1, MC_CBUFFER_SIZE, fpR_p ) ) > 0 )
        {
            if ( ( lenW = fwrite ( tmpbuf, 1, lenR, fpW_p ) ) != lenR )
            {
                fclose ( fpR_p );
                fclose ( fpW_p );
                snprintf ( errMsg, sizeof ( errMsg ),
                           "fwrite error lenW=[%d],lenR=[%d]",
                           lenW, lenR );
                outPutErrM ( oErrMsg_p, errMsg );

                return ERR_SYS_FUNCRET;
            }

            memset ( tmpbuf, 0, MC_CBUFFER_SIZE );
        }

        fclose ( fpR_p );
        fclose ( fpW_p );
        //printf("\n 000000\n");
    }
    else // flag!=0
    {
        vRet = executeShell ( "cp", "%s %s", srcFileName_p, dstFileName_p );

        if ( vRet != 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "executeShell error vRet=[%d]",
                       vRet );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_DIY_FUNCRET;
        }

        //printf("\n 1111\n");
    }

    return 0;


}


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
              const char* oErrMsg_p,
              const int& flag
            )
{
    char errMsg[MC_ERRMSG_MAX];
    memset ( errMsg, 0, sizeof ( errMsg ) );

    if ( oldpath_p == NULL
         || newpath_p == NULL
       )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input params oldpath_p or newpath_p is NULL error" );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_POINTER_NULL;
    }

    int vRet = 0;

    if ( flag == 0 )
    {
        if ( ( vRet = fileExists ( oldpath_p, oErrMsg_p ) ) != 0 ) //oldpath_p不存在
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "fileExists vRet[%d]\n%s",
                       vRet, oErrMsg_p );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_INPUT_VALUE;
        }
    }

    int dirFlag = 0;
    vRet = isDir ( newpath_p, oErrMsg_p );

    if ( vRet == 1 ) //不是目录
    {
        if ( ( vRet = fileExists ( newpath_p,
                                   oErrMsg_p ) ) == 0 ) //newpath_p如果存在则 报错
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "the file [%s] already exists",
                       newpath_p );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_INPUT_VALUE;
        }

        char dirName[MC_PATH_LEN] = {};
        char fileName[MC_NAME_LEN] = {};
        vRet = sptDirName ( newpath_p,
                            dirName,
                            fileName,
                            oErrMsg_p
                          );

        if ( vRet != 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "[%s] sptDirName error vRet=[%d]\n%s",
                       newpath_p, vRet, oErrMsg_p );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_DIY_FUNCRET;
        }

        vRet = isDir ( dirName, oErrMsg_p ); //判断目录是否存在

        if ( vRet != 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "[%s] isDir error vRet=[%d]\n%s",
                       dirName, vRet, oErrMsg_p );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_DIY_FUNCRET;
        }
    }
    else if ( vRet == 0 ) //是目录
    {
        dirFlag = 1;
    }

    if ( flag == 0 )
    {
        char tFileNmae[MC_NAME_LEN] = {};
        char dstFileNmae[MC_FULLN_LEN] = {};

        if ( dirFlag == 1 )
        {
            char tDirName[MC_PATH_LEN] = {};
            vRet = sptDirName ( oldpath_p,
                                tDirName,
                                tFileNmae,
                                oErrMsg_p,
                                2
                              );

            if ( vRet != 0 )
            {
                snprintf ( errMsg, sizeof ( errMsg ),
                           "sptDirName [%s] return error vRet=[%d]\n%s",
                           oldpath_p, vRet, oErrMsg_p );
                outPutErrM ( oErrMsg_p, errMsg );

                return ERR_DIY_FUNCRET;
            }

            snprintf ( dstFileNmae,
                       sizeof ( dstFileNmae ),
                       "%s/%s",
                       newpath_p, tFileNmae );

        }
        else
        {
            snprintf ( dstFileNmae, sizeof ( dstFileNmae ), newpath_p );
        }

        vRet = rename ( oldpath_p, dstFileNmae );

        if ( vRet != 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "rename oldpath_p=[%s],newpath_p=[%s] return vRet[%d]",
                       oldpath_p, dstFileNmae, vRet );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_SYS_FUNCRET;
        }
    }
    else if ( flag == 1 )
    {
        vRet = executeShell ( "mv ", "%s %s", oldpath_p, newpath_p );

        if ( vRet != 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "executeShell mv oldpath_p=[%s],newpath_p=[%s] return vRet[%d]",
                       oldpath_p, newpath_p, vRet );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_SYS_FUNCRET;
        }
    }
    else
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input flag error, flag=[%d]",
                   flag );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_INPUT_VALUE;
    }

    return 0;

}

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
              const char* oErrMsg_p,
              const int& flag
            )
{
    char errMsg[MC_ERRMSG_MAX];
    memset ( errMsg, 0, sizeof ( errMsg ) );

    if ( fileName_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input params fileName_p is NULL error" );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_POINTER_NULL;
    }

    int vRet = 0;

    if ( ( vRet = fileExists ( fileName_p, oErrMsg_p ) ) != 0 ) //fileName_p不存在
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "fileExists vRet[%d]\n%s",
                   vRet, oErrMsg_p );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_INPUT_VALUE;
    }


    if ( flag == 0 )
    {
        vRet = remove ( fileName_p );

        if ( vRet != 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "fileName_p=[%s] remove error[%s]",
                       fileName_p, strerror ( errno ) );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_SYS_FUNCRET;
        }
    }
    else if ( flag == 1 )
    {
        vRet = executeShell ( "rm -rf ", "%s", fileName_p );

        if ( vRet != 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "executeShell rm -rf %s vRet=[%d]",
                       fileName_p, vRet );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_SYS_FUNCRET;
        }
    }
    else
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input flag value error flag=[%d]",
                   flag );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_INPUT_VALUE;
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
  *修改：
  *    　20170629160441 创建函数
*************************************/
int executeShell ( const char* shName_p, const char* szFormat_p, ... )
{
    if ( shName_p == NULL )
    {
        return ERR_POINTER_NULL;
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

        actLen = vsprintf ( szParam, szFormat_p,
                            pvList ); //把所有参数都存入szParam字符串

        if ( ( actLen < 0 ) || ( ( u_int ) actLen >= sizeof ( szParam ) ) )
        {
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
        return -3;
    }
    else
    {
        if ( ! ( WIFEXITED ( status ) ) )
        {
            //system 执行shell异常结束(如被强制kill掉)
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
                  const int& timeout,
                  const int& times,
                  const int& killAllFlag,
                  char*  retMsg_p,
                  const int& sizeRetMsg
                )
{
    if ( NULL == cmdStr_p )
    {
        if ( retMsg_p != NULL
             && sizeRetMsg > 0
           )
        {
            snprintf ( retMsg_p, sizeRetMsg,
                       "Input parameter cmdStr_p"
                       "pointer NULL error" );
        }

        return -1;
    }

    int tTimes = 0;

    if ( times > 0 )
    {
        tTimes = times;
    }

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
        if ( retMsg_p != NULL
             && sizeRetMsg > 0
           )
        {
            snprintf ( retMsg_p, sizeRetMsg,
                       "sigaction \"SIGINT ignore "
                       "saveintr\" error,errno[%d],"
                       "errmsg[%s]",
                       errno, strerror ( errno ) );
        }

        return -1;
    }

    if ( sigaction ( SIGQUIT, &ignore, &savequit ) < 0 )
    {
        if ( retMsg_p != NULL
             && sizeRetMsg > 0
           )
        {
            snprintf ( retMsg_p, sizeRetMsg,
                       "sigaction \"SIGQUIT ignore "
                       "saveintr\" error,errno[%d],"
                       "errmsg[%s]",
                       errno, strerror ( errno ) );
        }

        return -1;
    }

    sigemptyset ( &chldmask );
    sigaddset ( &chldmask, SIGCHLD );

    if ( sigprocmask ( SIG_BLOCK, &chldmask, &savemask ) < 0 )
    {
        if ( retMsg_p != NULL
             && sizeRetMsg > 0
           )
        {
            snprintf ( retMsg_p, sizeRetMsg,
                       "sigprocmask \"SIG_BLOCK chldmask "
                       "savemask\" error,errno[%d],"
                       "errmsg[%s]",
                       errno, strerror ( errno ) );
        }

        return -1;
    }

    for ( int i = -1; i < tTimes && true == needRetry; i++ )
    {
        needRetry = false;

        if ( ( pid = fork() ) < 0 )
        {
            if ( retMsg_p != NULL
                 && sizeRetMsg > 0
               )
            {
                snprintf ( retMsg_p, sizeRetMsg,
                           "fork child process error"
                           ",errno[%d],errmsg[%s]",
                           errno, strerror ( errno ) );
            }

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
            int options = 0;

            if ( timeout > 0 )
            {
                options = WNOHANG;
            }

            while ( ( rv = waitpid ( pid, &status, options ) ) <= 0 )
            {
                /*
                printf("%s|%s|%d:  errno[%d],rv[%d]"
                       ",options[%d]\n",
                       __FILE__,__func__,__LINE__,
                       errno, rv, options );
                */

                if ( -1 == rv && EINTR != errno )
                {
                    if ( retMsg_p != NULL
                         && sizeRetMsg > 0
                       )
                    {
                        snprintf ( retMsg_p, sizeRetMsg,
                                   "waitpid [%d] returns error"
                                   ",errno[%d],errmsg[%s]",
                                   pid, errno, strerror ( errno ) );
                    }

                    status = -1;
                    break;
                }

                if ( timeout > 0 )
                {
                    sleep ( 1 );

                    leftTime--;

                    if ( leftTime <= 0 )
                    {
                        if ( killAllFlag == 1 )
                        {
                            char tPidFile[30];
                            char tStr[256];
                            memset ( tPidFile, 0, sizeof ( tPidFile ) );
                            memset ( tStr, 0, sizeof ( tStr ) );
                            strcpy( tPidFile, ".plusTmp_XXXXXX" );

                            //snprintf ( tPidFile, sizeof ( tPidFile ),
                            //           ".%d", pid );
                            int fd = mkstemp( tPidFile );           
                            if ( fd < 0 )
                            {
                                if ( retMsg_p != NULL
                                     && sizeRetMsg > 0
                                   )
                                {
                                    snprintf ( retMsg_p, sizeRetMsg,
                                               "mkstemp(%s) error,errno[%d],"
                                               "errMsg=[%s]\n",tPidFile, errno,
                                               strerror ( errno ) );
                                }

                                status = -1;
                                break;

                            }

                            snprintf ( tStr, sizeof ( tStr ),
                                       "pstree -np %d|awk -F'(' '{i = 2; while "
                                       "(i <= NF) {print $i;i++}}'|awk -F')' '"
                                       "{print $1}'|sort -r >%s",
                                       pid, tPidFile
                                     );

                            system ( tStr );
                            FILE* fp = NULL;
                            fp = fdopen ( fd, "rb" );

                            if ( fp == NULL )
                            {
                                if ( retMsg_p != NULL
                                     && sizeRetMsg > 0
                                   )
                                {
                                    snprintf ( retMsg_p, sizeRetMsg,
                                               "fdopen(%d,rb) error,errno[%d],"
                                               "errMsg=[%s]\n", fd, errno,
                                               strerror ( errno ) );
                                }

                                status = -1;
                                break;
                            }

                            int tPid = 0;
                            int tNum = 0;
                            int tFNum = 0;
                            char tStrErr[100];
                            memset ( tStrErr, 0, sizeof ( tStrErr ) );

                            while ( fscanf ( fp, "%d", &tPid ) > 0 )
                            {
                                tNum++;

                                if ( kill ( tPid, 9 ) == 0 )
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
                            //snprintf ( tStr, sizeof ( tStr ),
                            //           "rm -rf %s", tPidFile );
                            //system ( tStr );

                            if ( tNum > 0 && tNum == tFNum )
                            {
                                status = -3;

                                if ( retMsg_p != NULL
                                     && sizeRetMsg > 0
                                   )
                                {
                                    snprintf ( retMsg_p, sizeRetMsg,
                                               "After the shell script"
                                               "execution time-out,the "
                                               "kill process is abnormal "
                                               ",%s", tStrErr );
                                }
                            }
                            else
                            {
                                status = -2;
                            }

                            //                            snprintf( tStr, sizeof(tStr),
                            //                                "pstree -np %d|awk -F'(' '{i = 2; while "
                            //                                "(i <= NF) {print $i;i++}}'|awk -F')' '"
                            //                                "{print $1}'|sort -r|xargs kill -9 "
                            //                                ">/dev/null 2>&1",
                            //                                pid
                            //                                );
                            //                            int tRet = system( tStr );
                            //                            if( tRet == -1 )
                            //                            {
                            //                                /*system 在创建子进程等准备工作 失败*/
                            //                                if ( retMsg_p != NULL
                            //                                    && sizeRetMsg > 0
                            //                                   )
                            //                                {
                            //                                    snprintf( retMsg_p,sizeRetMsg,
                            //                                        "The system function fails"
                            //                                        "to prepare for creating a"
                            //                                        "child process,tRet[%d]",
                            //                                        tRet );

                            //                                }
                            //                                status = -3;

                            //                            }
                            //                            else
                            //                            {
                            //                                if ( !(WIFEXITED(tRet)) )
                            //                                {
                            //                                    /*system 执行shell异常结束
                            //                                    (如被强制kill掉)
                            //                                    */
                            //                                    if ( retMsg_p != NULL
                            //                                        && sizeRetMsg > 0
                            //                                       )
                            //                                    {
                            //                                        snprintf( retMsg_p,sizeRetMsg,
                            //                                            "run shell procedures,The"
                            //                                            " system function abnormal"
                            //                                            "exit  tRet[%d]",
                            //                                            WIFEXITED(tRet) );

                            //                                    }
                            //                                    status = -3;
                            //                                }
                            //                                else
                            //                                {
                            //                                    /*WEXITSTATUS(tRet) 一般为0 表示
                            //                                    shell执行正常；其他情况则为shell
                            //                                    中抛出的异常值
                            //                                    */
                            //                                    //return WEXITSTATUS(tRet);
                            //                                    status = -2;
                            //                                }

                            //                            }
                        }
                        else
                        {
                            if ( kill ( pid, 9 ) == 0 )
                            {
                                waitpid ( pid, NULL, 0 );
                                status = -2;
                            }
                            else
                            {
                                if ( retMsg_p != NULL
                                     && sizeRetMsg > 0
                                   )
                                {
                                    snprintf ( retMsg_p, sizeRetMsg,
                                               "After the shell script"
                                               "execution time-out,the "
                                               "kill process is abnormal "
                                               ",errno[%d],errMsg[%s]",
                                               errno, strerror ( errno ) );

                                }

                                status = -3;
                            }
                        }

                        needRetry = true;
                        break;

                    }
                }

            }

            /*printf("%s|%s|%d:  errno[%d],rv[%d],"
                "timeout[%d],leftTime[%d]"
                ",status=[%d]\n",
                __FILE__,__func__,__LINE__,
                errno, rv, timeout, leftTime,
                status );
             */

        }
    }

    /*
    printf("%s|%s|%d:  errno[%d],"
                "timeout[%d]"
                ",status=[%d]\n",
                __FILE__,__func__,__LINE__,
                errno, timeout,
                status );
    */


    if ( sigaction ( SIGINT, &saveintr, NULL ) < 0 )
    {
        if ( retMsg_p != NULL
             && sizeRetMsg > 0
           )
        {
            snprintf ( retMsg_p, sizeRetMsg,
                       "sigaction \"SIGINT ignore "
                       "NULL\" error,errno[%d],"
                       "errmsg[%s]",
                       errno, strerror ( errno ) );
        }

        return -1;
    }

    if ( sigaction ( SIGQUIT, &savequit, NULL ) < 0 )
    {
        if ( retMsg_p != NULL
             && sizeRetMsg > 0
           )
        {
            snprintf ( retMsg_p, sizeRetMsg,
                       "sigaction \"SIGQUIT ignore "
                       "NULL\" error,errno[%d],"
                       "errmsg[%s]",
                       errno, strerror ( errno ) );
        }

        return -1;
    }

    if ( sigprocmask ( SIG_SETMASK, &savemask, NULL ) < 0 )
    {
        if ( retMsg_p != NULL
             && sizeRetMsg > 0
           )
        {
            snprintf ( retMsg_p, sizeRetMsg,
                       "sigprocmask \"SIG_SETMASK "
                       "savemask NULL\" error,errno[%d],"
                       "errmsg[%s]",
                       errno, strerror ( errno ) );
        }

        return -1;
    }

    int tRetValue = ( WIFEXITED ( status ) ) ? ( WEXITSTATUS ( status ) ) : status;

    if ( tRetValue == 127 )
    {
        if ( retMsg_p != NULL
             && sizeRetMsg > 0
           )
        {
            snprintf ( retMsg_p, sizeRetMsg,
                       "child process execlp returns"
                       " error" );
        }
    }

    return tRetValue;

}




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
              const long&  overTimeSecs,
              const char* oErrMsg_p,
              const int& timeFlag
            )
{
    char errMsg[MC_ERRMSG_MAX];
    memset ( errMsg, 0, sizeof ( errMsg ) );

    if ( dirName_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input params dirName_p is NULL error" );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_POINTER_NULL;
    }

    if ( parttern_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input params parttern_p is NULL error" );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_POINTER_NULL;
    }

    if ( timeFlag != 0 && timeFlag != 1 && timeFlag != 2 )
    {

        snprintf ( errMsg, sizeof ( errMsg ),
                   "The value of The put timeFlag is not equal to 0 or 1 or 2" );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_INPUT_VALUE;
    }

    int vRet = 0;
    vRet = isDir ( dirName_p, oErrMsg_p );
    if ( vRet != 0 ) //not a directory
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "Enter dirName_p=[%s] is not a directory",
                 dirName_p );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_INPUT_VALUE;
    }


    DIR* dir_p = NULL;
    struct dirent* dirent_p = NULL;

    dir_p = opendir ( dirName_p );

    if ( dir_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                  "cannot opendir directory [%s]",
                  dirName_p );
        outPutErrM ( oErrMsg_p, errMsg );
        return ERR_SYS_FUNCRET;
 
    }

    while ( ( dirent_p = readdir ( dir_p ) ) != NULL )
    {
        if ( strcmp ( dirent_p->d_name, "." ) == 0
                || strcmp ( dirent_p->d_name, ".." ) == 0
           )
        {
            continue;
        }

        vRet = regexMatch ( dirent_p->d_name,
                            parttern_p,
                            oErrMsg_p
                          );

        if ( vRet < 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                      "regexMatch return error vRet=[%d] d_name=[%s] parttern_p=[%s],%s",
                      vRet, dirent_p->d_name, parttern_p, oErrMsg_p );
            outPutErrM ( oErrMsg_p, errMsg );
            closedir ( dir_p );
            return ERR_DIY_FUNCRET;
        }
        else if ( vRet == 0 ) //有匹配到的文件名
        {
            char tmpFullFile[MC_FULLN_LEN] ;
            memset ( tmpFullFile, 0, sizeof ( tmpFullFile ) );
            long subSecs = 0;

            snprintf ( tmpFullFile,
                       sizeof ( tmpFullFile ),
                       "%s/%s",
                       dirName_p, dirent_p->d_name );
            vRet = getSubFSec ( tmpFullFile,
                                timeFlag,
                                subSecs,
                                NULL,
                                oErrMsg_p,
                                1  
                              );

            if ( vRet != 0 )
            {
                snprintf ( errMsg, sizeof ( errMsg ),
                           "getSubFSec [%s] error vRet=[%d]\n%s",
                           tmpFullFile, vRet, oErrMsg_p );
                outPutErrM ( oErrMsg_p, errMsg );
                closedir ( dir_p );

                return ERR_DIY_FUNCRET;
            }

            if ( subSecs >= overTimeSecs ) 
            {
                //delete the file more than the specified time
                vRet = mdelete ( tmpFullFile, oErrMsg_p, 0 ); 
                if ( vRet != 0 )
                {
                    snprintf ( errMsg, sizeof ( errMsg ),
                           "mdelete [%s] error vRet=[%d]\n%s",
                           tmpFullFile, vRet, oErrMsg_p );
                    outPutErrM ( oErrMsg_p, errMsg );
                    closedir ( dir_p );

                    return ERR_DIY_FUNCRET;
                }
            }
        }
    }

    closedir ( dir_p );

    return 0;

}





