
#include "err.h"
#include "val.h"
#include "lex.h"
#include "parse.h"
#include "interp.h"
#include "bcode.h"

interp_t *interp_init(interp_t *interp, val_t *stack_ptr, int stack_size)
{
    if (interp) {
        interp->sb = stack_ptr;
        interp->ss = stack_size;
        interp->sp = stack_size;

        interp->error = 0;
        interp->skip  = 0;

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
        //printf("pc: %d, code: %x\n", pc, code[pc]);
        switch(code[pc++]) {
        case BC_STOP:       goto DO_END;

        /* Jump instruction */
        case BC_SJMP:       index = (int8_t) code[pc++];
                            pc += index; break;
        case BC_JMP:        index = (int8_t) code[pc++]; index = (index << 8) | code[pc++];
                            pc += index; break;
        case BC_SJMP_T:     index = (int8_t) code[pc++];
                            if (val_is_true(*interp_stack_peek(interp))) {
                                pc += index;
                            }
                            break;
        case BC_SJMP_F:     index = (int8_t) code[pc++];
                            if (!val_is_true(*interp_stack_peek(interp))) {
                                pc += index;
                            }
                            break;
        case BC_JMP_T:      index = (int8_t) code[pc++]; index = (index << 8) | code[pc++];
                            if (val_is_true(*interp_stack_peek(interp))) {
                                pc += index;
                            }
                            break;
        case BC_JMP_F:      index = (int8_t) code[pc++]; index = (index << 8) | code[pc++];
                            if (!val_is_true(*interp_stack_peek(interp))) {
                                pc += index;
                            }
                            break;
        case BC_SJMP_T_POP: index = (int8_t) code[pc++];
                            if (val_is_true(*interp_stack_pop(interp))) {
                                pc += index;
                            }
                            break;
        case BC_SJMP_F_POP: index = (int8_t) code[pc++];
                            if (!val_is_true(*interp_stack_pop(interp))) {
                                pc += index;
                            }
                            break;
        case BC_JMP_T_POP:  index = (int8_t) code[pc++]; index = (index << 8) | code[pc++];
                            if (val_is_true(*interp_stack_pop(interp))) {
                                pc += index;
                            }
                            break;
        case BC_JMP_F_POP:  index = (int8_t) code[pc++]; index = (index << 8) | code[pc++];
                            if (!val_is_true(*interp_stack_pop(interp))) {
                                pc += index;
                            }
                            break;

        case BC_PUSH_UND:   interp_push_undefined(interp); break;
        case BC_PUSH_NAN:   interp_push_nan(interp); break;
        case BC_PUSH_TRUE:  interp_push_boolean(interp, 1); break;
        case BC_PUSH_FALSE: interp_push_boolean(interp, 0); break;
        case BC_PUSH_ZERO:  interp_push_number(interp, 0); break;
        case BC_PUSH_NUM:   index = code[pc++];
#ifdef NUM_CODE_2
                            index = (index << 8) + code[pc++];
#endif
                            interp_push_number(interp, static_num[index]); break;

        case BC_POP:        interp_stack_pop(interp); break;
        case BC_POP_RESULT: interp->result = interp_stack_pop(interp); break;

        case BC_NEG:        interp_neg_stack(interp); break;
        case BC_NOT:        interp_not_stack(interp); break;
        case BC_LOGIC_NOT:  interp_logic_not_stack(interp); break;

        case BC_MUL:        interp_mul_stack(interp); break;
        case BC_DIV:        interp_div_stack(interp); break;
        case BC_MOD:        interp_mod_stack(interp); break;
        case BC_ADD:        interp_add_stack(interp); break;
        case BC_SUB:        interp_sub_stack(interp); break;

        case BC_AAND:       interp_and_stack(interp); break;
        case BC_AOR:        interp_or_stack(interp); break;
        case BC_AXOR:       interp_xor_stack(interp); break;

        case BC_LSHIFT:     interp_lshift_stack(interp); break;
        case BC_RSHIFT:     interp_rshift_stack(interp); break;

        case BC_TEQ:        interp_teq_stack(interp); break;
        case BC_TNE:        interp_tne_stack(interp); break;
        case BC_TGT:        interp_tgt_stack(interp); break;
        case BC_TGE:        interp_tge_stack(interp); break;
        case BC_TLT:        interp_tlt_stack(interp); break;
        case BC_TLE:        interp_tle_stack(interp); break;

        case BC_TIN:        interp_set_error(interp, ERR_InvalidByteCode); break;

        case BC_LAND:       interp_set_error(interp, ERR_InvalidByteCode); break;
        case BC_LOR:        interp_set_error(interp, ERR_InvalidByteCode); break;

        default:            interp_set_error(interp, ERR_InvalidByteCode);
        }
    }
DO_END:
    return -interp->error;
}

