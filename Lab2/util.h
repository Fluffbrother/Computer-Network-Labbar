#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

#define FMT_CLEAR "\033[0m"
#define FMT_BOLD "\033[1m"
// debug logging only when running `make debug` :-)
#ifdef DEBUGGING
#define debug(...) printf(FMT_BOLD "[DEBUG]" FMT_CLEAR " " __VA_ARGS__)
#else
#define debug(...) // no-op
#endif

#define btos(value) value ? "True" : "False"

#define A 0
#define B 1
#define DATA_SIZE 20
#define NACK 0
#define ACK 1

/** Simple hashing function that just sums all ascii characters in the string */
int hash(char data[DATA_SIZE]);

///
/// Queue
///

typedef struct queue {
	struct queue *next;
	void *data;
} *queue_t;

bool queue_is_empty(queue_t q);
/** Append to the end of the queue. Caller is responsible for freeing data */
queue_t queue_enqueue(queue_t q, void *data);
/** Remove from the start and returns the pointer to the new starting node */
queue_t queue_dequeue(queue_t q, void **data);

#endif // UTIL_H
