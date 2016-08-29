


#ifndef __LANG_FUNCTION_INC__
#define __LANG_FUNCTION_INC__

#include "config.h"

#include "val.h"
#include "env.h"
#include "interp.h"

#define MAGIC_FUNCTION  (MAGIC_BASE + 5)

/*
typedef struct function_info_t {
    uint8_t var_num;
    uint8_t arg_num;
    uint16_t size;
    uint8_t *code;
} function_info_t;
*/

typedef struct function_t {
    uint8_t magic;
    uint8_t reserved;
    uint8_t arg_num;
    uint8_t var_num;
    uint32_t size;
    uint8_t *code;
    scope_t *super;
} function_t;

typedef val_t (*function_native_t) (env_t *env, int ac, val_t *av);

intptr_t  function_create(env_t *env, uint8_t *code, uint32_t size, uint8_t vn, uint8_t an);
int function_destroy(intptr_t func);

int function_call(val_t *fv, env_t *env, int ac, val_t *av, uint8_t **pc);
int function_call_native(val_t * fv, env_t *env, int ac, val_t *av);

static inline
int function_mem_space(function_t *f) {
    return SIZE_ALIGN(sizeof(function_t));
}

/*
static inline
void function_info_read(uint8_t *data, function_info_t *info) {
    info->var_num = data[0];
    info->arg_num = data[1];
    info->size = data[2] * 256 + data[3];
    info->code = data + 4;
}
*/

#endif /* __LANG_FUNCTION_INC__ */

