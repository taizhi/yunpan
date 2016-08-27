#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAXNUM 10

//数组指针,指向一个一维数组
typedef char  (*FUN1)[MAXNUM];

//指针数组,栈上开辟,存放地址
typedef char *FUN2[MAXNUM];

//数组
typedef char FUN3[MAXNUM];


int main()
{

#if 0
	FUN1 a ;
	a = malloc(MAXNUM*3);
	int i;
	for(i = 0;i < 3;i++)
	{
		sprintf(a[i],"%s---%d","sads",i);
	}

	for(i = 0;i< 3;i++)
	{
		printf("%s\n",a[i]);
	}
#else
	//在堆上开辟10个int字节大小的空间,返回的是首元素地址
        union
	{
		int a;
		char b;
	}test;
	test.a = 1;
	printf("b=%d,a=%d\n",test.b,test.a);	

#endif	
	return 0;
}

