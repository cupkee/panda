#include "sal/sal.h"
#include "err.h"
#include "executable.h"
#include "function.h"

int executable_init(executable_t *exe, void *mem_ptr, int mem_size,
                    int number_max, int string_max, int func_max,
                    int main_code_max, int func_code_max)
{
    int mem_offset = 0;

    exe->main_code = (uint8_t*) (mem_ptr + mem_offset);
    exe->main_code_end = 0;
    exe->main_code_max = main_code_max;
    mem_offset += exe->main_code_max;

    exe->func_code = (uint8_t*) (mem_ptr + mem_offset);
    exe->func_code_end = 0;
    exe->func_code_max = func_code_max;
    mem_offset += exe->func_code_max;

    mem_offset = SIZE_ALIGN_8(mem_offset);

    // static number buffer init
    exe->number_max = number_max;
    exe->number_num = 0;
    exe->number_map = (double*) (mem_ptr + mem_offset);
    mem_offset += sizeof(double) * number_max;

    // static string buffer init
    exe->string_max = string_max;
    exe->string_num = 0;
    exe->string_map = (intptr_t *) (mem_ptr + mem_offset);
    mem_offset += sizeof(intptr_t) * string_max;

    // script function buffer init
    exe->func_max = func_max;
    exe->func_num = 0;
    exe->func_map = (uint8_t **) (mem_ptr + mem_offset);
    mem_offset += sizeof(uint8_t **) * func_max;

    if (mem_offset > mem_size) {
        return -1;
    } else {
        return mem_offset;
    }
}


static inline
void ef_write(executable_file_t *ef, int offset, void *buf, int size) {
    if (offset + size >= ef->size) {
        ef->error = 1;
    } else {
        memcpy(ef->base + offset, buf, size);
    }
}

static inline
void ef_write_zero(executable_file_t *ef, int offset, int size) {
    if (offset + size >= ef->size) {
        ef->error = 1;
    } else {
        bzero(ef->base + offset, size);
    }
}

static inline
void ef_write_byte(executable_file_t *ef, int offset, uint8_t d) {
    if (offset >= ef->size) {
        ef->error = 1;
    } else {
        ef->base[offset] = d;
    }
}

/*
static inline
void ef_write_uint16(executable_file_t *ef, int offset, uint16_t d) {
    if (offset >= ef->size) {
        ef->error = 1;
    } else {
        if (ef->byte_order == LE) {
            write_uint16_le(ef->base + offset, &d);
        } else {
            write_uint16_be(ef->base + offset, &d);
        }
    }
}

static inline
void ef_write_uint16_be(executable_file_t *ef, int offset, uint16_t d) {
    if (offset >= ef->size) {
        ef->error = 1;
    } else {
        write_uint16_be(ef->base + offset, &d);
    }
}
*/

static inline
void ef_write_uint32(executable_file_t *ef, int offset, uint32_t d) {
    if (offset + 4 >= ef->size) {
        ef->error = 1;
    } else {
        if (ef->byte_order == LE) {
            write_uint32_le(ef->base + offset, &d);
        } else {
            write_uint32_be(ef->base + offset, &d);
        }
    }
}

static inline
void ef_write_double(executable_file_t *ef, int offset, double *d) {
    if (offset + 8 >= ef->size) {
        ef->error = 1;
    } else {
        if (ef->byte_order == LE) {
            write_double_le(ef->base + offset, d);
        } else {
            write_double_be(ef->base + offset, d);
        }
    }
}

static inline
uint8_t ef_read_byte(executable_file_t *ef, int offset) {
    if (offset < ef->size) {
        return ef->base[offset];
    }

    ef->error = 1; return 0;
}

static inline
uint32_t ef_read_uint32(executable_file_t *ef, int offset) {
    if (offset + 4 < ef->size) {
        return ef->byte_order == LE ? read_uint32_le(ef->base + offset) :
               read_uint32_be(ef->base + offset);
    }

    ef->error = 1; return 0;
}

static inline
double ef_read_double(executable_file_t *ef, int offset) {
    if (offset + 8 < ef->size) {
        return ef->byte_order == LE ? read_double_le(ef->base + offset) :
               read_double_be(ef->base + offset);
    }

    ef->error = 1; return 0;
}

int executable_file_init(executable_file_t *ef, void *mem_ptr, int mem_size, int byte_order, int num_cnt, int str_cnt, int fn_cnt)
{
    if (!ef || !mem_ptr || mem_size < 64) {
        return -1;
    }

    ef->error = 0;

    ef->base = mem_ptr;
    ef->size = mem_size;
    ef->byte_order = byte_order;

    ef->num_cnt = num_cnt;
    ef->str_cnt = str_cnt;
    ef->fn_cnt = fn_cnt;

    ef->num_ent = 64;
    ef->str_ent = SIZE_ALIGN_8(ef->num_ent + 8 * num_cnt);
    ef->fn_ent  = SIZE_ALIGN_8(ef->str_ent + 4 * str_cnt);

    ef->end = SIZE_ALIGN_16(ef->fn_ent + 4 * fn_cnt + 16);

    ef_write(ef, 0, "\177ELF", 4);          // magic:
    ef_write_byte(ef, 4, 1);                // addr size:   1:32, 2:64
    ef_write_byte(ef, 5, byte_order);       // byte order: LE:BE
    ef_write_byte(ef, 6, 0);                // version:     0
    ef_write_zero(ef, 7, 9);                // padding
    ef_write_uint32(ef, 16, ef->num_cnt);
    ef_write_uint32(ef, 20, ef->num_ent);
    ef_write_uint32(ef, 24, ef->str_cnt);
    ef_write_uint32(ef, 28, ef->str_ent);
    ef_write_uint32(ef, 32, ef->fn_cnt);
    ef_write_uint32(ef, 36, ef->fn_ent);
    ef_write_zero(ef, 40, 24);

    return 0;
}

int executable_file_load(executable_file_t *ef, uint8_t *input, int size)
{
    if (!ef || !input || size < 64) {
        return -ERR_InvalidInput;
    }

    ef->base = input;
    ef->size = size;

    ef->error = 0;
    ef->addr_size  = ef_read_byte(ef, 4);
    ef->byte_order = ef_read_byte(ef, 5);
    ef->version    = ef_read_byte(ef, 6);

    if (memcmp(input, "\177ELF", 4) || ef->version != 0) {
        return -ERR_InvalidInput;
    }

    ef->num_cnt = ef_read_uint32(ef, 16);
    ef->num_ent = ef_read_uint32(ef, 20);
    ef->str_cnt = ef_read_uint32(ef, 24);
    ef->str_ent = ef_read_uint32(ef, 28);
    ef->fn_cnt  = ef_read_uint32(ef, 32);
    ef->fn_ent  = ef_read_uint32(ef, 36);

    return 0;
}

void executable_file_fill_data(executable_file_t *ef, int nc, double *nv, int sc, intptr_t *sv)
{
    int i, offset;

    if (nc != ef->num_cnt || sc != ef->str_cnt) {
        ef->error = 1;
        return;
    }

    for (i = 0; i < nc; i++) {
        ef_write_double(ef, ef->num_ent + i * 8, nv + i);
    }

    offset = ef->end;
    for (i = 0; !ef->error && i < sc; i++) {
        char *str = (char *)sv[i];
        int len = strlen(str) + 1;

        ef_write_uint32(ef, ef->str_ent + i * 4, offset);

        ef_write(ef, offset, str, len);
        offset += len;
    }

    ef->end = SIZE_ALIGN_16(offset);

    // padding fill with zero
    ef_write_zero(ef, offset, ef->end - offset);
}

void executable_file_fill_code(executable_file_t *ef, int entry, uint8_t vc, uint8_t ac, uint16_t stack_need, int closure, uint8_t *code, int size)
{
    int offset;

    if (ef->error || entry >= ef->fn_cnt) {
        ef->error = 1;
        return;
    }

    offset = ef->end;
    ef->end = SIZE_ALIGN_8(offset + FUNC_HEAD_SIZE + size);

    ef_write_uint32(ef, ef->fn_ent + entry * 4, offset);

    executable_func_set_head(ef->base + offset, vc, ac, size, stack_need, closure);
    offset += FUNC_HEAD_SIZE;
    ef_write(ef, offset, code, size);

    // padding fill with zero
    ef_write_zero(ef, offset + size, ef->end - offset - size);
}

double *executable_file_number_entry(executable_file_t *ef)
{
    if (!ef || ef->error) {
        return NULL;
    }

    return (double *)(ef->base + ef->num_ent);
}

double executable_file_get_number(executable_file_t *ef, int index)
{
    if (!ef || ef->error) {
        return 0;
    }

    return ef_read_double(ef, ef->num_ent + index * 8);
}

const char *executable_file_get_string(executable_file_t *ef, int index)
{
    uint32_t offset;

    if (!ef || ef->error) {
        return NULL;
    }

    offset = ef_read_uint32(ef, ef->str_ent + index * 4);

    return (const char*)(ef->base + offset);
}

const uint8_t *executable_file_get_function(executable_file_t *ef, int index)
{
    uint32_t offset;

    if (!ef || ef->error) {
        return NULL;
    }

    offset = ef_read_uint32(ef, ef->fn_ent + index * 4);

    return (const uint8_t*)(ef->base + offset);
}

int executable_number_find_add(executable_t *exe, double n)
{
    int i;

    for (i = 0; i < exe->number_num; i++) {
        if (exe->number_map[i] == n) {
            return i;
        }
    }

    if (exe->number_num < exe->number_max) {
        exe->number_map[exe->number_num++] = n;
        return i;
    } else {
        return -1;
    }
}

int executable_string_find_add(executable_t *exe, intptr_t s)
{
    int i;

    if (s == 0) {
        return -1;
    }

    for (i = 0; i < exe->string_num; i++) {
        if (exe->string_map[i] == s) {
            return i;
        }
    }

    if (exe->string_num < exe->string_max) {
        exe->string_map[exe->string_num++] = s;
        return i;
    } else {
        return -1;
    }
}
