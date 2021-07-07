/***********************************************************
 *
 * @file    ICallBack.h
 *
 * @brief   ICallBack头文件
 *
 * @author  fu.sky
 *
 * @date    2021-07-04
 *
 * @version V10.010.000
 *
 ***********************************************************/

#ifndef _I_CALLBACK_H_
#define _I_CALLBACK_H_

#include "json/json.h"

class ICallBack
{
public:
    virtual ~ICallBack();
    virtual void handleCallBack( Json::Value& ret) = 0;

}

#endif//_I_CALLBACK_H_
