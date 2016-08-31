#include "function.h"

intptr_t function_create(env_t *env, uint8_t *entry)
{
    function_t *fn = (function_t *) env_heap_alloc(env, sizeof(function_t));

    if (fn) {
        fn->magic = MAGIC_FUNCTION;
        fn->age   = 0;
        fn->entry = entry;
        fn->super = env->scope;
    }
    return (intptr_t) fn;
}

int function_destroy(intptr_t fn)
{
    return 0;
}

