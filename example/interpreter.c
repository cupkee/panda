#include "example.h"

#define HEAP_SIZE     (1024 * 480)
#define STACK_SIZE    (1024)
#define MEM_SIZE      (STACK_SIZE * sizeof(val_t) + HEAP_SIZE + EXE_MEM_SPACE + SYMBAL_MEM_SPACE)

static uint8_t memory[MEM_SIZE];

static int panda_binary(const char *input, void *mem_ptr, int mem_size, int heap_size, int stack_size)
{
    env_t env;
    val_t *res;
    int err, size;
    uint8_t *binary;
    image_info_t ef;

    binary = file_load(input, &size);
    if (!binary) {
        return -1;
    }

    if (0 != image_load(&ef, binary, size)) {
        file_release((void *)input, size);
        return -1;
    }

    if (0 != interp_env_init_image (&env, mem_ptr, mem_size, NULL, heap_size, NULL, stack_size, &ef)) {
        file_release((void *)input, size);
        return -1;
    }
    native_init(&env);

    err = interp_execute_image(&env, &res);
    if (err < 0) {
        printf("error: %d\n", err);
    }

    file_release((void *)input, size);

    return err;
}

static int panda_string(const char *input, void *mem_ptr, int mem_size, int heap_size, int stack_size)
{
    env_t env;
    val_t *res;
    int err, size;

    input = file_load(input, &size);
    if (!input) {
        return -1;
    }

    if(0 != interp_env_init_interpreter(&env, mem_ptr, mem_size, NULL, heap_size, NULL, stack_size)) {
        file_release((void *)input, size);
        return -1;
    }
    native_init(&env);

    err = interp_execute_string(&env, input, &res);
    if (err < 0) {
        printf("error: %d\n", err);
    }

    file_release((void *)input, size);

    return err;
}

static inline int interpreter(const char *input, int ac, char **av) {
    char *suffix;

    suffix = rindex(input, '.');
    if (suffix && !strcmp(suffix, ".pdc")) {
        return panda_binary(input, memory, MEM_SIZE, HEAP_SIZE, STACK_SIZE);
    } else {
        return panda_string(input, memory, MEM_SIZE, HEAP_SIZE, STACK_SIZE);
    }
}

int main(int ac, char **av)
{
    char *input;
    int   error;

    if (ac == 1) {
        printf("Usage: %s <input>\n", av[0]);
        return 0;
    }
    input = av[1];

    error = interpreter(input, ac - 1, av + 1);
    if (error < 0) {
        printf("execute %s fail:%d\n", input, error);
    }

    return error ? 1 : 0;
}

