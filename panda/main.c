
#include <unistd.h>
#include "panda.h"

#define HEAP_SIZE     (1024 * 480)
#define STACK_SIZE    (1024)
#define MEM_SIZE      (STACK_SIZE * sizeof(val_t) + HEAP_SIZE + EXE_MEM_SPACE + SYMBAL_MEM_SPACE)

char *MEM_PTR[MEM_SIZE];

static inline int interactive(void) {
    return panda_interactive(MEM_PTR, MEM_SIZE, HEAP_SIZE, STACK_SIZE);
}

static inline int interpreter(const char *input, int ac, char **av) {
    char *suffix;

    suffix = rindex(input, '.');
    if (suffix && !strcmp(suffix, ".pdc")) {
        return panda_binary(input, MEM_PTR, MEM_SIZE, HEAP_SIZE, STACK_SIZE);
    } else {
        return panda_string(input, MEM_PTR, MEM_SIZE, HEAP_SIZE, STACK_SIZE);
    }
}

int main(int ac, char **av)
{
    int ch;
    int compile_only = 0;
    int dump_binary = 0;
    char *input;
    int   error;

    if (ac == 1) {
        return interactive();
    }

    while(-1 != (ch = getopt(ac, av, "cd"))) {
        switch(ch) {
        case 'c': compile_only = 1; break;
        case 'd': dump_binary = 1; break;
        default: break;
        }
    }

    ac -= optind;
    av += optind;
    if (ac == 0) {
        printf("error: input\n");
        return -1;
    }

    input = av[0];

    if (compile_only) {
        error = panda_compile(input, MEM_PTR, MEM_SIZE);
    } else
    if (dump_binary){
        error = panda_dump_ef(input);
    } else {
        error = interpreter(input, ac - 1, av + 1);
    }

    if (error < 0) {
        printf("execute %s fail:%d\n", input, error);
    }

    return error ? 1 : 0;
}

