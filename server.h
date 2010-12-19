#ifndef SERVER_H
#define SERVER_H

#include <event.h>

struct dict;
struct cmd;
struct log;

struct server {
	struct event_base *base;
	struct dict *d;
	struct log *log;
};

struct server *
server_new(const char *logfile);

void
server_set(struct server *s, struct cmd *c);

void
server_get(struct server *s, struct cmd *c);

void
server_del(struct server *s, struct cmd *c);

#endif

