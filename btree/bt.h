#ifndef BTREE_H
#define BTREE_H

#include <arpa/inet.h>

struct bt_node;

struct bt_entry {
	char *key;
	size_t key_size;
	int value;
};

struct bt_node {

	int width;
	int n;

	int leaf;
	long id;

	struct bt_entry *entries;
	struct bt_node **children;
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
bt_save_to_mmap(struct bt_node *b, char *p, uint32_t *off, uint32_t *self_size);

#endif
