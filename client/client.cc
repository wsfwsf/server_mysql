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
#include "../read_write.h"
#include "../read_write.cc"
#include "../message_struct.h"

using namespace std;

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
	uint32_t ret;
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
		ret=read(STDIN_FILENO,message_send.info,sizeof(message_send.info));
		*(message_send.info+ret-1)='\0';
		message_send.len=htonl(ret);
		writen(sockfd,(void*)&message_send,HEAD_LEN+ret);
		readn(sockfd,(void*)&message_recv.source,sizeof(message_recv.source));
		readn(sockfd,(void*)&message_recv.dest,sizeof(message_recv.dest));
		readn(sockfd,(void*)&message_recv.len,sizeof(message_recv.len));
		ret=ntohl(message_recv.len);
		readn(sockfd,message_recv.info,ret);
		if(ret==0){
			std::cout<<"client close!"<<std::endl;
			break;
		}
		cout<<message_recv.info<<endl;
		memset(message_send.info,0,sizeof(message_send.info));
		memset(message_recv.info,0,sizeof(message_recv.info));
		message_send.len=0;
		message_recv.len=0;
	}
	return 0;
}
