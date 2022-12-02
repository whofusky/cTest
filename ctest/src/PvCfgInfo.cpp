/***********************************************************
 *
 * @file    PvCfgInfo.cpp
 *
 * @brief   PvCfgInfo源文件
 *
 * @author  
 *
 * @date    2022-10-27
 *
 * @version V10.010.000
 *
 ***********************************************************/

#include <stdio.h>
#include <string.h>
#include "PvCfgInfo.h"


static pthread_mutex_t  gsMutex = PTHREAD_MUTEX_INITIALIZER;


PvCfgInfo* PvCfgInfo::mInstance = NULL;


PvCfgInfo::PvCfgInfo()
{
    mCfg = Json::Value::null;
}


PvCfgInfo::~PvCfgInfo()
{

}

void PvCfgInfo::loadValue()
{
    mCfg["k1"] = "11";
    mCfg["par1"]["par2"] = "test 11";
    mCfg["par1"]["par3"] = "12.68";

    printf("%s:%s:%d:DEBUG:[mCfg:[%s]]\n", 
            __FILE__,__FUNCTION__,__LINE__, 
            mCfg.toStyledString().c_str() );

    return;
}

void PvCfgInfo::getCfg( 
        std::string& value,
        const char* key1,
        const char* key2
        )
{


    if ( key1 == NULL && key2 == NULL )
    {
        return;
    } 
    if ( key2 == NULL )
    {
        value = mCfg[key1].asCString();
    }
    else
    {
        value = mCfg[key1][key2].asCString();
    }

    return;
}

PvCfgInfo* PvCfgInfo::Initialize()
{
    if( mInstance == NULL )
    {
        pthread_mutex_lock( &gsMutex );
        if( mInstance == NULL )
        {
            mInstance = new PvCfgInfo;
        }
        pthread_mutex_unlock( &gsMutex );
    }

    return mInstance;
}


