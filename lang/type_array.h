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


#ifndef __LANG_ARRAY_INC__
#define __LANG_ARRAY_INC__

#include "def.h"
#include "val.h"
#include "env.h"

#define MAGIC_ARRAY         (MAGIC_BASE + 11)

typedef struct array_t {
    uint8_t magic;
    uint8_t age;
    uint16_t elem_size;
    uint16_t elem_bgn;
    uint16_t elem_end;
    val_t *elems;
} array_t;

static inline array_t *array_entry(val_t *v) {
    return val_is_array(v) ? (array_t *)val_2_intptr(v) : NULL;
}

static inline int array_mem_space(array_t *a) {
    return SIZE_ALIGN(sizeof(array_t) + sizeof(val_t) * a->elem_size);
}

static inline val_t *array_values(array_t *a) {
    return a->elems + a->elem_bgn;
}

static inline int array_length(array_t *a) {
    return a->elem_end - a->elem_bgn;
}

static inline val_t *array_get(array_t *a, int i)
{
    return (a->elem_bgn + i < a->elem_end) ? (a->elems + i) : NULL;
}


array_t *_array_create(env_t *env, int len);
intptr_t array_create(env_t *env, int ac, val_t *av);

void array_elem_val(val_t *self, int i, val_t *elem);
val_t *array_elem_ref(val_t *self, int i);

extern const val_metadata_t metadata_array;
void array_proto_init(env_t *env);

/* new api
void *array_create(env_t *env, int size, val_t *elems);

int array_length(void *entry);

int array_push(void *entry, env_t *env, val_t *elem);
int array_unshift(void *entry, env_t *env, val_t *elem);

val_t *array_pop(void *entry);
val_t *array_shift(void *entry);
*/

#endif /* __LANG_ARRAY_INC__ */

