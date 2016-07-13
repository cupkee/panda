
#include <stdio.h>
#include <string.h>

#include "cunit/CUnit.h"
#include "cunit/CUnit_Basic.h"

#include "esbl/list.h"

static int test_setup()
{
    return 0;
}

static int test_clean()
{
    return 0;
}

static void test_common(void)
{
    intptr_t list;
    intptr_t data;
    intptr_t buff[10];

    // Create a list
    CU_ASSERT(0 != (list = list_create(0, NULL)));
    CU_ASSERT(0 == list_length(list));

    // push data
    CU_ASSERT(0 == list_push(list, -1));
    CU_ASSERT(1 == list_length(list));
    CU_ASSERT(0 == list_push(list, 0));
    CU_ASSERT(2 == list_length(list));
    CU_ASSERT(0 == list_push(list, 1));
    CU_ASSERT(3 == list_length(list));

    // get data by index
    CU_ASSERT(1 == list_get(list, 0, &data) && -1 == data);
    CU_ASSERT(1 == list_get(list, 1, &data) && 0 == data);
    CU_ASSERT(1 == list_get(list, 2, &data) && 1 == data);
    CU_ASSERT(1 == list_get(list, -1, &data) && 1 == data);
    CU_ASSERT(1 == list_get(list, -2, &data) && 0 == data);
    CU_ASSERT(1 == list_get(list, -3, &data) && -1 == data);

    CU_ASSERT(0 == list_get(list, 4, &data));
    CU_ASSERT(1 == list_get(list, -4, &data) && 1 == data);
    CU_ASSERT(1 == list_get(list, -7, &data) && 1 == data);

    // lpush data
    CU_ASSERT(0 == list_lpush(list, -2));
    CU_ASSERT(4 == list_length(list));
    CU_ASSERT(0 == list_lpush(list, -3));
    CU_ASSERT(5 == list_length(list));
    CU_ASSERT(0 == list_lpush(list, -4));
    CU_ASSERT(6 == list_length(list));

    // set & get data by index
    CU_ASSERT(1 == list_get(list, 0, &data) && -4 == data);
    CU_ASSERT(1 == list_get(list, 1, &data) && -3 == data);
    CU_ASSERT(1 == list_get(list, 2, &data) && -2 == data);
    CU_ASSERT(1 == list_get(list, 3, &data) && -1 == data);
    CU_ASSERT(1 == list_get(list, 4, &data) && 0 == data);
    CU_ASSERT(1 == list_get(list, 5, &data) && 1 == data);

    CU_ASSERT(1 == list_get(list, -1, &data) && 1 == data);
    CU_ASSERT(1 == list_get(list, -2, &data) && 0 == data);
    CU_ASSERT(1 == list_get(list, -3, &data) && -1 == data);
    CU_ASSERT(1 == list_get(list, -4, &data) && -2 == data);
    CU_ASSERT(1 == list_get(list, -5, &data) && -3 == data);
    CU_ASSERT(1 == list_get(list, -6, &data) && -4 == data);

    CU_ASSERT(1 == list_gets(list, 0, 1, buff));
    CU_ASSERT(-4 == buff[0]);
    CU_ASSERT(1 == list_gets(list, 5, 6, buff));
    CU_ASSERT(1 == buff[0]);
    CU_ASSERT(6 == list_gets(list, 0, 6, buff));
    CU_ASSERT(-4 == buff[0] && -3 == buff[1] && -2 == buff[2] && -1 == buff[3] && 0 == buff[4] && 1 == buff[5]);

    CU_ASSERT(1 == list_set(list, 0, 0));
    CU_ASSERT(1 == list_get(list, 0, &data) && 0 == data);
    CU_ASSERT(1 == list_set(list, 0, -4));
    CU_ASSERT(1 == list_get(list, 0, &data) && -4 == data);

    CU_ASSERT(2 == list_sets(list, 0, 2, buff + 4));
    CU_ASSERT(2 == list_sets(list, 2, 2, buff + 4));
    CU_ASSERT(2 == list_sets(list, 4, 2, buff + 4));
    CU_ASSERT(2 == list_gets(list, 0, 2, buff));
    CU_ASSERT(0 == buff[0] && 1 == buff[1] && -2 == buff[2] && -1 == buff[3] && 0 == buff[4] && 1 == buff[5]);
    buff[0] = -4; buff[1] = -3; buff[2] = -2; buff[3] = -1; buff[4] = 0; buff[5] = 1;
    CU_ASSERT(6 == list_sets(list, 0, 6, buff));

    // lpop
    CU_ASSERT(0 == list_lpop(list, &data) && -4 == data);
    CU_ASSERT(5 == list_length(list));
    CU_ASSERT(0 == list_lpop(list, &data) && -3 == data);
    CU_ASSERT(4 == list_length(list));
    CU_ASSERT(0 == list_lpop(list, &data) && -2 == data);
    CU_ASSERT(3 == list_length(list));

    // pop
    CU_ASSERT(0 == list_pop(list, &data) && 1 == data);
    CU_ASSERT(2 == list_length(list));
    CU_ASSERT(0 == list_pop(list, &data) && 0 == data);
    CU_ASSERT(1 == list_length(list));
    CU_ASSERT(0 == list_pop(list, &data) && -1 == data);
    CU_ASSERT(0 == list_length(list));

    // destroy list
    CU_ASSERT(0 == list_destroy(list));
}

static void test_insert(void)
{
    intptr_t list;
    intptr_t datas[5] = {0, 1, 2, 3, 4};

    // Create a list
    CU_ASSERT(0 != (list = list_create(0, NULL)));

    // Insert to empty list
    CU_ASSERT(5 == list_insert(list, 0, 5, datas));
    CU_ASSERT(1 == list_get(list, 0, datas) && 0 == datas[0]);
    CU_ASSERT(1 == list_get(list, 2, datas) && 2 == datas[0]);
    CU_ASSERT(1 == list_get(list, 4, datas) && 4 == datas[0]);
    CU_ASSERT(0 == list_get(list, 5, datas));

    // Insert bgn, mid, end
    datas[0] = 8;
    datas[1] = 9;
    CU_ASSERT(2 == list_insert(list, 0, 2, datas));
    CU_ASSERT(2 == list_insert(list, 2, 2, datas));
    CU_ASSERT(2 == list_insert(list, 9, 2, datas));
    CU_ASSERT(11 == list_length(list));

    CU_ASSERT(1 == list_get(list, 0, datas) && 8 == datas[0]);
    CU_ASSERT(1 == list_get(list, 1, datas) && 9 == datas[0]);
    CU_ASSERT(1 == list_get(list, 2, datas) && 8 == datas[0]);
    CU_ASSERT(1 == list_get(list, 3, datas) && 9 == datas[0]);
    CU_ASSERT(1 == list_get(list, 4, datas) && 0 == datas[0]);
    CU_ASSERT(1 == list_get(list, 5, datas) && 1 == datas[0]);
    CU_ASSERT(1 == list_get(list, 6, datas) && 2 == datas[0]);
    CU_ASSERT(1 == list_get(list, 7, datas) && 3 == datas[0]);
    CU_ASSERT(1 == list_get(list, 8, datas) && 4 == datas[0]);
    CU_ASSERT(1 == list_get(list, 9, datas) && 8 == datas[0]);
    CU_ASSERT(1 == list_get(list, 10, datas) && 9 == datas[0]);

    // Insert behind tail
    datas[0] = 6;
    datas[1] = 7;
    CU_ASSERT(2 == list_insert(list, 20, 2, datas));
    CU_ASSERT(1 == list_get(list, 11, datas) && 6 == datas[0]);
    CU_ASSERT(1 == list_get(list, 12, datas) && 7 == datas[0]);

    // insert neg
    datas[0] = 0;
    datas[1] = 1;
    CU_ASSERT(2 == list_insert(list, -1, 2, datas));
    CU_ASSERT(1 == list_get(list, 11, datas) && 6 == datas[0]);
    CU_ASSERT(1 == list_get(list, 12, datas) && 0 == datas[0]);
    CU_ASSERT(1 == list_get(list, 13, datas) && 1 == datas[0]);
    CU_ASSERT(1 == list_get(list, 14, datas) && 7 == datas[0]);

    // destroy list
    CU_ASSERT(0 == list_destroy(list));
}

static void test_cut(void)
{
    intptr_t list, list2;
    intptr_t datas[5] = {0, 1, 2, 3, 4};

    // Create a list
    CU_ASSERT(0 != (list = list_create(5, datas)));
    CU_ASSERT(5 == list_length(list));

    // cut nothing
    CU_ASSERT(0 != (list2 = list_cut(list, 0, 0)));
    CU_ASSERT(0 == list_length(list2));
    CU_ASSERT(0 == list_destroy(list2));
    CU_ASSERT(0 != list_cut(list, 2, 2));
    CU_ASSERT(0 == list_length(list2));
    CU_ASSERT(0 == list_destroy(list2));
    CU_ASSERT(0 != list_cut(list, 4, 2));
    CU_ASSERT(0 == list_length(list2));
    CU_ASSERT(0 == list_destroy(list2));
    CU_ASSERT(0 != list_cut(list, 5, 5));
    CU_ASSERT(0 == list_length(list2));
    CU_ASSERT(0 == list_destroy(list2));
    CU_ASSERT(0 != list_cut(list, 5, 6));
    CU_ASSERT(0 == list_length(list2));
    CU_ASSERT(0 == list_destroy(list2));

    // cut
    CU_ASSERT(0 != (list2 = list_cut(list, 2, 3)) && 1 == list_length(list2) && 4 == list_length(list));
    CU_ASSERT(1 == list_get(list, 1, datas) && 1 == datas[0]);
    CU_ASSERT(1 == list_get(list, 2, datas) && 3 == datas[0]);
    CU_ASSERT(1 == list_get(list2, 0, datas) && 2 == datas[0]);
    CU_ASSERT(0 == list_destroy(list2));

    CU_ASSERT(0 != (list2 = list_cut(list, 2, 4)) && 2 == list_length(list2) && 2 == list_length(list));
    CU_ASSERT(1 == list_get(list, 1, datas) && 1 == datas[0]);
    CU_ASSERT(0 == list_get(list, 2, datas));
    CU_ASSERT(1 == list_get(list2, 0, datas) && 3 == datas[0]);
    CU_ASSERT(1 == list_get(list2, 1, datas) && 4 == datas[0]);
    CU_ASSERT(0 == list_destroy(list2));

    CU_ASSERT(0 != (list2 = list_cut(list, 0, 5)) && 2 == list_length(list2) && 0 == list_length(list));
    CU_ASSERT(0 == list_get(list, 0, datas));
    CU_ASSERT(1 == list_get(list2, 0, datas) && 0 == datas[0]);
    CU_ASSERT(1 == list_get(list2, 1, datas) && 1 == datas[0]);
    CU_ASSERT(0 == list_destroy(list2));

    // push and cut
    CU_ASSERT(0 == list_push(list, 5));
    CU_ASSERT(0 == list_push(list, 6));
    CU_ASSERT(0 == list_push(list, 7));
    CU_ASSERT(0 == list_push(list, 8));

    CU_ASSERT(4 == list_length(list));
    CU_ASSERT(1 == list_get(list, 0, datas) && 5 == datas[0]);
    CU_ASSERT(1 == list_get(list, 3, datas) && 8 == datas[0]);

    CU_ASSERT(0 != (list2 = list_cut(list, 0, 1)) && 1 == list_length(list2) && 3 == list_length(list));
    CU_ASSERT(1 == list_get(list2, 0, datas) && 5 == datas[0]);
    CU_ASSERT(0 == list_destroy(list2));

    CU_ASSERT(0 != (list2 = list_cut(list, 2, 3)) && 1 == list_length(list2) && 2 == list_length(list));
    CU_ASSERT(1 == list_get(list2, 0, datas) && 8 == datas[0]);
    CU_ASSERT(0 == list_destroy(list2));

    CU_ASSERT(1 == list_get(list, 0, datas) && 6 == datas[0]);
    CU_ASSERT(1 == list_get(list, 1, datas) && 7 == datas[0]);

    // destroy list
    CU_ASSERT(0 == list_destroy(list));
}

static void test_slice(void)
{
    intptr_t list, list2;
    intptr_t datas[5] = {0, 1, 2, 3, 4};

    // Create a list
    CU_ASSERT(0 != (list = list_create(5, datas)));

    list2 = list_slice(list, 0, 5);
    CU_ASSERT(5 == list_length(list2));
    CU_ASSERT(1 == list_get(list2, 0, datas) && 0 == datas[0]);
    CU_ASSERT(1 == list_get(list2, 4, datas) && 4 == datas[0]);
    CU_ASSERT(0 == list_destroy(list2));

    list2 = list_slice(list, -3, -1);
    CU_ASSERT(2 == list_length(list2));
    CU_ASSERT(1 == list_get(list2, 0, datas) && 2 == datas[0]);
    CU_ASSERT(1 == list_get(list2, 1, datas) && 3 == datas[0]);
    CU_ASSERT(0 == list_get(list2, 2, datas));
    CU_ASSERT(0 == list_destroy(list2));

    list2 = list_slice(list, 1, 10);
    CU_ASSERT(4 == list_length(list2));
    CU_ASSERT(1 == list_get(list2, 0, datas) && 1 == datas[0]);
    CU_ASSERT(1 == list_get(list2, 1, datas) && 2 == datas[0]);
    CU_ASSERT(1 == list_get(list2, 2, datas) && 3 == datas[0]);
    CU_ASSERT(1 == list_get(list2, 3, datas) && 4 == datas[0]);
    CU_ASSERT(0 == list_get(list2, 4, datas));
    CU_ASSERT(0 == list_destroy(list2));


    CU_ASSERT(0 == list_slice(list, 5, 6));
    CU_ASSERT(0 == list_slice(list, 3, 1));
    CU_ASSERT(0 == list_slice(list, -1, -3));

    // destroy list
    CU_ASSERT(0 == list_destroy(list));
}

static void test_concat(void)
{
    intptr_t list, list1, list2, list3, list4;
    intptr_t datas[5] = {0, 1, 2, 3, 4};

    // Create a list
    CU_ASSERT(0 != (list1 = list_create(5, datas)));
    CU_ASSERT(0 != (list2 = list_create(5, datas)));
    CU_ASSERT(0 != (list3 = list_create(0, NULL)));
    CU_ASSERT(0 != (list4 = list_create(0, NULL)));

    CU_ASSERT(0 != (list = list_concat(list1, list2)));
    CU_ASSERT(10 == list_length(list));
    CU_ASSERT(1 == list_get(list, 0, datas) && 0 == datas[0]);
    CU_ASSERT(1 == list_get(list, 1, datas) && 1 == datas[0]);
    CU_ASSERT(1 == list_get(list, 2, datas) && 2 == datas[0]);
    CU_ASSERT(1 == list_get(list, 3, datas) && 3 == datas[0]);
    CU_ASSERT(1 == list_get(list, 4, datas) && 4 == datas[0]);
    CU_ASSERT(1 == list_get(list, 5, datas) && 0 == datas[0]);
    CU_ASSERT(1 == list_get(list, 6, datas) && 1 == datas[0]);
    CU_ASSERT(1 == list_get(list, 7, datas) && 2 == datas[0]);
    CU_ASSERT(1 == list_get(list, 8, datas) && 3 == datas[0]);
    CU_ASSERT(1 == list_get(list, 9, datas) && 4 == datas[0]);
    CU_ASSERT(0 == list_destroy(list));

    CU_ASSERT(0 != (list = list_concat(list1, list3)));
    CU_ASSERT(5 == list_length(list));
    CU_ASSERT(1 == list_get(list, 0, datas) && 0 == datas[0]);
    CU_ASSERT(1 == list_get(list, 1, datas) && 1 == datas[0]);
    CU_ASSERT(1 == list_get(list, 2, datas) && 2 == datas[0]);
    CU_ASSERT(1 == list_get(list, 3, datas) && 3 == datas[0]);
    CU_ASSERT(1 == list_get(list, 4, datas) && 4 == datas[0]);
    CU_ASSERT(0 == list_destroy(list));

    CU_ASSERT(0 != (list = list_concat(list3, list1)));
    CU_ASSERT(5 == list_length(list));
    CU_ASSERT(1 == list_get(list, 0, datas) && 0 == datas[0]);
    CU_ASSERT(1 == list_get(list, 1, datas) && 1 == datas[0]);
    CU_ASSERT(1 == list_get(list, 2, datas) && 2 == datas[0]);
    CU_ASSERT(1 == list_get(list, 3, datas) && 3 == datas[0]);
    CU_ASSERT(1 == list_get(list, 4, datas) && 4 == datas[0]);
    CU_ASSERT(0 == list_destroy(list));

    CU_ASSERT(0 != (list = list_concat(list3, list4)));
    CU_ASSERT(0 == list_length(list));
    CU_ASSERT(0 == list_destroy(list));

    // destroy list
    CU_ASSERT(0 == list_destroy(list1));
    CU_ASSERT(0 == list_destroy(list2));
}

static void test_splice(void)
{
    intptr_t list, list1;
    intptr_t data, datas[5] = {0, 1, 2, 3, 4};

    // Create a list
    CU_ASSERT(0 != (list = list_create(5, datas)));

    // splice
    CU_ASSERT(0 != (list1 = list_splice(list, 0, 0, 0, NULL)));
    CU_ASSERT(0 == list_length(list1));
    CU_ASSERT(0 == list_destroy(list1));


    CU_ASSERT(0 != (list1 = list_splice(list, 2, 3, 0, NULL)));
    CU_ASSERT(4 == list_length(list));
    CU_ASSERT(1 == list_length(list1));
    CU_ASSERT(1 == list_get(list1, 0, &data) && 2 == data);
    CU_ASSERT(0 == list_destroy(list1));


    CU_ASSERT(0 != (list1 = list_splice(list, 1, 3, 5, datas)));
    CU_ASSERT(2 == list_length(list1));
    CU_ASSERT(1 == list_get(list1, 0, &data) && 1 == data);
    CU_ASSERT(1 == list_get(list1, 1, &data) && 3 == data);
    CU_ASSERT(0 == list_destroy(list1));

    CU_ASSERT(7 == list_length(list));
    CU_ASSERT(1 == list_get(list, 0, &data) && 0 == data);
    CU_ASSERT(1 == list_get(list, 1, &data) && 0 == data);
    CU_ASSERT(1 == list_get(list, 2, &data) && 1 == data);
    CU_ASSERT(1 == list_get(list, 3, &data) && 2 == data);
    CU_ASSERT(1 == list_get(list, 4, &data) && 3 == data);
    CU_ASSERT(1 == list_get(list, 5, &data) && 4 == data);
    CU_ASSERT(1 == list_get(list, 6, &data) && 4 == data);

    // destroy
    CU_ASSERT(0 == list_destroy(list));
}

CU_pSuite test_esbl_list_entry()
{
    CU_pSuite suite = CU_add_suite("esbl list", test_setup, test_clean);

    if (suite) {
        CU_add_test(suite, "common",     test_common);
        CU_add_test(suite, "insert",    test_insert);
        CU_add_test(suite, "cut",       test_cut);
        CU_add_test(suite, "slice",     test_slice);
        CU_add_test(suite, "concat",    test_concat);
        CU_add_test(suite, "splice",    test_splice);
    }

    return suite;
}

