#ifndef NET_LOOP_H
#define NET_LOOP_H

#include <arpa/inet.h>

struct dict;

struct client {

	int fd;
	char cmd;
	
	uint32_t key_sz;
	char *key;

	uint32_t val_sz;
	char *val;
};

int
net_start(const char *ip, short port);

void
net_loop(int fd, struct dict *d);

#endif

