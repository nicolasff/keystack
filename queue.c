#include <queue.h>

#include <stdlib.h>

queue_t *
queue_new() {

	queue_t *q = calloc(1, sizeof(queue_t));
	queue_node_t *n = calloc(1, sizeof(queue_node_t));

	pthread_spin_init(&q->h_lock, PTHREAD_PROCESS_SHARED);
	pthread_spin_init(&q->t_lock, PTHREAD_PROCESS_SHARED);

	q->head = q->tail = n;

	return q;
}


void
queue_push(queue_t *q, void *data) {

	queue_node_t *n = calloc(1, sizeof(queue_node_t));
	n->data = data;
	n->next = NULL;

	pthread_spin_lock(&q->t_lock);
	q->tail->next = n;
	q->tail = n;
	pthread_spin_unlock(&q->t_lock);
}

void *
queue_pop(queue_t *q) {

	queue_node_t *n;
	void *ret;

	pthread_spin_lock(&q->h_lock);
	n = q->head;
	if(NULL == n->next) {
		pthread_spin_unlock(&q->h_lock);
		return NULL;
	}
	ret = n->next->data;
	q->head = n->next;
	pthread_spin_unlock(&q->h_lock);
	free(n);

	return ret;
}
