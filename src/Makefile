OBJ=log.o log_config.o thread_pool.o epoll_socket.o http_parser.o parser_tool.o http_server.o test_http_server.o
CC=g++ 
CFLAGS= -std=c++11 -Wall -g
LDFLAGS=-lpthread /lib/libjson_libmt.a 
DEPS_INCLUDE_PATH=-I ../deps/jscon-cpp/include/ -I ../deps/http-parser/
SRC_INCLUDE_PATH=-I ./
ALL=main

$(ALL):$(OBJ)
	$(CC) -o $(ALL) $(OBJ) $(CFLAGS) $(LDFLAGS)
$(OBJ):%.o:%.cpp
	$(CC) -o $@ -c $< $(CFLAGS) 

.PHONY:clean
clean:
	rm -rf *.o main
