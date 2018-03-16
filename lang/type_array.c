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

static array_t *array_space_extend_tail(env_t *env, val_t *self, int n)
{
    array_t *a = (array_t *)val_2_intptr(self);
    val_t *elems;
    int len;

    if (a->elem_size - a->elem_end > n) {
        return a;
    }
    len = array_length(a);

    if (a->elem_size -len > n) {
        memmove(a->elems, a->elems + a->elem_bgn, len);
        a->elem_bgn = 0;
        a->elem_end = len;
        return a;
    } else {
        int size = SIZE_ALIGN_16(len + n);
        if (size > UINT16_MAX) {
            env_set_error(env, ERR_ResourceOutLimit);
            return NULL;
        }
        elems = env_heap_alloc(env, size * sizeof(val_t));
        if (elems) {
            a = (array_t *)val_2_intptr(self);
            memcpy(elems, a->elems + a->elem_bgn, sizeof(val_t) * len);
            a->elem_size = size;
            a->elem_bgn = 0;
            a->elem_end = len;
            return a;
        } else {
            return NULL;
        }
    }
}

static array_t *array_space_extend_head(env_t *env, val_t *self, int n)
{
    array_t *a = (array_t *)val_2_intptr(self);
    val_t *elems;
    int len;

    if (a->elem_bgn > n) {
        return a;
    }
    len = array_length(a);

    if (a->elem_size - len > n) {
        n = a->elem_size - a->elem_end;
        memmove(a->elems + a->elem_bgn + n, a->elems + a->elem_bgn, len);
        a->elem_bgn += n;
        a->elem_end += n;
        return a;
    } else {
        int size = SIZE_ALIGN_16(len + n);
        if (size > UINT16_MAX) {
            env_set_error(env, ERR_ResourceOutLimit);
            return NULL;
        }
        elems = env_heap_alloc(env, size * sizeof(val_t));
        if (elems) {
            a = (array_t *)val_2_intptr(self);
            memcpy(elems + size - len, a->elems + a->elem_bgn, sizeof(val_t) * len);
            a->elem_size = size;
            a->elem_bgn = size - len;
            a->elem_end = size;
            return a;
        } else {
            return NULL;
        }
    }
}

array_t *_array_create(env_t *env, int n)
{
    array_t *array;
    int size = n < DEF_ELEM_SIZE ? DEF_ELEM_SIZE : n;

    if (size > UINT16_MAX) {
        env_set_error(env, ERR_ResourceOutLimit);
        return NULL;
    }

    array = env_heap_alloc(env, sizeof(array_t) + sizeof(val_t) * size);
    if (array) {
        val_t *vals = (val_t *)(array + 1);

        array->magic = MAGIC_ARRAY;
        array->age = 0;
        array->elem_size = size;
        array->elem_bgn  = 0;
        array->elem_end  = n;
        array->elems = vals;
    }

    return array;
}

intptr_t array_create(env_t *env, int ac, val_t *av)
{
    array_t *array = _array_create(env, ac);

    if (array) {
        memcpy(array->elems + array->elem_bgn, av, sizeof(val_t) * ac);
    }

    return (intptr_t) array;
}

/*
val_t *array_elem_ref(val_t *self, int i)
{
    array_t *array = (array_t *) val_2_intptr(self);

    if (i >= 0 && i < array_len(array)) {
        return array->elems + (array->elem_bgn + i);
    } else {
        return NULL;
    }
}

void array_elem_val(val_t *self, int i, val_t *elem)
{
    val_t *ref = array_elem_ref(self, i);

    if (ref) {
        *elem = *ref;
    } else {
        val_set_undefined(elem);
    }
}
*/

static val_t native_push(env_t *env, int ac, val_t *av)
{
    if (ac > 1 && val_is_array(av)) {
        int n = ac - 1;
        array_t *a = array_space_extend_tail(env, av, n);

        if (a) {
            memcpy(a->elems + a->elem_end, av + 1, sizeof(val_t) * n);
            a->elem_end += n;
            return val_mk_number(array_length(a));
        }
    } else {
        env_set_error(env, ERR_InvalidInput);
    }
    return val_mk_undefined();
}

static val_t native_unshift(env_t *env, int ac, val_t *av)
{
    if (ac > 1 && val_is_array(av)) {
        int n = ac - 1;
        array_t *a = array_space_extend_head(env, av, n);

        if (a) {
            memcpy(a->elems + a->elem_bgn - n, av + 1, sizeof(val_t) * n);
            a->elem_bgn -= n;
            return val_mk_number(array_length(a));
        }
    } else {
        env_set_error(env, ERR_InvalidInput);
    }
    return val_mk_undefined();
}

static val_t native_pop(env_t *env, int ac, val_t *av)
{
    if (ac > 0 && val_is_array(av)) {
        array_t *a = (array_t *)val_2_intptr(av);

        if (array_length(a)) {
            return a->elems[--a->elem_end];
        }
    } else {
        env_set_error(env, ERR_InvalidInput);
    }
    return val_mk_undefined();
}

static val_t native_shift(env_t *env, int ac, val_t *av)
{
    if (ac > 0 && val_is_array(av)) {
        array_t *a = (array_t *)val_2_intptr(av);

        if (array_length(a)) {
            return a->elems[a->elem_bgn++];
        }
    } else {
        env_set_error(env, ERR_InvalidInput);
    }
    return val_mk_undefined();
}

static val_t native_foreach(env_t *env, int ac, val_t *av)
{
    if (ac > 1 && val_is_array(av) && val_is_function(av + 1)) {
        array_t *a = (array_t *)val_2_intptr(av);
        int i, max = array_length(a);

        for (i = 0; i < max && !env->error; i++) {
            val_t key = val_mk_number(i);

            env_push_call_argument(env, &key);
            env_push_call_argument(env, array_values(a) + i);
            env_push_call_function(env, av + 1);

            interp_execute_call(env, 2);
        }
    }

    return val_mk_undefined();
}

static inline int is_true(val_t *v) {
    array_t *a = array_entry(v);
    return a->elem_end - a->elem_bgn > 0 ? 1 : 0;
}

static double value_of(val_t *self)
{
    array_t *a = array_entry(self);
    val_t *elem;

    if (NULL != (elem = array_get(a, 0))) {
        if (val_is_number(elem)) {
            return val_2_double(elem);
        } else {
            return const_nan.d;
        }
    } else {
        return 0;
    }
}

static val_t get_prop_length(env_t *env, void *entry)
{
    array_t *a = entry;
    (void) env;
    return val_mk_number(array_length(a));
}

static val_t get_prop_push(env_t *env, void *entry)
{
    (void) env;
    (void) entry;
    return val_mk_native((intptr_t)native_push);
}

static val_t get_prop_pop(env_t *env, void *entry)
{
    (void) env;
    (void) entry;
    return val_mk_native((intptr_t)native_pop);
}

static val_t get_prop_shift(env_t *env, void *entry)
{
    (void) env;
    (void) entry;
    return val_mk_native((intptr_t)native_shift);
}

static val_t get_prop_unshift(env_t *env, void *entry)
{
    (void) env;
    (void) entry;
    return val_mk_native((intptr_t)native_unshift);
}

static val_t get_prop_foreach(env_t *env, void *entry)
{
    (void) env;
    (void) entry;
    return val_mk_native((intptr_t)native_foreach);
}

static const object_prop_t proto[] = {
    {(intptr_t)"length",   get_prop_length, NULL},
    {(intptr_t)"push",     get_prop_push, NULL},
    {(intptr_t)"pop",      get_prop_pop, NULL},
    {(intptr_t)"shift",    get_prop_shift, NULL},
    {(intptr_t)"unshift",  get_prop_unshift, NULL},
    {(intptr_t)"foreach",  get_prop_foreach, NULL},
};

static val_t get_prop(void *env, val_t *self, const char *name)
{
    array_t *a = val_is_array(self) ? (array_t *)val_2_intptr(self) : NULL;
    intptr_t symbal = (intptr_t)name;

    if (a) {
        unsigned i;

        for (i = 0; i < sizeof(proto) / sizeof(object_prop_t); i++) {
            if (proto[i].symbal == symbal) {
                return proto[i].getter(env, a);
            }
        }
    }

    return VAL_UNDEFINED;
}

static val_t get_elem(void *env, val_t *self, int id)
{
    array_t *a = val_is_array(self) ? (array_t *)val_2_intptr(self) : NULL;

    (void) env;
    if (a) {
        if (id >= 0 && id < array_length(a)) {
            return a->elems[a->elem_bgn + id];
        }
    }
    return VAL_UNDEFINED;
}

static val_t *ref_elem(void *env, val_t *self, int id)
{
    array_t *a = val_is_array(self) ? (array_t *)val_2_intptr(self) : NULL;

    (void) env;
    if (a) {
        if (id >= 0 && id < array_length(a)) {
            return a->elems + a->elem_bgn + id;
        }
    }
    return NULL;
}

const val_metadata_t metadata_array = {
    .name     = "object",

    .is_true  = is_true,
    .is_equal = val_op_false,

    .value_of = value_of,
    .get_prop = get_prop,
    .get_elem = get_elem,
    .ref_elem = ref_elem,
};

void array_proto_init(env_t *env)
{
    unsigned i;
    for (i = 0; i < sizeof(proto) / sizeof(object_prop_t); i++) {
        env_symbal_add_static(env, (const char *)proto[i].symbal);
    }
}

