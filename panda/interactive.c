#include <readline/readline.h>
#include <readline/history.h>

#include "panda.h"

static int output(const char *s)
{
    return printf("%s", s);
}

static char *input_last = NULL;

static inline void input_release(char *last)
{
    if (input_last) {
        free(input_last);
    }
    input_last = last;
}

static char *input_read(void)
{
    char *input;

    input = readline("> ");
    if (input && strlen(input)) {
        add_history(input);
    }

    input_release(input);

    return input;
}

static char *input_more(void)
{
    char *input;

    input = readline(". ");
    if (strlen(input)) {
        add_history(input);
    }

    input_release(input);

    return input;
}

static void print_error(int error)
{
    switch (error) {
    case ERR_SysError:          output("Error: System error\n"); break;

    case ERR_NotEnoughMemory:   output("Error: Not enought memory\n"); break;
    case ERR_NotImplemented:    output("Error: Not implemented\n"); break;
    case ERR_StaticNumberOverrun: output("Error: ..\n"); break;
    case ERR_StackOverflow:     output("Error: Stack overflow\n"); break;
    case ERR_ResourceOutLimit:  output("Error: Resource out of limited\n"); break;

    case ERR_InvalidToken:      output("Error: Invalid Token\n"); break;
    case ERR_InvalidSyntax:     output("Error: Invalid syntax\n"); break;
    case ERR_InvalidLeftValue:  output("Error: Invalid left value\n"); break;
    case ERR_InvalidSementic:   output("Error: Invalid Sementic\n"); break;

    case ERR_InvalidByteCode:   output("Error: Invalid Byte code\n"); break;
    case ERR_InvalidInput:      output("Error: Invalid input\n"); break;
    case ERR_InvalidCallor:     output("Error: Invalid callor\n"); break;
    case ERR_NotDefinedId:      output("Error: Not defined id\n"); break;
    case ERR_NotDefinedProp:    output("Error: Not defined proprity\n"); break;
    case ERR_HasNoneElement:    output("Error: Not defined element\n"); break;
    default: output("Error: unknown error\n");
    }
}

static void print_value(val_t *v)
{
    if (val_is_number(v)) {
        char buf[32];
        snprintf(buf, 32, "%f\n", val_2_double(v));
        output(buf);
    } else
    if (val_is_boolean(v)) {
        output(val_2_intptr(v) ? "true\n" : "false\n");
    } else
    if (val_is_string(v)) {
        output("\"");
        output(val_2_cstring(v));
        output("\"\n");
    } else
    if (val_is_undefined(v)) {
        output("undefined\n");
    } else
    if (val_is_nan(v)) {
        output("NaN\n");
    } else
    if (val_is_function(v)) {
        output("function\n");
    } else {
        output("object\n");
    }
}

int panda_interactive(void *mem_ptr, int mem_size, int heap_size, int stack_size)
{
    env_t env;

    if(0 != interp_env_init_interactive(&env, mem_ptr, mem_size, NULL, heap_size, NULL, stack_size)) {
        printf("env_init fail\n");
        return -1;
    }

    panda_native_init(&env);

    printf("PANDA V0.1\n\n");

    while (1) {
        int    err;
        val_t *res;
        char  *line = input_read();

        if (!line) {
            return 0;
        }

        err = interp_execute_interactive(&env, line, input_more, &res);
        if (err < 0) {
            print_error(-err);
        } else
        if (err > 0) {
            print_value(res);
        }
    }
}

