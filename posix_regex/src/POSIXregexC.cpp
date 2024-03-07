/***********************************************************
 *
 * @file    POSIXregexC.cpp
 *
 * @brief   自定义POSIX正则表达式接口
 *
 * @author  fu.sky
 *
 * @date    2024-02-28_14:35:09
 *
 * @version V10.010.000
 *
 ***********************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "POSIXregexC.h"



  
// 将通配符字符串转换为POSIX基本正则表达式(BRE:Basic Regular Expressions)字符串  
// return 
//   0  成功
//   -1 分配内存失败
int CPOSIXregC::glob_to_BRE(const char* glob, std::string &regStr) 
{  
    //std::string retStr;
    regStr.clear();

    int glob_len = strlen(glob);  

    // 最多需要双倍的字符数（用于转义） +1（用于结束符'\0'） +1(用于锚定开始^) +1(用于锚定结束$)  
    int regex_len = glob_len * 2 + 1 +1 +1; 
    char* tstr = (char*)malloc(regex_len);  
    if (!tstr) {  
        //perror("malloc error");  
        return -1;
    }  
  
    int i = 0; int j = 0;  
    tstr[j++] = '^';   // 添加 用于锚定开始 '^' 字符
    for (i = 0; glob[i] != '\0'; i++, j++) {  
        switch (glob[i]) {  
            case '.':  
                // 通配符'.'在正则表达式中匹配任意数量的字符（包括零个字符），使用'\.'表示  
                tstr[j++] = '\\';  
                tstr[j] = '.';  
                break;  
            case '*':  
                // 通配符'*'在正则表达式中匹配任意数量的字符（包括零个字符），使用'.*'表示  
                tstr[j++] = '.';  
                tstr[j] = '*';  
                break;  
            case '?':  
                // 通配符'?'在正则表达式中匹配任意单个字符，使用'.'表示  
                tstr[j] = '.';  
                break;  

            //case '[':  
            //    // 通配符'['开始一个字符类，在正则表达式中我们直接使用它  
            //    tstr[j] = '[';  
            //    break;  
            //case ']':  
            //    // 通配符']'结束一个字符类，在正则表达式中我们直接使用它  
            //    tstr[j] = ']';  
            //    break;  

            case '\\':  
                // 转义字符，我们只需要复制它  
                tstr[j] = '\\';  
                j++; // 因为下一个字符也需要被复制  
                tstr[j] = glob[++i]; // 复制转义后的字符  
                break;  
            default:  
                // 其他字符在正则表达式中直接表示它们自己  
                tstr[j] = glob[i];  
        }  
    }  
    tstr[j++] = '$';   // 添加 用于锚定结尾 '$' 字符
    tstr[j] = '\0'; // 添加字符串结束符  
    regStr = tstr;
    free(tstr);  
  
    return 0;  
}  
  



CPOSIXregC::CPOSIXregC()
{
    memset( &mPreg, 0, sizeof(mPreg) );
    mCflags = 0;
    mRet    = 0;
    memset( mErrMsg, 0, sizeof(mErrMsg) );
}




CPOSIXregC::~CPOSIXregC()
{
    regfree( &mPreg );
}




void CPOSIXregC::setERE( void )
{
    mCflags = (mCflags | REG_EXTENDED);
    return;
}


void CPOSIXregC::setCflags( int cflags )
{
    mCflags = (mCflags | cflags);
    return;
}

//regcomp(regex_t *preg, const char *regex, int cflags);'s regex
//return:
//    0 : successful
//    -1: error
int CPOSIXregC::setPattern(const char *regex)
{
    if ( regex == NULL ) {
        memset( mErrMsg, 0, sizeof(mErrMsg) );
        strncpy( mErrMsg, "setPattern input para regex is null", sizeof(mErrMsg));
        return -1;
    }

    std::string tStr = regex;
    if ( tStr.empty() ) {
        memset( mErrMsg, 0, sizeof(mErrMsg) );
        strncpy( mErrMsg, "setPattern input para regex is null string", sizeof(mErrMsg));
        return -1;
    }

    if ( mPattern != tStr ) {

        //Free the previous regex_t
        if ( ! mPattern.empty() ) {
            //printf ("fusktest:delete previrous pattern [%s]\n",mPattern.c_str());
            regfree( &mPreg );
            memset( &mPreg, 0, sizeof(mPreg) );
        }

        // Compile the regular expression
        mRet = regcomp(&mPreg, regex, mCflags);
        if ( mRet ) {
            memset( mErrMsg, 0, sizeof(mErrMsg) );
            regerror(mRet, &mPreg, mErrMsg, sizeof(mErrMsg));
            return -1;
        }
        mPattern = tStr;
        //printf ("fusktest:set pattern to[%s][%s]\n",mPattern.c_str(),regex);
    }

    return 0;
}




//return mErrMsg
const char* CPOSIXregC::getErrMsg( void )
{
    return mErrMsg;
}





//Whether the string str matches the regular expression
//return:
//   1  : match successful
//   0  : not match the regular expression
//   -1 : error
int CPOSIXregC::isMatch(const char *str)
{
    if ( str == NULL ) {
        memset( mErrMsg, 0, sizeof(mErrMsg) );
        strncpy( mErrMsg, "isMatch input para str is null", sizeof(mErrMsg));
        return -1;
    }
    mRet = regexec(&mPreg, str, 0, NULL, 0);
    if (!mRet) {
        //match successful
        memset( mErrMsg, 0, sizeof(mErrMsg) );
        return 1;

    } else if (mRet == REG_NOMATCH) {
        memset( mErrMsg, 0, sizeof(mErrMsg) );
        snprintf( mErrMsg, sizeof(mErrMsg),
                "[%s] not match the regular expression [%s]",
                str, mPattern.c_str() );
        return 0;
    } else {
        memset( mErrMsg, 0, sizeof(mErrMsg) );
        regerror(mRet, &mPreg, mErrMsg, sizeof(mErrMsg));
        return -1;
    }

    memset( mErrMsg, 0, sizeof(mErrMsg) );
    strncpy( mErrMsg, "unknown error", sizeof(mErrMsg));
    return 0;
}




