/*
MIT License

Copyright (c) 2016 Lixing Ding <ding.lixing@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "val.h"
#include "type_string.h"
#include "type_number.h"

val_t number_to_string(env_t *env, int ac, val_t *av)
{
    (void) env;
    (void) ac;
    (void) av;

    return val_mk_foreign_string((intptr_t)"<number>");
}

static int number_is_true(val_t *self) {
    return val_2_double(self) != 0;
}

int number_is_gt(val_t *self, val_t *to)
{
    if (val_is_boolean(to)) {
        return val_2_double(self) > val_2_intptr(to);
    } else
    if (val_is_number(to)) {
        return val_2_double(self) > val_2_double(to);
    } else {
        return 0;
    }
}

int number_is_ge(val_t *self, val_t *to)
{
    if (val_is_boolean(to)) {
        return val_2_double(self) >= val_2_intptr(to);
    } else
    if (val_is_number(to)) {
        return val_2_double(self) >= val_2_double(to);
    } else {
        return 0;
    }
}

int number_is_lt(val_t *self, val_t *to)
{
    if (val_is_boolean(to)) {
        return val_2_double(self) < val_2_intptr(to);
    } else
    if (val_is_number(to)) {
        return val_2_double(self) < val_2_double(to);
    } else {
        return 0;
    }
}

int number_is_le(val_t *self, val_t *to)
{
    if (val_is_boolean(to)) {
        return val_2_double(self) <= val_2_intptr(to);
    } else
    if (val_is_number(to)) {
        return val_2_double(self) <= val_2_double(to);
    } else {
        return 0;
    }
}

const val_metadata_t metadata_num = {
    .is_true  = number_is_true,
    .is_equal = val_op_false,
    .is_gt    = number_is_gt,
    .is_ge    = number_is_ge,
    .is_lt    = number_is_lt,
    .is_le    = number_is_le,

    .value_of = val_as_number,
};

