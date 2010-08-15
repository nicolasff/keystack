#include <stdlib.h>
#include <stdio.h>

#include "bt.h"

int
main() {

	struct bt_node *root;
	
	root = bt_node_new(5);


	struct bt_node *a0, *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8;

	a0 = bt_node_new(4);
	a1 = bt_node_new(4);
	a2 = bt_node_new(4);
	a3 = bt_node_new(4);
	a4 = bt_node_new(4);
	a5 = bt_node_new(4);
	a6 = bt_node_new(4);
	a7 = bt_node_new(4);
	a8 = bt_node_new(4);

	a0->n = 1;
	a0->entries[0].key = 'M';
	a0->children[0] = a1;
	a0->children[1] = a2;

	a1->n = 2;
	a1->entries[0].key = 'D';
	a1->entries[1].key = 'G';
	a1->children[0] = a3;
	a1->children[1] = a4;
	a1->children[2] = a5;

	a2->n = 2;
	a2->entries[0].key = 'Q';
	a2->entries[1].key = 'T';
	a2->children[0] = a6;
	a2->children[1] = a7;
	a2->children[2] = a8;

	a3->n = 2;
	a3->entries[0].key = 'A';
	a3->entries[1].key = 'C';

	a4->n = 2;
	a4->entries[0].key = 'E';
	a4->entries[1].key = 'F';

	a5->n = 3;
	a5->entries[0].key = 'H';
	a5->entries[1].key = 'K';
	a5->entries[2].key = 'L';

	a6->n = 2;
	a6->entries[0].key = 'N';
	a6->entries[1].key = 'P';

	a7->n = 2;
	a7->entries[0].key = 'R';
	a7->entries[1].key = 'S';

	a8->n = 4;
	a8->entries[0].key = 'W';
	a8->entries[1].key = 'X';
	a8->entries[2].key = 'Y';
	a8->entries[3].key = 'Z';


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
	printf("\n\nroot:\n");bt_dump(root);
	root = bt_insert(root, 14, NULL);
	printf("\n\nroot:\n");bt_dump(root);
	root = bt_insert(root, 15, NULL);
	printf("\n\nroot:\n");bt_dump(root);
	root = bt_insert(root, 16, NULL);
	printf("\n\nroot:\n");bt_dump(root);
	root = bt_insert(root, 17, NULL);
	printf("\n\nroot:\n");bt_dump(root);
	root = bt_insert(root, 18, NULL);
	printf("\n\nroot:\n");bt_dump(root);
	root = bt_insert(root, 19, NULL);
	printf("\n\nroot:\n");bt_dump(root);
	root = bt_insert(root, 20, NULL);
	printf("\n\nroot:\n");bt_dump(root);
	root = bt_insert(root, 21, NULL);
	printf("\n\nroot:\n");bt_dump(root);
	root = bt_insert(root, 22, NULL);
	printf("\n\nroot:\n");bt_dump(root);
	root = bt_insert(root, 23, NULL);
	printf("\n\nroot:\n");bt_dump(root);
	root = bt_insert(root, 24, NULL);
	printf("\n\nroot:\n");bt_dump(root);
	root = bt_insert(root, 25, NULL);
	printf("\n\nroot:\n");bt_dump(root);
	root = bt_insert(root, 26, NULL);
	printf("\n\nroot:\n");bt_dump(root);
	root = bt_insert(root, 27, NULL);
	printf("\n\nroot:\n");bt_dump(root);
	root = bt_insert(root, 28, NULL);
	printf("\n\nroot:\n");bt_dump(root);
	*/

	bt_dump(a0);
	bt_delete(a0, 'H');
	bt_dump(a0);
	bt_delete(a0, 'T');
	bt_dump(a0);

	/*
	int i, n = 10000;
	for(i = 1; i < n; i++) {
		root = bt_insert(root, i, i+1, NULL);
		int j = bt_lookup(root, i);
		if(j != i+1) {
			printf("at k=[%d]: %d\n", i, j);
		}
	}

	//bt_dump(root); return 0;
	for(i = 1; i < n; i++) {
		int j = bt_lookup(root, i);
		if(j != i+1) {
			printf("at k=[%d]: %d\n", i, j);
		}
	}
	
	tree_dot(root);
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


	return EXIT_SUCCESS;
}

