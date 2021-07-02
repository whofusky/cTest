/**********************************************************
* name:       mete_synBusList.cpp
*
* desc:      气象下载器的，合成业务清单
*
* date:      20170731164525
*
***********************************************************/

#include "mete_synBusList.h"

#include "funcLib.h"
#include "mete_init.h"
#include "mete_errHandling.h"
#include "hlthRprtWrtXml.h"

//using namespace tinyxml2;


C_MFsyBList* C_MFsyBList::cInstance_p = NULL;

C_MFsyBList::C_MFsyBListHandler::C_MFsyBListHandler ( C_MFsyBList* syBList_p )
{
    cSyBList_p = syBList_p;
}

C_MFsyBList::C_MFsyBListHandler::~C_MFsyBListHandler()
{

}

void C_MFsyBList::C_MFsyBListHandler::handleMessage ( Message msg )
{
    Json::Value  tmpAttr;
    tmpAttr["healthVal"] = cSyBList_p->healthVal;

    char vRetMsg[MC_ERRMSG_MAX];
    char vTmpMsg[MC_ERRMSG_MAX * 2];
    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    memset ( vTmpMsg, 0, sizeof ( vTmpMsg ) );

    int vRet = 0;

    switch ( msg.mWhat )
    {
        case PARSE_APPADD_FILE:
        {
            //取消息中文件名(如果带路径去掉路径)
            //            char tmpFileN[MC_NAME_LEN] = {};
            //            vRet=sptDirName(msg.mAttr["Path"].asCString(),
            //                NULL,
            //                tmpFileN,
            //                C_MSysInit::Instance()->getMeteLog().fullErrLog,
            //                2,
            //                );
            //            printf("%s|%s|%d:vRet[%d],[%s]\n",
            //                __FILE__,__func__,__LINE__, vRet,msg.mAttr["Path"].asCString());
            vRet = synthInList ( C_MSysInit::Instance()->getDoc(),
                                 msg.mAttr["Parameter"]["FileName"].asCString(),
                                 vRetMsg
                               );

            if ( vRet != 0 )
            {
                snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                           "%s|%s|%d:synthInList return[%d]\n%s",
                           __FILE__, __func__, __LINE__, vRet, vRetMsg );
                char tmpp[20] = {};
                sprintf ( tmpp, "%d", vRet );

                tmpAttr["retCode"] = METE_ERROR_MERLIST;
                tmpAttr["retMsg"] = vTmpMsg;
                tmpAttr["validHllth"] = 1;
                tmpAttr["healthVal"]["beWatchedS"] = tmpp;
                cSyBList_p->sendError ( tmpAttr );

                return;

            }

        }
        break;

        default:
            break;
    }

    return;
}

C_MFsyBList::C_MFsyBList()
{
    cHandler_p = NULL;
    cLooper_p  = NULL;
}

C_MFsyBList::~C_MFsyBList()
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


void C_MFsyBList::sendError ( const Json::Value&  errMsg )
{
    //    Json::Value handMsg;
    //    handMsg["retCode"] = errMsg["retCode"];
    //    handMsg["retMsg"] = errMsg["retMsg"];
    //    handMsg["validHllth"] = errMsg["validHllth"];
    //    handMsg["healthVal"]=errMsg["healthVal"];

    C_MErrHandling::Instance()->parseMsg ( errMsg );

    return;
}

void C_MFsyBList::init()
{
    if ( cHandler_p == NULL )
    {
        cHandler_p = new C_MFsyBListHandler ( this );
    }
    else
    {
        return;
    }

    appName = MC_M_SYNBUSI_NAME;
    //    healthVal["cfgFlag"]=MC_TOHFROMVAR;
    healthVal["healthType"] = MC_FLAG_HLTH;
    healthVal["taskNum"] = "1";
    healthVal["beWatchedN"] = appName;

    memset ( fullOtherLog, 0, sizeof ( fullOtherLog ) );

    //获取是否打印debug配置
    prtDebugFlag = C_MSysInit::Instance()->getPrtFlag();
    snprintf ( fullOtherLog, sizeof ( fullOtherLog ),
               "%s",
               C_MSysInit::Instance()->getMeteLog().fullOtherLog );

    return;
}


void C_MFsyBList::start()
{

    init();

    startThread();

    return;
}

void C_MFsyBList::stop()
{
    stopThread();
    delete this;
}

bool C_MFsyBList::parseFile ( const  Json::Value&  attr )
{
    Message msg;
    msg.mWhat = C_MFsyBListHandler::PARSE_APPADD_FILE;
    msg.mAttr = attr;
    msg.mTarget = cHandler_p;
    msg.mTargetLooper = cLooper_p;
    msg.setValid ( True );

    cHandler_p->sendMessage ( msg );

    return True;

}

void C_MFsyBList::threadHandler()
{
    Looper* me_p = Looper::getLooper();
    cLooper_p = me_p;
    cLooper_p->prepare();
    cLooper_p->Loop();

}

C_MFsyBList* C_MFsyBList::Instance()
{
    if ( cInstance_p == NULL )
    {
        cInstance_p = new C_MFsyBList;
    }

    return cInstance_p;
}



















