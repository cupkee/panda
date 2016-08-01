
#include "err.h"
#include "string.h"

int string_compare(val_t *a, val_t *b)
{
    const char *s1 = val_2_cstring(a);
    const char *s2 = val_2_cstring(b);

    if (s1 && s2) {
        printf("compare: '%s'%lu - '%s'%lu\n", s1, strlen(s1), s2, strlen(s2));
        return strcmp(s1, s2);
    } else {
        return 1;
    }
}

val_t string_concat(env_t *env, val_t *a, val_t *b)
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
