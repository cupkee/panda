

#ifndef __LANG_ENV_INC__
#define __LANG_ENV_INC__

#include "config.h"

#include "val.h"
#include "heap.h"
#include "executable.h"

#define MAGIC_SCOPE     (MAGIC_BASE + 1)

typedef struct scope_t {
    uint8_t magic;
    uint8_t size;
    uint8_t num;
    val_t   *var_buf;
    struct scope_t *super;
} scope_t;

typedef struct env_t {
    int16_t error;
    int16_t main_var_num;

    int fp;
    int ss;
    int sp;

    val_t *sb;

    heap_t *heap;

    heap_t heap_top;
    heap_t heap_bot;

    scope_t *scope;
    val_t *result;

    uint16_t symbal_tbl_size;
    uint16_t symbal_tbl_hold;
    uint16_t symbal_buf_end;
    uint16_t symbal_buf_used;
    intptr_t *symbal_tbl;
    char     *symbal_buf;

    intptr_t *main_var_map;

    executable_t exe;
} env_t;

int env_init(env_t *env, void * mem_ptr, int mem_size,
             void *heap_ptr, int heap_size, val_t *stack_ptr, int stack_size,
             int number_max, int string_max, int native_max, int func_max,
             int main_code_max, int func_code_max,
             int interactive);

int env_deinit(env_t *env);

void *env_heap_alloc(env_t *env, int size);
void env_heap_gc(env_t *env, int level);

scope_t *env_scope_create(env_t *env, scope_t *super, int vc, int ac, val_t *av);
int env_scope_extend(env_t *env, val_t *v);
int env_scope_extend_to(env_t *env, int size);
int env_scope_get(env_t *env, int id, val_t **v);
int env_scope_set(env_t *env, int id, val_t *v);

intptr_t env_symbal_add(env_t *env, const char *name);
intptr_t env_symbal_get(env_t *env, const char *name);

int env_string_find_add(env_t *env, intptr_t s);
int env_number_find_add(env_t *env, double);

int env_native_add(env_t *env, const char *name, val_t (*fn)(env_t *, int ac, val_t *av));
int env_native_find(env_t *env, intptr_t sym_id);

int  env_frame_setup(env_t *env, uint8_t *pc, scope_t *super, int vc, int ac, val_t *av);
void env_frame_restore(env_t *env, uint8_t **pc, scope_t **scope);
int  env_native_frame_setup(env_t *env, int vc);
void env_native_return(env_t *env, val_t res);

int env_entry_setup(env_t *env, uint8_t *entry, int ac, val_t *av, uint8_t **pc);

static inline
void env_set_error(env_t *env, int error) {
    if (env) env->error = error;
}

static inline
heap_t *env_heap_get_free(env_t *env) {
    return env->heap == &env->heap_top ? &env->heap_bot : &env->heap_top;
}

static inline
val_t *env_stack_push(env_t *env) {
    return env->sb + (--env->sp);
}

static inline
val_t *env_get_var_ref(env_t *env, int index) {
    if (index < 256) {
        return env->scope->var_buf + index;
    } else {
        int loop = index >> 8;

        index &= 0xff;
        scope_t *scope = env->scope;
        while(loop) {
            loop--;
            scope = scope->super;
        }
        return env->scope->var_buf + index;
    }
}

static inline
void env_return_noframe(env_t *env, int ac, val_t res) {
    env->sp += ac + 1; // release arguments & fobj in stack
    *env_stack_push(env) = res;
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

