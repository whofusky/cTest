/***********************************************************
 *
 * @file    PvCfg.h
 *
 * @brief   PvCfg头文件
 *
 * @author  fu.sky
 *
 * @date    2022-11-01_08:45:27
 *
 * @version V10.010.000
 *
 ***********************************************************/

#ifndef _PV_CFG_H_
#define _PV_CFG_H_

#include <string>
#include "json/value.h"
//#include "Mutex.h"
#include "PvDir.h"
#include "minIni.h"




/*************************************************************
  * brief : 返回PvCfg接口返回值的字符描述
  * note  : err_val为其他接口的返回值
**************************************************************/
const char* PvCfgErrMsg( int ret);




/*************************************************************
  * brief : 程序开始时调用此接口加载公共配置和软件私有配置文件
  * note  : 在程序开始时就调用 
**************************************************************/
int PvCfgInitialize();




/*************************************************************
  * brief : 重新加载公共配置文件的配置
  * note  : 在公共配置文件有修改时调用
**************************************************************/
int PvReLoadPubCfg();




/*************************************************************
  * brief : 重新加载软件私有配置文件的配置
  * note  : 在私有配置文件有修改时调用
**************************************************************/
int PvReLoadSftCfg();




/***************************************************************************
  * brief : 获取公共配置"/xxx/pvfs20/id1/pubfile/pub_config.init"的值
  *   par :
  *         val     从配置文件取得的配置值
  *         section 需要取值的域名
  *         key     需要取值的key名
  *   ret :
  *         0  成功
  *        !0  失败
  * note  :
  *         如果配置文件中存在没有域名的配置值,section也要输入,此时section
  *         的值为"ROOT"
  *      
***************************************************************************/
int GetPvPubCfgVal( std::string &val, const char* section, const char* key );





/***************************************************************************
  * brief : 获取软件私有配置"程序运行路径/程序名小写_config.init"的值
  *   par :
  *         val     从配置文件取得的配置值
  *         section 需要取值的域名
  *         key     需要取值的key名
  *   ret :
  *         0  成功
  *        !0  失败
  * note  :
  *         如果配置文件中存在没有域名的配置值,section也要输入,此时section
  *         的值为"ROOT"
  *      
***************************************************************************/
int GetPvSftCfgVal( std::string &val, const char* section, const char* key );












//返回值定义
enum PVCFGError{
    PVCFG_SUCCESS = 0,
    PVCFG_INPAR_ERROR,       //输入参数不符合要求
    PVCFG_PUBCFG_NOT_EXIST,  //公共配置文件不存在
    PVCFG_SFTCFG_NOT_EXIST,  //软件私有配置文件不存在
    PVCFG_SECTION_NOT_EXIST, //要取的域名在配置文件中不存在
    PVCFG_KEY_NOT_EXIST,     //要取的键名在配置文件中不存在

    PVCFG_ERROR_COUNT
};




class PvCfg
{

private:
    PvCfg();
    ~PvCfg();

public:
    static std::string mPubCfgPath;
    static std::string mPubCfgFile;

    static std::string mSftCfgPath;
    static std::string mSftCfgFile;

public:
    static PvCfg* Instance();
    static const char* ErrorIdName(int errorId);
public:
    static int s_isreg( const char* path); // it a regular file ? 0 no 1 yes 

private:
    static PvCfg* mInstance;
    static const char* _errorName[PVCFG_ERROR_COUNT+1];


public:
    int loadPubCfg();
    int loadSftCfg();
    int Initialize();
    int getPubCfgVal(std::string &value, const char* section, const char* key);
    int getSftCfgVal(std::string &value, const char* section, const char* key);

private:
    Json::Value mPubCfg;  //公共配置文件内容
    Json::Value mSftCfg;  //软件私有版本文件内容
    //CMutex      mLock;

    int         mPubRet;
    int         mSftRet;

private:
    class Delector
    {
        public:
            ~Delector()
            {
                if ( PvCfg::mInstance != NULL )
                {
                    delete PvCfg::mInstance;
                    PvCfg::mInstance = NULL;
                }
            }
    };

    static Delector delector;

};



#endif//_PV_CFG_H_
