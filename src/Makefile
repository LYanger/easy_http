OBJ=log.o log_config.o thread_pool.o epoll_socket.o test_epoll.o
CC=g++ 
CFLAGS= -std=c++11 -Wall -g
LIBPATH=-lpthread
ALL=main

$(ALL):$(OBJ)
	$(CC) -o $(ALL) $(OBJ) $(CFLAGS) $(LIBPATH)
$(OBJ):%.o:%.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY:clean
clean:
	rm -rf *.o main
