#ifndef MUTEX_H_
#define MUTEX_H_

#include <pthread.h>

class Mutex{
public:
	Mutex(){
		pthread_mutex_init(&mutex,NULL);
	}
	void mutex_lock(){
		pthread_mutex_lock(&mutex);	
	}
	void mutex_unlock(){
		pthread_mutex_unlock(&mutex);
	}
	~Mutex(){
	
	}
private:
	pthread_mutex_t mutex;
};

#endif
