/*
    平台定义
*/

#ifndef H_SYS_DEFINE_
#define H_SYS_DEFINE_

#pragma once

#ifdef _WIN32
    #ifndef WINDOWS_PLATFORM
        #define WINDOWS_PLATFORM
    #endif
    #ifndef WIN32
        #define WIN32
    #endif
#endif

#ifdef _WIN64
    #ifndef WINDOWS_PLATFORM
        #define WINDOWS_PLATFORM
    #endif
    #ifndef WIN64
        #define WIN64
    #endif
#endif

//是否输出到屏幕或标准输出
#define DEBUG               true

//定义日志输出级别(值越小级别最高,值建议设置成2的N次方)
#define DEBUG_LEVEL_0  0  //日志输出级别最高,一般指必须输出的错误日志
#define DEBUG_LEVEL_1  1  //1级日志输出
#define DEBUG_LEVEL_2  2  //2级日志输出

#endif//H_SYS_DEFINE_