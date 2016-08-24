
#ifndef __PANDA_INC__
#define __PANDA_INC__

#include "lang/interp.h"
#include "lang/err.h"

int panda_native_init(env_t *env);

int panda_interactive(void *mem_ptr, int mem_size, int stack_size, int heap_size);
int panda_binary(const char *input, void *mem_ptr, int mem_size, int stack_size, int heap_size);
int panda_string(const char *input, void *mem_ptr, int mem_size, int stack_size, int heap_size);

#endif /* __PANDA_INC__ */

