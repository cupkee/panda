
#include "err.h"
#include "string.h"

int string_compare(val_t *a, val_t *b)
{
    const char *s1 = val_2_cstring(a);
    const char *s2 = val_2_cstring(b);

    if (s1 && s2) {
        return strcmp(s1, s2);
    } else {
        return 1;
    }
}

void string_at(env_t *env, val_t *a, val_t *b, val_t *res)
{
    const char *s = val_2_cstring(a);
    int l = strlen(s);
    int i = val_2_integer(b);

    (void) env;
    if (i >= 0 && i < l) {
        val_set_inner_string(res, s[i]);
    } else {
        val_set_undefined(res);
    }
}

void string_add(env_t *env, val_t *a, val_t *b, val_t *res)
{
    if (!val_is_string(b)) {
        val_set_nan(res);
        return;
    }

    int size1 = string_len(a);
    int size2 = string_len(b);
    int len = size1 + size2, head = 3;
    char *buf = env_heap_alloc(env, head + size1 + size2 + 1);

    // Todo: length overflow should be check! or variable length field.
    if (buf) {
        buf[0] = MAGIC_STRING;
        buf[1] = len >> 8;
        buf[2] = len;
        memcpy(buf + head, val_2_cstring(a), size1);
        memcpy(buf + head + size1, val_2_cstring(b), size2 + 1);
        val_set_owned_string(res, (intptr_t) buf);
    } else {
        env_set_error(env, ERR_NotEnoughMemory);
        val_set_undefined(res);
    }
}

val_t string_length(env_t *env, int ac, val_t *av)
{
    if (ac > 0) {
        if (val_is_inline_string(av)) {
            return val_mk_number(1);
        } else
        if (val_is_static_string(av)) {
            return val_mk_number(strlen((char *)val_2_intptr(av)));
        } else
        if (val_is_owned_string(av)) {
            uint8_t *head = (uint8_t *)val_2_intptr(av);
            return val_mk_number(head[1] * 256 + head[2]);
        }
    }
    env_set_error(env, ERR_InvalidInput);
    return val_mk_undefined();
}

val_t string_index_of(env_t *env, int ac, val_t *av)
{
    const char *s, *f;

    if (ac < 2 || NULL == (s = val_2_cstring(av))) {
        env_set_error(env, ERR_InvalidInput);
        return val_mk_undefined();
    } else
    if (NULL == (f = val_2_cstring(av+1))) {
        return val_mk_number(-1);
    } else {
        char *pos = strstr(s, f);
        if (pos)  {
            return val_mk_number(pos - s);
        } else {
            return val_mk_number(-1);
        }
    }
}

