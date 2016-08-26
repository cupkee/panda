
#ifndef __SAL_INC__
#define __SAL_INC__

#include "config.h"

int output(const char *s);

void *file_load(const char *name, int *size);
int file_release(void *map, int sz);
int file_store(const char *name, void *data, int len);
int file_base_name(const char *name, void *buf, int sz);

void write_uint16_be(void *addr, uint16_t *d);
void write_uint16_le(void *addr, uint16_t *d);
void write_uint32_be(void *addr, uint32_t *d);
void write_uint32_le(void *addr, uint32_t *d);
void write_uint64_be(void *addr, uint64_t *d);
void write_uint64_le(void *addr, uint64_t *d);
void write_double_be(void *addr, double *d);
void write_double_le(void *addr, double *d);

uint32_t read_uint32_be(void *addr);
uint32_t read_uint32_le(void *addr);
double read_double_be(void *addr);
double read_double_le(void *addr);

#endif /* __SAL_INC__ */

