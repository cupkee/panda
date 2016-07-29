

#ifndef __LANG_PARSE_INC__
#define __LANG_PARSE_INC__

#include "config.h"

#include "lex.h"
#include "ast.h"

enum {
    PARSE_EOF = 0,
    PARSE_FAIL,
    PARSE_ENTER_BLOCK,
    PARSE_LEAVE_BLOCK
};

typedef struct parse_event_t {
    int type;
    struct {
        int code;
        int line, col;
    } error;
} parse_event_t;
typedef void (*parse_callback_t)(void *u, parse_event_t *e);

expr_t *parse_expr(intptr_t lex, parse_callback_t cb, void *u);
stmt_t *parse_stmt(intptr_t lex, parse_callback_t cb, void *u);
stmt_t *parse_stmt_list(intptr_t lex, parse_callback_t cb, void *u);

#endif /* __LANG_PARSE_INC__ */

