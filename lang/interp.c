
#include "err.h"
#include "val.h"
#include "lex.h"
#include "parse.h"
#include "interp.h"
#include "bcode.h"

#define SHOW(...) printf(__VA_ARGS__)
//#define SHOW(...) //

interp_t *interp_init(interp_t *interp, val_t *stack_ptr, int stack_size)
{
    if (interp) {
        interp->sb = stack_ptr;
        interp->ss = stack_size;
        interp->sp = stack_size;

        interp->error = 0;

        interp->result = NULL;
    }
    return interp;
}

int interp_deinit(interp_t *interp)
{
    return 0;
}

int interp_run(interp_t *interp, env_t *env, module_t *mod)
{
    uint8_t *code = mod->code;
    double  *static_num = mod->nums;

    int pc = 0;
    int index;

    while(!interp->error) {
        SHOW("pc: %d, sp: %d, code: %x\n", pc, interp->sp, code[pc]);
        switch(code[pc++]) {
        case BC_STOP:       SHOW("STOP\n"); goto DO_END;

        /* Jump instruction */
        case BC_SJMP:       index = (int8_t) code[pc++];
                            SHOW("SJMP: %d\n", index);
                            pc += index; break;

        case BC_JMP:        index = (int8_t) code[pc++]; index = (index << 8) | code[pc++];
                            SHOW("JMP: %d\n", index);
                            pc += index; break;

        case BC_SJMP_T:     index = (int8_t) code[pc++];
                            SHOW("SJMP_T: %d\n", index);
                            if (val_is_true(*interp_stack_peek(interp))) {
                                pc += index;
                            }
                            break;

        case BC_SJMP_F:     index = (int8_t) code[pc++];
                            SHOW("SJMP_F: %d\n", index);
                            if (!val_is_true(*interp_stack_peek(interp))) {
                                pc += index;
                            }
                            break;

        case BC_JMP_T:      index = (int8_t) code[pc++]; index = (index << 8) | code[pc++];
                            SHOW("JMP_T: %d\n", index);
                            if (val_is_true(*interp_stack_peek(interp))) {
                                pc += index;
                            }
                            break;
        case BC_JMP_F:      index = (int8_t) code[pc++]; index = (index << 8) | code[pc++];
                            SHOW("JMP_F: %d\n", index);
                            if (!val_is_true(*interp_stack_peek(interp))) {
                                pc += index;
                            }
                            break;
        case BC_SJMP_T_POP: index = (int8_t) code[pc++];
                            SHOW("SJMP_T_POP: %d\n", index);
                            if (val_is_true(*interp_stack_pop(interp))) {
                                pc += index;
                            }
                            break;
        case BC_SJMP_F_POP: index = (int8_t) code[pc++];
                            SHOW("SJMP_F_POP: %d\n", index);
                            if (!val_is_true(*interp_stack_pop(interp))) {
                                pc += index;
                            }
                            break;
        case BC_JMP_T_POP:  index = (int8_t) code[pc++]; index = (index << 8) | code[pc++];
                            SHOW("JMP_T_POP: %d\n", index);
                            if (val_is_true(*interp_stack_pop(interp))) {
                                pc += index;
                            }
                            break;
        case BC_JMP_F_POP:  index = (int8_t) code[pc++]; index = (index << 8) | code[pc++];
                            SHOW("JMP_F_POP: %d(%.4x)\n", index, index);
                            if (!val_is_true(*interp_stack_pop(interp))) {
                                pc += index;
                            }
                            break;

        case BC_PUSH_UND:   SHOW("PUSH_UND\n"); interp_push_undefined(interp); break;
        case BC_PUSH_NAN:   SHOW("PUSH_NAN\n"); interp_push_nan(interp); break;
        case BC_PUSH_TRUE:  SHOW("PUSH_TRUE\n"); interp_push_boolean(interp, 1); break;
        case BC_PUSH_FALSE: SHOW("PUSH_FALSE\n"); interp_push_boolean(interp, 0); break;
        case BC_PUSH_ZERO:  SHOW("PUSH_NUM 0\n"); interp_push_number(interp, 0); break;
        case BC_PUSH_NUM:   index = code[pc++]; index = (index << 8) + code[pc++];
                            SHOW("PUSH_NUM %f\n", static_num[index]);
                            interp_push_number(interp, static_num[index]); break;
        case BC_PUSH_VAR:   index = code[pc++]; SHOW("PUSH_VAR %d\n", index);
                            *(interp_stack_push(interp)) = env->scope->variables[index]; break;
        case BC_PUSH_VAR_REF:
                            index = code[pc++]; SHOW("PUSH_VAR_REF %d\n", index);
                            val_set_reference(interp_stack_push(interp), env->scope->variables + index); break;

        case BC_POP:        SHOW("POP\n"); interp_stack_pop(interp); break;
        case BC_POP_RESULT: SHOW("POP_RESULT\n"); interp->result = interp_stack_pop(interp); break;

        case BC_NEG:        SHOW("NEG\n"); interp_neg_stack(interp); break;
        case BC_NOT:        SHOW("NOT\n"); interp_not_stack(interp); break;
        case BC_LOGIC_NOT:  SHOW("LOGIC_NOT\n"); interp_logic_not_stack(interp); break;

        case BC_MUL:        SHOW("MUL\n"); interp_mul_stack(interp); break;
        case BC_DIV:        SHOW("DIV\n"); interp_div_stack(interp); break;
        case BC_MOD:        SHOW("MOD\n"); interp_mod_stack(interp); break;
        case BC_ADD:        SHOW("ADD\n"); interp_add_stack(interp); break;
        case BC_SUB:        SHOW("SUB\n"); interp_sub_stack(interp); break;

        case BC_AAND:       SHOW("LOGIC_AND\n"); interp_and_stack(interp); break;
        case BC_AOR:        SHOW("LOGIC_OR\n"); interp_or_stack(interp); break;
        case BC_AXOR:       SHOW("LOGIC_XOR\n"); interp_xor_stack(interp); break;

        case BC_LSHIFT:     SHOW("LSHIFT\n"); interp_lshift_stack(interp); break;
        case BC_RSHIFT:     SHOW("RSHIFT\n"); interp_rshift_stack(interp); break;

        case BC_TEQ:        SHOW("TEQ\n"); interp_teq_stack(interp); break;
        case BC_TNE:        SHOW("TNE\n"); interp_tne_stack(interp); break;
        case BC_TGT:        SHOW("TGT\n"); interp_tgt_stack(interp); break;
        case BC_TGE:        SHOW("TGE\n"); interp_tge_stack(interp); break;
        case BC_TLT:        SHOW("TLT\n"); interp_tlt_stack(interp); break;
        case BC_TLE:        SHOW("TLE\n"); interp_tle_stack(interp); break;

        case BC_TIN:        SHOW("TIN\n"); interp_set_error(interp, ERR_InvalidByteCode); break;

        case BC_ASSIGN:     SHOW("ASSING\n"); interp_assign(interp); break;

        default:            interp_set_error(interp, ERR_InvalidByteCode);
        }
    }
DO_END:
    return -interp->error;
}

