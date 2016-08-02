
#ifndef __LANG_STRING_INC__
#define __LANG_STRING_INC__

#include "config.h"

#include "val.h"
#include "env.h"

int string_compare(val_t *a, val_t *b);

val_t string_add(env_t *env, val_t *a, val_t *b);
val_t string_mul(env_t *env, val_t *s, val_t *n);
val_t string_length(env_t *env, int ac, val_t *av);
val_t string_index_of(env_t *env, int ac, val_t *av);


#endif /* __LANG_STRING_INC__ */

