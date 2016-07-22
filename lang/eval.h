


#ifndef __LANG_EVAL_INC__
#define __LANG_EVAL_INC__

#include "config.h"

#include "ast.h"
#include "val.h"
#include "env.h"
#include "interp.h"

int eval_string(interp_t *interp, env_t *env, const char *input, val_t **result);

#endif /* __LANG_EVAL_INC__ */

