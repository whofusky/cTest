#include "funcLib.h"
#include "pubExtern.h"

//int g_dug_flag = 0;   //是否打印日志标识
//int g_load_onece = 0; //加载一次标识


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
                 const char* oErrMsg_p
               )
{
    char errMsg[MC_ERRMSG_MAX] = {};

    if ( meteLogs_p == NULL || doc_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input params meteLogs_p or doc_p is NULL error" );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_POINTER_NULL;
    }

    //    int vRet = 0;

    //解析xml文件，格式如下
    /*
    <?xml version="1.0" encoding="UTF-8" standalone="no" ?>
    <!-- 气象下载器本地配置 -->
    <ToDo>
        <logCfg>
        <errlog>
            <path>/home/fczx/log</path>
            <name>err</name>
        </errlog>
        <mainLog>
            <path>/home/fczx/log</path>
            <name>Main</name>
        </mainLog>
        <getFtpLog>
            <path>/home/fczx/log</path>
            <name>getFtp</name>
        </getFtpLog>
    </logCfg>
    </ToDo>
    */

    //    XMLDocument doc;
    //  vRet = doc.LoadFile( xmlPth_p );
    //    if( vRet!=0 )
    //    {
    //        if(prtFlag==0)
    //        {
    //            writeLog(oErrMsg_p,
    //                "%s|%s|%d:load xml file failed vRet=[%d][%s]\n",
    //                __FILE__,__func__,__LINE__,vRet,doc.ErrorName());
    //        }
    //        return ERR_XML_LOAD;
    //    }

    const XMLElement* logCfgElement_p = doc_p->FirstChildElement (
                                        MC_L_TODO )->FirstChildElement ( MC_L_LOGCFG );

    if ( logCfgElement_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s is NULL",
                   MC_L_TODO, MC_L_LOGCFG );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    //debugFlag
    int tDebugFlag = 0;
    const XMLElement* logDebug_p = logCfgElement_p->FirstChildElement ( MC_L_DEBUG );

    if ( logDebug_p != NULL )
    {
        logDebug_p->QueryIntText ( &tDebugFlag );
    }

    //errlog
    const XMLElement* errElement_p = logCfgElement_p->FirstChildElement ( MC_L_ERRLOG );

    if ( errElement_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s is NULL",
                   MC_L_TODO, MC_L_LOGCFG, MC_L_ERRLOG );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    const XMLElement* errElePath_p = errElement_p->FirstChildElement ( MC_L_PATH );

    if ( errElePath_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s->%s is NULL",
                   MC_L_TODO, MC_L_LOGCFG, MC_L_ERRLOG, MC_L_PATH );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    const char* errPath_p = errElePath_p->GetText();

    if ( errPath_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s->%s GetText()is NULL",
                   MC_L_TODO, MC_L_LOGCFG, MC_L_ERRLOG, MC_L_PATH );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    const XMLElement* errEleName_p = errElement_p->FirstChildElement ( MC_L_NAME );

    if ( errEleName_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s->%s is NULL",
                   MC_L_TODO, MC_L_LOGCFG, MC_L_ERRLOG, MC_L_NAME );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    const char* errName_p = errEleName_p->GetText();

    if ( errName_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s->%s GetText() is NULL",
                   MC_L_TODO, MC_L_LOGCFG, MC_L_ERRLOG, MC_L_NAME );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    //mainLog
    const XMLElement* mainElement_p = logCfgElement_p->FirstChildElement ( MC_L_MAINLOG );

    if ( mainElement_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s is NULL",
                   MC_L_TODO, MC_L_LOGCFG, MC_L_MAINLOG );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    const XMLElement* mainElePath_p = mainElement_p->FirstChildElement ( MC_L_PATH );

    if ( mainElePath_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s->%s is NULL",
                   MC_L_TODO, MC_L_LOGCFG, MC_L_MAINLOG, MC_L_PATH );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    const char* mianPath_p = mainElePath_p->GetText();

    if ( mianPath_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s->%s GetText()is NULL",
                   MC_L_TODO, MC_L_LOGCFG, MC_L_MAINLOG, MC_L_PATH );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    const XMLElement* mainEleName_p = mainElement_p->FirstChildElement ( MC_L_NAME );

    if ( mainEleName_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s->%s is NULL",
                   MC_L_TODO, MC_L_LOGCFG, MC_L_MAINLOG, MC_L_NAME );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    const char* mianName_p = mainEleName_p->GetText();

    if ( mianName_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s->%s GetText() is NULL",
                   MC_L_TODO, MC_L_LOGCFG, MC_L_MAINLOG, MC_L_NAME );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    //getFtpLog
    const XMLElement* getElement_p = logCfgElement_p->FirstChildElement ( MC_L_GETFTPLOG );

    if ( getElement_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s is NULL",
                   MC_L_TODO, MC_L_LOGCFG, MC_L_GETFTPLOG );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    const XMLElement* getElePath_p = getElement_p->FirstChildElement ( MC_L_PATH );

    if ( getElePath_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s->%s is NULL",
                   MC_L_TODO, MC_L_LOGCFG, MC_L_GETFTPLOG, MC_L_PATH );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    const char* getPath_p = getElePath_p->GetText();

    if ( getPath_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s->%s GetText() is NULL",
                   MC_L_TODO, MC_L_LOGCFG, MC_L_GETFTPLOG, MC_L_PATH );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    const XMLElement* getEleName_p = getElement_p->FirstChildElement ( MC_L_NAME );

    if ( getEleName_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s->%s is NULL",
                   MC_L_TODO, MC_L_LOGCFG, MC_L_GETFTPLOG, MC_L_NAME );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    const char* getName_p = getEleName_p->GetText();

    if ( getName_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s->%s GetText() is NULL",
                   MC_L_TODO, MC_L_LOGCFG, MC_L_GETFTPLOG, MC_L_NAME );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    //    char buf_p[10] = {};
    //    vRet = getTime(3,buf_p,NULL,oErrMsg_p);


    //打印输出日志
    //    d_printf("%s|%s|%d:get from xml errPath_p=[%s]\n",__FILE__,__func__,__LINE__,errPath_p);
    //    d_printf("%s|%s|%d:get from xml errName_p=[%s]\n",__FILE__,__func__,__LINE__,errName_p);
    //    d_printf("%s|%s|%d:get from xml mianPath_p=[%s]\n",__FILE__,__func__,__LINE__,mianPath_p);
    //    d_printf("%s|%s|%d:get from xml mianName_p=[%s]\n",__FILE__,__func__,__LINE__,mianName_p);
    //    d_printf("%s|%s|%d:get from xml getPath_p=[%s]\n",__FILE__,__func__,__LINE__,getPath_p);
    //    d_printf("%s|%s|%d:get from xml getName_p=[%s]\n",__FILE__,__func__,__LINE__,getName_p);
    //    d_printf("%s|%s|%d:get from xml buf_p=[%s]\n",__FILE__,__func__,__LINE__,buf_p);

    //    strncpy(g_errLog.path,errPath_p,sizeof(g_errLog.path));
    //    strncpy(g_errLog.name,errName_p,sizeof(g_errLog.name));
    //    strncpy(g_errLog.suffix,buf_p,sizeof(g_errLog.suffix));

    //    strncpy(g_mainLog.path,mianPath_p,sizeof(g_mainLog.path));
    //    strncpy(g_mainLog.name,mianName_p,sizeof(g_mainLog.name));
    //    strncpy(g_mainLog.suffix,buf_p,sizeof(g_mainLog.suffix));

    //    strncpy(g_getFtpLog.path,getPath_p,sizeof(g_getFtpLog.path));
    //    strncpy(g_getFtpLog.name,getName_p,sizeof(g_getFtpLog.name));
    //    strncpy(g_getFtpLog.suffix,buf_p,sizeof(g_getFtpLog.suffix));

    //    snprintf(meteLogs_p->fullErrLog,sizeof(meteLogs_p->fullErrLog),"%s/%s%s.log",errPath_p,errName_p,buf_p);
    //    snprintf(meteLogs_p->fullMainLog,sizeof(meteLogs_p->fullMainLog),"%s/%s%s.log",mianPath_p,mianName_p,buf_p);
    //    snprintf(meteLogs_p->fullGetLog,sizeof(meteLogs_p->fullGetLog),"%s/%s%s.log",getPath_p,getName_p,buf_p);

    snprintf ( meteLogs_p->fullErrLog,
               sizeof ( meteLogs_p->fullErrLog ),
               "%s/%s",
               errPath_p, errName_p );
    snprintf ( meteLogs_p->fullMainLog,
               sizeof ( meteLogs_p->fullMainLog ),
               "%s/%s",
               mianPath_p, mianName_p );
    snprintf ( meteLogs_p->fullOtherLog,
               sizeof ( meteLogs_p->fullOtherLog ),
               "%s/%s",
               getPath_p, getName_p );
    meteLogs_p->debugFlag = tDebugFlag;

    //doc.Clear();
    return 0;
}

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
                     const char* oErrMsg_p
                   )
{
    char errMsg[MC_ERRMSG_MAX] = {};

    if ( doFlagStr_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input params doFlagStr_p is NULL  error" );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_POINTER_NULL;
    }

    if ( strcmp ( doFlagStr_p, MC_L_GETDOWN ) != 0
         && strcmp ( doFlagStr_p, MC_L_PUTUP ) != 0
       )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input parameter doFlagStr_p value error doFlagStr_p=[%s]",
                   doFlagStr_p );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_INPUT_VALUE;

    }


    int vRet = 0;

    //解析xml文件，格式如下
    /*
    <?xml version="1.0" encoding="UTF-8" standalone="no" ?>
    <!-- 气象下载器本地配置 -->
    <ToDo>
        <GetDown ftpnum="1">
        <FtpSer ip="192.168.0.154" port="21" user="Administrator" pwd="qwer1234" timeoutMS="32000">
            <ChkShName>/home/fczx/sh/getFserStas.sh</ChkShName>
            <DoShName>/home/fczx/sh/getFileFtp.sh</DoShName>
            <RemoteDir>/tmp</RemoteDir>
            <LocalDir>/home/fczx/tmp</LocalDir>
            <FileDes num="2">
                <FileName>"44_5m 227_m"</FileName>
            </FileDes>
        </FtpSer>
        </GetDown>
    </ToDo>
    */
    //    XMLDocument doc;
    //  vRet = doc.LoadFile( xmlPth_p );
    //    if( vRet!=0 )
    //    {
    //        if(prtFlag==0)
    //        {
    //            writeLog(oErrMsg_p,
    //                "%s|%s|%d:load xml file failed vRet=[%d][%s]\n",
    //                __FILE__,__func__,__LINE__,vRet,doc.ErrorName());
    //        }
    //        return ERR_XML_LOAD;
    //    }

    const XMLElement* getElement_p = doc.FirstChildElement ( MC_L_TODO )->FirstChildElement (
                                     doFlagStr_p );

    if ( getElement_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s NULL error",
                   MC_L_TODO, doFlagStr_p );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    // int serNum = 0;
    vRet = getElement_p->QueryIntAttribute ( MC_L_FTPNUM, &serNum );

    if ( vRet != 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "QueryIntAttribute %s failed  vRet=[%d][%s]",
                   MC_L_FTPNUM, vRet, doc.ErrorIDToName ( ( XMLError ) vRet ) );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    //doc.Clear();

    return 0;
}


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
                      const char* oErrMsg_p
                    )
{
    char errMsg[MC_ERRMSG_MAX] = {};

    if ( doFlagStr_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input params doFlagStr_p is NULL  error" );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_POINTER_NULL;
    }

    if ( strcmp ( doFlagStr_p, MC_L_GETDOWN ) != 0
         && strcmp ( doFlagStr_p, MC_L_PUTUP ) != 0
       )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input parameter doFlagStr_p value error doFlagStr_p=[%s]",
                   doFlagStr_p );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_INPUT_VALUE;

    }

    //解析xml文件，格式如下
    /*
    <?xml version="1.0" encoding="UTF-8" standalone="no" ?>
    <!-- 气象下载器本地配置 -->
    <ToDo>
        <GetDown ftpnum="1">
            <Cycle> <!--空闲时，循环检查是否有下载文件时间间隔设置-->
                <second>10</second>
                <microSecond>0</microSecond>
                <repet>1</repet> <!--重复次数:大于0为具体的重复次数，小于0为无限循环-->
            </Cycle>
        <FtpSer ip="192.168.0.154" port="21" user="Administrator" pwd="qwer1234" timeoutMS="32000">
            <ChkShName>/home/fczx/sh/getFserStas.sh</ChkShName>
            <DoShName>/home/fczx/sh/getFileFtp.sh</DoShName>
            <RemoteDir>/tmp</RemoteDir>
            <LocalDir>/home/fczx/tmp</LocalDir>
            <FileDes num="2">
                <FileName>"44_5m 227_m"</FileName>
            </FileDes>
        </FtpSer>
        </GetDown>
    </ToDo>
    */
    //    XMLDocument doc;
    //  vRet = doc.LoadFile( xmlPth_p );
    //    if( vRet!=0 )
    //    {
    //        if(prtFlag==0)
    //        {
    //            writeLog(oErrMsg_p,
    //                "%s|%s|%d:load xml file failed vRet=[%d][%s]\n",
    //                __FILE__,__func__,__LINE__,vRet,doc.ErrorName());
    //        }
    //        return ERR_XML_LOAD;
    //    }

    const XMLElement* getElement_p = doc.FirstChildElement ( MC_L_TODO )->FirstChildElement (
                                     doFlagStr_p );

    if ( getElement_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s NULL error",
                   MC_L_TODO, doFlagStr_p );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    const XMLElement* ftpCycle_p = getElement_p->FirstChildElement ( MC_L_CYCLE );

    if ( ftpCycle_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s NULL error",
                   MC_L_TODO, doFlagStr_p, MC_L_CYCLE );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    const XMLElement* second_p = ftpCycle_p->FirstChildElement ( MC_L_SECOND );

    if ( second_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s->%s is  NULL error",
                   MC_L_TODO, doFlagStr_p, MC_L_CYCLE, MC_L_SECOND );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    const XMLElement* micsecond_p = ftpCycle_p->FirstChildElement ( MC_L_MICROSECOND );

    if ( second_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s->%s is  NULL error",
                   MC_L_TODO, doFlagStr_p, MC_L_CYCLE, MC_L_MICROSECOND );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    const XMLElement* repet_p = ftpCycle_p->FirstChildElement ( MC_L_REPET );

    if ( second_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s->%s is  NULL error",
                   MC_L_TODO, doFlagStr_p, MC_L_CYCLE, MC_L_REPET );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    second_p->QueryIntText ( &second );
    micsecond_p->QueryIntText ( &micSecond );
    repet_p->QueryIntText ( &repet );

    return 0;

}



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
                 const char* oErrMsg_p
               )
{
    char errMsg[MC_ERRMSG_MAX] = {};

    //char* xmlPth_p=NULL;
    if ( doFlagStr_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input params doFlagStr_p is NULL  error" );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_POINTER_NULL;
    }

    //d_printf("%s|%s|%d:inter func...doFlagStr_p=[%s]\n",__FILE__,__func__,__LINE__, doFlagStr_p);


    if ( strcmp ( doFlagStr_p, MC_L_GETDOWN ) != 0
         && strcmp ( doFlagStr_p, MC_L_PUTUP ) != 0
       )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input parameter doFlagStr_p value error doFlagStr_p=[%s]",
                   doFlagStr_p );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_INPUT_VALUE;

    }

    if ( serNumTh <= 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input error serNumTh=[%d]",
                   serNumTh );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_INPUT_VALUE;
    }

    int vRet = 0;

    //解析xml文件，格式如下
    /*
    <?xml version="1.0" encoding="UTF-8" standalone="no" ?>
    <!-- 气象下载器本地配置 -->
    <ToDo>
        <GetDown ftpnum="1">
        <FtpSer ip="192.168.0.154" port="21" user="Administrator" pwd="qwer1234" timeoutMS="32000">
            <ChkShName>/home/fczx/sh/getFserStas.sh</ChkShName>
            <DoShName>/home/fczx/sh/getFileFtp.sh</DoShName>
            <RemoteDir>/tmp</RemoteDir>
            <LocalDir>/home/fczx/tmp</LocalDir>
            <FileDes num="2">
                <FileName>"44_5m 227_m"</FileName>
            </FileDes>
        </FtpSer>
        </GetDown>
    </ToDo>
    */
    //    XMLDocument doc;
    //  vRet = doc.LoadFile( xmlPth_p );
    //    if( vRet!=0 )
    //    {
    //        if(prtFlag==0)
    //        {
    //            writeLog(oErrMsg_p,
    //                "%s|%s|%d:load xml file failed vRet=[%d][%s]\n",
    //                __FILE__,__func__,__LINE__,vRet,doc.ErrorName());
    //        }
    //        return ERR_XML_LOAD;
    //    }

    const XMLElement* getElement_p = doc.FirstChildElement ( MC_L_TODO )->FirstChildElement (
                                     doFlagStr_p );

    if ( getElement_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s NULL error",
                   MC_L_TODO, doFlagStr_p );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    int serNum = 0;
    vRet = getElement_p->QueryIntAttribute ( MC_L_FTPNUM, &serNum );

    if ( vRet != 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "QueryIntAttribute %s failed  vRet=[%d][%s]",
                   MC_L_FTPNUM, vRet, doc.ErrorIDToName ( ( XMLError ) vRet ) );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    if ( serNumTh > serNum )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "输入或配置错误  serNumTh=[%d],serNum=[%d]",
                   serNumTh, serNum );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_INPUT_OR_CONF;
    }


    const XMLElement* ftpser1Elemet_p = getElement_p->FirstChildElement ( MC_L_FTPSER );

    if ( ftpser1Elemet_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s NULL error",
                   MC_L_TODO, doFlagStr_p, MC_L_FTPSER );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    int tmpSerNum = 1;

    while ( tmpSerNum < serNumTh ) //找到需要读的ftp server节点
    {
        ftpser1Elemet_p = ftpser1Elemet_p->NextSiblingElement ( MC_L_FTPSER );

        if ( ftpser1Elemet_p == NULL )
        {

            snprintf ( errMsg, sizeof ( errMsg ),
                       "%s->%s the %dth %s is NULL",
                       MC_L_TODO, doFlagStr_p, tmpSerNum + 1, MC_L_FTPSER );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_XML_CONFIG;
        }

        tmpSerNum++;
    }

    const char* serverIP_p = ftpser1Elemet_p->Attribute ( MC_L_IP );

    if ( serverIP_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s Attribute ip is  NULL error",
                   MC_L_TODO, doFlagStr_p, MC_L_FTPSER );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    int portN = 0;
    vRet = ftpser1Elemet_p->QueryIntAttribute ( MC_L_PORT, &portN );

    if ( vRet != 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "QueryIntAttribute ftpnum failed  vRet=[%d][%s]",
                   vRet, doc.ErrorIDToName ( ( XMLError ) vRet ) );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    const char* userName_p = ftpser1Elemet_p->Attribute ( MC_L_USER );

    if ( userName_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s  Attribute user is  NULL error",
                   MC_L_TODO, doFlagStr_p, MC_L_FTPSER );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    const char* userPwd_p = ftpser1Elemet_p->Attribute ( MC_L_PWD );

    if ( userPwd_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s  Attribute pwd is  NULL error",
                   MC_L_TODO, doFlagStr_p, MC_L_FTPSER );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    int outTimeMs = 0;
    vRet = ftpser1Elemet_p->QueryIntAttribute ( MC_L_TIMEOUTMS, &outTimeMs );

    if ( vRet != 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "QueryIntAttribute %s failed  vRet=[%d][%s]",
                   MC_L_TIMEOUTMS, vRet, doc.ErrorIDToName ( ( XMLError ) vRet ) );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    int trsType = 0;
    vRet = ftpser1Elemet_p->QueryIntAttribute ( MC_L_TRSTYPE, &trsType );

    if ( vRet != 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "QueryIntAttribute %s failed  vRet=[%d][%s]",
                   MC_L_TRSTYPE, vRet, doc.ErrorIDToName ( ( XMLError ) vRet ) );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    int trsMode = 0;
    vRet = ftpser1Elemet_p->QueryIntAttribute ( MC_L_TRSMODE, &trsMode );

    if ( vRet != 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "QueryIntAttribute %s failed  vRet=[%d][%s]",
                   MC_L_TRSMODE, vRet, doc.ErrorIDToName ( ( XMLError ) vRet ) );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    const char* chkShName_p = ftpser1Elemet_p->FirstChildElement ( MC_L_CHKSHNAME )->GetText();

    if ( chkShName_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s->%s is  NULL error",
                   MC_L_TODO, doFlagStr_p, MC_L_FTPSER, MC_L_CHKSHNAME );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    const char* doShName_p = ftpser1Elemet_p->FirstChildElement ( MC_L_DOSHNAME )->GetText();

    if ( doShName_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s->%s is  NULL error",
                   MC_L_TODO, doFlagStr_p, MC_L_FTPSER, MC_L_DOSHNAME );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    const char* remoteDir_p = ftpser1Elemet_p->FirstChildElement ( MC_L_REMOTEDIR )->GetText();

    if ( remoteDir_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s->%s is  NULL error",
                   MC_L_TODO, doFlagStr_p, MC_L_FTPSER, MC_L_REMOTEDIR );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    const char* localDir_p = ftpser1Elemet_p->FirstChildElement ( MC_L_LOCALDIR )->GetText();

    if ( localDir_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s->%s is  NULL error",
                   MC_L_TODO, doFlagStr_p, MC_L_FTPSER, MC_L_LOCALDIR );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    const XMLElement* fileDesEle_p = ftpser1Elemet_p->FirstChildElement ( MC_L_FILEDES );

    if ( fileDesEle_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s->%s is  NULL error",
                   MC_L_TODO, doFlagStr_p, MC_L_FTPSER, MC_L_FILEDES );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    int fileNum = 0;
    vRet = fileDesEle_p->QueryIntAttribute ( MC_L_FNUM, &fileNum );

    if ( vRet != 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "QueryIntAttribute %s failed  vRet=[%d][%s]",
                   MC_L_FNUM, vRet, doc.ErrorIDToName ( ( XMLError ) vRet ) );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    const XMLElement* fileName_p = fileDesEle_p->FirstChildElement ( MC_L_FILENAME );

    if ( fileName_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s->%s->%s is  NULL error",
                   MC_L_TODO, doFlagStr_p, MC_L_FTPSER, MC_L_FILEDES, MC_L_FILENAME );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    const char* fileName1_p = fileName_p->GetText();

    if ( fileName1_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s->%s->%s GetText is  NULL error",
                   MC_L_TODO, doFlagStr_p, MC_L_FTPSER, MC_L_FILEDES, MC_L_FILENAME );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    /*
    fileName_p = fileName_p->NextSiblingElement("FileName");
    const char * fileName2 = fileName_p->GetText();
    if (fileName2==NULL)
    {
        return ERR_XML_CONFIG;
    }*/

    char* vDirVal_p = NULL;

    if ( strcmp ( doFlagStr_p, MC_L_GETDOWN ) == 0 )
    {
        vRet = getDoDirFromL ( doc, MC_L_SRCLIST, vDirVal_p, oErrMsg_p );

        if ( vRet != 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "getDoDirFromL [%s] return error vRet=[%d]",
                       MC_L_SRCLIST, vRet );
            outPutErrM ( oErrMsg_p, errMsg );

            return vRet;
        }
    }

    //打印输出日志
    //    d_printf("get from xml serNum=[%d]\n",serNum);
    //    d_printf("get from xml ip=[%s]\n",serverIP_p);
    //    d_printf("get from xml portN=[%d]\n",portN);
    //    d_printf("get from xml userName_p=[%s]\n",userName_p);
    //    d_printf("get from xml userPwd_p=[%s]\n",userPwd_p);
    //    d_printf("get from xml outTimeMs=[%d]\n",outTimeMs);
    //    d_printf("get from xml chkShName_p=[%s]\n",chkShName_p);
    //    d_printf("get from xml doShName_p=[%s]\n",doShName_p);
    //    d_printf("get from xml remoteDir_p=[%s]\n",remoteDir_p);
    //    d_printf("get from xml localDir_p=[%s]\n",localDir_p);
    //    d_printf("get from xml fileNum=[%d]\n",fileNum);
    //    d_printf("get from xml fileName1_p=[%s]\n",fileName1_p);
    //d_printf("get from xml fileName2=[%s]\n",fileName2);
    //    if(vDirVal_p!=NULL)
    //    {
    //        d_printf("get from xml vDirVal_p=[%s]\n",vDirVal_p);
    //    }


    //将解析出的数据赋于输出变量
    strncpy ( ftpParas.serverIP, serverIP_p, sizeof ( ftpParas.serverIP ) );
    ftpParas.portN = portN;
    strncpy ( ftpParas.userName, userName_p, sizeof ( ftpParas.userName ) );
    strncpy ( ftpParas.userPwd, userPwd_p, sizeof ( ftpParas.userPwd ) );
    strncpy ( ftpParas.remoteDir, remoteDir_p, sizeof ( ftpParas.remoteDir ) );
    strncpy ( ftpParas.localDir, localDir_p, sizeof ( ftpParas.localDir ) );
    strncpy ( ftpParas.fileNames, fileName1_p, sizeof ( ftpParas.fileNames ) );

    ftpParas.outTimeMs = outTimeMs;
    ftpParas.trsType = trsType;
    ftpParas.trsMode = trsMode;

    if ( vDirVal_p != NULL )
    {
        strncpy ( ftpParas.cmpDir, vDirVal_p, sizeof ( ftpParas.cmpDir ) );
    }

    strncpy ( shNames.chkShName, chkShName_p, sizeof ( shNames.chkShName ) );
    strncpy ( shNames.doShName, doShName_p, sizeof ( shNames.doShName ) );

    //    doc.Clear();
    return 0;

}




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
  *      20180308162247 修改函数,原executeShell改为调用system_plus
  *注：
  *      oErrMsg_p的分配空间须>=MC_ERRMSG_MAXH个字符，避免异常
*************************************/
int downFtpFiles ( const T_doFtpSh& shNames,
                   const T_ftpParas& ftpParas,
                   const char* oErrMsg_p,
                   const char* ftpCmd_p
                 )
{
    int  tRet    = 0;
    char errMsg[MC_ERRMSG_MAX];
    char tStr[1024];
    char tStrRet[100];

    memset ( &errMsg, 0, sizeof ( errMsg ) );
    memset ( tStr,    0, sizeof ( tStr ) );
    memset ( tStrRet, 0, sizeof ( tStrRet ) );

    int outTimeSeconds = 0;

    if ( ftpParas.outTimeMs > 1000 )
    {
        outTimeSeconds = ftpParas.outTimeMs / 1000;
    }
    else if ( ftpParas.outTimeMs > 0 )
    {
        outTimeSeconds = 1;
    }

    snprintf ( tStr, sizeof ( tStr ),
               "%s %d %d %s %s %s %s %s %s %d",
               shNames.doShName,
               ftpParas.trsType,
               ftpParas.trsMode,
               ftpParas.serverIP,
               ftpParas.userName,
               ftpParas.userPwd,
               ftpParas.remoteDir,
               ftpParas.localDir,
               ftpParas.fileNames,
               ftpParas.portN
             );

    tRet = system_plus ( tStr,
                         outTimeSeconds, /*outTimeSeconds秒之后将kill脚本*/
                         0,              /*超时后将尝试0次*/
                         1,              /*超时后将kill脚本及脚本派生的进程*/
                         tStrRet,
                         sizeof ( tStrRet )
                       );

    if ( tRet < 0 || tRet == 127 )
    {
        if ( tRet == -2 ) //超时
        {
            return ERR_FTP_OUTTIME;
        }
        else if ( tRet == -3 ) //超时后kill报错
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "system_plus [%s] kill after"
                       " the timeout error vRetMsg=[%s]",
                       shNames.doShName, tStrRet );
            outPutErrM ( oErrMsg_p, errMsg );
        }
        else
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "system_plus [%s] return error"
                       "vRetMsg=[%s]",
                       shNames.doShName, tStrRet );
            outPutErrM ( oErrMsg_p, errMsg );
        }

        return tRet;
    }

    if ( tRet != 0 )
    {
        return tRet;
    }

    //移动文件
    char vLocFuallName[MC_FULLN_LEN];
    char ttStr[MC_NAME_LEN * MC_ONECE_FNUM];
    char* p = NULL;

    memset ( vLocFuallName, 0, sizeof ( vLocFuallName ) );
    memset ( ttStr,         0, sizeof ( ttStr ) );

    strcpy ( ttStr, ftpParas.fileNames );
    Trim ( ttStr );

    if ( ttStr[0] == '\"'
         && strlen ( ftpParas.cmpDir ) > 0
       ) //说明文件名参数中包含多个文件名 且 以空格分隔
    {

        ttStr[strlen ( ttStr ) - 1] = 0;
        p = ttStr + 1;

        char* tt_pp[MC_ONECE_FNUM];
        memset ( tt_pp, 0, sizeof ( tt_pp ) );
        tRet = strToPrts ( p, " ", tt_pp );
        int fNums = tRet;

        if ( tRet < 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "strToPrts return error tRet=[%d]",
                       tRet );
            outPutErrM ( oErrMsg_p, errMsg );

            return tRet;
        }
        else if ( fNums > 0 )
        {
            for ( int i = 0; i < fNums; i++ )
            {
                Trim ( tt_pp[i] );

                if ( strlen ( tt_pp[i] ) != 0 && tt_pp[i] != NULL )
                {
                    memset ( vLocFuallName, 0, sizeof ( vLocFuallName ) );
                    snprintf ( vLocFuallName, sizeof ( vLocFuallName ),
                               "%s/%s",
                               ftpParas.localDir, tt_pp[i] );
                    int sysFlag = 1;//用shell的mv命令
                    tRet = mrename ( vLocFuallName,
                                     ftpParas.cmpDir,
                                     oErrMsg_p, sysFlag
                                   );

                    if ( tRet != 0 )
                    {
                        snprintf ( errMsg, sizeof ( errMsg ),
                                   "mrename sysFlag=[%d] [%s]->[%s]"
                                   "return error tRet=[%d]",
                                   sysFlag, vLocFuallName,
                                   ftpParas.cmpDir, tRet );
                        outPutErrM ( oErrMsg_p, errMsg );

                        return ERR_DIY_FUNCRET;
                    }
                }
            }
        }
    }
    else if ( strlen ( ftpParas.cmpDir ) > 0 )
    {
        snprintf ( vLocFuallName, sizeof ( vLocFuallName ),
                   "%s/%s",
                   ftpParas.localDir, ftpParas.fileNames );
        int sysFlag = 1;//用shell的mv命令
        tRet = mrename ( vLocFuallName,
                         ftpParas.cmpDir,
                         oErrMsg_p,
                         sysFlag
                       );

        if ( tRet != 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "mrename sysFlag=[%d] [%s]->[%s]"
                       "return error tRet=[%d]",
                       sysFlag, vLocFuallName,
                       ftpParas.cmpDir, tRet );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_DIY_FUNCRET;
        }
    }

    return 0;

}






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
                  const char* oErrMsg_p,
                  const char* ftpCmd_p
                )
{

    int  tRet    = 0;
    char errMsg[MC_ERRMSG_MAX];
    char tStr[1024];
    char tStrRet[100];

    memset ( &errMsg, 0, sizeof ( errMsg ) );
    memset ( tStr,    0, sizeof ( tStr ) );
    memset ( tStrRet, 0, sizeof ( tStrRet ) );

    int outTimeSeconds = 0;

    if ( ftpParas.outTimeMs > 1000 )
    {
        outTimeSeconds = ftpParas.outTimeMs / 1000;
    }
    else if ( ftpParas.outTimeMs > 0 )
    {
        outTimeSeconds = 1;
    }

    snprintf ( tStr, sizeof ( tStr ),
               "%s %d %d %s %s %s %s %s %s %d",
               shNames.chkShName,
               ftpParas.trsType,
               ftpParas.trsMode,
               ftpParas.serverIP,
               ftpParas.userName,
               ftpParas.userPwd,
               ftpParas.remoteDir,
               ftpParas.localDir,
               ftpParas.fileNames,
               ftpParas.portN
             );
    tRet = system_plus ( tStr,
                         outTimeSeconds, /*outTimeSeconds秒之后将kill脚本*/
                         0,              /*超时后将尝试0次*/
                         1,              /*超时后将kill脚本及脚本派生的进程*/
                         tStrRet,
                         sizeof ( tStrRet )
                       );

    if ( tRet < 0 || tRet == 127 )
    {
        if ( tRet == -2 ) //超时
        {
            return ERR_FTP_OUTTIME;
        }
        else if ( tRet == -3 ) //超时后kill报错
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "system_plus [%s] kill after"
                       " the timeout error vRetMsg=[%s]",
                       shNames.doShName, tStrRet );
            outPutErrM ( oErrMsg_p, errMsg );
        }
        else
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "system_plus [%s] return error"
                       "vRetMsg=[%s]",
                       shNames.doShName, tStrRet );
            outPutErrM ( oErrMsg_p, errMsg );
        }

        return tRet;
    }
    else if ( tRet == 1 ) //无法连接到ftp服务器
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "[%s] connet ftp server error",
                   shNames.chkShName );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_FTP_CONNECT;
    }
    else if ( tRet == 2 ) //用户名或密码错误
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "[%s] the ftp username or passwd error",
                   shNames.chkShName );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_FTP_USERPWD;
    }
    else if ( tRet == 3 ) //路径错误
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "[%s] the file path error",
                   shNames.chkShName );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_FTP_RPATH;
    }
    else if ( tRet == 4 ) //不存在要下载的文件
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "[%s] the file is not exists",
                   shNames.chkShName );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_FTP_RNOFILE;
    }
    else if ( tRet == 5 ) //本地路径错误
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "[%s] the local file path is error",
                   shNames.chkShName );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_FTP_LNOFILE;
    }

    snprintf ( tStr, sizeof ( tStr ),
               "%s %d %d %s %s %s %s %s %s %d",
               shNames.doShName,
               ftpParas.trsType,
               ftpParas.trsMode,
               ftpParas.serverIP,
               ftpParas.userName,
               ftpParas.userPwd,
               ftpParas.remoteDir,
               ftpParas.localDir,
               ftpParas.fileNames,
               ftpParas.portN
             );
    tRet = system_plus ( tStr,
                         outTimeSeconds, /*outTimeSeconds秒之后将kill脚本*/
                         0,              /*超时后将尝试0次*/
                         1,              /*超时后将kill脚本及脚本派生的进程*/
                         tStrRet,
                         sizeof ( tStrRet )
                       );

    if ( tRet < 0 || tRet == 127 )
    {
        if ( tRet == -2 ) //超时
        {
            return ERR_FTP_OUTTIME;
        }
        else if ( tRet == -3 ) //超时后kill报错
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "system_plus [%s] kill after"
                       " the timeout error vRetMsg=[%s]",
                       shNames.doShName, tStrRet );
            outPutErrM ( oErrMsg_p, errMsg );
        }
        else
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "system_plus [%s] return error"
                       "vRetMsg=[%s]",
                       shNames.doShName, tStrRet );
            outPutErrM ( oErrMsg_p, errMsg );
        }

        return tRet;
    }

    return 0;

}

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
             const int& loadXFlag,
             XMLDocument* doc_p
           )
{
    char errMsg[MC_ERRMSG_MAX];
    memset ( errMsg, 0, sizeof ( errMsg ) );

    if ( meteLogs_p == NULL
         || locaXmlName_p == NULL
       )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input parametes meteLogs_p or locaXmlName_p  is NULL error" );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_POINTER_NULL;
    }

    int vRet = 0;


    char* cfgLocal_p = NULL;
    cfgLocal_p = getenv ( MC_PLOCAL_XML ); //取得配置locaCfg.xml路径 的环境变量值

    if ( cfgLocal_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "getenv [%s] return error",
                   MC_PLOCAL_XML );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_POINTER_NULL;
    }

    //snprintf(g_localXmlName,sizeof(g_localXmlName),cfgLocal_p);
    snprintf ( locaXmlName_p, MC_FULLN_LEN, cfgLocal_p );


    if ( loadXFlag != 0 )
    {
        vRet = doc_p->LoadFile ( cfgLocal_p );

        if ( vRet != 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "load [%s] xml file failed vRet=[%d][%s]",
                       cfgLocal_p, vRet, doc_p->ErrorName() );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_XML_LOAD;
        }

        //加载日志路径配置
        vRet = loadLogCfg ( doc_p, meteLogs_p, oErrMsg_p );

        if ( vRet != 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "loadLogCfg return error vRet=[%d]\n%s",
                       vRet, oErrMsg_p );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_DIY_FUNCRET;
        }
    }
    else
    {
        XMLDocument doc;
        vRet = doc.LoadFile ( cfgLocal_p );

        if ( vRet != 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "load xml file failed vRet=[%d][%s]",
                       vRet, doc.ErrorName() );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_XML_LOAD;
        }

        //加载日志路径配置
        vRet = loadLogCfg ( &doc, meteLogs_p, oErrMsg_p );

        if ( vRet != 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "loadLogCfg return error vRet=[%d]\n%s",
                       vRet, oErrMsg_p );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_DIY_FUNCRET;
        }

        doc.Clear();
    }

    return 0;
}

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
                     const char* oErrMsg_p
                   )
{
    char errMsg[MC_ERRMSG_MAX] = {};

    if ( linkXmlEles.linkNum <= 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input error linkXmlEles.linkNum=[%d]",
                   linkXmlEles.linkNum );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_INPUT_VALUE;
    }

    const XMLElement* tEleName_p = NULL;

    if ( linkXmlEles.linkEles[0] == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input error linkXmlEles.linkEles[0] is NULL" );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_INPUT_VALUE;
    }

    tEleName_p = doc.FirstChildElement ( linkXmlEles.linkEles[0] );

    if ( tEleName_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "xml element[0] [%s] is NULL",
                   linkXmlEles.linkEles[0] );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_INPUT_VALUE;
    }

    //循环找到最后一个要找的节点
    for ( int i = 1; ( i < linkXmlEles.linkNum ) && ( tEleName_p != NULL ); i++ )
    {
        if ( linkXmlEles.linkEles[i] == NULL )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "input error linkXmlEles.linkEles[%d] is NULL", i );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_INPUT_VALUE;
        }

        tEleName_p = tEleName_p->FirstChildElement ( linkXmlEles.linkEles[i] );

        if ( tEleName_p == NULL )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "xml element[%d] [%s] is NULL",
                       i, linkXmlEles.linkEles[i] );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_INPUT_VALUE;
        }
    }

    //取节点的值
    const char* tValue_p = tEleName_p->GetText();

    eleVal_p = ( char* ) tValue_p;

    //d_printf("%s|%s|%d:  eleVal_p=[%s]\n",__FILE__,__func__,__LINE__,eleVal_p);

    return 0;
}

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
                    )
{
    char errMsg[MC_ERRMSG_MAX] = {};

    if ( linkXmlEles.linkNum <= 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input error linkXmlEles.linkNum=[%d]",
                   linkXmlEles.linkNum );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_INPUT_VALUE;
    }

    const XMLElement* tEleName_p = NULL;

    if ( linkXmlEles.linkEles[0] == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input error linkXmlEles.linkEles[0] is NULL" );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_INPUT_VALUE;
    }

    tEleName_p = doc.FirstChildElement ( linkXmlEles.linkEles[0] );

    if ( tEleName_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "xml element[0] [%s] is NULL",
                   linkXmlEles.linkEles[0] );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_INPUT_VALUE;
    }

    //循环找到最后一个要找的节点
    int tFindNum = 0;

    for ( int i = 1; ( i < linkXmlEles.linkNum ) && ( tEleName_p != NULL ); i++ )
    {
        if ( linkXmlEles.linkEles[i] == NULL )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "input error linkXmlEles.linkEles[%d] is NULL", i );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_INPUT_VALUE;
        }

        tEleName_p = tEleName_p->FirstChildElement ( linkXmlEles.linkEles[i] );

        if ( tEleName_p == NULL )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "xml element[%d] [%s] is NULL",
                       i, linkXmlEles.linkEles[i] );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_INPUT_VALUE;
        }

        tFindNum = i;
    }

    int tSeqNum = 1;

    while ( tSeqNum < seqNum ) //找到需要读的节点
    {
        tEleName_p = tEleName_p->NextSiblingElement ( linkXmlEles.linkEles[tFindNum] );

        if ( tEleName_p == NULL )
        {

            snprintf ( errMsg, sizeof ( errMsg ),
                       "xml element[%d] [%s] NO.[%d] seq is NULL",
                       tFindNum, linkXmlEles.linkEles[tFindNum], tSeqNum );
            outPutErrM ( oErrMsg_p, errMsg );
            return ERR_INPUT_VALUE;
        }

        tSeqNum++;
    }

    //取节点的值
    const char* tValue_p = tEleName_p->GetText();

    eleVal_p = ( char* ) tValue_p;

    //d_printf("%s|%s|%d:  eleVal_p=[%s]\n",__FILE__,__func__,__LINE__,eleVal_p);

    return 0;
}


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
                    const char* oErrMsg_p
                  )
{
    char errMsg[MC_ERRMSG_MAX] = {};

    const XMLElement* dolistElement_p = doc.FirstChildElement (
                                        MC_L_TODO )->FirstChildElement ( MC_L_DOLIST );

    if ( dolistElement_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "local xml cfg %s->%s is NULL",
                   MC_L_TODO, MC_L_DOLIST );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    //取需要处理的业务清单路径
    const char* srValue = dolistElement_p->FirstChildElement (
                          eleName_p )->GetText();

    if ( srValue == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "local xml cfg %s->%s->%s value is NULL",
                   MC_L_TODO, MC_L_DOLIST, eleName_p );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    dirVal_p = ( char* ) srValue;
    //d_printf("%s|%s|%d:  dirVal_p=[%s]\n",__FILE__,__func__,__LINE__, dirVal_p);

    return 0;
}



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
                     const char* oErrMsg_p
                   )
{
    char errMsg[MC_ERRMSG_MAX] = {};

    const XMLElement* winElement_p = doc.FirstChildElement (
                                     MC_L_TODO )->FirstChildElement ( MC_L_WINDMEASURE );

    if ( winElement_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "local xml cfg %s->%s is NULL",
                   MC_L_TODO, MC_L_WINDMEASURE );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    //配置是否存在从III区接入
    const char* tAccValue_p = winElement_p->FirstChildElement (
                              MC_L_ACCTHREE )->GetText();

    if ( tAccValue_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "local xml cfg %s->%s->%s value is NULL",
                   MC_L_TODO, MC_L_WINDMEASURE, MC_L_ACCTHREE );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    accVal_p = ( char* ) tAccValue_p;
    //d_printf("%s|%s|%d:  accVal_p=[%s]\n",__FILE__,__func__,__LINE__, accVal_p);

    return 0;
}

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
                      const char* oErrMsg_p
                    )
{
    char errMsg[MC_ERRMSG_MAX] = {};

    const char* coorValue_p = doc.FirstChildElement (
                              MC_FILE )->FirstChildElement ( MC_COORDINATE )->GetText();

    if ( coorValue_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s value is NULL",
                   MC_FILE, MC_COORDINATE );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }


    outVal_p = ( char* ) coorValue_p;

    //d_printf("%s|%s|%d:  outVal_p=[%s]\n",__FILE__,__func__,__LINE__, outVal_p);

    return 0;
}

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
                    const char* oErrMsg_p
                  )
{
    char errMsg[MC_ERRMSG_MAX] = {};

    const XMLElement* itdescElement_p = doc.FirstChildElement (
                                        MC_FILE )->FirstChildElement ( MC_JOBDESCLIST )->FirstChildElement ( MC_JOBITEMDESC );

    if ( itdescElement_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "%s->%s->%s is NULL",
                   MC_FILE, MC_JOBDESCLIST, MC_JOBITEMDESC );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }

    int tmpSerNum = 1;

    while ( tmpSerNum < seqNum ) //找到需要读的MC_JOBITEMDESC节点
    {
        itdescElement_p = itdescElement_p->NextSiblingElement ( MC_JOBITEMDESC );

        if ( itdescElement_p == NULL )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "%s->%s the %dth %s is NULL",
                       MC_FILE, MC_JOBDESCLIST, tmpSerNum + 1, MC_JOBITEMDESC );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_XML_CONFIG;
        }

        tmpSerNum++;
    }

    const char* jobIDValue_p = itdescElement_p->FirstChildElement ( MC_JOBID )->GetText();

    if ( jobIDValue_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "the %dth %s value is NULL",
                   seqNum, MC_JOBITEMDESC );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_CONFIG;
    }


    outVal_p = ( char* ) jobIDValue_p;

    //d_printf("%s|%s|%d:  outVal_p=[%s]\n",__FILE__,__func__,__LINE__, outVal_p);

    return 0;
}


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
                   const char* oErrMsg_p
                 )
{
    char errMsg[MC_ERRMSG_MAX] = {};

    if ( ym_p == NULL || doFlagStr == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input param ym_p or doFlagStr is NULL error" );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_POINTER_NULL;
    }

    char tBuf[100] = {};


    if ( strcmp ( doFlagStr, MC_L_DOSTAPATH ) == 0 )
    {
        snprintf ( tBuf, sizeof ( tBuf ), "%s %s", ym_p, MC_STA_COMMENT );
        doc.InsertEndChild ( doc.NewDeclaration() );
        doc.InsertEndChild ( doc.NewComment ( tBuf ) );
        XMLNode* file = doc.InsertEndChild ( doc.NewElement ( MC_STA_FILE ) );

        //fusk@20190307 add statDesc节点 begine---->
        char tmpTime[MC_TIMES_LEN] = {};
        getTime ( 4, tmpTime, NULL, oErrMsg_p ); //%Y-%m-%d:%H-%M-%S
        XMLElement* statDescE_p = doc.NewElement ( MC_STA_STADESC );

        XMLElement* retTimeE_p = doc.NewElement ( MC_STA_RETTIME );
        retTimeE_p->SetText ( tmpTime );
        XMLElement* retCodeE_p = doc.NewElement ( MC_STA_RETCODE );
        retCodeE_p->SetText ( "0" );
        XMLElement* retMsgE_p = doc.NewElement ( MC_STA_RETMSG );
        retMsgE_p->SetText ( "success" );

        statDescE_p->InsertEndChild ( retTimeE_p );
        statDescE_p->InsertEndChild ( retCodeE_p );
        statDescE_p->InsertEndChild ( retMsgE_p );

        file->InsertEndChild ( statDescE_p );
        //<---- fusk@20190307 add statDesc节点 end
        
        XMLNode* itemDesc = file->InsertEndChild ( doc.NewElement ( MC_STA_ITEMDESC ) );
        XMLNode* jobNumber = itemDesc->InsertEndChild ( doc.NewElement ( MC_STA_JOBNUMBER ) );
        jobNumber->InsertFirstChild ( doc.NewText ( "0" ) );


        file->InsertEndChild ( doc.NewElement ( MC_STA_ITEMLIST ) );
    }
    else if ( strcmp ( doFlagStr, MC_L_DOWPLANPATH ) == 0 )
    {
        snprintf ( tBuf, sizeof ( tBuf ), "%s %s", ym_p, MC_W_COMMENT );
        doc.InsertEndChild ( doc.NewDeclaration() );
        doc.InsertEndChild ( doc.NewComment ( tBuf ) );
        XMLNode* task = doc.InsertEndChild ( doc.NewElement ( MC_W_TASK ) );
        XMLNode* itemDesc = task->InsertEndChild ( doc.NewElement ( MC_W_ITEMDESC ) );
        XMLNode* allJobNum = itemDesc->InsertEndChild ( doc.NewElement ( MC_W_ALLJOBNUM ) );
        allJobNum->InsertFirstChild ( doc.NewText ( "0" ) );
        XMLNode* cmpJobNum = itemDesc->InsertEndChild ( doc.NewElement ( MC_W_COMPLETENUM ) );
        cmpJobNum->InsertFirstChild ( doc.NewText ( "0" ) );

        task->InsertEndChild ( doc.NewElement ( MC_W_ITEMLIST ) );
    }


    return 0;
}

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
                   const char* oErrMsg_p
                 )
{
    char errMsg[MC_ERRMSG_MAX] = {};

    if ( doFlagStr_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input param doFlagStr_p is NULL error" );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_POINTER_NULL;
    }

    if ( strcmp ( doFlagStr_p, MC_L_DOSTAPATH ) == 0 )
    {
        //获取 doc 中现有的 jobNumber 值 并在现有值的基础之上+1
        int tNum = 0;
        doc.FirstChildElement ( MC_STA_FILE )->FirstChildElement (
        MC_STA_ITEMDESC )->FirstChildElement ( MC_STA_JOBNUMBER )->QueryIntText ( &tNum );
        tNum++;
        //printf("%s|%s|%d:  tNum[%d]\n",__FILE__,__func__,__LINE__, tNum);
        char buf[20] = {};
        snprintf ( buf, sizeof ( buf ), "%d", tNum );

        //根据UxmlLItem.xmlStatItem拼XMLElement
        XMLElement* itemData = doc.NewElement ( MC_STA_ITEMDATA );

        XMLElement* name = doc.NewElement ( MC_STA_NAME );
        name->SetText ( UxmlLItem.xmlStatItem.fileName );

        XMLElement* commitTime = doc.NewElement ( MC_STA_COMMITTIME );
        commitTime->SetText ( UxmlLItem.xmlStatItem.cmtTime );

        itemData->InsertEndChild ( name );
        itemData->InsertEndChild ( commitTime );

        //将拼装后的itemData插入xml
        doc.FirstChildElement ( MC_STA_FILE )->FirstChildElement (
        MC_STA_ITEMLIST )->InsertEndChild ( itemData );
        //将修改后的jobNumberp写入xml
        doc.FirstChildElement ( MC_STA_FILE )->FirstChildElement (
        MC_STA_ITEMDESC )->FirstChildElement ( MC_STA_JOBNUMBER )->SetText ( buf );

        XMLElement* statDescE_p = doc.FirstChildElement( MC_STA_FILE )->FirstChildElement(
                MC_STA_STADESC );
        if ( statDescE_p != NULL )
        {
            XMLElement* retCodeE_p = statDescE_p->FirstChildElement( MC_STA_RETCODE );
            if ( retCodeE_p != NULL )
            {
                retCodeE_p->SetText ( "0" );
            }
            XMLElement* retMsgE_p = statDescE_p->FirstChildElement( MC_STA_RETMSG );
            if ( retMsgE_p != NULL )
            {
                retMsgE_p->SetText ( "success" );
            }
            XMLElement* retTimeE_p = statDescE_p->FirstChildElement( MC_STA_RETTIME );
            if ( retTimeE_p != NULL )
            {
                char tmpTime[MC_TIMES_LEN] = {};
                getTime ( 4, tmpTime, NULL, oErrMsg_p ); //%Y-%m-%d:%H-%M-%S
                retTimeE_p->SetText ( tmpTime );
            }
        }

    }
    else if ( strcmp ( doFlagStr_p, MC_L_DOWPLANPATH ) == 0 )
    {
        //获取 doc 中现有的 allJobNum 值 并在现有值的基础之上+1
        int tAllNum = 0;
        doc.FirstChildElement ( MC_W_TASK )->FirstChildElement (
        MC_W_ITEMDESC )->FirstChildElement ( MC_W_ALLJOBNUM )->QueryIntText ( &tAllNum );
        tAllNum++;
        char buf[20] = {};
        snprintf ( buf, sizeof ( buf ), "%d", tAllNum );

        //根据UxmlLItem.xmlStatItem拼XMLElement
        XMLElement* itemData = doc.NewElement ( MC_W_ITEMDATA );
        itemData->SetAttribute ( MC_W_COMPLETEFLAG, 0 );

        XMLElement* busiName = doc.NewElement ( MC_W_BUSLSTNAME );
        busiName->SetText ( UxmlLItem.xmlCWItem.busLstName );

        XMLElement* winName = doc.NewElement ( MC_W_WINDNAME );

        XMLElement* commitTime = doc.NewElement ( MC_W_COMMITTIME );
        commitTime->SetText ( UxmlLItem.xmlCWItem.cmtTime );

        itemData->InsertEndChild ( busiName );
        itemData->InsertEndChild ( winName );
        itemData->InsertEndChild ( commitTime );

        //将拼装后的itemData插入xml
        doc.FirstChildElement ( MC_W_TASK )->FirstChildElement ( MC_W_ITEMLIST )->InsertEndChild (
        itemData );
        //将修改后的jobNumberp写入xml
        doc.FirstChildElement ( MC_W_TASK )->FirstChildElement (
        MC_W_ITEMDESC )->FirstChildElement ( MC_W_ALLJOBNUM )->SetText ( buf );

    }

    return 0;
}


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
                    const char* oErrMsg_p
                  )
{
    char errMsg[MC_ERRMSG_MAX] = {};

    if ( doFlagStr_p == NULL || fileEleName_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input params doFlagStr_p or fileEleName_p is NULL" );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_POINTER_NULL;
    }

    int vRet = 0;
    char tNameEle[MC_XMLELE_LEM] = {};

    if ( strcmp ( doFlagStr_p, MC_L_DOSTAPATH ) == 0 )
    {

        snprintf ( tNameEle, sizeof ( tNameEle ), MC_L_DOSTANAME );
        /*
        <?xml version="1.0" encoding="UTF-8" standalone="no" ?>
        <!-- 20170713 业务清单提供情况 -->
        <file>
            <itemDesc>
                <jobNumber>1</jobNumber>
            </itemDesc>
            <itemList>
                <itemData>
                    <name>111111</name>
                    <commitTime>2017-07-10:16-00-00</commitTime>
                </itemData>
            </itemList>
        </file>
        */
    }
    else if ( strcmp ( doFlagStr_p, MC_L_DOWPLANPATH ) == 0 )
    {
        snprintf ( tNameEle, sizeof ( tNameEle ), MC_L_DOWPLANNAME );
        //1.文件格式?
        /*
        <?xml version="1.0" encoding="UTF-8"?>
        <!--20170714 wind measurement mast processing plan-->
        <task>
            <itemDesc>
                <allJobNum>1</allJobNumber>
                <completeNum>1</completeNum>
            </itemDesc>
            <itemList>
                <itemData completeFlag="1">
                    <busLstName>haha</busLstName>
                    <windName>dddd</windName>
                    <commitTime>2017-07-10:16-00-00</commitTime>
                </itemData>
            </itemList>
        </task>
        */
    }
    else
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input error doFlagStr_p=[%s]",
                   doFlagStr_p );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_INPUT_VALUE;
    }


    char tFullName[MC_FULLN_LEN] = {};//带路径名的全文件名

    //1.加载本地配置xml
    //    XMLDocument docLocal;
    //    vRet = docLocal.LoadFile( locaXmlCfm_p);
    //    if( vRet!=0 )
    //    {
    //        if(prtFlag==0)
    //        {
    //            writeLog(oErrMsg_p,
    //                "%s|%s|%d:load [%s] xml file failed vRet=[%d][%s]\n",
    //                __FILE__,__func__,__LINE__,locaXmlCfm_p,vRet,docLocal.ErrorName());
    //        }
    //        return ERR_XML_LOAD;
    //    }

    //取得配置的路径
    char* srValue = NULL;
    vRet = getDoDirFromL ( docLocal, doFlagStr_p, srValue, oErrMsg_p );

    if ( vRet != 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "getDoDirFromL return vRet[%d]",
                   vRet );
        outPutErrM ( oErrMsg_p, errMsg );

        return vRet;
    }

    char* tName = NULL;
    vRet = getDoDirFromL ( docLocal, tNameEle, tName, oErrMsg_p );

    if ( vRet != 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "getDoDirFromL return vRet[%d]",
                   vRet );
        outPutErrM ( oErrMsg_p, errMsg );

        return vRet;
    }

    char tmpTime[MC_TIMES_LEN] = {};
    getTime ( 3, tmpTime, NULL, oErrMsg_p );

    snprintf ( tFullName, sizeof ( tFullName ), "%s/%s%s.xml", srValue, tName, tmpTime );

    XMLDocument toDoc;
    vRet = fileExists ( tFullName, oErrMsg_p );

    if ( vRet < 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "fileExists [%s] return error vRet=[%d]\n%s",
                   tFullName, vRet, oErrMsg_p );
        outPutErrM ( oErrMsg_p, errMsg );

        return vRet;
    }
    else if ( vRet == 1 ) //文件不存在则需要先构建文件头
    {
        putXmlHeadPs ( toDoc, tmpTime, doFlagStr_p, oErrMsg_p );
    }
    else //文件已经存在 加载xml文件
    {
        vRet = toDoc.LoadFile ( tFullName );

        if ( vRet != 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "load xml file [%s] failed vRet=[%d][%s]",
                       tFullName, vRet, toDoc.ErrorName() );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_XML_LOAD;
        }
    }

    //实现是接到消息后，用消息里的名称生成文件名和时间
    T_UxmlLItem UxmlLItemTmp = {};
    long tmpSec = 0;
    char bListFullName[MC_FULLN_LEN] = {};

    if ( strcmp ( doFlagStr_p, MC_L_DOSTAPATH ) == 0 )
    {
        T_xmlStatItem xmlStatItem = {};

        //取文件的修改时间
        char* dirVal_p = NULL;
        vRet = getDoDirFromL ( docLocal, MC_L_STALISTRIG, dirVal_p, oErrMsg_p );

        if ( vRet != 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "getDoDirFromL [%s] return error vRet=[%d]",
                       MC_L_STALISTRIG, vRet );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_DIY_FUNCRET;
        }

        snprintf ( bListFullName, sizeof ( bListFullName ), "%s/%s", dirVal_p, fileEleName_p );
        vRet = getSubFSec ( bListFullName, 1, tmpSec, xmlStatItem.cmtTime, oErrMsg_p, 0 );

        if ( vRet != 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "getSubFSec [%s] return error vRet=[%d]",
                       bListFullName, vRet );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_DIY_FUNCRET;
        }


        strcpy ( xmlStatItem.fileName, fileEleName_p );
        //strcpy(xmlStatItem.cmtTime,"2017-07-10:16-00-00");
        UxmlLItemTmp.xmlStatItem = xmlStatItem;
    }
    else if ( strcmp ( doFlagStr_p, MC_L_DOWPLANPATH ) == 0 )
    {
        T_xmlCWItem xmlCWItem = {};

        //取文件的修改时间
        char* dirVal_p = NULL;
        vRet = getDoDirFromL ( docLocal, MC_L_DOWPLANPATH, dirVal_p, oErrMsg_p );

        if ( vRet != 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "getDoDirFromL [%s] return error vRet=[%d]",
                       MC_L_DOWPLANPATH, vRet );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_DIY_FUNCRET;

        }

        snprintf ( bListFullName, sizeof ( bListFullName ), "%s/%s", dirVal_p, fileEleName_p );
        vRet = getSubFSec ( bListFullName, 1, tmpSec, xmlCWItem.cmtTime, oErrMsg_p, 0 );

        if ( vRet != 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "getSubFSec [%s] return error vRet=[%d]",
                       bListFullName, vRet );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_DIY_FUNCRET;

        }

        strcpy ( xmlCWItem.busLstName, fileEleName_p );
        //strcpy(xmlCWItem.cmtTime,"2017-07-15:09-00-00");
        UxmlLItemTmp.xmlCWItem = xmlCWItem;
    }


    //把信息追加到xml中
    putXmlitemPs ( toDoc, UxmlLItemTmp, doFlagStr_p, oErrMsg_p );

    //toDoc.Print();

    toDoc.SaveFile ( tFullName );

    //文件生成完毕后需要分情况处理:cp 或 mv 到下一目录,以便触发下一段的工作
    char vXmlEleCmpPath[MC_XMLELE_LEM] = {};

    if ( strcmp ( doFlagStr_p, MC_L_DOSTAPATH ) == 0 )
    {
        //业务清单提供情况生成完毕后 下一处理目录是 反隔摆渡文件目录
        snprintf ( vXmlEleCmpPath, sizeof ( vXmlEleCmpPath ), MC_L_TOANTI );
    }
    else if ( strcmp ( doFlagStr_p, MC_L_DOWPLANPATH ) == 0 )
    {
        //业务清单提供情况生成完毕后 下一处理目录是 测风数据接收触发目录
        snprintf ( vXmlEleCmpPath, sizeof ( vXmlEleCmpPath ), MC_L_CMPWPLANPATH );
    }

    //读取本地配置 取目录值
    char*  nxtPath_p = NULL;
    vRet = getDoDirFromL ( docLocal, vXmlEleCmpPath, nxtPath_p, oErrMsg_p );

    if ( vRet != 0 )
    {

        snprintf ( errMsg, sizeof ( errMsg ),
                   "getDoDirFromL return vRet[%d]",
                   vRet );
        outPutErrM ( oErrMsg_p, errMsg );

        return vRet;
    }

    //读取反隔备目录
    char* riBakPath_p = NULL;
    char  noRiBakFlag = 0;
    vRet = getDoDirFromL ( docLocal, MC_L_TOANTIBAK, riBakPath_p, oErrMsg_p );

    if ( vRet != 0 )
    {
        noRiBakFlag = 1; //没有备目录
        /*
        snprintf ( errMsg, sizeof ( errMsg ),
                   "getDoDirFromL [%s] return vRet[%d]",
                   MC_L_TOANTIBAK,vRet );
        outPutErrM ( oErrMsg_p, errMsg );

        return vRet;
        */
    }
    //移动或考贝生成的文件
    int sysFlag = 0;//是否用shell命令操作标识  0:不用

    if ( strcmp ( doFlagStr_p, MC_L_DOSTAPATH ) == 0 )
    {
        vRet = copyFile ( tFullName, nxtPath_p, oErrMsg_p, sysFlag );

        //vRet = mrename(tFullName,nxtPath_p,oErrMsg_p,sysFlag);
        if ( vRet != 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "[%s]->[%s] sysFlag=[%d] copyFile  return error vRet=[%d]",
                       tFullName, nxtPath_p, sysFlag, vRet );
            outPutErrM ( oErrMsg_p, errMsg );

            return vRet;
        }

        if ( noRiBakFlag == 0 && riBakPath_p != NULL )
        {
            vRet = copyFile ( tFullName, riBakPath_p, oErrMsg_p, sysFlag );
            if ( vRet != 0 )
            {
                snprintf ( errMsg, sizeof ( errMsg ),
                           "[%s]->[%s] sysFlag=[%d] copyFile  return error vRet=[%d]",
                           tFullName, riBakPath_p, sysFlag, vRet );
                outPutErrM ( oErrMsg_p, errMsg );

                return vRet;
            }
        }
    }
    else if ( strcmp ( doFlagStr_p, MC_L_DOWPLANPATH ) == 0 )
    {
        //如下一目录存在生存的文件名则考贝 否则 进行移动
        //modify by fusk @ 20170803143624 暂改为 copy 则不进行 mv
        char tNxtFullName[MC_FULLN_LEN] = {};
        snprintf ( tNxtFullName, sizeof ( tNxtFullName ), "%s/%s%s.xml", nxtPath_p, tName,
                   tmpTime );
        vRet = copyFile ( tFullName, nxtPath_p, oErrMsg_p, sysFlag );

        if ( vRet != 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "[%s]->[%s] sysFlag=[%d] copyFile  return error vRet=[%d]",
                       tFullName, nxtPath_p, sysFlag, vRet );
            outPutErrM ( oErrMsg_p, errMsg );

            return vRet;
        }

        //        vRet = fileExists(tNxtFullName,oErrMsg_p,1);
        //        if(vRet==0)//文件存在 cp文件
        //        {
        //            vRet = copyFile(tFullName,nxtPath_p,oErrMsg_p,sysFlag);
        //            //vRet = mrename(tFullName,nxtPath_p,oErrMsg_p,sysFlag);
        //            if (vRet!=0)
        //            {
        //                if(prtFlag==0)
        //                {
        //                    writeLog(oErrMsg_p,
        //                        "%s|%s|%d:[%s]->[%s] sysFlag=[%d] copyFile  return error vRet=[%d]\n",
        //                        __FILE__,__func__,__LINE__,tFullName,nxtPath_p,sysFlag, vRet);

        //                }

        //                return vRet;
        //            }
        //        }
        //        else //文件不存在 mv文件
        //        {
        //            //vRet = copyFile(tFullName,nxtPath_p,oErrMsg_p,sysFlag);
        //            vRet = mrename(tFullName,nxtPath_p,oErrMsg_p,sysFlag);
        //            if (vRet!=0)
        //            {
        //                if(prtFlag==0)
        //                {
        //                    writeLog(oErrMsg_p,
        //                        "%s|%s|%d:[%s]->[%s] sysFlag=[%d] mrename  return error vRet=[%d]\n",
        //                        __FILE__,__func__,__LINE__,tFullName,nxtPath_p,sysFlag, vRet);

        //                }

        //                return vRet;
        //            }
        //        }

    }

    //copy 业务清单到备目录
    if ( noRiBakFlag == 0 && riBakPath_p != NULL )
    {
        vRet = copyFile ( bListFullName, riBakPath_p, oErrMsg_p, sysFlag );
        if ( vRet != 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "[%s]->[%s] sysFlag=[%d] copyFile  return error vRet=[%d]",
                       bListFullName, riBakPath_p, sysFlag, vRet );
            outPutErrM ( oErrMsg_p, errMsg );

            return vRet;
        }
    }
    //需要把 业务清单文件一并移到下一目录
    vRet = mrename ( bListFullName, nxtPath_p, oErrMsg_p, sysFlag );

    if ( vRet != 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "[%s]->[%s] sysFlag=[%d] mrename  return error vRet=[%d]",
                   bListFullName, nxtPath_p, sysFlag, vRet );
        outPutErrM ( oErrMsg_p, errMsg );

        return vRet;
    }


    //docLocal.Clear();
    toDoc.Clear();

    return 0;
}


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
                 const char* oErrMsg_p
               )
{
    char errMsg[MC_ERRMSG_MAX] = {};

    if ( srcXml_p == NULL
         || mergeXml_p == NULL
         || doFlagStr_p == NULL
       )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input params srcXml_p or mergeXml_p doFlagStr_p is NULL" );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_POINTER_NULL;
    }

    int vRet = 0;
    vRet = fileExists ( srcXml_p, oErrMsg_p );

    if ( vRet != 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "fileExists [%s] return error vRet[%d]",
                   srcXml_p, vRet );
        outPutErrM ( oErrMsg_p, errMsg );

        return vRet;
    }

    vRet = fileExists ( mergeXml_p, oErrMsg_p );

    if ( vRet == 1 ) //文件不存在
    {
        //直接把 srcXml_p rename到 mergeXml_p
        int sysFlag = 0;
        vRet = mrename ( srcXml_p, mergeXml_p, oErrMsg_p, sysFlag );

        if ( vRet != 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "sysFlag=[%d] mrename  [%s] to [%s] return error vRet[%d]",
                       sysFlag, srcXml_p, mergeXml_p, vRet );
            outPutErrM ( oErrMsg_p, errMsg );

            return vRet;
        }

        return 0;
    }

    XMLDocument srcDoc;
    vRet = srcDoc.LoadFile ( srcXml_p );

    if ( vRet != 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "load xml file [%s]failed vRet=[%d][%s]",
                   srcXml_p, vRet, srcDoc.ErrorName() );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_LOAD;
    }

    XMLDocument merDoc;
    vRet = merDoc.LoadFile ( mergeXml_p );

    if ( vRet != 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "load xml file [%s]failed vRet=[%d][%s]",
                   mergeXml_p, vRet, merDoc.ErrorName() );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_LOAD;
    }


    if ( strcmp ( doFlagStr_p, MC_L_DOWPLANPATH ) == 0 ) //III区测风塔采集业务处理计划xml
    {
        int tsrcItNum = 0;
        int tAllMerItNum = 0;

        //取merg xml 的 MC_W_ALLJOBNUM
        merDoc.FirstChildElement ( MC_W_TASK )->FirstChildElement (
        MC_W_ITEMDESC )->FirstChildElement (
        MC_W_ALLJOBNUM )->QueryIntText ( &tAllMerItNum );

        XMLNode* tMergItemList_p = merDoc.FirstChildElement (
                                   MC_W_TASK )->FirstChildElement ( MC_W_ITEMLIST );

        if ( tMergItemList_p == NULL )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "xml file[%s] %s->%s is NULL",
                       mergeXml_p, MC_W_TASK, MC_W_ITEMLIST );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_XML_CONFIG;
        }

        XMLNode* srcItemData_p = srcDoc.FirstChildElement (
                                 MC_W_TASK )->FirstChildElement (
                                 MC_W_ITEMLIST )->FirstChildElement ( MC_W_ITEMDATA );

        while ( srcItemData_p != NULL )
        {
            tMergItemList_p->InsertEndChild ( srcItemData_p->DeepClone ( &merDoc ) );
            tsrcItNum++;
            srcItemData_p = srcItemData_p->NextSiblingElement ( MC_W_ITEMDATA );
        }

        if ( tsrcItNum > 0 )
        {
            tAllMerItNum = tAllMerItNum + tsrcItNum;
            //将修改后的jobNumberp写入xml
            merDoc.FirstChildElement ( MC_W_TASK )->FirstChildElement (
            MC_W_ITEMDESC )->FirstChildElement ( MC_W_ALLJOBNUM )->SetText ( tAllMerItNum );
        }
    }

    //merDoc.Print();
    merDoc.SaveFile ( mergeXml_p );

    merDoc.Clear();
    srcDoc.Clear();

    //删除已经合并完的xml文件
    int sysFlag = 0;
    vRet = mdelete ( srcXml_p, oErrMsg_p, sysFlag );

    if ( vRet != 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "mdelete sysFlag=[%d] file[%s] return error vRet=[%d]",
                   sysFlag, srcXml_p, vRet );
        outPutErrM ( oErrMsg_p, errMsg );

        return vRet;
    }

    return 0;
}


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
                  const char* oErrMsg_p
                )
{
    char errMsg[MC_ERRMSG_MAX];
    memset ( errMsg, 0, sizeof ( errMsg ) );

    if ( filName_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input param filName_p is NULL error" );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_POINTER_NULL;
    }

    //    printf("%s|%s|%d:  filName_p[%s]\n",__FILE__,__func__,__LINE__, filName_p);

    int vRet = 0;

    //1.加载本地配置xml
    //    XMLDocument docLocal;
    //    vRet = docLocal.LoadFile( localXmlName_p);
    //    if( vRet!=0 )
    //    {
    //        if(prtFlag==0)
    //        {
    //            writeLog(oErrMsg_p,
    //                "%s|%s|%d:load xml file failed vRet=[%d][%s]\n",
    //                __FILE__,__func__,__LINE__,vRet,docLocal.ErrorName());
    //        }
    //        return ERR_XML_LOAD;
    //    }

    //1.1取业务清单处理的配置路径
    char* busiPath_p = NULL;
    vRet = getDoDirFromL ( docLocal, MC_L_SRCLIST, busiPath_p, oErrMsg_p );

    if ( vRet != 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "getDoDirFromL return vRet[%d]", vRet );
        outPutErrM ( oErrMsg_p, errMsg );

        return vRet;
    }

    //d_printf("%s|%s|%d:  vRet=[%d],busiPath_p=[%s]\n",__FILE__,__func__,__LINE__,vRet, busiPath_p);

    //1.2 取测风塔接入配置
    char* accValue = NULL;
    vRet = getAccWinFromL ( docLocal, accValue, oErrMsg_p );

    if ( vRet != 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "getAccWinFromL return vRet[%d]", vRet );
        outPutErrM ( oErrMsg_p, errMsg );

        return vRet;
    }

    //d_printf("%s|%s|%d:  accValue=[%s]\n",__FILE__,__func__,__LINE__, accValue);


    //2.加载业务清单xml
    XMLDocument docList;
    char fullBusiName[MC_FULLN_LEN] = {};
    snprintf ( fullBusiName, sizeof ( fullBusiName ), "%s/%s", busiPath_p, filName_p );
    vRet = docList.LoadFile ( fullBusiName );

    if ( vRet != 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "load xml file failed vRet=[%d][%s]",
                   vRet, docList.ErrorName() );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_LOAD;
    }

    //2.1取多机协同编码
    char* coorValue = NULL;
    vRet = getCoordIdFromB ( docList, coorValue, oErrMsg_p );

    if ( vRet != 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "getCoordIdFromB return vRet[%d]",
                   vRet );
        outPutErrM ( oErrMsg_p, errMsg );

        return vRet;
    }

    //d_printf("%s|%s|%d:  coorValue=[%s]\n",__FILE__,__func__,__LINE__, coorValue);

    //2.2取业务编码
    //jobIDValue 为 第二位为3则表示 测风数据从3区接入
    char* jobIDValue = NULL;
    vRet = getJobIdFromB ( docList, 1, jobIDValue, oErrMsg_p );

    if ( vRet != 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "getJobIdFromB return vRet[%d]",
                   vRet );
        outPutErrM ( oErrMsg_p, errMsg );

        return vRet;
    }

    //d_printf("%s|%s|%d:  jobIDValue=[%s]\n",__FILE__,__func__,__LINE__, jobIDValue);


    //根据多机协同编码 进行处理业务清单
    if ( strncmp ( coorValue, MC_COORD_MASTER, 3 ) == 0 ) // 主 目前只有此种情况
    {

        //2.根据业务清单的业务编码与本地配置判断
        //  生成III区测风塔采集业务处理计划

        if ( ( strncmp ( accValue, "1", 1 ) == 0 ) && ( strncmp ( jobIDValue + 1, "3", 1 ) == 0 ) )
        {
            //把业务清单文件移动到 采集计划生成目录
            char* dowPlanPath_p = NULL;
            vRet = getDoDirFromL ( docLocal,
                                   MC_L_DOWPLANPATH,
                                   dowPlanPath_p,
                                   oErrMsg_p );

            if ( vRet != 0 )
            {
                snprintf ( errMsg, sizeof ( errMsg ),
                           "getDoDirFromL [%s] return vRet[%d]",
                           MC_L_DOWPLANPATH, vRet );
                outPutErrM ( oErrMsg_p, errMsg );

                return vRet;
            }

            int sysFlag = 0;
            vRet = mrename ( fullBusiName,
                             dowPlanPath_p,
                             oErrMsg_p,
                             sysFlag );

            if ( vRet != 0 )
            {
                snprintf ( errMsg, sizeof ( errMsg ),
                           "mrename [%s]->[%s]return error vRet=[%d]",
                           fullBusiName, dowPlanPath_p, vRet );
                outPutErrM ( oErrMsg_p, errMsg );

                return vRet;
            }

            //需要生成采集业务处理计划 完成后移至完成目录 触发下一任务
            vRet = putXmlLFilePs ( docLocal, MC_L_DOWPLANPATH, filName_p, oErrMsg_p );

            if ( vRet != 0 )
            {
                snprintf ( errMsg, sizeof ( errMsg ),
                           "putXmlLFilePs return error vRet[%d]",
                           vRet );
                outPutErrM ( oErrMsg_p, errMsg );

                return vRet;
            }

        }
        else//直接把文件放到业务清单提供情况触发目录
        {
            //取业务清单提供情况触发目录
            char* triStatDir = NULL;
            vRet = getDoDirFromL ( docLocal, MC_L_STALISTRIG, triStatDir, oErrMsg_p );

            if ( vRet != 0 )
            {
                snprintf ( errMsg, sizeof ( errMsg ),
                           "getDoDirFromL return error vRet=[%d]",
                           vRet );
                outPutErrM ( oErrMsg_p, errMsg );

                return vRet;
            }

            //d_printf("%s|%s|%d:  triStatDir=[%s]\n",__FILE__,__func__,__LINE__, triStatDir);
            //将业务清单转移到业务清单提供情况触发目录
            int sysFlag = 0;
            //vRet = copyFile(fullBusiName,triStatDir,oErrMsg_p,sysFlag);
            vRet = mrename ( fullBusiName, triStatDir, oErrMsg_p, sysFlag );

            if ( vRet != 0 )
            {
                snprintf ( errMsg, sizeof ( errMsg ),
                           "copyFile [%s]->[%s] return error vRet=[%d]",
                           fullBusiName, triStatDir, vRet );
                outPutErrM ( oErrMsg_p, errMsg );

                return vRet;
            }

            //d_printf("%s|%s|%d:  copyFile vRet=[%d]\n",__FILE__,__func__,__LINE__, vRet);

        }

    }
    else if ( strncmp ( coorValue, MC_COORD_SLAVES, 3 ) == 0 ) //备
    {

    }
    else if ( strncmp ( coorValue, MC_COORD_INTEG, 3 ) == 0 ) //综合
    {

    }
    else if ( strncmp ( coorValue, MC_COORD_PARAL, 3 ) == 0 ) //并行
    {

    }
    else
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "xml element [%s] value[%s] error",
                   MC_COORDINATE, coorValue );
        outPutErrM ( oErrMsg_p, errMsg );

        return ERR_XML_VALUE;
    }

    //docLocal.Clear();
    docList.Clear();

    //    printf("%s|%s|%d:  vRet=[%d]\n",__FILE__,__func__,__LINE__, vRet);

    return 0;
}


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
               const char* oErrMsg_p
             )
{
    char errMsg[MC_ERRMSG_MAX];
    memset ( errMsg, 0, sizeof ( errMsg ) );

    int vRet = 0;
    //1.文件格式?
    /*
    <?xml version="1.0" encoding="UTF-8" standalone="no" ?>
    <!-- 20170713 业务清单提供情况 -->
    <file>
        <itemDesc>
            <jobNumber>1</jobNumber>
        </itemDesc>
        <itemList>
            <itemData>
                <name>111111</name>
                <commitTime>2017-07-10:16-00-00</commitTime>
            </itemData>
        </itemList>
    </file>
    */
    //生成业务清单提供情况并 并把业务清单移至 摆渡目录
    vRet = putXmlLFilePs ( docLocal, MC_L_DOSTAPATH, fileNmae_p, oErrMsg_p );

    if ( vRet != 0 )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "putXmlLFilePs return error vRet=[%d]",
                   vRet );
        outPutErrM ( oErrMsg_p, errMsg );

        return vRet;
    }

    //    printf("%s|%s|%d:  vRet=[%d]\n",__FILE__,__func__,__LINE__, vRet);

    return 0;

}




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
                   const char* oErrMsg_p
                 )
{
    char errMsg[MC_ERRMSG_MAX] = {};

    if ( statDescItem_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input param statDescItem_p is NULL error" );
        outPutErrM ( oErrMsg_p, errMsg );
        return ERR_POINTER_NULL;
    }

    if ( doc_p == NULL && docPath_p == NULL )
    {
        snprintf ( errMsg, sizeof ( errMsg ),
                   "input param docPath_p and doc_p is NULL error" );
        outPutErrM ( oErrMsg_p, errMsg );
        return ERR_POINTER_NULL;
    }

    int vRet = 0;
    XMLDocument toDoc;
    XMLDocument* toDoc_p = NULL;

    if ( doc_p == NULL ) //从路径中加载xml文件
    {
        vRet = fileExists ( docPath_p, oErrMsg_p );

        if ( vRet < 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "fileExists [%s] return error vRet=[%d]\n%s",
                       docPath_p, vRet, oErrMsg_p );
            outPutErrM ( oErrMsg_p, errMsg );
            return vRet;
        }
        else if ( vRet == 1 ) //文件不存在则需要先构建文件头
        {
            char tmpTime[MC_TIMES_LEN] = {}; 
            getTime ( 3, tmpTime, NULL, oErrMsg_p ); //:%Y%m%d
            putXmlHeadPs ( toDoc, tmpTime, MC_L_DOSTAPATH, oErrMsg_p );
        }
        else //文件已经存在 加载xml文件
        {
            vRet = toDoc.LoadFile ( docPath_p );

            if ( vRet != 0 )
            {
                snprintf ( errMsg, sizeof ( errMsg ),
                           "load xml file [%s] failed vRet=[%d][%s]",
                           docPath_p, vRet, toDoc.ErrorName() );
                outPutErrM ( oErrMsg_p, errMsg );
                return ERR_XML_LOAD;
            }
        }

        toDoc_p = &toDoc;
    }
    else
    {
        toDoc_p = doc_p;
    }
    char tmpTime[MC_TIMES_LEN] = {};
    getTime ( 4, tmpTime, NULL, oErrMsg_p );//%Y-%m-%d:%H-%M-%S 
    XMLElement* fileE_p = toDoc_p->FirstChildElement ( MC_STA_FILE );
    if ( fileE_p != NULL )
    {
        XMLElement* statDescE_p = fileE_p->FirstChildElement( MC_STA_STADESC );
        if ( statDescE_p != NULL )
        {
            XMLElement* retTimeE_p = statDescE_p->FirstChildElement( MC_STA_RETTIME );
            if ( retTimeE_p != NULL )
            {
                if ( strlen ( statDescItem_p->retTime ) == 0 )
                {
                    retTimeE_p->SetText( tmpTime );
                    strcpy( statDescItem_p->retTime, tmpTime );
                }
                else
                {
                    retTimeE_p->SetText( statDescItem_p->retTime );
                }
            }

            XMLElement* retCodeE_p = statDescE_p->FirstChildElement( MC_STA_RETCODE );
            if ( retCodeE_p != NULL )
            {
                retCodeE_p->SetText( statDescItem_p->retCode );
            }

            XMLElement* retMsgE_p = statDescE_p->FirstChildElement( MC_STA_RETMSG );
            if ( retMsgE_p != NULL )
            {
                retMsgE_p->SetText( statDescItem_p->retMsg );
            }

        }
        else
        {
            XMLElement* statDescE_p = toDoc_p->NewElement ( MC_STA_STADESC );

            XMLElement* retTimeE_p = toDoc_p->NewElement ( MC_STA_RETTIME );
            if ( strlen ( statDescItem_p->retTime ) == 0 )
            {
                retTimeE_p->SetText( tmpTime );
                strcpy( statDescItem_p->retTime, tmpTime );
            }
            else
            {
                retTimeE_p->SetText( statDescItem_p->retTime );
            }

            XMLElement* retCodeE_p = toDoc_p->NewElement ( MC_STA_RETCODE );
            retCodeE_p->SetText ( statDescItem_p->retCode );
            XMLElement* retMsgE_p = toDoc_p->NewElement ( MC_STA_RETMSG );
            retMsgE_p->SetText ( statDescItem_p->retMsg );

            statDescE_p->InsertEndChild ( retTimeE_p );
            statDescE_p->InsertEndChild ( retCodeE_p );
            statDescE_p->InsertEndChild ( retMsgE_p );

            fileE_p->InsertFirstChild ( statDescE_p );

        }
    }
    
    if ( doc_p == NULL )
    {
        vRet = toDoc_p->SaveFile( docPath_p );
        if ( vRet != 0 )
        {
            snprintf ( errMsg, sizeof ( errMsg ),
                       "toDoc_p->SaveFile(%s),error[%d],errMsg[%s]",
                       docPath_p, toDoc_p->ErrorID(), 
                       toDoc_p->ErrorName() );
            outPutErrM ( oErrMsg_p, errMsg );

            return ERR_SYS_FUNCRET;
        }
    }


    return 0;
}



