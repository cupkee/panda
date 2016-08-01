
#include "err.h"
#include "val.h"
#include "lex.h"
#include "bcode.h"
#include "parse.h"
#include "interp.h"
#include "function.h"
#include "string.h"

//#define SHOW(...) printf(__VA_ARGS__)
#define SHOW(...) //

static inline void interp_set_error(interp_t *interp, int error) {
    interp->error = error;
}

static inline val_t *interp_stack_peek(interp_t *interp) {
    return interp->sb + interp->sp;
}

static inline val_t *interp_stack_pop(interp_t *interp) {
    return interp->sb + interp->sp++;
}

static inline val_t *interp_stack_push(interp_t *interp) {
    return interp->sb + (--interp->sp);
}

static inline void interp_push_ref(interp_t *interp, val_t *r) {
    val_set_reference(interp_stack_push(interp), r);
}

static inline void interp_push_undefined(interp_t *interp) {
    val_set_undefined(interp_stack_push(interp));
}

static inline void interp_push_nan(interp_t *interp) {
    val_set_nan(interp_stack_push(interp));
}

static inline void interp_push_number(interp_t *interp, double n) {
    val_set_number(interp_stack_push(interp), n);
}

static inline void interp_push_string(interp_t *interp, intptr_t s) {
    val_set_string(interp_stack_push(interp), s);
}

static inline void interp_push_boolean(interp_t *interp, int b) {
    val_set_boolean(interp_stack_push(interp), b);
}

static inline void interp_push_script(interp_t *interp, intptr_t p) {
    val_set_script(interp_stack_push(interp), p);
}

static inline void interp_push_native(interp_t *interp, intptr_t p) {
    val_set_native(interp_stack_push(interp), p);
}

static inline void interp_neg_stack(interp_t *interp) {
    val_t *v = interp_stack_peek(interp);

    if (val_is_number(v)) {
        return val_set_number(v, -val_2_double(v));
    } else {
        return val_set_nan(v);
    }
}

static inline void interp_not_stack(interp_t *interp) {
    val_t *v = interp_stack_peek(interp);

    if (val_is_number(v)) {
        return val_set_number(v, ~val_2_integer(v));
    } else {
        return val_set_nan(v);
    }
}

static inline void interp_logic_not_stack(interp_t *interp) {
    val_t *v = interp_stack_peek(interp);

    val_set_boolean(v, !val_is_true(v));
}

static inline void interp_mul_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    if (val_is_number(b)) {
        if (val_is_number(a)) {
            val_set_number(res, val_2_double(a) * val_2_double(b));
        } else {
            val_set_nan(res);
        }
    } else {
        val_set_nan(res);
    }

}

static inline void interp_div_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    if (val_is_number(b) && val_2_double(b)) {
        if (val_is_number(a)) {
            val_set_number(res, val_2_double(a) / val_2_double(b));
        } else {
            val_set_nan(res);
        }
    } else {
        val_set_nan(res);
    }
}

static inline void interp_mod_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    if (val_is_number(b) && val_2_double(b) && val_is_number(a)) {
        val_set_number(res, val_2_integer(a) % val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline void interp_add_stack(interp_t *interp, env_t *env) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    if (val_is_number(a)) {
        if (val_is_number(b)) {
            val_set_number(res, val_2_double(a) + val_2_double(b));
        } else {
            val_set_nan(res);
        }
    } else
    if (val_is_string(a)){
        if (val_is_string(b)) {
            *res = string_concat(env, a, b);
        } else {
            val_set_nan(res);
        }
    } else {
        val_set_nan(res);
    }
}

static inline void interp_sub_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    if (val_is_number(a)) {
        if (val_is_number(b)) {
            val_set_number(res, val_2_double(a) - val_2_double(b));
        } else {
            val_set_nan(res);
        }
    } else {
        val_set_nan(res);
    }
}

static inline void interp_and_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    if (val_is_number(a) && val_is_number(b)) {
        val_set_number(res, val_2_integer(a) & val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline void interp_or_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    if (val_is_number(a) && val_is_number(b)) {
        val_set_number(res, val_2_integer(a) | val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline void interp_xor_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    if (val_is_number(a) && val_is_number(b)) {
        val_set_number(res, val_2_integer(a) ^ val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline void interp_lshift_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    if (val_is_number(a) && val_is_number(b)) {
        val_set_number(res, val_2_integer(a) << val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline void interp_rshift_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    if (val_is_number(a) && val_is_number(b)) {
        val_set_number(res, val_2_integer(a) >> val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline int interp_test_equal(val_t *a, val_t *b) {
    if (*a == *b) {
        return !(val_is_nan(a) || val_is_undefined(a));
    } else {
        if (val_is_string(a)) {
            return string_compare(a, b) == 0;
        } else {
            return 0;
        }
    }
}

static inline void interp_teq_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    val_set_boolean(res, interp_test_equal(a, b));
}

static inline void interp_tne_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    val_set_boolean(res, !interp_test_equal(a, b));
}

static inline void interp_tgt_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    if (val_is_number(a)) {
        if (val_is_number(b)) {
            val_set_boolean(res, val_2_double(a) - val_2_double(b) > 0);
            return;
        }
    } else
    if (val_is_string(a)) {
        if (val_is_string(b)) {
            val_set_boolean(res, string_compare(a, b) > 0);
            return;
        }
    }
    val_set_boolean(res, 0);
}

static inline void interp_tge_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    if (val_is_number(a)) {
        if (val_is_number(b)) {
            val_set_boolean(res, val_2_double(a) - val_2_double(b) >= 0);
            return;
        }
    } else
    if (val_is_string(a)) {
        if (val_is_string(b)) {
            val_set_boolean(res, string_compare(a, b) >= 0);
            return;
        }
    }
    val_set_boolean(res, 0);
}

static inline void interp_tlt_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    if (val_is_number(a)) {
        if (val_is_number(b)) {
            val_set_boolean(res, val_2_double(a) - val_2_double(b) < 0);
            return;
        }
    } else
    if (val_is_string(a)) {
        if (val_is_string(b)) {
            val_set_boolean(res, string_compare(a, b) < 0);
            return;
        }
    }
    val_set_boolean(res, 0);
}

static inline void interp_tle_stack(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    if (val_is_number(a)) {
        if (val_is_number(b)) {
            val_set_boolean(res, val_2_double(a) - val_2_double(b) <= 0);
            return;
        }
    } else
    if (val_is_string(a)) {
        if (val_is_string(b)) {
            val_set_boolean(res, string_compare(a, b) <= 0);
            return;
        }
    }
    val_set_boolean(res, 0);
}

static inline void interp_assign(interp_t *interp) {
    val_t *b = interp_stack_pop(interp);
    val_t *a = interp_stack_peek(interp);
    val_t *res = a;

    *res = *val_2_reference(a) = *b;
}

interp_t *interp_init(interp_t *interp, val_t *stack_ptr, int stack_size)
{
    if (interp) {
        interp->sb = stack_ptr;
        interp->ss = stack_size;
        interp->sp = stack_size;
        interp->fp = stack_size;

        interp->error = 0;
        interp->result = NULL;
    }
    return interp;
}

int interp_deinit(interp_t *interp)
{
    return 0;
}

void interp_frame_setup(interp_t *interp, uint8_t *pc, scope_t *scope)
{
    int fp;
    interp_frame_t *frame;

    if (interp->sp < FRAME_SIZE) {
        interp->error = ERR_SysError;
        return;
    }

    fp = interp->sp - FRAME_SIZE;
    frame = (interp_frame_t *)(interp->sb + fp);

    //printf ("############  resave sp: %d, fp: %d\n", interp->sp, interp->fp);
    frame->fp = interp->fp;
    frame->sp = interp->sp;
    frame->pc = (intptr_t) pc;
    frame->scope = (intptr_t) scope;

    interp->fp = fp;
    interp->sp = fp;
}

void interp_frame_restore(interp_t *interp, uint8_t **pc, scope_t **scope)
{
    if (interp->fp != interp->ss) {
        interp_frame_t *frame = (interp_frame_t *)(interp->sb + interp->fp);

        //printf ("$$$$$$$$$$$$$$ restore sp: %d, fp: %d\n", frame->sp, frame->fp);
        interp->sp = frame->sp;
        interp->fp = frame->fp;
        *pc = (uint8_t *) frame->pc;
        *scope = (scope_t *) frame->scope;
    } else {
        *pc = NULL;
        *scope = NULL;
    }
}

int interp_run(interp_t *interp, env_t *env, module_t *mod)
{
    double   *numbers = mod->numbers;
    intptr_t *natives = mod->natives;
    intptr_t *strings = mod->strings;

    uint8_t *base = mod->ft[mod->entry].code;
    uint8_t *pc = base;

    int index;

    while(!interp->error) {
        uint8_t code = *pc++;
        SHOW("pc: %ld, sp: %d, code: %x\n", pc - base, interp->sp, code);
        switch(code) {
        case BC_STOP:       SHOW("STOP\n"); goto DO_END;
        case BC_PASS:       SHOW("PASS\n"); break;

        case BC_RET0:       interp_frame_restore(interp, &pc, &env->scope);
                            interp_push_undefined(interp);
                            SHOW("RET0\n"); break;

        case BC_RET:        {
                                val_t *res = interp_stack_peek(interp);
                                interp_frame_restore(interp, &pc, &env->scope);
                                *interp_stack_push(interp) = *res;
                            }
                            SHOW("RET\n"); break;

        /* Jump instruction */
        case BC_SJMP:       index = (int8_t) (*pc++);
                            pc += index;
                            SHOW("SJMP: %d\n", index); break;

        case BC_JMP:        index = (int8_t) (*pc++); index = (index << 8) | (*pc++);
                            pc += index;
                            SHOW("JMP: %d\n", index); break;

        case BC_SJMP_T:     index = (int8_t) (*pc++);
                            if (val_is_true(interp_stack_peek(interp))) {
                                pc += index;
                            }
                            SHOW("SJMP_T: %d\n", index); break;

        case BC_SJMP_F:     index = (int8_t) (*pc++);
                            SHOW("SJMP_F: %d\n", index);
                            if (!val_is_true(interp_stack_peek(interp))) {
                                pc += index;
                            }
                            break;

        case BC_JMP_T:      index = (int8_t) (*pc++); index = (index << 8) | (*pc++);
                            SHOW("JMP_T: %d\n", index);
                            if (val_is_true(interp_stack_peek(interp))) {
                                pc += index;
                            }
                            break;
        case BC_JMP_F:      index = (int8_t) (*pc++); index = (index << 8) | (*pc++);
                            SHOW("JMP_F: %d\n", index);
                            if (!val_is_true(interp_stack_peek(interp))) {
                                pc += index;
                            }
                            break;
        case BC_SJMP_T_POP: index = (int8_t) (*pc++);
                            SHOW("SJMP_T_POP: %d\n", index);
                            if (val_is_true(interp_stack_pop(interp))) {
                                pc += index;
                            }
                            break;
        case BC_SJMP_F_POP: index = (int8_t) (*pc++);
                            SHOW("SJMP_F_POP: %d\n", index);
                            if (!val_is_true(interp_stack_pop(interp))) {
                                pc += index;
                            }
                            break;
        case BC_JMP_T_POP:  index = (int8_t) (*pc++); index = (index << 8) | (*pc++);
                            SHOW("JMP_T_POP: %d\n", index);
                            if (val_is_true(interp_stack_pop(interp))) {
                                pc += index;
                            }
                            break;
        case BC_JMP_F_POP:  index = (int8_t) (*pc++); index = (index << 8) | (*pc++);
                            SHOW("JMP_F_POP: %d(%.4x)\n", index, index);
                            if (!val_is_true(interp_stack_pop(interp))) {
                                pc += index;
                            }
                            break;

        case BC_PUSH_UND:   SHOW("PUSH_UND\n"); interp_push_undefined(interp); break;
        case BC_PUSH_NAN:   SHOW("PUSH_NAN\n"); interp_push_nan(interp); break;
        case BC_PUSH_TRUE:  SHOW("PUSH_TRUE\n"); interp_push_boolean(interp, 1); break;
        case BC_PUSH_FALSE: SHOW("PUSH_FALSE\n"); interp_push_boolean(interp, 0); break;
        case BC_PUSH_ZERO:  SHOW("PUSH_NUM 0\n"); interp_push_number(interp, 0); break;
        case BC_PUSH_NUM:   index = (*pc++); index = (index << 8) + (*pc++);
                            SHOW("PUSH_NUM %f\n", numbers[index]);
                            interp_push_number(interp, numbers[index]); break;
        case BC_PUSH_STR:   index = (*pc++); index = (index << 8) + (*pc++);
                            SHOW("PUSH_STR %s\n", (const char *)strings[index]);
                            interp_push_string(interp, strings[index]); break;

        case BC_PUSH_VAR:   index = (*pc++); SHOW("PUSH_VAR %d\n", index);
                            *(interp_stack_push(interp)) = env->scope->variables[index]; break;

        case BC_PUSH_VAR_REF:
                            index = (*pc++); SHOW("PUSH_VAR_REF %d\n", index);
                            interp_push_ref(interp, env->scope->variables + index); break;

        case BC_PUSH_SCRIPT:index = (*pc++); index = (index << 8) | (*pc++);
                            {
                                fn_template_t *ft = mod->ft + index;
                                intptr_t fn = function_create(ft->code, ft->size, ft->var_num, ft->arg_num);
                                if (0 == fn) {
                                    interp->error = ERR_SysError;
                                } else {
                                    interp_push_script(interp, fn);
                                }
                            }
                            SHOW("PUSH_SCRIPT %d\n", index);
                            break;
        case BC_PUSH_NATIVE:index = (*pc++); index = (index << 8) | (*pc++);
                            interp_push_native(interp, (intptr_t) natives[index]);
                            SHOW("PUSH_NATIVE %d\n", index);
                            break;

        case BC_POP:        SHOW("POP\n"); interp_stack_pop(interp); break;
        case BC_POP_RESULT: SHOW("POP_RESULT\n"); interp->result = interp_stack_pop(interp); break;

        case BC_NEG:        SHOW("NEG\n"); interp_neg_stack(interp); break;
        case BC_NOT:        SHOW("NOT\n"); interp_not_stack(interp); break;
        case BC_LOGIC_NOT:  SHOW("LOGIC_NOT\n"); interp_logic_not_stack(interp); break;

        case BC_MUL:        SHOW("MUL\n"); interp_mul_stack(interp); break;
        case BC_DIV:        SHOW("DIV\n"); interp_div_stack(interp); break;
        case BC_MOD:        SHOW("MOD\n"); interp_mod_stack(interp); break;
        case BC_ADD:        SHOW("ADD\n"); interp_add_stack(interp, env); break;
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
        case BC_FUNC_CALL:  index = *pc++;
                            {
                                val_t *fn = interp_stack_pop(interp);
                                val_t *av = interp_stack_peek(interp);

                                interp_stack_release(interp, index);
                                if (val_is_script(fn)) {
                                    function_call(val_2_intptr(fn), interp, env, index, av, &pc);
                                } else
                                if (val_is_native(fn)) {
                                    function_native_call(val_2_intptr(fn), interp, env, index, av, &pc);
                                } else {
                                    interp_set_error(interp, ERR_SysError);
                                }
                            }
                            SHOW("CALL %d\n", index); break;

        default:            interp_set_error(interp, ERR_InvalidByteCode);
        }
    }
DO_END:
    return -interp->error;
}

