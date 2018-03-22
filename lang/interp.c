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

#include "err.h"
#include "val.h"
#include "bcode.h"
#include "parse.h"
#include "compile.h"
#include "interp.h"

#include "types.h"
#include "type_number.h"
#include "type_string.h"
#include "type_function.h"
#include "type_array.h"
#include "type_object.h"

static inline val_t *interp_var_ref(env_t *env, val_t *reg)
{
    if (val_is_reference(reg)) {
        uint8_t id, generation;
        val_2_reference(reg, &id, &generation);
        return env_get_var(env, id, generation);
    } else {
        return NULL;
    }
}

static inline void interp_op(env_t *env, val_opxx_t operate) {
    val_t *reg2 = env_stack_peek(env); // Note: keep in stack, deffence GC!
    val_t *reg1 = reg2 + 1;

    operate(env, reg1, reg2, reg1);

    env_stack_pop(env);
}

static inline void interp_op_self(env_t *env, val_opx_t operate) {
    val_t *reg = env_stack_peek(env);
    val_t *lft = interp_var_ref(env, reg);

    if (lft) {
        operate(env, lft, reg);
    } else {
        env_set_error(env, ERR_InvalidLeftValue);
    }
}

static inline void interp_prop_op_self(env_t *env, val_opx_t operate) {
    val_t *key = env_stack_peek(env); // keep the "key" in stack, defence GC
    val_t *obj = key + 1;
    val_t *res = obj;

    val_prop_opx(env, obj, key, res, operate);

    env_stack_pop(env);
}

static inline void interp_elem_op_self(env_t *env, val_opx_t operate) {
    val_t *key = env_stack_peek(env); // keep the "key" in stack, defence GC
    val_t *obj = key + 1;
    val_t *res = obj;

    val_prop_opx(env, obj, key, res, operate);

    env_stack_pop(env);
}

static inline void interp_op_set(env_t *env, val_opxx_t operate) {
    val_t *reg2 = env_stack_peek(env);
    val_t *reg1 = reg2 + 1;
    val_t *lft = interp_var_ref(env, reg1);
    if (lft) {
        operate(env, lft, reg2, lft);
        *reg1 = *lft;
        env_stack_pop(env);
    } else {
        env_set_error(env, ERR_InvalidLeftValue);
    }
}

static inline void interp_prop_op_set(env_t *env, val_opxx_t operate) {
    val_t *reg3 = env_stack_peek(env); // keep the "data" in stack, defence GC
    val_t *reg2 = reg3 + 1;
    val_t *reg1 = reg2 + 1;

    val_prop_opxx(env, reg1, reg2, reg3, reg1, operate);
    env_stack_release(env, 2);
}

static inline void interp_elem_op_set(env_t *env, val_opxx_t operate) {
    val_t *reg3 = env_stack_peek(env); // keep the "key" in stack, defence GC
    val_t *reg2 = reg3 + 1;
    val_t *reg1 = reg2 + 1;

    val_prop_opxx(env, reg1, reg2, reg3, reg1, operate);
    env_stack_release(env, 2);
}

static inline void interp_op_unary(env_t *env, val_opx_t operate) {
    val_t *reg = env_stack_peek(env);
    operate(env, reg, reg);
}

static inline void interp_logic_not(env_t *env) {
    val_t *reg = env_stack_peek(env);
    val_set_boolean(reg, !val_is_true(reg));
}

static inline void interp_teq(env_t *env) {
    val_t *reg2 = env_stack_pop(env);
    val_t *reg1 = reg2 + 1;

    val_set_boolean(reg1, val_is_equal(reg1, reg2));
}

static inline void interp_tne(env_t *env) {
    val_t *reg2 = env_stack_pop(env);
    val_t *reg1 = reg2 + 1;

    val_set_boolean(reg1, !val_is_equal(reg1, reg2));
}

static inline void interp_tgt(env_t *env) {
    val_t *reg2 = env_stack_pop(env);
    val_t *reg1 = reg2 + 1;

    val_set_boolean(reg1, val_is_gt(reg1, reg2));
}

static inline void interp_tge(env_t *env) {
    val_t *reg2 = env_stack_pop(env);
    val_t *reg1 = reg2 + 1;

    val_set_boolean(reg1, val_is_ge(reg1, reg2));
}

static inline void interp_tlt(env_t *env) {
    val_t *reg2 = env_stack_pop(env);
    val_t *reg1 = reg2 + 1;

    val_set_boolean(reg1, val_is_lt(reg1, reg2));
}

static inline void interp_tle(env_t *env) {
    val_t *reg2 = env_stack_pop(env);
    val_t *reg1 = reg2 + 1;

    val_set_boolean(reg1, val_is_le(reg1, reg2));
}

static inline void interp_set(env_t *env) {
    val_t *reg2 = env_stack_peek(env);
    val_t *reg1 = reg2 + 1;
    val_t *lft = interp_var_ref(env, reg1);


    if (lft) {
        if (!val_is_foreign(lft)) {
            *lft = *reg1 = *reg2;
        } else {
            *reg1 = foreign_set(env, lft, reg2);
        }
    } else {
        env_set_error(env, ERR_InvalidLeftValue);
    }
    env_stack_pop(env);
}

static inline const uint8_t *interp_call(env_t *env, int ac, const uint8_t *pc) {
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

static inline void interp_array_build(env_t *env, int n) {
    val_t *av = env_stack_peek(env);
    intptr_t array = array_create(env, n, av);

    if (array) {
        val_set_array(env_stack_release(env, n - 1), array);
    } else {
        val_set_undefined(env_stack_release(env, n - 1));
    }
}

static inline void interp_object_build(env_t *env, int n) {
    val_t *av = env_stack_peek(env);
    intptr_t dict = object_create(env, n, av);

    if (dict) {
        val_set_object(env_stack_release(env, n - 1), dict);
    } else {
        val_set_undefined(env_stack_release(env, n - 1));
    }
}

static inline void interp_prop_get(env_t *env) {
    val_t *key  = env_stack_peek(env);
    val_t *self = key + 1;
    val_t *prop = self;

    val_prop_get(env, self, key, prop);
    env_stack_pop(env);
}

static inline void interp_elem_get(env_t *env) {
    val_t *key = env_stack_peek(env);
    val_t *self = key + 1;
    val_t *prop = self;

    val_prop_get(env, self, key, prop);
    env_stack_pop(env);
}

static inline void interp_prop_set(env_t *env) {
    val_t *val = env_stack_peek(env); // keep the "key" in stack, defence GC
    val_t *key = val + 1;
    val_t *obj = key + 1;
    val_t *res = obj;

    val_prop_set(env, obj, key, val);
    *res = *val;

    env_stack_release(env, 2);
}

static inline void interp_elem_set(env_t *env) {
    val_t *val = env_stack_peek(env); // keep the "key" in stack, defence GC
    val_t *key = val + 1;
    val_t *obj = key + 1;
    val_t *res = obj;

    val_prop_set(env, obj, key, val);
    *res = *val;

    env_stack_release(env, 2);
}

static inline void interp_prop_meth(env_t *env) {
    val_t *key = env_stack_peek(env);
    val_t *self = key + 1;
    val_t *prop = key;

    val_prop_get(env, self, key, prop);
    // No pop, to leave self in stack
}

static inline void interp_elem_meth(env_t *env) {
    val_t *key = env_stack_peek(env);
    val_t *self = key + 1;
    val_t *prop = key;

    val_prop_get(env, self, key, prop);
    // No pop, to leave self in stack
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
#define __INTERP_SHOW__
static inline void interp_show(const uint8_t *pc, int sp) {
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
#endif

static int interp_run(env_t *env, const uint8_t *pc)
{
    int     index;

    while(!env->error) {
        uint8_t code;

#if defined(__INTERP_SHOW__)
        interp_show(pc, env->sp);
#endif
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

        case BC_NEG:        interp_op_unary(env, val_neg); break;
        case BC_NOT:        interp_op_unary(env, val_not); break;
        case BC_LOGIC_NOT:  interp_logic_not(env); break;

        case BC_MUL:        interp_op(env, val_mul); break;
        case BC_DIV:        interp_op(env, val_div); break;
        case BC_MOD:        interp_op(env, val_mod); break;
        case BC_ADD:        interp_op(env, val_add); break;
        case BC_SUB:        interp_op(env, val_sub); break;

        case BC_AAND:       interp_op(env, val_and); break;
        case BC_AOR:        interp_op(env, val_or);  break;
        case BC_AXOR:       interp_op(env, val_xor); break;

        case BC_LSHIFT:     interp_op(env, val_lshift); break;
        case BC_RSHIFT:     interp_op(env, val_rshift); break;

        case BC_TEQ:        interp_teq(env); break;
        case BC_TNE:        interp_tne(env); break;
        case BC_TGT:        interp_tgt(env); break;
        case BC_TGE:        interp_tge(env); break;
        case BC_TLT:        interp_tlt(env); break;
        case BC_TLE:        interp_tle(env); break;

        case BC_TIN:        env_set_error(env, ERR_InvalidByteCode); break;

        case BC_PROP:               interp_prop_get(env);  break;
        case BC_PROP_METH:          interp_prop_meth(env); break;
        case BC_ELEM:               interp_elem_get(env);  break;
        case BC_ELEM_METH:          interp_elem_meth(env); break;

        case BC_INC:                interp_op_self(env, val_inc); break;
        case BC_INCP:               interp_op_self(env, val_incp); break;
        case BC_DEC:                interp_op_self(env, val_dec); break;
        case BC_DECP:               interp_op_self(env, val_decp); break;

        case BC_ASSIGN:             interp_set(env); break;

        case BC_ADD_ASSIGN:         interp_op_set(env, val_add); break;
        case BC_SUB_ASSIGN:         interp_op_set(env, val_sub); break;
        case BC_MUL_ASSIGN:         interp_op_set(env, val_mul); break;
        case BC_DIV_ASSIGN:         interp_op_set(env, val_div); break;
        case BC_MOD_ASSIGN:         interp_op_set(env, val_mod); break;
        case BC_AND_ASSIGN:         interp_op_set(env, val_and); break;
        case BC_OR_ASSIGN:          interp_op_set(env, val_or); break;
        case BC_XOR_ASSIGN:         interp_op_set(env, val_xor); break;
        case BC_LSHIFT_ASSIGN:      interp_op_set(env, val_lshift); break;
        case BC_RSHIFT_ASSIGN:      interp_op_set(env, val_rshift); break;

        case BC_PROP_INC:           interp_prop_op_self(env, val_inc); break;
        case BC_PROP_INCP:          interp_prop_op_self(env, val_incp); break;
        case BC_PROP_DEC:           interp_prop_op_self(env, val_dec); break;
        case BC_PROP_DECP:          interp_prop_op_self(env, val_decp); break;
        case BC_PROP_ASSIGN:        interp_prop_set(env); break;

        case BC_PROP_ADD_ASSIGN:    interp_prop_op_set(env, val_add); break;
        case BC_PROP_SUB_ASSIGN:    interp_prop_op_set(env, val_sub); break;
        case BC_PROP_MUL_ASSIGN:    interp_prop_op_set(env, val_mul); break;
        case BC_PROP_DIV_ASSIGN:    interp_prop_op_set(env, val_div); break;
        case BC_PROP_MOD_ASSIGN:    interp_prop_op_set(env, val_mod); break;
        case BC_PROP_AND_ASSIGN:    interp_prop_op_set(env, val_and); break;
        case BC_PROP_OR_ASSIGN:     interp_prop_op_set(env, val_or); break;
        case BC_PROP_XOR_ASSIGN:    interp_prop_op_set(env, val_xor); break;
        case BC_PROP_LSHIFT_ASSIGN: interp_prop_op_set(env, val_lshift); break;
        case BC_PROP_RSHIFT_ASSIGN: interp_prop_op_set(env, val_rshift); break;

        case BC_ELEM_INC:           interp_elem_op_self(env, val_inc); break;
        case BC_ELEM_INCP:          interp_elem_op_self(env, val_incp); break;
        case BC_ELEM_DEC:           interp_elem_op_self(env, val_dec); break;
        case BC_ELEM_DECP:          interp_elem_op_self(env, val_decp); break;

        case BC_ELEM_ASSIGN:        interp_elem_set(env); break;

        case BC_ELEM_ADD_ASSIGN:    interp_elem_op_set(env, val_add); break;
        case BC_ELEM_SUB_ASSIGN:    interp_elem_op_set(env, val_sub); break;
        case BC_ELEM_MUL_ASSIGN:    interp_elem_op_set(env, val_mul); break;
        case BC_ELEM_DIV_ASSIGN:    interp_elem_op_set(env, val_div); break;
        case BC_ELEM_MOD_ASSIGN:    interp_elem_op_set(env, val_mod); break;
        case BC_ELEM_AND_ASSIGN:    interp_elem_op_set(env, val_and); break;
        case BC_ELEM_OR_ASSIGN:     interp_elem_op_set(env, val_or); break;
        case BC_ELEM_XOR_ASSIGN:    interp_elem_op_set(env, val_xor); break;
        case BC_ELEM_LSHIFT_ASSIGN: interp_elem_op_set(env, val_lshift); break;
        case BC_ELEM_RSHIFT_ASSIGN: interp_elem_op_set(env, val_rshift); break;

        case BC_FUNC_CALL:  index = *pc++;
                            pc = interp_call(env, index, pc);
                            break;

        case BC_ARRAY:      index = (*pc++); index = (index << 8) | (*pc++);
                            interp_array_build(env, index); break;

        case BC_DICT:       index = (*pc++); index = (index << 8) | (*pc++);
                            interp_object_build(env, index); break;

        default:            env_set_error(env, ERR_InvalidByteCode);
        }
    }
DO_END:
    return -env->error;
}

static inline void interp_reset_parser_heap(env_t *env, parser_t *psr)
{
    heap_t *heap = env_heap_get_free((env_t*)env);

    heap_init(&psr->heap, heap->base, heap->size);
}

static void parse_callback(void *u, parse_event_t *e)
{
    (void) u;
    (void) e;
}

int interp_env_init_interactive(env_t *env, void *mem_ptr, int mem_size, void *heap_ptr, int heap_size, val_t *stack_ptr, int stack_size)
{
    int exe_mem_size = mem_size;
    int exe_num_max, exe_str_max, exe_fn_max, exe_code_max;

    if (!heap_ptr) {
        exe_mem_size -= heap_size;
    }

    if (!stack_ptr) {
        exe_mem_size -= stack_size * sizeof(val_t);
    }

    if (env_exe_memery_distribute(exe_mem_size, &exe_num_max, &exe_str_max, &exe_fn_max, &exe_code_max)) {
        return -ERR_NotEnoughMemory;
    }

    return env_init(env, mem_ptr, mem_size,
                heap_ptr, heap_size, stack_ptr, stack_size,
                exe_num_max, exe_str_max, exe_fn_max,
                exe_code_max, 1);
}

int interp_env_init_interpreter(env_t *env, void *mem_ptr, int mem_size, void *heap_ptr, int heap_size, val_t *stack_ptr, int stack_size)
{
    int exe_mem_size = mem_size;
    int exe_num_max, exe_str_max, exe_fn_max, exe_code_max;

    if (!heap_ptr) {
        exe_mem_size -= heap_size;
    }

    if (!stack_ptr) {
        exe_mem_size -= stack_size * sizeof(val_t);
    }

    if (env_exe_memery_distribute(exe_mem_size, &exe_num_max, &exe_str_max, &exe_fn_max, &exe_code_max)) {
        return -ERR_NotEnoughMemory;
    }

    return env_init(env, mem_ptr, mem_size,
                heap_ptr, heap_size, stack_ptr, stack_size,
                exe_num_max, exe_str_max, exe_fn_max,
                exe_code_max, 0);
}

int interp_env_init_image(env_t *env, void *mem_ptr, int mem_size, void *heap_ptr, int heap_size, val_t *stack_ptr, int stack_size, image_info_t *image)
{
    unsigned int i;
    int exe_mem_size, exe_str_max, exe_fn_max;
    executable_t *exe;

    if (!image || image->byte_order != SYS_BYTE_ORDER) {
        return -1;
    }

    exe_mem_size = mem_size - heap_size - stack_size * sizeof(val_t);
    if (env_exe_memery_distribute(exe_mem_size, NULL, &exe_str_max, &exe_fn_max, NULL)) {
        return -ERR_NotEnoughMemory;
    }

    if ((unsigned)exe_str_max < image->str_cnt) {
        exe_str_max = image->str_cnt;
    }
    if ((unsigned)exe_fn_max < image->fn_cnt) {
        exe_fn_max = image->fn_cnt;
    }

    if (0 != env_init(env, mem_ptr, mem_size,
                    heap_ptr, heap_size, stack_ptr, stack_size,
                    0, exe_str_max, exe_fn_max, 0, 0)) {
        return -1;
    }

    exe = &env->exe;
    exe->number_map = image_number_entry(image);
    exe->number_num = image->num_cnt;

    exe->string_num = image->str_cnt;
    for (i = 0; i < image->str_cnt; i++) {
        exe->string_map[i] = (intptr_t)image_get_string(image, i);
    }

    exe->func_num = image->fn_cnt;
    for (i = 0; i < image->fn_cnt; i++) {
        exe->func_map[i] = (uint8_t *)image_get_function(image, i);
    }

    return 0;
}

val_t interp_execute_call(env_t *env, int ac)
{
    uint8_t stop = BC_STOP;
    const uint8_t *pc;

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

int interp_execute_image(env_t *env, val_t **v)
{

    if (!env || !v) {
        return -ERR_InvalidInput;
    }

    if (0 != interp_run(env, env_main_entry_setup(env, 0, NULL))) {
        return -env->error;
    }

    if (env->fp > env->sp) {
        *v = env_stack_pop(env);
    } else {
        *v = NULL;
    }

    return 0;
}

int interp_execute_string(env_t *env, const char *input, val_t **v)
{
    stmt_t *stmt;
    heap_t *heap = env_heap_get_free((env_t*)env);
    parser_t psr;
    compile_t cpl;

    if (!env || !input || !v) {
        return -1;
    }

    // The free heap can be used for parse and compile process
    parse_init(&psr, input, NULL, heap->base, heap->size);
    parse_set_cb(&psr, parse_callback, NULL);
    stmt = parse_stmt_multi(&psr);
    if (!stmt) {
        //printf("parse error: %d\n", psr.error);
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
        *v = NULL;
    }

    return 1;
}

int interp_execute_interactive(env_t *env, const char *input, char *(*input_more)(void), val_t **v)
{
    stmt_t *stmt;
    parser_t psr;
    compile_t cpl;
    heap_t *heap = env_heap_get_free((env_t*)env);

    if (!env || !input || !v) {
        return -1;
    }

    // The free heap can be used for parse and compile process
    parse_init(&psr, input, input_more, heap->base, heap->size);
    parse_set_cb(&psr, parse_callback, NULL);
    stmt = parse_stmt(&psr);
    if (!stmt) {
        return psr.error ? -psr.error : 0;
    }

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
        *v = NULL;
    }

    return 1;
}

int interp_execute_stmts(env_t *env, const char *input, val_t **v)
{
    stmt_t *stmt;
    parser_t psr;
    compile_t cpl;
    heap_t *heap = env_heap_get_free((env_t*)env);

    if (!env || !input || !v) {
        return -1;
    }

    // The free heap can be used for parse and compile process
    parse_init(&psr, input, NULL, heap->base, heap->size);
    parse_set_cb(&psr, parse_callback, NULL);

    stmt = parse_stmt(&psr);
    while (stmt) {
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
            *v = NULL;
        }

        interp_reset_parser_heap(env, &psr);
        stmt = parse_stmt(&psr);
    }

    return 1;
}

