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

#include "lang/compile.h"
#include "lang/interp.h"

#define CPL_BUF_SIZE    10240
#define IMG_BUF_SIZE    10240
#define RUN_BUF_SIZE    20480
//#define RUN_BUF_SIZE    10480

uint8_t cpl_buf[CPL_BUF_SIZE];
uint8_t img_buf[IMG_BUF_SIZE];
uint8_t run_buf[RUN_BUF_SIZE];

static int test_setup()
{
    return 0;
}

static int test_clean()
{
    return 0;
}

static void test_image_simple(void)
{
    int img_sz;
    env_t env;
    val_t *res;
    image_info_t image;
    const char *input = "       \
        var a = 0, b = 1;       \
        def fn() return a + b;  \
        fn() == 1;              \
        ";

    CU_ASSERT_FATAL(0 == compile_env_init(&env, cpl_buf, CPL_BUF_SIZE));
    CU_ASSERT_FATAL(0 < (img_sz = compile_exe(&env, input, img_buf, IMG_BUF_SIZE)));
    CU_ASSERT_FATAL(0 == image_load(&image, img_buf, img_sz));
    CU_ASSERT_FATAL(0 == interp_env_init_image(&env, run_buf, RUN_BUF_SIZE,
            NULL, 8192, NULL, 1024, &image));

    CU_ASSERT_FATAL(0 <= interp_execute_image(&env, &res));// && val_is_number(res) && 1 == val_2_double(res));
}

CU_pSuite test_lang_image_entry()
{
    CU_pSuite suite = CU_add_suite("lang image", test_setup, test_clean);

    if (suite) {
        CU_add_test(suite, "image simple",       test_image_simple);
    }

    return suite;
}

