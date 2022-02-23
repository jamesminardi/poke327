#include "heap.h"

struct heap_node {
	heap_node_t *next;
	heap_node_t *prev;
	heap_node_t *parent;
	heap_node_t *child;
	int key;
	int degree; // Number of children
	int mark; // For cut & decrease key (visited)
};

static void heap_insert_node_in_list(heap_node_t *n, heap_node_t *l) {
	n->next = l;
	n->prev = l->prev;
	n->prev->next = n;
	l->prev = n;
}

void heap_init(heap_t *h) {
	h->size = 0;
	h->min = NULL;
	//h->phi = 0;
}
void heap_node_delete(heap_t *h, heap_node_t *hn) {
	heap_node_t *next;
	// Recursively call heap_node_delete on every child
	// After going all the way down, call heap_node_delete on its siblings
	hn->prev->next = NULL;
	while (hn) {
		if (hn->child) {
			heap_node_delete(h, hn->child);
		}
		next = hn->next;
		free(hn);
		hn = next;
	}
}
void heap_delete(heap_t *h) {
	if (h->min) {
		heap_node_delete(h, h->min);
	}
	h->min = NULL;
	h->size = 0;
}
heap_node_t *heap_insert(heap_t *h, int v) {
	heap_node_t *n;
	//assert((n = calloc(1, sizeof (*n))));
	//n = malloc(sizeof (*n));
	// Allocate memory for new node and initialize all data to 0
	n = calloc(1, sizeof (*n));
	n->key = v;

	// If a min exists, insert new node to left of min.
	if (h->min) {
		// Insert heap node in list
		heap_insert_node_in_list(n, h->min);
	} else {
		n->next = n->prev = n; // TODO: Wtf is this
	}

	// Assign heap min to new node if it is smaller
	if (!h->min || (key_compare(v, h->min->key) < 0)) {
		h->min = n;
	}
	h->size++;
	return n;
}

int heap_peek(heap_t *h) {
	assert(h->min && "Peak on empty heap");
	return h->min->key;
}

int heap_degree(int size) {
	int count = 0;
	while (size > 0) {
		size = size/2;
		count++;
	}
	return count;
}

static void heap_link(heap_node_t *node, heap_node_t *root){
	if (root->child) {
		// insert heap node in list
		heap_insert_node_in_list(node, root->child);
	} else {
		root->child = node;
		node->next = node->prev = node;
	}
	node->parent = root;
	root->degree++;
	//node->mark = 0;
}

/*
 * Consolidate heap so that no root tree has the same degree as another
 */
static void heap_consolidate(heap_t *h) {
	int degree, i;
	degree = heap_degree(h->size);
	heap_node_t *a[degree], *current_node, *other_node, *next_node;
	memset(a, 0, sizeof(a));

	h->min->prev->next = NULL; // Break circular link

	current_node = next_node = h->min;
	while (next_node) {
		next_node = next_node->next;

		// If the current node has the same degree as another root node, continue in while loop
		while (a[current_node->degree]) {
			other_node = a[current_node->degree];
			// Swap current_node and other_node if Curr_node is bigger than other_node
			if (key_compare(current_node->key, other_node->key) > 0) {
				heap_node_t *tmp = other_node;
				other_node = current_node;
				current_node = tmp;
			}
			a[current_node->degree] = NULL;
			heap_link(other_node, current_node);
		}
		// Set current node to its degree in a[]
		a[current_node->degree] = current_node;

		current_node = next_node;
	}

	h->min = NULL;
	for (i = 0; i < degree; i++) {
		if (a[i]) {
			if (h->min) {
				// Inert heap node in list
				heap_insert_node_in_list(a[i], h->min);
				if (key_compare(a[i]->key, h->min->key) < 0) {
					h->min = a[i];
				}
			} else {
				h->min = a[i];
				a[i]->next = a[i]->prev = a[i];
			}
		}
	}

}

int heap_remove_min(heap_t *h){
	int value;
	heap_node_t *node;

	assert(h->min && "Remove min on empty heap");
	value = h->min->key;
	if (h->size == 1) { // If heap size is 1, free and set min = null
		free(h->min);
		h->min = NULL;
	} else { // If heap size > 1,
		node = h->min->child;
		if (node) { // Iterate through children of min and set parent to null
			while (node->parent) {
				node->parent = NULL;
				node = node->next;
			}
		}
		// Splice heap node lists
		if (h->min && h->min->child) {
			h->min->next->prev = h->min->child->prev;
			h->min->child->prev->next = h->min->next;
			h->min->next = h->min->child;
			h->min->child->prev = h->min->next;
		}
		node = h->min;
		// Remove heap node from list
		node->next->prev = node->prev;
		node->prev->next = node->next;

		h->min = node->next;
		free(node);
		heap_consolidate(h);
	}
	h->size--;
	return value;
}

static void heap_cut(heap_t *h, heap_node_t *n, heap_node_t *p) {
	// If p->degree == 1, set child to null?
	if (!--p->degree) {
		p->child = NULL;
	}
	if (p->child == n) {
		p->child = p->child->next;
	}
	// Remove heap node from list
	n->next->prev = n->prev;
	n->prev->next = n->next;
	n->parent = NULL;

	n->mark = 0;
	// Insert heap node in list
	heap_insert_node_in_list(n, h->min);
}

static void heap_cascading_cut(heap_t *h, heap_node_t *n) {
	heap_node_t *p;
	if ((p = n->parent)) {
		if (!n->mark) {
			n->mark = 1;
		} else {
			heap_cut(h, n, p);
			heap_cascading_cut(h, n);
		}
	}
}

int heap_decrease_key(heap_t *h, heap_node_t *n) {

	heap_node_t *p;

	p = n->parent;

	if (p && (key_compare(n->key, p->key) < 0)) {
		heap_cut(h, n, p);
		heap_cascading_cut(h, p);
	}
	if (key_compare(n->key, h->min->key) < 0) {
		h->min = n;
	}
	return 0;
}


int key_compare(int key, int with)
{
	return key - with;
}