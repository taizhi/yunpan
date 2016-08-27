/*
 * echo.c --
 *
 *	Produce a page containing all FastCGI inputs
 *
 *
 * Copyright (c) 1996 Open Market, Inc.
 *
 * See the file "LICENSE.TERMS" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */
#ifndef lint
static const char rcsid[] = "$Id: echo.c,v 1.5 1999/07/28 00:29:37 roberts Exp $";
#endif /* not lint */

#include "fcgi_config.h"

#include <stdlib.h>
#include <pthread.h>
#include "redis_op.h"
#include <stdio.h>
#include "cJSON.h"


#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef _WIN32
#include <process.h>
#else
extern char **environ;
#endif

#include "fcgi_stdio.h"




void create_objects(char **outjson,RVALUES mydata,int get_num,redisContext *conn)
{
		cJSON *root,*thm,*fld;char *out;int i;
		root=cJSON_CreateObject();	
	  cJSON_AddItemToObject(root, "games", thm=cJSON_CreateArray());			
	
	for (i=0;i<get_num;i++)
	{
		cJSON_AddItemToArray(thm,fld=cJSON_CreateObject());	
		
		char *tmp = mydata[i];
		char *id = NULL; 
		char *url = NULL;
		char *filename = NULL;
		char *time = NULL;
		char *user = NULL;
		char *filetype = NULL;
		
		
		
		id = strtok(tmp,"||");
		url = strtok(NULL,"||");
		filename = strtok(NULL,"||");
		time = strtok(NULL,"||");
		user = strtok(NULL,"||");
		filetype = strtok(NULL,"||");
		char picurl_m[50] = {0};
		sprintf(picurl_m,"/static/file_png/%s.png",filetype);
		
		cJSON_AddStringToObject(fld,"id",id);
		
		
		cJSON_AddNumberToObject(fld,"kind",2);
		
		cJSON_AddStringToObject(fld,"title_m",filename);
		
		
		cJSON_AddStringToObject(fld,"title_s","rect");
		cJSON_AddStringToObject(fld,"descrip",time);
		
		
		cJSON_AddStringToObject(fld,"picurl_m",picurl_m);
		cJSON_AddStringToObject(fld,"url",url);
		
		
		cJSON_AddNumberToObject(fld,"pv",rop_zset_get_score(conn,"FILE_HOT_ZSET",id));
		cJSON_AddNumberToObject(fld,"hot",2);
	}
	
	out=cJSON_Print(root);	
	cJSON_Delete(root);	
	*outjson = out;
	//printf("%s\n",out);	
	//free(out);
}


void getvalue(char *param,char *key,char *value)
{
			char *pst = NULL;
			char *plt = NULL;
			pst = strstr(param,key);
			
			if(pst == NULL)
				return;
			//查找=
			pst = strstr(pst,"=");
			if(pst == NULL)
				return;
			pst = pst + 1;
			
			//查找&
			plt = strstr(pst,"&");
			
			if(plt != NULL)
			{
					strncpy(value,pst,plt-pst);
			}
			else
			{
					strcpy(value,pst);
			}
		
}

//字符串替换
int StrReplace(char strRes[],char from[], char to[]) 
{
    int i,flag = 0;
    char *p,*q,*ts;
    for(i = 0; strRes[i]; ++i) {
        if(strRes[i] == from[0]) {
            p = strRes + i;
            q = from;
            while(*q && (*p++ == *q++));
            if(*q == '\0') {
                ts = (char *)malloc(strlen(strRes) + 1);
                strcpy(ts,p);
                strRes[i] = '\0';
                strcat(strRes,to);
                strcat(strRes,ts);
                free(ts);
                flag = 1;
            }
        }
    }
    return flag;
}

int main ()
{
    char **initialEnv = environ;
    int count = 0;

    while (FCGI_Accept() >= 0) {
			

	printf("Content-type: text/javascript\r\n"
	    "\r\n");

       
				char *queryString = getenv("QUERY_STRING");
				
				char cmd[40] = {0};
				getvalue(queryString,"cmd",cmd);
				
				char fromId[40] = {0};
				getvalue(queryString,"fromId",fromId);
				char count[40] = {0};
				getvalue(queryString,"count",count);
			
				redisContext* conn = rop_connectdb_nopwd("127.0.0.1","6379");
				if(NULL == conn)
				{
						printf("连接失败\n");
				}
				//取出数据
				if(0 == strcmp(cmd,"increase"))//显示数据
				{
						char fileId[60] = {0};
						getvalue(queryString,"fileId",fileId);
					
						StrReplace(fileId,"%2F","/");
						
						//点击一次,自增一次
						rop_zset_increment(conn,"FILE_HOT_ZSET",fileId);
						
						//得到分数
						int pv = rop_zset_get_score(conn,"FILE_HOT_ZSET",fileId);
						//printf("%d<br>",pv);
						
						
				}
				else if(0 == strcmp(cmd,"newFile")) //取数据
				{
						//元素个数
					int get_num = rop_get_list_cnt(conn,"FILE_INFO_LIST");
		
					
					RVALUES mydata; //数组指针
					mydata = malloc(VALUES_ID_SIZE*get_num); //数组指针分配空间
					//获取数据
					rop_range_list(conn,"FILE_INFO_LIST",atoi(fromId),atoi(count)+atoi(fromId),mydata,&get_num);
					char *outjson = NULL;
					create_objects(&outjson,mydata,get_num,conn);
					
					if(outjson != NULL)
					{
							printf("%s",outjson);
							free(outjson);
							free(mydata);
					}
				}
				
				   
				
				//释放
				rop_disconnect(conn);
				
				
    
    } /* while */

    return 0;
}
