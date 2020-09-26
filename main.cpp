#include <stdio.h>

int subsum(int a,int b,int& sum);

int main()
{
    int a=10;
    int b=14;
    int c=0;
    subsum(a,b,c);
    printf("hello,word,c=[%d]\n",c);
    return 0;
}
