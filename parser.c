#include "expression.h"
#include "parser.h"
#include <stdbool.h>
#include <string.h>

bool contains_require = false;

/**
 * @brief Implementace <prg>
 *
 * @param Posledni nacteny token
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int prg(token_t **token)
{
    if (token == NULL || *token == NULL)    return ERR_INTERNAL;
    int res = NO_ERR;
    Istring lof_data = { 0, };
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
            contains_require = true;
            
            // "string"
            LOAD_AND_CHECK(token, TYPE_STRING);
            
            if (strcmp((*token)->value.str_val, "ifj21") != 0)
                return ERR_SEMANT;

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
            if ((res = string_Init(&node->lof_params)) != NO_ERR)    return res;
            if ((res = parm(token, &node->lof_params)) != NO_ERR)    return res;

            CHECK_AND_LOAD(token, TYPE_RIGHT_PARENTHESES);      // )

            // inicializace a pridani datovych typ
            if ((res = string_Init(&node->lof_rets)) != NO_ERR)      return res;
            if ((res = ret(token, &node->lof_rets, false)) != NO_ERR)       return res;

            // zapis flagu, ze funkce byla jiz deklarovana
            node->is_declared = 1;

            return prg(token);

            // <prg> -> id ( <lof_e> ) <prg>
        case TYPE_IDENTIFIER:
            SEARCH_SYMBOL((*token)->value.str_val, FUNC, node);
            if (node == NULL)   return ERR_SEM_DEF;
            // vytvoreni ramce
            gen_code(&q_identifier, INS_CREATEFRAME, NULL, NULL, NULL);

            LOAD_TOKEN(token);
            CHECK_AND_LOAD(token, TYPE_LEFT_PARENTHESES);       // (
            gen_code(&q_identifier, INS_CLEARS, NULL, NULL, NULL);
            int index = 0;

            // inicializace a pridani datovych typu
            if ((res = lof_e(token, node, &index, 
                            true, &q_identifier)) != NO_ERR)        goto error;

            // pro write je potreba definovat pocet parametru
            if (node->unlim_parms)
            {
                CLEAR_OPERAND(OPERAND_DEST);
                snprintf(_dest, MAX_STR_LEN, "int@%d", index);
                gen_code(&q_identifier, INS_PUSHS, _dest, NULL, NULL);
            }

            // volani funkce node->key
            CHECK_AND_LOAD(token, TYPE_RIGHT_PARENTHESES);      // )
            gen_code(&q_identifier,INS_CALL,node->key,NULL,NULL);

            return prg(token);

            // <prg> -> function id ( <def_parm> ) <ret> <code> end <prg>
        case TYPE_KW_FUNCTION:

            LOAD_AND_CHECK(token, TYPE_IDENTIFIER);             // id
            SEARCH_SYMBOL((*token)->value.str_val, FUNC, node);

            // semanticka kontrola funkce
            if (node == NULL)
                INSERT_SYMBOL((*token)->value.str_val, FUNC, node);
            else
                if (node->is_defined)   return ERR_SEM_DEF;

            // inicializace funkce
            reset_gen_info(node);
            gen_code(NULL, INS_LABEL, (*token)->value.str_val, NULL, NULL);
            gen_code(NULL, INS_PUSHFRAME, NULL, NULL, NULL);
            CLEAR_OPERAND(OPERAND_DEST);
            snprintf(_dest, MAX_STR_LEN, "LF@$%s_tmp1", glob_cnt.func_name);
            gen_code(NULL, INS_DEFVAR, _dest, NULL, NULL);
            CLEAR_OPERAND(OPERAND_DEST);
            snprintf(_dest, MAX_STR_LEN, "LF@$%s_tmp2", glob_cnt.func_name);
            gen_code(NULL, INS_DEFVAR, _dest, NULL, NULL);
            CLEAR_OPERAND(OPERAND_DEST);
            snprintf(_dest, MAX_STR_LEN, "LF@$%s_tmp3", glob_cnt.func_name);
            gen_code(NULL, INS_DEFVAR, _dest, NULL, NULL);

            LOAD_TOKEN(token);

            CHECK_AND_LOAD(token, TYPE_LEFT_PARENTHESES);       // (

            INIT_SCOPE();   // potreba nacist jmena promennych

            // inicializace a pridani datovych typ
            if (!node->is_declared)
            {
                // <parm>
                if ((res = string_Init(&node->lof_params)) != NO_ERR)        return res;
                if ((res = def_parm(token, &node->lof_params)) != NO_ERR)    return res;

                CHECK_AND_LOAD(token, TYPE_RIGHT_PARENTHESES);  // )

                // inicializace a pridani datovych typ
                // <ret>
                if ((res = string_Init(&node->lof_rets)) != NO_ERR)  return res;
                if ((res = ret(token, &node->lof_rets, true)) != NO_ERR)   return res;
                node->is_declared = 1;
            }
            else    // jiz deklarovana funkce
            {
                // <parm>
                if ((res = string_Init(&lof_data)) != NO_ERR)       goto error;
                if ((res = def_parm(token, &lof_data)) != NO_ERR)   goto error;
                // typova kontrola
                if (string_compare(&node->lof_params, &lof_data))    { res = ERR_SEM_DEF; goto error; }
                string_Free(&lof_data);

                CHECK_AND_LOAD(token, TYPE_RIGHT_PARENTHESES);  // )
                // <ret>
                if ((res = string_Init(&lof_data)) != NO_ERR)       goto error;
                if ((res = ret(token, &lof_data, true)) != NO_ERR)   goto error;
                // typova kontrola
                if (string_compare(&node->lof_rets, &lof_data))      { res = ERR_SEM_DEF; goto error; }
                string_Free(&lof_data);
            }

            if ((res = code(token, &node, NULL)) != NO_ERR)   goto error;

            // ukonceni funkce
            CHECK_AND_LOAD(token, TYPE_KW_END);                 // end
            DESTROY_SCOPE();
            gen_code(NULL, INS_POPFRAME, NULL, NULL, NULL);
            gen_code(NULL, INS_RETURN, NULL, NULL, NULL);

            // error nil
            CLEAR_OPERAND(OPERAND_DEST);
            snprintf(_dest, MAX_STR_LEN, "%s&error_nil", glob_cnt.func_name);
            gen_code(NULL, INS_LABEL, _dest, NULL, NULL);
            gen_code(NULL, INS_EXIT, "int@8", NULL, NULL);

            // XXX: error div zero -> je to chyba?
            CLEAR_OPERAND(OPERAND_DEST);
            snprintf(_dest, MAX_STR_LEN, "%s&error_zero", glob_cnt.func_name);
            gen_code(NULL, INS_LABEL, _dest, NULL, NULL);
            gen_code(NULL, INS_EXIT, "int@9", NULL, NULL);
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

// TODO: predavat list ocekavanych parametru + index -> pro parsovani pri volani funkce
// id se bude prirazovat na ramec, semantika bude probihat na teto urovni (nebude se nic vracet)
int lof_e(token_t **token, node_ptr node, int *index, 
        bool is_parm, queue_t *q)
{
    if (node == NULL)   return ERR_INTERNAL;
    int res = NO_ERR;
    exp_nterm_t *final_exp = NULL;
    enum data_type data_t = DATA_NIL;
    Istring *exp = is_parm ? &node->lof_params : &node->lof_rets;
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
        case TYPE_KW_NOT:
        case TYPE_STRLEN:
        case TYPE_IDENTIFIER:

            *index += 1;
            if ((res = expression(token, &data_t, &final_exp)) != NO_ERR)           return res;
            if (!is_parm)
            {
                // typova kontrola
                TYPE_CHECK_AND_CONVERT_NTERM(data_t, exp, *index, final_exp, ERR_SEM_FUNC);

                // push(exp)
                generate_code_nterm(&final_exp,q,&data_t);
                exp_nterm_destroy(&final_exp);  // v budoucnu se muze zmenit

                // konverze
                CONVERT_TO_FLOAT(data_t, (exp->value)[*index - 1]);

                CLEAR_OPERAND(OPERAND_DEST);
                snprintf(_dest, MAX_STR_LEN, "LF@%%retvar$%d", *index);

                // tmp1 = pop()
                POP_TO_TMP(OPERAND_FIRST, _first, q);

                // retvar_$i = tmp1
                gen_code(q, INS_MOVE, _dest, _first, NULL);
                return lof_e_n(token, node, index, is_parm, q);
            }
            else
            {
                // predava se moc argumentu
                if (is_parm && exp->length == 0) return ERR_SEM_FUNC;

                if (node->unlim_parms)
                {
                    if ((res = lof_e_n(token, node, index, is_parm, q)) != NO_ERR)  return res;

                    // push(exp)
                    generate_code_nterm(&final_exp,q,&data_t);
                    exp_nterm_destroy(&final_exp);  // v budoucnu se muze zmenit
                }
                else
                {
                    // typova kontrola a konverze
                    TYPE_CHECK_AND_CONVERT_NTERM(data_t, exp, *index, final_exp, ERR_SEM_FUNC);

                    CLEAR_OPERAND(OPERAND_DEST);
                    snprintf(_dest, MAX_STR_LEN, "TF@%%%d", *index);
                    gen_code(q, INS_DEFVAR, _dest, NULL, NULL);
                    int temp = *index;
                    if ((res = lof_e_n(token, node, index, is_parm, q)) != NO_ERR)  return res;

                    // push(exp)
                    generate_code_nterm(&final_exp,q,&data_t);
                    exp_nterm_destroy(&final_exp);  // v budoucnu se muze zmenit

                    // konverze
                    CONVERT_TO_FLOAT(data_t, (exp->value)[*index - 1]);
                    CLEAR_OPERAND(OPERAND_DEST);
                    snprintf(_dest, MAX_STR_LEN, "TF@%%%d", temp);

                    // tmp1 = pop()
                    POP_TO_TMP(OPERAND_FIRST, _first, q);

                    // retvar_$i = tmp1
                    gen_code(q, INS_MOVE, _dest, _first, NULL);
                }
            }
            break;

            // <lof_e> -> eps
        case TYPE_KW_END:
        case TYPE_RIGHT_PARENTHESES:
        case TYPE_KW_LOCAL:
        case TYPE_KW_IF:
        case TYPE_KW_WHILE:
        case TYPE_KW_RETURN:
        case TYPE_KW_ELSE:
            // predalo male argumentu
            if (!node->unlim_parms && is_parm && exp->length-1 != 0)       return ERR_SEM_FUNC; 
            break;

        default:
            res = ERR_SYNTAX;
            break;
    }
    return res;
}

int lof_e_n(token_t **token, node_ptr node, int *index, 
        bool is_parm, queue_t *q)
{
    if (node == NULL)   return ERR_INTERNAL;
    int res = NO_ERR;
    enum data_type data_t = DATA_NIL;
    exp_nterm_t *final_exp = NULL;
    Istring *exp = is_parm ? &node->lof_params         : &node->lof_rets;
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
            // pokud se ocekava vic parametru
            if (!node->unlim_parms && is_parm && *index != exp->length - 1)       return ERR_SEM_FUNC; 
            break;

            // <lof_e_n> -> , <expression> <lof_e_n>
        case TYPE_COMMA:
            LOAD_TOKEN(token);
            *index += 1;
            if ((res = expression(token, &data_t, &final_exp)) != NO_ERR)           return res;
            if (!is_parm)
            {
                // typova kontrola a konverze
                TYPE_CHECK_AND_CONVERT_NTERM(data_t, exp, *index, final_exp, ERR_SEM_FUNC);

                // push(exp)
                generate_code_nterm(&final_exp,q,&data_t);
                exp_nterm_destroy(&final_exp);  // v budoucnu se muze zmenit

                // konverze
                CONVERT_TO_FLOAT(data_t, (exp->value)[*index - 1]);

                CLEAR_OPERAND(OPERAND_DEST);
                snprintf(_dest, MAX_STR_LEN, "LF@%%retvar$%d", *index);

                // tmp1 = pop()
                POP_TO_TMP(OPERAND_FIRST, _first, q);

                // retvar_$i = tmp1
                gen_code(q, INS_MOVE, _dest, _first, NULL);
                return lof_e_n(token, node, index, is_parm, q);
            }
            else
            {
                // predava se moc argumentu
                if (is_parm && exp->length == 0) return ERR_SEM_FUNC;

                if (node->unlim_parms)
                {
                    if ((res = lof_e_n(token, node, index, is_parm, q)) != NO_ERR)  return res;

                    // push(exp)
                    generate_code_nterm(&final_exp,q,&data_t);
                    exp_nterm_destroy(&final_exp);  // v budoucnu se muze zmenit
                }
                else
                {
                    // typova kontrola a konverze
                    TYPE_CHECK_AND_CONVERT_NTERM(data_t, exp, *index, final_exp, ERR_SEM_FUNC);

                    CLEAR_OPERAND(OPERAND_DEST);
                    snprintf(_dest, MAX_STR_LEN, "TF@%%%d", *index);
                    gen_code(q, INS_DEFVAR, _dest, NULL, NULL);
                    int temp = *index;
                    if ((res = lof_e_n(token, node, index, is_parm, q)) != NO_ERR)  return res;

                    // push(exp)
                    generate_code_nterm(&final_exp,q,&data_t);
                    exp_nterm_destroy(&final_exp);  // v budoucnu se muze zmenit

                    // konverze
                    CONVERT_TO_FLOAT(data_t, (exp->value)[*index - 1]);

                    CLEAR_OPERAND(OPERAND_DEST);
                    snprintf(_dest, MAX_STR_LEN, "TF@%%%d", temp);

                    // tmp1 = pop()
                    POP_TO_TMP(OPERAND_FIRST, _first, q);

                    // retvar_$i = tmp1
                    gen_code(q, INS_MOVE, _dest, _first, NULL);
                }
            }
            break;

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

            return parm_n(token, lof_data, false);

        default:
            res = ERR_SYNTAX;
            break;
    }
    return res;
}

int parm_n(token_t **token, Istring *lof_data, bool def_retvar)
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

            // vytvoreni navratove hodnoty
            if (def_retvar)
            {
                // define retvar_$i
                CLEAR_OPERAND(OPERAND_DEST);
                snprintf(_dest, MAX_STR_LEN, "LF@%%retvar$%d", lof_data->length);
                gen_code(NULL, INS_DEFVAR, _dest, NULL, NULL);

                // retvar_$i = nil
                gen_code(NULL, INS_MOVE, _dest, "nil@nil", NULL);
            }

            return parm_n(token, lof_data, def_retvar);

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

int ret(token_t **token, Istring *lof_data, bool gen_code_print)
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

            if (gen_code_print)
            {
                gen_code(NULL, INS_DEFVAR, "LF@%retvar$1", NULL, NULL);
                gen_code(NULL, INS_MOVE,   "LF@%retvar$1", "nil@nil",NULL);
            }

            return ret_n(token, lof_data, gen_code_print, 2);

        default:
            res = ERR_SYNTAX;
            break;
    }
    return res;
}

int ret_n(token_t **token, Istring *lof_data, bool gen_code_print, int index)
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

            if (gen_code_print)
            {
                CLEAR_OPERAND(OPERAND_DEST);
                snprintf(_dest, MAX_STR_LEN, "LF@%%retvar$%d", index);
                gen_code(NULL, INS_DEFVAR, _dest, NULL, NULL);
                gen_code(NULL, INS_MOVE,   _dest, "nil@nil",NULL);
            }

            return ret_n(token, lof_data, gen_code_print, index+1);
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
    node_ptr node = NULL;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
        // <def_parm> -> id : <d_type> <def_parm_n>
        case TYPE_IDENTIFIER:
            // pridava parametr do TS
            INSERT_SYMBOL((*token)->value.str_val, VAR, node);
            LOAD_TOKEN(token);

            CHECK_AND_LOAD(token, TYPE_DECLARE);    // :

            if ((res = d_type(token, &data_t)) != NO_ERR)               return res;
            node->var_type = data_t;
            node->is_defined = 1;
            node->is_param_var = 1;

            if ((res = string_Add_Char(lof_data, data_t)) != NO_ERR)    return res;

            // param1 = %1
            if ((res = define_variable(FRAME_LF, OPERAND_DEST, node)) != NO_ERR) return res;
            gen_code(NULL, INS_DEFVAR, _dest, NULL, NULL);
            gen_code(NULL, INS_MOVE, _dest, "LF@%1", NULL);

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

            LOAD_TOKEN(token);
            CHECK_AND_LOAD(token, TYPE_DECLARE);                // :

            if ((res = d_type(token, &data_t)) != NO_ERR)               return res;
            node->var_type = data_t;
            node->is_defined = 1;
            node->is_param_var = 1;
            if ((res = string_Add_Char(lof_data, data_t)) != NO_ERR)    return res;

            // param$i = %$i
            if ((res = define_variable(FRAME_LF, OPERAND_DEST, node)) != NO_ERR) return res;
            CLEAR_OPERAND(OPERAND_FIRST);
            snprintf(_first, MAX_STR_LEN, "LF@%%%d", lof_data->length);
            gen_code(NULL, INS_DEFVAR, _dest, NULL, NULL);
            gen_code(NULL, INS_MOVE, _dest, _first, NULL);

            return def_parm_n(token, lof_data);

        default:
            res = ERR_SYNTAX;
            break;
    }
    return res;
}

int code(token_t **token, node_ptr *func_node, queue_t *q)
{
    // TODO: int code(token_t **token, node_ptr *func_node, queue_t *q)

    int res = NO_ERR;
    enum data_type data_t = DATA_NIL;
    node_ptr local_node = NULL;
    exp_nterm_t *final_exp = NULL;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
        // <code> -> id <func_or_assign> <code>
        case TYPE_IDENTIFIER:
            // hleda zaznam v TS
            SEARCH_SYMBOL((*token)->value.str_val, FUNC, local_node);
            if (local_node == NULL)
                SEARCH_SYMBOL((*token)->value.str_val, VAR, local_node);

            LOAD_TOKEN(token);

            if ((res = func_or_assign(token, &local_node, q)) != NO_ERR)   return res;

            return code(token, func_node,q);

            // <code> -> local id : <d_type> <var_init_assign> <code>
        case TYPE_KW_LOCAL:
            LOAD_AND_CHECK(token, TYPE_IDENTIFIER);             // id
            INSERT_SYMBOL((*token)->value.str_val, VAR, local_node);

            // define id
            define_variable(FRAME_LF, OPERAND_DEST, local_node);
            gen_code(NULL, INS_DEFVAR, _dest, NULL, NULL);
            LOAD_TOKEN(token);

            CHECK_AND_LOAD(token, TYPE_DECLARE);                // :

            if ((res = d_type(token, &data_t)) != NO_ERR)   return res;

            local_node->var_type = data_t;

            if ((res = var_init_assign(token, &local_node, q)) != NO_ERR)    return res;

            local_node->is_defined = 1;

            // vlozeni do tabulky symbolu

            return code(token, func_node,q);

            // <code> -> if <expression> then <code> <elseif_block> <else_block> end <code>
        case TYPE_KW_IF:
            LOAD_TOKEN(token);

            // expression muze byt jakykoliv vyraz -> pokud to neni false nebo NIL
            // pak je to vzdy true; toto je ale reseno az pri generovani kodu
            if ((res = expression(token, &data_t, &final_exp)) != NO_ERR)   return res;

            // if label
            int temp = ++glob_cnt.if_i;
            define_label(OPERAND_DEST, LABEL_IF);
            gen_code(q, INS_LABEL, _dest,NULL,NULL);

            // push(exp)
            generate_code_nterm(&final_exp, q, &data_t);
            exp_nterm_destroy(&final_exp);

            // tmp1 = pop()
            POP_TO_TMP(OPERAND_FIRST, _first, q);

            // sprintf(s,"endif_%s_%d",(*func_node)->key,index_if);
            define_label(OPERAND_DEST, LABEL_ENDIF);
            
            gen_code(q, INS_JUMPIFEQ, _dest, _first, "nil@nil");

            CLEAR_OPERAND(OPERAND_SECOND);
            snprintf(_second, MAX_STR_LEN, "LF@$%s_tmp2", glob_cnt.func_name);
            gen_code(q, INS_TYPE, _second, _first, NULL);

            CLEAR_OPERAND(OPERAND_DEST);
            snprintf(_dest, MAX_STR_LEN, "%s_iftrue%d", glob_cnt.func_name, glob_cnt.if_i);
            gen_code(q, INS_JUMPIFNEQ, _dest, _second, "string@bool");

            snprintf(_dest, MAX_STR_LEN, "%s_endif%d", glob_cnt.func_name, glob_cnt.if_i);
            gen_code(q, INS_JUMPIFEQ, _dest, _first, "bool@false");

            snprintf(_dest, MAX_STR_LEN, "%s_iftrue%d", glob_cnt.func_name, glob_cnt.if_i);
            gen_code(q, INS_LABEL, _dest, NULL, NULL);

            CHECK_AND_LOAD(token, TYPE_KW_THEN);                // then
            INIT_SCOPE();
            // jde hlavne o kontrolu, pokud se zavola return
            if ((res = code(token, func_node,q)) != NO_ERR)    return res;
            DESTROY_SCOPE();

            // jmp func_end$i
            CLEAR_OPERAND(OPERAND_DEST);
            snprintf(_dest, MAX_STR_LEN, "%s_end%d", glob_cnt.func_name, temp);
            gen_code(q, INS_JUMP, _dest, NULL, NULL);

            // label func_endif$i
            CLEAR_OPERAND(OPERAND_DEST);
            snprintf(_dest, MAX_STR_LEN, "%s_endif%d", glob_cnt.func_name, temp);
            gen_code(q, INS_LABEL, _dest, NULL, NULL);

            // elseif
            if ((res = elseif_block(token, func_node, q, temp)) != NO_ERR)   return res;

            // else
            if ((res = else_block(token, func_node, q)) != NO_ERR)    return res;

            CHECK_AND_LOAD(token, TYPE_KW_END);                 // end

            // label func_endif$i
            CLEAR_OPERAND(OPERAND_DEST);
            snprintf(_dest, MAX_STR_LEN, "%s_end%d", glob_cnt.func_name, temp);
            gen_code(q, INS_LABEL, _dest, NULL, NULL);

            return code(token, func_node,q);

            // <code> -> while <expression> do <code> end <code>
        case TYPE_KW_WHILE:
            {
                LOAD_TOKEN(token);
                queue_t iq;
                bool is_first_cycle = q == NULL;

                // vytvoreni fronty pokud jeste neexistuje
                if (q == NULL)
                {
                    queue_init(&iq);
                    q = &iq;
                }

                int temp = ++glob_cnt.while_i;
                // expression muze byt jakykoliv vyraz -> pokud to neni false nebo NIL
                // pak je to vzdy true; toto je ale reseno az pri generovani kodu
                if ((res = expression(token, &data_t, &final_exp)) != NO_ERR)   return res;

                define_label(OPERAND_DEST, LABEL_WHILE);
                gen_code(q, INS_LABEL, _dest, NULL, NULL);

                generate_code_nterm(&final_exp,q,&data_t);

                // tmp1 = pop()
                POP_TO_TMP(OPERAND_FIRST, _first, q);

                // skok pri nepravde
                define_label(OPERAND_DEST, LABEL_ENDWHILE);
                gen_code(q, INS_JUMPIFEQ, _dest, _first, "nil@nil");
                gen_code(q, INS_JUMPIFEQ, _dest, _first, "bool@false");

                exp_nterm_destroy(&final_exp);

                CHECK_AND_LOAD(token, TYPE_KW_DO);

                INIT_SCOPE();
                // jde hlavne o kontrolu navratovych hodnot returnu ve scopu
                if ((res = code(token, func_node, q)) != NO_ERR)    return res;

                // TODO: go thru SYMTABLE and define vars
                // TODO: go thru queue_t and change delete init
                // TODO: flush code
                // TODO: jump to while label

                // skok na zacatek cyklu
                CLEAR_OPERAND(OPERAND_DEST);
                snprintf(_dest, MAX_STR_LEN, "%s_while%d", glob_cnt.func_name, temp);
                gen_code(q, INS_JUMP, _dest, NULL, NULL);
                DESTROY_SCOPE();

                CHECK_AND_LOAD(token, TYPE_KW_END);

                // label endwhile
                CLEAR_OPERAND(OPERAND_DEST);
                snprintf(_dest, MAX_STR_LEN, "%s_endwhile%d", glob_cnt.func_name, temp);
                gen_code(q, INS_LABEL, _dest, NULL, NULL);

                filter_defvar(q);
                if (is_first_cycle)
                {
                    queue_flush(q);
                    q = NULL;
                }

                return code(token, func_node,q);
            }

            // <code> -> return <lof_e>
        case TYPE_KW_RETURN:
            LOAD_TOKEN(token);

            // TODO: pred return se musi zkontrolovat stav listu
            int index = 0;
            if ((res = lof_e(token, *func_node, &index, false, q)) != NO_ERR)
                return res;

            // TODO: cisteni?
            if (index > (*func_node)->lof_rets.length)
                return ERR_SEM_FUNC;

            while (index < (*func_node)->lof_rets.length-1)
            {
                CLEAR_OPERAND(OPERAND_DEST);
                snprintf(_dest, MAX_STR_LEN, "LF@%%retvar$%d", ++index);
                gen_code(q, INS_MOVE, _dest, "nil@nil", NULL);
            }
            gen_code(q, INS_POPFRAME, NULL, NULL, NULL);
            gen_code(q, INS_RETURN, NULL, NULL, NULL);
            break;

            // <code> -> eps
        case TYPE_KW_END:
        case TYPE_KW_ELSE:
        case TYPE_KW_ELSEIF:
            break;

        default:
            res = ERR_SYNTAX;
            break;
    }
    return res;
}

int var_init_assign(token_t **token, node_ptr *var_node, queue_t *q)
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
            define_variable(FRAME_LF, OPERAND_DEST, *var_node);
            gen_code(q, INS_MOVE, _dest, "nil@nil", NULL);
            break;

            // <var_init_assign> -> = <fun_or_exp>
        case TYPE_ASSIGN:
            CHECK_AND_LOAD(token, TYPE_ASSIGN);     // =
            if ((res = fun_or_exp(token, var_node, q)) != NO_ERR)    return res;
            break;
        default:
            res = ERR_SYNTAX;
            break;
    }
    return res;
}

int fun_or_exp(token_t **token, node_ptr *var_node, queue_t *q)
{
    int res = NO_ERR;
    node_ptr local_node;
    enum data_type data_t;
    INIT_TOKEN(token, res);

    switch((*token)->type)
    {
        case TYPE_IDENTIFIER:
            SEARCH_SYMBOL((*token)->value.str_val, FUNC, local_node);

            // jedna se o volani funkce
            if (local_node != NULL)
            {
                // ziska, jakou navratovou hodnotu vraci fcu
                // pokud funkce nevraci nic, nastavi se DATA_NIL
                data_t = local_node->lof_rets.value[0];

                // priprava ramce pro volani funkce
                gen_code(q, INS_CREATEFRAME, NULL, NULL, NULL);
                gen_code(q, INS_CLEARS, NULL, NULL, NULL);

                LOAD_AND_CHECK(token, TYPE_LEFT_PARENTHESES);      // (
                LOAD_TOKEN(token);
                int index = 0;

                // kontrola parametru a prirazeni
                if ((res = lof_e(token, local_node, &index, true,NULL)) != NO_ERR)
                    return res;

                // pro write je potreba definovat pocet parametru
                if (local_node->unlim_parms)
                {
                    CLEAR_OPERAND(OPERAND_DEST);
                    snprintf(_dest, MAX_STR_LEN, "int@%d", index);
                    gen_code(q, INS_PUSHS, _dest, NULL, NULL);
                }

                CHECK_AND_LOAD(token, TYPE_RIGHT_PARENTHESES);     // )

                // volani fce
                gen_code(q, INS_CALL, local_node->key, NULL, NULL);

                if (local_node->lof_rets.length < 1)    return ERR_SEM_FUNC;

                // typova kontrola a konverze
                TYPE_CHECK_AND_CONVERT_TERM(data_t, (*var_node)->var_type, ERR_SEM_ASSIGN);
                gen_code(q, INS_PUSHS, "TF@%retvar$1", NULL, NULL);

                CONVERT_TO_FLOAT(data_t, (*var_node)->var_type);

                define_variable(FRAME_LF, OPERAND_DEST, *var_node);
                gen_code(q, INS_POPS, _dest, NULL, NULL);
            }
            else    // jedna se o promennou
            {
                exp_nterm_t *final_exp = NULL;
                if ((res = expression(token, &data_t, &final_exp)) != NO_ERR)
                    return res;

                // typova kontrola a konveerze
                if (type_control(data_t, (*var_node)->var_type))
                {
                    exp_nterm_destroy(&final_exp);
                    return ERR_SEM_ASSIGN;
                }
                // push(exp)
                generate_code_nterm(&final_exp, q, &data_t);
                exp_nterm_destroy(&final_exp);

                CONVERT_TO_FLOAT(data_t, (*var_node)->var_type);

                // tmp1 = pop(); id = tmp1;
                POP_TO_TMP(OPERAND_FIRST, _first, q);
                define_variable(FRAME_LF, OPERAND_DEST, *var_node);
                gen_code(q, INS_MOVE, _dest, _first, NULL);
            }
            break;
        case TYPE_BOOLEAN:
        case TYPE_STRING:
        case TYPE_NUMBER:
        case TYPE_INTEGER:
        case TYPE_LEFT_PARENTHESES:
        case TYPE_KW_NIL:
        case TYPE_KW_NOT:
        case TYPE_STRLEN:
            {
                exp_nterm_t *final_exp = NULL;
                if ((res = expression(token, &data_t, &final_exp)) != NO_ERR)
                    return res;

                // typova kontrola a konveerze
                if (type_control(data_t, (*var_node)->var_type))
                {
                    exp_nterm_destroy(&final_exp);
                    return ERR_SEM_ASSIGN;
                }
                // push(exp)
                generate_code_nterm(&final_exp, q, &data_t);
                exp_nterm_destroy(&final_exp);

                CONVERT_TO_FLOAT(data_t, (*var_node)->var_type);

                // tmp1 = pop(); id = tmp1;
                POP_TO_TMP(OPERAND_FIRST, _first, q);
                define_variable(FRAME_LF, OPERAND_DEST, *var_node);
                gen_code(q, INS_MOVE, _dest, _first, NULL);
            }
            break;
        default:
            res = ERR_SYNTAX;
            break;
    }
    return res;
}

int elseif_block(token_t **token, node_ptr *func_node, queue_t *q, int end_index)
{
    int res = NO_ERR;
    enum data_type data_t;
    exp_nterm_t *final_exp = NULL;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
        // <elseif_block> -> elseif <expression> then <code> <elseif_block>
        case TYPE_KW_ELSEIF:
            LOAD_TOKEN(token);
            int temp = ++glob_cnt.elseif_i;
            define_label(OPERAND_DEST, LABEL_ELSEIF);
            gen_code(q, INS_LABEL, _dest, NULL, NULL);

            if ((res = expression(token, &data_t, &final_exp)) != NO_ERR)   return res;
            generate_code_nterm(&final_exp, NULL, &data_t);
            exp_nterm_destroy(&final_exp);

            POP_TO_TMP(OPERAND_FIRST, _first, q);
            CLEAR_OPERAND(OPERAND_DEST);
            snprintf(_dest, MAX_STR_LEN, "%s_endelseif%d", glob_cnt.func_name, temp);

            gen_code(q, INS_JUMPIFEQ, _dest, _first, "nil@nil");
            gen_code(q, INS_JUMPIFEQ, _dest, _first, "bool@false");

            CHECK_AND_LOAD(token, TYPE_KW_THEN);                // then

            INIT_SCOPE();
            // jde hlavne o kontrolu, pokud se zavola return
            if ((res = code(token, func_node,q)) != NO_ERR)    return res;
            DESTROY_SCOPE();

            // jmp end
            CLEAR_OPERAND(OPERAND_DEST);
            snprintf(_dest, MAX_STR_LEN, "%s_end%d", glob_cnt.func_name, end_index);
            gen_code(q, INS_JUMP, _dest, NULL, NULL);

            // label endelseif
            CLEAR_OPERAND(OPERAND_DEST);
            snprintf(_dest, MAX_STR_LEN, "%s_endelseif%d", glob_cnt.func_name, temp);
            gen_code(q, INS_LABEL, _dest, NULL, NULL);

            return elseif_block(token, func_node, q, end_index);

            // <elseif_block> -> eps
        case TYPE_KW_ELSE:
        case TYPE_KW_END:
            break;

        default:
            res = ERR_SYNTAX;
            break;
    }
    return res;
}

int else_block(token_t **token, node_ptr *func_node, queue_t *q)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);
    switch ((*token)->type)
    {
        // <else_block> -> else <code>
        case TYPE_KW_ELSE:
            LOAD_TOKEN(token);

            INIT_SCOPE();
            glob_cnt.else_i++;
            define_label(OPERAND_DEST, LABEL_ELSE);
            gen_code(q, INS_LABEL, _dest, NULL, NULL);

            res = code(token, func_node, q);
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

int func_or_assign(token_t **token, node_ptr *node, queue_t *q)
{
    int res = NO_ERR;
    INIT_TOKEN(token, res);

    if (node == NULL)           return ERR_INTERNAL;
    if (*node == NULL)          return ERR_SEM_DEF;
    switch ((*token)->type)
    {
        // <func_or_assign> -> ( <lof_e> )
        case TYPE_LEFT_PARENTHESES:
            if ((*node)->type != FUNC)  return ERR_SEM_DEF;
            LOAD_TOKEN(token);

            gen_code(q, INS_CREATEFRAME, NULL, NULL, NULL);
            gen_code(q, INS_CLEARS, NULL, NULL, NULL);

            int index = 0;

            // kontrola a prirazeni parametru
            if ((res = lof_e(token, *node, &index, 1,q)) != NO_ERR)
                return res;

            // pro write je potreba definovat pocet parametru
            if ((*node)->unlim_parms)
            {
                CLEAR_OPERAND(OPERAND_DEST);
                snprintf(_dest, MAX_STR_LEN, "int@%d", index);
                gen_code(q, INS_PUSHS, _dest, NULL, NULL);
            }

            CHECK_AND_LOAD(token, TYPE_RIGHT_PARENTHESES);      // )

            // TODO: somehow call the function
            // GEN_CODE(CALL, NULL, NULL, (*node)->key);
            gen_code(q,INS_CALL,(*node)->key,NULL,NULL);
            break;

            // <func_or_assign> -> <multi_var> = <fun_or_multi_e>
        case TYPE_COMMA:
        case TYPE_ASSIGN:
            if ((*node)->type != VAR)   return ERR_SEM_DEF;

            // TODO: vytvorit list leve strany a list prave strany -> ty se pak postupne priradi
            // leva strana vyrazu
            stack_var_t lof_vars;
            stack_var_init(&lof_vars);

            // pridani prvni promenne
            if ((res = stack_var_push(&lof_vars, *node)) != NO_ERR)
            {
                stack_var_destroy(&lof_vars);
                return res;
            }

            if ((res = multi_var_n(token, &lof_vars, q)) != NO_ERR)
            {
                stack_var_destroy(&lof_vars);
                return res;
            }

            CHECK_AND_LOAD(token, TYPE_ASSIGN);         // =

            if ((res = fun_or_multi_e(token, &lof_vars, q)) != NO_ERR)
            {
                stack_var_destroy(&lof_vars);
                return res;
            }

            // na leve strane je vice promennych nez na prave
            if (!stack_var_isempty(lof_vars))
                res = ERR_SEM_ASSIGN;

            stack_var_destroy(&lof_vars);

            // TODO: get list of nodes and assign them all ??
            break;

        default:
            res = ERR_SYNTAX;
            break;
    }
    return res;
}

int multi_var_n(token_t **token, stack_var_t *lof_vars, queue_t *q)
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
            // pridani promenne do zasobniku
            if ((res = stack_var_push(lof_vars, node)) != NO_ERR) return res;
            LOAD_TOKEN(token);

            // TODO: add to assign list??

            return multi_var_n(token, lof_vars, q);

            // <multi_var_n> -> eps
        case TYPE_ASSIGN:
            break;

        default:
            res = ERR_SYNTAX;
            break;
    }
    return res;
}

int fun_or_multi_e(token_t **token, stack_var_t *lof_vars, queue_t *q)
{
    if (lof_vars == NULL)   return ERR_INTERNAL;
    int res = NO_ERR;
    enum data_type data_t = DATA_NIL;
    exp_nterm_t *final_exp = NULL;
    node_ptr local_node = NULL;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
        // <multi_e> -> <expression> <multi_e_n>
        case TYPE_IDENTIFIER:
            SEARCH_SYMBOL((*token)->value.str_val, FUNC, local_node);
            if (local_node != NULL)
            {
                LOAD_AND_CHECK(token, TYPE_LEFT_PARENTHESES);
                LOAD_TOKEN(token);
                int index = 0;

                // TODO: GEN_CODE(CREATEFRAME, NULL, NULL, NULL);
                gen_code(q, INS_CREATEFRAME, NULL, NULL, NULL);
                gen_code(q, INS_CLEARS, NULL, NULL, NULL);

                // TODO: nastaveni navratovych hodnot podle curr_val
                // kontrola parametru
                if ((res = lof_e(token, local_node, &index, true, q)) != NO_ERR)
                    return res;

                // pro write je potreba definovat pocet parametru
                if (local_node->unlim_parms)
                {
                    CLEAR_OPERAND(OPERAND_DEST);
                    snprintf(_dest, MAX_STR_LEN, "int@%d", index);
                    gen_code(q, INS_PUSHS, _dest, NULL, NULL);
                }

                // TODO: check lof_data and node->lof_parms
                // TODO: GEN_CODE(PUSHS, int@$index, NULL, NULL);
                CHECK_AND_LOAD(token, TYPE_RIGHT_PARENTHESES);

                // TODO: GEN_CODE(CALL, local_node->key, NULL, NULL);
                gen_code(q, INS_CALL, local_node->key, NULL, NULL);

                if (lof_vars->len > local_node->lof_rets.length - 1)    return ERR_SEM_ASSIGN;
                for (int i = lof_vars->len; i > 0; i--)
                {
                    item_var_t *item = stack_var_pop(lof_vars);

                    // typova kontrola
                    TYPE_CHECK_AND_CONVERT_TERM(local_node->lof_rets.value[i-1], 
                            item->var_node->var_type, ERR_SEM_ASSIGN);

                    CLEAR_OPERAND(OPERAND_FIRST);
                    snprintf(_first, MAX_STR_LEN, "TF@%%retvar$%d", i);
                    gen_code(q, INS_PUSHS, _first, NULL, NULL);

                    // pretypovani
                    CONVERT_TO_FLOAT(local_node->lof_rets.value[i-1], 
                            item->var_node->var_type);

                    define_variable(FRAME_LF, OPERAND_DEST, item->var_node);
                    gen_code(q, INS_POPS, _dest, NULL, NULL);

                    item_var_destroy(&item);
                }
                // while (nof_ret_vals

                        // TODO:


                // while (lof_vars->len > 0)
                // {
                //     // pokud je vice promennych na leve strane, je jim zprava prirazena hodnota nil
                //     if (lof_vars->len > nof_ret_vals)
                //     {
                //         item_var_t *item = stack_var_pop(lof_vars);
                //         // TODO: GEN_CODE(MOVE, var->id, nil, NULL);
                //         if(item->var_node->is_param_var)
                //             sprintf(fir,"LF@param_%s",item->var_node->key);
                //         else
                //             sprintf(fir, "LF@%s", item->var_node->key);
                //         gen_code(q, INS_MOVE,fir,"nil@nil",NULL);
                //         item_var_destroy(&item);
                //     }
                //     else    // jinak se jim prirazuje vysledek samotny a odecitaji se indexy
                //     {
                //         item_var_t *item = stack_var_pop(lof_vars);
                //         if (item->var_node->var_type != (enum data_type)(local_node->lof_rets.value)[nof_ret_vals - 1])
                //         {
                //             item_var_destroy(&item);
                //             return ERR_SEM_FUNC;
                //         }
                //         // TODO: GEN_CODE(MOVE, item->var_node->key, retvar$nof_ret_vals, NULL);
                //         char s[14] = {0,};
                //         sprintf(s,"TF@%%retvar$%d",nof_ret_vals);
                //         if(item->var_node->is_param_var)
                //             sprintf(fir,"LF@param_%s",item->var_node->key);
                //         else
                //             sprintf(fir, "LF@%s", item->var_node->key);
                //         gen_code(q, INS_MOVE,fir,s,NULL);
                //         item_var_destroy(&item);
                //         nof_ret_vals--;
                //     }
                // }

                // TODO: parse with lof_vars
                // je to zasobnik, takze se musi prirazovat odzadu
                // pres porovnani delky -> node->lof_rets->length - 1 if not 0
            }
            else
            {
                if ((res = expression(token, &data_t, &final_exp)) != NO_ERR)   return res;
                if (stack_var_isempty(*lof_vars))
                    return ERR_SEM_ASSIGN;

                // nacte datovy typ do data
                if ((res =  multi_e_n(token, lof_vars, q)) != NO_ERR)  return res;
                // TODO: generovani kodu prirazeni -- pass
                // popnuti a prirazeni

                item_var_t *item = stack_var_pop(lof_vars);
                // push(exp)
                generate_code_nterm(&final_exp, q, &data_t);
                exp_nterm_destroy(&final_exp);

                // typova kontrola
                TYPE_CHECK_AND_CONVERT_TERM(data_t, item->var_node->var_type, ERR_SEM_ASSIGN);

                // pretypovani
                CONVERT_TO_FLOAT(item->var_node->var_type, data_t);

                // prirazeni
                define_variable(FRAME_LF, OPERAND_DEST, item->var_node);
                gen_code(q, INS_POPS, _dest, NULL, NULL);

                item_var_destroy(&item);
            }
            break;
        case TYPE_STRING:
        case TYPE_LEFT_PARENTHESES:
        case TYPE_NUMBER:
        case TYPE_INTEGER:
        case TYPE_BOOLEAN:
        case TYPE_KW_NIL:
        case TYPE_KW_NOT:
        case TYPE_STRLEN:
            if ((res = expression(token, &data_t, &final_exp)) != NO_ERR)   return res;
            if (stack_var_isempty(*lof_vars))
                return ERR_SEM_ASSIGN;

            // nacte datovy typ do data
            if ((res =  multi_e_n(token, lof_vars, q)) != NO_ERR)  return res;
            // TODO: generovani kodu prirazeni -- pass
            // popnuti a prirazeni
            item_var_t *item = stack_var_pop(lof_vars);

            // push(exp)
            generate_code_nterm(&final_exp, q, &data_t);
            exp_nterm_destroy(&final_exp);

            // typova kontrola
            TYPE_CHECK_AND_CONVERT_TERM(data_t, item->var_node->var_type, ERR_SEM_ASSIGN);

            // pretypovani
            CONVERT_TO_FLOAT(item->var_node->var_type, data_t);

            // prirazeni
            define_variable(FRAME_LF, OPERAND_DEST, item->var_node);
            gen_code(q, INS_POPS, _dest, NULL, NULL);

            item_var_destroy(&item);
            break;
        default:
            res = ERR_SYNTAX;
            break;
    }
    return res;
}

int multi_e_n(token_t **token, stack_var_t *lof_vars, queue_t *q)
{
    int res = NO_ERR;
    enum data_type data_t = DATA_NIL;
    exp_nterm_t *final_exp = NULL;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
        // <multi_e_n> -> , <expression> <multi_e_n>
        case TYPE_COMMA:
            LOAD_TOKEN(token);

            if ((res = expression(token, &data_t, &final_exp)) != NO_ERR)   return res;
            if (stack_var_isempty(*lof_vars))
                return ERR_SEM_ASSIGN;

            // nacte datovy typ do data
            if ((res =  multi_e_n(token, lof_vars, q)) != NO_ERR)  return res;
            // TODO: generovani kodu prirazeni -- pass
            item_var_t *item = stack_var_pop(lof_vars);
           
            // push(exp)
            generate_code_nterm(&final_exp, q, &data_t);
            exp_nterm_destroy(&final_exp);


            // typova kontrola
            TYPE_CHECK_AND_CONVERT_TERM(data_t, item->var_node->var_type, ERR_SEM_ASSIGN);

            // pretypovani
            CONVERT_TO_FLOAT(item->var_node->var_type, data_t);

            // prirazeni
            define_variable(FRAME_LF, OPERAND_DEST, item->var_node);
            gen_code(q, INS_POPS, _dest, NULL, NULL);

            item_var_destroy(&item);
            break;

            // <multi_e_n> -> eps
        case TYPE_KW_LOCAL:
        case TYPE_KW_IF:
        case TYPE_KW_WHILE:
        case TYPE_KW_ELSE:
        case TYPE_KW_END:
        case TYPE_KW_RETURN:    // TODO: ZKONTROLOVAT GRAMATIKU?!
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

        case TYPE_KW_NIL:
            LOAD_TOKEN(token);
            *data_t = DATA_NIL;
            break;

        default:
            *data_t = DATA_NIL;
            res = ERR_SYNTAX;
            break;
    }
    return res;
}

static int insert_lib_functions()
{
    node_ptr node = NULL;
    int res = NO_ERR;

    // funciton reads() : string
    INSERT_SYMBOL("reads", FUNC, node);
    if (node == NULL)   return ERR_INTERNAL;
    node->is_defined = node->is_declared = 1;
    // predavani parametru
    if ((res = string_Init(&node->lof_params)) != NO_ERR)  return res;
    if ((res = string_Add_Char(&node->lof_params, DATA_NIL)) != NO_ERR) return res;
    // pridavani navratovych hodnot
    if ((res = string_Init(&node->lof_rets)) != NO_ERR)  return res;
    if ((res = string_Add_Char(&node->lof_rets, DATA_STR)) != NO_ERR) return res;
    if ((res = string_Add_Char(&node->lof_rets, DATA_NIL)) != NO_ERR) return res;

    // funciton readi() : integer
    INSERT_SYMBOL("readi", FUNC, node);
    if (node == NULL)   return ERR_INTERNAL;
    node->is_defined = node->is_declared = 1;
    // predavani parametru
    if ((res = string_Init(&node->lof_params)) != NO_ERR)  return res;
    if ((res = string_Add_Char(&node->lof_params, DATA_NIL)) != NO_ERR) return res;
    // pridavani navratovych hodnot
    if ((res = string_Init(&node->lof_rets)) != NO_ERR)  return res;
    if ((res = string_Add_Char(&node->lof_rets, DATA_INT)) != NO_ERR) return res;
    if ((res = string_Add_Char(&node->lof_rets, DATA_NIL)) != NO_ERR) return res;

    // funciton tointeger(f: number) : integer
    INSERT_SYMBOL("tointeger", FUNC, node);
    if (node == NULL)   return ERR_INTERNAL;
    node->is_defined = node->is_declared = 1;
    // pridavani parametru
    if ((res = string_Init(&node->lof_params)) != NO_ERR)  return res;
    if ((res = string_Add_Char(&node->lof_params, DATA_NUM)) != NO_ERR) return res;
    if ((res = string_Add_Char(&node->lof_params, DATA_NIL)) != NO_ERR) return res;
    // pridavani navratovych hodnot
    if ((res = string_Init(&node->lof_rets)) != NO_ERR)  return res;
    if ((res = string_Add_Char(&node->lof_rets, DATA_INT)) != NO_ERR) return res;
    if ((res = string_Add_Char(&node->lof_rets, DATA_NIL)) != NO_ERR) return res;

    // funciton ord(s: string, i: integer) : integer
    INSERT_SYMBOL("ord", FUNC, node);
    if (node == NULL)   return ERR_INTERNAL;
    node->is_defined = node->is_declared = 1;
    // pridavani parametru
    if ((res = string_Init(&node->lof_params)) != NO_ERR)  return res;
    if ((res = string_Add_Char(&node->lof_params, DATA_STR)) != NO_ERR) return res;
    if ((res = string_Add_Char(&node->lof_params, DATA_INT)) != NO_ERR) return res;
    if ((res = string_Add_Char(&node->lof_params, DATA_NIL)) != NO_ERR) return res;
    // pridavani navratovych hodnot
    if ((res = string_Init(&node->lof_rets)) != NO_ERR)  return res;
    if ((res = string_Add_Char(&node->lof_rets, DATA_INT)) != NO_ERR) return res;
    if ((res = string_Add_Char(&node->lof_rets, DATA_NIL)) != NO_ERR) return res;

    // function write(id1, id2, ..., idn)
    // ma konecny pocet (predem nedefinovany) parametru
    INSERT_SYMBOL("write", FUNC, node);
    if (node == NULL)   return ERR_INTERNAL;
    node->is_declared = node->is_defined = node->unlim_parms = 1;
    // predavani parametru
    if ((res = string_Init(&node->lof_params)) != NO_ERR)  return res;
    if ((res = string_Add_Char(&node->lof_params, DATA_NIL)) != NO_ERR) return res;
    // predavani navratovych hodnot
    if ((res = string_Init(&node->lof_rets)) != NO_ERR)  return res;
    if ((res = string_Add_Char(&node->lof_rets, DATA_NIL)) != NO_ERR) return res;

    // funciton readn() : number
    INSERT_SYMBOL("readn", FUNC, node);
    if (node == NULL)   return ERR_INTERNAL;
    node->is_defined = node->is_declared = 1;
    // predavani parametru
    if ((res = string_Init(&node->lof_params)) != NO_ERR)  return res;
    if ((res = string_Add_Char(&node->lof_params, DATA_NIL)) != NO_ERR) return res;
    // pridavani navratovych hodnot
    if ((res = string_Init(&node->lof_rets)) != NO_ERR)  return res;
    if ((res = string_Add_Char(&node->lof_rets, DATA_NUM)) != NO_ERR) return res;
    if ((res = string_Add_Char(&node->lof_rets, DATA_NIL)) != NO_ERR) return res;

    // funciton substr(s: string, i: number, j: number) : string
    INSERT_SYMBOL("substr", FUNC, node);
    if (node == NULL)   return ERR_INTERNAL;
    node->is_defined = node->is_declared = 1;
    // pridavani parametru
    if ((res = string_Init(&node->lof_params)) != NO_ERR)  return res;
    if ((res = string_Add_Char(&node->lof_params, DATA_STR)) != NO_ERR) return res;
    if ((res = string_Add_Char(&node->lof_params, DATA_INT)) != NO_ERR) return res;
    if ((res = string_Add_Char(&node->lof_params, DATA_INT)) != NO_ERR) return res;
    if ((res = string_Add_Char(&node->lof_params, DATA_NIL)) != NO_ERR) return res;
    // pridavani navratovych hodnot
    if ((res = string_Init(&node->lof_rets)) != NO_ERR)  return res;
    if ((res = string_Add_Char(&node->lof_rets, DATA_STR)) != NO_ERR) return res;
    if ((res = string_Add_Char(&node->lof_rets, DATA_NIL)) != NO_ERR) return res;

    // funciton chr(i: integer) : string
    INSERT_SYMBOL("chr", FUNC, node);
    if (node == NULL)   return ERR_INTERNAL;
    node->is_defined = node->is_declared = 1;
    // pridavani parametru
    if ((res = string_Init(&node->lof_params)) != NO_ERR)  return res;
    if ((res = string_Add_Char(&node->lof_params, DATA_INT)) != NO_ERR) return res;
    if ((res = string_Add_Char(&node->lof_params, DATA_NIL)) != NO_ERR) return res;
    // pridavani navratovych hodnot
    if ((res = string_Init(&node->lof_rets)) != NO_ERR)  return res;
    if ((res = string_Add_Char(&node->lof_rets, DATA_STR)) != NO_ERR) return res;
    if ((res = string_Add_Char(&node->lof_rets, DATA_NIL)) != NO_ERR) return res;

    return res;
}

FILE *global_file;
frame_stack local_stack;
node_ptr global_tree;
queue_t q_identifier;
unsigned file_line = 1;

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

    if ((res = insert_lib_functions()) != NO_ERR)
        goto post_local_stack_error;

    // TODO: generate code init
    queue_init(&q_identifier);
    gen_code(&q_identifier,INS_LABEL,"$$main",NULL,NULL);
    gen_code(&q_identifier,INS_CREATEFRAME,NULL,NULL,NULL);
    gen_code(&q_identifier,INS_PUSHFRAME,NULL,NULL,NULL);
    gen_code(&q_identifier, INS_DEFVAR, "LF@$main_tmp1", NULL, NULL);
    gen_code(&q_identifier, INS_DEFVAR, "LF@$main_tmp2", NULL, NULL);
    gen_code(&q_identifier, INS_DEFVAR, "LF@$main_tmp3", NULL, NULL);

    // TODO: vlozeni vnitrnich funkci do TS
    import_builtin_functions();

    // poslani prvniho tokenu do <prg>
    if ((res = get_token(global_file, &token)) != NO_ERR)
        goto post_local_stack_error;

    init_gen_info();
    res = prg(&token);
    if (res != NO_ERR)
        token_delete(&token);

    if (!contains_require)
        return ERR_SYNTAX;

    // print instruction queue
    queue_flush(&q_identifier);
    gen_code(NULL ,INS_POPFRAME,NULL,NULL,NULL);


    // TODO: destroy generate code
post_local_stack_error:
    stack_destroy(&local_stack, &tree_destroy);
post_tree_error:
    tree_destroy(&global_tree);
tree_init_error:
    return res;
}

/* parser.c */
