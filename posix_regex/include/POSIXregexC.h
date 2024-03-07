/***********************************************************
 *
 * @file    POSIXregexC.h
 *
 * @brief   自定义POSIX正则表达式接口头文件
 *
 * @author  fu.sky
 *
 * @date    2024-02-28_14:35:09
 *
 * @version V10.010.000
 *
 ***********************************************************/

#ifndef _POSIX_REGEXC_H_
#define _POSIX_REGEXC_H_

#include <sys/types.h>
#include <regex.h>
#include <string>
//#include <errno.h>




//Customized encapsulation of POSIX regular functions regcomp, regexec
class CPOSIXregC
{

public:
    // 将通配符字符串转换为POSIX基本正则表达式(BRE:Basic Regular Expressions)字符串  
    // return 
    //   0  成功
    //   -1 分配内存失败
    static int glob_to_BRE(const char* glob, std::string &regStr) ;


public:
    CPOSIXregC();
    ~CPOSIXregC();


    //Use  POSIX  Extended  Regular  Expression syntax when interpreting regex
    //regcomp(regex_t *preg, const char *regex, int cflags);'s cflags
    void setERE(void);


    //regcomp(regex_t *preg, const char *regex, int cflags);'s regex
    //return:
    //    0 : successful
    //    -1: error
    int setPattern(const char *regex);


    //Whether the string str matches the regular expression
    //return:
    //   1  : match successful
    //   0  : not match the regular expression
    //   -1 : error
    int isMatch(const char *str);


    //return mErrMsg
    const char* getErrMsg(void);


    //regcomp(regex_t *preg, const char *regex, int cflags);'s cflags
    //  cflags may be the bitwise-or of zero or more of the following:
    //  REG_EXTENDED REG_ICASE REG_NOSUB REG_NEWLINE
    void setCflags(int cflags);
    

private:

    regex_t     mPreg;        //regcomp()'s  preg 
    int         mCflags;      //regcomp()'s  cflags
    std::string mPattern;     //regcomp()'s  regex
    int         mRet;         //RETURN VALUE of functions regcomp() and regexec()
    char        mErrMsg[100]; //is used to turn the error codes that can be returned by both regcomp() and regexec() into error message strings

};



#endif//_POSIX_REGEXC_H_
