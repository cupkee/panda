
#include "env.h"
#include "object.h"
#include "string.h"
#include "function.h"

#define VACATED     (-1)
#define FRAME_SIZE  (sizeof(frame_t) / sizeof(val_t))

typedef struct frame_t {
    int fp;
    int sp;
    intptr_t pc;
    intptr_t scope;
} frame_t;

static uint32_t hash_pjw(const void *key)
{
    const char *ptr = key;
    uint32_t val = 0;

    while (*ptr) {
        uint32_t tmp;

        val = (val << 4) + *ptr;
        tmp = val & 0xf0000000;
        if (tmp) {
            val = (val ^ (tmp >> 24)) ^ tmp;
        }

        ptr++;
    }

    return val;
}

static inline uint32_t htbl_key(uint32_t size, uint32_t hash, int i) {
    return hash + i * (hash * 2 + 1);
}

static inline int scope_mem_space(scope_t *scope) {
    return SIZE_ALIGN(sizeof(scope_t)) + SIZE_ALIGN(sizeof(val_t) * scope->size);
}

static inline char *env_symbal_buf_alloc(env_t *env, int size)
{
    if (env->symbal_buf_used + size < env->symbal_buf_end) {
        char *p = env->symbal_buf + env->symbal_buf_used;
        env->symbal_buf_used += size;
        return p;
    }
    return NULL;
}

static char *env_symbal_put(env_t *env, const char *str)
{
    int size = strlen(str) + 1;
    char *sym = env_symbal_buf_alloc(env, size);

    if (sym) {
        memcpy(sym, str, size);
    }
    return sym;
}

static int env_symbal_lookup(env_t *env, const char *symbal, char **res)
{
    uint32_t size, pos, i, hash;
    intptr_t *tbl = env->symbal_tbl;


    if (env->symbal_tbl_hold == 0) {
        return 0;
    }

    size = env->symbal_tbl_size;
    hash = hash_pjw(symbal);

    for (i = 0; i < size; i++) {
        pos = htbl_key(size, hash, i) % size;

        if (tbl[pos] == 0) {
            break;
        }
        if ((intptr_t)symbal == tbl[pos] || !strcmp(symbal, (char*)tbl[pos])) {
            if (res) *res = (char *)tbl[pos];
            return 1;
        }
    }

    return 0;
}

static intptr_t env_symbal_insert(env_t *env, const char *symbal)
{
    uint32_t size, pos, i, hash;
    intptr_t *tbl = env->symbal_tbl;
    char *p;

    if (env_symbal_lookup(env, symbal, &p) == 1) {
        return (intptr_t)p;
    }

    size = env->symbal_tbl_size;
    if (env->symbal_tbl_hold >= size) {
        env_set_error(env, ERR_ResourceOutLimit);
        return 0;
    }

    hash = hash_pjw(symbal);
    for (i = 0; i < size; i++) {
        pos = htbl_key(size, hash, i) % size;

        if (tbl[pos] == 0 || tbl[pos] == VACATED) {
            p = env_symbal_put(env, symbal);
            tbl[pos] = (intptr_t)p;
            env->symbal_tbl_hold++;
            return (intptr_t)p;
        }
    }

    return 0;
}

intptr_t env_symbal_add(env_t *env, const char *name) {
    return env_symbal_insert(env, name);
}

intptr_t env_symbal_get(env_t *env, const char *name) {
    char *p;
    if (1 == env_symbal_lookup(env, name, &p)) {
        return (intptr_t)p;
    }
    return 0;
}

int env_init(env_t *env, void *mem_ptr, int mem_size,
             void *heap_ptr, int heap_size, val_t *stack_ptr, int stack_size,
             int number_max, int string_max, int native_max, int func_max,
             int main_code_max, int func_code_max, int interactive)
{
    int mem_offset;
    int half_size, exe_size, symbal_tbl_size;

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
    if (!heap_ptr) {
        // alloc memory for heap
        heap_size = SIZE_ALIGN_16(heap_size);
        heap_ptr = ADDR_ALIGN_16(mem_ptr + mem_offset);
        mem_offset += (intptr_t)heap_ptr - (intptr_t)(mem_ptr + mem_offset);
        mem_offset += heap_size;
    } else {
        if (heap_size % 16 || ((intptr_t)heap_ptr & 0xf)) {
            // heap should be align 16!
            return -1;
        }
    }
    half_size = heap_size / 2;
    heap_init(&env->heap_top, heap_ptr, half_size);
    heap_init(&env->heap_bot, heap_ptr + half_size, half_size);
    env->heap = &env->heap_top;

    // main_var_map init
    if (interactive) {
        env->main_var_map = (intptr_t *)(mem_ptr + mem_offset);
        mem_offset += sizeof(intptr_t) * INTERACTIVE_VAR_MAX;
    } else {
        env->main_var_map = NULL;
    }
    env->main_var_num = 0;

    // native init
    env->native_max = native_max;
    env->native_num = 0;
    env->native_map = (intptr_t *) (mem_ptr + mem_offset);
    mem_offset += sizeof(intptr_t) * native_max;
    env->native_entry = (intptr_t *) (mem_ptr + mem_offset);
    mem_offset += sizeof(intptr_t) * native_max;

    // static memory init
    exe_size = executable_init(&env->exe, mem_ptr + mem_offset, mem_size - mem_offset,
                    number_max, string_max, func_max, main_code_max, func_code_max);
    if (exe_size < 0) {
        return -1;
    }
    mem_offset += exe_size;

    symbal_tbl_size = EXE_STRING_MAX * sizeof(intptr_t);
    if (mem_offset + symbal_tbl_size > mem_size) {
        return -1;
    }
    env->symbal_tbl = (intptr_t *) (mem_ptr + mem_offset);
    env->symbal_tbl_size = EXE_STRING_MAX;
    env->symbal_tbl_hold = 0;
    memset(mem_ptr + mem_offset, 0, symbal_tbl_size);
    mem_offset += symbal_tbl_size;

    env->symbal_buf = mem_ptr + mem_offset;
    env->symbal_buf_end = mem_size - mem_offset;
    env->symbal_buf_used = 0;

#if 0
    printf("memory total: %d\n", mem_size);
    printf("stack: %d\n", mem_offset - exe_size - heap_size);
    printf("heap:  %d\n", heap_size);
    printf("exe:   %d\n", exe_size);
    printf("left:  %d\n", mem_size - mem_offset);
#endif

    env->scope = NULL;
    if (interactive && (NULL == (env->scope = env_scope_create(env, NULL, INTERACTIVE_VAR_MAX, 0, NULL)))) {
        return -1;
    }

    env->result = NULL;

    if (0 != objects_env_init(env)) {
        return -1;
    } else {
        return 0;
    }
}

int env_deinit(env_t *env)
{
    return 0;
}

scope_t *env_scope_create(env_t *env, scope_t *super, int vc, int ac, val_t *av)
{
    scope_t *scope;
    val_t   *buf;
    int      i;

    if (!env) {
        return NULL;
    }

    if (!(scope = (scope_t *) env_heap_alloc(env, sizeof(scope_t) + sizeof(val_t) * vc))) {
        env_set_error(env, ERR_NotEnoughMemory);
        return NULL;
    }
    buf = (val_t *) (scope + 1);

    for (i = 0; i < ac; i++) {
        buf[i] = av[i];
    }

    for (; i < vc; i++) {
        buf[i] = val_mk_undefined();
    }

    scope->magic = MAGIC_SCOPE;
    scope->num = 0;
    scope->size = vc;
    scope->super = super;
    scope->var_buf = buf;

    return scope;
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

int env_entry_setup(env_t *env, uint8_t *entry, int ac, val_t *av, uint8_t **pc)
{
    function_info_t info;
    scope_t *scope;

    function_info_read(entry, &info);

    if (env_is_interactive(env)) {
        // main scope already created, if in interactive mode
        scope = env->scope;
    } else {
        scope = env_scope_create(env, NULL, info.var_num, ac, av);
    }

    if (!scope) {
        return -1;
    }
    env->scope = scope;
    *pc = info.code;

    return 0;
}

int env_frame_setup(env_t *env, uint8_t *pc, scope_t *super, int vc, int ac, val_t *av)
{
    scope_t *scope = env->scope;
    frame_t *frame;
    int fp;

    if (env->sp < FRAME_SIZE) {
        env->error = ERR_StackOverflow;
        return -1;
    }

    if (NULL == (scope = env_scope_create(env, super, vc, ac, av))) {
        return -1;
    }

    //skip arguments & function
    env->sp += ac + 1;

    fp = env->sp - FRAME_SIZE;
    frame = (frame_t *)(env->sb + fp);

    //printf ("############  resave sp: %d, fp: %d\n", env->sp, env->fp);
    frame->fp = env->fp;
    frame->sp = env->sp;
    frame->pc = (intptr_t) pc;
    frame->scope = (intptr_t) env->scope;

    env->fp = fp;
    env->sp = fp;
    env->scope = scope;

    return 0;
}

void env_frame_restore(env_t *env, uint8_t **pc, scope_t **scope)
{
    if (env->fp != env->ss) {
        frame_t *frame = (frame_t *)(env->sb + env->fp);

        env->sp = frame->sp;
        env->fp = frame->fp;
        *pc = (uint8_t *) frame->pc;
        *scope = (scope_t *) frame->scope;
    } else {
        *pc = NULL;
        *scope = NULL;
    }
}

int env_native_frame_setup(env_t *env, int ac)
{
    frame_t *frame;
    int fp;

    if (env->sp < FRAME_SIZE) {
        env->error = ERR_StackOverflow;
        return -1;
    }

    // keep arguments & function in stack
    fp = env->sp - FRAME_SIZE;
    frame = (frame_t *)(env->sb + fp);
    frame->fp = env->fp;
    frame->sp = env->sp + ac + 1; // skip arguments & function when return
    frame->pc = 0;
    frame->scope = (intptr_t) env->scope;

    env->fp = fp;
    env->sp = fp;
    env->scope = NULL;

    return 0;
}

void env_native_return(env_t *env, val_t res)
{
    frame_t *frame = (frame_t *)(env->sb + env->fp);

    env->sp = frame->sp;
    env->fp = frame->fp;
    env->scope = (scope_t*) frame->scope;

    *env_stack_push(env) = res;
}

void *env_heap_alloc(env_t *env, int size)
{
    void *ptr = heap_alloc(env->heap, size);

    if (!ptr) {
        env_heap_gc(env, size);
        return heap_alloc(env->heap, size);
    }

    return ptr;
}

#define MAGIC_BYTE(x) (*((uint8_t *)(x)))
#define ADDR_VALUE(x) (*((void **)(x)))

static scope_t *heap_dup_scope(heap_t *heap, scope_t *scope)
{
    scope_t *dup = heap_alloc(heap, sizeof(scope_t));
    val_t   *buf = heap_alloc(heap, sizeof(val_t) * scope->size);

    //printf("%s: free %d\n", __func__, heap->free);
    memcpy(dup, scope, sizeof(scope_t));
    memcpy(buf, scope->var_buf, sizeof(val_t) * scope->size);
    dup->var_buf = buf;

    ADDR_VALUE(scope) = dup;
    return dup;
}

static intptr_t heap_dup_string(heap_t *heap, intptr_t str)
{
    int size = string_mem_space(str);
    void *dup = heap_alloc(heap, size);

    //printf("%s: free %d, %d, %s\n", __func__, heap->free, size, (char *)(str + 3));
    memcpy(dup, (void*)str, size);

    ADDR_VALUE(str) = dup;
    return (intptr_t) dup;
}

static intptr_t heap_dup_function(heap_t *heap, intptr_t func)
{
    function_t *dup = heap_alloc(heap, sizeof(function_t));

    //printf("%s: free %d\n", __func__, heap->free);
    memcpy(dup, (void*)func, sizeof(function_t));

    ADDR_VALUE(func) = dup;
    return (intptr_t) dup;
}

static scope_t *env_heap_copy_scope(heap_t *heap, scope_t *scope)
{
    if (!scope || heap_is_owned(heap, scope)) {
        //printf("[scope is nil or owned: %p]", scope);
        return scope;
    }

    if (MAGIC_BYTE(scope) != MAGIC_SCOPE) {
        //printf("[scope had copy to: %p]", ADDR_VALUE(scope));
        return ADDR_VALUE(scope);
    }
    //scope_t *dup = heap_dup_scope(heap, scope);
    //printf("[scope(%p) copy to: %p]", scope, dup);
    //return dup;

    return heap_dup_scope(heap, scope);
}

static intptr_t env_heap_copy_string(heap_t *heap, intptr_t str)
{
    if (!str || heap_is_owned(heap, (void*)str)) {
        //printf("[string is nil or owned: %lx]", str);
        return (intptr_t) str;
    }

    if (MAGIC_BYTE(str) != MAGIC_STRING) {
        //printf("[string had copy to: %p]", ADDR_VALUE(str));
        return (intptr_t) ADDR_VALUE(str);
    }
    //intptr_t dup = heap_dup_string(heap, str);
    //printf("[string(%lx) copy to: %lx]", str, dup);
    //return dup;

    return heap_dup_string(heap, str);
}

static intptr_t env_heap_copy_function(heap_t *heap, intptr_t func)
{
    if (!func || heap_is_owned(heap, (void *)func)) {
        //printf("[fn is nil or owned: %lx]", func);
        return (intptr_t) func;
    }

    if (MAGIC_BYTE(func) != MAGIC_FUNCTION) {
        //printf("[fn had copy to: %p]", ADDR_VALUE(func));
        return (intptr_t) ADDR_VALUE(func);
    }
    //intptr_t dup = heap_dup_function(heap, func);
    //printf("[fn(%lx) copy to: %lx]", func, dup);
    //return dup;

    return heap_dup_function(heap, func);
}

static void env_heap_copy_vals(heap_t *heap, int vc, val_t *vp)
{
    int i = 0;

    //printf("%s(%p, %d, %p)\n", __func__, heap, vc, vp);

    while (i < vc) {
        val_t *v = vp + i;

        if (val_is_owned_string(v)) {
            //printf("val[%d] s %llx reset to ", i, *v);
            val_set_owned_string(v, env_heap_copy_string(heap, val_2_intptr(v)));
            //printf("%llx\n", *v);
        } else
        if (val_is_script(v)) {
            //printf("val[%d] f %llx reset to ", i, *v);
            val_set_script(v, env_heap_copy_function(heap, val_2_intptr(v)));
            //printf("%llx\n", *v);
        }
        i++;
    }
}

static void env_heap_gc_init(env_t *env)
{
    heap_t  *heap = env_heap_get_free(env);
    val_t   *sb;
    int fp, sp, ss;

    heap_reset(heap);
    env->scope = env_heap_copy_scope(heap, env->scope);

    fp = env->fp, sp = env->sp, ss = env->ss;
    sb = env->sb;
    while (1) {
        if (fp == ss) {
            env_heap_copy_vals(heap, fp - sp, sb + sp);
            break;
        } else {
            frame_t *frame = (frame_t *)(sb + fp);

            frame->scope = (intptr_t)env_heap_copy_scope(heap, (scope_t *)frame->scope);
            env_heap_copy_vals(heap, fp - sp, sb + sp);

            fp = frame->fp;
            sp = frame->sp;
        }
    }
}

static void env_heap_gc_scan(env_t *env)
{
    heap_t *heap = env_heap_get_free(env);
    uint8_t*base = heap->base;
    int     scan = 0;

    while(scan < heap->free) {
        uint8_t magic = base[scan];

        switch(magic) {
        case MAGIC_STRING:
            scan += string_mem_space((intptr_t)(base + scan));
            break;
        case MAGIC_FUNCTION: {
            function_t *func = (function_t *)(base + scan);
            func->super = env_heap_copy_scope(heap, func->super);

            scan += function_mem_space(func);
        } break;
        case MAGIC_SCOPE: {
            scope_t *scope = (scope_t *) (base + scan);

            env_heap_copy_vals(heap, scope->size, scope->var_buf);
            scope->super = env_heap_copy_scope(heap, scope->super);

            scan += scope_mem_space(scope);
        } break;
        }
    }
}

void env_heap_gc(env_t *env, int level)
{
    env_heap_gc_init(env);
    env_heap_gc_scan(env);

    // Todo: this line is not useable looked, but will cause test fail if deleted! Fix it!
    heap_reset(env->heap);

    env->heap = env_heap_get_free(env);
}

int env_number_find_add(env_t *env, double n)
{
    return executable_number_find_add(&env->exe, n);
}

int env_string_find_add(env_t *env, intptr_t s)
{
    return executable_string_find_add(&env->exe, s);
}

int env_native_find(env_t *env, intptr_t sym_id)
{
    int i;

    for (i = 0; i < env->native_num; i++) {
        if (sym_id == env->native_map[i]) {
            return i;
        }
    }

    return -1;
}

int env_native_add(env_t *env, const char *name, val_t (*fn)(env_t *, int ac, val_t *av))
{
    intptr_t sym_id;
    int i;

    // Note: sym_id is a string point of symbal, should not be 0!
    if (env->error || 0 == (sym_id = env_symbal_add(env, name))) {
        env_set_error(env, ERR_SysError);
        return -1;
    }


    for (i = 0; i < env->native_num; i++) {
        if (sym_id == env->native_map[i]) {
            // already exist!
            return 0;
        }
    }

    if (i >= env->native_max) {
        return -1;
    }

    env->native_map[i] = sym_id;
    env->native_entry[i] = (intptr_t)fn;

    return env->native_num++;
}

