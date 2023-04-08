#include "threadpoll.h"

Threadpoll::Threadpoll(int thread_num,int task_num):thread_num_(thread_num),task_num_(task_num){
	tasks_ = vector<threadFunc>();
	threads_.reserve(thread_num);
	pthread_mutex_init(&mutex_,NULL);
	pthread_cond_init(&Notempty_,NULL);
	pthread_cond_init(&Notfull_,NULL);
}

void Threadpoll::start(){
	for(int i=0;i<thread_num_;i++){
		threads_.emplace_back(Thread(std::bind(&Threadpoll::init_func,this)));
		threads_[i].start();
	}
}

void Threadpoll::join(){
	for(int i=0;i<thread_num_;i++){
		threads_[i].join();
	}
}

void Threadpoll::init_func(){
	threadFunc task;
	while(true){
		task = take();
		task();
	}	
}

threadFunc Threadpoll::take(){
	pthread_mutex_lock(&mutex_);
	while(tasks_.empty()){
		pthread_cond_wait(&Notempty_,&mutex_);
	}
	threadFunc task = tasks_.back();
	tasks_.pop_back();
	pthread_cond_signal(&Notfull_);
	pthread_mutex_unlock(&mutex_);
	return task;
}

void Threadpoll::put(threadFunc func){
	pthread_mutex_lock(&mutex_);
	while(tasks_.size()==task_num_){
		pthread_cond_wait(&Notfull_,&mutex_);
	}
	tasks_.emplace_back(func);
	pthread_cond_signal(&Notempty_);
	pthread_mutex_unlock(&mutex_);
}

Threadpoll::~Threadpoll(){}
