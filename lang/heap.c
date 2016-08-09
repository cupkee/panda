#include "heap.h"

void heap_init(heap_t *heap, void *base, int size)
{
    if (heap && base && size) {
        heap->size = size;
        heap->free = 0;
        heap->base = base;
        //memset(base, 0, size);
    }
}

void heap_clean(heap_t *heap)
{
    if (heap) {
        heap->free = 0;
        memset(heap->base, 0, heap->size);
    }
}

void *heap_alloc(heap_t *heap, int size) {
    if (heap) {
        int free;

        size = SIZE_ALIGN(size);
        free = heap->free + size;
        //printf("Alloc %d, size: %u, free: %u\n", size, heap->size, heap->free);
        if (free < heap->size) {
            void *p = heap->base + heap->free;
            heap->free = free;
            return p;
        }
    }
    return NULL;
}

int heap_is_owner(heap_t *heap, void *p)
{
    return (p - heap->base) < heap->size;
}
