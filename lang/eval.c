
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

static int eval_main_var_get(eval_env_t *env, intptr_t sym)
{
    int i;

    for (i = 0; i < env->main_var_num; i++) {
        if (env->main_var_map[i] == sym) {
            return i;
        }
    }

    return -1;
}

static int eval_main_var_add(eval_env_t *env, intptr_t sym)
{
    int i;

    for (i = 0; i < env->main_var_num; i++) {
        if (env->main_var_map[i] == sym) {
            return 0;
        }
    }

    if (i < EVAL_MAIN_VAR_MAX) {
        env->main_var_map[env->main_var_num++] = sym;
        return 1;
    }

    return -1;
}

static int eval_compile_init(compile_t *cpl, eval_env_t *env, void *heap_addr, int heap_size)
{
    if (0 != compile_init(cpl, &env->env, heap_addr, heap_size)) {
        return -1;
    }

    // set main function compile info history
    cpl->func_buf[0].var_map = env->main_var_map;
    cpl->func_buf[0].var_num = env->main_var_num;
    cpl->func_buf[0].var_max = EVAL_MAIN_VAR_MAX;

    // set function offset for compile
//    cpl->func_offset = env->env.func_num;

    return 0;
}

static int eval_compile_update(eval_env_t *env, compile_t *cpl)
{
    if (0 != compile_code_relocate(cpl)) {
        return -1;
    }

    // update main var scope
    env->main_var_num = compile_var_num(cpl);
    return env_scope_extend_to(&env->env, compile_var_num(cpl));
}

static int eval_compile_deinit(compile_t *cpl)
{
    if (cpl) {
        cpl->func_buf[0].var_map = NULL;
        return compile_deinit(cpl);
    }
    return -1;
}

int eval_env_init_mini(eval_env_t *env, void *mem_ptr, int mem_size, void *heap_ptr, int heap_size, val_t *stack_ptr, int stack_size)
{
    if (!env || !mem_ptr) {
        return -1;
    }

    if (0 == env_init((env_t *)env, mem_ptr, mem_size, heap_ptr, heap_size, stack_ptr, stack_size,
                64, 64, 32, 16, 1024, 1024)) {
        env->main_var_num = 0;

        return 0;
    } else {
        return -1;
    }
}

int eval_env_deinit(eval_env_t *env)
{

    if (env_deinit((env_t *)env) == 0) {
        return 0;
    } else {
        return -1;
    }
}

int eval_env_add_var(eval_env_t *eval_env, const char *name, val_t *value)
{
    env_t    *env = (env_t *)eval_env;
    intptr_t sym_id = symtbl_add(env->sym_tbl, name);
    int      extend = eval_main_var_add(eval_env, sym_id);

    if (extend == 1) {
        return env_scope_extend(env, value);
    } else
    if (extend == 0) {
        return env_scope_set(env, eval_main_var_get(eval_env, sym_id), value);
    } else {
        return -1;
    }
}

int eval_env_get_var(eval_env_t *eval_env, const char *name, val_t **value)
{
    env_t    *env = (env_t *)eval_env;
    intptr_t sym_id = symtbl_get(env->sym_tbl, name);
    int      var_id = eval_main_var_get(eval_env, sym_id);

    return env_scope_get(env, var_id, value);
}

int eval_env_set_var(eval_env_t *eval_env, const char *name, val_t *value)
{
    env_t    *env = (env_t *)eval_env;
    intptr_t sym_id = symtbl_get(env->sym_tbl, name);
    int      var_id = eval_main_var_get(eval_env, sym_id);

    return env_scope_set(env, var_id, value);
}

int eval_env_add_native(eval_env_t *env, const char *name, function_native_t native)
{
    return env_native_add(&env->env, name, native);
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
        heap_t *heap = env_heap_get_free((env_t*)env);
        parser_t psr;
        compile_t cpl;

        /*
        * free heap used for parse and compile process
        */
        parse_init(&psr, lex, heap);
        stmt = parse_stmt(&psr, eval_parse_callback, &done);
        if (!stmt) break;
        stmt_cnt++;

        //printf("parse memory: %d\n", lex_heap_used(lex));

        //compile_code_clean(&env->cpl);
        eval_compile_init(&cpl, env, heap_free_addr(heap), heap_free_size(heap));
        if (0 == compile_one_stmt(&cpl, stmt, &mod) && 0 == eval_compile_update(env, &cpl)) {
            if (0 != interp_run((env_t *)env, &mod) && v) {
                printf("execute fail: %d\n", env->env.error);
                done = -2;
            }
        } else {
            printf("compile fail: %d\n", cpl.error);
            done = -1;
        }
        eval_compile_deinit(&cpl);

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

