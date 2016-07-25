
#ifndef __LANG_VAL_INC__
#define __LANG_VAL_INC__

#include "config.h"

typedef uint64_t val_t;
typedef union {
    val_t   v;
    double  d;
} valnum_t;

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
#define MAKE_TAG(s, t) \
  ((val_t)(s) << 63 | (val_t) 0x7ff0 <<48 | (val_t)(t) <<48)

#define TAG_INFINITE        MAKE_TAG(0, 0)
#define TAG_FUNC_SCRIPT     MAKE_TAG(1, 1)
#define TAG_FUNC_NATIVE     MAKE_TAG(1, 2)
#define TAG_UNDEFINED       MAKE_TAG(1, 3)
#define TAG_BOOLEAN         MAKE_TAG(1, 4)
#define TAG_ARRAY           MAKE_TAG(1, 5)
#define TAG_DICTIONARY      MAKE_TAG(1, 6)
#define TAG_OBJECT          MAKE_TAG(1, 7)
#define TAG_NAN             MAKE_TAG(1, 8)
#define TAG_REFERENCE       MAKE_TAG(1, 9)

#define TAG_STRING          MAKE_TAG(1, 0xC)
#define TAG_STRING_I        MAKE_TAG(1, 0xD)  // Inline string (length < 5)
#define TAG_STRING_5        MAKE_TAG(1, 0xE)  // Inline string (length = 5)
#define TAG_STRING_F        MAKE_TAG(1, 0xF)  // Foreign string

#define TAG_MASK            MAKE_TAG(1, 0xF)
#define VAR_MASK            (~MAKE_TAG(1, 0xF))

static inline double val_2_double(val_t v) {
    return ((valnum_t)v).d;
}

static inline int val_2_integer(val_t v) {
    return (int) ((valnum_t)v).d;
}

static inline char * val_2_string(val_t v) {
    return "NotImplemented";
}

static inline intptr_t val_2_intptr(val_t v) {
    return (intptr_t)(v & VAR_MASK);
}

static inline val_t *val_2_reference(val_t v) {
    return ((val_t *)(v & VAR_MASK));
}

static inline val_t double_2_val(double d) {
    return ((valnum_t)d).v;
}

static inline int val_is_number(val_t v) {
    return (v & TAG_INFINITE) != TAG_INFINITE;
}

static inline int val_is_nan(val_t v) {
    return (v & TAG_MASK) == TAG_NAN;
}

static inline int val_is_script(val_t v) {
    return (v & TAG_MASK) == TAG_FUNC_SCRIPT;
}

static inline int val_is_native(val_t v) {
    return (v & TAG_MASK) == TAG_FUNC_NATIVE;
}

static inline int val_is_function(val_t v) {
    return val_is_script(v) || val_is_native(v);
}

static inline int val_is_undefined(val_t v) {
    return (v & TAG_MASK) == TAG_UNDEFINED;
}

static inline int val_is_boolean(val_t v) {
    return (v & TAG_MASK) == TAG_BOOLEAN;
}

static inline int val_is_array(val_t v) {
    return (v & TAG_MASK) == TAG_ARRAY;
}

static inline int val_is_dictionary(val_t v) {
    return (v & TAG_MASK) == TAG_DICTIONARY;
}

static inline int val_is_object(val_t v) {
    return (v & TAG_MASK) == TAG_OBJECT;
}

static inline int val_is_string(val_t v) {
    return (v & TAG_MASK) >= TAG_STRING;
}

static inline int val_is_reference(val_t v) {
    return (v & TAG_MASK) == TAG_REFERENCE;
}

static inline int val_is_true(val_t v) {
    if (val_is_reference(v)) v = *val_2_reference(v);

    return val_is_boolean(v) ? val_2_intptr(v) :
           val_is_number(v) ? val_2_double(v) != 0 :
           //val_is_string(v) ? string_is_true(val_2_string(v)) :
           //val_is_array(v) ? array_is_true(val_2_intptr(v)) :
           //val_is_dictionary(v) ? dictionary_is_true(val_2_intptr(v)) :
           //val_is_object(v) ? object_is_true(val_2_intptr(v)) :
           //val_is_function ? function_is_true(val_2_intptr(v)) :
           0; // "undefined" and "nan", always be false.
}

static inline val_t val_mk_number(double d) {
    return double_2_val(d);
}

static inline val_t val_mk_nan(void) {
    return TAG_NAN;
}

static inline val_t val_mk_undefined(void) {
    return TAG_UNDEFINED;
}

static inline val_t val_mk_script(void *ptr) {
    return TAG_FUNC_SCRIPT | (val_t)ptr;
}

static inline val_t val_mk_native(void *ptr) {
    return TAG_FUNC_NATIVE | (val_t)ptr;
}

static inline val_t val_mk_boolean(int v) {
    return TAG_BOOLEAN | (!!v);
}

static inline val_t val_mk_string(const char *ptr) {
    return TAG_STRING | (val_t) ptr;
}

static inline val_t val_mk_reference(const val_t *ref) {
    return TAG_REFERENCE | (val_t) ref;
}

static inline val_t val_mk_array(void *ptr) {
    return TAG_ARRAY | (val_t) ptr;
}

static inline val_t val_mk_dictionary(void *ptr) {
    return TAG_DICTIONARY | (val_t) ptr;
}

static inline void val_set_nan(val_t *p) {
    *((uint64_t *)p) = TAG_NAN;
}

static inline void val_set_undefined(val_t *p) {
    *((uint64_t *)p) = TAG_UNDEFINED;
}

static inline void val_set_boolean(val_t *p, int b) {
    *((uint64_t *)p) = TAG_BOOLEAN | !!b;
}

static inline void val_set_number(val_t *p, double d) {
    *((double *)p) = d;
}

static inline void val_set_reference(val_t *p, val_t *r) {
    *((uint64_t *)p) = TAG_REFERENCE | (val_t) r;
}

static inline void val_set_string(val_t *p, intptr_t s) {
    *((uint64_t *)p) = TAG_STRING | s;
}

static inline void val_set_script(val_t *p, intptr_t s) {
    *((uint64_t *)p) = TAG_FUNC_SCRIPT | s;
}

static inline void val_set_native(val_t *p, intptr_t f) {
    *((uint64_t *)p) = TAG_FUNC_NATIVE | f;
}

static inline void val_set_array(val_t *p, intptr_t a) {
    *((uint64_t *)p) = TAG_ARRAY | a;
}

static inline void val_set_dictionary(val_t *p, intptr_t d) {
    *((uint64_t *)p) = TAG_DICTIONARY | d;
}

static inline void val_set_object(val_t *p, intptr_t o) {
    *((uint64_t *)p) = TAG_OBJECT | o;
}

static inline val_t val_negation(val_t v) {
    if (val_is_number(v)) {
        return val_mk_number(-val_2_double(v));
    } else {
        return val_mk_nan();
    }
}

static inline val_t val_not(val_t v) {
    if (val_is_number(v)) {
        return val_mk_number(~val_2_integer(v));
    } else {
        return val_mk_nan();
    }
}

static inline val_t val_logic_not(val_t v) {
    return val_mk_boolean(val_is_true(v) ? 0 : 1);
}

static inline val_t val_add(val_t a, val_t b) {
    if (val_is_number(a)) {
        return val_is_number(b) ? val_mk_number(val_2_double(a) + val_2_double(b)) : val_mk_nan();
    } else
    if (val_is_string(a)) {
        //return val_mk_string(string_concat(val_2_string(a), val_is_string(b) ?
        //            val_2_string(b) : val_cast_string(b)));
    }

    return val_mk_nan();
}

static inline val_t val_sub(val_t a, val_t b) {
    if (val_is_number(a) && val_is_number(b)) {
        return val_mk_number(val_2_double(a) - val_2_double(b));
    }

    return val_mk_nan();
}

static inline val_t val_mul(val_t a, val_t b) {
    if (val_is_number(a) && val_is_number(b)) {
        return val_mk_number(val_2_double(a) * val_2_double(b));
    } else
    if (val_is_string(a) && val_is_number(b)) {
        //return val_mk_string(string_concat2(val_2_string(a), val_2_integer(b));
    }

    return val_mk_nan();
}

static inline val_t val_div(val_t a, val_t b) {
    if (val_is_number(a) && val_is_number(b)) {
        return val_2_double(b) == 0 ? val_mk_nan() : val_mk_number(val_2_double(a) / val_2_double(b));
    } else
    if (val_is_string(a) && val_is_number(b)) {
        //return val_mk_string(string_cut(val_2_string(a), val_2_integer(b));
    }

    return val_mk_nan();
}

static inline val_t val_mod(val_t a, val_t b) {
    if (val_is_number(a) && val_is_number(b)) {
        return val_2_double(b) == 0 ? val_mk_nan() : val_mk_number(val_2_integer(a) % val_2_integer(b));
    }
    return val_mk_nan();
}

static inline val_t val_and(val_t a, val_t b) {
    if (val_is_number(a) && val_is_number(b)) {
        return val_mk_number(val_2_integer(a) & val_2_integer(b));
    }
    return val_mk_nan();
}

static inline val_t val_or (val_t a, val_t b) {
    if (val_is_number(a) && val_is_number(b)) {
        return val_mk_number(val_2_integer(a) | val_2_integer(b));
    }
    return val_mk_nan();
}

static inline val_t val_xor(val_t a, val_t b) {
    if (val_is_number(a) && val_is_number(b)) {
        return val_mk_number(val_2_integer(a) ^ val_2_integer(b));
    }
    return val_mk_nan();
}

static inline val_t val_neg(val_t a) {
    return val_is_number(a) ? val_mk_number(~val_2_integer(a)) : val_mk_nan();
}

static inline val_t val_lshift(val_t a, val_t b) {
    if (val_is_number(a) && val_is_number(b)) {
        return val_mk_number(val_2_integer(a) << val_2_integer(b));
    }
    return val_mk_nan();
}

static inline val_t val_rshift(val_t a, val_t b) {
    if (val_is_number(a) && val_is_number(b)) {
        return val_mk_number(val_2_integer(a) >> val_2_integer(b));
    }
    return val_mk_nan();
}

static inline int val_teq(val_t a, val_t b) {
    if (val_is_number(a)) {
        if (val_is_number(b))
            return (val_2_double(a) == val_2_double(b));
    } else
    if ((a & TAG_MASK) == (b & TAG_MASK)) {
        if (val_is_boolean(a))
            return (val_2_intptr(a) == val_2_intptr(b));
        /*
        if (val_is_string(a))
            return (!string_compare(val_2_string(a), val_2_string(b)));
        if (val_is_array(a))
            return (!array_compare(val_2_intptr(a), val_2_intptr(b)));
        if (val_is_dictionary(a))
            return (!dictionary_compare(val_2_intptr(a), val_2_intptr(b)));
        if (val_is_object(a))
            return (!object_compare(val_2_intptr(a), val_2_intptr(b)));
        */
    }
    return 0;
}

static inline int val_tgt(val_t a, val_t b) {
    if (val_is_number(a) && val_is_number(b)) {
        return (val_2_double(a) > val_2_double(b));
    }
    return 0;
}

static inline int val_tge(val_t a, val_t b) {
    if (val_is_number(a) && val_is_number(b)) {
        return (val_2_double(a) >= val_2_double(b));
    }
    return 0;
}

static inline val_t val_tlt(val_t a, val_t b) {
    if (val_is_number(a) && val_is_number(b)) {
        return (val_2_double(a) < val_2_double(b));
    }
    return 0;
}

static inline val_t val_tle(val_t a, val_t b) {
    if (val_is_number(a) && val_is_number(b)) {
        return (val_2_double(a) <= val_2_double(b));
    }
    return 0;
}

#endif /* __LANG_VALUE_INC__ */

