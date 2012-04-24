#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "time.h"
#include "priqueue.h"

typedef struct data data_t;
struct data {
    int key;
};

data_t *data;


void fatal_error(char *msg, ...)
{
    va_list ap;

    va_start(ap, msg);
    vprintf(msg, ap);
    
    exit(1);
}

static int cmpfunc(int *a, int *b)
{
    return *a - *b;
}

int main(int argc, char **argv)
{
    int 		i, maxnum;
    unsigned long long 	t1, t2;
    priqueue_t		*pq, *pqb;
    data_t		*d;

    if (argc < 2)
        fatal_error("Usage: %s <num elements to insert>\n", argv[0]);

    maxnum = atol(argv[1]);
    if (maxnum <= 0)
        fatal_error("Usage: %s <num elements to insert> <output file>\n", argv[0]);

    // Allocate data array
    data = (data_t*) malloc(sizeof(data_t)*maxnum);
    if (data == NULL)
        fatal_error("Unable to allocate memory\n");
    
    // Generate keys
    for (i = 0; i < maxnum; i++)
	data[i].key = i;
    
    // Create new queue
    pq = priqueue_new((cmpfunc_t)cmpfunc);
    if (pq == NULL)
	fatal_error("Unable to create new priqueue\n");
    
    // Test insert
    t1 = gettime();
    for (i = 0; i < maxnum; i++) {
	priqueue_insert(pq, &data[i].key, &data[i]);
    }
    t2 = gettime();
    printf("%d microseconds to insert %d elements\n", (int)(t2-t1), maxnum);

    // Test delmax
    t1 = gettime();
    for (i = 0; i < maxnum; i++) {
	d = priqueue_delmax(pq);
    }
    t2 = gettime();
    printf("%d microseconds to remove %d elements\n", (int)(t2-t1), maxnum);

    
    // Test join
    pqb = priqueue_new((cmpfunc_t)cmpfunc);
    if (pqb == NULL)
	fatal_error("Unable to create new priqueue\n");
    for (i = 0; i < maxnum/2; i++)
	priqueue_insert(pq, &data[i].key, &data[i]);
    for (i = maxnum/2; i < maxnum; i++)
	priqueue_insert(pqb, &data[i].key, &data[i]);
    
    t1 = gettime();
    pq = priqueue_join(pq, pqb);
    t2 = gettime();
    printf("%d microseconds to join two queues with %d elements\n", (int)(t2-t1), maxnum);
    
    
    priqueue_destroy(pq);
    printf("Priority queue destroyed\n");
    
    return 0;
}
