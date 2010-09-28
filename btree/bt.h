#ifndef BTREE_H
#define BTREE_H

#include <arpa/inet.h>

/**
 * Format description:
 *
 * uint32_t	number of nodes
 * uint32_t	width of nodes (W)
 * uint32_t	offset of root node
 * uint16_t	size of root node
 *
 * followed by a list of nodes, each one with the following format:
 *
 * uchar	number of elements in the node (N)
 * [list of N keys & values, as following]
 * 	uint32_t	key size
 * 	N		key
 * 	uint32_t	value
 * [list of (N+1) offsets for children, possibly 0]
 * 	uint32_t	offset
 * 	uint16_t	size
 */

struct bt_node;

struct bt_entry {
	char *key;
	size_t key_size;
	int value;
};

struct bt_node {

	int n;
	int width;
	int leaf;

	struct bt_entry *entries;
	struct bt_node **children;
};

struct bt_node_static {
	char *buffer;
	int width;
	uint16_t n;
	struct bt_entry *entries;
	uint32_t *children_offsets;
	uint16_t *children_sizes;
};


struct bt_node *
bt_node_new(int width);

void
bt_free(struct bt_node *);

struct bt_node *
bt_insert(struct bt_node *r, char *k, size_t sz, int v);

struct bt_entry *
bt_lookup(struct bt_node *b, char *k, size_t sz);

struct bt_node *
bt_delete(struct bt_node *b, char *k);

int
bt_save(struct bt_node *b, const char *filename);

void
bt_dump(struct bt_node *b);

struct bt_node *
bt_load(const char *filename);

void
tree_dot(struct bt_node *b);

uint32_t
bt_save_to_mmap(struct bt_node *b, char *p, uint32_t *off, uint16_t *self_size);

int
bt_find(const char *filename, const char *key, uint16_t key_sz, int *out);

#endif
