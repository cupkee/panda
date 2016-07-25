

#ifndef __LANG_BCODE_INC__
#define __LANG_BCODE_INC__

#include "config.h"

typedef enum bcode_t {
    BC_STOP = 0,
    BC_PASS = 1,

    BC_JMP,
    BC_SJMP,
    BC_JMP_POP,
    BC_SJMP_POP,

    BC_JMP_T,
    BC_SJMP_T,
    BC_JMP_T_POP,
    BC_SJMP_T_POP,

    BC_JMP_F,
    BC_SJMP_F,
    BC_JMP_F_POP,
    BC_SJMP_F_POP,

    BC_PUSH_UND,
    BC_PUSH_NAN,
    BC_PUSH_ZERO,
    BC_PUSH_TRUE,
    BC_PUSH_FALSE,
    BC_PUSH_NUM,
    BC_PUSH_STR,

    BC_POP,
    BC_POP_RESULT,
    BC_POP_VARIABLE,

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
    BC_LAND,
    BC_LOR,

    BC_TEQ,
    BC_TNE,
    BC_TGT,
    BC_TGE,
    BC_TLT,
    BC_TLE,
    BC_TIN,
} bcode_t;


#endif /* __LANG_BCODE_INC__ */

