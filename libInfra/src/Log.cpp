/***********************************************************
 *
 * @file    Log.cpp
 *
 * @brief   自定义Log类源文件
 *
 * @author  fu.sky
 *
 * @date    2021-07-03
 *
 * @version V10.010.000
 *
 ***********************************************************/

#include <stdarg.h>
#include <time.h>
#include <sys/stat.h>
#include <pthread.h>
#include <iostream>
#include <errno.h>
//#include <string>
//#include <string.h>

#include "Log.h"
#include "Time.h"


static std::vector<SYS_LOG_LEVEL> gLogLevel;

void OSAL_Syslog_Init()
{
	Log::Instance()->Syslog_Init();
	return;
}

void OSAL_Syslog_Enable( SYS_LOG_LEVEL level )
{
#if 0
	std::vector<SYS_LOG_LEVEL>::iterator it;
	for( it= gLogLevel.begin(); it!= gLogLevel.end(); it++ )
	{
		if ( *it == level )
		{
			return;
		}
	}
	gLogLevel.push_back( level );
#else
	Log::Instance()->Syslog_Enable( level )
#endif

	return;
}



void OSAL_Syslog_Disable( SYS_LOG_LEVEL level )
{
#if 0
	std::vector<SYS_LOG_LEVEL>::iterator it;
	for( it= gLogLevel.begin(); it!= gLogLevel.end(); it++ )
	{
		if ( *it == level )
		{
			gLogLevel.erase( it );
			return;
		}
	}
	gLogLevel.push_back( level );
#else
	Log::Instance()->Syslog_Disable( level )
#endif

		return;
}


void OSAL_Syslog( SYS_LOG_LEVEL level, const char* fmt, ...)
{
	bool levelEnable = false;
	char printBuf[1024*1024*2] = {0};
	std::string tototalPrintBuf;
	std::vector<SYS_LOG_LEVEL>::iterator it;
	for( it = Log::Instance()->mLogLevel.begin();
		 it != Log::Instance()->mLogLevel.end();
		 it ++
		)
	{
		if ( *it == level )
		{
			levelEnable = true;
		}
	}

	if ( !levelEnable )
	{
		return;
	}

	char curTime[32] = {0};
	struct timeval tv;
	time_t curTimeT;
	struct tm curTm;
	gettimeofday( &tv, NULL);
	curTimeT = tv.tv_sec;
	localtime_r( &curTimeT, &curTm);

	ConvertTimeToStr( curTime, 32, curTm);

	printf("[%s]###",curTime);

	tototalPrintBuf += curTime;
	tototalPrintBuf += ":";

	fwrite( tototalPrintBuf.c_str(),
		tototalPrintBuf.size(),
		1,
		Log::Instance()->Syslog_GetFD()
		);

	fflush( Log::Instance()->Syslog_GetFD() );

}


Log::Log()
{
	std::string filePath = "log/log_";
	char curTime[32] = {0};
	time_t curTimeT;
	struct tm curTm;
	time( &curTimeT);
	curTm = *( gmtime(&curTimeT));

	mYear = curTm.tm_year + 1900;
	mMonth = curTm.tm_mon;
	mDay = curTm.tm_mday;
	sprintf( curTime,"%d-%d-%d",mYear, mMonth, mDay );

	filePath += curTime;
	filePath += ".log";

	mkdir( "log", S_IRWXU);
	mFile = fopen( filePath.c_str()(),"a+" );
}

Log::~Log()
{
	if( mFile != NULL )
	{
		fclose( mFile );
	}
}


void Log::Syslog_Init()
{

}

void Log::Syslog_Enable( SYS_LOG_LEVEL level )
{
	std:vector<SYS_LOG_LEVEL>::iterator it;
	for( it = mLogLevel.begin();
		 it != mLogLevel.end();
		 it++
		)
	{
		if( *it == level )
		{
			return;
		}
	}
	mLogLevel.push_back( level );
}

void Log::Syslog_Disable( SYS_LOG_LEVEL level )
{
	std:vector<SYS_LOG_LEVEL>::iterator it;
	for( it = mLogLevel.begin();
		it != mLogLevel.end();
		it++
		)
	{
		if( *it == level )
		{
			mLogLevel.erase( it );
			return;
		}
	}
}


void Log::Syslog( SYS_LOG_LEVEL level, const char* fmt, ...)
{
	bool levelEnable = false;
	char printBuf[512] = {0};
	std::string tototalPrintBuf;
	std::vector<SYS_LOG_LEVEL>::iterator it;
	for( it = Log::Instance()->mLogLevel.begin();
		it != Log::Instance()->mLogLevel.end();
		it ++
		)
	{
		if ( *it == level )
		{
			levelEnable = true;
		}
	}

	if ( !levelEnable )
	{
		return;
	}

	char curTime[32] = {0};
	struct timeval tv;
	time_t curTimeT;
	struct tm curTm;
	gettimeofday( &tv, NULL);
	curTimeT = tv.tv_sec;
	localtime_r( &curTimeT, &curTm);

	ConvertTimeToStr( curTime, 32, curTm);

	printf("[%s]###",curTime);

	tototalPrintBuf += curTime;
	tototalPrintBuf += ":";

	va_list ap;
	va_start( ap, fmt );
	vsprintf( printBuf, fmt, ap);
	va_end( ap );
	tototalPrintBuf += printBuf;

	if ( mYear != curTm.tm_year +1900
		|| mMonth != curTm.tm_mon
		|| mDay != curTm.tm_mday
		)
	{
		std::string filePath = "log/log_";
		mYear = curTm.tm_year + 1900;
		mMonth = curTm.tm_mon;
		mDay = curTm.tm_mday;
		memset( curTime, 0, sizeof(curTime) );
		sprintf( curTime),"%d-%d-%d",mYear, mMonth, mDay );
		if( mFile != NULL )
		{
			fclose( mFile );
		}
		filePath += curTime;
		filePath += ".log";
		mFile = fopen( filePath.c_str(), "a+");

	}

	fwrite( tototalPrintBuf.c_str(),
		tototalPrintBuf.size(),
		1,
		mFile
		);

	fflush( mFile );
}

FILE* Log::Syslog_GetFD(void)
{
	char curTime[32] = {0};
	struct timeval tv;
	time_t curTimeT;
	struct tm curTm;
	gettimeofday( &tv, NULL);
	curTimeT = tv.tv_sec;
	localtime_r( &curTimeT, &curTm);

	if ( mYear != curTm.tm_year +1900
		|| mMonth != curTm.tm_mon
		|| mDay != curTm.tm_mday
		)
	{
		std::string filePath = "log/log_";
		mYear = curTm.tm_year + 1900;
		mMonth = curTm.tm_mon;
		mDay = curTm.tm_mday;
		memset( curTime, 0, sizeof(curTime) );
		sprintf( curTime),"%d-%d-%d",mYear, mMonth, mDay );
		if( mFile != NULL )
		{
			fclose( mFile );
		}
		filePath += curTime;
		filePath += ".log";
		mFile = fopen( filePath.c_str(), "a+");

	}

	return mFile;
}

Log* Log::Instance()
{
	static Log* mInstance = NULL;
	if( mInstance == NULL )
	{
		mInstance = new Log;
	}
	return mInstance;
}
