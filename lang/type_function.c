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

#include "type_function.h"

intptr_t function_create(env_t *env, uint8_t *entry)
{
    function_t *fn = (function_t *) env_heap_alloc(env, sizeof(function_t));

    if (fn) {
        fn->magic = MAGIC_FUNCTION;
        fn->age   = 0;
        fn->entry = entry;
        fn->super = env->scope;
    }
    return (intptr_t) fn;
}

int function_destroy(intptr_t fn)
{
    (void) fn;
    return 0;
}

const val_metadata_t metadata_function = {
    .is_true  = val_as_true,
    .is_equal = val_op_false,

    .value_of = val_as_nan,
};

const val_metadata_t metadata_function_native = {
    .name     = "function",

    .is_true  = val_as_true,
    .is_equal = val_op_false,

    .value_of = val_as_nan,
};

