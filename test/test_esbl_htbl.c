#include <stdio.h>
#include <string.h>

#include "cunit/CUnit.h"
#include "cunit/CUnit_Basic.h"

#include "esbl/htbl.h"

static int test_setup()
{
    return 0;
}

static int test_clean()
{
    return 0;
}

static uint32_t test_hash(intptr_t v)
{
    return (uint32_t) v * 131;
}

static int test_compare(intptr_t a, intptr_t b)
{
    return a - b;
}

static void test_common(void)
{
    intptr_t htbl;

    CU_ASSERT(0 != (htbl = htbl_create(test_hash, test_compare)));
    CU_ASSERT(0 == htbl_length(htbl));
    CU_ASSERT(0 == htbl_lookup(htbl, 0));
    CU_ASSERT(0 == htbl_lookup(htbl, 1));
    CU_ASSERT(0 == htbl_lookup(htbl, 2));

    CU_ASSERT(1 == htbl_insert(htbl, 0));
    CU_ASSERT(1 == htbl_length(htbl));
    CU_ASSERT(1 == htbl_lookup(htbl, 0));

    CU_ASSERT(1 == htbl_insert(htbl, 1));
    CU_ASSERT(1 == htbl_lookup(htbl, 1));
    CU_ASSERT(2 == htbl_length(htbl));

    CU_ASSERT(1 == htbl_insert(htbl, 2));
    CU_ASSERT(1 == htbl_lookup(htbl, 2));
    CU_ASSERT(3 == htbl_length(htbl));

    CU_ASSERT(1 == htbl_insert(htbl, 0));
    CU_ASSERT(1 == htbl_insert(htbl, 1));
    CU_ASSERT(1 == htbl_insert(htbl, 2));
    CU_ASSERT(3 == htbl_length(htbl));
    CU_ASSERT(1 == htbl_lookup(htbl, 0));
    CU_ASSERT(1 == htbl_lookup(htbl, 1));
    CU_ASSERT(1 == htbl_lookup(htbl, 2));
    CU_ASSERT(0 == htbl_lookup(htbl, 3));

    CU_ASSERT(1 == htbl_remove(htbl, 2));
    CU_ASSERT(2 == htbl_length(htbl));
    CU_ASSERT(0 == htbl_lookup(htbl, 2));
    CU_ASSERT(1 == htbl_lookup(htbl, 1));
    CU_ASSERT(1 == htbl_lookup(htbl, 0));

    CU_ASSERT(1 == htbl_remove(htbl, 1));
    CU_ASSERT(1 == htbl_length(htbl));
    CU_ASSERT(0 == htbl_lookup(htbl, 2));
    CU_ASSERT(0 == htbl_lookup(htbl, 1));
    CU_ASSERT(1 == htbl_lookup(htbl, 0));

    CU_ASSERT(1 == htbl_remove(htbl, 0));
    CU_ASSERT(0 == htbl_length(htbl));
    CU_ASSERT(0 == htbl_lookup(htbl, 2));
    CU_ASSERT(0 == htbl_lookup(htbl, 1));
    CU_ASSERT(0 == htbl_lookup(htbl, 0));

    CU_ASSERT(0 == htbl_destroy(htbl));
}

static void test_insert(void)
{
    intptr_t htbl;
    int i, matched = 0, inserted = 0;

    CU_ASSERT(0 != (htbl = htbl_create(test_hash, test_compare)));
    CU_ASSERT(0 == htbl_length(htbl));

    for (i = 0; i < 4097; i++) {
        inserted += htbl_insert(htbl, i);
    }
    CU_ASSERT(4097 == inserted);
    CU_ASSERT(4097 == htbl_length(htbl));

    for (i = 0; i < 8000 ; i++) {
        matched += htbl_lookup(htbl, i);
    }
    CU_ASSERT(4097 == matched);

    CU_ASSERT(0 == htbl_destroy(htbl));
}

static void test_remove(void)
{
    intptr_t htbl;
    int i, matched = 0, removed = 0;

    CU_ASSERT(0 != (htbl = htbl_create(test_hash, test_compare)));
    CU_ASSERT(0 == htbl_length(htbl));

    for (i = 0; i < 4097; i++) {
        htbl_insert(htbl, i);
    }
    CU_ASSERT(4097 == htbl_length(htbl));

    for (i = 0; i < 4097; i += 7) {
        removed += htbl_remove(htbl, i);
    }
    for (i = 0; i < 8000 ; i++) {
        matched += htbl_lookup(htbl, i);
    }
    CU_ASSERT(4097 - removed == matched);

    for (i = 0; i < 4097; i += 2) {
        htbl_remove(htbl, i);
        if (i % 7 != 0) {
            removed++;
        }
    }
    matched = 0;
    for (i = 0; i < 8000 ; i++) {
        matched += htbl_lookup(htbl, i);
    }
    CU_ASSERT(4097 - removed == matched);

    CU_ASSERT(0 == htbl_destroy(htbl));
}

CU_pSuite test_esbl_htbl_entry()
{
    CU_pSuite suite = CU_add_suite("esbl htbl", test_setup, test_clean);

    if (suite) {
        CU_add_test(suite, "common", test_common);
        CU_add_test(suite, "insert", test_insert);
        CU_add_test(suite, "remove", test_remove);
    }

    return suite;
}

