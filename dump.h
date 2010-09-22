#ifndef DUMP_H
#define DUMP_H

#include <pthread.h>

struct dict;

struct dump_info {

	struct dict *d;
	char *filename;

	pthread_t thread;
};

void
dump_flush(struct dict *d, char *filename);

#endif /* DUMP_H */
