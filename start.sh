#! /bin/sh
P1=`ps aux | grep nginx | wc -l`

#nginx 服务器开启
if [ $P1 -eq 1 ];then
	/usr/local/nginx/sbin/nginx -c /usr/local/nginx/conf/nginx.conf
else
	/usr/local/nginx/sbin/nginx -s reload
fi


#上传
P2=`ps aux | grep upload| wc -l`

#显示
P3=`ps aux | grep data| wc -l`

if [ $P2 -gt 1 ];then
	PID=`ps aux | grep upload | awk '{print $2}' | sed -n "1,1p"`
	kill $PID	
fi

if [ $P3 -gt 1 ];then
	PID=`ps aux | grep data | awk '{print $2}' | sed -n "1,1p"`
	kill $PID
	
fi

#gcc upload.c -o upload -lfcgi

#开启cgi托管
spawn-fcgi -a 127.0.0.1 -p 8083 -f ./upload
spawn-fcgi -a 127.0.0.1 -p 8084 -f ./data

echo "服务启动成功1"
