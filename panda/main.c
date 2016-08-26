
#include <unistd.h>
#include "panda.h"

#define HEAP_SIZE     (1024 * 480)
#define STACK_SIZE    (1024)
#define MEM_SIZE      (STACK_SIZE * sizeof(val_t) + HEAP_SIZE + EXE_MEM_SPACE + SYMBAL_MEM_SPACE)

char *MEM_PTR[MEM_SIZE];

int main(int ac, char **av)
{
    int i, ch;
    int compile_only = 0;
    int dump_binary = 0;

    if (ac == 1) {
        return panda_interactive(MEM_PTR, MEM_SIZE, HEAP_SIZE, STACK_SIZE);
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

    for (i = 0; i < ac; i++) {
        char *input = av[i];
        char *suffix = rindex(input, '.');
        int   error;

        if (compile_only) {
            error = panda_compile(input, MEM_PTR, MEM_SIZE);
        } else
        if (dump_binary){
            error = panda_dump_ef(input);
        } else {
            if (suffix && !strcmp(suffix, ".pdc")) {
                error = panda_binary(input, MEM_PTR, MEM_SIZE, HEAP_SIZE, STACK_SIZE);
            } else {
                error = panda_string(input, MEM_PTR, MEM_SIZE, HEAP_SIZE, STACK_SIZE);
            }
        }

        if (error < 0) {
            if (!dump_binary)
                printf("%s: %s fail:%d\n", compile_only ? "compile" : "execute", input, error);
            break;
        }
    }
}

