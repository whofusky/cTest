/***********************************************************
*
*
*  Log.h
*
*  Create by muxd
*
*  2017/07/27
***********************************************************/
#ifndef _LOG_H_
#define _LOG_H_

typedef enum sys_log_level
{
    OSAL_SYS_LOG_NORMAL,
    OSAL_SYS_LOG_WARNING,
    OSAL_SYS_LOG_ERROR,
    OSAL_SYS_LOG_LIB,
}SYS_LOG_LEVEL;

void OSAL_Syslog_Init();

void OSAL_Syslog_Enable( SYS_LOG_LEVEL level );

void OSAL_Syslog_Disable( SYS_LOG_LEVEL level );

void OSAL_Syslog( SYS_LOG_LEVEL level, const char* fmt, ...);

#endif //_LOG_H_

