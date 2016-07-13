
#include <stdio.h>
#include <string.h>

#include "cunit/CUnit.h"
#include "cunit/CUnit_Basic.h"

#include "test_util.h"

#include "lang/lex.h"
#include "lang/parse.h"

#define L_ ast_expr_lft
#define R_ ast_expr_rht
#define TEXT ast_expr_text
#define NUMBER ast_expr_num

static int test_setup()
{
    return 0;
}

static int test_clean()
{
    return 0;
}

static void test_expr_factor(void)
{
    lexer_t  lex_st;
    intptr_t lex;
    expr_t   *expr;

    test_clr_line();
    test_set_line("true false\n");
    test_set_line("undefined null NaN\n");
    test_set_line("'null' hello 12345\n");
    test_set_line(" (a + b) \n");
    test_set_line(" [a, b] \n");
    test_set_line(" {a: 1, b: 2} \n");

    CU_ASSERT(0 != (lex = lex_init(&lex_st, test_get_line)));

    CU_ASSERT(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_TRUE);
    ast_expr_relase(expr);

    CU_ASSERT(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_FALSE);
    ast_expr_relase(expr);

    CU_ASSERT(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_UND);
    ast_expr_relase(expr);

    CU_ASSERT(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_NULL);
    ast_expr_relase(expr);

    CU_ASSERT(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_NAN);
    ast_expr_relase(expr);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_STRING && !strcmp("null", TEXT(expr)));
    ast_expr_relase(expr);

    CU_ASSERT(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_ID && !strcmp("hello", TEXT(expr)));
    ast_expr_relase(expr);

    CU_ASSERT(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_NUM && NUMBER(expr) == 12345);
    ast_expr_relase(expr);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_ADD);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID && !strcmp("a", TEXT(L_(expr))));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_ID && !strcmp("b", TEXT(R_(expr))));
    ast_expr_relase(expr);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_ARRAY);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_COMMA);
    CU_ASSERT(L_(L_(expr)) && ast_expr_type(L_(L_((expr)))) == EXPR_ID && !strcmp("a", TEXT(L_(L_(expr)))));
    CU_ASSERT(R_(L_(expr)) && ast_expr_type(R_(L_((expr)))) == EXPR_ID && !strcmp("b", TEXT(R_(L_(expr)))));
    ast_expr_relase(expr);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_DICT);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_COMMA);
    CU_ASSERT(L_(L_(expr)) && ast_expr_type(L_(L_((expr)))) == EXPR_PAIR);
    CU_ASSERT(R_(L_(expr)) && ast_expr_type(R_(L_((expr)))) == EXPR_PAIR);
    CU_ASSERT(L_(L_(L_(expr))) && ast_expr_type(L_(L_(L_((expr))))) == EXPR_ID && !strcmp("a", TEXT(L_(L_(L_(expr))))));
    CU_ASSERT(R_(L_(L_(expr))) && ast_expr_type(R_(L_(L_((expr))))) == EXPR_NUM && 1 == NUMBER(R_(L_(L_(expr)))));
    CU_ASSERT(L_(R_(L_(expr))) && ast_expr_type(L_(R_(L_((expr))))) == EXPR_ID && !strcmp("b", TEXT(L_(R_(L_(expr))))));
    CU_ASSERT(R_(R_(L_(expr))) && ast_expr_type(R_(R_(L_((expr))))) == EXPR_NUM && 2 == NUMBER(R_(R_(L_(expr)))));
    ast_expr_relase(expr);
}

static void test_expr_primary(void)
{
    lexer_t  lex_st;
    intptr_t lex;
    expr_t   *expr;

    test_clr_line();
    test_set_line("a.b a.b.c\n");
    test_set_line("a[b] a[b][c]\n");
    test_set_line("a(b) a(b)(c)\n");
    test_set_line("f(a)[b].c[d](e)\n");

    CU_ASSERT(0 != (lex = lex_init(&lex_st, test_get_line)));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_ATTR);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID && !strcmp("a", TEXT(L_(expr))));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_ID && !strcmp("b", TEXT(R_(expr))));
    ast_expr_relase(expr);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_ATTR);
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_ID && !strcmp("c", TEXT(R_(expr))));
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ATTR);
    CU_ASSERT(L_(L_(expr)) && ast_expr_type(L_(L_(expr))) == EXPR_ID && !strcmp("a", TEXT(L_(L_(expr)))));
    CU_ASSERT(R_(L_(expr)) && ast_expr_type(R_(L_(expr))) == EXPR_ID && !strcmp("b", TEXT(R_(L_(expr)))));
    ast_expr_relase(expr);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_ELEM);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID && !strcmp("a", TEXT(L_(expr))));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_ID && !strcmp("b", TEXT(R_(expr))));
    ast_expr_relase(expr);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_ELEM);
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_ID && !strcmp("c", TEXT(R_(expr))));
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ELEM);
    CU_ASSERT(L_(L_(expr)) && ast_expr_type(L_(L_(expr))) == EXPR_ID && !strcmp("a", TEXT(L_(L_(expr)))));
    CU_ASSERT(R_(L_(expr)) && ast_expr_type(R_(L_(expr))) == EXPR_ID && !strcmp("b", TEXT(R_(L_(expr)))));
    ast_expr_relase(expr);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_CALL);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID && !strcmp("a", TEXT(L_(expr))));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_ID && !strcmp("b", TEXT(R_(expr))));
    ast_expr_relase(expr);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_CALL);
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_ID && !strcmp("c", TEXT(R_(expr))));
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_CALL);
    CU_ASSERT(L_(L_(expr)) && ast_expr_type(L_(L_(expr))) == EXPR_ID && !strcmp("a", TEXT(L_(L_(expr)))));
    CU_ASSERT(R_(L_(expr)) && ast_expr_type(R_(L_(expr))) == EXPR_ID && !strcmp("b", TEXT(R_(L_(expr)))));
    ast_expr_relase(expr);

    // f(a)[b].c[d](e)
    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_CALL);
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_ID && !strcmp("e", TEXT(R_(expr))));
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ELEM);
    CU_ASSERT(R_(L_(expr)) && ast_expr_type(R_(L_(expr))) == EXPR_ID && !strcmp("d", TEXT(R_(L_(expr)))));
    CU_ASSERT(L_(L_(expr)) && ast_expr_type(L_(L_(expr))) == EXPR_ATTR);
    CU_ASSERT(R_(L_(L_(expr))) && ast_expr_type(R_(L_(L_(expr)))) == EXPR_ID && !strcmp("c", TEXT(R_(L_(L_(expr))))));
    CU_ASSERT(L_(L_(L_(expr))) && ast_expr_type(L_(L_(L_(expr)))) == EXPR_ELEM);
    CU_ASSERT(R_(L_(L_(L_(expr)))) && ast_expr_type(R_(L_(L_(L_(expr))))) == EXPR_ID && !strcmp("b", TEXT(R_(L_(L_(L_(expr)))))));
    CU_ASSERT(L_(L_(L_(L_(expr)))) && ast_expr_type(L_(L_(L_(L_(expr))))) == EXPR_CALL);
    CU_ASSERT(R_(L_(L_(L_(L_(expr))))) && ast_expr_type(R_(L_(L_(L_(L_(expr)))))) == EXPR_ID && !strcmp("a", TEXT(R_(L_(L_(L_(L_(expr))))))));
    CU_ASSERT(L_(L_(L_(L_(L_(expr))))) && ast_expr_type(L_(L_(L_(L_(L_(expr)))))) == EXPR_ID && !strcmp("f", TEXT(L_(L_(L_(L_(L_(expr))))))));
    ast_expr_relase(expr);
}

static void test_expr_unary(void)
{
    lexer_t  lex_st;
    intptr_t lex;
    expr_t   *expr;

    test_clr_line();
    test_set_line("-a ~b !c !!d\n");

    CU_ASSERT(0 != (lex = lex_init(&lex_st, test_get_line)));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_MINUS);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID && !strcmp("a", TEXT(L_(expr))));
    ast_expr_relase(expr);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_NEGATE);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID && !strcmp("b", TEXT(L_(expr))));
    ast_expr_relase(expr);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_NOT);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID && !strcmp("c", TEXT(L_(expr))));
    ast_expr_relase(expr);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_NOT);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_NOT);
    CU_ASSERT(L_(L_(expr)) && ast_expr_type(L_(L_(expr))) == EXPR_ID
                && !strcmp("d", TEXT(L_(L_(expr)))));
    ast_expr_relase(expr);
}

static void test_expr_mul(void)
{
    lexer_t  lex_st;
    intptr_t lex;
    expr_t   *expr;

    test_clr_line();
    test_set_line("a*b 1/2 c%2\n");

    CU_ASSERT(0 != (lex = lex_init(&lex_st, test_get_line)));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_MUL);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID && !strcmp("a", TEXT(L_(expr))));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_ID && !strcmp("b", TEXT(R_(expr))));
    ast_expr_relase(expr);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_DIV);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_NUM && 1 == NUMBER(L_(expr)));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_NUM && 2 == NUMBER(R_(expr)));
    ast_expr_relase(expr);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_MOD);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID && !strcmp("c", TEXT(L_(expr))));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_NUM && 2 == NUMBER(R_(expr)));
    ast_expr_relase(expr);
}

static void test_expr_add(void)
{
    lexer_t  lex_st;
    intptr_t lex;
    expr_t   *expr;

    test_clr_line();
    test_set_line("a+b 1-2 c+1-2*f\n");

    CU_ASSERT(0 != (lex = lex_init(&lex_st, test_get_line)));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_ADD);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID && !strcmp("a", TEXT(L_(expr))));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_ID && !strcmp("b", TEXT(R_(expr))));
    ast_expr_relase(expr);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_SUB);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_NUM && 1 == NUMBER(L_(expr)));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_NUM && 2 == NUMBER(R_(expr)));
    ast_expr_relase(expr);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_SUB);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ADD);
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_MUL);
    CU_ASSERT(L_(L_(expr)) && ast_expr_type(L_(L_(expr))) == EXPR_ID);
    CU_ASSERT(L_(L_(expr)) && 0 == strcmp(TEXT(L_(L_(expr))), "c"));
    CU_ASSERT(R_(L_(expr)) && NUMBER(R_(L_(expr))) == 1);
    CU_ASSERT(L_(R_(expr)) && NUMBER(L_(R_(expr))) == 2);
    CU_ASSERT(R_(R_(expr)) && 0 == strcmp(TEXT(R_(R_(expr))), "f"));
    ast_expr_relase(expr);
}

static void test_expr_shift(void)
{
    lexer_t  lex_st;
    intptr_t lex;
    expr_t   *expr;

    test_clr_line();
    test_set_line("1<<1 1>>2 1<<1+2 1+2>>1\n");

    CU_ASSERT(0 != (lex = lex_init(&lex_st, test_get_line)));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_LSHIFT);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_NUM && 1 == NUMBER(L_(expr)));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_NUM && 1 == NUMBER(R_(expr)));
    ast_expr_relase(expr);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_RSHIFT);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_NUM && 1 == NUMBER(L_(expr)));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_NUM && 2 == NUMBER(R_(expr)));
    ast_expr_relase(expr);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_LSHIFT);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_NUM && NUMBER(L_(expr)) == 1);
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_ADD);
    CU_ASSERT(L_(R_(expr)) && NUMBER(L_(R_(expr))) == 1);
    CU_ASSERT(R_(R_(expr)) && NUMBER(R_(R_(expr))) == 2);
    ast_expr_relase(expr);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_RSHIFT);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ADD);
    CU_ASSERT(L_(L_(expr)) && NUMBER(L_(L_(expr))) == 1);
    CU_ASSERT(R_(L_(expr)) && NUMBER(R_(L_(expr))) == 2);
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_NUM && NUMBER(R_(expr)) == 1);
    ast_expr_relase(expr);
}

static void test_expr_test(void)
{
    lexer_t  lex_st;
    intptr_t lex;
    expr_t   *expr;

    test_clr_line();
    test_set_line("1>1 2<2 3>=3 4<=4 5!=5 6==6 a in b\n");

    CU_ASSERT(0 != (lex = lex_init(&lex_st, test_get_line)));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_TGT);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_NUM && 1 == NUMBER(L_(expr)));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_NUM && 1 == NUMBER(R_(expr)));
    ast_expr_relase(expr);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_TLT);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_NUM && 2 == NUMBER(L_(expr)));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_NUM && 2 == NUMBER(R_(expr)));
    ast_expr_relase(expr);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_TGE);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_NUM && 3 == NUMBER(L_(expr)));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_NUM && 3 == NUMBER(R_(expr)));
    ast_expr_relase(expr);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_TLE);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_NUM && 4 == NUMBER(L_(expr)));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_NUM && 4 == NUMBER(R_(expr)));
    ast_expr_relase(expr);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_TNE);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_NUM && 5 == NUMBER(L_(expr)));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_NUM && 5 == NUMBER(R_(expr)));
    ast_expr_relase(expr);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_TEQ);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_NUM && 6 == NUMBER(L_(expr)));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_NUM && 6 == NUMBER(R_(expr)));
    ast_expr_relase(expr);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_TIN);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID && !strcmp("a", TEXT(L_(expr))));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_ID && !strcmp("b", TEXT(R_(expr))));
    ast_expr_relase(expr);
}

static void test_expr_logic(void)
{
    lexer_t  lex_st;
    intptr_t lex;
    expr_t   *expr;

    test_clr_line();
    test_set_line("a&&b||c&&d\n");

    CU_ASSERT(0 != (lex = lex_init(&lex_st, test_get_line)));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_LOR);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_LAND);
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_LAND);
    CU_ASSERT(L_(L_(expr)) && ast_expr_type(L_(L_(expr))) == EXPR_ID);
    CU_ASSERT(L_(L_(expr)) && 0 == strcmp(TEXT(L_(L_(expr))), "a"));
    CU_ASSERT(R_(L_(expr)) && ast_expr_type(R_(L_(expr))) == EXPR_ID);
    CU_ASSERT(R_(L_(expr)) && 0 == strcmp(TEXT(R_(L_(expr))), "b"));
    CU_ASSERT(L_(R_(expr)) && ast_expr_type(L_(R_(expr))) == EXPR_ID);
    CU_ASSERT(L_(R_(expr)) && 0 == strcmp(TEXT(L_(R_(expr))), "c"));
    CU_ASSERT(R_(R_(expr)) && ast_expr_type(R_(R_(expr))) == EXPR_ID);
    CU_ASSERT(R_(R_(expr)) && 0 == strcmp(TEXT(R_(R_(expr))), "d"));

    ast_expr_relase(expr);
}

static void test_expr_ternary(void)
{
    lexer_t  lex_st;
    intptr_t lex;
    expr_t   *expr;

    test_clr_line();
    test_set_line("a ? b: c\n");

    CU_ASSERT(0 != (lex = lex_init(&lex_st, test_get_line)));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_TERNARY);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID);
    CU_ASSERT(L_(expr) && 0 == strcmp(TEXT(L_(expr)), "a"));

    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_PAIR);
    CU_ASSERT(L_(R_(expr)) && ast_expr_type(L_(R_(expr))) == EXPR_ID);
    CU_ASSERT(L_(R_(expr)) && 0 == strcmp(TEXT(L_(R_(expr))), "b"));
    CU_ASSERT(R_(R_(expr)) && ast_expr_type(R_(R_(expr))) == EXPR_ID);
    CU_ASSERT(R_(R_(expr)) && 0 == strcmp(TEXT(R_(R_(expr))), "c"));

    ast_expr_relase(expr);
}

static void test_expr_assign(void)
{
    lexer_t  lex_st;
    intptr_t lex;
    expr_t   *expr;

    test_clr_line();
    test_set_line("a = b = c\n");

    CU_ASSERT(0 != (lex = lex_init(&lex_st, test_get_line)));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_ASSIGN);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID);
    CU_ASSERT(L_(expr) && 0 == strcmp(TEXT(L_(expr)), "a"));

    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_ASSIGN);
    CU_ASSERT(L_(R_(expr)) && ast_expr_type(L_(R_(expr))) == EXPR_ID);
    CU_ASSERT(L_(R_(expr)) && 0 == strcmp(TEXT(L_(R_(expr))), "b"));
    CU_ASSERT(R_(R_(expr)) && ast_expr_type(R_(R_(expr))) == EXPR_ID);
    CU_ASSERT(R_(R_(expr)) && 0 == strcmp(TEXT(R_(R_(expr))), "c"));

    ast_expr_relase(expr);
}

static void test_expr_comma(void)
{
    lexer_t  lex_st;
    intptr_t lex;
    expr_t   *expr;

    test_clr_line();
    test_set_line("a , b , c\n");

    CU_ASSERT(0 != (lex = lex_init(&lex_st, test_get_line)));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(lex, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_COMMA);
    CU_ASSERT_FATAL(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID);
    CU_ASSERT(L_(expr) && 0 == strcmp(TEXT(L_(expr)), "a"));

    CU_ASSERT_FATAL(R_(expr) && ast_expr_type(R_(expr)) == EXPR_COMMA);
    CU_ASSERT(L_(R_(expr)) && ast_expr_type(L_(R_(expr))) == EXPR_ID);
    CU_ASSERT(L_(R_(expr)) && 0 == strcmp(TEXT(L_(R_(expr))), "b"));
    CU_ASSERT(R_(R_(expr)) && ast_expr_type(R_(R_(expr))) == EXPR_ID);
    CU_ASSERT(R_(R_(expr)) && 0 == strcmp(TEXT(R_(R_(expr))), "c"));

    ast_expr_relase(expr);
}

CU_pSuite test_lang_parse_entry()
{
    CU_pSuite suite = CU_add_suite("lang parse", test_setup, test_clean);

    if (suite) {
        CU_add_test(suite, "parse expression factor",   test_expr_factor);
        CU_add_test(suite, "parse expression primary",  test_expr_primary);
        CU_add_test(suite, "parse expression unary",    test_expr_unary);
        CU_add_test(suite, "parse expression multi",    test_expr_mul);
        CU_add_test(suite, "parse expression add",      test_expr_add);
        CU_add_test(suite, "parse expression shift",    test_expr_shift);
        CU_add_test(suite, "parse expression test",     test_expr_test);
        CU_add_test(suite, "parse expression logic",    test_expr_logic);
        CU_add_test(suite, "parse expression ternary",  test_expr_ternary);
        CU_add_test(suite, "parse expression assign",   test_expr_assign);
        CU_add_test(suite, "parse expression comma",    test_expr_comma);
    }

    return suite;
}

