


#ifndef __LANG_INTERP_INC__
#define __LANG_INTERP_INC__

#include "config.h"

#include "val.h"
#include "env.h"

typedef struct interp_t {
    int error;

    int fp;
    int ss;
    int sp;
    val_t *sb;
    val_t *result;
} interp_t;

int interp_run(env_t *env, module_t *mod);

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

