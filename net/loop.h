#ifndef NET_LOOP_H
#define NET_LOOP_H

#include <event.h>

struct dict;

int
net_start(const char *ip, short port);

void
net_loop(int fd, struct dict *d);

#endif

