#include "array.h"

intptr_t array_create(env_t *env, int ac, val_t *av)
{
    int size;
    array_t *array;

    size = ac < 4 ? 4 : ac;
    array = env_heap_alloc(env, sizeof(array_t) + sizeof(val_t) * size);
    if (array) {
        val_t *vals = (val_t *)(array + 1);

        array->magic = MAGIC_ARRAY;
        array->age = 0;
        array->elem_size = size;
        array->elem_num  = ac;
        array->elems = vals;

        memcpy(vals, av, sizeof(val_t) * ac);
    }

    return (intptr_t) array;
}

void array_elem_get(env_t *env, val_t *a, val_t *i, val_t *elem)
{
    array_t *array = (array_t *) val_2_intptr(a);
    int id = val_2_integer(i);

    if (id >= 0 && id < array->elem_num) {
        *elem = array->elems[id];
    } else {
        val_set_undefined(elem);
    }
}

void array_elem_set(env_t *env, val_t *a, val_t *i, val_t *elem)
{
    array_t *array = (array_t *) val_2_intptr(a);
    int id = val_2_integer(i);

    if (id >= 0 && id < array->elem_num) {
        array->elems[id] = *elem;
    } else {
        val_set_undefined(elem);
    }
}

val_t array_length(env_t *env, int ac, val_t *av)
{
    if (ac > 0 && val_is_array(av)) {
        array_t *a = (array_t *)val_2_intptr(av);

        return val_mk_number(a->elem_num);
    }
    env_set_error(env, ERR_InvalidInput);
    return val_mk_undefined();
}

static array_t *array_space_check_extend(env_t *env, val_t *self, int n)
{
    array_t *a = (array_t *)val_2_intptr(self);
    val_t *elems;
    int size;

    if (a->elem_size >= a->elem_num + n) {
        return a;
    }
    size = SIZE_ALIGN_16(a->elem_num + n);

    elems = env_heap_alloc(env, size * sizeof(val_t));
    if (elems) {
        a = (array_t *)val_2_intptr(self);
        memcpy(elems, a->elems, sizeof(val_t) * a->elem_num);
        a->elems = elems;
        return a;
    }

    return NULL;
}

val_t array_push(env_t *env, int ac, val_t *av)
{
    if (ac > 1 && val_is_array(av)) {
        array_t *a = array_space_check_extend(env, av, ac - 1);

        if (a) {
            memcpy(a->elems + a->elem_num, av + 1, sizeof(val_t) * (ac - 1));
            a->elem_num += ac - 1;
            return val_mk_number(a->elem_num);
        }
    } else {
        env_set_error(env, ERR_InvalidInput);
    }
    return val_mk_undefined();
}

val_t array_pop(env_t *env, int ac, val_t *av)
{
    if (ac > 0 && val_is_array(av)) {
        array_t *a = (array_t *)val_2_intptr(av);

        if (a->elem_num) {
            return a->elems[--a->elem_num];
        }
    } else {
        env_set_error(env, ERR_InvalidInput);
    }
    return val_mk_undefined();
}

