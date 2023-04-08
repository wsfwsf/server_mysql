#include "server.h"
#include <arpa/inet.h>

int server::num_online_user=0;

server::server(int port,std::string ip,int thread_num,int task_num):port_(port),ip_(ip),
								    running_(false),
								    poll(Threadpoll(thread_num,task_num)),
								    head(new online_user()),tail(head){
	listenfd_ = socket(AF_INET,SOCK_STREAM,0);

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
	recv(connfd,&message.source,sizeof(message.source),0);
	recv(connfd,&message.dest,sizeof(message.dest),0);
	int ret=recv(connfd,message.info,sizeof(message.info),0);
	if(ret==0){
		close(connfd);
		num_online_user--;
		cout<<connfd<<endl;
		ret=Removeuser(head,connfd);
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
	toUserport=finduser(head,uint32_t(message.dest));
	if(toUserport==-1){
		std::cout<<"Not find\n";
		return;
	}
	send(toUserport,&message,ret,0);
}

int server::Removeuser(online_user* head,int connfd){
	LockGuard lockguard;
	online_user* p=head->next;
	while(p!=nullptr){
		if(p->port==connfd){
			if(p->next==nullptr){
				tail=p->pre;
				tail->next=nullptr;
				delete p;
			}else{
				p->next->pre=p->pre;
				p->pre->next=p->next;
				delete p;
			}
			return 0;
		}
		p=p->next;
	}
	return -1;
}

int server::finduser(online_user* head,uint32_t ip){
	LockGuard lockguard;
	online_user* p=head->next;
	while(p!=nullptr){
		if(p->ip==ip){
			return p->port;
		}
		p=p->next;
	}
	return -1;
}

void server::Run(){
	poll.start();

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
	online_user* tmp = new online_user();
	tmp->next=nullptr;
	tmp->ip=ip;
	tmp->port=port;
	tail->next=tmp;
	tmp->pre=tail;
	tail=tail->next;
}

server::~server(){}
