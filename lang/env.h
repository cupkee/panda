


#ifndef __LANG_ENV_INC__
#define __LANG_ENV_INC__

#include "config.h"

#include "val.h"
#include "heap.h"
#include "symtbl.h"

typedef struct scope_t {
    uint8_t type;
    uint8_t size;
    uint8_t num;
    val_t   *var_buf;
    struct scope_t *super;
} scope_t;

typedef struct env_t {
    scope_t *scope;

    int error;

    int fp;
    int ss;
    int sp;
    val_t *sb;
    val_t *result;

    heap_t *heap;
    heap_t heap_top;
    heap_t heap_bot;

    intptr_t sym_tbl;
} env_t;

int env_init(env_t *env, val_t *stack_ptr, int stack_size, void *heap_ptr, int heap_size);
int env_deinit(env_t *env);

static inline void env_set_error(env_t *env, int error) {
    if (env) env->error = error;
}

void *env_heap_alloc(env_t *env, int size);
void env_heap_gc(env_t *env, int level);

int env_scope_create(env_t *env, scope_t *super, int vc, int ac, val_t *av);
int env_scope_extend(env_t *env, val_t *v);
int env_scope_extend_to(env_t *env, int size);
int env_scope_get(env_t *env, int id, val_t **v);
int env_scope_set(env_t *env, int id, val_t *v);

intptr_t env_symbal_add(env_t *env, const char *name);
intptr_t env_symbal_get(env_t *env, const char *name);

int  env_frame_setup(env_t *env, uint8_t *pc, scope_t *super, int vc, int ac, val_t *av);
void env_frame_restore(env_t *env, uint8_t **pc, scope_t **scope);

#endif /* __LANG_ENV_INC__ */

