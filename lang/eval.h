


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
    env_t env;
    //compile_t cpl;
    uint8_t *func_code;
    uint8_t *main_code;
    uint16_t func_code_max;
    uint16_t func_code_end;
    uint16_t main_code_max;
    uint16_t main_code_end;
    uint16_t main_var_num;
    intptr_t main_var_map[EVAL_MAIN_VAR_MAX];

    uint8_t *func_map[8];
} eval_env_t;

int eval_env_init_mini(eval_env_t *env, void *mem_ptr, int mem_size, void *heap_ptr, int heap_size, val_t *stack_ptr, int stack_size);
int eval_env_deinit(eval_env_t *env);

int eval_env_add_var(eval_env_t *env, const char *name, val_t *value);
int eval_env_set_var(eval_env_t *env, const char *name, val_t *value);
int eval_env_get_var(eval_env_t *env, const char *name, val_t **value);

int eval_env_add_native(eval_env_t *env, const char *name, function_native_t function);

int eval_string(eval_env_t *env, const char *input, val_t **result);

#endif /* __LANG_EVAL_INC__ */

