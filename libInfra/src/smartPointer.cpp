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
//#include "WriteLog.h"

RefBase::RefBase()
{
	mRefNum = 0;
}

RefBase::~RefBase()
{
}


void RefBase::incRef( void )
{
   //PrintToStdout( LOGDEBUG, "incRef:%d", mRefNum ); 
   printf("%s:%s:%d:DEBUG:[incRef:%d]\n", 
           __FILE__,__FUNCTION__,__LINE__,mRefNum );
	mRefNum++;
}

void RefBase::decRef( void )
{
	mRefNum--;
    //PrintToStdout( LOGDEBUG, "decRef:%d", mRefNum );
    printf("%s:%s:%d:DEBUG:[decRef:%d]\n",
            __FILE__,__FUNCTION__,__LINE__, mRefNum );
	if( mRefNum <= 0 )
	{
		delete this;
	}
	
}

