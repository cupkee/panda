
#include <stdio.h>
#include <string.h>

#include "cunit/CUnit.h"
#include "cunit/CUnit_Basic.h"

#include "lang/val.h"

static int test_setup()
{
    return 0;
}

static int test_clean()
{
    return 0;
}

static void test_val_make(void)
{
    val_t v;

    v = val_mk_number(1.1);
    CU_ASSERT(val_is_number(v));
    CU_ASSERT(!val_is_boolean(v));
    CU_ASSERT(!val_is_undefined(v));
    CU_ASSERT(!val_is_function(v));
    CU_ASSERT(!val_is_string(v));
    CU_ASSERT(!val_is_array(v));
    CU_ASSERT(!val_is_dictionary(v));
    CU_ASSERT(!val_is_object(v));
    CU_ASSERT(!val_is_nan(v));
    CU_ASSERT(1 == val_2_integer(v));
    CU_ASSERT(1.1 == val_2_double(v));
    CU_ASSERT(val_is_true(v));
    CU_ASSERT(!val_is_true(val_mk_number(0.0)));
    CU_ASSERT(!val_is_true(val_mk_number(-0.0)));
    CU_ASSERT(val_is_true(val_mk_number(0.000000001)));

    v = val_mk_undefined();
    CU_ASSERT(!val_is_number(v));
    CU_ASSERT(!val_is_boolean(v));
    CU_ASSERT(val_is_undefined(v));
    CU_ASSERT(!val_is_function(v));
    CU_ASSERT(!val_is_string(v));
    CU_ASSERT(!val_is_array(v));
    CU_ASSERT(!val_is_dictionary(v));
    CU_ASSERT(!val_is_object(v));
    CU_ASSERT(!val_is_nan(v));
    CU_ASSERT(!val_is_true(v));

    v = val_mk_nan();
    CU_ASSERT(!val_is_number(v));
    CU_ASSERT(!val_is_boolean(v));
    CU_ASSERT(!val_is_undefined(v));
    CU_ASSERT(!val_is_function(v));
    CU_ASSERT(!val_is_string(v));
    CU_ASSERT(!val_is_array(v));
    CU_ASSERT(!val_is_dictionary(v));
    CU_ASSERT(!val_is_object(v));
    CU_ASSERT(val_is_nan(v));
    CU_ASSERT(!val_is_true(v));

    v = val_mk_boolean(1);
    CU_ASSERT(!val_is_number(v));
    CU_ASSERT(!val_is_nan(v));
    CU_ASSERT(val_is_boolean(v));
    CU_ASSERT(!val_is_undefined(v));
    CU_ASSERT(!val_is_function(v));
    CU_ASSERT(!val_is_string(v));
    CU_ASSERT(!val_is_array(v));
    CU_ASSERT(!val_is_dictionary(v));
    CU_ASSERT(!val_is_object(v));
    CU_ASSERT(val_is_true(v));
    CU_ASSERT(!val_is_true(val_mk_boolean(0)));
}

static void test_val_set(void)
{
    val_t v;

    val_set_number(&v, 1.1);
    CU_ASSERT(v == val_mk_number(1.1));

    val_set_undefined(&v);
    CU_ASSERT(v == val_mk_undefined());

    val_set_nan(&v);
    CU_ASSERT(v == val_mk_nan());

    val_set_boolean(&v, 1);
    CU_ASSERT(v == val_mk_boolean(1));
}

static void test_val_add(void)
{
    CU_ASSERT(0   == val_2_double(val_add(val_mk_number(0), val_mk_number(0))));
    CU_ASSERT(1   == val_2_double(val_add(val_mk_number(1), val_mk_number(0))));
    CU_ASSERT(1   == val_2_double(val_add(val_mk_number(0), val_mk_number(1))));
    CU_ASSERT(1.1 == val_2_double(val_add(val_mk_number(1), val_mk_number(0.1))));
    CU_ASSERT(8   == val_2_double(val_add(val_mk_number(1), val_mk_number(7))));
    CU_ASSERT(0   == val_2_double(val_add(val_mk_number(-1.2), val_mk_number(1.2))));
    CU_ASSERT(1   == val_2_double(val_add(val_mk_number(0.25), val_mk_number(0.75))));

    CU_ASSERT(val_is_nan(val_add(val_mk_nan(), val_mk_nan())));

    CU_ASSERT(val_is_nan(val_add(val_mk_number(1), val_mk_nan())));
    CU_ASSERT(val_is_nan(val_add(val_mk_number(1), val_mk_undefined())));
    CU_ASSERT(val_is_nan(val_add(val_mk_number(1), val_mk_boolean(0))));
    CU_ASSERT(val_is_nan(val_add(val_mk_number(1), val_mk_boolean(1))));
}

static void test_val_sub(void)
{
    CU_ASSERT(0  == val_2_double(val_sub(val_mk_number(0), val_mk_number(0))));
    CU_ASSERT(1  == val_2_double(val_sub(val_mk_number(1), val_mk_number(0))));
    CU_ASSERT(-1 == val_2_double(val_sub(val_mk_number(0), val_mk_number(1))));
    CU_ASSERT(0.9 == val_2_double(val_sub(val_mk_number(1), val_mk_number(0.1))));
    CU_ASSERT(-0.9 == val_2_double(val_sub(val_mk_number(0.1), val_mk_number(1))));

    CU_ASSERT(val_is_nan(val_sub(val_mk_nan(), val_mk_nan())));

    CU_ASSERT(val_is_nan(val_sub(val_mk_number(1), val_mk_nan())));
    CU_ASSERT(val_is_nan(val_sub(val_mk_number(1), val_mk_undefined())));
    CU_ASSERT(val_is_nan(val_sub(val_mk_number(1), val_mk_boolean(0))));
    CU_ASSERT(val_is_nan(val_sub(val_mk_number(1), val_mk_boolean(1))));
}

static void test_val_mul(void)
{
    CU_ASSERT(0  == val_2_double(val_mul(val_mk_number(0), val_mk_number(0))));
    CU_ASSERT(0  == val_2_double(val_mul(val_mk_number(1), val_mk_number(0))));
    CU_ASSERT(0 == val_2_double(val_mul(val_mk_number(0), val_mk_number(1))));
    CU_ASSERT(10 == val_2_double(val_mul(val_mk_number(5), val_mk_number(2))));
    CU_ASSERT(10 == val_2_double(val_mul(val_mk_number(2), val_mk_number(5))));
    CU_ASSERT(1 * 0.1 == val_2_double(val_mul(val_mk_number(1), val_mk_number(0.1))));
    CU_ASSERT(10.0 * 10.0 == val_2_double(val_mul(val_mk_number(10), val_mk_number(10))));
    CU_ASSERT(0.1 * 0.1 == val_2_double(val_mul(val_mk_number(0.1), val_mk_number(0.1))));

    CU_ASSERT(val_is_nan(val_mul(val_mk_number(1), val_mk_nan())));
    CU_ASSERT(val_is_nan(val_mul(val_mk_number(1), val_mk_undefined())));
    CU_ASSERT(val_is_nan(val_mul(val_mk_number(1), val_mk_boolean(0))));
    CU_ASSERT(val_is_nan(val_mul(val_mk_number(1), val_mk_boolean(1))));

    CU_ASSERT(val_is_nan(val_mul(val_mk_nan(), val_mk_number(1))));
    CU_ASSERT(val_is_nan(val_mul(val_mk_nan(), val_mk_nan())));
}

static void test_val_div(void)
{
    CU_ASSERT(val_is_nan(val_div(val_mk_number(0), val_mk_boolean(0))));
    CU_ASSERT(val_is_nan(val_div(val_mk_number(1), val_mk_boolean(0))));
    CU_ASSERT(0  == val_2_double(val_div(val_mk_number(0), val_mk_number(1))));
    CU_ASSERT(10/2 == val_2_double(val_div(val_mk_number(10), val_mk_number(2))));
    CU_ASSERT(10.0/2.0 == val_2_double(val_div(val_mk_number(10), val_mk_number(2))));

    CU_ASSERT(val_is_nan(val_div(val_mk_number(1), val_mk_nan())));
    CU_ASSERT(val_is_nan(val_div(val_mk_number(1), val_mk_undefined())));
    CU_ASSERT(val_is_nan(val_div(val_mk_number(1), val_mk_boolean(0))));
    CU_ASSERT(val_is_nan(val_div(val_mk_number(1), val_mk_boolean(1))));

    CU_ASSERT(val_is_nan(val_div(val_mk_nan(), val_mk_number(1))));
    CU_ASSERT(val_is_nan(val_div(val_mk_nan(), val_mk_nan())));
}

static void test_val_mod(void)
{
    CU_ASSERT(val_is_nan(val_mod(val_mk_number(0), val_mk_boolean(0))));
    CU_ASSERT(val_is_nan(val_mod(val_mk_number(1), val_mk_boolean(0))));
    CU_ASSERT(0  == val_2_double(val_mod(val_mk_number(0), val_mk_number(1))));
    CU_ASSERT(10%3 == val_2_double(val_mod(val_mk_number(10), val_mk_number(3))));
    CU_ASSERT(10%3 == val_2_double(val_mod(val_mk_number(10.0), val_mk_number(3.0))));

    CU_ASSERT(val_is_nan(val_mod(val_mk_number(1), val_mk_nan())));
    CU_ASSERT(val_is_nan(val_mod(val_mk_number(1), val_mk_undefined())));
    CU_ASSERT(val_is_nan(val_mod(val_mk_number(1), val_mk_boolean(0))));
    CU_ASSERT(val_is_nan(val_mod(val_mk_number(1), val_mk_boolean(1))));

    CU_ASSERT(val_is_nan(val_mod(val_mk_nan(), val_mk_number(1))));
    CU_ASSERT(val_is_nan(val_mod(val_mk_nan(), val_mk_nan())));
}

static void test_val_and(void)
{
    CU_ASSERT(0  == val_2_double(val_and(val_mk_number(0), val_mk_number(0))));
    CU_ASSERT(0  == val_2_double(val_and(val_mk_number(0), val_mk_number(1))));
    CU_ASSERT(0  == val_2_double(val_and(val_mk_number(1), val_mk_number(0))));
    CU_ASSERT(1  == val_2_double(val_and(val_mk_number(1), val_mk_number(1))));

    CU_ASSERT(val_is_nan(val_and(val_mk_number(1), val_mk_nan())));
    CU_ASSERT(val_is_nan(val_and(val_mk_number(1), val_mk_undefined())));
    CU_ASSERT(val_is_nan(val_and(val_mk_number(1), val_mk_boolean(0))));
    CU_ASSERT(val_is_nan(val_and(val_mk_number(1), val_mk_boolean(1))));

    CU_ASSERT(val_is_nan(val_and(val_mk_nan(), val_mk_number(1))));
    CU_ASSERT(val_is_nan(val_and(val_mk_nan(), val_mk_nan())));
}

static void test_val_or(void)
{
    CU_ASSERT(0  == val_2_double(val_or(val_mk_number(0), val_mk_number(0))));
    CU_ASSERT(1  == val_2_double(val_or(val_mk_number(0), val_mk_number(1))));
    CU_ASSERT(1  == val_2_double(val_or(val_mk_number(1), val_mk_number(0))));
    CU_ASSERT(1  == val_2_double(val_or(val_mk_number(1), val_mk_number(1))));

    CU_ASSERT(val_is_nan(val_or(val_mk_number(1), val_mk_nan())));
    CU_ASSERT(val_is_nan(val_or(val_mk_number(1), val_mk_undefined())));
    CU_ASSERT(val_is_nan(val_or(val_mk_number(1), val_mk_boolean(0))));
    CU_ASSERT(val_is_nan(val_or(val_mk_number(1), val_mk_boolean(1))));

    CU_ASSERT(val_is_nan(val_or(val_mk_nan(), val_mk_number(1))));
    CU_ASSERT(val_is_nan(val_or(val_mk_nan(), val_mk_nan())));
}

static void test_val_xor(void)
{
    CU_ASSERT(0  == val_2_double(val_xor(val_mk_number(0), val_mk_number(0))));
    CU_ASSERT(1  == val_2_double(val_xor(val_mk_number(0), val_mk_number(1))));
    CU_ASSERT(1  == val_2_double(val_xor(val_mk_number(1), val_mk_number(0))));
    CU_ASSERT(0  == val_2_double(val_xor(val_mk_number(1), val_mk_number(1))));

    CU_ASSERT(val_is_nan(val_xor(val_mk_number(1), val_mk_nan())));
    CU_ASSERT(val_is_nan(val_xor(val_mk_number(1), val_mk_undefined())));
    CU_ASSERT(val_is_nan(val_xor(val_mk_number(1), val_mk_boolean(0))));
    CU_ASSERT(val_is_nan(val_xor(val_mk_number(1), val_mk_boolean(1))));

    CU_ASSERT(val_is_nan(val_xor(val_mk_nan(), val_mk_number(1))));
    CU_ASSERT(val_is_nan(val_xor(val_mk_nan(), val_mk_nan())));
}

static void test_val_neg(void)
{
    CU_ASSERT(~0  == val_2_integer(val_neg(val_mk_number(0))));
    CU_ASSERT(~1  == val_2_integer(val_neg(val_mk_number(1))));

    CU_ASSERT(val_is_nan(val_neg(val_mk_nan())));
    CU_ASSERT(val_is_nan(val_neg(val_mk_undefined())));
    CU_ASSERT(val_is_nan(val_neg(val_mk_boolean(0))));
    CU_ASSERT(val_is_nan(val_neg(val_mk_boolean(1))));
}

static void test_val_lshift(void)
{
    CU_ASSERT(0  == val_2_integer(val_lshift(val_mk_number(0), val_mk_number(0))));
    CU_ASSERT(0  == val_2_integer(val_lshift(val_mk_number(0), val_mk_number(1))));
    CU_ASSERT(1 << 0  == val_2_integer(val_lshift(val_mk_number(1), val_mk_number(0))));
    CU_ASSERT(1 << 1 == val_2_integer(val_lshift(val_mk_number(1), val_mk_number(1))));

    CU_ASSERT(val_is_nan(val_lshift(val_mk_number(1), val_mk_nan())));
    CU_ASSERT(val_is_nan(val_lshift(val_mk_number(1), val_mk_undefined())));
    CU_ASSERT(val_is_nan(val_lshift(val_mk_number(1), val_mk_boolean(0))));
    CU_ASSERT(val_is_nan(val_lshift(val_mk_number(1), val_mk_boolean(1))));

    CU_ASSERT(val_is_nan(val_lshift(val_mk_nan(), val_mk_number(1))));
    CU_ASSERT(val_is_nan(val_lshift(val_mk_nan(), val_mk_nan())));
}

static void test_val_rshift(void)
{
    CU_ASSERT(0  == val_2_integer(val_rshift(val_mk_number(0), val_mk_number(0))));
    CU_ASSERT(0  == val_2_integer(val_rshift(val_mk_number(0), val_mk_number(1))));
    CU_ASSERT(1 >> 0 == val_2_integer(val_rshift(val_mk_number(1), val_mk_number(0))));
    CU_ASSERT(1 >> 1 == val_2_integer(val_rshift(val_mk_number(1), val_mk_number(1))));
    CU_ASSERT(2 >> 1 == val_2_integer(val_rshift(val_mk_number(2), val_mk_number(1))));
    CU_ASSERT(-1 >> 1  == val_2_integer(val_rshift(val_mk_number(-1), val_mk_number(1))));

    CU_ASSERT(val_is_nan(val_rshift(val_mk_number(1), val_mk_nan())));
    CU_ASSERT(val_is_nan(val_rshift(val_mk_number(1), val_mk_undefined())));
    CU_ASSERT(val_is_nan(val_rshift(val_mk_number(1), val_mk_boolean(0))));
    CU_ASSERT(val_is_nan(val_rshift(val_mk_number(1), val_mk_boolean(1))));

    CU_ASSERT(val_is_nan(val_rshift(val_mk_nan(), val_mk_number(1))));
    CU_ASSERT(val_is_nan(val_rshift(val_mk_nan(), val_mk_nan())));
}

static void test_val_teq(void)
{
    CU_ASSERT((val_teq(val_mk_number(0), val_mk_number(0))));
    CU_ASSERT((val_teq(val_mk_number(1), val_mk_number(1))));
    CU_ASSERT(!(val_teq(val_mk_number(0), val_mk_number(1))));
    CU_ASSERT(!(val_teq(val_mk_number(1), val_mk_number(0))));

    CU_ASSERT(!(val_teq(val_mk_number(0), val_mk_undefined())));
    CU_ASSERT(!(val_teq(val_mk_number(1), val_mk_undefined())));

    CU_ASSERT(!(val_teq(val_mk_number(0), val_mk_nan())));
    CU_ASSERT(!(val_teq(val_mk_number(1), val_mk_nan())));

    CU_ASSERT((val_teq(val_mk_boolean(0), val_mk_boolean(0))));
    CU_ASSERT((val_teq(val_mk_boolean(1), val_mk_boolean(1))));
    CU_ASSERT(!(val_teq(val_mk_boolean(0), val_mk_boolean(1))));
    CU_ASSERT(!(val_teq(val_mk_boolean(1), val_mk_boolean(0))));

    CU_ASSERT(!(val_teq(val_mk_boolean(0), val_mk_number(0))));
    CU_ASSERT(!(val_teq(val_mk_boolean(1), val_mk_number(1))));

    CU_ASSERT(!(val_teq(val_mk_boolean(1), val_mk_undefined())));
    CU_ASSERT(!(val_teq(val_mk_boolean(0), val_mk_undefined())));

    CU_ASSERT(!(val_teq(val_mk_boolean(1), val_mk_nan())));
    CU_ASSERT(!(val_teq(val_mk_boolean(0), val_mk_nan())));

    CU_ASSERT(!(val_teq(val_mk_nan(), val_mk_undefined())));
    CU_ASSERT(!(val_teq(val_mk_undefined(), val_mk_nan())));
    CU_ASSERT(!(val_teq(val_mk_undefined(), val_mk_undefined())));
    CU_ASSERT(!(val_teq(val_mk_nan(), val_mk_nan())));
}

static void test_val_tgt(void)
{
    CU_ASSERT(!(val_tgt(val_mk_number(0), val_mk_number(1))));
    CU_ASSERT((val_tgt(val_mk_number(0), val_mk_number(-1))));
    CU_ASSERT((val_tgt(val_mk_number(2), val_mk_number(1))));
    CU_ASSERT(!(val_tgt(val_mk_number(1), val_mk_number(1))));

    CU_ASSERT(!(val_tgt(val_mk_number(0), val_mk_undefined())));
    CU_ASSERT(!(val_tgt(val_mk_number(1), val_mk_undefined())));

    CU_ASSERT(!(val_tgt(val_mk_number(0), val_mk_nan())));
    CU_ASSERT(!(val_tgt(val_mk_number(1), val_mk_nan())));

    CU_ASSERT(!(val_tgt(val_mk_boolean(0), val_mk_boolean(0))));
    CU_ASSERT(!(val_tgt(val_mk_boolean(1), val_mk_boolean(1))));
    CU_ASSERT(!(val_tgt(val_mk_boolean(0), val_mk_boolean(1))));
    CU_ASSERT(!(val_tgt(val_mk_boolean(1), val_mk_boolean(0))));

    CU_ASSERT(!(val_tgt(val_mk_boolean(0), val_mk_number(0))));
    CU_ASSERT(!(val_tgt(val_mk_boolean(1), val_mk_number(1))));

    CU_ASSERT(!(val_tgt(val_mk_boolean(1), val_mk_undefined())));
    CU_ASSERT(!(val_tgt(val_mk_boolean(0), val_mk_undefined())));

    CU_ASSERT(!(val_tgt(val_mk_boolean(1), val_mk_nan())));
    CU_ASSERT(!(val_tgt(val_mk_boolean(0), val_mk_nan())));

    CU_ASSERT(!(val_tgt(val_mk_nan(), val_mk_undefined())));
    CU_ASSERT(!(val_tgt(val_mk_undefined(), val_mk_nan())));
    CU_ASSERT(!(val_tgt(val_mk_undefined(), val_mk_undefined())));
    CU_ASSERT(!(val_tgt(val_mk_nan(), val_mk_nan())));
}

static void test_val_tge(void)
{
    CU_ASSERT(!(val_tge(val_mk_number(0), val_mk_number(1))));
    CU_ASSERT((val_tge(val_mk_number(0), val_mk_number(-1))));
    CU_ASSERT((val_tge(val_mk_number(2), val_mk_number(1))));
    CU_ASSERT((val_tge(val_mk_number(1), val_mk_number(1))));

    CU_ASSERT(!(val_tge(val_mk_number(0), val_mk_undefined())));
    CU_ASSERT(!(val_tge(val_mk_number(1), val_mk_undefined())));

    CU_ASSERT(!(val_tge(val_mk_number(0), val_mk_nan())));
    CU_ASSERT(!(val_tge(val_mk_number(1), val_mk_nan())));

    CU_ASSERT(!(val_tge(val_mk_boolean(0), val_mk_boolean(0))));
    CU_ASSERT(!(val_tge(val_mk_boolean(1), val_mk_boolean(1))));
    CU_ASSERT(!(val_tge(val_mk_boolean(0), val_mk_boolean(1))));
    CU_ASSERT(!(val_tge(val_mk_boolean(1), val_mk_boolean(0))));

    CU_ASSERT(!(val_tge(val_mk_boolean(0), val_mk_number(0))));
    CU_ASSERT(!(val_tge(val_mk_boolean(1), val_mk_number(1))));

    CU_ASSERT(!(val_tge(val_mk_boolean(1), val_mk_undefined())));
    CU_ASSERT(!(val_tge(val_mk_boolean(0), val_mk_undefined())));

    CU_ASSERT(!(val_tge(val_mk_boolean(1), val_mk_nan())));
    CU_ASSERT(!(val_tge(val_mk_boolean(0), val_mk_nan())));

    CU_ASSERT(!(val_tge(val_mk_nan(), val_mk_undefined())));
    CU_ASSERT(!(val_tge(val_mk_undefined(), val_mk_nan())));
    CU_ASSERT(!(val_tge(val_mk_undefined(), val_mk_undefined())));
    CU_ASSERT(!(val_tge(val_mk_nan(), val_mk_nan())));
}

static void test_val_tlt(void)
{
    CU_ASSERT( (val_tlt(val_mk_number(0), val_mk_number(1))));
    CU_ASSERT(!(val_tlt(val_mk_number(0), val_mk_number(-1))));
    CU_ASSERT(!(val_tlt(val_mk_number(2), val_mk_number(1))));
    CU_ASSERT( (val_tlt(val_mk_number(1), val_mk_number(2))));
    CU_ASSERT( (val_tlt(val_mk_number(-2), val_mk_number(-1))));
    CU_ASSERT(!(val_tlt(val_mk_number(-1), val_mk_number(-2))));
    CU_ASSERT(!(val_tlt(val_mk_number(1), val_mk_number(1))));
    CU_ASSERT(!(val_tlt(val_mk_number(-1), val_mk_number(-1))));

    CU_ASSERT(!(val_tlt(val_mk_number(0), val_mk_undefined())));
    CU_ASSERT(!(val_tlt(val_mk_number(1), val_mk_undefined())));

    CU_ASSERT(!(val_tlt(val_mk_number(0), val_mk_nan())));
    CU_ASSERT(!(val_tlt(val_mk_number(1), val_mk_nan())));

    CU_ASSERT(!(val_tlt(val_mk_boolean(0), val_mk_boolean(0))));
    CU_ASSERT(!(val_tlt(val_mk_boolean(1), val_mk_boolean(1))));
    CU_ASSERT(!(val_tlt(val_mk_boolean(0), val_mk_boolean(1))));
    CU_ASSERT(!(val_tlt(val_mk_boolean(1), val_mk_boolean(0))));

    CU_ASSERT(!(val_tlt(val_mk_boolean(0), val_mk_number(0))));
    CU_ASSERT(!(val_tlt(val_mk_boolean(1), val_mk_number(1))));

    CU_ASSERT(!(val_tlt(val_mk_boolean(1), val_mk_undefined())));
    CU_ASSERT(!(val_tlt(val_mk_boolean(0), val_mk_undefined())));

    CU_ASSERT(!(val_tlt(val_mk_boolean(1), val_mk_nan())));
    CU_ASSERT(!(val_tlt(val_mk_boolean(0), val_mk_nan())));

    CU_ASSERT(!(val_tlt(val_mk_nan(), val_mk_undefined())));
    CU_ASSERT(!(val_tlt(val_mk_undefined(), val_mk_nan())));
    CU_ASSERT(!(val_tlt(val_mk_undefined(), val_mk_undefined())));
    CU_ASSERT(!(val_tlt(val_mk_nan(), val_mk_nan())));
}

static void test_val_tle(void)
{
    CU_ASSERT( (val_tle(val_mk_number(0), val_mk_number(1))));
    CU_ASSERT(!(val_tle(val_mk_number(0), val_mk_number(-1))));
    CU_ASSERT(!(val_tle(val_mk_number(2), val_mk_number(1))));
    CU_ASSERT( (val_tle(val_mk_number(1), val_mk_number(2))));
    CU_ASSERT( (val_tle(val_mk_number(-2), val_mk_number(-1))));
    CU_ASSERT(!(val_tle(val_mk_number(-1), val_mk_number(-2))));
    CU_ASSERT( (val_tle(val_mk_number(1), val_mk_number(1))));
    CU_ASSERT( (val_tle(val_mk_number(-1), val_mk_number(-1))));

    CU_ASSERT(!(val_tle(val_mk_number(0), val_mk_undefined())));
    CU_ASSERT(!(val_tle(val_mk_number(1), val_mk_undefined())));

    CU_ASSERT(!(val_tle(val_mk_number(0), val_mk_nan())));
    CU_ASSERT(!(val_tle(val_mk_number(1), val_mk_nan())));

    CU_ASSERT(!(val_tle(val_mk_boolean(0), val_mk_boolean(0))));
    CU_ASSERT(!(val_tle(val_mk_boolean(1), val_mk_boolean(1))));
    CU_ASSERT(!(val_tle(val_mk_boolean(0), val_mk_boolean(1))));
    CU_ASSERT(!(val_tle(val_mk_boolean(1), val_mk_boolean(0))));

    CU_ASSERT(!(val_tle(val_mk_boolean(0), val_mk_number(0))));
    CU_ASSERT(!(val_tle(val_mk_boolean(1), val_mk_number(1))));

    CU_ASSERT(!(val_tle(val_mk_boolean(1), val_mk_undefined())));
    CU_ASSERT(!(val_tle(val_mk_boolean(0), val_mk_undefined())));

    CU_ASSERT(!(val_tle(val_mk_boolean(1), val_mk_nan())));
    CU_ASSERT(!(val_tle(val_mk_boolean(0), val_mk_nan())));

    CU_ASSERT(!(val_tle(val_mk_nan(), val_mk_undefined())));
    CU_ASSERT(!(val_tle(val_mk_undefined(), val_mk_nan())));
    CU_ASSERT(!(val_tle(val_mk_undefined(), val_mk_undefined())));
    CU_ASSERT(!(val_tle(val_mk_nan(), val_mk_nan())));
}

CU_pSuite test_lang_val_entry()
{
    CU_pSuite suite = CU_add_suite("lang value", test_setup, test_clean);

    if (suite) {
        CU_add_test(suite, "value make", test_val_make);
        CU_add_test(suite, "value set", test_val_set);
        CU_add_test(suite, "value add", test_val_add);
        CU_add_test(suite, "value sub", test_val_sub);
        CU_add_test(suite, "value mul", test_val_mul);
        CU_add_test(suite, "value div", test_val_div);
        CU_add_test(suite, "value mod", test_val_mod);
        CU_add_test(suite, "value and", test_val_and);
        CU_add_test(suite, "value or",  test_val_or);
        CU_add_test(suite, "value xor", test_val_xor);
        CU_add_test(suite, "value neg", test_val_neg);
        CU_add_test(suite, "value lshift", test_val_lshift);
        CU_add_test(suite, "value rshift", test_val_rshift);
        CU_add_test(suite, "value teq", test_val_teq);
        CU_add_test(suite, "value tgt", test_val_tgt);
        CU_add_test(suite, "value tge", test_val_tge);
        CU_add_test(suite, "value tlt", test_val_tlt);
        CU_add_test(suite, "value tle", test_val_tle);
    }

    return suite;
}

