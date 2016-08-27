CC=gcc
CFLAGS=-g -Wall
VPATH =./src

HEAD = ./inc

EXEC=redis_test upload  echo data  test
all:$(EXEC)


redis_test:redis_test.o make_log.o redis_op.o
	$(CC) $^ -o $@ $(CFLAGS) -lhiredis -lpthread 


upload:upload.o make_log.o redis_op.o
	$(CC) $^ -o $@ $(CFLAGS) -lhiredis -lpthread -lfcgi

echo:echo.o
	$(CC) $^ -o $@ -lfcgi
	
data:data.o make_log.o redis_op.o cJSON.o
	$(CC) $^ -o $@ $(CFLAGS) -lhiredis -lpthread -lfcgi -lm
	
test:test.o cJSON.o
	$(CC) $^ -o $@ $(CFLAGS) -lm
	
	
%.o:%.c
	$(CC) -c $< -o $@  -I/usr/local/include/hiredis -I$(HEAD)


.PHONY:clean
clean:
	rm -f *.o $(EXEC)
