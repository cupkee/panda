
#ifndef __PANDA_INC__
#define __PANDA_INC__

#include "sal/sal.h"
#include "lang/bcode.h"
#include "lang/interp.h"
#include "lang/compile.h"
#include "lang/err.h"

int panda_native_init(env_t *env);

int panda_interactive(void *mem_ptr, int mem_size, int stack_size, int heap_size);
int panda_binary(const char *input, void *mem_ptr, int mem_size, int stack_size, int heap_size);
int panda_string(const char *input, void *mem_ptr, int mem_size, int stack_size, int heap_size);
int panda_compile(const char *input, void *mem_ptr, int mem_size);

int panda_dump_ef(const char *input);

#endif /* __PANDA_INC__ */

