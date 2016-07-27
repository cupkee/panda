


#ifndef __LANG_ENV_INC__
#define __LANG_ENV_INC__

#include "config.h"

#include "val.h"

typedef struct scope_t {
    struct scope_t *super;
    uint8_t size;
    uint8_t num;
    val_t   *variables;
} scope_t;

typedef struct env_t {
    scope_t *scope;

    intptr_t *var_map;
    intptr_t sym_tbl;
} env_t;

int env_init(env_t *env, int scope_size, scope_t *super);
int env_deinit(env_t *env);

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

intptr_t env_add_symbal(env_t *env, const char *sym);
intptr_t env_get_symbal(env_t *env, const char *sym);
int env_add_variable(env_t *env, const char *sym);
int env_set_variable(env_t *env, const char *sym, val_t v);
int env_get_variable(env_t *env, const char *sym, val_t **p);

#endif /* __LANG_ENV_INC__ */

