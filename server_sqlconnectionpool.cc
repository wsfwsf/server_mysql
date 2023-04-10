#include "server_sqlconnectionpool.h"
#include <arpa/inet.h>

int server::num_online_user=0;

server::server(int port,std::string ip,int thread_num,int task_num,int pool_size_):port_(port),ip_(ip),
								    running_(false),
								    poll(Threadpoll(thread_num,task_num)),
								    pool_size(pool_size_),
								    connectionpool(sqlconnectionpool(pool_size)){
	listenfd_ = socket(AF_INET,SOCK_STREAM,0);

	int on=1;
	if(setsockopt(listenfd_,SOL_SOCKET,SO_REUSEADDR,(const void*)&on,sizeof(on))<0){
		std::cout<<"setsockopt error,"<<strerror(errno)<<std::endl;
	}

	memset(&servaddr_,0,sizeof(servaddr_));
	servaddr_.sin_family = AF_INET;
	servaddr_.sin_port = htons(port);
	servaddr_.sin_addr.s_addr = inet_addr(ip.c_str());
}

void server::Bind(){
	int on =1;
	socklen_t len = sizeof(on);
	if(setsockopt(listenfd_,SOL_SOCKET,SO_REUSEADDR,&on,len)<0){
		std::cout<<"errno: setsockopt,"<<strerror(errno)<<endl;
	}
	if(bind(listenfd_,(struct sockaddr*)&servaddr_,sizeof(servaddr_))<0){
		std::cout<<"errno: bind,"<<strerror(errno)<<std::endl;
	}	
}

void server::Listen(){
	if(listen(listenfd_,SOMAXCONN)<0){
		std::cout<<"errno: listen"<<strerror(errno)<<std::endl;
	}
}

void server::Read(int connfd,int epollfd){
	Message message;
	memset(&message,0,sizeof(message));
	//recv(connfd,&message.source,sizeof(message.source),0);
	//recv(connfd,&message.dest,sizeof(message.dest),0);
	readn(connfd,(void*)&message.source,sizeof(message.source));
	readn(connfd,(void*)&message.dest,sizeof(message.dest));
	uint32_t len =0;
	readn(connfd,&message.len,sizeof(message.len));
	len = ntohl(message.len);
	uint32_t ret=readn(connfd,(void*)message.info,len);
	if(ret==0){
		close(connfd);
		num_online_user--;
		cout<<connfd<<endl;
		ret=Removeuser(connfd);
		if(ret==0){
			std::cout<<"client close,remove success!\n";
		}else{
			std::cout<<"client close,remove fail!\n";
		}
		return;
	}else{
		std::cout<<"from ip:"<<message.source<<" to ip:"<<message.dest<<" message:"<<message.info<<'\n';
	}
	int toUserport =-1;
	toUserport=finduser(uint32_t(message.dest));
	if(toUserport==-1){
		std::cout<<"Not find\n";
		return;
	}
	writen(connfd,&message,HEAD_LEN+len);
}

int server::Removeuser(int connfd){
	LockGuard lockguard;
	MYSQL* conn = connectionpool.sqlconnection_get();
	string str;
       	str = str + "delete from online_user where port="+to_string(connfd);
	int ret = mysql_real_query(conn,str.c_str(),strlen(str.c_str()));
	if(ret!=0){
		cout<<"Removeuser error:"<<mysql_error(conn)<<endl;
		return -1;
	}
	connectionpool.sqlconnection_push(conn);
	return 0;
}

int server::finduser(uint32_t ip){
	LockGuard lockguard;
	MYSQL* conn = connectionpool.sqlconnection_get();
	string str;
	str = str + "select port from online_user where ip="+to_string(ip);
	int ret = mysql_real_query(conn,str.c_str(),strlen(str.c_str()));
	if(ret!=0){
		cout<<"finduser error:"<<mysql_error(conn)<<endl;
		connectionpool.sqlconnection_push(conn);
		return -1;
	}
	MYSQL_RES* res=mysql_store_result(conn);
	if(res==NULL){
		cout<<"finduser::mysql_store_result:"<<mysql_error(conn)<<endl;
		connectionpool.sqlconnection_push(conn);
		return -1;
	}
	int rows = mysql_num_rows(res);
	if(rows!=1){
		cout<<"finduser rows!=0"<<endl;
		connectionpool.sqlconnection_push(conn);
		return -1;
	}
	MYSQL_ROW row=mysql_fetch_row(res);
	connectionpool.sqlconnection_push(conn);

	return atoi(row[0]);
}

void server::Run(){
	poll.start();
	connectionpool.sqlconnectionpool_init();

	int epollfd = epoll_create(EPOLL_CLOEXEC);
	struct epoll_event event;
	event.data.fd = listenfd_;
	event.events = EPOLLIN;
	if(epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd_,&event)<0){
		std::cout<<"errno: epoll_ctl"<<strerror(errno)<<std::endl;
	}
	running_=true;
	int ret;
	std::vector<struct epoll_event> event_list(16);
	while(running_){
		ret=epoll_wait(epollfd,&*event_list.begin(),sizeof(event_list),-1);
		if(ret<0){
			std::cout<<"error: epoll_wait,"<<strerror(errno)<<std::endl;
		}else if(ret==0){
			continue;
		}
		for(int i=0;i<ret;i++){
			if(event_list[i].data.fd==listenfd_){
				struct sockaddr_in clientaddr;
				memset(&clientaddr,0,sizeof(clientaddr));
				socklen_t len = sizeof(clientaddr);
				int connfd = accept(listenfd_,(struct sockaddr*)&clientaddr,&len);
				event.data.fd=connfd;
				event.events=EPOLLIN|EPOLLET;
				epoll_ctl(epollfd,EPOLL_CTL_ADD,connfd,&event);
				num_online_user++;
				adduserTolist(clientaddr.sin_addr.s_addr,connfd);
				cout<<inet_ntoa(clientaddr.sin_addr)<<" Login,port:"<<connfd<<' ';
				cout<<"online_user:"<<num_online_user<<endl;
			}else if(event_list[i].events&EPOLLIN){
				int fd = event_list[i].data.fd;
				int efd = epollfd;
				poll.put(std::bind(&server::Read,this,fd,efd));
			}
		}
	}
}

void server::adduserTolist(uint32_t ip,int port){
	LockGuard lockguard;
	MYSQL* conn = connectionpool.sqlconnection_get();
	string str ;
	str = str + "insert into online_user values(" + to_string(ip) + "," + to_string(port) + ")";
	cout<<str<<endl;
	int ret = mysql_real_query(conn,str.c_str(),strlen(str.c_str()));
	if(ret!=0){
		cout<<"insert error: "<<mysql_error(conn)<<endl;
	}
}

server::~server(){}
