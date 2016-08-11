


#ifndef __LANG_HEAP_INC__
#define __LANG_HEAP_INC__

#include "config.h"

typedef struct heap_t {
    unsigned int size;
    unsigned int free;
    void *base;
} heap_t;

void heap_init(heap_t *heap, void *base, int size);
void heap_clean(heap_t *heap);

void *heap_alloc(heap_t *heap, int size);
int heap_is_owned(heap_t *heap, void *p);

static inline void heap_reset(heap_t *heap) {
    heap->free = 0;
}

static inline void heap_copy(heap_t *dst, heap_t *src) {
    dst->base = src->base;
    dst->size = src->size;
    dst->free = src->free;
}

static inline int heap_free_size(heap_t *heap) {
    return heap->size - heap->free;
}

static inline void *heap_free_addr(heap_t *heap) {
    return heap->base + heap->free;
}


#endif /* __LANG_HEAP_INC__ */

