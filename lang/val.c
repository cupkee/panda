
#include "val.h"
#include "number.h"
#include "string.h"

const val_t _Infinity  = TAG_INFINITE;
const val_t _Undefined = TAG_UNDEFINED;
const val_t _True      = TAG_BOOLEAN + 1;
const val_t _False     = TAG_BOOLEAN;
const val_t _NaN       = TAG_NAN;

typedef int (*val_int_fn_val_t)(val_t *);

/*
static const val_int_fn_val_t val_is_true_fns[TYPE_MAX] = {
    [TYPE_NUM]    =
    [TYPE_STR_I]  =
    [TYPE_STR_H]  =
    [TYPE_STR_F]  =
    [TYPE_BOOL]   =
    [TYPE_FUNC]   =
    [TYPE_FUNC_C] =
    [TYPE_UND]    =
    [TYPE_NAN]    =
    [TYPE_ARRAY]  =
    [TYPE_BUF]    =
    [TYPE_ERR]    =
    [TYPE_DATE]   =
    [TYPE_OBJ]    =
    [TYPE_OBJ_F]  =
    [TYPE_REF]    =
};
*/

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
