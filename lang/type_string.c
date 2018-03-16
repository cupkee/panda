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

#include "err.h"
#include "type_object.h"
#include "type_string.h"

static inline string_t *string_alloc(env_t *env, int size)
{
    string_t *s = env_heap_alloc(env, SIZE_ALIGN(sizeof(string_t) + size));

    if (s) {
        s->magic = MAGIC_STRING;
        s->age = 0;
        s->size = size;
    }
    return s;
}

val_t string_create_heap_val(env_t *env, const char *data)
{
    string_t *s;
    int size = strlen(data);

    size += 1;
    s = string_alloc(env, size);
    if (s) {
        memcpy(s->str, data, size);
        return val_mk_heap_string((intptr_t)s);
    }
    return VAL_UNDEFINED;
}

/*
void string_add(env_t *env, val_t *a, val_t *b, val_t *res)
{
    if (!val_is_string(b)) {
        val_set_nan(res);
        return;
    }

    int size1 = string_len(a);
    int size2 = string_len(b);
    int len = size1 + size2;
    string_t *s = string_alloc(env, len + 1);

    if (s) {
        // Todo: length overflow should be check! or variable length field.
        memcpy(s->str + 0, val_2_cstring(a), size1);
        memcpy(s->str + size1, val_2_cstring(b), size2 + 1);
        val_set_heap_string(res, (intptr_t) s);
    } else {
        env_set_error(env, ERR_NotEnoughMemory);
        val_set_undefined(res);
    }
}

void string_elem_get(val_t *self, int i, val_t *elem)
{
    const char *s = val_2_cstring(self);
    int len = string_len(self);

    if (i >= 0 && i < len) {
        val_set_inner_string(elem, s[i]);
    } else {
        val_set_undefined(elem);
    }
}
*/

static val_t native_index_of(env_t *env, int ac, val_t *av)
{
    const char *s, *f;

    if (ac < 2 || NULL == (s = val_2_cstring(av))) {
        env_set_error(env, ERR_InvalidInput);
        return val_mk_undefined();
    } else
    if (NULL == (f = val_2_cstring(av+1))) {
        return val_mk_number(-1);
    } else {
        char *pos = strstr(s, f);
        if (pos)  {
            return val_mk_number(pos - s);
        } else {
            return val_mk_number(-1);
        }
    }
}

static val_t get_length(env_t *env, void *s)
{
    (void) env;
    return val_mk_number(strlen(s));
}

static val_t get_index_of(env_t *env, void *s)
{
    (void) env;
    (void) s;
    return val_mk_native((intptr_t) native_index_of);
}

static object_prop_t proto[] = {
    {(intptr_t)"length",   get_length,   NULL},
    {(intptr_t)"indexOf",  get_index_of, NULL},
};

static int string_inline_is_true(val_t *self) {
    return ((const char *)self)[4];
}

static int string_inline_compare(val_t *self, val_t *to) {
    const char *a = ((const char *)self) + 4;
    const char *b = val_2_cstring(to);

    return b ? !strcmp(a, b) : 0;
}

static int string_heap_is_true(val_t *self) {
    return ((const char *) val_2_intptr(self))[4];
}

static int string_heap_compare(val_t *self, val_t *to) {
    const char *a = (const char *) (val_2_intptr(self) + 4);
    const char *b = val_2_cstring(to);

    return b ? !strcmp(a, b) : 0;
}

static int string_foreign_is_true(val_t *self) {
    return ((const char *) val_2_intptr(self))[0];
}

static int string_foreign_compare(val_t *self, val_t *to) {
    const char *a = (const char *) val_2_intptr(self);
    const char *b = val_2_cstring(to);

    return b ? !strcmp(a, b) : 0;
}

static double value_of_string(val_t *self)
{
    const char *s = val_2_cstring(self);

    return *s ? const_nan.d : 0;
}

static int string_concat(void *hnd, val_t *a, val_t *b, val_t *res)
{
    env_t *env = hnd;

    if (!val_is_string(b)) {
        val_set_nan(res);
        return 0;
    } else {
        int size1 = string_len(a);
        int size2 = string_len(b);
        int len = size1 + size2;
        string_t *s = string_alloc(env, len + 1);

        if (s) {
            // Todo: length overflow should be check! or variable length field.
            memcpy(s->str + 0, val_2_cstring(a), size1);
            memcpy(s->str + size1, val_2_cstring(b), size2 + 1);
            val_set_heap_string(res, (intptr_t) s);
            return 0;
        } else {
            env_set_error(env, ERR_NotEnoughMemory);
            val_set_undefined(res);
            return -ERR_NotEnoughMemory;
        }
    }
}

static val_t string_get_prop(void *env, val_t *self, const char *name)
{
    const char *s = val_2_cstring(self);
    intptr_t symbal = (intptr_t)name;

    if (s) {
        unsigned i;

        for (i = 0; i < sizeof(proto) / sizeof(object_prop_t); i++) {
            if (proto[i].symbal == symbal) {
                return proto[i].getter(env, (void *)s);
            }
        }
    }

    return VAL_UNDEFINED;
}

static val_t string_get_elem(void *env, val_t *self, int id)
{
    const char *s = val_2_cstring(self);
    int len = string_len(self);

    (void) env;
    if (id >= 0 && id < len) {
        return val_mk_inner_string(s[id]);
    } else {
        return VAL_UNDEFINED;
    }
}

const val_metadata_t metadata_str_inline = {
    .name     = "string",
    .is_true  = string_inline_is_true,
    .is_equal = string_inline_compare,

    .value_of = value_of_string,
    .get_prop = string_get_prop,
    .get_elem = string_get_elem,

    .concat   = string_concat,
};

const val_metadata_t metadata_str_heap = {
    .name     = "string",
    .is_true  = string_heap_is_true,
    .is_equal = string_heap_compare,

    .value_of = value_of_string,
    .get_prop = string_get_prop,
    .get_elem = string_get_elem,

    .concat   = string_concat,
};

const val_metadata_t metadata_str_foreign = {
    .name     = "string",
    .is_true  = string_foreign_is_true,
    .is_equal = string_foreign_compare,

    .value_of = value_of_string,
    .get_prop = string_get_prop,
    .get_elem = string_get_elem,

    .concat   = string_concat,
};

void string_proto_init(env_t *env)
{
    unsigned i;
    for (i = 0; i < sizeof(proto) / sizeof(object_prop_t); i++) {
        env_symbal_add_static(env, (const char *)proto[i].symbal);
    }
}

