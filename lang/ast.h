


#ifndef __LANG_AST_INC__
#define __LANG_AST_INC__

#include "config.h"

#include "lex.h"

enum OP_CODE {
    OP_EQ,          // ==
    OP_NE,          // !=
    OP_GE,          // >=
    OP_LE,          // <=

    OP_ADDASSIGN,
    OP_SUBASSIGN,
    OP_MULASSIGN,
    OP_DIVASSIGN,
    OP_MODASSIGN,
    OP_ANDASSIGN,
    OP_ORASSIGN,
    OP_XORASSIGN,
    OP_NOTASSIGN,
    OP_LSHIFTASSIGN,
    OP_RSHIFTASSIGN,

    OP_LSHIFT,      // <<
    OP_RSHIFT,      // >>

    OP_LAND,        // &&
    OP_LOR,         // ||
};

enum EXPR_TYPE {
    // factor expression
    EXPR_ID,
    EXPR_NUM,
    EXPR_NAN,
    EXPR_UND,
    EXPR_NULL,
    EXPR_TRUE,
    EXPR_FALSE,
    EXPR_STRING,

    // unary expression
    EXPR_MINUS,
    EXPR_NEGATE,
    EXPR_NOT,
    EXPR_ARRAY,
    EXPR_DICT,

    // binary expression
    EXPR_MUL,
    EXPR_DIV,
    EXPR_MOD,

    EXPR_ADD,
    EXPR_SUB,

    EXPR_LSHIFT,
    EXPR_RSHIFT,

    EXPR_AAND,
    EXPR_AOR,
    EXPR_AXOR,

    EXPR_TNE,
    EXPR_TEQ,
    EXPR_TGT,
    EXPR_TGE,
    EXPR_TLT,
    EXPR_TLE,
    EXPR_TIN,

    EXPR_LAND,
    EXPR_LOR,

    EXPR_ASSIGN,
    EXPR_COMMA,

    EXPR_ATTR,
    EXPR_ELEM,
    EXPR_CALL,

    EXPR_PAIR,
    EXPR_TERNARY,
};

typedef struct expr_t {
    int type;
    union {
        union {
            char *str;
            int num;
        } data;
        struct {
            struct expr_t *lft;
            struct expr_t *rht;
        } child;
    } body;
} expr_t;

typedef struct stmt_t {
    int type;
} stmt_t;

static inline expr_t *ast_expr_alloc(void) {
    return (expr_t *) calloc(1, sizeof(expr_t));
}

expr_t *ast_expr_alloc_str(int type, const char *text);
expr_t *ast_expr_alloc_num(int type, const char *text);
static inline expr_t *ast_expr_alloc_type(int type) {
    expr_t *e = (expr_t *) calloc(1, sizeof(expr_t));

    if (e)
        e->type = type;

    return e;
}

void ast_expr_relase(expr_t *e);

static inline const char * ast_expr_text(expr_t *e) {
    return e->body.data.str;
}

static inline int ast_expr_num(expr_t *e) {
    return e->body.data.num;
}

static inline int ast_expr_type(expr_t *e) {
    return e->type;
}

static inline expr_t *ast_expr_lft(expr_t *e) {
    return e->body.child.lft;
}

static inline expr_t *ast_expr_rht(expr_t *e) {
    return e->body.child.rht;
}

static inline void ast_expr_set_lft(expr_t *e, expr_t *lft) {
    e->body.child.lft = lft;
}

static inline void ast_expr_set_rht(expr_t *e, expr_t *rht) {
    e->body.child.rht = rht;
}

static inline int ast_expr_set_id(expr_t *e, token_t *tok) {
    e->type = EXPR_ID;
    e->body.data.str = strdup(tok->text);
    return e->body.data.str ? 0 : 1;
}

void ast_traveral_expr(expr_t *e, void (*cb)(void *, expr_t *), void *user_data);

#endif /* __LANG_AST_INC__ */

