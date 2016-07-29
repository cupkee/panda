


#ifndef __LANG_EVAL_INC__
#define __LANG_EVAL_INC__

#include "config.h"

#include "ast.h"
#include "val.h"
#include "env.h"
#include "interp.h"
#include "compile.h"
#include "function.h"

typedef struct eval_env_t {
    scope_t *scope;

    intptr_t sym_tbl;
    compile_t cpl;
} eval_env_t;

int eval_env_init(eval_env_t *env);
int eval_env_deinit(eval_env_t *env);

int eval_env_add_var(eval_env_t *env, const char *name, val_t value);
int eval_env_set_var(eval_env_t *env, const char *name, val_t value);
int eval_env_get_var(eval_env_t *env, const char *name, val_t **value);

int eval_env_add_native(eval_env_t *env, const char *name, function_native_t native);

int eval_string(interp_t *interp, eval_env_t *env, const char *input, val_t **result);


#endif /* __LANG_EVAL_INC__ */

