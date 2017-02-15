#include "epoll_socket.h"

#include <sys/socket.h>

class echo_watcher : public epoll_socket_watcher {
public:
	virtual int on_accept(epoll_context& epoll_ctx) {
		return 0;
	}
	virtual int on_readable(epoll_event& event) {
		epoll_context* epoll_ctx = (epoll_context*)(event.data.ptr);
		int fd = epoll_ctx->connfd_;
		memset(buff, '\0', 1024);
		int ret = recv(fd, buff, 1024, 0);
		if(ret == 0)
			return READ_CLOSE;
		printf("received: %s\n", buff);
		return READ_OVER;
	}
	virtual int on_writeable(epoll_context& epoll_ctx) {
		int fd = epoll_ctx.connfd_;
		send(fd, buff, strlen(buff), 0);
		printf("send: %s\n", buff);
		return WRITE_ALIVE;
	}
	virtual int on_close(epoll_context& epoll_ctx) {
		printf("bye\n");
		return 0;
	}
private:
	char buff[1024];
};

class echo_server {
public:
	echo_server(int bl, int max_ev, int pt) : backlog(bl), max_events(max_ev), port(pt)
	{}
	
	int start() {
		ep_socket.set_port(port);
		ep_socket.set_backlog(backlog);
		ep_socket.set_max_events(max_events);
		ep_socket.set_watcher(&echo_handler);
		return ep_socket.start_epoll();
	}	
private:
	int           backlog;
	int			  max_events;
	int 		  port;
	echo_watcher  echo_handler;
	epoll_socket  ep_socket;
};


#include <string.h>

int main()
{
	echo_server server(5, 1024, 6666);
	server.start();
	return 0;
}
