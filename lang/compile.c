
#include "err.h"
#include "bcode.h"
#include "compile.h"

static int compile_code_extend(compile_t *cpl, int size)
{
    uint8_t *ptr;

    if (size <= cpl->code_size) {
        cpl->error = ERR_SysError;
        return 1;
    }

    ptr = (uint8_t *)malloc(size);
    if (!ptr) {
        cpl->error = ERR_NotEnoughMemory;
        return 1;
    }

    memcpy(ptr, cpl->code_buf, cpl->code_pos);
    free(cpl->code_buf);

    cpl->code_buf  = ptr;
    cpl->code_size = size;

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
    if (cpl->code_pos >= cpl->code_size && 0 != compile_code_extend(cpl, cpl->code_size * 2)) {
        return 1;
    }

    cpl->code_buf[cpl->code_pos++] = code;

    return 0;
}

static int compile_code_insert(compile_t *cpl, int pos, int bytes)
{
    int i, n;

    if (cpl->code_size - cpl->code_pos < bytes && 0 != compile_code_extend(cpl, cpl->code_size + bytes)) {
        return 1;
    }

    n = cpl->code_pos - pos;
    for (i = 1; i <= n; i++) {
        cpl->code_buf[cpl->code_pos + bytes - i] = cpl->code_buf[cpl->code_pos - i];
    }
    cpl->code_pos += bytes;

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

int compile_init(compile_t *cpl, int code_size, int nums_size)
{
    double  *nums_buf;
    uint8_t *code_buf;

    code_buf = (uint8_t *) malloc(code_size);
    if (!code_buf) {
        return -1;
    }

    nums_buf = (double *) malloc(nums_size * sizeof(double));
    if (!nums_buf) {
        free(code_buf);
        return -1;
    }

    cpl->error = 0;
    cpl->code_size = code_size;
    cpl->code_pos = 0;
    cpl->code_buf = code_buf;
    cpl->nums_size = nums_size;
    cpl->nums_pos = 0;
    cpl->nums_buf = nums_buf;

    return 0;
}

int compile_deinit(compile_t *cpl)
{
    if (cpl) {
        if (cpl->code_buf) free(cpl->code_buf);
        if (cpl->nums_buf) free(cpl->nums_buf);
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

static void compile_true_jmp(compile_t *cpl, int pos, int len)
{
    if (!cpl->error && 0 == compile_code_insert(cpl, pos, len > 127 ? 4 : 3)) {
        if (len > 127) {
            cpl->code_buf[pos++] = BC_JMP_T;
            cpl->code_buf[pos++] = len >> 8;
        } else {
            cpl->code_buf[pos++] = BC_SJMP_T;
        }
        cpl->code_buf[pos++] = len;
        cpl->code_buf[pos++] = BC_POP;
    }
}

static void compile_false_jmp(compile_t *cpl, int pos, int len)
{
    if (!cpl->error && 0 == compile_code_insert(cpl, pos, len > 127 ? 4 : 3)) {
        if (len > 127) {
            cpl->code_buf[pos++] = BC_JMP_F;
            cpl->code_buf[pos++] = len >> 8;
        } else {
            cpl->code_buf[pos++] = BC_SJMP_F;
        }
        cpl->code_buf[pos++] = len;
        cpl->code_buf[pos++] = BC_POP;
    }
}

static void compile_expr(compile_t *cpl, expr_t *e, void (*cb)(void *, void *), void *u)
{
    if (cpl->error) {
        return;
    }

    switch (e->type) {
    case EXPR_ID:       cpl->error = ERR_NotImplemented; break;
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

    case EXPR_LOGIC_AND:{
                        int pos, len;
                        compile_expr(cpl, ast_expr_lft(e), cb, u); pos = cpl->code_pos;
                        compile_expr(cpl, ast_expr_rht(e), cb, u); len = cpl->code_pos - pos + 1;
                        compile_false_jmp(cpl, pos, len);
                        }
                        break;

    case EXPR_LOGIC_OR: {
                        int pos, len;
                        compile_expr(cpl, ast_expr_lft(e), cb, u); pos = cpl->code_pos;
                        compile_expr(cpl, ast_expr_rht(e), cb, u); len = cpl->code_pos - pos + 1;
                        compile_true_jmp(cpl, pos, len);
                        }
                        break;

    case EXPR_CALL:     cpl->error = ERR_NotImplemented; break;
    case EXPR_ELEM:     cpl->error = ERR_NotImplemented; break;
    case EXPR_ATTR:     cpl->error = ERR_NotImplemented; break;

    case EXPR_ASSIGN:   cpl->error = ERR_NotImplemented; break;

    case EXPR_COMMA:    cpl->error = ERR_NotImplemented; break;
    case EXPR_TERNARY:  cpl->error = ERR_NotImplemented; break;

    default:            cpl->error = ERR_NotImplemented; break;
    }
}

static void compile_stmt_expr(compile_t *cpl, stmt_t *s, void (*cb)(void *, void *), void *u)
{
    compile_expr(cpl, s->expr, cb, u);
    if (0 == cpl->error) {
        compile_code_append(cpl, BC_POP_RESULT);
    }
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
    case STMT_EXPR: compile_stmt_expr(cpl, stmt, cb, u); break;
    default: cpl->error = ERR_NotImplemented;
    }

    compile_code_append(cpl, BC_STOP);

    return -cpl->error;
}

