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
    uint8_t reserved[2];
    uint16_t prop_size;
    uint16_t prop_num;
    struct object_t   *proto;
    intptr_t *keys;
    val_t    *vals;
} object_t;

int objects_env_init(env_t *env);

intptr_t object_create(env_t *env, int n, val_t *av);
void   object_prop_val(env_t *env, val_t *self, val_t *key, val_t *prop);
val_t *object_prop_ref(env_t *env, val_t *self, val_t *key);

static inline int object_mem_space(object_t *o) {
    return SIZE_ALIGN(sizeof(object_t) + (sizeof(intptr_t) + sizeof(val_t)) * o->prop_size);
};

#endif /* __LANG_OBJECT_INC__ */

