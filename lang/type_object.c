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

#include "interp.h"
#include "type_number.h"
#include "type_string.h"
#include "type_array.h"
#include "type_object.h"

static inline object_t *object_entry(val_t *v) {
    return val_is_object(v) ? (object_t *)val_2_intptr(v) : NULL;
}

static val_t *object_add_prop(env_t *env, object_t *obj, intptr_t symbal) {
    val_t *vals;
    intptr_t *keys;

    if (obj->prop_size <= obj->prop_num) {
        int size;

        if (obj->prop_size >= UINT16_MAX) {
            env_set_error(env, ERR_ResourceOutLimit);
            return NULL;
        }
        size = obj->prop_size * 2;
        size = size < UINT16_MAX ? size : UINT16_MAX;

        keys = (intptr_t*) env_heap_alloc(env, sizeof(intptr_t) * size + sizeof(val_t) * size);
        if (!keys) {
            env_set_error(env, ERR_NotEnoughMemory);
            return NULL;
        }
        vals = (val_t *)(keys + size);

        memcpy(keys, obj->keys, sizeof(intptr_t) * obj->prop_num);
        memcpy(vals, obj->vals, sizeof(val_t) * obj->prop_num);
        obj->keys = keys;
        obj->vals = vals;
    } else {
        vals = obj->vals;
        keys = obj->keys;
    }

    keys[obj->prop_num] = symbal;
    return vals + obj->prop_num++;
}

static val_t *object_find_prop_owned(object_t *obj, intptr_t symbal) {
    int i;

    for (i = 0; i < obj->prop_num; i++) {
        if (obj->keys[i] == symbal) {
            return obj->vals + i;
        }
    }
    return NULL;
}

static val_t native_object_to_string(env_t *env, int ac, val_t *obj)
{
    (void) env;
    (void) ac;
    (void) obj;
    return val_mk_foreign_string((intptr_t)"Object");
}

static val_t native_object_foreach(env_t *env, int ac, val_t *av)
{
    if (ac > 1 && val_is_object(av) && val_is_function(av + 1)) {
        object_t *o = (object_t *)val_2_intptr(av);
        int i, max = o->prop_num;

        for (i = 0; i < max && !env->error; i++) {
            val_t key = val_mk_foreign_string(o->keys[i]);

            env_push_call_argument(env, &key);
            env_push_call_argument(env, o->vals + i);
            env_push_call_function(env, av + 1);

            interp_execute_call(env, 2);
        }
    }

    return val_mk_undefined();
}

intptr_t object_create(env_t *env, int n, val_t *av)
{
    object_t *obj;
    int size;

    if ((n & 1) || n > UINT16_MAX * 2) {
        return 0;
    }

    size = n / 2;
    size = size < DEF_PROP_SIZE ? DEF_PROP_SIZE : size;

    obj = (object_t *) env_heap_alloc(env, sizeof(object_t) + sizeof(intptr_t) * size + sizeof(val_t) * size);
    if (obj) {
        int i = 0, off = 0;

        obj->magic = MAGIC_OBJECT;
        obj->age = 0;
        obj->prop_size = size;
        obj->prop_num = n / 2;
        obj->keys = (intptr_t *)(obj + 1);
        obj->vals = (val_t *)(obj->keys + size);
        obj->proto = NULL;
        while (i < n) {
            val_t *k = av + i++;
            val_t *val = av + i++;
            const char *name = val_2_cstring(k);
            intptr_t key;

            if (!name) {
                return 0;
            }

            key = env_symbal_get(env, name);
            if (!key) {
                key = env_symbal_insert(env, name, !val_is_foreign_string(k));
            }

            if (!key) {
                return 0;
            }
            obj->keys[off] = key;
            obj->vals[off] = *val;

            off++;
        }
    }

    return (intptr_t) obj;
}

/*
static val_t *object_find_prop(object_t *obj, intptr_t symbal) {
    object_t *cur = obj;
    int i;

    while (cur) {
        intptr_t *keys = cur->keys;
        for (i = 0; i < cur->prop_num; i++) {
            if (keys[i] == symbal) {
                return cur->vals + i;
            }
        }
        cur = cur->proto;
    }
    return NULL;
}

val_t *object_prop_ref(env_t *env, val_t *self, val_t *key)
{
    object_t *obj = (object_t *) val_2_intptr(self);
    const char *name = val_2_cstring(key);
    val_t *prop = NULL;

    if (name) {
        intptr_t sym_id = env_symbal_get(env, name);

        if (sym_id) {
            prop = object_find_prop_owned(obj, sym_id);
            if (prop) {
                return prop;
            }
            prop = object_add_prop(env, obj, sym_id);
        } else {
            prop = object_add_prop(env, obj, env_symbal_add(env, name));
        }

        if (prop) {
            val_set_undefined(prop);
        }
    }
    return prop;
}

void object_prop_val(env_t *env, val_t *self, val_t *key, val_t *prop)
{
    const char *name = val_2_cstring(key);
    object_t *obj;

    if (!name) {
        val_set_undefined(prop);
        return;
    }
    obj = (object_t *) val_2_intptr(self);
    if (obj) {
        val_t *v = object_find_prop(obj, env_symbal_get(env, name));
        if (v) {
            *prop = *v;
        } else {
            val_set_undefined(prop);
        }
    } else {
        val_set_undefined(prop);
        env_set_error(env, ERR_SysError);
    }
}
*/

int object_iter_next(object_iter_t *it, const char **name, val_t **v)
{
    if (it->cur < it->obj->prop_num) {
        int id = it->cur++;

        *name = (const char *)(it->obj->keys[id]);
        *v = it->obj->vals + id;

        return 1;
    } else {
        return 0;
    }
}

static val_t get_prop_length(env_t *env, void *obj)
{
    return val_mk_number(object_length(obj));
}

static val_t get_prop_to_str(env_t *env, void *obj)
{
    (void) env;
    (void) obj;
    return val_mk_native((intptr_t) native_object_to_string);
}

static val_t get_prop_foreach(env_t *env, void *obj)
{
    (void) env;
    (void) obj;
    return val_mk_native((intptr_t) native_object_foreach);
}

static const object_prop_t proto[] = {
    {(intptr_t)"length",   get_prop_length, NULL},
    {(intptr_t)"toString", get_prop_to_str, NULL},
    {(intptr_t)"foreach",  get_prop_foreach, NULL},
};

static inline int is_true(val_t *v) {
    return object_entry(v) != NULL;
};

static val_t get_prop(void *env, val_t *self, const char *name)
{
    object_t *obj = val_is_object(self) ? (object_t *)val_2_intptr(self) : NULL;
    intptr_t symbal = (intptr_t)name;
    unsigned i;

    if (obj) {
        object_t *cur = obj;
        while (cur) {
            intptr_t *keys = cur->keys;

            for (i = 0; i < cur->prop_num; i++) {
                if (keys[i] == symbal) {
                    return cur->vals[i];
                }
            }
            cur = cur->proto;
        }

        for (i = 0; i < sizeof(proto) / sizeof(object_prop_t); i++) {
            if (proto[i].symbal == symbal) {
                return proto[i].getter(env, obj);
            }
        }
    }

    return VAL_UNDEFINED;
}

/*
int object_set_proto_prop(void *env, object_t *obj, intptr_t symbal, val_t *data)
{
    int i;

    for (i = 0; i < sizeof(proto) / sizeof(object_prop_t); i++) {
        if (proto[i].symbal == symbal) {
            proto[i].setter(env, obj, data);
            return 1;
        }
    }
    return 0;
}
*/

static void set_prop(void *env, val_t *self, const char *name, val_t *data)
{
    object_t *obj = object_entry(self);
    intptr_t sym = env_symbal_get(env, name);
    val_t *prop = NULL;

    if (sym) {
        prop = object_find_prop_owned(obj, sym);

        if (!prop) {
            prop = object_add_prop(env, obj, sym);
        }
    } else {
        prop = object_add_prop(env, obj, env_symbal_add(env, name));
    }

    if (prop) {
        *prop = *data;
    }
}

static void opx_prop(void *env, val_t *self, const char *name, val_t *res, val_opx_t op)
{
    object_t *obj = object_entry(self);
    intptr_t sym = env_symbal_get(env, name);
    val_t *prop = NULL;

    if (sym) {
        prop = object_find_prop_owned(obj, sym);

        if (prop) {
            op(env, prop, res);
            return;

        } else {
            prop = object_add_prop(env, obj, sym);
        }
    } else {
        prop = object_add_prop(env, obj, env_symbal_add(env, name));
    }

    // vaule of prop is Undefined
    if (prop) {
        *res = *prop = VAL_NAN;
        val_set_nan(res);
    } else {
        val_set_nan(res);
    }
}

static void opxx_prop(void *env, val_t *self, const char *name, val_t *data, val_t *res, val_opxx_t op)
{
    object_t *obj = object_entry(self);
    intptr_t sym = env_symbal_get(env, name);
    val_t *prop = NULL;

    if (sym) {
        prop = object_find_prop_owned(obj, sym);
        if (!prop) {
            prop = object_add_prop(env, obj, sym);
            val_set_undefined(prop);
        }
    } else {
        prop = object_add_prop(env, obj, env_symbal_add(env, name));
        val_set_undefined(prop);
    }

    if (prop) {
        op(env, prop, data, prop);
        *res = *prop;
    } else {
        val_set_nan(res);
    }
}

void object_proto_init(env_t *env)
{
    unsigned i;
    for (i = 0; i < sizeof(proto) / sizeof(object_prop_t); i++) {
        env_symbal_add_static(env, (const char *)proto[i].symbal);
    }
}

const val_metadata_t metadata_object = {
    .name     = "object",

    .is_true  = is_true,
    .is_equal = val_op_false,

    .value_of = val_as_nan,

    .get_prop = get_prop,
    .set_prop = set_prop,
    .opx_prop = opx_prop,
    .opxx_prop = opxx_prop,
};

