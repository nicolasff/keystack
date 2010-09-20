#ifndef CMD_H
#define CMD_H

#include <event.h>
#include <client.h>

#define CMD_GET 1
#define CMD_SET 2

void
cmd_parse(struct client *c);


#endif

