
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
    env_t *env = NULL;
    val_t stack[128];
    val_t *res;

    interp = interp_init(&interp_st, stack, 128);

    CU_ASSERT(0 == eval_string(interp, env, "NaN", &res) && val_is_nan(*res));
    CU_ASSERT(0 == eval_string(interp, env, "undefined", &res) && val_is_undefined(*res));
    CU_ASSERT(0 == eval_string(interp, env, "true", &res) && val_is_boolean(*res) && val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, env, "false", &res) && val_is_boolean(*res) && !val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, env, "0", &res) && val_is_number(*res) && val_2_integer(*res) == 0);
    CU_ASSERT(0 == eval_string(interp, env, "1", &res) && val_is_number(*res) && val_2_integer(*res) == 1);
    // float token is not supported by lex
    //CU_ASSERT(0 == eval_string(interp, env, "1.0001", &res) && val_is_number(*res) && val_2_double(*res) == 1.0001);

    interp_deinit(interp);
}

static void test_eval_calculate(void)
{
    interp_t interp_st, *interp;
    env_t *env = NULL;
    val_t stack[128];
    val_t *res;

    interp = interp_init(&interp_st, stack, 128);

    CU_ASSERT(0 == eval_string(interp, env, "-1", &res) && val_is_number(*res) && -1 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "~0", &res) && val_is_number(*res) && -1 == val_2_integer(*res));
    CU_ASSERT(0 == eval_string(interp, env, "!1", &res) && val_is_boolean(*res) && !val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, env, "!0", &res) && val_is_boolean(*res) && val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, env, "1 + 1", &res) && val_is_number(*res) && 2 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "1 + -1", &res) && val_is_number(*res) && 0 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "1 - 1", &res) && val_is_number(*res) && 0 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "1 - 2", &res) && val_is_number(*res) && -1 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "1 - -1", &res) && val_is_number(*res) && 2 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "0 * 2", &res) && val_is_number(*res) && 0 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "2 * 2", &res) && val_is_number(*res) && 4 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "0 / 2", &res) && val_is_number(*res) && 0 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "6 / 2", &res) && val_is_number(*res) && 3 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "6 / 0", &res) && val_is_nan(*res));
    CU_ASSERT(0 == eval_string(interp, env, "0 % 2", &res) && val_is_number(*res) && 0 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "3 % 2", &res) && val_is_number(*res) && 1 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "6 % 0", &res) && val_is_nan(*res));

    CU_ASSERT(0 == eval_string(interp, env, "1 + 2 * 3", &res) && val_is_number(*res) && 7 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "2 + 4 / 2", &res) && val_is_number(*res) && 4 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "9 - 4 % 2", &res) && val_is_number(*res) && 9 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "(1 + 2) * 3", &res) && val_is_number(*res) && 9 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "(2 + 4) / 2", &res) && val_is_number(*res) && 3 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "(9 - 4) % 2", &res) && val_is_number(*res) && 1 == val_2_double(*res));

    CU_ASSERT(0 == eval_string(interp, env, "0 & 0", &res) && val_is_number(*res) && 0 == val_2_integer(*res));
    CU_ASSERT(0 == eval_string(interp, env, "1 & 0", &res) && val_is_number(*res) && 0 == val_2_integer(*res));
    CU_ASSERT(0 == eval_string(interp, env, "0 & 1", &res) && val_is_number(*res) && 0 == val_2_integer(*res));
    CU_ASSERT(0 == eval_string(interp, env, "1 & 1", &res) && val_is_number(*res) && 1 == val_2_integer(*res));

    CU_ASSERT(0 == eval_string(interp, env, "0 | 0", &res) && val_is_number(*res) && 0 == val_2_integer(*res));
    CU_ASSERT(0 == eval_string(interp, env, "1 | 0", &res) && val_is_number(*res) && 1 == val_2_integer(*res));
    CU_ASSERT(0 == eval_string(interp, env, "0 | 1", &res) && val_is_number(*res) && 1 == val_2_integer(*res));
    CU_ASSERT(0 == eval_string(interp, env, "1 | 1", &res) && val_is_number(*res) && 1 == val_2_integer(*res));

    CU_ASSERT(0 == eval_string(interp, env, "0 ^ 0", &res) && val_is_number(*res) && 0 == val_2_integer(*res));
    CU_ASSERT(0 == eval_string(interp, env, "1 ^ 0", &res) && val_is_number(*res) && 1 == val_2_integer(*res));
    CU_ASSERT(0 == eval_string(interp, env, "0 ^ 1", &res) && val_is_number(*res) && 1 == val_2_integer(*res));
    CU_ASSERT(0 == eval_string(interp, env, "1 ^ 1", &res) && val_is_number(*res) && 0 == val_2_integer(*res));

    CU_ASSERT(0 == eval_string(interp, env, "1 << 0", &res) && val_is_number(*res) && 1 == val_2_integer(*res));
    CU_ASSERT(0 == eval_string(interp, env, "1 << 1", &res) && val_is_number(*res) && 2 == val_2_integer(*res));
    CU_ASSERT(0 == eval_string(interp, env, "1 >> 0", &res) && val_is_number(*res) && 1 == val_2_integer(*res));
    CU_ASSERT(0 == eval_string(interp, env, "1 >> 1", &res) && val_is_number(*res) && 0 == val_2_integer(*res));

    CU_ASSERT(0 == eval_string(interp, env, "(4 >> 1) * 5 - 7 % 3 + 6 / 2", &res) && val_is_number(*res) &&
              ((4 >> 1) * 5 - 7 % 3 + 6 / 2) == val_2_integer(*res));

    interp_deinit(interp);
    return;
}

static void test_eval_compare(void)
{
    interp_t interp_st, *interp;
    env_t *env = NULL;
    val_t stack[128];
    val_t *res;

    interp = interp_init(&interp_st, stack, 128);

    CU_ASSERT(0 == eval_string(interp, env, "1 != 0", &res) && val_is_boolean(*res) &&  val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, env, "1 == 0", &res) && val_is_boolean(*res) && !val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, env, "1 > 0", &res)  && val_is_boolean(*res) &&  val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, env, "1 >= 0", &res) && val_is_boolean(*res) &&  val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, env, "1 >= 1", &res) && val_is_boolean(*res) &&  val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, env, "0 < 1", &res)  && val_is_boolean(*res) &&  val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, env, "0 <= 1", &res) && val_is_boolean(*res) &&  val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, env, "1 <= 1", &res) && val_is_boolean(*res) &&  val_is_true(*res));

    /*
    CU_ASSERT(0 == eval_string(interp, env, "0 > 1 ? 0 : 1", &res) && val_is_number(*res) &&  1 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "0 < 1 ? 0 : 1", &res) && val_is_number(*res) &&  0 == val_2_double(*res));

    CU_ASSERT(0 == eval_string(interp, env, "0 > 1 ? 0 ? 10: 20 : 1 ? 30: 40", &res) && val_is_number(*res) &&  30 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "0 < 1 ? 0 ? 10: 20 : 1 ? 30: 40", &res) && val_is_number(*res) &&  20 == val_2_double(*res));
    */

    interp_deinit(interp);
}

static void test_eval_logic(void)
{
    interp_t interp_st, *interp;
    env_t *env = NULL;
    val_t stack[128];
    val_t *res;

    interp = interp_init(&interp_st, stack, 128);

    CU_ASSERT(0 == eval_string(interp, env, "false && false", &res) && val_is_boolean(*res) &&  !val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, env, "false && true",  &res) && val_is_boolean(*res) &&  !val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, env, "true && false",  &res) && val_is_boolean(*res) &&  !val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, env, "true && true",   &res) && val_is_boolean(*res) &&  val_is_true(*res));

    CU_ASSERT(0 == eval_string(interp, env, "false || false", &res) && val_is_boolean(*res) &&  !val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, env, "false || true",  &res) && val_is_boolean(*res) &&  val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, env, "true || false",  &res) && val_is_boolean(*res) &&  val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, env, "true || true",   &res) && val_is_boolean(*res) &&  val_is_true(*res));

    CU_ASSERT(0 == eval_string(interp, env, "0 && 0", &res) && val_is_number(*res) &&  0 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "0 && 1", &res) && val_is_number(*res) &&  0 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "1 && 0", &res) && val_is_number(*res) &&  0 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "1 && 1", &res) && val_is_number(*res) &&  1 == val_2_double(*res));

    CU_ASSERT(0 == eval_string(interp, env, "0 || 0", &res) && val_is_number(*res) &&  0 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "0 || 1", &res) && val_is_number(*res) &&  1 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "1 || 0", &res) && val_is_number(*res) &&  1 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "1 || 1", &res) && val_is_number(*res) &&  1 == val_2_double(*res));

    CU_ASSERT(0 == eval_string(interp, env, "0 && true",  &res) && val_is_number(*res)  &&  0 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "0 && false", &res) && val_is_number(*res)  &&  0 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "1 && true",  &res) && val_is_boolean(*res) &&  val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, env, "1 && false", &res) && val_is_boolean(*res) &&  !val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, env, "false && 0", &res) && val_is_boolean(*res) &&  !val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, env, "false && 1", &res) && val_is_boolean(*res) &&  !val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, env, "true  && 0", &res) && val_is_number(*res)  &&  0 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "true  && 1", &res) && val_is_number(*res)  &&  1 == val_2_double(*res));

    CU_ASSERT(0 == eval_string(interp, env, "0 || true", &res)  && val_is_boolean(*res) &&  val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, env, "0 || false", &res) && val_is_boolean(*res) &&  !val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, env, "1 || true", &res)  && val_is_number(*res)  &&  1 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "1 || false", &res) && val_is_number(*res)  &&  1 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "false || 0", &res) && val_is_number(*res)  &&  0 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "false || 1", &res) && val_is_number(*res)  &&  1 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "true  || 0", &res) && val_is_boolean(*res) &&  val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, env, "true  || 1", &res) && val_is_boolean(*res) &&  val_is_true(*res));
    if (0) {
    }

    interp_deinit(interp);
}
static void test_eval_symbal(void)
{
    interp_t interp_st, *interp;
    env_t env_st, *env = &env_st;
    val_t stack[128];
    val_t *res;
    val_t *p;

    interp = interp_init(&interp_st, stack, 128);

    CU_ASSERT_FATAL(0 == env_init(&env_st, 16));

    CU_ASSERT(env_get_variable(env, "a", &p) < 0);
    CU_ASSERT(env_get_variable(env, "b", &p) < 0);

    CU_ASSERT(env_add_variable(env, "a") >= 0);
    CU_ASSERT(env_add_variable(env, "b") >= 0);

    CU_ASSERT(env_get_variable(env, "a", &p) >= 0 && val_is_undefined(*p));
    CU_ASSERT(env_get_variable(env, "b", &p) >= 0 && val_is_undefined(*p));

    CU_ASSERT(-1 != env_set_variable(env, "a", val_mk_number(5)));
    CU_ASSERT(-1 != env_set_variable(env, "b", val_mk_number(2)));
    CU_ASSERT(-1 != env_get_variable(env, "a", &p) && val_is_number(*p) &&  5 == val_2_double(*p));
    CU_ASSERT(-1 != env_get_variable(env, "b", &p) && val_is_number(*p) &&  2 == val_2_double(*p));

    CU_ASSERT(0 == eval_string(interp, env, "a + 2", &res) && val_is_number(*res) &&  7 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "a - 2", &res) && val_is_number(*res) &&  3 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "a * 2", &res) && val_is_number(*res) &&  10 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "a / 2", &res) && val_is_number(*res) &&  5.0 / 2 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "a % 2", &res) && val_is_number(*res) &&  1 == val_2_double(*res));

    CU_ASSERT(0 == eval_string(interp, env, "5 + b", &res) && val_is_number(*res) &&  7 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "5 - b", &res) && val_is_number(*res) &&  3 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "5 * b", &res) && val_is_number(*res) &&  10 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "5 / b", &res) && val_is_number(*res) &&  5.0 / 2 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "5 % b", &res) && val_is_number(*res) &&  1 == val_2_double(*res));

    CU_ASSERT(0 == eval_string(interp, env, "a + b", &res) && val_is_number(*res) &&  7 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "a - b", &res) && val_is_number(*res) &&  3 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "a * b", &res) && val_is_number(*res) &&  10 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "a / b", &res) && val_is_number(*res) &&  5.0 / 2 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "a % b", &res) && val_is_number(*res) &&  1 == val_2_double(*res));

    CU_ASSERT(0 == eval_string(interp, env, "a = b = 1", &res) && val_is_number(*res) &&  1 == val_2_double(*res));
    CU_ASSERT(-1 != env_get_variable(env, "a", &p) && val_is_number(*p) &&  1 == val_2_double(*p));
    CU_ASSERT(-1 != env_get_variable(env, "b", &p) && val_is_number(*p) &&  1 == val_2_double(*p));

    CU_ASSERT(0 == eval_string(interp, env, "a << 1", &res) && val_is_number(*res) &&  2 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "a >> 1", &res) && val_is_number(*res) &&  0 == val_2_double(*res));

    CU_ASSERT(0 == eval_string(interp, env, "b = a == 1 ? 10: 20", &res) && val_is_number(*res) &&  10 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "a + b > 10", &res) && val_is_boolean(*res) &&  val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, env, "a - b < 10", &res) && val_is_boolean(*res) &&  val_is_true(*res));
    CU_ASSERT(0 == eval_string(interp, env, "a * b == 10", &res) && val_is_boolean(*res) &&  val_is_true(*res));

    env_deinit(env);
    interp_deinit(interp);
}

static void test_eval_var(void)
{
    interp_t interp_st, *interp;
    env_t env_st, *env = &env_st;
    val_t stack[128];
    val_t *res;
    val_t *p;

    interp = interp_init(&interp_st, stack, 128);

    CU_ASSERT_FATAL(0 == env_init(&env_st, 16));

    CU_ASSERT(0 == eval_string(interp, env, "var a;", &res) && val_is_undefined(*res));
    CU_ASSERT(-1 != env_get_variable(env, "a", &p) && val_is_undefined(*p));

    CU_ASSERT(0 == eval_string(interp, env, "var b = 2;", &res) && val_is_undefined(*res));
    CU_ASSERT(-1 != env_get_variable(env, "b", &p) && val_is_number(*p) &&  2 == val_2_double(*p));

    CU_ASSERT(0 == eval_string(interp, env, "var c = d = 3, e = b + c + d;", &res) && val_is_undefined(*res));
    //CU_ASSERT(0 == eval_string(interp, env, "var c = d = 3, e;", &res) && val_is_undefined(*res));
    CU_ASSERT(-1 != env_get_variable(env, "c", &p) && val_is_number(*p) &&  3 == val_2_double(*p));
    CU_ASSERT(-1 != env_get_variable(env, "d", &p) && val_is_number(*p) &&  3 == val_2_double(*p));
    CU_ASSERT(-1 != env_get_variable(env, "e", &p) && val_is_number(*p) &&  8 == val_2_double(*p));

    CU_ASSERT(0 == eval_string(interp, env, "var f, g, h, k = 0;", &res) && val_is_undefined(*res));
    CU_ASSERT(-1 != env_get_variable(env, "f", &p) && val_is_undefined(*p));
    CU_ASSERT(-1 != env_get_variable(env, "g", &p) && val_is_undefined(*p));
    CU_ASSERT(-1 != env_get_variable(env, "h", &p) && val_is_undefined(*p));
    CU_ASSERT(-1 != env_get_variable(env, "k", &p) && val_is_number(*p) &&  0 == val_2_double(*p));

    env_deinit(env);
    interp_deinit(interp);
}

static void test_eval_if(void)
{
    interp_t interp_st, *interp;
    env_t env_st, *env = &env_st;
    val_t stack[128];
    val_t *res;
    val_t *p;

    interp = interp_init(&interp_st, stack, 128);
    CU_ASSERT_FATAL(0 == env_init(&env_st, 16));

    CU_ASSERT(-1 != env_set_variable(env, "a", val_mk_number(0)));
    CU_ASSERT(-1 != env_set_variable(env, "b", val_mk_number(1)));
    CU_ASSERT(-1 != env_set_variable(env, "c", val_mk_number(2)));
    CU_ASSERT(-1 != env_set_variable(env, "d", val_mk_number(3)));

    CU_ASSERT(0 == eval_string(interp, env, "if (a) b = 2 else c = 9", &res) && val_is_undefined(*res));
    CU_ASSERT(-1 != env_get_variable(env, "b", &p) && val_is_number(*p) &&  1 == val_2_double(*p));
    CU_ASSERT(-1 != env_get_variable(env, "c", &p) && val_is_number(*p) &&  9 == val_2_double(*p));

    CU_ASSERT(0 == eval_string(interp, env, "if (a == 0) b = 9 else c = 2", &res) && val_is_undefined(*res));
    CU_ASSERT(-1 != env_get_variable(env, "b", &p) && val_is_number(*p) &&  9 == val_2_double(*p));
    CU_ASSERT(-1 != env_get_variable(env, "c", &p) && val_is_number(*p) &&  9 == val_2_double(*p));

    CU_ASSERT(0 == eval_string(interp, env, "if (a) d = 9 else if (b == c) d = 9 else a = 9", &res) && val_is_undefined(*res));
    CU_ASSERT(-1 != env_get_variable(env, "a", &p) && val_is_number(*p) &&  0 == val_2_double(*p));
    CU_ASSERT(-1 != env_get_variable(env, "b", &p) && val_is_number(*p) &&  9 == val_2_double(*p));
    CU_ASSERT(-1 != env_get_variable(env, "c", &p) && val_is_number(*p) &&  9 == val_2_double(*p));
    CU_ASSERT(-1 != env_get_variable(env, "d", &p) && val_is_number(*p) &&  9 == val_2_double(*p));

    // empty
    CU_ASSERT(0 == eval_string(interp, env, "if (a) {} else {}", &res) && val_is_undefined(*res));
    CU_ASSERT(-1 != env_get_variable(env, "a", &p) && val_is_number(*p) &&  0 == val_2_double(*p));
    CU_ASSERT(-1 != env_get_variable(env, "b", &p) && val_is_number(*p) &&  9 == val_2_double(*p));
    CU_ASSERT(-1 != env_get_variable(env, "c", &p) && val_is_number(*p) &&  9 == val_2_double(*p));
    CU_ASSERT(-1 != env_get_variable(env, "d", &p) && val_is_number(*p) &&  9 == val_2_double(*p));

    // block
    CU_ASSERT(0 == eval_string(interp, env, "if (a == 0) { b = 1; c = 1;} else {d = 2}", &res) && val_is_undefined(*res));
    CU_ASSERT(-1 != env_get_variable(env, "a", &p) && val_is_number(*p) &&  0 == val_2_double(*p));
    CU_ASSERT(-1 != env_get_variable(env, "b", &p) && val_is_number(*p) &&  1 == val_2_double(*p));
    CU_ASSERT(-1 != env_get_variable(env, "c", &p) && val_is_number(*p) &&  1 == val_2_double(*p));
    CU_ASSERT(-1 != env_get_variable(env, "d", &p) && val_is_number(*p) &&  9 == val_2_double(*p));

    CU_ASSERT(0 == eval_string(interp, env, "if (a) { b = 2;} else { c = 3; d = 3}", &res) && val_is_undefined(*res));
    CU_ASSERT(-1 != env_get_variable(env, "a", &p) && val_is_number(*p) &&  0 == val_2_double(*p));
    CU_ASSERT(-1 != env_get_variable(env, "b", &p) && val_is_number(*p) &&  1 == val_2_double(*p));
    CU_ASSERT(-1 != env_get_variable(env, "c", &p) && val_is_number(*p) &&  3 == val_2_double(*p));
    CU_ASSERT(-1 != env_get_variable(env, "d", &p) && val_is_number(*p) &&  3 == val_2_double(*p));

    env_deinit(env);
    interp_deinit(interp);
}

static void test_eval_while(void)
{
    interp_t interp_st, *interp;
    env_t env_st, *env = &env_st;
    val_t stack[128];
    val_t *res;
    val_t *p;

    interp = interp_init(&interp_st, stack, 128);
    CU_ASSERT_FATAL(0 == env_init(&env_st, 16));

    CU_ASSERT(-1 != env_set_variable(env, "a", val_mk_number(0)));
    CU_ASSERT(-1 != env_set_variable(env, "b", val_mk_number(9)));

    CU_ASSERT(0 == eval_string(interp, env, "while(a) {a = 1}", &res) && val_is_undefined(*res));
    CU_ASSERT(-1 != env_get_variable(env, "a", &p) && val_is_number(*p) &&  0 == val_2_double(*p));

    CU_ASSERT(-1 != env_get_variable(env, "b", &p) && val_is_number(*p) &&  9 == val_2_double(*p));
    CU_ASSERT(0 == eval_string(interp, env, "while (b) b = b - 1", &res) && val_is_undefined(*res));
    CU_ASSERT(-1 != env_get_variable(env, "b", &p) && val_is_number(*p) &&  0 == val_2_double(*p));

    CU_ASSERT(-1 != env_get_variable(env, "b", &p) && val_is_number(*p) &&  0 == val_2_double(*p));
    CU_ASSERT(0 == eval_string(interp, env, "while (b < 10) {b = b + 1 a = a + 1}", &res) && val_is_undefined(*res));
    CU_ASSERT(-1 != env_get_variable(env, "a", &p) && val_is_number(*p) &&  10 == val_2_double(*p));
    CU_ASSERT(-1 != env_get_variable(env, "b", &p) && val_is_number(*p) &&  10 == val_2_double(*p));

    // continue
    CU_ASSERT(-1 != env_set_variable(env, "a", val_mk_number(0)));
    CU_ASSERT(-1 != env_set_variable(env, "b", val_mk_number(0)));
    CU_ASSERT(0 == eval_string(interp, env, "while (b < 10) {b = b + 1; if (b == 5) continue; a = a + 1}", &res) && val_is_undefined(*res));
    CU_ASSERT(-1 != env_get_variable(env, "b", &p) && val_is_number(*p) &&  10 == val_2_double(*p));
    CU_ASSERT(-1 != env_get_variable(env, "a", &p) && val_is_number(*p) &&  9 == val_2_double(*p));

    // break;
    CU_ASSERT(0 == eval_string(interp, env, "while (a) {a = a - 1; if (a == 5) break; }", &res) && val_is_undefined(*res));
    CU_ASSERT(-1 != env_get_variable(env, "a", &p) && val_is_number(*p) &&  5 == val_2_double(*p));

    // nest
    CU_ASSERT(-1 != env_set_variable(env, "a", val_mk_number(9)));
    CU_ASSERT(-1 != env_set_variable(env, "b", val_mk_number(9)));
    CU_ASSERT(-1 != env_set_variable(env, "c", val_mk_number(0)));
    CU_ASSERT(0 == eval_string(interp, env, "while (a) {b=a; while(b){b=b-1;if(2*b==a){ c=c+1; break;}}; if(a+b==6)break; a=a-1;}", &res) && val_is_undefined(*res));
    CU_ASSERT(-1 != env_get_variable(env, "a", &p) && val_is_number(*p) &&  4 == val_2_double(*p));
    CU_ASSERT(-1 != env_get_variable(env, "b", &p) && val_is_number(*p) &&  2 == val_2_double(*p));
    CU_ASSERT(-1 != env_get_variable(env, "c", &p) && val_is_number(*p) &&  3 == val_2_double(*p));

    env_deinit(env);
    interp_deinit(interp);
}

static void test_eval_function(void)
{
    interp_t interp_st, *interp;
    env_t env_st, *env = &env_st;
    val_t stack[128];
    val_t *res;

    interp = interp_init(&interp_st, stack, 128);
    CU_ASSERT_FATAL(0 == env_init(&env_st, 16));

    CU_ASSERT(-1 != env_set_variable(env, "a", val_mk_number(1)));
    CU_ASSERT(-1 != env_set_variable(env, "b", val_mk_number(2)));

    CU_ASSERT(0 == eval_string(interp, env, "def c(a, b) return a + b", &res) && val_is_function(*res));
    CU_ASSERT(0 == eval_string(interp, env, "c()", &res) && val_is_nan(*res));
    CU_ASSERT(0 == eval_string(interp, env, "c(a)", &res) && val_is_nan(*res));
    CU_ASSERT(0 == eval_string(interp, env, "c(a, b)", &res) && val_is_number(*res) && 3 == val_2_double(*res));

    CU_ASSERT(0 == eval_string(interp, env, "def d(a = a+b, b = b*2) return a + b", &res) && val_is_function(*res));
    CU_ASSERT(0 == eval_string(interp, env, "d()", &res) && val_is_number(*res) && 7 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "d(a)", &res) && val_is_number(*res) && 5 == val_2_double(*res));
    CU_ASSERT(0 == eval_string(interp, env, "d(a, b)", &res) && val_is_number(*res) && 3 == val_2_double(*res));

    env_deinit(env);
    interp_deinit(interp);
}

CU_pSuite test_lang_eval_entry()
{
    CU_pSuite suite = CU_add_suite("lang eval", test_setup, test_clean);

    if (suite) {
        CU_add_test(suite, "eval simple",       test_eval_simple);
        CU_add_test(suite, "eval calculate",    test_eval_calculate);
        CU_add_test(suite, "eval compare",      test_eval_compare);
        CU_add_test(suite, "eval logic",        test_eval_logic);
        if (0) {
        CU_add_test(suite, "eval symbal",       test_eval_symbal);
        CU_add_test(suite, "eval var stmt",     test_eval_var);
        CU_add_test(suite, "eval if stmt",      test_eval_if);
        CU_add_test(suite, "eval while stmt",   test_eval_while);
        CU_add_test(suite, "eval function",     test_eval_function);
        }
    }

    return suite;
}

