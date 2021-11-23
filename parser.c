#include "expression.h"
#include "scanner.h"
#include "parser.h"
#include "symtable.h"
#include "stack.h"
#include "token.h"
#include "error.h"

/**
 * @brief Implementace <prg>
 *
 * @param Posledni nacteny token
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int prg(token_t **token)
{
    // TODO: kontrola existence token a *token atd.
    int res = NO_ERR;
    Istring lof_data;
    node_ptr node = NULL;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <prg> -> eof <prg>
    case TYPE_EOF:
        token_delete(token);
        break;

    // <prg> -> require "string" <prg>
    case TYPE_KW_REQUIRE:
        // "string"
        LOAD_AND_CHECK(token, TYPE_STRING);

        LOAD_TOKEN(token);
        return prg(token);

    // <prg> -> global id : function ( <parm> ) <ret> <prg>
    case TYPE_KW_GLOBAL:
        LOAD_AND_CHECK(token, TYPE_IDENTIFIER);             // id

        INSERT_SYMBOL((*token)->value.str_val, FUNC, node);

        LOAD_TOKEN(token);

        CHECK_AND_LOAD(token, TYPE_DECLARE);                // :

        CHECK_AND_LOAD(token, TYPE_KW_FUNCTION);            // function

        CHECK_AND_LOAD(token, TYPE_LEFT_PARENTHESES);       // (
        // inicializace a pridani datovych typ
        if ((res = string_Init(node->lof_params)) != NO_ERR)    return res;
        if ((res = parm(token, node->lof_params)) != NO_ERR)    return res;

        CHECK_AND_LOAD(token, TYPE_RIGHT_PARENTHESES);      // )

        // inicializace a pridani datovych typ
        if ((res = string_Init(node->lof_rets)) != NO_ERR)      return res;
        if ((res = ret(token, node->lof_rets)) != NO_ERR)       return res;

        // zapis flagu, ze funkce byla jiz deklarovana
        node->is_declared = 1;

        return prg(token);

        // <prg> -> id ( <lof_e> ) <prg>
    case TYPE_IDENTIFIER:
        SEARCH_SYMBOL((*token)->value.str_val, FUNC, node);
        if (node == NULL)   return ERR_SEM_DEF;
        LOAD_TOKEN(token);
        CHECK_AND_LOAD(token, TYPE_LEFT_PARENTHESES);       // (

        // inicializace a pridani datovych typu
        if ((res = string_Init(&lof_data)) != NO_ERR)         goto error;
        if ((res = lof_e(token, &lof_data)) != NO_ERR)        goto error;

        // typova kontrola parametru
        if (lof_type_control(&lof_data, node->lof_params))      { res = ERR_SEM_FUNC; goto error; }

        string_Free(&lof_data);

        CHECK_AND_LOAD(token, TYPE_RIGHT_PARENTHESES);      // )

        // TODO: GEN_CODE(call, NULL, NULL, function_name)
        return prg(token);

    // <prg> -> function id ( <def_parm> ) <ret> <code> end <prg>
    case TYPE_KW_FUNCTION:

        LOAD_AND_CHECK(token, TYPE_IDENTIFIER);             // id
        SEARCH_SYMBOL((*token)->value.str_val, FUNC, node);
        if (node == NULL)   return ERR_SEM_DEF;

        // TODO: GEN_CODE(label, NULL, NULL, function_name)
        // TODO: DEFINOVAT SCOPE??

        LOAD_TOKEN(token);

        if (node == NULL)
            INSERT_SYMBOL((*token)->value.str_val, FUNC, node);
        else
            if (node->is_defined)   return ERR_SEM_DEF;

        CHECK_AND_LOAD(token, TYPE_LEFT_PARENTHESES);       // (

        INIT_SCOPE();   // potreba nacist jmena promennych

        // inicializace a pridani datovych typ
        if (!node->is_declared)
        {
            // <parm>
            if ((res = string_Init(node->lof_params)) != NO_ERR)        return res;
            if ((res = def_parm(token, node->lof_params)) != NO_ERR)    return res;

            CHECK_AND_LOAD(token, TYPE_RIGHT_PARENTHESES);  // )

            // inicializace a pridani datovych typ
            // <ret>
            if ((res = string_Init(node->lof_rets)) != NO_ERR)  return res;
            if ((res = ret(token, node->lof_rets)) != NO_ERR)   return res;
            node->is_declared = 1;
        }
        else    // jiz deklarovana funkce
        {
            // <parm>
            if ((res = string_Init(&lof_data)) != NO_ERR)       goto error;
            if ((res = def_parm(token, &lof_data)) != NO_ERR)   goto error;
            if (string_compare(node->lof_params, &lof_data))    { res = ERR_SEM_DEF; goto error; }
            string_Free(&lof_data);

            CHECK_AND_LOAD(token, TYPE_RIGHT_PARENTHESES);  // )
            // <ret>
            if ((res = string_Init(&lof_data)) != NO_ERR)       goto error;
            if ((res = ret(token, node->lof_rets)) != NO_ERR)   goto error;
            if (string_compare(node->lof_rets, &lof_data))      { res = ERR_SEM_DEF; goto error; }
            string_Free(&lof_data);
        }

        // TODO: pripravit navratove promenne??

        if ((res = code(token, &node)) != NO_ERR)   goto error;

        DESTROY_SCOPE();

        // TODO: check whether ret types match

        string_Free(&lof_data);
        CHECK_AND_LOAD(token, TYPE_KW_END);                 // end
        node->is_defined = 1;

        return prg(token);

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
error:
    string_Free(&lof_data);
    return res;
}

int lof_e(token_t **token, Istring *lof_data)
{
    if (lof_data == NULL)   return ERR_INTERNAL;
    int res = NO_ERR;
    enum data_type data_t = DATA_NIL;
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

        if ((res = expression(token, &data_t)) != NO_ERR)           return res;
        if ((res = string_Add_Char(lof_data, data_t)) != NO_ERR)    return res;

        // TODO: add to passing argumets ???

        return lof_e_n(token, lof_data);

    // <lof_e> -> eps
    case TYPE_KW_END:
    case TYPE_RIGHT_PARENTHESES:
    case TYPE_KW_LOCAL:
    case TYPE_KW_IF:
    case TYPE_KW_WHILE:
    case TYPE_KW_RETURN:
    case TYPE_KW_ELSE:
        if ((res = string_Add_Char(lof_data, DATA_NIL)) != NO_ERR) return res;
        break;

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int lof_e_n(token_t **token, Istring *lof_data)
{
    int res = NO_ERR;
    enum data_type data_t = DATA_NIL;
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
        if ((res = string_Add_Char(lof_data, DATA_NIL)) != NO_ERR)  return res;
        break;

    // <lof_e_n> -> , <expression> <lof_e_n>
    case TYPE_COMMA:
        LOAD_TOKEN(token);

        if ((res = expression(token, &data_t)) != NO_ERR)           return res;
        if ((res = string_Add_Char(lof_data, data_t)) != NO_ERR)    return res;

        // TODO: add to passing argumets ???

        return lof_e_n(token, lof_data);

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int parm(token_t **token, Istring *lof_data)
{
    int res = NO_ERR;
    enum data_type data_t;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <parm> -> eps
    case TYPE_RIGHT_PARENTHESES:
        if ((res = string_Add_Char(lof_data, DATA_NIL)) != NO_ERR) return res;
        break;

    // <parm> -> <d_type> <parm_n>
    case TYPE_KW_STRING:
    case TYPE_KW_INTEGER:
    case TYPE_KW_NUMBER:
    case TYPE_KW_BOOLEAN:

        if ((res = d_type(token, &data_t)) != NO_ERR)               return res;
        if ((res = string_Add_Char(lof_data, data_t)) != NO_ERR)    return res;

        return parm_n(token, lof_data);

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int parm_n(token_t **token, Istring *lof_data)
{
    int res = NO_ERR;
    enum data_type data_t;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <parm_n> -> , <d_type> <parm_n>
    case TYPE_COMMA:
        LOAD_TOKEN(token);

        if ((res = d_type(token, &data_t)) != NO_ERR)               return res;
        if ((res = string_Add_Char(lof_data, data_t)) != NO_ERR)    return res;

        return parm_n(token, lof_data);

    // <parm_n> -> eps
    case TYPE_RIGHT_PARENTHESES:
        if ((res = string_Add_Char(lof_data, DATA_NIL)) != NO_ERR) return res;
        break;

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int ret(token_t **token, Istring *lof_data)
{
    int res = NO_ERR;
    enum data_type data_t = DATA_NIL;
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
        if ((res = string_Add_Char(lof_data, DATA_NIL)) != NO_ERR) return res;
        break;

    // <ret> -> : <d_type> <ret_n>
    case TYPE_DECLARE:
        LOAD_TOKEN(token);

        if ((res = d_type(token, &data_t)) != NO_ERR)               return res;
        if ((res = string_Add_Char(lof_data, data_t)) != NO_ERR)    return res;

        return ret_n(token, lof_data);

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int ret_n(token_t **token, Istring *lof_data)
{
    int res = NO_ERR;
    enum data_type data_t = DATA_NIL;
    INIT_TOKEN(token, res);

    switch((*token)->type)
    {
    // <ret_n> -> eps
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
        if ((res = string_Add_Char(lof_data, DATA_NIL)) != NO_ERR)  return res;
        break;
    // <ret_n> -> , <d_type> <ret_n>
    case TYPE_COMMA:
        LOAD_TOKEN(token);
        if ((res = d_type(token, &data_t)) != NO_ERR)               return res;
        if ((res = string_Add_Char(lof_data, data_t)) != NO_ERR)    return res;

        return ret_n(token, lof_data);

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int def_parm(token_t **token, Istring *lof_data)
{
    int res = NO_ERR;
    enum data_type data_t;
    node_ptr node;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <def_parm> -> id : <d_type> <def_parm_n>
    case TYPE_IDENTIFIER:
        // pridava parametr do TS
        INSERT_SYMBOL((*token)->value.str_val, VAR, node);
        LOAD_TOKEN(token);

        // TODO: neco s GEN_CODE() ??

        CHECK_AND_LOAD(token, TYPE_DECLARE);    // :

        if ((res = d_type(token, &data_t)) != NO_ERR)               return res;
        node->var_type = data_t;
        if ((res = string_Add_Char(lof_data, data_t)) != NO_ERR)    return res;

        return def_parm_n(token, lof_data);

    // <def_parm> -> eps
    case TYPE_RIGHT_PARENTHESES:
        if ((res = string_Add_Char(lof_data, DATA_NIL)) != NO_ERR) return res;
        break;

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int def_parm_n(token_t **token, Istring *lof_data)
{
    int res = NO_ERR;
    enum data_type data_t;
    node_ptr node;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <def_parm_n> -> eps
    case TYPE_RIGHT_PARENTHESES:
        if ((res = string_Add_Char(lof_data, DATA_NIL)) != NO_ERR) return res;
        break;

    // <def_parm_n> -> , id : <d_type> <def_parm_n>
    case TYPE_COMMA:
        // pridava parametr do TS
        LOAD_AND_CHECK(token, TYPE_IDENTIFIER);             // id
        INSERT_SYMBOL((*token)->value.str_val, VAR, node);

        // TODO: neco s GEN_CODE() ??

        LOAD_TOKEN(token);
        CHECK_AND_LOAD(token, TYPE_DECLARE);                // :

        if ((res = d_type(token, &data_t)) != NO_ERR)               return res;
        node->var_type = data_t;
        if ((res = string_Add_Char(lof_data, data_t)) != NO_ERR)    return res;

        return def_parm_n(token, lof_data);

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int code(token_t **token, node_ptr *func_node)
{
    int res = NO_ERR;
    enum data_type data_t = DATA_NIL;
    node_ptr local_node;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <code> -> id <func_or_assign> <code>
    case TYPE_IDENTIFIER:
        // hleda zaznam v TS
        SEARCH_SYMBOL((*token)->value.str_val, FUNC, local_node);
        if (local_node != NULL)
        {
            SEARCH_SYMBOL((*token)->value.str_val, VAR, local_node);
        }
        LOAD_TOKEN(token);

        if ((res = func_or_assign(token, &local_node)) != NO_ERR)   return res;

        return code(token, func_node);

    // <code> -> local id : <d_type> <var_init_assign> <code>
    case TYPE_KW_LOCAL:
        LOAD_AND_CHECK(token, TYPE_IDENTIFIER);             // id
        INSERT_SYMBOL((*token)->value.str_val, VAR, local_node);

        // TODO: add to local frame

        CHECK_AND_LOAD(token, TYPE_DECLARE);                // :

        if ((res = d_type(token, &data_t)) != NO_ERR)   return res;

        local_node->var_type = data_t;

        // TODO: set local variable type

        if ((res = var_init_assign(token, &data_t)) != NO_ERR)    return res;
        // kontrola prirazeni; pokud neprobehne prirazeni, bude se ocekavat v data_t NIL
        if (data_t != DATA_NIL && data_t != local_node->var_type)
            return ERR_SEM_ASSIGN;

        return code(token, func_node);

    // <code> -> if <expression> then <code> <else_block> end <code>
    case TYPE_KW_IF:
        LOAD_TOKEN(token);

        // expression muze byt jakykoliv vyraz -> pokud to neni false nebo NIL
        // pak je to vzdy true; toto je ale reseno az pri generovani kodu
        if ((res = expression(token, &data_t)) != NO_ERR)   return res;

        // TODO: jump to label ELSE if false

        CHECK_AND_LOAD(token, TYPE_KW_THEN);                // then

        INIT_SCOPE();
        // jde hlavne o kontrolu, pokud se zavola return
        if ((res = code(token, func_node)) != NO_ERR)    return res;
        DESTROY_SCOPE();

        // TODO: jump to lable END_IF

        if ((res = else_block(token, func_node)) != NO_ERR)    return res;

        CHECK_AND_LOAD(token, TYPE_KW_END);                 // end

        // TODO: label END

        return code(token, func_node);

    // <code> -> while <expression> do <code> end <code>
    case TYPE_KW_WHILE:
        LOAD_TOKEN(token);

        // expression muze byt jakykoliv vyraz -> pokud to neni false nebo NIL
        // pak je to vzdy true; toto je ale reseno az pri generovani kodu
        if ((res = expression(token, &data_t)) != NO_ERR)   return res;

        // TODO: jump to END_WHILE if false
        // TODO: label while

        CHECK_AND_LOAD(token, TYPE_KW_DO);

        INIT_SCOPE();
        // jde hlavne o kontrolu navratovych hodnot returnu ve scopu
        if ((res = code(token, func_node)) != NO_ERR)    return res;

        // TODO: go thru SYMTABLE and define vars
        // TODO: go thru gen_code_queue and change delete init
        // TODO: flush code
        // TODO: jump to while label

        DESTROY_SCOPE();

        CHECK_AND_LOAD(token, TYPE_KW_END);
        // TODO: create END_WHILE label

        return code(token, func_node);

    // <code> -> return <lof_e>
    case TYPE_KW_RETURN:
        LOAD_TOKEN(token);

        // TODO: pred return se musi zkontrolovat stav listu
        Istring lof_data;
        if ((res = string_Init(&lof_data)) != NO_ERR)   return res;
        if ((res = lof_e(token, &lof_data)) != NO_ERR)
        {
            string_Free(&lof_data);
            return res;
        }

        // TODO: passing variable to return variable ??

        if (lof_type_control(&lof_data, (*func_node)->lof_rets))
            res = ERR_SEM_FUNC;

        string_Free(&lof_data);
        break;

    // <code> -> eps
    case TYPE_KW_END:
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
        LOAD_AND_CHECK(token, TYPE_ASSIGN);     // =
        return fun_or_exp(token, data_t);

        // TODO: somehow return this expression to assign it to the variable !!

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int fun_or_exp(token_t **token, enum data_type *data_t)
{
     int res = NO_ERR;
     node_ptr local_node;
     INIT_TOKEN(token, res);

     switch((*token)->type)
     {
         case TYPE_IDENTIFIER:
             SEARCH_SYMBOL((*token)->value.str_val, FUNC, local_node);
             if (local_node != NULL)
             {
                 // ziska, jakou navratovou hodnotu vraci fcu
                 // pokud funkce nevraci nic, nastavi se DATA_NIL
                 *data_t = local_node->lof_rets != NULL ? 
                     local_node->lof_rets->value[0] : DATA_NIL;
                 LOAD_AND_CHECK(token, TYPE_LEFT_PARENTHESES);      // (
                 LOAD_TOKEN(token);
                 Istring lof_data;

                 if ((res = string_Init(&lof_data)) != NO_ERR)      return res;
                 if ((res = lof_e(token, &lof_data)) != NO_ERR)
                 {
                     string_Free(&lof_data);
                     return res;
                 }

                 // TODO: compare node->lof_params with lof_data

                 // TODO: get list of args and pass it to function
                 string_Free(&lof_data);
                 CHECK_AND_LOAD(token, TYPE_RIGHT_PARENTHESES);     // )

                 // TODO: GEN_CODE(call, NULL, NULL, funtion)
             }
             else
                 return expression(token, data_t);
             break;
         case TYPE_BOOLEAN:
         case TYPE_STRING:
         case TYPE_NUMBER:
         case TYPE_INTEGER:
         case TYPE_LEFT_PARENTHESES:
         case TYPE_KW_NIL:
             return expression(token, data_t);
         default:
             res = ERR_SYNTAX;
             break;
     }
     return res;
}

int else_block(token_t **token, node_ptr *func_node)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <else_block> -> else <code>
    case TYPE_KW_ELSE:
        LOAD_TOKEN(token);

        INIT_SCOPE();
        // TODO: LABEL ELSE
        res = code(token, func_node);
        DESTROY_SCOPE();
        break;
            
    // <else_block> -> eps
    case TYPE_KW_END:
        break;

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int func_or_assign(token_t **token, node_ptr *node)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <func_or_assign> -> ( <lof_e> )
    case TYPE_LEFT_PARENTHESES:
        if (node == NULL)           return ERR_SEM_DEF;
        if (*node == NULL)           return ERR_SEM_DEF;
        if ((*node)->type != FUNC)  return ERR_SEM_DEF;
        LOAD_AND_CHECK(token, TYPE_LEFT_PARENTHESES);       // (
        LOAD_TOKEN(token);

        Istring lof_data;
        if ((res = string_Init(&lof_data)) != NO_ERR)   return res;
        if ((res = lof_e(token, &lof_data)) != NO_ERR)
        {
            string_Free(&lof_data);
            return res;
        }

        if (lof_type_control(&lof_data, (*node)->lof_params))   return ERR_SEM_FUNC;
        CHECK_AND_LOAD(token, TYPE_RIGHT_PARENTHESES);      // )
        string_Free(&lof_data);

        // TODO: somehow call the function

        break;

    // <func_or_assign> -> <multi_var> = id <multi_e>
    case TYPE_COMMA:
    case TYPE_ASSIGN:
        if (node == NULL)           return ERR_SEM_DEF;
        if (*node == NULL)           return ERR_SEM_DEF;
        if ((*node)->type != VAR)   return ERR_SEM_DEF;

        // leva strana vyrazu
        Istring lof_var_type;
        if ((res = string_Init(&lof_var_type)) != NO_ERR)   return res;
        // pridani datoveho typu prvni promenne
        if ((res = string_Add_Char(&lof_var_type, (*node)->var_type)) != NO_ERR)
        {
            string_Free(&lof_var_type);
            return res;
        }

        if ((res = multi_var_n(token, &lof_data)) != NO_ERR)
        {
            string_Free(&lof_var_type);
            return res;
        }

        // navratove typy prave strany vyrazu
        Istring lof_ret_type;
        if ((res = string_Init(&lof_ret_type)) != NO_ERR)   return res;

        CHECK_AND_LOAD(token, TYPE_ASSIGN);         // =

        if ((res = fun_or_multi_e(token, &lof_ret_type)) != NO_ERR)
        {
            string_Free(&lof_var_type);
            string_Free(&lof_ret_type);
            return res;
        }

        // TODO: compare lof_var_type and lof_ret_type

        // TODO: get list of nodes and assign them all ??
        string_Free(&lof_var_type);
        string_Free(&lof_ret_type);
        break;

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int multi_var_n(token_t **token, Istring *lof_data)
{
    int res = NO_ERR;
    node_ptr node = NULL;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <multi_var_n> -> , id <multi_var_n>
    case TYPE_COMMA:
        LOAD_AND_CHECK(token, TYPE_IDENTIFIER);

        // vyhleda, zda byla promenna jiz definovana a pak ji tam prida
        SEARCH_SYMBOL((*token)->value.str_val, VAR, node);
        if (node == NULL)   return ERR_SEM_DEF;
        if ((res = string_Add_Char(lof_data, node->var_type)) != NO_ERR) return res;
        LOAD_TOKEN(token);

        // TODO: add to assign list??

        return multi_var_n(token, lof_data);

    // <multi_var_n> -> eps
    case TYPE_ASSIGN:
        if ((res = string_Add_Char(lof_data, DATA_NIL)) != NO_ERR) return res;
        break;

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int fun_or_multi_e(token_t **token, Istring *lof_data)
{
    int res = NO_ERR;
    enum data_type data_t = DATA_NIL;
    node_ptr node;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
    // <multi_e> -> <expression> <multi_var_n>
    case TYPE_IDENTIFIER:
        SEARCH_SYMBOL((*token)->value.str_val, FUNC, node);
        if (node != NULL)
        {
            LOAD_AND_CHECK(token, TYPE_LEFT_PARENTHESES);
            LOAD_TOKEN(token);
            Istring lof_data;

            if ((res = string_Init(&lof_data)) != NO_ERR)   return res;
            if ((res = lof_e(token, &lof_data)) != NO_ERR)
            {
                string_Free(&lof_data);
                return res;
            }

            // TODO: check lof_data and node->lof_parms

            CHECK_AND_LOAD(token, TYPE_RIGHT_PARENTHESES);
            // TODO: call a function ??
            string_Free(&lof_data);
        }
        else
        {
            if ((res = expression(token, &data_t)) != NO_ERR)   return res;
            // nacte datovy typ do data
            if ((res = string_Add_Char(lof_data, data_t)) != NO_ERR)    return res;

            return multi_e_n(token, lof_data);
        }
        break;
    case TYPE_STRING:
    case TYPE_LEFT_PARENTHESES:
    case TYPE_NUMBER:
    case TYPE_INTEGER:
    case TYPE_BOOLEAN:
    case TYPE_KW_NIL:
        if ((res = expression(token, &data_t)) != NO_ERR)   return res;
        // nacte datovy typ do data
        if ((res = string_Add_Char(lof_data, data_t)) != NO_ERR)    return res;

        return multi_e_n(token, lof_data);

    default:
        res = ERR_SYNTAX;
        break;
    }
    return res;
}

int multi_e_n(token_t **token, Istring *lof_data)
{
    int res = NO_ERR;
    enum data_type data_t = DATA_NIL;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
        // <multi_e_n> -> , <expression> <multi_e_n>
        case TYPE_COMMA:
            LOAD_TOKEN(token);
            if ((res = expression(token, &data_t)) != NO_ERR)       return res;
            if ((res = string_Add_Char(lof_data, data_t)) != NO_ERR)    return res;

            // TODO: add to assignment list??

            return multi_e_n(token, lof_data);

        // <multi_e_n> -> eps
        case TYPE_KW_LOCAL:
        case TYPE_KW_IF:
        case TYPE_KW_WHILE:
        case TYPE_KW_ELSE:
        case TYPE_KW_END:
        case TYPE_IDENTIFIER:
            if ((res = string_Add_Char(lof_data, DATA_NIL)) != NO_ERR)  return res;
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
            LOAD_TOKEN(token);
            *data_t = DATA_STR;
            break;

            // <d_type> -> integer
        case TYPE_KW_INTEGER:
            LOAD_TOKEN(token);
            *data_t = DATA_INT;
            break;

            // <d_type> -> number
        case TYPE_KW_NUMBER:
            LOAD_TOKEN(token);
            *data_t = DATA_NUM;
            break;

            // <d_type> -> boolean
        case TYPE_KW_BOOLEAN:
            LOAD_TOKEN(token);
            *data_t = DATA_BOOL;
            break;

        default:
            *data_t = DATA_NIL;
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

    // TODO: vlozeni vnitrnich funkci do TS

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
