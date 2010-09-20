#ifndef CLIENT_H
#define CLIENT_H

#include <arpa/inet.h>

struct client {

	int fd;
	struct event ev;
	struct event_base *base;

	char cmd;
	
	uint32_t key_sz;
	char *key;

	uint32_t val_sz;
	char *val;

	char *buffer;
	uint32_t buffer_sz;
	uint32_t buffer_got;
};


#endif
