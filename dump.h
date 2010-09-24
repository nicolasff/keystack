#ifndef DUMP_H
#define DUMP_H

#include <pthread.h>

struct dict;
struct server;

struct dump_info {

	struct dict *d;
	struct server *s;
	char *db_name;
	char *index_name;


	/* mmap info */
	void *ptr;
	int pos;

	/* btree index */
	struct bt_node *index;

	/* current thread */
	pthread_t thread;
};

void
dump_flush(struct server *s, struct dict *d, char *db_name, char *index_name);

#endif /* DUMP_H */
