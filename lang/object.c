
#include "interp.h"
#include "number.h"
#include "string.h"
#include "array.h"
#include "object.h"

static object_t object_proto;
static object_t array_proto;
static object_t undefined_proto;
static object_t nan_proto;
static object_t boolean_proto;
static object_t number_proto;
static object_t string_proto;

static intptr_t object_prop_keys[3] = {(intptr_t)"length", (intptr_t)"toString", (intptr_t)"foreach"};
static val_t object_prop_vals[3];

static intptr_t string_prop_keys[1] = {(intptr_t)"indexOf"};
static val_t string_prop_vals[1];

static intptr_t array_prop_keys[5]  = {(intptr_t)"push", (intptr_t)"pop", (intptr_t)"shift", (intptr_t)"unshift", (intptr_t)"foreach"};
static val_t array_prop_vals[5];


static val_t *object_add_prop(env_t *env, object_t *obj, intptr_t symbal) {
    val_t *vals;
    intptr_t *keys;

    if (obj->prop_size <= obj->prop_num) {
        int size;

        if (obj->prop_size >= UINT16_MAX) {
            env_set_error(env, ERR_ResourceOutLimit);
            return NULL;
        }
        size = obj->prop_size * 2;
        size = size < UINT16_MAX ? size : UINT16_MAX;

        keys = (intptr_t*) env_heap_alloc(env, sizeof(intptr_t) * size + sizeof(val_t) * size);
        if (!keys) {
            env_set_error(env, ERR_NotEnoughMemory);
            return NULL;
        }
        vals = (val_t *)(keys + size);

        memcpy(keys, obj->keys, sizeof(intptr_t) * obj->prop_num);
        memcpy(vals, obj->vals, sizeof(val_t) * obj->prop_num);
        obj->keys = keys;
        obj->vals = vals;
    } else {
        vals = obj->vals;
        keys = obj->keys;
    }

    keys[obj->prop_num] = symbal;
    return vals + obj->prop_num++;
}

static val_t *object_find_prop(object_t *obj, intptr_t symbal) {
    object_t *cur = obj;
    int i;

    while (cur) {
        intptr_t *keys = cur->keys;
        for (i = 0; i < cur->prop_num; i++) {
            if (keys[i] == symbal) {
                return cur->vals + i;
            }
        }
        cur = cur->proto;
    }
    return NULL;
}

static val_t *object_find_prop_owned(object_t *obj, intptr_t symbal) {
    int i;

    for (i = 0; i < obj->prop_num; i++) {
        if (obj->keys[i] == symbal) {
            return obj->vals + i;
        }
    }
    return NULL;
}

static inline void object_static_register(env_t *env, object_t *o) {
    int i;

    for (i = 0; i < o->prop_num; i++) {
        env_symbal_add_static(env, (const char *)o->keys[i]);
    }
}

static inline object_t *_object_proto_get(val_t *obj) {
    if (val_is_array(obj)) {
        return &array_proto;
    } else
    if (val_is_number(obj)) {
        return &number_proto;
    } else
    if (val_is_string(obj)) {
        return &string_proto;
    } else
    if (val_is_boolean(obj)) {
        return &boolean_proto;
    } else
    if (val_is_undefined(obj)) {
        return &undefined_proto;
    } else
    if (val_is_nan(obj)) {
        return &nan_proto;
    } else {
        return NULL;
    }
}

static val_t object_length(env_t *env, int ac, val_t *av)
{
    if (ac > 0) {
        if (val_is_dictionary(av)) {
            object_t *o = (object_t *)val_2_intptr(av);
            return val_mk_number(o->prop_num);
        } else
        if (val_is_array(av)) {
            array_t *a = (array_t *)val_2_intptr(av);
            return val_mk_number(array_length(a));
        } else
        if (val_is_inline_string(av)) {
            return val_mk_number(string_inline_len(av));
        } else
        if (val_is_static_string(av)) {
            return val_mk_number(string_static_len(av));
        } else
        if (val_is_owned_string(av)) {
            return val_mk_number(string_owned_len(av));
        } else {
            return val_mk_number(1);
        }
    }

    env_set_error(env, ERR_InvalidInput);
    return val_mk_undefined();
}

static val_t object_to_string(env_t *env, int ac, val_t *obj)
{
    if (ac < 1) {
        env_set_error(env, ERR_InvalidInput);
        return val_mk_undefined();
    }

    if (val_is_string(obj)) {
        return *obj;
    } else
    if (val_is_number(obj)) {
        return val_mk_static_string((intptr_t)"number");
    } else
    if (val_is_undefined(obj)) {
        return val_mk_static_string((intptr_t)"undefined");
    } else
    if (val_is_nan(obj)) {
        return val_mk_static_string((intptr_t)"NaN");
    } else
    if (val_is_boolean(obj)) {
        return val_mk_static_string((intptr_t)(val_2_intptr(obj) ? "true" : "false"));
    } else
    if (val_is_array(obj)) {
        return val_mk_static_string((intptr_t)"Array");
    } else {
        return val_mk_static_string((intptr_t)"Object");
    }
}

static val_t object_foreach(env_t *env, int ac, val_t *av)
{
    if (ac > 1 && val_is_dictionary(av) && val_is_function(av + 1)) {
        object_t *o = (object_t *)val_2_intptr(av);
        int i, max = o->prop_num;

        for (i = 0; i < max && !env->error; i++) {
            val_t key = val_mk_static_string(o->keys[i]);

            env_push_call_argument(env, &key);
            env_push_call_argument(env, o->vals + i);
            env_push_call_function(env, av + 1);

            interp_execute_call(env, 2);
        }
    }

    return val_mk_undefined();
}
void object_prop_get(env_t *env, val_t *self, val_t *key, val_t *prop)
{
    const char *name = val_2_cstring(key);
    object_t *obj;

    if (!name) {
        env_set_error(env, ERR_InvalidSementic);
        return;
    }

    if (val_is_dictionary(self)) {
        obj = (object_t *) val_2_intptr(self);
    } else {
        obj = _object_proto_get(self);
    }

    if (obj) {
        val_t *v = object_find_prop(obj, env_symbal_get(env, name));
        if (v) {
            *prop = *v;
        } else {
            val_set_undefined(prop);
        }
    } else {
        val_set_undefined(prop);
        env_set_error(env, ERR_SysError);
    }
}

void object_elem_get(env_t *env, val_t *obj, val_t *key, val_t *elem)
{
    if (val_is_number(key)) {
        if (val_is_string(obj)) {
            string_at(env, obj, key, elem);
        } else
        if (val_is_array(obj)) {
            array_elem_get(env, obj, key, elem);
        } else {
            env_set_error(env, ERR_HasNoneElement);
        }
    } else {
        object_prop_get(env, obj, key, elem);
    }
}

void object_prop_set(env_t *env, val_t *self, val_t *key, val_t *val)
{
    const char *name = val_2_cstring(key);

    if (!name) {
        env_set_error(env, ERR_SysError);
        return;
    }

    if (val_is_dictionary(self)) {
        object_t *obj = (object_t *) val_2_intptr(self);
        intptr_t sym_id = env_symbal_get(env, name);
        val_t *prop;

        if (sym_id) {
            prop = object_find_prop_owned(obj, sym_id);
            if (!prop) {
                prop = object_add_prop(env, obj, sym_id);
            }
        } else {
            prop = object_add_prop(env, obj, env_symbal_add(env, name));
        }

        if (prop) {
            *prop = *val;
        }
    } else {
        env_set_error(env, ERR_HasNoneProperty);
    }
}

void object_elem_set(env_t *env, val_t *self, val_t *key, val_t *val)
{
    if (val_is_number(key)) {
        if (val_is_array(self)) {
            array_elem_set(env, self, key, val);
        } else {
            env_set_error(env, ERR_InvalidSementic);
        }
    } else {
        object_prop_set(env, self, key, val);
    }
}

static int _object_prop_get_owned(env_t *env, val_t *o, val_t *k, val_t **p, const char **n)
{
    const char *name = val_2_cstring(k);
    if (!name) {
        env_set_error(env, ERR_InvalidInput);
        return -1;
    }

    if (!val_is_dictionary(o)) {
        env_set_error(env, ERR_HasNoneProperty);
        return -1;
    }

    *n = name;
    *p = object_find_prop_owned((object_t *)val_2_intptr(o), env_symbal_get(env, name));
    return 0;
}

void object_prop_add_set(env_t *env, val_t *self, val_t *key, val_t *val, val_t *res)
{
    const char  *name;
    val_t *prop;

    if (0 > _object_prop_get_owned(env, self, key, &prop, &name)) {
        return;
    }

    if (prop) {
        if (val_is_number(prop)) {
            number_add(env, prop, val, prop);
        } else
        if (val_is_string(prop)) {
            string_add(env, prop, val, prop);
        } else {
            val_set_nan(prop);
        }
        *res = *prop;
        return;
    }

    prop = object_add_prop(env, (object_t *)val_2_intptr(self), env_symbal_add(env, name));
    if (prop) {
        val_set_nan(prop);
        val_set_nan(res);
    }
}

void object_prop_sub_set(env_t *env, val_t *self, val_t *key, val_t *val, val_t *res)
{
    const char  *name;
    val_t *prop;

    if (0 > _object_prop_get_owned(env, self, key, &prop, &name)) {
        return;
    }

    if (prop) {
        if (val_is_number(prop)) {
            number_sub(env, prop, val, prop);
        } else {
            val_set_nan(prop);
        }
        *res = *prop;
        return;
    }

    prop = object_add_prop(env, (object_t *)val_2_intptr(self), env_symbal_add(env, name));
    if (prop) {
        val_set_nan(prop);
        val_set_nan(res);
    }
}

void object_prop_mul_set(env_t *env, val_t *self, val_t *key, val_t *val, val_t *res)
{
    const char  *name;
    val_t *prop;

    if (0 > _object_prop_get_owned(env, self, key, &prop, &name)) {
        return;
    }

    if (prop) {
        if (val_is_number(prop)) {
            number_mul(env, prop, val, prop);
        } else {
            val_set_nan(prop);
        }
        *res = *prop;
        return;
    }

    prop = object_add_prop(env, (object_t *)val_2_intptr(self), env_symbal_add(env, name));
    if (prop) {
        val_set_nan(prop);
        val_set_nan(res);
    }
}

void object_prop_div_set(env_t *env, val_t *self, val_t *key, val_t *val, val_t *res)
{
    const char  *name;
    val_t *prop;

    if (0 > _object_prop_get_owned(env, self, key, &prop, &name)) {
        return;
    }

    if (prop) {
        if (val_is_number(prop)) {
            number_div(env, prop, val, prop);
        } else {
            val_set_nan(prop);
        }
        *res = *prop;
        return;
    }

    prop = object_add_prop(env, (object_t *)val_2_intptr(self), env_symbal_add(env, name));
    if (prop) {
        val_set_nan(prop);
        val_set_nan(res);
    }
}

void object_prop_mod_set(env_t *env, val_t *self, val_t *key, val_t *val, val_t *res)
{
    const char  *name;
    val_t *prop;

    if (0 > _object_prop_get_owned(env, self, key, &prop, &name)) {
        return;
    }

    if (prop) {
        if (val_is_number(prop)) {
            number_mod(env, prop, val, prop);
        } else {
            val_set_nan(prop);
        }
        *res = *prop;
        return;
    }

    prop = object_add_prop(env, (object_t *)val_2_intptr(self), env_symbal_add(env, name));
    if (prop) {
        val_set_nan(prop);
        val_set_nan(res);
    }
}

void object_prop_and_set(env_t *env, val_t *self, val_t *key, val_t *val, val_t *res)
{
    const char  *name;
    val_t *prop;

    if (0 > _object_prop_get_owned(env, self, key, &prop, &name)) {
        return;
    }

    if (prop) {
        if (val_is_number(prop)) {
            number_and(env, prop, val, prop);
        } else {
            val_set_nan(prop);
        }
        *res = *prop;
        return;
    }

    prop = object_add_prop(env, (object_t *)val_2_intptr(self), env_symbal_add(env, name));
    if (prop) {
        val_set_nan(prop);
        val_set_nan(res);
    }
}

void object_prop_or_set(env_t *env, val_t *self, val_t *key, val_t *val, val_t *res)
{
    const char  *name;
    val_t *prop;

    if (0 > _object_prop_get_owned(env, self, key, &prop, &name)) {
        return;
    }

    if (prop) {
        if (val_is_number(prop)) {
            number_or(env, prop, val, prop);
        } else {
            val_set_nan(prop);
        }
        *res = *prop;
        return;
    }

    prop = object_add_prop(env, (object_t *)val_2_intptr(self), env_symbal_add(env, name));
    if (prop) {
        val_set_nan(prop);
        val_set_nan(res);
    }
}

void object_prop_xor_set(env_t *env, val_t *self, val_t *key, val_t *val, val_t *res)
{
    const char  *name;
    val_t *prop;

    if (0 > _object_prop_get_owned(env, self, key, &prop, &name)) {
        return;
    }

    if (prop) {
        if (val_is_number(prop)) {
            number_xor(env, prop, val, prop);
        } else {
            val_set_nan(prop);
        }
        *res = *prop;
        return;
    }

    prop = object_add_prop(env, (object_t *)val_2_intptr(self), env_symbal_add(env, name));
    if (prop) {
        val_set_nan(prop);
        val_set_nan(res);
    }
}

void object_prop_lshift_set(env_t *env, val_t *self, val_t *key, val_t *val, val_t *res)
{
    const char  *name;
    val_t *prop;

    if (0 > _object_prop_get_owned(env, self, key, &prop, &name)) {
        return;
    }

    if (prop) {
        if (val_is_number(prop)) {
            number_lshift(env, prop, val, prop);
        } else {
            val_set_nan(prop);
        }
        *res = *prop;
        return;
    }

    prop = object_add_prop(env, (object_t *)val_2_intptr(self), env_symbal_add(env, name));
    if (prop) {
        val_set_nan(prop);
        val_set_nan(res);
    }
}

void object_prop_rshift_set(env_t *env, val_t *self, val_t *key, val_t *val, val_t *res)
{
    const char  *name;
    val_t *prop;

    if (0 > _object_prop_get_owned(env, self, key, &prop, &name)) {
        return;
    }

    if (prop) {
        if (val_is_number(prop)) {
            number_rshift(env, prop, val, prop);
        } else {
            val_set_nan(prop);
        }
        *res = *prop;
        return;
    }

    prop = object_add_prop(env, (object_t *)val_2_intptr(self), env_symbal_add(env, name));
    if (prop) {
        val_set_nan(prop);
        val_set_nan(res);
    }
}

void object_elem_add_set(env_t *env, val_t *self, val_t *key, val_t *val, val_t *res)
{
    if (val_is_number(key)) {
        if (val_is_array(self)) {
            array_elem_add_set(env, self, key, val, res);
        } else {
            env_set_error(env, ERR_InvalidSementic);
        }
    } else {
        object_prop_add_set(env, self, key, val, res);
    }
}

void object_elem_sub_set(env_t *env, val_t *self, val_t *key, val_t *val, val_t *res)
{
    if (val_is_number(key)) {
        if (val_is_array(self)) {
            array_elem_sub_set(env, self, key, val, res);
        } else {
            env_set_error(env, ERR_InvalidSementic);
        }
    } else {
        object_prop_sub_set(env, self, key, val, res);
    }
}

void object_elem_mul_set(env_t *env, val_t *self, val_t *key, val_t *val, val_t *res)
{
    if (val_is_number(key)) {
        if (val_is_array(self)) {
            array_elem_mul_set(env, self, key, val, res);
        } else {
            env_set_error(env, ERR_InvalidSementic);
        }
    } else {
        object_prop_mul_set(env, self, key, val, res);
    }
}

void object_elem_div_set(env_t *env, val_t *self, val_t *key, val_t *val, val_t *res)
{
    if (val_is_number(key)) {
        if (val_is_array(self)) {
            array_elem_div_set(env, self, key, val, res);
        } else {
            env_set_error(env, ERR_InvalidSementic);
        }
    } else {
        object_prop_div_set(env, self, key, val, res);
    }
}

void object_elem_mod_set(env_t *env, val_t *self, val_t *key, val_t *val, val_t *res)
{
    if (val_is_number(key)) {
        if (val_is_array(self)) {
            array_elem_mod_set(env, self, key, val, res);
        } else {
            env_set_error(env, ERR_InvalidSementic);
        }
    } else {
        object_prop_mod_set(env, self, key, val, res);
    }
}

void object_elem_and_set(env_t *env, val_t *self, val_t *key, val_t *val, val_t *res)
{
    if (val_is_number(key)) {
        if (val_is_array(self)) {
            array_elem_and_set(env, self, key, val, res);
        } else {
            env_set_error(env, ERR_InvalidSementic);
        }
    } else {
        object_prop_and_set(env, self, key, val, res);
    }
}

void object_elem_or_set(env_t *env, val_t *self, val_t *key, val_t *val, val_t *res)
{
    if (val_is_number(key)) {
        if (val_is_array(self)) {
            array_elem_or_set(env, self, key, val, res);
        } else {
            env_set_error(env, ERR_InvalidSementic);
        }
    } else {
        object_prop_or_set(env, self, key, val, res);
    }
}

void object_elem_xor_set(env_t *env, val_t *self, val_t *key, val_t *val, val_t *res)
{
    if (val_is_number(key)) {
        if (val_is_array(self)) {
            array_elem_xor_set(env, self, key, val, res);
        } else {
            env_set_error(env, ERR_InvalidSementic);
        }
    } else {
        object_prop_xor_set(env, self, key, val, res);
    }
}

void object_elem_lshift_set(env_t *env, val_t *self, val_t *key, val_t *val, val_t *res)
{
    if (val_is_number(key)) {
        if (val_is_array(self)) {
            array_elem_lshift_set(env, self, key, val, res);
        } else {
            env_set_error(env, ERR_InvalidSementic);
        }
    } else {
        object_prop_lshift_set(env, self, key, val, res);
    }
}

void object_elem_rshift_set(env_t *env, val_t *self, val_t *key, val_t *val, val_t *res)
{
    if (val_is_number(key)) {
        if (val_is_array(self)) {
            array_elem_rshift_set(env, self, key, val, res);
        } else {
            env_set_error(env, ERR_InvalidSementic);
        }
    } else {
        object_prop_rshift_set(env, self, key, val, res);
    }
}

intptr_t object_create(env_t *env, int n, val_t *av)
{
    object_t *obj;
    int size;

    if ((n & 1) || n > UINT16_MAX * 2) {
        return 0;
    }

    size = n / 2;
    size = size < DEF_PROP_SIZE ? DEF_PROP_SIZE : size;

    obj = (object_t *) env_heap_alloc(env, sizeof(object_t) + sizeof(intptr_t) * size + sizeof(val_t) * size);
    if (obj) {
        int i = 0, off = 0;

        obj->magic = MAGIC_OBJECT;
        obj->age = 0;
        obj->prop_size = size;
        obj->prop_num = n / 2;
        obj->keys = (intptr_t *)(obj + 1);
        obj->vals = (val_t *)(obj->keys + size);
        obj->proto = &object_proto;
        while (i < n) {
            val_t *k = av + i++;
            val_t *val = av + i++;
            const char *name = val_2_cstring(k);
            intptr_t key;

            if (!name) {
                return 0;
            }

            key = env_symbal_get(env, name);
            if (!key) {
                key = env_symbal_insert(env, name, !val_is_static_string(k));
            }

            if (!key) {
                return 0;
            }
            obj->keys[off] = key;
            obj->vals[off] = *val;

            off++;
        }
    }

    return (intptr_t) obj;
}

int objects_env_init(env_t *env)
{
    object_t *Object    = &object_proto;
    object_t *String    = &string_proto;
    object_t *Number    = &number_proto;
    object_t *Array     = &array_proto;
    object_t *Undefined = &undefined_proto;
    object_t *NaN       = &nan_proto;
    object_t *Boolean   = &boolean_proto;

    object_prop_vals[0] = val_mk_native((intptr_t) object_length);
    object_prop_vals[1] = val_mk_native((intptr_t) object_to_string);
    object_prop_vals[2] = val_mk_native((intptr_t) object_foreach);
    Object->magic = MAGIC_OBJECT_STATIC;
    Object->proto = NULL;
    Object->prop_num = 3;
    Object->keys = object_prop_keys;
    Object->vals = object_prop_vals;
    object_static_register(env, &object_proto);

    string_prop_vals[0] = val_mk_native((intptr_t) string_index_of);
    String->magic = MAGIC_OBJECT_STATIC;
    String->proto = Object;
    String->prop_num = 1;
    String->keys = string_prop_keys;
    String->vals = string_prop_vals;
    object_static_register(env, &string_proto);

    array_prop_vals[0] = val_mk_native((intptr_t) array_push);
    array_prop_vals[1] = val_mk_native((intptr_t) array_pop);
    array_prop_vals[2] = val_mk_native((intptr_t) array_shift);
    array_prop_vals[3] = val_mk_native((intptr_t) array_unshift);
    array_prop_vals[4] = val_mk_native((intptr_t) array_foreach);
    Array->magic = MAGIC_OBJECT_STATIC;
    Array->proto = Object;
    Array->prop_num = 5;
    Array->keys = array_prop_keys;
    Array->vals = array_prop_vals;
    object_static_register(env, &array_proto);

    Number->magic = MAGIC_OBJECT_STATIC;
    Number->proto = Object;
    Number->prop_num = 0;
    object_static_register(env, &number_proto);

    Undefined->magic = MAGIC_OBJECT_STATIC;
    Undefined->proto = Object;
    Undefined->prop_num = 0;
    object_static_register(env, &undefined_proto);

    NaN->magic = MAGIC_OBJECT_STATIC;
    NaN->proto = Object;
    NaN->prop_num = 0;
    object_static_register(env, &nan_proto);

    Boolean->magic = MAGIC_OBJECT_STATIC;
    Boolean->proto = Object;
    Boolean->prop_num = 0;
    object_static_register(env, &boolean_proto);

    return env->error;
}

