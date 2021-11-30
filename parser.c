#include "expression.h"
#include "scanner.h"
#include "parser.h"
#include "symtable.h"
#include "stack.h"
#include "token.h"
#include "error.h"

#define CHECK_TOKEN(token, my_type)        \
    do                                     \
    {                                      \
        if ((*token)->type == my_type)     \
            get_token(global_file, token); \
        else                               \
            return ERR_SYNTAX;             \
    } while (0)


int prg(token_t **token)
{
    int res = NO_ERR;
    node_ptr node = NULL;
    list_t lof_data;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <prg> -> eof <prg>
    case TYPE_EOF:
        token_delete(token);
        break;

    // <prg> -> require "string" <prg>
    case TYPE_KW_REQUIRE:
        token_delete(token);
        get_token(global_file, token);

        // "string"
        CHECK_TOKEN(token, TYPE_STRING);

        return prg(token);

    // <prg> -> global id : function ( <parm> ) <ret> <prg>
    case TYPE_KW_GLOBAL:
        token_delete(token);
        get_token(global_file, token);

        CHECK_TOKEN(token, TYPE_IDENTIFIER);
        INSERT_SYMBOL((*token)->value.str_val, FUNC);
        SEARCH_SYMBOL((*token)->value.str_val, node, ERR_SEM_FUNC);

        CHECK_TOKEN(token, TYPE_DECLARE);

        CHECK_TOKEN(token, TYPE_KW_FUNCTION);

        CHECK_TOKEN(token, TYPE_LEFT_PARENTHESES);
        list_init(&lof_data);

        res = parm(token, &lof_data);
        if (res != NO_ERR)
            return res;

        // TODO: add parameters to SYMTABLE

        CHECK_TOKEN(token, TYPE_RIGHT_PARENTHESES);

        list_init(&lof_data);
        res = ret(token, &lof_data);
        if (res != NO_ERR)
            return res;

        // TODO: add return types to SYMTABLE   

        list_destroy(&lof_data);
        return prg(token);

        // <prg> -> id ( <lof_e> ) <prg>
    case TYPE_IDENTIFIER:
        token_delete(token);
        get_token(global_file, token);

        SEARCH_SYMBOL((*token)->value.str_val, node, ERR_SEM_FUNC);
        CHECK_TOKEN(token, TYPE_LEFT_PARENTHESES);

        list_t lof_e_data;
        list_init(&lof_e_data);
        res = lof_e(token, &lof_e_data);
        if (res != NO_ERR)
            return res;

        // TODO: check whether parameters match

        list_destroy(&lof_e_data);
        CHECK_TOKEN(token, TYPE_RIGHT_PARENTHESES);

        return prg(token);

    // <prg> -> function id ( <def_parm> ) <ret> <code> end <prg>
    case TYPE_KW_FUNCTION:

        CHECK_TOKEN(token, TYPE_IDENTIFIER);
        SEARCH_SYMBOL((*token)->value.str_val, node, ERR_SEM_FUNC);

        CHECK_TOKEN(token, TYPE_LEFT_PARENTHESES);

        list_init(&lof_data);
        res = def_parm(token, &lof_data);
        if (res != NO_ERR)
            return res;

        // TODO: check params
        
        list_destroy(&lof_data);
        CHECK_TOKEN(token, TYPE_RIGHT_PARENTHESES);
        
        list_init(&lof_data);
        res = ret(token, &lof_data);
        if (res != NO_ERR)
            return res;

        // TODO: check whether ret types match
        list_destroy(&lof_data);

        list_init(&lof_data);
        res = code(token, &lof_data); 
        if (res != NO_ERR)
            return res;

        // TODO: check whether ret types match
        list_destroy(&lof_data);

        CHECK_TOKEN(token, TYPE_KW_END);

        return prg(token);

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int lof_e(token_t **token, list_t *lof_data)
{
    int res = NO_ERR;
    enum data_type data_t;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <lof_e> -> <expression> <lof_e_n>
    case TYPE_STRING:
    case TYPE_LEFT_PARENTHESES:
    case TYPE_BOOLEAN:
    case TYPE_INTEGER:
    case TYPE_KW_NIL:
    case TYPE_NUMBER:
    case TYPE_IDENTIFIER:

        res = expression(token, &data_t);
        if (res != NO_ERR)
            return res;
        list_insert(lof_data, (void *)&data_t);

        return lof_e_n(token, lof_data); 

    // <lof_e> -> eps
        //TODO:
        break;

    // <lof_e> -> eps
    case TYPE_KW_END:
    case TYPE_RIGHT_PARENTHESES:
    case TYPE_KW_LOCAL:
    case TYPE_KW_IF:
    case TYPE_KW_WHILE:
    case TYPE_KW_RETURN:
    case TYPE_KW_ELSE:
        break;

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int lof_e_n(token_t **token, list_t *lof_data)
{
    int res = NO_ERR;
    enum data_type data_t;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <lof_e_n> -> eps
    case TYPE_IDENTIFIER:
    case TYPE_RIGHT_PARENTHESES:
    case TYPE_KW_END:
    case TYPE_KW_LOCAL:
    case TYPE_KW_IF:
    case TYPE_KW_WHILE:
    case TYPE_KW_RETURN:
    case TYPE_KW_ELSE:

    // <lof_e_n> -> , <expression> <lof_e_n>
    case TYPE_COMMA:

        res = expression(token, &data_t);
        if (res != NO_ERR)
            return res;
        list_insert(lof_data, (void *)&data_t);

        return lof_e_n(token, lof_data);

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int parm(token_t **token, list_t *lof_data)
{
    int res = NO_ERR;
    enum data_type data_t;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <parm> -> eps
    case TYPE_RIGHT_PARENTHESES:
        break;

    // <parm> -> <d_type> <parm_n>
    case TYPE_KW_STRING:
    case TYPE_KW_INTEGER:
    case TYPE_KW_NUMBER:
    case TYPE_KW_BOOLEAN:

        res = d_type(token, &data_t);
        if (res != NO_ERR)
            return res;
        list_insert(lof_data, (void *)&data_t);

        return parm_n(token, lof_data);

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int parm_n(token_t **token, list_t *lof_data)
{
    int res = NO_ERR;
    enum data_type data_t;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <parm_n> -> , <d_type> <parm_n>
    case TYPE_COMMA:

        res = d_type(token, &data_t);
        if (res != NO_ERR)
            return res;
        list_insert(lof_data, (void *)&data_t);

        return parm_n(token, lof_data);

    // <parm_n> -> eps
    case TYPE_EOF:
    case TYPE_KW_REQUIRE:
    case TYPE_KW_GLOBAL:
    case TYPE_IDENTIFIER:
    case TYPE_KW_FUNCTION:
    case TYPE_RIGHT_PARENTHESES:
    case TYPE_KW_END:
    case TYPE_KW_LOCAL:
    case TYPE_KW_IF:
    case TYPE_KW_WHILE:
    case TYPE_KW_RETURN:
        break;

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int ret(token_t **token, list_t *lof_data)
{
    int res = NO_ERR;
    enum data_type data_t;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <ret> -> eps
    case TYPE_EOF:
    case TYPE_KW_REQUIRE:
    case TYPE_KW_GLOBAL:
    case TYPE_IDENTIFIER:
    case TYPE_KW_FUNCTION:
    case TYPE_KW_END:
    case TYPE_KW_LOCAL:
    case TYPE_KW_IF:
    case TYPE_KW_WHILE:
    case TYPE_KW_RETURN:
        break;

    // <ret> -> : <d_type> <parm_n>
    case TYPE_DECLARE:

        res = d_type(token, &data_t);
        if (res != NO_ERR)
            return res;
        list_insert(lof_data, (void *)&data_t);

        return parm_n(token, lof_data);

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int def_parm(token_t **token, list_t *lof_data)
{
    int res = NO_ERR;
    enum data_type data_t;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <def_parm> -> id : <d_type> <def_parm_n>
    case TYPE_IDENTIFIER:
        token_delete(token);
        get_token(global_file, token);

        CHECK_TOKEN(token, TYPE_DECLARE);

        // TODO: kontrola, zda id existuje

        res = d_type(token, &data_t);
        if (res != NO_ERR)
            return res;
        list_insert(lof_data, (void *)&data_t);

        return def_parm_n(token, lof_data);

    // <def_parm> -> eps
    case TYPE_RIGHT_PARENTHESES:
        break;

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int def_parm_n(token_t **token, list_t *lof_data)
{
    int res = NO_ERR;
    enum data_type data_t;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <def_parm_n> -> eps
    case TYPE_RIGHT_PARENTHESES:
        break;

    // <def_parm_n> -> , id : <d_type> <def_parm_n>
    case TYPE_COMMA:
        token_delete(token);
        get_token(global_file, token);

        CHECK_TOKEN(token, TYPE_IDENTIFIER);

        // TODO: kontrola, zda existuje

        CHECK_TOKEN(token, TYPE_DECLARE);

        res = d_type(token, &data_t);
        if (res != NO_ERR)
            return res;

        list_insert(lof_data, (void *)&data_t);

        return def_parm_n(token, lof_data);

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int expression(token_t **token, enum data_type *data_t)
{
    if ((*token)->type == TYPE_IDENTIFIER)
    {
        // pokud to bude funkce -> syntakticka analyza id ( <lof_e> )
        // jinak se vola precedence(token, data_t)
    }
    else
        return precedence(token, data_t);
}
int code(token_t **token, list_t *lof_data)
{
    int res = NO_ERR;
    enum data_type data_t;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <code> -> id <func_or_assign> <code>
    case TYPE_IDENTIFIER:
        token_delete(token);
        get_token(global_file, token);

        res = func_or_assign(token);
        if (res != NO_ERR)
            return res;

        return code(token, lof_data);

    // <code> -> eps
    case TYPE_KW_END:
        break;

    // <code> -> local id : <d_type> <var_init_assign> <code>
    case TYPE_KW_LOCAL:
        token_delete(token);
        get_token(global_file, token);

        CHECK_TOKEN(token, TYPE_IDENTIFIER);

        // TODO: kontrola, ze neexistuje

        CHECK_TOKEN(token, TYPE_DECLARE);

        res = d_type(token, &data_t);
        if (res != NO_ERR)
            return res;

        // TODO: vlozit do tabulky symbolu

        enum data_type exp_type;
        res = var_init_assign(token, &exp_type);
        if (res != NO_ERR)
            return res;

        // TODO: zkontrolovat, ze datove typy odpovidaji
        // if (exp_type != data_t) -> err type (vyjimka je number/integer)

        return code(token, lof_data);

    // <code> -> if <expression> then <code> <else_block> end <code>
    case TYPE_KW_IF:
        token_delete(token);
        get_token(global_file, token);

        res = expression(token, &data_t);
        if (res != NO_ERR)
            return res;

        CHECK_TOKEN(token, TYPE_KW_THEN);

        res = code(token, lof_data);
        if (res != NO_ERR)
            return res;

        // TODO: zkontrolovat stav listu

        res = else_block(token, lof_data);
        if (res != NO_ERR)
            return res;

        // TODO: zkontrolovat stav listu

        CHECK_TOKEN(token, TYPE_KW_END);

        return code(token, lof_data);

    // <code> -> while <expression> then <code> end <code>
    case TYPE_KW_WHILE:
        token_delete(token);
        get_token(global_file, token);

        res = expression(token, &data_t);
        if (res != NO_ERR)
            return res;

        CHECK_TOKEN(token, TYPE_KW_THEN);

        res = code(token, lof_data);
        if (res != NO_ERR)
            return res;

        // TODO: zkontrolovat stav listu

        CHECK_TOKEN(token, TYPE_KW_END);

        return code(token, lof_data);

    // <code> -> return <lof_e>
    case TYPE_KW_RETURN:
        token_delete(token);
        get_token(global_file, token);

        // TODO: pred return se musi zkontrolovat stav listu
        return lof_e(token, lof_data);

    // <code> -> eps
    case TYPE_KW_ELSE:
        break;

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int var_init_assign(token_t **token, enum data_type *data_t)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <var_init_assign> -> eps
    case TYPE_IDENTIFIER:
    case TYPE_KW_END:
    case TYPE_KW_LOCAL:
    case TYPE_KW_IF:
    case TYPE_KW_WHILE:
    case TYPE_KW_RETURN:
    case TYPE_KW_ELSE:
        *data_t = DATA_NIL;
        break;

    // <var_init_assign> -> = <expression>
    case TYPE_ASSIGN:
        token_delete(token);
        get_token(global_file, token);

        return expression(token, data_t);

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int else_block(token_t **token, list_t *lof_data)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <else_block> -> else <code>
    case TYPE_KW_ELSE:
        token_delete(token);
        get_token(global_file, token);

        return code(token, lof_data);
        // TODO: pokud lof data neni prazdne -> mozna byl zavolan return
        // potreba zkontrolovat i s navestim fce

    // <else_block> -> eps
    case TYPE_KW_END:
        break;

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int func_or_assign(token_t **token, list_t *lof_data)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <func_or_assign> -> ( <lof_e> )
    case TYPE_LEFT_PARENTHESES:
        token_delete(token);
        get_token(global_file, token);

        res = lof_e(token, lof_data);
        if (res != NO_ERR)
            return res;

        CHECK_TOKEN(token, TYPE_RIGHT_PARENTHESES);

        break;

    // <func_or_assign> -> <multi_var> = <multi_e>
    case TYPE_IDENTIFIER:
    case TYPE_KW_END:
    case TYPE_COMMA:
    case TYPE_KW_LOCAL:
    case TYPE_KW_IF:
    case TYPE_KW_WHILE:
    case TYPE_KW_RETURN:
    case TYPE_ASSIGN:
    case TYPE_KW_ELSE:

        res = multi_var(token);
        if (res != NO_ERR)
            return res;

        CHECK_TOKEN(token, TYPE_ASSIGN);

        return multi_e(token, lof_data);

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
    // <multi_var> -> , id <multi_var_n>
    case TYPE_COMMA:
        token_delete(token);
        get_token(global_file, token);

        CHECK_TOKEN(token, TYPE_IDENTIFIER);

        return multi_var_n(token);

    // <multi_var> -> eps
    case TYPE_ASSIGN:
        break;

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int multi_var_n(token_t **token)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <multi_var_n> -> , id <multi_var_n>
    case TYPE_COMMA:
        token_delete(token);
        get_token(global_file, token);

        CHECK_TOKEN(token, TYPE_IDENTIFIER);

        return multi_var_n(token);

    // <multi_var_n> -> eps
    case TYPE_ASSIGN:
        break;

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int multi_e(token_t **token, list_t *lof_data)
{
    int res = NO_ERR;
    enum data_type data_t;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <multi_e> -> <expression> <multi_var_n>
    case TYPE_STRING:
    case TYPE_LEFT_PARENTHESES:
    case TYPE_IDENTIFIER:
    case TYPE_NUMBER:
    case TYPE_INTEGER:
    case TYPE_BOOLEAN:
    case TYPE_KW_NIL:
        res = expression(token, &data_t);
        if (res != NO_ERR)
            return res;
        list_insert(lof_data, (void *)&data_t);

        return multi_e_n(token, lof_data);

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int multi_e_n(token_t **token, list_t *lof_data)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
        // <multi_e_n> -> , id <multi_e_n>
        case TYPE_COMMA:
        token_delete(token);
            get_token(global_file, token);

            CHECK_TOKEN(token, TYPE_IDENTIFIER);

            return multi_e_n(token, lof_data);

        // <multi_e_n> -> eps
        case TYPE_KW_LOCAL:
        case TYPE_KW_IF:
        case TYPE_KW_WHILE:
        case TYPE_KW_ELSE:
        case TYPE_KW_END:
        case TYPE_IDENTIFIER:
            break;

        default:
            res = ERR_SYNTAX;
            break;
    }
    return res;
}

int d_type(token_t **token, enum data_type *data_t)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
        // <d_type> -> string
        case TYPE_KW_STRING:
        token_delete(token);
            get_token(global_file, token);
            *data_t = DATA_STR;
            break;

            // <d_type> -> integer
        case TYPE_KW_INTEGER:
        token_delete(token);
            get_token(global_file, token);
            *data_t = DATA_INT;
            break;

            // <d_type> -> number
        case TYPE_KW_NUMBER:
        token_delete(token);
            get_token(global_file, token);
            *data_t = DATA_NUM;
            break;

            // <d_type> -> boolean
        case TYPE_KW_BOOLEAN:
        token_delete(token);
            get_token(global_file, token);
            *data_t = DATA_BOOL;
            break;

        default:
            res = ERR_SYNTAX;
            break;
    }
    return res;
}

FILE *global_file;
frame_stack local_frame;
node_ptr global_tree;

int syntax_analysis(FILE *file)
{
    int res;
    token_t *token = NULL;
    global_file = file;

    // inicializace globalni TS a zasobniku TS
    if ((res = tree_init(&global_tree)) != NO_ERR)
        goto tree_init_error;
    if ((res = stack_init(&local_stack)) != NO_ERR)
        goto post_tree_error;
    // TODO: generate code init

    // poslani prvniho tokenu do <prg>
    res = get_token(global_file, &token);
    if (res != NO_ERR)
        goto post_local_stack_error;

    res = prg(&token);

    // TODO: destroy generate code
post_local_stack_error:
    stack_destroy(&local_stack, &tree_destroy);
post_tree_error:
    tree_destroy(&global_tree);
tree_init_error:
    return res;
}

/* parser.c */
