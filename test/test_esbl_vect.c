#include <stdio.h>
#include <string.h>

#include "cunit/CUnit.h"
#include "cunit/CUnit_Basic.h"

#include "esbl/vect.h"

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
    intptr_t vect;
    intptr_t data;
    intptr_t datas[8] = {9, 8, 7, 6, 5, 4, 3, 2};

    CU_ASSERT(0 != (vect = vect_create(8, datas)));
    CU_ASSERT(8 == vect_length(vect));

    CU_ASSERT(0 == vect_set(vect, 8, 8));
    CU_ASSERT(0 == vect_get(vect, 8, &data));

    CU_ASSERT(1 == vect_get(vect, 0, &data) && 9 == data);
    CU_ASSERT(1 == vect_get(vect, 7, &data) && 2 == data);

    CU_ASSERT(1 == vect_set(vect, 0, 0));
    CU_ASSERT(1 == vect_set(vect, 1, 1));
    CU_ASSERT(1 == vect_set(vect, 2, 2));
    CU_ASSERT(1 == vect_set(vect, 3, 3));
    CU_ASSERT(1 == vect_set(vect, 4, 4));
    CU_ASSERT(1 == vect_set(vect, 5, 5));
    CU_ASSERT(1 == vect_set(vect, 6, 6));
    CU_ASSERT(1 == vect_set(vect, 7, 7));

    CU_ASSERT(1 == vect_push(vect, 8));
    CU_ASSERT(1 == vect_push(vect, 9));
    CU_ASSERT(1 == vect_push(vect, 10));
    CU_ASSERT(1 == vect_push(vect, 11));
    CU_ASSERT(1 == vect_push(vect, 12));
    CU_ASSERT(1 == vect_push(vect, 13));
    CU_ASSERT(1 == vect_push(vect, 14));
    CU_ASSERT(1 == vect_push(vect, 15));
    CU_ASSERT(1 == vect_push(vect, 16));
    CU_ASSERT(17 == vect_length(vect));

    CU_ASSERT(1 == vect_lpush(vect, -1));
    CU_ASSERT(1 == vect_lpush(vect, -2));
    CU_ASSERT(1 == vect_lpush(vect, -3));
    CU_ASSERT(1 == vect_lpush(vect, -4));
    CU_ASSERT(1 == vect_lpush(vect, -5));
    CU_ASSERT(1 == vect_lpush(vect, -6));
    CU_ASSERT(1 == vect_lpush(vect, -7));
    CU_ASSERT(1 == vect_lpush(vect, -8));
    CU_ASSERT(25 == vect_length(vect));

    CU_ASSERT(1 == vect_get(vect, 0, &data) && -8 == data);
    CU_ASSERT(1 == vect_get(vect, 1, &data) && -7 == data);
    CU_ASSERT(1 == vect_get(vect, 7, &data) && -1 == data);
    CU_ASSERT(1 == vect_get(vect, 8, &data) && 0 == data);
    CU_ASSERT(1 == vect_get(vect, 15, &data) && 7 == data);
    CU_ASSERT(1 == vect_get(vect, 16, &data) && 8 == data);
    CU_ASSERT(1 == vect_get(vect, 24, &data) && 16 == data);

    CU_ASSERT(1 == vect_lpush(vect, -9));
    CU_ASSERT(1 == vect_lpush(vect, -10));
    CU_ASSERT(1 == vect_lpush(vect, -11));
    CU_ASSERT(1 == vect_lpush(vect, -12));
    CU_ASSERT(1 == vect_lpush(vect, -13));
    CU_ASSERT(1 == vect_lpush(vect, -14));
    CU_ASSERT(1 == vect_lpush(vect, -15));
    CU_ASSERT(1 == vect_lpush(vect, -16));
    CU_ASSERT(33 == vect_length(vect));

    CU_ASSERT(1 == vect_get(vect, 0, &data) && -16 == data);
    CU_ASSERT(1 == vect_get(vect, 15, &data) && -1 == data);
    CU_ASSERT(1 == vect_get(vect, 16, &data) && 0 == data);
    CU_ASSERT(1 == vect_get(vect, 32, &data) && 16 == data);

    CU_ASSERT(1 == vect_lpop(vect, &data) && -16 == data);
    CU_ASSERT(1 == vect_pop(vect, &data) && 16 == data);
    CU_ASSERT(31 == vect_length(vect));

    CU_ASSERT(0 == vect_destroy(vect));

    //
    CU_ASSERT(0 != (vect = vect_create(0, NULL)));
    CU_ASSERT(0 == vect_length(vect));
    CU_ASSERT(1 == vect_lpush(vect, -1));
    CU_ASSERT(1 == vect_push(vect, 1));
    CU_ASSERT(2 == vect_length(vect));
    CU_ASSERT(1 == vect_lpop(vect, &data) && -1 == data);
    CU_ASSERT(1 == vect_lpop(vect, &data) && 1 == data);
    CU_ASSERT(0 == vect_length(vect));
    CU_ASSERT(1 == vect_push(vect, 2));
    CU_ASSERT(1 == vect_lpush(vect, -2));
    CU_ASSERT(2 == vect_length(vect));
    CU_ASSERT(1 == vect_pop(vect, &data) && 2 == data);
    CU_ASSERT(1 == vect_pop(vect, &data) && -2 == data);
    CU_ASSERT(0 == vect_length(vect));
    CU_ASSERT(1 == vect_push(vect, 3));
    CU_ASSERT(1 == vect_lpush(vect, -3));
    CU_ASSERT(1 == vect_get(vect, 0, &data) && -3 == data);
    CU_ASSERT(1 == vect_get(vect, 1, &data) && 3 == data);

    CU_ASSERT(0 == vect_destroy(vect));
}

static int test_match(intptr_t a, intptr_t b)
{
    return a - b;
}

static int test_match_anti(intptr_t a, intptr_t b)
{
    return b - a;
}

static void test_index_of(void)
{
    intptr_t vect;
    intptr_t datas[10] = {9, 8, 8, 6, 5, 4, 0, 2, 1, 0};

    CU_ASSERT(0 != (vect = vect_create(10, datas)));
    CU_ASSERT(10 == vect_length(vect));

    CU_ASSERT(0 == vect_index_of(vect, 0, 9, NULL));
    CU_ASSERT(0 == vect_index_of(vect, 0, 9, test_match));
    CU_ASSERT(1 == vect_index_of(vect, 0, 8, NULL));
    CU_ASSERT(1 == vect_index_of(vect, 0, 8, test_match));
    CU_ASSERT(3 == vect_index_of(vect, 0, 6, NULL));
    CU_ASSERT(3 == vect_index_of(vect, 0, 6, test_match));
    CU_ASSERT(4 == vect_index_of(vect, 0, 5, NULL));
    CU_ASSERT(4 == vect_index_of(vect, 0, 5, test_match));
    CU_ASSERT(6 == vect_index_of(vect, 0, 0, NULL));
    CU_ASSERT(6 == vect_index_of(vect, 0, 0, test_match));
    CU_ASSERT(8 == vect_index_of(vect, 0, 1, NULL));
    CU_ASSERT(8 == vect_index_of(vect, 0, 1, test_match));

    CU_ASSERT(2 == vect_index_of(vect, 2, 8, NULL));
    CU_ASSERT(2 == vect_index_of(vect, 2, 8, test_match));
    CU_ASSERT(9 == vect_index_of(vect, 7, 0, NULL));
    CU_ASSERT(9 == vect_index_of(vect, 7, 0, test_match));

    CU_ASSERT(-1 == vect_index_of(vect, 0, 7, NULL));
    CU_ASSERT(-1 == vect_index_of(vect, 0, 7, test_match));
    CU_ASSERT(-1 == vect_index_of(vect, 0, 3, NULL));
    CU_ASSERT(-1 == vect_index_of(vect, 0, 3, test_match));

    CU_ASSERT(0 == vect_destroy(vect));
}

static void test_insert(void)
{
    intptr_t vect;
    intptr_t data;
    intptr_t datas[10] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

    CU_ASSERT(0 != (vect = vect_create(0, NULL)));
    CU_ASSERT(0 == vect_length(vect));

    CU_ASSERT(0 == vect_insert(vect, 1, 10, datas));

    CU_ASSERT(10 == vect_insert(vect, 0, 10, datas));
    CU_ASSERT(1 == vect_get(vect, 0, &data) && 9 == data);
    CU_ASSERT(1 == vect_get(vect, 9, &data) && 0 == data);


    CU_ASSERT(10 == vect_insert(vect, 5, 10, datas));
    CU_ASSERT(20 == vect_length(vect));
    CU_ASSERT(1 == vect_get(vect, 0, &data) && 9 == data);
    CU_ASSERT(1 == vect_get(vect, 5, &data) && 9 == data);
    CU_ASSERT(1 == vect_get(vect, 14, &data) && 0 == data);
    CU_ASSERT(1 == vect_get(vect, 19, &data) && 0 == data);


    CU_ASSERT(10 == vect_insert(vect, 20, 10, datas));
    CU_ASSERT(30 == vect_length(vect));
    CU_ASSERT(1 == vect_get(vect, 0, &data) && 9 == data);
    CU_ASSERT(1 == vect_get(vect, 5, &data) && 9 == data);
    CU_ASSERT(1 == vect_get(vect, 14, &data) && 0 == data);
    CU_ASSERT(1 == vect_get(vect, 19, &data) && 0 == data);
    CU_ASSERT(1 == vect_get(vect, 20, &data) && 9 == data);
    CU_ASSERT(1 == vect_get(vect, 29, &data) && 0 == data);

    CU_ASSERT(10 == vect_insert(vect, 0, 10, datas));
    CU_ASSERT(40 == vect_length(vect));
    CU_ASSERT(1 == vect_get(vect, 0, &data) && 9 == data);
    CU_ASSERT(1 == vect_get(vect, 9, &data) && 0 == data);
    CU_ASSERT(1 == vect_get(vect, 10, &data) && 9 == data);
    CU_ASSERT(1 == vect_get(vect, 15, &data) && 9 == data);
    CU_ASSERT(1 == vect_get(vect, 24, &data) && 0 == data);
    CU_ASSERT(1 == vect_get(vect, 29, &data) && 0 == data);
    CU_ASSERT(1 == vect_get(vect, 30, &data) && 9 == data);
    CU_ASSERT(1 == vect_get(vect, 39, &data) && 0 == data);

    CU_ASSERT(0 == vect_destroy(vect));
}

static void test_slice(void)
{
    intptr_t vect, vect2;
    intptr_t data;
    intptr_t datas[10] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

    CU_ASSERT(0 != (vect = vect_create(10, datas)));
    CU_ASSERT(10 == vect_length(vect));

    CU_ASSERT(1 == vect_get(vect, 0, &data) && 9 == data);
    CU_ASSERT(1 == vect_get(vect, 9, &data) && 0 == data);

    CU_ASSERT(0 != (vect2 = vect_slice(vect, 0, vect_length(vect))));
    CU_ASSERT(vect_length(vect2) == vect_length(vect));
    CU_ASSERT(1 == vect_get(vect2, 0, &data) && 9 == data);
    CU_ASSERT(1 == vect_get(vect2, 9, &data) && 0 == data);
    CU_ASSERT(0 == vect_destroy(vect2));

    CU_ASSERT(0 != (vect2 = vect_slice(vect, 0, -1)));
    CU_ASSERT(vect_length(vect2) == vect_length(vect) - 1);
    CU_ASSERT(1 == vect_get(vect2, 0, &data) && 9 == data);
    CU_ASSERT(1 == vect_get(vect2, 8, &data) && 1 == data);
    CU_ASSERT(0 == vect_get(vect2, 9, &data));
    CU_ASSERT(0 == vect_destroy(vect2));

    CU_ASSERT(0 != (vect2 = vect_slice(vect, 1, 2)));
    CU_ASSERT(vect_length(vect2) == 1);
    CU_ASSERT(1 == vect_get(vect2, 0, &data) && 8 == data);
    CU_ASSERT(0 == vect_get(vect2, 1, &data));
    CU_ASSERT(0 == vect_destroy(vect2));

    CU_ASSERT(0 != (vect2 = vect_slice(vect, -10, -11)));
    CU_ASSERT(vect_length(vect2) == 9);
    CU_ASSERT(1 == vect_get(vect2, 0, &data) && 9 == data);
    CU_ASSERT(1 == vect_get(vect2, 8, &data) && 1 == data);
    CU_ASSERT(0 == vect_get(vect2, 9, &data));
    CU_ASSERT(0 == vect_destroy(vect2));

    CU_ASSERT(0 != (vect2 = vect_slice(vect, 10, -11)));
    CU_ASSERT(vect_length(vect2) == 0);
    CU_ASSERT(0 == vect_get(vect2, 0, &data));
    CU_ASSERT(0 == vect_destroy(vect2));

    CU_ASSERT(0 != (vect2 = vect_slice(vect, 3, 3)));
    CU_ASSERT(vect_length(vect2) == 0);
    CU_ASSERT(0 == vect_get(vect2, 0, &data));
    CU_ASSERT(0 == vect_destroy(vect2));

    CU_ASSERT(0 != (vect2 = vect_slice(vect, 4, 3)));
    CU_ASSERT(vect_length(vect2) == 0);
    CU_ASSERT(0 == vect_get(vect2, 0, &data));
    CU_ASSERT(0 == vect_destroy(vect2));

    CU_ASSERT(0 == vect_destroy(vect));
}

static void test_cut(void)
{
    intptr_t vect, vect2;
    intptr_t data;
    intptr_t datas[10] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

    CU_ASSERT(0 != (vect = vect_create(10, datas)));
    CU_ASSERT(10 == vect_length(vect));

    CU_ASSERT(1 == vect_get(vect, 0, &data) && 9 == data);
    CU_ASSERT(1 == vect_get(vect, 9, &data) && 0 == data);

    CU_ASSERT(0 != (vect2 = vect_cut(vect, 10, -11)));
    CU_ASSERT(0 == vect_length(vect2));
    CU_ASSERT(0 == vect_get(vect2, 0, &data));
    CU_ASSERT(0 == vect_destroy(vect2));

    CU_ASSERT(0 != (vect2 = vect_cut(vect, 3, 3)));
    CU_ASSERT(0 == vect_length(vect2));
    CU_ASSERT(0 == vect_get(vect2, 0, &data));
    CU_ASSERT(0 == vect_destroy(vect2));

    CU_ASSERT(0 != (vect2 = vect_cut(vect, 4, 3)));
    CU_ASSERT(0 == vect_length(vect2));
    CU_ASSERT(0 == vect_get(vect2, 0, &data));
    CU_ASSERT(0 == vect_destroy(vect2));

    //cut all
    CU_ASSERT(0 != (vect2 = vect_cut(vect, 0, vect_length(vect))));
    CU_ASSERT(0 == vect_length(vect));
    CU_ASSERT(10 == vect_length(vect2));
    CU_ASSERT(1 == vect_get(vect2, 0, &data) && 9 == data);
    CU_ASSERT(1 == vect_get(vect2, 9, &data) && 0 == data);
    CU_ASSERT(0 == vect_destroy(vect));
    CU_ASSERT(0 == vect_destroy(vect2));

    // cut from start
    CU_ASSERT(0 != (vect = vect_create(10, datas)));
    CU_ASSERT(0 != (vect2 = vect_cut(vect, 0, -1)));
    CU_ASSERT(1 == vect_length(vect));
    CU_ASSERT(9 == vect_length(vect2));
    CU_ASSERT(1 == vect_get(vect, 0, &data) && 0 == data);
    CU_ASSERT(1 == vect_get(vect2, 0, &data) && 9 == data);
    CU_ASSERT(1 == vect_get(vect2, 8, &data) && 1 == data);
    CU_ASSERT(0 == vect_get(vect2, 9, &data));
    CU_ASSERT(0 == vect_destroy(vect));
    CU_ASSERT(0 == vect_destroy(vect2));

    CU_ASSERT(0 != (vect = vect_create(10, datas)));
    CU_ASSERT(0 != (vect2 = vect_cut(vect, -10, -11)));
    CU_ASSERT(1 == vect_length(vect));
    CU_ASSERT(9 == vect_length(vect2));
    CU_ASSERT(1 == vect_get(vect, 0, &data) && 0 == data);
    CU_ASSERT(1 == vect_get(vect2, 0, &data) && 9 == data);
    CU_ASSERT(1 == vect_get(vect2, 8, &data) && 1 == data);
    CU_ASSERT(0 == vect_get(vect2, 9, &data));
    CU_ASSERT(0 == vect_destroy(vect2));
    CU_ASSERT(0 == vect_destroy(vect));

    // cut to end
    CU_ASSERT(0 != (vect = vect_create(10, datas)));
    CU_ASSERT(0 != (vect2 = vect_cut(vect, 1, vect_length(vect))));
    CU_ASSERT(1 == vect_length(vect));
    CU_ASSERT(9 == vect_length(vect2));
    CU_ASSERT(1 == vect_get(vect, 0, &data) && 9 == data);
    CU_ASSERT(1 == vect_get(vect2, 0, &data) && 8 == data);
    CU_ASSERT(1 == vect_get(vect2, 8, &data) && 0 == data);
    CU_ASSERT(0 == vect_get(vect2, 9, &data));
    CU_ASSERT(0 == vect_destroy(vect));
    CU_ASSERT(0 == vect_destroy(vect2));

    // cut mid
    CU_ASSERT(0 != (vect = vect_create(10, datas)));
    CU_ASSERT(0 != (vect2 = vect_cut(vect, 1, 2)));
    CU_ASSERT(9 == vect_length(vect));
    CU_ASSERT(1 == vect_length(vect2));
    CU_ASSERT(1 == vect_get(vect, 0, &data) && 9 == data);
    CU_ASSERT(1 == vect_get(vect, 1, &data) && 7 == data);
    CU_ASSERT(1 == vect_get(vect, 8, &data) && 0 == data);
    CU_ASSERT(1 == vect_get(vect2, 0, &data) && 8 == data);
    CU_ASSERT(0 == vect_get(vect2, 1, &data));
    CU_ASSERT(0 == vect_destroy(vect));
    CU_ASSERT(0 == vect_destroy(vect2));
}

static void test_splice(void)
{
    intptr_t vect, vect1;
    intptr_t data, datas[5] = {0, 1, 2, 3, 4};

    // Create a vect
    CU_ASSERT(0 != (vect = vect_create(5, datas)));

    // splice
    CU_ASSERT(0 != (vect1 = vect_splice(vect, 0, 0, 0, NULL)));
    CU_ASSERT(0 == vect_length(vect1));
    CU_ASSERT(0 == vect_destroy(vect1));


    CU_ASSERT(0 != (vect1 = vect_splice(vect, 2, 3, 0, NULL)));
    CU_ASSERT(4 == vect_length(vect));
    CU_ASSERT(1 == vect_length(vect1));
    CU_ASSERT(1 == vect_get(vect1, 0, &data) && 2 == data);
    CU_ASSERT(0 == vect_destroy(vect1));


    CU_ASSERT(0 != (vect1 = vect_splice(vect, 1, 3, 5, datas)));
    CU_ASSERT(2 == vect_length(vect1));
    CU_ASSERT(1 == vect_get(vect1, 0, &data) && 1 == data);
    CU_ASSERT(1 == vect_get(vect1, 1, &data) && 3 == data);
    CU_ASSERT(0 == vect_destroy(vect1));

    CU_ASSERT(7 == vect_length(vect));
    CU_ASSERT(1 == vect_get(vect, 0, &data) && 0 == data);
    CU_ASSERT(1 == vect_get(vect, 1, &data) && 0 == data);
    CU_ASSERT(1 == vect_get(vect, 2, &data) && 1 == data);
    CU_ASSERT(1 == vect_get(vect, 3, &data) && 2 == data);
    CU_ASSERT(1 == vect_get(vect, 4, &data) && 3 == data);
    CU_ASSERT(1 == vect_get(vect, 5, &data) && 4 == data);
    CU_ASSERT(1 == vect_get(vect, 6, &data) && 4 == data);

    // destroy
    CU_ASSERT(0 == vect_destroy(vect));
}

static void test_concat(void)
{
    intptr_t vect, vect1, vect2, vect3, vect4;
    intptr_t datas[5] = {0, 1, 2, 3, 4};

    // Create a vect
    CU_ASSERT(0 != (vect1 = vect_create(5, datas)));
    CU_ASSERT(0 != (vect2 = vect_create(5, datas)));
    CU_ASSERT(0 != (vect3 = vect_create(0, NULL)));
    CU_ASSERT(0 != (vect4 = vect_create(0, NULL)));

    CU_ASSERT(0 != (vect = vect_concat(vect1, vect2)));
    CU_ASSERT(10 == vect_length(vect));
    CU_ASSERT(1 == vect_get(vect, 0, datas) && 0 == datas[0]);
    CU_ASSERT(1 == vect_get(vect, 1, datas) && 1 == datas[0]);
    CU_ASSERT(1 == vect_get(vect, 2, datas) && 2 == datas[0]);
    CU_ASSERT(1 == vect_get(vect, 3, datas) && 3 == datas[0]);
    CU_ASSERT(1 == vect_get(vect, 4, datas) && 4 == datas[0]);
    CU_ASSERT(1 == vect_get(vect, 5, datas) && 0 == datas[0]);
    CU_ASSERT(1 == vect_get(vect, 6, datas) && 1 == datas[0]);
    CU_ASSERT(1 == vect_get(vect, 7, datas) && 2 == datas[0]);
    CU_ASSERT(1 == vect_get(vect, 8, datas) && 3 == datas[0]);
    CU_ASSERT(1 == vect_get(vect, 9, datas) && 4 == datas[0]);
    CU_ASSERT(0 == vect_destroy(vect));

    CU_ASSERT(0 != (vect = vect_concat(vect1, vect3)));
    CU_ASSERT(5 == vect_length(vect));
    CU_ASSERT(1 == vect_get(vect, 0, datas) && 0 == datas[0]);
    CU_ASSERT(1 == vect_get(vect, 1, datas) && 1 == datas[0]);
    CU_ASSERT(1 == vect_get(vect, 2, datas) && 2 == datas[0]);
    CU_ASSERT(1 == vect_get(vect, 3, datas) && 3 == datas[0]);
    CU_ASSERT(1 == vect_get(vect, 4, datas) && 4 == datas[0]);
    CU_ASSERT(0 == vect_destroy(vect));

    CU_ASSERT(0 != (vect = vect_concat(vect3, vect1)));
    CU_ASSERT(5 == vect_length(vect));
    CU_ASSERT(1 == vect_get(vect, 0, datas) && 0 == datas[0]);
    CU_ASSERT(1 == vect_get(vect, 1, datas) && 1 == datas[0]);
    CU_ASSERT(1 == vect_get(vect, 2, datas) && 2 == datas[0]);
    CU_ASSERT(1 == vect_get(vect, 3, datas) && 3 == datas[0]);
    CU_ASSERT(1 == vect_get(vect, 4, datas) && 4 == datas[0]);
    CU_ASSERT(0 == vect_destroy(vect));

    CU_ASSERT(0 != (vect = vect_concat(vect3, vect4)));
    CU_ASSERT(0 == vect_length(vect));
    CU_ASSERT(0 == vect_destroy(vect));

    // destroy vect
    CU_ASSERT(0 == vect_destroy(vect1));
    CU_ASSERT(0 == vect_destroy(vect2));
}

static void test_heapify(void)
{
    intptr_t vect;
    intptr_t datas[] = {3, 5, 0, 8, 6, 1, 5, 8, 6, 2, 4, 9, 4, 7, 0, 1, 8, 9, 7, 3, 1, 2, 5, 9, 7, 4, 0, 2, 6 };
    intptr_t curr, prev;
    int i, num = sizeof(datas) / sizeof(intptr_t);

    // Create a vect
    CU_ASSERT(0 != (vect = vect_create(sizeof(datas) / sizeof(intptr_t), datas)));
    CU_ASSERT(num == vect_length(vect));
    CU_ASSERT(0 == vect_heapify(vect, NULL));

    CU_ASSERT(1 == vect_heapify_extract(vect, &prev, NULL));
    for (i = 1; i < num; i++) {
        CU_ASSERT(1 == vect_heapify_extract(vect, &curr, NULL));
        CU_ASSERT(prev >= curr);
        //printf("[%.2d]: %ld <= %ld\n", i, prev, curr);
        prev = curr;
    }
    CU_ASSERT(0 == vect_length(vect));

    for (i = 0; i < num; i++) {
        if (i % 2 == 0) {
            CU_ASSERT(1 == vect_heapify_insert(vect, datas[i], NULL));
        } else {
            CU_ASSERT(1 == vect_heapify_insert(vect, datas[i], test_match));
        }
    }
    CU_ASSERT(1 == vect_heapify_extract(vect, &prev, NULL));
    for (i = 1; i < num; i++) {
        CU_ASSERT(1 == vect_heapify_extract(vect, &curr, NULL));
        CU_ASSERT(prev >= curr);
        //printf("[%.2d]: %ld <= %ld\n", i, prev, curr);
        prev = curr;
    }
    CU_ASSERT(0 == vect_length(vect));

    // destroy vect
    CU_ASSERT(0 == vect_destroy(vect));
}

static void test_sort(void)
{
    intptr_t vect;
    intptr_t datas[] = {3, 5, 0, 8, 6, 1, 5, 8, 6, 2, 4, 9, 4, 7, 0, 1, 8, 9, 7, 3, 1, 2, 5, 9, 7, 4, 0, 2, 6 };
    intptr_t curr, prev;
    int i, num = sizeof(datas) / sizeof(intptr_t);

    // Create a vect
    CU_ASSERT(0 != (vect = vect_create(sizeof(datas) / sizeof(intptr_t), datas)));
    CU_ASSERT(num == vect_length(vect));

    CU_ASSERT(0 == vect_sort(vect, NULL));
    CU_ASSERT(1 == vect_get(vect, 0, &prev));
    for (i = 1; i < num; i++) {
        CU_ASSERT(1 == vect_get(vect, i, &curr));
        CU_ASSERT(prev <= curr);
        //printf("[%.2d]: %ld <= %ld\n", i, prev, curr);
        prev = curr;
    }

    CU_ASSERT(0 == vect_sort(vect, test_match_anti));
    CU_ASSERT(1 == vect_get(vect, 0, &prev));
    for (i = 1; i < num; i++) {
        CU_ASSERT(1 == vect_get(vect, i, &curr));
        CU_ASSERT(prev >= curr);
        //printf("[%.2d]: %ld <= %ld\n", i, prev, curr);
        prev = curr;
    }

    CU_ASSERT(0 == vect_sort(vect, test_match));
    CU_ASSERT(1 == vect_get(vect, 0, &prev));
    for (i = 1; i < num; i++) {
        CU_ASSERT(1 == vect_get(vect, i, &curr));
        CU_ASSERT(prev <= curr);
        //printf("[%.2d]: %ld <= %ld\n", i, prev, curr);
        prev = curr;
    }

    CU_ASSERT(num == vect_length(vect));
    // destroy vect
    CU_ASSERT(0 == vect_destroy(vect));
}

static void test_reverse(void)
{
    intptr_t vect;
    intptr_t datas[] = {3, 5, 0, 8, 6, 1, 5, 8, 6, 2, 4, 9, 4, 7, 0, 1, 8, 9, 7, 3, 1, 2, 5, 9, 7, 4, 0, 2, 6 };
    intptr_t curr, prev;
    int i, num = sizeof(datas) / sizeof(intptr_t);

    // Create a vect
    CU_ASSERT(0 != (vect = vect_create(sizeof(datas) / sizeof(intptr_t), datas)));
    CU_ASSERT(num == vect_length(vect));

    CU_ASSERT(0 == vect_sort(vect, NULL));
    CU_ASSERT(0 == vect_reverse(vect));
    CU_ASSERT(1 == vect_get(vect, 0, &prev));
    for (i = 1; i < num; i++) {
        CU_ASSERT(1 == vect_get(vect, i, &curr));
        CU_ASSERT(prev >= curr);
        //printf("[%.2d]: %ld <= %ld\n", i, prev, curr);
        prev = curr;
    }

    CU_ASSERT(0 == vect_sort(vect, test_match_anti));
    CU_ASSERT(0 == vect_reverse(vect));
    CU_ASSERT(1 == vect_get(vect, 0, &prev));
    for (i = 1; i < num; i++) {
        CU_ASSERT(1 == vect_get(vect, i, &curr));
        CU_ASSERT(prev <= curr);
        //printf("[%.2d]: %ld <= %ld\n", i, prev, curr);
        prev = curr;
    }

    CU_ASSERT(num == vect_length(vect));
    // destroy vect
    CU_ASSERT(0 == vect_destroy(vect));
}

CU_pSuite test_esbl_vect_entry()
{
    CU_pSuite suite = CU_add_suite("esbl vect", test_setup, test_clean);

    if (suite) {
        CU_add_test(suite, "common",    test_common);
        CU_add_test(suite, "index_of",  test_index_of);
        CU_add_test(suite, "insert",    test_insert);
        CU_add_test(suite, "slice",     test_slice);
        CU_add_test(suite, "cut",       test_cut);
        CU_add_test(suite, "splice",    test_splice);
        CU_add_test(suite, "concat",    test_concat);
        CU_add_test(suite, "heapify",   test_heapify);
        CU_add_test(suite, "sort",      test_sort);
        CU_add_test(suite, "reverse",   test_reverse);
    }

    return suite;
}

