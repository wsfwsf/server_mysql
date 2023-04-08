#include "../threadpoll.h"
#include <unistd.h>
#include <iostream>
using namespace std;

void task(){
	int t = random()%5;
	cout<<" this task is sleep "<<t<<" s"<<endl;
	sleep(random()%5);
}

int main(){
	Threadpoll poll(5,10);
	poll.start();
	while(true){
		cout<<"put a task"<<endl;
		poll.put(task);
		sleep(1);	
	}
	poll.join();
	return 0;
}
