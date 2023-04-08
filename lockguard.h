#ifndef LOCKGUARD_H_
#define LOCKGUARD_H_

#include <pthread.h>
#include "mutex.h"

class LockGuard{
public:
	LockGuard():mutex_(Mutex()){
		mutex_.mutex_lock();
	}
	~LockGuard(){
		mutex_.mutex_unlock();
	}
private:
	Mutex mutex_;
};

#endif
