#ifndef CMD_H
#define CMD_H

#include <event.h>
#include <client.h>

typedef enum {REPLY_BOOL, REPLY_STRING} reply_type;
typedef enum {CMD_GET = 1, CMD_SET} cmd_type;

struct cmd {
	cmd_type type;

	const char *key;
	uint32_t key_size;
	const char *val;
	uint32_t val_size;

	const char *raw; /* raw buffer */
	uint32_t raw_size;

	struct client *client;
};

struct cmd *
cmd_parse(const char *p, uint32_t size);

void
cmd_run(struct server *s, struct cmd *c);

void
cmd_reply(struct client *c, reply_type t, ...);

#endif

