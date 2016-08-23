#include "panda.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

static char *input_load(const char *name)
{

    char *addr;
    int fd;
    struct stat sb;
    size_t length;

    fd = open(name, O_RDONLY);
    if (fd < 0) {
        return NULL;
    }

    if (fstat(fd, &sb) == -1) {
        close(fd);
        return NULL;
    }
    length = sb.st_size;

    addr = mmap(NULL, length + 1, PROT_READ, MAP_PRIVATE, fd, 0);

    return addr;
}

int binary_panda(const char *input, void *mem_ptr, int mem_size, int heap_size, int stack_size)
{
    return 0;
}

int string_panda(const char *input, void *mem_ptr, int mem_size, int heap_size, int stack_size)
{
    env_t env;
    val_t *res;
    int err;

    input = input_load(input);
    if (!input) {
        return -1;
    }
    //printf("input:\n%s\n", input);

    if(0 != interp_env_init_interpreter(&env, mem_ptr, mem_size, NULL, heap_size, NULL, stack_size)) {
        return -1;
    }

    panda_native_init(&env);

    err = interp_execute_string(&env, input, &res);
    if (err < 0) {
        printf("error: %d\n", err);
        //print_error(-err);
        return -1;
    } else {
        return 0;
    }
}

