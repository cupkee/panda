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

#include <math.h>
#include "val.h"
#include "env.h"
#include "types.h"
#include "type_boolean.h"
#include "type_number.h"
#include "type_string.h"
#include "type_array.h"
#include "type_object.h"
#include "type_function.h"

const valnum_t const_nan = {
    .v = TAG_NAN
};

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
    if (val_is_number(self)) {
        if (val_is_number(to)) {
            return val_2_double(self) > val_2_double(to);
        } else {
            const val_metadata_t *meta = base_metadata[val_type(to)];
            return val_2_double(self) > meta->value_of(to);
        }
    } else {
        const char *s = val_2_cstring(self);
        if (s) {
            const char *b = val_2_cstring(to);

            return b ? strcmp(s, b) > 0 : 0;
        } else {
            const val_metadata_t *meta_a = base_metadata[val_type(self)];
            const val_metadata_t *meta_b = base_metadata[val_type(to)];

            return meta_a->value_of(self) > meta_b->value_of(to);
        }
    }
}

int val_is_ge(val_t *self, val_t *to)
{
    if (val_is_number(self)) {
        if (val_is_number(to)) {
            return val_2_double(self) >= val_2_double(to);
        } else {
            const val_metadata_t *meta = base_metadata[val_type(to)];
            return val_2_double(self) >= meta->value_of(to);
        }
    } else {
        const char *s = val_2_cstring(self);
        if (s) {
            const char *b = val_2_cstring(to);

            return b ? strcmp(s, b) >= 0 : 0;
        } else {
            const val_metadata_t *meta_a = base_metadata[val_type(self)];
            const val_metadata_t *meta_b = base_metadata[val_type(to)];

            return meta_a->value_of(self) >= meta_b->value_of(to);
        }
    }
}

int val_is_le(val_t *self, val_t *to)
{
    if (val_is_number(self)) {
        if (val_is_number(to)) {
            return val_2_double(self) <= val_2_double(to);
        } else {
            const val_metadata_t *meta = base_metadata[val_type(to)];
            return val_2_double(self) <= meta->value_of(to);
        }
    } else {
        const char *s = val_2_cstring(self);
        if (s) {
            const char *b = val_2_cstring(to);

            return b ? strcmp(s, b) <= 0 : 0;
        } else {
            const val_metadata_t *meta_a = base_metadata[val_type(self)];
            const val_metadata_t *meta_b = base_metadata[val_type(to)];

            return meta_a->value_of(self) <= meta_b->value_of(to);
        }
    }
}

int val_is_lt(val_t *self, val_t *to)
{
    if (val_is_number(self)) {
        if (val_is_number(to)) {
            return val_2_double(self) < val_2_double(to);
        } else {
            const val_metadata_t *meta = base_metadata[val_type(to)];
            return val_2_double(self) < meta->value_of(to);
        }
    } else {
        const char *s = val_2_cstring(self);
        if (s) {
            const char *b = val_2_cstring(to);

            return b ? strcmp(s, b) < 0 : 0;
        } else {
            const val_metadata_t *meta_a = base_metadata[val_type(self)];
            const val_metadata_t *meta_b = base_metadata[val_type(to)];

            return meta_a->value_of(self) < meta_b->value_of(to);
        }
    }
}

void val_neg(void *env, val_t *self, val_t *result)
{
    (void) env;

    if (val_is_not_number(self)) {
        const val_metadata_t *meta = base_metadata[val_type(self)];

        val_set_number(result, -meta->value_of(self));
    } else {
        val_set_number(result, -val_2_double(self));
    }
}

void val_not(void *env, val_t *self, val_t *result)
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

void val_inc(void *env, val_t *self, val_t *res)
{
    (void) env;
    if (val_is_number(self)) {
        ((valnum_t *)res)->d = ((valnum_t *)self)->d++;
    } else {
        val_set_nan(res);
    }
}

void val_incp(void *env, val_t *self, val_t *res)
{
    (void) env;
    if (val_is_number(self)) {
        ((valnum_t *)res)->d = ++((valnum_t *)self)->d;
    } else {
        val_set_nan(res);
    }
}

void val_dec(void *env, val_t *self, val_t *res)
{
    (void) env;
    if (val_is_number(self)) {
        ((valnum_t *)res)->d = ((valnum_t *)self)->d--;
    } else {
        val_set_nan(res);
    }
}

void val_decp(void *env, val_t *self, val_t *res)
{
    (void) env;
    if (val_is_number(self)) {
        ((valnum_t *)res)->d = --((valnum_t *)self)->d;
    } else {
        val_set_nan(res);
    }
}

void val_mul(void *env, val_t *a, val_t *b, val_t *res)
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

void val_div(void *env, val_t *a, val_t *b, val_t *res)
{
    const val_metadata_t *meta_a = base_metadata[val_type(a)];
    const val_metadata_t *meta_b = base_metadata[val_type(b)];

    ((valnum_t *)res)->d = meta_a->value_of(a) / meta_b->value_of(b);

    (void) env;
}

void val_mod(void *env, val_t *a, val_t *b, val_t *res)
{
    const val_metadata_t *meta_a = base_metadata[val_type(a)];
    const val_metadata_t *meta_b = base_metadata[val_type(b)];

    ((valnum_t *)res)->d = fmod(meta_a->value_of(a), meta_b->value_of(b));

    (void) env;
}

void val_add(void *env, val_t *a, val_t *b, val_t *res)
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

void val_sub(void *env, val_t *a, val_t *b, val_t *res)
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

void val_and(void *env, val_t *a, val_t *b, val_t *res)
{
    int ia = val_is_number(a) ? val_2_integer(a) : val_is_boolean(a) ? val_2_intptr(a) : 0;
    int ib = val_is_number(b) ? val_2_integer(b) : val_is_boolean(b) ? val_2_intptr(b) : 0;

    (void) env;

    val_set_number(res, ia & ib);
}

void val_or(void *env, val_t *a, val_t *b, val_t *res)
{
    int ia = val_is_number(a) ? val_2_integer(a) : val_is_boolean(a) ? val_2_intptr(a) : 0;
    int ib = val_is_number(b) ? val_2_integer(b) : val_is_boolean(b) ? val_2_intptr(b) : 0;

    (void) env;

    val_set_number(res, ia | ib);
}

void val_xor(void *env, val_t *a, val_t *b, val_t *res)
{
    int ia = val_is_number(a) ? val_2_integer(a) : val_is_boolean(a) ? val_2_intptr(a) : 0;
    int ib = val_is_number(b) ? val_2_integer(b) : val_is_boolean(b) ? val_2_intptr(b) : 0;

    (void) env;

    val_set_number(res, ia ^ ib);
}

void val_lshift(void *env, val_t *a, val_t *b, val_t *res)
{
    int ia = val_is_number(a) ? val_2_integer(a) : val_is_boolean(a) ? val_2_intptr(a) : 0;
    int ib = val_is_number(b) ? val_2_integer(b) : val_is_boolean(b) ? val_2_intptr(b) : 0;

    (void) env;

    val_set_number(res, ia << ib);
}

void val_rshift(void *env, val_t *a, val_t *b, val_t *res)
{
    int ia = val_is_number(a) ? val_2_integer(a) : val_is_boolean(a) ? val_2_intptr(a) : 0;
    int ib = val_is_number(b) ? val_2_integer(b) : val_is_boolean(b) ? val_2_intptr(b) : 0;

    (void) env;

    val_set_number(res, ia >> ib);
}

void val_set(void *env, val_t *a, val_t *b, val_t *r)
{
    if (!val_is_foreign(a)) {
        *r = *a = *b;
    } else {
        foreign_set(env, a, b, r);
    }
}

void val_prop_get(void *env, val_t *self, val_t * key, val_t *prop)
{
    const val_metadata_t *meta = base_metadata[val_type(self)];

    if (val_is_number(key)) {
        if (meta->get_elem) {
            *prop = meta->get_elem(env, self, val_2_integer(key));
        } else {
            val_set_undefined(prop);
        }
    } else {
        const char *name = val_2_cstring(key);

        if (name && meta->get_prop) {
            *prop = meta->get_prop(env, self, name);
        } else {
            val_set_undefined(prop);
        }
    }
}

void val_prop_set(void *env, val_t *self, val_t *key, val_t *data)
{
    const val_metadata_t *meta = base_metadata[val_type(self)];

    if (val_is_number(key)) {
        if (meta->set_elem) {
            meta->set_elem(env, self, val_2_integer(key), data);
        }
    } else {
        const char *name = val_2_cstring(key);

        if (name && meta->set_prop) {
            meta->set_prop(env, self, name, data);
        }
    }
}

void val_prop_opx(void *env, val_t *self, val_t * key, val_t *res, val_opx_t op)
{
    const val_metadata_t *meta = base_metadata[val_type(self)];

    if (val_is_number(key)) {
        if (meta->opx_elem) {
            meta->opx_elem(env, self, val_2_integer(key), res, op);
        } else {
            val_set_nan(res);
        }
    } else {
        const char *name = val_2_cstring(key);

        if (name && meta->opx_prop) {
            meta->opx_prop(env, self, name, res, op);
        } else {
            val_set_nan(res);
        }
    }
}

void val_prop_opxx(void *env, val_t *self, val_t * key, val_t *data, val_t *res, val_opxx_t op)
{
    const val_metadata_t *meta = base_metadata[val_type(self)];

    if (val_is_number(key)) {
        if (meta->opxx_elem) {
            meta->opxx_elem(env, self, val_2_integer(key), data, res, op);
        } else {
            val_set_nan(res);
        }
    } else {
        const char *name = val_2_cstring(key);

        if (name && meta->opxx_prop) {
            meta->opxx_prop(env, self, name, data, res, op);
        } else {
            val_set_nan(res);
        }
    }
}

