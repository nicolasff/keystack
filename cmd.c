#include <string.h>
#include <stdio.h>

#include <cmd.h>

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

	switch(c->cmd) {
		case CMD_GET:
			printf("GET ["); fflush(stdout);
			write(1, c->key, c->key_sz);
			printf("]\n");
			break;

		case CMD_SET:
			printf("SET ["); fflush(stdout);
			write(1, c->key, c->key_sz);
			printf("]  ["); fflush(stdout);
			write(1, c->val, c->val_sz);
			printf("]\n");
			break;
	}

}
