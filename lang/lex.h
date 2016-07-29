
#ifndef __LANG_LEX_INC__
#define __LANG_LEX_INC__

#include "config.h"

enum LEX_TOKEN_TYPE{
    TOK_EOF = 0,
    TOK_ID  = 256,
    TOK_NUM,
    TOK_STR,

    TOK_EQ, // ==
    TOK_NE, // !=
    TOK_GE, // >=
    TOK_LE, // <=

    TOK_ADDASSIGN,
    TOK_SUBASSIGN,
    TOK_MULASSIGN,
    TOK_DIVASSIGN,
    TOK_MODASSIGN,
    TOK_ANDASSIGN,
    TOK_ORASSIGN,
    TOK_XORASSIGN,
    TOK_NOTASSIGN,
    TOK_LSHIFTASSIGN,
    TOK_RSHIFTASSIGN,

    TOK_LSHIFT, // <<
    TOK_RSHIFT, // >>

    TOK_LOGICAND, // &&
    TOK_LOGICOR,  // ||

    /* Key words */
    TOK_UND,
    TOK_NAN,
    TOK_NULL,
    TOK_TRUE,
    TOK_FALSE,

    TOK_IN,
    TOK_IF,
    TOK_VAR,
    TOK_DEF,
    TOK_RET,
    TOK_ELSE,
    TOK_ELIF,
    TOK_WHILE,
    TOK_BREAK,
    TOK_CONTINUE
};

typedef struct lexer_t {
    int  curr_ch;
    int  next_ch;
    int  curr_tok;
    int  line, col;
    int  line_end, line_pos;
    int  token_str_len;
    int  (*getline)(void *, int);
    char token_str_buf[LEX_TOK_SIZE + 1];
    char line_buf[LEX_LINE_BUF_SIZE];
} lexer_t;

typedef struct token_t {
    int type;
    int line, col;
    int value; // value or length of id | string
    char *text;
} token_t;

intptr_t lex_init(lexer_t *lex, int getline(void *buf, int size));
int lex_deinit(lexer_t *lex);

intptr_t lex_create();
int lex_destroy();

int lex_token(intptr_t lex, token_t *tok);
int lex_match(intptr_t lex, int tok);
int lex_position(intptr_t lex, int *line, int *col);


#endif /* __LANG_LEX_INC__ */

