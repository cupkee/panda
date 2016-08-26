
#include "err.h"
#include "val.h"
#include "bcode.h"
#include "parse.h"
#include "compile.h"
#include "interp.h"
#include "object.h"
#include "function.h"
#include "string.h"

static val_t undefined = TAG_UNDEFINED;

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

static inline void interp_push_ref(env_t *env, intptr_t r) {
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
    val_t *a = b + 1;
    val_t *res = a;

    if (val_is_number(b) && val_is_number(a)) {
        val_set_number(res, val_2_double(a) * val_2_double(b));
    } else {
        val_set_nan(res);
    }
}

static inline void interp_div_stack(env_t *env) {
    val_t *b = interp_stack_pop(env);
    val_t *a = b + 1;
    val_t *res = a;

    if (val_is_number(b) && val_2_double(b) && val_is_number(a)) {
        val_set_number(res, val_2_double(a) / val_2_double(b));
    } else {
        val_set_nan(res);
    }
}

static inline void interp_mod_stack(env_t *env) {
    val_t *b = interp_stack_pop(env);
    val_t *a = b + 1;
    val_t *res = a;

    if (val_is_number(b) && val_2_double(b) && val_is_number(a)) {
        val_set_number(res, val_2_integer(a) % val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline void interp_add_stack(env_t *env) {
    val_t *b = interp_stack_peek(env); // Note: keep in stack, deffence GC!
    val_t *a = b + 1;
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
            // Maybe cause gc here!
            string_add(env, a, b, res);
        } else {
            val_set_nan(res);
        }
    } else {
        val_set_nan(res);
    }
    interp_stack_pop(env);
}

static inline void interp_sub_stack(env_t *env) {
    val_t *b = interp_stack_pop(env);
    val_t *a = b + 1;
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
    val_t *a = b + 1;
    val_t *res = a;

    if (val_is_number(a) && val_is_number(b)) {
        val_set_number(res, val_2_integer(a) & val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline void interp_or_stack(env_t *env) {
    val_t *b = interp_stack_pop(env);
    val_t *a = b + 1;
    val_t *res = a;

    if (val_is_number(a) && val_is_number(b)) {
        val_set_number(res, val_2_integer(a) | val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline void interp_xor_stack(env_t *env) {
    val_t *b = interp_stack_pop(env);
    val_t *a = b + 1;
    val_t *res = a;

    if (val_is_number(a) && val_is_number(b)) {
        val_set_number(res, val_2_integer(a) ^ val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline void interp_lshift_stack(env_t *env) {
    val_t *b = interp_stack_pop(env);
    val_t *a = b + 1;
    val_t *res = a;

    if (val_is_number(a) && val_is_number(b)) {
        val_set_number(res, val_2_integer(a) << val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline void interp_rshift_stack(env_t *env) {
    val_t *b = interp_stack_pop(env);
    val_t *a = b + 1;
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
    val_t *a = b + 1;
    val_t *res = a;

    val_set_boolean(res, interp_test_equal(a, b));
}

static inline void interp_tne_stack(env_t *env) {
    val_t *b = interp_stack_pop(env);
    val_t *a = b + 1;
    val_t *res = a;

    val_set_boolean(res, !interp_test_equal(a, b));
}

static inline void interp_tgt_stack(env_t *env) {
    val_t *b = interp_stack_pop(env);
    val_t *a = b + 1;
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
    val_t *a = b + 1;
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
    val_t *a = b + 1;
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
    val_t *a = b + 1;
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
    val_t *rht = interp_stack_peek(env);
    val_t *lft = rht + 1;

    if (val_is_reference(lft)) {
        val_t *res = lft;

        *res = *env_get_var_ref(env, val_2_intptr(lft)) = *rht;
        interp_stack_pop(env);
    } else {
        val_t *obj = lft + 1;
        val_t *res = obj;
        *res = object_prop_set(env, obj, lft, rht);
        interp_stack_release(env, 2);
    }
}

static inline void interp_prop_get(env_t *env) {
    val_t *key = interp_stack_peek(env);
    val_t *obj = key + 1;
    val_t *res = obj;
    int err = object_prop_get(env, obj, key, res);

    if (err) {
        interp_set_error(env, err);
    }
    interp_stack_pop(env);
}

static inline void interp_prop_call(env_t *env) {
    val_t *key = interp_stack_peek(env);
    val_t *obj = key + 1; // a as the first argument: self
    val_t *prop = key;
    int err = object_prop_get(env, obj, key, prop);

    if (err) {
        interp_set_error(env, err);
    }
    // no pop
}

static inline void interp_elem_get(env_t *env) {
    val_t *key = interp_stack_peek(env);
    val_t *obj = key + 1;
    val_t *res = obj;
    int err = object_elem_get(env, obj, key, res);

    if (err) {
        interp_set_error(env, err);
    }
    interp_stack_pop(env);
}

static inline void interp_elem_call(env_t *env) {
    val_t *key = interp_stack_peek(env);
    val_t *obj = key + 1; // a as the first argument: self
    val_t *elem = key;
    int err = object_elem_get(env, obj, key, elem);

    if (err) {
        interp_set_error(env, err);
    }
    // no pop
}

#if 0
static inline void interp_show(uint8_t *pc, int sp) {
    const char *cmd;
    int param, n;

    n = bcode_parse(pc, NULL, &cmd, &param);

    if (n == 0) {
        printf("[PC: %p, SP: %d] %s\n", pc, sp, cmd);
    } else {
        printf("[PC: %p, SP: %d] %s %d\n", pc, sp, cmd, param);
    }
}
#else
static inline void interp_show(uint8_t *pc, int sp) {}
#endif

static int interp_run(env_t *env, uint8_t *entry, val_t **result)
{
    double   *numbers = env->exe.number_map;
    intptr_t *strings = env->exe.string_map;
    intptr_t *natives = env->native_entry;
    uint8_t  **functions = env->exe.func_map;
    uint8_t *base, *pc;

    env_entry_setup(env, entry, 0, NULL, &pc);
    base = pc;

    int index;
    while(!env->error) {
        uint8_t code;

        interp_show(pc, env->sp);
        code = *pc++;
        switch(code) {
        case BC_STOP:       goto DO_END;
        case BC_PASS:       break;

        /* Return instruction */
        case BC_RET0:       env_frame_restore(env, &pc, &env->scope);
                            interp_push_undefined(env);
                            break;

        case BC_RET:        {
                                val_t *res = interp_stack_peek(env);
                                env_frame_restore(env, &pc, &env->scope);
                                *interp_stack_push(env) = *res;
                            }
                            break;

        /* Jump instruction */
        case BC_SJMP:       index = (int8_t) (*pc++); pc += index;
                            break;

        case BC_JMP:        index = (int8_t) (*pc++); index = (index << 8) | (*pc++); pc += index;
                            break;

        case BC_SJMP_T:     index = (int8_t) (*pc++);
                            if (val_is_true(interp_stack_peek(env))) {
                                pc += index;
                            }
                            break;

        case BC_SJMP_F:     index = (int8_t) (*pc++);
                            if (!val_is_true(interp_stack_peek(env))) {
                                pc += index;
                            }
                            break;

        case BC_JMP_T:      index = (int8_t) (*pc++); index = (index << 8) | (*pc++);
                            if (val_is_true(interp_stack_peek(env))) {
                                pc += index;
                            }
                            break;
        case BC_JMP_F:      index = (int8_t) (*pc++); index = (index << 8) | (*pc++);
                            if (!val_is_true(interp_stack_peek(env))) {
                                pc += index;
                            }
                            break;
        case BC_SJMP_T_POP: index = (int8_t) (*pc++);
                            if (val_is_true(interp_stack_pop(env))) {
                                pc += index;
                            }
                            break;
        case BC_SJMP_F_POP: index = (int8_t) (*pc++);
                            if (!val_is_true(interp_stack_pop(env))) {
                                pc += index;
                            }
                            break;
        case BC_JMP_T_POP:  index = (int8_t) (*pc++); index = (index << 8) | (*pc++);
                            if (val_is_true(interp_stack_pop(env))) {
                                pc += index;
                            }
                            break;
        case BC_JMP_F_POP:  index = (int8_t) (*pc++); index = (index << 8) | (*pc++);
                            if (!val_is_true(interp_stack_pop(env))) {
                                pc += index;
                            }
                            break;

        case BC_PUSH_UND:   interp_push_undefined(env);  break;
        case BC_PUSH_NAN:   interp_push_nan(env);        break;
        case BC_PUSH_TRUE:  interp_push_boolean(env, 1); break;
        case BC_PUSH_FALSE: interp_push_boolean(env, 0); break;
        case BC_PUSH_ZERO:  interp_push_number(env, 0);  break;

        case BC_PUSH_NUM:   index = (*pc++); index = (index << 8) + (*pc++);
                            interp_push_number(env, numbers[index]);
                            break;
        case BC_PUSH_STR:   index = (*pc++); index = (index << 8) + (*pc++);
                            interp_push_string(env, strings[index]);
                            break;

        case BC_PUSH_VAR:   index = (*pc++);
                            *(interp_stack_push(env)) = env->scope->var_buf[index];
                            break;

        case BC_PUSH_VAR_REF:
                            index = (*pc++); interp_push_ref(env, index);
                            break;

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
                            break;

        case BC_PUSH_NATIVE:index = (*pc++); index = (index << 8) | (*pc++);
                            interp_push_native(env, (intptr_t) natives[index]);
                            break;

        case BC_POP:        interp_stack_pop(env); break;
        case BC_POP_RESULT: *result = interp_stack_pop(env); break;

        case BC_NEG:        interp_neg_stack(env); break;
        case BC_NOT:        interp_not_stack(env); break;
        case BC_LOGIC_NOT:  interp_logic_not_stack(env); break;

        case BC_MUL:        interp_mul_stack(env); break;
        case BC_DIV:        interp_div_stack(env); break;
        case BC_MOD:        interp_mod_stack(env); break;
        case BC_ADD:        interp_add_stack(env); break;
        case BC_SUB:        interp_sub_stack(env); break;

        case BC_AAND:       interp_and_stack(env); break;
        case BC_AOR:        interp_or_stack(env);  break;
        case BC_AXOR:       interp_xor_stack(env); break;

        case BC_LSHIFT:     interp_lshift_stack(env); break;
        case BC_RSHIFT:     interp_rshift_stack(env); break;

        case BC_TEQ:        interp_teq_stack(env); break;
        case BC_TNE:        interp_tne_stack(env); break;
        case BC_TGT:        interp_tgt_stack(env); break;
        case BC_TGE:        interp_tge_stack(env); break;
        case BC_TLT:        interp_tlt_stack(env); break;
        case BC_TLE:        interp_tle_stack(env); break;

        case BC_TIN:        interp_set_error(env, ERR_InvalidByteCode); break;

        case BC_ASSIGN:     interp_assign(env); break;
        case BC_FUNC_CALL:  index = *pc++;
                            {
                                val_t *fn = interp_stack_peek(env);
                                val_t *av = fn + 1;
                                int ac = index;

                                if (val_is_script(fn)) {
                                    function_call(fn, env, ac, av, &pc);
                                } else
                                if (val_is_native(fn)) {
                                    function_call_native(fn, env, ac, av);
                                } else {
                                    interp_set_error(env, ERR_InvalidCallor);
                                }
                            }
                            break;

        case BC_PROP:       interp_prop_get(env);  break;
        case BC_PROP_METH:  interp_prop_call(env); break;

        case BC_ELEM:       interp_elem_get(env);  break;
        case BC_ELEM_METH:  interp_elem_call(env); break;

        default:            interp_set_error(env, ERR_InvalidByteCode);
        }
    }
DO_END:
    return -env->error;
}

static void parse_callback(void *u, parse_event_t *e)
{
    if (e->type == PARSE_EOF) {
    } else
    if (e->type == PARSE_SIMPLE) {
    } else
    if (e->type == PARSE_ENTER_BLOCK) {
    } else
    if (e->type == PARSE_LEAVE_BLOCK) {
    } else
    if (e->type == PARSE_FAIL) {
    }
}

int interp_env_init_interactive(env_t *env, void *mem_ptr, int mem_size, void *heap_ptr, int heap_size, val_t *stack_ptr, int stack_size)
{
    return env_init(env, mem_ptr, mem_size,
                heap_ptr, heap_size, stack_ptr, stack_size,
                EXE_NUMBER_MAX, EXE_STRING_MAX, EXE_NATIVE_MAX, EXE_FUNCTION_MAX,
                EXE_MAIN_CODE_MAX, EXE_FUNC_CODE_MAX, 1);
}

int interp_env_init_interpreter(env_t *env, void *mem_ptr, int mem_size, void *heap_ptr, int heap_size, val_t *stack_ptr, int stack_size)
{
    return env_init(env, mem_ptr, mem_size,
                heap_ptr, heap_size, stack_ptr, stack_size,
                EXE_NUMBER_MAX, EXE_STRING_MAX, EXE_NATIVE_MAX, EXE_FUNCTION_MAX,
                EXE_MAIN_CODE_MAX, EXE_FUNC_CODE_MAX, 0);
}

int interp_env_init_executable (env_t *env, void *mem_ptr, int mem_size, void *heap_ptr, int heap_size, val_t *stack_ptr, int stack_size, executable_file_t *ef)
{
    int i;
    executable_t *exe;

    if (!ef || ef->error || ef->byte_order != SYS_BYTE_ORDER) {
        return -1;
    }

    if (0 != env_init(env, mem_ptr, mem_size,
                    heap_ptr, heap_size, stack_ptr, stack_size,
                    0, ef->str_cnt, EXE_NATIVE_MAX, ef->fn_cnt, 0, 0, 0)) {
        return -1;
    }

    exe = &env->exe;
    exe->number_map = executable_file_number_entry(ef);
    exe->number_num = ef->num_cnt;

    for (i = 0; i < ef->str_cnt; i++) {
        exe->string_map[i] = (intptr_t)executable_file_get_string(ef, i);
    }

    for (i = 0; i < ef->fn_cnt; i++) {
        exe->func_map[i] = (uint8_t *)executable_file_get_function(ef, i);
    }

    return 0;
}

int interp_execute(env_t *env, val_t **v)
{

    if (!env) {
        return -1;
    }

    if (v) {
        *v = &undefined;
    }

    if (0 != interp_run(env, env_get_main_entry(env), v)) {
        return -env->error;
    }

    return 0;
}

int interp_execute_string(env_t *env, const char *input, val_t **v)
{
    stmt_t  *stmt;
    heap_t *heap = env_heap_get_free((env_t*)env);
    parser_t psr;
    compile_t cpl;
    int error = 0, stmt_type;

    if (!env || !input) {
        return -1;
    }

    // The free heap can be used for parse and compile process
    parse_init(&psr, input, NULL, heap->base, heap->size);
    stmt = parse_stmt_multi(&psr, parse_callback, NULL);
    if (!stmt) {
        return psr.error ? -psr.error : 0;
    }

    compile_init(&cpl, env, heap_free_addr(&psr.heap), heap_free_size(&psr.heap));
    if (0 == compile_multi_stmt(&cpl, stmt) && 0 == compile_update(&cpl)) {
        //Todo: get other way!!
        while (stmt) {
            if (!stmt->next) {
                stmt_type = stmt->type;
            }
            stmt = stmt->next;
        }

        if (0 != interp_run(env, env_get_main_entry(env), v)) {
            error = -env->error;
        }
    } else {
        error = -cpl.error;
    }

    if (!error && stmt_type != STMT_EXPR) {
        *v = &undefined;
    }

    return error ? error : 1;
}

int interp_execute_interactive(env_t *env, const char *input, char *(*input_more)(void), val_t **v)
{
    stmt_t  *stmt;
    parser_t psr;
    compile_t cpl;
    int stmt_type;
    heap_t *heap = env_heap_get_free((env_t*)env);

    if (!env || !input) {
        return -1;
    }

    // The free heap can be used for parse and compile process
    parse_init(&psr, input, input_more, heap->base, heap->size);
    stmt = parse_stmt(&psr, parse_callback, NULL);
    if (!stmt) {
        return psr.error ? -psr.error : 0;
    }
    stmt_type = stmt->type;

    compile_init(&cpl, env, heap_free_addr(&psr.heap), heap_free_size(&psr.heap));
    if (0 == compile_one_stmt(&cpl, stmt) && 0 == compile_update(&cpl)) {

        if (0 != interp_run(env, env_get_main_entry(env), v)) {
            return -env->error;
        }
    } else {
        return -cpl.error;
    }

    if (stmt_type != STMT_EXPR) {
        *v = &undefined;
    }

    return 1;
}

