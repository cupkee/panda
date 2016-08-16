
#include <stdio.h>
#include <string.h>

#include "cunit/CUnit.h"
#include "cunit/CUnit_Basic.h"

#include "lang/exec.h"

#define STACK_SIZE      128
#define HEAP_SIZE       4096
#define ENV_BUF_SIZE    (sizeof(val_t) * STACK_SIZE + HEAP_SIZE + EXE_MEM_SPACE + SYMBAL_MEM_SPACE)
#define EXEC_BUF_SIZE   128

uint8_t env_buf[ENV_BUF_SIZE];
char    exec_buf[EXEC_BUF_SIZE];

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

    CU_ASSERT_FATAL(0 == exec_env_init(&env_st, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "NaN", &res) && val_is_nan(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "undefined", &res) && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "true", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "false", &res) && val_is_boolean(res) && !val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "0", &res) && val_is_number(res) && val_2_integer(res) == 0);
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1", &res) && val_is_number(res) && val_2_integer(res) == 1);

    // float token is not supported by lex
    //CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1.0001", &res) && val_is_number(*res) && val_2_double(*res) == 1.0001);

    env_deinit(env);
}

static void test_exec_calculate(void)
{
    env_t env_st, *env = &env_st;
    val_t *res;

    CU_ASSERT_FATAL(0 == exec_env_init(&env_st, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "-1", &res) && val_is_number(res) && -1 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "~0", &res) && val_is_number(res) && -1 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "!1", &res) && val_is_boolean(res) && !val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "!0", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1 + 1", &res) && val_is_number(res) && 2 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1 + -1", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1 - 1", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1 - 2", &res) && val_is_number(res) && -1 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1 - -1", &res) && val_is_number(res) && 2 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "0 * 2", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "2 * 2", &res) && val_is_number(res) && 4 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "0 / 2", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "6 / 2", &res) && val_is_number(res) && 3 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "6 / 0", &res) && val_is_nan(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "0 % 2", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "3 % 2", &res) && val_is_number(res) && 1 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "6 % 0", &res) && val_is_nan(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1 + 2 * 3", &res) && val_is_number(res) && 7 == val_2_integer(res));
    //printf("---------------------------------------------------\n");

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "2 + 4 / 2", &res) && val_is_number(res) && 4 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "9 - 4 % 2", &res) && val_is_number(res) && 9 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "(1 + 2) * 3", &res) && val_is_number(res) && 9 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "(2 + 4) / 2", &res) && val_is_number(res) && 3 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "(9 - 4) % 2", &res) && val_is_number(res) && 1 == val_2_integer(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "0 & 0", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1 & 0", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "0 & 1", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1 & 1", &res) && val_is_number(res) && 1 == val_2_integer(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "0 | 0", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1 | 0", &res) && val_is_number(res) && 1 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "0 | 1", &res) && val_is_number(res) && 1 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1 | 1", &res) && val_is_number(res) && 1 == val_2_integer(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "0 ^ 0", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1 ^ 0", &res) && val_is_number(res) && 1 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "0 ^ 1", &res) && val_is_number(res) && 1 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1 ^ 1", &res) && val_is_number(res) && 0 == val_2_integer(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1 << 0", &res) && val_is_number(res) && 1 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1 << 1", &res) && val_is_number(res) && 2 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1 >> 0", &res) && val_is_number(res) && 1 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1 >> 1", &res) && val_is_number(res) && 0 == val_2_integer(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "(4 >> 1) * 5 - 7 % 3 + 6 / 2", &res) && val_is_number(res) &&
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

    CU_ASSERT_FATAL(0 == exec_env_init(&env_st, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1 != 0", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1 == 0", &res) && val_is_boolean(res) && !val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1 > 0", &res)  && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1 >= 0", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1 >= 1", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "0 < 1", &res)  && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "0 <= 1", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1 <= 1", &res) && val_is_boolean(res) &&  val_is_true(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "0 > 1 ? 0 : 1", &res) && val_is_number(res) &&  1 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "0 < 1 ? 0 : 1", &res) && val_is_number(res) &&  0 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "0 > 1 ? 0 ? 10: 20 : 1 ? 30: 40", &res) && val_is_number(res) &&  30 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "0 < 1 ? 0 ? 10: 20 : 1 ? 30: 40", &res) && val_is_number(res) &&  20 == val_2_double(res));

    env_deinit(env);
}

static void test_exec_logic(void)
{
    env_t env_st, *env = &env_st;
    val_t *res;

    CU_ASSERT_FATAL(0 == exec_env_init(&env_st, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "false && false", &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "false && true",  &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "true && false",  &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "true && true",   &res) && val_is_boolean(res) &&  val_is_true(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "false || false", &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "false || true",  &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "true || false",  &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "true || true",   &res) && val_is_boolean(res) &&  val_is_true(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "0 && 0", &res) && val_is_number(res) &&  0 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "0 && 1", &res) && val_is_number(res) &&  0 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1 && 0", &res) && val_is_number(res) &&  0 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1 && 1", &res) && val_is_number(res) &&  1 == val_2_double(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "0 || 0", &res) && val_is_number(res) &&  0 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "0 || 1", &res) && val_is_number(res) &&  1 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1 || 0", &res) && val_is_number(res) &&  1 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1 || 1", &res) && val_is_number(res) &&  1 == val_2_double(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "0 && true",  &res) && val_is_number(res)  &&  0 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "0 && false", &res) && val_is_number(res)  &&  0 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1 && true",  &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1 && false", &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "false && 0", &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "false && 1", &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "true  && 0", &res) && val_is_number(res)  &&  0 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "true  && 1", &res) && val_is_number(res)  &&  1 == val_2_double(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "0 || true", &res)  && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "0 || false", &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1 || true", &res)  && val_is_number(res)  &&  1 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "1 || false", &res) && val_is_number(res)  &&  1 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "false || 0", &res) && val_is_number(res)  &&  0 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "false || 1", &res) && val_is_number(res)  &&  1 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "true  || 0", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "true  || 1", &res) && val_is_boolean(res) &&  val_is_true(res));
    /*
    */

    env_deinit(env);
}
static void test_exec_symbal(void)
{
    env_t env_st, *env = &env_st;
    val_t *res;

    CU_ASSERT_FATAL(0 == exec_env_init(&env_st, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "var a = 5, b = 2", &res) && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a", &res) && val_is_number(res) &&  5 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "b", &res) && val_is_number(res) &&  2 == val_2_double(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a == 5", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "b == 2", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a != 0", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "b != 1", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a != 5", &res) && val_is_boolean(res) &&  !val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "b != 2", &res) && val_is_boolean(res) &&  !val_is_true(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a + 2", &res) && val_is_number(res) &&  7 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a - 2", &res) && val_is_number(res) &&  3 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a * 2", &res) && val_is_number(res) &&  10 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a / 2", &res) && val_is_number(res) &&  5.0 / 2 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a % 2", &res) && val_is_number(res) &&  1 == val_2_double(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "5 + b", &res) && val_is_number(res) &&  7 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "5 - b", &res) && val_is_number(res) &&  3 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "5 * b", &res) && val_is_number(res) &&  10 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "5 / b", &res) && val_is_number(res) &&  5.0 / 2 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "5 % b", &res) && val_is_number(res) &&  1 == val_2_double(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a + b", &res) && val_is_number(res) &&  7 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a - b", &res) && val_is_number(res) &&  3 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a * b", &res) && val_is_number(res) &&  10 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a / b", &res) && val_is_number(res) &&  5.0 / 2 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a % b", &res) && val_is_number(res) &&  1 == val_2_double(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a = b = 1", &res) && val_is_number(res) &&  1 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a == 1", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "b == 1", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "b == a", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a << 1", &res) && val_is_number(res) &&  2 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a >> 1", &res) && val_is_number(res) &&  0 == val_2_double(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "b = a == 1 ? 10: 20", &res) && val_is_number(res) &&  10 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a + b > 10", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a - b < 10", &res) && val_is_boolean(res) &&  val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a * b == 10", &res) && val_is_boolean(res) &&  val_is_true(res));

    env_deinit(env);
}

static void test_exec_var(void)
{
    env_t env_st, *env = &env_st;
    val_t *res;


    CU_ASSERT_FATAL(0 == exec_env_init(&env_st, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "var a;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a;", &res) && val_is_undefined(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "var b = 2;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "b;", &res) && 2 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "b == 2", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "var c = d = 3, e = b + c + d, f;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "c == 3", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "d == 3", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "e == 8", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "f;", &res) && val_is_undefined(res));

    // redefine f
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "var f, g, h, k = 0, j, s, m;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "f;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "g;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "h;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "k;", &res) && val_is_number(res) && 0 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "j;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "s;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "m;", &res) && val_is_undefined(res));
    /*
    */

    env_deinit(env);
}

static void test_exec_if(void)
{
    env_t env_st, *env = &env_st;
    val_t *res;

    CU_ASSERT_FATAL(0 == exec_env_init(&env_st, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "var a = 0, b = 1, c = 1, d = 0;", &res) && val_is_undefined(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "if (a == 0) d = 3", &res) && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "d == 3", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "if (a) b = 2 else c = 9", &res) && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "b == 1", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "c == 9", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "if (a == 0) b = 9 else c = 2", &res) && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "b == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "c == 9", &res) && val_is_boolean(res) && val_is_true(res));

    // nest
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "if (a) d = 9 else if (b == c) d = 9 else a = 9", &res) && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a == 0", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "b == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "c == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "d == 9", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "if (a == 0) if (b != c) d = 9 else a = 9", &res) && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "b == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "c == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "d == 9", &res) && val_is_boolean(res) && val_is_true(res));

    // empty
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "if (a) {} else {}", &res));// && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "b == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "c == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "d == 9", &res) && val_is_boolean(res) && val_is_true(res));

    // block
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "if (a == 9) { b = 1; c = 1;} else {d = 2}", &res) && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "b == 1", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "c == 1", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "d == 9", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "if (!a) { b = 2;} else { c = 3; d = 3}", &res) && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a == 9", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "b == 1", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "c == 3", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "d == 3", &res) && val_is_boolean(res) && val_is_true(res));
    /*
    */

    env_deinit(env);
}

static void test_exec_while(void)
{
    env_t env_st, *env = &env_st;
    val_t *res;

    CU_ASSERT_FATAL(0 == exec_env_init(&env_st, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "var a = 0, b = 9;", &res) && val_is_undefined(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "while(a) {a = 1}", &res) && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a == 0", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "while (b) b = b - 1", &res) && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "b == 0", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "while (b < 10) {b = b + 1 a = a + 1}", &res) && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a == 10 && b == 10", &res) && val_is_boolean(res) && val_is_true(res));

    // nest
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "var c = 0; a = 9;", &res) && val_is_number(res) && 9 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "while (a) {a=a-1; b=9; while(b){b=b-1;c=c+1;}}", &res) && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "c == 81", &res) && val_is_boolean(res) && val_is_true(res));

    // continue
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a = 0, b = 0;", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "while (b < 10) {b = b + 1; if (b == 5) continue; a = a + 1}", &res) && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a == 9 && b == 10", &res) && val_is_boolean(res) && val_is_true(res));

    // break;
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "while (a) {a = a - 1; if (a == 5) break; }", &res) && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a == 5", &res) && val_is_boolean(res) && val_is_true(res));

    // compose
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a = 9, b = 9, c = 0;", &res) && val_is_number(res) && 0 == val_2_integer(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "while (a) {b=a; while(b){b=b-1;if(2*b==a){ c=c+1; break;}}; if(a+b==6)break; a=a-1;}", &res) && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a == 4", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "b == 2", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "c == 3", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a == 4 && b == 2 && c == 3", &res) && val_is_boolean(res) && val_is_true(res));

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

    CU_ASSERT_FATAL(0 == exec_env_init(&env_st, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "var a = 1, b = 0;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "def zero() return 0", &res) && val_is_function(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "b = a + zero() + 2", &res) && val_is_number(res) && 3 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "b == 3", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "def fn(a, b) return a + b", &res) && val_is_function(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "fn", &res) && val_is_function(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "fn(1, 3)", &res) && val_is_number(res) && 4 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a = b = fn(1, 3)", &res) && val_is_number(res) && 4 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a == 4 && b == 4", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, fib, &res) && val_is_function(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a = fib(1000)", &res) && val_is_number(res) && 1597 == val_2_double(res));

    // closure & recursion
    // CU_ASSERT(0 < exec_string(env, fff, &res) && val_is_function(*res));
    // CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a = fff(10)", &res) && val_is_number(*res) && 362880 == val_2_double(*res));


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

    CU_ASSERT_FATAL(0 == exec_env_init(&env_st, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 <= env_native_add(env, "one", test_native_one));
    CU_ASSERT(0 <= env_native_add(env, "add", test_native_add));
    CU_ASSERT(0 <= env_native_add(env, "fib", test_native_fib));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "var a = 1, b = 1;", &res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "b = a + one()", &res) && val_is_number(res) && 2 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "b == 2", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a = b + add(a, b);", &res) && val_is_number(res) && 5 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a == 5", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a = one()", &res) && val_is_number(res) && 1 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "b = 999", &res) && val_is_number(res) && 999 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "fib(add(a, b))", &res) && val_is_number(res) && 1597 == val_2_double(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a = (0 + add(b, one())) * 1;", &res) && val_is_number(res) && 1000 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a == 1000", &res) && val_is_boolean(res) && val_is_true(res));

    env_deinit(env);
}

static void test_exec_native_call_script(void)
{
}

static void test_exec_string(void)
{
    env_t env_st, *env = &env_st;
    val_t *res;

    CU_ASSERT_FATAL(0 == exec_env_init(&env_st, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "var a, b = 'world', c;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "\"hello\"", &res) && val_is_string(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a = \"hello\"", &res) && val_is_string(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a == \"hello\"", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a >= \"hello\"", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "b == \"world\"", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "b >= \"world\"", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a < b", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "b > a", &res) && val_is_boolean(res) && val_is_true(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "c = a + ' ' + b", &res) && val_is_string(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "c == 'hello world'", &res) && val_is_true(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "c ? true : false", &res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "'' ? true : false", &res) && !val_is_true(res));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a.length", &res) && val_is_function(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a.length()", &res) && val_is_number(res) && 5 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "c.length()", &res) && val_is_number(res) && 11 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "c.indexOf", &res) && val_is_function(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "c.indexOf(a)", &res) && val_is_number(res) && 0 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "c.indexOf(b)", &res) && val_is_number(res) && 6 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "true.toString().length()", &res) && val_is_number(res) && 4 == val_2_double(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a[0] == 'h'", &res) && val_is_boolean(res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a[0].length()", &res) && val_is_number(res) && 1 == val_2_integer(res));

    env_deinit(env);
}

static void test_exec_gc(void)
{
    env_t env_st, *env = &env_st;
    val_t *res;

    CU_ASSERT_FATAL(0 == exec_env_init(&env_st, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));

    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "var a = 0, b = 'world', c = 'hello';", &res) && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "var d = c + ' ', e = b + '.', f;", &res) && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "def add(a, b) {var n = 10; while(n) {n = n-1; a+b}return a + b}", &res) && val_is_function(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "while (a < 1000) { f = add(d, e); a = a + 1}", &res) && val_is_undefined(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "a == 1000", &res) && val_is_true(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "d", &res) && val_is_string(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "e", &res) && val_is_string(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "f", &res) && val_is_string(res));
    CU_ASSERT(0 < exec_string(env, exec_buf, EXEC_BUF_SIZE, "f == 'hello world.'", &res) && val_is_boolean(res) && val_is_true(res));

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
        CU_add_test(suite, "eval string",       test_exec_string);
        CU_add_test(suite, "eval gc",           test_exec_gc);
        if (0) {
        }
    }

    return suite;
}

