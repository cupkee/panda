
#include "alg.h"

uint32_t hash_pjw(const void *key)
{
    const char *ptr = key;
    uint32_t val = 0;

    while (*ptr) {
        uint32_t tmp;

        val = (val << 4) + *ptr;
        tmp = val & 0xf0000000;
        if (tmp) {
            val = (val ^ (tmp >> 24)) ^ tmp;
        }

        ptr++;
    }

    return val;
}

uint32_t hash_djb(const void *key)
{
    const char *ptr = key;
    uint32_t val = 5381;

    while (*ptr) {
        val = (val << 5) + val + *ptr++;
    }

    return val;
}
