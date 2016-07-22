


#ifndef __LANG_ENV_INC__
#define __LANG_ENV_INC__

#include "config.h"

#include "val.h"

typedef struct scope_t {
    struct scope_t *super;
    uint8_t size;
    uint8_t num;
    val_t  variables[0];
} scope_t;

typedef struct env_t {
    scope_t *scope;

    intptr_t *var_map;
    intptr_t sym_tbl;
} env_t;

int env_init(env_t *env, int scope_size);
int env_deinit(env_t *env);

intptr_t env_add_symbal(env_t *env, const char *sym);
intptr_t env_get_symbal(env_t *env, const char *sym);
int env_add_variable(env_t *env, const char *sym);
int env_set_variable(env_t *env, const char *sym, val_t v);
int env_get_variable(env_t *env, const char *sym, val_t **p);

#endif /* __LANG_ENV_INC__ */

