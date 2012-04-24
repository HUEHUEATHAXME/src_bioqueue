#ifndef PRIQUEUE_H
#define PRIQUEUE_H

#include "common.h"

typedef struct priqueue priqueue_t;
struct priqueue;


// Interface
priqueue_t *priqueue_new(cmpfunc_t cmpfunc);
void priqueue_destroy(priqueue_t *priqueue);
int priqueue_insert(priqueue_t *priqueue, void *key, void *value);
void *priqueue_delmax(priqueue_t *priqueue);
priqueue_t *priqueue_join(priqueue_t *priqueue_a, priqueue_t *priqueue_b);


#endif
