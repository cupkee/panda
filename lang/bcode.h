

#ifndef __LANG_BCODE_INC__
#define __LANG_BCODE_INC__

#include "config.h"

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

    BC_DICT,

    BC_PROP,
    BC_PROP_METH,
    BC_PROP_SET,

    BC_ELEM,
    BC_ELEM_METH,
    BC_ELEM_SET,

    BC_FUNC_CALL,

    BC_ASSIGN,
    BC_PROP_ASSIGN,
    BC_ELEM_ASSIGN,
} bcode_t;

int bcode_parse(uint8_t *code, int *offset, const char **name, int *param1, int *param2);

#endif /* __LANG_BCODE_INC__ */

