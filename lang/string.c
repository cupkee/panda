
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

val_t string_add(env_t *env, val_t *a, val_t *b)
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
        return val_mk_owned_string((intptr_t) buf);
    } else {
        env_set_error(env, ERR_NotEnoughMemory);
        return val_mk_undefined();
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

