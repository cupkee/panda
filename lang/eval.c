
#include "err.h"
#include "val.h"
#include "lex.h"
#include "ast.h"
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

    if (interp->error) {
        return;
    }

    switch (e->type) {
    case EXPR_ID:       interp_set_error(interp, ERR_NotImplemented); break;
    case EXPR_NAN:      interp_push_nan(interp); break;
    case EXPR_UND:      interp_push_undefined(interp); break;
    case EXPR_NUM:      interp_push_number(interp, ast_expr_num(e)); break;
    case EXPR_TRUE:     interp_push_boolean(interp, 1); break;
    case EXPR_FALSE:    interp_push_boolean(interp, 0); break;
    case EXPR_FUNCPROC: interp_set_error(interp, ERR_NotImplemented); break;
    case EXPR_STRING:   interp_set_error(interp, ERR_NotImplemented); break;

    case EXPR_MINUS:    eval_expr(interp, ast_expr_lft(e)); interp_neg_stack(interp); break;
    case EXPR_NEGATE:   eval_expr(interp, ast_expr_lft(e)); interp_not_stack(interp); break;
    case EXPR_NOT:      eval_expr(interp, ast_expr_lft(e)); interp_logic_not_stack(interp); break;
    case EXPR_ARRAY:    interp_set_error(interp, ERR_NotImplemented); break;
    case EXPR_DICT:     interp_set_error(interp, ERR_NotImplemented); break;

    case EXPR_MUL:      eval_expr(interp, ast_expr_lft(e)); eval_expr(interp, ast_expr_rht(e)); interp_mul_stack(interp); break;
    case EXPR_DIV:      eval_expr(interp, ast_expr_lft(e)); eval_expr(interp, ast_expr_rht(e)); interp_div_stack(interp); break;
    case EXPR_MOD:      eval_expr(interp, ast_expr_lft(e)); eval_expr(interp, ast_expr_rht(e)); interp_mod_stack(interp); break;
    case EXPR_ADD:      eval_expr(interp, ast_expr_lft(e)); eval_expr(interp, ast_expr_rht(e)); interp_add_stack(interp); break;
    case EXPR_SUB:      eval_expr(interp, ast_expr_lft(e)); eval_expr(interp, ast_expr_rht(e)); interp_sub_stack(interp); break;

    case EXPR_AAND:     eval_expr(interp, ast_expr_lft(e)); eval_expr(interp, ast_expr_rht(e)); interp_and_stack(interp); break;
    case EXPR_AOR:      eval_expr(interp, ast_expr_lft(e)); eval_expr(interp, ast_expr_rht(e)); interp_or_stack(interp); break;
    case EXPR_AXOR:     eval_expr(interp, ast_expr_lft(e)); eval_expr(interp, ast_expr_rht(e)); interp_xor_stack(interp); break;

    case EXPR_LSHIFT:   eval_expr(interp, ast_expr_lft(e)); eval_expr(interp, ast_expr_rht(e)); interp_lshift_stack(interp); break;
    case EXPR_RSHIFT:   eval_expr(interp, ast_expr_lft(e)); eval_expr(interp, ast_expr_rht(e)); interp_rshift_stack(interp); break;

    case EXPR_TEQ:      eval_expr(interp, ast_expr_lft(e)); eval_expr(interp, ast_expr_rht(e)); interp_teq_stack(interp); break;
    case EXPR_TNE:      eval_expr(interp, ast_expr_lft(e)); eval_expr(interp, ast_expr_rht(e)); interp_tne_stack(interp); break;
    case EXPR_TGT:      eval_expr(interp, ast_expr_lft(e)); eval_expr(interp, ast_expr_rht(e)); interp_tgt_stack(interp); break;
    case EXPR_TGE:      eval_expr(interp, ast_expr_lft(e)); eval_expr(interp, ast_expr_rht(e)); interp_tge_stack(interp); break;
    case EXPR_TLT:      eval_expr(interp, ast_expr_lft(e)); eval_expr(interp, ast_expr_rht(e)); interp_tlt_stack(interp); break;
    case EXPR_TLE:      eval_expr(interp, ast_expr_lft(e)); eval_expr(interp, ast_expr_rht(e)); interp_tle_stack(interp); break;

    case EXPR_TERNARY:
                        eval_expr(interp, ast_expr_lft(e));
                        if (val_is_true(*interp_stack_pop(interp))) {
                            eval_expr(interp, ast_expr_lft(ast_expr_rht(e)));
                        } else {
                            eval_expr(interp, ast_expr_rht(ast_expr_rht(e)));
                        }
                        break;
    default:            interp_set_error(interp, ERR_NotImplemented); break;
    }
}

static inline void eval_stmt_expr(interp_t *interp, stmt_t *s) {
    eval_expr(interp, s->expr);
}

static int  eval_var_def(interp_t *interp, expr_t *e)
{
    if (e->type == EXPR_ID) {
        return interp_add_symbal(interp, ast_expr_text(e));
    }

    while(e->type == EXPR_ASSIGN) {
        expr_t *lft = ast_expr_lft(e);

        if (lft->type == EXPR_ID && 0 > interp_add_symbal(interp, ast_expr_text(lft))) {
            return -1;
        }

        e = ast_expr_rht(e);
    }

    return 0;
}

static void eval_stmt_var(interp_t *interp, stmt_t *s)
{
    expr_t *e = s->expr;

    while(e) {
        expr_t *next;

        if (e->type == EXPR_COMMA) {
            e    = ast_expr_lft(e);
            next = ast_expr_rht(e);
        } else {
            next = NULL;
        }

        if (0 > eval_var_def(interp, e)) {
            return;
        }

        e = next;
    }

    eval_expr(interp, s->expr);
}

static void eval_stmt(interp_t *interp, stmt_t *s)
{
    switch(s->type) {
    case STMT_EXPR: eval_stmt_expr(interp, s); break;
    case STMT_VAR:  eval_stmt_var(interp, s); break;
    default: interp_set_error(interp, ERR_NotImplemented);
    }
}

int eval_string(interp_t *interp, const char *input, val_t **v)
{
    lexer_t lex_st;
    stmt_t  *stmt;
    intptr_t lex;

    if (!interp || !input || !v) {
        return -1;
    }

    get_line_init(input);
    lex = lex_init(&lex_st, get_line_from_string);

    stmt = parse_stmt(lex, NULL, NULL);
    if (!stmt) {
        return -1;
    }
    eval_stmt(interp, stmt);

    ast_stmt_release(stmt);

    if (interp->error == 0) {
        *v = interp_stack_pop(interp);
    }

    return interp->error;
}

val_t eval_expr_ast(interp_t *interp, expr_t *expr)
{
    return val_mk_undefined();
}

