#include <stdlib.h>
#include <stdio.h>

#include "bt.h"

int
main() {
	struct bt_node *root;
	int i, n;

	root = bt_node_new(5);
	/*
	n = 20;
	
	for(i = n; i >= 1; --i) {
		root = bt_insert(root, i, i+1);
		bt_dump(root);
	}
	printf("----------------\n");
	
	root = bt_node_new(5);
	for(i = 1; i <= n; ++i) {
		root = bt_insert(root, i, i+1);
		bt_dump(root);
	}
	printf("----------------\n");
	*/

	n = 2000*1000;
#if 1
	for(i = n; i >= 4; i--) {
		struct bt_entry *e;
		root = bt_insert(root, i, i+1);
		e = bt_lookup(root, i); /* immediate read-back */
		if(e && e->value != i+1) {
			printf("at k=[%d]: %d\n", i, e->value);
		}
	}
	printf("built.\n");

	//bt_dump(root); return 0;
	for(i = 1; i < n; i++) {
		struct bt_entry *e = bt_lookup(root, i); /* read-back after the whole insertion */
		if(e && e->value != i+1) {
			printf("at k=[%d]: %d\n", i, e->value);
		}
	}
	printf("re-read.\n");
	/*
	bt_save(root, "large.bin");
	printf("saved.\n");
	root = bt_load("large.bin");
	printf("loaded.\n");
	*/


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

