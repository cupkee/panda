#include "function.h"

typedef struct function_t {
    uint8_t arg_num;
    uint8_t var_num;
    int     size;
    uint8_t *code;
    scope_t *super;
} function_t;

intptr_t function_create(uint8_t *code, int size, uint8_t vn, uint8_t an)
{
    function_t *fn = (function_t *) malloc(sizeof(function_t));

    if (fn) {
        fn->code = code;
        fn->size = size;
        fn->arg_num = an;
        fn->var_num = vn;
        fn->super = NULL;
    }
    return (intptr_t) fn;
}

int function_destroy(intptr_t fn)
{
    if (fn) {
        free((void *)fn);
    }
    return 0;
}

int function_call(intptr_t fv, env_t *env, int ac, val_t *av, uint8_t **pc)
{
    function_t *fn = (function_t *) fv;

    if (!fn) {
        return -1;
    }

    if (fn->size == 0) { // empty script
        interp_result_set(env, val_mk_undefined());
        return 0;
    }

    if (0 != env_frame_setup(env, *pc, fn->super, fn->var_num, ac, av)) {
        return -1;
    }

    *pc = fn->code;
    return 0;
}

int function_call_native(intptr_t fv, env_t *env, int ac, val_t *av, uint8_t **pc)
{
    function_native_t fn = (function_native_t) fv;

    interp_result_set(env, fn(env, ac, av));

    return 0;
}

