#ifndef READ_WRITE_H
#define READ_WRITE_H

#include <unistd.h>
#include <iostream>
#include <errno.h>
#include <string.h>

struct packet{
	int len;
	char buf[1024];
};

uint32_t readn(int fd,void* buf,uint32_t count);
uint32_t writen(int fd,const void* buf,uint32_t count);

#endif
