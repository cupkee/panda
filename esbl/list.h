/*
 *
 */


#ifndef __ESBL_LIST_INC__
#define __ESBL_LIST_INC__

#include "config.h"

intptr_t list_create(int n, intptr_t *datas);
int list_destroy(intptr_t list);
int list_length(intptr_t list);

int list_set(intptr_t list, int index, intptr_t data);
int list_get(intptr_t list, int index, intptr_t *data);

int list_insert(intptr_t list, int start, int n, intptr_t *datas);

int list_push(intptr_t list, intptr_t data);
int list_pop(intptr_t list, intptr_t *data);
int list_lpush(intptr_t list, intptr_t data);
int list_lpop(intptr_t list, intptr_t *data);

intptr_t list_cut(intptr_t list, int start, int end);
intptr_t list_slice(intptr_t list, int start, int end);
intptr_t list_splice(intptr_t list, int start, int end, int n, intptr_t *datas);
intptr_t list_concat(intptr_t a, intptr_t b);

int list_foreach(intptr_t list, void (*fn)(intptr_t data, void *udata), void *udata);

/* not implemented */
int list_index_of(intptr_t list, intptr_t data);
int list_index_of2(intptr_t list, intptr_t data, int (*match)(intptr_t a, intptr_t b));

int list_sets(intptr_t list, int start, int n, intptr_t *data);
int list_gets(intptr_t list, int start, int n, intptr_t *data);

int list_remove(intptr_t list, intptr_t data);
int list_remove2(intptr_t list, intptr_t *data, int (*match)(intptr_t a, intptr_t b));

#endif /* __ESBL_LIST_INC__ */

