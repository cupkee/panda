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

#include <math.h>
#include "val.h"
#include "env.h"
#include "type_boolean.h"
#include "type_number.h"
#include "type_string.h"
#include "type_array.h"
#include "type_object.h"
#include "type_function.h"

/*
const val_t _Infinity  = TAG_INFINITE;
const val_t _Undefined = TAG_UNDEFINED;
const val_t _True      = TAG_BOOLEAN + 1;
const val_t _False     = TAG_BOOLEAN;
const val_t _NaN       = TAG_NAN;
*/

const valnum_t const_nan = {
    .v = TAG_NAN
};

static inline int foreign_is_true(val_t *v) {
    val_foreign_t *vf = (val_foreign_t *)val_2_intptr(v);

    if (vf && vf->op && vf->op->is_true) {
        return vf->op->is_true(vf->data);
    } else {
        return 0;
    }
}

static inline int foreign_is_equal(val_t *a, val_t *b) {
    val_foreign_t *f = (val_foreign_t *)val_2_intptr(a);

    if (f && f->op && f->op->is_equal) {
        return f->op->is_equal(f->data, b);
    } else {
        return 0;
    }
}

static inline int foreign_is_gt(val_t *a, val_t *b) {
    val_foreign_t *f = (val_foreign_t *)val_2_intptr(a);

    if (f && f->op && f->op->is_gt) {
        return f->op->is_gt(f->data, b);
    } else {
        return 0;
    }
}

static inline int foreign_is_ge(val_t *a, val_t *b) {
    val_foreign_t *f = (val_foreign_t *)val_2_intptr(a);

    if (f && f->op && f->op->is_ge) {
        return f->op->is_ge(f->data, b);
    } else {
        return 0;
    }
}

static inline int foreign_is_lt(val_t *a, val_t *b) {
    val_foreign_t *f = (val_foreign_t *)val_2_intptr(a);

    if (f && f->op && f->op->is_lt) {
        return f->op->is_lt(f->data, b);
    } else {
        return 0;
    }
}

static inline int foreign_is_le(val_t *a, val_t *b) {
    val_foreign_t *f = (val_foreign_t *)val_2_intptr(a);

    if (f && f->op && f->op->is_le) {
        return f->op->is_le(f->data, b);
    } else {
        return 0;
    }
}

static inline void foreign_set(void *env, val_t *a, val_t *b, val_t *res) {
    val_foreign_t *f = (val_foreign_t *)val_2_intptr(a);

    if (f && f->op && f->op->set) {
        f->op->set(env, f->data, b, res);
    } else {
        *res = *a = *b;
    }
}

static inline void foreign_prop(void *env, val_t *a, val_t *b, val_t *res) {
    val_foreign_t *f = (val_foreign_t *)val_2_intptr(a);

    if (f && f->op && f->op->prop) {
        f->op->prop(env, f->data, b, res);
    } else {
        val_set_undefined(res);
    }
}

static inline void foreign_elem(void *env, val_t *a, val_t *b, val_t *res) {
    val_foreign_t *f = (val_foreign_t *)val_2_intptr(a);

    if (f && f->op && f->op->elem) {
        f->op->elem(env, f->data, b, res);
    } else {
        val_set_undefined(res);
    }
}

static inline val_t *foreign_prop_ref(void *env, val_t *a, val_t *b) {
    val_foreign_t *f = (val_foreign_t *)val_2_intptr(a);

    if (f && f->op && f->op->prop_ref) {
        return f->op->prop_ref(env, f->data, b);
    } else {
        return NULL;
    }
}

static inline val_t *foreign_elem_ref(void *env, val_t *a, val_t *b) {
    val_foreign_t *f = (val_foreign_t *)val_2_intptr(a);

    if (f && f->op && f->op->elem_ref) {
        return f->op->elem_ref(env, f->data, b);
    } else {
        return NULL;
    }
}

static void def_elem_get(val_t *self, int index, val_t *elem)
{
    (void) self;
    (void) index;
    val_set_undefined(elem);
}

static val_t *def_elem_ref(val_t *self, int index)
{
    (void) self;
    (void) index;
    return NULL;
}

static val_t def_to_string(env_t *env, int ac, val_t *obj)
{
    (void) env;
    if (ac < 1) {
        return val_mk_foreign_string((intptr_t)"");
    }

    if (val_is_string(obj)) {
        return *obj;
    } else
    if (val_is_number(obj)) {
        return val_mk_foreign_string((intptr_t)"<number>");
    } else
    if (val_is_undefined(obj)) {
        return val_mk_foreign_string((intptr_t)"<undefined>");
    } else
    if (val_is_nan(obj)) {
        return val_mk_foreign_string((intptr_t)"<NaN>");
    } else
    if (val_is_boolean(obj)) {
        return val_mk_foreign_string((intptr_t)(val_2_intptr(obj) ? "true" : "false"));
    } else
    if (val_is_function(obj)) {
        return val_mk_foreign_string((intptr_t)"<function>");
    } else
    if (val_is_array(obj)) {
        return val_mk_foreign_string((intptr_t)"<array>");
    } else {
        return val_mk_foreign_string((intptr_t)"<object>");
    }
}

typedef struct prop_desc_t {
    const char *name;
    const function_native_t entry;
} prop_desc_t;

typedef struct type_desc_t {
    void               (*elem_get)(val_t *, int, val_t*);
    val_t             *(*elem_ref)(val_t *, int index);
    int                prop_num;
    const prop_desc_t *prop_descs;
} type_desc_t;

static const prop_desc_t number_prop_descs [] = {
    {
        .name = "toString",
        .entry = def_to_string
    }
};
static const prop_desc_t string_prop_descs [] = {
    {
        .name = "length",
        .entry = string_length
    }, {
        .name = "indexOf",
        .entry = string_index_of
    }
};
static const prop_desc_t boolean_prop_descs [] = {
    {
        .name = "toString",
        .entry = def_to_string
    }
};
static const prop_desc_t function_prop_descs [] = {
    {
        .name = "toString",
        .entry = def_to_string
    }
};
static const prop_desc_t und_prop_descs [] = {
    {
        .name = "toString",
        .entry = def_to_string
    }
};
static const prop_desc_t nan_prop_descs [] = {
    {
        .name = "toString",
        .entry = def_to_string
    }
};
static const prop_desc_t array_prop_descs [] = {
    {
        .name = "toString",
        .entry = def_to_string
    }, {
        .name = "length",
        .entry = array_length
    }, {
        .name = "push",
        .entry = array_push
    }, {
        .name = "pop",
        .entry = array_pop
    }, {
        .name = "shift",
        .entry = array_shift
    }, {
        .name = "unshift",
        .entry = array_unshift
    }, {
        .name = "foreach",
        .entry = array_foreach
    }
};

static const prop_desc_t date_prop_descs [] = {
    {
        .name = "toString",
        .entry = def_to_string
    }
};

static const type_desc_t type_desc_num = {
    .elem_get = def_elem_get,
    .elem_ref = def_elem_ref,
    .prop_num = sizeof(number_prop_descs) / sizeof(prop_desc_t),
    .prop_descs = number_prop_descs,
};
static const type_desc_t type_desc_str = {
    .elem_get = string_elem_get,
    .elem_ref = def_elem_ref,
    .prop_num = sizeof(string_prop_descs) / sizeof(prop_desc_t),
    .prop_descs = string_prop_descs,
};
static const type_desc_t type_desc_bool = {
    .elem_get = def_elem_get,
    .elem_ref = def_elem_ref,
    .prop_num = sizeof(boolean_prop_descs) / sizeof(prop_desc_t),
    .prop_descs = boolean_prop_descs,
};
static const type_desc_t type_desc_func = {
    .elem_get = def_elem_get,
    .elem_ref = def_elem_ref,
    .prop_num = sizeof(function_prop_descs) / sizeof(prop_desc_t),
    .prop_descs = function_prop_descs,
};
static const type_desc_t type_desc_array = {
    .elem_get = array_elem_val,
    .elem_ref = array_elem_ref,
    .prop_num = sizeof(array_prop_descs) / sizeof(prop_desc_t),
    .prop_descs = array_prop_descs,
};
static const type_desc_t type_desc_und = {
    .elem_get = def_elem_get,
    .elem_ref = def_elem_ref,
    .prop_num = sizeof(und_prop_descs) / sizeof(prop_desc_t),
    .prop_descs = und_prop_descs,
};
static const type_desc_t type_desc_nan = {
    .elem_get = def_elem_get,
    .elem_ref = def_elem_ref,
    .prop_num = sizeof(nan_prop_descs) / sizeof(prop_desc_t),
    .prop_descs = nan_prop_descs,
};
static const type_desc_t type_desc_date = {
    .elem_get = def_elem_get,
    .elem_ref = def_elem_ref,
    .prop_num = sizeof(date_prop_descs) / sizeof(prop_desc_t),
    .prop_descs = date_prop_descs,
};
static const type_desc_t type_desc_obj = {
    .elem_get = def_elem_get,
    .elem_ref = def_elem_ref,
    .prop_num = 0,
};
static const type_desc_t type_desc_foreign = {
    .elem_get = def_elem_get,
    .elem_ref = def_elem_ref,
    .prop_num = 0,
};

static const type_desc_t *const type_descs[] = {
    [TYPE_NUM]    = &type_desc_num,
    [TYPE_STR_I]  = &type_desc_str,
    [TYPE_STR_H]  = &type_desc_str,
    [TYPE_STR_F]  = &type_desc_str,
    [TYPE_BOOL]   = &type_desc_bool,
    [TYPE_FUNC]   = &type_desc_func,
    [TYPE_FUNC_C] = &type_desc_func,
    [TYPE_UND]    = &type_desc_und,
    [TYPE_NAN]    = &type_desc_nan,
    [TYPE_ARRAY]  = &type_desc_array,
    [TYPE_DATE]   = &type_desc_date,
    [TYPE_OBJ]    = &type_desc_obj,
    [TYPE_FOREIGN]  = &type_desc_foreign,
};

static const prop_desc_t *prop_desc_get(const prop_desc_t *descs, int max, val_t *key)
{
    const char *name = val_2_cstring(key);

    if (name) {
        int i;
        for (i = 0; i < max; i++) {
            if (!strcmp(name, descs[i].name)) {
                return descs + i;
            }
        }
    }
    return NULL;
}

static void type_prop_val(int type, val_t *key, val_t *prop)
{
    const prop_desc_t *desc;

    desc = prop_desc_get(type_descs[type]->prop_descs, type_descs[type]->prop_num, key);
    if (desc) {
        val_set_native(prop, (intptr_t)desc->entry);
    } else {
        val_set_undefined(prop);
    }
}

static inline void type_elem_val(int type, val_t *self, int index, val_t *elem)
{
    type_descs[type]->elem_get(self, index, elem);
}

static inline val_t *type_elem_ref(int type, val_t *self, int index)
{
    return type_descs[type]->elem_ref(self, index);
}

int val_as_true(val_t *self)
{
    (void) self;
    return 1;
}

int val_as_false(val_t *self)
{
    (void) self;
    return 0;
}

double val_as_zero(val_t *self)
{
    (void) self;
    return 0;
}

double val_as_nan(val_t *self)
{
    (void) self;
    return const_nan.d;
}

double val_as_integer(val_t *self)
{
    (void) self;
    return val_2_intptr(self);
}

double val_as_number(val_t *self)
{
    (void) self;
    return val_2_double(self);
}

int val_op_true(val_t *self, val_t *to)
{
    (void) self;
    (void) to;
    return 1;
}

int val_op_false(val_t *self, val_t *to)
{
    (void) self;
    (void) to;
    return 0;
}

const val_metadata_t metadata_undefined = {
    .is_true  = val_as_false,
    .is_equal = val_op_false,
    .is_gt    = val_op_false,
    .is_ge    = val_op_false,
    .is_lt    = val_op_false,
    .is_le    = val_op_false,

    .value_of = val_as_nan,
};

const val_metadata_t metadata_nan = {
    .is_true  = val_as_false,
    .is_equal = val_op_false,
    .is_gt    = val_op_false,
    .is_ge    = val_op_false,
    .is_lt    = val_op_false,
    .is_le    = val_op_false,

    .value_of = val_as_nan,
};

const val_metadata_t metadata_date = {
    .is_true  = val_as_true,
    .is_equal = val_op_false,
    .is_gt    = val_op_false,
    .is_ge    = val_op_false,
    .is_lt    = val_op_false,
    .is_le    = val_op_false,

    .value_of = val_as_integer,
};

const val_metadata_t metadata_array_buffer = {
    .is_true  = val_as_false,
    .is_equal = val_op_false,
    .is_gt    = val_op_false,
    .is_ge    = val_op_false,
    .is_lt    = val_op_false,
    .is_le    = val_op_false,

    .value_of = val_as_zero,
};

const val_metadata_t metadata_data_view = {
    .is_true  = val_as_false,
    .is_equal = val_op_false,
    .is_gt    = val_op_false,
    .is_ge    = val_op_false,
    .is_lt    = val_op_false,
    .is_le    = val_op_false,

    .value_of = val_as_zero,
};

const val_metadata_t metadata_object_foreign = {
    .is_true  = foreign_is_true,
    .is_equal = foreign_is_equal,
    .is_gt    = foreign_is_gt,
    .is_ge    = foreign_is_ge,
    .is_lt    = foreign_is_lt,
    .is_le    = foreign_is_le,

    .value_of = val_as_zero,
};

const val_metadata_t metadata_none = {
    .is_true  = val_as_false,
    .is_equal = val_op_false,
    .is_gt    = val_op_false,
    .is_ge    = val_op_false,
    .is_lt    = val_op_false,
    .is_le    = val_op_false,

    .value_of = val_as_zero,
};


static const val_metadata_t *base_metadata[] = {
    &metadata_num,            // 0
    &metadata_str_inline,     // 1
    &metadata_str_heap,       // 2
    &metadata_str_foreign,    // 3
    &metadata_boolean,           // 4
    &metadata_function,          // 5
    &metadata_function_native,   // 6
    &metadata_undefined,         // 7
    &metadata_nan,               // 8
    &metadata_date,              // 9
    &metadata_array_buffer,      // 10
    &metadata_data_view,         // 11
    &metadata_array,             // 12
    &metadata_object,            // 13
    &metadata_object_foreign,    // 14
    &metadata_none,              // 15
};

int val_is_true(val_t *v)
{
    const val_metadata_t *meta = base_metadata[val_type(v)];

    return meta->is_true(v);
}

int val_is_equal(val_t *a, val_t *b)
{
    if (*a == *b) {
        return !val_is_nan(a);
    } else {
        const val_metadata_t *meta = base_metadata[val_type(a)];

        return meta->is_equal(a, b);
    }
}

int val_is_gt(val_t *self, val_t *to)
{
    const val_metadata_t *meta = base_metadata[val_type(self)];

    return meta->is_gt(self, to);
}

int val_is_ge(val_t *self, val_t *to)
{
    const val_metadata_t *meta = base_metadata[val_type(self)];

    return meta->is_ge(self, to);
}

int val_is_le(val_t *self, val_t *to)
{
    const val_metadata_t *meta = base_metadata[val_type(self)];

    return meta->is_le(self, to);
}

int val_is_lt(val_t *self, val_t *to)
{
    const val_metadata_t *meta = base_metadata[val_type(self)];

    return meta->is_lt(self, to);
}

void val_op_neg(void *env, val_t *self, val_t *result)
{
    (void) env;

    if (val_is_not_number(self)) {
        const val_metadata_t *meta = base_metadata[val_type(self)];

        val_set_number(result, -meta->value_of(self));
    } else {
        val_set_number(result, -val_2_double(self));
    }
}

void val_op_not(void *env, val_t *self, val_t *result)
{
    (void) env;

    if (val_is_number(self)) {
        val_set_number(result, ~val_2_integer(self));
    } else {
        const val_metadata_t *meta = base_metadata[val_type(self)];
        double v = meta->value_of(self);

        if (isnan(v)) {
            val_set_nan(result);
        } else {
            val_set_number(result, ~lround(v));
        }
    }
}

void val_op_inc(void *env, val_t *self, val_t *res)
{
    (void) env;
    if (val_is_number(self)) {
        ((valnum_t *)res)->d = ((valnum_t *)self)->d++;
    } else {
        val_set_nan(res);
    }
}

void val_op_incp(void *env, val_t *self, val_t *res)
{
    (void) env;
    if (val_is_number(self)) {
        ((valnum_t *)res)->d = ++((valnum_t *)self)->d;
    } else {
        val_set_nan(res);
    }
}

void val_op_dec(void *env, val_t *self, val_t *res)
{
    (void) env;
    if (val_is_number(self)) {
        ((valnum_t *)res)->d = ((valnum_t *)self)->d--;
    } else {
        val_set_nan(res);
    }
}

void val_op_decp(void *env, val_t *self, val_t *res)
{
    (void) env;
    if (val_is_number(self)) {
        ((valnum_t *)res)->d = --((valnum_t *)self)->d;
    } else {
        val_set_nan(res);
    }
}

void val_op_mul(void *env, val_t *a, val_t *b, val_t *res)
{
    double v = val_2_double(a) * val_2_double(b);

    (void) env;
    if (isnan(v)) {
        const val_metadata_t *meta_a = base_metadata[val_type(a)];
        const val_metadata_t *meta_b = base_metadata[val_type(b)];

        ((valnum_t *)res)->d = meta_a->value_of(a) * meta_b->value_of(b);
    } else {
        val_set_number(res, v);
    }
}

void val_op_div(void *env, val_t *a, val_t *b, val_t *res)
{
    const val_metadata_t *meta_a = base_metadata[val_type(a)];
    const val_metadata_t *meta_b = base_metadata[val_type(b)];

    ((valnum_t *)res)->d = meta_a->value_of(a) / meta_b->value_of(b);

    (void) env;
}

void val_op_mod(void *env, val_t *a, val_t *b, val_t *res)
{
    const val_metadata_t *meta_a = base_metadata[val_type(a)];
    const val_metadata_t *meta_b = base_metadata[val_type(b)];

    ((valnum_t *)res)->d = fmod(meta_a->value_of(a), meta_b->value_of(b));

    (void) env;
}

void val_op_add(void *env, val_t *a, val_t *b, val_t *res)
{
    double v = val_2_double(a) + val_2_double(b);

    (void) env;
    if (isnan(v)) {
        const val_metadata_t *meta_a = base_metadata[val_type(a)];

        if (meta_a->concat) {
            meta_a->concat(env, a, b, res);
        } else {
            const val_metadata_t *meta_b = base_metadata[val_type(b)];
            ((valnum_t *)res)->d = meta_a->value_of(a) + meta_b->value_of(b);
        }
    } else {
        val_set_number(res, v);
    }
}

void val_op_sub(void *env, val_t *a, val_t *b, val_t *res)
{
    double v = val_2_double(a) - val_2_double(b);

    (void) env;
    if (isnan(v)) {
        const val_metadata_t *meta_a = base_metadata[val_type(a)];
        const val_metadata_t *meta_b = base_metadata[val_type(b)];

        ((valnum_t *)res)->d = meta_a->value_of(a) - meta_b->value_of(b);
    } else {
        val_set_number(res, v);
    }
}

void val_op_and(void *env, val_t *a, val_t *b, val_t *res)
{
    int ia = val_is_number(a) ? val_2_integer(a) : val_is_boolean(a) ? val_2_intptr(a) : 0;
    int ib = val_is_number(b) ? val_2_integer(b) : val_is_boolean(b) ? val_2_intptr(b) : 0;

    (void) env;

    val_set_number(res, ia & ib);
}

void val_op_or(void *env, val_t *a, val_t *b, val_t *res)
{
    int ia = val_is_number(a) ? val_2_integer(a) : val_is_boolean(a) ? val_2_intptr(a) : 0;
    int ib = val_is_number(b) ? val_2_integer(b) : val_is_boolean(b) ? val_2_intptr(b) : 0;

    (void) env;

    val_set_number(res, ia | ib);
}

void val_op_xor(void *env, val_t *a, val_t *b, val_t *res)
{
    int ia = val_is_number(a) ? val_2_integer(a) : val_is_boolean(a) ? val_2_intptr(a) : 0;
    int ib = val_is_number(b) ? val_2_integer(b) : val_is_boolean(b) ? val_2_intptr(b) : 0;

    (void) env;

    val_set_number(res, ia ^ ib);
}

void val_op_lshift(void *env, val_t *a, val_t *b, val_t *res)
{
    int ia = val_is_number(a) ? val_2_integer(a) : val_is_boolean(a) ? val_2_intptr(a) : 0;
    int ib = val_is_number(b) ? val_2_integer(b) : val_is_boolean(b) ? val_2_intptr(b) : 0;

    (void) env;

    val_set_number(res, ia << ib);
}

void val_op_rshift(void *env, val_t *a, val_t *b, val_t *res)
{
    int ia = val_is_number(a) ? val_2_integer(a) : val_is_boolean(a) ? val_2_intptr(a) : 0;
    int ib = val_is_number(b) ? val_2_integer(b) : val_is_boolean(b) ? val_2_intptr(b) : 0;

    (void) env;

    val_set_number(res, ia >> ib);
}

void val_op_prop(void *env, val_t *self, val_t *key, val_t *prop)
{
    int type = val_type(self);

    if (type == TYPE_OBJ) {
        object_prop_val(env, self, key, prop);
    } else
    if (type == TYPE_FOREIGN) {
        foreign_prop(env, self, key, prop);
    } else {
        type_prop_val(type, key, prop);
    }
}

void val_op_elem(void *env, val_t *self, val_t *key, val_t *prop)
{
    int type = val_type(self);

    if (type == TYPE_OBJ) {
        object_prop_val(env, self, key, prop);
    } else
    if (type == TYPE_FOREIGN) {
        foreign_elem(env, self, key, prop);
    } else {
        if (val_is_number(key)) {
            type_elem_val(type, self, val_2_integer(key), prop);
        } else {
            type_prop_val(type, key, prop);
        }
    }
}

void val_op_set(void *env, val_t *a, val_t *b, val_t *r)
{
    if (!val_is_foreign(a)) {
        *r = *a = *b;
    } else {
        foreign_set(env, a, b, r);
    }
}

val_t *val_prop_ref(void *env, val_t *self, val_t *key)
{
    int type = val_type(self);

    if (type == TYPE_OBJ) {
        return object_prop_ref(env, self, key);
    } else
    if (type == TYPE_FOREIGN) {
        return foreign_prop_ref(env, self, key);
    } else {
        return NULL;
    }
}


val_t *val_elem_ref(void *env, val_t *self, val_t *id)
{
    int type = val_type(self);

    if (type == TYPE_OBJ) {
        return object_prop_ref(env, self, id);
    } else
    if (type == TYPE_FOREIGN) {
        return foreign_elem_ref(env, self, id);
    } else {
        if (val_is_number(id)) {
            return type_elem_ref(type, self, val_2_integer(id));
        } else {
            return NULL;
        }
    }
}

val_t val_create(void *env, const val_foreign_op_t *op, intptr_t data)
{
    val_foreign_t *vf = env_heap_alloc(env, SIZE_ALIGN(sizeof(val_foreign_t)));

    if (vf) {
        vf->magic = MAGIC_FOREIGN;
        vf->age = 0;
        vf->op = op;
        vf->data = data;
        return val_mk_foreign((intptr_t)vf);
    } else {
        env_set_error(env, ERR_NotEnoughMemory);
        return VAL_UNDEFINED;
    }
}

int val_foreign_create(void *env, const val_foreign_op_t *op, intptr_t data, val_t *foreign)
{
    val_foreign_t *vf = env_heap_alloc(env, SIZE_ALIGN(sizeof(val_foreign_t)));

    if (vf) {
        vf->magic = MAGIC_FOREIGN;
        vf->age = 0;
        vf->op = op;
        vf->data = data;
        val_set_foreign(foreign, (intptr_t)vf);
        return 0;
    } else {
        env_set_error(env, ERR_NotEnoughMemory);
        return -1;
    }
}

