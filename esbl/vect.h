
/*
 *
 */


#ifndef __ESBL_VECT_INC__
#define __ESBL_VECT_INC__

#include "config.h"

intptr_t vect_create(uint32_t n, intptr_t *datas);
int vect_destroy(intptr_t v);
int vect_length(intptr_t v);
intptr_t *vect_ptr(intptr_t v);

int vect_insert(intptr_t v, int start, int n, intptr_t *datas);
int vect_set(intptr_t v, int i, intptr_t d);
int vect_get(intptr_t v, int i, intptr_t *d);

int vect_push(intptr_t v, intptr_t data);
int vect_pop(intptr_t v, intptr_t *data);
int vect_lpush(intptr_t v, intptr_t data);
int vect_lpop(intptr_t v, intptr_t *data);

intptr_t vect_cut(intptr_t v, int start, int end);
intptr_t vect_slice(intptr_t v, int start, int end);
intptr_t vect_splice(intptr_t v, int start, int end, int n, intptr_t *datas);
intptr_t vect_concat(intptr_t a, intptr_t b);

int vect_index_of(intptr_t v, int start, intptr_t data, int (*compare)(intptr_t, intptr_t));
int vect_foreach(intptr_t v, void (*fn)(intptr_t data, void *udata), void *udata);

// interface as heap
int vect_heapify(intptr_t vect, int (*compare)(intptr_t a, intptr_t b));
int vect_heapify_insert(intptr_t vect, intptr_t data, int (*compare)(intptr_t , intptr_t));
int vect_heapify_extract(intptr_t vect, intptr_t *data, int (*compare)(intptr_t , intptr_t));

// sort
int vect_sort(intptr_t vect, int (*compare)(intptr_t, intptr_t));
int vect_reverse(intptr_t vect);

void vect_dump(intptr_t v, const char *s);

/* not implemented */
int vect_sets(intptr_t v, int start, int n, intptr_t *d);
int vect_gets(intptr_t v, int start, int n, intptr_t *d);

int vect_remove(intptr_t v, intptr_t *data, int (*compare)(intptr_t, intptr_t));

// interface as set
intptr_t vect_unique(intptr_t v, int (*compare)(intptr_t a, intptr_t b));
int vect_unique_push(intptr_t v, intptr_t data, int (*compare)(intptr_t, intptr_t));

intptr_t vect_union(intptr_t v1, intptr_t v2, int (*compare)(intptr_t, intptr_t));
intptr_t vect_intersection(intptr_t v1, intptr_t v2, int (*compare)(intptr_t, intptr_t));
intptr_t vect_difference(intptr_t v1, intptr_t v2, int (*compare)(intptr_t, intptr_t));

int vect_is_subset(intptr_t v1, intptr_t v2, int (*compare)(intptr_t, intptr_t));

static inline int vect_is_equal(intptr_t v1, intptr_t v2, int (*compare)(intptr_t, intptr_t)) {
    return vect_length(v1) == vect_length(v2) &&
           vect_is_subset(v1, v2, compare);
}

static inline int vect_is_member(intptr_t vect, intptr_t d, int (*compare)(intptr_t, intptr_t)) {
    return vect_index_of(vect, 0, d, compare) >= 0;
}

#endif /* __ESBL_VECT_INC__ */

