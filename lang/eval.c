
#include "err.h"
#include "val.h"
#include "lex.h"
#include "ast.h"
#include "parse.h"
#include "interp.h"
#include "compile.h"

#include "eval.h"

static const char *string_ptr;
static int string_pos;
static int string_end;

static void get_line_init(const char *string)
{
    string_ptr = string;
    string_end = strlen(string);
    string_pos = 0;
}

static int get_line_from_string(void *buf, int size)
{
    int lft = string_end - string_pos;
    int max = lft < size ? lft : size;
    const char *bgn = string_ptr + string_pos;

    if (max > 0)
        memcpy(buf, bgn, max);
    string_pos += max;

    return max;
}

static int eval_env_adjust(eval_env_t *env)
{
    return env_scope_extend_to(env->scope, compile_var_num(&env->cpl));
}

int eval_env_init(eval_env_t *env)
{
    int size = 16;
    scope_t *scope = env_scope_create(size, NULL);
    intptr_t sym_tbl  = symtbl_create();

    if (!scope || !sym_tbl) {
        if (scope) env_scope_destroy(scope);
        if (sym_tbl) symtbl_destroy(sym_tbl);
        return -1;
    }

    env->scope = scope;
    env->sym_tbl = sym_tbl;

    return compile_init(&env->cpl, sym_tbl);
}

int eval_env_deinit(eval_env_t *env)
{
    if (!env) {
        return -1;
    }

    if (env->scope) env_scope_destroy(env->scope);
    if (env->sym_tbl) symtbl_destroy(env->sym_tbl);

    compile_deinit(&env->cpl);

    return 0;
}

int eval_env_add_var(eval_env_t *env, const char *name, val_t value)
{
    intptr_t sym_id = symtbl_add(env->sym_tbl, name);
    int      extend = compile_var_add(&env->cpl, sym_id);

    if (extend == 1) {
        return env_scope_extend(env->scope, value);
    } else
    if (extend == 0) {
        return env_scope_set(env->scope, compile_var_get(&env->cpl, sym_id), value);
    } else {
        return -1;
    }
}

int eval_env_get_var(eval_env_t *env, const char *name, val_t **value)
{
    intptr_t sym_id = symtbl_get(env->sym_tbl, name);
    int      var_id = compile_var_get(&env->cpl, sym_id);

    return env_scope_get(env->scope, var_id, value);
}

int eval_env_set_var(eval_env_t *env, const char *name, val_t value)
{
    intptr_t sym_id = symtbl_get(env->sym_tbl, name);
    int      var_id = compile_var_get(&env->cpl, sym_id);

    return env_scope_set(env->scope, var_id, value);
}

int eval_env_add_native(eval_env_t *env, const char *name, function_native_t native)
{
    return compile_native_add(&env->cpl, name, native);
}

static void eval_parse_callback(void *u, parse_event_t *e)
{
    int *done = (int *)u;

    if (e->type == PARSE_EOF) {
        //printf("Parse end\n");
    } else
    if (e->type == PARSE_FAIL) {
        *done = -3;
        printf("Parse fail: %d\n", e->error.code);
    }
}

int eval_string(interp_t *interp, eval_env_t *env, const char *input, val_t **v)
{
    lexer_t lex_st;
    stmt_t  *stmt;
    module_t mod;
    intptr_t lex;
    static val_t undefined = TAG_UNDEFINED;
    int done = 0, last_type = 0;

    if (!interp || !env || !input) {
        return -1;
    }

    get_line_init(input);
    lex = lex_init(&lex_st, get_line_from_string);

    int stmt_cnt = 0;
    while (!done) {
        stmt = parse_stmt(lex, eval_parse_callback, &done);
        if (!stmt) break;
        stmt_cnt++;

        compile_code_clean(&env->cpl);
        if (0 == compile_one_stmt(&env->cpl, stmt, &mod) && 0 == eval_env_adjust(env)) {
            if (0 != interp_run(interp, (env_t *)env, &mod) && v) {
                printf("execute fail: %d\n", interp->error);
                done = -2;
            }
        } else {
            printf("compile fail: %d\n", env->cpl.error);
            done = -1;
        }

        last_type = stmt->type;
        ast_stmt_release(stmt);
    }

    if (!done && v) {
        if (last_type == STMT_EXPR) {
            *v = interp->result;
        } else {
            *v = &undefined;
        }
    }

    return done;
}

