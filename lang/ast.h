


#ifndef __LANG_AST_INC__
#define __LANG_AST_INC__

#include "config.h"

#include "lex.h"

enum EXPR_TYPE {
    // factor expression
    EXPR_ID = 1,
    EXPR_NUM,
    EXPR_NAN,
    EXPR_UND,
    EXPR_NULL,
    EXPR_TRUE,
    EXPR_FALSE,
    EXPR_FUNCPROC,
    EXPR_STRING,

    // unary expression
    EXPR_NEG,
    EXPR_NOT,
    EXPR_LOGIC_NOT,
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

    EXPR_AND,
    EXPR_OR,
    EXPR_XOR,

    EXPR_TNE,
    EXPR_TEQ,
    EXPR_TGT,
    EXPR_TGE,
    EXPR_TLT,
    EXPR_TLE,
    EXPR_TIN,

    EXPR_LOGIC_AND,
    EXPR_LOGIC_OR,

    EXPR_ASSIGN,
    EXPR_ADD_ASSIGN,
    EXPR_SUB_ASSIGN,
    EXPR_MUL_ASSIGN,
    EXPR_DIV_ASSIGN,
    EXPR_MOD_ASSIGN,
    EXPR_AND_ASSIGN,
    EXPR_OR_ASSIGN,
    EXPR_XOR_ASSIGN,
    EXPR_NOT_ASSIGN,
    EXPR_LSHIFT_ASSIGN,
    EXPR_RSHIFT_ASSIGN,

    EXPR_COMMA,

    EXPR_PROP,
    EXPR_ELEM,
    EXPR_CALL,

    EXPR_TERNARY,

    // form
    EXPR_FUNCDEF,
    EXPR_FUNCHEAD,
    EXPR_PAIR,

    EXPR_DUMMY
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
    int line, col;
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

#endif /* __LANG_AST_INC__ */

