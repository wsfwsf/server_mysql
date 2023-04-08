#ifndef SERVER_H
#define SERVER_H
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <vector>
#include <iostream>
#include <string>
#include <functional>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "lockguard.h"
#include "threadpoll.h"

struct Message{
	uint32_t source;
	uint32_t dest;
	char info[128];
};

struct online_user{
	uint32_t ip;
	int port;
	online_user* pre;
	online_user* next;
};

class server{
	public:
		server(int port,std::string ip,int thread_num,int task_num);
		void Bind();
		void Listen();
		void Run();
		void Read(int connfd,int epollfd);
		~server();
	private:
		std::string ip_;
		int listenfd_;

		int port_;
		struct sockaddr_in servaddr_;
		bool running_;
		Threadpoll poll;
		online_user* head,*tail;
		void adduserTolist(uint32_t ip,int port);
		int finduser(online_user* head,uint32_t ip);
		int Removeuser(online_user* head, int port);
		static int num_online_user;
};

#endif
