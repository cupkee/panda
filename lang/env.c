
#include "symtbl.h"
#include "env.h"
#include "object.h"

#define FRAME_SIZE (sizeof(frame_t) / sizeof(val_t))

typedef struct frame_t {
    int fp;
    int sp;
    intptr_t pc;
    intptr_t scope;
} frame_t;

static scope_t *scope_create(int size, scope_t *super)
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

static int scope_destroy(scope_t *scope)
{
    if (scope) {
        free(scope->variables);
        free(scope);
    }
    return 0;
}

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

static int scope_extend(scope_t *scope, val_t *v)
{
    if (!scope) {
        return -1;
    }

    if (scope->num >= scope->size && 0 != scope_expand(scope, scope->size * 2)) {
        return -1;
    }
    scope->variables[scope->num++] = *v;

    return scope->num;
}

static int scope_extend_to(scope_t *scope, int size)
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


int env_init(env_t *env, val_t *stack_ptr, int stack_size)
{
    scope_t *scope = scope_create(16, NULL);
    intptr_t sym_tbl  = symtbl_create();

    if (!scope || !sym_tbl) {
        if (scope)   scope_destroy(scope);
        if (sym_tbl) symtbl_destroy(sym_tbl);
        return -1;
    }

    env->error = 0;
    env->scope = scope;
    env->sym_tbl = sym_tbl;

    env->sb = stack_ptr;
    env->ss = stack_size;
    env->sp = stack_size;
    env->fp = stack_size;

    env->result = NULL;

    if (0 != objects_env_init((env_t *)env)) {
        if (scope)   scope_destroy(scope);
        if (sym_tbl) symtbl_destroy(sym_tbl);
        return -1;
    } else {
        return 0;
    }
}

int env_deinit(env_t *env)
{
    if (!env) {
        return -1;
    }

    if (env->scope) scope_destroy(env->scope);
    if (env->sym_tbl) symtbl_destroy(env->sym_tbl);

    return 0;
}

int env_scope_create(env_t *env, scope_t *super, int vc, int ac, val_t *av)
{
    if (env) {
        scope_t *scope = scope_create(vc, super);
        int i;

        if (!scope) {
            env_set_error(env, ERR_SysError);
            return -1;
        }

        for (i = 0; i < ac; i++) {
            scope->variables[i] = av[i];
        }
        for (; i < vc; i++) {
            scope->variables[i] = val_mk_undefined();
        }

        env->scope = scope;
        return 0;
    }

    return -1;
}

int env_scope_extend(env_t *env, val_t *v)
{
    if (env) {
        return scope_extend(env->scope, v);
    } else {
        return -1;
    }
}

int env_scope_extend_to(env_t *env, int size)
{
    if (env) {
        return scope_extend_to(env->scope, size);
    } else {
        return -1;
    }
}

int env_scope_set(env_t *env, int id, val_t *v) {
    if (env && env->scope && id >= 0 && id < env->scope->num) {
        env->scope->variables[id] = *v;
        return 0;
    }
    return -1;
}

int env_scope_get(env_t *env, int id, val_t **v) {
    if (env && env->scope && id >= 0 && id < env->scope->num) {
        *v = env->scope->variables + id;
        return 0;
    }
    return -1;
}

intptr_t env_symbal_add(env_t *env, const char *name) {
    if (env) {
        return symtbl_add(env->sym_tbl, name);
    } else {
        return 0;
    }
}

intptr_t env_symbal_get(env_t *env, const char *name) {
    if (env) {
        return symtbl_get(env->sym_tbl, name);
    } else {
        return 0;
    }
}

int env_frame_setup(env_t *env, uint8_t *pc, scope_t *super, int vc, int ac, val_t *av)
{
    int fp;
    scope_t *scope = env->scope;
    frame_t *frame;

    if (env->sp < FRAME_SIZE) {
        env->error = ERR_SysError;
        return -1;
    }

    if (env_scope_create(env, super, vc, ac, av) != 0) {
        return -1;
    }

    fp = env->sp - FRAME_SIZE;
    frame = (frame_t *)(env->sb + fp);

    //printf ("############  resave sp: %d, fp: %d\n", env->sp, env->fp);
    frame->fp = env->fp;
    frame->sp = env->sp;
    frame->pc = (intptr_t) pc;
    frame->scope = (intptr_t) scope;

    env->fp = fp;
    env->sp = fp;

    return 0;
}

void env_frame_restore(env_t *env, uint8_t **pc, scope_t **scope)
{
    if (env->fp != env->ss) {
        frame_t *frame = (frame_t *)(env->sb + env->fp);

        //printf ("$$$$$$$$$$$$$$ restore sp: %d, fp: %d\n", frame->sp, frame->fp);
        env->sp = frame->sp;
        env->fp = frame->fp;
        *pc = (uint8_t *) frame->pc;
        *scope = (scope_t *) frame->scope;
    } else {
        *pc = NULL;
        *scope = NULL;
    }
}

