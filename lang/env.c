
#include "symtbl.h"
#include "env.h"

static int scope_expand(scope_t *scope, int size)
{
    val_t *buf = (val_t *) malloc(sizeof(val_t) * size);

    if (!buf) return -1;

    memcpy(buf, scope->variables, scope->num * sizeof(val_t));
    free(scope->variables);

    scope->variables = buf;
    scope->size = size;

    return 0;
}

scope_t *env_scope_create(int size, scope_t *super)
{
    scope_t *scope;

    scope = (scope_t *) malloc(sizeof(scope_t));
    if (scope) {
        val_t *buf = (val_t *) malloc(sizeof(val_t) * size);
        if (buf) {
            scope->num = 0;
            scope->size = size;
            scope->super = super;
            scope->variables = buf;
        } else {
            free(scope);
            scope = NULL;
        }
    }
    return scope;
}

int env_scope_destroy(scope_t *scope)
{
    if (scope) {
        free(scope->variables);
        free(scope);
    }
    return 0;
}

int env_scope_extend(scope_t *scope, val_t v)
{
    if (!scope) {
        return -1;
    }

    if (scope->num >= scope->size && 0 != scope_expand(scope, scope->size * 2)) {
        return -1;
    }
    scope->variables[scope->num++] = v;

    return scope->num;
}

int env_scope_extend_to(scope_t *scope, int size)
{
    int i;

    if (!scope) {
        return -1;
    }

    if (size > scope->size && 0 != scope_expand(scope, size)) {
        return -1;
    }
    i = scope->num;
    while(i < size) {
        scope->variables[i++] = val_mk_undefined();
    }
    scope->num = i;

    return 0;
}

