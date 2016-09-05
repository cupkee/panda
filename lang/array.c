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

