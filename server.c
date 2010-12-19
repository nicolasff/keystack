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

static char *
duplicate_key(const char *k, size_t sz) {

	char *ret = malloc(sz);
	return memcpy(ret, k, sz);
}

struct server *
server_new(const char *logfile) {

	struct stat file_info;

	struct server *s = calloc(sizeof(struct server), 1);
	s->d = dict_new(1024);
	s->d->key_dup = duplicate_key;
	s->base = event_base_new();

	if(0 == stat(logfile, &file_info)) { /* file exists, rewrite. */
		log_rewrite(logfile, s);
	}

	s->log = log_open(logfile);

	return s;
}

void
server_get(struct server *s, struct cmd *c) {

	char *str;
	size_t sz;

	/* HT lookup */
	str = dict_get(s->d, (char*)c->key, c->key_size, &sz);

	/* send reply to client */
	if(!c->client) {
		return;
	}
	if(str) {
		cmd_reply(c->client, REPLY_STRING, str, sz);
	} else {
		cmd_reply(c->client, REPLY_BOOL, 0);
	}
}

void
server_del(struct server *s, struct cmd *c) {

	/* HT lookup */
	int ret = dict_remove(s->d, (char*)c->key, c->key_size);

	/* send reply to client */
	if(!c->client) {
		return;
	}
	cmd_reply(c->client, REPLY_BOOL, (ret == 0 ? 1 : 0));
}

void
server_set(struct server *s, struct cmd *c) {

	/* duplicate client value */
	char *str = malloc(c->val_size);
	memcpy(str, c->val, c->val_size);
	
	/* log write */
	if(s->log) {
		log_record(s->log, c);
	}

	dict_set(s->d, (char*)c->key, c->key_size, str, c->val_size);

	/* send reply to client */
	if(!c->client) {
		return;
	}
	cmd_reply(c->client, REPLY_BOOL, 1);
}

