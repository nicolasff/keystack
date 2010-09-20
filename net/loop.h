#ifndef NET_LOOP_H
#define NET_LOOP_H

#include <event.h>
#include <server.h>

int
net_start(const char *ip, short port);

void
net_loop(int fd, struct server *s);

void
on_available_header(int fd, short event, void *ptr);

#endif

