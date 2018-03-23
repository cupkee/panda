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

#include "lang/env.h"
#include "lang/gc.h"
#include "lang/type_function.h"
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

static val_t ref[4];
static int gc_enter_count = 0;
static int gc_leave_count = 0;
static void gc_callback(env_t *env, int event)
{
    if (event == PANDA_EVENT_GC_START) {
        ++gc_enter_count;
        gc_types_copy(env, 4, ref);
    } else
    if (event == PANDA_EVENT_GC_END) {
        ++gc_leave_count;
    }
}

static val_t test_async_register(env_t *env, int ac, val_t *av)
{
    int i;

    (void) env;
    (void) ac;
    (void) av;

    if (ac > 0) {
        for (i = 0; i < 4; i++) {
            if (val_is_undefined(ref + i)) {
                ref[i] = *av;
                return val_mk_number(i);
            }
        }
    }

    return val_mk_undefined();
}

static val_t test_async_call(env_t *env, val_t *fn)
{
    if (val_is_native(fn)) {
        function_native_t native = (function_native_t) val_2_intptr(fn);
        return native(env, 0, NULL);
    } else {
        env_push_call_function(env, fn);
        return interp_execute_call(env, 0);
    }
}

static void test_async_common(void)
{
    env_t env;
    val_t *res;
    native_t native_entry[] = {
        {"register", test_async_register}
    };
    int i;

    for (i = 0; i < 4; i++) {
        val_set_undefined(ref + i);
    }

    CU_ASSERT_FATAL(0 == interp_env_init_interactive(&env, env_buf, ENV_BUF_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE));
    CU_ASSERT(0 == env_native_set(&env, native_entry, 1));
    CU_ASSERT(0 == env_callback_set(&env, gc_callback));

    CU_ASSERT(0 < interp_execute_string(&env, "register", &res) && val_is_function(res));
    CU_ASSERT(0 < interp_execute_string(&env, "var n = 0, a = [], o = {}, s;", &res));
    CU_ASSERT(0 < interp_execute_string(&env, "def fn() {n += 1}", &res) && val_is_function(res));
    CU_ASSERT(0 < interp_execute_string(&env, "register(fn)", &res) && val_is_number(res) && 0 == val_2_double(res));

    for (i = 0; i < 4; i++) {
        val_t *fn = ref + i;
        if (val_is_function(fn)) {
            test_async_call(&env, fn);
            val_set_undefined(fn);
        }
    }
    CU_ASSERT(0 < interp_execute_string(&env, "n == 1", &res) && val_is_true(res));

    env_deinit(&env);
}

CU_pSuite test_lang_async_entry()
{
    CU_pSuite suite = CU_add_suite("lang async execute", test_setup, test_clean);

    if (suite) {
        CU_add_test(suite, "async common", test_async_common);
    }

    return suite;
}

