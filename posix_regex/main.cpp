#include <stdio.h>
#include <string.h>

#include "POSIXregexC.h"



//测试POSIX正则表达式的匹配
void test_posix_reg( CPOSIXregC &tobj )
{
    int reti;
    char input[100];
    char pattern[100];

    // 得到正则表达式字符串(从用户输入)
    printf("Enter a regular expression pattern: ");
    fgets(pattern, sizeof(pattern), stdin);
    pattern[strcspn(pattern,"\n")]=0; //将最后的换行符替换为字符串结束符'\0'

    //设置正则表达式的： 正则字符串(含有正则字符的字符串)
    reti = tobj.setPattern( pattern );
    if ( reti != 0 ){
        fprintf(stderr, "Failed to compile the regular expression:errmsg[%s]\n",
                tobj.getErrMsg() );
        exit(1);

    }


    // 得到将要去匹配正则表达式的 字符串
    printf("Enter an input string: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input,"\n")]=0; //将最后的换行符替换为字符串结束符'\0'

    //printf("input string:[%s] \n",input);

    //判断输入字符串是否符合正则表达式
    //return:
    //   1  : match successful
    //   0  : not match the regular expression
    //   -1 : error
    reti = tobj.isMatch( input );
    if (reti == 1) {
        //匹配成功
        printf("\n[%s] MATCH the regular expression [%s]\n",input, pattern);

    } else if (reti == 0) {
        //输入字符串不符合当前正则表达式
        //printf("[%s] NOT MATCH the regular expression [%s]\n",input, pattern);
        printf("\n%s\n",tobj.getErrMsg());

    } else {
        //错误
        fprintf(stderr, "\nRegular expression [%s] matching [%s] ERROR; errmsg=[%s]\n", 
                pattern,input,tobj.getErrMsg());
        exit(1);
    }
    return;
}




//测试 通配符字符串 转发成 POSIX正则表达式字符串
void test_glob_regex( void )
{
    int ret;
    std::string ostr;
    char gstr[100];

    // Get the wildcard string
    printf("Enter glob string: ");
    fgets(gstr, sizeof(gstr), stdin);
    gstr[strcspn(gstr,"\n")]=0; //将最后的换行符替换为字符串结束符'\0'

    ret = CPOSIXregC::glob_to_BRE( gstr, ostr );
    if ( ret != 0 ){
        fprintf(stderr, "POSIXregexC::glob_to_BRE return[%d]\n", ret );
        exit(1);
    }

    printf("\n wildcard string:[%s]\n", gstr);
    printf("\n POSIX regex string:[%s]\n", ostr.c_str());
    return;
}




int main(int argc, char* argv[]) {

    ////测试通配符转换到基本正则表达式
    //for ( int k=0; k<3; k++ ){
    //    test_glob_regex();
    //}
    //return 0;
    
    //1: 设置成支持 POSIX的 扩展正则表达式(ERE)
    //0: 默认不设置时 只支持 POSIX的 基本正则表达式(BRE) 
    int ERE_flag=0; 

    //for (int count = 0; count < argc; count++)
    //    printf("%d: %s \n", count, argv[count]);

    if ( argc > 1  && strcmp("1", argv[1]) == 0 ){
        ERE_flag=1;
    }

    printf("\nTIPS:POSIX正则表达式有两种匹配方式,对应此程序运行参数如下:\n");
    printf("\t1:不用操作参数时：POSIX的 基本正则表达式\n");
    printf("\t2:参数1时：扩展正则表达式(ERE) 匹配\n");

    CPOSIXregC tobj;

    if ( ERE_flag == 1 ) {
        //设置成支持 POSIX的 扩展正则表达式(ERE)
        tobj.setERE();
        printf("\n\t当前使用规则: POSIX的 扩展正则表达式(ERE) 匹配\n");
    }
    else {
        // 默认不设置时 只支持 POSIX的 基本正则表达式(BRE) 
        printf("\n\t当前使用规则: POSIX的 基本正则表达式(BRE) 匹配\n");
    }

    for(int i=0; i<1; i++) {
        test_posix_reg( tobj );
    }

    return 0;
}


