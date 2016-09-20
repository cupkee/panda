#include "panda.h"

int panda_binary(const char *input, void *mem_ptr, int mem_size, int heap_size, int stack_size)
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
    panda_native_init(&env);

    err = interp_execute_image(&env, &res);
    if (err < 0) {
        printf("error: %d\n", err);
    }

    file_release((void *)input, size);

    return err;
}

int panda_string(const char *input, void *mem_ptr, int mem_size, int heap_size, int stack_size)
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
    panda_native_init(&env);

    err = interp_execute_string(&env, input, &res);
    if (err < 0) {
        printf("error: %d\n", err);
    }

    file_release((void *)input, size);

    return err;
}

