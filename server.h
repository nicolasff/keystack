#ifndef SERVER_H
#define SERVER_H

#include <event.h>

struct dict;
struct client;
struct log;

struct server {
	struct event_base *base;
	struct dict *d;
	struct log *log;
};

struct server *
server_new();

void
server_set(struct server *s, struct client *c);

void
server_get(struct server *s, struct client *c);

#endif

