
#include "err.h"
#include "val.h"
#include "lex.h"
#include "bcode.h"
#include "parse.h"
#include "interp.h"
#include "object.h"
#include "function.h"
#include "string.h"

//#define SHOW(...) printf(__VA_ARGS__)
#define SHOW(...) //

static inline void interp_set_error(env_t *env, int error) {
    env->error = error;
}

static inline val_t *interp_stack_peek(env_t *env) {
    return env->sb + env->sp;
}

static inline val_t *interp_stack_pop(env_t *env) {
    return env->sb + env->sp++;
}

static inline val_t *interp_stack_push(env_t *env) {
    return env->sb + (--env->sp);
}

static inline void interp_push_ref(env_t *env, val_t *r) {
    val_set_reference(interp_stack_push(env), r);
}

static inline void interp_push_undefined(env_t *env) {
    val_set_undefined(interp_stack_push(env));
}

static inline void interp_push_nan(env_t *env) {
    val_set_nan(interp_stack_push(env));
}

static inline void interp_push_number(env_t *env, double n) {
    val_set_number(interp_stack_push(env), n);
}

static inline void interp_push_string(env_t *env, intptr_t s) {
    val_set_string(interp_stack_push(env), s);
}

static inline void interp_push_boolean(env_t *env, int b) {
    val_set_boolean(interp_stack_push(env), b);
}

static inline void interp_push_script(env_t *env, intptr_t p) {
    val_set_script(interp_stack_push(env), p);
}

static inline void interp_push_native(env_t *env, intptr_t p) {
    val_set_native(interp_stack_push(env), p);
}

static inline void interp_neg_stack(env_t *env) {
    val_t *v = interp_stack_peek(env);

    if (val_is_number(v)) {
        return val_set_number(v, -val_2_double(v));
    } else {
        return val_set_nan(v);
    }
}

static inline void interp_not_stack(env_t *env) {
    val_t *v = interp_stack_peek(env);

    if (val_is_number(v)) {
        return val_set_number(v, ~val_2_integer(v));
    } else {
        return val_set_nan(v);
    }
}

static inline void interp_logic_not_stack(env_t *env) {
    val_t *v = interp_stack_peek(env);

    val_set_boolean(v, !val_is_true(v));
}

static inline void interp_mul_stack(env_t *env) {
    val_t *b = interp_stack_pop(env);
    val_t *a = interp_stack_peek(env);
    val_t *res = a;

    if (val_is_number(b) && val_is_number(a)) {
        val_set_number(res, val_2_double(a) * val_2_double(b));
    } else {
        val_set_nan(res);
    }
}

static inline void interp_div_stack(env_t *env) {
    val_t *b = interp_stack_pop(env);
    val_t *a = interp_stack_peek(env);
    val_t *res = a;

    if (val_is_number(b) && val_2_double(b) && val_is_number(a)) {
        val_set_number(res, val_2_double(a) / val_2_double(b));
    } else {
        val_set_nan(res);
    }
}

static inline void interp_mod_stack(env_t *env) {
    val_t *b = interp_stack_pop(env);
    val_t *a = interp_stack_peek(env);
    val_t *res = a;

    if (val_is_number(b) && val_2_double(b) && val_is_number(a)) {
        val_set_number(res, val_2_integer(a) % val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline void interp_add_stack(env_t *env) {
    val_t *b = interp_stack_pop(env);
    val_t *a = interp_stack_peek(env);
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
            string_add(env, a, b, res);
        } else {
            val_set_nan(res);
        }
    } else {
        val_set_nan(res);
    }
}

static inline void interp_sub_stack(env_t *env) {
    val_t *b = interp_stack_pop(env);
    val_t *a = interp_stack_peek(env);
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

static inline void interp_and_stack(env_t *env) {
    val_t *b = interp_stack_pop(env);
    val_t *a = interp_stack_peek(env);
    val_t *res = a;

    if (val_is_number(a) && val_is_number(b)) {
        val_set_number(res, val_2_integer(a) & val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline void interp_or_stack(env_t *env) {
    val_t *b = interp_stack_pop(env);
    val_t *a = interp_stack_peek(env);
    val_t *res = a;

    if (val_is_number(a) && val_is_number(b)) {
        val_set_number(res, val_2_integer(a) | val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline void interp_xor_stack(env_t *env) {
    val_t *b = interp_stack_pop(env);
    val_t *a = interp_stack_peek(env);
    val_t *res = a;

    if (val_is_number(a) && val_is_number(b)) {
        val_set_number(res, val_2_integer(a) ^ val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline void interp_lshift_stack(env_t *env) {
    val_t *b = interp_stack_pop(env);
    val_t *a = interp_stack_peek(env);
    val_t *res = a;

    if (val_is_number(a) && val_is_number(b)) {
        val_set_number(res, val_2_integer(a) << val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline void interp_rshift_stack(env_t *env) {
    val_t *b = interp_stack_pop(env);
    val_t *a = interp_stack_peek(env);
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

static inline void interp_teq_stack(env_t *env) {
    val_t *b = interp_stack_pop(env);
    val_t *a = interp_stack_peek(env);
    val_t *res = a;

    val_set_boolean(res, interp_test_equal(a, b));
}

static inline void interp_tne_stack(env_t *env) {
    val_t *b = interp_stack_pop(env);
    val_t *a = interp_stack_peek(env);
    val_t *res = a;

    val_set_boolean(res, !interp_test_equal(a, b));
}

static inline void interp_tgt_stack(env_t *env) {
    val_t *b = interp_stack_pop(env);
    val_t *a = interp_stack_peek(env);
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

static inline void interp_tge_stack(env_t *env) {
    val_t *b = interp_stack_pop(env);
    val_t *a = interp_stack_peek(env);
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

static inline void interp_tlt_stack(env_t *env) {
    val_t *b = interp_stack_pop(env);
    val_t *a = interp_stack_peek(env);
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

static inline void interp_tle_stack(env_t *env) {
    val_t *b = interp_stack_pop(env);
    val_t *a = interp_stack_peek(env);
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

static inline void interp_assign(env_t *env) {
    val_t *b = interp_stack_pop(env);
    val_t *a = interp_stack_peek(env);
    val_t *res = a;

    *res = *val_2_reference(a) = *b;
}

static inline void interp_prop_get(env_t *env) {
    val_t *key = interp_stack_pop(env);
    val_t *obj = interp_stack_peek(env);
    val_t *prop = obj;
    int err = object_prop_get(env, obj, key, prop);

    if (err) {
        interp_set_error(env, err);
    }
}

static inline void interp_prop_call(env_t *env) {
    val_t *key = interp_stack_pop(env);
    val_t *obj = interp_stack_peek(env); // a as the first argument: self
    val_t *prop = interp_stack_push(env);
    int err = object_prop_get(env, obj, key, prop);

    if (err) {
        interp_set_error(env, err);
    }
}

static inline void interp_elem_get(env_t *env) {
    val_t *key = interp_stack_pop(env);
    val_t *obj = interp_stack_peek(env);
    val_t *elem = obj;
    int err = object_elem_get(env, obj, key, elem);

    if (err) {
        interp_set_error(env, err);
    }
}

static inline void interp_elem_call(env_t *env) {
    val_t *key = interp_stack_pop(env);
    val_t *obj = interp_stack_peek(env); // a as the first argument: self
    val_t *prop = interp_stack_push(env);
    int err = object_elem_get(env, obj, key, prop);

    if (err) {
        interp_set_error(env, err);
    }
}

int interp_run(env_t *env)
{
    double   *numbers = env->exe.number_map;
    intptr_t *strings = env->exe.string_map;
    intptr_t *natives = env->exe.native_entry;
    uint8_t  **functions = env->exe.func_map;

    uint8_t *base = functions[0]; // entry
    uint8_t *pc = base;

    int index;
    while(!env->error) {
        uint8_t code = *pc++;
        SHOW("pc: %ld, sp: %d, code: %x\n", pc - base, env->sp, code);
        switch(code) {
        case BC_STOP:       SHOW("STOP\n"); goto DO_END;
        case BC_PASS:       SHOW("PASS\n"); break;

        case BC_RET0:       env_frame_restore(env, &pc, &env->scope);
                            interp_push_undefined(env);
                            SHOW("RET0\n"); break;

        case BC_RET:        {
                                val_t *res = interp_stack_peek(env);
                                env_frame_restore(env, &pc, &env->scope);
                                *interp_stack_push(env) = *res;
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
                            if (val_is_true(interp_stack_peek(env))) {
                                pc += index;
                            }
                            SHOW("SJMP_T: %d\n", index); break;

        case BC_SJMP_F:     index = (int8_t) (*pc++);
                            SHOW("SJMP_F: %d\n", index);
                            if (!val_is_true(interp_stack_peek(env))) {
                                pc += index;
                            }
                            break;

        case BC_JMP_T:      index = (int8_t) (*pc++); index = (index << 8) | (*pc++);
                            SHOW("JMP_T: %d\n", index);
                            if (val_is_true(interp_stack_peek(env))) {
                                pc += index;
                            }
                            break;
        case BC_JMP_F:      index = (int8_t) (*pc++); index = (index << 8) | (*pc++);
                            SHOW("JMP_F: %d\n", index);
                            if (!val_is_true(interp_stack_peek(env))) {
                                pc += index;
                            }
                            break;
        case BC_SJMP_T_POP: index = (int8_t) (*pc++);
                            SHOW("SJMP_T_POP: %d\n", index);
                            if (val_is_true(interp_stack_pop(env))) {
                                pc += index;
                            }
                            break;
        case BC_SJMP_F_POP: index = (int8_t) (*pc++);
                            SHOW("SJMP_F_POP: %d\n", index);
                            if (!val_is_true(interp_stack_pop(env))) {
                                pc += index;
                            }
                            break;
        case BC_JMP_T_POP:  index = (int8_t) (*pc++); index = (index << 8) | (*pc++);
                            SHOW("JMP_T_POP: %d\n", index);
                            if (val_is_true(interp_stack_pop(env))) {
                                pc += index;
                            }
                            break;
        case BC_JMP_F_POP:  index = (int8_t) (*pc++); index = (index << 8) | (*pc++);
                            SHOW("JMP_F_POP: %d(%.4x)\n", index, index);
                            if (!val_is_true(interp_stack_pop(env))) {
                                pc += index;
                            }
                            break;

        case BC_PUSH_UND:   SHOW("PUSH_UND\n"); interp_push_undefined(env); break;
        case BC_PUSH_NAN:   SHOW("PUSH_NAN\n"); interp_push_nan(env); break;
        case BC_PUSH_TRUE:  SHOW("PUSH_TRUE\n"); interp_push_boolean(env, 1); break;
        case BC_PUSH_FALSE: SHOW("PUSH_FALSE\n"); interp_push_boolean(env, 0); break;
        case BC_PUSH_ZERO:  SHOW("PUSH_NUM 0\n"); interp_push_number(env, 0); break;
        case BC_PUSH_NUM:   index = (*pc++); index = (index << 8) + (*pc++);
                            SHOW("PUSH_NUM %f\n", numbers[index]);
                            interp_push_number(env, numbers[index]); break;
        case BC_PUSH_STR:   index = (*pc++); index = (index << 8) + (*pc++);
                            interp_push_string(env, strings[index]);
                            SHOW("PUSH_STR %s\n", (const char *)strings[index]); break;

        case BC_PUSH_VAR:   index = (*pc++);
                            *(interp_stack_push(env)) = env->scope->var_buf[index];
                            SHOW("PUSH_VAR %d\n", index);
                            break;

        case BC_PUSH_VAR_REF:
                            index = (*pc++); SHOW("PUSH_VAR_REF %d\n", index);
                            interp_push_ref(env, env->scope->var_buf + index); break;

        case BC_PUSH_SCRIPT:index = (*pc++); index = (index << 8) | (*pc++);
                            {
                                uint8_t *head = functions[index];
                                function_info_t info;

                                function_info_read(head, &info);
                                intptr_t fn = function_create(env, info.code, info.size, info.var_num, info.arg_num);
                                if (0 == fn) {
                                    interp_set_error(env, ERR_SysError);
                                } else {
                                    interp_push_script(env, fn);
                                }
                            }
                            SHOW("PUSH_SCRIPT %d\n", index);
                            break;
        case BC_PUSH_NATIVE:index = (*pc++); index = (index << 8) | (*pc++);
                            interp_push_native(env, (intptr_t) natives[index]);
                            SHOW("PUSH_NATIVE %d\n", index);
                            break;

        case BC_POP:        SHOW("POP\n"); interp_stack_pop(env); break;
        case BC_POP_RESULT: SHOW("POP_RESULT\n"); env->result = interp_stack_pop(env); break;

        case BC_NEG:        SHOW("NEG\n"); interp_neg_stack(env); break;
        case BC_NOT:        SHOW("NOT\n"); interp_not_stack(env); break;
        case BC_LOGIC_NOT:  SHOW("LOGIC_NOT\n"); interp_logic_not_stack(env); break;

        case BC_MUL:        SHOW("MUL\n"); interp_mul_stack(env); break;
        case BC_DIV:        SHOW("DIV\n"); interp_div_stack(env); break;
        case BC_MOD:        SHOW("MOD\n"); interp_mod_stack(env); break;
        case BC_ADD:        SHOW("ADD\n"); interp_add_stack(env); break;
        case BC_SUB:        SHOW("SUB\n"); interp_sub_stack(env); break;

        case BC_AAND:       SHOW("LOGIC_AND\n"); interp_and_stack(env); break;
        case BC_AOR:        SHOW("LOGIC_OR\n"); interp_or_stack(env); break;
        case BC_AXOR:       SHOW("LOGIC_XOR\n"); interp_xor_stack(env); break;

        case BC_LSHIFT:     SHOW("LSHIFT\n"); interp_lshift_stack(env); break;
        case BC_RSHIFT:     SHOW("RSHIFT\n"); interp_rshift_stack(env); break;

        case BC_TEQ:        SHOW("TEQ\n"); interp_teq_stack(env); break;
        case BC_TNE:        SHOW("TNE\n"); interp_tne_stack(env); break;
        case BC_TGT:        SHOW("TGT\n"); interp_tgt_stack(env); break;
        case BC_TGE:        SHOW("TGE\n"); interp_tge_stack(env); break;
        case BC_TLT:        SHOW("TLT\n"); interp_tlt_stack(env); break;
        case BC_TLE:        SHOW("TLE\n"); interp_tle_stack(env); break;

        case BC_TIN:        SHOW("TIN\n"); interp_set_error(env, ERR_InvalidByteCode); break;

        case BC_ASSIGN:     SHOW("ASSING\n"); interp_assign(env); break;
        case BC_FUNC_CALL:  index = *pc++;
                            {
                                val_t *fn = interp_stack_pop(env);
                                val_t *av = interp_stack_peek(env);

                                interp_stack_release(env, index);
                                if (val_is_script(fn)) {
                                    function_call(val_2_intptr(fn), env, index, av, &pc);
                                } else
                                if (val_is_native(fn)) {
                                    function_call_native(val_2_intptr(fn), env, index, av, &pc);
                                } else {
                                    interp_set_error(env, ERR_InvalidCallor);
                                }
                            }
                            SHOW("CALL %d\n", index); break;

        case BC_PROP:       interp_prop_get(env); SHOW("PROP\n"); break;
        case BC_PROP_METH:  interp_prop_call(env); SHOW("PROP_METH\n"); break;

        case BC_ELEM:       interp_elem_get(env); SHOW("ELEM\n"); break;
        case BC_ELEM_METH:  interp_elem_call(env); SHOW("ELEM_METH\n"); break;

        default:            interp_set_error(env, ERR_InvalidByteCode);
        }
    }
DO_END:
    return -env->error;
}

