
#include <stdio.h>
#include <string.h>

#include "cunit/CUnit.h"
#include "cunit/CUnit_Basic.h"

#include "test_util.h"
#include "lang/lex.h"

static int test_setup()
{
    return 0;
}

static int test_clean()
{
    return 0;
}

static uint8_t memory[8192];

static void test_common(void)
{
    lexer_t lex_st;
    token_t tok;
    intptr_t lex;

    test_clr_line();
    test_set_line(" \t    \r \n");
    test_set_line("# comments 1\r\n");
    test_set_line("+-*/%\n");
    test_set_line("// comments 2\r\n");
    test_set_line("+= -= *= /= %= &= |= ^= ~= >>= <<= >> << && ||\n");
    test_set_line("' bbbb\" ' \" abcd' &$|!\" \n");
    test_set_line("12345 09876\n");
    test_set_line("/* comments 3\r\n comments 3 continue*/");
    test_set_line("abc a12 _11 a_b _a_ $1 $_a \n");
    test_set_line("undefined null NaN true false var def return while break continue in if elif else\n");

    CU_ASSERT(0 != (lex = lex_init(&lex_st, memory, 8192, test_get_line)));

    CU_ASSERT(lex_match(lex, '+'));
    CU_ASSERT(lex_match(lex, '-'));
    CU_ASSERT(lex_match(lex, '*'));
    CU_ASSERT(lex_match(lex, '/'));
    CU_ASSERT(lex_match(lex, '%'));

    CU_ASSERT(lex_match(lex, TOK_ADDASSIGN));
    CU_ASSERT(lex_match(lex, TOK_SUBASSIGN));
    CU_ASSERT(lex_match(lex, TOK_MULASSIGN));
    CU_ASSERT(lex_match(lex, TOK_DIVASSIGN));
    CU_ASSERT(lex_match(lex, TOK_MODASSIGN));
    CU_ASSERT(lex_match(lex, TOK_ANDASSIGN));
    CU_ASSERT(lex_match(lex, TOK_ORASSIGN));
    CU_ASSERT(lex_match(lex, TOK_XORASSIGN));
    CU_ASSERT(lex_match(lex, TOK_NOTASSIGN));
    CU_ASSERT(lex_match(lex, TOK_RSHIFTASSIGN));
    CU_ASSERT(lex_match(lex, TOK_LSHIFTASSIGN));
    CU_ASSERT(lex_match(lex, TOK_RSHIFT));
    CU_ASSERT(lex_match(lex, TOK_LSHIFT));
    CU_ASSERT(lex_match(lex, TOK_LOGICAND));
    CU_ASSERT(lex_match(lex, TOK_LOGICOR));

    CU_ASSERT(TOK_STR == lex_token(lex, &tok) && 0 == strcmp(tok.text, " bbbb\" "));
    CU_ASSERT(lex_match(lex, TOK_STR));
    CU_ASSERT(TOK_STR == lex_token(lex, &tok) && 0 == strcmp(tok.text, " abcd' &$|!"));
    CU_ASSERT(lex_match(lex, TOK_STR));

    CU_ASSERT(TOK_NUM == lex_token(lex, &tok) && 0 == strcmp(tok.text, "12345") && tok.value == 12345);
    CU_ASSERT(lex_match(lex, TOK_NUM));
    CU_ASSERT(TOK_NUM == lex_token(lex, &tok) && 0 == strcmp(tok.text, "09876") && tok.value == 9876);
    CU_ASSERT(lex_match(lex, TOK_NUM));

    CU_ASSERT(TOK_ID == lex_token(lex, &tok) && 0 == strcmp(tok.text, "abc"));
    CU_ASSERT(lex_match(lex, TOK_ID));
    CU_ASSERT(TOK_ID == lex_token(lex, &tok) && 0 == strcmp(tok.text, "a12"));
    CU_ASSERT(lex_match(lex, TOK_ID));
    CU_ASSERT(TOK_ID == lex_token(lex, &tok) && 0 == strcmp(tok.text, "_11"));
    CU_ASSERT(lex_match(lex, TOK_ID));
    CU_ASSERT(TOK_ID == lex_token(lex, &tok) && 0 == strcmp(tok.text, "a_b"));
    CU_ASSERT(lex_match(lex, TOK_ID));
    CU_ASSERT(TOK_ID == lex_token(lex, &tok) && 0 == strcmp(tok.text, "_a_"));
    CU_ASSERT(lex_match(lex, TOK_ID));
    CU_ASSERT(TOK_ID == lex_token(lex, &tok) && 0 == strcmp(tok.text, "$1"));
    CU_ASSERT(lex_match(lex, TOK_ID));
    CU_ASSERT(TOK_ID == lex_token(lex, &tok) && 0 == strcmp(tok.text, "$_a"));
    CU_ASSERT(lex_match(lex, TOK_ID));

    CU_ASSERT(lex_match(lex, TOK_UND));
    CU_ASSERT(lex_match(lex, TOK_NULL));
    CU_ASSERT(lex_match(lex, TOK_NAN));
    CU_ASSERT(lex_match(lex, TOK_TRUE));
    CU_ASSERT(lex_match(lex, TOK_FALSE));
    CU_ASSERT(lex_match(lex, TOK_VAR));
    CU_ASSERT(lex_match(lex, TOK_DEF));
    CU_ASSERT(lex_match(lex, TOK_RET));
    CU_ASSERT(lex_match(lex, TOK_WHILE));
    CU_ASSERT(lex_match(lex, TOK_BREAK));
    CU_ASSERT(lex_match(lex, TOK_CONTINUE));
    CU_ASSERT(lex_match(lex, TOK_IN));
    CU_ASSERT(lex_match(lex, TOK_IF));
    CU_ASSERT(lex_match(lex, TOK_ELIF));
    CU_ASSERT(lex_match(lex, TOK_ELSE));

    CU_ASSERT(0 == lex_deinit(&lex_st));
}

CU_pSuite test_lang_lex_entry()
{
    CU_pSuite suite = CU_add_suite("lang lex", test_setup, test_clean);

    if (suite) {
        CU_add_test(suite, "common", test_common);
    }

    return suite;
}

