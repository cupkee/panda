
#include "panda.h"


static void print_value(val_t *v)
{
    if (val_is_number(v)) {
        char buf[32];
        snprintf(buf, 32, "%f", val_2_double(v));
        output(buf);
    } else
    if (val_is_boolean(v)) {
        output(val_2_intptr(v) ? "true" : "false");
    } else
    if (val_is_string(v)) {
        output("\"");
        output(val_2_cstring(v));
        output("\"");
    } else
    if (val_is_undefined(v)) {
        output("undefined");
    } else
    if (val_is_nan(v)) {
        output("NaN");
    } else
    if (val_is_function(v)) {
        char buf[32];
        snprintf(buf, 32, "function:%ld", val_2_intptr(v));
        output(buf);
    } else {
        output("object");
    }
}

static val_t panda_print(env_t *env, int ac, val_t *av)
{
    int i;

    for (i = 0; i < ac; i++) {
        if (i > 0) {
            output(" ");
        }
        print_value(av+i);
    }
    output("\n");

    return val_mk_undefined();
}

static native_t native_entry[] = {
    {"print", panda_print}
};

int panda_native_init(env_t *env)
{
    return env_native_add(env, 1, native_entry);
}

