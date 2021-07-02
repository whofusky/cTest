/**********************************************************
* name:       mete_healthRep.cpp
*
* desc:      气象下载器的，生成健康报告
*
* date:      20170731191858
*
***********************************************************/

#include "mete_healthRep.h"
#include "funcLib.h"
#include "mete_init.h"
#include "hlthRprtWrtXml.h"
#include "mete_errHandling.h"
#include "mete_statBusList.h"

//using namespace tinyxml2;



C_MHealthRep* C_MHealthRep::cInstance_p = NULL;

C_MHealthRep::C_MHealthRepHandler::C_MHealthRepHandler ( C_MHealthRep* healthRep_p )
{
    cHealthRep_p = healthRep_p;
}

C_MHealthRep::C_MHealthRepHandler::~C_MHealthRepHandler()
{

}

void C_MHealthRep::C_MHealthRepHandler::handleMessage ( Message msg )
{
    Json::Value  tmpAttr;

    char vRetMsg[MC_ERRMSG_MAX];
    char vTmpMsg[MC_ERRMSG_MAX * 2];
    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    memset ( vTmpMsg, 0, sizeof ( vTmpMsg ) );

    int vRet = 0;
    int sysFlag = 1;//1 //1 调用shell命令cp处理

    switch ( msg.mWhat )
    {
        case PARSE_APPADD_ITEM://生成健康报告 或 综合健康报告
        {

            T_xmlHVar xmlHVar;
            memset ( &xmlHVar, 0, sizeof ( T_xmlHVar ) );

            if ( msg.mAttr.isMember ( "taskNum" ) )
            {
                snprintf ( xmlHVar.xmlHtItem.taskNum,
                           sizeof ( xmlHVar.xmlHtItem.taskNum ),
                           "%s",
                           msg.mAttr["taskNum"].asCString() );
            }

            if ( msg.mAttr.isMember ( "beWatchedN" ) )
            {
                snprintf ( xmlHVar.xmlHtItem.beWatchedN,
                           sizeof ( xmlHVar.xmlHtItem.beWatchedN ),
                           "%s",
                           msg.mAttr["beWatchedN"].asCString() );
            }

            if ( msg.mAttr.isMember ( "beWatchedS" ) )
            {
                snprintf ( xmlHVar.xmlHtItem.beWatchedS,
                           sizeof ( xmlHVar.xmlHtItem.beWatchedS ),
                           "%s",
                           msg.mAttr["beWatchedS"].asCString() );
            }

            if ( msg.mAttr.isMember ( "capTime" ) )
            {
                snprintf ( xmlHVar.xmlHtItem.capTime,
                           sizeof ( xmlHVar.xmlHtItem.capTime ),
                           "%s",
                           msg.mAttr["capTime"].asCString() );
            }


            //            xmlHVar.cfgFlag=msg.mAttr["cfgFlag"].asInt();
            //            //是否需要根据配置文件得到文件名及场站名:0 需要 ; 1不需要
            //            if(xmlHVar.cfgFlag==MC_TOHFROMCFG)
            //            {
            //                vRet = putXmlLFileH(C_MSysInit::Instance()->getXmlName(),
            //                                    msg.mAttr["healthType"].asCString(),
            //                                    xmlHVar,
            //                                    vRetMsg
            //                                    );

            //            }
            //            else
            //            {
            snprintf ( xmlHVar.fuallName,
                       sizeof ( xmlHVar.fuallName ),
                           "%s",
                       msg.mAttr["HfuallName"].asCString() );

            snprintf ( xmlHVar.fieldName,
                       sizeof ( xmlHVar.fieldName ),
                           "%s",
                       cHealthRep_p->otherAttr[MC_L_FIELDNAME].asCString() );

            snprintf ( xmlHVar.xmlHtItem.hostName,
                       sizeof ( xmlHVar.xmlHtItem.hostName ),
                           "%s",
                       cHealthRep_p->otherAttr[MC_L_HOSTNMAE].asCString() );
            snprintf ( xmlHVar.xmlHtItem.theMonitor,
                       sizeof ( xmlHVar.xmlHtItem.theMonitor ),
                           "%s",
                       cHealthRep_p->otherAttr[MC_L_THEMONITOR].asCString() );

            vRet = putXmlLFileH ( xmlHVar.fuallName,
                                  msg.mAttr["healthType"].asCString(),
                                  xmlHVar,
                                  vRetMsg
                                );

            //}
            if ( vRet != 0 )
            {
                snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                           "%s|%s|%d:statList return[%d]\n%s",
                           __FILE__, __func__, __LINE__, vRet, vRetMsg );
                tmpAttr["retCode"] = METE_ERROR_GENHREPORT;
                tmpAttr["retMsg"] = vTmpMsg;
                tmpAttr["validHllth"] = 0;
                cHealthRep_p->sendError ( tmpAttr );

                return;
            }

            //健康报告生成后，移到完成目录
            if ( strcmp ( msg.mAttr["healthType"].asCString(),
                          MC_FLAG_HLTH
                        ) == 0
               ) //健康报告
            {
                vRet = copyFile ( msg.mAttr["HfuallName"].asCString(),
                                  cHealthRep_p->otherAttr[MC_L_YESHPATH].asCString(),
                                  vRetMsg,
                                  sysFlag
                                );

                if ( vRet != 0 )
                {
                    snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                               "%s|%s|%d:copyFile return[%d]\n%s",
                               __FILE__, __func__, __LINE__, vRet, vRetMsg );
                    tmpAttr["retCode"] = vRet;
                    tmpAttr["retMsg"] = vTmpMsg;
                    tmpAttr["validHllth"] = 0;
                    cHealthRep_p->sendError ( tmpAttr );

                    return;
                }
            }
            else//综合健康报告
            {
                vRet = copyFile ( msg.mAttr["HfuallName"].asCString(),
                                  cHealthRep_p->otherAttr[MC_L_YESINTEGHPATH].asCString(),
                                  vRetMsg,
                                  sysFlag
                                );

                if ( vRet != 0 )
                {
                    snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                               "%s|%s|%d:copyFile return[%d]\n%s",
                               __FILE__, __func__, __LINE__, vRet, vRetMsg );
                    tmpAttr["retCode"] = METE_ERROR_MVHREPORT;
                    tmpAttr["retMsg"] = vTmpMsg;
                    tmpAttr["validHllth"] = 0;
                    cHealthRep_p->sendError ( tmpAttr );
                    return;
                }
            }
        }
        break;

        case PARSE_MERGE_TO_INTEG://合并健康报告到综合健康报告
        {
            char tmpSrcFile[MC_FULLN_LEN] = {};
            snprintf ( tmpSrcFile,
                       sizeof ( tmpSrcFile ),
                       "%s/%s",
                       msg.mAttr["Parameter"]["Dir"].asCString(),
                       msg.mAttr["Parameter"]["FileName"].asCString() );

            vRet = mergeToIntegXml ( tmpSrcFile,
                                     msg.mAttr["HfuallName"].asCString(),
                                     vRetMsg
                                   );

            if ( vRet != 0 )
            {
                snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                           "%s|%s|%d:mergeToIntegXml return[%d]\n%s",
                           __FILE__, __func__, __LINE__, vRet, vRetMsg );
                tmpAttr["retCode"] = METE_ERROR_MERHREPORT;
                tmpAttr["retMsg"] = vTmpMsg;
                tmpAttr["validHllth"] = 0;
                cHealthRep_p->sendError ( tmpAttr );

                return;
            }
            else//cp到完成目录
            {
                vRet = copyFile ( msg.mAttr["HfuallName"].asCString(),
                                  cHealthRep_p->otherAttr[MC_L_YESINTEGHPATH].asCString(),
                                  vRetMsg,
                                  sysFlag
                                );

                if ( vRet != 0 )
                {
                    snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                               "%s|%s|%d:copyFile return[%d]\n%s",
                               __FILE__, __func__, __LINE__, vRet, vRetMsg );
                    tmpAttr["retCode"] = METE_ERROR_MVHREPORT;
                    tmpAttr["retMsg"] = vTmpMsg;
                    tmpAttr["validHllth"] = 0;
                    cHealthRep_p->sendError ( tmpAttr );

                    return;
                }
            }

        }
        break;

        default:
            break;
    }

    return;
}

C_MHealthRep::C_MHealthTimer::C_MHealthTimer ( Json::Value&  tiAttr,
        bool autoRelaseFlag,
        C_MHealthRep* healthRep_p
                                             ) : CTimer ( autoRelaseFlag )
{
    tAttr = tiAttr;
    cMHealthRep_p = healthRep_p;
}

C_MHealthRep::C_MHealthTimer::~C_MHealthTimer()
{

}

C_MHealthRep::C_MHealthTimer*
C_MHealthRep::C_MHealthTimer::getObj ( Json::Value&  tiAttr,
                                       bool autoRelaseFlag,
                                       C_MHealthRep* healthRep_p
                                     )
{
    return new C_MHealthTimer ( tiAttr, autoRelaseFlag, healthRep_p );
}

void C_MHealthRep::C_MHealthTimer::timerHandler()
{
    char vRetMsg[MC_ERRMSG_MAX];
    char vTmpMsg[MC_ERRMSG_MAX * 2];
    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    memset ( vTmpMsg, 0, sizeof ( vTmpMsg ) );

    Json::Value  tmpAttr;

    int vRet = 0;

    //定时查看综合健康报告目录，并mv 可 cp 文件到上传文件目录
    DIR* dir_p = NULL;
    struct dirent* dirent_p = NULL;

    dir_p = opendir ( tAttr[MC_L_YESINTEGHPATH].asCString() );

    if ( dir_p == NULL )
    {
        snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                   "%s|%s|%d:cannot open directory [%s]\n",
                   __FILE__, __func__, __LINE__,
                   tAttr[MC_L_YESINTEGHPATH].asCString() );
        tmpAttr["retCode"] = ERR_POINTER_NULL;
        tmpAttr["retMsg"] = vTmpMsg;
        tmpAttr["validHllth"] = 0;
        cMHealthRep_p->sendError ( tmpAttr );

        return;
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
                            tAttr[MC_L_NAMEINTEREGEX].asCString(),
                            vRetMsg
                          );

        if ( vRet == 0 ) //有匹配到的文件名
        {
            char tmpFullFile[MC_FULLN_LEN] = {0};
            snprintf ( tmpFullFile,
                       sizeof ( tmpFullFile ),
                       "%s/%s",
                       tAttr[MC_L_YESINTEGHPATH].asCString(), dirent_p->d_name );
            //把文件移到上传目录
            int sysFlag = 1;//用shell的mv命令
            vRet = mrename ( tmpFullFile,
                             tAttr[MC_L_UPHPATH].asCString(),
                             vRetMsg,
                             sysFlag
                           );

            if ( vRet != 0 )
            {
                snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                           "%s|%s|%d:mrename return[%d] sysFlag=[%d] [%s] [%s]\n%s",
                           __FILE__, __func__, __LINE__, vRet, sysFlag, tmpFullFile,
                           tAttr[MC_L_UPHPATH].asCString(), vRetMsg );
                tmpAttr["retCode"] = METE_ERROR_MVHREPORT;
                tmpAttr["retMsg"] = vTmpMsg;
                tmpAttr["validHllth"] = 0;
                cMHealthRep_p->sendError ( tmpAttr );

                closedir ( dir_p );
                return;
            }

        }
    }

    closedir ( dir_p );

    cMHealthRep_p->doStatFlag = 1;
    if (  C_MStatBList::Instance()->doingFlag == 0 
        )
    {
        char tFullName[MC_FULLN_LEN];//带路径名的全文件名
        char tmpTime[MC_TIMES_LEN]; 
        memset ( tFullName, 0, sizeof ( tFullName ) );
        memset ( tmpTime, 0, sizeof ( tmpTime ) );
        getTime ( 3, tmpTime, NULL, vRetMsg );

        //状态统计文件名
        snprintf ( tFullName, sizeof ( tFullName ), 
                "%s/%s%s.xml", 
                C_MStatBList::Instance()->doStaPath_p,
                C_MStatBList::Instance()->doStaName_p, 
                tmpTime );

        //周期性的把软件状态写到文件中并copy到反隔目录
        //1.把软件状态写入状态文件
        T_xmlStatDescItem* tmpStatDesc_p = C_MErrHandling::Instance()->convertRetInfo();
        vRet = writeRetToXml ( NULL, tFullName, 
                tmpStatDesc_p,
                vRetMsg );
        if ( vRet != 0 )
        {
            snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                       "%s|%s|%d:writeRetToXml "
                       "tFullName=[%s] return error vRet=[%d]\n%s",
                       __FILE__, __func__, __LINE__, 
                       tFullName, vRet,
                       vRetMsg );
            tmpAttr["retCode"] = METE_ERROR_WTSTAT;
            tmpAttr["retMsg"] = vTmpMsg;
            tmpAttr["validHllth"] = 0;
            cMHealthRep_p->doStatFlag = 0;
            cMHealthRep_p->sendError ( tmpAttr );

            return;


        }

        //2.把状态文件copy到反隔目录
        char* toAnti_p = C_MSysInit::Instance()->toAnti_p;
        char* toAntiBak_p = C_MSysInit::Instance()->toAntiBak_p;

        int sysFlag = 0;//是否用shell命令操作标识  0:不用
        if ( toAnti_p != NULL )
        {
            vRet = copyFile ( tFullName, toAnti_p, vRetMsg, sysFlag );

            //vRet = mrename(tFullName,nxtPath_p,oErrMsg_p,sysFlag);
            if ( vRet != 0 )
            {
                snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                           "%s|%s|%d:[%s]->[%s] sysFlag=[%d] "
                           "copyFile  return error vRet=[%d]\n%s",
                           __FILE__, __func__, __LINE__, 
                           tFullName,toAnti_p, sysFlag, vRet,
                           vRetMsg );
                tmpAttr["retCode"] = METE_ERROR_CPFILE;
                tmpAttr["retMsg"] = vTmpMsg;
                tmpAttr["validHllth"] = 0;
                cMHealthRep_p->doStatFlag = 0;
                cMHealthRep_p->sendError ( tmpAttr );

                return;
            }

            if ( toAntiBak_p != NULL )
            {
                vRet = copyFile ( tFullName, toAntiBak_p, vRetMsg, sysFlag );
                if ( vRet != 0 )
                {
                    snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                           "%s|%s|%d:[%s]->[%s] sysFlag=[%d] "
                           "copyFile  return error vRet=[%d]\n%s",
                           __FILE__, __func__, __LINE__, 
                           tFullName,toAntiBak_p, sysFlag, vRet,
                           vRetMsg );
                    tmpAttr["retCode"] = METE_ERROR_CPFILE;
                    tmpAttr["retMsg"] = vTmpMsg;
                    tmpAttr["validHllth"] = 0;
                    cMHealthRep_p->doStatFlag = 0;
                    cMHealthRep_p->sendError ( tmpAttr );

                    return;
                }
            }
        }
    }
    cMHealthRep_p->doStatFlag = 0;

    return;

}


C_MHealthRep::C_MHealthRep()
{
    second     = 0;
    micSecond  = 0;
    repet      = 0;

    cHandler_p = NULL;
    cLooper_p  = NULL;
    cMHTimer_p = NULL;

}

C_MHealthRep::~C_MHealthRep()
{
    if ( cHandler_p != NULL )
    {
        delete cHandler_p;
        cHandler_p = NULL;
    }

    if ( cLooper_p != NULL )
    {
        cLooper_p->decRef();
    }

    if ( cMHTimer_p != NULL )
    {
        delete cMHTimer_p;
        cMHTimer_p = NULL;
    }
}


void C_MHealthRep::sendError ( const Json::Value&  errMsg )
{
    //    Json::Value handMsg;
    //    handMsg["retCode"] = errMsg["retCode"];
    //    handMsg["retMsg"] = errMsg["retMsg"];
    //    handMsg["validHllth"] = errMsg["validHllth"];
    //handMsg["healthVal"]=errMsg["healthVal"];

    C_MErrHandling::Instance()->parseMsg ( errMsg );

    return;
}


void C_MHealthRep::init()
{
    Json::Value  tmpAttr;
    char vRetMsg[MC_ERRMSG_MAX];
    char vTmpMsg[MC_ERRMSG_MAX * 2];
    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    memset ( vTmpMsg, 0, sizeof ( vTmpMsg ) );


    if ( cHandler_p == NULL )
    {
        cHandler_p = new C_MHealthRepHandler ( this );
    }
    else
    {
        return;
    }

    doStatFlag = 0;
    memset ( fullOtherLog, 0, sizeof ( fullOtherLog ) );
    int vRet = 0;

    //获取是否打印debug配置及日志文件配置
    prtDebugFlag = C_MSysInit::Instance()->getPrtFlag();
    snprintf ( fullOtherLog, sizeof ( fullOtherLog ),
               "%s",
               C_MSysInit::Instance()->getMeteLog().fullOtherLog );

    char ttStr[][MC_XMLELE_LEM] = {MC_L_TODO, MC_L_HEALTHRP, MC_L_FIELDNAME};
    T_linkXmlEles linkXmlEles = {ttStr, 3};

    char* eleVal_p = NULL;
    vRet = getFirstEleVal ( C_MSysInit::Instance()->getDoc(),
                            linkXmlEles,
                            eleVal_p,
                            vRetMsg
                          );

    if ( vRet != 0 )
    {
        snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                   "%s|%s|%d: getFirstEleVal return vRet[%d]\n%s",
                   __FILE__, __func__, __LINE__, vRet, vRetMsg );
        tmpAttr["retCode"] = METE_ERROR_PARSECFG;
        tmpAttr["retMsg"] = vTmpMsg;
        tmpAttr["validHllth"] = 0;
        sendError ( tmpAttr );

        return;
    }

    otherAttr[MC_L_FIELDNAME] = eleVal_p; //场站名称

    //取宿主名称
    snprintf ( ttStr[2], MC_XMLELE_LEM, MC_L_HOSTNMAE );
    vRet = getFirstEleVal ( C_MSysInit::Instance()->getDoc(),
                            linkXmlEles,
                            eleVal_p,
                            vRetMsg
                          );

    if ( vRet != 0 )
    {
        snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                   "%s|%s|%d: getFirstEleVal return vRet[%d]\n%s",
                   __FILE__, __func__, __LINE__, vRet, vRetMsg );
        tmpAttr["retCode"] = METE_ERROR_PARSECFG;
        tmpAttr["retMsg"] = vTmpMsg;
        tmpAttr["validHllth"] = 0;
        sendError ( tmpAttr );

        return;
    }

    otherAttr[MC_L_HOSTNMAE] = eleVal_p;

    //取监视者
    snprintf ( ttStr[2], MC_XMLELE_LEM, MC_L_THEMONITOR );
    vRet = getFirstEleVal ( C_MSysInit::Instance()->getDoc(),
                            linkXmlEles,
                            eleVal_p,
                            vRetMsg
                          );

    if ( vRet != 0 )
    {
        snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                   "%s|%s|%d: getFirstEleVal return vRet[%d]\n%s",
                   __FILE__, __func__, __LINE__, vRet, vRetMsg );
        tmpAttr["retCode"] = METE_ERROR_PARSECFG;
        tmpAttr["retMsg"] = vTmpMsg;
        tmpAttr["validHllth"] = 0;
        sendError ( tmpAttr );

        return;
    }

    otherAttr[MC_L_THEMONITOR] = eleVal_p;

    //生成健康报告目录
    snprintf ( ttStr[1], MC_XMLELE_LEM, MC_L_DOLIST );
    snprintf ( ttStr[2], MC_XMLELE_LEM, MC_L_DOHPATH );
    vRet = getFirstEleVal ( C_MSysInit::Instance()->getDoc(),
                            linkXmlEles,
                            eleVal_p,
                            vRetMsg
                          );

    if ( vRet != 0 )
    {
        snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                   "%s|%s|%d: getFirstEleVal return vRet[%d]\n%s",
                   __FILE__, __func__, __LINE__, vRet, vRetMsg );
        tmpAttr["retCode"] = METE_ERROR_PARSECFG;
        tmpAttr["retMsg"] = vTmpMsg;
        tmpAttr["validHllth"] = 0;
        sendError ( tmpAttr );

        return;
    }

    otherAttr[MC_L_DOHPATH] = eleVal_p;

    //健康报告文件名前缀
    snprintf ( ttStr[2], MC_XMLELE_LEM, MC_L_PRENAMEH );
    vRet = getFirstEleVal ( C_MSysInit::Instance()->getDoc(),
                            linkXmlEles,
                            eleVal_p,
                            vRetMsg
                          );

    if ( vRet != 0 )
    {
        snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                   "%s|%s|%d: getFirstEleVal return vRet[%d]\n%s",
                   __FILE__, __func__, __LINE__, vRet, vRetMsg );
        tmpAttr["retCode"] = METE_ERROR_PARSECFG;
        tmpAttr["retMsg"] = vTmpMsg;
        tmpAttr["validHllth"] = 0;
        sendError ( tmpAttr );

        return;
    }

    otherAttr[MC_L_PRENAMEH] = eleVal_p;

    //健康报告完成目录 也是综合健康报告触发目录
    snprintf ( ttStr[2], MC_XMLELE_LEM, MC_L_YESHPATH );
    vRet = getFirstEleVal ( C_MSysInit::Instance()->getDoc(),
                            linkXmlEles,
                            eleVal_p,
                            vRetMsg
                          );

    if ( vRet != 0 )
    {
        snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                   "%s|%s|%d: getFirstEleVal return vRet[%d]\n%s",
                   __FILE__, __func__, __LINE__, vRet, vRetMsg );
        tmpAttr["retCode"] = METE_ERROR_PARSECFG;
        tmpAttr["retMsg"] = vTmpMsg;
        tmpAttr["validHllth"] = 0;
        sendError ( tmpAttr );

        return;
    }

    otherAttr[MC_L_YESHPATH] = eleVal_p;

    //场内上传的健康报告 也是综合健康报告触发目录
    snprintf ( ttStr[2], MC_XMLELE_LEM, MC_L_FRMIIHPATH );
    vRet = getFirstEleVal ( C_MSysInit::Instance()->getDoc(),
                            linkXmlEles,
                            eleVal_p,
                            vRetMsg
                          );

    if ( vRet != 0 )
    {
        snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                   "%s|%s|%d: getFirstEleVal return vRet[%d]\n%s",
                   __FILE__, __func__, __LINE__, vRet, vRetMsg );
        tmpAttr["retCode"] = METE_ERROR_PARSECFG;
        tmpAttr["retMsg"] = vTmpMsg;
        tmpAttr["validHllth"] = 0;
        sendError ( tmpAttr );

        return;
    }

    otherAttr[MC_L_FRMIIHPATH] = eleVal_p;

    //综合健康报告生成目录
    snprintf ( ttStr[2], MC_XMLELE_LEM, MC_L_INTEGHPATH );
    vRet = getFirstEleVal ( C_MSysInit::Instance()->getDoc(),
                            linkXmlEles,
                            eleVal_p,
                            vRetMsg
                          );

    if ( vRet != 0 )
    {
        snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                   "%s|%s|%d: getFirstEleVal return vRet[%d]\n%s",
                   __FILE__, __func__, __LINE__, vRet, vRetMsg );
        tmpAttr["retCode"] = METE_ERROR_PARSECFG;
        tmpAttr["retMsg"] = vTmpMsg;
        tmpAttr["validHllth"] = 0;
        sendError ( tmpAttr );

        return;
    }

    otherAttr[MC_L_INTEGHPATH] = eleVal_p;


    //综合健康报告文件名前缀
    snprintf ( ttStr[2], MC_XMLELE_LEM, MC_L_PRENAMEINTE );
    vRet = getFirstEleVal ( C_MSysInit::Instance()->getDoc(),
                            linkXmlEles,
                            eleVal_p,
                            vRetMsg
                          );

    if ( vRet != 0 )
    {
        snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                   "%s|%s|%d: getFirstEleVal return vRet[%d]\n%s",
                   __FILE__, __func__, __LINE__, vRet, vRetMsg );
        tmpAttr["retCode"] = METE_ERROR_PARSECFG;
        tmpAttr["retMsg"] = vTmpMsg;
        tmpAttr["validHllth"] = 0;
        sendError ( tmpAttr );

        return;
    }

    otherAttr[MC_L_PRENAMEINTE] = eleVal_p;

    //综合健康报告文件名POSIX正则表达式
    snprintf ( ttStr[2], MC_XMLELE_LEM, MC_L_NAMEINTEREGEX );
    vRet = getFirstEleVal ( C_MSysInit::Instance()->getDoc(),
                            linkXmlEles,
                            eleVal_p,
                            vRetMsg
                          );

    if ( vRet != 0 )
    {
        snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                   "%s|%s|%d: getFirstEleVal return vRet[%d]\n%s",
                   __FILE__, __func__, __LINE__, vRet, vRetMsg );
        tmpAttr["retCode"] = METE_ERROR_PARSECFG;
        tmpAttr["retMsg"] = vTmpMsg;
        tmpAttr["validHllth"] = 0;
        sendError ( tmpAttr );

        return;
    }

    otherAttr[MC_L_NAMEINTEREGEX] = eleVal_p;

    //综合健康报告上传目录
    snprintf ( ttStr[2], MC_XMLELE_LEM, MC_L_UPHPATH );
    vRet = getFirstEleVal ( C_MSysInit::Instance()->getDoc(),
                            linkXmlEles,
                            eleVal_p,
                            vRetMsg
                          );

    if ( vRet != 0 )
    {
        snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                   "%s|%s|%d: getFirstEleVal return vRet[%d]\n%s",
                   __FILE__, __func__, __LINE__, vRet, vRetMsg );
        tmpAttr["retCode"] = METE_ERROR_PARSECFG;
        tmpAttr["retMsg"] = vTmpMsg;
        tmpAttr["validHllth"] = 0;
        sendError ( tmpAttr );

        return;
    }

    otherAttr[MC_L_UPHPATH] = eleVal_p;

    //综合健康报告完成目录
    snprintf ( ttStr[2], MC_XMLELE_LEM, MC_L_YESINTEGHPATH );
    vRet = getFirstEleVal ( C_MSysInit::Instance()->getDoc(),
                            linkXmlEles,
                            eleVal_p,
                            vRetMsg
                          );

    if ( vRet != 0 )
    {
        snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                   "%s|%s|%d: getFirstEleVal return vRet[%d]\n%s",
                   __FILE__, __func__, __LINE__, vRet, vRetMsg );
        tmpAttr["retCode"] = METE_ERROR_PARSECFG;
        tmpAttr["retMsg"] = vTmpMsg;
        tmpAttr["validHllth"] = 0;
        sendError ( tmpAttr );

        return;
    }

    otherAttr[MC_L_YESINTEGHPATH] = eleVal_p;


    vRet = getChkCycleFxml ( C_MSysInit::Instance()->getDoc(),
                             MC_L_PUTUP,
                             second,
                             micSecond,
                             repet,
                             vRetMsg
                           );
    writeLog ( fullOtherLog, "%s|%s|%d:second[%d],micSecond[%d],repet[%d]\n",
               __FILE__, __func__, __LINE__, second, micSecond, repet );

    if ( vRet != 0 )
    {
        snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                   "%s|%s|%d:getChkCycleFxml return[%d]\n%s",
                   __FILE__, __func__, __LINE__, vRet, vRetMsg );
        tmpAttr["retCode"] = METE_ERROR_PARSECFG;
        tmpAttr["retMsg"] = vTmpMsg;
        tmpAttr["validHllth"] = 0;
        sendError ( tmpAttr );

        return;
    }

    if ( repet == 0 )
    {
        snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                   "%s|%s|%d:repet value is error\n",
                   __FILE__, __func__, __LINE__ );
        tmpAttr["retCode"] = METE_ERROR_CFGVALUE;
        tmpAttr["retMsg"] = vTmpMsg;
        tmpAttr["validHllth"] = 0;
        sendError ( tmpAttr );

        return;
    }

    return;
}


void C_MHealthRep::start()
{

    init();

    startThread();


    bool autoReleaseFlag = 0;

    if ( repet < 0 )
    {
        autoReleaseFlag = False;
    }
    else
    {
        autoReleaseFlag = True;
    }

    C_MHealthTimer* mvTimer_p = C_MHealthTimer::getObj ( otherAttr, autoReleaseFlag, this );
    mvTimer_p->setTimer ( second, micSecond, repet );
    mvTimer_p->startTimer();

    //tTimer_p 的释放由 repet 决定:
    //          当 repet <0 定时器无限循环 此时因 tTimer_p 自己不自动释放，所以需要外部释放
    //          当 repet>0  定时器只运行repet次，然后自动释放
    if ( repet < 0 )
    {
        cMHTimer_p = mvTimer_p;
    }

    return;
}

void C_MHealthRep::stop()
{
    stopThread();
    delete this;
}

bool C_MHealthRep::parseMsg ( const  Json::Value&  attr )
{
    Json::Value  tmpAttr;
    char vRetMsg[MC_ERRMSG_MAX];
    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );

    //    printf("%s|%s|%d:  健康报告 parseMsg attr=[%s]\n",
    //        __FILE__,__func__,__LINE__, attr.toStyledString().c_str());

    //attr 中需要有 cfgFlag healthType taskNum beWatchedN beWatchedS
    Message msg;
    msg.mAttr         = attr;
    msg.mTarget       = cHandler_p;
    msg.mTargetLooper = cLooper_p;
    msg.setValid ( True );

    char tmpFullFile[MC_FULLN_LEN] = {};
    char tmpTime[MC_TIMES_LEN]     = {};
    getTime ( 3,
              tmpTime,
              NULL,
              vRetMsg
            );//获得系统时间:%Y%m%d

    if ( strcmp ( attr["healthType"].asCString(),
                  MC_FLAG_HLTH
                ) == 0
       ) //健康报告
    {
        msg.mWhat         = C_MHealthRepHandler::PARSE_APPADD_ITEM;
        snprintf ( tmpFullFile,
                   sizeof ( tmpFullFile ),
                   "%s/%s%s.xml",
                   otherAttr[MC_L_DOHPATH].asCString(),
                   otherAttr[MC_L_PRENAMEH].asCString(), tmpTime );
    }
    else if ( strcmp ( attr["healthType"].asCString(),
                       MC_FLAG_INTHLTH
                     ) == 0
            ) //综合健康报告
    {
        msg.mWhat         = C_MHealthRepHandler::PARSE_APPADD_ITEM;
        snprintf ( tmpFullFile,
                   sizeof ( tmpFullFile ),
                   "%s/%s%s.xml",
                   otherAttr[MC_L_INTEGHPATH].asCString(),
                   otherAttr[MC_L_PRENAMEINTE].asCString(), tmpTime );
    }
    else if ( strcmp ( attr["healthType"].asCString(),
                       MC_FLAG_MERGEH
                     ) == 0
            ) //合成综合健康报告
    {
        msg.mWhat         = C_MHealthRepHandler::PARSE_MERGE_TO_INTEG;
        snprintf ( tmpFullFile,
                   sizeof ( tmpFullFile ),
                   "%s/%s%s.xml",
                   otherAttr[MC_L_INTEGHPATH].asCString(),
                   otherAttr[MC_L_PRENAMEINTE].asCString(), tmpTime );
    }
    else
    {
        msg.setValid ( False );
        snprintf ( vRetMsg, sizeof ( vRetMsg ),
                   "%s|%s|%d:health report type is [%s] or [%s] not [%s]\n",
                   __FILE__, __func__, __LINE__,
                   MC_L_DOHPATH, MC_L_INTEGHPATH, attr["healthType"].asCString() );
        tmpAttr["retCode"] = ERR_INPUT_OR_CONF;
        tmpAttr["retMsg"] = vRetMsg;
        tmpAttr["validHllth"] = 0;
        sendError ( tmpAttr );

        return False;

    }

    msg.mAttr["HfuallName"] = tmpFullFile;

    memset ( tmpTime, 0, sizeof ( tmpTime ) );
    getTime ( 4,
              tmpTime,
              NULL,
              vRetMsg
            );//获得系统时间:%Y-%m-%d:%H-%M-%S"

    msg.mAttr["capTime"] = tmpTime;

    cHandler_p->sendMessage ( msg );

    return True;

}


void C_MHealthRep::threadHandler()
{
    Looper* me_p = Looper::getLooper();
    cLooper_p = me_p;
    cLooper_p->prepare();
    cLooper_p->Loop();

}

C_MHealthRep* C_MHealthRep::Instance()
{
    if ( cInstance_p == NULL )
    {
        cInstance_p = new C_MHealthRep;
    }

    return cInstance_p;
}



















