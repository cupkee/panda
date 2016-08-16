#include "executable.h"

int executable_init(executable_t *exe, void *mem_ptr, int mem_size,
                    int number_max, int string_max, int native_max, int func_max,
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

    // native function buffer init
    exe->native_max = native_max;
    exe->native_num = 0;
    exe->native_map = (intptr_t *) (mem_ptr + mem_offset);
    mem_offset += sizeof(intptr_t) * native_max;
    exe->native_entry = (intptr_t *) (mem_ptr + mem_offset);
    mem_offset += sizeof(intptr_t) * native_max;

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

int executable_native_add(executable_t *exe, intptr_t sym_id, intptr_t entry)
{
    int i;

    for (i = 0; i < exe->native_num; i++) {
        if (sym_id == exe->native_map[i]) {
            // already exist!
            return 0;
        }
    }

    if (i >= exe->native_max) {
        return -1;
    }

    exe->native_map[i] = sym_id;
    exe->native_entry[i] = entry;

    return exe->native_num++;
}

int executable_native_find(executable_t *exe, intptr_t sym_id)
{
    int i;

    for (i = 0; i < exe->native_num; i++) {
        if (sym_id == exe->native_map[i]) {
            return i;
        }
    }
    return -1;
}
