#ifndef DICT_H
#define DICT_H

#include <stdlib.h>

struct bucket {
	
	char *k;
	void *v;

	size_t sz;

	struct bucket *collision_prev; /* same hash! */
	struct bucket *collision_next;

	struct bucket *next;
	struct bucket *prev;
} __attribute__((__packed__));

struct ht {
	size_t sz;

	struct bucket *slots;
	struct bucket *first;
};

struct dict {
	long count;

	unsigned long (*key_hash)(char *, size_t);
	char* (*key_dup)(const char *, size_t);
	void (*key_free)(void*);

	struct ht *ht;
	struct ht *ht_old;
};


/* Create a new dictionary */
struct dict *
dict_new(long sz);

/* Delete a dictionary */
void
dict_free(struct dict *d);

/* Add d[k] = v, with sz = length(k) */
void
dict_add(struct dict *d, char *k, size_t sz, void *v);

/* retrieve d[k], with sz = length(k) */
void*
dict_get(struct dict *d, char *k, size_t sz);

/* key, key size, value, "data" extra */
typedef void (*foreach_cb)(char *, size_t, void*, void*);

/* run f(key, key size, value, data) on each item. */
void
dict_foreach(struct dict *d, foreach_cb fun, void *data);

#endif /* DICT_H */
