#ifndef CLIENT_H
#define CLIENT_H

#include <arpa/inet.h>

struct client {

	int fd;
	struct event *ev;
	struct server *s;

	char *buffer;
	uint32_t buffer_sz;
	uint32_t buffer_got;
};

void
client_reset(struct client *c);

void
client_listen(struct client *c, void (*fun)(int, short, void*));

void
client_free(struct client *c);

#endif
