#include "SqlConnectionPool.h"

sqlconnectionpool::sqlconnectionpool(int size_):size(size_),vec(vector<MYSQL*>(size)),mutex(PTHREAD_MUTEX_INITIALIZER),notempty(PTHREAD_COND_INITIALIZER){}

void sqlconnectionpool::sqlconnectionpool_init(){
	for(int i=0;i<size;i++){
		MYSQL* mysql=NULL;
		mysql=mysql_init(mysql);
		if(mysql==NULL){
			cout<<"mysql_init error: "<<mysql_error(mysql)<<endl;
			return;
		}
		mysql=mysql_real_connect(mysql,"localhost","wsf","123wang","wsfdb",0,NULL,0);
		if(mysql==NULL){
			cout<<"mysql_real_connect err:"<<mysql_error(mysql)<<endl;
			return;
		}
		vec[i]=mysql;
	}
	cout<<"sql connection pool init success!"<<endl;
}

MYSQL* sqlconnectionpool::sqlconnection_get(){
	pthread_mutex_lock(&mutex);
	while(vec.size()==0){
		pthread_cond_wait(&notempty,&mutex);
	}
	MYSQL* ret = vec.back();
	vec.pop_back();
	pthread_mutex_unlock(&mutex);
	return ret;
}

void sqlconnectionpool::sqlconnection_push(MYSQL* mysql){
	pthread_mutex_lock(&mutex);
	vec.push_back(mysql);
	pthread_cond_broadcast(&notempty);
	pthread_mutex_unlock(&mutex);
}

sqlconnectionpool::~sqlconnectionpool(){
	for(int i=0;i<size;i++){
		mysql_close(vec[i]);
	}
	cout<<"sql connection pool destory!"<<endl;
}
