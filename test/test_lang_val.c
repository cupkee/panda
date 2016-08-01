
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

    v = val_mk_undefined();
    CU_ASSERT(!val_is_number(&v));
    CU_ASSERT(!val_is_boolean(&v));
    CU_ASSERT(val_is_undefined(&v));
    CU_ASSERT(!val_is_function(&v));
    CU_ASSERT(!val_is_string(&v));
    CU_ASSERT(!val_is_array(&v));
    CU_ASSERT(!val_is_dictionary(&v));
    CU_ASSERT(!val_is_object(&v));
    CU_ASSERT(!val_is_nan(&v));
    CU_ASSERT(!val_is_true(&v));

    v = val_mk_nan();
    CU_ASSERT(!val_is_number(&v));
    CU_ASSERT(!val_is_boolean(&v));
    CU_ASSERT(!val_is_undefined(&v));
    CU_ASSERT(!val_is_function(&v));
    CU_ASSERT(!val_is_string(&v));
    CU_ASSERT(!val_is_array(&v));
    CU_ASSERT(!val_is_dictionary(&v));
    CU_ASSERT(!val_is_object(&v));
    CU_ASSERT(val_is_nan(&v));
    CU_ASSERT(!val_is_true(&v));

    v = val_mk_boolean(1);
    CU_ASSERT(!val_is_number(&v));
    CU_ASSERT(!val_is_nan(&v));
    CU_ASSERT(val_is_boolean(&v));
    CU_ASSERT(!val_is_undefined(&v));
    CU_ASSERT(!val_is_function(&v));
    CU_ASSERT(!val_is_string(&v));
    CU_ASSERT(!val_is_array(&v));
    CU_ASSERT(!val_is_dictionary(&v));
    CU_ASSERT(!val_is_object(&v));
    CU_ASSERT(val_is_true(&v));

    v = val_mk_boolean(0);
    CU_ASSERT(!val_is_true(&v));

    v = val_mk_number(1.1);
    CU_ASSERT(val_is_number(&v));
    CU_ASSERT(!val_is_boolean(&v));
    CU_ASSERT(!val_is_undefined(&v));
    CU_ASSERT(!val_is_function(&v));
    CU_ASSERT(!val_is_string(&v));
    CU_ASSERT(!val_is_array(&v));
    CU_ASSERT(!val_is_dictionary(&v));
    CU_ASSERT(!val_is_object(&v));
    CU_ASSERT(!val_is_nan(&v));
    CU_ASSERT(1 == val_2_integer(&v));
    CU_ASSERT(1.1 == val_2_double(&v));
    CU_ASSERT(val_is_true(&v));
    v = val_mk_number(0.0);
    CU_ASSERT(!val_is_true(&v));
    v = val_mk_number(-0.0);
    CU_ASSERT(!val_is_true(&v));
    v = val_mk_number(0.0000000001);
    CU_ASSERT(val_is_true(&v));

    v = val_mk_script(1);
    CU_ASSERT(val_is_script(&v));
    CU_ASSERT(!val_is_native(&v));
    CU_ASSERT(val_is_function(&v));
    v = val_mk_native(1);
    CU_ASSERT(val_is_native(&v));
    CU_ASSERT(!val_is_script(&v));
    CU_ASSERT(val_is_function(&v));

    v = val_mk_static_string(1);
    CU_ASSERT(val_is_string(&v));
    CU_ASSERT(val_is_static_string(&v));
    CU_ASSERT(!val_is_owned_string(&v));
    CU_ASSERT(!val_is_inline_string(&v));

    v = val_mk_owned_string(1);
    CU_ASSERT(val_is_string(&v));
    CU_ASSERT(!val_is_static_string(&v));
    CU_ASSERT(val_is_owned_string(&v));
    CU_ASSERT(!val_is_inline_string(&v));
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

CU_pSuite test_lang_val_entry()
{
    CU_pSuite suite = CU_add_suite("lang value", test_setup, test_clean);

    if (suite) {
        CU_add_test(suite, "value make", test_val_make);
        CU_add_test(suite, "value set", test_val_set);
    }

    return suite;
}

