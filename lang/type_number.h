/* GPLv2 License
 *
 * Copyright (C) 2016-2018 Lixing Ding <ding.lixing@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 **/


#ifndef __LANG_NUMBER_INC__
#define __LANG_NUMBER_INC__

#include "def.h"

#include "val.h"
#include "env.h"

extern const val_metadata_t metadata_num;

static inline void number_incp(val_t *a, val_t *res) {
    val_set_number(a, val_2_double(a) + 1);
    *res = *a;
}

static inline void number_inc(val_t *a, val_t *res) {
    *res = *a;
    val_set_number(a, val_2_double(a) + 1);
}

static inline void number_decp(val_t *a, val_t *res) {
    val_set_number(a, val_2_double(a) - 1);
    *res = *a;
}

static inline void number_dec(val_t *a, val_t *res) {
    *res = *a;
    val_set_number(a, val_2_double(a) - 1);
}

static inline void number_add(val_t *a, val_t *b, val_t *res) {
    if (val_is_number(b)) {
        val_set_number(res, val_2_double(a) + val_2_double(b));
    } else {
        val_set_nan(res);
    }
}

static inline void number_sub(val_t *a, val_t *b, val_t *res) {
    if (val_is_number(b)) {
        val_set_number(res, val_2_double(a) - val_2_double(b));
    } else {
        val_set_nan(res);
    }
}

static inline void number_mul(val_t *a, val_t *b, val_t *res) {
    if (val_is_number(b)) {
        val_set_number(res, val_2_double(a) * val_2_double(b));
    } else {
        val_set_nan(res);
    }
}

static inline void number_div(val_t *a, val_t *b, val_t *res) {
    if (val_is_number(b)) {
        val_set_number(res, val_2_double(a) / val_2_double(b));
    } else {
        val_set_nan(res);
    }
}

static inline void number_mod(val_t *a, val_t *b, val_t *res) {
    if (val_is_number(b) && 0 != val_2_integer(b)) {
        val_set_number(res, val_2_integer(a) % val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline void number_and(val_t *a, val_t *b, val_t *res) {
    if (val_is_number(b)) {
        val_set_number(res, val_2_integer(a) & val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline void number_or(val_t *a, val_t *b, val_t *res) {
    if (val_is_number(b)) {
        val_set_number(res, val_2_integer(a) | val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline void number_xor(val_t *a, val_t *b, val_t *res) {
    if (val_is_number(b)) {
        val_set_number(res, val_2_integer(a) ^ val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline void number_lshift(val_t *a, val_t *b, val_t *res) {
    if (val_is_number(b)) {
        val_set_number(res, val_2_integer(a) << val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline void number_rshift(val_t *a, val_t *b, val_t *res) {
    if (val_is_number(b)) {
        val_set_number(res, val_2_integer(a) >> val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

val_t number_to_string(env_t *env, int ac, val_t *av);

#endif /* __LANG_NUMBER_INC__ */

