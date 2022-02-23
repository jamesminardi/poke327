#ifndef HEAP_H
#define HEAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct heap_node;
typedef struct heap_node heap_node_t;

typedef struct heap {
	heap_node_t *min;
	int size;
	//int degree; // # of children
	//int phi; // TODO: What is this
	//int (*compare)(int *key, int *with);
	//void (*datum_delete)(int *);
} heap_t;

/*
 * Sets heap size to 0 and null heap min
 */
void heap_init(heap_t *h);

/*
 * Deletes node and all child nodes?
 */
void heap_node_delete(heap_t *h, heap_node_t *hn);

/*
 * Frees entire heap and resets to a size 0 heap
 */
void heap_delete(heap_t *h);

heap_node_t *heap_insert(heap_t *h, int v);

/*
 * Returns key of the heap's min node
 * Error if called on empty heap
 */
int heap_peek(heap_t *h);

int heap_degree(int size);

int heap_remove_min(heap_t *h);

/*
 * Decrease key without replacing with user value
 * Return 1 for error
 */
int heap_decrease_key(heap_t *h, heap_node_t *n);

int key_compare(int key, int with);

#endif // HEAP_H