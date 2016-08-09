
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
    int i;

    if (0 != compile_init(cpl, &env->env, heap_addr, heap_size)) {
        return -1;
    }

    // set main function compile info history
    for (i = 0; i < env->main_var_num; i++) {
        compile_var_add(cpl, env->main_var_map[i]);
    }

    return 0;
}

static int eval_compile_update(eval_env_t *env, compile_t *cpl)
{
    int num;

    compile_clear_main(cpl);
    if (0 != compile_code_relocate(cpl)) {
        return -1;
    }

    // save main var map info & update scope
    num = compile_vmap_copy(cpl, env->main_var_map, EVAL_MAIN_VAR_MAX);
    if (num < 0) {
        return -1;
    }
    env->main_var_num = num;

    // all memory used on parse & compile process can be free here
    heap_reset(env_heap_get_free(&env->env));

    return env_scope_extend_to(&env->env, num);
}

static int eval_compile_deinit(compile_t *cpl)
{
    if (cpl) {
        cpl->func_buf[0].var_map = NULL;
        return compile_deinit(cpl);
    }
    return -1;
}

int eval_env_init(eval_env_t *env, void *mem_ptr, int mem_size, void *heap_ptr, int heap_size, val_t *stack_ptr, int stack_size)
{
    if (!env || !mem_ptr) {
        return -1;
    }

    if (0 == env_init((env_t *)env, mem_ptr, mem_size,
                heap_ptr, heap_size, stack_ptr, stack_size,
                EXE_NUMBER_MAX, EXE_STRING_MAX, EXE_NATIVE_MAX, EXE_FUNCTION_MAX,
                EXE_MAIN_CODE_MAX, EXE_FUNC_CODE_MAX)) {
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
    intptr_t sym_id = env_symbal_add(env, name);
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
    intptr_t sym_id = env_symbal_get(env, name);
    int      var_id = eval_main_var_get(eval_env, sym_id);

    return env_scope_get(env, var_id, value);
}

int eval_env_set_var(eval_env_t *eval_env, const char *name, val_t *value)
{
    env_t    *env = (env_t *)eval_env;
    intptr_t sym_id = env_symbal_get(env, name);
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
        *done = -e->error.code;
        //printf("Parse fail: %d\n", e->error.code);
    }
}

int eval_string(eval_env_t *env, void *mem_ptr, int mem_size, const char *input, val_t **v)
{
    lexer_t lex_st;
    intptr_t lex;
    static val_t undefined = TAG_UNDEFINED;
    int done = 0, last_type = 0;

    if (!env || !input) {
        return -1;
    }

    get_line_init(input);
    lex = lex_init(&lex_st, mem_ptr, mem_size, get_line_from_string);

    while (!done) {
        stmt_t  *stmt;
        heap_t *heap = env_heap_get_free((env_t*)env);
        parser_t psr;
        compile_t cpl;

        /*
        * free heap used for parse and compile process
        */
        parse_init(&psr, lex, heap);
        stmt = parse_stmt(&psr, eval_parse_callback, &done);
        if (!stmt) break;
        //printf("parse memory: %d\n", heap->free);

        eval_compile_init(&cpl, env, heap_free_addr(heap), heap_free_size(heap));
        if (0 == compile_one_stmt(&cpl, stmt) && 0 == eval_compile_update(env, &cpl)) {
            if (0 != interp_run((env_t *)env) && v) {
                done = -env->env.error;
                //printf("execute fail: %d\n", env->env.error);
            }
        } else {
            done = -cpl.error;
            //printf("compile fail: %d\n", cpl.error);
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
    } else {
        printf("FAIL: %d\n", done);
    }

    return done;
}

