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

	struct bt_node *parent;

	struct bt_entry *entries;
	struct bt_node **children;
};

struct bt_node *
bt_node_new(int width);

struct bt_node *
bt_insert(struct bt_node *b, int k, int v, struct bt_node *right);

int
bt_lookup(struct bt_node *b, int k);

void
tree_dump(struct bt_node *b);

void
tree_dot(struct bt_node *b);
#endif
