
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

static inline void interp_neg(env_t *env) {
    val_t *v = env_stack_peek(env);

    if (val_is_number(v)) {
        return val_set_number(v, -val_2_double(v));
    } else {
        return val_set_nan(v);
    }
}

static inline void interp_not(env_t *env) {
    val_t *v = env_stack_peek(env);

    if (val_is_number(v)) {
        return val_set_number(v, ~val_2_integer(v));
    } else {
        return val_set_nan(v);
    }
}

static inline void interp_logic_not(env_t *env) {
    val_t *v = env_stack_peek(env);

    val_set_boolean(v, !val_is_true(v));
}

static inline void interp_mul(env_t *env) {
    val_t *b = env_stack_pop(env);
    val_t *a = b + 1;
    val_t *res = a;

    if (val_is_number(b) && val_is_number(a)) {
        val_set_number(res, val_2_double(a) * val_2_double(b));
    } else {
        val_set_nan(res);
    }
}

static inline void interp_div(env_t *env) {
    val_t *b = env_stack_pop(env);
    val_t *a = b + 1;
    val_t *res = a;

    if (val_is_number(b) && val_2_double(b) && val_is_number(a)) {
        val_set_number(res, val_2_double(a) / val_2_double(b));
    } else {
        val_set_nan(res);
    }
}

static inline void interp_mod(env_t *env) {
    val_t *b = env_stack_pop(env);
    val_t *a = b + 1;
    val_t *res = a;

    if (val_is_number(b) && val_2_double(b) && val_is_number(a)) {
        val_set_number(res, val_2_integer(a) % val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline void interp_add(env_t *env) {
    val_t *b = env_stack_peek(env); // Note: keep in stack, deffence GC!
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
    env_stack_pop(env);
}

static inline void interp_sub(env_t *env) {
    val_t *b = env_stack_pop(env);
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

static inline void interp_and(env_t *env) {
    val_t *b = env_stack_pop(env);
    val_t *a = b + 1;
    val_t *res = a;

    if (val_is_number(a) && val_is_number(b)) {
        val_set_number(res, val_2_integer(a) & val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline void interp_or(env_t *env) {
    val_t *b = env_stack_pop(env);
    val_t *a = b + 1;
    val_t *res = a;

    if (val_is_number(a) && val_is_number(b)) {
        val_set_number(res, val_2_integer(a) | val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline void interp_xor(env_t *env) {
    val_t *b = env_stack_pop(env);
    val_t *a = b + 1;
    val_t *res = a;

    if (val_is_number(a) && val_is_number(b)) {
        val_set_number(res, val_2_integer(a) ^ val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline void interp_lshift(env_t *env) {
    val_t *b = env_stack_pop(env);
    val_t *a = b + 1;
    val_t *res = a;

    if (val_is_number(a) && val_is_number(b)) {
        val_set_number(res, val_2_integer(a) << val_2_integer(b));
    } else {
        val_set_nan(res);
    }
}

static inline void interp_rshift(env_t *env) {
    val_t *b = env_stack_pop(env);
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

static inline void interp_teq(env_t *env) {
    val_t *b = env_stack_pop(env);
    val_t *a = b + 1;
    val_t *res = a;

    val_set_boolean(res, interp_test_equal(a, b));
}

static inline void interp_tne(env_t *env) {
    val_t *b = env_stack_pop(env);
    val_t *a = b + 1;
    val_t *res = a;

    val_set_boolean(res, !interp_test_equal(a, b));
}

static inline void interp_tgt(env_t *env) {
    val_t *b = env_stack_pop(env);
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

static inline void interp_tge(env_t *env) {
    val_t *b = env_stack_pop(env);
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

static inline void interp_tlt(env_t *env) {
    val_t *b = env_stack_pop(env);
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

static inline void interp_tle(env_t *env) {
    val_t *b = env_stack_pop(env);
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
    val_t *rht = env_stack_peek(env);
    val_t *lft = rht + 1;

    if (val_is_reference(lft)) {
        val_t *res = lft;
        uint8_t id, generation;

        val_2_reference(lft, &id, &generation);
        lft = env_get_var(env, id, generation);
        if (lft) {
            *res = *lft = *rht;
            env_stack_pop(env);
            return;
        }
    }
    env_set_error(env, ERR_InvalidLeftValue);
}

static inline uint8_t *interp_call(env_t *env, int ac, uint8_t *pc) {
    val_t *fn = env_stack_peek(env);
    val_t *av = fn + 1;

    if (val_is_script(fn)) {
        return env_frame_setup(env, pc, fn, ac, av);
    } else
    if (val_is_native(fn)) {
        env_native_call(env, fn, ac, av);
    } else {
        env_set_error(env, ERR_InvalidCallor);
    }
    return pc;
}

static inline void interp_prop_get(env_t *env) {
    val_t *key = env_stack_peek(env); // keep the "key" in stack, defence GC
    val_t *obj = key + 1;
    val_t *res = obj;
    int err = object_prop_get(env, obj, key, res);

    if (err) {
        env_set_error(env, err);
    }
    env_stack_pop(env);
}

static inline void interp_prop_self(env_t *env) {
    val_t *key = env_stack_peek(env);
    val_t *self = key + 1;
    val_t *prop = key;
    int err = object_prop_get(env, self, key, prop);

    if (err) {
        env_set_error(env, err);
    }
    // no pop
}

static inline void interp_elem_get(env_t *env) {
    val_t *key = env_stack_peek(env);
    val_t *obj = key + 1;
    val_t *res = obj;
    int err = object_elem_get(env, obj, key, res);

    if (err) {
        env_set_error(env, err);
    }
    env_stack_pop(env);
}

static inline void interp_elem_self(env_t *env) {
    val_t *key = env_stack_peek(env); // keey the "key" in stack
    val_t *self = key + 1;
    val_t *elem = key;
    int err = object_elem_get(env, self, key, elem);

    if (err) {
        env_set_error(env, err);
    }
    // no pop
}

static inline
void interp_push_function(env_t *env, unsigned int id)
{
    uint8_t *entry;
    intptr_t fn;

    if (id >= env->exe.func_num) {
        env_set_error(env, ERR_SysError);
        return;
    }

    entry = env->exe.func_map[id];
    fn = function_create(env, entry);
    if (0 == fn) {
        env_set_error(env, ERR_SysError);
    } else {
        env_push_script(env, fn);
    }
}

#if 0
static inline void interp_show(uint8_t *pc, int sp) {
    const char *cmd;
    int param1, param2, n;

    n = bcode_parse(pc, NULL, &cmd, &param1, &param2);

    if (n == 0) {
        printf("[PC: %p, SP: %d] %s\n", pc, sp, cmd);
    } else
    if (n == 1) {
        printf("[PC: %p, SP: %d] %s %d\n", pc, sp, cmd, param1);
    } else {
        printf("[PC: %p, SP: %d] %s %d %d\n", pc, sp, cmd, param1, param2);
    }
}
#else
static inline void interp_show(uint8_t *pc, int sp) {}
#endif

static int interp_run(env_t *env, uint8_t *pc)
{
    int     index;

    while(!env->error) {
        uint8_t code;

        interp_show(pc, env->sp);
        code = *pc++;
        switch(code) {
        case BC_STOP:       goto DO_END;
        case BC_PASS:       break;

        /* Return instruction */
        case BC_RET0:       env_frame_restore(env, &pc, &env->scope);
                            env_push_undefined(env);
                            break;

        case BC_RET:        {
                                val_t *res = env_stack_peek(env);
                                env_frame_restore(env, &pc, &env->scope);
                                *env_stack_push(env) = *res;
                            }
                            break;

        /* Jump instruction */
        case BC_SJMP:       index = (int8_t) (*pc++); pc += index;
                            break;

        case BC_JMP:        index = (int8_t) (*pc++); index = (index << 8) | (*pc++); pc += index;
                            break;

        case BC_SJMP_T:     index = (int8_t) (*pc++);
                            if (val_is_true(env_stack_peek(env))) {
                                pc += index;
                            }
                            break;

        case BC_SJMP_F:     index = (int8_t) (*pc++);
                            if (!val_is_true(env_stack_peek(env))) {
                                pc += index;
                            }
                            break;

        case BC_JMP_T:      index = (int8_t) (*pc++); index = (index << 8) | (*pc++);
                            if (val_is_true(env_stack_peek(env))) {
                                pc += index;
                            }
                            break;
        case BC_JMP_F:      index = (int8_t) (*pc++); index = (index << 8) | (*pc++);
                            if (!val_is_true(env_stack_peek(env))) {
                                pc += index;
                            }
                            break;
        case BC_POP_SJMP_T: index = (int8_t) (*pc++);
                            if (val_is_true(env_stack_pop(env))) {
                                pc += index;
                            }
                            break;
        case BC_POP_SJMP_F: index = (int8_t) (*pc++);
                            if (!val_is_true(env_stack_pop(env))) {
                                pc += index;
                            }
                            break;
        case BC_POP_JMP_T:  index = (int8_t) (*pc++); index = (index << 8) | (*pc++);
                            if (val_is_true(env_stack_pop(env))) {
                                pc += index;
                            }
                            break;
        case BC_POP_JMP_F:  index = (int8_t) (*pc++); index = (index << 8) | (*pc++);
                            if (!val_is_true(env_stack_pop(env))) {
                                pc += index;
                            }
                            break;

        case BC_PUSH_UND:   env_push_undefined(env);  break;
        case BC_PUSH_NAN:   env_push_nan(env);        break;
        case BC_PUSH_TRUE:  env_push_boolean(env, 1); break;
        case BC_PUSH_FALSE: env_push_boolean(env, 0); break;
        case BC_PUSH_ZERO:  env_push_zero(env);  break;

        case BC_PUSH_NUM:   index = (*pc++); index = (index << 8) + (*pc++);
                            env_push_number(env, index);
                            break;

        case BC_PUSH_STR:   index = (*pc++); index = (index << 8) + (*pc++);
                            env_push_string(env, index);
                            break;

        case BC_PUSH_VAR:   index = (*pc++); env_push_var(env, index, *pc++);
                            break;

        case BC_PUSH_REF:   index = (*pc++); env_push_ref(env, index, *pc++);
                            break;

        case BC_PUSH_SCRIPT:index = (*pc++); index = (index << 8) | (*pc++);
                            interp_push_function(env, index);
                            break;

        case BC_PUSH_NATIVE:index = (*pc++); index = (index << 8) | (*pc++);
                            env_push_native(env, index);
                            break;

        case BC_POP:        env_stack_pop(env); break;

        case BC_NEG:        interp_neg(env); break;
        case BC_NOT:        interp_not(env); break;
        case BC_LOGIC_NOT:  interp_logic_not(env); break;

        case BC_MUL:        interp_mul(env); break;
        case BC_DIV:        interp_div(env); break;
        case BC_MOD:        interp_mod(env); break;
        case BC_ADD:        interp_add(env); break;
        case BC_SUB:        interp_sub(env); break;

        case BC_AAND:       interp_and(env); break;
        case BC_AOR:        interp_or(env);  break;
        case BC_AXOR:       interp_xor(env); break;

        case BC_LSHIFT:     interp_lshift(env); break;
        case BC_RSHIFT:     interp_rshift(env); break;

        case BC_TEQ:        interp_teq(env); break;
        case BC_TNE:        interp_tne(env); break;
        case BC_TGT:        interp_tgt(env); break;
        case BC_TGE:        interp_tge(env); break;
        case BC_TLT:        interp_tlt(env); break;
        case BC_TLE:        interp_tle(env); break;

        case BC_TIN:        env_set_error(env, ERR_InvalidByteCode); break;

        case BC_ASSIGN:     interp_assign(env); break;

        case BC_FUNC_CALL:  index = *pc++;
                            pc = interp_call(env, index, pc);
                            break;

        case BC_PROP:       interp_prop_get(env);  break;
        case BC_PROP_METH:  interp_prop_self(env); break;

        case BC_ELEM:       interp_elem_get(env);  break;
        case BC_ELEM_METH:  interp_elem_self(env); break;

        default:            env_set_error(env, ERR_InvalidByteCode);
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

val_t interp_execute_call(env_t *env, int ac)
{
    uint8_t stop = BC_STOP;
    uint8_t *pc;

    pc = interp_call(env, ac, &stop);
    if (pc != &stop) {
        // call a script function
        interp_run(env, pc);
    }

    if (env->error) {
        return val_mk_undefined();
    } else {
        return *env_stack_pop(env);
    }
}

int interp_execute(env_t *env, val_t **v)
{

    if (!env || !v) {
        return -1;
    }

    if (0 != interp_run(env, env_main_entry_setup(env, 0, NULL))) {
        return -env->error;
    }

    if (env->fp > env->sp) {
        *v = env_stack_pop(env);
    } else {
        *v = &undefined;
    }

    return 0;
}

int interp_execute_string(env_t *env, const char *input, val_t **v)
{
    stmt_t *stmt;
    heap_t *heap = env_heap_get_free((env_t*)env);
    parser_t psr;
    compile_t cpl;
    int error = 0;

    if (!env || !input || !v) {
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
        if (0 != interp_run(env, env_main_entry_setup(env, 0, NULL))) {
            //printf("execute error: %d\n", env->error);
            return -env->error;
        }
    } else {
        //printf("cmpile error: %d\n", cpl.error);
        return -cpl.error;
    }

    if (env->fp > env->sp) {
        *v = env_stack_pop(env);
    } else {
        *v = &undefined;
    }

    return error ? error : 1;
}

int interp_execute_interactive(env_t *env, const char *input, char *(*input_more)(void), val_t **v)
{
    stmt_t *stmt;
    parser_t psr;
    compile_t cpl;
    int stmt_type;
    heap_t *heap = env_heap_get_free((env_t*)env);

    if (!env || !input || !v) {
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
        if (0 != interp_run(env, env_main_entry_setup(env, 0, NULL))) {
            return -env->error;
        }
    } else {
        return -cpl.error;
    }

    if (env->fp > env->sp) {
        *v = env_stack_pop(env);
    } else {
        *v = &undefined;
    }

    return 1;
}

