/**********************************************************
* name:       mete_ftpPut.cpp
*
* desc:      气象下载器的，上传健康报告
*
* date:      20170731192249
*
***********************************************************/

#include "mete_ftpPut.h"
#include "mete_errHandling.h"
#include "funcLib.h"
#include "mete_init.h"
#include "hlthRprtWrtXml.h"



//using namespace tinyxml2;


C_MPutFtp* C_MPutFtp::cInstance_p = NULL;

C_MPutFtp::C_MPutFtpHandler::C_MPutFtpHandler ( C_MPutFtp* putFtp_p )
{
    cPutFtp_p = putFtp_p;
}

C_MPutFtp::C_MPutFtpHandler::~C_MPutFtpHandler()
{

}

void C_MPutFtp::C_MPutFtpHandler::handleMessage ( Message msg )
{
    Json::Value  tmpAttr;
    tmpAttr["healthVal"] = cPutFtp_p->healthVal;

    char vRetMsg[MC_ERRMSG_MAX];
    char vTmpMsg[MC_ERRMSG_MAX * 2];
    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    memset ( vTmpMsg, 0, sizeof ( vTmpMsg ) );

    int vRet = 0;

    switch ( msg.mWhat )
    {
        case PARSE_APPADD_FILE:
        {
            cPutFtp_p->doingFlag = 1;
            //msg.mAttr["Path"].asCString()
            if ( cPutFtp_p->numFtpSer > 0 )
            {
                for ( int i = 0; i < cPutFtp_p->numFtpSer; i++ )
                {
                    if ( cPutFtp_p->validFlag[i] == 1 )
                    {
                        //取消息中文件名(如果带路径去掉路径) 给ftpParas结构
                        memset ( cPutFtp_p->ftpParas[i].fileNames,
                                 0,
                                 sizeof ( cPutFtp_p->ftpParas[i].fileNames )
                               );
                        //                        sptDirName(msg.mAttr["Path"].asCString(),
                        //                                   NULL,
                        //                                   cPutFtp_p->ftpParas[i].fileNames,
                        //                                   C_MSysInit::Instance()->getMeteLog().fullErrLog,
                        //                                   2,
                        //                                   );
                        snprintf ( cPutFtp_p->ftpParas[i].fileNames,
                                   sizeof ( cPutFtp_p->ftpParas[i].fileNames ),
                                   "%s",
                                   msg.mAttr["Parameter"]["FileName"].asCString()
                                 );

                        vRet = putFtpFiles ( cPutFtp_p->shNames[i],
                                             cPutFtp_p->ftpParas[i],
                                             vRetMsg
                                           );

                        if ( vRet != 0 )
                        {
                            snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                                       "%s|%s|%d:putFtpFiles return[%d]\n%s",
                                   __FILE__, __func__, __LINE__, vRet, vRetMsg );
                            char tmpp[20] = {};
                            sprintf ( tmpp, "%d", vRet );
                            tmpAttr["healthVal"]["beWatchedS"] = tmpp;

                            tmpAttr["retCode"]  = METE_ERROR_FTPPUT;
                            tmpAttr["retMsg"]   = vTmpMsg;
                            tmpAttr["validHllth"] = 1;
                            tmpAttr["healthVal"]["beWatchedS"] = tmpp;
                            cPutFtp_p->sendError ( tmpAttr );

                        }
                    }
                }
            }

        }
        break;

        default:
            break;
    }

    cPutFtp_p->doingFlag = 0;

    return;
}

C_MPutFtp::C_MPutFtp()
{
    numFtpSer = 0;
    cHandler_p              = NULL;

}

C_MPutFtp::~C_MPutFtp()
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

void C_MPutFtp::sendError ( const Json::Value&  errMsg )
{
    //    Json::Value handMsg;
    //    handMsg["retCode"] = errMsg["retCode"];
    //    handMsg["retMsg"] = errMsg["retMsg"];
    //    handMsg["validHllth"] = errMsg["validHllth"];
    //    handMsg["healthVal"]=errMsg["healthVal"];

    C_MErrHandling::Instance()->parseMsg ( errMsg );

    return;
}


void C_MPutFtp::init()
{
    Json::Value  tmpAttr;
    char vRetMsg[MC_ERRMSG_MAX];
    char vTmpMsg[MC_ERRMSG_MAX * 2];
    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    memset ( vTmpMsg, 0, sizeof ( vTmpMsg ) );

    if ( cHandler_p == NULL )
    {
        cHandler_p = new C_MPutFtpHandler ( this );
    }
    else
    {
        return;
    }

    doingFlag = 0;
    memset ( ftpParas, 0, sizeof ( T_ftpParas ) *MC_FTPSERNUM_MAX );
    memset ( shNames, 0, sizeof ( T_doFtpSh ) *MC_FTPSERNUM_MAX );
    memset ( validFlag, 0, sizeof ( validFlag ) );
    memset ( fullOtherLog, 0, sizeof ( fullOtherLog ) );

    appName                 = MC_M_UPFTP_NAME;
    //healthVal["cfgFlag"]    = MC_TOHFROMVAR;
    healthVal["healthType"] = MC_FLAG_HLTH;
    healthVal["taskNum"]    = "1";
    healthVal["beWatchedN"] = appName;

    int vRet = 0;

    //获取是否打印debug配置 及日志文件配置
    prtDebugFlag = C_MSysInit::Instance()->getPrtFlag();
    snprintf ( fullOtherLog, sizeof ( fullOtherLog ),
               "%s",
               C_MSysInit::Instance()->getMeteLog().fullOtherLog );

    //取ftp server 数量
    vRet = getFserNumFxml ( C_MSysInit::Instance()->getDoc(),
                            MC_L_PUTUP,
                            numFtpSer,
                            vRetMsg
                          );

    if ( vRet != 0 )
    {
        snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                   "%s|%s|%d:getFserNumFxml return vRet=[%d]\n%s",
                   __FILE__, __func__, __LINE__, vRet, vRetMsg );
        tmpAttr["retCode"] = METE_ERROR_PARSECFG;
        tmpAttr["retMsg"] = vTmpMsg;
        tmpAttr["validHllth"] = 0;
        tmpAttr["healthVal"] = healthVal;
        sendError ( tmpAttr );

        return;
    }

    if ( numFtpSer > MC_FTPSERNUM_MAX ) //最大ftp server数量
    {
        snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                   "%s|%s|%d:ftp server numbers [%d] errors,the maximum is  [%d]\n",
                   __FILE__, __func__, __LINE__, numFtpSer, MC_FTPSERNUM_MAX );
        tmpAttr["retCode"] = METE_ERROR_CFGVALUE;
        tmpAttr["retMsg"] = vTmpMsg;
        tmpAttr["validHllth"] = 0;
        tmpAttr["healthVal"] = healthVal;
        sendError ( tmpAttr );

        return;
    }

    if ( numFtpSer > 0 )
    {

        T_ftpParas tftpParas = {};
        T_doFtpSh  tshNames  = {};

        //取每一个ftp server的配置进行下载
        for ( int i = 0; i < numFtpSer; i++ )
        {
            memset ( &tftpParas, 0, sizeof ( T_ftpParas ) );
            memset ( &tshNames, 0, sizeof ( T_doFtpSh ) );
            vRet = loadFtpCfg ( C_MSysInit::Instance()->getDoc(),
                                MC_L_PUTUP,
                                i + 1,
                                tshNames,
                                tftpParas,
                                vRetMsg
                              );

            if ( vRet != 0 )
            {
                snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                           "%s|%s|%d:loadFtpCfg i=[%d] return vRet[%d]\n%s",
                           __FILE__, __func__, __LINE__, i, vRet, vRetMsg );
                tmpAttr["retCode"] = METE_ERROR_PARSECFG;
                tmpAttr["retMsg"] = vTmpMsg;
                tmpAttr["validHllth"] = 0;
                tmpAttr["healthVal"] = healthVal;
                sendError ( tmpAttr );

                return;
            }
            else
            {
                validFlag[i] = 1;
                memcpy ( & ( ftpParas[i] ), &tftpParas, sizeof ( T_ftpParas ) );
                memcpy ( & ( shNames[i] ), &tshNames, sizeof ( T_ftpParas ) );

            }

        }

    }

    return;

}


void C_MPutFtp::start()
{

    init();

    startThread();

    return;

}

void C_MPutFtp::stop()
{
    stopThread();
    delete this;
}

bool C_MPutFtp::parseFile ( const  Json::Value&  attr )
{
    Message msg;
    msg.mWhat         = C_MPutFtpHandler::PARSE_APPADD_FILE;
    msg.mAttr         = attr;
    msg.mTarget       = cHandler_p;
    msg.mTargetLooper = cLooper_p;
    msg.setValid ( True );

    cHandler_p->sendMessage ( msg );

    return True;

}

void C_MPutFtp::threadHandler()
{
    Looper* me_p = Looper::getLooper();
    cLooper_p    = me_p;
    cLooper_p->prepare();
    cLooper_p->Loop();

}

C_MPutFtp* C_MPutFtp::Instance()
{
    if ( cInstance_p == NULL )
    {
        cInstance_p = new C_MPutFtp;
    }

    return cInstance_p;
}



















