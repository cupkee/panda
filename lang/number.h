

#ifndef __LANG_NUMBER_INC__
#define __LANG_NUMBER_INC__

#include "config.h"

#include "val.h"
#include "env.h"


static inline void number_add(env_t *env, val_t *a, val_t *b, val_t *res) {
    (void) env;
    if (val_is_number(b)) {
        val_set_number(res, val_2_double(a) + val_2_double(b));
    } else {
        val_set_nan(res);
    }
}

static inline void number_sub(env_t *env, val_t *a, val_t *b, val_t *res) {
    (void) env;
    if (val_is_number(b)) {
        val_set_number(res, val_2_double(a) - val_2_double(b));
    } else {
        val_set_nan(res);
    }
}

static inline void number_mul(env_t *env, val_t *a, val_t *b, val_t *res) {
    (void) env;
    if (val_is_number(b)) {
        val_set_number(res, val_2_double(a) * val_2_double(b));
    } else {
        val_set_nan(res);
    }
}

static inline void number_div(env_t *env, val_t *a, val_t *b, val_t *res) {
    (void) env;
    if (val_is_number(b) && 0 != val_2_double(b)) {
        val_set_number(res, val_2_double(a) / val_2_double(b));
    } else {
        val_set_nan(res);
    }
}

static inline void number_mod(env_t *env, val_t *a, val_t *b, val_t *res) {
    (void) env;
    if (val_is_number(b) && 0 != val_2_integer(b)) {
        val_set_number(res, val_2_integer(a) % val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline void number_and(env_t *env, val_t *a, val_t *b, val_t *res) {
    (void) env;
    if (val_is_number(b)) {
        val_set_number(res, val_2_integer(a) & val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline void number_or(env_t *env, val_t *a, val_t *b, val_t *res) {
    (void) env;
    if (val_is_number(b)) {
        val_set_number(res, val_2_integer(a) | val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline void number_xor(env_t *env, val_t *a, val_t *b, val_t *res) {
    (void) env;
    if (val_is_number(b)) {
        val_set_number(res, val_2_integer(a) ^ val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline void number_lshift(env_t *env, val_t *a, val_t *b, val_t *res) {
    (void) env;
    if (val_is_number(b)) {
        val_set_number(res, val_2_integer(a) << val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline void number_rshift(env_t *env, val_t *a, val_t *b, val_t *res) {
    (void) env;
    if (val_is_number(b)) {
        val_set_number(res, val_2_integer(a) >> val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

#endif /* __LANG_NUMBER_INC__ */

