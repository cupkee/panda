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



#ifndef __LANG_EXECUTABLE_INC__
#define __LANG_EXECUTABLE_INC__

#include "def.h"
#include "val.h"

#define FUNC_HEAD_SIZE 8

#define EXEC_FL_BE     1
#define EXEC_FL_64     2

typedef struct executable_t {
    uint16_t  string_max;
    uint16_t  string_num;

    uint16_t  number_max;
    uint16_t  number_num;

    uint16_t  func_max;
    uint16_t  func_num;

    double   *number_map;
    intptr_t *string_map;
    uint8_t **func_map;

    uint32_t  main_code_end;
    uint32_t  func_code_end;

    uint8_t  *code;
} executable_t;

typedef struct image_info_t {
    int8_t      error;
    uint8_t     addr_size;
    uint8_t     byte_order;
    uint8_t     version;

    uint32_t    size;
    uint32_t    end;

    uint32_t    num_cnt, num_ent;
    uint32_t    str_cnt, str_ent;
    uint32_t    fn_cnt, fn_ent;

    uint8_t    *base;
} image_info_t;


int executable_init(executable_t *exe, void *memory, int size,
                    int number_max, int string_max, int func_max, int code_max);

static inline
void executable_main_clr(executable_t *exe)
{
    exe->main_code_end = 0;
}

int executable_func_set_head(void *buf, uint8_t vc, uint8_t ac, uint32_t code_size, uint16_t stack_size, int closure);
int executable_func_get_head(void *buf, uint8_t *vc, uint8_t *ac, uint32_t *code_size, uint16_t *stack_size, int *closure);
int executable_main_add(executable_t *exe, void *code, uint16_t size, uint8_t vc, uint8_t ac, uint16_t stack_size, int closure);
int executable_func_add(executable_t *exe, void *code, uint16_t size, uint8_t vc, uint8_t ac, uint16_t stack_size, int closure);

static inline
uint8_t executable_func_get_var_cnt(const uint8_t *entry) {
    return entry[0];
}

static inline
uint8_t executable_func_get_arg_cnt(const uint8_t *entry) {
    return entry[1];
}

static inline
uint16_t executable_func_get_stack_high(const uint8_t *entry) {
    return (entry[2] * 0x100 + entry[3]) & 0x7FFF;
}

static inline
uint32_t executable_func_get_code_size(const uint8_t *entry) {
    return (entry[4] * 0x1000000 + entry[5] * 0x10000 + entry[6] * 0x100 + entry[7]);
}

static inline
const uint8_t *executable_func_get_code(const uint8_t *entry) {
    return entry + FUNC_HEAD_SIZE;
}

static inline
int executable_func_is_closure(const uint8_t *entry) {
    return (entry[2] & 0x80) == 0x80;
}

int executable_number_find_add(executable_t *exe, double n);
int executable_string_find_add(executable_t *exe, intptr_t s);

int image_init(image_info_t *img, void *mem_ptr, int mem_size, int byte_order, int nc, int sc, int fc);
int image_load(image_info_t *img, uint8_t *input, int size);
int image_fill_data(image_info_t *img, unsigned int nc, double *nv, unsigned int sc, intptr_t *sv);
int image_fill_code(image_info_t *img, unsigned int entry, uint8_t vc, uint8_t ac, uint16_t stack_need, int closure, uint8_t *code, unsigned int size);
double *image_number_entry(image_info_t *img);
double image_get_number(image_info_t *img, int index);
const char *image_get_string(image_info_t *img, int index);
const uint8_t *image_get_function(image_info_t *img, int index);

static inline int image_size(image_info_t *img) {
    return img->end;
}

#endif /* __LANG_EXECUTABLE_INC__ */

