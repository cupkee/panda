#include "function.h"

intptr_t function_create(env_t *env, uint8_t *code, int size, uint8_t vn, uint8_t an)
{
    function_t *fn = (function_t *) env_heap_alloc(env, sizeof(function_t));

    if (fn) {
        fn->magic = MAGIC_FUNCTION;
        fn->code = code;
        fn->size = size;
        fn->arg_num = an;
        fn->var_num = vn;
        fn->super = env->scope;
    }
    return (intptr_t) fn;
}

int function_destroy(intptr_t fn)
{
    return 0;
}

int function_call(val_t *fv, env_t *env, int ac, val_t *av, uint8_t **pc)
{
    uint8_t *entry;

    if (NULL == (entry = env_frame_setup(env, *pc, fv, ac, av))) {
        return -1;
    }
    *pc = entry;

    return 0;
}

int function_call_native(val_t *fv, env_t *env, int ac, val_t *av)
{
    function_native_t fn = (function_native_t) val_2_intptr(fv);

    env_native_frame_setup(env, ac);
    env_native_return(env, fn(env, ac, av));
    //env_return_noframe(env, ac, fn(env, ac, av));

    return 0;
}

