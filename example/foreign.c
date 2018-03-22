#include "example.h"

val_t foreign_set(void *env, val_t *self, val_t *val)
{
    (void) env;
    (void) self;

    return *val;
}

void foreign_keep(intptr_t entry)
{
    (void) entry;
}

int foreign_is_true(val_t *self)
{
    (void) self;
    return 0;
}

int foreign_is_equal(val_t *self, val_t *other)
{
    (void) self;
    (void) other;
    return 0;
}

double foreign_value_of(val_t *self)
{
    (void) self;
    return 0;
}

val_t  foreign_get_prop(void *env, val_t *self, const char *key)
{
    (void) env;
    (void) self;
    (void) key;
    return VAL_UNDEFINED;
}

val_t foreign_get_elem(void *env, val_t *self, int id)
{
    (void) env;
    (void) self;
    (void) id;
    return VAL_UNDEFINED;
}

void foreign_set_prop(void *env, val_t *self, const char *key, val_t *data)
{
    (void) env;
    (void) self;
    (void) key;
    (void) data;
}

void foreign_set_elem(void *env, val_t *self, int id, val_t *data)
{
    (void) env;
    (void) self;
    (void) id;
    (void) data;
}

void foreign_opx_prop(void *env, val_t *self, const char *key, val_t *res, val_opx_t op)
{
    (void) env;
    (void) self;
    (void) key;
    (void) res;
    (void) op;
}

void foreign_opx_elem(void *env, val_t *self, int id, val_t *res, val_opx_t op)
{
    (void) env;
    (void) self;
    (void) id;
    (void) res;
    (void) op;
}

void foreign_opxx_prop(void *env, val_t *self, const char *key, val_t *data, val_t *res, val_opxx_t op)
{
    (void) env;
    (void) self;
    (void) key;
    (void) data;
    (void) res;
    (void) op;
}

void foreign_opxx_elem(void *env, val_t *self, int id, val_t *data, val_t *res, val_opxx_t op)
{
    (void) env;
    (void) self;
    (void) id;
    (void) data;
    (void) res;
    (void) op;
}

