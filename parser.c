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

    //TODO: EXPRESSION
    switch((*token)->type)
    {
        // <prg> -> eps
        // <ret_exp> -> eps
        // <ret> -> eps
        // <ret_n> -> eps
        // <var_init_assign> -> eps
        // <multi_exp> -> eps
        // <ret_exp_n> -> eps
        case TYPE_EOF:
            token_delete(token);
            break;

        // <prg> -> require "string" <prg>
        // <ret> -> eps
        // <ret_n> -> eps
        // <var_init_assign> -> eps
        // <multi_exp> -> eps
        case TYPE_KW_REQUIRE:
            // TODO:
            break;

        // <prg> -> id <func_or_assign> <prg>
        // <ret_exp> -> <expression> <ret_exp_n>
        // <ret> -> eps
        // <ret_n> -> eps
        // <args> -> id : <d_type> <args_n>
        // <code> -> id <func_or_assign> <code>
        // <var_init_assign> -> eps
        // <func_args> -> <expression> <func_args_n>
        // <multi_exp> -> eps
        case TYPE_IDENTIFIER:
            // TODO:
            break;

        // <prg> -> return <ret_exp>>
        // <ret> -> eps
        // <ret_n> -> eps
        // <code> -> return <ret_exp>
        // <var_init_assign> -> eps
        // <multi_exp> -> eps
        case TYPE_KW_RETURN:
            // TODO:
            break;

        // <prg> -> <func_dec> <prg>
        // <func_dec> -> global id : function ( <parm> ) : <ret>
        // <ret> -> eps
        // <ret_n> -> eps
        // <var_init_assign> -> eps
        // <multi_exp> -> eps
        case TYPE_KW_GLOBAL:
            // TODO:
            break;

        // <prg> -> <func_def> <prg>
        // <func_def> -> function id ( <args> ) : <ret> <code> end
        // <ret> -> eps
        // <ret_n> -> eps
        // <var_init_assign> -> eps
        // <multi_exp> -> eps
        case TYPE_KW_FUNCTION:
            // TODO:
            break;

        // <prg> -> <var_init> <prg>
        // <var_init> -> local id : <d_type> <var_init_assign>
        // <ret> -> eps
        // <ret_n> -> eps
        // <code> -> <var_init> <code>
        // <var_init_assign> -> eps
        // <multi_exp> -> eps
        case TYPE_KW_LOCAL:
            // TODO:
            break;

        // <prg> -> <if_block> <prg>
        // <if_block> -> if <expression> then <code > <elif_block> <else_block> end
        // <ret> -> eps
        // <ret_n> -> eps
        // <code> -> <if_block> <code>
        // <var_init_assign> -> eps
        // <multi_exp> -> eps
        case TYPE_KW_IF:
            // TODO:
            break;

        // <ret_exp> -> eps
        // <code> -> eps
        // <var_init_assign> -> eps
        // <elif_block> -> elseif <expression> do <code> end
        // <multi_exp> -> eps
        // <ret_exp_n> -> eps
        case TYPE_KW_ELSEIF:
            // TODO:
            break;

        // <ret_exp> -> eps
        // <code> -> eps
        // <var_init_assign> -> eps
        // <elif_block> -> eps
        // <else_block> -> else <code>
        // <multi_exp> -> eps
        // <ret_exp_n> -> eps
        case TYPE_KW_ELSE:
            // TODO:
            break;

        // <prg> -> <while_block> <prg>
        // <while_block> -> while <expression> do <code> end
        // <ret> -> eps
        // <ret_n> -> eps
        // <code> -> <while_block> <code>
        // <var_init_assign> -> eps
        // <multi_exp> -> eps
        case TYPE_KW_WHILE:
            // TODO:
            break;

        // <prg> -> <repeat_block> <prg>
        // <repeat_block> -> repeat <code> until ( <expresssion )
        // <ret> -> eps
        // <ret_n> -> eps
        // <code> -> <repeat_block> <code>
        // <var_init_assign> -> eps
        // <multi_exp> -> eps
        case TYPE_KW_REPEAT:
            // TODO:
            break;

        // <ret_exp> -> eps
        // <code> -> eps
        // <var_init_assign> -> eps
        // <multi_exp> -> eps
        // <ret_exp_n> -> eps
        case TYPE_KW_UNTIL:
            // TODO:
            break;

        // <prg> -> <for_block> <prg>
        // <for_block> -> for <expression> , <expression> <exp_opt> do <code> end
        // <ret> -> eps
        // <ret_n> -> eps
        // <code> -> <for_block> <code>
        // <var_init_assign> -> eps
        // <multi_exp> -> eps
        case TYPE_KW_FOR:
            // TODO:
            break;

        // <func_or_assign> -> ( <func_args> )
        // <ret_exp> -> <expression> <ret_exp_n>
        // <func_args> -> <expression> <func_args_n>
        case TYPE_LEFT_PARENTHESES:
            // TODO:
            break;

        // <parm> -> eps
        // <parm_n> -> eps
        // <args> -> eps
        // <args_n> -> eps
        // <func_args_n> -> eps
        case TYPE_RIGHT_PARENTHESES:
            // TODO:
            break;

        // <func_or_assign> -> <multi_var> = <expression> <multi_exp>
        // <parm_n> -> <d_type> <parm_n>
        // <ret_n> -> <d_type> <ret_n>
        // <args_n> -> id : <d_type> <args_n>
        // <exp_opt> -> "integer"
        // <multi_var> -> id <multi_var>
        // <multi_exp> -> , <expression> <multi_exp>
        // <func_args_n> -> , <expression> <func_args_n>
        // <ret_exp_n> -> , <expression> <ret_exp_n>
        case TYPE_COMMA:
            // TODO:
            break;

        // <func_or_assign> -> <multi_var> = <expression> <multi_exp>
        // <multi_var> -> eps
        case TYPE_EQ:
            //TOOD:
            break;

        // <ret_exp> -> <expression> <ret_exp_n>
        // <func_args> -> <expression> <func_args_n>
        case TYPE_STRING:
            //TODO:
            break;

        // <ret_exp> -> <expression> <ret_exp_n>
        // <func_args> -> <expression> <func_args_n>
        case TYPE_NUMBER:
            // TODO:
            break;

        // <ret_exp> -> <expression> <ret_exp_n>
        // <func_args> -> <expression> <func_args_n>
        case TYPE_INTEGER:
            // TODO:
            break;

        // <ret_exp> -> <expression> <ret_exp_n>
        // <func_args> -> <expression> <func_args_n>
        case TYPE_KW_NIL:
            // TODO:
            break;

        // <ret_exp> -> <expression> <ret_exp_n>
        // <func_args> -> <expression> <func_args_n>
        case TYPE_HASH:
            // TODO:
            break;

        // <ret_exp> -> <expression> <ret_exp_n>
        // <func_args> -> <expression> <func_args_n>
        case TYPE_KW_TRUE:
            // TODO:
            break;

        // <ret_exp> -> <expression> <ret_exp_n>
        // <func_args> -> <expression> <func_args_n>
        case TYPE_KW_FALSE:
            // TODO:
            break;

        // <ret_exp> -> <expression> <ret_exp_n>
        // <func_args> -> <expression> <func_args_n>
        case TYPE_KW_NOT:
            // TODO:
            break;

        // <ret_exp> -> <expression> <ret_exp_n>
        // <func_args> -> <expression> <func_args_n>
        case TYPE_CONCAT:
            // TODO:
            break;

        // <ret_exp> -> eps
        // <ret> -> eps
        // <ret_n> -> eps
        // <code> -> eps
        // <var_init_assign> -> eps
        // <elif_block> -> eps
        // <else_block> -> eps
        // <multi_exp> -> eps
        // <ret_exp_n> -> eps
        case TYPE_KW_END:
            // TODO:
            break;

        // <parm> -> <d_type> <parm_n>
        // <ret> -> <d_type> <ret_n>
        // <d_type> -> string
        case TYPE_KW_STRING:
            // TODO:
            break;

        // <parm> -> <d_type> <parm_n>
        // <ret> -> <d_type> <ret_n>
        // <d_type> -> integer
        case TYPE_KW_INTEGER:
            // TODO:
            break;

        // <parm> -> <d_type> <parm_n>
        // <ret> -> <d_type> <ret_n>
        // <d_type> -> number
        case TYPE_KW_NUMBER:
            // TODO:
            break;

        // <parm> -> <d_type> <parm_n>
        // <ret> -> <d_type> <ret_n>
        // <d_type> -> boolean
        case TYPE_KW_BOOLEAN:
            // TODO:
            break;

        // <ret> -> eps
        // <ret_n> -> eps
        // <code> -> break
        // <var_init_assign> -> eps
        // <multi_exp> -> eps
        case TYPE_KW_BREAK:
            // TODO:
            break;

        // <exp_opt> -> eps
        case TYPE_KW_DO:
            // TODO:
            break;

        default:
            token_delete(token);
            res = ERR_SYNTAX;
    }
    return res;
}

int func_dec(token_t **token);
int func_def(token_t **token);
int if_block(token_t **token);
int while_block(token_t **token);
int repeat_block(token_t **token);
int for_block(token_t **token);
int var_init(token_t **token);
int func_or_assign(token_t **token);
int ret_exp(token_t **token);
int parm(token_t **token);
int ret(token_t **token);
int d_type(token_t **token);
int parm_n(token_t **token);
int ret_n(token_t **token);
int args(token_t **token);
int code(token_t **token);
int args_n(token_t **token);
int var_init_assign(token_t **token);
int expression(token_t **token);
int elif_block(token_t **token);
int else_block(token_t **token);
int exp_opt(token_t **token);
int func_args(token_t **token);
int multi_var(token_t **token);
int multi_exp(token_t **token);
int func_args_n(token_t **token);
int ret_exp_n(token_t **token);

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
