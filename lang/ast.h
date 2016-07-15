


#ifndef __LANG_AST_INC__
#define __LANG_AST_INC__

#include "config.h"

#include "lex.h"
/*
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
*/

enum EXPR_TYPE {
    // factor expression
    EXPR_ID,
    EXPR_NUM,
    EXPR_NAN,
    EXPR_UND,
    EXPR_NULL,
    EXPR_TRUE,
    EXPR_FALSE,
    EXPR_FUNCPROC,
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

    EXPR_FUNCHEAD,
    EXPR_FUNCDEF,
    EXPR_PAIR,
    EXPR_TERNARY,
};

enum STMT_TYPE {
    STMT_EXPR,
    STMT_IF,
    STMT_VAR,
    STMT_RET,
    STMT_WHILE,
    STMT_BREAK,
    STMT_CONTINUE,
    STMT_PASS,
};

struct expr_t;
typedef struct stmt_t {
    int type;

    struct expr_t *expr;
    struct stmt_t *block;
    struct stmt_t *other;

    struct stmt_t *next;
} stmt_t;

typedef struct expr_t {
    int type;
    union {
        union {
            char   *str;
            int     num;
            stmt_t *proc;
        } data;
        struct {
            struct expr_t *lft;
            struct expr_t *rht;
        } child;
    } body;
} expr_t;

expr_t *ast_expr_alloc_str(int type, const char *text);
expr_t *ast_expr_alloc_num(int type, const char *text);
static inline expr_t *ast_expr_alloc_proc(stmt_t * s) {
    expr_t *e = (expr_t *) calloc(1, sizeof(expr_t));

    if (e) {
        e->type = EXPR_FUNCPROC;
        e->body.data.proc = s;
    }

    return e;
}
static inline expr_t *ast_expr_alloc_type(int type) {
    expr_t *e = (expr_t *) calloc(1, sizeof(expr_t));

    if (e)
        e->type = type;

    return e;
}

void ast_expr_release(expr_t *e);

static inline const char * ast_expr_text(expr_t *e) {
    return e->body.data.str;
}

static inline int ast_expr_num(expr_t *e) {
    return e->body.data.num;
}

static inline stmt_t * ast_expr_stmt(expr_t *e) {
    return e->body.data.proc;
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

void ast_traveral_expr(expr_t *e, void (*cb)(void *, expr_t *), void *user_data);

static inline stmt_t *ast_stmt_alloc_0(int type) {
    stmt_t *s = (stmt_t *) calloc(1, sizeof(stmt_t));

    if (s)
        s->type = type;

    return s;
}

static inline stmt_t *ast_stmt_alloc_1(int t, expr_t *e) {
    stmt_t *s = (stmt_t *) calloc(1, sizeof(stmt_t));

    if (s) {
        s->type = t;
        s->expr = e;
    }

    return s;
}

static inline stmt_t *ast_stmt_alloc_2(int t, expr_t *e, stmt_t *block) {
    stmt_t *s = (stmt_t *) calloc(1, sizeof(stmt_t));

    if (s) {
        s->type = t;
        s->expr = e;
        s->block = block;
    }

    return s;
}

static inline stmt_t *ast_stmt_alloc_3(int t, expr_t *e, stmt_t *block, stmt_t *other) {
    stmt_t *s = (stmt_t *) calloc(1, sizeof(stmt_t));

    if (s) {
        s->type = t;
        s->expr = e;
        s->block = block;
        s->other = other;
    }

    return s;
}

void ast_stmt_release(stmt_t *s);

#endif /* __LANG_AST_INC__ */

