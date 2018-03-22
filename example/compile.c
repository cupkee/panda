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

#include "example.h"

#define MEM_SIZE  10240
static char *MEM_PTR[MEM_SIZE];

static int set_output_name(const char *input, void *buf, int sz)
{
    int pos = file_base_name(input, buf, sz - 5);

    if (pos < 1) {
        return -1;
    }

    memcpy(buf + pos, ".pdc", 5);

    return 0;
}

#define OUTPUT_NAME_MAX     (128)

static int compile(const char *input, void *mem_ptr, int mem_size)
{
    char *output  = (char *) mem_ptr;
    void *cpl_mem, *exe_mem;
    env_t env;
    int cpl_mem_sz, exe_mem_sz, input_sz;
    int exe_sz;

    exe_mem_sz = (mem_size / 3) & (~0xf);
    cpl_mem_sz = mem_size - OUTPUT_NAME_MAX - exe_mem_sz;

    exe_mem = mem_ptr + OUTPUT_NAME_MAX;
    cpl_mem = mem_ptr + OUTPUT_NAME_MAX + exe_mem_sz;

    if (0 != set_output_name(input, output, OUTPUT_NAME_MAX)) {
        return -1;
    }

    if (0 != compile_env_init(&env, cpl_mem, cpl_mem_sz)) {
        return -1;
    }

    native_init(&env);

    input = file_load(input, &input_sz);
    if (!input) {
        return -1;
    }

    exe_sz = compile_exe(&env, input, exe_mem, exe_mem_sz);
    file_release((void *)input, input_sz);

    if (exe_sz <= 0) {
        return -1;
    } else {
        return file_store(output, exe_mem, exe_sz);
    }
}

int main(int ac, char **av)
{
    int   error;

    if (ac == 1) {
        printf("Usage: %s <input>\n", av[0]);
        return 0;
    }

    if (0 > (error = compile(av[1], MEM_PTR, MEM_SIZE))) {
        printf("compile: %s fail:%d\n", av[1], error);
    }

    return error ? 1 : 0;
}

