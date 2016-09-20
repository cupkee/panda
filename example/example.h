
#ifndef __EXAMPLE_INC__
#define __EXAMPLE_INC__

#include "lang/bcode.h"
#include "lang/interp.h"
#include "lang/compile.h"
#include "lang/err.h"

int output(const char *s);

void *file_load(const char *name, int *size);
int file_release(void *map, int sz);
int file_store(const char *name, void *data, int len);
int file_base_name(const char *name, void *buf, int sz);

int native_init(env_t *env);

#endif /* __EXAMPLE_INC__ */

