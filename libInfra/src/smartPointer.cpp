/***********************************************************
 *
 * @file    smartPointer.h
 *
 * @brief   smartPointer源文件
 *
 * @author  fu.sky
 *
 * @date    2021-07-04
 *
 * @version V10.010.000
 *
 ***********************************************************/

#include "smartPointer.h"
#include "WriteLog.h"

RefBase::RefBase()
{
	mRefNum = 0;
}

RefBase::~RefBase()
{
}


void RefBase::incRef( void )
{
    PrintToStdout( LOGDEBUG, "incRef:%d", mRefNum );
	mRefNum++;
}

void RefBase::decRef( void )
{
	mRefNum--;
    PrintToStdout( LOGDEBUG, "decRef:%d", mRefNum );
	if( mRefNum <= 0 )
	{
		delete this;
	}
	
}

