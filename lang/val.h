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

#ifndef __LANG_VAL_INC__
#define __LANG_VAL_INC__

#include "def.h"

typedef union valnum_t {
    val_t   v;
    double  d;
} valnum_t;

typedef void (*val_opx_t)(void *env, val_t *x, val_t *result);
typedef void (*val_opxx_t)(void *env, val_t *x1, val_t *x2, val_t *result);

extern const valnum_t const_nan;

/*
 *  Double-precision floating-point number, IEEE 754
 *
 *  64 bit (8 bytes) in total
 *  1  bit sign
 *  11 bits exponent
 *  52 bits mantissa
 *      7         6        5        4        3        2        1        0
 *  seeeeeee|eeeemmmm|mmmmmmmm|mmmmmmmm|mmmmmmmm|mmmmmmmm|mmmmmmmm|mmmmmmmm
 *
 * If an exponent is all-1 and mantissa is all-0, then it is an INFINITY:
 *  11111111|11110000|00000000|00000000|00000000|00000000|00000000|00000000
 *
 * If an exponent is all-1 and mantissa's MSB is 1, it is a quiet NaN:
 *  11111111|11111000|00000000|00000000|00000000|00000000|00000000|00000000
 *
 * object_t NaN-packing:
 *    sign and exponent is 0xfff
 *    4 bits specify type (tag), must be non-zero
 *    48 bits specify value
 *
 *  11111111|1111tttt|vvvvvvvv|vvvvvvvv|vvvvvvvv|vvvvvvvv|vvvvvvvv|vvvvvvvv
 *   NaN marker |type|  48-bit placeholder for values: pointers, strings
 *
 * On 64-bit platforms, pointers are really 48 bit only, so they can fit,
 * provided they are sign extended
 */
#define MAKE_TAG(s, t)  \
  ((val_t)(s) << 63 | (val_t) 0x7ff0 <<48 | (val_t)(t) <<48)

#define TAG_UINT16(t)   \
  ((uint16_t) (0xfff0 | t))

#define TYPE_NUM            0       // number or infinity
#define TYPE_STR_I          1       // string (inner/inline)
#define TYPE_STR_H          2       // string (heap)
#define TYPE_STR_F          3       // string (foreign)
#define TYPE_BOOL           4       // boolean
#define TYPE_FUNC           5       // function script
#define TYPE_FUNC_C         6       // function c
#define TYPE_UND            7       // undefined
#define TYPE_NAN            8       // not a number
#define TYPE_DATE           9       // date
#define TYPE_ARRAY_BUF      10      // array buffer
#define TYPE_DATA_VIEW      11      // view of data
#define TYPE_ARRAY          12      // array
#define TYPE_OBJ            13      // object
#define TYPE_FOREIGN        14      // object (foreign)
#define TYPE_REF            15      // reference to variable
#define TYPE_MAX            16

#define TAG_INFINITE        MAKE_TAG(0, TYPE_NUM)
#define TAG_STRING_I        MAKE_TAG(1, TYPE_STR_I)
#define TAG_STRING_H        MAKE_TAG(1, TYPE_STR_H)
#define TAG_STRING_F        MAKE_TAG(1, TYPE_STR_F)
#define TAG_BOOLEAN         MAKE_TAG(1, TYPE_BOOL)
#define TAG_FUNC_SCRIPT     MAKE_TAG(1, TYPE_FUNC)
#define TAG_FUNC_NATIVE     MAKE_TAG(1, TYPE_FUNC_C)

#define TAG_UNDEFINED       MAKE_TAG(1, TYPE_UND)
#define TAG_NAN             MAKE_TAG(0, TYPE_NAN)
#define TAG_ARR_BUF         MAKE_TAG(1, TYPE_ARRAY_BUF)
#define TAG_VIEW            MAKE_TAG(1, TYPE_DATA_VIEW)
#define TAG_DATE            MAKE_TAG(0, TYPE_DATE)

#define TAG_ARRAY           MAKE_TAG(1, TYPE_ARRAY)
#define TAG_OBJECT          MAKE_TAG(1, TYPE_OBJ)
#define TAG_FOREIGN         MAKE_TAG(1, TYPE_FOREIGN)
#define TAG_REFERENCE       MAKE_TAG(1, TYPE_REF)

#define NUM_MASK            MAKE_TAG(0, 0xF)
#define TAG_MASK            MAKE_TAG(1, 0xF)
#define VAL_MASK            (~MAKE_TAG(1, 0xF))

#define VAL_UNDEFINED       TAG_UNDEFINED
#define VAL_NULL            TAG_OBJECT
#define VAL_INFINITE        TAG_INFINITE
#define VAL_NAN             TAG_NAN
#define VAL_TRUE            (TAG_BOOLEAN | 1)
#define VAL_FALSE           (TAG_BOOLEAN)

#define MAGIC_FOREIGN       (MAGIC_BASE + 15)

typedef struct val_metadata_t {
    const char *name;   // Type name of value

    int (*is_true)  (val_t *self);
    int (*is_equal) (val_t *self, val_t *to);

    double (*value_of)(val_t *self);

    val_t  (*get_prop)(void *env, val_t *self, const char *key);
    val_t  (*get_elem)(void *env, val_t *self, int id);

    void   (*set_prop)(void *env, val_t *self, const char *key, val_t *data);
    void   (*set_elem)(void *env, val_t *self, int id, val_t *data);

    void   (*opx_prop)(void *env, val_t *self, const char *key, val_t *res, val_opx_t op);
    void   (*opx_elem)(void *env, val_t *self, int id,          val_t *res, val_opx_t op);

    void   (*opxx_prop)(void *env, val_t *self, const char *key, val_t *data, val_t *res, val_opxx_t op);
    void   (*opxx_elem)(void *env, val_t *self, int id,          val_t *data, val_t *res, val_opxx_t op);

    int (*concat)(void *env, val_t *self, val_t *other, val_t *to);
} val_metadata_t;

static inline int val_type(val_t *v) {
    int type = (*v) >> 48;

    return ((type & 0x7ff0) != 0x7ff0) ? TYPE_NUM : type & 0xf;
}

static inline double val_2_double(val_t *v) {
    return ((valnum_t*)v)->d;
}

static inline int val_2_integer(val_t *v) {
    return (int) (((valnum_t*)v)->d);
}

static inline intptr_t val_2_intptr(val_t *v) {
    return (intptr_t)(*v & VAL_MASK);
}

static inline void val_2_reference(val_t *v, uint8_t *id, uint8_t *generation) {
    uint8_t *p = (uint8_t *)v;
#if SYS_BYTE_ORDER == LE
    *id = p[1]; *generation = p[0];
#else
    *id = p[6]; *generation = p[7];
#endif
}

#define double_2_val(d) (((valnum_t*)&(d))->v)

static inline int val_is_infinity(val_t *v) {
    return (*v & NUM_MASK) == TAG_INFINITE;
}

static inline int val_is_number(val_t *v) {
    return (*v & TAG_INFINITE) != TAG_INFINITE;
}

static inline int val_is_not_number(val_t *v) {
    return (*v & TAG_INFINITE) == TAG_INFINITE;
}

static inline int val_is_inline_string(val_t *v) {
    return (*v & TAG_MASK) == TAG_STRING_I;
}

static inline int val_is_heap_string(val_t *v) {
    return (*v & TAG_MASK) == TAG_STRING_H;
}

static inline int val_is_foreign_string(val_t *v) {
    return (*v & TAG_MASK) == TAG_STRING_F;
}

static inline int val_is_string(val_t *v) {
    uint16_t tag = ((*v) >> 48);

    return tag < TAG_UINT16(TYPE_STR_I) ? 0 : tag > TAG_UINT16(TYPE_STR_F) ? 0 : 1;
}

static inline int val_is_boolean(val_t *v) {
    return (*v & TAG_MASK) == TAG_BOOLEAN;
}

static inline int val_is_script(val_t *v) {
    return (*v & TAG_MASK) == TAG_FUNC_SCRIPT;
}

static inline int val_is_native(val_t *v) {
    return (*v & TAG_MASK) == TAG_FUNC_NATIVE;
}

static inline int val_is_function(val_t *v) {
    uint16_t tag = ((*v) >> 48);
    return tag == TAG_UINT16(TYPE_FUNC) || tag == TAG_UINT16(TYPE_FUNC_C);
}

static inline int val_is_undefined(val_t *v) {
    return (*v & TAG_MASK) == TAG_UNDEFINED;
}

static inline int val_is_nan(val_t *v) {
    return (*v & NUM_MASK) == TAG_NAN;
}

static inline int val_is_array(val_t *v) {
    return (*v & TAG_MASK) == TAG_ARRAY;
}

static inline int val_is_object(val_t *v) {
    return (*v & TAG_MASK) == TAG_OBJECT;
}

static inline int val_is_null(val_t *v) {
    return *v == TAG_OBJECT;
}

static inline int val_is_foreign(val_t *v) {
    return (*v & TAG_MASK) == TAG_FOREIGN;
}

static inline int val_is_reference(val_t *v) {
    return (*v & TAG_MASK) == TAG_REFERENCE;
}

static inline const char *val_2_cstring(val_t *v) {
    uint64_t t = *v & TAG_MASK;

    if (t == TAG_STRING_I) {
        return ((const char *)v) + 4;
    } else
    if (t == TAG_STRING_F) {
        return (const char *) val_2_intptr(v);
    } else
    if (t == TAG_STRING_H) {
        return (const char *) (val_2_intptr(v) + 4);
    } else {
        return NULL;
    }
}

static inline val_t val_mk_number(double d) {
    return double_2_val(d);
}

static inline val_t val_mk_inner_string(unsigned c) {
#if SYS_BYTE_ORDER == LE
    return TAG_STRING_I | (c * 0x100000000);
#else
    return TAG_STRING_I | (c * 0x1000000);
#endif
}

static inline val_t val_mk_foreign_string(intptr_t s) {
    return TAG_STRING_F | s;
}

static inline val_t val_mk_heap_string(intptr_t s) {
    return TAG_STRING_H | s;
}

static inline val_t val_mk_boolean(int v) {
    return TAG_BOOLEAN | (!!v);
}

static inline val_t val_mk_script(intptr_t s) {
    return TAG_FUNC_SCRIPT | s;
}

static inline val_t val_mk_native(intptr_t n) {
    return TAG_FUNC_NATIVE | n;
}

static inline val_t val_mk_nan(void) {
    return TAG_NAN;
}

static inline val_t val_mk_undefined(void) {
    return TAG_UNDEFINED;
}

static inline val_t val_mk_array(void *ptr) {
    return TAG_ARRAY | (intptr_t) ptr;
}

static inline val_t val_mk_foreign(intptr_t f) {
    return TAG_FOREIGN | f;
}

static inline val_t val_mk_null(void) {
    return TAG_OBJECT;
}

static inline void val_set_boolean(val_t *p, int b) {
    *((uint64_t *)p) = TAG_BOOLEAN | !!b;
}

static inline void val_set_number(val_t *p, double d) {
    *((double *)p) = d;
}

static inline void val_set_foreign_string(val_t *p, intptr_t s) {
    *((uint64_t *)p) = TAG_STRING_F | s;
}

static inline void val_set_heap_string(val_t *p, intptr_t s) {
    *((uint64_t *)p) = TAG_STRING_H | s;
}

static inline void val_set_inner_string(val_t *p, char c) {
    *((uint64_t *)p) = TAG_STRING_I;
    *(((char *)p) + 4) = c;
}

static inline void val_set_script(val_t *p, intptr_t s) {
    *((uint64_t *)p) = TAG_FUNC_SCRIPT | s;
}

static inline void val_set_native(val_t *p, intptr_t f) {
    *((uint64_t *)p) = TAG_FUNC_NATIVE | f;
}

static inline void val_set_nan(val_t *p) {
    *((uint64_t *)p) = TAG_NAN;
}

static inline void val_set_undefined(val_t *p) {
    *((uint64_t *)p) = TAG_UNDEFINED;
}

static inline void val_set_array(val_t *p, intptr_t a) {
    *((uint64_t *)p) = TAG_ARRAY | a;
}

static inline void val_set_object(val_t *p, intptr_t d) {
    *((uint64_t *)p) = TAG_OBJECT | d;
}

static inline void val_set_foreign(val_t *p, intptr_t f) {
    *((uint64_t *)p) = TAG_FOREIGN | f;
}

static inline void val_set_reference(val_t *p, uint8_t id, uint8_t generation) {
    *((uint64_t *)p) = TAG_REFERENCE | id * 256 | generation;
}

int val_as_true(val_t *self);
int val_as_false(val_t *self);
int val_op_true(val_t *self, val_t *to);
int val_op_false(val_t *self, val_t *to);

double val_as_zero(val_t *self);
double val_as_nan(val_t *self);
double val_as_integer(val_t *self);
double val_as_number(val_t *self);

int  val_is_true(val_t *self);
int  val_is_equal(val_t *self, val_t *other);
int  val_is_gt(val_t *self, val_t *other);
int  val_is_ge(val_t *self, val_t *other);
int  val_is_lt(val_t *self, val_t *other);
int  val_is_le(val_t *self, val_t *other);

void val_neg(void *env, val_t *self, val_t *res);
void val_not(void *env, val_t *self, val_t *res);

void val_mul(void *env, val_t *self, val_t *b, val_t *r);
void val_div(void *env, val_t *self, val_t *b, val_t *r);
void val_mod(void *env, val_t *self, val_t *b, val_t *r);
void val_add(void *env, val_t *self, val_t *b, val_t *r);
void val_sub(void *env, val_t *self, val_t *b, val_t *r);
void val_and(void *env, val_t *self, val_t *b, val_t *r);
void val_or (void *env, val_t *self, val_t *b, val_t *r);
void val_xor(void *env, val_t *self, val_t *b, val_t *r);
void val_lshift(void *env, val_t *self, val_t *b, val_t *r);
void val_rshift(void *env, val_t *self, val_t *b, val_t *r);

void val_inc (void *env, val_t *self, val_t *res);
void val_incp(void *env, val_t *self, val_t *res);
void val_dec (void *env, val_t *self, val_t *res);
void val_decp(void *env, val_t *self, val_t *res);

void val_set(void *env, val_t *self, val_t *b, val_t *r);

void val_prop_get(void *env, val_t *self, val_t *key, val_t *prop);
void val_prop_set(void *env, val_t *self, val_t *key, val_t *data);
void val_prop_opx(void *env, val_t *self, val_t *key, val_t *res, val_opx_t op);
void val_prop_opxx(void *env, val_t *self, val_t *key, val_t *data, val_t *res, val_opxx_t op);

#endif /* __LANG_VALUE_INC__ */

