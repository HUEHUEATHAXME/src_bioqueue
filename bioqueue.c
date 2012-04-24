#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "priqueue.h"

#define DEBUG_BIOADD	0

#define MAXNUM_QUEUE	32

typedef struct node node_t;
struct node {
    void 	*key;
    void 	*value;
    node_t 	*left;
    node_t 	*right;
};

struct priqueue {
    cmpfunc_t	cmpfunc;
    int		nqueue;
    int		size;
    node_t	*queue[MAXNUM_QUEUE];
    node_t 	*tmpq[MAXNUM_QUEUE];  
};


// Validate that root is larger than left child
static void _validate(priqueue_t *pq, node_t *node)
{
    if (node == NULL)
	return;
  
    if (node->left != NULL) {
	if (pq->cmpfunc(node->key, node->left->key) < 0) {
	    printf("INVALID!\n");
	    while (1);
	}
    }

    _validate(pq, node->left);
    _validate(pq, node->right);
}


// Create new node
static node_t *node_new(void *key, void *value)
{
    node_t *node;
    
    node = (node_t*)calloc(1, sizeof(node_t));
    if (node == NULL)
      goto error;
    node->key = key;
    node->value = value;
    return node;
error:
    return NULL;
}


// Free all nodes in heap
static node_t *_nodefree(node_t *root)
{
    if (root->left != NULL)
      root->left = _nodefree(root->left);
    
    if (root->right != NULL)
      root->right = _nodefree(root->right);
    
    free(root);
    return NULL;
}


// Merge two equal sized heaps
static node_t *merge(priqueue_t *pq, node_t *a, node_t *b)
{
    if (pq->cmpfunc(a->key, b->key) >= 0) {
	// a becomes root
	b->right = a->left;
	a->left = b;
	a->right = NULL;
	return a;
    } else {
	// b becomes root
	a->right = b->left;
	b->left = a;
	b->right = NULL;
	return b;
    }
}


// Add queues
static int qadd(priqueue_t *pq, node_t **qb)
{
    int 	i, v;
    node_t 	*a, *b, *c;
  
    
    /*  a   b   c
     *  0 + 0 + 0 = 0		(0)
     *  0 + 0 + 1 = 1		(1) 
     *  0 + 1 + 0 = 1		(2)
     *  0 + 1 + 1 = 0 + c	(3)
     *  1 + 0 + 0 = 1		(4)
     *  1 + 0 + 1 = 0 + c	(5)
     *  1 + 1 + 0 = 0 + c	(6)
     *  1 + 1 + 1 = 1 + c	(7)
     * */
    c = NULL;
    for (i = 0; i < pq->nqueue; i++) {
	v = 0;
	a = pq->queue[i];
	b = qb[i];
	if (c != NULL)
	  v |= 1 << 0;
	if (b != NULL)
	  v |= 1 << 1;
	if (a != NULL)
	  v |= 1 << 2;
	
	switch (v) {
	  case 0:
	    // Nothing to do
	    break;
	  case 1:
	    pq->queue[i] = c;
	    c = NULL;
	    break;
	  case 2:
	    pq->queue[i] = b;
	    break;
	  case 3:
	    c = merge(pq, b, c);
	    break;
	  case 4:
	    // Keep existing queue
	    break;
	  case 5:
	    pq->queue[i] = NULL;
	    c = merge(pq, a, c);
	    break;
	  case 6:
	    pq->queue[i] = NULL;
	    c = merge(pq, a, b);
	    break;
	  case 7:
	    pq->queue[i] = c;
	    c = merge(pq, a, b);
	    break;
	}
	qb[i] = NULL;
	
	if (DEBUG_BIOADD)
	  _validate(pq, pq->queue[i]);
    }

    if (c != NULL) {
	// Overflow
	return -1;
    } else {
	return 0;
    }
}



priqueue_t *priqueue_new(cmpfunc_t cmpfunc)
{
    priqueue_t *pq;
    
    pq = (priqueue_t*)calloc(1, sizeof(priqueue_t));
    if (pq == NULL)
      goto error;
    pq->cmpfunc = cmpfunc;
    pq->nqueue = MAXNUM_QUEUE;
    return pq;
error:
  return NULL;
}


void priqueue_destroy(priqueue_t *pq)
{
    int i;

    // Free heaps
    for (i = 0; i < pq->nqueue; i++) {
	if (pq->queue[i] != NULL) {
	    _nodefree(pq->queue[i]);
	}
    }
    
    // Free pq
    free(pq);
}


int priqueue_insert(priqueue_t *pq, void *key, void *value)
{
    int 	retval;
  
    // Create queue with 1-heap
    pq->tmpq[0] = node_new(key, value);
    if (pq->tmpq[0] == NULL)
      goto error;
    
    // Add to queue
    retval = qadd(pq, pq->tmpq);
    if (retval != 0)
      goto error;
    
    // Update queue size
    pq->size++;
    
    return 0;
error:
    return -1;
}


void *priqueue_delmax(priqueue_t *pq)
{
    int 	i, maxidx;
    node_t	*node;
    void	*value;
    
    // Find largest key
    maxidx = -1;
    for (i = 0; i < pq->nqueue; i++) {
	if (pq->queue[i] != NULL) {
	    if (maxidx == -1) {
		maxidx = i;
	    } else {
		if (pq->cmpfunc(pq->queue[maxidx], pq->queue[i]) < 0)
		    maxidx = i;
	    }
	}
    }
    
    // Queue might be empty
    if (maxidx == -1)
	goto error;
    
    // Break heap with largest key up into a 2^(maxidx-1) , 2^(maxidx-2),... queue
    value = pq->queue[maxidx]->value;
    node = pq->queue[maxidx]->left;
    free(pq->queue[maxidx]);
    pq->queue[maxidx] = NULL;
    for (i = maxidx-1; node != NULL && i >= 0; i--) {
	pq->tmpq[i] = node;
	node = node->right;
	pq->tmpq[i]->right = NULL;
    }
    
    // Add to queue
    qadd(pq, pq->tmpq);
    
    // Update queue size
    pq->size--;
    
    return value;
error:
    return NULL;
}


priqueue_t *priqueue_join(priqueue_t *pqa, priqueue_t *pqb)
{
    // Add b to a
    qadd(pqa, pqb->queue);
    
    // Update queue size
    pqa->size += pqb->size;
    pqb->size = 0;
    
    // Destroy b
    priqueue_destroy(pqb);
    
    // Return a
    return pqa;
}

