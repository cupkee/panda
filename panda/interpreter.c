#include "panda.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

static char *file_load(const char *name, int *size)
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
    length = sb.st_size + 1;

    addr = mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, 0);
    *size = length;

    return addr;
}

static void file_release(void *addr, int size)
{
    munmap(addr, size);
}

int binary_panda(const char *input, void *mem_ptr, int mem_size, int heap_size, int stack_size)
{
    return 0;
}

int string_panda(const char *input, void *mem_ptr, int mem_size, int heap_size, int stack_size)
{
    env_t env;
    val_t *res;
    int err, size;

    input = file_load(input, &size);
    if (!input) {
        return -1;
    }

    if(0 != interp_env_init_interpreter(&env, mem_ptr, mem_size, NULL, heap_size, NULL, stack_size)) {
        return -1;
    }
    panda_native_init(&env);

    err = interp_execute_string(&env, input, &res);

    file_release((void *)input, size);

    if (err < 0) {
        printf("error: %d\n", err);
        return -1;
    } else {
        return 0;
    }
}

