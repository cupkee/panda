

#ifndef __LANG_PARSE_INC__
#define __LANG_PARSE_INC__

#include "config.h"

#include "lex.h"
#include "ast.h"
#include "heap.h"

enum {
    PARSE_EOF = 0,
    PARSE_FAIL,
    PARSE_ENTER_BLOCK,
    PARSE_LEAVE_BLOCK
};

typedef struct parser_t {
    int      error;
    intptr_t lex;
    heap_t  *heap;
} parser_t;

typedef struct parse_event_t {
    int type;
    struct {
        int code;
        int line, col;
    } error;
} parse_event_t;

typedef void (*parse_callback_t)(void *u, parse_event_t *e);

static inline int parse_init(parser_t *psr, intptr_t lex, heap_t *heap) {
    if (psr && lex && heap) {
        psr->error = 0;
        psr->lex = lex;
        psr->heap = heap;
        return 0;
    } else {
        return -1;
    }
}

expr_t *parse_expr(parser_t *psr, parse_callback_t cb, void *u);
stmt_t *parse_stmt(parser_t *psr, parse_callback_t cb, void *u);
stmt_t *parse_stmt_list(parser_t *psr, parse_callback_t cb, void *u);

static inline int parse_position(parser_t *psr, int *line, int *col) {
    return lex_position(psr->lex, line, col);
}
static inline int parse_token(parser_t *psr, token_t *token) {
    return lex_token(psr->lex, token);
}
static inline int parse_match(parser_t *psr, int tok) {
    return lex_match(psr->lex, tok);
}

#endif /* __LANG_PARSE_INC__ */

