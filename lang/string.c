
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

    if (i >= 0 && i < l) {
        val_set_inner_string(res, s[i]);
    } else {
        val_set_undefined(res);
    }
}

void string_add(env_t *env, val_t *a, val_t *b, val_t *res)
{
    const char *s1 = val_2_cstring(a);
    const char *s2 = val_2_cstring(b);
    int size1 = strlen(s1);
    int size2 = strlen(s2);
    char *buf = env_heap_alloc(env, size1 + size2 + 1);

    if (buf) {
        buf[0] = 0;
        memcpy(buf, s1, size1);
        memcpy(buf + size1, s2, size2);
        buf[size1 + size2] = 0;
        val_set_owned_string(res, (intptr_t) buf);
    } else {
        env_set_error(env, ERR_NotEnoughMemory);
        val_set_undefined(res);
    }
}

val_t string_length(env_t *env, int ac, val_t *av)
{
    const char *s;

    if (ac < 1 || NULL == (s = val_2_cstring(av))) {
        env_set_error(env, ERR_InvalidInput);
        return val_mk_undefined();
    } else {
        return val_mk_number(strlen(s));
    }
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

