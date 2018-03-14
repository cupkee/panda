/* GPLv2 License
 *
 * Copyright (C) 2016-2018 Lixing Ding <ding.lixing@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 **/


#ifndef __LANG_PARSE_INC__
#define __LANG_PARSE_INC__

#include "def.h"

#include "lex.h"
#include "ast.h"
#include "heap.h"

enum {
    PARSE_EOF = 0,
    PARSE_FAIL,
    PARSE_SIMPLE,
    PARSE_COMPOSE,
    PARSE_ENTER_BLOCK,
    PARSE_LEAVE_BLOCK
};

struct parser_t;
typedef struct parse_event_t {
    int type;
    int line, col;
    struct parser_t *psr;
} parse_event_t;

typedef struct parser_t {
    int      error;
    lexer_t  lex;
    heap_t   heap;
    void (*usr_cb) (void *, parse_event_t *);
    void *usr_data;
} parser_t;

typedef void (*parse_callback_t)(void *u, parse_event_t *e);

static inline int parse_init(parser_t *psr, const char *input, char *(*more)(void), void *mem, int size) {
    if (psr && input && mem) {
        psr->error = 0;
        lex_init(&psr->lex, input, more);
        heap_init(&psr->heap, mem, size);
        psr->usr_cb = NULL;
        psr->usr_data = NULL;
        return 0;
    } else {
        return -1;
    }
}

static inline void parse_set_cb(parser_t *psr, parse_callback_t cb, void *data) {
    if (psr) {
        psr->usr_cb = cb;
        psr->usr_data = data;
    }
}

static inline void parse_disable_more(parser_t *psr) {
    psr->lex.line_more = NULL;
}

expr_t *parse_expr(parser_t *psr);
stmt_t *parse_stmt(parser_t *psr);
stmt_t *parse_stmt_multi(parser_t *psr);

static inline int parse_position(parser_t *psr, int *line, int *col) {
    return lex_position(&psr->lex, line, col);
}
static inline int parse_token(parser_t *psr, token_t *token) {
    return lex_token(&psr->lex, token);
}
static inline int parse_match(parser_t *psr, int tok) {
//    printf("match: %d(%c)\n", tok, tok);
    return lex_match(&psr->lex, tok);
}

#endif /* __LANG_PARSE_INC__ */

