#include "funcLibSo.h"

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
int isDirTest(const char * dirName_p)
{
    struct stat bufStat = {};
 //   char fileMode[10] = {};

    if (stat(dirName_p,&bufStat)!=0)
    {
        return -1;
    }
    if( (bufStat.st_mode&S_IFDIR) !=0 )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}



