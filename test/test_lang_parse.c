
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
#define STMT ast_expr_stmt

#define LEX_BUF_SIZE    128
#define PSR_BUF_SIZE    8192

static uint8_t lex_buf [LEX_BUF_SIZE];
static uint8_t heap_buf[PSR_BUF_SIZE];

static heap_t heap;

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
    lexer_t  lex;
    parser_t psr;
    expr_t   *expr;

    test_clr_line();
    test_set_line(" [a, b] \n");
    test_set_line("true false\n");
    test_set_line("undefined null NaN\n");
    test_set_line("'null' hello 12345\n");
    test_set_line(" (a + b) \n");
    test_set_line(" {a: 1, b: 2} \n");

    CU_ASSERT(0 == lex_init(&lex, lex_buf, LEX_BUF_SIZE, test_get_line));
    heap_init(&heap, heap_buf, PSR_BUF_SIZE);
    parse_init(&psr, &lex, &heap);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_ARRAY);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_COMMA);
    CU_ASSERT(L_(L_(expr)) && ast_expr_type(L_(L_((expr)))) == EXPR_ID && !strcmp("a", TEXT(L_(L_(expr)))));
    CU_ASSERT(R_(L_(expr)) && ast_expr_type(R_(L_((expr)))) == EXPR_ID && !strcmp("b", TEXT(R_(L_(expr)))));

    CU_ASSERT(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_TRUE);

    CU_ASSERT(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_FALSE);

    CU_ASSERT(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_UND);

    CU_ASSERT(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_NULL);

    CU_ASSERT(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_NAN);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_STRING && !strcmp("null", TEXT(expr)));

    CU_ASSERT(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_ID && !strcmp("hello", TEXT(expr)));

    CU_ASSERT(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_NUM && NUMBER(expr) == 12345);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_ADD);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID && !strcmp("a", TEXT(L_(expr))));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_ID && !strcmp("b", TEXT(R_(expr))));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_DICT);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_COMMA);
    CU_ASSERT(L_(L_(expr)) && ast_expr_type(L_(L_((expr)))) == EXPR_PAIR);
    CU_ASSERT(R_(L_(expr)) && ast_expr_type(R_(L_((expr)))) == EXPR_PAIR);
    CU_ASSERT(L_(L_(L_(expr))) && ast_expr_type(L_(L_(L_((expr))))) == EXPR_ID && !strcmp("a", TEXT(L_(L_(L_(expr))))));
    CU_ASSERT(R_(L_(L_(expr))) && ast_expr_type(R_(L_(L_((expr))))) == EXPR_NUM && 1 == NUMBER(R_(L_(L_(expr)))));
    CU_ASSERT(L_(R_(L_(expr))) && ast_expr_type(L_(R_(L_((expr))))) == EXPR_ID && !strcmp("b", TEXT(L_(R_(L_(expr))))));
    CU_ASSERT(R_(R_(L_(expr))) && ast_expr_type(R_(R_(L_((expr))))) == EXPR_NUM && 2 == NUMBER(R_(R_(L_(expr)))));
}

static void test_expr_primary(void)
{
    lexer_t  lex;
    parser_t psr;
    expr_t   *expr;

    test_clr_line();
    test_set_line("a.b a.b.c\n");
    test_set_line("a[b] a[b][c]\n");
    test_set_line("a(b) a(b)(c)\n");
    test_set_line("f(a)[b].c[d](e)\n");

    CU_ASSERT(0 == lex_init(&lex, lex_buf, LEX_BUF_SIZE, test_get_line));
    heap_init(&heap, heap_buf, PSR_BUF_SIZE);
    parse_init(&psr, &lex, &heap);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_PROP);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID && !strcmp("a", TEXT(L_(expr))));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_ID && !strcmp("b", TEXT(R_(expr))));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_PROP);
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_ID && !strcmp("c", TEXT(R_(expr))));
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_PROP);
    CU_ASSERT(L_(L_(expr)) && ast_expr_type(L_(L_(expr))) == EXPR_ID && !strcmp("a", TEXT(L_(L_(expr)))));
    CU_ASSERT(R_(L_(expr)) && ast_expr_type(R_(L_(expr))) == EXPR_ID && !strcmp("b", TEXT(R_(L_(expr)))));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_ELEM);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID && !strcmp("a", TEXT(L_(expr))));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_ID && !strcmp("b", TEXT(R_(expr))));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_ELEM);
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_ID && !strcmp("c", TEXT(R_(expr))));
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ELEM);
    CU_ASSERT(L_(L_(expr)) && ast_expr_type(L_(L_(expr))) == EXPR_ID && !strcmp("a", TEXT(L_(L_(expr)))));
    CU_ASSERT(R_(L_(expr)) && ast_expr_type(R_(L_(expr))) == EXPR_ID && !strcmp("b", TEXT(R_(L_(expr)))));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_CALL);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID && !strcmp("a", TEXT(L_(expr))));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_ID && !strcmp("b", TEXT(R_(expr))));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_CALL);
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_ID && !strcmp("c", TEXT(R_(expr))));
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_CALL);
    CU_ASSERT(L_(L_(expr)) && ast_expr_type(L_(L_(expr))) == EXPR_ID && !strcmp("a", TEXT(L_(L_(expr)))));
    CU_ASSERT(R_(L_(expr)) && ast_expr_type(R_(L_(expr))) == EXPR_ID && !strcmp("b", TEXT(R_(L_(expr)))));

    // f(a)[b].c[d](e)
    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_CALL);
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_ID && !strcmp("e", TEXT(R_(expr))));
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ELEM);
    CU_ASSERT(R_(L_(expr)) && ast_expr_type(R_(L_(expr))) == EXPR_ID && !strcmp("d", TEXT(R_(L_(expr)))));
    CU_ASSERT(L_(L_(expr)) && ast_expr_type(L_(L_(expr))) == EXPR_PROP);
    CU_ASSERT(R_(L_(L_(expr))) && ast_expr_type(R_(L_(L_(expr)))) == EXPR_ID && !strcmp("c", TEXT(R_(L_(L_(expr))))));
    CU_ASSERT(L_(L_(L_(expr))) && ast_expr_type(L_(L_(L_(expr)))) == EXPR_ELEM);
    CU_ASSERT(R_(L_(L_(L_(expr)))) && ast_expr_type(R_(L_(L_(L_(expr))))) == EXPR_ID && !strcmp("b", TEXT(R_(L_(L_(L_(expr)))))));
    CU_ASSERT(L_(L_(L_(L_(expr)))) && ast_expr_type(L_(L_(L_(L_(expr))))) == EXPR_CALL);
    CU_ASSERT(R_(L_(L_(L_(L_(expr))))) && ast_expr_type(R_(L_(L_(L_(L_(expr)))))) == EXPR_ID && !strcmp("a", TEXT(R_(L_(L_(L_(L_(expr))))))));
    CU_ASSERT(L_(L_(L_(L_(L_(expr))))) && ast_expr_type(L_(L_(L_(L_(L_(expr)))))) == EXPR_ID && !strcmp("f", TEXT(L_(L_(L_(L_(L_(expr))))))));
}

static void test_expr_unary(void)
{
    lexer_t  lex;
    parser_t psr;
    expr_t   *expr;

    test_clr_line();
    test_set_line("-a ~b !c !!d\n");

    CU_ASSERT(0 == lex_init(&lex, lex_buf, LEX_BUF_SIZE, test_get_line));
    heap_init(&heap, heap_buf, PSR_BUF_SIZE);
    parse_init(&psr, &lex, &heap);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_NEG);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID && !strcmp("a", TEXT(L_(expr))));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_NOT);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID && !strcmp("b", TEXT(L_(expr))));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_LOGIC_NOT);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID && !strcmp("c", TEXT(L_(expr))));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_LOGIC_NOT);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_LOGIC_NOT);
    CU_ASSERT(L_(L_(expr)) && ast_expr_type(L_(L_(expr))) == EXPR_ID
                && !strcmp("d", TEXT(L_(L_(expr)))));
}

static void test_expr_mul(void)
{
    lexer_t  lex;
    parser_t psr;
    expr_t   *expr;

    test_clr_line();
    test_set_line("a*b 1/2 c%2\n");

    CU_ASSERT(0 == lex_init(&lex, lex_buf, LEX_BUF_SIZE, test_get_line));
    heap_init(&heap, heap_buf, PSR_BUF_SIZE);
    parse_init(&psr, &lex, &heap);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_MUL);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID && !strcmp("a", TEXT(L_(expr))));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_ID && !strcmp("b", TEXT(R_(expr))));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_DIV);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_NUM && 1 == NUMBER(L_(expr)));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_NUM && 2 == NUMBER(R_(expr)));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_MOD);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID && !strcmp("c", TEXT(L_(expr))));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_NUM && 2 == NUMBER(R_(expr)));
}

static void test_expr_add(void)
{
    lexer_t  lex;
    parser_t psr;
    expr_t   *expr;

    test_clr_line();
    test_set_line("a+b 1-2 c+1-2*f\n");

    CU_ASSERT(0 == lex_init(&lex, lex_buf, LEX_BUF_SIZE, test_get_line));
    heap_init(&heap, heap_buf, PSR_BUF_SIZE);
    parse_init(&psr, &lex, &heap);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_ADD);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID && !strcmp("a", TEXT(L_(expr))));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_ID && !strcmp("b", TEXT(R_(expr))));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_SUB);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_NUM && 1 == NUMBER(L_(expr)));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_NUM && 2 == NUMBER(R_(expr)));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_SUB);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ADD);
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_MUL);
    CU_ASSERT(L_(L_(expr)) && ast_expr_type(L_(L_(expr))) == EXPR_ID);
    CU_ASSERT(L_(L_(expr)) && 0 == strcmp(TEXT(L_(L_(expr))), "c"));
    CU_ASSERT(R_(L_(expr)) && NUMBER(R_(L_(expr))) == 1);
    CU_ASSERT(L_(R_(expr)) && NUMBER(L_(R_(expr))) == 2);
    CU_ASSERT(R_(R_(expr)) && 0 == strcmp(TEXT(R_(R_(expr))), "f"));
}

static void test_expr_shift(void)
{
    lexer_t  lex;
    parser_t psr;
    expr_t   *expr;

    test_clr_line();
    test_set_line("1<<1 1>>2 1<<1+2 1+2>>1\n");

    CU_ASSERT(0 == lex_init(&lex, lex_buf, LEX_BUF_SIZE, test_get_line));
    heap_init(&heap, heap_buf, PSR_BUF_SIZE);
    parse_init(&psr, &lex, &heap);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_LSHIFT);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_NUM && 1 == NUMBER(L_(expr)));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_NUM && 1 == NUMBER(R_(expr)));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_RSHIFT);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_NUM && 1 == NUMBER(L_(expr)));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_NUM && 2 == NUMBER(R_(expr)));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_LSHIFT);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_NUM && NUMBER(L_(expr)) == 1);
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_ADD);
    CU_ASSERT(L_(R_(expr)) && NUMBER(L_(R_(expr))) == 1);
    CU_ASSERT(R_(R_(expr)) && NUMBER(R_(R_(expr))) == 2);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_RSHIFT);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ADD);
    CU_ASSERT(L_(L_(expr)) && NUMBER(L_(L_(expr))) == 1);
    CU_ASSERT(R_(L_(expr)) && NUMBER(R_(L_(expr))) == 2);
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_NUM && NUMBER(R_(expr)) == 1);
}

static void test_expr_test(void)
{
    lexer_t  lex;
    parser_t psr;
    expr_t   *expr;

    test_clr_line();
    test_set_line("1>1 2<2 3>=3 4<=4 5!=5 6==6 a in b\n");

    CU_ASSERT(0 == lex_init(&lex, lex_buf, LEX_BUF_SIZE, test_get_line));
    heap_init(&heap, heap_buf, PSR_BUF_SIZE);
    parse_init(&psr, &lex, &heap);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_TGT);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_NUM && 1 == NUMBER(L_(expr)));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_NUM && 1 == NUMBER(R_(expr)));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_TLT);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_NUM && 2 == NUMBER(L_(expr)));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_NUM && 2 == NUMBER(R_(expr)));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_TGE);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_NUM && 3 == NUMBER(L_(expr)));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_NUM && 3 == NUMBER(R_(expr)));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_TLE);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_NUM && 4 == NUMBER(L_(expr)));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_NUM && 4 == NUMBER(R_(expr)));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_TNE);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_NUM && 5 == NUMBER(L_(expr)));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_NUM && 5 == NUMBER(R_(expr)));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_TEQ);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_NUM && 6 == NUMBER(L_(expr)));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_NUM && 6 == NUMBER(R_(expr)));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_TIN);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID && !strcmp("a", TEXT(L_(expr))));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_ID && !strcmp("b", TEXT(R_(expr))));
}

static void test_expr_logic(void)
{
    lexer_t  lex;
    parser_t psr;
    expr_t   *expr;

    test_clr_line();
    test_set_line("a&&b||c&&d\n");

    CU_ASSERT(0 == lex_init(&lex, lex_buf, LEX_BUF_SIZE, test_get_line));
    heap_init(&heap, heap_buf, PSR_BUF_SIZE);
    parse_init(&psr, &lex, &heap);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_LOGIC_OR);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_LOGIC_AND);
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_LOGIC_AND);
    CU_ASSERT(L_(L_(expr)) && ast_expr_type(L_(L_(expr))) == EXPR_ID);
    CU_ASSERT(L_(L_(expr)) && 0 == strcmp(TEXT(L_(L_(expr))), "a"));
    CU_ASSERT(R_(L_(expr)) && ast_expr_type(R_(L_(expr))) == EXPR_ID);
    CU_ASSERT(R_(L_(expr)) && 0 == strcmp(TEXT(R_(L_(expr))), "b"));
    CU_ASSERT(L_(R_(expr)) && ast_expr_type(L_(R_(expr))) == EXPR_ID);
    CU_ASSERT(L_(R_(expr)) && 0 == strcmp(TEXT(L_(R_(expr))), "c"));
    CU_ASSERT(R_(R_(expr)) && ast_expr_type(R_(R_(expr))) == EXPR_ID);
    CU_ASSERT(R_(R_(expr)) && 0 == strcmp(TEXT(R_(R_(expr))), "d"));

}

static void test_expr_ternary(void)
{
    lexer_t  lex;
    parser_t psr;
    expr_t   *expr;

    test_clr_line();
    test_set_line("a ? b: c\n");

    CU_ASSERT(0 == lex_init(&lex, lex_buf, LEX_BUF_SIZE, test_get_line));
    heap_init(&heap, heap_buf, PSR_BUF_SIZE);
    parse_init(&psr, &lex, &heap);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_TERNARY);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID);
    CU_ASSERT(L_(expr) && 0 == strcmp(TEXT(L_(expr)), "a"));

    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_PAIR);
    CU_ASSERT(L_(R_(expr)) && ast_expr_type(L_(R_(expr))) == EXPR_ID);
    CU_ASSERT(L_(R_(expr)) && 0 == strcmp(TEXT(L_(R_(expr))), "b"));
    CU_ASSERT(R_(R_(expr)) && ast_expr_type(R_(R_(expr))) == EXPR_ID);
    CU_ASSERT(R_(R_(expr)) && 0 == strcmp(TEXT(R_(R_(expr))), "c"));

}

static void test_expr_assign(void)
{
    lexer_t  lex;
    parser_t psr;
    expr_t   *expr;

    test_clr_line();
    test_set_line("a = b = c\n");

    CU_ASSERT(0 == lex_init(&lex, lex_buf, LEX_BUF_SIZE, test_get_line));
    heap_init(&heap, heap_buf, PSR_BUF_SIZE);
    parse_init(&psr, &lex, &heap);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_ASSIGN);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID);
    CU_ASSERT(L_(expr) && 0 == strcmp(TEXT(L_(expr)), "a"));

    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_ASSIGN);
    CU_ASSERT(L_(R_(expr)) && ast_expr_type(L_(R_(expr))) == EXPR_ID);
    CU_ASSERT(L_(R_(expr)) && 0 == strcmp(TEXT(L_(R_(expr))), "b"));
    CU_ASSERT(R_(R_(expr)) && ast_expr_type(R_(R_(expr))) == EXPR_ID);
    CU_ASSERT(R_(R_(expr)) && 0 == strcmp(TEXT(R_(R_(expr))), "c"));

}

static void test_expr_comma(void)
{
    lexer_t  lex;
    parser_t psr;
    expr_t   *expr;

    test_clr_line();
    test_set_line("a , b , c\n");

    CU_ASSERT(0 == lex_init(&lex, lex_buf, LEX_BUF_SIZE, test_get_line));
    heap_init(&heap, heap_buf, PSR_BUF_SIZE);
    parse_init(&psr, &lex, &heap);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_COMMA);
    CU_ASSERT_FATAL(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID);
    CU_ASSERT(L_(expr) && 0 == strcmp(TEXT(L_(expr)), "a"));

    CU_ASSERT_FATAL(R_(expr) && ast_expr_type(R_(expr)) == EXPR_COMMA);
    CU_ASSERT(L_(R_(expr)) && ast_expr_type(L_(R_(expr))) == EXPR_ID);
    CU_ASSERT(L_(R_(expr)) && 0 == strcmp(TEXT(L_(R_(expr))), "b"));
    CU_ASSERT(R_(R_(expr)) && ast_expr_type(R_(R_(expr))) == EXPR_ID);
    CU_ASSERT(R_(R_(expr)) && 0 == strcmp(TEXT(R_(R_(expr))), "c"));

}

static void test_expr_funcdef(void)
{
    lexer_t  lex;
    parser_t psr;
    expr_t   *expr;

    test_clr_line();
    test_set_line("def (x) return x + b\n");
    test_set_line("def f() {x = x + 1 return x * 2}\n");
    test_set_line("def () 100\n");
    test_set_line("def f(x) {x = x + 1; return x * 2}\n");
    test_set_line("def () {}\n");

    CU_ASSERT(0 == lex_init(&lex, lex_buf, LEX_BUF_SIZE, test_get_line));
    heap_init(&heap, heap_buf, PSR_BUF_SIZE);
    parse_init(&psr, &lex, &heap);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_FUNCDEF);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_FUNCHEAD);
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_FUNCPROC);
    CU_ASSERT(!L_(L_(expr)));
    CU_ASSERT(R_(L_(expr)) && ast_expr_type(R_(L_(expr))) == EXPR_ID);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_FUNCDEF);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_FUNCHEAD);
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_FUNCPROC);
    CU_ASSERT(L_(L_(expr)) && ast_expr_type(L_(L_(expr))) == EXPR_ID);
    CU_ASSERT(!R_(L_(expr)));

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_FUNCDEF);
    CU_ASSERT(!L_(expr));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_FUNCPROC);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_FUNCDEF);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_FUNCHEAD);
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_FUNCPROC);
    CU_ASSERT(L_(L_(expr)) && ast_expr_type(L_(L_(expr))) == EXPR_ID);
    CU_ASSERT(R_(L_(expr)) && ast_expr_type(R_(L_(expr))) == EXPR_ID);
    CU_ASSERT(STMT(R_(expr)) && STMT(R_(expr))->next);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_FUNCDEF);
    CU_ASSERT(!L_(expr));
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_FUNCPROC);
}

static void test_expr_funcall(void)
{
    lexer_t  lex;
    parser_t psr;
    expr_t   *expr;

    test_clr_line();
    test_set_line("a()\n");
    test_set_line("a ()\n");
    test_set_line("a (b)\n");
    test_set_line("a (b, c)\n");

    CU_ASSERT(0 == lex_init(&lex, lex_buf, LEX_BUF_SIZE, test_get_line));
    heap_init(&heap, heap_buf, PSR_BUF_SIZE);
    parse_init(&psr, &lex, &heap);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_CALL);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID);
    CU_ASSERT(R_(expr) == NULL);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_CALL);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID);
    CU_ASSERT(R_(expr) == NULL);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_CALL);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID);
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_ID);

    CU_ASSERT_FATAL(0 != (expr = parse_expr(&psr, NULL, NULL)));
    CU_ASSERT(ast_expr_type(expr) == EXPR_CALL);
    CU_ASSERT(L_(expr) && ast_expr_type(L_(expr)) == EXPR_ID);
    CU_ASSERT(R_(expr) && ast_expr_type(R_(expr)) == EXPR_COMMA);
}

static void test_stmt_simple(void)
{
    lexer_t  lex;
    parser_t psr;
    stmt_t   *stmt;

    test_clr_line();
    test_set_line("a + b;\n");
    test_set_line("a - b\n");
    test_set_line("break\n");
    test_set_line("continue\n");
    test_set_line("var a = 1, b = c = 0\n");
    test_set_line("return a + b\n");
    test_set_line("return;\n");

    CU_ASSERT(0 == lex_init(&lex, lex_buf, LEX_BUF_SIZE, test_get_line));
    heap_init(&heap, heap_buf, PSR_BUF_SIZE);
    parse_init(&psr, &lex, &heap);

    // expression
    CU_ASSERT_FATAL(0 != (stmt = parse_stmt(&psr, NULL, NULL)));
    CU_ASSERT(stmt->type == STMT_EXPR);
    CU_ASSERT(stmt->expr->type == EXPR_ADD);

    CU_ASSERT_FATAL(0 != (stmt = parse_stmt(&psr, NULL, NULL)));
    CU_ASSERT(stmt->type == STMT_EXPR);
    CU_ASSERT(stmt->expr->type == EXPR_SUB);

    // break
    CU_ASSERT_FATAL(0 != (stmt = parse_stmt(&psr, NULL, NULL)));
    CU_ASSERT(stmt->type == STMT_BREAK);
    CU_ASSERT(!stmt->expr);

    // continue
    CU_ASSERT_FATAL(0 != (stmt = parse_stmt(&psr, NULL, NULL)));
    CU_ASSERT(stmt->type == STMT_CONTINUE);
    CU_ASSERT(!stmt->expr);

    // var
    CU_ASSERT_FATAL(0 != (stmt = parse_stmt(&psr, NULL, NULL)));
    CU_ASSERT(stmt->type == STMT_VAR);
    CU_ASSERT(stmt->expr->type == EXPR_COMMA);

    // return
    CU_ASSERT_FATAL(0 != (stmt = parse_stmt(&psr, NULL, NULL)));
    CU_ASSERT(stmt->type == STMT_RET);
    CU_ASSERT(stmt->expr->type == EXPR_ADD);

    CU_ASSERT_FATAL(0 != (stmt = parse_stmt(&psr, NULL, NULL)));
    CU_ASSERT(stmt->type == STMT_RET);
    CU_ASSERT(!stmt->expr);
}

static void test_stmt_if(void)
{
    lexer_t  lex;
    parser_t psr;
    stmt_t   *stmt;

    test_clr_line();
    test_set_line("if (a + b)\n");
    test_set_line("   a = a - b\n");
    test_set_line("else {\n");
    test_set_line("   a = b - a\n");
    test_set_line("   b = b - a\n");
    test_set_line("}\n");

    CU_ASSERT(0 == lex_init(&lex, lex_buf, LEX_BUF_SIZE, test_get_line));
    heap_init(&heap, heap_buf, PSR_BUF_SIZE);
    parse_init(&psr, &lex, &heap);

    // expression
    CU_ASSERT_FATAL(0 != (stmt = parse_stmt(&psr, NULL, NULL)));
    CU_ASSERT(stmt->type == STMT_IF);
    CU_ASSERT(stmt->expr->type == EXPR_ADD);
    CU_ASSERT(stmt->block != NULL);
    CU_ASSERT(stmt->other != NULL);
}

static void test_stmt_while(void)
{
    lexer_t  lex;
    parser_t psr;
    stmt_t   *stmt;

    test_clr_line();
    test_set_line("while (a > b) {\n");
    test_set_line("   a = a - 1\n");
    test_set_line("   b = b + 1\n");
    test_set_line("}\n");

    CU_ASSERT(0 == lex_init(&lex, lex_buf, LEX_BUF_SIZE, test_get_line));
    heap_init(&heap, heap_buf, PSR_BUF_SIZE);
    parse_init(&psr, &lex, &heap);

    // expression
    CU_ASSERT_FATAL(0 != (stmt = parse_stmt(&psr, NULL, NULL)));
    CU_ASSERT(stmt->type == STMT_WHILE);
    CU_ASSERT(stmt->expr->type == EXPR_TGT);
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
        CU_add_test(suite, "parse expression funcdef",  test_expr_funcdef);
        CU_add_test(suite, "parse expression funcall",  test_expr_funcall);

        CU_add_test(suite, "parse statements simple",   test_stmt_simple);
        CU_add_test(suite, "parse statements if",       test_stmt_if);
        CU_add_test(suite, "parse statements while",    test_stmt_while);
    }

    return suite;
}

