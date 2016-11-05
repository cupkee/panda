/*
MIT License

Copyright (c) 2016 Lixing Ding <ding.lixing@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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
CU_pSuite test_lang_foreign_entry();

int main(int argc, const char *argv[])
{
    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }

    // Set test suite here:
    if (!test_hello_entry()) {
        printf("Init test suite \"%s\" fail\n", "hello");
    }
    if (!test_lang_lex_entry()) {
        printf("Init test suite \"%s\" fail\n", "lang lex");
    }
    if (!test_lang_val_entry()) {
        printf("Init test suite \"%s\" fail\n", "lang val");
    }
    if (!test_lang_parse_entry()) {
        printf("Init test suite \"%s\" fail\n", "lang parse");
    }
    if (!test_lang_symtbl_entry()) {
        printf("Init test suite \"%s\" fail\n", "lang symtbl");
    }
    if (!test_lang_interp_entry()) {
        printf("Init test suite \"%s\" fail\n", "lang interp");
    }
    if (!test_lang_image_entry()) {
        printf("Init test suite \"%s\" fail\n", "lang image");
    }
    if (!test_lang_async_entry()) {
        printf("Init test suite \"%s\" fail\n", "lang async");
    }
    if (!test_lang_foreign_entry()) {
        printf("Init test suite \"%s\" fail\n", "lang async");
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}

