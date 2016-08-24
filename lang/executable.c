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

#if SYS_BYTE_ORDER == LE
//Note: only for LittleEndian host system
void write_double_le(void *addr, double *d)
{
    *((double *)addr) = *d;
}

void write_double_be(void *addr, double *d)
{
    uint8_t *a = (uint8_t *)addr;
    uint8_t *b = (uint8_t *)d;

    a[0] = b[7]; a[1] = b[6]; a[2] = b[5]; a[3] = b[4];
    a[4] = b[3]; a[5] = b[2]; a[6] = b[1]; a[7] = b[0];
}

void write_uint32_le(void *addr, uint32_t *d)
{
    *((uint32_t *)addr) = *d;
}

void write_uint32_be(void *addr, uint32_t *d)
{
    uint8_t *a = (uint8_t *)addr;
    uint8_t *b = (uint8_t *)d;

    a[0] = b[3]; a[1] = b[2]; a[2] = b[1]; a[3] = b[0];
}

uint32_t read_uint32_le(void *addr)
{
    return *((uint32_t *)addr);
}

uint32_t read_uint32_be(void *addr)
{
    uint8_t *p = (uint8_t *)addr;

    return p[0] * 0x1000000 + p[1] * 0x10000 + p[2] * 0x100 + p[3];
}
#endif

static inline void efh_set_double(panda_efh_t *efh, void *addr, double d) {

    if (efh->byteorder == BE) {
        write_double_be(addr, &d);
    } else {
        write_double_le(addr, &d);
    }
}

static inline void efh_set_uint32(panda_efh_t *efh, void *addr, uint32_t d) {
    if (efh->byteorder == BE) {
        write_uint32_be(addr, &d);
    } else {
        write_uint32_le(addr, &d);
    }
}

static inline uint32_t efh_get_uint32(panda_efh_t *efh, void *addr) {
    if (efh->byteorder == BE) {
        return read_uint32_be(addr);
    } else {
        return read_uint32_le(addr);
    }
}

int executable_save(executable_t *exe, void *mem_ptr, int mem_size, int flag)
{
    panda_efh_t *efh = (panda_efh_t *)mem_ptr;
    int num_ent, str_ent, sec_ent, str_base, sec_base;
    int offset  = SIZE_ALIGN_8(sizeof(panda_efh_t));
    int i;

    efh->magic[0] = '\177';
    efh->magic[1] = 'E';
    efh->magic[2] = 'L';
    efh->magic[3] = 'F';

    if (flag & EXEC_FL_64) {
        efh->addrsize = ADDRSIZE_64;
    } else {
        efh->addrsize = ADDRSIZE_32;
    }

    if (flag & EXEC_FL_BE) {
        efh->byteorder = BE;
    } else {
        efh->byteorder = LE;
    }

    efh->version = 0;

    num_ent = offset;
    efh_set_uint32(efh, efh->num_ent, num_ent);
    efh_set_uint32(efh, efh->num_cnt, exe->number_num);
    offset = SIZE_ALIGN_8(offset + sizeof(double) * exe->number_num);
    if (offset >= mem_size) return -1;

    for (i = 0; i < exe->number_num; i++) {
        efh_set_double(efh, mem_ptr + num_ent, exe->number_map[i]);
        num_ent += sizeof(double);
    }

    str_ent = offset;
    efh_set_uint32(efh, efh->str_ent, str_ent);
    efh_set_uint32(efh, efh->str_cnt, exe->string_num);
    offset = SIZE_ALIGN_8(offset + 4 * exe->string_num);
    if (offset >= mem_size) return -1;

    sec_ent = offset;
    efh_set_uint32(efh, efh->sec_ent, sec_ent);
    efh_set_uint32(efh, efh->sec_cnt, exe->func_num);
    offset = SIZE_ALIGN_8(offset + 4 * exe->string_num);
    if (offset >= mem_size) return -1;

    str_base = offset;
    efh_set_uint32(efh, efh->str_base, str_base);
    for (i = 0; i < exe->string_num; i++) {
        char *str = (char *)exe->string_map[i];
        int len = strlen(str) + 1;

        if (offset + len >= mem_size) return -1;

        efh_set_uint32(efh, mem_ptr + str_ent, offset);
        str_ent += 4;

        memcpy(mem_ptr + offset, str, len);
        offset += len;
    }

    offset = SIZE_ALIGN_8(offset);
    sec_base = offset;
    efh_set_uint32(efh, efh->sec_base, sec_base);
    for (i = 0; i < exe->func_num; i++) {
        function_info_t fi;
        uint8_t *entry = exe->func_map[i];
        int len;

        function_info_read(entry, &fi);
        len = fi.size + FUNC_HEAD_SIZE;
        if (offset + len >= mem_size) return -1;

        efh_set_uint32(efh, mem_ptr + sec_ent, offset);
        sec_ent += 4;

        memcpy(mem_ptr + offset, entry, len);
        offset = SIZE_ALIGN_8(offset + len);
    }

    return offset;
}

int executable_load(executable_t *exe, void *mem_ptr, int mem_size, void *input, int input_size)
{
    panda_efh_t *efh = (panda_efh_t *) input;
    int mem_offset = 0, i;
    uint32_t num_ent, num_cnt;
    uint32_t str_ent, str_cnt;
    uint32_t func_ent, func_cnt;
    uint32_t str_base, func_base;

    if (input_size < sizeof(panda_efh_t) || efh->version != 0) {
        return -1;
    }

    if (efh->magic[0] != '\177' || efh->magic[1] != 'E' || efh->magic[2] != 'L' || efh->magic[3] != 'F') {
        return -1;
    }

    if (efh->byteorder != SYS_BYTE_ORDER) {
        return -1;
    }

    num_ent = efh_get_uint32(efh, efh->num_ent);
    num_cnt = efh_get_uint32(efh, efh->num_cnt);
    str_ent = efh_get_uint32(efh, efh->str_ent);
    str_cnt = efh_get_uint32(efh, efh->str_cnt);
    func_ent = efh_get_uint32(efh, efh->sec_ent);
    func_cnt = efh_get_uint32(efh, efh->sec_cnt);

    str_base = efh_get_uint32(efh, efh->str_base);
    func_base = efh_get_uint32(efh, efh->sec_base);

    exe->main_code = NULL;
    exe->main_code_end = 0;
    exe->main_code_max = 0;

    exe->func_code = NULL;
    exe->func_code_end = 0;
    exe->func_code_max = 0;


    // static number buffer init
    exe->number_max = num_cnt;
    exe->number_num = num_cnt;
    exe->number_map = (input + num_ent);

    mem_offset = SIZE_ALIGN_8(mem_offset);
    // static string buffer init
    exe->string_max = str_cnt;
    exe->string_num = str_cnt;
    exe->string_map = (intptr_t *) (mem_ptr + mem_offset);
    mem_offset += sizeof(intptr_t) * str_cnt;
    if (mem_offset > mem_size) return -1;
    for (i = 0; i < str_ent; i++) {
        uint32_t str_offset = efh_get_uint32(efh, input + str_ent + i * 4);
        exe->string_map[i] = (intptr_t) (input + str_base + str_offset);
    }

    // script function buffer init
    exe->func_max = func_cnt;
    exe->func_num = func_cnt;
    exe->func_map = (uint8_t **) (mem_ptr + mem_offset);
    mem_offset += sizeof(uint8_t **) * func_cnt;
    if (mem_offset > mem_size) return -1;
    for (i = 0; i < func_ent; i++) {
        uint32_t func_offset = efh_get_uint32(efh, input + func_ent + i * 4);
        exe->func_map[i] = (uint8_t *) (input + func_base + func_offset);
    }

    return mem_offset;
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
