#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

int hash(char data[DATA_SIZE]) {
	int hash = 0;
	for (int i = 0; i < DATA_SIZE; i++) {
		hash += data[i];
	}
	return hash;
}

///
/// Queue
///

static queue_t queue_get_last_node(queue_t q) {
	if (!q) {
		return NULL;
	} else if (!q->next) {
		return q;
	}
	return queue_get_last_node(q->next);
}

bool queue_is_empty(queue_t q) {
	return !q;
}

queue_t queue_enqueue(queue_t q, void *data) {
	queue_t new_node = calloc(1, sizeof(*new_node));
	new_node->next = NULL;
	new_node->data = calloc(1, sizeof(data));
	memcpy(new_node->data, data, sizeof(data));

	queue_t last_node = queue_get_last_node(q);
	if (last_node && q) {
		last_node->next = new_node;
		return q;
	}

	// No nodes, return the newly created node
	return new_node;
}

queue_t queue_dequeue(queue_t q, void **data) {
	if (queue_is_empty(q)) {
		return NULL;
	}

	queue_t next = q->next;
	*data = q->data;

	free(q);
	return next;
}
