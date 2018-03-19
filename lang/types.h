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

#ifndef __LANG_TYPES_INC__
#define __LANG_TYPES_INC__

#include "def.h"
#include "val.h"

extern const val_metadata_t metadata_undefined;
extern const val_metadata_t metadata_nan;
extern const val_metadata_t metadata_date;
extern const val_metadata_t metadata_array_buffer;
extern const val_metadata_t metadata_data_view;
extern const val_metadata_t metadata_object_foreign;
extern const val_metadata_t metadata_none;

int foreign_is_true(val_t *self);
int foreign_is_equal(val_t *self, val_t *other);
double foreign_value_of(val_t *self);

val_t foreign_get_prop(void *env, val_t *self, const char *key);
val_t foreign_get_elem(void *env, val_t *self, int id);

void foreign_set_prop(void *env, val_t *self, const char *key, val_t *data);
void foreign_set_elem(void *env, val_t *self, int id, val_t *data);

void foreign_opx_prop(void *env, val_t *self, const char *key, val_t *res, val_opx_t op);
void foreign_opx_elem(void *env, val_t *self, int id, val_t *res, val_opx_t op);

void foreign_opxx_prop(void *env, val_t *self, const char *key, val_t *data, val_t *res, val_opxx_t op);
void foreign_opxx_elem(void *env, val_t *self, int id, val_t *data, val_t *res, val_opxx_t op);

void foreign_set(void *env, val_t *self, val_t *b, val_t *r);
void foreign_keep(intptr_t entry);

#endif /* __LANG_TYPES_INC__ */


