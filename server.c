#include <server.h>
#include <ht/dict.h>

struct server *
server_new() {

	struct server *s = calloc(sizeof(struct server), 1);
	s->d = dict_new(1024);
	s->base = event_base_new();

	return s;
}

