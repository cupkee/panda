

#ifndef __LANG_PARSE_INC__
#define __LANG_PARSE_INC__

#include "config.h"

#include "lex.h"
#include "ast.h"

typedef struct parse_event_t {
    int noused;
} parse_event_t;

expr_t *parse_expr(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data);
stmt_t *parse_stmt(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data);
stmt_t *parse_stmt_list(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data);

#endif /* __LANG_PARSE_INC__ */

