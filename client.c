#include <event.h>
#include <stdlib.h>

#include <client.h>
#include <server.h>
#include <net/loop.h>

void
client_reset(struct client *c) {

	c->buffer_got = c->buffer_sz = 0;

	client_listen(c, on_available_header);
}

void
client_listen(struct client *c, void (*fun)(int, short, void*)) {
	event_set(&c->ev, c->fd, EV_READ, fun, c);
	event_base_set(c->s->base, &c->ev);
	event_add(&c->ev, NULL);
}

