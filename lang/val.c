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

#include "val.h"
#include "env.h"
#include "number.h"
#include "string.h"
#include "array.h"
#include "object.h"
#include "function.h"

const val_t _Infinity  = TAG_INFINITE;
const val_t _Undefined = TAG_UNDEFINED;
const val_t _True      = TAG_BOOLEAN + 1;
const val_t _False     = TAG_BOOLEAN;
const val_t _NaN       = TAG_NAN;

static void def_elem_get(val_t *self, int index, val_t *elem)
{
    val_set_undefined(elem);
}

static val_t *def_elem_ref(val_t *self, int index)
{
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
static const prop_desc_t err_prop_descs [] = {
    {
        .name = "toString",
        .entry = def_to_string
    }
};
static const prop_desc_t buf_prop_descs [] = {
    {
        .name = "toString",
        .entry = def_to_string
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
static const type_desc_t type_desc_err = {
    .elem_get = def_elem_get,
    .elem_ref = def_elem_ref,
    .prop_num = sizeof(err_prop_descs) / sizeof(prop_desc_t),
    .prop_descs = err_prop_descs,
};
static const type_desc_t type_desc_buf = {
    .elem_get = def_elem_get,
    .elem_ref = def_elem_ref,
    .prop_num = sizeof(buf_prop_descs) / sizeof(prop_desc_t),
    .prop_descs = buf_prop_descs,
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
    [TYPE_BUF]    = &type_desc_buf,
    [TYPE_ERR]    = &type_desc_err,
    [TYPE_DATE]   = &type_desc_date,
    [TYPE_OBJ]    = &type_desc_obj,
    [TYPE_OBJ_F]  = &type_desc_foreign,
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

int val_is_true(val_t *v)
{
    switch(val_type(v)) {
    case TYPE_NUM:      return val_2_double(v) != 0;
    case TYPE_STR_I:
    case TYPE_STR_H:
    case TYPE_STR_F:    return *val_2_cstring(v);
    case TYPE_BOOL:     return val_2_intptr(v);
    case TYPE_FUNC:
    case TYPE_FUNC_C:   return 1;
    case TYPE_UND:
    case TYPE_NAN:      return 0;
    case TYPE_ARRAY:
    case TYPE_BUF:
    case TYPE_ERR:
    case TYPE_DATE:
    case TYPE_OBJ:
    case TYPE_OBJ_F:
    default: return 0;
    }
}

int val_is_equal(val_t *a, val_t *b)
{
    if (*a == *b) {
        return !((val_is_nan(a) || val_is_undefined(a)));
    } else {
        if (val_is_string(a)) {
            return string_compare(a, b) == 0;
        } else {
            return 0;
        }
    }
}

int val_is_ge(val_t *op1, val_t *op2)
{
    switch(val_type(op1)) {
    case TYPE_NUM:      return val_is_number(op2) ? val_2_double(op1) >= val_2_double(op2) : 0;
    case TYPE_STR_I:
    case TYPE_STR_H:
    case TYPE_STR_F:    return val_is_string(op2) ? string_compare(op1, op2) >= 0: 0;
    case TYPE_BOOL:     return val_is_boolean(op2) ? val_2_intptr(op1) >= val_2_intptr(op2) : 0;
    case TYPE_FUNC:
    case TYPE_FUNC_C:
    case TYPE_UND:
    case TYPE_NAN:
    case TYPE_ARRAY:
    case TYPE_BUF:
    case TYPE_ERR:
    case TYPE_DATE:
    case TYPE_OBJ:
    case TYPE_OBJ_F:
    default: return 0;
    }
}

int val_is_gt(val_t *op1, val_t *op2)
{
    switch(val_type(op1)) {
    case TYPE_NUM:      return val_is_number(op2) ? val_2_double(op1) > val_2_double(op2) : 0;
    case TYPE_STR_I:
    case TYPE_STR_H:
    case TYPE_STR_F:    return val_is_string(op2) ? string_compare(op1, op2) > 0: 0;
    case TYPE_BOOL:     return val_is_boolean(op2) ? val_2_intptr(op1) > val_2_intptr(op2) : 0;
    case TYPE_FUNC:
    case TYPE_FUNC_C:
    case TYPE_UND:
    case TYPE_NAN:
    case TYPE_ARRAY:
    case TYPE_BUF:
    case TYPE_ERR:
    case TYPE_DATE:
    case TYPE_OBJ:
    case TYPE_OBJ_F:
    default: return 0;
    }
}

int val_is_le(val_t *op1, val_t *op2)
{
    switch(val_type(op1)) {
    case TYPE_NUM:      return val_is_number(op2) ? val_2_double(op1) <= val_2_double(op2) : 0;
    case TYPE_STR_I:
    case TYPE_STR_H:
    case TYPE_STR_F:    return val_is_string(op2) ? string_compare(op1, op2) <= 0: 0;
    case TYPE_BOOL:     return val_is_boolean(op2) ? val_2_intptr(op1) <= val_2_intptr(op2) : 0;
    case TYPE_FUNC:
    case TYPE_FUNC_C:
    case TYPE_UND:
    case TYPE_NAN:
    case TYPE_ARRAY:
    case TYPE_BUF:
    case TYPE_ERR:
    case TYPE_DATE:
    case TYPE_OBJ:
    case TYPE_OBJ_F:
    default: return 0;
    }
}

int val_is_lt(val_t *op1, val_t *op2)
{
    switch(val_type(op1)) {
    case TYPE_NUM:      return val_is_number(op2) ? val_2_double(op1) < val_2_double(op2) : 0;
    case TYPE_STR_I:
    case TYPE_STR_H:
    case TYPE_STR_F:    return val_is_string(op2) ? string_compare(op1, op2) < 0: 0;
    case TYPE_BOOL:     return val_is_boolean(op2) ? val_2_intptr(op1) < val_2_intptr(op2) : 0;
    case TYPE_FUNC:
    case TYPE_FUNC_C:
    case TYPE_UND:
    case TYPE_NAN:
    case TYPE_ARRAY:
    case TYPE_BUF:
    case TYPE_ERR:
    case TYPE_DATE:
    case TYPE_OBJ:
    case TYPE_OBJ_F:
    default: return 0;
    }
}

static void foreign_prop_val(val_t *self, val_t *key, val_t *prop)
{
    (void) self;
    (void) key;
    val_set_undefined(prop);
}

void val_op_prop(void *env, val_t *self, val_t *key, val_t *prop)
{
    int type = val_type(self);

    if (type == TYPE_OBJ) {
        object_prop_val(env, self, key, prop);
    } else
    if (type == TYPE_OBJ_F) {
        foreign_prop_val(self, key, prop);
    } else {
        type_prop_val(type, key, prop);
    }
}

void val_op_elem(void *env, val_t *self, val_t *key, val_t *prop)
{
    int type = val_type(self);

    if (val_is_number(key)) {
        type_elem_val(type, self, val_2_integer(key), prop);
    } else {
        if (type == TYPE_OBJ) {
            object_prop_val(env, self, key, prop);
        } else
        if (type == TYPE_OBJ_F) {
            foreign_prop_val(self, key, prop);
        } else {
            type_prop_val(type, key, prop);
        }
    }
}

val_t *val_prop_ref(void *env, val_t *self, val_t *key)
{
    int type = val_type(self);

    if (type == TYPE_OBJ) {
        return object_prop_ref(env, self, key);
    } else {
        return NULL;
    }
}


val_t * val_elem_ref(void *env, val_t *self, val_t *id)
{
    int type = val_type(self);

    if (val_is_number(id)) {
        return type_elem_ref(type, self, val_2_integer(id));
    } else {
        if (type == TYPE_OBJ) {
            return object_prop_ref(env, self, id);
        } else {
            return NULL;
        }
    }
}

void val_op_neg(val_t *ops)
{
    if (val_is_number(ops)) {
        return val_set_number(ops, -val_2_double(ops));
    } else {
        return val_set_nan(ops);
    }
}

void val_op_not(val_t *ops)
{
    if (val_is_number(ops)) {
        return val_set_number(ops, ~val_2_integer(ops));
    } else {
        return val_set_nan(ops);
    }
}

void val_op_inc(void *env, val_t *op1, val_t *res)
{
    switch(val_type(op1)) {
    case TYPE_NUM:   number_inc(op1, res); break;
    case TYPE_STR_I:
    case TYPE_STR_H:
    case TYPE_STR_F:
    case TYPE_BOOL:
    case TYPE_FUNC:
    case TYPE_FUNC_C:
    case TYPE_UND:
    case TYPE_NAN:
    case TYPE_ARRAY:
    case TYPE_BUF:
    case TYPE_ERR:
    case TYPE_DATE:
    case TYPE_OBJ:
    case TYPE_OBJ_F:
    default: val_set_nan(res);
    }
}

void val_op_incp(void *env, val_t *op1, val_t *res)
{
    switch(val_type(op1)) {
    case TYPE_NUM:   number_incp(op1, res); break;
    case TYPE_STR_I:
    case TYPE_STR_H:
    case TYPE_STR_F:
    case TYPE_BOOL:
    case TYPE_FUNC:
    case TYPE_FUNC_C:
    case TYPE_UND:
    case TYPE_NAN:
    case TYPE_ARRAY:
    case TYPE_BUF:
    case TYPE_ERR:
    case TYPE_DATE:
    case TYPE_OBJ:
    case TYPE_OBJ_F:
    default: val_set_nan(res);
    }
}

void val_op_dec(void *env, val_t *op1, val_t *res)
{
    switch(val_type(op1)) {
    case TYPE_NUM:   number_dec(op1, res); break;
    case TYPE_STR_I:
    case TYPE_STR_H:
    case TYPE_STR_F:
    case TYPE_BOOL:
    case TYPE_FUNC:
    case TYPE_FUNC_C:
    case TYPE_UND:
    case TYPE_NAN:
    case TYPE_ARRAY:
    case TYPE_BUF:
    case TYPE_ERR:
    case TYPE_DATE:
    case TYPE_OBJ:
    case TYPE_OBJ_F:
    default: val_set_nan(res);
    }
}

void val_op_decp(void *env, val_t *op1, val_t *res)
{
    switch(val_type(op1)) {
    case TYPE_NUM:   number_decp(op1, res); break;
    case TYPE_STR_I:
    case TYPE_STR_H:
    case TYPE_STR_F:
    case TYPE_BOOL:
    case TYPE_FUNC:
    case TYPE_FUNC_C:
    case TYPE_UND:
    case TYPE_NAN:
    case TYPE_ARRAY:
    case TYPE_BUF:
    case TYPE_ERR:
    case TYPE_DATE:
    case TYPE_OBJ:
    case TYPE_OBJ_F:
    default: val_set_nan(res);
    }
}

void val_op_mul(void *env, val_t *op1, val_t *op2, val_t *res)
{
    switch(val_type(op1)) {
    case TYPE_NUM:   number_mul(op1, op2, res); break;
    case TYPE_STR_I:
    case TYPE_STR_H:
    case TYPE_STR_F:
    case TYPE_BOOL:
    case TYPE_FUNC:
    case TYPE_FUNC_C:
    case TYPE_UND:
    case TYPE_NAN:
    case TYPE_ARRAY:
    case TYPE_BUF:
    case TYPE_ERR:
    case TYPE_DATE:
    case TYPE_OBJ:
    case TYPE_OBJ_F:
    default: val_set_nan(res);
    }
}

void val_op_div(void *env, val_t *op1, val_t *op2, val_t *res)
{
    switch(val_type(op1)) {
    case TYPE_NUM:   number_div(op1, op2, res); break;
    case TYPE_STR_I:
    case TYPE_STR_H:
    case TYPE_STR_F:
    case TYPE_BOOL:
    case TYPE_FUNC:
    case TYPE_FUNC_C:
    case TYPE_UND:
    case TYPE_NAN:
    case TYPE_ARRAY:
    case TYPE_BUF:
    case TYPE_ERR:
    case TYPE_DATE:
    case TYPE_OBJ:
    case TYPE_OBJ_F:
    default: val_set_nan(res);
    }
}

void val_op_mod(void *env, val_t *op1, val_t *op2, val_t *res)
{
    switch(val_type(op1)) {
    case TYPE_NUM:   number_mod(op1, op2, res); break;
    case TYPE_STR_I:
    case TYPE_STR_H:
    case TYPE_STR_F:
    case TYPE_BOOL:
    case TYPE_FUNC:
    case TYPE_FUNC_C:
    case TYPE_UND:
    case TYPE_NAN:
    case TYPE_ARRAY:
    case TYPE_BUF:
    case TYPE_ERR:
    case TYPE_DATE:
    case TYPE_OBJ:
    case TYPE_OBJ_F:
    default: val_set_nan(res);
    }
}

void val_op_add(void *env, val_t *op1, val_t *op2, val_t *res)
{
    switch(val_type(op1)) {
    case TYPE_NUM:   number_add(op1, op2, res); break;
    case TYPE_STR_I:
    case TYPE_STR_H:
    case TYPE_STR_F: string_add(env, op1, op2, res); break;
    case TYPE_BOOL:
    case TYPE_FUNC:
    case TYPE_FUNC_C:
    case TYPE_UND:
    case TYPE_NAN:
    case TYPE_ARRAY:
    case TYPE_BUF:
    case TYPE_ERR:
    case TYPE_DATE:
    case TYPE_OBJ:
    case TYPE_OBJ_F:
    default: val_set_nan(res);
    }
}

void val_op_sub(void *env, val_t *op1, val_t *op2, val_t *res)
{
    switch(val_type(op1)) {
    case TYPE_NUM:   number_sub(op1, op2, res); break;
    case TYPE_STR_I:
    case TYPE_STR_H:
    case TYPE_STR_F:
    case TYPE_BOOL:
    case TYPE_FUNC:
    case TYPE_FUNC_C:
    case TYPE_UND:
    case TYPE_NAN:
    case TYPE_ARRAY:
    case TYPE_BUF:
    case TYPE_ERR:
    case TYPE_DATE:
    case TYPE_OBJ:
    case TYPE_OBJ_F:
    default: val_set_nan(res);
    }
}

void val_op_and(void *env, val_t *op1, val_t *op2, val_t *res)
{
    switch(val_type(op1)) {
    case TYPE_NUM:   number_and(op1, op2, res); break;
    case TYPE_STR_I:
    case TYPE_STR_H:
    case TYPE_STR_F:
    case TYPE_BOOL:
    case TYPE_FUNC:
    case TYPE_FUNC_C:
    case TYPE_UND:
    case TYPE_NAN:
    case TYPE_ARRAY:
    case TYPE_BUF:
    case TYPE_ERR:
    case TYPE_DATE:
    case TYPE_OBJ:
    case TYPE_OBJ_F:
    default: val_set_nan(res);
    }
}

void val_op_or(void *env, val_t *op1, val_t *op2, val_t *res)
{
    switch(val_type(op1)) {
    case TYPE_NUM:   number_or(op1, op2, res); break;
    case TYPE_STR_I:
    case TYPE_STR_H:
    case TYPE_STR_F:
    case TYPE_BOOL:
    case TYPE_FUNC:
    case TYPE_FUNC_C:
    case TYPE_UND:
    case TYPE_NAN:
    case TYPE_ARRAY:
    case TYPE_BUF:
    case TYPE_ERR:
    case TYPE_DATE:
    case TYPE_OBJ:
    case TYPE_OBJ_F:
    default: val_set_nan(res);
    }
}

void val_op_xor(void *env, val_t *op1, val_t *op2, val_t *res)
{
    switch(val_type(op1)) {
    case TYPE_NUM:   number_xor(op1, op2, res); break;
    case TYPE_STR_I:
    case TYPE_STR_H:
    case TYPE_STR_F:
    case TYPE_BOOL:
    case TYPE_FUNC:
    case TYPE_FUNC_C:
    case TYPE_UND:
    case TYPE_NAN:
    case TYPE_ARRAY:
    case TYPE_BUF:
    case TYPE_ERR:
    case TYPE_DATE:
    case TYPE_OBJ:
    case TYPE_OBJ_F:
    default: val_set_nan(res);
    }
}

void val_op_lshift(void *env, val_t *op1, val_t *op2, val_t *res)
{
    switch(val_type(op1)) {
    case TYPE_NUM:   number_lshift(op1, op2, res); break;
    case TYPE_STR_I:
    case TYPE_STR_H:
    case TYPE_STR_F:
    case TYPE_BOOL:
    case TYPE_FUNC:
    case TYPE_FUNC_C:
    case TYPE_UND:
    case TYPE_NAN:
    case TYPE_ARRAY:
    case TYPE_BUF:
    case TYPE_ERR:
    case TYPE_DATE:
    case TYPE_OBJ:
    case TYPE_OBJ_F:
    default: val_set_nan(res);
    }
}

void val_op_rshift(void *env, val_t *op1, val_t *op2, val_t *res)
{
    switch(val_type(op1)) {
    case TYPE_NUM:   number_rshift(op1, op2, res); break;
    case TYPE_STR_I:
    case TYPE_STR_H:
    case TYPE_STR_F:
    case TYPE_BOOL:
    case TYPE_FUNC:
    case TYPE_FUNC_C:
    case TYPE_UND:
    case TYPE_NAN:
    case TYPE_ARRAY:
    case TYPE_BUF:
    case TYPE_ERR:
    case TYPE_DATE:
    case TYPE_OBJ:
    case TYPE_OBJ_F:
    default: val_set_nan(res);
    }
}
