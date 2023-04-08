#ifndef THREAD_H
#define THREAD_H

#include <iostream>
#include <pthread.h>
#include <functional>
using namespace std;

typedef std::function<void(void)> threadFunc;

void* start_thread(void*);

class Thread{
public:
	Thread(threadFunc threadfunc);
	void start();
	void join();
	~Thread();
	threadFunc threadFunc_;
private:
	pthread_t thread_id;
};

#endif
