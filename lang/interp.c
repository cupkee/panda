

#include "val.h"
#include "lex.h"
#include "parse.h"
#include "interp.h"

#include "symtbl.h"

interp_t *interp_init(interp_t *interp, val_t *stack_ptr, int stack_size)
{
    if (interp) {
        interp->sb = stack_ptr;
        interp->ss = stack_size;
        interp->sp = stack_size;

        interp->error = 0;

        // env init
        interp->var_num = 0;
        int i;
        for (i = 0; i < 16; i++) {
            interp->regs[i] = val_mk_undefined();
        }

        interp->sym_tbl = symtbl_create();
    }
    return interp;
}

int interp_deinit(interp_t *interp)
{
    if (interp) {
        symtbl_destroy(interp->sym_tbl);
    }
    return 0;
}

int interp_add_symbal(interp_t *interp, const char *sym)
{
    intptr_t id;

    if (interp->var_num >= 16) {
        return -1;
    }

    id = symtbl_add(interp->sym_tbl, sym);
    if (id) {
        interp->var_map[interp->var_num] = id;
        return interp->var_num++;
    }

    return -1;
}

int interp_get_symbal(interp_t *interp, const char *sym)
{
    intptr_t id;

    id = symtbl_get(interp->sym_tbl, sym);
    if (id) {
        int i;
        for (i = 0; i < interp->var_num; i++) {
            if (interp->var_map[i] == id) {
                return i;
            }
        }
    }

    return -1;
}

int interp_set_symbal_val(interp_t *interp, const char *sym, val_t v)
{
    int idx;

    if (0 > (idx = interp_get_symbal(interp, sym))) {
        return -1;
    }
    interp->regs[idx] = v;

    return 0;
}

int interp_get_symbal_val(interp_t *interp, const char *sym, val_t *p)
{
    int idx;

    if (0 > (idx = interp_get_symbal(interp, sym))) {
        return -1;
    }
    *p = interp->regs[idx];

    return 0;
}

