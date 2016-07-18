
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
    CU_ASSERT(val_is_nan(v));
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
    CU_ASSERT(val_is_nan(v));
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

CU_pSuite test_lang_val_entry()
{
    CU_pSuite suite = CU_add_suite("lang value", test_setup, test_clean);

    if (suite) {
        CU_add_test(suite, "value make", test_val_make);
        CU_add_test(suite, "value add", test_val_add);
        CU_add_test(suite, "value sub", test_val_sub);
    }

    return suite;
}

