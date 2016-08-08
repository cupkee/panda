


#ifndef __LANG_EXECUTABLE_INC__
#define __LANG_EXECUTABLE_INC__

#include "config.h"

#include "val.h"

typedef struct module_t {
    double      *numbers;
    intptr_t    *natives;
    intptr_t    *strings;

    int entry;
} module_t;

typedef struct executable_t {
    uint32_t  memory_size;

    uint16_t  string_max;
    uint16_t  string_num;

    uint16_t  number_max;
    uint16_t  number_num;

    uint16_t  native_max;
    uint16_t  native_num;

    uint16_t  func_max;
    uint16_t  func_num;

    uint16_t  symbal_buf_max;
    uint16_t  symbal_buf_end;

    void     *memory_base;

    double   *number_map;
    intptr_t *string_map;
    intptr_t *native_map;
    intptr_t *native_entry;
    uint8_t **func_map;
    char     *symbal_buf;

    uint32_t  main_code_end;
    uint32_t  main_code_max;
    uint32_t  func_code_end;
    uint32_t  func_code_max;

    uint8_t  *main_code;
    uint8_t  *func_code;
} executable_t;

int executable_init(executable_t *exe, void *memory, uint32_t size,
                    int number_max, int string_max, int native_max, int func_max,
                    int main_code_max, int func_code_max);

int executable_number_find_add(executable_t *exe, double n);
int executable_string_find_add(executable_t *exe, intptr_t s);
int executable_native_add(executable_t *exe, intptr_t sym_id, intptr_t entry);
int executable_native_find(executable_t *exe, intptr_t sym_id);

/*
int executable_string_add(executable_t *exe, const char *s);
int executable_string_get(executable_t *exe, int id, const char **p);
int executable_string_lookup(executable_t *exe, const char *p);

int executable_number_add(executable_t *exe, double n);
int executable_number_get(executable_t *exe, int id, double *p);
int executable_number_lookup(executable_t *exe, double n);

int executable_native_add(executable_t *exe, intptr_t fn);
int executable_native_get(executable_t *exe, int id, intptr_t *p);
int executable_native_lookup(executable_t *exe, intptr_t fn);

int executable_function_add(executable_t *exe, int id, uint8_t *code, uint16_t size, uint8_t vc, uint8_t ac);
int executable_function_get(executable_t *exe, int id, uint8_t **code, uint16_t *size, uint8_t *vc, uint8_t *ac);
*/

#endif /* __LANG_EXECUTABLE_INC__ */

