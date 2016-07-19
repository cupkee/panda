
#include <stdio.h>
#include <string.h>

#include "cunit/CUnit.h"
#include "cunit/CUnit_Basic.h"

#include "lang/eval.h"

static int test_setup()
{
    return 0;
}

static int test_clean()
{
    return 0;
}

static void test_eval_simple(void)
{
    interp_t interp_st, *interp;
    val_t stack[128];
    val_t *res;

    interp = interp_init(&interp_st, stack, 128);

    CU_ASSERT(0 == eval_string(interp, "NaN", &res) && val_is_nan(*res));
    CU_ASSERT(0 == eval_string(interp, "undefined", &res) && val_is_undefined(*res));
    CU_ASSERT(0 == eval_string(interp, "1", &res) && val_is_number(*res) && val_2_integer(*res) == 1);
    CU_ASSERT(0 == eval_string(interp, "true", &res) && val_is_boolean(*res) && val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, "false", &res) && val_is_boolean(*res) && !val_is_true(*res));
}

CU_pSuite test_lang_eval_entry()
{
    CU_pSuite suite = CU_add_suite("lang eval", test_setup, test_clean);

    if (suite) {
        CU_add_test(suite, "eval simple", test_eval_simple);
    }

    return suite;
}

