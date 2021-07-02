#ifndef HLTHRPRTWRTXML_H
#define HLTHRPRTWRTXML_H

#include <unistd.h>
#include<time.h>
#include<sys/time.h>
#include "tinyxml2.h"

#if defined(TINYXML2_INCLUDED)
    using namespace tinyxml2;
#endif

#define MC_ERRMSG_MAXH             512    //错误信息临时变量大小

//错误输出码
#define ERR_POINTER_NULL     -0x01  //指针变量为空报错
#define ERR_XML_LOAD         -0x04  //加载xml文件错误
#define ERR_INPUT_VALUE      -0x06  //输入值错误


//健康报告标识控制
#define MC_FLAG_MERGEH            "toIntegHealth" //由健康报告合并成综合健康报告标识
#define MC_FLAG_HLTH              "health"        //健康报告标识
#define MC_FLAG_INTHLTH           "integHealth"   //综合健康报告


//健康报告xml元素定义
#define MC_H_COMMENT                  "state health report"            // 康健报告xml描述
#define MC_H_COMMENT_INTEG        "comprehensive health report"   // 综合健康报告xml描述
#define MC_H_FNM                   "FNM"         //根节点
#define MC_H_NAME                  "name"        //场站名称
#define MC_H_ITEMLIST              "itemList"    //信息列表
#define MC_H_ITEMDATA              "itemData"    //数据项
#define MC_H_TASKNUM               "taskNum"     //任务编号
#define MC_H_HOSTNAME              "hostName"    //宿主名称
#define MC_H_BEWATCHEDN            "beWatchedN"  //被监视者名称
#define MC_H_BEWATCHEDS            "beWatchedS"  //被监视者状态
#define MC_H_CAPTIME               "capTime"     //捕获时间
#define MC_H_THEMONITOR            "theMonitor"  //监视者
#define MC_H_TASKATTRIBV           "value"       //在综合健康报告里 任务编号的属性名


//健康报告数据项变量长度定义
#define MC_FNM_LEN              30  //场站名称长度
#define MC_TASKNUM_LEN          10  //任务编号长度
#define MC_HOSTNAME_LEN         20  //宿主名称长度
#define MC_BEWATCHEDN_LEN       30  //被监视者名称长度
#define MC_BEWATCHEDS_LEN       10  //被监视者状态长度
#define MC_THEMONITOR_LEN       30  //监视者长度


//健康报告生成方式配置
//#define MC_TOHFROMCFG            0  //在生成接口中直接读取本地xml配置文件相关配置 (此种方式可能会产生频繁读xml配置的问题)
//#define MC_TOHFROMVAR            1  //在生成接口中健康报告的有关配置从变量中取得



//健康报告的数据项
typedef struct
{
    char taskNum[MC_TASKNUM_LEN];
    char hostName[MC_HOSTNAME_LEN];
    char beWatchedN[MC_BEWATCHEDN_LEN];
    char beWatchedS[MC_BEWATCHEDS_LEN];
    char capTime[25];
    char theMonitor[MC_THEMONITOR_LEN];
} T_xmlHtItem;

//健康报告生成相关变量
typedef struct
{
    char fuallName[170];           //生成文件:路径+文件名  cfgFlag=1时此值可为空
    char fieldName[MC_FNM_LEN];   //场站名
    T_xmlHtItem xmlHtItem;        //数据项
} T_xmlHVar;




/*************************************
  *功能 ：生成健康报告xml的文件头
  ------------------------------------------
  * 输入：
  *      doc           :XMLDocument对象
  *      ym_p          :年月日 时间变量
  *      doFlagStr_p   :业务控制字符;(
  判断是生成健康报告 还是 综合健康报告)
                        MC_L_DOHPATH,生
  成健康报告; MC_L_INTEGHPATH生成综合健
  康报告
  *      fieldNmae_p   :场站名称
  *      oErrMsg_p    如果此变量为NULL则
  错误信息直接printf
  *                   如果此变量不为NULL
  则将错误信息加上行号函数名返回至此变量
  *
  ------------------------------------------
  * 输出：
  *       无
  ------------------------------------------
  *返回：
  *     　0         :成功，
  *       <0        :失败
  ------------------------------------------
  *注:
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH
                  个字符，避免异常
*************************************/
int putXmlHeadH ( XMLDocument& doc,
                  const char* ym_p,
                  const char* doFlagStr_p,
                  const char* fieldNmae_p,
                  const char* oErrMsg_p = NULL
                );

/*************************************
  *功能 ：向康健报告的xml文件加入数据项
  ------------------------------------------
  * 输入：
  *      doc           :XMLDocument对象
  *      xmlHtItem     :xml元素结构体
  *      doFlagStr_p   :业务控制字符;(
  判断是生成健康报告 还是 综合健康报告)
                        MC_L_DOHPATH,生
  成健康报告; MC_L_INTEGHPATH生成综合健康报告
  *      oErrMsg_p    如果此变量为NULL
  则错误信息直接printf
  *                   如果此变量不为NULL
  则将错误信息加上行号函数名返回至此变量
  *

  ------------------------------------------
  * 输出：无

  ------------------------------------------
  *返回：
  *     　0         :成功，
  *       <0        :失败

  ------------------------------------------
  *注:
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH
                  个字符，避免异常
*************************************/
int putXmlitemH ( XMLDocument& doc,
                  const T_xmlHtItem& xmlHtItem,
                  const char* doFlagStr_p,
                  const char* errLog_p = NULL
                );


/*************************************
  *功能 ：生成健康报告相应的xml文件
  ------------------------------------------
  * 输入：
  *      locaXmlCfm_p   :代表生成xml文
                         件的文件名(带路径)
  *       doFlagStr_p   :MC_L_DOHPATH
                         生成健康报告xml;
  *                      MC_L_INTEGHPATH ，
                         生成综合健康报告xml
  *       xmlHVar       :健康报告生成相关变量
  *      oErrMsg_p      如果此变量为NULL
                         则错误信息直接printf
  *                     如果此变量不为NULL
                         则将错误信息加上行号
                         函数名返回至此变量
  *
  ------------------------------------------
  * 输出：无
  ------------------------------------------
  *返回：
  *     　0         :成功，
  *       <0        :失败
  ------------------------------------------
  *注:
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH
                  个字符，避免异常
*************************************/
int putXmlLFileH ( const char* locaXmlCfg_p,
                   const char* doFlagStr_p,
                   T_xmlHVar& xmlHVar,
                   const char* oErrMsg_p = NULL
                 );

/*************************************
  *功能 ：由已有健康报告合并成综合健康报告
  ------------------------------------------
  * 输入：
  *       srcXml_p      :健康报告xml文件路径;
  *       mergeXml_p    :最终合并到的文件路径;
  *      oErrMsg_p    如果此变量为NULL则错误信息直接printf
  *                   如果此变量不为NULL则将错误信息加上行号函数名返回至此变量
  *
  *
  ------------------------------------------
  * 输出：无
  ------------------------------------------
  *返回：
  *     　0         :成功，
  *       <0        :失败
  ------------------------------------------
  *注:
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH
                  个字符，避免异常
*************************************/
int mergeToIntegXml ( const char* srcXml_p,
                      const char* mergeXml_p,
                      const char* oErrMsg_p = NULL
                    );


#endif // HLTHRPRTWRTXML_H
