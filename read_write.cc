#include "read_write.h"

uint32_t readn(int fd, void* buf, uint32_t count){
	size_t nleft=count;
	ssize_t nread;
	char* bufp=(char*)buf;
	while(nleft>0){
		if((nread=read(fd,bufp,nleft))<0){
			if(errno==EINTR)
				continue;
			std::cout<<"readn error: "<<strerror(errno)<<std::endl;
			return -1;
		}else if(nread==0){
			return count-nleft;
		}
		bufp += nread;
		nleft -= nread;
	}
	return count;
}

uint32_t writen(int fd,const void* buf,uint32_t count){
	size_t nleft=count;
	ssize_t nwritten;
	char* bufp=(char*)buf;

	while(nleft>0){
		if((nwritten=write(fd,bufp,nleft))<0){
			if(errno==EINTR)
				continue;
			std::cout<<"writen error:"<<strerror(errno)<<std::endl;
			return -1;
		}else if(nwritten==0){
			continue;
		}
		bufp += nwritten;
		nleft -= nwritten;
	}
	return count;
}
