#include <stdio.h>

int subsum(int a,int b,int& sum)
{
    sum=a+b;
    printf("%s.%s.%d:sum=[%d]\n",__FILE__,__FUNCTION__,__LINE__,sum);
    return 0;
}
