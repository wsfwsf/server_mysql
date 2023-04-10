#ifndef MESSAGE_STRUCT_H
#define MESSAGE_STRUCT_H

struct Message{
	uint32_t source;
	uint32_t dest;
	uint32_t len;
	char info[2048];
};

#define HEAD_LEN (2*sizeof(uint32_t)+sizeof(int))

#endif
