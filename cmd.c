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

/**
 * Run command
 */
void
cmd_run(struct client *c) {

	struct server *s = c->s;
	void* v;

	switch(c->cmd) {
		case CMD_GET:
			server_get(c->s, c);
			break;

		case CMD_SET:
			server_set(c->s, c);
			break;
	}
}

/* send reply to client */
void
cmd_reply(struct client *c, reply_type t, ...) {

	char *s;
	uint32_t sz, sz_net;
	char b, type = t;
	va_list ap;
	va_start(ap, t);


	switch(t) {
		case REPLY_BOOL:
			/* write size first */
			sz = htonl(2);
			write(c->fd, &sz, sizeof(uint32_t));

			/* write type, 1 byte */
			write(c->fd, &type, 1);

			/* write value, 1 byte */
			b = va_arg(ap, int);
			write(c->fd, &b, 1);
			break;

		case REPLY_STRING:
			s = va_arg(ap, char*); /* get string */
			sz = va_arg(ap, size_t);

			/* write total size first */
			sz_net = htonl(1 + sizeof(uint32_t) + sz);
			write(c->fd, &sz, sizeof(uint32_t));

			sz_net = htonl(sz);
			write(c->fd, &sz_net, sizeof(uint32_t));
			write(c->fd, s, sz);
			break;
	}
	
	va_end(ap);
	
	client_reset(c);
}

