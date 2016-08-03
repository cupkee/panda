
#include "ast.h"

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

