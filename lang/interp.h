


#ifndef __LANG_INTERP_INC__
#define __LANG_INTERP_INC__

#include "config.h"

#include "val.h"
#include "env.h"
#include "compile.h"

#define FRAME_SIZE (sizeof(interp_frame_t) / sizeof(val_t))

typedef struct interp_frame_t {
    int fp;
    int sp;
    intptr_t pc;
    intptr_t scope;
} interp_frame_t;

typedef struct interp_t {
    int error;
    int skip;

    int fp;
    int ss;
    int sp;
    val_t *sb;
    val_t *result;
} interp_t;

interp_t *interp_init(interp_t *intrep, val_t *stack_ptr, int stack_size);
int interp_deinit(interp_t *interp);

void interp_frame_setup(interp_t *interp, uint8_t *pc, scope_t *scope);
void interp_frame_restore(interp_t *interp, uint8_t **pc, scope_t **scope);
int interp_run(interp_t *interp, env_t *env, module_t *mod);

static inline void interp_stack_alloc(interp_t *interp, int n) {
    interp->sp -= n;
}

static inline void interp_stack_release(interp_t *interp, int n) {
    interp->sp += n;
}

static inline void interp_result_set(interp_t *interp, val_t v) {
    *(interp->sb + (--interp->sp)) = v;
}

#endif /* __LANG_INTERP_INC__ */

