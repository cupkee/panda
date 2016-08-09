
#include "err.h"
#include "bcode.h"
#include "compile.h"

void compile_code_dump(compile_t *cpl);

static inline intptr_t *compile_mem_head(void *mem) {
    return (intptr_t *) (((unsigned long)mem) - sizeof(intptr_t) * 2);
}

static inline void swap(intptr_t *buf, uint32_t a, uint32_t b) {
    intptr_t tmp = buf[a];

    buf[a] = buf[b];
    buf[b] = tmp;
}

static void max_heapify(intptr_t *buf, uint32_t start, uint32_t end)
{
    uint32_t dad = start;
    uint32_t son = dad * 2 + 1;

    while(son < end) {
        // choice the max
        if (son + 1 < end && buf[son] < buf[son + 1]) {
            son ++;
        }

        if (buf[son] > buf[dad]) {
            swap(buf, dad, son);
            dad = son;
            son = dad * 2 + 1;
        } else {
            return;
        }
    }
}

static void heap_sort(intptr_t *buf, int num)
{
    int i;

    for (i = num / 2 - 1; i >= 0; i--) {
        max_heapify(buf, i, num);
    }
    for (i = num - 1; i > 0; i--) {
        swap(buf, 0, i);
        max_heapify(buf, 0, i);
    }
}

static void compile_gc(compile_t *cpl)
{
    intptr_t *keep_tbl = (intptr_t *)(cpl->heap.base + cpl->heap.free);
    int keep_num = cpl->func_num * 2 + 1;
    intptr_t *head;
    int i, n, free = 0;

    keep_tbl[0] = (intptr_t) compile_mem_head(cpl->func_buf);
    for (i = 0, n = 1; i < cpl->func_num; i++) {
        keep_tbl[n++] = (intptr_t)compile_mem_head(cpl->func_buf[i].var_map);
        keep_tbl[n++] = (intptr_t)compile_mem_head(cpl->func_buf[i].code_buf);
    }

    // compute new address
    heap_sort(keep_tbl, keep_num);

    for (i = 0; i < keep_num; i++) {
        head = (intptr_t *)(keep_tbl[i]);

        head[1] = (intptr_t)cpl->heap.base + free;
        free += head[0];
    }

    // update memory reference
    head = compile_mem_head(cpl->func_buf);
    cpl->func_buf = (compile_func_t *) (head[1] + sizeof(intptr_t) * 2);
    for (i = 0; i < cpl->func_num; i++) {
        head = compile_mem_head(cpl->func_buf[i].var_map);
        cpl->func_buf[i].var_map = (intptr_t *)(head[1] + sizeof(intptr_t) * 2);

        head = compile_mem_head(cpl->func_buf[i].code_buf);
        cpl->func_buf[i].code_buf = (uint8_t *)(head[1] + sizeof(intptr_t) * 2);
    }

    // move
    for (i = 0; i < keep_num; i++) {
        intptr_t *head = (intptr_t *)(keep_tbl[i]);
        memmove((void*)(head[1]), head, head[0]);
    }

    //printf("compile gc: %d -> %d\n", cpl->heap.free, free);
    cpl->heap.free = free;
}

static void *compile_malloc(compile_t *cpl, int size)
{
    int keep_size = sizeof(intptr_t) * (cpl->func_num * 2 + 1);
    void *p;

    size += sizeof(intptr_t) * 2;
    size = SIZE_ALIGN(size);

    //printf("compile alloc: %d, size: %u, free: %u\n", size, cpl->heap.size, cpl->heap.free);
    if (cpl->heap.free + size + keep_size > cpl->heap.size) {
        compile_gc(cpl);
        if (cpl->heap.free + size + keep_size > cpl->heap.size) {
            return NULL;
        }
    }

    p = cpl->heap.base + cpl->heap.free;
    cpl->heap.free += size;
    *((intptr_t*)p) = size;

    return p + sizeof(intptr_t) * 2;
}

static inline compile_func_t *compile_func_cur(compile_t *cpl) {
    return cpl->func_buf + cpl->func_cur;
}

static inline uint8_t *compile_code_buf(compile_t *cpl) {
    return cpl->func_buf[cpl->func_cur].code_buf;
}

static inline int compile_code_pos(compile_t *cpl) {
    return cpl->func_buf[cpl->func_cur].code_num;
}

static int compile_extend_size(compile_t *cpl, int size, int used, int extend, int limit, int def)
{
    int res;
    //ASSERT(used <= size);

    if (used + extend <= size) {
        return 0;
    }

    if (size == 0) {
        return extend < def ? def : extend;
    }

    if (used + extend > limit) {
        cpl->error = ERR_ResourceOutLimit;
        return -1;
    }


    res = size * 2;
    return used + extend < res ? res : used + extend + 1;
}

static inline int compile_number_find_add(compile_t *cpl, double n)
{
    return env_number_find_add(cpl->env, n);
}

static inline int compile_string_find_add(compile_t *cpl, intptr_t s)
{
    return env_string_find_add(cpl->env, s);
}

static inline int compile_native_lookup(compile_t *cpl, intptr_t sym_id)
{
    return env_native_find(cpl->env, sym_id);
}

static int compile_func_check_extend(compile_t *cpl, int space)
{
    int size;

    if (0 < (size = compile_extend_size(cpl, cpl->func_size, cpl->func_num, space,
                                LIMIT_FUNC_SIZE, DEF_FUNC_SIZE))) {
        compile_func_t *ptr;

        if (NULL == (ptr = (compile_func_t*) compile_malloc(cpl, size * sizeof(compile_func_t)))) {
            cpl->error = ERR_NotEnoughMemory;
            return -1;
        }

        if (cpl->func_buf) {
            memcpy(ptr, cpl->func_buf, sizeof(compile_func_t) * cpl->func_num);
        }

        cpl->func_buf = ptr;
        cpl->func_size = size;
    }
    return size;
}

static int compile_func_append(compile_t *cpl, int owner)
{
    int func_id;

    if (cpl->error || 0 > compile_func_check_extend(cpl, 1)) {
        return -1;
    }

    func_id = cpl->func_num++;
    cpl->func_buf[func_id].owner = owner;
    cpl->func_buf[func_id].var_max = 0;
    cpl->func_buf[func_id].var_num = 0;
    cpl->func_buf[func_id].arg_num = 0;
    cpl->func_buf[func_id].code_max = 0;
    cpl->func_buf[func_id].code_num = 0;
    cpl->func_buf[func_id].code_buf = NULL;
    cpl->func_buf[func_id].var_map = NULL;

    return func_id;
}

static int compile_varmap_check_extend(compile_t *cpl, int space)
{
    compile_func_t *func;
    int size;

    func = cpl->func_buf + cpl->func_cur;
    if (0 < (size = compile_extend_size(cpl, func->var_max, func->var_num, space,
                                 LIMIT_VMAP_SIZE, DEF_VMAP_SIZE))) {
        intptr_t *ptr;
        if (NULL == (ptr = (intptr_t *) compile_malloc(cpl,size * sizeof(intptr_t)))) {
            cpl->error = ERR_NotEnoughMemory;
            return -1;
        }

        if (func->var_map) {
            memcpy(ptr, func->var_map, func->var_num * sizeof(intptr_t));
        }

        func->var_map = ptr;
        func->var_max = size;
    }
    return size;
}

// return: id of variable or -1
static int compile_varmap_find_add(compile_t *cpl, intptr_t sym_id)
{
    compile_func_t *func;
    int i, num;

    // Note: sym_id is a string point of symbal, should not be 0!
    if (cpl->error || sym_id == 0) {
        return -1;
    }

    func = compile_func_cur(cpl);
    num = func->var_num;
    for (i = 0; i < num; i++) {
        if (sym_id == func->var_map[i]) return i; // already exist!
    }

    if (0 > compile_varmap_check_extend(cpl, 1)) {
        return -1;
    }

    func->var_map[func->var_num++] = sym_id;

    return i;
}

static int compile_varmap_lookup(compile_t *cpl, intptr_t sym_id)
{
    compile_func_t *func;
    int i, num;

    // Note: sym_id is a string point of symbal, should not be 0!
    if (cpl->error || sym_id == 0) {
        return -1;
    }

    func = compile_func_cur(cpl);
    num = func->var_num;
    for (i = 0; i < num; i++) {
        if (sym_id == func->var_map[i]) return i; // already exist!
    }

    return -1;
}

static inline int compile_varmap_lookup_name(compile_t *cpl, const char *name) {
    return compile_varmap_lookup(cpl, compile_sym_find(cpl, name));
}

static inline int compile_var_add_name(compile_t *cpl, const char *name) {
    return compile_var_add(cpl, compile_sym_add(cpl, name));
}

static inline int compile_arg_add_name(compile_t *cpl, const char *name) {
    return compile_arg_add(cpl, compile_sym_add(cpl, name));
}

static int compile_code_check_extend(compile_t *cpl, int space)
{
    compile_func_t *func;
    int size;

    func = cpl->func_buf + cpl->func_cur;
    if (0 < (size = compile_extend_size(cpl, func->code_max, func->code_num, space,
                                 LIMIT_FUNC_CODE_SIZE, DEF_FUNC_CODE_SIZE))) {
        uint8_t *ptr;
        if (NULL == (ptr = (uint8_t *) compile_malloc(cpl,size))) {
            cpl->error = ERR_NotEnoughMemory;
            return -1;
        }

        if (func->code_buf) {
            memcpy(ptr, func->code_buf, func->code_num);
        }

        func->code_buf = ptr;
        func->code_max = size;
    }
    return size;
}

static inline int compile_code_append(compile_t *cpl, uint8_t code)
{
    compile_func_t *func;

    if (cpl->error || 0 > compile_code_check_extend(cpl, 1)) {
        return 1;
    }

    func = cpl->func_buf + cpl->func_cur;
    func->code_buf[func->code_num++] = code;

    return 0;
}

static inline int compile_code_appends(compile_t *cpl, int n, uint8_t *code)
{
    compile_func_t *func;
    int i;

    if (cpl->error || 0 > compile_code_check_extend(cpl, n)) {
        return 1;
    }

    func = cpl->func_buf + cpl->func_cur;
    for (i = 0; i < n; i++) {
        func->code_buf[func->code_num++] = code[i];
    }

    return 0;
}

static int compile_code_extend(compile_t *cpl, int bytes)
{
    compile_func_t *func = cpl->func_buf + cpl->func_cur;

    if (cpl->error || 0 > compile_code_check_extend(cpl, bytes)) {
        return 1;
    }

    func->code_num += bytes;

    return 0;
}

static int compile_code_insert(compile_t *cpl, int pos, int bytes)
{
    compile_func_t *func = cpl->func_buf + cpl->func_cur;
    int i, n;

    if (cpl->error || 0 > compile_code_check_extend(cpl, bytes)) {
        return 1;
    }

    n = func->code_num - pos;
    for (i = 1; i <= n; i++) {
        func->code_buf[func->code_num + bytes - i] = func->code_buf[func->code_num - i];
    }
    func->code_num += bytes;

    return 0;
}

static void compile_code_append_num(compile_t *cpl, double n)
{
    int id;

    if (n == 0) {
        compile_code_append(cpl, BC_PUSH_ZERO);
        return;
    }

    if (0 > (id = compile_number_find_add(cpl, n))) {
        cpl->error = ERR_SysError;
        return;
    }

    compile_code_append(cpl, BC_PUSH_NUM);
    compile_code_append(cpl, id >> 8);
    compile_code_append(cpl, id);
}

static void compile_code_append_str(compile_t *cpl, const char *s)
{
    int id;

    if (0 > (id = compile_string_find_add(cpl, compile_sym_add(cpl, s)))) {
        cpl->error = ERR_SysError;
        return;
    }

    compile_code_append(cpl, BC_PUSH_STR);
    compile_code_append(cpl, id >> 8);
    compile_code_append(cpl, id);
}

static inline int compile_code_append_func(compile_t *cpl, int func_id)
{
    compile_func_t *func;

    if (cpl->error || 0 > compile_code_check_extend(cpl, 3)) {
        return 1;
    }

    func = cpl->func_buf + cpl->func_cur;
    func->code_buf[func->code_num++] = BC_PUSH_SCRIPT;
    func->code_buf[func->code_num++] = func_id >> 8;
    func->code_buf[func->code_num++] = func_id;

    return 0;
}

static inline int compile_code_append_native(compile_t *cpl, int id)
{
    compile_func_t *func;

    if (cpl->error || 0 > compile_code_check_extend(cpl, 3)) {
        return 1;
    }

    func = cpl->func_buf + cpl->func_cur;
    func->code_buf[func->code_num++] = BC_PUSH_NATIVE;
    func->code_buf[func->code_num++] = id >> 8;
    func->code_buf[func->code_num++] = id;

    return 0;
}

static inline int compile_code_append_var(compile_t *cpl, int id)
{
    compile_func_t *func;

    if (cpl->error || 0 > compile_code_check_extend(cpl, 2)) {
        return 1;
    }

    func = cpl->func_buf + cpl->func_cur;
    func->code_buf[func->code_num++] = BC_PUSH_VAR;
    func->code_buf[func->code_num++] = id;

    return 0;
}

static inline int compile_code_append_call(compile_t *cpl, int ac)
{
    compile_func_t *func;

    if (cpl->error || 0 > compile_code_check_extend(cpl, 2)) {
        return 1;
    }

    func = cpl->func_buf + cpl->func_cur;
    func->code_buf[func->code_num++] = BC_FUNC_CALL;
    func->code_buf[func->code_num++] = ac;

    return 0;
}

static void compile_expr(compile_t *cpl, expr_t *e);

static void compile_code_set_jmp(compile_t *cpl, int pos, uint8_t jmp, int step)
{
    uint8_t *buf = compile_code_buf(cpl) + pos;

    if (step > 32767 || step < -32768) {
        cpl->error = 5555;
    }
    buf[0] = jmp;
    buf[1] = step >> 8;
    buf[2] = step;
}

static void compile_code_append_jmp(compile_t *cpl, uint8_t jmp, int step)
{
    int pos = compile_code_pos(cpl);

    if (0 == compile_code_extend(cpl, 3)) {
        compile_code_set_jmp(cpl, pos, jmp, step);
    }
}

/*
static void compile_code_insert_jmp(compile_t *cpl, int pos, uint8_t jmp, int step)
{
    if (0 == compile_code_insert(cpl, pos, 3)) {
        uint8_t *code_buf = compile_code_buf(cpl);
        code_buf[pos] = jmp;
        code_buf[pos+1] = step >> 8;
        code_buf[pos+2] = step;
    }
}
*/

static void compile_code_insert_xjmp(compile_t *cpl, int from, int step)
{
    int jmps = (step < -128 || step > 127) ? 3 : 2;

    if (0 == compile_code_insert(cpl, from, jmps)) {
        uint8_t *code_buf = compile_code_buf(cpl);
        if (jmps == 3) {
            code_buf[from++] = BC_JMP;
            code_buf[from++] = step >> 8;
        } else {
            code_buf[from++] = BC_SJMP;
        }
        code_buf[from++] = step;
    }
}

static inline void compile_code_insert_jmp_to(compile_t *cpl, int from, int to)
{
    return compile_code_insert_xjmp(cpl, from, to - from);
}

static void compile_tt_jmp_pop(compile_t *cpl, int from, int to)
{
    int step = to - from + 1; // one POP instruction

    if (!cpl->error && 0 == compile_code_insert(cpl, from, step > 127 ? 4 : 3)) {
        uint8_t *code_buf = compile_code_buf(cpl);
        if (step > 127) {
            code_buf[from++] = BC_JMP_T;
            code_buf[from++] = step >> 8;
        } else {
            code_buf[from++] = BC_SJMP_T;
        }
        code_buf[from++] = step;
        code_buf[from++] = BC_POP;
    }
}

static void compile_nt_jmp_pop(compile_t *cpl, int from, int to)
{
    int step = to - from + 1; // one POP instruction

    if (!cpl->error && 0 == compile_code_insert(cpl, from, step > 127 ? 4 : 3)) {
        uint8_t *code_buf = compile_code_buf(cpl);
        if (step > 127) {
            code_buf[from++] = BC_JMP_F;
            code_buf[from++] = step >> 8;
        } else {
            code_buf[from++] = BC_SJMP_F;
        }
        code_buf[from++] = step;
        code_buf[from++] = BC_POP;
    }
}

static void compile_pop_nt_jmp(compile_t *cpl, int from, int to)
{
    int step = to - from;

    if (!cpl->error && 0 == compile_code_insert(cpl, from, step > 127 ? 3 : 2)) {
        uint8_t *code_buf = compile_code_buf(cpl);
        if (step > 127) {
            code_buf[from++] = BC_JMP_F_POP;
            code_buf[from++] = step >> 8;
        } else {
            code_buf[from++] = BC_SJMP_F_POP;
        }
        code_buf[from++] = step;
    }
}

static void compile_expr_binary(compile_t *cpl, expr_t *e, uint8_t code)
{
    compile_expr(cpl, ast_expr_lft(e));
    if (e->type == EXPR_PROP) {
        if (ast_expr_rht(e)->type == EXPR_ID) {
            compile_code_append_str(cpl, ast_expr_text(ast_expr_rht(e)));
        } else {
            cpl->error = ERR_InvalidSyntax;
        }
    } else {
        compile_expr(cpl, ast_expr_rht(e));
    }
    compile_code_append(cpl, code);
}

static void compile_expr_logic_and(compile_t *cpl, expr_t *e)
{
    int pos;
    compile_expr(cpl, ast_expr_lft(e)); pos = compile_code_pos(cpl);
    compile_expr(cpl, ast_expr_rht(e));
    compile_nt_jmp_pop(cpl, pos, compile_code_pos(cpl));
}

static void compile_expr_logic_or(compile_t *cpl, expr_t *e)
{
    int pos;
    compile_expr(cpl, ast_expr_lft(e)); pos = compile_code_pos(cpl);
    compile_expr(cpl, ast_expr_rht(e));
    compile_tt_jmp_pop(cpl, pos, compile_code_pos(cpl));
}

void compile_expr_id(compile_t *cpl, expr_t *e)
{
    intptr_t sym_id = compile_sym_add(cpl, ast_expr_text(e));
    int id = compile_varmap_lookup(cpl, sym_id);

    if (id >= 0) {
        compile_code_append_var(cpl, id);
    } else {
        id = compile_native_lookup(cpl, sym_id);

        if (id >= 0) {
            compile_code_append_native(cpl, id);
        } else {
            cpl->error = ERR_NotDefinedId;
        }
    }
}

static void compile_expr_lft(compile_t *cpl, expr_t *e)
{
    switch (e->type) {
    case EXPR_ID: {
                    int var_id = compile_varmap_lookup_name(cpl, ast_expr_text(e));
                    if (var_id < 0) {
                        cpl->error = ERR_NotDefinedId;
                    } else {
                        uint8_t code[2];
                        code[0] = BC_PUSH_VAR_REF;
                        code[1] = var_id;
                        compile_code_appends(cpl, 2, code);
                    }
                  }
                  break;
    case EXPR_PROP: cpl->error = ERR_NotImplemented; break;
    case EXPR_ELEM: cpl->error = ERR_NotImplemented; break;
    default:        cpl->error = ERR_InvalidSyntax;
    }
}

static int compile_var_def(compile_t *cpl, expr_t *e)
{
    if (e->type == EXPR_ID) {
        return compile_var_add_name(cpl, ast_expr_text(e));
    }

    while(e->type == EXPR_ASSIGN) {
        expr_t *lft = ast_expr_lft(e);
        if (lft->type == EXPR_ID && 0 > compile_var_add_name(cpl, ast_expr_text(lft))) {
            return -1;
        }

        e = ast_expr_rht(e);
    }

    return 0;
}

static int compile_arg_def(compile_t *cpl, expr_t *e)
{
    if (e->type == EXPR_ID) {
        return compile_arg_add_name(cpl, ast_expr_text(e));
    }

    if (e->type == EXPR_ASSIGN) {
        expr_t *lft = ast_expr_lft(e);
        if (lft->type != EXPR_ID) {
            cpl->error = ERR_InvalidSyntax;
            return -1;
        }
        return compile_arg_add_name(cpl, ast_expr_text(lft));
    }

    cpl->error = ERR_InvalidSyntax;
    return -1;
}

static void compile_arg_def_list(compile_t *cpl, expr_t *e)
{
    while (!cpl->error && e) {
        expr_t *next;
        if (e->type == EXPR_COMMA) {
            next = ast_expr_rht(e);
            e    = ast_expr_lft(e);
        } else {
            next = NULL;
        }

        if (0 > compile_arg_def(cpl, e)) {
            break;
        }

        e = next;
    }
}

static void compile_arg_list(compile_t *cpl, expr_t *e, int *ac)
{
    if(!cpl->error && e) {
        if (e->type != EXPR_COMMA) {
            *ac += 1;
            compile_expr(cpl, e);
        } else {
            compile_arg_list(cpl, ast_expr_rht(e), ac);
            compile_arg_list(cpl, ast_expr_lft(e), ac);
        }
    }
}

static void compile_stmt_block(compile_t *cpl, stmt_t *s)
{
    while(s && !cpl->error) {
        compile_stmt(cpl, s);
        s = s->next;
    }
}

static void compile_func_def(compile_t *cpl, expr_t *e)
{
    int owner, curr, func_id;
    expr_t *args, *name;
    stmt_t *block;

    name = ast_expr_lft(e) ? ast_expr_lft(ast_expr_lft(e)) : NULL;
    args = ast_expr_lft(e) ? ast_expr_rht(ast_expr_lft(e)) : NULL;
    block = ast_expr_rht(e) ? ast_expr_stmt(ast_expr_rht(e)) : NULL;

    owner = cpl->func_cur;
    if (0 > (curr = compile_func_append(cpl, owner))) {
        return;
    }
    cpl->func_cur = curr;
    compile_arg_def_list(cpl, args);
    compile_stmt_block(cpl, block);
    cpl->func_cur = owner;

    func_id = curr + cpl->func_offset;
    if (name) {
        compile_var_def(cpl, name);
        compile_expr_lft(cpl, name);
        compile_code_append_func(cpl, func_id);
        compile_code_append(cpl, BC_ASSIGN);
    } else {
        compile_code_append_func(cpl, func_id);
    }
}

static void compile_callor(compile_t *cpl, expr_t *e, int argc)
{
    if (e->type == EXPR_PROP) {
        compile_expr_binary(cpl, e, BC_PROP_METH);
        argc += 1; // insert sefl at first of arguments
    } else
    if (e->type == EXPR_ELEM) {
        compile_expr_binary(cpl, e, BC_ELEM_METH);
        argc += 1; // insert sefl at first of arguments
    } else {
        compile_expr(cpl, e);
    }
    compile_code_append_call(cpl, argc);
}

static void compile_func_call(compile_t *cpl, expr_t *e)
{
    int argc = 0;
    expr_t *args, *func;

    func = ast_expr_lft(e);
    args = ast_expr_rht(e);

    compile_arg_list(cpl, args, &argc);
    compile_callor(cpl, func, argc);
}

static void compile_expr(compile_t *cpl, expr_t *e)
{
    if (cpl->error) {
        return;
    }

    switch (e->type) {
    case EXPR_ID:       compile_expr_id(cpl, e); break;
    case EXPR_NAN:      compile_code_append(cpl, BC_PUSH_NAN); break;
    case EXPR_UND:      compile_code_append(cpl, BC_PUSH_UND); break;
    case EXPR_NUM:      compile_code_append_num(cpl, ast_expr_num(e)); break;
    case EXPR_TRUE:     compile_code_append(cpl, BC_PUSH_TRUE); break;
    case EXPR_FALSE:    compile_code_append(cpl, BC_PUSH_FALSE); break;
    case EXPR_FUNCDEF:  compile_func_def(cpl, e); break;
    case EXPR_STRING:   compile_code_append_str(cpl, ast_expr_text(e)); break;

    case EXPR_NEG:      compile_expr(cpl, ast_expr_lft(e)); compile_code_append(cpl, BC_NEG); break;
    case EXPR_NOT:      compile_expr(cpl, ast_expr_lft(e)); compile_code_append(cpl, BC_NOT); break;
    case EXPR_LOGIC_NOT:compile_expr(cpl, ast_expr_lft(e)); compile_code_append(cpl, BC_LOGIC_NOT); break;
    case EXPR_ARRAY:    cpl->error = ERR_NotImplemented; break;
    case EXPR_DICT:     cpl->error = ERR_NotImplemented; break;

    case EXPR_MUL:      compile_expr_binary(cpl, e, BC_MUL); break;
    case EXPR_DIV:      compile_expr_binary(cpl, e, BC_DIV); break;
    case EXPR_MOD:      compile_expr_binary(cpl, e, BC_MOD); break;
    case EXPR_ADD:      compile_expr_binary(cpl, e, BC_ADD); break;
    case EXPR_SUB:      compile_expr_binary(cpl, e, BC_SUB); break;

    case EXPR_AND:      compile_expr_binary(cpl, e, BC_AAND); break;
    case EXPR_OR:       compile_expr_binary(cpl, e, BC_AOR); break;
    case EXPR_XOR:      compile_expr_binary(cpl, e, BC_AXOR); break;

    case EXPR_LSHIFT:   compile_expr_binary(cpl, e, BC_LSHIFT); break;
    case EXPR_RSHIFT:   compile_expr_binary(cpl, e, BC_RSHIFT); break;

    case EXPR_TEQ:      compile_expr_binary(cpl, e, BC_TEQ); break;
    case EXPR_TNE:      compile_expr_binary(cpl, e, BC_TNE); break;
    case EXPR_TGT:      compile_expr_binary(cpl, e, BC_TGT); break;
    case EXPR_TGE:      compile_expr_binary(cpl, e, BC_TGE); break;
    case EXPR_TLT:      compile_expr_binary(cpl, e, BC_TLT); break;
    case EXPR_TLE:      compile_expr_binary(cpl, e, BC_TLE); break;
    case EXPR_TIN:      compile_expr_binary(cpl, e, BC_TIN); break;

    case EXPR_LOGIC_AND:compile_expr_logic_and(cpl, e); break;
    case EXPR_LOGIC_OR: compile_expr_logic_or(cpl, e); break;

    case EXPR_CALL:     compile_func_call(cpl, e); break;
    case EXPR_PROP:     compile_expr_binary(cpl, e, BC_PROP); break;
    case EXPR_ELEM:     compile_expr_binary(cpl, e, BC_ELEM); break;

    case EXPR_ASSIGN:   compile_expr_lft(cpl, ast_expr_lft(e));
                        compile_expr(cpl, ast_expr_rht(e));
                        compile_code_append(cpl, BC_ASSIGN); break;

    case EXPR_COMMA:    compile_expr(cpl, ast_expr_lft(e)); compile_code_append(cpl, BC_POP);
                        compile_expr(cpl, ast_expr_rht(e)); break;
    case EXPR_TERNARY:  {
                            int pos1, pos2, end;
                            compile_expr(cpl, ast_expr_lft(e)); pos1 = compile_code_pos(cpl);
                            compile_expr(cpl, ast_expr_lft(ast_expr_rht(e))); pos2 = compile_code_pos(cpl);
                            compile_expr(cpl, ast_expr_rht(ast_expr_rht(e))); end = compile_code_pos(cpl);

                            compile_code_insert_jmp_to(cpl, pos2, end);
                            compile_pop_nt_jmp(cpl, pos1, pos2 + (compile_code_pos(cpl) - end));
                        }
                        break;
    default:            cpl->error = ERR_NotImplemented; break;
    }
}

static void compile_stmt_expr(compile_t *cpl, stmt_t *s)
{
    compile_expr(cpl, s->expr);
    compile_code_append(cpl, BC_POP_RESULT);
}

static void compile_stmt_return(compile_t *cpl, stmt_t *s)
{
    if (s->expr) {
        compile_expr(cpl, s->expr);
        compile_code_append(cpl, BC_RET);
    } else {
        compile_code_append(cpl, BC_RET0);
    }
}

static void compile_stmt_var(compile_t *cpl, stmt_t *s)
{
    expr_t *e = s->expr;

    while (!cpl->error && e) {
        expr_t *next;

        if (e->type == EXPR_COMMA) {
            next = ast_expr_rht(e);
            e    = ast_expr_lft(e);
        } else {
            next = NULL;
        }

        if (-1 == compile_var_def(cpl, e)) {
            cpl->error = ERR_NotEnoughMemory;
            break;
        }

        if (e->type == EXPR_ASSIGN) {
            compile_expr(cpl, e);
            compile_code_append(cpl, BC_POP);
        }

        e = next;
    }
}

/****************************************************************
 *                        if else form
 *
 * Begin:       +------------+
 *              |  condition +
 * test:        + ---------- +
 *         +----|  test jmp  |
 * Block:  |    + ---------- +
 *         |    |            |
 *         |    |   Block    |
 * skip:   |    + ---------- +
 *         |    |    skip    |----+
 * Other:  +--->+ ---------- +    |
 *              |            |    |
 *              |   Other    |    |
 *              |            |    |
 * End:         + ---------- +<---+
 ***************************************************************/
static void compile_stmt_cond(compile_t *cpl, stmt_t *s)
{
    int test_pos, skip_pos, block, other;

    compile_expr(cpl, s->expr);
    test_pos = compile_code_pos(cpl);

    compile_code_extend(cpl, 3);
    block = compile_code_pos(cpl);

    compile_stmt_block(cpl, s->block);
    other = compile_code_pos(cpl);
    if (s->other) {
        skip_pos = other;
        compile_code_extend(cpl, 3);

        other = compile_code_pos(cpl);
        compile_stmt_block(cpl, s->other);

        compile_code_set_jmp(cpl, skip_pos, BC_JMP, compile_code_pos(cpl) - other);
    }

    compile_code_set_jmp(cpl, test_pos, BC_JMP_F_POP, other - block);
}

/****************************************************************
 *                        Loop form
 *
 * Begin:       +------------+ <---------------------+   <------+
 *              |  condition +                       |          |
 *              + ---------- +                       |          |
 *              | JMP 3 Step | -- is true -+         |          |
 *              |  (2 BYTE)  |             |         |          |
 * skip:        + ---------- + <-----------|----+    |          |
 *         +--- |   JMP to   |             |    |    |          |
 *         |    |  LoopEnd   |             |    |    |          |
 *         |    |  (3 BYTE)  |             |    |    |        Total
 *         |    + ---------- + <-----------+    |    |   <--+   |
 *         |    |            |                  |    |      |   |
 *         |    | statements | -- break --------+    |      |   |
 *         |    |            | -- continue ----------+    Block |
 * End:    |    + ---------- +                       |      |   |
 *         |    | JMP Begin  | ----------------------+      |   |
 * LoopEnd +--> +------------+                           <--+---+
 ***************************************************************/
static void compile_stmt_while(compile_t *cpl, stmt_t *s)
{
    int bgn, skip, end, total, block, bgn_bk, skip_bk;
    uint8_t code[2] = {BC_SJMP_T_POP, 3};

    bgn = compile_code_pos(cpl);
    compile_expr(cpl, s->expr);
    compile_code_appends(cpl, 2, code);

    skip = compile_code_pos(cpl);
    compile_code_extend(cpl, 3);

    // Set begin and skip position, and save old/super position
    // used for statements compile of break & continue
    bgn_bk = cpl->bgn_pos; skip_bk = cpl->skip_pos;
    cpl->bgn_pos = bgn;    cpl->skip_pos = skip;

    compile_stmt_block(cpl, s->block); if (cpl->error) return;

    // Restore the begin and skip position
    cpl->bgn_pos = bgn_bk;
    cpl->skip_pos = skip_bk;

    end = compile_code_pos(cpl);
    total = end - bgn + 3;
    compile_code_append_jmp(cpl, BC_JMP, -total);

    block = total - (skip - bgn + 3);
    compile_code_set_jmp(cpl, skip, BC_JMP, block);
}

static void compile_stmt_break(compile_t *cpl, stmt_t *s)
{
    int bgn, end, total;

    bgn = cpl->skip_pos;
    end = compile_code_pos(cpl);
    total = end - bgn + 3;

    compile_code_append_jmp(cpl, BC_JMP, -total);
}

static void compile_stmt_continue(compile_t *cpl, stmt_t *s)
{
    int bgn, end, total;

    bgn = cpl->bgn_pos;
    end = compile_code_pos(cpl);
    total = end - bgn + 3;
    compile_code_append_jmp(cpl, BC_JMP, -total);
}

int compile_init(compile_t *cpl, env_t *env, void *heap_ptr, int heap_size)
{
    cpl->error = 0;

    cpl->func_size = 0;
    cpl->func_cur = 0;
    cpl->func_num = 0;
    cpl->func_buf = NULL;
    cpl->func_offset = env->exe.func_num;

    cpl->env = env;

    heap_init(&cpl->heap, heap_ptr, heap_size);

    return compile_func_append(cpl, -1); // should return zero;
}

int compile_deinit(compile_t *cpl)
{
    if (cpl) {
        int i;
        //if (cpl->func_buf) {
        if (0) {
            for (i = 0; i < cpl->func_num; i++) {
                compile_func_t *f = cpl->func_buf + i;

                if (f->var_map) free(f->var_map);
                if (f->code_buf) free(f->code_buf);
            }

            free(cpl->func_buf);
        }
        return 0;
    }
    return -1;
}

int compile_arg_add(compile_t *cpl, intptr_t sym_id)
{
    int var_id, var_max;
    if (!cpl || cpl->error) {
        return -1;
    }

    // Should not add local variable before argument !
    if (compile_arg_num(cpl) != compile_var_num(cpl)) {
        return -1;
    }

    var_max = compile_var_num(cpl);
    var_id  = compile_varmap_find_add(cpl, sym_id);
    if (var_id < var_max) {
        // named arguments should not be redefined!
        return -1;
    }
    compile_func_cur(cpl)->arg_num++;

    return 0;
}

int compile_var_add(compile_t *cpl, intptr_t sym_id)
{
    int var_id, var_max;
    if (!cpl || cpl->error) {
        return -1;
    }

    var_max = compile_var_num(cpl);
    var_id  = compile_varmap_find_add(cpl, sym_id);
    // -1 is invalid id, and
    // named arguments should not be redefined!
    if (var_id < compile_arg_num(cpl)) {
        // named arguments should not be redefined!
        return -1;
    }

    // 0 : redefined variable
    // 1 : new variable
    return var_id < var_max ? 0 : 1;
}

int compile_var_get(compile_t *cpl, intptr_t sym_id)
{
    return (!cpl || !sym_id) ? -1 : compile_varmap_lookup(cpl, sym_id);
}

int compile_vmap_copy(compile_t *cpl, intptr_t *buf, int size)
{
    compile_func_t *f = compile_func_cur(cpl);
    int i;

    if (f->var_num > size) {
        return -1;
    }

    for (i = 0; i < f->var_num; i++) {
        buf[i] = f->var_map[i];
    }

    return i;
}

int compile_stmt(compile_t *cpl, stmt_t *stmt)
{
    if (!cpl || !stmt) {
        return -1;
    }

    if (cpl->error) {
        return -cpl->error;
    }

    switch(stmt->type) {
    case STMT_PASS: break;
    case STMT_EXPR: compile_stmt_expr(cpl, stmt); break;
    case STMT_VAR:  compile_stmt_var(cpl, stmt); break;
    case STMT_IF:   compile_stmt_cond(cpl, stmt); break;
    case STMT_WHILE:    compile_stmt_while(cpl, stmt); break;
    case STMT_BREAK:    compile_stmt_break(cpl, stmt); break;
    case STMT_CONTINUE: compile_stmt_continue(cpl, stmt); break;
    case STMT_RET:  compile_stmt_return(cpl, stmt); break;
    default: cpl->error = ERR_NotImplemented;
    }

    return -cpl->error;
}

int compile_one_stmt(compile_t *cpl, stmt_t *stmt)
{
    int ret = compile_stmt(cpl, stmt);

    if (ret == 0) {
        //compile_code_dump(cpl);
        return compile_code_append(cpl, BC_STOP);
    }

    return ret;
}

int compile_code_relocate(compile_t *cpl)
{
    executable_t   *exe;
    compile_func_t *fp;
    int i;

    if (!cpl || cpl->error || !cpl->env) {
        return -1;
    }

    exe = &cpl->env->exe;
    fp  = cpl->func_buf;

    if (exe->main_code_end + fp->code_num >= exe->main_code_max) {
        cpl->error = ERR_NotEnoughMemory;
        return -1;
    }
    exe->func_map[0] = exe->main_code + exe->main_code_end;
    memcpy(exe->main_code + exe->main_code_end, fp->code_buf, fp->code_num);
    exe->main_code_end += fp->code_num;

    for (i = 1; i < cpl->func_num; i++) {
        fp = cpl->func_buf + i;

        if (exe->func_code_end + fp->code_num >= exe->func_code_max) {
            cpl->error = ERR_NotEnoughMemory;
            return -1;
        }

        exe->func_map[++exe->func_num] = exe->func_code + exe->func_code_end;
        exe->func_code[exe->func_code_end++] = fp->var_num;
        exe->func_code[exe->func_code_end++] = fp->arg_num;
        exe->func_code[exe->func_code_end++] = fp->code_num >> 8;
        exe->func_code[exe->func_code_end++] = fp->code_num & 0xFF;
        memcpy(exe->func_code + exe->func_code_end, fp->code_buf, fp->code_num);
        exe->func_code_end += fp->code_num;
    }

    return 0;
}

void compile_code_dump(compile_t *cpl)
{
    uint8_t *code = compile_code_buf(cpl);
    int size =  compile_code_pos(cpl);
    int pc = 0, index;

    if (cpl->error) {
        printf("has error: %d\n", cpl->error);
        return;
    }

    printf("Code [%p : %d] :\n", code, size);
    printf("-------------------------------\n");
    while(pc < size) {
        int pos = pc;
        uint8_t c = code[pc++];
        switch(c) {
        case BC_STOP:       printf("[%.3d] ", pos); printf("STOP\n"); break;
        case BC_PASS:       printf("[%.3d] ", pos); printf("PASS\n"); break;

        case BC_RET0:       printf("[%.3d] ", pos); printf("RET0\n"); break;
        case BC_RET:        printf("[%.3d] ", pos); printf("RET\n"); break;

        /* Jump instruction */
        case BC_SJMP:       index = (int8_t) code[pc++];
                            printf("[%.3d] ", pos); printf("SJMP: %d\n", index);
                            break;
        case BC_JMP:        index = (int8_t) code[pc++]; index = (index << 8) | code[pc++];
                            printf("[%.3d] ", pos); printf("JMP: %d\n", index);
                            break;

        case BC_SJMP_T:     index = (int8_t) code[pc++];
                            printf("[%.3d] ", pos); printf("SJMP_T: %d\n", index);
                            break;

        case BC_SJMP_F:     index = (int8_t) code[pc++];
                            printf("[%.3d] ", pos); printf("SJMP_F: %d\n", index);
                            break;

        case BC_JMP_T:      index = (int8_t) code[pc++]; index = (index << 8) | code[pc++];
                            printf("[%.3d] ", pos); printf("JMP_T: %d\n", index);
                            break;
        case BC_JMP_F:      index = (int8_t) code[pc++]; index = (index << 8) | code[pc++];
                            printf("[%.3d] ", pos); printf("JMP_F: %d\n", index);
                            break;
        case BC_SJMP_T_POP: index = (int8_t) code[pc++];
                            printf("[%.3d] ", pos); printf("SJMP_T_POP: %d\n", index);
                            break;
        case BC_SJMP_F_POP: index = (int8_t) code[pc++];
                            printf("[%.3d] ", pos); printf("SJMP_F_POP: %d\n", index);
                            break;
        case BC_JMP_T_POP:  index = (int8_t) code[pc++]; index = (index << 8) | code[pc++];
                            printf("[%.3d] ", pos); printf("JMP_T_POP: %d\n", index);
                            break;
        case BC_JMP_F_POP:  index = (int8_t) code[pc++]; index = (index << 8) | code[pc++];
                            printf("[%.3d] ", pos); printf("JMP_F_POP: %d(%.4x)\n", index, index);
                            break;

        case BC_PUSH_UND:   printf("[%.3d] ", pos); printf("PUSH_UND\n"); break;
        case BC_PUSH_NAN:   printf("[%.3d] ", pos); printf("PUSH_NAN\n"); break;
        case BC_PUSH_TRUE:  printf("[%.3d] ", pos); printf("PUSH_TRUE\n"); break;
        case BC_PUSH_FALSE: printf("[%.3d] ", pos); printf("PUSH_FALSE\n"); break;
        case BC_PUSH_ZERO:  printf("[%.3d] ", pos); printf("PUSH_NUM 0\n"); break;
        case BC_PUSH_NUM:   index = code[pc++]; index = (index << 8) + code[pc++];
                            printf("[%.3d] ", pos); printf("PUSH_NUM %d\n", index);
                            break;
        case BC_PUSH_STR:   index = code[pc++]; index = (index << 8) + code[pc++];
                            printf("[%.3d] ", pos); printf("PUSH_STR %d\n", index);
                            break;
        case BC_PUSH_VAR:   index = code[pc++]; printf("[%.3d] ", pos); printf("PUSH_VAR %d\n", index); break;
        case BC_PUSH_VAR_REF:
                            index = code[pc++]; printf("[%.3d] ", pos); printf("PUSH_VAR_REF %d\n", index); break;
        case BC_PUSH_SCRIPT:index = code[pc++]; index = (index << 8) | code[pc++];
                            printf("[%.3d] ", pos); printf("PUSH_FUNC %d\n", index);
                            break;

        case BC_POP:        printf("[%.3d] ", pos); printf("POP\n"); break;
        case BC_POP_RESULT: printf("[%.3d] ", pos); printf("POP_RESULT\n"); break;

        case BC_NEG:        printf("[%.3d] ", pos); printf("NEG\n"); break;
        case BC_NOT:        printf("[%.3d] ", pos); printf("NOT\n"); break;
        case BC_LOGIC_NOT:  printf("[%.3d] ", pos); printf("LOGIC_NOT\n"); break;

        case BC_MUL:        printf("[%.3d] ", pos); printf("MUL\n"); break;
        case BC_DIV:        printf("[%.3d] ", pos); printf("DIV\n"); break;
        case BC_MOD:        printf("[%.3d] ", pos); printf("MOD\n"); break;
        case BC_ADD:        printf("[%.3d] ", pos); printf("ADD\n"); break;
        case BC_SUB:        printf("[%.3d] ", pos); printf("SUB\n"); break;

        case BC_AAND:       printf("[%.3d] ", pos); printf("LOGIC_AND\n"); break;
        case BC_AOR:        printf("[%.3d] ", pos); printf("LOGIC_OR\n"); break;
        case BC_AXOR:       printf("[%.3d] ", pos); printf("LOGIC_XOR\n"); break;

        case BC_LSHIFT:     printf("[%.3d] ", pos); printf("LSHIFT\n"); break;
        case BC_RSHIFT:     printf("[%.3d] ", pos); printf("RSHIFT\n"); break;

        case BC_TEQ:        printf("[%.3d] ", pos); printf("TEQ\n"); break;
        case BC_TNE:        printf("[%.3d] ", pos); printf("TNE\n"); break;
        case BC_TGT:        printf("[%.3d] ", pos); printf("TGT\n"); break;
        case BC_TGE:        printf("[%.3d] ", pos); printf("TGE\n"); break;
        case BC_TLT:        printf("[%.3d] ", pos); printf("TLT\n"); break;
        case BC_TLE:        printf("[%.3d] ", pos); printf("TLE\n"); break;

        case BC_TIN:        printf("[%.3d] ", pos); printf("TIN\n"); break;
        case BC_ASSIGN:     printf("[%.3d] ", pos); printf("ASSING\n"); break;
        case BC_FUNC_CALL:  index = code[pc++]; printf("[%.3d] ", pos); printf("CALL %d\n", index); break;

        case BC_PROP:       printf("[%.3d] ", pos); printf("PROP\n"); break;
        case BC_PROP_METH:  printf("[%.3d] ", pos); printf("PROP_CALL\n"); break;

        case BC_ELEM:       printf("[%.3d] ", pos); printf("ELEM\n"); break;
        case BC_ELEM_METH:  printf("[%.3d] ", pos); printf("ELEM_CALL\n"); break;

        default:            printf("[%.3d] ", pos); printf("Unknown: %.2x", c);
        }
    }
    printf("-------------------------------\n");
}
