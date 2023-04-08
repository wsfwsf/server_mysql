#include "thread.h"

typedef std::function<void(void)> threadFunc;

Thread::Thread(threadFunc func):threadFunc_(func){}

void Thread::start(){
	pthread_create(&thread_id,NULL,&start_thread,this);
}

void Thread::join(){
	pthread_join(thread_id,NULL);
}

Thread::~Thread(){}

void* start_thread(void* value){
	Thread *thread = (Thread*) value;
	thread->threadFunc_();
	return (void*)0;
}
