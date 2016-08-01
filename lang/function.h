


#ifndef __LANG_FUNCTION_INC__
#define __LANG_FUNCTION_INC__

#include "config.h"

#include "val.h"
#include "env.h"
#include "interp.h"

typedef val_t (*function_native_t) (env_t *env, int ac, val_t *av);

intptr_t  function_create(uint8_t *code, int size, uint8_t vn, uint8_t an);
int function_destroy(intptr_t func);

int function_call(intptr_t fn, env_t *env, int ac, val_t *av, uint8_t **pc);
int function_call_native(intptr_t fn, env_t *env, int ac, val_t *av, uint8_t **pc);

#endif /* __LANG_FUNCTION_INC__ */

