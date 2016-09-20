
#ifndef __SAL_INC__
#define __SAL_INC__

#include "config.h"

int output(const char *s);

void *file_load(const char *name, int *size);
int file_release(void *map, int sz);
int file_store(const char *name, void *data, int len);
int file_base_name(const char *name, void *buf, int sz);

#endif /* __SAL_INC__ */

