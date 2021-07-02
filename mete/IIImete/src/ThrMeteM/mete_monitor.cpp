/**********************************************************
* name:       mete_monitor.cpp
*
* desc:      气象下载器的，文件监视器
*
* date:      20170801153843
*
***********************************************************/

#include "mete_monitor.h"
#include "funcLib.h"
#include "mete_synBusList.h"
#include "mete_synBusList.h"
#include "mete_statBusList.h"
#include "mete_ftpPut.h"
#include "mete_init.h"
#include "mete_errHandling.h"
#include "hlthRprtWrtXml.h"
#include "mete_healthRep.h"


//using namespace tinyxml2;


C_MMonitor* C_MMonitor::cInstance_p = NULL;

C_MMonitor::C_MMonitorHandler::C_MMonitorHandler ( C_MMonitor* mMonitor_p )
{
    cmMonitor_p = mMonitor_p;
}

C_MMonitor::C_MMonitorHandler::~C_MMonitorHandler()
{

}

void C_MMonitor::C_MMonitorHandler::handleMessage ( Message msg )
{
    //    int vRet = 0;
    switch ( msg.mWhat )
    {
        case MSG_DOBULIST_FILE:
        {
            //给业务清单合成线程发消息
            if ( 1 == cmMonitor_p->prtDebugFlag )
                writeLog ( cmMonitor_p->fullOtherLog, "%s|%s|%d: [%s]\n",
                           __FILE__, __func__, __LINE__, "给业务清单合成线程发消息" );

            C_MFsyBList::Instance()->parseFile ( msg.mAttr );

        }
        break;

        case MSG_STATBULIST:
        {
            //给生成业务清单情况线程发消息
            if ( 1 == cmMonitor_p->prtDebugFlag )
                writeLog ( cmMonitor_p->fullOtherLog, "%s|%s|%d: [%s]\n",
                           __FILE__, __func__, __LINE__, "给生成业务清单情况线程发消息" );

            C_MStatBList::Instance()->parseFile ( msg.mAttr );

        }
        break;

        case MSG_UPHEALTH:
        {
            //给上传健康报告线程发消息
            if ( 1 == cmMonitor_p->prtDebugFlag )
                writeLog ( cmMonitor_p->fullOtherLog, "%s|%s|%d: [%s]\n",
                           __FILE__, __func__, __LINE__, "给上传健康报告线程发消息" );

            C_MPutFtp::Instance()->parseFile ( msg.mAttr );

        }
        break;

        case MSG_YESHPATH:
        {
            //III区健康报告生成给合并成综合健康报告线程发消息
            if ( 1 == cmMonitor_p->prtDebugFlag )
                writeLog ( cmMonitor_p->fullOtherLog, "%s|%s|%d: [%s]\n",
                           __FILE__, __func__, __LINE__,
                           "III区健康报告生成给合并成综合健康报告线程发消息" );

            msg.mAttr["healthType"] = MC_FLAG_MERGEH;
            C_MHealthRep::Instance()->parseMsg ( msg.mAttr );

        }
        break;

        default:
            break;
    }

    return;
}

C_MMonitor::C_MMonitor()
{
    monInteFlag     = -1;

    cHandler_p      = NULL;
    cMonitorDoB_p   = NULL;
    cMonitorStaB_p  = NULL;
    cMonitorHtI_p   = NULL;
    cMonitorUpFtp_p = NULL;
    cMonitorInteg   = NULL;



}

C_MMonitor::~C_MMonitor()
{
    if ( cHandler_p != NULL )
    {
        delete cHandler_p;
        cHandler_p = NULL;
    }

    if ( cMonitorDoB_p != NULL )
    {
        delete cMonitorDoB_p;
        cMonitorDoB_p = NULL;
    }

    if ( cMonitorStaB_p != NULL )
    {
        delete cMonitorStaB_p;
        cMonitorStaB_p = NULL;
    }

    if ( cMonitorHtI_p != NULL )
    {
        delete cMonitorHtI_p;
        cMonitorHtI_p = NULL;
    }

    if ( cMonitorUpFtp_p != NULL )
    {
        delete cMonitorUpFtp_p;
        cMonitorUpFtp_p = NULL;
    }

    if ( cLooper_p != NULL )
    {
        cLooper_p->decRef();
    }

}

void C_MMonitor::sendError ( const Json::Value&  errMsg )
{
    //    Json::Value handMsg;
    //    handMsg["retCode"] = errMsg["retCode"];
    //    handMsg["retMsg"] = errMsg["retMsg"];
    //    handMsg["validHllth"] = errMsg["validHllth"];
    //    handMsg["healthVal"]=errMsg["healthVal"];

    C_MErrHandling::Instance()->parseMsg ( errMsg );

    return;
}


void C_MMonitor::init()
{
    Json::Value  tmpAttr;

    char vRetMsg[MC_ERRMSG_MAX];
    char vTmpMsg[MC_ERRMSG_MAX * 2];
    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    memset ( vTmpMsg, 0, sizeof ( vTmpMsg ) );

    if ( cHandler_p == NULL )
    {
        cHandler_p = new C_MMonitorHandler ( this );
    }
    else
    {
        return;
    }

    //健康报告相关值
    appName = MC_M_FMON_NAME;
    //healthVal["cfgFlag"]=MC_TOHFROMVAR;
    healthVal["healthType"] = MC_FLAG_HLTH;
    healthVal["taskNum"] = "1";
    healthVal["beWatchedN"] = appName;

    int vRet = 0;

    memset ( fullOtherLog, 0, sizeof ( fullOtherLog ) );

    //获取是否打印debug配置及日志文件配置
    prtDebugFlag = C_MSysInit::Instance()->getPrtFlag();
    snprintf ( fullOtherLog, sizeof ( fullOtherLog ),
               "%s",
               C_MSysInit::Instance()->getMeteLog().fullOtherLog );

    char* eleVal_p = NULL;
    char ttStr[][MC_XMLELE_LEM] = {MC_L_TODO, MC_L_DOLIST, MC_L_MONINTEFLAG};
    T_linkXmlEles linkXmlEles  = {ttStr, 3};

    //取是否启用综合监视器标识
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
        tmpAttr["healthVal"] = healthVal;
        sendError ( tmpAttr );

        return;
    }

    monInteFlag = atoi ( eleVal_p );

    if ( 1 == prtDebugFlag )
        writeLog ( fullOtherLog, "%s|%s|%d:monInteFlag=[%d]\n",
                   __FILE__, __func__, __LINE__, monInteFlag );

    if ( monInteFlag == 1 ) //启用综合监视器:一个监视器监视多个目录
    {
        cMonitorInteg = new FileMonitor();
    }
    else//启用多个监视器，每个监视器只监视一个目录
    {
        cMonitorDoB_p   = new FileMonitor();
        cMonitorStaB_p  = new FileMonitor();
        cMonitorHtI_p   = new FileMonitor();
        cMonitorUpFtp_p = new FileMonitor();
    }

    //取ftp完成 业务清单合成触发目录
    snprintf ( ttStr[2], MC_XMLELE_LEM, MC_L_SRCLIST );
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
        tmpAttr["healthVal"] = healthVal;
        sendError ( tmpAttr );

        return;
    }

    otherAttr[MC_L_SRCLIST] = eleVal_p;

    //业务清单提供情况 任务触发目录
    snprintf ( ttStr[2], MC_XMLELE_LEM, MC_L_STALISTRIG );
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
        tmpAttr["healthVal"] = healthVal;
        sendError ( tmpAttr );

        return;
    }

    otherAttr[MC_L_STALISTRIG] = eleVal_p;

    //III区健康报告合并成综合健康报告 任务触发目录
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
        tmpAttr["healthVal"] = healthVal;
        sendError ( tmpAttr );

        return;
    }

    otherAttr[MC_L_YESHPATH] = eleVal_p;

    //综合健康报告上传文件触发目录
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
        tmpAttr["healthVal"] = healthVal;
        sendError ( tmpAttr );

        return;
    }

    otherAttr[MC_L_UPHPATH] = eleVal_p;

    return;
}

void C_MMonitor::start()
{

    init();

    if ( monInteFlag == 1 )
    {
        //业务清单合成文件源监视器

        if ( 1 == prtDebugFlag )
            writeLog ( fullOtherLog, "%s|%s|%d:综合监视器  业务清单合成文件源监视[%s]\n",
                       __FILE__, __func__, __LINE__, otherAttr[MC_L_SRCLIST].asCString() );

        cMonitorInteg->startMonitor ( otherAttr[MC_L_SRCLIST].asCString(),
                                      EVENT_MOVE_TO | EVENT_CREATE
                                    );


        //业务清单提供情况文件源监视器
        if ( 1 == prtDebugFlag )
            writeLog ( fullOtherLog, "%s|%s|%d:综合监视器  业务清单提供情况文件源监视[%s]\n",
                       __FILE__, __func__, __LINE__, otherAttr[MC_L_STALISTRIG].asCString() );

        cMonitorInteg->startMonitor ( otherAttr[MC_L_STALISTRIG].asCString(),
                                      EVENT_MOVE_TO | EVENT_CREATE
                                    );


        //III区健康报告合并成综合健康报告 文件源监视器
        if ( 1 == prtDebugFlag )
            writeLog ( fullOtherLog, "%s|%s|%d:综合监视器  III区健康报告合并成综合健康报告 文件源监视[%s]\n",
                       __FILE__, __func__, __LINE__, otherAttr[MC_L_YESHPATH].asCString() );

        cMonitorInteg->startMonitor ( otherAttr[MC_L_YESHPATH].asCString(),
                                      EVENT_MOVE_TO | EVENT_CREATE
                                    );


        //ftp上传健康报告文件源监视器
        if ( 1 == prtDebugFlag )
            writeLog ( fullOtherLog, "%s|%s|%d:综合监视器  ftp上传健康报告文件源监视[%s]\n",
                       __FILE__, __func__, __LINE__, otherAttr[MC_L_UPHPATH].asCString() );

        cMonitorInteg->startMonitor ( otherAttr[MC_L_UPHPATH].asCString(),
                                      EVENT_CREATE | EVENT_MOVE_TO
                                    );
    }
    else
    {
        //业务清单合成文件源监视器
        if ( 1 == prtDebugFlag )
            writeLog ( fullOtherLog, "%s|%s|%d:  业务清单合成文件源监视器[%s]\n",
                       __FILE__, __func__, __LINE__, otherAttr[MC_L_SRCLIST].asCString() );

        cMonitorDoB_p->startMonitor ( otherAttr[MC_L_SRCLIST].asCString(),
                                      EVENT_MOVE_TO | EVENT_CREATE
                                    );


        //业务清单提供情况文件源监视器
        if ( 1 == prtDebugFlag )
            writeLog ( fullOtherLog, "%s|%s|%d:  业务清单提供情况文件源监视器[%s]\n",
                       __FILE__, __func__, __LINE__, otherAttr[MC_L_STALISTRIG].asCString() );

        cMonitorStaB_p->startMonitor ( otherAttr[MC_L_STALISTRIG].asCString(),
                                       EVENT_MOVE_TO | EVENT_CREATE
                                     );


        //III区健康报告合并成综合健康报告 文件源监视器
        if ( 1 == prtDebugFlag )
            writeLog ( fullOtherLog, "%s|%s|%d:  III区健康报告合并成综合健康报告 文件源监视器[%s]\n",
                       __FILE__, __func__, __LINE__, otherAttr[MC_L_YESHPATH].asCString() );

        cMonitorHtI_p->startMonitor ( otherAttr[MC_L_YESHPATH].asCString(),
                                      EVENT_MOVE_TO | EVENT_CREATE
                                    );


        //ftp上传健康报告文件源监视器
        if ( 1 == prtDebugFlag )
            writeLog ( fullOtherLog, "%s|%s|%d:  ftp上传健康报告文件源监视器[%s]\n",
                       __FILE__, __func__, __LINE__, otherAttr[MC_L_UPHPATH].asCString() );

        cMonitorUpFtp_p->startMonitor ( otherAttr[MC_L_UPHPATH].asCString(),
                                        EVENT_CREATE | EVENT_MOVE_TO
                                      );
    }


    //注册事件 STORAGE_EVENT
    IEventManager::Initialize()->attachEventHandler (
    STORAGE_EVENT,
    ( IEventHandler* ) this,
    ( IEventManager::HANDLER_FUNC ) &C_MMonitor::onAppEvent );


    startThread();

    return;

}

void C_MMonitor::stop()
{
    stopThread();
    delete this;
}

void C_MMonitor::onAppEvent ( const char* event,
                              IEventManager::EVENT_ACTION action,
                              Json::Value eventAttr )
{
    Message msg;

    if ( strcmp ( event, STORAGE_EVENT ) == 0 )
    {
        if ( 1 == prtDebugFlag )
            writeLog ( fullOtherLog, "%s|%s|%d: -----[%s]\n",
                       __FILE__, __func__, __LINE__, eventAttr["Parameter"]["Dir"].asCString() );

        if ( strcmp ( eventAttr["Parameter"]["Dir"].asCString(),
                      otherAttr[MC_L_SRCLIST].asCString()
                    ) == 0
           )
        {
            if ( 1 == prtDebugFlag )
                writeLog ( fullOtherLog, "%s|%s|%d:  业务清单合成文件源触发的[%s]\n",
                           __FILE__, __func__, __LINE__, eventAttr["Parameter"]["Dir"].asCString() );

            //业务清单合成文件源触发的
            msg.mWhat = C_MMonitorHandler::MSG_DOBULIST_FILE;


        }
        else if ( strcmp ( eventAttr["Parameter"]["Dir"].asCString(),
                           otherAttr[MC_L_STALISTRIG].asCString()
                         ) == 0
                )
        {
            if ( 1 == prtDebugFlag )
                writeLog ( fullOtherLog, "%s|%s|%d:  业务清单提供情况文件源触发的[%s]\n",
                           __FILE__, __func__, __LINE__, eventAttr["Parameter"]["Dir"].asCString() );

            //业务清单提供情况文件源触发的
            msg.mWhat = C_MMonitorHandler::MSG_STATBULIST;


        }
        else if ( strcmp ( eventAttr["Parameter"]["Dir"].asCString(),
                           otherAttr[MC_L_YESHPATH].asCString()
                         ) == 0
                )
        {
            if ( 1 == prtDebugFlag )
                writeLog ( fullOtherLog, "%s|%s|%d:  III区健康报告合并成综合健康报告文件源触发的[%s]\n",
                           __FILE__, __func__, __LINE__, eventAttr["Parameter"]["Dir"].asCString() );

            //III区健康报告合并成综合健康报告文件源触发的
            msg.mWhat = C_MMonitorHandler::MSG_YESHPATH;


        }
        else if ( strcmp ( eventAttr["Parameter"]["Dir"].asCString(),
                           otherAttr[MC_L_UPHPATH].asCString()
                         ) == 0
                )
        {
            if ( 1 == prtDebugFlag )
                writeLog ( fullOtherLog, "%s|%s|%d:  ftp上传健康报告文件源触发的[%s]\n",
                           __FILE__, __func__, __LINE__, eventAttr["Parameter"]["Dir"].asCString() );

            //ftp上传健康报告文件源触发的
            msg.mWhat = C_MMonitorHandler::MSG_UPHEALTH;


        }
        else
        {
            msg.mWhat = C_MMonitorHandler::MSG_NULL;


        }


        msg.mArg1 = action;


    }
    else
    {
        msg.mWhat = C_MMonitorHandler::MSG_NULL;


    }


    msg.mMetaStr      = event;
    msg.mArg1         = action;
    msg.mAttr         = eventAttr;
    msg.setValid ( True );
    msg.mTarget       = cHandler_p;
    msg.mTargetLooper = cLooper_p;

    cHandler_p->sendMessage ( msg );

    return;

}


void C_MMonitor::threadHandler()
{
    Looper* me_p = Looper::getLooper();
    cLooper_p    = me_p;
    cLooper_p->prepare();
    cLooper_p->Loop();

}

C_MMonitor* C_MMonitor::Instance()
{
    if ( cInstance_p == NULL )
    {
        cInstance_p = new C_MMonitor;
    }

    return cInstance_p;
}



















