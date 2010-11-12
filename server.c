#include <server.h>
#include <ht/dict.h>
#include <client.h>
#include <cmd.h>
#include <log.h>

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define THRESHOLD_MAX_COUNT	100000

struct server *
server_new() {

	struct server *s = calloc(sizeof(struct server), 1);
	s->d = dict_new(1024);
	s->d->key_dup = strndup;
	s->base = event_base_new();

	s->log = log_open("/tmp/log");

	return s;
}

void
server_get(struct server *s, struct client *c) {

	char *str;
	size_t sz;

	str = dict_get(s->d, c->key, c->key_sz, &sz);

	/* send reply */
	if(str) {
		cmd_reply(c, REPLY_STRING, str, sz);
	} else {
		cmd_reply(c, REPLY_BOOL, 0);
	}
}

void
server_set(struct server *s, struct client *c) {

	char *str = malloc(c->val_sz);
	memcpy(str, c->val, c->val_sz);

	//printf("set [%s] -> (%d)[", c->key, c->val_sz); fflush(stdout);
	//write(1, str, c->val_sz);
	//printf("]\n");
	
	log_record(s->log, c);

	dict_set(s->d, c->key, c->key_sz, str, c->val_sz);
	cmd_reply(c, REPLY_BOOL, 1);
}

