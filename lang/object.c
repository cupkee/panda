
#include "object.h"
#include "string.h"

typedef struct property_t {
    intptr_t key;
    val_t    val;
} property_t;

typedef struct object_t {
    uint8_t type;
    uint8_t reserved;
    uint8_t prop_size;
    uint8_t prop_num;
    struct object_t   *proto;
    struct property_t *props;
} object_t;

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


static object_t *Object = NULL, *String = NULL, *Number = NULL, *Boolean = NULL, *Undefined = NULL, *NaN = NULL;

static object_t object_proto;
static object_t undefined_proto;
static object_t nan_proto;
static object_t boolean_proto;
static object_number_t number_proto;
static object_string_t string_proto;

static property_t object_props[1];
static property_t number_props[1];
static property_t string_props[2];

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

static void object_get_prop(object_t *obj, intptr_t symbal, val_t *prop) {
    object_t *cur = obj;
    int i;

    while (cur) {
        for (i = 0; i < cur->prop_num; i++) {
            if (cur->props[i].key == symbal) {
                *prop = cur->props[i].val;
                return;
            }
        }
        cur = cur->proto;
    }
    val_set_undefined(prop);
}

int objects_env_init(env_t *env)
{
    object_props[0].key = env_symbal_add(env, "toString");
    object_props[0].val = val_mk_native((intptr_t) object_to_string);

    string_props[0].key = env_symbal_add(env, "length");
    string_props[0].val = val_mk_native((intptr_t) string_length);
    string_props[1].key = env_symbal_add(env, "indexOf");
    string_props[1].val = val_mk_native((intptr_t) string_index_of);

    number_props[0].key = env_symbal_add(env, "length");
    number_props[0].val = val_mk_native((intptr_t) string_length);

    Object    = &object_proto;
    String    = (object_t *)&string_proto;
    Number    = (object_t *)&number_proto;
    Undefined = &undefined_proto;
    NaN       = &nan_proto;
    Boolean   = &boolean_proto;

    Object->proto = NULL;
    Object->props = object_props;
    Object->prop_num = 1;

    String->proto = Object;
    String->props = string_props;
    String->prop_num = 2;

    Number->proto = Object;
    Number->props = string_props;
    Number->prop_num = 1;

    Undefined->proto = Object;
    Undefined->props = NULL;
    Undefined->prop_num = 0;

    NaN->proto = Object;
    NaN->props = NULL;
    NaN->prop_num = 0;

    Boolean->proto = Object;
    Boolean->props = NULL;
    Boolean->prop_num = 0;

    return env->error;
}

int object_prop_set(env_t *env, val_t *self, val_t *key, val_t *prop)
{
    env_set_error(env, ERR_NotImplemented);
    return -1;
}

int object_prop_get(env_t *env, val_t *self, val_t *key, val_t *prop)
{
    const char *name = val_2_cstring(key);
    object_t *obj;

    if (!name) {
        return ERR_InvalidSementic;
    }

    if (val_is_object(self)) {
        obj = (object_t *) val_2_intptr(self);
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
        object_get_prop(obj, env_symbal_get(env, name), prop);
    } else {
        val_set_undefined(prop);
        env_set_error(env, ERR_SysError);
    }

    return env->error;
}

int object_elem_get(env_t *env, val_t *self, val_t *key, val_t *elem)
{
    if (val_is_string(self) && val_is_number(key)) {
        string_at(env, self, key, elem);
    } else {
        env_set_error(env, ERR_HasNoneElement);
    }

    return env->error;
}
