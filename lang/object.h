


#ifndef __LANG_OBJECT_INC__
#define __LANG_OBJECT_INC__

#include "config.h"

#include "val.h"
#include "env.h"
#include "err.h"

extern val_t *object_string_ptr;

int objects_env_init(env_t *env);

int object_prop_get(env_t *env, val_t *obj, val_t *key, val_t *prop);
int object_elem_get(env_t *env, val_t *obj, val_t *key, val_t *prop);

#endif /* __LANG_OBJECT_INC__ */

