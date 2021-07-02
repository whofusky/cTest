/**********************************************************
* name:       mete_ftpDown.cpp
*
* desc:      气象下载器的，ftp下载
*
* date:      20170727163539
*
***********************************************************/

#include "mete_ftpDown.h"
#include "funcLib.h"
#include "mete_init.h"
#include "mete_errHandling.h"
#include "hlthRprtWrtXml.h"


//using namespace tinyxml2;


C_MFtpDown* C_MFtpDown::cInstance_p = NULL;

C_MFtpDown::C_DownFtpTimer::C_DownFtpTimer (
C_MFtpDown* mFtpDown_p,
C_MFtpDownHandler* handler_p,
Message msg,
bool autoRelaseFlag ) : CTimer ( autoRelaseFlag )
{
    cMsg = msg;
    cHandler_p = handler_p;
    cMFtpDown_p = mFtpDown_p;
}


C_MFtpDown::C_DownFtpTimer::~C_DownFtpTimer()
{
    if ( 1 == cMFtpDown_p->prtDebugFlag )
    {
        writeLog ( cMFtpDown_p->fullOtherLog,
                   "%s|%s|%d:[C_DownFtpTimer]###"
                   "~C_DownFtpTimer\n",
                   __FILE__, __func__, __LINE__ );
    }
}

void C_MFtpDown::C_DownFtpTimer::timerHandler()
{
    int vRet = 0;
    char vTmpMsg[MC_ERRMSG_MAX];
    memset ( vTmpMsg, 0, sizeof ( vTmpMsg ) );

    Json::Value  tmpAttr;
    tmpAttr["healthVal"] = cMFtpDown_p->healthVal;

    //查询服务器是否有文件，有且下载程序未运行则发消息
    //是否有下载线程正在下载
    if ( cMFtpDown_p->downingFlag == 1 )
    {
        if ( 1 == cMFtpDown_p->prtDebugFlag )
        {
            writeLog ( cMFtpDown_p->fullOtherLog,
                       "%s|%s|%d:There are currently download"
                       " thread is downloading files,this "
                       "operation is ignored!\n",
                       __FILE__, __func__, __LINE__ );
        }
    }
    else//没有下载线程在下载
    {
        //查询服务器是及本地状态
        if ( cMFtpDown_p->numFtpSer > 0 )
        {

            //取每一个ftp server的配置进行下载
            for ( int i = 0; i < cMFtpDown_p->numFtpSer; i++ )
            {
                if ( cMFtpDown_p->validFlag[i] == 1 )
                {
                    T_doFtpSh* tShNames_p = & ( cMFtpDown_p->shNames[i] );
                    T_ftpParas* tFtpParas_p = & ( cMFtpDown_p->ftpParas[i] );

                    char tStr[1024];
                    char tStrRet[100];
                    memset ( tStr, 0, sizeof ( tStr ) );
                    memset ( tStrRet, 0, sizeof ( tStrRet ) );
                    snprintf ( tStr, sizeof ( tStr ),
                               "%s %d %d %s %s %s %s %s %s %d",
                               tShNames_p->chkShName,
                               tFtpParas_p->trsType,
                               tFtpParas_p->trsMode,
                               tFtpParas_p->serverIP,
                               tFtpParas_p->userName,
                               tFtpParas_p->userPwd,
                               tFtpParas_p->remoteDir,
                               tFtpParas_p->localDir,
                               tFtpParas_p->fileNames,
                               tFtpParas_p->portN
                             );
                    //用ftp shell 脚本进行服务器状态检查
                    vRet = system_plus ( tStr,
                                         /*cMFtpDown_p->second秒之后将kill脚本*/
                                         cMFtpDown_p->second,
                                         2,  /*超时后将尝试2次*/
                                         1,  /*超时后将kill脚本及脚本派生的进程*/
                                         tStrRet,
                                         sizeof ( tStrRet )
                                       );

                    /*vRet = executeShell(tShNames_p->chkShName,
                                        "%s %s %s %s %s %s %d",
                                        tFtpParas_p->serverIP,
                                        tFtpParas_p->userName,
                                        tFtpParas_p->userPwd,
                                        tFtpParas_p->remoteDir,
                                        tFtpParas_p->localDir,
                                        tFtpParas_p->fileNames,
                                        tFtpParas_p->portN
                                        );
                     */
                    //                    printf("%s|%s|%d: %s vRet=[%d]\n",
                    //                        __FILE__,__func__,__LINE__,shNames.chkShName,vRet);
                    if ( vRet == 0 )
                    {


                        cMsg.mTargetLooper = cMFtpDown_p->cLooper_p;

                        Message tMsg = cMsg;



                        tMsg.mAttr["chkShName"] = tShNames_p->chkShName;
                        tMsg.mAttr["doShName"]  = tShNames_p->doShName;
                        tMsg.mAttr["trsType"]  = tFtpParas_p->trsType;
                        tMsg.mAttr["trsMode"]  = tFtpParas_p->trsMode;
                        tMsg.mAttr["serverIP"]  = tFtpParas_p->serverIP;
                        tMsg.mAttr["portN"]     = tFtpParas_p->portN;
                        tMsg.mAttr["userName"]  = tFtpParas_p->userName;
                        tMsg.mAttr["userPwd"]   = tFtpParas_p->userPwd;
                        tMsg.mAttr["remoteDir"] = tFtpParas_p->remoteDir;
                        tMsg.mAttr["localDir"]  = tFtpParas_p->localDir;
                        tMsg.mAttr["fileNames"] = tFtpParas_p->fileNames;
                        tMsg.mAttr["outTimeMs"] = tFtpParas_p->outTimeMs;
                        tMsg.mAttr["cmpDir"]    = tFtpParas_p->cmpDir;

                        tMsg.setValid ( True );
                        cHandler_p->sendMessage ( tMsg );

                    }
                    else if ( vRet == 1 ) //无法连接到ftp服务器
                    {
                        snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                                   "%s|%s|%d:[%s] connet ftp server"
                                   "error\n", __FILE__, __func__, __LINE__,
                                   tShNames_p->chkShName );
                        tmpAttr["retCode"]    = METE_ERROR_CONFSER;
                        tmpAttr["retMsg"]     = vTmpMsg;
                        tmpAttr["validHllth"] = 1;
                        cMFtpDown_p->sendError ( tmpAttr );
                    }
                    else if ( vRet == 2 ) //用户名或密码错误
                    {
                        snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                                   "%s|%s|%d:[%s] the ftp username"
                                   "or passwd error\n",
                                   __FILE__, __func__, __LINE__,
                                   tShNames_p->chkShName );
                        tmpAttr["retCode"] = METE_ERROR_UORPWD;
                        tmpAttr["retMsg"] = vTmpMsg;
                        tmpAttr["validHllth"] = 1;
                        cMFtpDown_p->sendError ( tmpAttr );

                    }
                    else if ( vRet == 3 ) //路径错误
                    {
                        snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                                   "%s|%s|%d:[%s] the file path"
                                   "error\n",
                                   __FILE__, __func__, __LINE__,
                                   tShNames_p->chkShName );
                        tmpAttr["retCode"]    = METE_ERROR_FSPATH;
                        tmpAttr["retMsg"]     = vTmpMsg;
                        tmpAttr["validHllth"] = 1;
                        cMFtpDown_p->sendError ( tmpAttr );

                    }
                    else if ( vRet == 4 ) //不存在要下载的文件
                    {
                        /*
                        snprintf( vTmpMsg,sizeof(vTmpMsg),
                           "%s|%s|%d:[%s] the file "
                            "is not exists\n",
                            __FILE__,__func__,__LINE__,
                            tShNames_p->chkShName );
                        tmpAttr["retCode"]    = vRet;
                        tmpAttr["retMsg"]     = vTmpMsg;
                        tmpAttr["validHllth"] = 1;
                        cMFtpDown_p->sendError(tmpAttr);
                        */
                    }
                    else if ( vRet == 5 ) //本地路径错误
                    {
                        snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                                   "%s|%s|%d:[%s] the local file "
                                   "path is error\n",
                                   __FILE__, __func__, __LINE__,
                                   tShNames_p->chkShName );
                        tmpAttr["retCode"]    = METE_ERROR_LCPATH;
                        tmpAttr["retMsg"]     = vTmpMsg;
                        tmpAttr["validHllth"] = 1;
                        cMFtpDown_p->sendError ( tmpAttr );

                    }
                    else //if ( vRet == -3 )
                    {
//                        if ( 1 == cMFtpDown_p->prtDebugFlag )
//                        {
//                            writeLog ( cMFtpDown_p->fullOtherLog,
//                                       "%s|%s|%d:system_plus,tStr[%s]"
//                                       ",vRet[%d],tStrRet[%s] \n",
//                                       __FILE__, __func__, __LINE__,
//                                       tStr, vRet, tStrRet );
//                        }
                        snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                                        "%s|%s|%d:system_plus,tStr[%s]"
                                       ",vRet[%d],tStrRet[%s] \n",
                                       __FILE__, __func__, __LINE__,
                                       tStr, vRet, tStrRet );

                        tmpAttr["retMsg"]     = vTmpMsg;
                        tmpAttr["validHllth"] = 1;


                        if ( vRet == DOSH_ERROR_OUTTIME ) //执行shell脚本超时
                        {
                            tmpAttr["retCode"]    = METE_ERROR_CHKDOUT;

                        }
                        else
                        {
                            tmpAttr["retCode"]    = METE_ERROR_CHKOTHER;
                        }
                        cMFtpDown_p->sendError ( tmpAttr );


                    }

                }

            }

        }


    }


}

C_MFtpDown::C_DownFtpTimer*
C_MFtpDown::C_DownFtpTimer::getObj (
C_MFtpDown* mFtpDown_p,
C_MFtpDownHandler* handler_p,
Message msg,
bool autoRelaseFlag
)
{

    return new C_DownFtpTimer ( mFtpDown_p, handler_p, msg, autoRelaseFlag );
}

C_MFtpDown::C_MFtpDownHandler::C_MFtpDownHandler ( C_MFtpDown* ftpDown_p )
{
    cFtpDown_p = ftpDown_p;

}

C_MFtpDown::C_MFtpDownHandler::~C_MFtpDownHandler()
{


}


void C_MFtpDown::C_MFtpDownHandler::handleMessage ( Message msg )
{
    int vRet = 0;
    Json::Value  tmpAttr;
    tmpAttr["healthVal"] = cFtpDown_p->healthVal;

    char vRetMsg[MC_ERRMSG_MAX];
    char vTmpMsg[MC_ERRMSG_MAX * 2];
    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    memset ( vTmpMsg, 0, sizeof ( vTmpMsg ) );

    switch ( msg.mWhat )
    {
        case METE_DOWN_MSG:
        {

            T_ftpParas ftpParas = {};
            T_doFtpSh  shNames = {};
            char xmlPath[MC_FULLN_LEN] = {};
            char errLogPath[MC_FULLN_LEN] = {};

            snprintf ( xmlPath,
                       sizeof ( xmlPath ),
                           "%s",
                       msg.mAttr["xmlPath"].asCString() );
            snprintf ( errLogPath,
                       sizeof ( errLogPath ),
                           "%s",
                       msg.mAttr["errLogPath"].asCString() );

            snprintf ( shNames.chkShName,
                       sizeof ( shNames.chkShName ),
                           "%s",
                       msg.mAttr["chkShName"].asCString() );
            snprintf ( shNames.doShName,
                       sizeof ( shNames.doShName ),
                           "%s",
                       msg.mAttr["doShName"].asCString() );

            snprintf ( ftpParas.serverIP,
                       sizeof ( ftpParas.serverIP ),
                           "%s",
                       msg.mAttr["serverIP"].asCString() );

            ftpParas.portN = msg.mAttr["portN"].asUInt();

            snprintf ( ftpParas.userName,
                       sizeof ( ftpParas.userName ),
                           "%s",
                       msg.mAttr["userName"].asCString() );
            snprintf ( ftpParas.userPwd,
                       sizeof ( ftpParas.userPwd ),
                           "%s",
                       msg.mAttr["userPwd"].asCString() );
            snprintf ( ftpParas.remoteDir,
                       sizeof ( ftpParas.remoteDir ),
                           "%s",
                       msg.mAttr["remoteDir"].asCString() );
            snprintf ( ftpParas.localDir,
                       sizeof ( ftpParas.localDir ),
                           "%s",
                       msg.mAttr["localDir"].asCString() );
            snprintf ( ftpParas.fileNames,
                       sizeof ( ftpParas.fileNames ),
                           "%s",
                       msg.mAttr["fileNames"].asCString() );

            ftpParas.outTimeMs = msg.mAttr["outTimeMs"].asUInt();
            ftpParas.trsType = msg.mAttr["trsType"].asInt();
            ftpParas.trsMode = msg.mAttr["trsMode"].asInt();

            snprintf ( ftpParas.cmpDir,
                       sizeof ( ftpParas.cmpDir ),
                           "%s",
                       msg.mAttr["cmpDir"].asCString() );

            cFtpDown_p->downingFlag = 1;
            vRet = downFtpFiles ( shNames,
                                  ftpParas,
                                  vRetMsg );
            cFtpDown_p->downingFlag = 0;

            if ( vRet == ERR_FTP_OUTTIME ) //超时退出
            {
                snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                           "%s|%s|%d:downFtpFiles return"
                           "[%d]\n%s",
                           __FILE__, __func__, __LINE__,
                           vRet, vRetMsg );
                tmpAttr["retCode"]    = METE_ERROR_GETFTPFILE;
                tmpAttr["retMsg"]     = vTmpMsg;
                tmpAttr["validHllth"] = 1;
                tmpAttr["healthVal"]["beWatchedS"] = "ftp time out";
                cFtpDown_p->sendError ( tmpAttr );



            }
            else if ( vRet != 0 && vRet != DFTP_ERROR_DUPFILE )
            {
                snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                           "%s|%s|%d:downFtpFiles return"
                           "[%d]\n%s",
                           __FILE__, __func__, __LINE__,
                           vRet, vRetMsg );
                char tmpp[20] = {};
                sprintf ( tmpp, "%d", vRet );

                tmpAttr["retCode"]    = METE_ERROR_GETOTHER;
                tmpAttr["retMsg"]     = vTmpMsg;
                tmpAttr["validHllth"] = 1;
                tmpAttr["healthVal"]["beWatchedS"] = tmpp;
                cFtpDown_p->sendError ( tmpAttr );
            }

            /*
            //fusk 测试添加 begine--->
            snprintf( vTmpMsg,sizeof(vTmpMsg),
                    "%s|%s|%d:downFtpFiles return[%d]\n%s",
                    __FILE__,__func__,__LINE__, vRet,vRetMsg);
                tmpAttr["retCode"]=vRet;
                tmpAttr["retMsg"]=vTmpMsg;
                tmpAttr["validHllth"]=1;
                tmpAttr["healthVal"]["beWatchedS"]="ftp time out";
                cFtpDown_p->sendError(tmpAttr);
            //fusk 测试添加 end--->
             */


        }
        break;

        default:
            break;



    }

}


C_MFtpDown::C_MFtpDown()
{
    cHandler_p = NULL;
    cTimer_p   = NULL;
    cLooper_p  = NULL;

    getPidNumPath_p = NULL;

    numFtpSer = 0;
    second    = 0;
    micSecond = 0;
    repet     = 0;

}

C_MFtpDown::~C_MFtpDown()
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

    if ( cTimer_p != NULL )
    {
        delete cTimer_p;
        cTimer_p = NULL;
    }

}
void C_MFtpDown::sendError ( const Json::Value&  errMsg )
{
    /*
    Json::Value handMsg;
    handMsg["retCode"] = errMsg["retCode"];
    handMsg["retMsg"] = errMsg["retMsg"];
    handMsg["validHllth"] = errMsg["validHllth"];
    handMsg["healthVal"]=errMsg["healthVal"];
    */

    C_MErrHandling::Instance()->parseMsg ( errMsg );

    return;
}

void C_MFtpDown::init()
{
    Json::Value  tmpAttr;
    int vRet = 0;
    char vRetMsg[MC_ERRMSG_MAX];
    char vTmpMsg[MC_ERRMSG_MAX * 2];
    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );
    memset ( vTmpMsg, 0, sizeof ( vTmpMsg ) );

    if ( cHandler_p == NULL )
    {
        cHandler_p = new C_MFtpDownHandler ( this );
    }
    else
    {
        return;
    }

    downingFlag = 0;

    memset ( ftpParas, 0, sizeof ( T_ftpParas ) *MC_FTPSERNUM_MAX );
    memset ( shNames, 0, sizeof ( T_doFtpSh ) *MC_FTPSERNUM_MAX );
    memset ( validFlag, 0, sizeof ( validFlag ) );
    memset ( fullOtherLog, 0, sizeof ( fullOtherLog ) );

    //健康报告相关值
    appName = MC_M_DOWNFTP_NAME;
    //healthVal["cfgFlag"]=MC_TOHFROMVAR;
    healthVal["healthType"] = MC_FLAG_HLTH;
    healthVal["taskNum"] = "1";
    healthVal["beWatchedN"] = appName;

    //获取是否打印debug配置 及日志文件配置
    prtDebugFlag = C_MSysInit::Instance()->getPrtFlag();
    snprintf ( fullOtherLog, sizeof ( fullOtherLog ),
               "%s",
               C_MSysInit::Instance()->getMeteLog().fullOtherLog );

    char ttStr[][MC_XMLELE_LEM] = { MC_L_TODO,
                                    MC_L_SHTOOL,
                                    MC_L_GETPIDNUM
                                  };
    T_linkXmlEles linkXmlEles = { ttStr, 3 };
    //取getPidNum.sh路径
    //char* eleVal_p = NULL;
    vRet = getFirstEleVal ( C_MSysInit::Instance()->getDoc(),
                            linkXmlEles,
                            getPidNumPath_p,
                            vRetMsg );

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

    //取ftp server 数量
    vRet = getFserNumFxml ( C_MSysInit::Instance()->getDoc(),
                            MC_L_GETDOWN,
                            numFtpSer,
                            vRetMsg
                          );

    if ( vRet != 0 )
    {
        snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                   "%s|%s|%d: getFserNumFxml return vRet[%d]\n%s",
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
                   "%s|%s|%d:ftp server numbers [%d]"
                   "errors,the maximum is  [%d]\n",
                   __FILE__, __func__, __LINE__,
                   numFtpSer, MC_FTPSERNUM_MAX );
        tmpAttr["retCode"]    = ERR_INPUT_OR_CONF;
        tmpAttr["retMsg"]     = vTmpMsg;
        tmpAttr["validHllth"] = 0;
        tmpAttr["healthVal"]  = healthVal;
        sendError ( tmpAttr );

        return;
    }

    vRet = getChkCycleFxml ( C_MSysInit::Instance()->getDoc(),
                             MC_L_GETDOWN,
                             second,
                             micSecond,
                             repet,
                             vRetMsg
                           );

    //    printf("%s|%s|%d:second[%d],micSecond[%d],repet[%d]\n",
    //        __FILE__,__func__,__LINE__, second,micSecond,repet);
    if ( vRet != 0 )
    {
        snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                   "%s|%s|%d:getChkCycleFxml return[%d]\n%s",
                   __FILE__, __func__, __LINE__, vRet, vRetMsg );
        tmpAttr["retCode"] = METE_ERROR_PARSECFG;
        tmpAttr["retMsg"] = vTmpMsg;
        tmpAttr["validHllth"] = 0;
        tmpAttr["healthVal"] = healthVal;
        sendError ( tmpAttr );

        return;
    }

    if ( repet == 0 )
    {
        snprintf ( vTmpMsg, sizeof ( vTmpMsg ),
                   "%s|%s|%d:repet value is error\n",
                   __FILE__, __func__, __LINE__ );
        tmpAttr["retCode"] = ERR_INPUT_OR_CONF;
        tmpAttr["retMsg"] = vTmpMsg;
        tmpAttr["validHllth"] = 0;
        tmpAttr["healthVal"] = healthVal;
        sendError ( tmpAttr );

        return;
    }

    if ( numFtpSer > 0 )
    {

        T_ftpParas tftpParas = {};
        T_doFtpSh  tshNames = {};

        //取每一个ftp server的配置进行下载
        for ( int i = 0; i < numFtpSer; i++ )
        {
            memset ( &tftpParas, 0, sizeof ( T_ftpParas ) );
            memset ( &tshNames, 0, sizeof ( T_doFtpSh ) );
            vRet = loadFtpCfg ( C_MSysInit::Instance()->getDoc(),
                                MC_L_GETDOWN,
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


void C_MFtpDown::start()
{
    //初始化相关变量
    init();

    startThread();


    Message msg;
    Json::Value msgAttr;

    msgAttr[MC_L_GETPIDNUM] = getPidNumPath_p;
    msgAttr["xmlPath"]      = C_MSysInit::Instance()->getXmlName();
    msgAttr["errLogPath"]   = C_MSysInit::Instance()->getMeteLog().fullErrLog;

    msg.mWhat               = METE_DOWN_MSG;
    msg.mAttr               = msgAttr;
    msg.mTarget             = cHandler_p;
    // msg.mTargetLooper = cLooper_p;
    bool autoReleaseFlag = 0;

    if ( repet < 0 )
    {
        autoReleaseFlag = False;
    }
    else
    {
        autoReleaseFlag = True;
    }

    C_DownFtpTimer* tTimer_p = C_DownFtpTimer::getObj ( this,
                               cHandler_p,
                               msg,
                               autoReleaseFlag
                                                      );
    tTimer_p->setTimer ( second, micSecond, repet );
    tTimer_p->startTimer();

    /*
    tTimer_p 的释放由 repet 决定:
          当 repet <0 定时器无限循环
              此时因 tTimer_p 自己不
              自动释放，所以需要外部
              释放
          当 repet>0  定时器只运行repet
              次，然后自动释放
    */
    if ( repet < 0 )
    {
        cTimer_p = tTimer_p;
    }

    return;

}


void C_MFtpDown::stop()
{
    stopThread();
    delete this;
}


C_MFtpDown* C_MFtpDown::Instance()
{
    if ( cInstance_p == NULL )
    {
        cInstance_p = new C_MFtpDown();
    }

    return cInstance_p;

}

void C_MFtpDown::threadHandler()
{
    Looper* me_p = Looper::getLooper();
    cLooper_p = me_p;
    cLooper_p->prepare();
    cLooper_p->Loop();

}


















