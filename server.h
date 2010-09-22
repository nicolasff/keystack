#ifndef SERVER_H
#define SERVER_H

#include <event.h>

struct dict;
struct client;

typedef enum {IDLE, DUMPING} server_status;

struct server {

	struct event_base *base;

	server_status status;

	struct dict *d;
	struct dict *d_old;

};

struct server *
server_new();

void
server_set(struct server *s, struct client *c);

void
server_get(struct server *s, struct client *c);

#endif

