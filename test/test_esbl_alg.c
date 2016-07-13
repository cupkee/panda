#include <stdio.h>
#include <string.h>

#include "cunit/CUnit.h"
#include "cunit/CUnit_Basic.h"

#include "esbl/alg.h"

static int test_setup()
{
    return 0;
}

static int test_clean()
{
    return 0;
}

static void test_hash(void)
{
}

CU_pSuite test_esbl_alg_entry()
{
    CU_pSuite suite = CU_add_suite("esbl alg", test_setup, test_clean);

    if (suite) {
        CU_add_test(suite, "hash",    test_hash);
    }

    return suite;
}

