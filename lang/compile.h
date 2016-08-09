

#ifndef __LANG_COMPILE_INC__
#define __LANG_COMPILE_INC__

#include "config.h"

#include "env.h"
#include "ast.h"
#include "interp.h"
#include "function.h"

typedef struct compile_func_t {
    int16_t owner;
    uint8_t var_max;
    uint8_t var_num;
    uint8_t arg_num;
    uint16_t code_max;
    uint16_t code_num;
    uint8_t  *code_buf;
    intptr_t *var_map;
} compile_func_t;

typedef struct compile_t {
    int     error;     //

    int16_t bgn_pos;   // for loop continue
    int16_t skip_pos;  // for loop break

    uint16_t func_size;
    uint16_t func_num;
    uint16_t func_cur;
    uint16_t func_offset;

    env_t  *env;
    heap_t  heap;

    compile_func_t *func_buf;
} compile_t;

int compile_init(compile_t *cpl, env_t *env, void *heap_ptr, int heap_size);
int compile_deinit(compile_t *cpl);

int compile_arg_add(compile_t *cpl, intptr_t sym_id);
int compile_var_add(compile_t *cpl, intptr_t sym_id);
int compile_var_get(compile_t *cpl, intptr_t sym_id);

int compile_stmt(compile_t *cpl, stmt_t *stmt);
int compile_one_stmt(compile_t *cpl, stmt_t *stmt);
int compile_vmap_copy(compile_t *cpl, intptr_t *buf, int size);

int compile_code_relocate(compile_t *cpl);

static inline intptr_t compile_sym_add(compile_t *cpl, const char *sym)
{
    return env_symbal_add(cpl->env, sym);
}

static inline intptr_t compile_sym_find(compile_t *cpl, const char *sym)
{
    return env_symbal_get(cpl->env, sym);
}

static inline void compile_clear_main(compile_t *cpl) {
    if (cpl && cpl->env) {
        executable_t *exe = &cpl->env->exe;
        exe->main_code_end = 0;
    }
}

static inline int compile_var_num(compile_t *cpl) {
    return (cpl && cpl->func_buf && cpl->func_num) ?
            cpl->func_buf[cpl->func_cur].var_num : 0;
}

static inline int compile_arg_num(compile_t *cpl) {
    return (cpl && cpl->func_buf && cpl->func_num) ?
            cpl->func_buf[cpl->func_cur].arg_num : 0;
}


#endif /* __LANG_COMPILE_INC__ */

