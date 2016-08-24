
#include "panda.h"

#define HEAP_SIZE     (1024 * 480)
#define STACK_SIZE    (1024)
#define MEM_SIZE      (STACK_SIZE * sizeof(val_t) + HEAP_SIZE + EXE_MEM_SPACE + SYMBAL_MEM_SPACE)

char *MEM_PTR[MEM_SIZE];

int main(int ac, char **av)
{
    int i;

    if (ac == 1) {
        return panda_interactive(MEM_PTR, MEM_SIZE, HEAP_SIZE, STACK_SIZE);
    }

    for (i = 1; i < ac; i++) {
        char *input = av[i];
        char *suffix = rindex(input, '.');
        int   error;

        if (suffix && !strcmp(suffix, ".pdc")) {
            error = panda_binary(input, MEM_PTR, MEM_SIZE, HEAP_SIZE, STACK_SIZE);
        } else {
            error = panda_string(input, MEM_PTR, MEM_SIZE, HEAP_SIZE, STACK_SIZE);
        }

        if (error) {
            break;
        }
    }
}

