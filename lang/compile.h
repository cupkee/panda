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


#ifndef __LANG_COMPILE_INC__
#define __LANG_COMPILE_INC__

#include "def.h"

#include "ast.h"
#include "env.h"
#include "interp.h"

typedef struct compile_func_t {
    int16_t owner;
    uint16_t stack_space;
    uint16_t stack_high;

    uint8_t closure;
    uint8_t var_max;
    uint8_t var_num;
    uint8_t arg_num;

    uint16_t code_max;
    uint16_t code_num;
    uint8_t  *code_buf;
    intptr_t *var_map;
} compile_func_t;

typedef struct compile_t {
    int     error;     //

    int16_t bgn_pos;   // for loop continue
    int16_t skip_pos;  // for loop break

    uint16_t func_size;
    uint16_t func_num;
    uint16_t func_cur;
    uint16_t func_offset;

    env_t  *env;
    heap_t  heap;

    compile_func_t *func_buf;
} compile_t;

int compile_init(compile_t *cpl, env_t *env, void *heap_ptr, int heap_size);
int compile_deinit(compile_t *cpl);

/*
int compile_arg_add(compile_t *cpl, intptr_t sym_id);
int compile_var_add(compile_t *cpl, intptr_t sym_id);
int compile_var_get(compile_t *cpl, intptr_t sym_id);
*/

int compile_stmt(compile_t *cpl, stmt_t *stmt);
int compile_one_stmt(compile_t *cpl, stmt_t *stmt);
int compile_multi_stmt(compile_t *cpl, stmt_t *stmt);

int compile_update(compile_t *cpl);

int compile_env_init(env_t *env, void *mem_ptr, int mem_size);
int compile_exe(env_t *env, const char *input, void *mem_ptr, int mem_size);


#endif /* __LANG_COMPILE_INC__ */
