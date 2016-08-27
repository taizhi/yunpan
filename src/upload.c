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
#define FILE_ID_LEN     (256)
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "redis_op.h"



#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef _WIN32
#include <process.h>
#else
extern char **environ;
#endif

#include "fcgi_stdio.h"


//find 'substr' from a fixed-length buffer   
//('full_data' will be treated as binary data buffer)  
//return NULL if not found  
char* memstr(char* full_data, int full_data_len, char* substr) 
{ 
    if (full_data == NULL || full_data_len <= 0 || substr == NULL) { 
        return NULL; 
    } 

    if (*substr == '\0') { 
        return NULL; 
    } 

    int sublen = strlen(substr); 

    int i; 
    char* cur = full_data; 
    int last_possible = full_data_len - sublen + 1; 
    for (i = 0; i < last_possible; i++) { 
        if (*cur == *substr) { 
            //assert(full_data_len - i >= sublen);  
            if (memcmp(cur, substr, sublen) == 0) { 
                //found  
                return cur; 
            } 
        } 
        cur++; 
    } 

    return NULL; 
} 



static void PrintEnv(char *label, char **envp)
{
    printf("%s:<br>\n<pre>\n", label);
    for ( ; *envp != NULL; envp++) {
        printf("%s\n", *envp);
    }
    printf("</pre><p>\n");
}


//取出左右\n
void mytrim(char *name)
{
		if(NULL == name)
		{
			return ;
		}
		
		int start = 0;
		int end = strlen(name)-1;
		//先去除左边
		while(start < end && name[start] != 0)
		{
				if(name[start] == '\n' || name[start] == '\r' || name[start] == ' ')
				{
					start++;
				}
				else
				{
					break;
				}
		}
		//先去除右边
		while(start < end && name[end] != 0)
		{
				
			  if(name[end] == '\n' || name[end] == '\r' || name[end] == ' ')
				{
					//printf("----\n");
					end--;
				}
				else
				{
					 break;
				}
		}
		memcpy(name,name+start,end-start+1);
		name[end-start+1] = 0;
		//printf("新文件%s|",name);
}

//得到文件信息
void getinfo(char *key,char *targetValue,char *filename)
{
		//printf("%s",key);
		pid_t pid;
		int pfd[2];
		char buf[FILE_ID_LEN] = {0};
		if(pipe(pfd) < 0)
		{
			 printf("pip error\n");
			 return;
		}
		pid = fork();
		if(0 == pid)
		{
			  //关闭读端
				close(pfd[0]);
				//重定向写端
				dup2(pfd[1],STDOUT_FILENO);
				execlp("fdfs_file_info", "fdfs_file_info", "/etc/fdfs/client.conf", key, NULL);
			  printf("execlp fdfs_file_info error\n");
        close(pfd[1]);
		}
		//关闭写端
		close(pfd[1]);
		wait(NULL);
		read(pfd[0],buf,FILE_ID_LEN);
		
		char *tmp = NULL;
		char *pstart = NULL;
		char *pend = NULL;
		char address[100] = {0};//地址
		char stime[100] = {0};//事件
		char type[40] = {0};//类型
		char url[150] = {0};
		//解析出地址
		tmp = memstr(buf,strlen(buf),"source ip address: ");
		pstart = tmp + strlen("source ip address: "); //首地址
		pend = memstr(pstart,strlen(buf),"\n"); //末尾地址
		//printf("首地址=%p,尾地址=%p,buflen=%d,startlen=%d\n",pstart,pend,strlen(buf),strlen(pstart));
		strncpy(address,pstart,pend - pstart);
		mytrim(address);
		//printf("地址=%s",address);
		sprintf(url,"http://%s/%s",address,key);
		//上传时间
		tmp = memstr(pend,strlen(pend),"file create timestamp: ");
		pstart = tmp + strlen("file create timestamp: ");//首地址
		pend = memstr(pstart,strlen(buf),"\n"); //末尾地址
		strncpy(stime,pstart,pend-pstart);
		mytrim(stime);
		//printf("\n时间=%s",stime);
		//文件类型
		tmp = memstr(key,strlen(key),".");
		strcpy(type,tmp+1);
		
		//printf("\n文件类型=%s",type);
		
		sprintf(targetValue,"%s||%s||%s||%s||%s||%s",key,url,filename,stime,"1",type);
		
		printf("下载地址=%s<br>",url);
		
		
		//printf("\n信息=%s",targetValue);
		
}



//保存至fast_fdfs,持久化保存
int savefdfs(char *path,char *filename)
{
		char file_id[FILE_ID_LEN] = {0};
		char *file_name = path;
		
		pid_t pid;
		int pfd[2];
		
		if(pipe(pfd) < 0)
		{
			 printf("pip error\n");
			 return -1;
		}
		pid = fork();
    if (pid < 0) {
        printf("fork error\n");
    }
		//上传,子进程
		if(pid == 0)
		{
			close(pfd[0]);
			
			//将标准输出重定向到写管道
      dup2(pfd[1], STDOUT_FILENO);
			execlp("fdfs_upload_file", "fdfs_upload_file", "/etc/fdfs/client.conf", file_name, NULL);
			printf("execlp fdfs_upload_file error\n");
      close(pfd[1]);
		}
		//关闭写
		close(pfd[1]);
		
		//回收子进程
		wait(NULL);

    //read from 读管道
    read(pfd[0], file_id, FILE_ID_LEN);
		
		//删除源文件
		remove(file_name);
		
		//printf("源文件%s",file_id);
		mytrim(file_id);
		//printf("新文件%s|",file_id);
		//得到文件信息
		char targetValue[BUFSIZ] = {0};
		getinfo(file_id,targetValue,filename);
		
	
		
		redisContext* conn = rop_connectdb_nopwd("127.0.0.1","6379");
		if(NULL == conn)
		{
				printf("连接失败\n");
		}

		//保存至redis
		
	  int ret = rop_list_push(conn,"FILE_INFO_LIST",targetValue);
		if(0 == ret)
		{
			printf("保存key成功\n");
		}
		else{
			printf("保存key失败\n");
		}
		 
		char command[50] = {0};
		sprintf(command,"ZADD FILE_HOT_ZSET 0 %s ",file_id);
		ret = rop_redis_command(conn,command);
		if(-1 == ret)
		{
			printf("点击量表保存失败\n");
		}
		else
		{
			printf("<br>ok%s<br>",command);
		}
		
		rop_disconnect(conn);
		
		//printf("key=%s\n",file_id);
		
		return 0;
		
}

int main ()
{
    char **initialEnv = environ;
    int count = 0;
		char *data = NULL;
    while (FCGI_Accept() >= 0) {
        char *contentLength = getenv("CONTENT_LENGTH");
        int len;

	printf("Content-type: text/html\r\n"
	    "\r\n");

        if (contentLength != NULL) {
            len = strtol(contentLength, NULL, 10);
        }
        else {
            len = 0;
        }

        if (len <= 0) {
	    printf("No data from standard input.<p>\n");
        }
        else {
            int i, ch;
					  data = (char *)malloc(len);
						memset(data,0,len);
	          printf("<br>\n<pre>\n");
						
           for (i = 0; i < len; i++) {
                if ((ch = getchar()) < 0) {
                    printf("Error: Not enough bytes received on standard input<p>\n");
                    break;
								}
								//putchar(ch);
								data[i] = ch;
								
            }
						
						//解析data里面的数据
						char * ptmp = strstr(data,"; filename=\"");
            ptmp = ptmp + strlen("; filename=\"");
						char name[40] = {0}; //文件名称
						char flag[50] = {0}; //结束的标识符
						char file[50] = "images/";
						
						i = 0;
						while((*ptmp) != '\"')
						{
							name[i] = *ptmp;
							i++;
							ptmp++;
						}
						/////////////////////////////
					  //查找标识符						
						ptmp = memstr(data,len,"------");
						memcpy(flag,ptmp,memstr(ptmp,len,"\r\n")-ptmp);		 
						
						ptmp = memstr(ptmp,len,"\r\n\r\n");
						ptmp = ptmp + strlen("\r\n\r\n");//内容首地址
						
						char *pEnd = memstr(ptmp,len,flag) - strlen("\r\n");
						
						//判断文件是否存在
						int fd = 0;
						char *path = strcat(file,name);
						if(access(name,F_OK)!=0)
						{
							 fd = open(path,O_WRONLY|O_CREAT,0777);
							 //printf("文件不存在\n");
						}
						else
						{
							fd = open(path,O_WRONLY|O_TRUNC);
							//printf("文件存在\n");
						}
						//创建文件
						
						
						write(fd,ptmp,pEnd-ptmp);
						
						
						close(fd);
						//printf("address = %d\n",pEnd-ptmp);

						
						//printf("%d=%s",len,flag);
						
						//printf("\n</pre><p>\n");
						free(data);
						
						if(name[0] != 0)
						{
							savefdfs(path,name);
							printf("上传成功\n");
							
						}
						else{
							printf("上传失败\n");
						}
						
        }
				
				
				
				
				
				//printf("文件大小%d\n\r",getenv("CONTENT_LENGTH"));

        //PrintEnv("Request environment", environ);
        //PrintEnv("Initial environment", initialEnv);
    } /* while */

    return 0;
}
