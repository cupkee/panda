
#ifndef __LANG_SYMTBL_INC__
#define __LANG_SYMTBL_INC__

#include "config.h"

intptr_t symtbl_create(void);
int      symtbl_destroy(intptr_t symtbl);

intptr_t symtbl_add(intptr_t symtbl, const char *sym);
intptr_t symtbl_get(intptr_t symtbl, const char *sym);

#endif /* __LANG_SYMTBL_INC__ */

