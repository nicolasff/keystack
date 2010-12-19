#ifndef LOG_H
#define LOG_H

#include <event.h>
#include <queue.h>

struct cmd;
struct server;

struct log {
	int fd;
	time_t last_flush;
};

struct log *
log_open(const char *filename);

void
log_record(struct log *l, struct cmd *c);

void
log_rewrite(const char *filename, struct server *s);

#endif
