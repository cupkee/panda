
#ifndef __PANDA_INC__
#define __PANDA_INC__

#include "lang/interp.h"
#include "lang/err.h"

int panda_native_init(env_t *env);

int interactive_panda(void *mem_ptr, int mem_size, int stack_size, int heap_size);
int binary_panda(const char *input, void *mem_ptr, int mem_size, int stack_size, int heap_size);
int string_panda(const char *input, void *mem_ptr, int mem_size, int stack_size, int heap_size);

#endif /* __PANDA_INC__ */

