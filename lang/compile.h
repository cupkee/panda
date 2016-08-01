

#ifndef __LANG_COMPILE_INC__
#define __LANG_COMPILE_INC__

#include "config.h"

#include "env.h"
#include "ast.h"
#include "symtbl.h"
#include "interp.h"
#include "module.h"
#include "function.h"

typedef struct compile_func_t {
    int owner;
    int var_size;
    int var_num;
    int arg_num;
    int code_size;
    int code_pos;
    uint8_t  *code_buf;
    intptr_t *vars_map;
} compile_func_t;

typedef struct compile_t {
    int error;     //

    int number_size;
    int number_pos;
    double  *number_buf;

    int string_size;
    int string_num;
    intptr_t *string_buf;

    int native_size;
    int native_num;
    intptr_t *native_buf;
    intptr_t *native_map;

    int bgn_pos;   // for loop continue
    int skip_pos;  // for loop break

    int func_size;
    int func_num;
    int func_cur;
    compile_func_t *func_buf;
    intptr_t sym_tbl;
} compile_t;

int compile_init(compile_t *cpl, intptr_t sym_tbl);
int compile_deinit(compile_t *cpl);

intptr_t compile_sym_add(compile_t *cpl, const char *sym);
intptr_t compile_sym_find(compile_t *cpl, const char *sym);

int compile_arg_add(compile_t *cpl, intptr_t sym_id);

int compile_var_add(compile_t *cpl, intptr_t sym_id);
int compile_var_get(compile_t *cpl, intptr_t sym_id);

int compile_native_add(compile_t *cpl, const char *name, function_native_t native);

int compile_stmt(compile_t *cpl, stmt_t *stmt);
int compile_one_stmt(compile_t *cpl, stmt_t *stmt, module_t *mod);

int compile_build_module(compile_t *cpl, module_t *mod);

static inline void compile_code_clean(compile_t *cpl) {
    if (cpl && cpl->func_buf && cpl->func_num)
        cpl->func_buf[cpl->func_cur].code_pos = 0;
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

