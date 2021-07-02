
#ifndef METEFUNCLIB_H
#define METEFUNCLIB_H

//#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
//#include<time.h>
//#include<sys/time.h>
#include<stdlib.h>

//#include<unistd.h>
//#include<fcntl.h>

//#include<stdarg.h>
#include<dirent.h>
//#include<sys/stat.h>
//#include<errno.h>
//#include<regex.h>


#include "baseFunc.h"

#include "mete_datatype.h"
//#include "mete_macro.h"  已经包含在 mete_datatype.h 中了
#include "tinyxml2.h"

using namespace tinyxml2;


/*************************************
  *功能 ：加载日志配置到全局变量
  * 输入：xml配置文件
  *
  * 输出：
  *    oErrMsg_p  如果此变量为NULL则错误信息直接printf
  *               如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *返回：
  *     　0         :成功，
  *     　<0        :失败
  *修改：
  *    　20170706141957 创建函数
  *注：
*      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int loadLogCfg ( const XMLDocument* doc_p,
                 T_meteLogs* meteLogs_p,
                 const char* oErrMsg_p = NULL
               );



/*************************************
  *功能 ：取得ftp服务器配置数量
  * 输入：xmlPth     :xml配置文件
  *       doFlagStr_p:操作标识符
  *                     MC_L_GETDOWN:加载ftp下载的配置
  *                     MC_L_PUTUP:  加载ftp上传的配置
  * 输出：
  *       serNum    服务器数量
  *      oErrMsg_p  如果此变量为NULL则错误信息直接printf
  *                 如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *返回：
  *     　0         :成功，
  *       !=0       :失败
  *修改：
  *    　20170728161348 创建函数
  *注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int getFserNumFxml ( const XMLDocument& doc,
                     const char* doFlagStr_p,
                     int& serNum,
                     const char* oErrMsg_p = NULL
                   );

/*************************************
  *功能 ：ftp空间时循环查找是否有处理任务的时间间隔设置
  * 输入：doc        :xml配置文件XMLDocument对象
  *       doFlagStr_p:操作标识符
  *                     MC_L_GETDOWN:加载ftp下载的配置
  *                     MC_L_PUTUP:  加载ftp上传的配置
  * 输出：
  *       second    秒
  *       micSecond 微秒
  *       repet     >0 表示重复次数，<0表示无限重复
  *      oErrMsg_p  如果此变量为NULL则错误信息直接printf
  *                 如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *返回：
  *     　0         :成功，
  *       !=0       :失败
  *修改：
  *    　20170728161348 创建函数
  *注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int getChkCycleFxml ( const XMLDocument& doc,
                      const char* doFlagStr_p,
                      int& second,
                      int& micSecond,
                      int& repet,
                      const char* oErrMsg_p = NULL
                    );


/*************************************
  *功能 ：加载ftp配置
  * 输入：xmlPth     :xml配置文件
  *       doFlagStr_p:操作标识符
  *                     MC_L_GETDOWN:加载ftp下载的配置
  *                     MC_L_PUTUP:  加载ftp上传的配置
  *       serNumTh : 需要读第几个ftp的配置(如1则是读第1个，2则是读第2个 等等)
  *
  * 输出：
  *       shNames  :加载后的shell配置
  *       ftpParas :加载后的ftp参数
  *       oErrMsg_p  如果此变量为NULL则错误信息直接printf
  *                  如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *返回：
  *     　0         :成功，
  *       !=0       :失败
  *修改：
  *    　20170705135530 创建函数
  *注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int loadFtpCfg ( const XMLDocument& doc,
                 const char* doFlagStr_p,
                 const int& serNumTh,
                 T_doFtpSh& shNames,
                 T_ftpParas& ftpParas,
                 const char* oErrMsg_p = NULL
               );

/*************************************
  *功能 ：下载ftp文件
  * 输入：shNames   ftp脚本配置
  *       ftpParas  ftp处理的参数
  * 输出：
  *    oErrMsg_p  如果此变量为NULL则错误信息直接printf
  *               如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *返回：
  *     　0         :成功，
  *       <0        :失败
  *修改：
  *    　20170630152837 创建函数
  *注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int downFtpFiles ( const T_doFtpSh& shNames,
                   const T_ftpParas& ftpParas,
                   const char* oErrMsg_p = NULL,
                   const char* ftpCmd_p = NULL
                 );


/*************************************
  *功能 ：上传ftp文件
  * 输入：shNames   ftp脚本配置
  *       ftpParas  ftp处理的参数
  * 输出：
  *    oErrMsg_p  如果此变量为NULL则错误信息直接printf
  *               如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *返回：
  *     　0         :成功，
  *       <0        :失败
  *修改：
  *    　20170724155845 创建函数
  *注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int putFtpFiles ( const T_doFtpSh& shNames,
                  const T_ftpParas& ftpParas,
                  const char* oErrMsg_p = NULL,
                  const char* ftpCmd_p = NULL
                );


/*************************************
  *功能 ：初始化函数
  * 输入：
  *       loadXFlag       是否加载本地xml文件生成输出XMLDocument对象
  *                           0表示不加载，其他值为加载
  * 输出：
  *       meteLogs_p      日志配置变量
  *       locaXmlName_p   本地xml配置
  *       oErrMsg_p       如果此变量为NULL则错误信息直接printf
  *                       如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *       doc_p           本地配置文件的xml对象，当loadXFlag非0时有此变量
  *返回：
  *     　0         :成功，
  *       <0        :失败
  *修改：
  *    　20170630152837 创建函数
*************************************/
int iniSys ( T_meteLogs* meteLogs_p,
             char* locaXmlName_p,
             const char* oErrMsg_p,
             const int& loadXFlag = 0,
             XMLDocument* doc_p = NULL
           );

/*************************************
  *功能 ：从xml文件中取变量指定第一元素的值
  * 输入：
  *      doc         加载xml后的XMLDocument变量
  *      linkXmlEles 需要取的element元素结构体变量(存储要找的节点名)
  *
  * 输出：
  *      dirVal 路径值
  *      oErrMsg_p  如果此变量为NULL则错误信息直接printf
  *                 如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *返回：
  *     　0         :成功，
  *       <0        :失败
  *修改：
  *    　20170713104803 创建函数
  *注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int getFirstEleVal ( const XMLDocument& doc,
                     const T_linkXmlEles& linkXmlEles,
                     char*& eleVal_p,
                     const char* oErrMsg_p = NULL
                   );

/*************************************
  *功能 ：从xml文件中取变量指定第n元素的值
  * 输入：
  *      doc         加载xml后的XMLDocument变量
  *      linkXmlEles 需要取的element元素结构体变量(存储要找的节点名)
  *      seqNum      需要读的第个节点的节，基数从1而不是0开始
  *
  * 输出：
  *      dirVal 路径值
  *      oErrMsg_p  如果此变量为NULL则错误信息直接printf
  *                 如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *返回：
  *     　0         :成功，
  *       <0        :失败
  *修改：
  *    　20171124094620 创建函数
  *注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int getSeqNumEleVal ( const XMLDocument& doc,
                      const T_linkXmlEles& linkXmlEles,
                      const int& seqNum,
                      char*& eleVal_p,
                      const char* oErrMsg_p
                    );

/*************************************
  *功能 ：从本地配置取得处理路径
  * 输入：
  *      doc         加载xml后的XMLDocument变量
  *      eleName_p   需要取的element元素名
  *
  * 输出：
  *      dirVal_p    路径值
  *      oErrMsg_p  如果此变量为NULL则错误信息直接printf
  *                 如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *返回：
  *     　0         :成功，
  *       <0        :失败
  *修改：
  *    　20170713104803 创建函数
  *注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int getDoDirFromL ( const XMLDocument& doc,
                    const char* eleName_p,
                    char*& dirVal_p,
                    const char* oErrMsg_p = NULL
                  );



/*************************************
  *功能 ：从本地配置取得测风塔接入配置
  * 输入：
  *      doc        加载xml后的XMLDocument变量
  *
  * 输出：
  *      accVal_p  配置值
   *     oErrMsg_p  如果此变量为NULL则错误信息直接printf
  *                 如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *返回：
  *     　0         :成功，
  *       <0        :失败
  *修改：
  *    　20170713104803 创建函数
  *注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int getAccWinFromL ( const XMLDocument& doc,
                     char*& accVal_p,
                     const char* oErrMsg_p = NULL
                   );

/*************************************
  *功能 ：从业务清单取多机协调编码
  * 输入：
  *      doc        加载xml后的XMLDocument变量
  *
  * 输出：
  *    outVal_p   配置值
  *    oErrMsg_p  如果此变量为NULL则错误信息直接printf
  *               如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *返回：
  *     　0         :成功，
  *       <0        :失败
  *修改：
  *    　20170713113204 创建函数
  *注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int getCoordIdFromB ( const XMLDocument& doc,
                      char*& outVal_p,
                      const char* oErrMsg_p = NULL
                    );

/*************************************
  *功能 ：从业务清单取业务编码
  * 输入：
  *      doc        加载xml后的XMLDocument变量
  *      seqNum     业务项标识符序列号(即如果有多个业务清单编码时应取哪一个)
  *
  * 输出：
  *      outVal_p  配置值
  *      oErrMsg_p  如果此变量为NULL则错误信息直接printf
  *                 如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *返回：
  *     　0         :成功，
  *       <0        :失败
  *修改：
  *    　20170713131706 创建函数
  *注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int getJobIdFromB ( const XMLDocument& doc,
                    const int& seqNum,
                    char*& outVal_p,
                    const char* oErrMsg_p = NULL
                  );

/*************************************
  *功能 ：生成业务清单或测风计划提供情况的文件头
  * 输入：
  *      doc           :XMLDocument对象
  *      ym_p          :年月日 时间变量
  *      doFlagStr_p   :业务控制字符;
  *
  * 输出：
  *    oErrMsg_p  如果此变量为NULL则错误信息直接printf
  *               如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *返回：
  *     　0         :成功，
  *       <0        :失败
  *修改：
  *    　20170714104643 创建函数
  *注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int putXmlHeadPs ( XMLDocument& doc,
                   const char* ym_p,
                   const char* doFlagStr,
                   const char* oErrMsg_p = NULL
                 );


/*************************************
  *功能 ：向业务清单提供情况的xml文件加入数据项
  * 输入：
  *      doc           :XMLDocument对象
  *      UxmlLItem     :xml元素结构体
  *      doFlagStr_p   :业务控制字符;
  *
  * 输出：
  *    oErrMsg_p  如果此变量为NULL则错误信息直接printf
  *               如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *返回：
  *     　0         :成功，
  *       <0        :失败
  *修改：
  *    　20170714105915 创建函数
  *注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int putXmlitemPs ( XMLDocument& doc,
                   const T_UxmlLItem& UxmlLItem,
                   const char* doFlagStr_p,
                   const char* oErrMsg_p = NULL
                 );

/*************************************
  *功能 ：生成相应的xml文件
  * 输入：
  *      locaXmlCfm_p   :本地配置文件名(带路径)
  *       doFlagStr_p   :MC_L_DOSTAPATH 生成业务清单提供情况xml;
  *                     MC_L_DOWPLANPATH ，生成III区测风塔采集业务处理计划xml
  *       fileEleName_p :业务清单文件名(不带路径);

  *
  * 输出：
  *    oErrMsg_p  如果此变量为NULL则错误信息直接printf
  *               如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *返回：
  *     　0         :成功，
  *       <0        :失败
  *修改：
  *    　20170714194914 创建函数
  *注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int putXmlLFilePs ( const XMLDocument& docLocal,
                    const char* doFlagStr_p,
                    const char* fileEleName_p,
                    const char* oErrMsg_p = NULL
                  );

/*************************************
  *功能 ：合并xml文件
  * 输入：
  *       srcXml_p      :源xml文件路径;
  *       mergeXml_p    :最终合并到的文件路径;
  *       doFlagStr_p   :业务控制字符;
  *
  *
  * 输出：
  *    oErrMsg_p  如果此变量为NULL则错误信息直接printf
  *               如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *返回：
  *     　0         :成功，
  *       <0        :失败
  *修改：
  *    　20170718102239 创建函数
  *注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int mergeXmlPs ( const char* srcXml_p,
                 const char* mergeXml_p,
                 const char* doFlagStr_p,
                 const char* oErrMsg_p = NULL
               );



/*************************************
  *功能 ：合成在用业务清单
  * 输入：
  *      localXmlName_p:本地xml配置文件
  *      fileName_p    :业务清单文件名(不带路径)
  *
  * 输出：
  *    oErrMsg_p  如果此变量为NULL则错误信息直接printf
  *               如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *返回：
  *     　0         :成功，
  *       <0        :失败
  *修改：
  *    　20170710164850 创建函数
  *注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int synthInList ( const XMLDocument& docLocal,
                  const char* filName_p,
                  const char* oErrMsg_p = NULL
                );



/*************************************
  *功能 ：生成业务清单提供情况
  * 输入：
  *      localXmlName_p:本地xml配置文件
  *      fileName_p    :业务清单文件名(不带路径)
  *
  * 输出：
  *      oErrMsg_p  如果此变量为NULL则错误信息直接printf
  *                 如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *返回：
  *     　0         :成功，
  *       <0        :失败
  *修改：
  *    　20170710174802 创建函数
  *注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int statList ( const XMLDocument& docLocal,
               const char* fileNmae_p,
               const char* oErrMsg_p = NULL
             );



/*************************************
  *功能 ：写返回信息到xml状态文件
  * 输入：
  *      doc_p           :XMLDocument对象
  *      docPath_p       :xml文件路径
  *      (doc_p与docPath_p有一个有值即可)
  *      statDescItem_p :T_xmlStatDescItem指针变量
  *
  * 输出：
  *    oErrMsg_p  如果此变量为NULL则错误信息直接printf
  *               如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *返回：
  *     　0         :成功，
  *       <0        :失败
  *修改：
  *    　20190307 创建函数
  *注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int writeRetToXml ( XMLDocument* doc_p,
                   const char* docPath_p,
                   T_xmlStatDescItem* statDescItem_p,
                   const char* oErrMsg_p = NULL
                 );



#endif // METEFUNCLIB_H
