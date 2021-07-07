/***********************************************************
 *
 * @file    Log.h
 *
 * @brief   自定义Log类文件
 *
 * @author  fu.sky
 *
 * @date    2021-07-03
 *
 * @version V10.010.000
 *
 ***********************************************************/

#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum sys_log_level
{
	OSAL_SYS_LOG_NORMAL,
	OSAL_SYS_LOG_WARNING,
	OSAL_SYS_LOG_ERROR,
	OSAL_SYS_LOG_LIB,
	OSAL_SYS_LOG_NED
}SYS_LOG_LEVEL;


void OSAL_Syslog_Init();

void OSAL_Syslog_Enable( SYS_LOG_LEVEL level );

void OSAL_Syslog_Disable( SYS_LOG_LEVEL level );

void OSAL_Syslog( SYS_LOG_LEVEL level, const char* fmt, ...);

void OSAL_SyslogNoPrefix( SYS_LOG_LEVEL level, const char* fmt, ...);

class Log
{
private:
	Log();
	~Log();
public:

	void Syslog_Init();

	void Syslog_Enable( SYS_LOG_LEVEL level );

	void Syslog_Disable( SYS_LOG_LEVEL level );

	//void Syslog( SYS_LOG_LEVEL level, const char* fmt, ...);
	void Syslog( SYS_LOG_LEVEL level, const char* msg);

	//FILE* Syslog_GetFD(void);

private:
    bool Syslog_LevelTest( SYS_LOG_LEVEL level );

public:
	static Log* Instance( void );

	//FILE* mFile;
	//std::vector<SYS_LOG_LEVEL> mLogLevel;

private:
    unsigned int mLogLevel;
	FILE*        mLogFp;

//private:
//	int mYear;
//	int mMonth;
//	int mDay;

};



#endif//_LOG_H_
