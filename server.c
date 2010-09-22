#include <server.h>
#include <ht/dict.h>
#include <client.h>
#include <cmd.h>

#include <string.h>

#define THRESHOLD_MAX_COUNT	1000

struct server *
server_new() {

	struct server *s = calloc(sizeof(struct server), 1);
	s->d = dict_new(1024);
	s->d->key_dup = strndup;
	s->base = event_base_new();
	s->state = IDLE;

	return s;
}

void
server_get(struct server *s, struct client *c) {

	char *str;
	size_t sz;

	str = dict_get(s->d, c->key, c->key_sz, &sz);
	if(!str) {
		if(s->state == DUMPING) {
		str = dict_get(s->d_old, c->key, c->key_sz, &sz);
		} else {
			/* TODO: disk lookup */
		}
	}

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

	dict_set(s->d, c->key, c->key_sz, str, c->val_sz);
	cmd_reply(c, REPLY_BOOL, 1);

	if(1/*dict_count(s->d) > THRESHOLD_MAX_COUNT*/) { /* TODO: use a proper condition */
		server_split(s);
	}
}


/**
 * Swap hash tables and dump the old one.
 */
int
server_split(struct server *s) {

	if(s->state == DUMPING) {
		return -1;
	}

	s->state = DUMPING;

	/* swap */
	s->d_old = s->d;
	s->d = dict_new(dict_count(s->d_old));
	s->d->key_dup = strndup;

	dump_flush(s, s->d_old, "/tmp/out.bin");

	return 0;
}


