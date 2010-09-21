#include <string.h>
#include <stdio.h>

#include <cmd.h>
#include <ht/dict.h>
#include <server.h>

static void
cmd_parse_get(struct client *c) {

	/* key size */
	memcpy(&c->key_sz, c->buffer + 1, sizeof(uint32_t));
	c->key_sz = ntohl(c->key_sz);

	/* key */
	c->key = c->buffer + 1 + sizeof(uint32_t);
}

static void
cmd_parse_set(struct client *c) {

	/* key size */
	memcpy(&c->key_sz, c->buffer + 1, sizeof(uint32_t));
	c->key_sz = ntohl(c->key_sz);

	/* key */
	c->key = c->buffer + 1 + sizeof(uint32_t);

	/* value size */
	memcpy(&c->val_sz, c->buffer + 1 + sizeof(uint32_t) + c->key_sz, sizeof(uint32_t));
	c->val_sz = ntohl(c->val_sz);

	/* value */
	c->val = c->buffer + 1 + sizeof(uint32_t) + c->key_sz + sizeof(uint32_t);
}

void
cmd_parse(struct client *c) {

	/* command */
	c->cmd = c->buffer[0];
	switch(c->cmd) {
		case CMD_GET:
			cmd_parse_get(c);
			break;

		case CMD_SET:
			cmd_parse_set(c);
			break;
	}
}

void
cmd_run(struct client *c) {

	struct server *s = c->s;
	void* v;
	struct string *str;

	switch(c->cmd) {
		case CMD_GET:
			/*
			printf("GET ["); fflush(stdout);
			write(1, c->key, c->key_sz);
			printf("]\n");
			*/

			v = dict_get(c->s->d, c->key, c->key_sz);
			if(v) {
				cmd_reply(c, REPLY_STRING, v);
			} else {
				cmd_reply(c, REPLY_BOOL, 0);
			}

			break;

		case CMD_SET:
			/*
			printf("SET ["); fflush(stdout);
			write(1, c->key, c->key_sz);
			printf("]  ["); fflush(stdout);
			write(1, c->val, c->val_sz);
			printf("]\n");
			*/

			str = malloc(sizeof(struct string));
			str->sz = c->val_sz;
			str->data = malloc(c->val_sz);
			memcpy(str->data, c->val, c->val_sz);

			dict_add(s->d, c->key, c->key_sz, str);
			cmd_reply(c, REPLY_BOOL, 1);

			break;
	}
}

void
cmd_reply(struct client *c, reply_type t, ...) {

	struct string *s;
	uint32_t sz;
	char b, type = t;
	va_list ap;
	va_start(ap, t);

	/* write type first */
	write(c->fd, &type, 1);

	switch(t) {
		case REPLY_BOOL:
			b = va_arg(ap, int);
			write(c->fd, &b, 1);
			break;

		case REPLY_STRING:
			s = va_arg(ap, struct string *); /* get string */
			sz = htonl(s->sz);
			write(c->fd, &sz, sizeof(uint32_t));
			write(c->fd, s->data, s->sz);
			break;
	}
	
	va_end(ap);
	
	client_reset(c);
}

