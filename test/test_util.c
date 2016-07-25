#include <stdio.h>
#include <string.h>

#include "test_util.h"

static const char *lines[256];
static int max = 0;
static int cur = 0;
static int start = 0;

void test_clr_line(void)
{
    max = 0;
    cur = 0;
    start = 0;
}

void test_set_line(const char *line)
{
    if (max < 256) {
        lines[max++] = line;
    }
}

int test_get_line(void *buf, int size)
{
    const char *line_cur = lines[cur];
    int end, lft;

    if (cur >= max) {
        return 0;
    }

    end = strlen(line_cur);
    lft = end - start;
    if (lft > size) {
        memcpy(buf, line_cur + start, size);
        start += size;
        return size;
    } else {
        memcpy(buf, line_cur + start, lft);
        start = 0;
        cur++;

        return lft;
    }
}

struct sbuf_t {
    int  pos, end;
    char *buf;
};

static void expr_stringify(void *u, expr_t *e)
{
    struct sbuf_t *b = (struct sbuf_t *)u;

    if (b->pos) {
        b->buf[b->pos++] = ' ';
    }

    switch (e->type) {
        case EXPR_ID:
            b->pos += snprintf(b->buf + b->pos, b->end - b->pos, "%s", ast_expr_text(e)); break;
        case EXPR_NUM:
            b->pos += snprintf(b->buf + b->pos, b->end - b->pos, "%d", ast_expr_num(e)); break;
        case EXPR_NAN:
            b->pos += snprintf(b->buf + b->pos, b->end - b->pos, "%s", "NaN"); break;
        case EXPR_UND:
            b->pos += snprintf(b->buf + b->pos, b->end - b->pos, "%s", "undefined"); break;
        case EXPR_NULL:
            b->pos += snprintf(b->buf + b->pos, b->end - b->pos, "%s", "null"); break;
        case EXPR_TRUE:
            b->pos += snprintf(b->buf + b->pos, b->end - b->pos, "%s", "true"); break;
        case EXPR_FALSE:
            b->pos += snprintf(b->buf + b->pos, b->end - b->pos, "%s", "false"); break;
        case EXPR_STRING:
            b->pos += snprintf(b->buf + b->pos, b->end - b->pos, "'%s'", ast_expr_text(e)); break;
        // unary expression
        case EXPR_NEG: b->buf[b->pos++] = '-'; break;
        case EXPR_NOT: b->buf[b->pos++] = '~'; break;
        case EXPR_LOGIC_NOT: b->buf[b->pos++] = '!'; break;
        case EXPR_ARRAY: b->buf[b->pos++] = '['; break;
        case EXPR_DICT: b->buf[b->pos++] = '{'; break;
        // binary expression
        case EXPR_MUL: b->buf[b->pos++] = '*'; break;
        case EXPR_DIV: b->buf[b->pos++] = '/'; break;
        case EXPR_MOD: b->buf[b->pos++] = '%'; break;
        case EXPR_ADD: b->buf[b->pos++] = '+'; break;
        case EXPR_SUB: b->buf[b->pos++] = '-'; break;
        case EXPR_LSHIFT: b->buf[b->pos++] = '<'; b->buf[b->pos++] = '<'; break;
        case EXPR_RSHIFT: b->buf[b->pos++] = '>'; b->buf[b->pos++] = '>'; break;
        case EXPR_AND: b->buf[b->pos++] = '&'; break;
        case EXPR_OR: b->buf[b->pos++] = '|'; break;
        case EXPR_XOR: b->buf[b->pos++] = '^'; break;
        case EXPR_TNE: b->buf[b->pos++] = '!'; b->buf[b->pos++] = '='; break;
        case EXPR_TEQ: b->buf[b->pos++] = '='; b->buf[b->pos++] = '='; break;
        case EXPR_TGT: b->buf[b->pos++] = '>'; break;
        case EXPR_TGE: b->buf[b->pos++] = '>'; b->buf[b->pos++] = '='; break;
        case EXPR_TLT: b->buf[b->pos++] = '<'; break;
        case EXPR_TLE: b->buf[b->pos++] = '<'; b->buf[b->pos++] = '='; break;
        case EXPR_TIN: b->buf[b->pos++] = 'i'; b->buf[b->pos++] = 'n'; break;
        case EXPR_LOGIC_AND: b->buf[b->pos++] = '&'; b->buf[b->pos++] = '&'; break;
        case EXPR_LOGIC_OR:  b->buf[b->pos++] = '|'; b->buf[b->pos++] = '|'; break;
        case EXPR_ASSIGN: b->buf[b->pos++] = '='; break;
        case EXPR_COMMA: b->buf[b->pos++] = ','; break;
        case EXPR_ATTR: b->buf[b->pos++] = '.'; break;
        case EXPR_ELEM: b->buf[b->pos++] = '['; b->buf[b->pos++] = ']'; break;
        case EXPR_CALL: b->buf[b->pos++] = '('; break;
        case EXPR_PAIR: b->buf[b->pos++] = ':'; break;
        case EXPR_TERNARY: b->buf[b->pos++] = '?'; break;
        default: b->buf[b->pos++] = '#'; break;
    }

    b->buf[b->pos] = 0;
}

char * expr_stringify_after_older(expr_t *e, int size, char *buf)
{
    struct sbuf_t sbuf;

    sbuf.buf = buf;
    sbuf.pos = 0;
    sbuf.end = size;

    memset(buf, 0, size);
    ast_traveral_expr(e, expr_stringify, &sbuf);

    return buf;
}

