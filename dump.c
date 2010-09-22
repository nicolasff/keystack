#include <dump.h>

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>


void *
dump_thread_main(void *ptr) {

	struct dump_info *di = ptr;

	printf("Dumping to [%s]\n", di->filename);

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

