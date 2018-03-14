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


#ifndef __LANG_OBJECT_INC__
#define __LANG_OBJECT_INC__

#include "def.h"

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

typedef struct object_prop_t {
    intptr_t symbal;
    val_t (*getter) (env_t *env, void *entry);
    void  (*setter) (env_t *env, void *entry, val_t *data);
} object_prop_t;

typedef struct object_iter_t {
    object_t *obj;
    int       cur;
} object_iter_t;

int objects_env_init(env_t *env);

intptr_t object_create(env_t *env, int n, val_t *av);
void   object_prop_val(env_t *env, val_t *self, val_t *key, val_t *prop);
val_t *object_prop_ref(env_t *env, val_t *self, val_t *key);

static inline int object_mem_space(object_t *o) {
    return SIZE_ALIGN(sizeof(object_t) + (sizeof(intptr_t) + sizeof(val_t)) * o->prop_size);
};

static inline void _object_iter_init(object_iter_t *it, object_t *obj) {
    it->obj = obj;
    it->cur = 0;
};

static inline int object_iter_init(object_iter_t *it, val_t *obj)
{
    if (val_is_object(obj)) {
        _object_iter_init(it, (object_t *)val_2_intptr(obj));
        return 0;
    }
    return -1;
}

int object_iter_next(object_iter_t *it, const char **k, val_t **v);

void object_proto_init(env_t *env);
extern const val_metadata_t metadata_object;

#endif /* __LANG_OBJECT_INC__ */

