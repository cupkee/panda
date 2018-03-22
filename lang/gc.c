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

#include "val.h"
#include "heap.h"
#include "gc.h"
#include "scope.h"
#include "env.h"
#include "types.h"
#include "type_string.h"
#include "type_function.h"
#include "type_array.h"
#include "type_object.h"
#include "type_buffer.h"

#define MAGIC_BYTE(x) (*((uint8_t *)(x)))
#define ADDR_VALUE(x) (*((void **)(x)))

static inline scope_t *scope_gc_dup(void *env, scope_t *scope) {
    scope_t *dup;
    val_t   *buf = NULL;

    dup = env_heap_alloc(env, scope_mem_space(scope));
    buf = (val_t *)(dup + 1);

    //printf("%s: free %d\n", __func__, heap->free);
    memcpy(dup, scope, sizeof(scope_t));
    memcpy(buf, scope->var_buf, sizeof(val_t) * scope->num);
    dup->var_buf = buf;

    return dup;
}

static inline void scope_gc_scan(void *env, scope_t *scope) {
    scope->super = gc_scope_copy(env, scope->super);
    gc_types_copy(env, scope->num, scope->var_buf);
}

static inline object_t *object_gc_dup(void *env, object_t *obj)
{
    object_t *dup;
    intptr_t *keys;
    val_t    *vals;

    //dup = heap_alloc(heap, sizeof(scope_t) + sizeof(val_t) * scope->num);
    dup = env_heap_alloc(env, object_mem_space(obj));
    keys = (intptr_t *) (dup + 1);
    vals = (val_t *)(keys + obj->prop_size);

    //printf("%s: free %d\n", __func__, heap->free);
    memcpy(dup, obj, sizeof(object_t));
    memcpy(keys, obj->keys, sizeof(intptr_t) * obj->prop_num);
    memcpy(vals, obj->vals, sizeof(val_t) * obj->prop_num);
    dup->keys = keys;
    dup->vals = vals;

    return dup;
}

static inline void object_gc_scan(void *env, object_t *obj)
{
    gc_types_copy(env, obj->prop_num, obj->vals);
}

static inline array_t *array_gc_dup(void *env, array_t *a)
{
    array_t *dup;
    val_t   *vals;

    dup = env_heap_alloc(env, array_mem_space(a));
    vals = (val_t *)(dup + 1);

    //printf("%s: free %d\n", __func__, heap->free);
    memcpy(dup, a, sizeof(array_t));
    memcpy(vals, array_values(a), sizeof(val_t) * array_length(a));
    dup->elems = vals;

    return dup;
}

static inline void array_gc_scan(void *env, array_t *a)
{
    gc_types_copy(env, array_length(a), array_values(a));
}

static inline intptr_t string_gc_dup(void *env, intptr_t str)
{
    int size = string_mem_space(str);
    void *dup = env_heap_alloc(env, size);

    //printf("%s: free %d, %d, %s\n", __func__, heap->free, size, (char *)(str + 3));
    //printf("[str size: %d, '%s']", size, (char *)str + 3);
    memcpy(dup, (void*)str, size);
    //printf("[dup size: %d, '%s']", string_mem_space((intptr_t)dup), dup + 3);

    return (intptr_t) dup;
}

static inline function_t *function_gc_dup(void *env, function_t *func)
{
    function_t *dup = env_heap_alloc(env, function_mem_space(func));

    //printf("%s: free %d\n", __func__, heap->free);
    memcpy(dup, (void*)func, sizeof(function_t));

    return dup;
}

static inline void function_gc_scan(void *env, function_t *func)
{
    func->super = gc_scope_copy(env, func->super);
}

static inline object_t *gc_object_copy(void *env, object_t *obj)
{
    object_t *dup;

    if (!obj || env_is_heap_memory(env, obj)) {
        return obj;
    }

    if (MAGIC_BYTE(obj) != MAGIC_OBJECT) {
        return ADDR_VALUE(obj);
    }
    dup = object_gc_dup(env, obj);
    ADDR_VALUE(obj) = dup;

    return dup;
}

static inline array_t *gc_array_copy(void *env, array_t *a)
{
    array_t *dup;

    if (!a || env_is_heap_memory(env, a)) {
        return a;
    }

    if (MAGIC_BYTE(a) != MAGIC_ARRAY) {
        return ADDR_VALUE(a);
    }
    dup = array_gc_dup(env, a);
    ADDR_VALUE(a) = dup;

    return dup;
}

static inline intptr_t gc_string_copy(void *env, intptr_t str)
{
    if (!str || env_is_heap_memory(env, (void*)str)) {
        return (intptr_t) str;
    }

    if (MAGIC_BYTE(str) != MAGIC_STRING) {
        return (intptr_t) ADDR_VALUE(str);
    } else {
        intptr_t dup = string_gc_dup(env, str);
        ADDR_VALUE(str) = (void *)dup;

        return dup;
    }
}

static inline function_t *gc_function_copy(void *env, function_t *func)
{
    if (!func || env_is_heap_memory(env, func)) {
        return func;
    }

    if (MAGIC_BYTE(func) != MAGIC_FUNCTION) {
        return ADDR_VALUE(func);
    } else {
        function_t *dup = function_gc_dup(env, func);
        ADDR_VALUE(func) = dup;
        return dup;
    }
}

scope_t *gc_scope_copy(void *env, scope_t *scope)
{
    scope_t *dup;

    if (!scope || env_is_heap_memory(env, scope)) {
        return scope;
    }

    if (MAGIC_BYTE(scope) != MAGIC_SCOPE) {
        return ADDR_VALUE(scope);
    }

    dup = scope_gc_dup(env, scope);
    ADDR_VALUE(scope) = dup;

    return dup;
}

void gc_types_copy(void *env, int n, val_t *p)
{
    int i = 0;

    while (i < n) {
        val_t *v = p + i;


        if (val_is_heap_string(v)) {
            val_set_heap_string(v, gc_string_copy(env, val_2_intptr(v)));
        } else
        if (val_is_script(v)) {
            val_set_script(v, (intptr_t)gc_function_copy(env, (void *)val_2_intptr(v)));
        } else
        if (val_is_object(v)) {
            val_set_object(v, (intptr_t)gc_object_copy(env, (object_t *)val_2_intptr(v)));
        } else
        if (val_is_array(v)) {
            val_set_array(v, (intptr_t)gc_array_copy(env, (array_t *)val_2_intptr(v)));
        } else
        if (val_is_foreign(v)) {
            foreign_keep(val_2_intptr(v));
        }
        i++;
    }
}

void gc_scan(void *env)
{
    heap_t  *heap = ((env_t *)env)->heap;
    uint8_t *base = heap->base;
    int      scan = 0;

    while(scan < heap->free) {
        uint8_t magic = base[scan];

        switch(magic) {
        case MAGIC_STRING:
            scan += string_mem_space((intptr_t)(base + scan));
            break;
        case MAGIC_FUNCTION: {
            function_t *func = (function_t *)(base + scan);
            scan += function_mem_space(func);

            function_gc_scan(env, func);

            break;
            }
        case MAGIC_SCOPE: {
            scope_t *scope = (scope_t *) (base + scan);
            scan += scope_mem_space(scope);

            scope_gc_scan(env, scope);

            break;
            }
        case MAGIC_OBJECT: {
            object_t *obj = (object_t *) (base + scan);
            scan += object_mem_space(obj);

            object_gc_scan(env, obj);

            break;
            }
        case MAGIC_ARRAY: {
            array_t *array= (array_t*) (base + scan);
            scan += array_mem_space(array);

            array_gc_scan(env, array);

            break;
            }
        default: break;
        }
    }
}

