
#include "err.h"
#include "val.h"
#include "lex.h"
#include "ast.h"
#include "parse.h"
#include "interp.h"
#include "compile.h"
#include "object.h"

#include "exec.h"

/*
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
*/

static void exec_parse_callback(void *u, parse_event_t *e)
{
    int *error = (int *)u;

    if (e->type == PARSE_EOF) {
        //printf("Parse end\n");
    } else
    if (e->type == PARSE_ENTER_BLOCK) {
        printf("enter block\n");
    } else
    if (e->type == PARSE_LEAVE_BLOCK) {
        printf("leave block\n");
    } else
    if (e->type == PARSE_FAIL) {
        *error = -e->error.code;
        //printf("Parse fail: %d\n", e->error.code);
    }
}

static int exec_compile_update(env_t *env, compile_t *cpl)
{

    if (0 != compile_code_relocate(cpl)) {
        return -1;
    }

    // All memory used on parse & compile process can be release here
    heap_reset(env_heap_get_free(env));

    return 0;
}

int exec_env_init(env_t *env, void *mem_ptr, int mem_size, void *heap_ptr, int heap_size, val_t *stack_ptr, int stack_size)
{
    return env_init(env, mem_ptr, mem_size,
                heap_ptr, heap_size, stack_ptr, stack_size,
                EXE_NUMBER_MAX, EXE_STRING_MAX, EXE_NATIVE_MAX, EXE_FUNCTION_MAX,
                EXE_MAIN_CODE_MAX, EXE_FUNC_CODE_MAX, 1);
}

int exec_string(env_t *env, void *mem_ptr, int mem_size, const char *input, val_t **v)
{
    lexer_t lex_st;
    intptr_t lex;
    static val_t undefined = TAG_UNDEFINED;
    int error = 0, exec_cnt = 0, last_type = 0;

    if (!env || !input) {
        return -1;
    }

    lex = lex_init2(&lex_st, mem_ptr, mem_size, input);

    while (!error) {
        stmt_t  *stmt;
        heap_t *heap = env_heap_get_free((env_t*)env);
        parser_t psr;
        compile_t cpl;

        // The free heap can be used for parse and compile process
        parse_init(&psr, lex, heap);
        stmt = parse_stmt(&psr, exec_parse_callback, &error);
        if (!stmt) break;
        last_type = stmt->type;

        compile_init(&cpl, env, heap_free_addr(heap), heap_free_size(heap));
        if (0 == compile_one_stmt(&cpl, stmt) && 0 == exec_compile_update(env, &cpl)) {
            if (0 != interp_run((env_t *)env) && v) {
                error = -env->error;
                //printf("execute fail: %d\n", env->env.error);
            }
        } else {
            error = -cpl.error;
            //printf("compile fail: %d\n", cpl.error);
        }
        compile_deinit(&cpl);
        exec_cnt ++;
    }

    if (exec_cnt && !error && v) {
        if (last_type == STMT_EXPR) {
            *v = env->result;
        } else {
            *v = &undefined;
        }
    }

    return error ? error : exec_cnt;
}

