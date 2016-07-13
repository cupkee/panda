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

static void test_hello(void)
{
    CU_ASSERT(1);
}

CU_pSuite test_hello_entry()
{
    CU_pSuite suite = CU_add_suite("hello", test_setup, test_clean);

    if (suite) {
        CU_add_test(suite, "hello", test_hello);
    }

    return suite;
}

