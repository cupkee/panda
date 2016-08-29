#include "bcode.h"

int bcode_parse(uint8_t *code, int *offset, const char **name, int *param1, int *param2)
{
    int shift, index;

    if (!name || !param1 || !param2) {
        return -1;
    }

    shift = offset ? *offset : 0;
    switch(code[shift++]) {
    case BC_STOP:       *name = "STOP"; if(offset) *offset = shift; return 0;
    case BC_PASS:       *name = "PASS"; if(offset) *offset = shift; return 0;

    /* Return instruction */
    case BC_RET0:       *name = "RET0"; if(offset) *offset = shift; return 0;
    case BC_RET:        *name = "RET";  if(offset) *offset = shift; return 0;

    /* Jump instruction */
    case BC_SJMP:       *param1 = (int8_t) (code[shift++]);
                        *name = "SJMP"; if(offset) *offset = shift; return 1;

    case BC_JMP:        index = (int8_t) (code[shift++]);
                        *param1 = (index << 8) | (code[shift++]);
                        *name = "JMP"; if(offset) *offset = shift; return 1;

    case BC_SJMP_T:     *param1 = (int8_t) (code[shift++]);
                        *name  = "SJMP_T"; if(offset) *offset = shift; return 1;

    case BC_SJMP_F:     *param1 = (int8_t) (code[shift++]);
                        *name = "SJMP_F"; if(offset) *offset = shift; return 1;

    case BC_JMP_T:      index = (int8_t) (code[shift++]);
                        *param1 = (index << 8) | (code[shift++]);
                        *name  = "JMP_T"; if(offset) *offset = shift; return 1;

    case BC_JMP_F:      index = (int8_t) (code[shift++]);
                        *param1 = (index << 8) | (code[shift++]);
                        *name  = "JMP_F"; if(offset) *offset = shift; return 1;

    case BC_POP_SJMP_T: *param1 = (int8_t) (code[shift++]);
                        *name  = "POP_SJMP_T"; if(offset) *offset = shift; return 1;

    case BC_POP_SJMP_F: *param1 = (int8_t) (code[shift++]);
                        *name  = "POP_SJMP_F"; if(offset) *offset = shift; return 1;

    case BC_POP_JMP_T:  index = (int8_t) (code[shift++]);
                        *param1 = (index << 8) | (code[shift++]);
                        *name  = "POP_JMP_T"; if(offset) *offset = shift; return 1;

    case BC_POP_JMP_F:  index = (int8_t) (code[shift++]);
                        *param1 = (index << 8) | (code[shift++]);
                        *name  = "POP_JMP_F"; if(offset) *offset = shift; return 1;

    case BC_PUSH_UND:   *name = "PUSH_UND"; if(offset) *offset = shift; return 0;
    case BC_PUSH_NAN:   *name = "PUSH_NAN"; if(offset) *offset = shift; return 0;
    case BC_PUSH_TRUE:  *name = "PUSH_TRUE"; if(offset) *offset = shift; return 0;
    case BC_PUSH_FALSE: *name = "PUSH_FALSE"; if(offset) *offset = shift; return 0;
    case BC_PUSH_ZERO:  *name = "PUSH_NUM 0"; if(offset) *offset = shift; return 0;

    case BC_PUSH_NUM:   index = (code[shift++]);
                        *param1 = (index << 8) | (code[shift++]);
                        *name  = "PUSH_NUM"; if(offset) *offset = shift; return 1;

    case BC_PUSH_STR:   index = (code[shift++]);
                        *param1 = (index << 8) | (code[shift++]);
                        *name  = "PUSH_STR"; if(offset) *offset = shift; return 1;

    case BC_PUSH_VAR:   *param1 = (code[shift++]);
                        *param2 = (code[shift++]);
                        *name  = "PUSH_VAR"; if(offset) *offset = shift; return 2;

    case BC_PUSH_REF:*param1 = (code[shift++]);
                        *param2 = (code[shift++]);
                        *name  = "PUSH_REF"; if(offset) *offset = shift; return 2;

    case BC_PUSH_SCRIPT:index = (code[shift++]);
                        *param1 = (index << 8) | (code[shift++]);
                        *name  = "PUSH_SCRIPT"; if(offset) *offset = shift; return 1;

    case BC_PUSH_NATIVE:index = (code[shift++]);
                        *param1 = (index << 8) | (code[shift++]);
                        *name  = "PUSH_NATIVE"; if(offset) *offset = shift; return 1;

    case BC_POP:        *name  = "POP"; if(offset) *offset = shift; return 0;
    case BC_POP_RESULT: *name  = "POP_RESULT"; if(offset) *offset = shift; return 0;

    case BC_NEG:        *name  = "NEG"; if(offset) *offset = shift; return 0;
    case BC_NOT:        *name  = "NOT"; if(offset) *offset = shift; return 0;
    case BC_LOGIC_NOT:  *name  = "LOGIC_NOT"; if(offset) *offset = shift; return 0;

    case BC_MUL:        *name  = "MUL"; if(offset) *offset = shift; return 0;
    case BC_DIV:        *name  = "DIV"; if(offset) *offset = shift; return 0;
    case BC_MOD:        *name  = "MOD"; if(offset) *offset = shift; return 0;
    case BC_ADD:        *name  = "ADD"; if(offset) *offset = shift; return 0;
    case BC_SUB:        *name  = "SUB"; if(offset) *offset = shift; return 0;

    case BC_AAND:       *name = "LOGIC_AND"; if(offset) *offset = shift; return 0;
    case BC_AOR:        *name = "LOGIC_OR"; if(offset) *offset = shift; return 0;
    case BC_AXOR:       *name = "LOGIC_XOR"; if(offset) *offset = shift; return 0;

    case BC_LSHIFT:     *name = "LSHIFT"; if(offset) *offset = shift; return 0;
    case BC_RSHIFT:     *name = "RSHIFT"; if(offset) *offset = shift; return 0;

    case BC_TEQ:        *name = "TEQ"; if(offset) *offset = shift; return 0;
    case BC_TNE:        *name = "TNE"; if(offset) *offset = shift; return 0;
    case BC_TGT:        *name = "TGT"; if(offset) *offset = shift; return 0;
    case BC_TGE:        *name = "TGE"; if(offset) *offset = shift; return 0;
    case BC_TLT:        *name = "TLT"; if(offset) *offset = shift; return 0;
    case BC_TLE:        *name = "TLE"; if(offset) *offset = shift; return 0;

    case BC_TIN:        *name = "TIN"; if(offset) *offset = shift; return 0;

    case BC_ASSIGN:     *name = "ASSING"; if(offset) *offset = shift; return 0;
    case BC_FUNC_CALL:  *param1 = code[shift++];
                        *name = "CALL"; if(offset) *offset = shift; return 1;

    case BC_PROP:       *name = "PROP"; if(offset) *offset = shift; return 0;
    case BC_PROP_METH:  *name = "PROP_METH"; if(offset) *offset = shift; return 0;

    case BC_ELEM:       *name = "ELEM"; if(offset) *offset = shift; return 0;
    case BC_ELEM_METH:  *name = "ELEM_METH"; if(offset) *offset = shift; return 0;

    default:            *name = "UNKNOWN"; if(offset) *offset = shift; return 0;
    }
}

