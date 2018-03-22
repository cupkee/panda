/* GPLv2 License
 *
 * Copyright (C) 2016-2018 Lixing Ding <ding.lixing@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 **/



#ifndef __LANG_HEAP_INC__
#define __LANG_HEAP_INC__

#include "def.h"

typedef struct heap_t {
    int size;
    int free;
    void *base;
} heap_t;

void heap_init(heap_t *heap, void *base, int size);
void heap_clean(heap_t *heap);

void *heap_alloc(heap_t *heap, int size);

static inline
int heap_is_owned(heap_t *heap, void *p) {
    int dis = p - heap->base;
    return dis >= 0 && dis < heap->size;
}

static inline
void heap_reset(heap_t *heap) {
    heap->free = 0;
}

static inline
void heap_copy(heap_t *dst, heap_t *src) {
    dst->base = src->base;
    dst->size = src->size;
    dst->free = src->free;
}

static inline
int heap_free_size(heap_t *heap) {
    return heap->size - heap->free;
}

static inline
void *heap_free_addr(heap_t *heap) {
    return heap->base + heap->free;
}


#endif /* __LANG_HEAP_INC__ */

