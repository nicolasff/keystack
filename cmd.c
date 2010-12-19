#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <cmd.h>
#include <ht/dict.h>
#include <server.h>

struct cmd *
cmd_new(const char *p, uint32_t size) {

	struct cmd *c = calloc(1, sizeof(struct cmd));
	c->raw = p;
	c->raw_size = size;

	return c;
}

void
cmd_free(struct cmd *c) {
	free((char*)c->raw);
	free(c);
}

static struct cmd *
cmd_parse_get(const char *p, uint32_t size) {

	struct cmd *c = cmd_new(p, size);
	c->type = CMD_GET;

	/* key size */
	memcpy(&c->key_size, p + 1, sizeof(uint32_t));
	c->key_size = ntohl(c->key_size);

	/* key */
	c->key = p + 1 + sizeof(uint32_t);

	return c;
}

static struct cmd *
cmd_parse_set(const char *p, uint32_t size) {

	struct cmd *c = cmd_new(p, size);
	c->type = CMD_SET;

	/* key size */
	memcpy(&c->key_size, p + 1, sizeof(uint32_t));
	c->key_size = ntohl(c->key_size);

	/* key */
	c->key = p + 1 + sizeof(uint32_t);

	/* value size */
	memcpy(&c->val_size, p + 1 + sizeof(uint32_t) + c->key_size, sizeof(uint32_t));
	c->val_size = ntohl(c->val_size);

	/* value */
	c->val = p + 1 + sizeof(uint32_t) + c->key_size + sizeof(uint32_t);

	return c;
}

struct cmd *
cmd_parse(const char *p, uint32_t size) {

	/* command */
	switch((uint32_t)p[0]) {
		case CMD_GET:
		case CMD_DEL:
			return cmd_parse_get(p, size);

		case CMD_SET:
			return cmd_parse_set(p, size);
	}

	return NULL;
}

/**
 * Run command
 */
void
cmd_run(struct server *s, struct cmd *c) {

	switch(c->type) {
		case CMD_GET:
			server_get(s, c);
			break;

		case CMD_DEL:
			server_del(s, c);
			break;

		case CMD_SET:
			server_set(s, c);
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

