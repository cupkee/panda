

#ifndef __LANG_COMPILE_INC__
#define __LANG_COMPILE_INC__

#include "config.h"

#include "env.h"
#include "ast.h"
#include "symtbl.h"

typedef struct module_t {
    double  *nums;
    uint8_t *code;
} module_t;

typedef struct compile_t {
    int error;
    int code_size;
    int code_pos;
    int nums_size;
    int nums_pos;
    double  *nums_buf;
    uint8_t *code_buf;
} compile_t;

int compile_init(compile_t *cpl, int code_size, int nums_size);
int compile_deinit(compile_t *cpl);

int compile_stmt(compile_t *cpl, stmt_t *stmt, void (*cb)(void *, void *), void *user_data);

static inline int compile_build_module(compile_t *cpl, module_t *mod) {
    mod->code = cpl->code_buf;
    mod->nums = cpl->nums_buf;

    return 0;
}

#endif /* __LANG_COMPILE_INC__ */

