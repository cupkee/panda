/*
MIT License

Copyright (c) 2016 Lixing Ding <ding.lixing@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef __LANG_STRING_INC__
#define __LANG_STRING_INC__

#include "config.h"

#include "val.h"
#include "env.h"


#define MAGIC_STRING    (MAGIC_BASE + 3)

static inline int string_inline_len(val_t *s) {
    (void)s;
    return 1;
}

static inline int string_static_len(val_t *s) {
    return strlen((void*)val_2_intptr(s));
}

static inline int string_owned_len(val_t *s) {
    uint8_t *p = (uint8_t *)val_2_intptr(s);
    return p[1] * 256 + p[2];
}

static inline int string_len(val_t *s) {
    if (val_is_inline_string(s)) {
        return 1;
    } else
    if (val_is_foreign_string(s)) {
        return strlen((void*)val_2_intptr(s));
    } else
    if (val_is_heap_string(s)) {
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

int string_compare(val_t *a, val_t *b);

void string_add(env_t *env, val_t *a, val_t *b, val_t *res);
void string_at(env_t *env, val_t *a, val_t *b, val_t *res);
void string_elem_get(val_t *self, int i, val_t *elem);
val_t string_length(env_t *env, int ac, val_t *av);
val_t string_index_of(env_t *env, int ac, val_t *av);

#endif /* __LANG_STRING_INC__ */

