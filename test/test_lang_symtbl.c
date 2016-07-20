
#include <stdio.h>
#include <string.h>

#include "cunit/CUnit.h"
#include "cunit/CUnit_Basic.h"

#include "lang/symtbl.h"

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
    intptr_t symtbl;
    intptr_t id;

    CU_ASSERT(0 != (symtbl = symtbl_create()));

    CU_ASSERT(0 == symtbl_get(symtbl, "hello"));
    CU_ASSERT(0 == symtbl_get(symtbl, "world"));
    CU_ASSERT(0 == symtbl_get(symtbl, "_a_"));
    CU_ASSERT(0 == symtbl_get(symtbl, "symbal"));

    CU_ASSERT(0 != (id = symtbl_add(symtbl, "hello")) && id == symtbl_get(symtbl, "hello"));
    CU_ASSERT(0 != (id = symtbl_add(symtbl, "world")) && id == symtbl_get(symtbl, "world"));
    CU_ASSERT(0 != (id = symtbl_add(symtbl, "_a_")) && id == symtbl_get(symtbl, "_a_"));
    CU_ASSERT(0 != (id = symtbl_add(symtbl, "symbal")) && id == symtbl_get(symtbl, "symbal"));

    CU_ASSERT(0 == symtbl_destroy(symtbl));
}

CU_pSuite test_lang_symtbl_entry()
{
    CU_pSuite suite = CU_add_suite("lang symtbl", test_setup, test_clean);

    if (suite) {
        CU_add_test(suite, "symtbl common", test_symtbl_common);
    }

    return suite;
}

