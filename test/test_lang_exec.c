
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
    env_t env;
    val_t *res;

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < interp_execute_string(&env, "NaN", &res) && val_is_nan(res));
    CU_ASSERT(0 < interp_execute_string(&env, "undefined", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "true", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "false", &res) && val_is_boolean(res) && !val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "0", &res) && val_is_number(res) && val_2_integer(res) == 0);
    CU_ASSERT(0 < interp_execute_string(&env, "1", &res) && val_is_number(res) && val_2_integer(res) == 1);

    // float token is not supported by lex
    //CU_ASSERT(0 < interp_execute_string((envenv, "1.0001", &res) && val_is_number(*res) && val_2_double(*res) == 1.0001);

    env_deinit(&env);
}

static void test_exec_calculate(void)
{
    env_t env;
    val_t *res;

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < interp_execute_string(&env, "-1", &res) && val_is_number(res) && -1 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "~0", &res) && val_is_number(res) && -1 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "!1", &res) && val_is_boolean(res) && !val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "!0", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "1 + 1", &res) && val_is_number(res) && 2 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "1 + -1", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "1 - 1", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "1 - 2", &res) && val_is_number(res) && -1 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "1 - -1", &res) && val_is_number(res) && 2 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "0 * 2", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "2 * 2", &res) && val_is_number(res) && 4 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "0 / 2", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "6 / 2", &res) && val_is_number(res) && 3 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "6 / 0", &res) && val_is_nan(res));
    CU_ASSERT(0 < interp_execute_string(&env, "0 % 2", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "3 % 2", &res) && val_is_number(res) && 1 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "6 % 0", &res) && val_is_nan(res));

    CU_ASSERT(0 < interp_execute_string(&env, "1 + 2 * 3", &res) && val_is_number(res) && 7 == val_2_integer(res));
    //printf("---------------------------------------------------\n");

    CU_ASSERT(0 < interp_execute_string(&env, "2 + 4 / 2", &res) && val_is_number(res) && 4 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "9 - 4 % 2", &res) && val_is_number(res) && 9 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "(1 + 2) * 3", &res) && val_is_number(res) && 9 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "(2 + 4) / 2", &res) && val_is_number(res) && 3 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "(9 - 4) % 2", &res) && val_is_number(res) && 1 == val_2_integer(res));

    CU_ASSERT(0 < interp_execute_string(&env, "0 & 0", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "1 & 0", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "0 & 1", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "1 & 1", &res) && val_is_number(res) && 1 == val_2_integer(res));

    CU_ASSERT(0 < interp_execute_string(&env, "0 | 0", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "1 | 0", &res) && val_is_number(res) && 1 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "0 | 1", &res) && val_is_number(res) && 1 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "1 | 1", &res) && val_is_number(res) && 1 == val_2_integer(res));

    CU_ASSERT(0 < interp_execute_string(&env, "0 ^ 0", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "1 ^ 0", &res) && val_is_number(res) && 1 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "0 ^ 1", &res) && val_is_number(res) && 1 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "1 ^ 1", &res) && val_is_number(res) && 0 == val_2_integer(res));

    CU_ASSERT(0 < interp_execute_string(&env, "1 << 0", &res) && val_is_number(res) && 1 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "1 << 1", &res) && val_is_number(res) && 2 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "1 >> 0", &res) && val_is_number(res) && 1 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "1 >> 1", &res) && val_is_number(res) && 0 == val_2_integer(res));

    CU_ASSERT(0 < interp_execute_string(&env, "(4 >> 1) * 5 - 7 % 3 + 6 / 2", &res) && val_is_number(res) &&
              ((4 >> 1) * 5 - 7 % 3 + 6 / 2) == val_2_integer(res));

    /*
    */
    env_deinit(&env);
    return;
}

static void test_exec_compare(void)
{
    env_t env;
    val_t *res;

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < interp_execute_string(&env, "1 != 0", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "1 == 0", &res) && val_is_boolean(res) && !val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "1 > 0", &res)  && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "1 >= 0", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "1 >= 1", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "0 < 1", &res)  && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "0 <= 1", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "1 <= 1", &res) && val_is_boolean(res) &&  val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "0 > 1 ? 0 : 1", &res) && val_is_number(res) &&  1 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "0 < 1 ? 0 : 1", &res) && val_is_number(res) &&  0 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "0 > 1 ? 0 ? 10: 20 : 1 ? 30: 40", &res) && val_is_number(res) &&  30 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "0 < 1 ? 0 ? 10: 20 : 1 ? 30: 40", &res) && val_is_number(res) &&  20 == val_2_double(res));

    env_deinit(&env);
}

static void test_exec_logic(void)
{
    env_t env;
    val_t *res;

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < interp_execute_string(&env, "false && false", &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "false && true",  &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "true && false",  &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "true && true",   &res) && val_is_boolean(res) &&  val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "false || false", &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "false || true",  &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "true || false",  &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "true || true",   &res) && val_is_boolean(res) &&  val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "0 && 0", &res) && val_is_number(res) &&  0 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "0 && 1", &res) && val_is_number(res) &&  0 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "1 && 0", &res) && val_is_number(res) &&  0 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "1 && 1", &res) && val_is_number(res) &&  1 == val_2_double(res));

    CU_ASSERT(0 < interp_execute_string(&env, "0 || 0", &res) && val_is_number(res) &&  0 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "0 || 1", &res) && val_is_number(res) &&  1 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "1 || 0", &res) && val_is_number(res) &&  1 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "1 || 1", &res) && val_is_number(res) &&  1 == val_2_double(res));

    CU_ASSERT(0 < interp_execute_string(&env, "0 && true",  &res) && val_is_number(res)  &&  0 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "0 && false", &res) && val_is_number(res)  &&  0 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "1 && true",  &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "1 && false", &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "false && 0", &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "false && 1", &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "true  && 0", &res) && val_is_number(res)  &&  0 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "true  && 1", &res) && val_is_number(res)  &&  1 == val_2_double(res));

    CU_ASSERT(0 < interp_execute_string(&env, "0 || true", &res)  && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "0 || false", &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "1 || true", &res)  && val_is_number(res)  &&  1 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "1 || false", &res) && val_is_number(res)  &&  1 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "false || 0", &res) && val_is_number(res)  &&  0 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "false || 1", &res) && val_is_number(res)  &&  1 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "true  || 0", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "true  || 1", &res) && val_is_boolean(res) &&  val_is_true(res));
    /*
    */

    env_deinit(&env);
}
static void test_exec_symbal(void)
{
    env_t env;
    val_t *res;

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < interp_execute_string(&env, "var a = 5, b = 2", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a", &res) && val_is_number(res) &&  5 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b", &res) && val_is_number(res) &&  2 == val_2_double(res));

    CU_ASSERT(0 < interp_execute_string(&env, "a == 5", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b == 2", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a != 0", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b != 1", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a != 5", &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b != 2", &res) && val_is_boolean(res) &&  !val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "a + 2", &res) && val_is_number(res) &&  7 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a - 2", &res) && val_is_number(res) &&  3 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a * 2", &res) && val_is_number(res) &&  10 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a / 2", &res) && val_is_number(res) &&  5.0 / 2 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a % 2", &res) && val_is_number(res) &&  1 == val_2_double(res));

    CU_ASSERT(0 < interp_execute_string(&env, "5 + b", &res) && val_is_number(res) &&  7 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "5 - b", &res) && val_is_number(res) &&  3 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "5 * b", &res) && val_is_number(res) &&  10 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "5 / b", &res) && val_is_number(res) &&  5.0 / 2 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "5 % b", &res) && val_is_number(res) &&  1 == val_2_double(res));

    CU_ASSERT(0 < interp_execute_string(&env, "a + b", &res) && val_is_number(res) &&  7 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a - b", &res) && val_is_number(res) &&  3 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a * b", &res) && val_is_number(res) &&  10 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a / b", &res) && val_is_number(res) &&  5.0 / 2 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a % b", &res) && val_is_number(res) &&  1 == val_2_double(res));

    CU_ASSERT(0 < interp_execute_string(&env, "a = b = 1", &res) && val_is_number(res) &&  1 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a == 1", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b == 1", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b == a", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "a << 1", &res) && val_is_number(res) &&  2 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a >> 1", &res) && val_is_number(res) &&  0 == val_2_double(res));

    CU_ASSERT(0 < interp_execute_string(&env, "b = a == 1 ? 10: 20", &res) && val_is_number(res) &&  10 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a + b > 10", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a - b < 10", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a * b == 10", &res) && val_is_boolean(res) &&  val_is_true(res));

    env_deinit(&env);
}

static void test_exec_var(void)
{
    env_t env;
    val_t *res;


    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < interp_execute_string(&env, "var a;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a;", &res) && val_is_undefined(res));

    CU_ASSERT(0 < interp_execute_string(&env, "var b = 2;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b;", &res) && 2 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b == 2", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "var c = d = 3, e = b + c + d, f;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c == 3", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "d == 3", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "e == 8", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f;", &res) && val_is_undefined(res));

    // redefine f
    CU_ASSERT(0 < interp_execute_string(&env, "var f, g, h, k = 0, j, s, m;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "g;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "h;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "k;", &res) && val_is_number(res) && 0 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "j;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "s;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "m;", &res) && val_is_undefined(res));

    env_deinit(&env);
}

static void test_exec_assign(void)
{
    env_t env;
    val_t *res;

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < interp_execute_string(&env, "var a = 1;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a += 1;", &res) && val_is_number(res) && 2 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a -= 1;", &res) && val_is_number(res) && 1 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a *= 2;", &res) && val_is_number(res) && 2 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a /= 2;", &res) && val_is_number(res) && 1 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a %= 2;", &res) && val_is_number(res) && 1 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a &= 2;", &res) && val_is_number(res) && 0 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a |= 2;", &res) && val_is_number(res) && 2 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a ^= 1;", &res) && val_is_number(res) && 3 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a <<= 1;", &res) && val_is_number(res) && 6 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a >>= 1;", &res) && val_is_number(res) && 3 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a == 3;", &res) && val_is_true(res));

    /*
    CU_ASSERT(0 < interp_execute_string(&env, "var b = [1];", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b[0] += 1;", &res) && val_is_number(res) && 2 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b[0] -= 1;", &res) && val_is_number(res) && 1 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b[0] *= 2;", &res) && val_is_number(res) && 2 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b[0] /= 2;", &res) && val_is_number(res) && 1 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b[0] %= 2;", &res) && val_is_number(res) && 1 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b[0] &= 2;", &res) && val_is_number(res) && 0 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b[0] |= 2;", &res) && val_is_number(res) && 2 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b[0] ^= 1;", &res) && val_is_number(res) && 3 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b[0] <<= 1;", &res) && val_is_number(res) && 6 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b[0] >>= 1;", &res) && val_is_number(res) && 3 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b[0] == 3;", &res) && val_is_true(res));
    */

    CU_ASSERT(0 < interp_execute_string(&env, "var c = {a: 1};", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c.a += 1;", &res) && val_is_number(res) && 2 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c.a -= 1;", &res) && val_is_number(res) && 1 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c.a *= 2;", &res) && val_is_number(res) && 2 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c.a /= 2;", &res) && val_is_number(res) && 1 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c.a %= 2;", &res) && val_is_number(res) && 1 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c.a &= 2;", &res) && val_is_number(res) && 0 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c.a |= 2;", &res) && val_is_number(res) && 2 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c.a ^= 1;", &res) && val_is_number(res) && 3 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c.a <<= 1;", &res) && val_is_number(res) && 6 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c.a >>= 1;", &res) && val_is_number(res) && 3 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c.a == 3;", &res) && val_is_true(res));

    env_deinit(&env);
}

static void test_exec_if(void)
{
    env_t env;
    val_t *res;

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < interp_execute_string(&env, "var a = 0, b = 1, c = 1, d = 0;", &res) && val_is_undefined(res));

    CU_ASSERT(0 < interp_execute_string(&env, "if (a == 0) d = 3", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "d == 3", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "if (a) b = 2 else c = 9", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b == 1", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c == 9", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "if (a == 0) b = 9 else c = 2", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c == 9", &res) && val_is_boolean(res) && val_is_true(res));

    // nest
    CU_ASSERT(0 < interp_execute_string(&env, "if (a) d = 9 else if (b == c) d = 9 else a = 9", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a == 0", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "d == 9", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "if (a == 0) if (b != c) d = 9 else a = 9", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "d == 9", &res) && val_is_boolean(res) && val_is_true(res));

    // empty
    CU_ASSERT(0 < interp_execute_string(&env, "if (a) {} else {}", &res));// && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "d == 9", &res) && val_is_boolean(res) && val_is_true(res));

    // block
    CU_ASSERT(0 < interp_execute_string(&env, "if (a == 9) { b = 1; c = 1;} else {d = 2}", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b == 1", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c == 1", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "d == 9", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "if (!a) { b = 2;} else { c = 3; d = 3}", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b == 1", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c == 3", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "d == 3", &res) && val_is_boolean(res) && val_is_true(res));
    /*
    */

    env_deinit(&env);
}

static void test_exec_while(void)
{
    env_t env;
    val_t *res;

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < interp_execute_string(&env, "var a = 0, b = 9;", &res) && val_is_undefined(res));

    CU_ASSERT(0 < interp_execute_string(&env, "while(a) {a = 1}", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a == 0", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "while (b) b = b - 1", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b == 0", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "while (b < 10) {b = b + 1 a = a + 1}", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a == 10 && b == 10", &res) && val_is_boolean(res) && val_is_true(res));

    // nest
    CU_ASSERT(0 < interp_execute_string(&env, "var c = 0; a = 9;", &res) && val_is_number(res) && 9 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "while (a) {a=a-1; b=9; while(b){b=b-1;c=c+1;}}", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c == 81", &res) && val_is_boolean(res) && val_is_true(res));

    // continue
    CU_ASSERT(0 < interp_execute_string(&env, "a = 0, b = 0;", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "while (b < 10) {b = b + 1; if (b == 5) continue; a = a + 1}", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a == 9 && b == 10", &res) && val_is_boolean(res) && val_is_true(res));

    // break;
    CU_ASSERT(0 < interp_execute_string(&env, "while (a) {a = a - 1; if (a == 5) break; }", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a == 5", &res) && val_is_boolean(res) && val_is_true(res));

    // compose
    CU_ASSERT(0 < interp_execute_string(&env, "a = 9, b = 9, c = 0;", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "while (a) {b=a; while(b){b=b-1;if(2*b==a){ c=c+1; break;}}; if(a+b==6)break; a=a-1;}", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a == 4", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b == 2", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c == 3", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a == 4 && b == 2 && c == 3", &res) && val_is_boolean(res) && val_is_true(res));

    env_deinit(&env);
}

static void test_exec_function(void)
{
    env_t env;
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

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    //printf("-------------------------------------------\n");
    CU_ASSERT(0 < interp_execute_string(&env, "var a = 1, b = 0;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "def zero() return 0", &res) && val_is_function(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b = a + zero() + 2", &res) && val_is_number(res) && 3 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b == 3", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "def fn(a, b) return a + b", &res) && val_is_function(res));
    CU_ASSERT(0 < interp_execute_string(&env, "fn", &res) && val_is_function(res));
    CU_ASSERT(0 < interp_execute_string(&env, "fn(1, 3)", &res) && val_is_number(res) && 4 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a = b = fn(1, 3)", &res) && val_is_number(res) && 4 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a == 4 && b == 4", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, fib, &res) && val_is_function(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a = fib(1000)", &res) && val_is_number(res) && 1597 == val_2_double(res));
    if (0) {
    }

    // closure & recursion
    // CU_ASSERT(0 < interp_execute_string(&env, fff, &res) && val_is_function(*res));
    // CU_ASSERT(0 < interp_execute_string(&env, "a = fff(10)", &res) && val_is_number(*res) && 362880 == val_2_double(*res));


    env_deinit(&env);
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
    env_t env;
    val_t *res;
    native_t native_entry[] = {
        {"one", test_native_one},
        {"add", test_native_add},
        {"fib", test_native_fib}
    };

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 == env_native_add(&env, 3, native_entry));

    CU_ASSERT(0 < interp_execute_string(&env, "var a = 1, b = 1;", &res));
    CU_ASSERT(0 < interp_execute_string(&env, "b = a + one()", &res) && val_is_number(res) && 2 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b == 2", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "a = b + add(a, b);", &res) && val_is_number(res) && 5 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a == 5", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "a = one()", &res) && val_is_number(res) && 1 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b = 999", &res) && val_is_number(res) && 999 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "fib(add(a, b))", &res) && val_is_number(res) && 1597 == val_2_double(res));

    CU_ASSERT(0 < interp_execute_string(&env, "a = (0 + add(b, one())) * 1;", &res) && val_is_number(res) && 1000 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a == 1000", &res) && val_is_boolean(res) && val_is_true(res));

    env_deinit(&env);
}

static val_t test_native_call(env_t *env, int ac, val_t *av)
{
    if (ac > 0 && val_is_function(av)) {
        int i;

        for (i = 0; i < ac -1; i++) {
            // push arguments from last to first
            env_push_call_argument(env, av + ac - i - 1);
        }
        env_push_call_function(env, av);

        return interp_execute_call(env, ac - 1);
    } else {
        return val_mk_undefined();
    }
}

static void test_exec_native_call_script(void)
{
    env_t env;
    val_t *res;
    native_t native_entry[] = {
        {"one", test_native_one},
        {"call", test_native_call}
    };

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 == env_native_add(&env, 2, native_entry));

    CU_ASSERT(0 < interp_execute_string(&env, "var a = 0, b = 1, c = 2", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "def zero() return 0;", &res) && val_is_function(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a = call(zero)", &res));
    CU_ASSERT(0 < interp_execute_string(&env, "a == 0", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a = call(one)", &res));
    CU_ASSERT(0 < interp_execute_string(&env, "a == 1", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "def add(a, b) return a + b;", &res) && val_is_function(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a = call(add, b, c)", &res));
    CU_ASSERT(0 < interp_execute_string(&env, "a == 3", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "def add2(x) return add(b + c, x);", &res) && val_is_function(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a = call(add2, 1)", &res));
    CU_ASSERT(0 < interp_execute_string(&env, "a == 4", &res) && val_is_boolean(res) && val_is_true(res));

    env_deinit(&env);
}

static void test_exec_string(void)
{
    env_t env;
    val_t *res;

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < interp_execute_string(&env, "var a, b = 'world', c;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "\"hello\"", &res) && val_is_string(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a = \"hello\"", &res) && val_is_string(res));

    CU_ASSERT(0 < interp_execute_string(&env, "a == \"hello\"", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a >= \"hello\"", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b == \"world\"", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b >= \"world\"", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "a < b", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b > a", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "c = a + ' ' + b", &res) && val_is_string(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c == 'hello world'", &res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "c ? true : false", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "'' ? true : false", &res) && !val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "a.length", &res) && val_is_function(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a.length()", &res) && val_is_number(res) && 5 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c.length()", &res) && val_is_number(res) && 11 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c.indexOf", &res) && val_is_function(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c.indexOf(a)", &res) && val_is_number(res) && 0 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c.indexOf(b)", &res) && val_is_number(res) && 6 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "true.toString().length()", &res) && val_is_number(res) && 4 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a[0] == 'h'", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a[0].length()", &res) && val_is_number(res) && 1 == val_2_integer(res));

    env_deinit(&env);
}

static void test_exec_dict(void)
{
    env_t env;
    val_t *res;

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < interp_execute_string(&env, "var a = {a: 1, b: 'hello', c: 2 * 2 - 1};", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a", &res) && val_is_dictionary(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a.a == 1", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a.b == \"hello\"", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a.c == 3", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "a.a = 'world'", &res) && val_is_string(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a.a == 'world'", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a.a + a.b == 'worldhello'", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "var b = {a: 1, b: def(self) return self.a};", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b", &res) && val_is_dictionary(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b.b() == 1", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "var c = {}", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c", &res) && val_is_dictionary(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c.a", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "(c.a = 1) == 1", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "(c.b = 'hello') == 'hello'", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "(c.c = true) == true", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "(c.d = false) == false", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c.e = def() return 0", &res) && val_is_function(res));

    CU_ASSERT(0 < interp_execute_string(&env, "var o = {'a': a, 'b': b, 'c': c}", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "o.a == a", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "o.b == b", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "o.c == c", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "o.a.a == 'world'", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "o.b.b() == 1", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "o.c.e() == 0", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "o = {'a': 'hello', 'b': ' ', 'c': 'world'}", &res) && val_is_dictionary(res));
    CU_ASSERT(0 < interp_execute_string(&env, "var ks = '', vs = ''", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "o.foreach(def(v, k) {ks = ks + k; vs = vs + v })", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "ks == 'abc'", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "vs == 'hello world'", &res) && val_is_boolean(res) && val_is_true(res));

    /*
    */

    env_deinit(&env);
}

static void test_exec_array(void)
{
    env_t env;
    val_t *res;

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < interp_execute_string(&env, "var a = [0, 'hello', [1, 2], {a: 0}], b = [], c = [1], d = [1, 2];", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a", &res) && val_is_array(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b", &res) && val_is_array(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c", &res) && val_is_array(res));
    CU_ASSERT(0 < interp_execute_string(&env, "d", &res) && val_is_array(res));

    CU_ASSERT(0 < interp_execute_string(&env, "a.length() == 4", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b.length() == 0", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c.length() == 1", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "d.length() == 2", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "a[0] == 0", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a[1] == 'hello'", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a[2][0] == 1", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a[2][1] == 2", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a[3].a == 0", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "a[0] = a[1]", &res) && val_is_string(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a[1] = a[2]", &res) && val_is_array(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a[2] = a[3]", &res) && val_is_dictionary(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a[0] == 'hello'", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a[1][0] == 1", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a[1][1] == 2", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a[2].a == 0", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a[2] == a[3]", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "var o = a.pop();", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a.length() == 3", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "o == a[2]", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "a.push(o) == 4", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "o == a[3]", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a.push(1) == 5", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "1 == a[4]", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "var s = a.shift();", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a.length() == 4", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "'hello' == s", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a.shift()[1] == 2", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a.length() == 3", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a[0].a == 0", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a[0].b = 1", &res) && val_is_number(res) && 1 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a[1] = 1", &res) && val_is_number(res) && 1 == val_2_double(res));

    CU_ASSERT(0 < interp_execute_string(&env, "a.unshift(s) == 4", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "s == a[0]", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a[1].a == 0", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "var sum = 0;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a = [1, 2, 3, 4, 5]", &res) && val_is_array(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a.foreach(def(v) sum = sum + v)", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "sum == 15", &res) && val_is_boolean(res) && val_is_true(res));

    env_deinit(&env);
}

static void test_exec_closure(void)
{
    env_t env;
    val_t *res;

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < interp_execute_string(&env, "var a = 0;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "def f() {return a};", &res) && val_is_function(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f()", &res) && val_is_number(res) && 0 == val_2_double(res));

    CU_ASSERT(0 < interp_execute_string(&env, "def inc(){return a = a + 1}", &res) && val_is_function(res));
    CU_ASSERT(0 < interp_execute_string(&env, "inc()", &res) && val_is_number(res) && 1 == val_2_double(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a == 1", &res) && val_is_true(res));


    CU_ASSERT(0 < interp_execute_string(&env, "def adder(x){return def(b){return a + x + b}}", &res) && val_is_function(res));
    CU_ASSERT(0 < interp_execute_string(&env, "var b = adder(10)", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "b(100) == 111", &res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "def fact(x){ if(x > 1) return x * fact(x - 1) else return 1}", &res) && val_is_function(res));
    CU_ASSERT(0 < interp_execute_string(&env, "fact(5) == 120", &res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "def cover(x){cover = def () return 1; return 0}", &res) && val_is_function(res));
    CU_ASSERT(0 < interp_execute_string(&env, "cover() == 0", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "cover() == 1", &res) && val_is_true(res));
    /*
    */

    env_deinit(&env);
}

static void test_exec_stack_check(void)
{
    env_t env;
    val_t *res;

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < interp_execute_string(&env, "var a = 0, b = 1, c = 2, d = 3;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "def deep() { return a + b + c + deep()}", &res) && val_is_function(res));
    CU_ASSERT(-ERR_StackOverflow == interp_execute_string(&env, "deep()", &res));

    env_deinit(&env);
}

static void test_exec_func_arg(void)
{
    env_t env;
    val_t *res;

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < interp_execute_string(&env, "def narg(a) { return a ? 1 : 0 }", &res) && val_is_function(res));
    CU_ASSERT(0 < interp_execute_string(&env, "narg() == 0", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "narg('a') == 1", &res) && val_is_true(res));

    env_deinit(&env);
}

static void test_exec_gc(void)
{
    env_t env;
    val_t *res;

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < interp_execute_string(&env, "var n = 0;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "var b = 'world', c = 'hello';", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "var d = c + ' ', e = b + '.', f;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "var a = [b, c, 0];", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "var o = {a: b, b: c};", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "def add(a, b) {var n = 10; while(n) {n = n-1; a+b} return a + b}", &res) && val_is_function(res));
    CU_ASSERT(0 < interp_execute_string(&env, "def join(){return e + c}", &res) && val_is_function(res));

    CU_ASSERT(0 < interp_execute_string(&env, "a[0] == b", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a[1] == c", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "o.a == b", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "o.b == c", &res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "while (n < 100) { f = add(d, e); n = n + 1}", &res) && val_is_undefined(res));
    CU_ASSERT(0 < interp_execute_string(&env, "n == 100", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "d", &res) && val_is_string(res));
    CU_ASSERT(0 < interp_execute_string(&env, "e", &res) && val_is_string(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f", &res) && val_is_string(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f == 'hello world.'", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "n = 0", &res) && val_is_number(res));
    CU_ASSERT(0 < interp_execute_string(&env, "while (n < 1000) { f = join(); n = n + 1}", &res));
    CU_ASSERT(0 < interp_execute_string(&env, "join() == 'world.hello'", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "join() == e + c", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f == e + c", &res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "n = 0", &res) && val_is_number(res));
    CU_ASSERT(0 < interp_execute_string(&env, "while (n < 1000) { f = o.a + o.b; n = n + 1}", &res));
    CU_ASSERT(0 < interp_execute_string(&env, "f == 'worldhello'", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "n = 0", &res) && val_is_number(res));
    CU_ASSERT(0 < interp_execute_string(&env, "while (n < 1000) { f = a[0] + a[1]; n = n + 1; a[2] = n;}", &res));
    CU_ASSERT(0 < interp_execute_string(&env, "f == 'worldhello'", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a[2] == 1000", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "b == 'world'", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "c == 'hello'", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "o.a == b", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "o.b == c", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a[0] == b", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a[1] == c", &res) && val_is_true(res));


    env_deinit(&env);
}

CU_pSuite test_lang_interp_entry()
{
    CU_pSuite suite = CU_add_suite("lang eval", test_setup, test_clean);

    if (suite) {
        CU_add_test(suite, "exec simple",       test_exec_simple);
        CU_add_test(suite, "exec calculate",    test_exec_calculate);
        CU_add_test(suite, "exec compare",      test_exec_compare);
        CU_add_test(suite, "exec logic",        test_exec_logic);
        CU_add_test(suite, "exec symbal",       test_exec_symbal);
        CU_add_test(suite, "exec var stmt",     test_exec_var);
        CU_add_test(suite, "exec assign",       test_exec_assign);
        CU_add_test(suite, "exec if stmt",      test_exec_if);
        CU_add_test(suite, "exec while stmt",   test_exec_while);
        CU_add_test(suite, "exec function",     test_exec_function);
        CU_add_test(suite, "exec native",       test_exec_native);
        CU_add_test(suite, "exec native call",  test_exec_native_call_script);
        CU_add_test(suite, "exec string",       test_exec_string);
        CU_add_test(suite, "exec dictionary",   test_exec_dict);
        CU_add_test(suite, "exec array",        test_exec_array);
        CU_add_test(suite, "exec closure",      test_exec_closure);
        CU_add_test(suite, "exec stack check",  test_exec_stack_check);
        CU_add_test(suite, "exec function arg", test_exec_func_arg);
        CU_add_test(suite, "exec gc",           test_exec_gc);
        if (0) {
        }
    }

    return suite;
}

