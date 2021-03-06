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


#ifndef __LANG_ENV_INC__
#define __LANG_ENV_INC__

#include "def.h"

#include "val.h"
#include "err.h"
#include "heap.h"
#include "executable.h"
#include "scope.h"

#define PANDA_EVENT_GC_START 1
#define PANDA_EVENT_GC_END   2

struct native_t;

typedef struct env_t {
    int16_t error;
    int16_t main_var_num;

    int fp;
    int ss;
    int sp;
    val_t *sb;

    scope_t *scope;                     // Root scope

    heap_t *heap;                       // inused heap ptr: top or bot
    heap_t heap_top;
    heap_t heap_bot;

    uint16_t ref_num;                   // External reference number
    uint16_t native_num;                // Native function number

    uint16_t symbal_tbl_size;           // Symbal hash table size
    uint16_t symbal_tbl_hold;           // Symbal saved counter
    uint16_t symbal_buf_end;            // Symbal buffer size
    uint16_t symbal_buf_used;           // Symbal buffer used

    intptr_t *symbal_tbl;
    char     *symbal_buf;
    val_t    *ref_ent;                  // External reference entry
    const struct native_t *native_ent;  // Native function entry

    intptr_t *main_var_map;

    void (*callback)(struct env_t *, int);

    executable_t exe;
} env_t;

typedef struct native_t {
    const char *name;
    val_t (*fn)(env_t *, int ac, val_t *av);
} native_t;


int env_exe_memery_distribute(int mem_size, int *num_max, int *str_max, int *fn_max, int *code_max);
int env_init(env_t *env, void * mem_ptr, int mem_size,
             void *heap_ptr, int heap_size, val_t *stack_ptr, int stack_size,
             int number_max, int string_max, int func_max, int code_max,
             int interactive);

int env_deinit(env_t *env);
int env_reference_set(env_t *env, val_t *ent, int num);
int env_callback_set(env_t *env, void (*cb)(env_t *, int));
int env_native_set(env_t *env, const native_t *ent, int num);

void env_heap_gc(env_t *env, int level);

scope_t *env_scope_create(env_t *env, scope_t *super, uint8_t *entry, int ac, val_t *av);
int env_scope_get(env_t *env, int id, val_t **v);
int env_scope_set(env_t *env, int id, val_t *v);

static inline void *env_heap_alloc(env_t *env, int size) {
    void *ptr = heap_alloc(env->heap, size);

    if (!ptr) {
        env_heap_gc(env, size);
        return heap_alloc(env->heap, size);
    }

    return ptr;
}

intptr_t env_symbal_insert(env_t *env, const char *symbal, int alloc);
intptr_t env_symbal_get(env_t *env, const char *name);
static inline
intptr_t env_symbal_add(env_t *env, const char *name) {
    return env_symbal_insert(env, name, 1);
}
static inline
intptr_t env_symbal_add_static(env_t *env, const char *name) {
    return env_symbal_insert(env, name, 0);
}
void env_symbal_foreach(env_t *env, int (*cb)(const char *, void *), void *param);

int env_string_find_add(env_t *env, intptr_t s);
int env_number_find_add(env_t *env, double);

int env_native_find(env_t *env, intptr_t sym_id);

const uint8_t *env_frame_setup(env_t *env, const uint8_t *pc, val_t *fv, int ac, val_t *av);
const uint8_t *env_func_entry_setup(env_t *env, uint8_t *entry, int ac, val_t *av);
void env_frame_restore(env_t *env, const uint8_t **pc, scope_t **scope);
void env_native_call(env_t *env, val_t *fv, int ac, val_t *av);

const uint8_t *env_main_entry_setup(env_t *env, int ac, val_t *av);
const uint8_t *env_entry_setup(env_t *env, uint8_t *entry, int ac, val_t *av);

static inline
void env_set_error(env_t *env, int error) {
    if (env) env->error = error;
}

static inline val_t *env_stack_peek(env_t *env) {
    return env->sb + env->sp;
}

static inline val_t *env_stack_pop(env_t *env) {
    return env->sb + env->sp++;
}

static inline val_t *env_stack_push(env_t *env) {
    return env->sb + (--env->sp);
}

static inline val_t *env_stack_release(env_t *env, int n) {
    env->sp += n;
    return env->sb + env->sp;
}

static inline void env_push_call_argument(env_t *env, val_t *v) {
    *env_stack_push(env) = *v;
}

static inline void env_push_call_function(env_t *env, val_t *v) {
    *env_stack_push(env) = *v;
}

static inline val_t *env_get_var(env_t *env, uint8_t id, uint8_t generation) {
    scope_t *scope = env->scope;

    while(scope && generation--) {
        scope = scope->super;
    }

    if (scope && id < scope->num) {
        return scope->var_buf + id;
    } else {
        return NULL;
    }
}

static inline void env_push_var(env_t *env, uint8_t id, uint8_t generation) {
    val_t *v = env_get_var(env, id, generation);

    if (v) {
        *(env_stack_push(env)) = *v;
    } else {
        env->error = ERR_SysError;
    }
}

static inline void env_push_ref(env_t *env, uint8_t id, uint8_t generation) {
    val_set_reference(env_stack_push(env), id, generation);
}

static inline void env_push_undefined(env_t *env) {
    val_set_undefined(env_stack_push(env));
}

static inline void env_push_nan(env_t *env) {
    val_set_nan(env_stack_push(env));
}

static inline void env_push_zero(env_t *env) {
    val_set_number(env_stack_push(env), 0);
}

static inline void env_push_number(env_t *env, int id) {
    if (env->exe.number_num > id) {
        val_set_number(env_stack_push(env), env->exe.number_map[id]);
    } else {
        env_set_error(env, ERR_SysError);
    }
}

static inline void env_push_string(env_t *env, int id) {
    if (env->exe.string_num > id) {
        val_set_foreign_string(env_stack_push(env), env->exe.string_map[id]);
    } else {
        env_set_error(env, ERR_SysError);
    }
}

static inline void env_push_boolean(env_t *env, int b) {
    val_set_boolean(env_stack_push(env), b);
}

static inline void env_push_script(env_t *env, intptr_t p) {
    val_set_script(env_stack_push(env), p);
}

static inline void env_push_native(env_t *env, int id) {
    if (env->native_num > id) {
        val_set_native(env_stack_push(env), (intptr_t)(env->native_ent[id].fn));
    } else {
        env_set_error(env, ERR_SysError);
    }
}

static inline
heap_t *env_heap_get_free(env_t *env) {
    return env->heap == &env->heap_top ? &env->heap_bot : &env->heap_top;
}

static inline
int env_is_heap_memory(env_t *env, void *p) {
    return heap_is_owned(env->heap, p);
}

static inline
uint8_t *env_get_main_entry(env_t *env) {
    return env->exe.func_map[0];
}

static inline
int env_is_interactive(env_t *env) {
    return env->main_var_map != NULL;
}

#endif /* __LANG_ENV_INC__ */

