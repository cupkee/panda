


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

typedef struct panda_efh_t {
    uint8_t     magic[4];
    uint8_t     addrsize;
    uint8_t     byteorder;
    uint8_t     version;
    uint8_t     pad[9];
    uint8_t     num_ent[4];
    uint8_t     num_cnt[4];
    uint8_t     str_ent[4];
    uint8_t     str_cnt[4];
    uint8_t     sec_ent[4];
    uint8_t     sec_cnt[4];
    uint8_t     str_base[4];
    uint8_t     sec_base[4];
} panda_efh_t;

int executable_init(executable_t *exe, void *memory, int size,
                    int number_max, int string_max, int func_max,
                    int main_code_max, int func_code_max);
int executable_save(executable_t *exe, void *mem_ptr, int mem_size, int flag);
int executable_load(executable_t *exe, void *mem_prt, int mem_size, void *input, int input_size);

int executable_number_find_add(executable_t *exe, double n);
int executable_string_find_add(executable_t *exe, intptr_t s);

#endif /* __LANG_EXECUTABLE_INC__ */

