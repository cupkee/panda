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


typedef struct foreign_entry_t {
    struct foreign_entry_t *next;
    int mark;
    int prop;
} foreign_entry_t;

static uint8_t env_buf[ENV_BUF_SIZE];
static foreign_entry_t *data_list = NULL;

static intptr_t foreign_create(int a)
{
    foreign_entry_t *data = malloc(sizeof(foreign_entry_t));

    if (data) {
        data->prop = a;

        data->next = data_list;
        data_list = data;
    }

    return (intptr_t) data;
}

static val_t cache;

static int test_setup()
{
    return 0;
}

static int test_clean()
{
    return 0;
}

int foreign_is_true(val_t *self)
{
    foreign_entry_t *entry = (void *)val_2_intptr(self);

    return entry && entry->prop;
}

int foreign_is_equal(val_t *self, val_t *other)
{
    foreign_entry_t *a = (void *)val_2_intptr(self);

    if (val_is_foreign(other)) {
        foreign_entry_t *b = (void *)val_2_intptr(other);

        if (a && b) {
            return a->prop == b->prop;
        } else {
            return 0;
        }
    } else {
        return a->prop == val_2_integer(other);
    }
}

double foreign_value_of(val_t *self)
{
    foreign_entry_t *a = (void *)val_2_intptr(self);

    if (a) {
        return a->prop;
    } else {
        return 0;
    }
}

val_t foreign_get_prop(void *env, val_t *self, const char *key)
{
    foreign_entry_t *a = (void *)val_2_intptr(self);

    (void) env;

    if (a && !strcmp(key, "a")) {
        return val_mk_number(a->prop);
    } else {
        return VAL_UNDEFINED;
    }
}

val_t foreign_get_elem(void *env, val_t *self, int id)
{
    foreign_entry_t *a = (void *)val_2_intptr(self);

    (void) env;

    if (a && id == 0) {
        return val_mk_number(a->prop);
    } else {
        return VAL_UNDEFINED;
    }
}

void foreign_set_prop(void *env, val_t *self, const char *key, val_t *data)
{
    foreign_entry_t *a = (void *)val_2_intptr(self);

    (void) env;

    if (a && val_is_number(data) && !strcmp(key, "a") ) {
        a->prop = val_2_integer(data);
    }
}

void foreign_set_elem(void *env, val_t *self, int id, val_t *data)
{
    foreign_entry_t *a = (void *)val_2_intptr(self);

    (void) env;

    if (a && val_is_number(data) && id == 0) {
        a->prop = val_2_integer(data);
    }
}

void foreign_opx_prop(void *env, val_t *self, const char *key, val_t *res, val_opx_t op)
{
    foreign_entry_t *a = (void *)val_2_intptr(self);

    (void) env;

    if (a && !strcmp(key, "a") ) {
        val_t tmp = val_mk_number(a->prop);

        op(env, &tmp, res);
        if (val_is_number(&tmp)) {
            a->prop = val_2_integer(&tmp);
        }
    }
}

void foreign_opx_elem(void *env, val_t *self, int id, val_t *res, val_opx_t op)
{
    foreign_entry_t *a = (void *)val_2_intptr(self);

    (void) env;

    if (a && id == 0) {
        val_t tmp = val_mk_number(a->prop);

        op(env, &tmp, res);
        if (val_is_number(&tmp)) {
            a->prop = val_2_integer(&tmp);
        }
    }
}

void foreign_opxx_prop(void *env, val_t *self, const char *key, val_t *data, val_t *res, val_opxx_t op)
{
    foreign_entry_t *a = (void *)val_2_intptr(self);

    (void) env;
    if (a && !strcmp(key, "a") ) {
        val_t tmp = val_mk_number(a->prop);

        op(env, &tmp, data, res);

        if (val_is_number(res)) {
            a->prop = val_2_integer(res);
        }
    }
}

void foreign_opxx_elem(void *env, val_t *self, int id, val_t *data, val_t *res, val_opxx_t op)
{
    foreign_entry_t *a = (void *)val_2_intptr(self);

    (void) env;
    if (a && id == 0) {
        val_t tmp = val_mk_number(a->prop);

        op(env, &tmp, data, res);

        if (val_is_number(res)) {
            a->prop = val_2_integer(res);
        }
    }
}

val_t foreign_set(void *env, val_t *self, val_t *data)
{
    foreign_entry_t *a = (void *)val_2_intptr(self);

    (void) env;
    if (a) {
        if (val_is_number(data)) {
            a->prop = val_2_integer(data);
        }
    }

    return *data;
}

void foreign_keep(intptr_t entry)
{
    foreign_entry_t *a = (void *)entry;

    if (a) {
        a->mark = 1;
    }
}

val_t test_native_foreign(env_t *env, int ac, val_t *av)
{
    intptr_t data = 0;

    if (ac > 0 && val_is_number(av)) {
        data = val_2_integer(av);
    } else {
    }

    return val_mk_foreign(foreign_create(data));
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
    CU_ASSERT(0 < interp_execute_string(&env, "f != 0", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f != ''", &res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "f > 0", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f < 2", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f >= 1", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f <= 1", &res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "-f == -1", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "~f == -2", &res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "f++", &res) && val_is_nan(res));
    CU_ASSERT(0 < interp_execute_string(&env, "++f", &res) && val_is_nan(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f--", &res) && val_is_nan(res));
    CU_ASSERT(0 < interp_execute_string(&env, "--f", &res) && val_is_nan(res));

    CU_ASSERT(0 < interp_execute_string(&env, "f * 1 == 1", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f / 2 == 0.5", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f % 4 == 1", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f + 0 == 1", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f - 1 == 0", &res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "f & 1 != 1", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f | 2 != 3", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f ^ 4 != 5", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f >> 0 != 1", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f << 1 != 2", &res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "f.a == 1", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f.a != 0", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f.a > 0", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f.a < 2", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f.a >= 1", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f.a <= 1", &res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "f.b == 1", &res) && !val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f.b", &res) && val_is_undefined(res));

    CU_ASSERT(0 < interp_execute_string(&env, "f[0] == 1", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f[0] != 0", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f[0] > 0", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f[0] < 2", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f[0] >= 1", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f[0] <= 1", &res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "f[1] != 1", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f[1]", &res) && val_is_undefined(res));

    CU_ASSERT(0 < interp_execute_string(&env, "f.a = 2", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f.a == 2", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f.a > 1", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f.a < 3", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f.a >= 2", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f.a <= 2", &res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "f[0] = 3", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f[0] == 3", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f[0] != 0", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f[0] > 2", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f[0] < 4", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f[0] >= 3", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f[0] <= 3", &res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "f.a++ == 3", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f.a == 4", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "++f.a == 5", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f.a == 5", &res) && val_is_true(res));


    CU_ASSERT(0 < interp_execute_string(&env, "f.a += 4", &res) && val_is_number(res) && 9 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f.a == 9", &res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "f = 1", &res) && val_is_number(res) && 1 == val_2_integer(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f", &res) && val_is_foreign(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f.a == 1", &res) && val_is_true(res));

    /*
    CU_ASSERT(0 < interp_execute_string(&env, "f[0] == 12", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f.a == 13", &res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "f = 2", &res) && val_is_number(res) && 0 == val_2_double(res));
    */

    env_deinit(&env);
}

static void foreign_mark_reset(void)
{
    foreign_entry_t *fp = data_list;
    while (fp) {
        fp->mark = 0;
        fp = fp->next;
    }
}

static void foreign_release(void)
{
    foreign_entry_t *fp = data_list;

    data_list = NULL;
    while (fp) {
        foreign_entry_t *next = fp->next;

        if (fp->mark) {
            fp->next = data_list;
            data_list = fp;
        } else {
            free(fp);
        }
        fp = next;
    }
}

static int foreign_count(void)
{
    foreign_entry_t *fp = data_list;
    int cnt = 0;
    while (fp) {
        fp->mark = 0;
        fp = fp->next;
        ++cnt;
    }

    return cnt;
}

static void test_callback(env_t *env, int event)
{
    int cnt = 0;
    if (event == PANDA_EVENT_GC_START) {
        foreign_mark_reset();
    }
    if (event == PANDA_EVENT_GC_END) {
        foreign_release();
    }
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
    CU_ASSERT(0 == env_callback_set(&env, test_callback));

    CU_ASSERT(0 < interp_execute_string(&env, "var f1 = Foreign(101)", &res));
    CU_ASSERT(0 < interp_execute_string(&env, "var f2 = Foreign(102)", &res));
    CU_ASSERT(0 < interp_execute_string(&env, "f1", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f2", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f1 == 101", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f2 == 102", &res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "var n = 0;", &res));
    CU_ASSERT(0 < interp_execute_string(&env, "var b = 'world', c = 'hello';", &res));
    CU_ASSERT(0 < interp_execute_string(&env, "var d = c + ' ', e = b + '.', f;", &res));
    CU_ASSERT(0 < interp_execute_string(&env, "var a = [b, c, 0];", &res));
    CU_ASSERT(0 < interp_execute_string(&env, "var o = {a: b, b: c};", &res));
    CU_ASSERT(0 < interp_execute_string(&env, "o.f = Foreign(2)", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "o.f", &res) && val_is_foreign(res));

    CU_ASSERT(0 < interp_execute_string(&env, "def add(a, b) {var n = 10; while(n) {n = n-1; a+b} return a + b}", &res) && val_is_function(res));
    CU_ASSERT(0 < interp_execute_string(&env, "def join(){return e + c}", &res) && val_is_function(res));

    CU_ASSERT(0 < interp_execute_string(&env, "a[0] == b", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "a[1] == c", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "o.a == b", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "o.b == c", &res) && val_is_true(res));

    CU_ASSERT(0 < interp_execute_string(&env, "while (n < 10) { f = add(d, e); (n++)}", &res));
    CU_ASSERT(0 < interp_execute_string(&env, "n == 10", &res) && val_is_true(res));
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
    CU_ASSERT(0 < interp_execute_string(&env, "f1 == 101", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "f2 == 102", &res) && val_is_true(res));
    CU_ASSERT(0 < interp_execute_string(&env, "o.f == 2", &res) && val_is_true(res));

    env_deinit(&env);
}

CU_pSuite test_lang_type_foreign(void)
{
    CU_pSuite suite = CU_add_suite("TYPE: foreign", test_setup, test_clean);

    if (suite) {
        CU_add_test(suite, "foreign simple",       test_foreign_simple);
        CU_add_test(suite, "foreign gc",           test_foreign_gc);
    }

    return suite;
}

