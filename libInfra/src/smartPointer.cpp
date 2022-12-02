/***********************************************************
 *
 * @file    smartPointer.h
 *
 * @brief   smartPointer源文件
 *
 * @author  
 *
 * @date    2021-07-04
 *
 * @version V10.010.000
 *
 ***********************************************************/

#include <stdio.h>
#include "smartPointer.h"
#include "CustomOutLog.h"

RefBase::RefBase()
{
	mRefNum = 0;
}

RefBase::~RefBase()
{
}


void RefBase::incRef( void )
{
   b_write_log(_DEBUG,"incRef:%d", mRefNum );
	mRefNum++;
}

void RefBase::decRef( void )
{
	mRefNum--;
    b_write_log(_DEBUG, "decRef:%d", mRefNum );
	if( mRefNum <= 0 )
	{
		delete this;
	}
	
}

