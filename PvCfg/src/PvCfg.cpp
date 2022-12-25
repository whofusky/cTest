/***********************************************************
 *
 * @file    PvCfg.cpp
 *
 * @brief   PvCfg源文件
 *
 * @author  fu.sky
 *
 * @date    2022-11-01_08:45:27
 *
 * @version V10.010.000
 *
 ***********************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "PvCfg.h"


static pthread_mutex_t  gsMutex = PTHREAD_MUTEX_INITIALIZER;


//minInit中解析ini文件的回调函数
static int IniCallBack(
        const char *section, 
        const char *key, 
        const char *value, 
        void *userdata
        )
{
    Json::Value* g_cfgJ = (Json::Value*) userdata; 
    if ( strlen(section) == 0 )
    {
      (*g_cfgJ)["ROOT"][key] = value;
    }
    else
    {
      (*g_cfgJ)[section][key] = value;
    }

    return 1;
}




PvCfg* PvCfg::mInstance = NULL;
std::string PvCfg::mPubCfgPath = "";
std::string PvCfg::mPubCfgFile = "";
std::string PvCfg::mSftCfgPath = "";
std::string PvCfg::mSftCfgFile = "";


const char* PvCfg::_errorName[PVCFG_ERROR_COUNT+1]={
    "PVCFG_SUCCESS",
    "PVCFG_INPAR_ERROR",
    "PVCFG_PUBCFG_NOT_EXIST",
    "PVCFG_SFTCFG_NOT_EXIST",
    "PVCFG_SECTION_NOT_EXIST",
    "PVCFG_KEY_NOT_EXIST",
    ""
};


/*static*/ int PvCfg:: s_isreg( const char* path)
{
    struct stat statbuf;

    if ( stat(path, &statbuf) == -1 ) 
    {
        return 0;
    }

    if ( (statbuf.st_mode & S_IFMT) == S_IFREG ) //regular file
    {
        return 1;
    }

    return 0;
}


/*static*/ const char* PvCfg::ErrorIdName(int errorId)
{
    const char* name_p = _errorName[PVCFG_ERROR_COUNT];
    if ( errorId < 0 || errorId >= PVCFG_ERROR_COUNT )
    {
        return name_p;
    }
    name_p = _errorName[ errorId ];
    return name_p;
}


PvCfg::PvCfg()
{
    mPubRet = 0;
    mSftRet = 0;
    mPubCfg = Json::Value::null;
    mSftCfg = Json::Value::null;
}


PvCfg::~PvCfg()
{

}


int PvCfg::loadPubCfg()
{
    const char* pubCfgFile = GetPvPubFile( PV_PUBCFG_FILE );
    if ( s_isreg( pubCfgFile ) != 1 )
    {
        mPubRet = PVCFG_PUBCFG_NOT_EXIST;
        return mPubRet;
    }

    //mLock.Lock();
    pthread_mutex_lock( &gsMutex );
    ini_browse(IniCallBack, &mPubCfg, pubCfgFile);
    //mLock.unLock();
    pthread_mutex_unlock( &gsMutex );

    return PVCFG_SUCCESS;
}




int PvCfg::loadSftCfg()
{
    const char* sftCfgFile = GetPvRunPar( RUNPAR_CFGFILE );
    if ( s_isreg( sftCfgFile ) != 1 )
    {
        mSftRet = PVCFG_SFTCFG_NOT_EXIST;
        return mSftRet;
    }

    //CMutexGuard autoGuard( mLock );
    //mLock.Lock();
    pthread_mutex_lock( &gsMutex );
    ini_browse(IniCallBack, &mSftCfg, sftCfgFile);
    //mLock.unLock();
    pthread_mutex_unlock( &gsMutex );

    return PVCFG_SUCCESS;
}




int PvCfg::Initialize()
{
    int ret = loadPubCfg();
    if ( ret == PVCFG_SUCCESS )
    {
        ret = loadSftCfg();
    }

    return ret;
}




int PvCfg::getPubCfgVal(
        std::string &value, 
        const char* section, 
        const char* key
        )
{
    if ( section == NULL || key == NULL )
    {
        return PVCFG_INPAR_ERROR;
    }

    //CMutexGuard autoGuard( mLock );
    pthread_mutex_lock( &gsMutex );

    if ( ! mPubCfg.isMember(section) )
    {
        pthread_mutex_unlock( &gsMutex );
        return PVCFG_SECTION_NOT_EXIST;
    }

    if ( ! mPubCfg[section].isMember( key ) )
    {
        pthread_mutex_unlock( &gsMutex );
        return PVCFG_KEY_NOT_EXIST;
    }
    
    value = mPubCfg[section][key].asString();

    pthread_mutex_unlock( &gsMutex );
    return PVCFG_SUCCESS;
}




int PvCfg::getSftCfgVal(
        std::string &value, 
        const char* section, 
        const char* key
        )
{
    if ( section == NULL || key == NULL )
    {
        return PVCFG_INPAR_ERROR;
    }

    //CMutexGuard autoGuard( mLock );
    pthread_mutex_lock( &gsMutex );

    if ( ! mSftCfg.isMember(section) )
    {
        pthread_mutex_unlock( &gsMutex );
        return PVCFG_SECTION_NOT_EXIST;
    }

    if ( ! mSftCfg[section].isMember( key ) )
    {
        pthread_mutex_unlock( &gsMutex );
        return PVCFG_KEY_NOT_EXIST;
    }
    
    value = mSftCfg[section][key].asString();

    pthread_mutex_unlock( &gsMutex );
    return PVCFG_SUCCESS;
}




/*static*/ PvCfg* PvCfg::Instance()
{
    if ( mInstance == NULL )
    {
        pthread_mutex_lock( &gsMutex );
        if ( mInstance == NULL )
        {
            mInstance = new PvCfg();

            std::string ts = GetPvPubFile( PV_PUBCFG_FILE );
            int npos = ts.rfind("/");

            mPubCfgPath = ts.substr( 0, npos );
            mPubCfgFile = ts.substr( npos + 1 );

            ts = GetPvRunPar( RUNPAR_CFGFILE );
            npos = ts.rfind("/");
            mSftCfgPath = ts.substr( 0, npos );
            mSftCfgFile = ts.substr( npos + 1 );
        }
        pthread_mutex_unlock( &gsMutex );
    }

    return mInstance;
}





/*************************************************************
  * brief : 返回PvCfg接口返回值的字符描述
  * note  : err_val为其他接口的返回值
**************************************************************/
const char* PvCfgErrMsg( int ret)
{
    return PvCfg::ErrorIdName( ret );

}




/*************************************************************
  * brief : 程序开始时调用此接口加载公共配置和软件私有配置文件
  * note  : 在程序开始时就调用 
**************************************************************/
int PvCfgInitialize()
{
    return PvCfg::Instance()->Initialize();
}




/*************************************************************
  * brief : 重新加载公共配置文件的配置
  * note  : 在公共配置文件有修改时调用
**************************************************************/
int PvReLoadPubCfg()
{
    return PvCfg::Instance()->loadPubCfg();
}




/*************************************************************
  * brief : 重新加载软件私有配置文件的配置
  * note  : 在私有配置文件有修改时调用
**************************************************************/
int PvReLoadSftCfg()
{
    return PvCfg::Instance()->loadSftCfg();
}




/***************************************************************************
  * brief : 获取公共配置"/xxx/pvfs20/id1/pubfile/pub_config.init"的值
  * note  :
  *         如果配置文件中存在没有域名的配置值,section也要输入,此时section
  *         的值为"ROOT"
  *      
***************************************************************************/
int GetPvPubCfgVal( std::string &val, const char* section, const char* key )
{
    return PvCfg::Instance()->getPubCfgVal( val, section, key );
}



/***************************************************************************
  * brief : 获取软件私有配置"程序运行路径/程序名小写_config.init"的值
  * note  :
  *         如果配置文件中存在没有域名的配置值,section也要输入,此时section
  *         的值为"ROOT"
  *      
***************************************************************************/
int GetPvSftCfgVal( std::string &val, const char* section, const char* key )
{
    return PvCfg::Instance()->getSftCfgVal( val, section, key );
}




