#ifndef METE_BASE_H
#define METE_BASE_H

#include "mete_macro.h"
#include "baseFunc.h"

typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;

//日志名参数
typedef struct
{
    char path[MC_PATH_LEN];        //路径
    char name[MC_NAME_LEN];        //名称
    char suffix[MC_SUFNAME_LEN];   //后缀
} T_logName;

typedef struct
{
    char fullErrLog[MC_FULLN_LEN];   //错误日志名
    char fullMainLog[MC_FULLN_LEN];  //主程序日志名
    char fullOtherLog[MC_FULLN_LEN]; //其他程序日志名
    int  debugFlag; //0 不打印debug,1打印debug

} T_meteLogs;

//ftp需要的参数
typedef struct
{
    char  serverIP[MC_IP_LEN];        //服务器ip
    u_int portN;                      //端口号
    char  userName[MC_USERNAME_LEN]; //用户名
    char  userPwd[MC_USERPWD_LEN];   //密码
    char  trsType;                   //传输类型0:ascii ,1:binary;
    char  trsMode;                   //传输模式0:passive,1:active
    char  remoteDir[MC_PATH_LEN];   //服务器目录
    char  localDir[MC_PATH_LEN];    //客户端目录
    char  fileNames[MC_NAME_LEN * MC_ONECE_FNUM]; //文件名
    u_int outTimeMs;                //超时设置时间  单位毫秒
    char  cmpDir[MC_PATH_LEN];      //完成ftp传输后需要移动到的目录

} T_ftpParas;


//ftp需要的参数
typedef struct
{
    char  chkShName[100];    //检测状态的shell名(带路径)
    char  doShName[100];     //执行ftp的shell名(带路径)

} T_doFtpSh;

//业务清单提供情况xml文件的数据项
typedef struct
{
    char fileName[MC_NAME_LEN];
    char cmtTime[MC_TIMES_LEN];
} T_xmlStatItem;

//III区测风塔采集业务处理计划xml 任务数据项
typedef struct
{
    char busLstName[MC_NAME_LEN];
    char windName[MC_NAME_LEN];
    char cmtTime[MC_TIMES_LEN];
} T_xmlCWItem;

typedef union
{
    T_xmlStatItem xmlStatItem;
    T_xmlCWItem   xmlCWItem;
} T_UxmlLItem;


//存在"查找xml元素名"的变量结构
typedef struct
{
    char ( *linkEles ) [MC_XMLELE_LEM]; // 指向数组的指针:存储多个xml节点名
    int  linkNum;                   //节点名的个数
} T_linkXmlEles;


typedef struct
{
    char retTime[MC_TIMES_LEN];
    char retCode[MC_RETCODE_LEN];
    char retMsg[MC_RETMSG_LEN];
} T_xmlStatDescItem;





#endif // METE_BASE_H
