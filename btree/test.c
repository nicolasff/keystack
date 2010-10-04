#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "bt.h"

int
main() {
	struct bt_node *root;
	int i, n;

	root = bt_node_new(651);
#if 0
	n = 20;
	/*
	for(i = n; i >= 1; --i) {
		root = bt_insert(root, i, i+1);
		bt_dump(root);
	}
	printf("----------------\n");
	
	root = bt_node_new(5);
	*/
	for(i = 1; i <= n; ++i) {
		root = bt_insert(root, i, i+1);
	}
	bt_dump(root);
	printf("----------------\n");
	bt_save(root, "/tmp/large.bin");
	root = bt_load("/tmp/large.bin");
	bt_dump(root);
	return 0;
#endif

	n = 1000*1000;
	//n = 20;
#if 1
	struct timespec t0, t1, t2, t3, t4;

	clock_gettime(CLOCK_MONOTONIC, &t0);
	//for(i = n; i >= 1; i--) {
	for(i = 1; i < n; i++) {
		struct bt_entry *e;
		char *key = calloc(20, 1);
		sprintf(key, "key-%d", i);
		root = bt_insert(root, key, strlen(key), i+1, 0);
		continue;
		e = bt_lookup(root, key, strlen(key)); /* immediate read-back */
		if(e && e->value_offset != (uint32_t)i+1) {
			printf("at k=[%d]: %d\n", i, e->value_offset);
		}
	}
	clock_gettime(CLOCK_MONOTONIC, &t1);
	printf("saving index of %d elements.\n", n);
	bt_save(root, "/tmp/large.bin");
	clock_gettime(CLOCK_MONOTONIC, &t2);
//	bt_dump(root);
	bt_free(root);
	printf("saved.\n");
	uint32_t offset, size;
	char *key = malloc(40);
	sprintf(key, "key-%d", 1 + (rand() % (n-1)));

	if(0 == bt_find("/tmp/large.bin", key, strlen(key), &offset, &size)) {
		printf("%s: offset=%d, size=%d\n", key, (int)offset, (int)size);
	} else {
		printf("could not find %s\n", key);
	}
	return 0;
//	root = bt_load("/tmp/large.bin");
//	clock_gettime(CLOCK_MONOTONIC, &t3);
//	printf("loaded.\n");
	bt_dump(root);

	for(i = 1; i < n; i++) {
		char *key = calloc(20, 1);
		sprintf(key, "key-%d", i);
		struct bt_entry *e = bt_lookup(root, key, strlen(key)); /* read-back after the whole insertion */
		if(!e) {
			printf("e=nil.\n");
		} else if(e->value_offset != (uint32_t)i+1) {
			printf("at k=[%d]: %d\n", i, e->value_offset);
		}
	}
	clock_gettime(CLOCK_MONOTONIC, &t4);
	printf("checked.\n");

	float mili0 = t0.tv_sec * 1000 + t0.tv_nsec / 1000000;
	float mili1 = t1.tv_sec * 1000 + t1.tv_nsec / 1000000;
	float mili2 = t2.tv_sec * 1000 + t2.tv_nsec / 1000000;
	float mili3 = t3.tv_sec * 1000 + t3.tv_nsec / 1000000;
	float mili4 = t4.tv_sec * 1000 + t4.tv_nsec / 1000000;

	printf("build time: %0.2f sec.\n", (mili1 - mili0)/1000.0);
	printf("dump time: %0.2f sec.\n", (mili2 - mili1)/1000.0);
	printf("reload time: %0.2f sec.\n", (mili3 - mili2)/1000.0);
	printf("check time: %0.2f sec.\n", (mili4 - mili3)/1000.0);

	/*
	root = bt_node_new(5);
	bt_insert(root, 706);
	bt_insert(root, 176);
	bt_insert(root, 601);
	bt_insert(root, 153);
	bt_insert(root, 513);
	bt_insert(root, 773);
	*/

#endif
	return EXIT_SUCCESS;
}

