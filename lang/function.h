


#ifndef __LANG_FUNCTION_INC__
#define __LANG_FUNCTION_INC__

#include "config.h"

#include "val.h"
#include "env.h"
#include "interp.h"

typedef struct function_info_t {
    uint8_t var_num;
    uint8_t arg_num;
    uint16_t size;
    uint8_t *code;
} function_info_t;

typedef val_t (*function_native_t) (env_t *env, int ac, val_t *av);

intptr_t  function_create(env_t *env, uint8_t *code, int size, uint8_t vn, uint8_t an);
int function_destroy(intptr_t func);

int function_call(intptr_t fn, env_t *env, int ac, val_t *av, uint8_t **pc);
int function_call_native(intptr_t fn, env_t *env, int ac, val_t *av, uint8_t **pc);

static inline void function_info_read(uint8_t *data, function_info_t *info) {
    info->var_num = data[0];
    info->arg_num = data[1];
    info->size = data[2] * 256 + data[3];
    info->code = data + 4;
}

#endif /* __LANG_FUNCTION_INC__ */

