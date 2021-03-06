/**
 * @file expression.c
 * @brief Definice funkci algoritmu syntakticke analyzy zdola nahoru
 *
 * @authors David Kedra        (xkedra00)
 *          Hung Do            (xdohun00)
 *          Petr Kolarik       (xkolar79)
 *          Jaroslav Kvasnicka (xkvasn14)
 *
 * Reseni projektu IFJ a IAL, FIT VUT 21/22
 */
#include "token.h"
#include "expression.h"
#include "error.h"
#include "symtable.h"
#include "parser.h"
#include "generator.h"
#include <stdlib.h>
#include <string.h>


static int get_index_from_token(enum Token_type type)
{
    switch(type)
    {
        case TYPE_LEFT_PARENTHESES:  return TI_LEFT_PARENTHESES;
        case TYPE_RIGHT_PARENTHESES: return TI_RIGHT_PARENTHESES;
        case TYPE_POWER:             return TI_POWER;
        case TYPE_STRLEN:            return TI_STRLEN;
        case TYPE_KW_NOT:            return TI_NOT;
        case TYPE_MULTIPLY:
        case TYPE_DIVIDE:
        case TYPE_DIVIDE_WHOLE:
        case TYPE_DIVIDE_MODULO:     return TI_MULTIPLY;
        case TYPE_PLUS:
        case TYPE_MINUS:             return TI_PLUS;
        case TYPE_CONCAT:            return TI_CONCAT;
        case TYPE_LESSER:
        case TYPE_LESSER_OR_EQ:
        case TYPE_GREATER:
        case TYPE_EQ:
        case TYPE_NOT_EQ:
        case TYPE_GREATER_OR_EQ:     return TI_LOGIC;
        case TYPE_KW_AND:            return TI_AND;
        case TYPE_KW_OR:             return TI_OR;
        case TYPE_COMMA:             return TI_COMMA;
        case TYPE_IDENTIFIER:
        case TYPE_NUMBER:
        case TYPE_INTEGER: 
        case TYPE_STRING:  
        case TYPE_BOOLEAN:
        case TYPE_KW_NIL:            return TI_CONST;
        default:                     return TI_DOLLAR;
    }
    return -1;
}

static int get_index_from_item(const exp_item_t item)
{
    switch(item.type)
    {
        case SYM_TOKEN:
            return get_index_from_token(item.data.oper);
        case SYM_DOLLAR:
            return TI_DOLLAR;
        case SYM_TERM:
            return TI_CONST;
        default:
            return -1;
    }
}

static enum prec_type table[PREC_TABLE_SIZE][PREC_TABLE_SIZE] = {
//   (  )  ^  #  !  *  +  .. >  &  |  ,  c  $
    {S, E, S, S, S, S, S, S, S, S, S, E, S, U}, // (
    {U, R, R, U, U, R, R, R, R, R, R, R, U, R}, // )
    {S, R, S, S, S, R, R, R, R, R, R, R, S, R}, // ^
    {S, R, S, U, S, R, R, R, R, R, R, R, S, R}, // #
    {S, R, S, S, S, R, R, R, R, R, R, R, S, R}, // not
    {S, R, S, S, S, R, R, R, R, R, R, R, S, R}, // * / // %
    {S, R, S, S, S, S, R, R, R, R, R, R, S, R}, // + - 
    {S, R, S, S, S, S, S, S, R, R, R, R, S, R}, // ..
    {S, R, S, S, S, S, S, S, R, R, R, R, S, R}, // > >= < <= == ~=
    {S, R, S, S, S, S, S, S, S, S, R, R, S, R}, // and
    {S, R, S, S, S, S, S, S, S, S, R, R, S, R}, // or
    {S, E, S, S, S, S, S, S, S, S, S, E, S, U}, // ,
    {U, R, R, U, U, R, R, R, R, R, R, R, U, R}, // const
    {S, U, S, S, S, S, S, S, S, S, S, U, S, U}  // $
};

int exp_stack_reduce(exp_stack_t *s)
{
    if (!s)
        return ERR_INTERNAL;

    int res = NO_ERR;
    int op_num = 0;         // pocet polozek v zasobniku pred '<' shiftem
    int i = s->len - 1;     // index posledni polozky v zasobniku

    // dokud nenalezneme '<'
    while (i >= 0 && (s->array)[i].type != SYM_SHIFT)
    {
        i--;
        op_num++;
        // nenalezen znak shiftu
        if(i < 0) return ERR_SYNTAX;
    }

    // inicializace noveho prvku
    exp_nterm_t *new_nterm = NULL;
    if ((res = exp_nterm_init(&new_nterm)) != NO_ERR)   return res;

    new_nterm->id = reduc_id++;

    exp_item_t *operand1 = NULL;
    exp_item_t *oper = NULL;
    exp_item_t *operand2 = NULL;

    if(op_num == 1)
    {
        // ziska ten jeden operand
        operand1 = exp_stack_pop(s);
        if (operand1 == NULL)
            CLEAR_UP_IN_REDUCE(ERR_INTERNAL);
        if (operand1->type != SYM_TERM)  
            CLEAR_UP_IN_REDUCE(ERR_SYNTAX);

        // pro termy:
        switch (operand1->data.term.type)
        {
            case DATA_ID:
                {
                    node_ptr node = NULL;
                    new_nterm->rule = RULE_ID;
                    SEARCH_SYMBOL(operand1->data.term.value.id, VAR, node);
                    if (node == NULL)
                        CLEAR_UP_IN_REDUCE(ERR_SEM_DEF);
                    if (!node->is_defined)
                        CLEAR_UP_IN_REDUCE(ERR_SEM_DEF);
                    new_nterm->data_t = node->var_type;     // prirazeni podle hodnoty v TS
                }
                break;
            case DATA_STR:
                new_nterm->rule = RULE_STR;
                new_nterm->data_t = DATA_STR;
                break;
            case DATA_INT:
                new_nterm->rule = RULE_INT;
                new_nterm->data_t = DATA_INT;
                break;
            case DATA_NUM:
                new_nterm->rule = RULE_NUM;
                new_nterm->data_t = DATA_NUM;
                break;
            case DATA_BOOL:
                new_nterm->rule = RULE_BOOL;
                new_nterm->data_t = DATA_BOOL;
                break;
            case DATA_NIL: 
                new_nterm->rule = RULE_NIL;
                new_nterm->data_t = DATA_NIL;
                break;
            
            default:
                break;
        }
        exp_data_copy(&new_nterm->val1, &operand1->data.term);

        // uvolni 
        exp_stack_destroy_item(operand1);
    }
    else if(op_num == 2)
    {
        operand1 = exp_stack_pop(s);
        oper = exp_stack_pop(s);
        // kontrola poradi
        if (oper == NULL || operand1 == NULL)
            CLEAR_UP_IN_REDUCE(ERR_INTERNAL);
        if (oper->type != SYM_TOKEN && operand1->type != SYM_EXPR)
            CLEAR_UP_IN_REDUCE(ERR_SYNTAX);

        // semanticka kontrola a prirazeni pravidla
        switch (oper->data.oper)
        {
            case TYPE_STRLEN:   // # 
                // operand musi byt typu string
                if (operand1->data.nterm->data_t == DATA_NIL)
                    CLEAR_UP_IN_REDUCE(ERR_UNEXP_VAL);
                else if (operand1->data.nterm->data_t != DATA_STR)
                    CLEAR_UP_IN_REDUCE(ERR_SEM_TYPE);

                new_nterm->rule = RULE_STRLEN;
                new_nterm->data_t = DATA_INT;
                break;
            case TYPE_KW_NOT:   // not
                new_nterm->rule = RULE_NOT;
                new_nterm->data_t = DATA_BOOL;
                break;
            
            default:
                break;
        }

        new_nterm->val1.type = DATA_SUB_EXP;
        new_nterm->val1.value.sub_expr = operand1->data.nterm;

        // TODO: nejake cisteni?? memory leak??
        exp_stack_destroy_item(oper);
    }
    else if(op_num == 3)
    {
        operand2 = exp_stack_pop(s);
        oper = exp_stack_pop(s);
        operand1 = exp_stack_pop(s);
        if (operand1 == NULL || oper == NULL || operand2 == NULL)
            CLEAR_UP_IN_REDUCE(ERR_INTERNAL);

        // odlisne pravidlo ( E )
        if((operand1->type == SYM_TOKEN && operand2->type == SYM_TOKEN && oper->type == SYM_EXPR) && 
            (operand1->data.oper == TYPE_LEFT_PARENTHESES && operand2->data.oper == TYPE_RIGHT_PARENTHESES))
        {
            free(new_nterm);
            new_nterm = oper->data.nterm;
            exp_stack_destroy_item(operand2);   // odstraneni )
            exp_stack_destroy_item(operand1);   // odstraneni (
            // odstraneni '<'
            exp_stack_pop(s);
            return exp_stack_push(s, SYM_EXPR, NULL, NULL, new_nterm);
        }
        else if(operand1->type != SYM_EXPR || oper->type != SYM_TOKEN || operand2->type != SYM_EXPR)
            CLEAR_UP_IN_REDUCE(ERR_SYNTAX);

        // zbyle pravidla E op E
        switch (oper->data.oper)
        {
            case TYPE_CONCAT:
                new_nterm->rule = RULE_CONCAT;
                SEMANTIC_CHECK_STR(new_nterm, operand1->data.nterm, operand2->data.nterm);
                break;
            case TYPE_PLUS:
                new_nterm->rule = RULE_PLUS;
                SEMANTIC_CHECK_INT_AND_NUM(new_nterm, operand1->data.nterm, operand2->data.nterm, 
                        new_nterm->val1_to_num, new_nterm->val2_to_num);
                break;
            case TYPE_MINUS:
                new_nterm->rule = RULE_MINUS;
                SEMANTIC_CHECK_INT_AND_NUM(new_nterm, operand1->data.nterm, operand2->data.nterm, 
                        new_nterm->val1_to_num, new_nterm->val2_to_num);
                break;
            case TYPE_MULTIPLY:
                new_nterm->rule = RULE_MULTIPLY;
                SEMANTIC_CHECK_INT_AND_NUM(new_nterm, operand1->data.nterm, operand2->data.nterm, 
                        new_nterm->val1_to_num, new_nterm->val2_to_num);
                break;
            case TYPE_DIVIDE:
                new_nterm->rule = RULE_DIVIDE;
                SEMANTIC_CHECK_INT_AND_NUM(new_nterm, operand1->data.nterm, operand2->data.nterm, 
                        new_nterm->val1_to_num, new_nterm->val2_to_num);
                // check for div 0
                if (operand2->data.nterm->rule == RULE_INT || 
                        operand2->data.nterm->rule == RULE_NUM)
                    CHECK_DIV_ZERO(operand2->data.nterm->val1);
                break;
            case TYPE_DIVIDE_WHOLE:
                new_nterm->rule = RULE_DIVIDE_WHOLE;
                SEMANTIC_CHECK_INT_AND_NUM(new_nterm, operand1->data.nterm, operand2->data.nterm, 
                        new_nterm->val1_to_num, new_nterm->val2_to_num);
                // check for div 0
                if (operand2->data.nterm->rule == RULE_INT ||
                        operand2->data.nterm->rule == RULE_NUM)
                    CHECK_DIV_ZERO(operand2->data.nterm->val1); 
                break;
            case TYPE_EQ:
                new_nterm->rule = RULE_EQ;
                SEMANTIC_GENERAL_TYPE_CHECK(operand1->data.nterm, operand2->data.nterm);
                new_nterm->data_t = DATA_BOOL;
                break;
            case TYPE_NOT_EQ:
                new_nterm->rule = RULE_NE;
                SEMANTIC_GENERAL_TYPE_CHECK(operand1->data.nterm, operand2->data.nterm);
                new_nterm->data_t = DATA_BOOL;
                break;
            case TYPE_GREATER_OR_EQ:
                new_nterm->rule = RULE_GE;
                SEMANTIC_GENERAL_TYPE_CHECK(operand1->data.nterm, operand2->data.nterm);
                SEMANTIC_CHECK_BOOL_NON_NIL(new_nterm, operand1->data.nterm, operand2->data.nterm);
                new_nterm->data_t = DATA_BOOL;
                break;
            case TYPE_GREATER:
                new_nterm->rule = RULE_GT;
                SEMANTIC_GENERAL_TYPE_CHECK(operand1->data.nterm, operand2->data.nterm);
                SEMANTIC_CHECK_BOOL_NON_NIL(new_nterm, operand1->data.nterm, operand2->data.nterm);
                new_nterm->data_t = DATA_BOOL;
                break;
            case TYPE_LESSER_OR_EQ:
                new_nterm->rule = RULE_LE;
                SEMANTIC_GENERAL_TYPE_CHECK(operand1->data.nterm, operand2->data.nterm);
                SEMANTIC_CHECK_BOOL_NON_NIL(new_nterm, operand1->data.nterm, operand2->data.nterm);
                new_nterm->data_t = DATA_BOOL;
                break;
            case TYPE_LESSER:
                new_nterm->rule = RULE_LT;
                SEMANTIC_GENERAL_TYPE_CHECK(operand1->data.nterm, operand2->data.nterm);
                SEMANTIC_CHECK_BOOL_NON_NIL(new_nterm, operand1->data.nterm, operand2->data.nterm);
                new_nterm->data_t = DATA_BOOL;
                break;
            case TYPE_KW_AND:
                new_nterm->rule = RULE_AND;
                SEMANTIC_GENERAL_TYPE_CHECK(operand1->data.nterm, operand2->data.nterm);
                new_nterm->data_t = DATA_BOOL;
                break;
            case TYPE_KW_OR:
                new_nterm->rule = RULE_OR;
                SEMANTIC_GENERAL_TYPE_CHECK(operand1->data.nterm, operand2->data.nterm);
                new_nterm->data_t = DATA_BOOL;
                break;

            default:
                return ERR_SYNTAX;
                break;
        }
        new_nterm->val1.type = DATA_SUB_EXP;
        new_nterm->val2.type = DATA_SUB_EXP;

        new_nterm->val1.value.sub_expr = operand1->data.nterm;
        new_nterm->val2.value.sub_expr = operand2->data.nterm;
        exp_stack_destroy_item(oper);
    }
    else
        return ERR_SYNTAX;

    // popnuti '<'
    exp_stack_pop(s);
    return exp_stack_push(s, SYM_EXPR, NULL, NULL, new_nterm);
}

int exp_stack_shift(exp_stack_t *s, token_t *token)
{
    if (!s)
        return ERR_INTERNAL;

    //zasobnik pro odlozene neterminaly
    struct exp_stack temp_s; 
    int res = NO_ERR;
    if ((res = exp_stack_init(&temp_s)))     return res;
    // vrchni polozka
    struct exp_stack_item *top = exp_stack_top(s);

    // prohledani prvniho termu v zasobniku
    while (top != NULL && top->type == SYM_EXPR)
    {
        exp_stack_pop(s);
        if ((res = exp_stack_push_item(&temp_s, top)) != NO_ERR)
        {
            exp_stack_destroy(&temp_s);
            return res;
        }
        top = exp_stack_top(s);
    }
    // top je ted nejvrchnejsi nalezeny terminal
    if (top == NULL)    // nenalezl zadny term -> chyba (musi tam byt alespon $)
    {
        exp_stack_destroy(&temp_s);
        return ERR_INTERNAL;
    }

    // vlozeni '<' znaku za top terminal
    if ((res = exp_stack_push(s, SYM_SHIFT, NULL, NULL, NULL)) != NO_ERR)
    {
        exp_stack_destroy(&temp_s);
        return res;
    }

    // navrat vsech odlozenych polozek
    while (!exp_stack_isempty(&temp_s))
    {
        if ((res = exp_stack_push_item(s, exp_stack_pop(&temp_s))) != NO_ERR)
        {
            exp_stack_destroy(&temp_s);
            return res;
        }
    }
    exp_stack_destroy(&temp_s);

    // pridani noveho tokenu ze vstupu
    PUSH_DATA_TO_EXP_STACK((*s), token, res);
    return res;
}

// promenna pro urceni id docasnych promennych
unsigned reduc_id = 0;
int expression(token_t **token, enum data_type *data_t, exp_nterm_t **final_exp)
{
    if (token == NULL || *token == NULL)    return ERR_INTERNAL;
    reduc_id = 0;
    int res = NO_ERR;
    if (data_t == NULL || final_exp == NULL)    return ERR_INTERNAL;
    *data_t = DATA_NIL;
    struct exp_stack s;
    int loading = 1;
    if ((res = exp_stack_init(&s)))    return res;
    if ((res = exp_stack_push(&s, SYM_DOLLAR, NULL, NULL, NULL)))   return res;

    if (token == NULL || *token == NULL)   return ERR_INTERNAL;
    do
    {
        exp_item_t *top_item = exp_stack_top_term(&s);
        //ziskani patricne operace shift/redukce/.. z tabulky
        // enum prec_type  index = get_index_from_item(*top_item);

        // ziska symbol operace z tabulky
        int row = get_index_from_item(*top_item);
        int col = get_index_from_token((*token)->type);
        switch(table[row][col])
        {
            case E: 
                {
                    PUSH_DATA_TO_EXP_STACK(s, *token, res);
                    if (res != NO_ERR)
                        loading = 0;
                    else
                        LOAD_TOKEN(token);
                }
                break;
            case S:
                if ((res = exp_stack_shift(&s, *token)) != NO_ERR)
                    loading = 0;
                else
                    LOAD_TOKEN(token);
                break;
            case R:
                if ((res = exp_stack_reduce(&s)) != NO_ERR)
                    loading=0;
                break;
            case U:
                // TODO: error handling
                loading = 0;
                break;
        }
    } while (loading);
    if (res == ERR_INTERNAL || res == ERR_SEM_DEF || 
            res == ERR_SEM_TYPE || res == ERR_UNEXP_VAL)
    {
        exp_stack_destroy(&s);
        *final_exp = NULL;
        return res;
    }
    if ((*token)->type == TYPE_IDENTIFIER)
    {
        while (exp_stack_contains_shift(s))
        {
            if ((res = exp_stack_reduce(&s)) != NO_ERR)
            {
                exp_stack_destroy(&s);
                *final_exp = NULL;
                return res;
            }
        }
    }
    if (exp_stack_top_term(&s)->type != SYM_DOLLAR)
    {
        exp_stack_destroy(&s);
        *final_exp = NULL;
        return ERR_SYNTAX;
    }

    // TODO: dalsi veci
    exp_item_t *final_item = exp_stack_pop(&s);
    if (final_item == NULL)
    {   
        exp_stack_destroy(&s);
        return ERR_SYNTAX;
    }
    else        // zisk nejakych dat 
    {
        if (final_item->type != SYM_EXPR)
        {
            exp_stack_destroy(&s);
            return ERR_SYNTAX;
        }

        if (data_t == NULL || final_exp == NULL)
        {   /* TODO: goto cleaning and error handling */ }
        else
        {
            *data_t = final_item->data.nterm->data_t;
            *final_exp = final_item->data.nterm;
        }
    }
    exp_stack_destroy(&s);
    return res;
}

static int push_to_gen_stack(queue_t *q, exp_nterm_t *expr, enum data_type *data_t, bool is_global)
{
    if (expr == NULL)
        return ERR_INTERNAL;
    enum data_type op1;
    enum data_type op2;
    switch(expr->rule)
    {
        // jeden operand
        case RULE_ID:
            {
                node_ptr var_node = NULL;
                SEARCH_SYMBOL(expr->val1.value.id, VAR, var_node);

                if (var_node == NULL)   return ERR_INTERNAL;
                define_variable(FRAME_LF, OPERAND_DEST, var_node);
                gen_code(q, INS_PUSHS, _dest, NULL, NULL);
                if (data_t != NULL)
                    *data_t = var_node->var_type;
                break;
            }
        case RULE_BOOL:
            CLEAR_OPERAND(OPERAND_DEST);
            snprintf(_dest, MAX_STR_LEN, "bool@%s", expr->val1.value.boolean ? "true" : "false");
            gen_code(q, INS_PUSHS, _dest, NULL, NULL);
            if (data_t != NULL)
                *data_t = DATA_BOOL;
            break;
        case RULE_INT:
            CLEAR_OPERAND(OPERAND_DEST);
            snprintf(_dest, MAX_STR_LEN, "int@%d", expr->val1.value.integer);
            gen_code(q, INS_PUSHS, _dest, NULL, NULL);
            if (data_t != NULL)
                *data_t = DATA_INT;
            break;
        case RULE_NIL:
            gen_code(q, INS_PUSHS, "nil@nil", NULL, NULL);
            if (data_t != NULL)
                *data_t = DATA_NIL;
            break;
        case RULE_NUM:
            CLEAR_OPERAND(OPERAND_DEST);
            snprintf(_dest, MAX_STR_LEN, "float@%a", expr->val1.value.number);
            gen_code(q, INS_PUSHS, _dest, NULL, NULL);
            if (data_t != NULL)
                *data_t = DATA_NUM;
            break;
        case RULE_STR:
            CLEAR_OPERAND(OPERAND_DEST);
            snprintf(_dest, MAX_STR_LEN, "string@%s", expr->val1.value.string);
            gen_code(q, INS_PUSHS, _dest, NULL, NULL);
            if (data_t != NULL)
                *data_t = DATA_STR;
            break;

            // jeden operand a operator
        case RULE_STRLEN:
            push_to_gen_stack(q, expr->val1.value.sub_expr, NULL, is_global);

            IS_NOT_NIL(1, q);
            CLEAR_OPERAND(OPERAND_DEST);
            if (!is_global)
                snprintf(_dest, MAX_STR_LEN, "LF@$%s_tmp2", glob_cnt.func_name);
            else
                snprintf(_dest, MAX_STR_LEN, "LF@$global_tmp2");

            CLEAR_OPERAND(OPERAND_FIRST);
            if (!is_global)
                snprintf(_first, MAX_STR_LEN, "LF@$%s_tmp1", glob_cnt.func_name);
            else
                snprintf(_first, MAX_STR_LEN, "LF@$global_tmp1");
            gen_code(q,INS_STRLEN,_dest,_first,NULL);
            gen_code(q,INS_PUSHS,_dest,NULL,NULL);
            break;
        case RULE_NOT:
            push_to_gen_stack(q, expr->val1.value.sub_expr, NULL, is_global);
            gen_code(q, INS_NOTS, NULL, NULL, NULL);
            break;
        case RULE_CONCAT:
            push_to_gen_stack(q, expr->val1.value.sub_expr, NULL, is_global);
            push_to_gen_stack(q, expr->val2.value.sub_expr, NULL, is_global);
            IS_NOT_NIL(2, q);
            IS_NOT_NIL(1, q);
            CLEAR_OPERAND(OPERAND_DEST);
            CLEAR_OPERAND(OPERAND_FIRST);
            CLEAR_OPERAND(OPERAND_SECOND);
            if (!is_global)
            {
                snprintf(_dest, MAX_STR_LEN, "LF@$%s_tmp3", glob_cnt.func_name);
                snprintf(_first, MAX_STR_LEN, "LF@$%s_tmp2", glob_cnt.func_name);
                snprintf(_second, MAX_STR_LEN, "LF@$%s_tmp1", glob_cnt.func_name);
            }
            else
            {
                snprintf(_dest, MAX_STR_LEN, "LF@$global_tmp3");
                snprintf(_first, MAX_STR_LEN, "LF@$global_tmp2");
                snprintf(_second, MAX_STR_LEN, "LF@$global_tmp1");
            }
            gen_code(q, INS_CONCAT, _dest, _second, _first);
            gen_code(q, INS_PUSHS, _dest, NULL, NULL);
            break;
        case RULE_PLUS:
            push_to_gen_stack(q, expr->val1.value.sub_expr, &op1, is_global);
            push_to_gen_stack(q, expr->val2.value.sub_expr, &op2, is_global);
            IS_NOT_NIL(2, q);
            IS_NOT_NIL(1, q);

            FLOAT_IF_NEEDED(op1, op2, true);
            gen_code(q,INS_ADDS,NULL,NULL,NULL);
            break;
        case RULE_MINUS:
            push_to_gen_stack(q, expr->val1.value.sub_expr, &op1, is_global);
            push_to_gen_stack(q, expr->val2.value.sub_expr, &op2, is_global);
            IS_NOT_NIL(2, q);
            IS_NOT_NIL(1, q);

            FLOAT_IF_NEEDED(op1, op2, true);
            gen_code(q,INS_SUBS,NULL,NULL,NULL);
            break;
        case RULE_MULTIPLY:
            push_to_gen_stack(q, expr->val1.value.sub_expr, &op1, is_global);
            push_to_gen_stack(q, expr->val2.value.sub_expr, &op2, is_global);
            IS_NOT_NIL(2, q);
            IS_NOT_NIL(1, q);

            FLOAT_IF_NEEDED(op1, op2, true);
            gen_code(q, INS_MULS, NULL, NULL , NULL);
            break;
        case RULE_DIVIDE:
            push_to_gen_stack(q, expr->val1.value.sub_expr, &op1, is_global);
            push_to_gen_stack(q, expr->val2.value.sub_expr, &op2, is_global);
            IS_NOT_NIL(2, q);
            IS_NOT_NIL(1, q);
            IS_NOT_ZERO(2, q, op2); 
            FLOAT_IF_NEEDED(op1, op2, true);

            gen_code(q, INS_DIVS, NULL, NULL, NULL);
            break;
        case RULE_DIVIDE_WHOLE:
            push_to_gen_stack(q, expr->val1.value.sub_expr, &op1, is_global);
            push_to_gen_stack(q, expr->val2.value.sub_expr, &op2, is_global);
            IS_NOT_NIL(2, q);
            IS_NOT_NIL(1, q);
            IS_NOT_ZERO(2, q, op2);
            INT_IF_NEEDED(op1, op2);

            gen_code(q, INS_IDIVS, NULL, NULL, NULL);
            break;
        case RULE_EQ:
            push_to_gen_stack(q, expr->val1.value.sub_expr, &op1, is_global);
            push_to_gen_stack(q, expr->val2.value.sub_expr, &op2, is_global);
            CLEAR_OPERAND(OPERAND_FIRST);
            CLEAR_OPERAND(OPERAND_SECOND);
            if (!is_global)
            {
                snprintf(_first, MAX_STR_LEN, "LF@$%s_tmp2", glob_cnt.func_name);
                snprintf(_second, MAX_STR_LEN, "LF@$%s_tmp1", glob_cnt.func_name);
            }
            else
            {
                snprintf(_first, MAX_STR_LEN, "LF@$global_tmp2");
                snprintf(_second, MAX_STR_LEN, "LF@$global_tmp1");
            }
            gen_code(q, INS_POPS, _first, NULL, NULL);

            gen_code(q, INS_POPS, _second, NULL, NULL);

            if (op1 == DATA_NUM || op2 == DATA_NUM)
                FLOAT_IF_NEEDED(op1, op2, false);

            CLEAR_OPERAND(OPERAND_DEST);
            CLEAR_OPERAND(OPERAND_FIRST);
            CLEAR_OPERAND(OPERAND_SECOND);
            if (!is_global)
            {
                snprintf(_dest, MAX_STR_LEN, "LF@$%s_tmp3", glob_cnt.func_name);
                snprintf(_first, MAX_STR_LEN, "LF@$%s_tmp1", glob_cnt.func_name);
                snprintf(_second, MAX_STR_LEN, "LF@$%s_tmp2", glob_cnt.func_name);
            }
            else
            {
                snprintf(_dest, MAX_STR_LEN,   "LF@$global_tmp3");
                snprintf(_first, MAX_STR_LEN,  "LF@$global_tmp1");
                snprintf(_second, MAX_STR_LEN, "LF@$global_tmp2");
            }
            gen_code(q, INS_EQ, _dest, _first, _second);
            gen_code(q, INS_PUSHS, _dest, NULL, NULL);
            break;
        case RULE_NE:
            push_to_gen_stack(q, expr->val1.value.sub_expr, &op1, is_global);
            push_to_gen_stack(q, expr->val2.value.sub_expr, &op2, is_global);

            if (!is_global)
            {
                CLEAR_OPERAND(OPERAND_FIRST);
                snprintf(_first, MAX_STR_LEN, "LF@$%s_tmp2", glob_cnt.func_name);
                gen_code(q, INS_POPS, _first, NULL, NULL);

                CLEAR_OPERAND(OPERAND_FIRST);
                snprintf(_first, MAX_STR_LEN, "LF@$%s_tmp1", glob_cnt.func_name);
                gen_code(q, INS_POPS, _first, NULL, NULL);
            }
            else
            {
                CLEAR_OPERAND(OPERAND_FIRST);
                snprintf(_first, MAX_STR_LEN, "LF@$global_tmp2");
                gen_code(q, INS_POPS, _first, NULL, NULL);

                CLEAR_OPERAND(OPERAND_FIRST);
                snprintf(_first, MAX_STR_LEN, "LF@$global_tmp1");
                gen_code(q, INS_POPS, _first, NULL, NULL);
            }

            if (op1 == DATA_NUM || op2 == DATA_NUM)
                FLOAT_IF_NEEDED(op1, op2, false);

            CLEAR_OPERAND(OPERAND_DEST);
            snprintf(_dest, MAX_STR_LEN, "LF@$%s_tmp3", glob_cnt.func_name);
            CLEAR_OPERAND(OPERAND_FIRST);
            snprintf(_first, MAX_STR_LEN, "LF@$%s_tmp1", glob_cnt.func_name);
            CLEAR_OPERAND(OPERAND_SECOND);
            snprintf(_second, MAX_STR_LEN, "LF@$%s_tmp2", glob_cnt.func_name);
            gen_code(q, INS_EQ, _dest, _first, _second);
            gen_code(q, INS_PUSHS, _dest, NULL, NULL);
            gen_code(q, INS_NOTS, NULL, NULL, NULL);
            break;
        case RULE_GT:
            push_to_gen_stack(q, expr->val1.value.sub_expr, &op1, is_global);
            push_to_gen_stack(q, expr->val2.value.sub_expr, &op2, is_global);
            IS_NOT_NIL(2, q);
            IS_NOT_NIL(1, q);
            if (op1 == DATA_NUM || op2 == DATA_NUM)
                FLOAT_IF_NEEDED(op1, op2, true);
            else
            {
                if (!is_global)
                {
                    CLEAR_OPERAND(OPERAND_DEST);
                    snprintf(_dest, MAX_STR_LEN, "LF@$%s_tmp1", glob_cnt.func_name);
                    gen_code(q, INS_PUSHS, _dest, NULL, NULL);

                    CLEAR_OPERAND(OPERAND_DEST);
                    snprintf(_dest, MAX_STR_LEN, "LF@$%s_tmp2", glob_cnt.func_name);
                    gen_code(q, INS_PUSHS, _dest, NULL, NULL);
                }
                else
                {
                    CLEAR_OPERAND(OPERAND_DEST);
                    snprintf(_dest, MAX_STR_LEN, "LF@$global_tmp1");
                    gen_code(q, INS_PUSHS, _dest, NULL, NULL);

                    CLEAR_OPERAND(OPERAND_DEST);
                    snprintf(_dest, MAX_STR_LEN, "LF@$global_tmp2");
                    gen_code(q, INS_PUSHS, _dest, NULL, NULL);
                }
            }
            gen_code(q, INS_GTS, NULL, NULL, NULL);
            break;
        case RULE_GE:
            push_to_gen_stack(q, expr->val1.value.sub_expr, &op1, is_global);
            push_to_gen_stack(q, expr->val2.value.sub_expr, &op2, is_global);
            // nil kontrola
            IS_NOT_NIL(2, q);
            IS_NOT_NIL(1, q);
            if (op1 == DATA_NUM || op2 == DATA_NUM)
                FLOAT_IF_NEEDED(op1, op2, true);
            else
            {
                if (!is_global)
                {
                    CLEAR_OPERAND(OPERAND_DEST);
                    snprintf(_dest, MAX_STR_LEN, "LF@$%s_tmp1", glob_cnt.func_name);
                    gen_code(q, INS_PUSHS, _dest, NULL, NULL);

                    CLEAR_OPERAND(OPERAND_DEST);
                    snprintf(_dest, MAX_STR_LEN, "LF@$%s_tmp2", glob_cnt.func_name);
                    gen_code(q, INS_PUSHS, _dest, NULL, NULL);
                }
                else
                {
                    CLEAR_OPERAND(OPERAND_DEST);
                    snprintf(_dest, MAX_STR_LEN, "LF@$global_tmp1");
                    gen_code(q, INS_PUSHS, _dest, NULL, NULL);

                    CLEAR_OPERAND(OPERAND_DEST);
                    snprintf(_dest, MAX_STR_LEN, "LF@$global_tmp2");
                    gen_code(q, INS_PUSHS, _dest, NULL, NULL);
                }
            }

            CLEAR_OPERAND(OPERAND_FIRST);

            CLEAR_OPERAND(OPERAND_SECOND);
            if (!is_global)
            {
                snprintf(_first, MAX_STR_LEN, "LF@$%s_tmp2", glob_cnt.func_name);
                snprintf(_second, MAX_STR_LEN, "LF@$%s_tmp1", glob_cnt.func_name);
            }
            else
            {
                snprintf(_first, MAX_STR_LEN, "LF@$global_tmp2");
                snprintf(_second, MAX_STR_LEN, "LF@$global_tmp1");
            }

            gen_code(q,INS_POPS,_first,NULL,NULL);
            gen_code(q,INS_POPS,_second,NULL,NULL);

            gen_code(q,INS_PUSHS,_second,NULL,NULL);
            gen_code(q,INS_PUSHS,_first,NULL,NULL);
            gen_code(q,INS_GTS,NULL,NULL,NULL);
            
            gen_code(q,INS_PUSHS,_second,NULL,NULL);
            gen_code(q,INS_PUSHS,_first,NULL,NULL);
            gen_code(q,INS_EQS,NULL,NULL,NULL);

            gen_code(q,INS_ORS,NULL,NULL,NULL);
            break;
        case RULE_LT:
            push_to_gen_stack(q, expr->val1.value.sub_expr, &op1, is_global);
            push_to_gen_stack(q, expr->val2.value.sub_expr, &op2, is_global);
            IS_NOT_NIL(2, q);
            IS_NOT_NIL(1, q);
            if (op1 == DATA_NUM || op2 == DATA_NUM)
                FLOAT_IF_NEEDED(op1, op2, true);
            else
            {
                CLEAR_OPERAND(OPERAND_DEST);
                snprintf(_dest, MAX_STR_LEN, "LF@$%s_tmp1", glob_cnt.func_name);
                gen_code(q, INS_PUSHS, _dest, NULL, NULL);

                CLEAR_OPERAND(OPERAND_DEST);
                snprintf(_dest, MAX_STR_LEN, "LF@$%s_tmp2", glob_cnt.func_name);
                gen_code(q, INS_PUSHS, _dest, NULL, NULL);
            }
            gen_code(q, INS_LTS, NULL, NULL, NULL);
            break;
        case RULE_LE:
            push_to_gen_stack(q, expr->val1.value.sub_expr, &op1, is_global);
            push_to_gen_stack(q, expr->val2.value.sub_expr, &op2, is_global);
            // nil kontrola
            IS_NOT_NIL(2, q);
            IS_NOT_NIL(1, q);
            if (op1 == DATA_NUM || op2 == DATA_NUM)
                FLOAT_IF_NEEDED(op1, op2, true);
            else
            {
                CLEAR_OPERAND(OPERAND_DEST);
                snprintf(_dest, MAX_STR_LEN, "LF@$%s_tmp1", glob_cnt.func_name);
                gen_code(q, INS_PUSHS, _dest, NULL, NULL);

                CLEAR_OPERAND(OPERAND_DEST);
                snprintf(_dest, MAX_STR_LEN, "LF@$%s_tmp2", glob_cnt.func_name);
                gen_code(q, INS_PUSHS, _dest, NULL, NULL);
            }
            CLEAR_OPERAND(OPERAND_FIRST);

            CLEAR_OPERAND(OPERAND_SECOND);
            if (!is_global)
            {
                snprintf(_first, MAX_STR_LEN, "LF@$%s_tmp2", glob_cnt.func_name);
                snprintf(_second, MAX_STR_LEN, "LF@$%s_tmp1", glob_cnt.func_name);
            }
            else
            {
                snprintf(_first, MAX_STR_LEN, "LF@$global_tmp2");
                snprintf(_second, MAX_STR_LEN, "LF@$global_tmp1");
            }

            gen_code(q, INS_POPS, _first, NULL, NULL);
            gen_code(q, INS_POPS, _second, NULL, NULL);
            
            gen_code(q,INS_PUSHS,_second,NULL,NULL);
            gen_code(q,INS_PUSHS,_first,NULL,NULL);
            gen_code(q,INS_LTS,NULL,NULL,NULL);

            gen_code(q,INS_PUSHS,_second,NULL,NULL);
            gen_code(q,INS_PUSHS,_first,NULL,NULL);
            gen_code(q,INS_EQS,NULL,NULL,NULL);

            gen_code(q,INS_ORS,NULL,NULL,NULL);

            break;
        case RULE_AND:
            push_to_gen_stack(q, expr->val1.value.sub_expr, &op1, is_global);
            push_to_gen_stack(q, expr->val2.value.sub_expr, &op2, is_global);
            if (op1 == DATA_NUM || op2 == DATA_NUM)
                FLOAT_IF_NEEDED(op1, op2, true);
            gen_code(q, INS_ANDS, NULL, NULL, NULL);
            break;
        case RULE_OR:
            push_to_gen_stack(q, expr->val1.value.sub_expr, &op1, is_global);
            push_to_gen_stack(q, expr->val2.value.sub_expr, &op2, is_global);
            if (op1 == DATA_NUM || op2 == DATA_NUM)
                FLOAT_IF_NEEDED(op1, op2, true);
            gen_code(q, INS_ORS, NULL, NULL, NULL);
            break;
            // RULE_POWER,             // E -> E ^ E
            // RULE_MODULO,            // E -> E % E
        default:
            break;
    }
    return NO_ERR;
}

int generate_code_nterm(exp_nterm_t **expr, queue_t *q, enum data_type *data_t, bool is_global)
{
    if (expr != NULL && *expr != NULL)
    {
        int res = push_to_gen_stack(q, *expr, data_t, is_global);
        return res;
    }
    return ERR_INTERNAL;
}

/* expression.c */
