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

#include "cunit/CUnit.h"
#include "cunit/CUnit_Basic.h"

CU_pSuite test_hello_entry();

CU_pSuite test_lang_lex_entry();
CU_pSuite test_lang_val_entry();
CU_pSuite test_lang_parse_entry();
CU_pSuite test_lang_symtbl_entry();
CU_pSuite test_lang_interp_entry();
CU_pSuite test_lang_image_entry();
CU_pSuite test_lang_async_entry();

CU_pSuite test_lang_type_foreign();

int main(int argc, const char *argv[])
{
    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }

    // Set test suite here:
    test_hello_entry();
    test_lang_lex_entry();
    test_lang_val_entry();
    test_lang_parse_entry();
    test_lang_symtbl_entry();
    test_lang_interp_entry();
    test_lang_image_entry();
    test_lang_async_entry();

    test_lang_type_foreign();

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}

