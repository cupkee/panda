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

#ifndef __LANG_SCOPE_INC__
#define __LANG_SCOPE_INC__

#define MAGIC_SCOPE             (MAGIC_BASE + 1)

#define SCOPE_FL_HEAP           (1)     // variable space alloced in heap

typedef struct scope_t {
    uint8_t magic;
    uint8_t age;
    uint8_t num;                // all variables number
    uint8_t nao;                // nonamed arguments offset
    val_t   *var_buf;
    struct scope_t *super;
} scope_t;

static inline int scope_mem_space(scope_t *scope) {
    return SIZE_ALIGN(sizeof(scope_t)) + SIZE_ALIGN(sizeof(val_t) * scope->num);
}

#endif /* __LANG_SCOPE_INC__ */

