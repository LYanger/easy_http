OBJ=log.o log_config.o thread_pool.cpp test_thread_pool.cpp  
CC=clang++ 
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
