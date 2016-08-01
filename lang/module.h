


#ifndef __LANG_MODULE_INC__
#define __LANG_MODULE_INC__

#include "config.h"

typedef struct fn_template_t {
    uint8_t var_num;
    uint8_t arg_num;
    int     size;
    uint8_t *code;
} fn_template_t;

typedef struct module_t {
    double      *numbers;
    intptr_t    *natives;
    intptr_t    *strings;

    int entry;
    fn_template_t ft[4];
} module_t;

#endif /* __LANG_MODULE_INC__ */

