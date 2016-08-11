


#ifndef __LANG_EVAL_INC__
#define __LANG_EVAL_INC__

#include "config.h"

#include "ast.h"
#include "val.h"
#include "env.h"
#include "interp.h"
#include "compile.h"
#include "function.h"

int eval_env_init(env_t *env, void *mem_ptr, int mem_size, void *heap_ptr, int heap_size, val_t *stack_ptr, int stack_size);
int eval_string(env_t *env, void *mem_ptr, int mem_size, const char *input, val_t **result);

#endif /* __LANG_EVAL_INC__ */

