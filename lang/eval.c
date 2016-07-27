
#include "err.h"
#include "val.h"
#include "lex.h"
#include "ast.h"
#include "parse.h"
#include "interp.h"
#include "compile.h"

#include "eval.h"

static const char *string_ptr;
static int string_pos;
static int string_end;

static void get_line_init(const char *string)
{
    string_ptr = string;
    string_end = strlen(string);
    string_pos = 0;
}

static int get_line_from_string(void *buf, int size)
{
    int lft = string_end - string_pos;
    int max = lft < size ? lft : size;
    const char *bgn = string_ptr + string_pos;

    if (max > 0)
        memcpy(buf, bgn, max);
    string_pos += max;

    return max;
}


static intptr_t eval_funcdef(interp_t *interp, env_t *env, expr_t *e)
{
    return 0;
}

static void eval_expr_lft(interp_t *interp, env_t *env, expr_t *e)
{
    if (interp->error) {
        return;
    }

    switch(e->type) {
    case EXPR_ID:
    {
        val_t *p;

        if (-1 == env_get_variable(env, ast_expr_text(e), &p)) {
            interp_set_error(interp, ERR_NotDefinedId);
        } else {
            interp_push_ref(interp, p);
        }
        break;
    }
    case EXPR_ELEM: interp_set_error(interp, ERR_NotImplemented); break;
    case EXPR_ATTR: interp_set_error(interp, ERR_NotImplemented); break;
    default: interp_set_error(interp, ERR_InvalidSyntax); break;
    }
}

static void eval_expr(interp_t *interp, env_t *env, expr_t *e)
{
    if (interp->error) {
        return;
    }

    switch (e->type) {
    case EXPR_ID:
    {
        val_t *v;
        if (-1 == env_get_variable(env, ast_expr_text(e), &v)) {
            interp_set_error(interp, ERR_NotDefinedId);
        } else {
            interp_push_val(interp, *v);
        }
        break;
    }
    case EXPR_NAN:      interp_push_nan(interp); break;
    case EXPR_UND:      interp_push_undefined(interp); break;
    case EXPR_NUM:      interp_push_number(interp, ast_expr_num(e)); break;
    case EXPR_TRUE:     interp_push_boolean(interp, 1); break;
    case EXPR_FALSE:    interp_push_boolean(interp, 0); break;
    case EXPR_FUNCDEF:  interp_push_script(interp, eval_funcdef(interp, env, e)); break;
    case EXPR_STRING:   interp_set_error(interp, ERR_NotImplemented); break;

    case EXPR_NEG:      eval_expr(interp, env, ast_expr_lft(e)); interp_neg_stack(interp); break;
    case EXPR_NOT:      eval_expr(interp, env, ast_expr_lft(e)); interp_not_stack(interp); break;
    case EXPR_LOGIC_NOT:eval_expr(interp, env, ast_expr_lft(e)); interp_logic_not_stack(interp); break;
    case EXPR_ARRAY:    interp_set_error(interp, ERR_NotImplemented); break;
    case EXPR_DICT:     interp_set_error(interp, ERR_NotImplemented); break;

    case EXPR_MUL:      eval_expr(interp, env, ast_expr_lft(e)); eval_expr(interp, env, ast_expr_rht(e)); interp_mul_stack(interp); break;
    case EXPR_DIV:      eval_expr(interp, env, ast_expr_lft(e)); eval_expr(interp, env, ast_expr_rht(e)); interp_div_stack(interp); break;
    case EXPR_MOD:      eval_expr(interp, env, ast_expr_lft(e)); eval_expr(interp, env, ast_expr_rht(e)); interp_mod_stack(interp); break;
    case EXPR_ADD:      eval_expr(interp, env, ast_expr_lft(e)); eval_expr(interp, env, ast_expr_rht(e)); interp_add_stack(interp); break;
    case EXPR_SUB:      eval_expr(interp, env, ast_expr_lft(e)); eval_expr(interp, env, ast_expr_rht(e)); interp_sub_stack(interp); break;

    case EXPR_AND:      eval_expr(interp, env, ast_expr_lft(e)); eval_expr(interp, env, ast_expr_rht(e)); interp_and_stack(interp); break;
    case EXPR_OR:       eval_expr(interp, env, ast_expr_lft(e)); eval_expr(interp, env, ast_expr_rht(e)); interp_or_stack(interp); break;
    case EXPR_XOR:      eval_expr(interp, env, ast_expr_lft(e)); eval_expr(interp, env, ast_expr_rht(e)); interp_xor_stack(interp); break;

    case EXPR_LSHIFT:   eval_expr(interp, env, ast_expr_lft(e)); eval_expr(interp, env, ast_expr_rht(e)); interp_lshift_stack(interp); break;
    case EXPR_RSHIFT:   eval_expr(interp, env, ast_expr_lft(e)); eval_expr(interp, env, ast_expr_rht(e)); interp_rshift_stack(interp); break;

    case EXPR_TEQ:      eval_expr(interp, env, ast_expr_lft(e)); eval_expr(interp, env, ast_expr_rht(e)); interp_teq_stack(interp); break;
    case EXPR_TNE:      eval_expr(interp, env, ast_expr_lft(e)); eval_expr(interp, env, ast_expr_rht(e)); interp_tne_stack(interp); break;
    case EXPR_TGT:      eval_expr(interp, env, ast_expr_lft(e)); eval_expr(interp, env, ast_expr_rht(e)); interp_tgt_stack(interp); break;
    case EXPR_TGE:      eval_expr(interp, env, ast_expr_lft(e)); eval_expr(interp, env, ast_expr_rht(e)); interp_tge_stack(interp); break;
    case EXPR_TLT:      eval_expr(interp, env, ast_expr_lft(e)); eval_expr(interp, env, ast_expr_rht(e)); interp_tlt_stack(interp); break;
    case EXPR_TLE:      eval_expr(interp, env, ast_expr_lft(e)); eval_expr(interp, env, ast_expr_rht(e)); interp_tle_stack(interp); break;
    case EXPR_TIN:      interp_set_error(interp, ERR_NotImplemented); break;

    case EXPR_LOGIC_AND:eval_expr(interp, env, ast_expr_lft(e));
                        if (val_is_true(*interp_stack_peek(interp))) {
                            interp_stack_pop(interp);
                            eval_expr(interp, env, ast_expr_rht(e));
                        }
                        break;
    case EXPR_LOGIC_OR: eval_expr(interp, env, ast_expr_lft(e));
                        if (!val_is_true(*interp_stack_peek(interp))) {
                            interp_stack_pop(interp);
                            eval_expr(interp, env, ast_expr_rht(e));
                        }
                        break;

    case EXPR_CALL:     interp_set_error(interp, ERR_NotImplemented); break;
    case EXPR_ELEM:     interp_set_error(interp, ERR_NotImplemented); break;
    case EXPR_ATTR:     interp_set_error(interp, ERR_NotImplemented); break;

    case EXPR_ASSIGN:   eval_expr_lft(interp, env, ast_expr_lft(e)); eval_expr(interp, env, ast_expr_rht(e)); interp_assign(interp); break;

    case EXPR_COMMA:    eval_expr(interp, env, ast_expr_lft(e)); interp_stack_pop(interp); eval_expr(interp, env, ast_expr_rht(e)); break;
    case EXPR_TERNARY:  eval_expr(interp, env, ast_expr_lft(e));
                        if (val_is_true(*interp_stack_pop(interp))) {
                            eval_expr(interp, env, ast_expr_lft(ast_expr_rht(e)));
                        } else {
                            eval_expr(interp, env, ast_expr_rht(ast_expr_rht(e)));
                        }
                        break;
    default:            interp_set_error(interp, ERR_InvalidSyntax); break;
    }
}

static inline void eval_stmt_expr(interp_t *interp, env_t *env, stmt_t *s) {
    eval_expr(interp, env, s->expr);
}

static int  eval_var_def(interp_t *interp, env_t *env, expr_t *e)
{
    if (e->type == EXPR_ID) {
        return env_add_variable(env, ast_expr_text(e));
    }

    while(e->type == EXPR_ASSIGN) {
        expr_t *lft = ast_expr_lft(e);
        if (lft->type == EXPR_ID && 0 > env_add_variable(env, ast_expr_text(lft))) {
            return -1;
        }

        e = ast_expr_rht(e);
    }

    return 0;
}

static void eval_stmt_var(interp_t *interp, env_t *env, stmt_t *s)
{
    expr_t *e = s->expr;

    while (e && interp->error == 0) {
        expr_t *next;

        if (e->type == EXPR_COMMA) {
            next = ast_expr_rht(e);
            e    = ast_expr_lft(e);
        } else {
            next = NULL;
        }

        if (-1 == eval_var_def(interp, env, e)) {
            interp_set_error(interp, ERR_NotEnoughMemory);
            return;
        }

        if (e->type == EXPR_ASSIGN) {
            eval_expr(interp, env, e);
            interp_stack_pop(interp);
        }

        e = next;
    }
}

static void eval_stmt(interp_t *interp, env_t *env, stmt_t *s);

static void eval_stmt_if(interp_t *interp, env_t *env, stmt_t *s)
{
    stmt_t *curr;

    eval_expr(interp, env, s->expr);
    if (interp->error) return;

    if (val_is_true(*interp_stack_pop(interp))) {
        curr = s->block;
    } else {
        curr = s->other;
    }

    while (curr && interp->error == 0) {
        eval_stmt(interp, env, curr);
        curr = curr->next;
    }
}

static void eval_stmt_while(interp_t *interp, env_t *env, stmt_t *s)
{
    while (interp->error == 0) {
        stmt_t *curr;

        eval_expr(interp, env, s->expr);
        if (!val_is_true(*interp_stack_pop(interp))) {
            break;
        }
        curr = s->block;

        while (curr && interp->error == 0 && interp->skip == 0) {
            eval_stmt(interp, env, curr);
            curr = curr->next;
        }

        if (interp->skip == 1) { // continue
            interp->skip = 0;
        } else
        if (interp->skip == 2) { // break;
            interp->skip = 0;
            break;
        }
    }
}

static void eval_stmt_break(interp_t *interp, env_t *env, stmt_t *s)
{
    interp->skip = 2;
}

static void eval_stmt_continue(interp_t *interp, env_t *env, stmt_t *s)
{
    interp->skip = 1;
}

static void eval_stmt_ret(interp_t *interp, env_t *env, stmt_t *s)
{
    interp_set_error(interp, ERR_NotImplemented);
}

static void eval_stmt(interp_t *interp, env_t *env, stmt_t *s)
{
    static val_t undefined = TAG_UNDEFINED;

    switch(s->type) {
    case STMT_PASS: break;
    case STMT_EXPR: eval_stmt_expr(interp, env, s); interp->result = interp_stack_pop(interp); break;
    case STMT_VAR:  eval_stmt_var(interp, env, s); interp->result = &undefined; break;
    case STMT_IF:       eval_stmt_if(interp, env, s); interp->result = &undefined; break;
    case STMT_WHILE:    eval_stmt_while(interp, env, s); interp->result = &undefined; break;
    case STMT_RET:      eval_stmt_ret(interp, env, s); interp->result = &undefined; break;
    case STMT_BREAK:    eval_stmt_break(interp, env, s); interp->result = &undefined; break;
    case STMT_CONTINUE: eval_stmt_continue(interp, env, s); interp->result = &undefined; break;
    default: interp_set_error(interp, ERR_InvalidSyntax);
    }
}

int eval_env_init(eval_env_t *env)
{
    int size = 16;
    scope_t *scope = env_scope_create(size, NULL);
    intptr_t sym_tbl  = symtbl_create();

    if (!scope || !sym_tbl) {
        if (scope) env_scope_destroy(scope);
        if (sym_tbl) symtbl_destroy(sym_tbl);
        return -1;
    }

    env->scope = scope;
    env->sym_tbl = sym_tbl;

    return compile_init(&env->cpl, sym_tbl);
}

int eval_env_deinit(eval_env_t *env)
{
    if (!env) {
        return -1;
    }

    if (env->scope) env_scope_destroy(env->scope);
    if (env->sym_tbl) symtbl_destroy(env->sym_tbl);

    compile_deinit(&env->cpl);

    return 0;
}

static int eval_env_adjust(eval_env_t *env)
{
    return env_scope_extend_to(env->scope, compile_var_num(&env->cpl));
}

int eval_env_add_var(eval_env_t *env, const char *name, val_t value)
{
    intptr_t sym_id = symtbl_add(env->sym_tbl, name);
    int      extend = compile_var_add(&env->cpl, sym_id);

    if (extend == 1) {
        return env_scope_extend(env->scope, value);
    } else
    if (extend == 0) {
        return env_scope_set(env->scope, compile_var_get(&env->cpl, sym_id), value);
    } else {
        return -1;
    }
}

int eval_env_get_var(eval_env_t *env, const char *name, val_t **value)
{
    intptr_t sym_id = symtbl_get(env->sym_tbl, name);
    int      var_id = compile_var_get(&env->cpl, sym_id);

    return env_scope_get(env->scope, var_id, value);
}

int eval_env_set_var(eval_env_t *env, const char *name, val_t value)
{
    intptr_t sym_id = symtbl_get(env->sym_tbl, name);
    int      var_id = compile_var_get(&env->cpl, sym_id);

    return env_scope_set(env->scope, var_id, value);
}

int eval_string(interp_t *interp, eval_env_t *env, const char *input, val_t **v)
{
    lexer_t lex_st;
    stmt_t  *stmt;
    module_t mod;
    intptr_t lex;
    static val_t undefined = TAG_UNDEFINED;
    int done = 0, last_type = 0;

    if (!interp || !env || !input) {
        return -1;
    }

    get_line_init(input);
    lex = lex_init(&lex_st, get_line_from_string);

    while (!done) {
        stmt = parse_stmt(lex, NULL, NULL);
        if (!stmt) break;


        compile_code_clean(&env->cpl);
        if (0 == compile_one_stmt(&env->cpl, stmt, NULL, NULL) && 0 == eval_env_adjust(env)) {
            compile_build_module(&env->cpl, &mod);
            if (0 != interp_run(interp, (env_t *)env, &mod) && v) {
                done = -2;
            }
        } else {
            done = -1;
        }

        last_type = stmt->type;
        ast_stmt_release(stmt);
    }

    if (!done && v) {
        if (last_type == STMT_EXPR) {
            *v = interp->result;
        } else {
            *v = &undefined;
        }
    }

    return done;
}

