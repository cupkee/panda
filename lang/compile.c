
#include "err.h"
#include "bcode.h"
#include "compile.h"

void compile_code_dump(compile_t *cpl);
static inline int compile_var_add_name(compile_t *cpl, const char *name) {
    return compile_var_add(cpl, compile_sym_add(cpl, name));
}

static inline compile_func_t *compile_func_cur(compile_t *cpl) {
    return cpl->func_buf + cpl->func_cur;
}

static inline uint8_t *compile_code_buf(compile_t *cpl) {
    return cpl->func_buf[cpl->func_cur].code_buf;
}

static inline uint8_t *compile_code_cur(compile_t *cpl) {
    compile_func_t *f = &cpl->func_buf[cpl->func_cur];
    return f->code_buf + f->code_pos;
}

static inline int compile_code_pos(compile_t *cpl) {
    return cpl->func_buf[cpl->func_cur].code_pos;
}

static intptr_t compile_sym_lookup(compile_t *cpl, const char *sym)
{
    if (cpl->sym_tbl) {
        return symtbl_get(cpl->sym_tbl, sym);
    }
    return 0;
}

static int compile_var_lookup(compile_t *cpl, const char *sym)
{
    intptr_t sym_id = compile_sym_lookup(cpl, sym);

    if (0 == sym_id) {
        return -1;
    }
    return compile_var_get(cpl, sym_id);
}

static int compile_code_check_extend(compile_t *cpl, int space)
{
    compile_func_t *func;
    int size;
    uint8_t *ptr;

    func = cpl->func_buf + cpl->func_cur;
    if (func->code_pos + space < func->code_size) {
        return 0;
    }
    size = func->code_size;
    size += size < space ? space : size;

    ptr = (uint8_t *)malloc(size);
    if (!ptr) {
        cpl->error = ERR_NotEnoughMemory;
        return 1;
    }

    memcpy(ptr, func->code_buf, func->code_pos);
    free(func->code_buf);

    func->code_buf  = ptr;
    func->code_size = size;

    return 0;
}

static int compile_vmap_check_extend(compile_t *cpl, int space)
{
    compile_func_t *func;
    int size;
    intptr_t *ptr;

    func = cpl->func_buf + cpl->func_cur;
    if (func->var_num + space < func->var_size) {
        return 0;
    }

    size = func->var_size;
    size += size < space ? space: size;
    ptr = (intptr_t *)malloc(size * sizeof(intptr_t));
    if (!ptr) {
        cpl->error = ERR_NotEnoughMemory;
        return 1;
    }

    memcpy(ptr, func->vars_map, func->var_num * sizeof(intptr_t));
    free(func->vars_map);

    func->vars_map = ptr;
    func->var_size = size;

    return 0;
}

static int compile_nums_extend(compile_t *cpl, int size)
{
    double *ptr;
    int i;

    if (size <= cpl->nums_size) {
        cpl->error = ERR_SysError;
        return 1;
    }

    if (cpl->nums_size >= STATIC_NUM_LIMIT) {
        cpl->error = ERR_StaticNumberOverrun;
        return 1;
    }

    size = cpl->nums_size * 2;
    if (size >= STATIC_NUM_LIMIT) {
        size = STATIC_NUM_LIMIT;
    }

    ptr = (double *)malloc(size * sizeof(double));
    if (!ptr) {
        cpl->error = ERR_NotEnoughMemory;
        return 1;
    }

    for (i = 0; i < cpl->nums_pos; i++) {
        ptr[i] = cpl->nums_buf[i];
    }
    free(cpl->nums_buf);

    cpl->nums_buf = ptr;
    cpl->nums_size = size;

    return 0;
}

static inline int compile_code_append(compile_t *cpl, uint8_t code)
{
    compile_func_t *func;

    if (cpl->error || 0 != compile_code_check_extend(cpl, 1)) {
        return 1;
    }

    func = cpl->func_buf + cpl->func_cur;
    func->code_buf[func->code_pos++] = code;

    return 0;
}

static inline int compile_code_append_n(compile_t *cpl, int n, uint8_t *code)
{
    compile_func_t *func;
    int i;

    if (cpl->error || 0 != compile_code_check_extend(cpl, n)) {
        return 1;
    }

    func = cpl->func_buf + cpl->func_cur;
    for (i = 0; i < n; i++) {
        func->code_buf[func->code_pos++] = code[i];
    }

    return 0;
}

// return: id of variable or -1
static inline int compile_vmap_append(compile_t *cpl, intptr_t sym_id)
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
        if (sym_id == func->vars_map[i]) return i; // already exist!
    }

    if (0 != compile_vmap_check_extend(cpl, 1)) {
        return -1;
    }

    func->vars_map[func->var_num++] = sym_id;

    return i;
}

static int compile_code_extend(compile_t *cpl, int bytes)
{
    compile_func_t *func = cpl->func_buf + cpl->func_cur;

    if (cpl->error || 0 != compile_code_check_extend(cpl, bytes)) {
        return 1;
    }

    func->code_pos += bytes;

    return 0;
}

static int compile_code_insert(compile_t *cpl, int pos, int bytes)
{
    compile_func_t *func = cpl->func_buf + cpl->func_cur;
    int i, n;

    if (cpl->error || 0 != compile_code_check_extend(cpl, bytes)) {
        return 1;
    }

    n = func->code_pos - pos;
    for (i = 1; i <= n; i++) {
        func->code_buf[func->code_pos + bytes - i] = func->code_buf[func->code_pos - i];
    }
    func->code_pos += bytes;

    return 0;
}

static int compile_nums_lookup(compile_t *cpl, double n)
{
    int i;

    for (i = 0; i < cpl->nums_pos; i++) {
        if (cpl->nums_buf[i] == n) {
            return i;
        }
    }

    if (i >= cpl->nums_size && 0 != compile_nums_extend(cpl, cpl->nums_size * 2)) {
        return -1;
    }
    cpl->nums_buf[cpl->nums_pos++] = n;

    return i;
}

static int compile_func_add(compile_t *cpl, int owner)
{
    int func_id;
    uint8_t  *code_buf;
    intptr_t *vars_map;

    if (cpl->func_num >= cpl->func_size) {
        int size = cpl->func_size * 2;
        int i;

        size = size > 0 ? size : DEF_FUNC_SIZE;
        compile_func_t *ptr = (compile_func_t *) malloc(sizeof(compile_func_t) * size);

        if (!ptr) {
            cpl->error = ERR_NotEnoughMemory;
            return -1;
        }

        for (i = 0; i < cpl->func_num; i++) {
            ptr[i].owner = cpl->func_buf[i].owner;
            ptr[i].var_size = cpl->func_buf[i].var_size;
            ptr[i].var_num  = cpl->func_buf[i].var_num;
            ptr[i].arg_num  = cpl->func_buf[i].arg_num;
            ptr[i].code_size = cpl->func_buf[i].code_size;
            ptr[i].code_pos = cpl->func_buf[i].code_pos;
            ptr[i].code_buf = cpl->func_buf[i].code_buf;
            ptr[i].vars_map = cpl->func_buf[i].vars_map;
        }
        if (cpl->func_buf) free(cpl->func_buf);
        cpl->func_buf = ptr;
    }

    code_buf = (uint8_t *) malloc(DEF_CODE_SIZE);
    if (!code_buf) {
        cpl->error = ERR_NotEnoughMemory;
        return -1;
    }

    vars_map  = (intptr_t *) malloc(sizeof(intptr_t) * DEF_VMAP_SIZE);
    if (!code_buf) {
        free(code_buf);
        cpl->error = ERR_NotEnoughMemory;
        return -1;
    }

    func_id = cpl->func_num++;

    cpl->func_buf[func_id].owner = owner;
    cpl->func_buf[func_id].var_size = DEF_VMAP_SIZE;
    cpl->func_buf[func_id].var_num  = 0;
    cpl->func_buf[func_id].arg_num  = 0;
    cpl->func_buf[func_id].code_size = DEF_CODE_SIZE;
    cpl->func_buf[func_id].code_pos = 0;
    cpl->func_buf[func_id].code_buf = code_buf;
    cpl->func_buf[func_id].vars_map = vars_map;

    return func_id;
}

int compile_init(compile_t *cpl, intptr_t sym_tbl)
{
    double  *nums_buf;

    nums_buf = (double *) malloc(DEF_STATIC_NUM_SIZE * sizeof(double));
    if (!nums_buf) {
        return -1;
    }

    cpl->error = 0;

    cpl->nums_size = DEF_STATIC_NUM_SIZE;
    cpl->nums_pos = 0;
    cpl->nums_buf = nums_buf;

    cpl->func_size = 0;
    cpl->func_cur = 0;
    cpl->func_num = 0;
    cpl->func_buf = NULL;

    cpl->sym_tbl = sym_tbl;

    return compile_func_add(cpl, -1); // should return zero;
}

int compile_deinit(compile_t *cpl)
{
    if (cpl) {
        int i;

        if (cpl->nums_buf) free(cpl->nums_buf);

        if (cpl->func_buf) {
            for (i = 0; i < cpl->func_num; i++) {
                compile_func_t *f = cpl->func_buf + i;

                if (f->vars_map) free(f->vars_map);
                if (f->code_buf) free(f->code_buf);
            }

            free(cpl->func_buf);
        }
        return 0;
    }
    return -1;
}

static void compile_code_append_num(compile_t *cpl, double n)
{
    int id;

    if (n == 0) {
        compile_code_append(cpl, BC_PUSH_ZERO);
        return;
    }

    if (0 > (id = compile_nums_lookup(cpl, n))) {
        return;
    }

    compile_code_append(cpl, BC_PUSH_NUM);
#if STATIC_NUM_LIMIT
    compile_code_append(cpl, id >> 8);
#endif
    compile_code_append(cpl, id);
}

static void compile_expr(compile_t *cpl, expr_t *e, void (*cb)(void *, void *), void *u);

static void compile_expr_binary(compile_t *cpl, expr_t *e, uint8_t code, void (*cb)(void *, void *), void *u)
{
    compile_expr(cpl, ast_expr_lft(e), cb, u);
    compile_expr(cpl, ast_expr_rht(e), cb, u);

    if (!cpl->error) {
        compile_code_append(cpl, code);
    }
}

static void compile_jmp_step(compile_t *cpl, int from, int step)
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

static inline void compile_jmp(compile_t *cpl, int from, int to)
{
    return compile_jmp_step(cpl, from, to - from);
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

static void compile_expr_logic_and(compile_t *cpl, expr_t *e, void (*cb)(void *, void *), void *u)
{
    int pos;
    compile_expr(cpl, ast_expr_lft(e), cb, u); pos = compile_code_pos(cpl);
    compile_expr(cpl, ast_expr_rht(e), cb, u);
    compile_nt_jmp_pop(cpl, pos, compile_code_pos(cpl));
}

static void compile_expr_logic_or(compile_t *cpl, expr_t *e, void (*cb)(void *, void *), void *u)
{
    int pos;
    compile_expr(cpl, ast_expr_lft(e), cb, u); pos = compile_code_pos(cpl);
    compile_expr(cpl, ast_expr_rht(e), cb, u);
    compile_tt_jmp_pop(cpl, pos, compile_code_pos(cpl));
}

static void compile_expr_lft(compile_t *cpl, expr_t *e, void (*cb)(void *, void *), void *u)
{
    switch (e->type) {
    case EXPR_ID: {
                    int var_id = compile_var_lookup(cpl, ast_expr_text(e));
                    if (var_id < 0) {
                        cpl->error = ERR_NotDefinedId;
                    } else {
                        uint8_t code[2];
                        code[0] = BC_PUSH_VAR_REF;
                        code[1] = var_id;
                        compile_code_append_n(cpl, 2, code);
                    }
                  }
                  break;
    case EXPR_ATTR: cpl->error = ERR_NotImplemented; break;
    case EXPR_ELEM: cpl->error = ERR_NotImplemented; break;
    default:        cpl->error = ERR_InvalidSyntax;
    }
}


void compile_expr_id(compile_t *cpl, expr_t *e, void (*cb)(void *, void *), void *u)
{
    int var_id = compile_var_lookup(cpl, ast_expr_text(e));

    if (var_id < 0) {
        cpl->error = ERR_NotDefinedId;
    } else {
        uint8_t code[2];
        code[0] = BC_PUSH_VAR;
        code[1] = var_id;
        compile_code_append_n(cpl, 2, code);
    }
}

static void compile_expr(compile_t *cpl, expr_t *e, void (*cb)(void *, void *), void *u)
{
    if (cpl->error) {
        return;
    }

    switch (e->type) {
    case EXPR_ID:       compile_expr_id(cpl, e, cb, u); break;
    case EXPR_NAN:      compile_code_append(cpl, BC_PUSH_NAN); break;
    case EXPR_UND:      compile_code_append(cpl, BC_PUSH_UND); break;
    case EXPR_NUM:      compile_code_append_num(cpl, ast_expr_num(e)); break;
    case EXPR_TRUE:     compile_code_append(cpl, BC_PUSH_TRUE); break;
    case EXPR_FALSE:    compile_code_append(cpl, BC_PUSH_FALSE); break;
    case EXPR_FUNCDEF:  cpl->error = ERR_NotImplemented; break;
    case EXPR_STRING:   cpl->error = ERR_NotImplemented; break;

    case EXPR_NEG:      compile_expr(cpl, ast_expr_lft(e), cb, u); compile_code_append(cpl, BC_NEG); break;
    case EXPR_NOT:      compile_expr(cpl, ast_expr_lft(e), cb, u); compile_code_append(cpl, BC_NOT); break;
    case EXPR_LOGIC_NOT:compile_expr(cpl, ast_expr_lft(e), cb, u); compile_code_append(cpl, BC_LOGIC_NOT); break;
    case EXPR_ARRAY:    cpl->error = ERR_NotImplemented; break;
    case EXPR_DICT:     cpl->error = ERR_NotImplemented; break;

    case EXPR_MUL:      compile_expr_binary(cpl, e, BC_MUL, cb, u); break;
    case EXPR_DIV:      compile_expr_binary(cpl, e, BC_DIV, cb, u); break;
    case EXPR_MOD:      compile_expr_binary(cpl, e, BC_MOD, cb, u); break;
    case EXPR_ADD:      compile_expr_binary(cpl, e, BC_ADD, cb, u); break;
    case EXPR_SUB:      compile_expr_binary(cpl, e, BC_SUB, cb, u); break;

    case EXPR_AND:      compile_expr_binary(cpl, e, BC_AAND, cb, u); break;
    case EXPR_OR:       compile_expr_binary(cpl, e, BC_AOR, cb, u); break;
    case EXPR_XOR:      compile_expr_binary(cpl, e, BC_AXOR, cb, u); break;

    case EXPR_LSHIFT:   compile_expr_binary(cpl, e, BC_LSHIFT, cb, u); break;
    case EXPR_RSHIFT:   compile_expr_binary(cpl, e, BC_RSHIFT, cb, u); break;

    case EXPR_TEQ:      compile_expr_binary(cpl, e, BC_TEQ, cb, u); break;
    case EXPR_TNE:      compile_expr_binary(cpl, e, BC_TNE, cb, u); break;
    case EXPR_TGT:      compile_expr_binary(cpl, e, BC_TGT, cb, u); break;
    case EXPR_TGE:      compile_expr_binary(cpl, e, BC_TGE, cb, u); break;
    case EXPR_TLT:      compile_expr_binary(cpl, e, BC_TLT, cb, u); break;
    case EXPR_TLE:      compile_expr_binary(cpl, e, BC_TLE, cb, u); break;
    case EXPR_TIN:      compile_expr_binary(cpl, e, BC_TIN, cb, u); break;

    case EXPR_LOGIC_AND:compile_expr_logic_and(cpl, e, cb, u); break;
    case EXPR_LOGIC_OR: compile_expr_logic_or(cpl, e, cb, u); break;

    case EXPR_CALL:     cpl->error = ERR_NotImplemented; break;
    case EXPR_ELEM:     cpl->error = ERR_NotImplemented; break;
    case EXPR_ATTR:     cpl->error = ERR_NotImplemented; break;

    case EXPR_ASSIGN:   compile_expr_lft(cpl, ast_expr_lft(e), cb, u);
                        compile_expr(cpl, ast_expr_rht(e), cb, u);
                        compile_code_append(cpl, BC_ASSIGN); break;

    case EXPR_COMMA:    compile_expr(cpl, ast_expr_lft(e), cb, u); compile_code_append(cpl, BC_POP);
                        compile_expr(cpl, ast_expr_rht(e), cb, u); break;
    case EXPR_TERNARY:  {
                            int pos1, pos2, end;
                            compile_expr(cpl, ast_expr_lft(e), cb, u); pos1 = compile_code_pos(cpl);
                            compile_expr(cpl, ast_expr_lft(ast_expr_rht(e)), cb, u); pos2 = compile_code_pos(cpl);
                            compile_expr(cpl, ast_expr_rht(ast_expr_rht(e)), cb, u); end = compile_code_pos(cpl);
                            compile_jmp(cpl, pos2, end);
                            compile_pop_nt_jmp(cpl, pos1, pos2 + (compile_code_pos(cpl) - end));
                        }
                        break;
    default:            cpl->error = ERR_NotImplemented; break;
    }
}

static void compile_stmt_block(compile_t *cpl, stmt_t *s, void (*cb)(void *, void *), void *u)
{
    while(s && cpl->error == 0) {
        compile_stmt(cpl, s, cb, u);
        s = s->next;
    }
}

static void compile_stmt_expr(compile_t *cpl, stmt_t *s, void (*cb)(void *, void *), void *u)
{
    compile_expr(cpl, s->expr, cb, u);
    if (0 == cpl->error) {
        compile_code_append(cpl, BC_POP_RESULT);
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
static void compile_code_jmp(compile_t *cpl, uint8_t *buf, uint8_t jmp, int step)
{
    if (step > 32767 || step < -32768) {
        cpl->error = 5555;
    }
    buf[0] = jmp;
    buf[1] = step >> 8;
    buf[2] = step;
}

static void compile_stmt_cond(compile_t *cpl, stmt_t *s, void (*cb)(void *, void *), void *u)
{
    int test_pos, skip_pos, block, other;

    compile_expr(cpl, s->expr, cb, u); if (cpl->error) return;
    test_pos = compile_code_pos(cpl);

    compile_code_extend(cpl, 3);
    block = compile_code_pos(cpl);

    compile_stmt_block(cpl, s->block, cb, u); if (cpl->error) return;
    other = compile_code_pos(cpl);

    if (s->other) {
        skip_pos = other;
        compile_code_extend(cpl, 3);
        other = compile_code_pos(cpl);

        compile_stmt_block(cpl, s->other, cb, u); if (cpl->error) return;
        compile_code_jmp(cpl, compile_code_buf(cpl) + skip_pos, BC_JMP,
                         compile_code_pos(cpl) - other);
    }

    compile_code_jmp(cpl, compile_code_buf(cpl) + test_pos, BC_JMP_F_POP,
                     other - block);
    //compile_code_dump(cpl);
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
static void compile_stmt_while(compile_t *cpl, stmt_t *s, void (*cb)(void *, void *), void *u)
{
    int bgn, skip, end, total, block, bgn_bk, skip_bk;
    uint8_t code[5] = {BC_SJMP_T_POP, 3, 0, 0, BC_PASS};
    uint8_t *code_skip;

    bgn = compile_code_pos(cpl);
    compile_expr(cpl, s->expr, cb, u);
    skip = compile_code_pos(cpl) + 2;
    code_skip = compile_code_cur(cpl) + 2;

    printf("skip p: %ld\n", code_skip - compile_code_buf(cpl));

    compile_code_append_n(cpl, 5, code); if (cpl->error) return;

    // save begin and skip position, used for statements compile of break & continue
    bgn_bk = cpl->bgn_pos;
    skip_bk = cpl->skip_pos;
    cpl->bgn_pos = bgn;
    cpl->skip_pos = skip;
    printf("bgn: %d, skip: %d\n", bgn, skip);

    compile_stmt_block(cpl, s->block, cb, u); if (cpl->error) return;
    end = compile_code_pos(cpl);

    cpl->bgn_pos = bgn_bk;
    cpl->skip_pos = skip_bk;

    total = end - bgn;
    if (total > 126) {
        total += 3; // 3 byte jmp instruction
    } else {
        total += 2; // 2 byte sjmp instruction
    }
    compile_jmp_step(cpl, end, -total);
    printf("end: %d\n", end);
    printf("total: %d\n", total);

    block = total - (skip - bgn + 3);
    if (block < 127) {
        code_skip[0] = BC_SJMP;
        code_skip[1] = block + 1;
    } else {
        code_skip[0] = BC_JMP;
        code_skip[1] = block >> 8;
        code_skip[2] = block;
    }
    printf("block: %d\n", block);
}

static void compile_stmt_break(compile_t *cpl, stmt_t *s, void (*cb)(void *, void *), void *u)
{
    int bgn, end, total;

    bgn = cpl->skip_pos;
    end = compile_code_pos(cpl);
    total = end - bgn;
    printf("bgn: %d, end: %d, total: %d\n", bgn, end, total);
    if (total > 126) {
        total += 3; // 3 byte jmp instruction
    } else {
        total += 2; // 2 byte sjmp instruction
    }
    compile_jmp_step(cpl, end, -total);
}

static void compile_stmt_continue(compile_t *cpl, stmt_t *s, void (*cb)(void *, void *), void *u)
{
    int bgn, end, total;

    bgn = cpl->bgn_pos;
    end = compile_code_pos(cpl);
    total = end - bgn;
    if (total > 126) {
        total += 3; // 3 byte jmp instruction
    } else {
        total += 2; // 2 byte sjmp instruction
    }
    compile_jmp_step(cpl, end, -total);
}

static void compile_stmt_var(compile_t *cpl, stmt_t *s, void (*cb)(void *, void *), void *u)
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
            compile_expr(cpl, e, cb, u);
            compile_code_append(cpl, BC_POP);
        }

        e = next;
    }
}

intptr_t compile_sym_add(compile_t *cpl, const char *sym)
{
    if (!cpl || cpl->error || !cpl->sym_tbl) {
        return 0;
    }

    return symtbl_add(cpl->sym_tbl, sym);
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
    var_id  = compile_vmap_append(cpl, sym_id);
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
    var_id  = compile_vmap_append(cpl, sym_id);
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
    compile_func_t *func = compile_func_cur(cpl);
    int var_id;

    for (var_id = 0; var_id < func->var_num; var_id++) {
        if (func->vars_map[var_id] == sym_id) {
            return var_id;
        }
    }
    return -1;
}

int compile_stmt(compile_t *cpl, stmt_t *stmt, void (*cb)(void *, void *), void *u)
{
    if (!cpl || !stmt) {
        return -1;
    }

    if (cpl->error) {
        return -cpl->error;
    }

    switch(stmt->type) {
    case STMT_PASS: break;
    case STMT_EXPR: compile_stmt_expr(cpl, stmt, cb, u); break;
    case STMT_VAR:  compile_stmt_var(cpl, stmt, cb, u); break;
    case STMT_IF:   compile_stmt_cond(cpl, stmt, cb, u); break;
    case STMT_WHILE:    compile_stmt_while(cpl, stmt, cb, u); break;
    case STMT_BREAK:    compile_stmt_break(cpl, stmt, cb, u); break;
    case STMT_CONTINUE: compile_stmt_continue(cpl, stmt, cb, u); break;
    case STMT_RET:
    default: cpl->error = ERR_NotImplemented;
    }

    return -cpl->error;
}

int compile_one_stmt(compile_t *cpl, stmt_t *stmt, void (*cb)(void *, void *), void *u)
{
    int ret = compile_stmt(cpl, stmt, cb, u);

    if (ret == 0)
        compile_code_append(cpl, BC_STOP);

    return ret;
}

void compile_code_dump(compile_t *cpl)
{
    uint8_t *code = compile_code_buf(cpl);
    int size =  compile_code_pos(cpl);
    int pc = 0, index;

    printf("ByteCode [%p:%d] :\n", code, size);
    printf("-------------------------------\n");
    while(pc < size) {
        uint8_t c = code[pc++];
        switch(c) {
        case BC_STOP:       printf("STOP\n");

        /* Jump instruction */
        case BC_SJMP:       index = (int8_t) code[pc++];
                            printf("SJMP: %d\n", index);
                            break;

        case BC_JMP:        index = (int8_t) code[pc++]; index = (index << 8) | code[pc++];
                            printf("JMP: %d\n", index);
                            break;

        case BC_SJMP_T:     index = (int8_t) code[pc++];
                            printf("SJMP_T: %d\n", index);
                            break;

        case BC_SJMP_F:     index = (int8_t) code[pc++];
                            printf("SJMP_F: %d\n", index);
                            break;

        case BC_JMP_T:      index = (int8_t) code[pc++]; index = (index << 8) | code[pc++];
                            printf("JMP_T: %d\n", index);
                            break;
        case BC_JMP_F:      index = (int8_t) code[pc++]; index = (index << 8) | code[pc++];
                            printf("JMP_F: %d\n", index);
                            break;
        case BC_SJMP_T_POP: index = (int8_t) code[pc++];
                            printf("SJMP_T_POP: %d\n", index);
                            break;
        case BC_SJMP_F_POP: index = (int8_t) code[pc++];
                            printf("SJMP_F_POP: %d\n", index);
                            break;
        case BC_JMP_T_POP:  index = (int8_t) code[pc++]; index = (index << 8) | code[pc++];
                            printf("JMP_T_POP: %d\n", index);
                            break;
        case BC_JMP_F_POP:  index = (int8_t) code[pc++]; index = (index << 8) | code[pc++];
                            printf("JMP_F_POP: %d(%.4x)\n", index, index);
                            break;

        case BC_PUSH_UND:   printf("PUSH_UND\n"); break;
        case BC_PUSH_NAN:   printf("PUSH_NAN\n"); break;
        case BC_PUSH_TRUE:  printf("PUSH_TRUE\n"); break;
        case BC_PUSH_FALSE: printf("PUSH_FALSE\n"); break;
        case BC_PUSH_ZERO:  printf("PUSH_NUM 0\n"); break;
        case BC_PUSH_NUM:   index = code[pc++]; index = (index << 8) + code[pc++];
                            printf("PUSH_NUM %d\n", index);
                            break;
        case BC_PUSH_VAR:   index = code[pc++]; printf("PUSH_VAR %d\n", index); break;
        case BC_PUSH_VAR_REF:
                            index = code[pc++]; printf("PUSH_VAR_REF %d\n", index); break;

        case BC_POP:        printf("POP\n"); break;
        case BC_POP_RESULT: printf("POP_RESULT\n"); break;

        case BC_NEG:        printf("NEG\n"); break;
        case BC_NOT:        printf("NOT\n"); break;
        case BC_LOGIC_NOT:  printf("LOGIC_NOT\n"); break;

        case BC_MUL:        printf("MUL\n"); break;
        case BC_DIV:        printf("DIV\n"); break;
        case BC_MOD:        printf("MOD\n"); break;
        case BC_ADD:        printf("ADD\n"); break;
        case BC_SUB:        printf("SUB\n"); break;

        case BC_AAND:       printf("LOGIC_AND\n"); break;
        case BC_AOR:        printf("LOGIC_OR\n"); break;
        case BC_AXOR:       printf("LOGIC_XOR\n"); break;

        case BC_LSHIFT:     printf("LSHIFT\n"); break;
        case BC_RSHIFT:     printf("RSHIFT\n"); break;

        case BC_TEQ:        printf("TEQ\n"); break;
        case BC_TNE:        printf("TNE\n"); break;
        case BC_TGT:        printf("TGT\n"); break;
        case BC_TGE:        printf("TGE\n"); break;
        case BC_TLT:        printf("TLT\n"); break;
        case BC_TLE:        printf("TLE\n"); break;

        case BC_TIN:        printf("TIN\n"); break;
        case BC_ASSIGN:     printf("ASSING\n"); break;

        default:            printf("Unknown: %.2x", c);
        }
    }
    printf("-------------------------------\n");
}
