
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

    interp_deinit(interp);
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

    interp_deinit(interp);
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

    interp_deinit(interp);
}

static void test_eval_symbal(void)
{
    interp_t interp_st, *interp;
    val_t stack[128];
    val_t *res;
    val_t v;

    interp = interp_init(&interp_st, stack, 128);

    CU_ASSERT(interp_get_symbal(interp, "a") < 0);
    CU_ASSERT(interp_get_symbal(interp, "b") < 0);

    CU_ASSERT(interp_add_symbal(interp, "a") >= 0);
    CU_ASSERT(interp_add_symbal(interp, "b") >= 0);

    CU_ASSERT(interp_get_symbal(interp, "a") >= 0);
    CU_ASSERT(interp_get_symbal(interp, "b") >= 0);
    CU_ASSERT(0 == interp_get_symbal_val(interp, "a", &v) && val_is_undefined(v));
    CU_ASSERT(0 == interp_get_symbal_val(interp, "b", &v) && val_is_undefined(v));

    CU_ASSERT(0 == interp_set_symbal_val(interp, "a", val_mk_number(5)));
    CU_ASSERT(0 == interp_set_symbal_val(interp, "b", val_mk_number(2)));
    CU_ASSERT(0 == interp_get_symbal_val(interp, "a", &v) && val_is_number(v) &&  5 == val_2_double(v));
    CU_ASSERT(0 == interp_get_symbal_val(interp, "b", &v) && val_is_number(v) &&  2 == val_2_double(v));

    CU_ASSERT(0 == eval_string(interp, "a + 2", &res) && val_is_number(*res) &&  7 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "a - 2", &res) && val_is_number(*res) &&  3 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "a * 2", &res) && val_is_number(*res) &&  10 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "a / 2", &res) && val_is_number(*res) &&  5.0 / 2 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "a % 2", &res) && val_is_number(*res) &&  1 == val_2_double(*res));

    CU_ASSERT(0 == eval_string(interp, "5 + b", &res) && val_is_number(*res) &&  7 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "5 - b", &res) && val_is_number(*res) &&  3 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "5 * b", &res) && val_is_number(*res) &&  10 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "5 / b", &res) && val_is_number(*res) &&  5.0 / 2 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "5 % b", &res) && val_is_number(*res) &&  1 == val_2_double(*res));

    CU_ASSERT(0 == eval_string(interp, "a + b", &res) && val_is_number(*res) &&  7 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "a - b", &res) && val_is_number(*res) &&  3 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "a * b", &res) && val_is_number(*res) &&  10 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "a / b", &res) && val_is_number(*res) &&  5.0 / 2 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "a % b", &res) && val_is_number(*res) &&  1 == val_2_double(*res));

    CU_ASSERT(0 == eval_string(interp, "a = b = 1", &res) && val_is_number(*res) &&  1 == val_2_double(*res));
    CU_ASSERT(0 == interp_get_symbal_val(interp, "a", &v) && val_is_number(v) &&  1 == val_2_double(v));
    CU_ASSERT(0 == interp_get_symbal_val(interp, "b", &v) && val_is_number(v) &&  1 == val_2_double(v));

    CU_ASSERT(0 == eval_string(interp, "a << 1", &res) && val_is_number(*res) &&  2 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "a >> 1", &res) && val_is_number(*res) &&  0 == val_2_double(*res));

    CU_ASSERT(0 == eval_string(interp, "b = a == 1 ? 10: 20", &res) && val_is_number(*res) &&  10 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, "a + b > 10", &res) && val_is_boolean(*res) &&  val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, "a - b < 10", &res) && val_is_boolean(*res) &&  val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, "a * b == 10", &res) && val_is_boolean(*res) &&  val_is_true(*res));

    interp_deinit(interp);
}

CU_pSuite test_lang_eval_entry()
{
    CU_pSuite suite = CU_add_suite("lang eval", test_setup, test_clean);

    if (suite) {
        CU_add_test(suite, "eval simple",       test_eval_simple);
        CU_add_test(suite, "eval calculate",    test_eval_calculate);
        CU_add_test(suite, "eval compare",      test_eval_compare);
        CU_add_test(suite, "eval symbal",       test_eval_symbal);
    }

    return suite;
}

