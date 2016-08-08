
#include <stdio.h>
#include <string.h>

#include "cunit/CUnit.h"
#include "cunit/CUnit_Basic.h"

#include "lang/eval.h"

#define STACK_SIZE  128
#define HEAP_SIZE   32768
uint8_t mem_buf[40960];

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
    eval_env_t env_st, *env = &env_st;
    val_t *res;

    CU_ASSERT_FATAL(0 == eval_env_init_mini(&env_st, mem_buf, 16384, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 == eval_string(env, "NaN", &res) && val_is_nan(res));
    CU_ASSERT(0 == eval_string(env, "undefined", &res) && val_is_undefined(res));
    CU_ASSERT(0 == eval_string(env, "true", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "false", &res) && val_is_boolean(res) && !val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "0", &res) && val_is_number(res) && val_2_integer(res) == 0);
    CU_ASSERT(0 == eval_string(env, "1", &res) && val_is_number(res) && val_2_integer(res) == 1);

    // float token is not supported by lex
    //CU_ASSERT(0 == eval_string(env, "1.0001", &res) && val_is_number(*res) && val_2_double(*res) == 1.0001);

    eval_env_deinit(env);
}

static void test_eval_calculate(void)
{
    eval_env_t env_st, *env = &env_st;
    val_t *res;

    CU_ASSERT_FATAL(0 == eval_env_init_mini(&env_st, mem_buf, 16384, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 == eval_string(env, "-1", &res) && val_is_number(res) && -1 == val_2_integer(res));
    CU_ASSERT(0 == eval_string(env, "~0", &res) && val_is_number(res) && -1 == val_2_integer(res));
    CU_ASSERT(0 == eval_string(env, "!1", &res) && val_is_boolean(res) && !val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "!0", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "1 + 1", &res) && val_is_number(res) && 2 == val_2_integer(res));
    CU_ASSERT(0 == eval_string(env, "1 + -1", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 == eval_string(env, "1 - 1", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 == eval_string(env, "1 - 2", &res) && val_is_number(res) && -1 == val_2_integer(res));
    CU_ASSERT(0 == eval_string(env, "1 - -1", &res) && val_is_number(res) && 2 == val_2_integer(res));
    CU_ASSERT(0 == eval_string(env, "0 * 2", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 == eval_string(env, "2 * 2", &res) && val_is_number(res) && 4 == val_2_integer(res));
    CU_ASSERT(0 == eval_string(env, "0 / 2", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 == eval_string(env, "6 / 2", &res) && val_is_number(res) && 3 == val_2_integer(res));
    CU_ASSERT(0 == eval_string(env, "6 / 0", &res) && val_is_nan(res));
    CU_ASSERT(0 == eval_string(env, "0 % 2", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 == eval_string(env, "3 % 2", &res) && val_is_number(res) && 1 == val_2_integer(res));
    CU_ASSERT(0 == eval_string(env, "6 % 0", &res) && val_is_nan(res));

    CU_ASSERT(0 == eval_string(env, "1 + 2 * 3", &res) && val_is_number(res) && 7 == val_2_integer(res));
    //printf("---------------------------------------------------\n");

    CU_ASSERT(0 == eval_string(env, "2 + 4 / 2", &res) && val_is_number(res) && 4 == val_2_integer(res));
    CU_ASSERT(0 == eval_string(env, "9 - 4 % 2", &res) && val_is_number(res) && 9 == val_2_integer(res));
    CU_ASSERT(0 == eval_string(env, "(1 + 2) * 3", &res) && val_is_number(res) && 9 == val_2_integer(res));
    CU_ASSERT(0 == eval_string(env, "(2 + 4) / 2", &res) && val_is_number(res) && 3 == val_2_integer(res));
    CU_ASSERT(0 == eval_string(env, "(9 - 4) % 2", &res) && val_is_number(res) && 1 == val_2_integer(res));

    CU_ASSERT(0 == eval_string(env, "0 & 0", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 == eval_string(env, "1 & 0", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 == eval_string(env, "0 & 1", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 == eval_string(env, "1 & 1", &res) && val_is_number(res) && 1 == val_2_integer(res));

    CU_ASSERT(0 == eval_string(env, "0 | 0", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 == eval_string(env, "1 | 0", &res) && val_is_number(res) && 1 == val_2_integer(res));
    CU_ASSERT(0 == eval_string(env, "0 | 1", &res) && val_is_number(res) && 1 == val_2_integer(res));
    CU_ASSERT(0 == eval_string(env, "1 | 1", &res) && val_is_number(res) && 1 == val_2_integer(res));

    CU_ASSERT(0 == eval_string(env, "0 ^ 0", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 == eval_string(env, "1 ^ 0", &res) && val_is_number(res) && 1 == val_2_integer(res));
    CU_ASSERT(0 == eval_string(env, "0 ^ 1", &res) && val_is_number(res) && 1 == val_2_integer(res));
    CU_ASSERT(0 == eval_string(env, "1 ^ 1", &res) && val_is_number(res) && 0 == val_2_integer(res));

    CU_ASSERT(0 == eval_string(env, "1 << 0", &res) && val_is_number(res) && 1 == val_2_integer(res));
    CU_ASSERT(0 == eval_string(env, "1 << 1", &res) && val_is_number(res) && 2 == val_2_integer(res));
    CU_ASSERT(0 == eval_string(env, "1 >> 0", &res) && val_is_number(res) && 1 == val_2_integer(res));
    CU_ASSERT(0 == eval_string(env, "1 >> 1", &res) && val_is_number(res) && 0 == val_2_integer(res));

    CU_ASSERT(0 == eval_string(env, "(4 >> 1) * 5 - 7 % 3 + 6 / 2", &res) && val_is_number(res) &&
              ((4 >> 1) * 5 - 7 % 3 + 6 / 2) == val_2_integer(res));

    /*
    */
    eval_env_deinit(env);
    return;
}

static void test_eval_compare(void)
{
    eval_env_t env_st, *env = &env_st;
    val_t *res;

    CU_ASSERT_FATAL(0 == eval_env_init_mini(&env_st, mem_buf, 16384, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 == eval_string(env, "1 != 0", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "1 == 0", &res) && val_is_boolean(res) && !val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "1 > 0", &res)  && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "1 >= 0", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "1 >= 1", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "0 < 1", &res)  && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "0 <= 1", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "1 <= 1", &res) && val_is_boolean(res) &&  val_is_true(res));

    CU_ASSERT(0 == eval_string(env, "0 > 1 ? 0 : 1", &res) && val_is_number(res) &&  1 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "0 < 1 ? 0 : 1", &res) && val_is_number(res) &&  0 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "0 > 1 ? 0 ? 10: 20 : 1 ? 30: 40", &res) && val_is_number(res) &&  30 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "0 < 1 ? 0 ? 10: 20 : 1 ? 30: 40", &res) && val_is_number(res) &&  20 == val_2_double(res));

    eval_env_deinit(env);
}

static void test_eval_logic(void)
{
    eval_env_t env_st, *env = &env_st;
    val_t *res;

    CU_ASSERT_FATAL(0 == eval_env_init_mini(&env_st, mem_buf, 16384, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 == eval_string(env, "false && false", &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "false && true",  &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "true && false",  &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "true && true",   &res) && val_is_boolean(res) &&  val_is_true(res));

    CU_ASSERT(0 == eval_string(env, "false || false", &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "false || true",  &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "true || false",  &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "true || true",   &res) && val_is_boolean(res) &&  val_is_true(res));

    CU_ASSERT(0 == eval_string(env, "0 && 0", &res) && val_is_number(res) &&  0 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "0 && 1", &res) && val_is_number(res) &&  0 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "1 && 0", &res) && val_is_number(res) &&  0 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "1 && 1", &res) && val_is_number(res) &&  1 == val_2_double(res));

    CU_ASSERT(0 == eval_string(env, "0 || 0", &res) && val_is_number(res) &&  0 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "0 || 1", &res) && val_is_number(res) &&  1 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "1 || 0", &res) && val_is_number(res) &&  1 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "1 || 1", &res) && val_is_number(res) &&  1 == val_2_double(res));

    CU_ASSERT(0 == eval_string(env, "0 && true",  &res) && val_is_number(res)  &&  0 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "0 && false", &res) && val_is_number(res)  &&  0 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "1 && true",  &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "1 && false", &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "false && 0", &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "false && 1", &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "true  && 0", &res) && val_is_number(res)  &&  0 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "true  && 1", &res) && val_is_number(res)  &&  1 == val_2_double(res));

    CU_ASSERT(0 == eval_string(env, "0 || true", &res)  && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "0 || false", &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "1 || true", &res)  && val_is_number(res)  &&  1 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "1 || false", &res) && val_is_number(res)  &&  1 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "false || 0", &res) && val_is_number(res)  &&  0 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "false || 1", &res) && val_is_number(res)  &&  1 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "true  || 0", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "true  || 1", &res) && val_is_boolean(res) &&  val_is_true(res));
    /*
    */

    eval_env_deinit(env);
}
static void test_eval_symbal(void)
{
    eval_env_t env_st, *env = &env_st;
    val_t *res;
    val_t *p, v;

    CU_ASSERT_FATAL(0 == eval_env_init_mini(&env_st, mem_buf, 16384, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(eval_env_get_var(env, "a", &p) < 0);
    CU_ASSERT(eval_env_get_var(env, "b", &p) < 0);
    v = val_mk_undefined();
    CU_ASSERT(eval_env_add_var(env, "a", &v) >= 0);
    CU_ASSERT(eval_env_add_var(env, "b", &v) >= 0);

    CU_ASSERT(eval_env_get_var(env, "a", &p) >= 0 && val_is_undefined(p));
    CU_ASSERT(eval_env_get_var(env, "b", &p) >= 0 && val_is_undefined(p));

    v = val_mk_number(5);
    CU_ASSERT(-1 != eval_env_set_var(env, "a", &v));
    v = val_mk_number(2);
    CU_ASSERT(-1 != eval_env_set_var(env, "b", &v));
    CU_ASSERT(-1 != eval_env_get_var(env, "a", &p) && val_is_number(p) &&  5 == val_2_double(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "b", &p) && val_is_number(p) &&  2 == val_2_double(p));

    CU_ASSERT(0 == eval_string(env, "a", &res) && val_is_number(res) &&  5 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "b", &res) && val_is_number(res) &&  2 == val_2_double(res));

    CU_ASSERT(0 == eval_string(env, "a == 5", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "b == 2", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "a != 0", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "b != 1", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "a != 5", &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "b != 2", &res) && val_is_boolean(res) &&  !val_is_true(res));

    CU_ASSERT(0 == eval_string(env, "a + 2", &res) && val_is_number(res) &&  7 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "a - 2", &res) && val_is_number(res) &&  3 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "a * 2", &res) && val_is_number(res) &&  10 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "a / 2", &res) && val_is_number(res) &&  5.0 / 2 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "a % 2", &res) && val_is_number(res) &&  1 == val_2_double(res));

    CU_ASSERT(0 == eval_string(env, "5 + b", &res) && val_is_number(res) &&  7 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "5 - b", &res) && val_is_number(res) &&  3 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "5 * b", &res) && val_is_number(res) &&  10 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "5 / b", &res) && val_is_number(res) &&  5.0 / 2 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "5 % b", &res) && val_is_number(res) &&  1 == val_2_double(res));

    CU_ASSERT(0 == eval_string(env, "a + b", &res) && val_is_number(res) &&  7 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "a - b", &res) && val_is_number(res) &&  3 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "a * b", &res) && val_is_number(res) &&  10 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "a / b", &res) && val_is_number(res) &&  5.0 / 2 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "a % b", &res) && val_is_number(res) &&  1 == val_2_double(res));

    CU_ASSERT(0 == eval_string(env, "a = b = 1", &res) && val_is_number(res) &&  1 == val_2_double(res));
    CU_ASSERT(-1 != eval_env_get_var(env, "a", &p) && val_is_number(p) &&  1 == val_2_double(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "b", &p) && val_is_number(p) &&  1 == val_2_double(p));

    CU_ASSERT(0 == eval_string(env, "a << 1", &res) && val_is_number(res) &&  2 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "a >> 1", &res) && val_is_number(res) &&  0 == val_2_double(res));

    CU_ASSERT(0 == eval_string(env, "b = a == 1 ? 10: 20", &res) && val_is_number(res) &&  10 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "a + b > 10", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "a - b < 10", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "a * b == 10", &res) && val_is_boolean(res) &&  val_is_true(res));

    eval_env_deinit(env);
}

static void test_eval_var(void)
{
    eval_env_t env_st, *env = &env_st;
    val_t *res;
    val_t *p;

    CU_ASSERT_FATAL(0 == eval_env_init_mini(&env_st, mem_buf, 16384, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 == eval_string(env, "var a;", &res) && val_is_undefined(res));
    CU_ASSERT(-1 != eval_env_get_var(env, "a", &p) && val_is_undefined(p));
    CU_ASSERT(0 == eval_string(env, "a;", &res) && val_is_undefined(res));

    CU_ASSERT(0 == eval_string(env, "var b = 2;", &res) && val_is_undefined(res));
    CU_ASSERT(-1 != eval_env_get_var(env, "b", &p) && val_is_number(p) &&  2 == val_2_double(p));
    CU_ASSERT(0 == eval_string(env, "b;", &res) && 2 == val_2_double(res));

    CU_ASSERT(0 == eval_string(env, "var c = d = 3, e = b + c + d, f;", &res) && val_is_undefined(res));
    CU_ASSERT(-1 != eval_env_get_var(env, "c", &p) && val_is_number(p) &&  3 == val_2_double(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "d", &p) && val_is_number(p) &&  3 == val_2_double(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "e", &p) && val_is_number(p) &&  8 == val_2_double(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "f", &p) && val_is_undefined(p));

    // redefine f
    CU_ASSERT(0 == eval_string(env, "var f, g, h, k = 0, j, s, m;", &res) && val_is_undefined(res));
    CU_ASSERT(-1 != eval_env_get_var(env, "f", &p) && val_is_undefined(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "g", &p) && val_is_undefined(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "h", &p) && val_is_undefined(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "k", &p) && val_is_number(p) &&  0 == val_2_double(p));
    /*
    */

    eval_env_deinit(env);
}

static void test_eval_if(void)
{
    eval_env_t env_st, *env = &env_st;
    val_t *res;
    val_t *p,v;

    CU_ASSERT_FATAL(0 == eval_env_init_mini(&env_st, mem_buf, 16384, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    v = val_mk_number(0);
    CU_ASSERT(-1 != eval_env_add_var(env, "a", &v));
    CU_ASSERT(-1 != eval_env_add_var(env, "d", &v));
    v = val_mk_number(1);
    CU_ASSERT(-1 != eval_env_add_var(env, "b", &v));
    CU_ASSERT(-1 != eval_env_add_var(env, "c", &v));

    CU_ASSERT(0 == eval_string(env, "if (a == 0) d = 3", &res) && val_is_undefined(res));
    CU_ASSERT(-1 != eval_env_get_var(env, "d", &p) && val_is_number(p) &&  3 == val_2_double(p));

    CU_ASSERT(0 == eval_string(env, "if (a) b = 2 else c = 9", &res) && val_is_undefined(res));
    CU_ASSERT(-1 != eval_env_get_var(env, "b", &p) && val_is_number(p) &&  1 == val_2_double(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "c", &p) && val_is_number(p) &&  9 == val_2_double(p));

    CU_ASSERT(0 == eval_string(env, "if (a == 0) b = 9 else c = 2", &res) && val_is_undefined(res));
    CU_ASSERT(-1 != eval_env_get_var(env, "b", &p) && val_is_number(p) &&  9 == val_2_double(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "c", &p) && val_is_number(p) &&  9 == val_2_double(p));

    // nest
    CU_ASSERT(0 == eval_string(env, "if (a) d = 9 else if (b == c) d = 9 else a = 9", &res) && val_is_undefined(res));
    CU_ASSERT(-1 != eval_env_get_var(env, "a", &p) && val_is_number(p) &&  0 == val_2_double(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "b", &p) && val_is_number(p) &&  9 == val_2_double(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "c", &p) && val_is_number(p) &&  9 == val_2_double(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "d", &p) && val_is_number(p) &&  9 == val_2_double(p));

    CU_ASSERT(0 == eval_string(env, "if (a == 0) if (b != c) d = 9 else a = 9", &res) && val_is_undefined(res));
    CU_ASSERT(-1 != eval_env_get_var(env, "a", &p) && val_is_number(p) &&  9 == val_2_double(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "b", &p) && val_is_number(p) &&  9 == val_2_double(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "c", &p) && val_is_number(p) &&  9 == val_2_double(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "d", &p) && val_is_number(p) &&  9 == val_2_double(p));

    // empty
    CU_ASSERT(0 == eval_string(env, "if (a) {} else {}", &res));// && val_is_undefined(res));
    CU_ASSERT(-1 != eval_env_get_var(env, "a", &p) && val_is_number(p) &&  9 == val_2_double(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "b", &p) && val_is_number(p) &&  9 == val_2_double(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "c", &p) && val_is_number(p) &&  9 == val_2_double(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "d", &p) && val_is_number(p) &&  9 == val_2_double(p));

    // block
    CU_ASSERT(0 == eval_string(env, "if (a == 9) { b = 1; c = 1;} else {d = 2}", &res) && val_is_undefined(res));
    CU_ASSERT(-1 != eval_env_get_var(env, "a", &p) && val_is_number(p) &&  9 == val_2_double(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "b", &p) && val_is_number(p) &&  1 == val_2_double(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "c", &p) && val_is_number(p) &&  1 == val_2_double(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "d", &p) && val_is_number(p) &&  9 == val_2_double(p));

    CU_ASSERT(0 == eval_string(env, "if (!a) { b = 2;} else { c = 3; d = 3}", &res) && val_is_undefined(res));
    CU_ASSERT(-1 != eval_env_get_var(env, "a", &p) && val_is_number(p) &&  9 == val_2_double(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "b", &p) && val_is_number(p) &&  1 == val_2_double(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "c", &p) && val_is_number(p) &&  3 == val_2_double(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "d", &p) && val_is_number(p) &&  3 == val_2_double(p));
    /*
    */

    eval_env_deinit(env);
}

static void test_eval_while(void)
{
    eval_env_t env_st, *env = &env_st;
    val_t *res;
    val_t *p,v;

    CU_ASSERT_FATAL(0 == eval_env_init_mini(&env_st, mem_buf, 16384, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    v = val_mk_number(0);
    CU_ASSERT(-1 != eval_env_add_var(env, "a", &v));
    v = val_mk_number(9);
    CU_ASSERT(-1 != eval_env_add_var(env, "b", &v));

    CU_ASSERT(0 == eval_string(env, "while(a) {a = 1}", &res) && val_is_undefined(res));
    CU_ASSERT(-1 != eval_env_get_var(env, "a", &p) && val_is_number(p) &&  0 == val_2_double(p));

    CU_ASSERT(-1 != eval_env_get_var(env, "b", &p) && val_is_number(p) &&  9 == val_2_double(p));
    CU_ASSERT(0 == eval_string(env, "while (b) b = b - 1", &res) && val_is_undefined(res));
    CU_ASSERT(-1 != eval_env_get_var(env, "b", &p) && val_is_number(p) &&  0 == val_2_double(p));

    CU_ASSERT(-1 != eval_env_get_var(env, "b", &p) && val_is_number(p) &&  0 == val_2_double(p));
    CU_ASSERT(0 == eval_string(env, "while (b < 10) {b = b + 1 a = a + 1}", &res) && val_is_undefined(res));
    CU_ASSERT(-1 != eval_env_get_var(env, "a", &p) && val_is_number(p) &&  10 == val_2_double(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "b", &p) && val_is_number(p) &&  10 == val_2_double(p));

    // nest
    v = val_mk_number(9);
    CU_ASSERT(-1 != eval_env_set_var(env, "a", &v));
    v = val_mk_number(0);
    CU_ASSERT(-1 != eval_env_add_var(env, "c", &v));
    CU_ASSERT(0 == eval_string(env, "while (a) {a=a-1; b=9; while(b){b=b-1;c=c+1;}}", &res) && val_is_undefined(res));
    CU_ASSERT(-1 != eval_env_get_var(env, "c", &p) && val_is_number(p) && 81 == val_2_double(p));

    // continue
    v = val_mk_number(0);
    CU_ASSERT(-1 != eval_env_set_var(env, "a", &v)); 
    CU_ASSERT(-1 != eval_env_set_var(env, "b", &v));
    CU_ASSERT(0 == eval_string(env, "while (b < 10) {b = b + 1; if (b == 5) continue; a = a + 1}", &res) && val_is_undefined(res));
    CU_ASSERT(-1 != eval_env_get_var(env, "b", &p) && val_is_number(p) &&  10 == val_2_double(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "a", &p) && val_is_number(p) &&  9 == val_2_double(p));

    // break;
    CU_ASSERT(0 == eval_string(env, "while (a) {a = a - 1; if (a == 5) break; }", &res) && val_is_undefined(res));
    CU_ASSERT(-1 != eval_env_get_var(env, "a", &p) && val_is_number(p) &&  5 == val_2_double(p));

    // compose
    v = val_mk_number(9);
    CU_ASSERT(-1 != eval_env_set_var(env, "a", &v));
    CU_ASSERT(-1 != eval_env_set_var(env, "b", &v));
    v = val_mk_number(0);
    CU_ASSERT(-1 != eval_env_set_var(env, "c", &v));
    CU_ASSERT(0 == eval_string(env, "while (a) {b=a; while(b){b=b-1;if(2*b==a){ c=c+1; break;}}; if(a+b==6)break; a=a-1;}", &res) && val_is_undefined(res));
    CU_ASSERT(-1 != eval_env_get_var(env, "a", &p) && val_is_number(p) &&  4 == val_2_double(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "b", &p) && val_is_number(p) &&  2 == val_2_double(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "c", &p) && val_is_number(p) &&  3 == val_2_double(p));

    eval_env_deinit(env);
}

static void test_eval_function(void)
{
    eval_env_t env_st, *env = &env_st;
    val_t *res;
    val_t *p,v;

    char *fib = "def fib(max) {     \
                   var a = 1, b = 1;\
                   while(b < max) { \
                       var c = b;   \
                       b = b + a;   \
                       a = c;       \
                   }                \
                    return b;       \
                 }";
/*
    char *fff = "def fff(n) {                   \
                     if (n > 1)                 \
                         return n * fff(n - 1)  \
                     return 1;                  \
                  }";
*/

    CU_ASSERT_FATAL(0 == eval_env_init_mini(&env_st, mem_buf, 16384, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    v = val_mk_number(1);
    CU_ASSERT(-1 != eval_env_add_var(env, "a", &v));
    v = val_mk_number(0);
    CU_ASSERT(-1 != eval_env_add_var(env, "b", &v));

    CU_ASSERT(-1 != eval_env_get_var(env, "a", &p) && val_is_number(p) &&  1 == val_2_double(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "b", &p) && val_is_number(p) &&  0 == val_2_double(p));

    CU_ASSERT(0 == eval_string(env, "def zero() return 0", &res) && val_is_function(res));
    CU_ASSERT(0 == eval_string(env, "b = a + zero() + 2", &res) && val_is_number(res) && 3 == val_2_double(res));
    CU_ASSERT(-1 != eval_env_get_var(env, "b", &p) && val_is_number(p) &&  3 == val_2_double(p));

    CU_ASSERT(0 == eval_string(env, "def fn(a, b) return a + b", &res) && val_is_function(res));
    CU_ASSERT(-1 != eval_env_get_var(env, "fn", &p) && val_is_function(p));
    CU_ASSERT(0 == eval_string(env, "fn(1, 3)", &res) && val_is_number(res) && 4 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "a = b = fn(1, 3)", &res) && val_is_number(res) && 4 == val_2_double(res));
    CU_ASSERT(-1 != eval_env_get_var(env, "a", &p) && val_is_number(p) &&  4 == val_2_double(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "b", &p) && val_is_number(p) &&  4 == val_2_double(p));

    CU_ASSERT(0 == eval_string(env, fib, &res) && val_is_function(res));
    CU_ASSERT(0 == eval_string(env, "a = fib(1000)", &res) && val_is_number(res) && 1597 == val_2_double(res));

    // closure & recursion
    // CU_ASSERT(0 == eval_string(env, fff, &res) && val_is_function(*res));
    // CU_ASSERT(0 == eval_string(env, "a = fff(10)", &res) && val_is_number(*res) && 362880 == val_2_double(*res));


    eval_env_deinit(env);
}

val_t test_native_one(env_t *env, int ac, val_t *av)
{
    return val_mk_number(1);
}

val_t test_native_add(env_t *env, int ac, val_t *av)
{
//    printf("native add be called!\n");
    if (ac == 0)
        return val_mk_nan();

    if (ac == 1)
        return av[0];

    return val_mk_number(val_2_integer(av) + val_2_integer(av + 1));
}

val_t test_native_fib(env_t *env, int ac, val_t *av)
{
    int a, b, c, max;

    if (ac < 1 || !val_is_number(av)) {
        return val_mk_nan();
    }
    max = val_2_integer(av);
    a = b = 1;

    while(b < max) {
        c = b; b = a + b; a = c;
    }

    return val_mk_number(b);
}

static void test_eval_native(void)
{
    eval_env_t env_st, *env = &env_st;
    val_t *res;
    val_t *p,v;

    CU_ASSERT_FATAL(0 == eval_env_init_mini(&env_st, mem_buf, 16384, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 <= eval_env_add_native(env, "add", test_native_add));
    CU_ASSERT(0 <= eval_env_add_native(env, "fib", test_native_fib));

    v = val_mk_number(1);
    CU_ASSERT(-1 != eval_env_add_var(env, "a", &v));
    CU_ASSERT(-1 != eval_env_add_var(env, "b", &v));
    v = val_mk_native((intptr_t)test_native_one);
    CU_ASSERT(-1 != eval_env_add_var(env, "one", &v));

    CU_ASSERT(0 == eval_string(env, "b = a + one()", &res) && val_is_number(res) && 2 == val_2_double(res));
    CU_ASSERT(-1 != eval_env_get_var(env, "b", &p) && val_is_number(p) &&  2 == val_2_double(p));

    CU_ASSERT(0 == eval_string(env, "a = b + add(a, b);", &res) && val_is_number(res) && 5 == val_2_double(res));
    CU_ASSERT(-1 != eval_env_get_var(env, "a", &p) && val_is_number(p) &&  5 == val_2_double(p));

    CU_ASSERT(0 == eval_string(env, "a = one()", &res) && val_is_number(res) && 1 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "b = 999", &res) && val_is_number(res) && 999 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "fib(add(a, b))", &res) && val_is_number(res) && 1597 == val_2_double(res));

    CU_ASSERT(0 == eval_string(env, "a = (0 + add(b, one())) * 1;", &res) && val_is_number(res) && 1000 == val_2_double(res));
    CU_ASSERT(-1 != eval_env_get_var(env, "a", &p) && val_is_number(p) &&  1000 == val_2_double(p));

    eval_env_deinit(env);
}

static void test_eval_native_call_script(void)
{
}

static void test_eval_string(void)
{
    eval_env_t env_st, *env = &env_st;
    val_t *res;
    val_t *p,v;

    CU_ASSERT_FATAL(0 == eval_env_init_mini(&env_st, mem_buf, 16384, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    v = val_mk_undefined();
    CU_ASSERT(-1 != eval_env_add_var(env, "a", &v));
    CU_ASSERT(-1 != eval_env_add_var(env, "c", &v));
    v = val_mk_static_string((intptr_t) "world");
    CU_ASSERT(-1 != eval_env_add_var(env, "b", &v));

    CU_ASSERT(0 == eval_string(env, "\"hello\"", &res) && val_is_string(res));
    CU_ASSERT(0 == eval_string(env, "a = \"hello\"", &res) && val_is_string(res));
    CU_ASSERT(-1 != eval_env_get_var(env, "a", &p) && val_is_string(p));
    CU_ASSERT(-1 != eval_env_get_var(env, "b", &p) && val_is_string(p));

    CU_ASSERT(0 == eval_string(env, "a == \"hello\"", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "a >= \"hello\"", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "b == \"world\"", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "b >= \"world\"", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 == eval_string(env, "a < b", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "b > a", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 == eval_string(env, "c = a + ' ' + b", &res) && val_is_string(res));
    CU_ASSERT(0 == eval_string(env, "c == 'hello world'", &res) && val_is_true(res));

    CU_ASSERT(0 == eval_string(env, "c ? true : false", &res) && val_is_true(res));
    CU_ASSERT(0 == eval_string(env, "'' ? true : false", &res) && !val_is_true(res));

    CU_ASSERT(0 == eval_string(env, "a.length", &res) && val_is_function(res));
    CU_ASSERT(0 == eval_string(env, "a.length()", &res) && val_is_number(res) && 5 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "c.indexOf", &res) && val_is_function(res));
    CU_ASSERT(0 == eval_string(env, "c.indexOf(a)", &res) && val_is_number(res) && 0 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "c.indexOf(b)", &res) && val_is_number(res) && 6 == val_2_double(res));
    CU_ASSERT(0 == eval_string(env, "true.toString().length()", &res) && val_is_number(res) && 4 == val_2_double(res));

    eval_env_deinit(env);
}

CU_pSuite test_lang_eval_entry()
{
    CU_pSuite suite = CU_add_suite("lang eval", test_setup, test_clean);

    if (suite) {
        CU_add_test(suite, "eval simple",       test_eval_simple);
        CU_add_test(suite, "eval calculate",    test_eval_calculate);
        CU_add_test(suite, "eval compare",      test_eval_compare);
        CU_add_test(suite, "eval logic",        test_eval_logic);
        CU_add_test(suite, "eval symbal",       test_eval_symbal);
        CU_add_test(suite, "eval var stmt",     test_eval_var);
        CU_add_test(suite, "eval if stmt",      test_eval_if);
        CU_add_test(suite, "eval while stmt",   test_eval_while);
        CU_add_test(suite, "eval function",     test_eval_function);
        CU_add_test(suite, "eval native",       test_eval_native);
        CU_add_test(suite, "eval native call",  test_eval_native_call_script);
        CU_add_test(suite, "eval string",       test_eval_string);
        if (0) {
        }
    }

    return suite;
}

