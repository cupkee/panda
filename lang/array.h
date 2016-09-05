

#ifndef __LANG_ARRAY_INC__
#define __LANG_ARRAY_INC__

#include "config.h"
#include "val.h"
#include "env.h"
#include "object.h"

#define MAGIC_ARRAY         (MAGIC_BASE + 11)

typedef struct array_t {
    uint8_t magic;
    uint8_t age;
    uint8_t elem_size;
    uint8_t elem_num;
    val_t *elems;
} array_t;

intptr_t array_create(env_t *env, int ac, val_t *av);

void array_elem_get(env_t *env, val_t *a, val_t *i, val_t *elem);
void array_elem_set(env_t *env, val_t *a, val_t *i, val_t *elem);


#endif /* __LANG_ARRAY_INC__ */

