/* GPLv2 License
 *
 * Copyright (C) 2016-2018 Lixing Ding <ding.lixing@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 **/

#include <stdio.h>
#include <string.h>

#include "cunit/CUnit.h"
#include "cunit/CUnit_Basic.h"

#include "lang/interp.h"


#define STACK_SIZE      128
#define HEAP_SIZE       4096

#define EXE_MEM_SPACE   4096
#define SYM_MEM_SPACE   1024
#define ENV_BUF_SIZE    (sizeof(val_t) * STACK_SIZE + HEAP_SIZE + EXE_MEM_SPACE + SYM_MEM_SPACE)

uint8_t env_buf[ENV_BUF_SIZE];

static int test_setup()
{
    return 0;
}

static int test_clean()
{
    return 0;
}

val_t test_native_foreign(env_t *env, int ac, val_t *av)
{
    intptr_t data = 0;

    if (ac > 0 && val_is_number(av)) {
        data = val_2_double(av);
    }
    return val_mk_foreign(data);
}

val_t test_native_show(env_t *env, int ac, val_t *av)
{
    intptr_t data = 0;

    if (ac > 0 && val_is_number(av)) {
        printf("%f \n", val_2_double(av));
    }
    return VAL_UNDEFINED;
}

static void test_foreign_simple(void)
{
    env_t env;
    val_t *res;
    native_t native_entry[] = {
        {"Foreign", test_native_foreign},
    };

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));
    CU_ASSERT(0 == env_native_set(&env, native_entry, 1));

    CU_ASSERT(0 < interp_execute_string(&env, "var f = Foreign(1)", &res));

    CU_ASSERT(0 < interp_execute_string(&env, "f", &res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "f == 1", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f != 2", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f != 1", &res) && !val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f == 2", &res) && !val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "f > 0", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f >= 1", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f < 2", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f <= 1", &res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "f == 1", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "-f == 1", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "~f == 1", &res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "f++ == 1", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "++f == 2", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f-- == 1", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "--f == 0", &res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "f * 1 == 2", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f / 2 == 3", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f % 4 == 4", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f + 0 == 5", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f - 1 == 6", &res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "f & 1 == 7", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f | 2 == 8", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f ^ 4 == 9", &res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "f >> 1 == 10", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f << 1 == 11", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f[0] == 12", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f.a == 13", &res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "f = 2", &res) && val_is_number(res) && 0 == val_2_double(res));



    //CU_ASSERT(0 < interp_execute_string(&env, "f.is(Foreign)", &res) && val_is_true(res));

    env_deinit(&env);
}



static void test_foreign_gc(void)
{
    env_t env;
    val_t *res;
    native_t native_entry[] = {
        {"Foreign", test_native_foreign},
        {"show",    test_native_show},
    };

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));
    CU_ASSERT(0 == env_native_set(&env, native_entry, 2));

    CU_ASSERT(0 < interp_execute_string(&env, "var foreign = Foreign(0)", &res));
    CU_ASSERT(0 < interp_execute_string(&env, "foreign", &res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "var n = 0;", &res));
    CU_ASSERT(0 < interp_execute_string(&env, "var b = 'world', c = 'hello';", &res));
    CU_ASSERT(0 < interp_execute_string(&env, "var d = c + ' ', e = b + '.', f;", &res));
    CU_ASSERT(0 < interp_execute_string(&env, "var a = [b, c, 0];", &res));
    CU_ASSERT(0 < interp_execute_string(&env, "var o = {a: b, b: c};", &res));
    CU_ASSERT(0 < interp_execute_string(&env, "def add(a, b) {var n = 10; while(n) {n = n-1; a+b} return a + b}", &res) && val_is_function(res));
    CU_ASSERT(0 < interp_execute_string(&env, "def join(){return e + c}", &res) && val_is_function(res));

    CU_ASSERT(0 < interp_execute_string(&env, "a[0] == b", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a[1] == c", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "o.a == b", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "o.b == c", &res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "while (n < 100) { f = add(d, e); n++}", &res));
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

    // foreign should keep ok
    CU_ASSERT(0 < interp_execute_string(&env, "foreign", &res) && val_is_true(res));

    env_deinit(&env);
}

CU_pSuite test_lang_type_foreign(void)
{
    CU_pSuite suite = CU_add_suite("TYPE: foreign", test_setup, test_clean);

    if (suite) {
        //CU_add_test(suite, "foreign simple",       test_foreign_simple);
        CU_add_test(suite, "foreign gc",           test_foreign_gc);
        if (0) {
        }
    }

    return suite;
}

