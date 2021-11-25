#include "expression.h"
#include "parser.h"
#include "generator.h"

/**
 * @brief Implementace <prg>
 *
 * @param Posledni nacteny token
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */

queue_t q_identifier;
int index_while;

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
            LOAD_TOKEN(token);
            CHECK_AND_LOAD(token, TYPE_LEFT_PARENTHESES);       // (
            int index = 0;

            // TODO: GEN_CODE(CREATEFRAME, NULL, NULL, NULL)
            gen_code(&q_identifier,INS_CREATEFRAME,NULL,NULL,NULL);
            // inicializace a pridani datovych typu
            if ((res = lof_e(token, node, &index, 1)) != NO_ERR)        goto error;

            // TODO: GEN_CODE(PUSHS, "int@$index", NULL, NULL)
            char s[14];
            sprintf(s,"int@%d",index);
            gen_code(&q_identifier,INS_PUSHS,s,NULL,NULL);
            CHECK_AND_LOAD(token, TYPE_RIGHT_PARENTHESES);      // )

            // TODO: GEN_CODE(call, function_name, NULL, NULL)
            gen_code(&q_identifier,INS_CALL,(*token)->value.str_val,NULL,NULL);

            return prg(token);

            // <prg> -> function id ( <def_parm> ) <ret> <code> end <prg>
        case TYPE_KW_FUNCTION:

            LOAD_AND_CHECK(token, TYPE_IDENTIFIER);             // id
            SEARCH_SYMBOL((*token)->value.str_val, FUNC, node);

            // TODO: GEN_CODE(label, function_name, NULL, NULL)
            // TODO: GEN_CODE(PUSHFRAME, NULL, NULL, NULL)
            gen_code(NULL,INS_LABEL,(*token)->value.str_val, NULL, NULL);
            gen_code(NULL, INS_PUSHFRAME, NULL, NULL, NULL);

            // funkce jeste neexistuje
            if (node == NULL)
                INSERT_SYMBOL((*token)->value.str_val, FUNC, node);
            else
                if (node->is_defined)   return ERR_SEM_DEF;
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

            // TODO: pripravit navratove promenne??

            if ((res = code(token, &node)) != NO_ERR)   goto error;

            DESTROY_SCOPE();
            // TODO: GEN_CODE(POPFRAME, NULL, NULL, NULL);
            // TODO: GEN_CODE(RETURN, NULL, NULL, NULL);
            gen_code(NULL, INS_POPFRAME, NULL, NULL, NULL);
            gen_code(NULL, INS_RETURN, NULL, NULL, NULL);

            // TODO: check whether ret types match

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

// TODO: predavat list ocekavanych parametru + index -> pro parsovani pri volani funkce
// id se bude prirazovat na ramec, semantika bude probihat na teto urovni (nebude se nic vracet)
int lof_e(token_t **token, node_ptr node, int *index, bool is_parm)
{
    if (node == NULL)   return ERR_INTERNAL;
    int res = NO_ERR;
    exp_nterm_t *final_exp = NULL;
    enum data_type data_t = DATA_NIL;
    Istring *exp = is_parm ? &node->lof_params         : &node->lof_rets;
    INIT_TOKEN(token, res);
    char s[14] = {0,};

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

            if (node->unlim_parms)
            {
                if ((res = expression(token, &data_t, &final_exp)) != NO_ERR)           return res;
                *index += 1;
                // TODO: add to passing argumets ???

                if (!is_parm)
                    ; // TODO: GEN_CODE(MOVE, "_tmp$temp_index", NULL, "retval$index");
                // TODO: pokud chybi -> doplnim nil
                generate_code_nterm(&final_exp,NULL);
                exp_nterm_destroy(&final_exp);  // v budoucnu se muze zmenit
                if ((res = lof_e_n(token, node, index, is_parm)) != NO_ERR)
                    return res;

                // XXX: bude prace se zasobnikem
                // int temp_index = gener_code_exp(final_exp)
                if (is_parm)
                    ; // TODO: GEN_CODE(PUSHS, NULL, NULL, "_tmp$temp_index")
            }
            else
            {
                // predava se moc argumentu
                if (is_parm && exp->length == 0) return ERR_SEM_FUNC;
                // if (*index >= exp->length - 1)                              return ERR_SEM_FUNC;

                // ziska datovy typ vyrazu a porovna to s ocekavanym
                if ((res = expression(token, &data_t, &final_exp)) != NO_ERR) return res;

                // TODO: jeste zkontrolovat!!!
                if (type_control(data_t, (exp->value)[*index]))
                {
                    exp_nterm_destroy(&final_exp);
                    return ERR_SEM_FUNC;
                }

                if (data_t == DATA_INT && (exp->value)[*index] == DATA_NUM); // TODO: pretypovani

                // TODO: MOVE TMP_EXP TO PARM$index+1 -- pass
                *index += 1;
                // TODO: add to passing argumets ???

                generate_code_nterm(&final_exp, NULL);
                if (!is_parm)
                {
                    // TODO: GEN_CODE(MOVE, "retvar$index", "_tmp$temp_index", NULL); -- POPS
                    // TODO: pokud chybi -> doplnim nil -- PASS
                    sprintf(s, "LF@%%retvar$%d", *index);
                    gen_code(NULL, INS_POPS, s, NULL, NULL);
                }
                exp_nterm_destroy(&final_exp);  // v budoucnu se muze zmenit
                // TODO: move to tmp
                // TODO: GENCODE POP
                sprintf(s, "LF@%%tmp$%d", *index);
                gen_code(NULL, INS_POPS, s, NULL, NULL);

                if ((res = lof_e_n(token, node, index, is_parm)) != NO_ERR)
                    return res;


                // XXX: bude prace se zasobnikem
                // int temp_index = gener_code_exp(final_exp)
                if (is_parm)
                {
                    // TODO: GEN_CODE(PUSHS, "_tmp$temp_index", NULL, NULL)
                    sprintf(s, "LF@%%tmp$%d", *index);
                    gen_code(NULL, INS_PUSHS, s, NULL, NULL);
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
            // TODO: pro !is_parm je potreba donastavit nil hodnoty pri generovani -> mozno udelat az venku
            break;

        default:
            res = ERR_SYNTAX;
            break;
    }
    return res;
}

int lof_e_n(token_t **token, node_ptr node, int *index, bool is_parm)
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
            // TODO: pro !is_parm je potreba donastavit nil hodnoty pri generovani -> mozno udelat az venku
            // if ((res = string_Add_Char(lof_data, DATA_NIL)) != NO_ERR)  return res;
            break;

            // <lof_e_n> -> , <expression> <lof_e_n>
        case TYPE_COMMA:
            LOAD_TOKEN(token);
            if (node->unlim_parms)
            {
                if ((res = expression(token, &data_t, &final_exp)) != NO_ERR)           return res;
                *index += 1;
                // TODO: add to passing argumets ???

                if (!is_parm)
                    ; // TODO: GEN_CODE(MOVE, "_tmp$temp_index", NULL, "retval$index");

                generate_code_nterm(&final_exp,NULL);
                exp_nterm_destroy(&final_exp);  // v budoucnu se muze zmenit
                if ((res = lof_e_n(token, node, index, is_parm)) != NO_ERR)
                    return res;

                // int temp_index = gener_code_exp(final_exp)
                if (is_parm)
                    ; // TODO: GEN_CODE(PUSHS, NULL, NULL, "_tmp$temp_index")
            }
            else
            {
                // pokud se ocekava mene parametru
                if (!node->unlim_parms && *index > exp->length - 1)       return ERR_SEM_FUNC;

                if ((res = expression(token, &data_t, &final_exp)) != NO_ERR)           return res;

                // TODO: GEN_CODE

                if (type_control(data_t, (exp->value)[*index]))
                {
                    exp_nterm_destroy(&final_exp);
                    return ERR_SEM_FUNC;
                }
                if (data_t == DATA_INT && (exp->value)[*index] == DATA_NUM)
                    ; // TODO: pretypovani

                // TODO: MOVE TMP_EXP TO PARM$index+1
                *index += 1;
                // TODO: add to passing argumets ???

            if (!is_parm)
            {
                // TODO: GEN_CODE(MOVE, "_tmp$temp_index", "retval$index", NULL);
                char s[14];
                sprintf(s,"LF@%%retvar$%d",*index);
                gen_code(NULL, INS_POPS, s,NULL,NULL);
            }
                generate_code_nterm(&final_exp, NULL);
                exp_nterm_destroy(&final_exp);  // v budoucnu se muze zmenit
                if ((res = lof_e_n(token, node, index, is_parm)) != NO_ERR)
                    return res;

                // int temp_index = gener_code_exp(final_exp)
                if (is_parm)
                    ; // TODO: GEN_CODE(PUSHS, "_tmp$temp_index", NULL, NULL)
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
                // TODO: GEN_CODE(DEFVAR, retvar$i1, NULL, NULL)
                // TODO: GEN_CODE(MOVE, retvar$1, nil@nil, NULL)
                gen_code(NULL, INS_DEFVAR, "LF@%%retvar$1", NULL, NULL);
                gen_code(NULL,INS_MOVE,"LF@%%retvar$1","nil@nil",NULL);
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
                // TODO: GEN_CODE(DEFVAR, retvar$i1, NULL, NULL)
                // TODO: GEN_CODE(MOVE, retvar$1, nil@nil, NULL)
                char s[14];
                sprintf(s,"LF@%%retvar$%d",index);
                gen_code(NULL,INS_DEFVAR,s,NULL, NULL);
                gen_code(NULL, INS_MOVE,s,"nil@nil",NULL);
            }

            return ret_n(token, lof_data, gen_code_print, index + 1);
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

            CHECK_AND_LOAD(token, TYPE_DECLARE);    // :

            if ((res = d_type(token, &data_t)) != NO_ERR)               return res;
            node->var_type = data_t;
            node->is_defined = 1;
            if ((res = string_Add_Char(lof_data, data_t)) != NO_ERR)    return res;

            // TODO: GEN_CODE(DEFVAR, id, NULL, NULL);
            // TODO: GEN_CODE(POPS, id, NULL, NULL);
            gen_code(NULL, INS_DEFVAR,(*token)->value.str_val, NULL, NULL);
            gen_code(NULL, INS_POPS,(*token)->value.str_val,NULL,NULL);

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
            if ((res = string_Add_Char(lof_data, data_t)) != NO_ERR)    return res;

            // TODO: GEN_CODE(DEFVAR, id, NULL, NULL);
            // TODO: GEN_CODE(POPS, id, NULL, NULL);
            gen_code(NULL, INS_DEFVAR, (*token)->value.str_val, NULL, NULL);
            gen_code(NULL, INS_POPS, (*token)->value.str_val, NULL, NULL);

            return def_parm_n(token, lof_data);

        default:
            res = ERR_SYNTAX;
            break;
    }
    return res;
}

int code(token_t **token, node_ptr *func_node)
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
            {
                // stack_reset_index(&local_stack);
                // while (stack_isempty(&local_stack))   
                // {
                //     local_node = tree_search(stack_top(&local_stack), (*token)->value.str_val);
                //     if (local_node != NULL)    
                //         break;
                //     else
                //         stack_dec_index(&local_stack);
                // }
                SEARCH_SYMBOL((*token)->value.str_val, VAR, local_node);
            }
            LOAD_TOKEN(token);

            if ((res = func_or_assign(token, &local_node)) != NO_ERR)   return res;

            return code(token, func_node);

            // <code> -> local id : <d_type> <var_init_assign> <code>
        case TYPE_KW_LOCAL:
            LOAD_AND_CHECK(token, TYPE_IDENTIFIER);             // id
            INSERT_SYMBOL((*token)->value.str_val, VAR, local_node);

            // TODO: GEN_CODE(DEF_VAR, id, NULL, NULL)
            gen_code(NULL, INS_DEFVAR,(*token)->value.str_val,NULL,NULL);
            LOAD_TOKEN(token);

            CHECK_AND_LOAD(token, TYPE_DECLARE);                // :

            if ((res = d_type(token, &data_t)) != NO_ERR)   return res;

            local_node->var_type = data_t;

            if ((res = var_init_assign(token, &data_t, &local_node)) != NO_ERR)    return res;
            // kontrola prirazeni; pokud neprobehne prirazeni, bude se ocekavat v data_t NIL
            if (type_control(data_t, local_node->var_type))
                return ERR_SEM_ASSIGN;

            local_node->is_defined = 1;

            // vlozeni do tabulky symbolu

            return code(token, func_node);

            // <code> -> if <expression> then <code> <elseif_block> <else_block> end <code>
        case TYPE_KW_IF:
            LOAD_TOKEN(token);

            // expression muze byt jakykoliv vyraz -> pokud to neni false nebo NIL
            // pak je to vzdy true; toto je ale reseno az pri generovani kodu
            if ((res = expression(token, &data_t, &final_exp)) != NO_ERR)   return res;
            generate_code_nterm(&final_exp, NULL);
            exp_nterm_destroy(&final_exp);
            // TODO: gen_code

            // TODO: jump to label ELSE if false

            CHECK_AND_LOAD(token, TYPE_KW_THEN);                // then

            INIT_SCOPE();
            // jde hlavne o kontrolu, pokud se zavola return
            if ((res = code(token, func_node)) != NO_ERR)    return res;
            DESTROY_SCOPE();

            if ((res = elseif_block(token, func_node)) != NO_ERR)   return res;

            // TODO: jump to lable END_IF

            if ((res = else_block(token, func_node)) != NO_ERR)    return res;

            CHECK_AND_LOAD(token, TYPE_KW_END);                 // end

            // TODO: label END

            return code(token, func_node);

            // <code> -> while <expression> do <code> end <code>
        case TYPE_KW_WHILE:
            LOAD_TOKEN(token);
            queue_t q;
            queue_init(&q);

            // expression muze byt jakykoliv vyraz -> pokud to neni false nebo NIL
            // pak je to vzdy true; toto je ale reseno az pri generovani kodu
            if ((res = expression(token, &data_t, &final_exp)) != NO_ERR)   return res;

            char w[10];
            char w_end[14];
            sprintf(w,"while_%d",index_while);

            // TODO: GENCODE LABEL WHILE_index
            gen_code(&q,INS_LABEL,w,NULL,NULL);

            generate_code_nterm(&final_exp,&q);

            // TODO: ničit strom až po while!
            // TODO: GENCODE LT, GT, EQ expression
            // TODO: GENCODE POPS GF@%%temp_var1
            gen_code(&q,INS_POPS,"GF@%%temp_var1",NULL,NULL);

            sprintf(w_end,"end_while_%d",index_while);

            // TODO: GEN_CODE JUMPIFEQ ENDWHILE GF@%%temp_var1 nil@nil
            // TODO: GENCODE JUMPIFEQ ENDWHILE GF@%%temp_var1 bool@false
            gen_code(&q,INS_JUMPIFEQ,w_end,"GF@%%temp_var1","nil@nil");
            gen_code(&q,INS_JUMPIFEQ,w_end,"GF@%%temp_var1","bool@false");

            exp_nterm_destroy(&final_exp);


            CHECK_AND_LOAD(token, TYPE_KW_DO);

            INIT_SCOPE();
            // jde hlavne o kontrolu navratovych hodnot returnu ve scopu
            if ((res = code(token, func_node)) != NO_ERR)    return res;

            // TODO: go thru SYMTABLE and define vars
            // TODO: go thru queue_t and change delete init
            // TODO: flush code
            // TODO: jump to while label



            gen_code(&q,INS_JUMP,w,NULL,NULL);
            DESTROY_SCOPE();

            CHECK_AND_LOAD(token, TYPE_KW_END);
            // TODO: GENCODE LABEL ENDWHILE
            gen_code(&q,INS_LABEL,w_end,NULL,NULL);
            index_while += 1;

            queue_flush(&q);

            return code(token, func_node);

            // <code> -> return <lof_e>
        case TYPE_KW_RETURN:
            LOAD_TOKEN(token);

            // TODO: pred return se musi zkontrolovat stav listu
            int index = 0;
            if ((res = lof_e(token, *func_node, &index, 0)) != NO_ERR)
                return res;

            // TODO: donastavit nil pro chybejici parametry -> nebo mozno jiz implicitne nastavit predem

            // TODO: passing variable to return variable ??

            // TODO: typova kontrola
            // if (lof_type_control(&lof_data, (*func_node)->lof_rets))
            //     res = ERR_SEM_FUNC;

            // string_Free(&lof_data);
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

int var_init_assign(token_t **token, enum data_type *data_t, node_ptr *var_node)
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

            // <var_init_assign> -> = <fun_or_exp>
        case TYPE_ASSIGN:
            CHECK_AND_LOAD(token, TYPE_ASSIGN);     // =
            if ((res = fun_or_exp(token, data_t, var_node)) != NO_ERR)    return res;
            break;
        default:
            res = ERR_SYNTAX;
            break;
    }
    return res;
}

int fun_or_exp(token_t **token, enum data_type *data_t, node_ptr *var_node)
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
                *data_t = local_node->lof_rets.value[0];

                // TODO: GEN_CODE(CREATEFRAME, NULL, NULL, NULL);
                gen_code(NULL,INS_CREATEFRAME,NULL,NULL,NULL);
                LOAD_AND_CHECK(token, TYPE_LEFT_PARENTHESES);      // (
                LOAD_TOKEN(token);
                int index = 0;

                // kontrola parametru a prirazeni
                if ((res = lof_e(token, local_node, &index, true)) != NO_ERR)
                    return res;

                // TODO: GEN_CODE(PUSHS, int@$index, NULL, NULL);
                if (index != local_node->lof_params.length - 1)
                    return ERR_SEM_FUNC;

                CHECK_AND_LOAD(token, TYPE_RIGHT_PARENTHESES);     // )

                // TODO: GEN_CODE(call, local_node->key, NULL, NULL);
                // TODO: GEN_CODE(MOVE, var_node->key, retval1, NULL);
                gen_code(NULL, INS_CALL, local_node->key,NULL,NULL);
                gen_code(NULL,INS_MOVE,(*var_node)->key,"TF@%%retvar$1",NULL);
            }
            else
            {
                exp_nterm_t *final_exp = NULL;
                if ((res = expression(token, data_t, &final_exp)) != NO_ERR)
                    return res;
                generate_code_nterm(&final_exp, NULL);
                exp_nterm_destroy(&final_exp);

                // TODO: gen_code(final_exp);
                // TODO: GEN_CODE(MOVE, var_node->key, "_tmp$temp_index", NULL);
                gen_code(NULL, INS_POPS, "GF@%%temp_var1",NULL,NULL);
                gen_code(NULL, INS_MOVE,(*var_node)->key,"GF@%%temp_var1",NULL);
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
                if ((res = expression(token, data_t, &final_exp)) != NO_ERR)
                    return res;
                generate_code_nterm(&final_exp, NULL);
                exp_nterm_destroy(&final_exp);

                // TODO: gen_code(final_exp);
                // TODO: GEN_CODE(MOVE, var_node->key, "_tmp$temp_index", NULL);
                gen_code(NULL, INS_POPS, "GF@%%temp_var1",NULL,NULL);
                gen_code(NULL, INS_MOVE,(*var_node)->key,"GF@%%temp_var1",NULL);
            }
            break;
        default:
            res = ERR_SYNTAX;
            break;
    }
    return res;
}

int elseif_block(token_t **token, node_ptr *func_node)
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

            if ((res = expression(token, &data_t, &final_exp)) != NO_ERR)   return res;
            generate_code_nterm(&final_exp, NULL);
            exp_nterm_destroy(&final_exp);

            // TODO: generovat exp

            CHECK_AND_LOAD(token, TYPE_KW_THEN);    // then

            // TODO: jump to END_ELSEIF_cislo if false

            INIT_SCOPE();
            // TODO: LABEL ELSEIF_cislo
            res = code(token, func_node);
            DESTROY_SCOPE();

            if (res != NO_ERR)  return res;
            // TODO: LABEL END_ELSEIF_cislo
            return elseif_block(token, func_node);

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

    if (node == NULL)           return ERR_INTERNAL;
    if (*node == NULL)          return ERR_SEM_DEF;
    switch ((*token)->type)
    {
        // <func_or_assign> -> ( <lof_e> )
        case TYPE_LEFT_PARENTHESES:
            if ((*node)->type != FUNC)  return ERR_SEM_DEF;
            LOAD_TOKEN(token);

            int index = 0;

            // TODO: GEN_CODE(CREATEFRAME, NULL, NULL, NULL);
            gen_code(NULL, INS_CREATEFRAME, NULL, NULL, NULL);

            // kontrola a prirazeni parametru
            if ((res = lof_e(token, *node, &index, 1)) != NO_ERR)
                return res;

            // TODO: GEN_CODE(PUSHS, int@$index, NULL, NULL);

            CHECK_AND_LOAD(token, TYPE_RIGHT_PARENTHESES);      // )

            // TODO: somehow call the function
            // GEN_CODE(CALL, NULL, NULL, (*node)->key);
            gen_code(NULL,INS_CALL,(*node)->key,NULL,NULL);
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

            if ((res = multi_var_n(token, &lof_vars)) != NO_ERR)
            {
                stack_var_destroy(&lof_vars);
                return res;
            }

            CHECK_AND_LOAD(token, TYPE_ASSIGN);         // =

            if ((res = fun_or_multi_e(token, &lof_vars)) != NO_ERR)
            {
                stack_var_destroy(&lof_vars);
                return res;
            }

            // na leve strane je vice promennych nez na prave
            if (!stack_var_isempty(lof_vars))
                res = ERR_SEMANT;

            stack_var_destroy(&lof_vars);

            // TODO: get list of nodes and assign them all ??
            break;

        default:
            res = ERR_SYNTAX;
            break;
    }
    return res;
}

int multi_var_n(token_t **token, stack_var_t *lof_vars)
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

            return multi_var_n(token, lof_vars);

            // <multi_var_n> -> eps
        case TYPE_ASSIGN:
            break;

        default:
            res = ERR_SYNTAX;
            break;
    }
    return res;
}

int fun_or_multi_e(token_t **token, stack_var_t *lof_vars)
{
    if (lof_vars == NULL)   return ERR_INTERNAL;
    int res = NO_ERR;
    enum data_type data_t = DATA_NIL;
    exp_nterm_t *final_exp = NULL;
    node_ptr local_node;
    INIT_TOKEN(token, res);

    switch ((*token)->type)
    {
        // <multi_e> -> <expression> <multi_var_n>
        case TYPE_IDENTIFIER:
            SEARCH_SYMBOL((*token)->value.str_val, FUNC, local_node);
            if (local_node != NULL)
            {
                LOAD_AND_CHECK(token, TYPE_LEFT_PARENTHESES);
                LOAD_TOKEN(token);
                int index = 0;

                // TODO: GEN_CODE(CREATEFRAME, NULL, NULL, NULL);
                gen_code(NULL, INS_CREATEFRAME, NULL, NULL, NULL);

                // TODO: nastaveni navratovych hodnot podle curr_val
                // kontrola parametru
                if ((res = lof_e(token, local_node, &index, 1)) != NO_ERR)
                    return res;

                // TODO: check lof_data and node->lof_parms

                CHECK_AND_LOAD(token, TYPE_RIGHT_PARENTHESES);

                // TODO: GEN_CODE(CALL, local_node->key, NULL, NULL);
                gen_code(NULL, INS_CALL, local_node->key,NULL ,NULL);

                int nof_ret_vals = local_node->lof_rets.length - 1;
                while (lof_vars->len > 0)
                {
                    // pokud je vice promennych na leve strane, je jim zprava prirazena hodnota nil
                    if (lof_vars->len > nof_ret_vals)
                    {
                        item_var_t *item = stack_var_pop(lof_vars);
                        // TODO: GEN_CODE(MOVE, var->id, nil, NULL);
                        gen_code(NULL, INS_MOVE,item->var_node->key,"nil@nil",NULL);
                        item_var_destroy(&item);
                    }
                    else    // jinak se jim prirazuje vysledek samotny a odecitaji se indexy
                    {
                        item_var_t *item = stack_var_pop(lof_vars);
                        if (item->var_node->var_type != (enum data_type)(local_node->lof_rets.value)[nof_ret_vals - 1])
                        {
                            item_var_destroy(&item);
                            return ERR_SEM_FUNC;
                        }
                        // TODO: GEN_CODE(MOVE, item->var_node->key, retval$nof_ret_vals, NULL);
                        char s[14];
                        sprintf(s,"TF@%%retvar$%d",nof_ret_vals);
                        gen_code(NULL,INS_MOVE, item->var_node->key, s,NULL);
                        item_var_destroy(&item);
                        nof_ret_vals--;
                    }
                }

                // TODO: parse with lof_vars
                // je to zasobnik, takze se musi prirazovat odzadu
                // pres porovnani delky -> node->lof_rets->length - 1 if not 0
            }
            else
            {
                if ((res = expression(token, &data_t, &final_exp)) != NO_ERR)   return res;
                if (stack_var_isempty(*lof_vars))
                    return ERR_SEMANT;

                // nacte datovy typ do data
                if ((res =  multi_e_n(token, lof_vars)) != NO_ERR)  return res;
                // TODO: generovani kodu prirazeni -- pass
                // popnuti a prirazeni

                item_var_t *item = stack_var_pop(lof_vars);
                if (data_t != DATA_NIL && data_t != item->var_node->var_type)
                    return ERR_SEM_TYPE;
                generate_code_nterm(&final_exp, NULL);
                exp_nterm_destroy(&final_exp);
                // TODO: int ind = gen_exp(final_exp)
                // TODO: GEN_CODE(MOVE, item->var_node->key, _tmp$ind, NULL);
                gen_code(NULL,INS_POPS,"GF@%%temp_var1",NULL,NULL);
                gen_code(NULL,INS_MOVE,item->var_node->key,"GF@%%temp_var1",NULL);
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
                return ERR_SEMANT;

            // nacte datovy typ do data
            if ((res =  multi_e_n(token, lof_vars)) != NO_ERR)  return res;
            // TODO: generovani kodu prirazeni - pass
            // popnuti a prirazeni

            item_var_t *item = stack_var_pop(lof_vars);
            if (data_t != DATA_NIL && data_t != item->var_node->var_type)
                return ERR_SEM_TYPE;
            generate_code_nterm(&final_exp, NULL);
            exp_nterm_destroy(&final_exp);  // XXX: toto se pozdeni zmeni
            // TODO: int ind = gen_exp(final_exp)
            // TODO: GEN_CODE(MOVE, item->var_node->key, _tmp$ind, NULL);
            gen_code(NULL,INS_POPS,"GF@%%temp_var1",NULL,NULL);
            gen_code(NULL,INS_MOVE,item->var_node->key,"GF@%%temp_var1",NULL);
            item_var_destroy(&item);        // XXX: toto se muze pozdeji zmenit
            break;
        default:
            res = ERR_SYNTAX;
            break;
    }
    return res;
}

int multi_e_n(token_t **token, stack_var_t *lof_vars)
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
                return ERR_SEMANT;

            // nacte datovy typ do data
            if ((res =  multi_e_n(token, lof_vars)) != NO_ERR)  return res;
            // TODO: generovani kodu prirazeni -pass
            // popnuti a prirazeni

            item_var_t *item = stack_var_pop(lof_vars);
            if (data_t != DATA_NIL && data_t != item->var_node->var_type)
                return ERR_SEM_TYPE;
            // TODO: int ind = gen_exp(final_exp)
            // TODO: GEN_CODE(MOVE, item->var_node->key, _tmp$ind, NULL);
            gen_code(NULL,INS_POPS,"GF@%%temp_var1",NULL,NULL);
            gen_code(NULL,INS_MOVE,item->var_node->key,"GF@%%temp_var1",NULL);

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
    if ((res = string_Add_Char(&node->lof_params, DATA_NUM)) != NO_ERR) return res;
    if ((res = string_Add_Char(&node->lof_params, DATA_NUM)) != NO_ERR) return res;
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
unsigned file_line = 1;

int syntax_analysis(FILE *file)
{
    int res;
    token_t *token = NULL;
    global_file = file;
    queue_init(&q_identifier);
    index_while = 0;

    // inicializace globalni TS a zasobniku TS
    if ((res = tree_init(&global_tree)) != NO_ERR)
        goto tree_init_error;
    if ((res = stack_init(&local_stack)) != NO_ERR)
        goto post_tree_error;

    if ((res = insert_lib_functions()) != NO_ERR)
        goto post_local_stack_error;

    // TODO: generate code init

    // TODO: vlozeni vnitrnich funkci do TS

    // poslani prvniho tokenu do <prg>
    if ((res = get_token(global_file, &token)) != NO_ERR)
        goto post_local_stack_error;

    res = prg(&token);
    if (res != NO_ERR)
        token_delete(&token);

    // print instruction queue
    queue_flush(&q_identifier);

    // TODO: destroy generate code
post_local_stack_error:
    stack_destroy(&local_stack, &tree_destroy);
post_tree_error:
    tree_destroy(&global_tree);
tree_init_error:
    return res;
}

/* parser.c */
