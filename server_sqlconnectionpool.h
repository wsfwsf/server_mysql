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

#include <stdlib.h>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "lockguard.h"
#include "threadpoll.h"

#include <mysql/mysql.h>
#include "SqlConnectionPool.h"

struct Message{
	uint32_t source;
	uint32_t dest;
	char info[128];
};

class server{
	public:
		server(int port,std::string ip,int thread_num,int task_num,int sql_num);
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

		int pool_size;
		sqlconnectionpool connectionpool;
		MYSQL* mysql;
		MYSQL_RES* mysql_res;
		MYSQL_ROW row;

		void adduserTolist(uint32_t ip,int port);
		int finduser(uint32_t ip);
		int Removeuser(int port);
		static int num_online_user;
};

#endif
