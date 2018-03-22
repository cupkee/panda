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


#include "example.h"

static void print_value(val_t *v)
{
    if (val_is_number(v)) {
        char buf[32];
        if (*v & 0xffff) {
            snprintf(buf, 32, "%f", val_2_double(v));
        } else {
            snprintf(buf, 32, "%d", (int)val_2_double(v));
        }
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

static val_t print(env_t *env, int ac, val_t *av)
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
    {"print", print}
};

int native_init(env_t *env)
{
    return env_native_set(env, native_entry, 1);
}

