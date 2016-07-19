


#ifndef __LANG_EVAL_INC__
#define __LANG_EVAL_INC__

#include "config.h"

#include "ast.h"
#include "val.h"
#include "interp.h"

int eval_string(interp_t *interp, const char *input, val_t **v);
val_t eval_expr_ast(interp_t *interp, expr_t *expr);

#endif /* __LANG_EVAL_INC__ */

