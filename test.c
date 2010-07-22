#include <stdlib.h>
#include <stdio.h>

#include "bt.h"

int
main() {

	struct bt_node *root, *left, *right;
	
	root = bt_node_new(5);

//	printf("root = %p\n", root);
//	printf("left = %p\n", left);
//	printf("right = %p\n", right);


	/*
	left->parent = root;
	left->n = 5;
	left->entries[0].key = 1;
	left->entries[1].key = 2;
	left->entries[2].key = 3;
	left->entries[3].key = 4;
	left->entries[4].key = 5;

	right->parent = root;
	right->n = 3;
	right->entries[0].key = 11;
	right->entries[1].key = 12;
	right->entries[2].key = 13;

	root->n = 1;
	root->entries[0].key = 10;
	root->children[0] = left;
	root->children[1] = right;
	*/
/*
	root = bt_insert(root, 6, NULL);
	printf("\n\nroot:\n");tree_dump(root);
	root = bt_insert(root, 14, NULL);
	printf("\n\nroot:\n");tree_dump(root);
	root = bt_insert(root, 15, NULL);
	printf("\n\nroot:\n");tree_dump(root);
	root = bt_insert(root, 16, NULL);
	printf("\n\nroot:\n");tree_dump(root);
	root = bt_insert(root, 17, NULL);
	printf("\n\nroot:\n");tree_dump(root);
	root = bt_insert(root, 18, NULL);
	printf("\n\nroot:\n");tree_dump(root);
	root = bt_insert(root, 19, NULL);
	printf("\n\nroot:\n");tree_dump(root);
	root = bt_insert(root, 20, NULL);
	printf("\n\nroot:\n");tree_dump(root);
	root = bt_insert(root, 21, NULL);
	printf("\n\nroot:\n");tree_dump(root);
	root = bt_insert(root, 22, NULL);
	printf("\n\nroot:\n");tree_dump(root);
	root = bt_insert(root, 23, NULL);
	printf("\n\nroot:\n");tree_dump(root);
	root = bt_insert(root, 24, NULL);
	printf("\n\nroot:\n");tree_dump(root);
	root = bt_insert(root, 25, NULL);
	printf("\n\nroot:\n");tree_dump(root);
	root = bt_insert(root, 26, NULL);
	printf("\n\nroot:\n");tree_dump(root);
	root = bt_insert(root, 27, NULL);
	printf("\n\nroot:\n");tree_dump(root);
	root = bt_insert(root, 28, NULL);
	printf("\n\nroot:\n");tree_dump(root);
	*/

	int i, n = 10000;
	for(i = 1; i < n; i++) {
		root = bt_insert(root, i, i+1, NULL);
		int j = bt_lookup(root, i);
		if(j != i+1) {
			printf("at k=[%d]: %d\n", i, j);
		}
	}

	//tree_dump(root); return 0;
	for(i = 1; i < n; i++) {
		int j = bt_lookup(root, i);
		if(j != i+1) {
			printf("at k=[%d]: %d\n", i, j);
		}
	}
	
	tree_dot(root);


	/*
	root = bt_node_new(5);
	bt_insert(root, 706);
	bt_insert(root, 176);
	bt_insert(root, 601);
	bt_insert(root, 153);
	bt_insert(root, 513);
	bt_insert(root, 773);
	*/


	return EXIT_SUCCESS;
}

