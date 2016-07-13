#ifndef __ESBL_HTBL_INC__
#define __ESBL_HTBL_INC__

#include "config.h"

intptr_t htbl_create(uint32_t (*hash)(intptr_t data),
                     int (*compare)(intptr_t d1, intptr_t d2));
int htbl_destroy(intptr_t htbl);

int htbl_insert(intptr_t htbl, intptr_t data);
int htbl_remove(intptr_t htbl, intptr_t data);
int htbl_lookup(intptr_t htbl, intptr_t data);
int htbl_length(intptr_t htbl);

#endif /* __ESBL_HTBL_INC__ */

