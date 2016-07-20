

#include "val.h"
#include "lex.h"
#include "parse.h"
#include "interp.h"

interp_t *interp_init(interp_t *interp, val_t *stack_ptr, int stack_size)
{
    if (interp) {
        interp->sb = stack_ptr;
        interp->ss = stack_size;
        interp->sp = stack_size;

        interp->error = 0;
    }
    return interp;
}

int interp_deinit(interp_t *interp)
{
    return 0;
}

int interp_add_symbal(interp_t *interp, const char *sym)
{
    return -1;
}

int interp_get_symbal(interp_t *interp, const char *sym)
{
    return -1;
}
