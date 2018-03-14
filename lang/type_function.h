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


#ifndef __LANG_FUNCTION_INC__
#define __LANG_FUNCTION_INC__

#include "def.h"

#include "val.h"
#include "env.h"
#include "interp.h"

#define MAGIC_FUNCTION  (MAGIC_BASE + 5)

typedef struct function_t {
    uint8_t magic;
    uint8_t age;
    uint8_t reserved[2];
    uint8_t *entry;
    scope_t *super;
} function_t;

typedef val_t (*function_native_t) (env_t *env, int ac, val_t *av);

intptr_t  function_create(env_t *env, uint8_t *code);
int function_destroy(intptr_t func);

static inline
int function_mem_space(function_t *f) {
    (void) f;
    return SIZE_ALIGN(sizeof(function_t));
}

static inline
uint8_t function_varc(function_t *fn) {
    return executable_func_get_var_cnt(fn->entry);
}

static inline
uint8_t function_size(function_t *fn) {
    return executable_func_get_code_size(fn->entry);
}

static inline
uint8_t function_argc(function_t *fn) {
    return executable_func_get_arg_cnt(fn->entry);
}

static inline
uint16_t function_stack_high(function_t *fn) {
    return executable_func_get_stack_high(fn->entry);
}

static inline
int function_is_closure(function_t *fn) {
    return executable_func_is_closure(fn->entry);
}

static inline
uint8_t *function_code(function_t *fn) {
    return fn->entry + FUNC_HEAD_SIZE;
}

//extern const val_metadata_t metadata_boolean;

extern const val_metadata_t metadata_function;
extern const val_metadata_t metadata_function_native;

#endif /* __LANG_FUNCTION_INC__ */

