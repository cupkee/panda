


#ifndef __LANG_EXECUTABLE_INC__
#define __LANG_EXECUTABLE_INC__

#include "config.h"

#include "val.h"

#define FUNC_HEAD_SIZE 4

#define EXEC_FL_BE     1
#define EXEC_FL_64     2

typedef struct executable_t {
    uint32_t  memory_size;

    uint16_t  string_max;
    uint16_t  string_num;

    uint16_t  number_max;
    uint16_t  number_num;

    uint16_t  func_max;
    uint16_t  func_num;

    void     *memory_base;

    double   *number_map;
    intptr_t *string_map;
    uint8_t **func_map;

    uint32_t  main_code_end;
    uint32_t  main_code_max;
    uint32_t  func_code_end;
    uint32_t  func_code_max;

    uint8_t  *main_code;
    uint8_t  *func_code;
} executable_t;

typedef struct executable_file_t {
    int8_t      error;
    uint8_t     addr_size;
    uint8_t     byte_order;
    uint8_t     version;

    uint32_t    size;
    uint32_t    end;

    uint32_t    num_cnt, num_ent;
    uint32_t    str_cnt, str_ent;
    uint32_t    fn_cnt, fn_ent;

    uint8_t    *base;
} executable_file_t;


int executable_init(executable_t *exe, void *memory, int size,
                    int number_max, int string_max, int func_max,
                    int main_code_max, int func_code_max);

int executable_number_find_add(executable_t *exe, double n);
int executable_string_find_add(executable_t *exe, intptr_t s);

int executable_file_init(executable_file_t *ef, void *mem_ptr, int mem_size, int byte_order, int nc, int sc, int fc);
int executable_file_load(executable_file_t *ef, uint8_t *input, int size);
void executable_file_fill_data(executable_file_t *ef, int nc, double *nv, int sc, intptr_t *sv);
void executable_file_fill_code(executable_file_t *ef, int entry, uint8_t vc, uint8_t ac, uint8_t *code, int size);
double *executable_file_number_entry(executable_file_t *ef);
double executable_file_get_number(executable_file_t *ef, int index);
const char *executable_file_get_string(executable_file_t *ef, int index);
const uint8_t *executable_file_get_function(executable_file_t *ef, int index);

static inline int executable_file_size(executable_file_t *ef) {
    return ef->error ? -1 : ef->end;
}




#endif /* __LANG_EXECUTABLE_INC__ */

