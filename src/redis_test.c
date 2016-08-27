#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "redis_op.h"


int main()
{
		redisContext* conn = rop_connectdb_nopwd("127.0.0.1","6379");
		if(NULL == conn)
		{
				printf("连接失败\n");
		}

	  rop_list_push(conn,"FILE_INFO_LIST","年龄|姓名|19");
		rop_disconnect(conn);
		return 0;
}
