
#include "object.h"
#include "string.h"

typedef struct object_string_t {
    object_t    obj;
    int         len;
    const char *buf;
} object_string_t;

typedef struct object_number_t {
    object_t obj;
    double value;
} object_number_t;

typedef struct object_array_t {
    object_t obj;
    uint32_t size;
    uint32_t bgn;
    uint32_t end;
    val_t   *buf;
} object_array_t;

static object_t *Object = NULL;
static object_t *Array = NULL;
static object_t *String = NULL;
static object_t *Number = NULL;
static object_t *Boolean = NULL;
static object_t *Undefined = NULL;
static object_t *NaN = NULL;

static object_t object_proto;
static object_t undefined_proto;
static object_t nan_proto;
static object_t boolean_proto;
static object_number_t number_proto;
static object_string_t string_proto;

static intptr_t object_prop_keys[1] = {(intptr_t)"toString"};
static intptr_t string_prop_keys[2] = {(intptr_t)"length", (intptr_t)"indexOf"};
static val_t object_prop_vals[1];
static val_t string_prop_vals[2];

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

static val_t *object_add_prop(env_t *env, object_t *obj, intptr_t symbal) {
    val_t *vals;
    intptr_t *keys;

    if (obj->prop_size <= obj->prop_num) {
        int size = obj->prop_size * 2;

        keys = (intptr_t*) env_heap_alloc(env, sizeof(intptr_t) * size + sizeof(val_t) * size);
        if (!keys) {
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

static val_t *object_get_prop(object_t *obj, intptr_t symbal) {
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

static val_t *object_get_prop_owned(object_t *obj, intptr_t symbal) {
    int i;

    for (i = 0; i < obj->prop_num; i++) {
        if (obj->keys[i] == symbal) {
            return obj->vals + i;
        }
    }
    return NULL;
}

int objects_env_init(env_t *env)
{
    int i;
    object_prop_vals[0] = val_mk_native((intptr_t) object_to_string);

    string_prop_vals[0] = val_mk_native((intptr_t) string_length);
    string_prop_vals[1] = val_mk_native((intptr_t) string_index_of);

    Object    = &object_proto;
    String    = (object_t *)&string_proto;
    Number    = (object_t *)&number_proto;
    Undefined = &undefined_proto;
    NaN       = &nan_proto;
    Boolean   = &boolean_proto;

    Object->magic = MAGIC_OBJECT_STATIC;
    Object->proto = NULL;
    Object->prop_num = 1;
    Object->keys = object_prop_keys;
    Object->vals = object_prop_vals;
    for (i = 0; i < 1; i++) {
        env_symbal_add_static(env, (const char *)object_prop_keys[i]);
    }

    String->magic = MAGIC_OBJECT_STATIC;
    String->proto = Object;
    String->prop_num = 2;
    String->keys = string_prop_keys;
    String->vals = string_prop_vals;
    for (i = 0; i < 2; i++) {
        env_symbal_add_static(env, (const char *)string_prop_keys[i]);
    }

    Number->magic = MAGIC_OBJECT_STATIC;
    Number->proto = Object;
    Number->prop_num = 0;

    Undefined->magic = MAGIC_OBJECT_STATIC;
    Undefined->proto = Object;
    Undefined->prop_num = 0;

    NaN->magic = MAGIC_OBJECT_STATIC;
    NaN->proto = Object;
    NaN->prop_num = 0;

    Boolean->magic = MAGIC_OBJECT_STATIC;
    Boolean->proto = Object;
    Boolean->prop_num = 0;

    return env->error;
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
    } else
    if (val_is_array(self)) {
        obj = Array;
    } else
    if (val_is_number(self)) {
        obj = Number;
    } else
    if (val_is_string(self)) {
        obj = String;
    } else
    if (val_is_boolean(self)) {
        obj = Boolean;
    } else
    if (val_is_undefined(self)) {
        obj = Undefined;
    } else
    if (val_is_nan(self)) {
        obj = NaN;
    } else {
        obj = NULL;
    }

    if (obj) {
        val_t *v = object_get_prop(obj, env_symbal_get(env, name));
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

void object_elem_get(env_t *env, val_t *self, val_t *key, val_t *elem)
{
    if (val_is_number(key)) {
        if (val_is_string(self)) {
            string_at(env, self, key, elem);
        } else
        if (val_is_array(self)) {
            //array_at(env, self, key, elem);
            env_set_error(env, ERR_NotImplemented);
        } else {
            env_set_error(env, ERR_HasNoneElement);
        }
    } else {
        object_prop_get(env, self, key, elem);
    }
}

void object_prop_set(env_t *env, val_t *self, val_t *key, val_t *val)
{
    const char *name = val_2_cstring(key);

    if (!name) {
        env_set_error(env, ERR_SysError);
        return;
    }

    if (val_is_dictionary(self) || val_is_array(self)) {
        object_t *obj = (object_t *) val_2_intptr(self);
        intptr_t sym_id = env_symbal_get(env, name);
        val_t *prop;

        if (sym_id) {
            prop = object_get_prop_owned(obj, sym_id);
            if (!prop) {
                prop = object_add_prop(env, obj, sym_id);
            }
        } else {
            prop = object_add_prop(env, obj, env_symbal_add(env, name));
        }

        if (prop) {
            *prop = *val;
        } else {
            env_set_error(env, ERR_NotEnoughMemory);
        }
    } else {
        env_set_error(env, ERR_HasNoneProperty);
    }
}

void object_elem_set(env_t *env, val_t *self, val_t *key, val_t *val)
{
    if (val_is_number(key)) {
        if (val_is_array(self)) {
            //array_at(env, self, key, elem);
            env_set_error(env, ERR_NotImplemented);
        } else {
            env_set_error(env, ERR_InvalidSementic);
        }
    } else {
        object_prop_set(env, self, key, val);
    }
}

intptr_t object_create(env_t *env, int n, val_t *av)
{
    object_t *obj;
    int size;

    if ((n & 1) || n > 255 * 2) {
        return 0;
    }
    size = n / 2;
    size = size < 4 ? 4 : size;

    obj = (object_t *) env_heap_alloc(env, sizeof(object_t) + sizeof(intptr_t) * size + sizeof(val_t) * size);
    if (obj) {
        int i = 0, off = 0;

        obj->magic = MAGIC_OBJECT;
        obj->age = 0;
        obj->prop_size = size;
        obj->prop_num = n / 2;
        obj->keys = (intptr_t *)(obj + 1);
        obj->vals = (val_t *)(obj->keys + size);
        obj->proto = Object;
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

