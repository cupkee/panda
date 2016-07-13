/*
 *
 */

#ifndef __ARRAY_INC__
#define __ARRAY_INC__

#include <config.h>

typedef void * array_t;

array_t array_create(int size, int flags);
array_t array_create(int n, DATA_T *datas, int flags);
int     array_destroy(array_t a);

int array_isArray(array_t a);
int array_length(array_t a);

int array_get(array_t a, int idx, void **pdata);
int array_set(array_t a, int idx, void *data);
int array_buf(array_t a, void **pbuf);

int array_push(array_t a, void *data);
int array_pop (array_t a, void **pdata);
int array_unshift(array_t a, void *data);
int array_shift  (array_t a, void **pdata);
int array_sort(array_t a, int (*cmp)(void *, void *));
int array_reverse(array_t a);
int array_splice(array_t a, int start, int count, int n, void * datas);

int array_indexOf(array_t a, void *data);
int array_lastIndexOf(array_t a, void *data);

int array_every(array_t a, int (*fn)(void *));
int array_some (array_t a, int (*fn)(void *));
int array_foreach(array_t a, int (*fn)(void *));
int array_foreach2(array_t a, int (*fn)(void *, void *), void *udata);

array_t array_concat(array_t a, int n, void *datas);
array_t array_slice(array_t a, int start, int end);


#endif /* __ARRAY_INC__ */

