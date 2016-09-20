#include "panda.h"

int panda_dump_ef(const char *file)
{
    int err, size;
    executable_file_t ef;
    uint8_t *binary;

    binary = file_load(file, &size);
    if (!binary) {
        printf("file load fail: %s\n", file);
        return -1;
    }

    if (0 == (err = executable_file_load(&ef, binary, size))) {
        int i;
        double *numbers = executable_file_number_entry(&ef);

        //const char *executable_file_string(&ef, )

        printf("================ executable image file ==================\n");
        printf("+ Version  : %d\n", 0);
        printf("+ AddrSize : %d\n", ef.addr_size == ADDRSIZE_32 ? 32 : 64);
        printf("+ ByteOrder: %s\n", ef.byte_order == LE ? "LittleEndian" : "BigEndian");
        printf("+ Number count: %d\n", ef.num_cnt);
        printf("+ String count: %d\n", ef.str_cnt);
        printf("+ Function count: %d\n", ef.fn_cnt);
        printf("-------------------- static numbers ---------------------\n");
        for (i = 0; i < ef.num_cnt; i++) {
            printf("N[%d] %f\n", i, numbers[i]);
        }
        printf("-------------------- static strings ---------------------\n");
        for (i = 0; i < ef.str_cnt; i++) {
            printf("S[%d] %s\n", i, executable_file_get_string(&ef, i));
        }
        printf("----------------------- functions -----------------------\n");
        for (i = 0; i < ef.fn_cnt; i++) {
            const uint8_t *entry = executable_file_get_function(&ef, i);
            const uint8_t *code = executable_func_get_code(entry);
            uint32_t size = executable_func_get_code_size(entry);
            int off = 0;

            printf("\n* Function[%d] %c\n", i, executable_func_is_closure(entry) ? '*' : ' ');
            printf("* variables: %u, arguments: %u, stack_need: %u, code_size: %u\n",
                    executable_func_get_var_cnt(entry), executable_func_get_arg_cnt(entry),
                    executable_func_get_stack_high(entry), size);
            while (off < size) {
                const char *name;
                int p1, p2, pos = off;
                int n = bcode_parse(code, &off, &name, &p1, &p2);
                if (n == 2) {
                    printf("[%4d] %s %d %d\n", pos, name, p1, p2);
                } else if (n == 1) {
                    printf("[%4d] %s %d\n", pos, name, p1);
                } else {
                    printf("[%4d] %s\n", pos, name);
                }
            }
        }
        printf("========================= end ===========================\n");
    } else {
        printf("%s is not panda code file\n", file);
    }

    file_release((void *)binary, size);

    return err;
}

