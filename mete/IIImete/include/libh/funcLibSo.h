#ifndef METEFUNCLIBSO_H
#define METEFUNCLIBSO_H

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include<time.h>
#include<sys/time.h>
#include<stdlib.h>

#include<unistd.h>
#include<fcntl.h>

#include<stdarg.h>
#include<dirent.h>
#include<sys/stat.h>


#include "mete_datatype.h"
//#include "mete_macro.h"  已经包含在 mete_datatype.h 中了
#include "tinyxml2.h"

using namespace tinyxml2;

/*************************************
  *功能 ：判断是否为目录
  * 输入：
  *       
  * 输出：
  *返回：
  *     1 是目录 
  *     0 不是
  *     -1 错误
  *     
  *修改：
  *    　20170707134628 创建函数
*************************************/
int isDirTest(const char * dirName_p);

#endif // METEFUNCLIBSO_H