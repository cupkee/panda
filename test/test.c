#include "cunit/CUnit.h"
#include "cunit/CUnit_Basic.h"

CU_pSuite test_hello_entry();
CU_pSuite test_esbl_alg_entry();
CU_pSuite test_esbl_list_entry();
CU_pSuite test_esbl_vect_entry();
CU_pSuite test_esbl_htbl_entry();

CU_pSuite test_lang_lex_entry();
CU_pSuite test_lang_val_entry();
CU_pSuite test_lang_parse_entry();

int main(int argc, const char *argv[])
{
    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }

    // Set test suite here:
    if (!test_hello_entry()) {
        printf("Init test suite \"%s\" fail\n", "hello");
    }
    if (!test_esbl_alg_entry()) {
        printf("Init test suite \"%s\" fail\n", "esbl alg");
    }
    if (!test_esbl_list_entry()) {
        printf("Init test suite \"%s\" fail\n", "esbl list");
    }
    if (!test_esbl_vect_entry()) {
        printf("Init test suite \"%s\" fail\n", "esbl vect");
    }
    if (!test_esbl_htbl_entry()) {
        printf("Init test suite \"%s\" fail\n", "esbl htbl");
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

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}

