#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bt.h"

struct bt_node *
bt_node_new(int width) {

	struct bt_node *b = calloc(sizeof(struct bt_node), 1);
	b->width = width;

	b->entries = calloc(sizeof(struct bt_entry), width);
	b->children = calloc(sizeof(struct bt_node), width+1);

	return b;
}


struct bt_node *
bt_split(struct bt_node *b, int *key, int *value) {

	int pivot = (b->width) / 2;
	// printf("pivot = %d (key=%d, value=%d)\n", pivot, b->entries[pivot].key, b->entries[pivot].value);

	struct bt_node *right = bt_node_new(b->width);
	*key = b->entries[pivot].key;
	*value = b->entries[pivot].value;

	/* copy right part into a new node. */
	right->n = b->width - pivot - 1;
	memcpy(right->entries, b->entries + pivot + 1, right->n * sizeof(struct bt_entry));
	memcpy(right->children, b->children + pivot + 1, right->n * sizeof(struct bt_node));

	/* truncate current node */
	b->n = pivot;
	b->entries[b->n].key = 0;
	memset(b->entries+pivot+1, 0, (b->width - pivot - 1) * sizeof(struct bt_entry));
	memset(b->children+pivot+1, 0, (b->width - pivot - 1) * sizeof(struct bt_node));

	// printf("n=%d in :", right->n); bt_dump(right);
	return right;
}

struct bt_entry *
bt_lookup(struct bt_node *b, int k) {
	
	int i;

//	printf("look for %d in %p\n", k, b);
	for(i = 0; i <= b->n; ++i) {
		if(i != b->n && k == b->entries[i].key) { /* update */
			return b->entries + i;
		}
		if((i == b->n || k < b->entries[i].key) && b->children[i]) { /* there is more */
			// printf("lookup under.\n");
			return bt_lookup(b->children[i], k);
		}
	}
	return NULL;
}

static struct bt_node *
bt_root(struct bt_node *b) {
	return b->parent ? bt_root(b->parent) : b;
}


struct bt_node *
bt_insert(struct bt_node *b, int k, int v, struct bt_node *right) {

	int i, j;

	for(i = 0; i < b->n; ++i) {
		if(k == b->entries[i].key) { /* update in-place. */
			b->entries[i].value = v;
			return bt_root(b);
		}
		if(k < b->entries[i].key) { /* insert right before */

			/* either we have to go lower or we can just insert here */
			if(b->children[i] && !right) { /* there is more! insert under. */
				return bt_insert(b->children[i], k, v, right);
			}

			/* simple case: shift right */
			for(j = b->n; j > i; --j) {
				b->entries[j] = b->entries[j-1];
			}

			// insert at position i
			b->entries[i].key = k;
			b->entries[i].value = v;
			if(right) {
				// shift children to the right.
				for(j = b->n+1; j > i+1; --j) {
					b->children[j] = b->children[j-1];
				}
				b->children[i+1] = right;
				right->parent = b;
			}
			b->n++;
			return bt_root(b);
		}
	}

	// insert in this node.
	if(b->children[b->n] && !right) { /* there is more */
		// follow link to child
		return bt_insert(b->children[b->n], k, v, right);
	} else {
		if(b->n == b->width) {
			// full. split!
			int pivot_key, pivot_value;
			struct bt_node *added = bt_split(b, &pivot_key, &pivot_value);

			if(b->parent == NULL) {
				struct bt_node *root = bt_node_new(b->width);
				root->children[0] = b;
				b->parent = root;
				
			} 
			bt_insert(b->parent, pivot_key, pivot_value, added);
			return bt_insert(added, k, v, right);
			
		} else {
			// no problem, just append.
			b->entries[b->n].key = k;
			b->entries[b->n].value = v;
			if(right) {
				b->children[b->n+1] = right;
				right->parent = b;
			}
			b->n++;
			
		}
	}
	return bt_root(b);
}

struct bt_node *
bt_delete(struct bt_node *b, int k) {

	(void)b;
	(void)k;
#if 0
	printf("Deleting %c in %p\n", k, b);

	struct bt_entry *e = NULL;

	struct bt_node *b_cur = b;
	
	/* look for the bt_node, as well as the bt_entry */
	int i;
	while(1) {
		int go_down = 0;
		for(i = 0; i <= b_cur->n; ++i) {
			if(i != b_cur->n && k == b_cur->entries[i].key) { /* found */
				e = b_cur->entries + i;
				break;
			}
			if((i == b_cur->n || k < b_cur->entries[i].key) && b_cur->children[i]) { /* there is more */
				b_cur = b_cur->children[i];
				go_down = 1; /* go down a level */
				break;
			}
		}
		if(!go_down) {
			break;
		}
	}

	if(!e) { /* not found */
		return b;
	}

	printf("found in bt_node=%p, bt_entry=%p: key=%c, i = %d\n", b_cur, e, e->key, i);

	/* We need to consider several cases. */
	/* if the node is a leaf, this is easy, let's just move its right neighbours a bit to the left */

	if(b_cur->children[i] == NULL) { /* leaf, easiest case. */
		int j;
		for(j = i; j != b_cur->n - 1; ++j) {
			b_cur->children[j] = b_cur->children[j+1];
			b_cur->entries[j] = b_cur->entries[j+1];
		}
		b_cur->n--;

		/* check that the size of the node is still acceptable */
		if(b_cur->n >= b_cur->width/2) {
			return b; /* still good. */
		}

		/* we must readjust the tree. */
		printf("fffuuuu-\n");
	} else {
		printf("not a leaf.\n");

	}

#endif
	return NULL;
}

void
bt_dump_(struct bt_node *b, int indent) {

	int i;
	for(i = 0; i < indent; i++) printf("\t");
	printf("%p: ", b);
	if(!b) {
		printf("\n");
		return;
	}

	for(i = 0; i < b->n; ++i) {
		if(i == 0) {
			printf("[");
		} else {
			printf(",");
		}

		if(b->entries[i].key) {
			//printf("%2d", b->entries[i].key);
			printf("%c", b->entries[i].key);
		}
	}
	printf("]\n");

	for(i = 0; i < b->width+1; ++i) {
		if(b->children[i]) {
			bt_dump_(b->children[i], indent+1);
		}
	}
	if(b->children[0] != NULL) {
		printf("\n");
	}

}

void
bt_dump(struct bt_node *b) {

	bt_dump_(b, 0);
}

void
tree_dot(struct bt_node *b) {

	unsigned int self = (unsigned int)(long)(void*)b;

	if(b->parent == NULL) {
		printf("digraph G {\n");
	}

	printf("node_%x [label=\"", self);
	int i;
	for(i = 0; i < b->n; ++i) {
		printf("%d", b->entries[i].key);
		if(i != b->n - 1) {
			printf(", ");
		} 
	}
	printf("\"];\n");

	for(i = 0; i <= b->n; ++i) {
		if(b->children[i]) {
			printf("node_%x -> node_%x;\n", self, (unsigned int)(long)(b->children[i]));
			tree_dot(b->children[i]);
		}
	}

	if(b->parent == NULL) {
		printf("}\n");
	}
}

