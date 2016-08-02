


#include "esbl/htbl.h"
#include "esbl/alg.h"

#include "symtbl.h"

typedef uint32_t (*hash_fn)(intptr_t);
typedef int (*cmp_fn)(intptr_t, intptr_t);
typedef void (*free_fn)(intptr_t);

intptr_t symtbl_create(void)
{
    return htbl_create((hash_fn)hash_pjw, (cmp_fn)strcmp);
}

int symtbl_destroy(intptr_t symtbl)
{
    return htbl_destroy(symtbl, (free_fn)free);
}

intptr_t symtbl_add(intptr_t symtbl, const char *sym)
{
    intptr_t ret;

    if (!symtbl) {
        return 0;
    }

    if (htbl_lookup(symtbl, (intptr_t)sym, &ret) == 1) {
        return ret;
    }

    ret = (intptr_t) strdup(sym);
    if (ret && 1 != htbl_insert(symtbl, ret)) {
        free((void *)ret);
        return 0;
    }

    return ret;
}

intptr_t symtbl_get(intptr_t symtbl, const char *sym)
{
    intptr_t ret;

    if (symtbl && htbl_lookup(symtbl, (intptr_t)sym, &ret) == 1) {
        return ret;
    }

    return 0;
}
