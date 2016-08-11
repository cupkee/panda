
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

static void eval_parse_callback(void *u, parse_event_t *e)
{
    int *error = (int *)u;

    if (e->type == PARSE_EOF) {
        //printf("Parse end\n");
    } else
    if (e->type == PARSE_FAIL) {
        *error = -e->error.code;
        //printf("Parse fail: %d\n", e->error.code);
    }
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

    // save main var map info
    num = compile_vmap_copy(cpl, env->main_var_map, EVAL_MAIN_VAR_MAX);
    if (num < 0) {
        return -1;
    }
    env->main_var_num = num;

    // All memory used on parse & compile process can be release here
    heap_reset(env_heap_get_free(&env->env));

    // Check the main scope space is enought?
    if (compile_var_num(cpl) > EVAL_MAIN_VAR_MAX) {
        env_set_error(&env->env, ERR_ResourceOutLimit);
        return -1;
    }

    return 0;
}

static int eval_compile_deinit(compile_t *cpl)
{
    if (cpl) {
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

int eval_string(eval_env_t *env, void *mem_ptr, int mem_size, const char *input, val_t **v)
{
    lexer_t lex_st;
    intptr_t lex;
    static val_t undefined = TAG_UNDEFINED;
    int error = 0, last_type = 0;

    if (!env || !input) {
        return -1;
    }

    get_line_init(input);
    if (0) {
        lex = lex_init(&lex_st, mem_ptr, mem_size, get_line_from_string);
    }
    lex = lex_init2(&lex_st, mem_ptr, mem_size, input);

    while (!error) {
        stmt_t  *stmt;
        heap_t *heap = env_heap_get_free((env_t*)env);
        parser_t psr;
        compile_t cpl;

        /*
        * free heap used for parse and compile process
        */
        parse_init(&psr, lex, heap);
        stmt = parse_stmt(&psr, eval_parse_callback, &error);
        if (!stmt) break;

        last_type = stmt->type;
        eval_compile_init(&cpl, env, heap_free_addr(heap), heap_free_size(heap));
        if (0 == compile_one_stmt(&cpl, stmt) && 0 == eval_compile_update(env, &cpl)) {
            if (0 != interp_run((env_t *)env) && v) {
                error = -env->env.error;
                //printf("execute fail: %d\n", env->env.error);
            }
        } else {
            error = -cpl.error;
            //printf("compile fail: %d\n", cpl.error);
        }
        eval_compile_deinit(&cpl);
    }

    if (!error && v) {
        if (last_type == STMT_EXPR) {
            *v = env->env.result;
        } else {
            *v = &undefined;
        }
    } else {
        printf("FAIL: %d\n", error);
    }

    return error;
}

