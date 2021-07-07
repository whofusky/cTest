/***********************************************************
 *
 * @file    Message.cpp
 *
 * @brief   Message源文件
 *
 * @author  fu.sky
 *
 * @date    2021-07-04
 *
 * @version V10.010.000
 *
 ***********************************************************/

#include <stdio.h>
#include "Message.h"

Message::Message()
{
	mIsValid = false;
	mCallBack = NULL;
}

Message::~Message()
{

}

Message::Message( const Message& obj )
{
	mIsValid      = obj.isValid();
	mWhat         = obj.mWhat;
	mArg1         = obj.mArg1;
	mArg2         = obj.mArg2;
	mMetaStr      = obj.mMetaStr;
	mAttr         = obj.mAttr;
	mTarget       = obj.mTarget;
	mCallBack     = obj.mCallBack;
	mTargetLooper = obj.mTargetLooper;
	mWhen         = obj.mWhen;
}

Message& Message::operator = ( Message& obj )
{
	this->mIsValid      = obj.isValid();
	this->mWhat         = obj.mWhat;
	this->mArg1         = obj.mArg1;
	this->mArg2         = obj.mArg2;
	this->mMetaStr      = obj.mMetaStr;
	this->mAttr         = obj.mAttr;
	this->mTarget       = obj.mTarget;
	this->mCallBack     = obj.mCallBack;
	this->mTargetLooper = obj.mTargetLooper;

	this->mWhen         = obj.mWhen;

	return *this;
}

bool Message::isValid()
{
	return mIsValid;
}


void Message::setValid( bool flag )
{
	mIsValid = flag;
    return;
}




