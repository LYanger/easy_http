#include "http_server.h"

#include <unistd.h>

pthread_key_t key;

void usleep(request& req, response& res)
{
	Json::Value root;
	std::string sleep_time = req.get_param("usleep");
	if(sleep_time.empty()) {
		root["msg"] = "usleep is empty!";
		res.set_body(root);
		return ;
	}
	usleep(atoi(sleep_time.c_str()));
	root["code"] = 0;
	root["msg"] = "success!";
	res.set_body(root);
}

void thread_worker()
{
	pthread_t t = pthread_self();
	LOG_INFO("start thread data function, tid: %u", t);
	unsigned long* l = new unsigned long;
	*l = t;
	pthread_setspecific(key, l);
}

void hello(request& req, Json::Value& root)
{
	root["hello"] = "world";
	pthread_t t = pthread_self();
	int *tmp = (int*)pthread_getspecific(key);
	if(tmp == NULL) {
		LOG_INFO("not thread data, tid: %u", t);
		return ;
	}
	LOG_INFO("get therad data: %lu", *tmp);
}

void sayhello(request& req, Json::Value& root)
{
	std::string name = req.get_param("name");
	std::string age = req.get_param("age");

	root["name"] = name;
	root["age"] = atoi(age.c_str());
}

void login(request& req, Json::Value& root)
{
	std::string name = req.get_param("name");
	std::string pwd = req.get_param("pwd");

	LOG_DEBUG("login user which name: %s, pwd: %s", name.c_str(), pwd.c_str());
	root["code"] = 0;
	root["msg"] = "login success!";
}

int main(int argc, char** argv)
{
	if(argc < 2) {
		return -1;
	}

	pthread_key_create(&key, NULL);

	http_server server;

	thread_pool tp;
	tp.set_thread_start_cb(thread_worker);
	tp.set_pool_size(3);
	server.set_thread_pool(&tp);

	server.add_mapping("/hello", hello);
	server.add_mapping("/usleep", usleep);
	server.add_mapping("/sayhello", sayhello);
	server.add_mapping("/login", login, GET_METHOD | POST_METHOD);

	server.add_bind_ip("127.0.0.1");
	server.set_port(atoi(argv[1]));
	server.set_backlog(100000);
	server.set_max_events(100000);

	server.start_async();

	server.join();

	return 0;
}
