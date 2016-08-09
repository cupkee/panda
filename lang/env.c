
#include "symtbl.h"
#include "env.h"
#include "object.h"

#define FRAME_SIZE (sizeof(frame_t) / sizeof(val_t))

typedef struct frame_t {
    int fp;
    int sp;
    intptr_t pc;
    intptr_t scope;
} frame_t;

static int env_scope_expand(env_t *env, scope_t *scope, int size)
{
    val_t *buf = (val_t *) env_heap_alloc(env, sizeof(val_t) * size);

    if (!buf) {
        env_set_error(env, ERR_NotEnoughMemory);
        return -1;
    }

    memcpy(buf, scope->var_buf, scope->num * sizeof(val_t));
    scope->var_buf = buf;
    scope->size = size;

    return 0;
}

int env_init(env_t *env, void *mem_ptr, int mem_size,
             void *heap_ptr, int heap_size, val_t *stack_ptr, int stack_size,
             int number_max, int string_max, int native_max, int func_max,
             int main_code_max, int func_code_max)
{
    int mem_offset;
    int half_size, exe_size;

    env->error = 0;

    // stack init
    if (!stack_ptr) {
        // alloc memory for stack
        stack_ptr = ADDR_ALIGN_8(mem_ptr);
        mem_offset = (intptr_t)stack_ptr - (intptr_t)mem_ptr;
        mem_offset += sizeof(val_t) * stack_size;
        // TODO: create a barrier here
    } else {
        mem_offset = 0;
    }
    env->sb = stack_ptr;
    env->ss = stack_size;
    env->sp = stack_size;
    env->fp = stack_size;

    // heap init
    if (heap_size % 16) {
        return -1;
    }

    if (!heap_ptr) {
        // alloc memory for heap
        heap_ptr = ADDR_ALIGN_16(mem_ptr + mem_offset);
        mem_offset += (intptr_t)heap_ptr - (intptr_t)(mem_ptr + mem_offset);
        mem_offset += heap_size;
    }
    half_size = heap_size / 2;
    heap_init(&env->heap_top, heap_ptr, half_size);
    heap_init(&env->heap_bot, heap_ptr + half_size, half_size);
    env->heap = &env->heap_top;

    // static memory init
    exe_size = executable_init(&env->exe, mem_ptr + mem_offset, mem_size - mem_offset,
                    number_max, string_max, native_max, func_max, main_code_max, func_code_max);
    if (exe_size < 0) {
        return -1;
    }

    env->result = NULL;

    if (0 != env_scope_create(env, NULL, DEF_MAIN_VAR_NUM, 0, NULL)) {
        return -1;
    }

    if (!(env->sym_tbl = symtbl_create())) {
        return -1;
    }

    if (0 != objects_env_init((env_t *)env)) {
        if (env->sym_tbl) symtbl_destroy(env->sym_tbl);
        return -1;
    } else {
        return 0;
    }
}

int env_deinit(env_t *env)
{
    if (!env) {
        return -1;
    }

    if (env->sym_tbl) symtbl_destroy(env->sym_tbl);

    return 0;
}

int env_scope_create(env_t *env, scope_t *super, int vc, int ac, val_t *av)
{
    scope_t *scope;
    val_t   *buf;
    int      i;

    if (!env) {
        return -1;
    }

    if (!(scope = (scope_t *) env_heap_alloc(env, sizeof(scope_t)))) {
        env_set_error(env, ERR_NotEnoughMemory);
        return -1;
    }

    if (!(buf = (val_t *) env_heap_alloc(env, sizeof(val_t) * vc))) {
        env_set_error(env, ERR_NotEnoughMemory);
        return -1;
    }

    for (i = 0; i < ac; i++) {
        buf[i] = av[i];
    }

    for (; i < vc; i++) {
        buf[i] = val_mk_undefined();
    }

    scope->type = 0x19;
    scope->num = 0;
    scope->size = vc;
    scope->super = super;
    scope->var_buf = buf;
    env->scope = scope;

    return 0;
}

int env_scope_extend(env_t *env, val_t *v)
{
    scope_t *scope;

    if (!env || !env->scope) {
        return -1;
    }

    scope = env->scope;
    if (scope->num >= scope->size && 0 != env_scope_expand(env, scope, scope->size * 2)) {
        return -1;
    }
    scope->var_buf[scope->num++] = *v;

    return scope->num;
}

int env_scope_extend_to(env_t *env, int size)
{
    scope_t *scope;
    int i;

    if (!env || !env->scope) {
        return -1;
    }

    scope = env->scope;
    if (size > scope->size && 0 != env_scope_expand(env, scope, size)) {
        return -1;
    }
    i = scope->num;
    while(i < size) {
        scope->var_buf[i++] = val_mk_undefined();
    }
    scope->num = i;

    return 0;
}

int env_scope_set(env_t *env, int id, val_t *v) {
    if (env && env->scope && id >= 0 && id < env->scope->num) {
        env->scope->var_buf[id] = *v;
        return 0;
    }
    return -1;
}

int env_scope_get(env_t *env, int id, val_t **v) {
    if (env && env->scope && id >= 0 && id < env->scope->num) {
        *v = env->scope->var_buf + id;
        return 0;
    }
    return -1;
}

intptr_t env_symbal_add(env_t *env, const char *name) {
    if (env) {
        return symtbl_add(env->sym_tbl, name);
    } else {
        return 0;
    }
}

intptr_t env_symbal_get(env_t *env, const char *name) {
    if (env) {
        return symtbl_get(env->sym_tbl, name);
    } else {
        return 0;
    }
}

int env_frame_setup(env_t *env, uint8_t *pc, scope_t *super, int vc, int ac, val_t *av)
{
    int fp;
    scope_t *scope = env->scope;
    frame_t *frame;

    if (env->sp < FRAME_SIZE) {
        env->error = ERR_SysError;
        return -1;
    }

    if (env_scope_create(env, super, vc, ac, av) != 0) {
        return -1;
    }

    fp = env->sp - FRAME_SIZE;
    frame = (frame_t *)(env->sb + fp);

    //printf ("############  resave sp: %d, fp: %d\n", env->sp, env->fp);
    frame->fp = env->fp;
    frame->sp = env->sp;
    frame->pc = (intptr_t) pc;
    frame->scope = (intptr_t) scope;

    env->fp = fp;
    env->sp = fp;

    return 0;
}

void env_frame_restore(env_t *env, uint8_t **pc, scope_t **scope)
{
    if (env->fp != env->ss) {
        frame_t *frame = (frame_t *)(env->sb + env->fp);

        //printf ("$$$$$$$$$$$$$$ restore sp: %d, fp: %d\n", frame->sp, frame->fp);
        env->sp = frame->sp;
        env->fp = frame->fp;
        *pc = (uint8_t *) frame->pc;
        *scope = (scope_t *) frame->scope;
    } else {
        *pc = NULL;
        *scope = NULL;
    }
}

void *env_heap_alloc(env_t *env, int size)
{
    void *ptr = heap_alloc(env->heap, size);

    if (!ptr) {
        env_heap_gc(env, 0);
        return heap_alloc(env->heap, size);
    }

    return ptr;
}

void env_heap_gc(env_t *env, int level)
{
    env_set_error(env, ERR_NotImplemented);
}

int env_number_find_add(env_t *env, double n)
{
    return executable_number_find_add(&env->exe, n);
}

int env_string_find_add(env_t *env, intptr_t s)
{
    return executable_string_find_add(&env->exe, s);
}

/*
int env_add_string(env_t *env, const char *s)
{
    int i, space, size;
    char *locate;

    for (i = 0; i < env->string_num; i++) {
        if (!strcmp((void *)env->string_map[i], s)) {
            return 0;
        }
    }

    if (i >= env->string_max) {
        return -1;
    }

    if (env->string_num) {
        int last = env->string_num - 1;

        locate = (void *)env->string_map[last] + strlen((void*)env->string_map[last]) + 1;
    } else {
        locate = env->symbal_buf;
    }

    space = (env->symbal_buf_size - (locate - env->symbal_buf));
    size = strlen(s) + 1;

    if (size < space) {
        return -1;
    }

    memcpy(locate, s, size);
    env->string_map[env->string_num++] = (intptr_t) locate;

    return 1;
}
*/

int env_native_add(env_t *env, const char *name, val_t (*fn)(env_t *, int ac, val_t *av))
{
    intptr_t sym_id;

    // Note: sym_id is a string point of symbal, should not be 0!
    if (env->error || 0 == (sym_id = symtbl_add(env->sym_tbl, name))) {
        env_set_error(env, ERR_SysError);
        return -1;
    }

    return executable_native_add(&env->exe, sym_id, (intptr_t) fn);
}

int env_native_find(env_t *env, intptr_t sym_id)
{
    return executable_native_find(&env->exe, sym_id);
}
