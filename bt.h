#ifndef BTREE_H
#define BTREE_H

struct bt_node;

struct bt_entry {
	int key;
	int value;
};

struct bt_node {

	int width;
	int n;

	int leaf;

	struct bt_entry *entries;
	struct bt_node **children;
};

struct bt_node *
bt_node_new(int width);

void
bt_free(struct bt_node *);

struct bt_node *
bt_insert(struct bt_node *r, int k, int v);

struct bt_entry *
bt_lookup(struct bt_node *b, int k);

struct bt_node *
bt_delete(struct bt_node *b, int k);

int
bt_save(struct bt_node *b, const char *filename);

void
bt_dump(struct bt_node *b);

struct bt_node *
bt_load(const char *filename);

void
tree_dot(struct bt_node *b);

#endif
