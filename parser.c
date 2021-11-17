#include "scanner.h"
#include "parser.h"
#include "symtable.h"
#include "stack.h"
#include "token.h"
#include "error.h"

#define NO_ERR 0
#define CHECK_TOKEN(token, my_type)        \
    do                                     \
    {                                      \
        if ((*token)->type == my_type)     \
            get_token(global_file, token); \
        else                               \
            return ERR_SYNTAX;             \
    } while (0)

/**
 * @brief Implementace <prg>
 *
 * @param Posledni nacteny token
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int prg(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <prg> -> eof <prg>
    case TYPE_EOF:

        return prg(token);

    // <prg> -> require "string" <prg>
    case TYPE_KW_REQUIRE:
        get_token(global_file, token);

        // "string"
        CHECK_TOKEN(token, TYPE_STRING);

        return prg(token);

    // <prg> -> global id : function ( <parm> ) <ret> <prg>
    case TYPE_KW_GLOBAL:
        get_token(global_file, token);

        CHECK_TOKEN(token, TYPE_IDENTIFIER);

        CHECK_TOKEN(token, TYPE_ASSIGN);

        CHECK_TOKEN(token, TYPE_KW_FUNCTION);

        CHECK_TOKEN(token, TYPE_LEFT_PARENTHESES);

        res = parm(token);
        if (res != NO_ERR)
            return res;

        CHECK_TOKEN(token, TYPE_RIGHT_PARENTHESES);

        res = ret(token);
        if (res != NO_ERR)
            return res;

        return prg(token);

        // <prg> -> id ( <lof_e> ) <prg>
    case TYPE_IDENTIFIER:
        get_token(global_file, token);

        CHECK_TOKEN(token, TYPE_LEFT_PARENTHESES);

        res = lof_e(token);
        if (res != NO_ERR)
            return res;

        CHECK_TOKEN(token, TYPE_RIGHT_PARENTHESES);

        return prg(token);

    // <prg> -> function id ( <def_parm> ) <ret> <code> end <prg>
    case TYPE_KW_FUNCTION:

        CHECK_TOKEN(token, TYPE_IDENTIFIER);

        CHECK_TOKEN(token, TYPE_LEFT_PARENTHESES);

        res = def_parm(token);
        if (res != NO_ERR)
            return res;

        CHECK_TOKEN(token, TYPE_RIGHT_PARENTHESES);

        res = ret(token);
        if (res != NO_ERR)
            return res;

        res = code(token);
        if (res != NO_ERR)
            return res;

        CHECK_TOKEN(token, TYPE_KW_END);

        return prg(token);

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int lof_e(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <lof_e> -> <expression> <lof_e_n>
    case TYPE_STRING:

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return lof_e_n(token);

    // <lof_e> -> eps
    // <lof_e> -> <expression> <lof_e_n>
    case TYPE_IDENTIFIER:
        //TODO:
        break;

    // <lof_e> -> <expression> <lof_e_n>
    case TYPE_LEFT_PARENTHESES:

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return lof_e_n(token);

    // <lof_e> -> eps
    case TYPE_RIGHT_PARENTHESES:
        break;

    // <lof_e> -> eps
    case TYPE_KW_END:
        break;

    // <lof_e> -> eps
    case TYPE_KW_LOCAL:
        break;

    // <lof_e> -> eps
    case TYPE_KW_IF:
        break;

    // <lof_e> -> eps
    case TYPE_KW_WHILE:
        break;

    // <lof_e> -> eps
    case TYPE_KW_RETURN:
        break;

    // <lof_e> -> eps
    case TYPE_KW_ELSE:
        break;

    // <lof_e> -> <expression> <lof_e_n>
    case TYPE_BOOLEAN:

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return lof_e_n(token);

    // <lof_e> -> <expression> <lof_e_n>
    case TYPE_INTEGER:

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return lof_e_n(token);

    // <lof_e> -> <expression> <lof_e_n>
    case TYPE_KW_NIL:

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return lof_e_n(token);

    // <lof_e> -> <expression> <lof_e_n>
    case TYPE_NUMBER:

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return lof_e_n(token);

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int lof_e_n(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <lof_e_n> -> eps
    case TYPE_IDENTIFIER:
        break;

    // <lof_e_n> -> eps
    case TYPE_RIGHT_PARENTHESES:
        break;

    // <lof_e_n> -> eps
    case TYPE_KW_END:
        break;

    // <lof_e_n> -> , <expression> <lof_e_n>
    case TYPE_COMMA:

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return lof_e_n(token);

    // <lof_e_n> -> eps
    case TYPE_KW_LOCAL:
        break;

    // <lof_e_n> -> eps
    case TYPE_KW_IF:
        break;

    // <lof_e_n> -> eps
    case TYPE_KW_WHILE:
        break;

    // <lof_e_n> -> eps
    case TYPE_KW_RETURN:
        break;

    // <lof_e_n> -> eps
    case TYPE_KW_ELSE:
        break;

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int parm(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <parm> -> eps
    case TYPE_RIGHT_PARENTHESES:
        break;

    // <parm> -> <d_type> <parm_n>
    case TYPE_KW_STRING:

        res = d_type(token);
        if (res != NO_ERR)
            return res;

        return parm_n(token);

    // <parm> -> <d_type> <parm_n>
    case TYPE_KW_INTEGER:

        res = d_type(token);
        if (res != NO_ERR)
            return res;

        return parm_n(token);

    // <parm> -> <d_type> <parm_n>
    case TYPE_KW_NUMBER:

        res = d_type(token);
        if (res != NO_ERR)
            return res;

        return parm_n(token);

    // <parm> -> <d_type> <parm_n>
    case TYPE_KW_BOOLEAN:

        res = d_type(token);
        if (res != NO_ERR)
            return res;

        return parm_n(token);

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int parm_n(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <parm_n> -> , <d_type> <parm_n>
    case TYPE_COMMA:

        res = d_type(token);
        if (res != NO_ERR)
            return res;

        return parm_n(token);

    // <parm_n> -> eps
    case TYPE_EOF:
        token_delete(token);
        break;

    // <parm_n> -> eps
    case TYPE_KW_REQUIRE:
        break;

    // <parm_n> -> eps
    case TYPE_KW_GLOBAL:
        break;

    // <parm_n> -> eps
    case TYPE_IDENTIFIER:
        break;

    // <parm_n> -> eps
    case TYPE_KW_FUNCTION:
        break;

    // <parm_n> -> eps
    case TYPE_RIGHT_PARENTHESES:
        break;

    // <parm_n> -> eps
    case TYPE_KW_END:
        break;

    // <parm_n> -> eps
    case TYPE_KW_LOCAL:
        break;

    // <parm_n> -> eps
    case TYPE_KW_IF:
        break;

    // <parm_n> -> eps
    case TYPE_KW_WHILE:
        break;

    // <parm_n> -> eps
    case TYPE_KW_RETURN:
        break;

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int ret(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <ret> -> eps
    case TYPE_EOF:
        token_delete(token);
        break;

    // <ret> -> eps
    case TYPE_KW_REQUIRE:
        break;

    // <ret> -> eps
    case TYPE_KW_GLOBAL:
        break;

    // <ret> -> eps
    case TYPE_IDENTIFIER:
        break;

    // <ret> -> : <d_type> <parm_n>
    case TYPE_ASSIGN:

        res = d_type(token);
        if (res != NO_ERR)
            return res;

        return parm_n(token);

    // <ret> -> eps
    case TYPE_KW_FUNCTION:
        break;

    // <ret> -> eps
    case TYPE_KW_END:
        break;

    // <ret> -> eps
    case TYPE_KW_LOCAL:
        break;

    // <ret> -> eps
    case TYPE_KW_IF:
        break;

    // <ret> -> eps
    case TYPE_KW_WHILE:
        break;

    // <ret> -> eps
    case TYPE_KW_RETURN:
        break;

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int def_parm(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <def_parm> -> id : <d_type> <def_parm_n>
    case TYPE_IDENTIFIER:
        get_token(global_file, token);

        CHECK_TOKEN(token, TYPE_ASSIGN);

        res = d_type(token);
        if (res != NO_ERR)
            return res;

        return def_parm_n(token);

    // <def_parm> -> eps
    case TYPE_RIGHT_PARENTHESES:
        break;

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int def_parm_n(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <def_parm_n> -> eps
    case TYPE_RIGHT_PARENTHESES:
        break;

    // <def_parm_n> -> , id : <d_type> <def_parm_n>
    case TYPE_COMMA:
        get_token(global_file, token);

        CHECK_TOKEN(token, TYPE_IDENTIFIER);

        CHECK_TOKEN(token, TYPE_ASSIGN);

        res = d_type(token);
        if (res != NO_ERR)
            return res;

        return def_parm_n(token);

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

int repeat_block(token_t **token)
{
    // TODO: zatim ignorovat
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <repeat_block> -> repeat <code> until ( <expresssion )
    case TYPE_KW_REPEAT:
        get_token(global_file, token);

        // code
        res = code(token);
        if (res != NO_ERR)
            return res;

        // until
        CHECK_TOKEN(token, TYPE_KW_UNTIL);

        // (
        CHECK_TOKEN(token, TYPE_LEFT_PARENTHESES);

        // exp
        res = expression(token);
        if (res != NO_ERR)
            return res;

        // )
        CHECK_TOKEN(token, TYPE_RIGHT_PARENTHESES);

        break;

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}
int for_block(token_t **token)
{
    // TODO: zatim ignorovat
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <for_block> -> for <expression> , <expression> <exp_opt> do <code> end
    case TYPE_KW_FOR:
        get_token(global_file, token);

        res = expression(token);
        if (res != NO_ERR)
            return res;

        CHECK_TOKEN(token, TYPE_COMMA);

        res = expression(token);
        if (res != NO_ERR)
            return res;

        res = exp_opt(token);
        if (res != NO_ERR)
            return res;

        CHECK_TOKEN(token, TYPE_KW_DO);

        res = code(token);
        if (res != NO_ERR)
            return res;

        CHECK_TOKEN(token, TYPE_KW_END);

        break;

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}
int var_init(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <var_init> -> local id : <d_type> <var_init_assign>
    case TYPE_KW_LOCAL:
        get_token(global_file, token);

        CHECK_TOKEN(token, TYPE_IDENTIFIER);

        CHECK_TOKEN(token, TYPE_DECLARE);

        res = d_type(token);
        if (res != NO_ERR)
            return res;

        return var_init_assign(token);

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}
int func_or_assign(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <func_or_assign> -> ( <func_args> )
    case TYPE_LEFT_PARENTHESES:
        get_token(global_file, token);

        res = func_args(token);
        if (res != NO_ERR)
            return res;

        CHECK_TOKEN(token, TYPE_RIGHT_PARENTHESES);

        break;

    // <func_or_assign> -> <multi_var> = <expression> <multi_exp>
    case TYPE_COMMA:

        res = multi_var(token);
        if (res != NO_ERR)
            return res;

        CHECK_TOKEN(token, TYPE_ASSIGN);

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return multi_exp(token);

    // <func_or_assign> -> <multi_var> = <expression> <multi_exp>
    case TYPE_ASSIGN:

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return multi_exp(token);

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}
int ret_exp(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <ret_exp> -> eps
    case TYPE_EOF:
        token_delete(token);
        break;

    // <ret_exp> -> <expression> <ret_exp_n>
    case TYPE_IDENTIFIER:

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return ret_exp_n(token);

    // <ret_exp> -> eps
    case TYPE_KW_ELSEIF:
        break;

    // <ret_exp> -> eps
    case TYPE_KW_ELSE:
        break;

    // <ret_exp> -> eps
    case TYPE_KW_UNTIL:
        break;

    // <ret_exp> -> <expression> <ret_exp_n>
    case TYPE_LEFT_PARENTHESES:

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return ret_exp_n(token);

    // <ret_exp> -> <expression> <ret_exp_n>
    case TYPE_STRING:

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return ret_exp_n(token);

    // <ret_exp> -> <expression> <ret_exp_n>
    case TYPE_NUMBER:

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return ret_exp_n(token);

    // <ret_exp> -> <expression> <ret_exp_n>
    case TYPE_INTEGER:

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return ret_exp_n(token);

    // <ret_exp> -> <expression> <ret_exp_n>
    case TYPE_KW_NIL:

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return ret_exp_n(token);

    // <ret_exp> -> <expression> <ret_exp_n>
    case TYPE_STRLEN:

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return ret_exp_n(token);

    // <ret_exp> -> <expression> <ret_exp_n>
    case TYPE_BOOLEAN:

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return ret_exp_n(token);

    // <ret_exp> -> <expression> <ret_exp_n>
    case TYPE_KW_NOT:

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return ret_exp_n(token);

    // <ret_exp> -> <expression> <ret_exp_n>
    case TYPE_CONCAT:

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return ret_exp_n(token);

    // <ret_exp> -> eps
    case TYPE_KW_END:
        break;

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}


int d_type(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <d_type> -> string
    case TYPE_KW_STRING:
        get_token(global_file, token);
        // TODO:
        break;

    // <d_type> -> integer
    case TYPE_KW_INTEGER:
        get_token(global_file, token);
        // TODO:
        break;

    // <d_type> -> number
    case TYPE_KW_NUMBER:
        get_token(global_file, token);
        // TODO:
        break;

    // <d_type> -> boolean
    case TYPE_KW_BOOLEAN:
        get_token(global_file, token);
        // TODO:
        break;

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int ret_n(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <ret_n> -> eps
    case TYPE_EOF:
        token_delete(token);
        break;

    // <ret_n> -> eps
    case TYPE_KW_REQUIRE:
        break;

    // <ret_n> -> eps
    case TYPE_IDENTIFIER:
        break;

    // <ret_n> -> eps
    case TYPE_KW_RETURN:
        break;

    // <ret_n> -> eps
    case TYPE_KW_GLOBAL:
        break;

    // <ret_n> -> eps
    case TYPE_KW_FUNCTION:
        break;

    // <ret_n> -> eps
    case TYPE_KW_LOCAL:
        break;

    // <ret_n> -> eps
    case TYPE_KW_IF:
        break;

    // <ret_n> -> eps
    case TYPE_KW_WHILE:
        break;

    // <ret_n> -> eps
    case TYPE_KW_REPEAT:
        break;

    // <ret_n> -> eps
    case TYPE_KW_FOR:
        break;

    // <ret_n> -> eps
    case TYPE_KW_END:
        break;

    // <ret_n> -> eps
    case TYPE_KW_BREAK:
        break;

    // <ret_n> -> <d_type> <ret_n>
    case TYPE_COMMA:

        res = d_type(token);
        if (res != NO_ERR)
            return res;

        return ret_n(token);

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}
int args(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <args> -> id : <d_type> <args_n>
    case TYPE_IDENTIFIER:
        get_token(global_file, token);

        CHECK_TOKEN(token, TYPE_DECLARE);

        res = d_type(token);
        if (res != NO_ERR)
            return res;

        return args_n(token);

    // <args> -> eps
    case TYPE_RIGHT_PARENTHESES:
        break;

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}
int code(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <code> -> id <func_or_assign> <code>
    case TYPE_IDENTIFIER:
        get_token(global_file, token);

        res = func_or_assign(token);
        if (res != NO_ERR)
            return res;

        return code(token);

    // <code> -> return <ret_exp>
    case TYPE_KW_RETURN:
        get_token(global_file, token);

        CHECK_TOKEN(token, TYPE_KW_RETURN);

        return ret_exp(token);

    // <code> -> <var_init> <code>
    case TYPE_KW_LOCAL:

        res = var_init(token);
        if (res != NO_ERR)
            return res;

        return code(token);

    // <code> -> <if_block> <code>
    case TYPE_KW_IF:

        res = if_block(token);
        if (res != NO_ERR)
            return res;

        return code(token);

    // <code> -> eps
    case TYPE_KW_ELSEIF:
        break;

    // <code> -> eps
    case TYPE_KW_ELSE:
        break;

    // <code> -> <while_block> <code>
    case TYPE_KW_WHILE:

        res = while_block(token);
        if (res != NO_ERR)
            return res;

        return code(token);

    // <code> -> <repeat_block> <code>
    case TYPE_KW_REPEAT:

        res = repeat_block(token);
        if (res != NO_ERR)
            return res;

        return code(token);

    // <code> -> eps
    case TYPE_KW_UNTIL:
        break;

    // <code> -> <for_block> <code>
    case TYPE_KW_FOR:

        res = for_block(token);
        if (res != NO_ERR)
            return res;

        return code(token);

    // <code> -> eps
    case TYPE_KW_END:
        break;

    // <code> -> break
    case TYPE_KW_BREAK:
        get_token(global_file, token);

        CHECK_TOKEN(token, TYPE_KW_BREAK);

        break;

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}
int args_n(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <args_n> -> eps
    case TYPE_RIGHT_PARENTHESES:
        break;

    // <args_n> -> id : <d_type> <args_n>
    case TYPE_COMMA:
        get_token(global_file, token);

        CHECK_TOKEN(token, TYPE_IDENTIFIER);

        CHECK_TOKEN(token, TYPE_DECLARE);

        res = d_type(token);
        if (res != NO_ERR)
            return res;

        return args_n(token);

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}
int var_init_assign(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <var_init_assign> -> eps
    case TYPE_EOF:
        token_delete(token);
        break;

    // <var_init_assign> -> eps
    case TYPE_KW_REQUIRE:
        break;

    // <var_init_assign> -> eps
    case TYPE_IDENTIFIER:
        break;

    // <var_init_assign> -> eps
    case TYPE_KW_RETURN:
        break;

    // <var_init_assign> -> eps
    case TYPE_KW_GLOBAL:
        break;

    // <var_init_assign> -> eps
    case TYPE_KW_FUNCTION:
        break;

    // <var_init_assign> -> eps
    case TYPE_KW_LOCAL:
        break;

    // <var_init_assign> -> eps
    case TYPE_KW_IF:
        break;

    // <var_init_assign> -> eps
    case TYPE_KW_ELSEIF:
        break;

    // <var_init_assign> -> eps
    case TYPE_KW_ELSE:
        break;

    // <var_init_assign> -> eps
    case TYPE_KW_WHILE:
        break;

    // <var_init_assign> -> eps
    case TYPE_KW_REPEAT:
        break;

    // <var_init_assign> -> eps
    case TYPE_KW_UNTIL:
        break;

    // <var_init_assign> -> eps
    case TYPE_KW_FOR:
        break;

    // <var_init_assign> -> eps
    case TYPE_KW_END:
        break;

    // <var_init_assign> -> eps
    case TYPE_KW_BREAK:
        break;

    // <var_init_assign> -> = <expression>
    case TYPE_ASSIGN:
        get_token(global_file, token);

        return expression(token);

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}
int expression(token_t **token)
{
    // TODO: zatim ignorovat
    //TODO:
}
int elif_block(token_t **token)
{
    // TODO: zatim ignorovat
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <elif_block> -> elseif <expression> do <code> end
    case TYPE_KW_ELSEIF:
        get_token(global_file, token);

        // expression
        res = expression(token);
        if (res != NO_ERR)
            return res;

        // do
        CHECK_TOKEN(token, TYPE_KW_DO);

        // code
        res = code(token);
        if (res != NO_ERR)
            return res;

        // end
        CHECK_TOKEN(token, TYPE_KW_END);

        break;

    // <elif_block> -> eps
    case TYPE_KW_ELSE:
        break;

    // <elif_block> -> eps
    case TYPE_KW_END:
        break;

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}
int else_block(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <else_block> -> else <code>
    case TYPE_KW_ELSE:
        get_token(global_file, token);

        return code(token);

    // <else_block> -> eps
    case TYPE_KW_END:
        break;

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}
int exp_opt(token_t **token)
{
    // TODO: zatim ignorovat
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <exp_opt> -> eps
    case TYPE_KW_DO:
        break;

    // <exp_opt> -> "integer"
    case TYPE_COMMA:
        get_token(global_file, token);

        CHECK_TOKEN(token, TYPE_INTEGER);

        break;

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}
int func_args(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <func_args> -> <expression> <func_args_n>
    case TYPE_IDENTIFIER:

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return func_args_n(token);

    // <func_args> -> <expression> <func_args_n>
    case TYPE_LEFT_PARENTHESES:

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return func_args_n(token);

    // <func_args> -> eps
    case TYPE_RIGHT_PARENTHESES:
        break;

    // <func_args> -> <expression> <func_args_n>
    case TYPE_STRING:

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return func_args_n(token);

    // <func_args> -> <expression> <func_args_n>
    case TYPE_NUMBER:

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return func_args_n(token);

    // <func_args> -> <expression> <func_args_n>
    case TYPE_INTEGER:

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return func_args_n(token);

    // <func_args> -> <expression> <func_args_n>
    case TYPE_KW_NIL:

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return func_args_n(token);

    // <func_args> -> <expression> <func_args_n>
    case TYPE_STRLEN:

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return func_args_n(token);

    // <func_args> -> <expression> <func_args_n>
    case TYPE_BOOLEAN:

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return func_args_n(token);

    // <func_args> -> <expression> <func_args_n>
    case TYPE_KW_NOT:

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return func_args_n(token);

    // <func_args> -> <expression> <func_args_n>
    case TYPE_CONCAT:

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return func_args_n(token);

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}
int multi_var(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <multi_var> -> eps
    case TYPE_ASSIGN:
        break;

    // <multi_var> -> id <multi_var>
    case TYPE_COMMA:
        get_token(global_file, token);

        CHECK_TOKEN(token, TYPE_IDENTIFIER);

        return multi_var(token);

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}
int multi_exp(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <multi_exp> -> eps
    case TYPE_EOF:
        token_delete(token);
        break;

    // <multi_exp> -> eps
    case TYPE_KW_REQUIRE:
        break;

    // <multi_exp> -> eps
    case TYPE_IDENTIFIER:
        break;

    // <multi_exp> -> eps
    case TYPE_KW_RETURN:
        break;

    // <multi_exp> -> eps
    case TYPE_KW_GLOBAL:
        break;

    // <multi_exp> -> eps
    case TYPE_KW_FUNCTION:
        break;

    // <multi_exp> -> eps
    case TYPE_KW_LOCAL:
        break;

    // <multi_exp> -> eps
    case TYPE_KW_IF:
        break;

    // <multi_exp> -> eps
    case TYPE_KW_ELSEIF:
        break;

    // <multi_exp> -> eps
    case TYPE_KW_ELSE:
        break;

    // <multi_exp> -> eps
    case TYPE_KW_WHILE:
        break;

    // <multi_exp> -> eps
    case TYPE_KW_REPEAT:
        break;

    // <multi_exp> -> eps
    case TYPE_KW_UNTIL:
        break;

    // <multi_exp> -> eps
    case TYPE_KW_FOR:
        break;

    // <multi_exp> -> , <expression> <multi_exp>
    case TYPE_COMMA:
        get_token(global_file, token);

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return multi_exp(token);

    // <multi_exp> -> eps
    case TYPE_KW_END:
        break;

    // <multi_exp> -> eps
    case TYPE_KW_BREAK:
        break;

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}
int func_args_n(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <func_args_n> -> eps
    case TYPE_RIGHT_PARENTHESES:
        break;

    // <func_args_n> -> , <expression> <func_args_n>
    case TYPE_COMMA:
        get_token(global_file, token);

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return func_args_n(token);

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}
int ret_exp_n(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <ret_exp_n> -> eps
    case TYPE_EOF:
        token_delete(token);
        break;

    // <ret_exp_n> -> eps
    case TYPE_KW_ELSEIF:
        break;

    // <ret_exp_n> -> eps
    case TYPE_KW_ELSE:
        break;

    // <ret_exp_n> -> eps
    case TYPE_KW_UNTIL:
        break;

    // <ret_exp_n> -> , <expression> <ret_exp_n>
    case TYPE_COMMA:
        get_token(global_file, token);

        res = expression(token);
        if (res != NO_ERR)
            return res;

        return ret_exp_n(token);

    // <ret_exp_n> -> eps
    case TYPE_KW_END:
        break;

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

FILE *global_file;
frame_stack global_stack;
node_ptr global_tree;

int syntax_analysis(FILE *file)
{
    int res;
    token_t *token = NULL;
    global_file = file;

    // inicializace globalni TS a zasobniku TS
    if ((res = tree_init(&global_tree)) != NO_ERR)
        goto tree_init_error;
    if ((res = stack_init(&global_stack)) != NO_ERR)
        goto post_tree_error;
    // TODO: generate code init

    // poslani prvniho tokenu do <prg>
    res = get_token(global_file, &token);
    if (res != NO_ERR)
        goto post_stack_error;

    res = prg(&token);

    // TODO: destroy generate code
post_stack_error:
    stack_destroy(&global_stack, &tree_destroy);
post_tree_error:
    tree_destroy(&global_tree);
tree_init_error:
    return res;
}

/* parser.c */
