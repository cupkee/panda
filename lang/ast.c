
#include "ast.h"

expr_t *ast_expr_alloc_str(int type, const char *text) {
    expr_t *expr = calloc(1, sizeof(expr_t));
    if (expr) {
        char *str = strdup(text);

        if (!str) {
            free(expr);
            return NULL;
        }
        expr->body.data.str = str;
        expr->type = type;
    }
    return expr;
}

expr_t *ast_expr_alloc_num(int type, const char *text) {
    expr_t *expr = calloc(1, sizeof(expr_t));
    if (expr) {
        expr->body.data.num = atoi(text);
        expr->type = type;
    }
    return expr;
}

void ast_expr_free(void *noused, expr_t *e)
{
    if (e) {
        switch (e->type) {
        case EXPR_STRING:
        case EXPR_ID:
            free(e->body.data.str);
            break;
        case EXPR_FUNCPROC:
            ast_stmt_release(e->body.data.proc);
            break;
        default:
            ; // do nothing
        }

        free(e);
    }
}

void ast_traveral_expr(expr_t *e, void (*cb)(void *, expr_t *), void *ud)
{
    if (e) {
        if (e->type > EXPR_STRING) {
            ast_traveral_expr(e->body.child.lft, cb, ud);
        }

        if (e->type > EXPR_DICT) {
            ast_traveral_expr(e->body.child.rht, cb, ud);
        }

        cb(ud, e);
    }
}

void ast_expr_release(expr_t *e)
{
    ast_traveral_expr(e, ast_expr_free, NULL);
}

void ast_stmt_release(stmt_t *s)
{
    while (s) {
        stmt_t *next = s->next;

        if (s->expr)  ast_expr_release(s->expr);
        if (s->block) ast_stmt_release(s->block);
        if (s->ext)   ast_stmt_release(s->ext);

        free(s);
        s = next;
    }
}
