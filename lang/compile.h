

#ifndef __LANG_COMPILE_INC__
#define __LANG_COMPILE_INC__

#include "config.h"

#include "env.h"
#include "ast.h"
#include "symtbl.h"

typedef struct fn_template_t {
    uint8_t var_num;
    uint8_t arg_num;
    int     size;
    uint8_t *code;
} fn_template_t;

typedef struct module_t {
    double  *nums;
    intptr_t*natives;

    int entry;
    fn_template_t ft[4];
} module_t;


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
    int error;
    int nums_size;
    int nums_pos;
    double  *nums_buf;

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
int compile_arg_add(compile_t *cpl, intptr_t sym_id);
int compile_var_add(compile_t *cpl, intptr_t sym_id);
int compile_var_get(compile_t *cpl, intptr_t sym_id);
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

int compile_stmt(compile_t *cpl, stmt_t *stmt, void (*cb)(void *, void *), void *user_data);
int compile_one_stmt(compile_t *cpl, stmt_t *stmt, void (*cb)(void *, void *), void *user_data);

static inline void compile_build_module(compile_t *cpl, module_t *mod)
{
    int i;

    mod->nums = cpl->nums_buf;
    mod->entry = 0;

    for (i = 0; i < cpl->func_num; i++) {
        mod->ft[i].var_num = cpl->func_buf[i].var_num;
        mod->ft[i].arg_num = cpl->func_buf[i].arg_num;
        mod->ft[i].size = cpl->func_buf[i].code_size;
        mod->ft[i].code = cpl->func_buf[i].code_buf;
    }
}

#endif /* __LANG_COMPILE_INC__ */

