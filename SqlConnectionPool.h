#ifndef SQLCONNECTIONPOOL_H
#define SQLCONNECTIONPOOL_H

#include<mysql/mysql.h>
#include <vector>
#include <iostream>
#include <pthread.h>

using namespace std;

class sqlconnectionpool{
	public:
		sqlconnectionpool(int size_);
		void sqlconnectionpool_init();
		MYSQL* sqlconnection_get();
		void sqlconnection_push(MYSQL* mysql);
		~sqlconnectionpool();
	private:
		int size;
		vector<MYSQL*> vec;
		pthread_mutex_t mutex;
		pthread_cond_t notempty;
};

#endif
