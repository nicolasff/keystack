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

struct bt_node *
bt_insert(struct bt_node *b, int k, int v, struct bt_node *right);

struct bt_node *
bt_insert_(struct bt_node *b, int k, int v, struct bt_node *left, struct bt_node *right);

struct bt_node *
bt_insert_2(struct bt_node *r, int k);

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
