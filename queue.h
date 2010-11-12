#ifndef QUEUE_LOCKS_H
#define QUEUE_LOCKS_H

#include <pthread.h>

typedef struct queue_node_t_ {
	void *data;
	struct queue_node_t_ *next;
} queue_node_t;

typedef struct {

	queue_node_t *head;
	queue_node_t *tail;

	pthread_spinlock_t h_lock;
	pthread_spinlock_t t_lock;
} queue_t;

queue_t *
queue_new();

void
queue_push(queue_t *q, void *data);

void *
queue_pop(queue_t *q);

#endif /* QUEUE_LOCKS_H */
