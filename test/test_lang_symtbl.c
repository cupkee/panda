
#include <stdio.h>
#include <string.h>

#include "cunit/CUnit.h"
#include "cunit/CUnit_Basic.h"

static int test_setup()
{
    return 0;
}

static int test_clean()
{
    return 0;
}

static void test_symtbl_common(void)
{
}

CU_pSuite test_lang_symtbl_entry()
{
    CU_pSuite suite = CU_add_suite("lang symtbl", test_setup, test_clean);

    if (suite) {
        CU_add_test(suite, "symtbl common", test_symtbl_common);
    }

    return suite;
}

