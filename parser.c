#include "scanner.h"
#include "parser.h"
#include "symtable.h"
#include "stack.h"
#include "token.h"
#include "error.h"

#define NO_ERR 0

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

    switch((*token)->type)
    {
        // <prg> -> eps
        case TYPE_EOF:
            token_delete(token);
            break;

        // <prg> -> require "string" <prg>
        case TYPE_KW_REQUIRE:
            // TODO:
            break;

        // <prg> -> id <func_or_assign> <prg>
        case TYPE_IDENTIFIER:
            // TODO:
            break;

        // <prg> -> return <ret_exp>>
        case TYPE_KW_RETURN:
            // TODO:
            break;

        // <prg> -> <func_dec> <prg>
        case TYPE_KW_GLOBAL:
            // TODO:
            break;

        // <prg> -> <func_def> <prg>
        case TYPE_KW_FUNCTION:
            // TODO:
            break;

        // <prg> -> <var_init> <prg>
        case TYPE_KW_LOCAL:
            // TODO:
            break;

        // <prg> -> <if_block> <prg>
        case TYPE_KW_IF:
            // TODO:
            break;

        // <prg> -> <while_block> <prg>
        case TYPE_KW_WHILE:
            // TODO:
            break;

        // <prg> -> <repeat_block> <prg>
        case TYPE_KW_REPEAT:
            // TODO:
            break;

        // <prg> -> <for_block> <prg>
        case TYPE_KW_FOR:
            // TODO:
            break;

        default:
            res = ERR_SYNTAX;
            break;
    }
    return res;
}

int func_dec(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch((*token)->type)
    {
        // <func_dec> -> global id : function ( <parm> ) : <ret>
        case TYPE_KW_GLOBAL:
            // TODO:
            break;

        default:
            res = ERR_SYNTAX;
            break;
    }
    return res;
}
int func_def(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch((*token)->type)
    {
        // <func_def> -> function id ( <args> ) : <ret> <code> end
        case TYPE_KW_FUNCTION:
            // TODO:
            break;

        default:
            res = ERR_SYNTAX;
            break;
    }
    return res;
}
int if_block(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch((*token)->type)
    {
        // <if_block> -> if <expression> then <code > <elif_block> <else_block> end
        case TYPE_KW_IF:
            // TODO:
            break;

        default:
            res = ERR_SYNTAX;
            break;
    }
    return res;
}
int while_block(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch((*token)->type)
    {
        // <while_block> -> while <expression> do <code> end
        case TYPE_KW_WHILE:
            // TODO:
            break;

        default:
            res = ERR_SYNTAX;
            break;
    }
    return res;
}
int repeat_block(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch((*token)->type)
    {
        // <repeat_block> -> repeat <code> until ( <expresssion )
        case TYPE_KW_REPEAT:
            // TODO:
            break;

        default:
            res = ERR_SYNTAX;
            break;
    }
    return res;
}
int for_block(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch((*token)->type)
    {
        // <for_block> -> for <expression> , <expression> <exp_opt> do <code> end
        case TYPE_KW_FOR:
            // TODO:
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

    switch((*token)->type)
    {
        // <var_init> -> local id : <d_type> <var_init_assign>
        case TYPE_KW_LOCAL:
            // TODO:
            break;

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

    switch((*token)->type)
    {
        // <func_or_assign> -> ( <func_args> )
        case TYPE_LEFT_PARENTHESES:
            // TODO:
            break;

        // <func_or_assign> -> <multi_var> = <expression> <multi_exp>
        case TYPE_COMMA:
            // TODO:
            break;

        // <func_or_assign> -> <multi_var> = <expression> <multi_exp>
        case TYPE_EQ:
            // TODO:
            break;

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

    switch((*token)->type)
    {
        // <ret_exp> -> eps
        case TYPE_EOF:
            token_delete(token);
            break;

        // <ret_exp> -> <expression> <ret_exp_n>
        case TYPE_IDENTIFIER:
            // TODO:
            break;

        // <ret_exp> -> eps
        case TYPE_KW_ELSEIF:
            // TODO:
            break;

        // <ret_exp> -> eps
        case TYPE_KW_ELSE:
            // TODO:
            break;

        // <ret_exp> -> eps
        case TYPE_KW_UNTIL:
            // TODO:
            break;

        // <ret_exp> -> <expression> <ret_exp_n>
        case TYPE_LEFT_PARENTHESES:
            // TODO:
            break;

        // <ret_exp> -> <expression> <ret_exp_n>
        case TYPE_STRING:
            //TODO:
            break;

        // <ret_exp> -> <expression> <ret_exp_n>
        case TYPE_NUMBER:
            // TODO:
            break;

        // <ret_exp> -> <expression> <ret_exp_n>
        case TYPE_INTEGER:
            // TODO:
            break;

        // <ret_exp> -> <expression> <ret_exp_n>
        case TYPE_KW_NIL:
            // TODO:
            break;

        // <ret_exp> -> <expression> <ret_exp_n>
        case TYPE_STRLEN:
            // TODO:
            break;

        // <ret_exp> -> <expression> <ret_exp_n>
        case TYPE_KW_TRUE:
            // TODO:
            break;

        // <ret_exp> -> <expression> <ret_exp_n>
        case TYPE_KW_FALSE:
            // TODO:
            break;

        // <ret_exp> -> <expression> <ret_exp_n>
        case TYPE_KW_NOT:
            // TODO:
            break;

        // <ret_exp> -> <expression> <ret_exp_n>
        case TYPE_CONCAT:
            // TODO:
            break;

        // <ret_exp> -> eps
        case TYPE_KW_END:
            // TODO:
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

    switch((*token)->type)
    {
        // <parm> -> eps
        case TYPE_RIGHT_PARENTHESES:
            // TODO:
            break;

        // <parm> -> <d_type> <parm_n>
        case TYPE_KW_STRING:
            // TODO:
            break;

        // <parm> -> <d_type> <parm_n>
        case TYPE_KW_INTEGER:
            // TODO:
            break;

        // <parm> -> <d_type> <parm_n>
        case TYPE_KW_NUMBER:
            // TODO:
            break;

        // <parm> -> <d_type> <parm_n>
        case TYPE_KW_BOOLEAN:
            // TODO:
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

    switch((*token)->type)
    {
        // <ret> -> eps
        case TYPE_EOF:
            token_delete(token);
            break;

        // <ret> -> eps
        case TYPE_KW_REQUIRE:
            // TODO:
            break;

        // <ret> -> eps
        case TYPE_IDENTIFIER:
            // TODO:
            break;

        // <ret> -> eps
        case TYPE_KW_RETURN:
            // TODO:
            break;

        // <ret> -> eps
        case TYPE_KW_GLOBAL:
            // TODO:
            break;

        // <ret> -> eps
        case TYPE_KW_FUNCTION:
            // TODO:
            break;

        // <ret> -> eps
        case TYPE_KW_LOCAL:
            // TODO:
            break;

        // <ret> -> eps
        case TYPE_KW_IF:
            // TODO:
            break;

        // <ret> -> eps
        case TYPE_KW_WHILE:
            // TODO:
            break;

        // <ret> -> eps
        case TYPE_KW_REPEAT:
            // TODO:
            break;

        // <ret> -> eps
        case TYPE_KW_FOR:
            // TODO:
            break;

        // <ret> -> eps
        case TYPE_KW_END:
            // TODO:
            break;

        // <ret> -> <d_type> <ret_n>
        case TYPE_KW_STRING:
            // TODO:
            break;

        // <ret> -> <d_type> <ret_n>
        case TYPE_KW_INTEGER:
            // TODO:
            break;

        // <ret> -> <d_type> <ret_n>
        case TYPE_KW_NUMBER:
            // TODO:
            break;

        // <ret> -> <d_type> <ret_n>
        case TYPE_KW_BOOLEAN:
            // TODO:
            break;

        // <ret> -> eps
        case TYPE_KW_BREAK:
            // TODO:
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

    switch((*token)->type)
    {
        // <d_type> -> string
        case TYPE_KW_STRING:
            // TODO:
            break;

        // <d_type> -> integer
        case TYPE_KW_INTEGER:
            // TODO:
            break;

        // <d_type> -> number
        case TYPE_KW_NUMBER:
            // TODO:
            break;

        // <d_type> -> boolean
        case TYPE_KW_BOOLEAN:
            // TODO:
            break;

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

    switch((*token)->type)
    {
        // <parm_n> -> <d_type> <parm_n>
        case TYPE_COMMA:
            // TODO:
            break;

        // <parm_n> -> eps
        case TYPE_RIGHT_PARENTHESES:
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

    switch((*token)->type)
    {
        // <ret_n> -> eps
        case TYPE_EOF:
            token_delete(token);
            break;

        // <ret_n> -> eps
        case TYPE_KW_REQUIRE:
            // TODO:
            break;

        // <ret_n> -> eps
        case TYPE_IDENTIFIER:
            // TODO:
            break;

        // <ret_n> -> eps
        case TYPE_KW_RETURN:
            // TODO:
            break;

        // <ret_n> -> eps
        case TYPE_KW_GLOBAL:
            // TODO:
            break;

        // <ret_n> -> eps
        case TYPE_KW_FUNCTION:
            // TODO:
            break;

        // <ret_n> -> eps
        case TYPE_KW_LOCAL:
            // TODO:
            break;

        // <ret_n> -> eps
        case TYPE_KW_IF:
            // TODO:
            break;

        // <ret_n> -> eps
        case TYPE_KW_WHILE:
            // TODO:
            break;

        // <ret_n> -> eps
        case TYPE_KW_REPEAT:
            // TODO:
            break;

        // <ret_n> -> eps
        case TYPE_KW_FOR:
            // TODO:
            break;

        // <ret_n> -> eps
        case TYPE_KW_END:
            // TODO:
            break;

        // <ret_n> -> eps
        case TYPE_KW_BREAK:
            // TODO:
            break;

        // <ret_n> -> <d_type> <ret_n>
        case TYPE_COMMA:
            // TODO:
            break;

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

    switch((*token)->type)
    {
        // <args> -> id : <d_type> <args_n>
        case TYPE_IDENTIFIER:
            // TODO:
            break;

        // <args> -> eps
        case TYPE_RIGHT_PARENTHESES:
            // TODO:
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

    switch((*token)->type)
    {
        // <code> -> id <func_or_assign> <code>
        case TYPE_IDENTIFIER:
            // TODO:
            break;

        // <code> -> return <ret_exp>
        case TYPE_KW_RETURN:
            // TODO:
            break;

        // <code> -> <var_init> <code>
        case TYPE_KW_LOCAL:
            // TODO:
            break;

        // <code> -> <if_block> <code>
        case TYPE_KW_IF:
            // TODO:
            break;

        // <code> -> eps
        case TYPE_KW_ELSEIF:
            // TODO:
            break;

        // <code> -> eps
        case TYPE_KW_ELSE:
            // TODO:
            break;

        // <code> -> <while_block> <code>
        case TYPE_KW_WHILE:
            // TODO:
            break;

        // <code> -> <repeat_block> <code>
        case TYPE_KW_REPEAT:
            // TODO:
            break;

        // <code> -> eps
        case TYPE_KW_UNTIL:
            // TODO:
            break;

        // <code> -> <for_block> <code>
        case TYPE_KW_FOR:
            // TODO:
            break;

        // <code> -> eps
        case TYPE_KW_END:
            // TODO:
            break;

        // <code> -> break
        case TYPE_KW_BREAK:
            // TODO:
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

    switch((*token)->type)
    {
        // <args_n> -> eps
        case TYPE_RIGHT_PARENTHESES:
            // TODO:
            break;

        // <args_n> -> id : <d_type> <args_n>
        case TYPE_COMMA:
            // TODO:
            break;

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

    switch((*token)->type)
    {
        // <var_init_assign> -> eps
        case TYPE_EOF:
            token_delete(token);
            break;

        // <var_init_assign> -> eps
        case TYPE_KW_REQUIRE:
            // TODO:
            break;

        // <var_init_assign> -> eps
        case TYPE_IDENTIFIER:
            // TODO:
            break;

        // <var_init_assign> -> eps
        case TYPE_KW_RETURN:
            // TODO:
            break;

        // <var_init_assign> -> eps
        case TYPE_KW_GLOBAL:
            // TODO:
            break;

        // <var_init_assign> -> eps
        case TYPE_KW_FUNCTION:
            // TODO:
            break;

        // <var_init_assign> -> eps
        case TYPE_KW_LOCAL:
            // TODO:
            break;

        // <var_init_assign> -> eps
        case TYPE_KW_IF:
            // TODO:
            break;

        // <var_init_assign> -> eps
        case TYPE_KW_ELSEIF:
            // TODO:
            break;

        // <var_init_assign> -> eps
        case TYPE_KW_ELSE:
            // TODO:
            break;

        // <var_init_assign> -> eps
        case TYPE_KW_WHILE:
            // TODO:
            break;

        // <var_init_assign> -> eps
        case TYPE_KW_REPEAT:
            // TODO:
            break;

        // <var_init_assign> -> eps
        case TYPE_KW_UNTIL:
            // TODO:
            break;

        // <var_init_assign> -> eps
        case TYPE_KW_FOR:
            // TODO:
            break;

        // <var_init_assign> -> eps
        case TYPE_KW_END:
            // TODO:
            break;

        // <var_init_assign> -> eps
        case TYPE_KW_BREAK:
            // TODO:
            break;

        default:
            res = ERR_SYNTAX;
            break;
    }
    return res;
}
int expression(token_t **token)
{
    //TODO:
}
int elif_block(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch((*token)->type)
    {
        // <elif_block> -> elseif <expression> do <code> end
        case TYPE_KW_ELSEIF:
            // TODO:
            break;

        // <elif_block> -> eps
        case TYPE_KW_ELSE:
            // TODO:
            break;

        // <elif_block> -> eps
        case TYPE_KW_END:
            // TODO:
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

    switch((*token)->type)
    {
        // <else_block> -> else <code>
        case TYPE_KW_ELSE:
            // TODO:
            break;

        // <else_block> -> eps
        case TYPE_KW_END:
            // TODO:
            break;

        default:
            res = ERR_SYNTAX;
            break;
    }
    return res;
}
int exp_opt(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch((*token)->type)
    {
        // <exp_opt> -> eps
        case TYPE_KW_DO:
            // TODO:
            break;

        // <exp_opt> -> "integer"
        case TYPE_COMMA:
            // TODO:
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

    switch((*token)->type)
    {
        // <func_args> -> <expression> <func_args_n>
        case TYPE_IDENTIFIER:
            // TODO:
            break;

        // <func_args> -> <expression> <func_args_n>
        case TYPE_LEFT_PARENTHESES:
            // TODO:
            break;

        // <func_args> -> eps
        case TYPE_RIGHT_PARENTHESES:
            // TODO:
            break;

        // <func_args> -> <expression> <func_args_n>
        case TYPE_STRING:
            //TODO:
            break;

        // <func_args> -> <expression> <func_args_n>
        case TYPE_NUMBER:
            // TODO:
            break;

        // <func_args> -> <expression> <func_args_n>
        case TYPE_INTEGER:
            // TODO:
            break;

        // <func_args> -> <expression> <func_args_n>
        case TYPE_KW_NIL:
            // TODO:
            break;

        // <func_args> -> <expression> <func_args_n>
        case TYPE_STRLEN:
            // TODO:
            break;

        // <func_args> -> <expression> <func_args_n>
        case TYPE_KW_TRUE:
            // TODO:
            break;

        // <func_args> -> <expression> <func_args_n>
        case TYPE_KW_FALSE:
            // TODO:
            break;

        // <func_args> -> <expression> <func_args_n>
        case TYPE_KW_NOT:
            // TODO:
            break;

        // <func_args> -> <expression> <func_args_n>
        case TYPE_CONCAT:
            // TODO:
            break;

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

    switch((*token)->type)
    {
        // <multi_var> -> eps
        case TYPE_EQ:
            //TODO:
            break;

        // <multi_var> -> id <multi_var>
        case TYPE_COMMA:
            //TODO:
            break;

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

    switch((*token)->type)
    {
        // <multi_exp> -> eps
        case TYPE_EOF:
            token_delete(token);
            break;

        // <multi_exp> -> eps
        case TYPE_KW_REQUIRE:
            // TODO:
            break;

        // <multi_exp> -> eps
        case TYPE_IDENTIFIER:
            // TODO:
            break;

        // <multi_exp> -> eps
        case TYPE_KW_RETURN:
            // TODO:
            break;

        // <multi_exp> -> eps
        case TYPE_KW_GLOBAL:
            // TODO:
            break;

        // <multi_exp> -> eps
        case TYPE_KW_FUNCTION:
            // TODO:
            break;

        // <multi_exp> -> eps
        case TYPE_KW_LOCAL:
            // TODO:
            break;

        // <multi_exp> -> eps
        case TYPE_KW_IF:
            // TODO:
            break;

        // <multi_exp> -> eps
        case TYPE_KW_ELSEIF:
            // TODO:
            break;

        // <multi_exp> -> eps
        case TYPE_KW_ELSE:
            // TODO:
            break;

        // <multi_exp> -> eps
        case TYPE_KW_WHILE:
            // TODO:
            break;

        // <multi_exp> -> eps
        case TYPE_KW_REPEAT:
            // TODO:
            break;

        // <multi_exp> -> eps
        case TYPE_KW_UNTIL:
            // TODO:
            break;

        // <multi_exp> -> eps
        case TYPE_KW_FOR:
            // TODO:
            break;

        // <multi_exp> -> , <expression> <multi_exp>
        case TYPE_COMMA:
            // TODO:
            break;

        // <multi_exp> -> eps
        case TYPE_KW_END:
            // TODO:
            break;

        // <multi_exp> -> eps
        case TYPE_KW_BREAK:
            // TODO:
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

    switch((*token)->type)
    {
        // <func_args_n> -> eps
        case TYPE_RIGHT_PARENTHESES:
            // TODO:
            break;

        // <func_args_n> -> , <expression> <func_args_n>
        case TYPE_COMMA:
            // TODO:
            break;

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

    switch((*token)->type)
    {
        // <ret_exp_n> -> eps
        case TYPE_EOF:
            token_delete(token);
            break;

        // <ret_exp_n> -> eps
        case TYPE_KW_ELSEIF:
            // TODO:
            break;

        // <ret_exp_n> -> eps
        case TYPE_KW_ELSE:
            // TODO:
            break;

        // <ret_exp_n> -> eps
        case TYPE_KW_UNTIL:
            // TODO:
            break;

        // <ret_exp_n> -> , <expression> <ret_exp_n>
        case TYPE_COMMA:
            // TODO:
            break;

        // <ret_exp_n> -> eps
        case TYPE_KW_END:
            // TODO:
            break;

        default:
            res = ERR_SYNTAX;
            break;
    }
    return res;
}

int syntax_analysis(FILE *f)
{
    int res;
    token_t *token = NULL;
    
    // inicializace globalni TS a zasobniku TS
    if ( (res = tree_init(&global_tree))   != NO_ERR)  goto tree_init_error;
    if ( (res = stack_init(&global_stack)) != NO_ERR)  goto post_tree_error;
    // TODO: generate code init

    // poslani prvniho tokenu do <prg>
    res = get_token(f, &token);
    if (res != NO_ERR)   goto post_stack_error;

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