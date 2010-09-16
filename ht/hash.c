#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "dict.h"

float now() {
#if _POSIX_C_SOURCE >= 199309L
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return t.tv_sec * 1000 + t.tv_nsec / 1000000;
#else
	return 0;
#endif
}

void
print_item(char *k, size_t sz, void *v, void *p) {

	(void)sz;
	(void)p;
	printf("k=[%s], v=[%s]\n", k, (char*)v);
}

unsigned long
sdbm(char *s, size_t sz) {
	unsigned long hash = 0;
	char *p;

	for(p = s; p < s+sz; ++p) {
		hash = (*p) + (hash << 6) + (hash << 16) - hash;
	}

	return hash;
}


int
main() {

	/* speed measure */
	float t0, t1;

	int i;
	int count = 1*1000*1000;
	int key_size = 20, val_size = 20;

	struct dict *d = dict_new(count);
	/* uncomment the following to use custom functions. */
/*
	d->key_alloc = malloc;
	d->key_free = free;
	d->key_hash = sdbm;
*/

	char *keys = malloc(key_size * count);
	char *vals = malloc(val_size * count);
#define KEY(i) (keys + key_size * i)
#define VAL(i) (vals + val_size * i)

	/* uncomment the following to use custom functions. */
	/*
	d->key_dup = strndup;
	d->key_free = free;
	d->key_hash = sdbm;
	*/

	printf("[info] Each key has at least %d bytes of overhead.\n", (int)sizeof(struct bucket));

	printf("Setting up keys and values...\n");
	for(i = 0; i < count; ++i) {
		sprintf(KEY(i), "k-%d", i);
		sprintf(VAL(i), "v-%d", i);
	}

	printf("Adding...\n");
	t0 = now();
	for(i = 0; i < count; ++i) {
		if(i != 0 && i % 100000 == 0) {
			printf("Done %d/%d\n", i, count);
		}
		dict_add(d, KEY(i), 1+strlen(KEY(i)), VAL(i));
		/*
		if(i == 100) {
			dict_foreach(d, print_item, NULL);
		}
		*/
	}

	t1 = now();
	printf("Added %d elements in %0.2f sec: %0.2f/sec\n", count, (t1-t0)/1000.0, 1000*(float)count/(t1-t0));
	t0 = t1;

	printf("Reading back...\n");
	for(i = 0; i < count; ++i) {

		void * data = dict_get(d, KEY(i), 1+strlen(KEY(i)));

		if(!data || strcmp(data, VAL(i)) != 0) {
			printf("HT is corrupted: got [%s] instead of [%s]\n", (char*)data, VAL(i));
			return EXIT_FAILURE;
		}
	}

	t1 = now();
	printf("Retrieved %d elements in %0.2f sec: %0.2f/sec\n", count, (t1-t0)/1000.0, 1000*(float)count/(t1-t0));
	t0 = t1;

	free(keys);
	free(vals);

	dict_free(d);

	return EXIT_SUCCESS;
}
