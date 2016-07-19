


#ifndef __LANG_INTERP_INC__
#define __LANG_INTERP_INC__

#include "config.h"

#include "val.h"

typedef struct interp_t{
    int error;
    int ss;
    int sp;
    val_t *sb;
} interp_t;

interp_t *interp_init(interp_t *intrep, val_t *stack_ptr, int stack_size);

int interp_deinit(interp_t *interp);

static inline val_t *interp_stack_peek(interp_t *interp) {
    return interp->sb + interp->sp;
}

static inline val_t *interp_stack_pop(interp_t *interp) {
    return interp->sb + interp->sp++;
}

static inline val_t *interp_stack_push(interp_t *interp) {
    return interp->sb + (--interp->sp);
}

static inline void interp_push_undefined(interp_t *interp) {
    val_set_undefined(interp_stack_push(interp));
}

static inline void interp_push_nan(interp_t *interp) {
    val_set_nan(interp_stack_push(interp));
}

static inline void interp_push_number(interp_t *interp, int n) {
    val_set_number(interp_stack_push(interp), n);
}

static inline void interp_push_boolean(interp_t *interp, int b) {
    val_set_boolean(interp_stack_push(interp), b);
}

#endif /* __LANG_INTERP_INC__ */

