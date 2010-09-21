#ifndef CMD_H
#define CMD_H

#include <event.h>
#include <client.h>

#define CMD_GET 1
#define CMD_SET 2

typedef enum {REPLY_BOOL, REPLY_STRING} reply_type;

void
cmd_parse(struct client *c);

void
cmd_run(struct client *c);

void
cmd_reply(struct client *c, reply_type t, ...);

struct string {
	char *data;
	uint32_t sz;
};

#endif

