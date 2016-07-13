
#include "err.h"
#include "ast.h"
#include "lex.h"
#include "parse.h"

#define TRY(call, err) if (call != 0) {  \
    event = err; goto DO_ERROR;     \
}

static expr_t *parse_expr_form_parenth(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data);
static expr_t *parse_expr_form_array(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data);
static expr_t *parse_expr_form_dict(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data);
static expr_t *parse_expr_form_attr(intptr_t lex, expr_t *lft, void (*cb)(void *, parse_event_t *), void *user_data);
static expr_t *parse_expr_form_elem(intptr_t lex, expr_t *lft, void (*cb)(void *, parse_event_t *), void *user_data);
static expr_t *parse_expr_form_call(intptr_t lex, expr_t *lft, void (*cb)(void *, parse_event_t *), void *user_data);
static expr_t *parse_expr_form_pair(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data);
static expr_t *parse_expr_form_unary(intptr_t lex, expr_t *lft, void (*cb)(void *, parse_event_t *), void *user_data);
static expr_t *parse_expr_form_binary(intptr_t lex, expr_t *lft, expr_t *rht, void (*cb)(void *, parse_event_t *), void *user_data);

static expr_t *parse_expr_factor(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    token_t token;
    int tok = lex_token(lex, &token);
    expr_t *expr;

    switch (tok) {
        case '(': expr = parse_expr_form_parenth(lex, cb, user_data); break;
        case '[': expr = parse_expr_form_array(lex, cb, user_data); break;
        case '{': expr = parse_expr_form_dict(lex, cb, user_data); break;
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
        expr = parse_expr_form_unary(lex, parse_expr_unary(lex, cb, user_data), cb, user_data);
        if (expr) expr->type = EXPR_NOT;
    } else
    if (tok == '-' || tok == '~') {
        lex_match(lex, tok);
        expr = parse_expr_form_unary(lex, parse_expr_unary(lex, cb, user_data), cb, user_data);
        if (expr) expr->type = tok == '-' ? EXPR_MINUS : EXPR_NEGATE;
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
        lex_match(lex, tok);
        expr = parse_expr_form_binary(lex, expr, parse_expr_unary(lex, cb, user_data), cb, user_data);
        if (expr) {
            expr->type = tok == '*' ? EXPR_MUL : tok == '/' ? EXPR_DIV : EXPR_MOD;
            tok = lex_token(lex, NULL);
        }
    }

    return expr;
}

static expr_t *parse_expr_add(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr = parse_expr_mul(lex, cb, user_data);
    int tok = lex_token(lex, NULL);

    while (expr && (tok == '+' || tok == '-')) {
        lex_match(lex, tok);
        expr = parse_expr_form_binary(lex, expr, parse_expr_mul(lex, cb, user_data), cb, user_data);
        if (expr) {
            expr->type = tok == '+' ? EXPR_ADD : EXPR_SUB;
            tok = lex_token(lex, NULL);
        }
    }

    return expr;
}

static expr_t *parse_expr_shift(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr = parse_expr_add(lex, cb, user_data);
    int tok = lex_token(lex, NULL);

    while (expr && (tok == TOK_RSHIFT || tok == TOK_LSHIFT)) {
        lex_match(lex, tok);
        expr = parse_expr_form_binary(lex, expr, parse_expr_add(lex, cb, user_data), cb, user_data);
        if (expr) {
            expr->type = tok == TOK_RSHIFT ? EXPR_RSHIFT : EXPR_LSHIFT;
            tok = lex_token(lex, NULL);
        }
    }

    return expr;
}

static expr_t *parse_expr_aand (intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr = parse_expr_shift(lex, cb, user_data);
    int tok = lex_token(lex, NULL);

    while (expr && (tok == '&' || tok == '|' || tok == '^')) {
        lex_match(lex, tok);
        expr = parse_expr_form_binary(lex, expr, parse_expr_shift(lex, cb, user_data), cb, user_data);
        if (expr) {
            expr->type = tok == '&' ? EXPR_AAND : tok == '|' ? EXPR_AOR : EXPR_AXOR;
            tok = lex_token(lex, NULL);
        }
    }

    return expr;
}

static expr_t *parse_expr_test (intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr = parse_expr_aand(lex, cb, user_data);
    int tok = lex_token(lex, NULL);

    while (expr && (tok == '>' || tok == '<' || tok == TOK_NE ||
                    tok == TOK_EQ || tok == TOK_GE || tok == TOK_LE || tok == TOK_IN)) {
        lex_match(lex, tok);
        expr = parse_expr_form_binary(lex, expr, parse_expr_aand(lex, cb, user_data), cb, user_data);
        if (expr) {
            switch(tok) {
            case '>': expr->type = EXPR_TGT; break;
            case '<': expr->type = EXPR_TLT; break;
            case TOK_NE: expr->type = EXPR_TNE; break;
            case TOK_EQ: expr->type = EXPR_TEQ; break;
            case TOK_GE: expr->type = EXPR_TGE; break;
            case TOK_LE: expr->type = EXPR_TLE; break;
            default: expr->type = EXPR_TIN;
            }
            tok = lex_token(lex, NULL);
        }
    }

    return expr;
}

static expr_t *parse_expr_logic_and(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr = parse_expr_test(lex, cb, user_data);
    int tok = lex_token(lex, NULL);

    if (expr && (tok == TOK_LOGICAND)) {
        lex_match(lex, tok);
        expr = parse_expr_form_binary(lex, expr, parse_expr_logic_and(lex, cb, user_data), cb, user_data);
        if (expr) {
            expr->type = EXPR_LAND;
        }
    }

    return expr;
}

static expr_t *parse_expr_logic_or(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr = parse_expr_logic_and(lex, cb, user_data);
    int tok = lex_token(lex, NULL);

    if (expr && (tok == TOK_LOGICOR)) {
        lex_match(lex, tok);
        expr = parse_expr_form_binary(lex, expr, parse_expr_logic_or(lex, cb, user_data), cb, user_data);
        if (expr) {
            expr->type = EXPR_LOR;
        }
    }

    return expr;
}

static expr_t *parse_expr_ternary(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr = parse_expr_logic_or(lex, cb, user_data);
    int tok = lex_token(lex, NULL);

    if (expr && tok == '?') {
        lex_match(lex, tok);
        expr = parse_expr_form_binary(lex, expr, parse_expr_form_pair(lex, cb, user_data), cb, user_data);
        if (expr) {
            expr->type = EXPR_TERNARY;
        }
    }

    return expr;
}

static expr_t *parse_expr_assign(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr = parse_expr_ternary(lex, cb, user_data);
    int tok = lex_token(lex, NULL);

    if (expr && tok == '=') {
        if (expr->type != EXPR_ID) {
            ast_expr_relase(expr);
            if (cb) cb(user_data, NULL); // ERR_InvalidLeftValue
            return NULL;
        }

        lex_match(lex, tok);
        expr = parse_expr_form_binary(lex, expr, parse_expr_assign(lex, cb, user_data), cb, user_data);
        if (expr) {
            expr->type = EXPR_ASSIGN;
        }
    }

    return expr;
}

static expr_t *parse_expr_kvlist(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr = parse_expr_form_pair(lex, cb, user_data);

    if (expr && lex_token(lex, NULL) == ',') {
        lex_match(lex, ',');
        expr = parse_expr_form_binary(lex, expr, parse_expr_kvlist(lex, cb, user_data), cb, user_data);
        if (expr) {
            expr->type = EXPR_COMMA;
        }
    }

    return expr;
}

static expr_t *parse_expr_comma(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr = parse_expr_assign(lex, cb, user_data);
    int tok = lex_token(lex, NULL);

    if (expr && tok == ',') {
        lex_match(lex, tok);
        expr = parse_expr_form_binary(lex, expr, parse_expr_comma(lex, cb, user_data), cb, user_data);
        if (expr) {
            expr->type = EXPR_COMMA;
        }
    }

    return expr;
}

static expr_t *parse_expr_form_attr(intptr_t lex, expr_t *lft, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr;

    lex_match(lex, '.');

    if (TOK_ID != lex_token(lex, NULL)) {
        ast_expr_relase(lft);
        if (cb) cb(user_data, NULL); // ERR_InvalidToken
        return NULL;
    }

    expr = parse_expr_form_binary(lex, lft, parse_expr_factor(lex, cb, user_data), cb, user_data);
    if (expr) {
        expr->type = EXPR_ATTR;
    }

    return expr;
}

static expr_t *parse_expr_form_elem(intptr_t lex, expr_t *lft, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr;

    lex_match(lex, '[');

    expr = parse_expr_form_binary(lex, lft, parse_expr_ternary(lex, cb, user_data), cb, user_data);
    if (expr) {
        expr->type = EXPR_ELEM;
        if (!lex_match(lex, ']')) {
            ast_expr_relase(expr);
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

    expr = parse_expr_form_binary(lex, lft, parse_expr_comma(lex, cb, user_data), cb, user_data);
    if (expr) {
        expr->type = EXPR_CALL;
        if (!lex_match(lex, ')')) {
            ast_expr_relase(expr);
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
            ast_expr_relase(expr);
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

    expr = parse_expr_form_unary(lex, parse_expr_comma(lex, cb, user_data), cb, user_data);
    if (expr) {
        if (!lex_match(lex, ']')) {
            ast_expr_relase(expr);
            if (cb) cb(user_data, NULL); // ERR_InvalidToken
            return NULL;
        }
        expr->type = EXPR_ARRAY;
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

    expr = parse_expr_form_unary(lex, parse_expr_kvlist(lex, cb, user_data), cb, user_data);
    if (expr) {
        if (!lex_match(lex, '}')) {
            ast_expr_relase(expr);
            if (cb) cb(user_data, NULL); // ERR_InvalidToken
            return NULL;
        }
        expr->type = EXPR_DICT;
    }
    return expr;
}

static expr_t *parse_expr_form_pair(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr = parse_expr_ternary(lex, cb, user_data);
    int tok = lex_token(lex, NULL);

    if (expr) {
        if (tok != ':') {
            ast_expr_relase(expr);
            if (cb) cb(user_data, NULL); // ERR_InvalidToken
            return NULL;
        }
        lex_match(lex, tok);
        expr = parse_expr_form_binary(lex, expr, parse_expr_ternary(lex, cb, user_data), cb, user_data);
        if (expr) {
            expr->type = EXPR_PAIR;
        }
    }

    return expr;
}

static expr_t *parse_expr_form_unary(intptr_t lex, expr_t *lft, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr;

    if (!lft) {
        return NULL;
    }

    expr = ast_expr_alloc();
    if (!expr) {
        ast_expr_relase(lft);
        if (cb) cb(user_data, NULL); // ERR_NOTENOUGHMEMORY
        return NULL;
    }
    ast_expr_set_lft(expr, lft);

    return expr;
}

static expr_t *parse_expr_form_binary(intptr_t lex, expr_t *lft, expr_t *rht, void (*cb)(void *, parse_event_t *), void *user_data)
{
    expr_t *expr;

    if (!rht) {
        ast_expr_relase(lft);
        return NULL;
    }

    expr = ast_expr_alloc();
    if (!expr) {
        ast_expr_relase(lft);
        ast_expr_relase(rht);
        if (cb) cb(user_data, NULL);
        return NULL;
    }
    ast_expr_set_lft(expr, lft);
    ast_expr_set_rht(expr, rht);

    return expr;
}

expr_t *parse_expr(intptr_t lex, void (*cb)(void *, parse_event_t *), void *user_data)
{
    return parse_expr_comma(lex, cb, user_data);
}

