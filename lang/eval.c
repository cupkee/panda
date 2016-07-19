
#include "val.h"
#include "lex.h"
#include "parse.h"
#include "interp.h"

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

static void eval_expr(void *u, expr_t *e)
{
    interp_t *interp = (interp_t *)u;

    switch (e->type) {
    case EXPR_NAN: interp_push_nan(interp); break;
    case EXPR_UND: interp_push_undefined(interp); break;
    case EXPR_NUM: interp_push_number(interp, ast_expr_num(e)); break;
    case EXPR_TRUE: interp_push_boolean(interp, 1); break;
    case EXPR_FALSE: interp_push_boolean(interp, 0); break;
    default: break;
    }
}

int eval_string(interp_t *interp, const char *input, val_t **v)
{
    lexer_t lex_st;
    expr_t  *expr;
    intptr_t lex;

    if (!interp || !input || !v) {
        return -1;
    }

    get_line_init(input);
    lex = lex_init(&lex_st, get_line_from_string);

    expr = parse_expr(lex, NULL, NULL);
    if (!expr) {
        return -1;
    }
    ast_traveral_expr(expr, eval_expr, interp);
    ast_expr_release(expr);

    if (interp->error == 0) {
        *v = interp_stack_pop(interp);
    }

    return interp->error;
}

val_t eval_expr_ast(interp_t *interp, expr_t *expr)
{
    return val_mk_undefined();
}

