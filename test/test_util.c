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

