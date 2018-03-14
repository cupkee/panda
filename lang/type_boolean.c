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

#include "type_boolean.h"

static int boolean_is_true(val_t *self)
{
    return val_2_intptr(self);
}

int boolean_is_gt(val_t *self, val_t *to)
{
    if (val_is_boolean(to)) {
        return val_2_intptr(self) > val_2_intptr(to);
    } else
    if (val_is_number(to)) {
        return val_2_intptr(self) > val_2_double(to);
    } else {
        return 0;
    }
}

int boolean_is_ge(val_t *self, val_t *to)
{
    if (val_is_boolean(to)) {
        return val_2_intptr(self) >= val_2_intptr(to);
    } else
    if (val_is_number(to)) {
        return val_2_intptr(self) >= val_2_double(to);
    } else {
        return 0;
    }
}

int boolean_is_lt(val_t *self, val_t *to)
{
    if (val_is_boolean(to)) {
        return val_2_intptr(self) < val_2_intptr(to);
    } else
    if (val_is_number(to)) {
        return val_2_intptr(self) < val_2_double(to);
    } else {
        return 0;
    }
}

int boolean_is_le(val_t *self, val_t *to)
{
    if (val_is_boolean(to)) {
        return val_2_intptr(self) <= val_2_intptr(to);
    } else
    if (val_is_number(to)) {
        return val_2_intptr(self) <= val_2_double(to);
    } else {
        return 0;
    }
}

const val_metadata_t metadata_boolean = {
    .name     = "boolean",

    .is_true  = boolean_is_true,
    .is_equal = val_op_false,

    .value_of = val_as_integer,
};


