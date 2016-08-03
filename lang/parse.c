
#include "err.h"
#include "ast.h"
#include "lex.h"
#include "parse.h"

static expr_t *parse_expr_funcdef(intptr_t lex, parse_callback_t cb, void *ud);
static expr_t *parse_expr_form_parenth(intptr_t lex, parse_callback_t cb, void *ud);
static expr_t *parse_expr_form_array(intptr_t lex, parse_callback_t cb, void *ud);
static expr_t *parse_expr_form_dict(intptr_t lex, parse_callback_t cb, void *ud);
static expr_t *parse_expr_form_attr(intptr_t lex, expr_t *lft, parse_callback_t cb, void *ud);
static expr_t *parse_expr_form_elem(intptr_t lex, expr_t *lft, parse_callback_t cb, void *ud);
static expr_t *parse_expr_form_call(intptr_t lex, expr_t *lft, parse_callback_t cb, void *ud);
static expr_t *parse_expr_form_pair(intptr_t lex, parse_callback_t cb, void *ud);
static expr_t *parse_expr_form_unary(intptr_t lex, int type, expr_t *lft, parse_callback_t cb, void *ud);
static expr_t *parse_expr_form_binary(intptr_t lex, int type, expr_t *lft, expr_t *rht, parse_callback_t cb, void *ud);
static stmt_t *parse_stmt_block(intptr_t lex, parse_callback_t cb, void *ud);

static parse_event_t parse_event;

static void parse_fail(intptr_t lex, int err, parse_callback_t cb, void *ud)
{
    if (cb) {
        parse_event.type = PARSE_FAIL;
        parse_event.error.code = err;
        lex_position(lex, &parse_event.error.line, &parse_event.error.col);

        cb(ud, &parse_event);
    }
}

static void parse_eof(intptr_t lex, parse_callback_t cb, void *ud)
{
    if (cb) {
        parse_event.type = PARSE_EOF;
        cb(ud, &parse_event);
    }
}

static inline char *parse_strdup(intptr_t lex, const char *s) {
    int size  = strlen(s) + 1;
    char *dup = lex_malloc(lex, size);

    if (dup) {
        memcpy(dup, s, size);
    }
    return dup;
}

static inline expr_t *parse_expr_alloc_type(intptr_t lex, int type) {
    expr_t *e = (expr_t *) lex_malloc(lex, sizeof(expr_t));

    if (e)
        e->type = type;

    return e;
}

static inline expr_t *parse_expr_alloc_str(intptr_t lex, int type, const char *text) {
    expr_t *e = (expr_t *) lex_malloc(lex, sizeof(expr_t));

    if (e) {
        char *str = parse_strdup(lex, text);

        if (!str) {
            return NULL;
        }
        e->body.data.str = str;
        e->type = type;
    }
    return e;
}

static inline expr_t *parse_expr_alloc_num(intptr_t lex, int type, const char *text) {
    expr_t *e = (expr_t *) lex_malloc(lex, sizeof(expr_t));

    if (e) {
        e->body.data.num = atoi(text);
        e->type = type;
    }
    return e;
}

static inline expr_t *parse_expr_alloc_proc(intptr_t lex, stmt_t * s) {
    expr_t *e = (expr_t *) lex_malloc(lex, sizeof(expr_t));

    if (e) {
        e->type = EXPR_FUNCPROC;
        e->body.data.proc = s;
    }

    return e;
}

static inline stmt_t *parse_stmt_alloc_0(intptr_t lex, int type) {
    stmt_t *s = (stmt_t *) lex_malloc(lex, sizeof(stmt_t));

    if (s)
        s->type = type;

    return s;
}

static inline stmt_t *parse_stmt_alloc_1(intptr_t lex, int t, expr_t *e) {
    stmt_t *s = (stmt_t *) lex_malloc(lex, sizeof(stmt_t));

    if (s) {
        s->type = t;
        s->expr = e;
    }

    return s;
}

static inline stmt_t *parse_stmt_alloc_2(intptr_t lex, int t, expr_t *e, stmt_t *block) {
    stmt_t *s = (stmt_t *) lex_malloc(lex, sizeof(stmt_t));

    if (s) {
        s->type = t;
        s->expr = e;
        s->block = block;
    }

    return s;
}

static inline stmt_t *parse_stmt_alloc_3(intptr_t lex, int t, expr_t *e, stmt_t *block, stmt_t *other) {
    stmt_t *s = (stmt_t *) lex_malloc(lex, sizeof(stmt_t));

    if (s) {
        s->type = t;
        s->expr = e;
        s->block = block;
        s->other = other;
    }

    return s;
}

static expr_t *parse_expr_factor(intptr_t lex, parse_callback_t cb, void *ud)
{
    token_t token;
    int tok = lex_token(lex, &token);
    expr_t *expr = NULL;

    switch (tok) {
        case TOK_EOF:   parse_fail(lex, ERR_InvalidSyntax, cb, ud); break;
        case '(':       expr = parse_expr_form_parenth(lex, cb, ud); break;
        case '[':       expr = parse_expr_form_array(lex, cb, ud); break;
        case '{':       expr = parse_expr_form_dict(lex, cb, ud); break;
        case TOK_DEF:   expr = parse_expr_funcdef(lex, cb, ud) ; break;
        case TOK_ID:    if(!(expr = parse_expr_alloc_str(lex, EXPR_ID, token.text))) parse_fail(lex, ERR_NotEnoughMemory, cb, ud); lex_match(lex, tok); break;
        case TOK_NUM:   if(!(expr = parse_expr_alloc_num(lex, EXPR_NUM, token.text))) parse_fail(lex, ERR_NotEnoughMemory, cb, ud); lex_match(lex, tok); break;
        case TOK_STR:   if(!(expr = parse_expr_alloc_str(lex, EXPR_STRING, token.text))) parse_fail(lex, ERR_NotEnoughMemory, cb, ud); lex_match(lex, tok); break;
        case TOK_UND:   if(!(expr = parse_expr_alloc_type(lex, EXPR_UND))) parse_fail(lex, ERR_NotEnoughMemory, cb, ud); lex_match(lex, tok); break;
        case TOK_NAN:   if(!(expr = parse_expr_alloc_type(lex, EXPR_NAN))) parse_fail(lex, ERR_NotEnoughMemory, cb, ud); lex_match(lex, tok); break;
        case TOK_NULL:  if(!(expr = parse_expr_alloc_type(lex, EXPR_NULL))) parse_fail(lex, ERR_NotEnoughMemory, cb, ud); lex_match(lex, tok); break;
        case TOK_TRUE:  if(!(expr = parse_expr_alloc_type(lex, EXPR_TRUE))) parse_fail(lex, ERR_NotEnoughMemory, cb, ud); lex_match(lex, tok); break;
        case TOK_FALSE: if(!(expr = parse_expr_alloc_type(lex, EXPR_FALSE))) parse_fail(lex, ERR_NotEnoughMemory, cb, ud); lex_match(lex, tok); break;
        default:
                        parse_fail(lex, ERR_InvalidToken, cb, ud); break;
    }

    return expr;
}

static expr_t *parse_expr_primary(intptr_t lex, parse_callback_t cb, void *ud)
{
    expr_t *expr = parse_expr_factor(lex, cb, ud);
    int tok = lex_token(lex, NULL);

    //if (expr && expr->type != EXPR_ID) {
    //    return expr;
    //}

    while (expr && (tok == '.' || tok == '[' || tok == '(')) {
        if (tok == '.') {
            expr = parse_expr_form_attr(lex, expr, cb, ud);
        } else
        if (tok == '[') {
            expr = parse_expr_form_elem(lex, expr, cb, ud);
        } else {
            if (expr->type != EXPR_ID && expr->type != EXPR_PROP && expr->type != EXPR_ELEM && expr->type != EXPR_CALL) {
                break;
            }
            expr = parse_expr_form_call(lex, expr, cb, ud);
        }
        tok = lex_token(lex, NULL);
    }

    return expr;
}

static expr_t *parse_expr_unary(intptr_t lex, parse_callback_t cb, void *ud)
{
    expr_t *expr;
    int tok = lex_token(lex, NULL);

    if (tok == '!') {
        lex_match(lex, tok);
        expr = parse_expr_form_unary(lex, EXPR_LOGIC_NOT, parse_expr_unary(lex, cb, ud), cb, ud);
    } else
    if (tok == '-' || tok == '~') {
        lex_match(lex, tok);
        expr = parse_expr_form_unary(lex, tok == '-' ? EXPR_NEG : EXPR_NOT,
                                     parse_expr_unary(lex, cb, ud), cb, ud);
    } else {
        expr = parse_expr_primary(lex, cb, ud);
    }

    return expr;
}

static expr_t *parse_expr_mul(intptr_t lex, parse_callback_t cb, void *ud)
{
    expr_t *expr = parse_expr_unary(lex, cb, ud);
    int tok = lex_token(lex, NULL);

    while (expr && (tok == '*' || tok == '/' || tok == '%')) {
        int type = tok == '*' ? EXPR_MUL : tok == '/' ? EXPR_DIV : EXPR_MOD;

        lex_match(lex, tok);
        expr = parse_expr_form_binary(lex, type, expr, parse_expr_unary(lex, cb, ud), cb, ud);

        tok = lex_token(lex, NULL);
    }

    return expr;
}

static expr_t *parse_expr_add(intptr_t lex, parse_callback_t cb, void *ud)
{
    expr_t *expr = parse_expr_mul(lex, cb, ud);
    int tok = lex_token(lex, NULL);

    while (expr && (tok == '+' || tok == '-')) {
        lex_match(lex, tok);
        expr = parse_expr_form_binary(lex, tok == '+' ? EXPR_ADD : EXPR_SUB,
                                      expr, parse_expr_mul(lex, cb, ud), cb, ud);
        tok = lex_token(lex, NULL);
    }

    return expr;
}

static expr_t *parse_expr_shift(intptr_t lex, parse_callback_t cb, void *ud)
{
    expr_t *expr = parse_expr_add(lex, cb, ud);
    int tok = lex_token(lex, NULL);

    while (expr && (tok == TOK_RSHIFT || tok == TOK_LSHIFT)) {
        lex_match(lex, tok);
        expr = parse_expr_form_binary(lex, tok == TOK_RSHIFT ? EXPR_RSHIFT : EXPR_LSHIFT,
                                      expr, parse_expr_add(lex, cb, ud), cb, ud);
        tok = lex_token(lex, NULL);
    }

    return expr;
}

static expr_t *parse_expr_aand (intptr_t lex, parse_callback_t cb, void *ud)
{
    expr_t *expr = parse_expr_shift(lex, cb, ud);
    int tok = lex_token(lex, NULL);

    while (expr && (tok == '&' || tok == '|' || tok == '^')) {
        lex_match(lex, tok);
        expr = parse_expr_form_binary(lex, tok == '&' ? EXPR_AND : tok == '|' ? EXPR_OR : EXPR_XOR,
                                      expr, parse_expr_shift(lex, cb, ud), cb, ud);
        tok = lex_token(lex, NULL);
    }

    return expr;
}

static expr_t *parse_expr_test (intptr_t lex, parse_callback_t cb, void *ud)
{
    expr_t *expr = parse_expr_aand(lex, cb, ud);
    int tok = lex_token(lex, NULL);

    while (expr && (tok == '>' || tok == '<' || tok == TOK_NE ||
                    tok == TOK_EQ || tok == TOK_GE || tok == TOK_LE || tok == TOK_IN)) {
        int type;

        lex_match(lex, tok);
        switch(tok) {
            case '>': type = EXPR_TGT; break;
            case '<': type = EXPR_TLT; break;
            case TOK_NE: type = EXPR_TNE; break;
            case TOK_EQ: type = EXPR_TEQ; break;
            case TOK_GE: type = EXPR_TGE; break;
            case TOK_LE: type = EXPR_TLE; break;
            default: type = EXPR_TIN;
        }

        expr = parse_expr_form_binary(lex, type, expr, parse_expr_aand(lex, cb, ud), cb, ud);
        tok = lex_token(lex, NULL);
    }

    return expr;
}

static expr_t *parse_expr_logic_and(intptr_t lex, parse_callback_t cb, void *ud)
{
    expr_t *expr = parse_expr_test(lex, cb, ud);
    int tok = lex_token(lex, NULL);

    if (expr && (tok == TOK_LOGICAND)) {
        lex_match(lex, tok);
        expr = parse_expr_form_binary(lex, EXPR_LOGIC_AND, expr, parse_expr_logic_and(lex, cb, ud), cb, ud);
    }

    return expr;
}

static expr_t *parse_expr_logic_or(intptr_t lex, parse_callback_t cb, void *ud)
{
    expr_t *expr = parse_expr_logic_and(lex, cb, ud);
    int tok = lex_token(lex, NULL);

    if (expr && (tok == TOK_LOGICOR)) {
        lex_match(lex, tok);
        expr = parse_expr_form_binary(lex, EXPR_LOGIC_OR, expr, parse_expr_logic_or(lex, cb, ud), cb, ud);
    }

    return expr;
}

static expr_t *parse_expr_ternary(intptr_t lex, parse_callback_t cb, void *ud)
{
    expr_t *expr = parse_expr_logic_or(lex, cb, ud);
    int tok = lex_token(lex, NULL);

    if (expr && tok == '?') {
        lex_match(lex, tok);
        expr = parse_expr_form_binary(lex, EXPR_TERNARY, expr, parse_expr_form_pair(lex, cb, ud), cb, ud);
    }

    return expr;
}

static expr_t *parse_expr_assign(intptr_t lex, parse_callback_t cb, void *ud)
{
    expr_t *expr = parse_expr_ternary(lex, cb, ud);
    int tok = lex_token(lex, NULL);

    if (expr && tok == '=') {
        if (expr->type != EXPR_ID && expr->type != EXPR_PROP && expr->type != EXPR_ELEM) {
            parse_fail(lex, ERR_InvalidLeftValue, cb, ud);
            return NULL;
        }

        lex_match(lex, tok);
        expr = parse_expr_form_binary(lex, EXPR_ASSIGN, expr, parse_expr_assign(lex, cb, ud), cb, ud);
    }

    return expr;
}

static expr_t *parse_expr_kv(intptr_t lex, parse_callback_t cb, void *ud)
{
    int tok = lex_token(lex, NULL);
    expr_t *expr;

    if (tok != TOK_ID && tok != TOK_STR) {
        parse_fail(lex, ERR_InvalidToken, cb, ud);
        return NULL;
    }

    expr = parse_expr_factor(lex, cb, ud);
    if (expr) {
        if (!lex_match(lex, ':')) {
            parse_fail(lex, ERR_InvalidToken, cb, ud);
            return NULL;
        }
        expr = parse_expr_form_binary(lex, EXPR_PAIR, expr, parse_expr_assign(lex, cb, ud), cb, ud);
    }

    return expr;
}

static expr_t *parse_expr_vardef(intptr_t lex, parse_callback_t cb, void *ud)
{
    int tok = lex_token(lex, NULL);
    expr_t *expr;

    if (tok != TOK_ID) {
        parse_fail(lex, ERR_InvalidToken, cb, ud);
        return NULL;
    }

    expr = parse_expr_factor(lex, cb, ud);
    if (expr && lex_match(lex, '=')) {
        expr = parse_expr_form_binary(lex, EXPR_ASSIGN, expr, parse_expr_assign(lex, cb, ud), cb, ud);
    }

    return expr;
}

static expr_t *parse_expr_kvlist(intptr_t lex, parse_callback_t cb, void *ud)
{
    expr_t *expr = parse_expr_kv(lex, cb, ud);

    if (expr && lex_match(lex, ',')) {
        expr = parse_expr_form_binary(lex, EXPR_COMMA, expr, parse_expr_kvlist(lex, cb, ud), cb, ud);
    }

    return expr;
}

static expr_t *parse_expr_vardef_list(intptr_t lex, parse_callback_t cb, void *ud)
{
    expr_t *expr = parse_expr_vardef(lex, cb, ud);

    if (expr && lex_match(lex, ',')) {
        expr = parse_expr_form_binary(lex, EXPR_COMMA, expr, parse_expr_vardef_list(lex, cb, ud), cb, ud);
    }

    return expr;
}

static expr_t *parse_expr_comma(intptr_t lex, parse_callback_t cb, void *ud)
{
    expr_t *expr = parse_expr_assign(lex, cb, ud);

    if (expr && lex_match(lex, ',')) {
        expr = parse_expr_form_binary(lex, EXPR_COMMA, expr, parse_expr_comma(lex, cb, ud), cb, ud);
    }

    return expr;
}

static expr_t *parse_expr_funcdef(intptr_t lex, parse_callback_t cb, void *ud)
{
    expr_t *name = NULL, *param = NULL, *head = NULL, *proc = NULL;
    stmt_t *block = NULL;

    lex_match(lex, TOK_DEF);

    if (lex_token(lex, NULL) == TOK_ID) {
        if (!(name = parse_expr_factor(lex, cb, ud))) {
            return NULL;
        }
    }

    if (!lex_match(lex, '(')) {
        parse_fail(lex, ERR_InvalidToken, cb, ud);
        goto DO_ERROR;
    }

    if (!lex_match(lex, ')')) {
        if (!(param = parse_expr_vardef_list(lex, cb, ud))) {
            goto DO_ERROR;
        }
        if (!lex_match(lex, ')')) {
            parse_fail(lex, ERR_InvalidToken, cb, ud);
            goto DO_ERROR;
        }
    }

    if (!(block = parse_stmt_block(lex, cb, ud))) {
        goto DO_ERROR;
    }

    if (name || param) {
        if (!(head = parse_expr_alloc_type(lex, EXPR_FUNCHEAD))) {
            parse_fail(lex, ERR_NotEnoughMemory, cb, ud);
            goto DO_ERROR;
        }
        ast_expr_set_lft(head, name);
        ast_expr_set_rht(head, param);
    }

    if (!(proc = parse_expr_alloc_proc(lex, block))) {
        parse_fail(lex, ERR_NotEnoughMemory, cb, ud);
        goto DO_ERROR;
    }

    return parse_expr_form_binary(lex, EXPR_FUNCDEF, head, proc, cb, ud);

DO_ERROR:
    return NULL;
}

static expr_t *parse_expr_form_attr(intptr_t lex, expr_t *lft, parse_callback_t cb, void *ud)
{
    lex_match(lex, '.');

    if (TOK_ID != lex_token(lex, NULL)) {
        parse_fail(lex, ERR_InvalidToken, cb, ud);
        return NULL;
    }

    return parse_expr_form_binary(lex, EXPR_PROP, lft, parse_expr_factor(lex, cb, ud), cb, ud);
}

static expr_t *parse_expr_form_elem(intptr_t lex, expr_t *lft, parse_callback_t cb, void *ud)
{
    expr_t *expr;

    lex_match(lex, '[');

    expr = parse_expr_form_binary(lex, EXPR_ELEM, lft, parse_expr_ternary(lex, cb, ud), cb, ud);
    if (expr) {
        if (!lex_match(lex, ']')) {
            parse_fail(lex, ERR_InvalidToken, cb, ud);
            return NULL;
        }
    }

    return expr;
}

static expr_t *parse_expr_form_call(intptr_t lex, expr_t *lft, parse_callback_t cb, void *ud)
{
    expr_t *expr;

    lex_match(lex, '(');

    if (lex_match(lex, ')')) {
        expr = parse_expr_form_unary(lex, EXPR_CALL, lft, cb, ud);
    } else {
        expr = parse_expr_form_binary(lex, EXPR_CALL, lft, parse_expr_comma(lex, cb, ud), cb, ud);
        if (expr && ! lex_match(lex, ')')) {
            parse_fail(lex, ERR_InvalidToken, cb, ud);
            return NULL;
        }
    }

    return expr;
}

static expr_t *parse_expr_form_parenth(intptr_t lex, parse_callback_t cb, void *ud)
{
    expr_t *expr;

    // should not empty
    lex_match(lex, '(');
    expr = parse_expr_comma(lex, cb, ud);
    if (expr) {
        if (!lex_match(lex, ')')) {
            parse_fail(lex, ERR_InvalidToken, cb, ud);
            return NULL;
        }
    }
    return expr;
}

static expr_t *parse_expr_form_array(intptr_t lex, parse_callback_t cb, void *ud)
{
    expr_t *expr;

    lex_match(lex, '[');

    // empty array
    if (lex_match(lex, ']')) {
        if (NULL == (expr = parse_expr_alloc_type(lex, EXPR_ARRAY))) {
            parse_fail(lex, ERR_NotEnoughMemory, cb, ud);
        }
        return expr;
    }

    expr = parse_expr_form_unary(lex, EXPR_ARRAY, parse_expr_comma(lex, cb, ud), cb, ud);
    if (expr) {
        if (!lex_match(lex, ']')) {
            parse_fail(lex, ERR_InvalidToken, cb, ud);
            return NULL;
        }
    }
    return expr;
}

static expr_t *parse_expr_form_dict(intptr_t lex, parse_callback_t cb, void *ud)
{
    expr_t *expr;

    lex_match(lex, '{');

    // empty dict
    if (lex_match(lex, '}')) {
        if (NULL == (expr = parse_expr_alloc_type(lex, EXPR_DICT))) {
            parse_fail(lex, ERR_NotEnoughMemory, cb, ud);
        }
        return expr;
    }

    expr = parse_expr_form_unary(lex, EXPR_DICT, parse_expr_kvlist(lex, cb, ud), cb, ud);
    if (expr) {
        if (!lex_match(lex, '}')) {
            parse_fail(lex, ERR_InvalidToken, cb, ud);
            return NULL;
        }
    }
    return expr;
}

static expr_t *parse_expr_form_pair(intptr_t lex, parse_callback_t cb, void *ud)
{
    expr_t *expr = parse_expr_ternary(lex, cb, ud);

    if (expr) {
        if (!lex_match(lex, ':')) {
            parse_fail(lex, ERR_InvalidToken, cb, ud);
            return NULL;
        }
        expr = parse_expr_form_binary(lex, EXPR_PAIR, expr, parse_expr_ternary(lex, cb, ud), cb, ud);
    }

    return expr;
}

static expr_t *parse_expr_form_unary(intptr_t lex, int type, expr_t *lft, parse_callback_t cb, void *ud)
{
    expr_t *expr;

    if (!lft) {
        return NULL;
    }

    if (NULL == (expr = parse_expr_alloc_type(lex, type))) {
        parse_fail(lex, ERR_NotEnoughMemory, cb, ud);
    } else {
        ast_expr_set_lft(expr, lft);
    }

    return expr;
}

static expr_t *parse_expr_form_binary(intptr_t lex, int type, expr_t *lft, expr_t *rht, parse_callback_t cb, void *ud)
{
    expr_t *expr;

    if (!rht) {
        return NULL;
    }

    if (NULL == (expr = parse_expr_alloc_type(lex, type))) {
        parse_fail(lex, ERR_NotEnoughMemory, cb, ud);
    } else {
        ast_expr_set_lft(expr, lft);
        ast_expr_set_rht(expr, rht);
    }

    return expr;
}

static stmt_t *parse_stmt_block(intptr_t lex, parse_callback_t cb, void *ud)
{
    stmt_t *s = NULL;

    if (lex_match(lex, '{')) {
        if (!(s = parse_stmt_list(lex, cb, ud))) {
            return NULL;
        }
        if (!lex_match(lex, '}')) {
            parse_fail(lex, ERR_InvalidToken, cb, ud);
            return NULL;
        }
    } else {
        if (!(s = parse_stmt(lex, cb, ud))) {
            return NULL;
        }
    }

    return s;
}

static stmt_t *parse_stmt_if(intptr_t lex, parse_callback_t cb, void *ud)
{
    expr_t *cond = NULL;
    stmt_t *block = NULL;
    stmt_t *other = NULL;
    stmt_t *s;

    lex_match(lex, TOK_IF);

    if (!(cond = parse_expr(lex, cb, ud))) {
        return NULL;
    }

    if (!(block = parse_stmt_block(lex, cb, ud))) {
        return NULL;
    }

    if (lex_match(lex, TOK_ELSE)) {
        if (!(other = parse_stmt_block(lex, cb, ud))) {
            return NULL;
        }
    }

    if (other) {
        s = parse_stmt_alloc_3(lex, STMT_IF, cond, block, other);
    } else {
        s = parse_stmt_alloc_2(lex, STMT_IF, cond, block);
    }

    if (!s) {
        parse_fail(lex, ERR_NotEnoughMemory, cb, ud);
    }

    return s;
}

static stmt_t *parse_stmt_var(intptr_t lex, parse_callback_t cb, void *ud)
{
    expr_t *expr;

    lex_match(lex, TOK_VAR);
    expr = parse_expr_vardef_list(lex, cb, ud);
    if (expr) {
        stmt_t *s;
        lex_match(lex, ';');
        if (NULL != (s = parse_stmt_alloc_1(lex, STMT_VAR, expr))) {
            return s;
        } else {
            parse_fail(lex, ERR_NotEnoughMemory, cb, ud);
        }
    }

    return NULL;
}

static stmt_t *parse_stmt_ret(intptr_t lex, parse_callback_t cb, void *ud)
{
    expr_t *expr = NULL;
    stmt_t *s;

    lex_match(lex, TOK_RET);

    if (!lex_match(lex, ';')) {
        if (NULL == (expr = parse_expr(lex, cb, ud))) {
            return NULL;
        }
        lex_match(lex, ';');
    }

    if (!(s = parse_stmt_alloc_1(lex, STMT_RET, expr))) {
        parse_fail(lex, ERR_NotEnoughMemory, cb, ud);
    }
    return s;
}

static stmt_t *parse_stmt_while(intptr_t lex, parse_callback_t cb, void *ud)
{
    expr_t *cond = NULL;
    stmt_t *block = NULL;
    stmt_t *s;

    lex_match(lex, TOK_WHILE);

    if (!(cond = parse_expr(lex, cb, ud))) {
        return NULL;
    }

    if (!(block = parse_stmt_block(lex, cb, ud))) {
        return NULL;
    }

    s = parse_stmt_alloc_2(lex, STMT_WHILE, cond, block);
    if (!s) {
        parse_fail(lex, ERR_NotEnoughMemory, cb, ud);
    }

    return s;
}

static stmt_t *parse_stmt_break(intptr_t lex, parse_callback_t cb, void *ud)
{
    stmt_t *s;

    lex_match(lex, TOK_BREAK);
    lex_match(lex, ';');

    if (!(s = parse_stmt_alloc_0(lex, STMT_BREAK))) {
        parse_fail(lex, ERR_NotEnoughMemory, cb, ud);
    }
    return s;
}

static inline stmt_t *parse_stmt_continue(intptr_t lex, parse_callback_t cb, void *ud)
{
    stmt_t *s;

    lex_match(lex, TOK_CONTINUE);
    lex_match(lex, ';');

    if (!(s = parse_stmt_alloc_0(lex, STMT_CONTINUE))) {
        parse_fail(lex, ERR_NotEnoughMemory, cb, ud);
    }
    return s;
}

static stmt_t *parse_stmt_expr(intptr_t lex, parse_callback_t cb, void *ud)
{
    expr_t *expr = parse_expr(lex, cb, ud);

    lex_match(lex, ';');
    if (expr) {
        stmt_t *s = parse_stmt_alloc_1(lex, STMT_EXPR, expr);
        if (s) {
            return s;
        }
        parse_fail(lex, ERR_NotEnoughMemory, cb, ud);
    }

    return NULL;
}

expr_t *parse_expr(intptr_t lex, parse_callback_t cb, void *ud)
{
    return parse_expr_comma(lex, cb, ud);
}

stmt_t *parse_stmt(intptr_t lex, parse_callback_t cb, void *ud)
{
    int tok = lex_token(lex, NULL);

    switch (tok) {
        case TOK_EOF:       parse_eof(lex, cb, ud); return NULL;
        case TOK_IF:        return parse_stmt_if(lex, cb, ud);
        case TOK_VAR:       return parse_stmt_var(lex, cb, ud);
        case TOK_RET:       return parse_stmt_ret(lex, cb, ud);
        case TOK_WHILE:     return parse_stmt_while(lex, cb, ud);
        case TOK_BREAK:     return parse_stmt_break(lex, cb, ud);
        case TOK_CONTINUE:  return parse_stmt_continue(lex, cb, ud);
        default:            return parse_stmt_expr(lex, cb, ud);
    }
}

stmt_t *parse_stmt_list(intptr_t lex, parse_callback_t cb, void *ud)
{
    stmt_t *head = NULL, *last, *curr;
    int tok = lex_token(lex, NULL);

    while (tok != 0 && tok != '}') {
        while (lex_match(lex, ';'));

        if (!(curr = parse_stmt(lex, cb, ud))) {
            return NULL;
        }

        if (head) {
            last = last->next = curr;
        } else {
            last = head = curr;
        }

        tok = lex_token(lex, NULL);
    }

    if (!head) {
        if (!(head = parse_stmt_alloc_0(lex, STMT_PASS))) {
            parse_fail(lex, ERR_NotEnoughMemory, cb, ud);
        }
    }

    return head;
}

