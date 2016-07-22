
#include "symtbl.h"
#include "env.h"

int env_init(env_t *env, int size)
{
    scope_t *scope = (scope_t *)malloc(sizeof(scope_t) + size *sizeof(val_t));
    intptr_t *vmap = (intptr_t *)malloc(sizeof(intptr_t) * size);
    intptr_t stbl  = symtbl_create();

    if (!scope || !vmap || !stbl) {
        if (scope) free(scope);
        if (vmap) free(vmap);
        if (stbl) symtbl_destroy(stbl);
        return -1;
    }

    scope->super = NULL;
    scope->size  = size;
    scope->num = 0;

    env->scope = scope;
    env->var_map = vmap;
    env->sym_tbl = stbl;

    return 0;
}

int env_deinit(env_t *env)
{
    if (env) {
        if (env->scope) free(env->scope);
        if (env->var_map) free(env->var_map);
        if (env->sym_tbl) symtbl_destroy(env->sym_tbl);
    }
    return 0;
}

intptr_t env_add_symbal(env_t *env, const char *sym)
{
    return symtbl_add(env->sym_tbl, sym);
}

intptr_t env_get_symbal(env_t *env, const char *sym)
{
    return symtbl_get(env->sym_tbl, sym);
}

int env_add_variable(env_t *env, const char *sym)
{
    intptr_t sym_id;
    int      var_id;

    if (!env->var_map) {
        return -1;
    }

    if (0 == (sym_id = env_add_symbal(env, sym))) {
        return -1;
    }

    for (var_id = 0; var_id < env->scope->num; var_id++) {
        if (env->var_map[var_id] == sym_id) {
            return var_id;
        }
    }

    if (env->scope->num < env->scope->size) {
        env->var_map[var_id] = sym_id;

        val_set_undefined(env->scope->variables + var_id);
        env->scope->num++;
        return var_id;
    }

    return -1;
}

int env_set_variable(env_t *env, const char *sym, val_t v)
{
    intptr_t sym_id;
    int      var_id;

    if (!env->var_map) {
        return -1;
    }

    if (0 == (sym_id = env_add_symbal(env, sym))) {
        return -1;
    }

    for (var_id = 0; var_id < env->scope->num; var_id++) {
        if (env->var_map[var_id] == sym_id) {
            env->scope->variables[var_id] = v;
            return var_id;
        }
    }

    if (env->scope->num < env->scope->size) {
        env->var_map[var_id] = sym_id;

        env->scope->variables[var_id] = v;
        env->scope->num++;
        return var_id;
    }

    return -1;
}

int env_get_variable(env_t *env, const char *sym, val_t **p)
{
    intptr_t sym_id;
    int      var_id;

    if (!env->var_map || !p) {
        return -1;
    }

    if (0 == (sym_id = env_get_symbal(env, sym))) {
        return -1;
    }

    for (var_id = 0; var_id < env->scope->num; var_id++) {
        if (env->var_map[var_id] == sym_id) {
            *p = &env->scope->variables[var_id];
            return var_id;
        }
    }

    return -1;
}

