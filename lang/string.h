
#ifndef __LANG_STRING_INC__
#define __LANG_STRING_INC__

#include "config.h"

#include "val.h"
#include "env.h"


#define MAGIC_STRING    (MAGIC_BASE + 3)

int string_compare(val_t *a, val_t *b);

void string_add(env_t *env, val_t *a, val_t *b, val_t *res);
void string_at(env_t *env, val_t *a, val_t *b, val_t *res);
val_t string_length(env_t *env, int ac, val_t *av);
val_t string_index_of(env_t *env, int ac, val_t *av);

static inline int string_len(val_t *s) {
    if (val_is_inline_string(s)) {
        return 1;
    } else
    if (val_is_static_string(s)) {
        return strlen((void*)val_2_intptr(s));
    } else
    if (val_is_owned_string(s)) {
        uint8_t *p = (uint8_t *)val_2_intptr(s);
        return p[1] * 256 + p[2];
    } else {
        return -1;
    }
}

static inline int string_mem_space(intptr_t s) {
    uint8_t *p = (uint8_t *)s;

    return SIZE_ALIGN(p[1] * 256 + p[2] + 4);
}

static inline intptr_t string_mem_ptr(intptr_t s) {
    return s + 3;
}

#endif /* __LANG_STRING_INC__ */

