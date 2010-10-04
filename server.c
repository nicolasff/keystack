#include <server.h>
#include <ht/dict.h>
#include <client.h>
#include <cmd.h>

#include <string.h>
#include <stdio.h>

#define THRESHOLD_MAX_COUNT	100000

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
		//printf("not found.\n");
		if(s->state == DUMPING) {
			//printf("check in other table.\n");
			str = dict_get(s->d_old, c->key, c->key_sz, &sz);
		} else {
			printf("disk lookup.\n");
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

	//printf("set [%s] -> (%d)[", c->key, c->val_sz); fflush(stdout);
	//write(1, str, c->val_sz);
	//printf("]\n");
	dict_set(s->d, c->key, c->key_sz, str, c->val_sz);
	cmd_reply(c, REPLY_BOOL, 1);

	if(dict_count(s->d) >= THRESHOLD_MAX_COUNT) { /* TODO: use a proper condition */
		server_split(s);
	}
}


/**
 * Swap hash tables and dump the old one.
 */
int
server_split(struct server *s) {

	char *db_name, *index_name;
	if(s->state == DUMPING) {
		return -1;
	}

	s->state = DUMPING;

	/* swap */
	s->d_old = s->d;
	s->d = dict_new(dict_count(s->d_old));
	s->d->key_dup = strndup;

	db_name = calloc(50, 1);
	sprintf(db_name, "/tmp/kv-%d.db", s->dump_count);
	index_name = calloc(50, 1);
	sprintf(index_name, "/tmp/kv-%d.index", s->dump_count);

	s->dump_count++;
	dump_flush(s, s->d_old, db_name, index_name);

	return 0;
}


