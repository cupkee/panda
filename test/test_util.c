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

