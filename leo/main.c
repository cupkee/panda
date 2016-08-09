#include <readline/readline.h>
#include <readline/history.h>

#include "lang/eval.h"

#define MEM_SIZE    (1024 * 512)
#define HEAP_SIZE    (1024 * 496)
#define STACK_SIZE    (1024)

void *MEM_PTR[MEM_SIZE];

char eval_buf[128];

int main(int ac, char **av)
{
    eval_env_t env_st, *env = &env_st;
    val_t *res;
    char *line;

    if(0 != eval_env_init(&env_st, MEM_PTR, MEM_SIZE, NULL, HEAP_SIZE, NULL, STACK_SIZE)) {
        printf("eval_env_init fail\n");
        return -1;
    }

    printf("LEO V0.1\n\n");

    while ((line = readline("> ")) != NULL) {
        if (0 != eval_string(env, eval_buf, 128, line, &res)) {
            printf("eval fail\n");
            break;
        }
        if (val_is_number(res)) {
            printf("%f\n", val_2_double(res));
        } else
        if (val_is_boolean(res)) {
            printf("%s\n", val_2_intptr(res) ? "true" : "false");
        } else
        if (val_is_string(res)) {
            printf("%s\n", val_2_cstring(res));
        } else {
            printf("wawa\n");
        }
    }

    return 0;
}
