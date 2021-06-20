/****************************************
 * 
 *    WriteLog.h 
 * 
 *    Create by fushikai
 * 
 *    2021-06-19
 * 
 ****************************************/
#ifndef LIB_WRITELOG_H
#define LIB_WRITELOG_H



/******************************************************************************/

// 说明:

// 1.日志默认输出到程序运行同级目录下的 LOG/$YYYY/$MM/$DD.log
//
//         其中$YYYY: 为运行程序的时刻对应的年份
//             $MM  : 为运行程序的时刻对应的月份
//             $DD  : 为运行程序的时刻对应的日
//
//         例如：当2021-06-14 这一天运行程序则会生成
//               LOG/2021/06/14.log


// 2.不采用默认输出日志路径时需要用WriteLogInit的接口进行设置

// 3.日志输出控制通过logLevel控制，初始设置此值有2种方法:
//   (1) 在程序中用WriteLogInit接口进行设置
//   (2) 在程序运行的同级目录下新建立文件LogLevel.ini
//       文件内容为LogLevel = 7 其中7为日志级别，数值
//       参考logLevel enum定义

// 4.如果不设置logLevel，则所有的日志都默认输出

// 5. logLevel的控制逻辑是 小于等于 logLevel的日志级别都会输出
//    例如: logLevel 设置为LOGERROR 则在日志输出中如下日志都会输出
//              LOGERROR,LOGCRIT,LOGALERT,LOGEMERG

// 6. 代码中有获取程序当前运行路径名的buf定义是1024，因此当前只支持
//    程序运行绝对路径名字符串长度小于1024的可执行程序输出日志


/******************************************************************************/

// logLevel
enum{
    LOGEMERG,
    LOGALERT,
    LOGCRIT,
    LOGERROR,
    LOGWARNING,
    LOGNOTICE,
    LOGINFO,
    LOGDEBUG,
    LOGEND
};

#define LOGINITINFO LOGEND


/******************************************************************************/

void WriteLogInit( 
        const int& iLevel=LOGDEBUG,      /*设置日志显示的等级(输出日志等级<=iLevel时才输出)*/
        const char* dirName_p=NULL,      /*不设置时默认日志路径为程序运行路径下LOG/$YYYY/$MM*/
        const char* logPrefixName_p=NULL /*当设置dirName_p时才设置此值：日志名为logPrefixName_$YYYYMMDD.log*/
        );

void _WriteLog ( const char* FILE_p, int LINE, int logLevel, const char* format_p, ... );
void _PrintToStdout( const char* FILE_p, int LINE, int logLevel, const char* format_p, ... );

/******************************************************************************/

#define WriteLog( logLevel, str, arg... )  \
    _WriteLog(__FILE__, __LINE__, logLevel, str, ##arg); 


#define PrintToStdout( logLevel, str, arg... )  \
    _PrintToStdout(__FILE__, __LINE__, logLevel, str, ##arg); 

/******************************************************************************/


#endif //LIB_WRITELOG_H
