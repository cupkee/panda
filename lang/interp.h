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

#ifndef __LANG_INTERP_INC__
#define __LANG_INTERP_INC__

#include "def.h"

#include "val.h"
#include "env.h"
#include "executable.h"

int interp_env_init_interactive(env_t *env, void *mem_ptr, int mem_size, void *heap_ptr, int heap_size, val_t *stack_ptr, int stack_size);
int interp_env_init_interpreter(env_t *env, void *mem_ptr, int mem_size, void *heap_ptr, int heap_size, val_t *stack_ptr, int stack_size);
int interp_env_init_image(env_t *env, void *mem_ptr, int mem_size, void *heap_ptr, int heap_size, val_t *stack_ptr, int stack_size, image_info_t *image);

int interp_execute_interactive(env_t *env, const char *input, char *(*input_more)(void), val_t **v);
int interp_execute_string(env_t *env, const char *input, val_t **result);
int interp_execute_image(env_t *env, val_t **result);

val_t interp_execute_call(env_t *env, int ac);


int interp_execute_stmts(env_t *env, const char *input, val_t **v);

#endif /* __LANG_INTERP_INC__ */

