
#include <stdio.h>
#include <string.h>

#include "cunit/CUnit.h"
#include "cunit/CUnit_Basic.h"

#include "lang/interp.h"

#define STACK_SIZE      128
#define HEAP_SIZE       4096
#define ENV_BUF_SIZE    (sizeof(val_t) * STACK_SIZE + HEAP_SIZE + EXE_MEM_SPACE + SYMBAL_MEM_SPACE)

uint8_t env_buf[ENV_BUF_SIZE];

static int test_setup()
{
    return 0;
}

static int test_clean()
{
    return 0;
}

static void test_exec_simple(void)
{
    env_t env_st, *env = &env_st;
    val_t *res;

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env_st, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < interp_execute_string(env, "NaN", &res) && val_is_nan(res));
    CU_ASSERT(0 < interp_execute_string(env, "undefined", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "true", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "false", &res) && val_is_boolean(res) && !val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "0", &res) && val_is_number(res) && val_2_integer(res) == 0);
    CU_ASSERT(0 < interp_execute_string(env, "1", &res) && val_is_number(res) && val_2_integer(res) == 1);

    // float token is not supported by lex
    //CU_ASSERT(0 < interp_execute_string(env, "1.0001", &res) && val_is_number(*res) && val_2_double(*res) == 1.0001);

    env_deinit(env);
}

static void test_exec_calculate(void)
{
    env_t env_st, *env = &env_st;
    val_t *res;

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env_st, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < interp_execute_string(env, "-1", &res) && val_is_number(res) && -1 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "~0", &res) && val_is_number(res) && -1 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "!1", &res) && val_is_boolean(res) && !val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "!0", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "1 + 1", &res) && val_is_number(res) && 2 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "1 + -1", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "1 - 1", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "1 - 2", &res) && val_is_number(res) && -1 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "1 - -1", &res) && val_is_number(res) && 2 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "0 * 2", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "2 * 2", &res) && val_is_number(res) && 4 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "0 / 2", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "6 / 2", &res) && val_is_number(res) && 3 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "6 / 0", &res) && val_is_nan(res));
    CU_ASSERT(0 < interp_execute_string(env, "0 % 2", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "3 % 2", &res) && val_is_number(res) && 1 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "6 % 0", &res) && val_is_nan(res));

    CU_ASSERT(0 < interp_execute_string(env, "1 + 2 * 3", &res) && val_is_number(res) && 7 == val_2_integer(res));
    //printf("---------------------------------------------------\n");

    CU_ASSERT(0 < interp_execute_string(env, "2 + 4 / 2", &res) && val_is_number(res) && 4 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "9 - 4 % 2", &res) && val_is_number(res) && 9 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "(1 + 2) * 3", &res) && val_is_number(res) && 9 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "(2 + 4) / 2", &res) && val_is_number(res) && 3 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "(9 - 4) % 2", &res) && val_is_number(res) && 1 == val_2_integer(res));

    CU_ASSERT(0 < interp_execute_string(env, "0 & 0", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "1 & 0", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "0 & 1", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "1 & 1", &res) && val_is_number(res) && 1 == val_2_integer(res));

    CU_ASSERT(0 < interp_execute_string(env, "0 | 0", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "1 | 0", &res) && val_is_number(res) && 1 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "0 | 1", &res) && val_is_number(res) && 1 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "1 | 1", &res) && val_is_number(res) && 1 == val_2_integer(res));

    CU_ASSERT(0 < interp_execute_string(env, "0 ^ 0", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "1 ^ 0", &res) && val_is_number(res) && 1 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "0 ^ 1", &res) && val_is_number(res) && 1 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "1 ^ 1", &res) && val_is_number(res) && 0 == val_2_integer(res));

    CU_ASSERT(0 < interp_execute_string(env, "1 << 0", &res) && val_is_number(res) && 1 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "1 << 1", &res) && val_is_number(res) && 2 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "1 >> 0", &res) && val_is_number(res) && 1 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "1 >> 1", &res) && val_is_number(res) && 0 == val_2_integer(res));

    CU_ASSERT(0 < interp_execute_string(env, "(4 >> 1) * 5 - 7 % 3 + 6 / 2", &res) && val_is_number(res) &&
              ((4 >> 1) * 5 - 7 % 3 + 6 / 2) == val_2_integer(res));

    /*
    */
    env_deinit(env);
    return;
}

static void test_exec_compare(void)
{
    env_t env_st, *env = &env_st;
    val_t *res;

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env_st, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < interp_execute_string(env, "1 != 0", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "1 == 0", &res) && val_is_boolean(res) && !val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "1 > 0", &res)  && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "1 >= 0", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "1 >= 1", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "0 < 1", &res)  && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "0 <= 1", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "1 <= 1", &res) && val_is_boolean(res) &&  val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(env, "0 > 1 ? 0 : 1", &res) && val_is_number(res) &&  1 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "0 < 1 ? 0 : 1", &res) && val_is_number(res) &&  0 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "0 > 1 ? 0 ? 10: 20 : 1 ? 30: 40", &res) && val_is_number(res) &&  30 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "0 < 1 ? 0 ? 10: 20 : 1 ? 30: 40", &res) && val_is_number(res) &&  20 == val_2_double(res));

    env_deinit(env);
}

static void test_exec_logic(void)
{
    env_t env_st, *env = &env_st;
    val_t *res;

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env_st, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < interp_execute_string(env, "false && false", &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "false && true",  &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "true && false",  &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "true && true",   &res) && val_is_boolean(res) &&  val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(env, "false || false", &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "false || true",  &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "true || false",  &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "true || true",   &res) && val_is_boolean(res) &&  val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(env, "0 && 0", &res) && val_is_number(res) &&  0 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "0 && 1", &res) && val_is_number(res) &&  0 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "1 && 0", &res) && val_is_number(res) &&  0 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "1 && 1", &res) && val_is_number(res) &&  1 == val_2_double(res));

    CU_ASSERT(0 < interp_execute_string(env, "0 || 0", &res) && val_is_number(res) &&  0 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "0 || 1", &res) && val_is_number(res) &&  1 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "1 || 0", &res) && val_is_number(res) &&  1 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "1 || 1", &res) && val_is_number(res) &&  1 == val_2_double(res));

    CU_ASSERT(0 < interp_execute_string(env, "0 && true",  &res) && val_is_number(res)  &&  0 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "0 && false", &res) && val_is_number(res)  &&  0 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "1 && true",  &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "1 && false", &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "false && 0", &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "false && 1", &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "true  && 0", &res) && val_is_number(res)  &&  0 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "true  && 1", &res) && val_is_number(res)  &&  1 == val_2_double(res));

    CU_ASSERT(0 < interp_execute_string(env, "0 || true", &res)  && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "0 || false", &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "1 || true", &res)  && val_is_number(res)  &&  1 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "1 || false", &res) && val_is_number(res)  &&  1 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "false || 0", &res) && val_is_number(res)  &&  0 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "false || 1", &res) && val_is_number(res)  &&  1 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "true  || 0", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "true  || 1", &res) && val_is_boolean(res) &&  val_is_true(res));
    /*
    */

    env_deinit(env);
}
static void test_exec_symbal(void)
{
    env_t env_st, *env = &env_st;
    val_t *res;

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env_st, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < interp_execute_string(env, "var a = 5, b = 2", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "a", &res) && val_is_number(res) &&  5 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "b", &res) && val_is_number(res) &&  2 == val_2_double(res));

    CU_ASSERT(0 < interp_execute_string(env, "a == 5", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "b == 2", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "a != 0", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "b != 1", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "a != 5", &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "b != 2", &res) && val_is_boolean(res) &&  !val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(env, "a + 2", &res) && val_is_number(res) &&  7 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "a - 2", &res) && val_is_number(res) &&  3 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "a * 2", &res) && val_is_number(res) &&  10 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "a / 2", &res) && val_is_number(res) &&  5.0 / 2 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "a % 2", &res) && val_is_number(res) &&  1 == val_2_double(res));

    CU_ASSERT(0 < interp_execute_string(env, "5 + b", &res) && val_is_number(res) &&  7 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "5 - b", &res) && val_is_number(res) &&  3 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "5 * b", &res) && val_is_number(res) &&  10 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "5 / b", &res) && val_is_number(res) &&  5.0 / 2 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "5 % b", &res) && val_is_number(res) &&  1 == val_2_double(res));

    CU_ASSERT(0 < interp_execute_string(env, "a + b", &res) && val_is_number(res) &&  7 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "a - b", &res) && val_is_number(res) &&  3 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "a * b", &res) && val_is_number(res) &&  10 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "a / b", &res) && val_is_number(res) &&  5.0 / 2 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "a % b", &res) && val_is_number(res) &&  1 == val_2_double(res));

    CU_ASSERT(0 < interp_execute_string(env, "a = b = 1", &res) && val_is_number(res) &&  1 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "a == 1", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "b == 1", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "b == a", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(env, "a << 1", &res) && val_is_number(res) &&  2 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "a >> 1", &res) && val_is_number(res) &&  0 == val_2_double(res));

    CU_ASSERT(0 < interp_execute_string(env, "b = a == 1 ? 10: 20", &res) && val_is_number(res) &&  10 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "a + b > 10", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "a - b < 10", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "a * b == 10", &res) && val_is_boolean(res) &&  val_is_true(res));

    env_deinit(env);
}

static void test_exec_var(void)
{
    env_t env_st, *env = &env_st;
    val_t *res;


    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env_st, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < interp_execute_string(env, "var a;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "a;", &res) && val_is_undefined(res));

    CU_ASSERT(0 < interp_execute_string(env, "var b = 2;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "b;", &res) && 2 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "b == 2", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(env, "var c = d = 3, e = b + c + d, f;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "c == 3", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "d == 3", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "e == 8", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "f;", &res) && val_is_undefined(res));

    // redefine f
    CU_ASSERT(0 < interp_execute_string(env, "var f, g, h, k = 0, j, s, m;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "f;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "g;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "h;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "k;", &res) && val_is_number(res) && 0 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "j;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "s;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "m;", &res) && val_is_undefined(res));
    /*
    */

    env_deinit(env);
}

static void test_exec_if(void)
{
    env_t env_st, *env = &env_st;
    val_t *res;

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env_st, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < interp_execute_string(env, "var a = 0, b = 1, c = 1, d = 0;", &res) && val_is_undefined(res));

    CU_ASSERT(0 < interp_execute_string(env, "if (a == 0) d = 3", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "d == 3", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(env, "if (a) b = 2 else c = 9", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "b == 1", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "c == 9", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(env, "if (a == 0) b = 9 else c = 2", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "b == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "c == 9", &res) && val_is_boolean(res) && val_is_true(res));

    // nest
    CU_ASSERT(0 < interp_execute_string(env, "if (a) d = 9 else if (b == c) d = 9 else a = 9", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "a == 0", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "b == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "c == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "d == 9", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(env, "if (a == 0) if (b != c) d = 9 else a = 9", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "a == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "b == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "c == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "d == 9", &res) && val_is_boolean(res) && val_is_true(res));

    // empty
    CU_ASSERT(0 < interp_execute_string(env, "if (a) {} else {}", &res));// && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "a == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "b == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "c == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "d == 9", &res) && val_is_boolean(res) && val_is_true(res));

    // block
    CU_ASSERT(0 < interp_execute_string(env, "if (a == 9) { b = 1; c = 1;} else {d = 2}", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "a == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "b == 1", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "c == 1", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "d == 9", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(env, "if (!a) { b = 2;} else { c = 3; d = 3}", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "a == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "b == 1", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "c == 3", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "d == 3", &res) && val_is_boolean(res) && val_is_true(res));
    /*
    */

    env_deinit(env);
}

static void test_exec_while(void)
{
    env_t env_st, *env = &env_st;
    val_t *res;

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env_st, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < interp_execute_string(env, "var a = 0, b = 9;", &res) && val_is_undefined(res));

    CU_ASSERT(0 < interp_execute_string(env, "while(a) {a = 1}", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "a == 0", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(env, "while (b) b = b - 1", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "b == 0", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(env, "while (b < 10) {b = b + 1 a = a + 1}", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "a == 10 && b == 10", &res) && val_is_boolean(res) && val_is_true(res));

    // nest
    CU_ASSERT(0 < interp_execute_string(env, "var c = 0; a = 9;", &res) && val_is_number(res) && 9 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "while (a) {a=a-1; b=9; while(b){b=b-1;c=c+1;}}", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "c == 81", &res) && val_is_boolean(res) && val_is_true(res));

    // continue
    CU_ASSERT(0 < interp_execute_string(env, "a = 0, b = 0;", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "while (b < 10) {b = b + 1; if (b == 5) continue; a = a + 1}", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "a == 9 && b == 10", &res) && val_is_boolean(res) && val_is_true(res));

    // break;
    CU_ASSERT(0 < interp_execute_string(env, "while (a) {a = a - 1; if (a == 5) break; }", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "a == 5", &res) && val_is_boolean(res) && val_is_true(res));

    // compose
    CU_ASSERT(0 < interp_execute_string(env, "a = 9, b = 9, c = 0;", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(env, "while (a) {b=a; while(b){b=b-1;if(2*b==a){ c=c+1; break;}}; if(a+b==6)break; a=a-1;}", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "a == 4", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "b == 2", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "c == 3", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "a == 4 && b == 2 && c == 3", &res) && val_is_boolean(res) && val_is_true(res));

    env_deinit(env);
}

static void test_exec_function(void)
{
    env_t env_st, *env = &env_st;
    val_t *res;

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

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env_st, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < interp_execute_string(env, "var a = 1, b = 0;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "def zero() return 0", &res) && val_is_function(res));
    CU_ASSERT(0 < interp_execute_string(env, "b = a + zero() + 2", &res) && val_is_number(res) && 3 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "b == 3", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(env, "def fn(a, b) return a + b", &res) && val_is_function(res));
    CU_ASSERT(0 < interp_execute_string(env, "fn", &res) && val_is_function(res));
    CU_ASSERT(0 < interp_execute_string(env, "fn(1, 3)", &res) && val_is_number(res) && 4 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "a = b = fn(1, 3)", &res) && val_is_number(res) && 4 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "a == 4 && b == 4", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(env, fib, &res) && val_is_function(res));
    CU_ASSERT(0 < interp_execute_string(env, "a = fib(1000)", &res) && val_is_number(res) && 1597 == val_2_double(res));

    // closure & recursion
    // CU_ASSERT(0 < interp_execute_string(env, fff, &res) && val_is_function(*res));
    // CU_ASSERT(0 < interp_execute_string(env, "a = fff(10)", &res) && val_is_number(*res) && 362880 == val_2_double(*res));


    env_deinit(env);
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

static void test_exec_native(void)
{
    env_t env_st, *env = &env_st;
    val_t *res;
    native_t native_entry[] = {
        {"one", test_native_one},
        {"add", test_native_add},
        {"fib", test_native_fib}
    };

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env_st, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 == env_native_add(env, 3, native_entry));

    CU_ASSERT(0 < interp_execute_string(env, "var a = 1, b = 1;", &res));
    CU_ASSERT(0 < interp_execute_string(env, "b = a + one()", &res) && val_is_number(res) && 2 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "b == 2", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(env, "a = b + add(a, b);", &res) && val_is_number(res) && 5 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "a == 5", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(env, "a = one()", &res) && val_is_number(res) && 1 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "b = 999", &res) && val_is_number(res) && 999 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "fib(add(a, b))", &res) && val_is_number(res) && 1597 == val_2_double(res));

    CU_ASSERT(0 < interp_execute_string(env, "a = (0 + add(b, one())) * 1;", &res) && val_is_number(res) && 1000 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "a == 1000", &res) && val_is_boolean(res) && val_is_true(res));

    env_deinit(env);
}

static void test_exec_native_call_script(void)
{
}

static void test_interp_execute_string(void)
{
    env_t env_st, *env = &env_st;
    val_t *res;

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env_st, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < interp_execute_string(env, "var a, b = 'world', c;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "\"hello\"", &res) && val_is_string(res));
    CU_ASSERT(0 < interp_execute_string(env, "a = \"hello\"", &res) && val_is_string(res));

    CU_ASSERT(0 < interp_execute_string(env, "a == \"hello\"", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "a >= \"hello\"", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "b == \"world\"", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "b >= \"world\"", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(env, "a < b", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "b > a", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(env, "c = a + ' ' + b", &res) && val_is_string(res));
    CU_ASSERT(0 < interp_execute_string(env, "c == 'hello world'", &res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(env, "c ? true : false", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "'' ? true : false", &res) && !val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(env, "a.length", &res) && val_is_function(res));
    CU_ASSERT(0 < interp_execute_string(env, "a.length()", &res) && val_is_number(res) && 5 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "c.length()", &res) && val_is_number(res) && 11 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "c.indexOf", &res) && val_is_function(res));
    CU_ASSERT(0 < interp_execute_string(env, "c.indexOf(a)", &res) && val_is_number(res) && 0 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "c.indexOf(b)", &res) && val_is_number(res) && 6 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "true.toString().length()", &res) && val_is_number(res) && 4 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(env, "a[0] == 'h'", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "a[0].length()", &res) && val_is_number(res) && 1 == val_2_integer(res));

    env_deinit(env);
}

static void test_exec_gc(void)
{
    env_t env_st, *env = &env_st;
    val_t *res;

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env_st, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < interp_execute_string(env, "var a = 0, b = 'world', c = 'hello';", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "var d = c + ' ', e = b + '.', f;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "def add(a, b) {var n = 10; while(n) {n = n-1; a+b}return a + b}", &res) && val_is_function(res));
    CU_ASSERT(0 < interp_execute_string(env, "while (a < 1000) { f = add(d, e); a = a + 1}", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(env, "a == 1000", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(env, "d", &res) && val_is_string(res));
    CU_ASSERT(0 < interp_execute_string(env, "e", &res) && val_is_string(res));
    CU_ASSERT(0 < interp_execute_string(env, "f", &res) && val_is_string(res));
    CU_ASSERT(0 < interp_execute_string(env, "f == 'hello world.'", &res) && val_is_boolean(res) && val_is_true(res));

    env_deinit(env);
}

CU_pSuite test_lang_eval_entry()
{
    CU_pSuite suite = CU_add_suite("lang eval", test_setup, test_clean);

    if (suite) {
        CU_add_test(suite, "eval simple",       test_exec_simple);
        CU_add_test(suite, "eval calculate",    test_exec_calculate);
        CU_add_test(suite, "eval compare",      test_exec_compare);
        CU_add_test(suite, "eval logic",        test_exec_logic);
        CU_add_test(suite, "eval symbal",       test_exec_symbal);
        CU_add_test(suite, "eval var stmt",     test_exec_var);
        CU_add_test(suite, "eval if stmt",      test_exec_if);
        CU_add_test(suite, "eval while stmt",   test_exec_while);
        CU_add_test(suite, "eval function",     test_exec_function);
        CU_add_test(suite, "eval native",       test_exec_native);
        CU_add_test(suite, "eval native call",  test_exec_native_call_script);
        CU_add_test(suite, "eval string",       test_interp_execute_string);
        CU_add_test(suite, "eval gc",           test_exec_gc);
        if (0) {
        }
    }

    return suite;
}

