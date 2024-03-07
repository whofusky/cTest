/****************************************
 * 
 *    FileToBase64.h
 * 
 *    Create by fushikai
 * 
 *    2023-09-13_10:16:44
 * 
 ****************************************/
#ifndef __FILE_TO_BASE64_H__
#define __FILE_TO_BASE64_H__

#include "libbase64.h"



/******************************************************************************
 *
 *    功能:
 *
 *    1. 加密: 
 *      (1) 将文件 file 的内容转换成base64 存储为新文件 file.encode 
 *      (2) 将文件 file 的内容转换成base64 存储为新文件 newfile 
 *
 *    2. 解密: 
 *      (1) 将文件内容为base64的文件 file 的内容decode成原内容存储为file.decode
 *      (2) 将文件内容为base64的文件 file 的内容decode成原内容存储为newfile
 *
******************************************************************************/



//关于接口中wrap的说明:
//      (1) 其值为设置一行的长度,不设置时默认是不换行的
//      (2) 如果设置了一个>0的值,则encode和decode的过程中要求wrap不能变否则会异常


class C_FB64
{
    public:
        C_FB64();
        C_FB64(size_t wrap); 
        ~C_FB64();

        void set_wrap(size_t wrap);

        //
        //1.如果outfile为NULL
        //  将文件 file 的内容转换成base64 存储为新文件file.encode 
        //2.如果outfile不为NULL
        //  将文件 file 的内容转换成base64 存储为新文件outfile
        //
        // return: 0 sucess
        //
        int  encode_file(const char* file, const char *outfile=NULL);


        //
        //1.如果outfile为NULL
        //  将文件内容为base64的文件file 的内容decode成原内容存储为新文件file.decode 
        //2.如果outfile不为NULL
        //  将文件内容为base64的文件file 的内容decode成原内容存储为新文件outfile
        //
        // return: 0 sucess
        //
        int  decode_file(const char* file, const char *outfile=NULL);

    private:
        size_t m_wrap;
};







#endif //__FILE_TO_BASE64_H__
