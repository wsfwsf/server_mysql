#include "../thread.h"
#include <iostream>

using namespace std;

void Print(){
	cout<<"thread test"<<endl;
}

int main(){
	Thread t(Print);
	t.start();
	t.join();
	return 0;
}
