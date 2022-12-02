/***********************************************************
 *
 * @file    PvCfgInfo.h
 *
 * @brief   PvCfgInfo头文件
 *
 * @author  
 *
 * @date    2022-10-27
 *
 * @version V10.010.000
 *
 ***********************************************************/

#ifndef _I_EVENTMANAGER_H_
#define _I_EVENTMANAGER_H_
#include <string>
#include "json/json.h"

class PvCfgInfo
{
private:
    PvCfgInfo();
    ~PvCfgInfo();
private:
        static PvCfgInfo* mInstance;
    
public:
    static PvCfgInfo* Initialize();
    void getCfg( 
            std::string &value,
            const char* key1=NULL,
            const char* key2=NULL
            );
public:
    void loadValue();

        
private:
        Json::Value mCfg; 

};


#endif//_I_EVENTMANAGER_H_
