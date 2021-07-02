/**********************************************************
* name:       mete_errHandling.cpp
*
* desc:      气象下载器的，差错处理
*
* date:      20170814095257
*
***********************************************************/

#include "mete_errHandling.h"
#include "mete_healthRep.h"
#include "mete_init.h"
#include "funcLib.h"


//using namespace tinyxml2;



C_MErrHandling* C_MErrHandling::cInstance_p = NULL;

C_MErrHandling::C_MErrHandlingHandler::C_MErrHandlingHandler ( C_MErrHandling* errHandObj_p )
{
    cErrHandObj_p = errHandObj_p;
}

C_MErrHandling::C_MErrHandlingHandler::~C_MErrHandlingHandler()
{

}

void C_MErrHandling::C_MErrHandlingHandler::handleMessage ( Message msg )
{
    char vRetMsg[MC_ERRMSG_MAX];
    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );

    //    int vRet = 0;
    switch ( msg.mWhat )
    {
        case PARSE_ERRADD_ITEM://新错误信息处理
        {

            cErrHandObj_p->doingFlag = 1;
            cErrHandObj_p->oneceFlag = 1;
            //上报状态 生成健康报告
            if ( msg.mAttr.isMember ( "validHllth" ) &&
                 msg.mAttr["validHllth"].asInt() == 1 )
            {
                if ( msg.mAttr.isMember ( "healthVal" ) )
                {

                    C_MHealthRep::Instance()->parseMsg ( msg.mAttr["healthVal"] );
                }
                else
                {
                    writeLog ( cErrHandObj_p->fullErrLog,
                               "%s|%s|%d: %s,msg.mAttr=[%s]\n",
                               __FILE__, __func__, __LINE__, "msg.mAttr[healthVal] is null",
                               msg.mAttr.toStyledString().c_str() );

//                    writeLog ( cErrHandObj_p->fullErrLog,
//                               "%s|%s|%d: %s\n",
//                               __FILE__, __func__, __LINE__, "msg.mAttr[healthVal] is null"
//                             );
                }
            }

            //存在入本地数据库

            //记入log
            if ( msg.mAttr.isMember ( "retMsg" ) && msg.mAttr.isMember ( "retCode" ) )
            {
                if ( msg.mAttr["retCode"].asInt() != METE_SUCCS_DOWNADO )
                {
                    writeLog ( cErrHandObj_p->fullErrLog,
                               "%s|%s|%d: retCode=[%d],%s\n",
                               __FILE__, __func__, __LINE__, 
                               msg.mAttr["retCode"].asInt(),
                               msg.mAttr["retMsg"].asCString()
                             );
                }
            }
            else
            {
                writeLog ( cErrHandObj_p->fullErrLog,
                           "%s|%s|%d: %s,msg.mAttr=[%s]\n",
                           __FILE__, __func__, __LINE__, "msg.mAttr[retMsg] is null",
                           msg.mAttr.toStyledString().c_str() );

//                writeLog ( cErrHandObj_p->fullErrLog,
//                           "%s|%s|%d: %s\n",
//                           __FILE__, __func__, __LINE__, "null"
//                         );
            }


        }
        break;

        default:
            break;
    }

    cErrHandObj_p->doingFlag = 0;

    return;
}


C_MErrHandling::C_MErrHandling()
{

    cHandler_p = NULL;
    cLooper_p  = NULL;


}

C_MErrHandling::~C_MErrHandling()
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

void C_MErrHandling::init()
{
    char vRetMsg[MC_ERRMSG_MAX];
    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );

    if ( cHandler_p == NULL )
    {
        cHandler_p = new C_MErrHandlingHandler ( this );
    }
    else
    {
        return;
    }

    doingFlag = 0;
    oneceFlag = 0;
    memset ( &retDesc, 0, sizeof ( retDesc ) );
    memset ( fullErrLog, 0, sizeof ( fullErrLog ) );
    memset ( fullOtherLog, 0, sizeof ( fullOtherLog ) );

    //获取是否打印debug配置 及日志文件配置
    prtDebugFlag = C_MSysInit::Instance()->getPrtFlag();
    snprintf ( fullErrLog, sizeof ( fullErrLog ),
               "%s",
               C_MSysInit::Instance()->getMeteLog().fullErrLog );
    snprintf ( fullOtherLog, sizeof ( fullOtherLog ),
               "%s",
               C_MSysInit::Instance()->getMeteLog().fullOtherLog );

    //    int vRet = 0;

    return;
}


void C_MErrHandling::start()
{

    init();

    startThread();

    return;
}

void C_MErrHandling::stop()
{
    stopThread();
    delete this;
}

bool C_MErrHandling::parseMsg ( const  Json::Value&  attr )
{
    char vRetMsg[MC_ERRMSG_MAX];
    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );

    if ( attr.isMember ( "retCode" ) )
    {
        T_xmlStatDescItem tRetDesc;
        memset( &tRetDesc, 0, sizeof(tRetDesc) );

        snprintf ( tRetDesc.retCode, 
                sizeof ( tRetDesc.retCode ),
                "%d",
                attr["retCode"].asInt() 
                );

        if ( attr.isMember ( "retMsg" ) )
        {
            snprintf ( tRetDesc.retMsg, 
                    sizeof ( tRetDesc.retMsg ),
                    "%s",
                    attr["retMsg"].asCString() 
                    );
        }
        else
        {
            strcpy( tRetDesc.retMsg, "null" );
        }
        char tmpTime[MC_TIMES_LEN];
        memset ( tmpTime, 0, sizeof( tmpTime ) );
        getTime ( 4, tmpTime, NULL, vRetMsg );//%Y-%m-%d:%H-%M-%S 
        snprintf ( tRetDesc.retTime, 
                sizeof ( tRetDesc.retTime ),
                "%s",
                tmpTime 
                );

        convertRetInfo( &tRetDesc );

    }
    //attr 中需要有 cfgFlag healthType taskNum beWatchedN beWatchedS
    Message msg;
    msg.mAttr         = attr;
    msg.mTarget       = cHandler_p;
    msg.mTargetLooper = cLooper_p;
    msg.setValid ( True );

    msg.mWhat         = C_MErrHandlingHandler::PARSE_ERRADD_ITEM;


    cHandler_p->sendMessage ( msg );

    return True;

}

T_xmlStatDescItem* C_MErrHandling::convertRetInfo( T_xmlStatDescItem* retDesc_p)
{
    if ( retDesc_p == NULL )
    {
        char vRetMsg[MC_ERRMSG_MAX];
        memset ( vRetMsg, 0, sizeof ( vRetMsg ) );

        char tmpTime[MC_TIMES_LEN];
        memset ( tmpTime, 0, sizeof( tmpTime ) );
        getTime ( 4, tmpTime, NULL, vRetMsg );//%Y-%m-%d:%H-%M-%S 
        if ( strlen( retDesc.retTime ) == 0 
            || strncmp( retDesc.retTime, tmpTime, 10 ) != 0 
            )
        {
            
            snprintf ( retDesc.retTime, 
                    sizeof ( retDesc.retTime ),
                    "%s",
                    tmpTime 
                    );

            snprintf ( retDesc.retCode, 
                    sizeof ( retDesc.retCode ),
                    "%d",
                    METE_NOFILE_DOWN 
                    );

            snprintf ( retDesc.retMsg, 
                    sizeof ( retDesc.retMsg ),
                    "%s",
                    "状态正常，但没有要下载的文件" 
                    );

//            writeLog ( fullErrLog,
//                       "%s|%s|%d: -----retCode=[%s],%s\n",
//                       __FILE__, __func__, __LINE__, 
//                       retDesc.retCode,
//                       retDesc.retMsg
//                     );

        }
        
    }
    else
    {
        strcpy( retDesc.retTime,retDesc_p->retTime );
        int tIntVal = 0;
        if ( strlen( retDesc.retCode ) != 0 )
        {
            tIntVal = atoi ( retDesc.retCode );

        }
        int tIntValInput = 0;
        if ( strlen( retDesc_p->retCode ) > 0 )
        {
            tIntValInput = atoi ( retDesc_p->retCode );
        }

//        writeLog ( fullErrLog,
//                   "%s|%s|%d: --1---retCode=[%s],retDesc_p->retCode=[%s],"
//                   "tIntVal[%d],tIntValInput[%d]\n",
//                   __FILE__, __func__, __LINE__, 
//                   retDesc.retCode,
//                   retDesc_p->retCode,
//                   tIntVal,tIntValInput
//                 );
        
        char tmpMsg_a[MC_RETMSG_LEN];
        memset( tmpMsg_a, 0, sizeof( tmpMsg_a ) );

        if ( tIntValInput < 0 || tIntVal < 0 )
        {
            tIntVal = tIntValInput;
            strcpy( tmpMsg_a, retDesc_p->retMsg );
        }
        else
        {
            if ( ( tIntVal & tIntValInput ) != tIntValInput )
            {
                snprintf ( tmpMsg_a,
                        sizeof( tmpMsg_a ),
                        "%s,%s",
                        retDesc.retMsg ,retDesc_p->retMsg );
                
            }
            tIntVal = ( tIntVal | tIntValInput );
        }

        snprintf ( retDesc.retCode, 
                sizeof ( retDesc.retCode ),
                "%d",
                tIntVal 
                );
        //strcpy( retDesc.retCode,retDesc_p->retCode );
        snprintf ( retDesc.retMsg, 
                sizeof ( retDesc.retMsg ),
                "%s",
                tmpMsg_a 
                );
        //strcpy( retDesc.retMsg,retDesc_p->retMsg );

//        writeLog ( fullErrLog,
//                   "%s|%s|%d: --2---retCode=[%s],retDesc_p->retCode=[%s],"
//                   "tIntVal[%d],tIntValInput[%d]\n",
//                   __FILE__, __func__, __LINE__, 
//                   retDesc.retCode,
//                   retDesc_p->retCode,
//                   tIntVal,tIntValInput
//                );
    }

    return &retDesc;
}

void C_MErrHandling::threadHandler()
{
    Looper* me_p = Looper::getLooper();
    cLooper_p = me_p;
    cLooper_p->prepare();
    cLooper_p->Loop();

}

C_MErrHandling* C_MErrHandling::Instance()
{
    if ( cInstance_p == NULL )
    {
        cInstance_p = new C_MErrHandling ;
    }

    return cInstance_p;
}



















