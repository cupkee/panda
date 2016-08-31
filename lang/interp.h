


#ifndef __LANG_INTERP_INC__
#define __LANG_INTERP_INC__

#include "config.h"

#include "val.h"
#include "env.h"
#include "executable.h"

int interp_env_init_interactive(env_t *env, void *mem_ptr, int mem_size, void *heap_ptr, int heap_size, val_t *stack_ptr, int stack_size);
int interp_env_init_interpreter(env_t *env, void *mem_ptr, int mem_size, void *heap_ptr, int heap_size, val_t *stack_ptr, int stack_size);
int interp_env_init_executable (env_t *env, void *mem_ptr, int mem_size, void *heap_ptr, int heap_size, val_t *stack_ptr, int stack_size, executable_file_t *ef);

val_t interp_execute_call(env_t *env, int ac);
int interp_execute(env_t *env, val_t **result);
int interp_execute_string(env_t *env, const char *input, val_t **result);
int interp_execute_interactive(env_t *env, const char *input, char *(*input_more)(void), val_t **v);

static inline void interp_push_call_argument(env_t *env, val_t *v) {
    *env_stack_push(env) = *v;
}

static inline void interp_push_call_function(env_t *env, val_t *v) {
    *env_stack_push(env) = *v;
}

static inline void interp_stack_alloc(env_t *env, int n) {
    env->sp -= n;
}

static inline void interp_stack_release(env_t *env, int n) {
    env->sp += n;
}

static inline void interp_result_set(env_t *env, val_t v) {
    *(env->sb + (--env->sp)) = v;
}

#endif /* __LANG_INTERP_INC__ */

