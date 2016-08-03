
#include "err.h"
#include "val.h"
#include "lex.h"
#include "ast.h"
#include "parse.h"
#include "interp.h"
#include "compile.h"
#include "object.h"

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
    return env_scope_extend_to(&env->env, compile_var_num(&env->cpl));
}

int eval_env_init(eval_env_t *env, val_t *stack_ptr, int stack_size, void *heap_ptr, int heap_size)
{
    if (0 == env_init((env_t *)env, stack_ptr, stack_size, heap_ptr, heap_size)) {
        return compile_init(&env->cpl, env->env.sym_tbl);
    } else {
        return -1;
    }
}

int eval_env_init2(eval_env_t *eval_env, env_t *env)
{
    return -1;
}

int eval_env_deinit(eval_env_t *env)
{

    if (env_deinit((env_t *)env) == 0) {
        return compile_deinit(&env->cpl);
    } else {
        return -1;
    }
}

int eval_env_add_var(eval_env_t *eval_env, const char *name, val_t *value)
{
    env_t    *env = (env_t *)eval_env;
    compile_t *cpl = &eval_env->cpl;
    intptr_t sym_id = symtbl_add(env->sym_tbl, name);
    int      extend = compile_var_add(cpl, sym_id);

    if (extend == 1) {
        return env_scope_extend(env, value);
    } else
    if (extend == 0) {
        return env_scope_set(env, compile_var_get(cpl, sym_id), value);
    } else {
        return -1;
    }
}

int eval_env_get_var(eval_env_t *eval_env, const char *name, val_t **value)
{
    env_t    *env = (env_t *)eval_env;
    compile_t *cpl = &eval_env->cpl;
    intptr_t sym_id = symtbl_get(env->sym_tbl, name);
    int      var_id = compile_var_get(cpl, sym_id);

    return env_scope_get(env, var_id, value);
}

int eval_env_set_var(eval_env_t *eval_env, const char *name, val_t *value)
{
    env_t    *env = (env_t *)eval_env;
    compile_t *cpl = &eval_env->cpl;
    intptr_t sym_id = symtbl_get(env->sym_tbl, name);
    int      var_id = compile_var_get(cpl, sym_id);

    return env_scope_set(env, var_id, value);
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

int eval_string(eval_env_t *env, const char *input, val_t **v)
{
    lexer_t lex_st;
    stmt_t  *stmt;
    module_t mod;
    intptr_t lex;
    uint8_t  lex_memory[8192];
    static val_t undefined = TAG_UNDEFINED;
    int done = 0, last_type = 0;

    if (!env || !input) {
        return -1;
    }

    get_line_init(input);
    lex = lex_init(&lex_st, lex_memory, 8192, get_line_from_string);

    int stmt_cnt = 0;
    while (!done) {
        stmt = parse_stmt(lex, eval_parse_callback, &done);
        if (!stmt) break;
        stmt_cnt++;

        compile_code_clean(&env->cpl);
        if (0 == compile_one_stmt(&env->cpl, stmt, &mod) && 0 == eval_env_adjust(env)) {
            if (0 != interp_run((env_t *)env, &mod) && v) {
                printf("execute fail: %d\n", env->env.error);
                done = -2;
            }
        } else {
            printf("compile fail: %d\n", env->cpl.error);
            done = -1;
        }

        last_type = stmt->type;
    }

    if (!done && v) {
        if (last_type == STMT_EXPR) {
            *v = env->env.result;
        } else {
            *v = &undefined;
        }
    }

    return done;
}

