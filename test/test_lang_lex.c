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

static void test_common(void)
{
    lexer_t lex;
    token_t tok;
    char *input =  "\
    \t    \r \n\
    # comments 1\r\n\
    +-*/%\n\
    // comments 2\r\n\
    += -= *= /= %= &= |= ^= ~= >>= <<= >> << && ||\n\
    ' bbbb\" ' \" abcd' &$|!\" \n\
    12345 09876\n\
    /* comments 3\r\n comments 3 continue*/\
    abc a12 _11 a_b _a_ $1 $_a \n\
    undefined null NaN true false var def return while break continue in if elif else try catch throw\n";

    CU_ASSERT(0 == lex_init(&lex, input, NULL));

    CU_ASSERT(lex_match(&lex, '+'));
    CU_ASSERT(lex_match(&lex, '-'));
    CU_ASSERT(lex_match(&lex, '*'));
    CU_ASSERT(lex_match(&lex, '/'));
    CU_ASSERT(lex_match(&lex, '%'));

    CU_ASSERT(lex_match(&lex, TOK_ADDASSIGN));
    CU_ASSERT(lex_match(&lex, TOK_SUBASSIGN));
    CU_ASSERT(lex_match(&lex, TOK_MULASSIGN));
    CU_ASSERT(lex_match(&lex, TOK_DIVASSIGN));
    CU_ASSERT(lex_match(&lex, TOK_MODASSIGN));
    CU_ASSERT(lex_match(&lex, TOK_ANDASSIGN));
    CU_ASSERT(lex_match(&lex, TOK_ORASSIGN));
    CU_ASSERT(lex_match(&lex, TOK_XORASSIGN));
    CU_ASSERT(lex_match(&lex, TOK_NOTASSIGN));
    CU_ASSERT(lex_match(&lex, TOK_RSHIFTASSIGN));
    CU_ASSERT(lex_match(&lex, TOK_LSHIFTASSIGN));
    CU_ASSERT(lex_match(&lex, TOK_RSHIFT));
    CU_ASSERT(lex_match(&lex, TOK_LSHIFT));
    CU_ASSERT(lex_match(&lex, TOK_LOGICAND));
    CU_ASSERT(lex_match(&lex, TOK_LOGICOR));

    CU_ASSERT(TOK_STR == lex_token(&lex, &tok) && 0 == strcmp(tok.text, " bbbb\" "));
    CU_ASSERT(lex_match(&lex, TOK_STR));
    CU_ASSERT(TOK_STR == lex_token(&lex, &tok) && 0 == strcmp(tok.text, " abcd' &$|!"));
    CU_ASSERT(lex_match(&lex, TOK_STR));

    CU_ASSERT(TOK_NUM == lex_token(&lex, &tok) && 0 == strcmp(tok.text, "12345"));
    CU_ASSERT(lex_match(&lex, TOK_NUM));
    CU_ASSERT(TOK_NUM == lex_token(&lex, &tok) && 0 == strcmp(tok.text, "09876"));
    CU_ASSERT(lex_match(&lex, TOK_NUM));

    CU_ASSERT(TOK_ID == lex_token(&lex, &tok) && 0 == strcmp(tok.text, "abc"));
    CU_ASSERT(lex_match(&lex, TOK_ID));
    CU_ASSERT(TOK_ID == lex_token(&lex, &tok) && 0 == strcmp(tok.text, "a12"));
    CU_ASSERT(lex_match(&lex, TOK_ID));
    CU_ASSERT(TOK_ID == lex_token(&lex, &tok) && 0 == strcmp(tok.text, "_11"));
    CU_ASSERT(lex_match(&lex, TOK_ID));
    CU_ASSERT(TOK_ID == lex_token(&lex, &tok) && 0 == strcmp(tok.text, "a_b"));
    CU_ASSERT(lex_match(&lex, TOK_ID));
    CU_ASSERT(TOK_ID == lex_token(&lex, &tok) && 0 == strcmp(tok.text, "_a_"));
    CU_ASSERT(lex_match(&lex, TOK_ID));
    CU_ASSERT(TOK_ID == lex_token(&lex, &tok) && 0 == strcmp(tok.text, "$1"));
    CU_ASSERT(lex_match(&lex, TOK_ID));
    CU_ASSERT(TOK_ID == lex_token(&lex, &tok) && 0 == strcmp(tok.text, "$_a"));
    CU_ASSERT(lex_match(&lex, TOK_ID));

    CU_ASSERT(lex_match(&lex, TOK_UND));
    CU_ASSERT(lex_match(&lex, TOK_NULL));
    CU_ASSERT(lex_match(&lex, TOK_NAN));
    CU_ASSERT(lex_match(&lex, TOK_TRUE));
    CU_ASSERT(lex_match(&lex, TOK_FALSE));
    CU_ASSERT(lex_match(&lex, TOK_VAR));
    CU_ASSERT(lex_match(&lex, TOK_DEF));
    CU_ASSERT(lex_match(&lex, TOK_RET));
    CU_ASSERT(lex_match(&lex, TOK_WHILE));
    CU_ASSERT(lex_match(&lex, TOK_BREAK));
    CU_ASSERT(lex_match(&lex, TOK_CONTINUE));
    CU_ASSERT(lex_match(&lex, TOK_IN));
    CU_ASSERT(lex_match(&lex, TOK_IF));
    CU_ASSERT(lex_match(&lex, TOK_ELIF));
    CU_ASSERT(lex_match(&lex, TOK_ELSE));
    CU_ASSERT(lex_match(&lex, TOK_TRY));
    CU_ASSERT(lex_match(&lex, TOK_CATCH));
    CU_ASSERT(lex_match(&lex, TOK_THROW));

    CU_ASSERT(0 == lex_deinit(&lex));
}

static void test_mch_tok(void)
{
    lexer_t lex;

    CU_ASSERT(0 == lex_init(&lex, "++ -- << >> += -= *= /= %= &= |= ^= != == && ||", NULL));

    CU_ASSERT(lex_match(&lex, TOK_INC));
    CU_ASSERT(lex_match(&lex, TOK_DEC));
    CU_ASSERT(lex_match(&lex, TOK_LSHIFT));
    CU_ASSERT(lex_match(&lex, TOK_RSHIFT));
    CU_ASSERT(lex_match(&lex, TOK_ADDASSIGN));
    CU_ASSERT(lex_match(&lex, TOK_SUBASSIGN));
    CU_ASSERT(lex_match(&lex, TOK_MULASSIGN));
    CU_ASSERT(lex_match(&lex, TOK_DIVASSIGN));
    CU_ASSERT(lex_match(&lex, TOK_MODASSIGN));
    CU_ASSERT(lex_match(&lex, TOK_ANDASSIGN));
    CU_ASSERT(lex_match(&lex, TOK_ORASSIGN));
    CU_ASSERT(lex_match(&lex, TOK_XORASSIGN));
    CU_ASSERT(lex_match(&lex, TOK_NE));
    CU_ASSERT(lex_match(&lex, TOK_EQ));
    CU_ASSERT(lex_match(&lex, TOK_LOGICAND));
    CU_ASSERT(lex_match(&lex, TOK_LOGICOR));
}

static void test_floating_number(void)
{
    lexer_t lex;
    token_t tok;

    CU_ASSERT(0 == lex_init(&lex, "1.5 1e-3 123E+12 0.1e2", NULL));
    CU_ASSERT(TOK_NUM == lex_token(&lex, &tok) && 0 == strcmp(tok.text, "1.5"));
    CU_ASSERT(lex_match(&lex, TOK_NUM));

    CU_ASSERT(TOK_NUM == lex_token(&lex, &tok) && 0 == strcmp(tok.text, "1E-3"));
    CU_ASSERT(lex_match(&lex, TOK_NUM));

    CU_ASSERT(TOK_NUM == lex_token(&lex, &tok) && 0 == strcmp(tok.text, "123E12"));
    CU_ASSERT(lex_match(&lex, TOK_NUM));

    CU_ASSERT(TOK_NUM == lex_token(&lex, &tok) && 0 == strcmp(tok.text, "0.1E2"));
    CU_ASSERT(lex_match(&lex, TOK_NUM));
}

static void test_hex_number(void)
{
    lexer_t lex;
    token_t tok;

    CU_ASSERT(0 == lex_init(&lex, "0x10 0xFF 0Xff 0X01", NULL));
    CU_ASSERT(TOK_NUM == lex_token(&lex, &tok) && 0 == strcmp(tok.text, "0x10"));
    CU_ASSERT(lex_match(&lex, TOK_NUM));

    CU_ASSERT(TOK_NUM == lex_token(&lex, &tok) && 0 == strcmp(tok.text, "0xFF"));
    CU_ASSERT(lex_match(&lex, TOK_NUM));

    CU_ASSERT(TOK_NUM == lex_token(&lex, &tok) && 0 == strcmp(tok.text, "0xff"));
    CU_ASSERT(lex_match(&lex, TOK_NUM));

    CU_ASSERT(TOK_NUM == lex_token(&lex, &tok) && 0 == strcmp(tok.text, "0x01"));
    CU_ASSERT(lex_match(&lex, TOK_NUM));
}

CU_pSuite test_lang_lex_entry()
{
    CU_pSuite suite = CU_add_suite("lang lex", test_setup, test_clean);

    if (suite) {
        CU_add_test(suite, "common",            test_common);
        CU_add_test(suite, "multiCh token",     test_mch_tok);
        CU_add_test(suite, "floating number",   test_floating_number);
        CU_add_test(suite, "hex number",   test_floating_number);
    }

    return suite;
}

