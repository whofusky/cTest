/****************************************
 * 
 *    log4cplusDiy.h 
 * 
 *    Create by fushikai
 * 
 *    2021-07-11
 * 
 ****************************************/
#ifndef LOG4CPLUS_DIY_H
#define LOG4CPLUS_DIY_H

#include "log4cplus/logger.h"
#include "log4cplus/configurator.h"
#include "log4cplus/helpers/loglog.h"
#include "log4cplus/helpers/stringhelper.h"
#include "log4cplus/helpers/sleep.h"
#include "log4cplus/helpers/fileinfo.h"
#include "log4cplus/loggingmacros.h"

//level: 
//     TRACE_LOG_LEVEL 
//     DEBUG_LOG_LEVEL 
//     INFO_LOG_LEVEL 
//     WARN_LOG_LEVEL 
//     ERROR_LOG_LEVEL 
//     FATAL_LOG_LEVEL
//
#define _TRACE  TRACE_LOG_LEVEL
#define _DEBUG  DEBUG_LOG_LEVEL
#define _INFO   INFO_LOG_LEVEL
#define _WARN   WARN_LOG_LEVEL
#define _ERROR  ERROR_LOG_LEVEL
#define _FATAL  FATAL_LOG_LEVEL


#define WRITE_LOG(logger,level,args...) \
    LOG4CPLUS_MACRO_FMT_BODY(logger,level,##args);

#define WRITE_LOG_BYNAME(logerName,level,args...) \
    LOG4CPLUS_MACRO_FMT_BODY(log4cplus::Logger::getInstance(LOG4CPLUS_TEXT(logerName)),level,##args);

#endif //LOG4CPLUS_DIY_H
