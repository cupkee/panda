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

#ifndef __LANG_GC_INC__
#define __LANG_GC_INC__

#include "scope.h"

//scope_t *gc_copy_scope(heap_t *heap, scope_t *scope);
//void gc_copy_vals(heap_t *heap, int vc, val_t *vp);
void gc_scan(void *env);

void     gc_types_copy(void *env, int n, val_t *p);
scope_t *gc_scope_copy(void *env, scope_t *scope);

#endif /* __LANG_GC_INC__ */

