/**********************************************************
* name:       mete_init.cpp
*
* desc:      气象下载器的，初始化
*
* date:      20170803100107
*
***********************************************************/

#include "mete_init.h"
#include "funcLib.h"


//using namespace tinyxml2;


C_MSysInit* C_MSysInit::cInstance_p = NULL;


C_MSysInit::C_MSysInit()
{
    prtDebugFlag = 0;

}

C_MSysInit::~C_MSysInit()
{

}

void C_MSysInit::setPrtFlag ( int iprtFlag )
{
    prtDebugFlag = iprtFlag;
}

const int& C_MSysInit::getPrtFlag()
{
    return prtDebugFlag;
}

const T_meteLogs& C_MSysInit::getMeteLog()
{
    return meteLogs;
}

const char*  C_MSysInit::getXmlName()
{
    return locaXmlName;
}

const XMLDocument&  C_MSysInit::getDoc()
{
    return docLxml;
}

void C_MSysInit::init()
{
    memset ( &meteLogs, 0, sizeof ( T_meteLogs ) );
    memset ( locaXmlName, 0, sizeof ( locaXmlName ) );
    memset ( fullOtherLog, 0, sizeof ( fullOtherLog ) );

    //prtDebugFlag = 0;//打印错误日志flag, 0 打印,其他值 不打印
    char vRetMsg[MC_ERRMSG_MAX];
    memset ( vRetMsg, 0, sizeof ( vRetMsg ) );

    int vRet = 0;
    vRet = iniSys ( &meteLogs,
                    locaXmlName,
                    vRetMsg,
                    1,
                    &docLxml
                  );

    if ( vRet != 0 )
    {
        printf ( "%s|%s|%d:  iniSys vRet=[%d]\n%s",
                 __FILE__, __func__, __LINE__, vRet, vRetMsg );
        return;
    }

    //获取是否打印debug配置及日志文件配置
    prtDebugFlag = meteLogs.debugFlag;
    snprintf ( fullOtherLog, sizeof ( fullOtherLog ),
               "%s",
               meteLogs.fullOtherLog );

    vRet = getDoDirFromL ( docLxml,
                          MC_L_TOANTI,
                          toAnti_p,
                          vRetMsg
                          );
    if ( vRet != 0 )
    {
        printf ( "%s|%s|%d: getDoDirFromL %s  vRet=[%d]\n%s",
                 __FILE__, __func__, __LINE__, MC_L_TOANTI,
                 vRet, vRetMsg );
        return;
    }
    
    vRet = getDoDirFromL ( docLxml,
                          MC_L_TOANTIBAK,
                          toAntiBak_p,
                          vRetMsg
                          );
    if ( vRet != 0 )
    {
        printf ( "%s|%s|%d: getDoDirFromL %s  vRet=[%d]\n%s",
                 __FILE__, __func__, __LINE__, MC_L_TOANTIBAK,
                 vRet, vRetMsg );
        return;
    }
 
    return;

}


//void C_MSysInit::start()
//{
//
//    init();
//
//}


//void C_MSysInit::stop()
void C_MSysInit::clear()
{
    docLxml.Clear();
    delete this;
}


C_MSysInit* C_MSysInit::Instance()
{
    if ( cInstance_p == NULL )
    {
        cInstance_p = new C_MSysInit();
    }

    return cInstance_p;

}



















