

#ifndef __LANG_OBJECT_INC__
#define __LANG_OBJECT_INC__

#include "config.h"

#include "val.h"
#include "env.h"
#include "err.h"

#define MAGIC_OBJECT (MAGIC_BASE + 7)
#define MAGIC_OBJECT_STATIC (MAGIC_BASE + 9)

typedef struct object_t {
    uint8_t magic;
    uint8_t age;
    uint8_t prop_size;
    uint8_t prop_num;
    struct object_t   *proto;
    intptr_t *keys;
    val_t    *vals;
} object_t;

extern val_t *object_string_ptr;

int objects_env_init(env_t *env);

void object_prop_get(env_t *env, val_t *obj, val_t *key, val_t *prop);
void object_elem_get(env_t *env, val_t *obj, val_t *key, val_t *prop);
void object_prop_set(env_t *env, val_t *obj, val_t *key, val_t *prop);
void object_elem_set(env_t *env, val_t *obj, val_t *key, val_t *prop);

static inline int object_mem_space(object_t *o) {
    return sizeof(object_t) + sizeof(intptr_t) * o->prop_size + sizeof(val_t) * o->prop_size;
};

intptr_t object_create(env_t *env, int n, val_t *av);

#endif /* __LANG_OBJECT_INC__ */

