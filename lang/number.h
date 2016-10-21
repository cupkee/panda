/*
MIT License

Copyright (c) 2016 Lixing Ding <ding.lixing@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#ifndef __LANG_NUMBER_INC__
#define __LANG_NUMBER_INC__

#include "config.h"

#include "val.h"
#include "env.h"

static inline void number_incp(env_t *env, val_t *a, val_t *res) {
    (void) env;
    val_set_number(a, val_2_double(a) + 1);
    *res = *a;
}

static inline void number_inc(env_t *env, val_t *a, val_t *res) {
    *res = *a;
    val_set_number(a, val_2_double(a) + 1);
}

static inline void number_decp(env_t *env, val_t *a, val_t *res) {
    val_set_number(a, val_2_double(a) - 1);
    *res = *a;
}

static inline void number_dec(env_t *env, val_t *a, val_t *res) {
    *res = *a;
    val_set_number(a, val_2_double(a) - 1);
}

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

