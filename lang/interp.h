


#ifndef __LANG_INTERP_INC__
#define __LANG_INTERP_INC__

#include "config.h"

#include "val.h"

typedef struct interp_t{
    int error;
    int skip;

    int ss;
    int sp;
    val_t *sb;
    val_t *result;
} interp_t;

interp_t *interp_init(interp_t *intrep, val_t *stack_ptr, int stack_size);

int interp_deinit(interp_t *interp);

static inline void interp_set_error(interp_t *interp, int error) {
    interp->error = error;
}

static inline val_t *interp_stack_peek(interp_t *interp) {
    return interp->sb + interp->sp;
}

static inline val_t *interp_stack_pop(interp_t *interp) {
    return interp->sb + interp->sp++;
}

static inline val_t *interp_stack_push(interp_t *interp) {
    return interp->sb + (--interp->sp);
}

static inline void interp_push_val(interp_t *interp, val_t v) {
    *interp_stack_push(interp) = v;
}

static inline void interp_push_ref(interp_t *interp, val_t *r) {
    val_set_reference(interp_stack_push(interp), r);
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

static inline void interp_push_script(interp_t *interp, intptr_t p) {
    val_set_script(interp_stack_push(interp), p);
}

static inline void interp_neg_stack(interp_t *interp) {
    val_t *s = interp_stack_peek(interp);

    *s = val_negation(*s);
}

static inline void interp_not_stack(interp_t *interp) {
    val_t *s = interp_stack_peek(interp);

    *s = val_not(*s);
}

static inline void interp_logic_not_stack(interp_t *interp) {
    val_t *s = interp_stack_peek(interp);

    *s = val_logic_not(*s);
}

static inline void interp_mul_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    *res = val_mul(*a, *b);
}

static inline void interp_div_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    *res = val_div(*a, *b);
}

static inline void interp_mod_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    *res = val_mod(*a, *b);
}

static inline void interp_add_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    *res = val_add(*a, *b);
}

static inline void interp_sub_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    *res = val_sub(*a, *b);
}

static inline void interp_and_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    *res = val_and(*a, *b);
}

static inline void interp_or_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    *res = val_or(*a, *b);
}

static inline void interp_xor_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    *res = val_xor(*a, *b);
}

static inline void interp_lshift_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    *res = val_lshift(*a, *b);
}

static inline void interp_rshift_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    *res = val_rshift(*a, *b);
}

static inline void interp_teq_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    *res = val_mk_boolean(val_teq(*a, *b));
}

static inline void interp_tne_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    *res = val_mk_boolean(!val_teq(*a, *b));
}

static inline void interp_tgt_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    *res = val_mk_boolean(val_tgt(*a, *b));
}

static inline void interp_tge_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    *res = val_mk_boolean(val_tge(*a, *b));
}

static inline void interp_tlt_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    *res = val_mk_boolean(val_tlt(*a, *b));
}

static inline void interp_tle_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    *res = val_mk_boolean(val_tle(*a, *b));
}

static inline void interp_assign(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    *res = *val_2_reference(*a) = *b;
}

#endif /* __LANG_INTERP_INC__ */

