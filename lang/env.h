


#ifndef __LANG_ENV_INC__
#define __LANG_ENV_INC__

#include "config.h"

#include "val.h"
#include "symtbl.h"

typedef struct scope_t {
    struct scope_t *super;
    uint8_t size;
    uint8_t num;
    val_t   *variables;
} scope_t;

typedef struct env_t {
    scope_t *scope;

    int error;

    int fp;
    int ss;
    int sp;
    val_t *sb;
    val_t *result;

    intptr_t sym_tbl;
} env_t;

int env_init(env_t *env, int scope_size, scope_t *super);
int env_deinit(env_t *env);

static inline void env_set_error(env_t *env, int error) {
    if (env) env->error = error;
}

static inline void *env_heap_alloc(env_t *env, int size) {
    return env ? malloc(size) : NULL;
}

static inline void env_heap_free(env_t *env, void *mem) {
    if (env && mem) free(mem);
}

int env_heap_gc(env_t *env, int level);

scope_t *env_scope_create(int size, scope_t *super);
int env_scope_destroy(scope_t *scope);
int env_scope_extend(scope_t *scope, val_t v);
int env_scope_extend_to(scope_t *scope, int size);
static inline void __env_scope_set(scope_t *scope, int id, val_t v) {
    scope->variables[id] = v;
}
static inline void __env_scope_get(scope_t *scope, int id, val_t **v) {
    *v = scope->variables + id;
}
static inline int env_scope_set(scope_t *scope, int id, val_t v) {
    if (scope && id >= 0 && id < scope->num) {
        scope->variables[id] = v;
        return 0;
    }
    return -1;
}
static inline int env_scope_get(scope_t *scope, int id, val_t **v) {
    if (scope && id >= 0 && id < scope->num) {
        *v = scope->variables + id;
        return 0;
    }
    return -1;
}

static inline intptr_t env_add_symbal(env_t *env, const char *sym) {
    return symtbl_add(env->sym_tbl, sym);
}

static inline intptr_t env_get_symbal(env_t *env, const char *sym) {
    return symtbl_get(env->sym_tbl, sym);
}

int env_add_variable(env_t *env, const char *sym);
int env_set_variable(env_t *env, const char *sym, val_t v);
int env_get_variable(env_t *env, const char *sym, val_t **p);

#endif /* __LANG_ENV_INC__ */

