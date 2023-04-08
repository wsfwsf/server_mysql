#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <vector>
#include <iostream>

#include <stdio.h>
#include <string.h>
#include <errno.h>

using namespace std;

struct Message{
	uint32_t source;
	uint32_t dest;
	char info[64];
};

int main(){
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5188);
	servaddr.sin_addr.s_addr= inet_addr("192.168.183.128");
	socklen_t len = sizeof(servaddr);
	int i=0;
	connect(sockfd,(struct sockaddr*)&servaddr,len);
	int ret;
	Message message_send;
	Message message_recv;
	memset(&message_send,'\0',sizeof(message_send));
	memset(&message_recv,'\0',sizeof(message_recv));
	string s_ip="192.168.183.128";
	string d_ip="192.168.183.128";
	message_send.source = inet_addr(s_ip.c_str());
	message_send.dest = inet_addr(d_ip.c_str());
	cout<<(message_send.source)<<endl;
	while(true){
		//ret=read(STDIN_FILENO,message_send.info,sizeof(message_send.info));
		//*(message_send.info+ret-1)='\0';
		//send(sockfd,(void*)&message_send,sizeof(message_send),0);
		int ret=recv(sockfd,(void*)&message_recv,sizeof(message_recv),0);
		if(ret==0){
			std::cout<<"client close!"<<std::endl;
			break;
		}
		cout<<message_recv.info<<endl;
		memset(message_send.info,0,sizeof(message_send.info));
		memset(message_recv.info,0,sizeof(message_recv.info));
	}
	return 0;
}
