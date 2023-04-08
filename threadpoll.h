#ifndef THREADPOLL_H
#define THREADPOLL_H

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <unistd.h>
#include <functional>
#include <condition_variable>

#include "thread.h"

using namespace std;
typedef std::function<void(void)> threadFunc;
class Threadpoll{
	public:
		Threadpoll(int thread_num, int task_num);
		void start();
		void join();
		void init_func();
		~Threadpoll();

		threadFunc take();
		void put(threadFunc func);
	private:
		int thread_num_;
		int task_num_;
		pthread_mutex_t mutex_;
		pthread_cond_t Notfull_,Notempty_;

		vector<threadFunc> tasks_;
		vector<Thread> threads_;

};

#endif
