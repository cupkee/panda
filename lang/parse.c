
#include "err.h"
#include "ast.h"
#include "lex.h"
#include "parse.h"

#define TRY(call, err) if (call != 0) {  \
    event = err; goto DO_ERROR;     \
}

static expr_t *parse_expr_funcdef(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data);
static expr_t *parse_expr_form_parenth(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data);
static expr_t *parse_expr_form_array(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data);
static expr_t *parse_expr_form_dict(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data);
static expr_t *parse_expr_form_attr(intptr_t lex, expr_t *lft, void (*cb)(void *, parse_event_t *), void *user_data);
static expr_t *parse_expr_form_elem(intptr_t lex, expr_t *lft, void (*cb)(void *, parse_event_t *), void *user_data);
static expr_t *parse_expr_form_call(intptr_t lex, expr_t *lft, void (*cb)(void *, parse_event_t *), void *user_data);
static expr_t *parse_expr_form_pair(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data);
static expr_t *parse_expr_form_unary(intptr_t lex, int type, expr_t *lft, void (*cb)(void *, parse_event_t *), void *user_data);
static expr_t *parse_expr_form_binary(intptr_t lex, int type, expr_t *lft, expr_t *rht, void (*cb)(void *, parse_event_t *), void *user_data);

static stmt_t *parse_stmt_block(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data);

static expr_t *parse_expr_factor(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    token_t token;
    int tok = lex_token(lex, &token);
    expr_t *expr;

    switch (tok) {
        case '(': expr = parse_expr_form_parenth(lex, cb, user_data); break;
        case '[': expr = parse_expr_form_array(lex, cb, user_data); break;
        case '{': expr = parse_expr_form_dict(lex, cb, user_data); break;
        case TOK_DEF:   expr = parse_expr_funcdef(lex, cb, user_data); break;
        case TOK_ID:    expr = ast_expr_alloc_str(EXPR_ID, token.text); lex_match(lex, tok); break;
        case TOK_NUM:   expr = ast_expr_alloc_num(EXPR_NUM, token.text); lex_match(lex, tok); break;
        case TOK_STR:   expr = ast_expr_alloc_str(EXPR_STRING, token.text); lex_match(lex, tok); break;
        case TOK_UND:   expr = ast_expr_alloc_type(EXPR_UND); lex_match(lex, tok); break;
        case TOK_NAN:   expr = ast_expr_alloc_type(EXPR_NAN); lex_match(lex, tok); break;
        case TOK_NULL:  expr = ast_expr_alloc_type(EXPR_NULL); lex_match(lex, tok); break;
        case TOK_TRUE:  expr = ast_expr_alloc_type(EXPR_TRUE); lex_match(lex, tok); break;
        case TOK_FALSE: expr = ast_expr_alloc_type(EXPR_FALSE); lex_match(lex, tok); break;
        default:
            if (cb) cb(user_data, NULL); // ERR_InvalidToken;
            return NULL;
    }

    if (!expr) {
        if (cb) cb(user_data, NULL); // ERR_NOTENOUGHMEMORY;
    }
    return expr;
}

static expr_t *parse_expr_primary(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr = parse_expr_factor(lex, cb, user_data);
    int tok = lex_token(lex, NULL);

    // the head of a primary expression should be a ID
    if (expr && expr->type != EXPR_ID) {
        return expr;
    }

    while (expr && (tok == '.' || tok == '[' || tok == '(')) {
        if (tok == '.') {
            expr = parse_expr_form_attr(lex, expr, cb, user_data);
        } else
        if (tok == '[') {
            expr = parse_expr_form_elem(lex, expr, cb, user_data);
        } else
        if (tok == '(') {
            expr = parse_expr_form_call(lex, expr, cb, user_data);
        }
        tok = lex_token(lex, NULL);
    }

    return expr;
}

static expr_t *parse_expr_unary(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr;
    int tok = lex_token(lex, NULL);

    if (tok == '!') {
        lex_match(lex, tok);
        expr = parse_expr_form_unary(lex, EXPR_NOT, parse_expr_unary(lex, cb, user_data), cb, user_data);
    } else
    if (tok == '-' || tok == '~') {
        lex_match(lex, tok);
        expr = parse_expr_form_unary(lex, tok == '-' ? EXPR_MINUS : EXPR_NEGATE,
                                     parse_expr_unary(lex, cb, user_data), cb, user_data);
    } else {
        expr = parse_expr_primary(lex, cb, user_data);
    }

    return expr;
}

static expr_t *parse_expr_mul(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr = parse_expr_unary(lex, cb, user_data);
    int tok = lex_token(lex, NULL);

    while (expr && (tok == '*' || tok == '/' || tok == '%')) {
        int type = tok == '*' ? EXPR_MUL : tok == '/' ? EXPR_DIV : EXPR_MOD;

        lex_match(lex, tok);
        expr = parse_expr_form_binary(lex, type, expr, parse_expr_unary(lex, cb, user_data), cb, user_data);

        tok = lex_token(lex, NULL);
    }

    return expr;
}

static expr_t *parse_expr_add(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr = parse_expr_mul(lex, cb, user_data);
    int tok = lex_token(lex, NULL);

    while (expr && (tok == '+' || tok == '-')) {
        lex_match(lex, tok);
        expr = parse_expr_form_binary(lex, tok == '+' ? EXPR_ADD : EXPR_SUB,
                                      expr, parse_expr_mul(lex, cb, user_data), cb, user_data);
        tok = lex_token(lex, NULL);
    }

    return expr;
}

static expr_t *parse_expr_shift(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr = parse_expr_add(lex, cb, user_data);
    int tok = lex_token(lex, NULL);

    while (expr && (tok == TOK_RSHIFT || tok == TOK_LSHIFT)) {
        lex_match(lex, tok);
        expr = parse_expr_form_binary(lex, tok == TOK_RSHIFT ? EXPR_RSHIFT : EXPR_LSHIFT,
                                      expr, parse_expr_add(lex, cb, user_data), cb, user_data);
        tok = lex_token(lex, NULL);
    }

    return expr;
}

static expr_t *parse_expr_aand (intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr = parse_expr_shift(lex, cb, user_data);
    int tok = lex_token(lex, NULL);

    while (expr && (tok == '&' || tok == '|' || tok == '^')) {
        lex_match(lex, tok);
        expr = parse_expr_form_binary(lex, tok == '&' ? EXPR_AAND : tok == '|' ? EXPR_AOR : EXPR_AXOR,
                                      expr, parse_expr_shift(lex, cb, user_data), cb, user_data);
        tok = lex_token(lex, NULL);
    }

    return expr;
}

static expr_t *parse_expr_test (intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr = parse_expr_aand(lex, cb, user_data);
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

        expr = parse_expr_form_binary(lex, type, expr, parse_expr_aand(lex, cb, user_data), cb, user_data);
        tok = lex_token(lex, NULL);
    }

    return expr;
}

static expr_t *parse_expr_logic_and(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr = parse_expr_test(lex, cb, user_data);
    int tok = lex_token(lex, NULL);

    if (expr && (tok == TOK_LOGICAND)) {
        lex_match(lex, tok);
        expr = parse_expr_form_binary(lex, EXPR_LAND, expr, parse_expr_logic_and(lex, cb, user_data), cb, user_data);
    }

    return expr;
}

static expr_t *parse_expr_logic_or(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr = parse_expr_logic_and(lex, cb, user_data);
    int tok = lex_token(lex, NULL);

    if (expr && (tok == TOK_LOGICOR)) {
        lex_match(lex, tok);
        expr = parse_expr_form_binary(lex, EXPR_LOR, expr, parse_expr_logic_or(lex, cb, user_data), cb, user_data);
    }

    return expr;
}

static expr_t *parse_expr_ternary(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr = parse_expr_logic_or(lex, cb, user_data);
    int tok = lex_token(lex, NULL);

    if (expr && tok == '?') {
        lex_match(lex, tok);
        expr = parse_expr_form_binary(lex, EXPR_TERNARY, expr, parse_expr_form_pair(lex, cb, user_data), cb, user_data);
    }

    return expr;
}

static expr_t *parse_expr_assign(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr = parse_expr_ternary(lex, cb, user_data);
    int tok = lex_token(lex, NULL);

    if (expr && tok == '=') {
        if (expr->type != EXPR_ID && expr->type != EXPR_ATTR && expr->type != EXPR_ELEM) {
            ast_expr_release(expr);
            if (cb) cb(user_data, NULL); // ERR_InvalidLeftValue
            return NULL;
        }

        lex_match(lex, tok);
        expr = parse_expr_form_binary(lex, EXPR_ASSIGN, expr, parse_expr_assign(lex, cb, user_data), cb, user_data);
    }

    return expr;
}

static expr_t *parse_expr_kv(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    int tok = lex_token(lex, NULL);
    expr_t *expr;

    if (tok != TOK_ID && tok != TOK_STR) {
        if (cb) cb(user_data, NULL);     // ERR_InvalidToken
        return NULL;
    }

    expr = parse_expr_factor(lex, cb, user_data);
    if (expr) {
        if (!lex_match(lex, ':')) {
            ast_expr_release(expr);
            if (cb) cb(user_data, NULL); // ERR_InvalidToken
            return NULL;
        }
        expr = parse_expr_form_binary(lex, EXPR_PAIR, expr, parse_expr_assign(lex, cb, user_data), cb, user_data);
    }

    return expr;
}

static expr_t *parse_expr_vardef(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    int tok = lex_token(lex, NULL);
    expr_t *expr;

    if (tok != TOK_ID) {
        if (cb) cb(user_data, NULL);     // ERR_InvalidToken
        return NULL;
    }

    expr = parse_expr_factor(lex, cb, user_data);
    if (expr && lex_match(lex, '=')) {
        expr = parse_expr_form_binary(lex, EXPR_ASSIGN, expr, parse_expr_assign(lex, cb, user_data), cb, user_data);
    }

    return expr;
}

static expr_t *parse_expr_kvlist(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr = parse_expr_kv(lex, cb, user_data);

    if (expr && lex_match(lex, ',')) {
        expr = parse_expr_form_binary(lex, EXPR_COMMA, expr, parse_expr_kvlist(lex, cb, user_data), cb, user_data);
    }

    return expr;
}

static expr_t *parse_expr_vardef_list(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr = parse_expr_vardef(lex, cb, user_data);

    if (expr && lex_match(lex, ',')) {
        expr = parse_expr_form_binary(lex, EXPR_COMMA, expr, parse_expr_vardef(lex, cb, user_data), cb, user_data);
    }

    return expr;
}

static expr_t *parse_expr_comma(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr = parse_expr_assign(lex, cb, user_data);

    if (expr && lex_match(lex, ',')) {
        expr = parse_expr_form_binary(lex, EXPR_COMMA, expr, parse_expr_comma(lex, cb, user_data), cb, user_data);
    }

    return expr;
}

static expr_t *parse_expr_funcdef(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *name = NULL, *param = NULL, *head = NULL, *proc = NULL;
    stmt_t *block = NULL;

    lex_match(lex, TOK_DEF);

    if (lex_token(lex, NULL) == TOK_ID) {
        if (!(name = parse_expr_factor(lex, cb, user_data))) {
            return NULL;
        }
    }

    if (!lex_match(lex, '(')) {
        if (cb) cb(user_data, NULL);  // err_invalidtoken
        goto DO_ERROR;
    }

    if (!lex_match(lex, ')')) {
        if (!(param = parse_expr_vardef_list(lex, cb, user_data))) {
            goto DO_ERROR;
        }
        if (!lex_match(lex, ')')) {
            if (cb) cb(user_data, NULL);  // err_invalidtoken
            goto DO_ERROR;
        }
    }

    if (!(block = parse_stmt_block(lex, cb, user_data))) {
        goto DO_ERROR;
    }

    if (name || param) {
        if (!(head = ast_expr_alloc_type(EXPR_FUNCHEAD))) {
            if (cb) cb(user_data, NULL);  // ERR_NOTENOUGHMEMORY
            goto DO_ERROR;
        }
        ast_expr_set_lft(head, name);
        ast_expr_set_rht(head, param);
    }

    if (!(proc = ast_expr_alloc_proc(block))) {
        if (cb) cb(user_data, NULL);  // ERR_NOTENOUGHMEMORY
        goto DO_ERROR;
    }

    return parse_expr_form_binary(lex, EXPR_FUNCDEF, head, proc, cb, user_data);

DO_ERROR:
    if (head) {
        ast_expr_release(head);
    } else {
        if (name) ast_expr_release(name);
        if (param) ast_expr_release(param);
    }
    if (block) ast_stmt_release(block);

    return NULL;
}

static expr_t *parse_expr_form_attr(intptr_t lex, expr_t *lft, void (*cb)(void *, parse_event_t *), void *user_data)
{
    lex_match(lex, '.');

    if (TOK_ID != lex_token(lex, NULL)) {
        ast_expr_release(lft);
        if (cb) cb(user_data, NULL); // ERR_InvalidToken
        return NULL;
    }

    return parse_expr_form_binary(lex, EXPR_ATTR, lft, parse_expr_factor(lex, cb, user_data), cb, user_data);
}

static expr_t *parse_expr_form_elem(intptr_t lex, expr_t *lft, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr;

    lex_match(lex, '[');

    expr = parse_expr_form_binary(lex, EXPR_ELEM, lft, parse_expr_ternary(lex, cb, user_data), cb, user_data);
    if (expr) {
        if (!lex_match(lex, ']')) {
            ast_expr_release(expr);
            if (cb) cb(user_data, NULL); // ERR_InvalidToken
            return NULL;
        }
    }

    return expr;
}

static expr_t *parse_expr_form_call(intptr_t lex, expr_t *lft, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr;

    lex_match(lex, '(');

    expr = parse_expr_form_binary(lex, EXPR_CALL, lft, parse_expr_comma(lex, cb, user_data), cb, user_data);
    if (expr) {
        if (!lex_match(lex, ')')) {
            ast_expr_release(expr);
            if (cb) cb(user_data, NULL); // ERR_InvalidToken
            return NULL;
        }
    }

    return expr;
}

static expr_t *parse_expr_form_parenth(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr;

    // should not empty
    lex_match(lex, '(');
    expr = parse_expr_comma(lex, cb, user_data);
    if (expr) {
        if (!lex_match(lex, ')')) {
            ast_expr_release(expr);
            if (cb) cb(user_data, NULL); // ERR_InvalidToken
            return NULL;
        }
    }
    return expr;
}

static expr_t *parse_expr_form_array(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr;

    lex_match(lex, '[');

    // empty array
    if (lex_match(lex, ']')) {
        return ast_expr_alloc_type(EXPR_ARRAY);
    }

    expr = parse_expr_form_unary(lex, EXPR_ARRAY, parse_expr_comma(lex, cb, user_data), cb, user_data);
    if (expr) {
        if (!lex_match(lex, ']')) {
            ast_expr_release(expr);
            if (cb) cb(user_data, NULL); // ERR_InvalidToken
            return NULL;
        }
    }
    return expr;
}

static expr_t *parse_expr_form_dict(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr;

    lex_match(lex, '{');

    // empty dict
    if (lex_match(lex, '}')) {
        return ast_expr_alloc_type(EXPR_DICT);
    }

    expr = parse_expr_form_unary(lex, EXPR_DICT, parse_expr_kvlist(lex, cb, user_data), cb, user_data);
    if (expr) {
        if (!lex_match(lex, '}')) {
            ast_expr_release(expr);
            if (cb) cb(user_data, NULL); // ERR_InvalidToken
            return NULL;
        }
    }
    return expr;
}

static expr_t *parse_expr_form_pair(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr = parse_expr_ternary(lex, cb, user_data);

    if (expr) {
        if (!lex_match(lex, ':')) {
            ast_expr_release(expr);
            if (cb) cb(user_data, NULL); // ERR_InvalidToken
            return NULL;
        }
        expr = parse_expr_form_binary(lex, EXPR_PAIR, expr, parse_expr_ternary(lex, cb, user_data), cb, user_data);
    }

    return expr;
}

static expr_t *parse_expr_form_unary(intptr_t lex, int type, expr_t *lft, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr;

    if (!lft) {
        return NULL;
    }

    expr = ast_expr_alloc_type(type);
    if (!expr) {
        ast_expr_release(lft);
        if (cb) cb(user_data, NULL); // ERR_NOTENOUGHMEMORY
        return NULL;
    }
    ast_expr_set_lft(expr, lft);

    return expr;
}

static expr_t *parse_expr_form_binary(intptr_t lex, int type, expr_t *lft, expr_t *rht, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr;

    if (!rht) {
        ast_expr_release(lft);
        return NULL;
    }

    expr = ast_expr_alloc_type(type);
    if (!expr) {
        ast_expr_release(lft);
        ast_expr_release(rht);
        if (cb) cb(user_data, NULL);
        return NULL;
    }
    ast_expr_set_lft(expr, lft);
    ast_expr_set_rht(expr, rht);

    return expr;
}

static stmt_t *parse_stmt_block(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    stmt_t *s = NULL;

    if (lex_match(lex, '{')) {
        if (!(s = parse_stmt_list(lex, cb, user_data))) {
            return NULL;
        }
        if (!lex_match(lex, '}')) {
            if (cb) cb(user_data, NULL);  // err_invalidtoken
            ast_stmt_release(s);
            return NULL;
        }
    } else {
        if (!(s = parse_stmt(lex, cb, user_data))) {
            return NULL;
        }
    }

    if (s) {
        return s;
    }

    return s;
}

static stmt_t *parse_stmt_if(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *cond = NULL;
    stmt_t *block = NULL;
    stmt_t *other = NULL;
    stmt_t *s;

    lex_match(lex, TOK_IF);

    if (!(cond = parse_expr(lex, cb, user_data))) {
        return NULL;
    }

    if (!(block = parse_stmt_block(lex, cb, user_data))) {
        ast_expr_release(cond);
        return NULL;
    }

    if (lex_match(lex, TOK_ELSE)) {
        if (!(other = parse_stmt_block(lex, cb, user_data))) {
            ast_expr_release(cond);
            ast_stmt_release(block);
            return NULL;
        }
    }

    if (other) {
        s = ast_stmt_alloc_2(STMT_IF, cond, block);
    } else {
        s = ast_stmt_alloc_3(STMT_IF, cond, block, other);
    }

    if (!s) {
        ast_expr_release(cond);
        ast_stmt_release(block);
        if (other) ast_stmt_release(other);
    }

    return s;
}

static stmt_t *parse_stmt_var(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr;

    lex_match(lex, TOK_VAR);
    expr = parse_expr_vardef_list(lex, cb, user_data);
    if (expr) {
        lex_match(lex, ';');
        return ast_stmt_alloc_1(STMT_VAR, expr);
    }

    return NULL;
}

static stmt_t *parse_stmt_ret(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr = NULL;

    lex_match(lex, TOK_RET);

    if (!lex_match(lex, ';')) {
        if (NULL == (expr = parse_expr(lex, cb, user_data))) {
            return NULL;
        }
        lex_match(lex, ';');
    }

    return ast_stmt_alloc_1(STMT_RET, expr);
}

static stmt_t *parse_stmt_while(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    return NULL;
}

static stmt_t *parse_stmt_break(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    lex_match(lex, TOK_BREAK);
    lex_match(lex, ';');

    return ast_stmt_alloc_0(STMT_BREAK);
}

static inline stmt_t *parse_stmt_continue(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    lex_match(lex, TOK_CONTINUE);
    lex_match(lex, ';');

    return ast_stmt_alloc_0(STMT_CONTINUE);
}

static stmt_t *parse_stmt_expr(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr = parse_expr(lex, cb, user_data);

    if (expr) {
        lex_match(lex, ';');
        return ast_stmt_alloc_1(STMT_EXPR, expr);
    }

    return NULL;
}

expr_t *parse_expr(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    return parse_expr_comma(lex, cb, user_data);
}

stmt_t *parse_stmt(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    int tok = lex_token(lex, NULL);

    switch (tok) {
        case TOK_IF:        return parse_stmt_if(lex, cb, user_data);
        case TOK_VAR:       return parse_stmt_var(lex, cb, user_data);
        case TOK_RET:       return parse_stmt_ret(lex, cb, user_data);
        case TOK_WHILE:     return parse_stmt_while(lex, cb, user_data);
        case TOK_BREAK:     return parse_stmt_break(lex, cb, user_data);
        case TOK_CONTINUE:  return parse_stmt_continue(lex, cb, user_data);
        default:            return parse_stmt_expr(lex, cb, user_data);
    }
}

stmt_t *parse_stmt_list(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    stmt_t *head = NULL, *last, *curr;
    int tok = lex_token(lex, NULL);

    while (tok != 0 && tok != '}') {
        // eat the empty statements
        while (lex_match(lex, ';'));

        if (!(curr = parse_stmt(lex, cb, user_data))) {
            if (head) ast_stmt_release(head);
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
        if (!(head = ast_stmt_alloc_0(STMT_PASS))) {
            if (cb) cb(user_data, NULL); // err_notenoughmemory
        }
    }

    return head;
}

