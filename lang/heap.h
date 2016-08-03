


#ifndef __LANG_HEAP_INC__
#define __LANG_HEAP_INC__

#include "config.h"

typedef struct heap_t {
    unsigned int size;
    unsigned int free;
    void *base;
} heap_t;

void heap_init(heap_t *heap, void *base, int size);
void heap_free(heap_t *heap);

void *heap_alloc(heap_t *heap, int size);
int heap_is_owner(heap_t *heap, void *p);


#endif /* __LANG_HEAP_INC__ */

