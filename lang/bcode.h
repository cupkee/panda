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


#ifndef __LANG_BCODE_INC__
#define __LANG_BCODE_INC__

#include "def.h"

typedef enum bcode_t {
    BC_STOP = 0,
    BC_PASS = 1,

    BC_RET,
    BC_RET0,

    BC_JMP,
    BC_SJMP,

    BC_JMP_T,
    BC_SJMP_T,
    BC_JMP_F,
    BC_SJMP_F,

    BC_POP_JMP_T,
    BC_POP_SJMP_T,
    BC_POP_JMP_F,
    BC_POP_SJMP_F,
    BC_POP,

    BC_PUSH_UND,
    BC_PUSH_NAN,
    BC_PUSH_ZERO,
    BC_PUSH_TRUE,
    BC_PUSH_FALSE,
    BC_PUSH_NUM,
    BC_PUSH_STR,
    BC_PUSH_VAR,
    BC_PUSH_REF,
    BC_PUSH_SCRIPT,
    BC_PUSH_NATIVE,

    BC_NEG,
    BC_NOT,
    BC_LOGIC_NOT,

    BC_MUL,
    BC_DIV,
    BC_MOD,
    BC_ADD,
    BC_SUB,
    BC_LSHIFT,
    BC_RSHIFT,
    BC_AAND,
    BC_AOR,
    BC_AXOR,

    BC_TEQ,
    BC_TNE,
    BC_TGT,
    BC_TGE,
    BC_TLT,
    BC_TLE,
    BC_TIN,

    BC_PROP,
    BC_PROP_METH,

    BC_ELEM,
    BC_ELEM_METH,

    BC_INC,
    BC_INCP,
    BC_DEC,
    BC_DECP,

    BC_ASSIGN,
    BC_ADD_ASSIGN,
    BC_SUB_ASSIGN,
    BC_MUL_ASSIGN,
    BC_DIV_ASSIGN,
    BC_MOD_ASSIGN,
    BC_AND_ASSIGN,
    BC_OR_ASSIGN,
    BC_XOR_ASSIGN,
    BC_NOT_ASSIGN,
    BC_LSHIFT_ASSIGN,
    BC_RSHIFT_ASSIGN,

    BC_PROP_INC,
    BC_PROP_INCP,
    BC_PROP_DEC,
    BC_PROP_DECP,

    BC_PROP_ASSIGN,
    BC_PROP_ADD_ASSIGN,
    BC_PROP_SUB_ASSIGN,
    BC_PROP_MUL_ASSIGN,
    BC_PROP_DIV_ASSIGN,
    BC_PROP_MOD_ASSIGN,
    BC_PROP_AND_ASSIGN,
    BC_PROP_OR_ASSIGN,
    BC_PROP_XOR_ASSIGN,
    BC_PROP_NOT_ASSIGN,
    BC_PROP_LSHIFT_ASSIGN,
    BC_PROP_RSHIFT_ASSIGN,

    BC_ELEM_INC,
    BC_ELEM_INCP,
    BC_ELEM_DEC,
    BC_ELEM_DECP,

    BC_ELEM_ASSIGN,
    BC_ELEM_ADD_ASSIGN,
    BC_ELEM_SUB_ASSIGN,
    BC_ELEM_MUL_ASSIGN,
    BC_ELEM_DIV_ASSIGN,
    BC_ELEM_MOD_ASSIGN,
    BC_ELEM_AND_ASSIGN,
    BC_ELEM_OR_ASSIGN,
    BC_ELEM_XOR_ASSIGN,
    BC_ELEM_NOT_ASSIGN,
    BC_ELEM_LSHIFT_ASSIGN,
    BC_ELEM_RSHIFT_ASSIGN,

    BC_FUNC_CALL,

    BC_ARRAY,
    BC_DICT,

} bcode_t;

int bcode_parse(const uint8_t *code, int *offset, const char **name, int *param1, int *param2);

#endif /* __LANG_BCODE_INC__ */

