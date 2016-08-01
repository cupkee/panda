
#ifndef __LANG_STRING_INC__
#define __LANG_STRING_INC__

#include "config.h"

#include "val.h"
#include "env.h"

int string_compare(val_t *a, val_t *b);

val_t string_concat(env_t *env, val_t *a, val_t *b);

#endif /* __LANG_STRING_INC__ */

