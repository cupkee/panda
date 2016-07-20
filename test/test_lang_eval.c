
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

static void test_eval_calculate(void)
{
    interp_t interp_st, *interp;
    val_t stack[128];
    val_t *res;

    interp = interp_init(&interp_st, stack, 128);

    CU_ASSERT(0 == eval_string(interp, "-1", &res) && val_is_number(*res) && -1 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "~0", &res) && val_is_number(*res) && -1 == val_2_integer(*res));
    CU_ASSERT(0 == eval_string(interp, "!1", &res) && val_is_boolean(*res) && !val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, "!0", &res) && val_is_boolean(*res) && val_is_true(*res));

    CU_ASSERT(0 == eval_string(interp, "1 + 1", &res) && val_is_number(*res) && 2 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "1 + -1", &res) && val_is_number(*res) && 0 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "1 - 1", &res) && val_is_number(*res) && 0 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "1 - 2", &res) && val_is_number(*res) && -1 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "1 - -1", &res) && val_is_number(*res) && 2 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "0 * 2", &res) && val_is_number(*res) && 0 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "2 * 2", &res) && val_is_number(*res) && 4 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "0 / 2", &res) && val_is_number(*res) && 0 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "6 / 2", &res) && val_is_number(*res) && 3 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "6 / 0", &res) && val_is_nan(*res));

    CU_ASSERT(0 == eval_string(interp, "0 % 2", &res) && val_is_number(*res) && 0 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "3 % 2", &res) && val_is_number(*res) && 1 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "6 % 0", &res) && val_is_nan(*res));

    CU_ASSERT(0 == eval_string(interp, "1 + 2 * 3", &res) && val_is_number(*res) && 7 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "2 + 4 / 2", &res) && val_is_number(*res) && 4 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "9 - 4 % 2", &res) && val_is_number(*res) && 9 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "(1 + 2) * 3", &res) && val_is_number(*res) && 9 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "(2 + 4) / 2", &res) && val_is_number(*res) && 3 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "(9 - 4) % 2", &res) && val_is_number(*res) && 1 == val_2_double(*res));

    CU_ASSERT(0 == eval_string(interp, "0 & 0", &res) && val_is_number(*res) && 0 == val_2_integer(*res));
    CU_ASSERT(0 == eval_string(interp, "1 & 0", &res) && val_is_number(*res) && 0 == val_2_integer(*res));
    CU_ASSERT(0 == eval_string(interp, "0 & 1", &res) && val_is_number(*res) && 0 == val_2_integer(*res));
    CU_ASSERT(0 == eval_string(interp, "1 & 1", &res) && val_is_number(*res) && 1 == val_2_integer(*res));

    CU_ASSERT(0 == eval_string(interp, "0 | 0", &res) && val_is_number(*res) && 0 == val_2_integer(*res));
    CU_ASSERT(0 == eval_string(interp, "1 | 0", &res) && val_is_number(*res) && 1 == val_2_integer(*res));
    CU_ASSERT(0 == eval_string(interp, "0 | 1", &res) && val_is_number(*res) && 1 == val_2_integer(*res));
    CU_ASSERT(0 == eval_string(interp, "1 | 1", &res) && val_is_number(*res) && 1 == val_2_integer(*res));

    CU_ASSERT(0 == eval_string(interp, "0 ^ 0", &res) && val_is_number(*res) && 0 == val_2_integer(*res));
    CU_ASSERT(0 == eval_string(interp, "1 ^ 0", &res) && val_is_number(*res) && 1 == val_2_integer(*res));
    CU_ASSERT(0 == eval_string(interp, "0 ^ 1", &res) && val_is_number(*res) && 1 == val_2_integer(*res));
    CU_ASSERT(0 == eval_string(interp, "1 ^ 1", &res) && val_is_number(*res) && 0 == val_2_integer(*res));

    CU_ASSERT(0 == eval_string(interp, "1 << 0", &res) && val_is_number(*res) && 1 == val_2_integer(*res));
    CU_ASSERT(0 == eval_string(interp, "1 << 1", &res) && val_is_number(*res) && 2 == val_2_integer(*res));
    CU_ASSERT(0 == eval_string(interp, "1 >> 0", &res) && val_is_number(*res) && 1 == val_2_integer(*res));
    CU_ASSERT(0 == eval_string(interp, "1 >> 1", &res) && val_is_number(*res) && 0 == val_2_integer(*res));

    CU_ASSERT(0 == eval_string(interp, "(4 >> 1) * 5 - 7 % 3 + 6 / 2", &res) && val_is_number(*res) &&
              ((4 >> 1) * 5 - 7 % 3 + 6 / 2) == val_2_integer(*res));

    return;
}

static void test_eval_compare(void)
{
    interp_t interp_st, *interp;
    val_t stack[128];
    val_t *res;

    interp = interp_init(&interp_st, stack, 128);

    CU_ASSERT(0 == eval_string(interp, "1 != 0", &res) && val_is_boolean(*res) &&  val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, "1 == 0", &res) && val_is_boolean(*res) && !val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, "1 > 0", &res)  && val_is_boolean(*res) &&  val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, "1 >= 0", &res) && val_is_boolean(*res) &&  val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, "1 >= 1", &res) && val_is_boolean(*res) &&  val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, "0 < 1", &res)  && val_is_boolean(*res) &&  val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, "0 <= 1", &res) && val_is_boolean(*res) &&  val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, "1 <= 1", &res) && val_is_boolean(*res) &&  val_is_true(*res));

    CU_ASSERT(0 == eval_string(interp, "0 > 1 ? 0 : 1", &res) && val_is_number(*res) &&  1 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "0 < 1 ? 0 : 1", &res) && val_is_number(*res) &&  0 == val_2_double(*res));

    CU_ASSERT(0 == eval_string(interp, "0 > 1 ? 0 ? 10: 20 : 1 ? 30: 40", &res) && val_is_number(*res) &&  30 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "0 < 1 ? 0 ? 10: 20 : 1 ? 30: 40", &res) && val_is_number(*res) &&  20 == val_2_double(*res));
}

CU_pSuite test_lang_eval_entry()
{
    CU_pSuite suite = CU_add_suite("lang eval", test_setup, test_clean);

    if (suite) {
        CU_add_test(suite, "eval simple",       test_eval_simple);
        CU_add_test(suite, "eval calculate",    test_eval_calculate);
        CU_add_test(suite, "eval compare",      test_eval_compare);
    }

    return suite;
}

