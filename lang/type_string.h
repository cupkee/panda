/* GPLv2 License
 *
 * Copyright (C) 2016-2018 Lixing Ding <ding.lixing@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 **/

#ifndef __LANG_STRING_INC__
#define __LANG_STRING_INC__

#include "def.h"

#include "val.h"
#include "env.h"


#define MAGIC_STRING    (MAGIC_BASE + 3)

typedef struct string_t {
    uint8_t magic;
    uint8_t age;

    uint16_t size;
    char    str[0];
} string_t;

static inline int string_mem_space(intptr_t p) {
    string_t *s = (string_t *) p;

    return SIZE_ALIGN(sizeof(string_t) + s->size);
}

static inline intptr_t string_mem_ptr(intptr_t s) {
    return s + sizeof(string_t);
}

static inline int string_len(val_t *v) {
    if (val_is_inline_string(v)) {
        return 1;
    } else
    if (val_is_foreign_string(v)) {
        return strlen((void*)val_2_intptr(v));
    } else
    if (val_is_heap_string(v)) {
        string_t *s = (string_t *) val_2_intptr(v);
        return strlen(s->str);
    } else {
        return -1;
    }
}

val_t string_create_heap_val(env_t *env, const char *data);

int string_compare(val_t *a, val_t *b);

void string_add(env_t *env, val_t *a, val_t *b, val_t *res);
void string_at(env_t *env, val_t *a, val_t *b, val_t *res);
void string_elem_get(val_t *self, int i, val_t *elem);

void string_proto_init(env_t *env);
extern const val_metadata_t metadata_str_inline;
extern const val_metadata_t metadata_str_heap;
extern const val_metadata_t metadata_str_foreign;

#endif /* __LANG_STRING_INC__ */

