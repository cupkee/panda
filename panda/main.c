#include <readline/readline.h>
#include <readline/history.h>

#include "lang/interp.h"

#define HEAP_SIZE     (1024 * 480)
#define STACK_SIZE    (1024)
#define MEM_SIZE      (STACK_SIZE * sizeof(val_t) + HEAP_SIZE + EXE_MEM_SPACE + SYMBAL_MEM_SPACE)

void *memory[MEM_SIZE];

static void print_value(val_t *v)
{
    if (val_is_number(v)) {
        printf("%f\n", val_2_double(v));
    } else
    if (val_is_boolean(v)) {
        printf("%s\n", val_2_intptr(v) ? "true" : "false");
    } else
    if (val_is_string(v)) {
        printf("'%s'\n", val_2_cstring(v));
    } else
    if (val_is_undefined(v)) {
        printf("undefined\n");
    } else
    if (val_is_nan(v)) {
        printf("NaN\n");
    } else {
        printf("[object]");
    }

}

int main(int ac, char **av)
{
    env_t env_st, *env = &env_st;
    val_t *res;
    char *line;

    if(0 != interp_env_init_interactive(&env_st, memory, MEM_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE)) {
        printf("env_init fail\n");
        return -1;
    }

    printf("LEO V0.1\n\n");

    while ((line = readline("> ")) != NULL) {
        int err = interp_execute_string(env, line, &res);

        if (err < 0) {
            printf("Fail: %d\n", err);
        } else
        if (err > 0) {
            print_value(res);
            add_history(line);
        }

        free(line);
    }

    return 0;
}
