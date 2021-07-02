/**********************************************************
* name:       mete_statBusList.cpp
*
* desc:      气象下载器的，生成业务清单提供情况
*
* date:      20170731180033
*
***********************************************************/

#include "mete_statBusList.h"
#include "funcLib.h"
#include "mete_init.h"
#include "mete_errHandling.h"
#include "hlthRprtWrtXml.h"
#include "mete_healthRep.h"



C_MStatBList* C_MStatBList::cInstance_p = NULL;

C_MStatBList::C_MStatBListHandler::C_MStatBListHandler ( C_MStatBList* statBList_p )
{
    cStatBList_p = statBList_p;
}

C_MStatBList::C_MStatBListHandler::~C_MStatBListHandler()
{

}

void C_MStatBList::C_MStatBListHandler::handleMessage ( Message msg )
{
    Json::Value  tmpAttr;
    tmpAttr["healthVal"] = cStatBList_p->healthVal;

    char vRetMsg[MC_ERRMSG_MAX];
    char vTmpMsg[MC_ERRMSG_MAX * 2];
    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    memset ( vTmpMsg, 0, sizeof ( vTmpMsg ) );

    int vRet = 0;
    int waitMaxSec = 301;

    switch ( msg.mWhat )
    {
        case PARSE_APPADD_FILE:
        {
            //防止生成软件状态线程与本线程同时运行
            while ( C_MHealthRep::Instance()->doStatFlag == 1
                    && waitMaxSec > 1
                  )
            {
                waitMaxSec--;
                sleepSecond( 1 );
            }

            cStatBList_p->doingFlag = 1;
            //取消息中文件名(如果带路径去掉路径)
            //            char tmpFileN[MC_NAME_LEN] = {};
            //            sptDirName(msg.mAttr["Path"].asCString(),
            //                       NULL,
            //                       tmpFileN,
            //                       C_MSysInit::Instance()->getMeteLog().fullErrLog,
            //                       2,
            //                       );

            vRet = statList ( C_MSysInit::Instance()->getDoc(),
                              msg.mAttr["Parameter"]["FileName"].asCString(),
                              vRetMsg
                            );

            if ( vRet != 0 )
            {
                cStatBList_p->doingFlag = 0;
                snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                           "%s|%s|%d:statList return[%d]\n%s",
                           __FILE__, __func__, __LINE__, vRet, vRetMsg );
                char tmpp[20] = {};
                sprintf ( tmpp, "%d", vRet );

                tmpAttr["retCode"] = METE_ERROR_GENLSTSTAT;
                tmpAttr["retMsg"] = vTmpMsg;
                tmpAttr["validHllth"] = 1;
                tmpAttr["healthVal"]["beWatchedS"] = tmpp;
                cStatBList_p->sendError ( tmpAttr );

                return;

            }
            else
            {
                snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                           "气象下载，业务清单处理成功!"
                        );
                int tInt = METE_SUCCS_DOWNADO;
                tmpAttr["retCode"] = tInt;
                tmpAttr["retMsg"] = vTmpMsg;
                tmpAttr["validHllth"] = 0;
                cStatBList_p->sendError ( tmpAttr );

            }

        }
        break;

        default:
            break;
    }

    cStatBList_p->doingFlag = 0;

    return;
}

C_MStatBList::C_MStatBList()
{
    cHandler_p              = NULL;

}

C_MStatBList::~C_MStatBList()
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
}

void C_MStatBList::sendError ( const Json::Value&  errMsg )
{
    //    Json::Value handMsg;
    //    handMsg["retCode"] = errMsg["retCode"];
    //    handMsg["retMsg"] = errMsg["retMsg"];
    //    handMsg["validHllth"] = errMsg["validHllth"];
    //    handMsg["healthVal"]=errMsg["healthVal"];

    C_MErrHandling::Instance()->parseMsg ( errMsg );

    return;
}


void C_MStatBList::init()
{
    if ( cHandler_p == NULL )
    {
        cHandler_p = new C_MStatBListHandler ( this );
    }
    else
    {
        return;
    }

    doingFlag = 0;
    Json::Value  tmpAttr;
    char vRetMsg[MC_ERRMSG_MAX];
    char vTmpMsg[MC_ERRMSG_MAX * 2];
    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    memset ( vTmpMsg, 0, sizeof ( vTmpMsg ) );
    int vRet = 0;

    vRet = getDoDirFromL ( C_MSysInit::Instance()->getDoc(), 
                            MC_L_STALISTRIG, 
                            staLisTrig_p, 
                            vRetMsg 
                            );
    if ( vRet != 0 )
    {
        snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                   "%s|%s|%d: getDoDirFromL %s return vRet[%d]\n%s",
                  __FILE__, __func__, __LINE__, MC_L_STALISTRIG,
                  vRet, vRetMsg );
        tmpAttr["retCode"] = METE_ERROR_PARSECFG;
        tmpAttr["retMsg"] = vTmpMsg;
        tmpAttr["validHllth"] = 0;
        sendError ( tmpAttr );

        return;
    }


    vRet = getDoDirFromL ( C_MSysInit::Instance()->getDoc(), 
                            MC_L_DOSTAPATH, 
                            doStaPath_p, 
                            vRetMsg 
                            );
    if ( vRet != 0 )
    {
        snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                   "%s|%s|%d: getDoDirFromL %s return vRet[%d]\n%s",
                  __FILE__, __func__, __LINE__, MC_L_DOSTAPATH,
                  vRet, vRetMsg );
        tmpAttr["retCode"] = METE_ERROR_PARSECFG;
        tmpAttr["retMsg"] = vTmpMsg;
        tmpAttr["validHllth"] = 0;
        sendError ( tmpAttr );

        return;
    }

    vRet = getDoDirFromL ( C_MSysInit::Instance()->getDoc(), 
                            MC_L_DOSTANAME, 
                            doStaName_p, 
                            vRetMsg 
                            );
    if ( vRet != 0 )
    {
        snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                   "%s|%s|%d: getDoDirFromL %s return vRet[%d]\n%s",
                  __FILE__, __func__, __LINE__, MC_L_DOSTANAME,
                  vRet, vRetMsg );
        tmpAttr["retCode"] = METE_ERROR_PARSECFG;
        tmpAttr["retMsg"] = vTmpMsg;
        tmpAttr["validHllth"] = 0;
        sendError ( tmpAttr );

        return;
    }

    appName                 = MC_M_STATBUSI_NAME;
    //    healthVal["cfgFlag"]    = MC_TOHFROMVAR;
    healthVal["healthType"] = MC_FLAG_HLTH;
    healthVal["taskNum"]    = "1";
    healthVal["beWatchedN"] = appName;

    memset ( fullOtherLog, 0, sizeof ( fullOtherLog ) );

    //获取是否打印debug配置及日志文件配置
    prtDebugFlag = C_MSysInit::Instance()->getPrtFlag();
    snprintf ( fullOtherLog, sizeof ( fullOtherLog ),
               "%s",
               C_MSysInit::Instance()->getMeteLog().fullOtherLog );

    return;
}


void C_MStatBList::start()
{
    init();
    startThread();
    return;
}

void C_MStatBList::stop()
{
    stopThread();
    delete this;
}

bool C_MStatBList::parseFile ( const  Json::Value&  attr )
{
    Message msg;
    msg.mWhat         = C_MStatBListHandler::PARSE_APPADD_FILE;
    msg.mAttr         = attr;
    msg.mTarget       = cHandler_p;
    msg.mTargetLooper = cLooper_p;
    msg.setValid ( True );

    cHandler_p->sendMessage ( msg );

    return True;

}

void C_MStatBList::threadHandler()
{
    Looper* me_p = Looper::getLooper();
    cLooper_p    = me_p;
    cLooper_p->prepare();
    cLooper_p->Loop();

}

C_MStatBList* C_MStatBList::Instance()
{
    if ( cInstance_p == NULL )
    {
        cInstance_p = new C_MStatBList;
    }

    return cInstance_p;
}



















