

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
    uint16_t elem_size;
    uint16_t elem_bgn;
    uint16_t elem_num;
    val_t *elems;
} array_t;

static inline int array_mem_space(array_t *a) {
    return SIZE_ALIGN(sizeof(array_t) + sizeof(val_t) * a->elem_size);
}
intptr_t array_create(env_t *env, int ac, val_t *av);

void array_elem_get(env_t *env, val_t *a, val_t *i, val_t *elem);
void array_elem_set(env_t *env, val_t *a, val_t *i, val_t *elem);

val_t array_length(env_t *env, int ac, val_t *av);
val_t array_push(env_t *env, int ac, val_t *av);
val_t array_pop(env_t *env, int ac, val_t *av);



#endif /* __LANG_ARRAY_INC__ */

