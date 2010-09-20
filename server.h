#ifndef SERVER_H
#define SERVER_H

#include <event.h>

struct dict;

struct server {

	struct event_base *base;
	struct dict *d;

};

struct server *
server_new();

#endif

