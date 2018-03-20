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

#include "types.h"

const val_metadata_t metadata_undefined = {
    .name     = "undefined",

    .is_true  = val_as_false,
    .is_equal = val_op_false,

    .value_of = val_as_nan,
};

const val_metadata_t metadata_nan = {
    .name     = "number",

    .is_true  = val_as_false,
    .is_equal = val_op_false,

    .value_of = val_as_nan,
};

const val_metadata_t metadata_date = {
    .name     = "object",

    .is_true  = val_as_true,
    .is_equal = val_op_false,

    .value_of = val_as_integer,
};

const val_metadata_t metadata_array_buffer = {
    .name     = "object",

    .is_true  = val_as_false,
    .is_equal = val_op_false,

    .value_of = val_as_zero,
};

const val_metadata_t metadata_data_view = {
    .name     = "object",

    .is_true  = val_as_false,
    .is_equal = val_op_false,

    .value_of = val_as_zero,
};

const val_metadata_t metadata_object_foreign = {
    .name     = "object",

    .is_true   = foreign_is_true,
    .is_equal  = foreign_is_equal,

    .value_of  = foreign_value_of,

    .get_prop  = foreign_get_prop,
    .get_elem  = foreign_get_elem,

    .set_prop  = foreign_set_prop,
    .set_elem  = foreign_set_elem,

    .opx_prop  = foreign_opx_prop,
    .opx_elem  = foreign_opx_elem,

    .opxx_prop = foreign_opxx_prop,
    .opxx_elem = foreign_opxx_elem,
};

const val_metadata_t metadata_none = {
    .name     = "object",

    .is_true  = val_as_false,
    .is_equal = val_op_false,

    .value_of = val_as_zero,
};

