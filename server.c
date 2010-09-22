#include <server.h>
#include <ht/dict.h>
#include <client.h>
#include <cmd.h>

#include <string.h>

struct server *
server_new() {

	struct server *s = calloc(sizeof(struct server), 1);
	s->d = dict_new(1024);
	s->base = event_base_new();
	s->status = IDLE;

	return s;
}

void
server_get(struct server *s, struct client *c) {

	char *str;
	size_t sz;

	str = dict_get(s->d, c->key, c->key_sz, &sz);
	if(!str) {
		if(s->status == DUMPING) {
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
}


/**
 * Swap hash tables and dump the old one.
 */
int
server_split(struct server *s) {

	if(s->status == DUMPING) {
		return -1;
	}

	s->status = DUMPING;

	/* swap */
	s->d_old = s->d;
	s->d = dict_new(s->d_old->ht->sz);

	dump_flush(s->d_old, "/tmp/out.bin");

	return 0;
}


