#include <dump.h>

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <string.h>

static void
dump_item(char *k, size_t sz, char *v, size_t v_sz, void *ctx) { 

	struct dump_info *di = ctx;
	int pos = di->pos;

	// printf("dumping item %s\n", k);

	/* key size */
	memcpy(di->ptr + di->pos, &sz, sizeof(uint32_t));
	di->pos += sizeof(uint32_t);

	/* key */
	memcpy(di->ptr + di->pos, k, sz);
	di->pos += sz;

	/* val size */
	memcpy(di->ptr + di->pos, &v_sz, sizeof(uint32_t));
	di->pos += sizeof(uint32_t);

	/* key */
	memcpy(di->ptr + di->pos, v, v_sz);
	di->pos += v_sz;

	// di->tree = bt_insert(di->tree, (int)(long)k, pos);
}

void *
dump_thread_main(void *ptr) {

	struct dump_info *di = ptr;

	printf("Dumping to [%s]\n", di->filename);

	/* mmap */

	dict_foreach(di->d, dump_item, di);

	dict_free(di->d);
	free(di);
	return NULL;
}


void
dump_flush(struct dict *d, char *filename) {

	struct dump_info *di = calloc(sizeof(struct dump_info), 1);

	di->d = d;
	di->filename = filename;

	pthread_create(&di->thread, NULL, dump_thread_main, di);
}

