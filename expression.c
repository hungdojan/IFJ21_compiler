#include "token.h"
#include "expression.h"
#include "error.h"
#include <stdlib.h>

#define PREC_TABLE_SIZE 14

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
        case TYPE_GREATER_OR_EQ:     return TI_LOGIC;
        case TYPE_EQ:
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
    {U, R, R, U, U, R, R, R, R, R, R, R, U, U}, // )
    {S, R, S, S, S, R, R, R, R, R, R, R, S, R}, // ^
    {S, R, S, U, S, R, R, R, R, R, R, R, S, R}, // #
    {S, R, S, S, S, R, R, R, R, R, R, R, S, R}, // not
    {S, R, S, S, S, R, R, R, R, R, R, R, S, R}, // * / // %
    {S, R, S, S, S, S, R, R, R, R, R, R, S, R}, // + - 
    {S, R, S, S, S, S, S, S, R, R, R, R, S, R}, // ..
    {S, R, S, S, S, S, S, S, R, R, R, R, S, R}, // > >= < <= == ~=
    {S, R, S, S, S, S, S, S, S, S, R, S, S, R}, // and
    {S, R, S, S, S, S, S, S, S, S, R, R, S, R}, // or
    {S, E, S, S, S, S, S, S, S, S, S, E, S, R}, // ,
    {U, R, R, U, U, R, R, R, R, R, R, R, U, R}, // const
    {S, U, S, S, S, S, S, S, S, S, S, S, U, U}  // $
};

int exp_stack_reduce(exp_stack_t *s)
{
    if (!s)
        return ERR_INTERNAL;

    int op_num = 0;         // pocet operandu + operatoru
    int i = s->len - 1;
    // dokud nenalezneme '<'
    while (i >= 0 && (s->array)[i].type != SYM_SHIFT)
    {
        i--;
        op_num++;
        // nenalezen znak shiftu
        if(i < 0) return ERR_SYNTAX;
    }

    exp_item_t *op1;
    exp_item_t *op2;
    exp_item_t *op3;

    //exp_data_t *exp_data;
    exp_nterm_t *nterm;
    exp_nterm_init(&nterm);

    if(op_num == 1)
    {
        op1 = exp_stack_pop(s);

        //if(op1->type != SYM_TERM)
        //  TODO return

        // pro termy:
        switch (op1->data.term.type)
        {
            case DATA_ID:
                nterm->rule = RULE_ID;
                nterm->data_t = DATA_ID;
                break;
            case DATA_STR:
                nterm->rule = RULE_STR;
                nterm->data_t = DATA_STR;
                break;
            case DATA_INT:
                nterm->rule = RULE_INT;
                nterm->data_t = DATA_INT;
                break;
            case DATA_NUM:
                nterm->rule = RULE_NUM;
                nterm->data_t = DATA_NUM;
                break;
            case DATA_BOOL:
                nterm->data_t = DATA_BOOL;
                if(op1->data.term.value.boolean == true)
                    nterm->rule = RULE_TRUE;
                else
                    nterm->rule = RULE_FALSE;
                break;
            case DATA_NIL: 
                nterm->rule = RULE_NIL;
                nterm->data_t = DATA_NIL;
                break;
            
            default:
                break;
        }
        exp_data_copy(&nterm->val1, &op1->data.term);

    }
    else if(op_num == 2)
    {
        op2 = exp_stack_pop(s);
        op1 = exp_stack_pop(s);

        if (op1->type == SYM_TOKEN)
        {
            switch (op1->data.oper)
            {
                case TYPE_STRLEN:   //# 
                    nterm->rule = RULE_STRLEN;
                    nterm->data_t = DATA_INT;
                    break;
                case TYPE_KW_NOT:
                    nterm->rule = RULE_NOT;
                    nterm->data_t = DATA_BOOL;
                    break;
                
                default:
                    break;
            }
        }
        else
            return ERR_SYNTAX;

        nterm->val1.type = DATA_SUB_EXP;
        nterm->val1.value.sub_expr = op2->data.nterm;
        
    }
    else if(op_num == 3)
    {
        op3 = exp_stack_pop(s);
        op2 = exp_stack_pop(s);
        op1 = exp_stack_pop(s);

        // odlisne pravidlo ( E )
        if((op1->type == SYM_TOKEN && op3->type == SYM_TOKEN && op2->type == SYM_EXPR) && 
            (op1->data.oper == TYPE_LEFT_PARENTHESES && op3->data.oper == TYPE_RIGHT_PARENTHESES))
        {
            nterm->val1.type = DATA_SUB_EXP;
            nterm->val1.value.sub_expr = op2->data.nterm;
            nterm->rule = RULE_PARENTHESES;
        }
        else if(op3->type != SYM_EXPR || op1->type != SYM_EXPR || op2->type != SYM_TOKEN)
            return ERR_SYNTAX;

        // zbyle pravidla E op E
        switch (op2->data.oper)
        {
            case TYPE_POWER:
                nterm->rule = RULE_POWER;
                break;
            case TYPE_CONCAT:
                nterm->rule = RULE_CONCAT;
                break;
            case TYPE_PLUS:
                nterm->rule = RULE_PLUS;
                break;
            case TYPE_MINUS:
                nterm->rule = RULE_MINUS;
                break;
            case TYPE_MULTIPLY:
                nterm->rule = RULE_MULTIPLY;
                break;
            case TYPE_DIVIDE:
                nterm->rule = RULE_DIVIDE;
                break;
            case TYPE_DIVIDE_WHOLE:
                nterm->rule = RULE_DIVIDE_WHOLE;
                break;
            case TYPE_DIVIDE_MODULO:
                nterm->rule = RULE_MODULO;
                break;
            case TYPE_EQ:
                nterm->rule = RULE_EQ;
                break;
            case TYPE_NOT_EQ:
                nterm->rule = RULE_NE;
                break;
            case TYPE_GREATER_OR_EQ:
                nterm->rule = RULE_GE;
                break;
            case TYPE_GREATER:
                nterm->rule = RULE_GT;
                break;
            case TYPE_LESSER_OR_EQ:
                nterm->rule = RULE_LE;
                break;
            case TYPE_LESSER:
                nterm->rule = RULE_LT;
                break;
            case TYPE_KW_AND:
                nterm->rule = RULE_AND;
                break;
            case TYPE_KW_OR:
                nterm->rule = RULE_OR;
                break;

            default:
                return ERR_SYNTAX;
                break;
        }


        nterm->val1.type = DATA_SUB_EXP;
        nterm->val2.type = DATA_SUB_EXP;

        nterm->val1.value.sub_expr = op1->data.nterm;
        nterm->val2.value.sub_expr = op2->data.nterm;
    }
    else
        return ERR_SYNTAX;

    // 3 operandy je pripadne potreba smazat
    if (op1->type == SYM_EXPR) exp_nterm_destroy(&op1->data.nterm);
    else if (op1->type == SYM_TERM) exp_data_destroy(&op1->data.term);
    if (op2->type == SYM_EXPR) exp_nterm_destroy(&op2->data.nterm);
    else if (op1->type == SYM_TERM) exp_data_destroy(&op2->data.term);
    if (op2->type == SYM_EXPR) exp_nterm_destroy(&op2->data.nterm);
    else if (op1->type == SYM_TERM) exp_data_destroy(&op2->data.term);
    
    // popnuti '<'
    exp_stack_pop(s);
    exp_stack_push(s, SYM_EXPR, NULL, NULL, nterm);
}

int exp_stack_shift(exp_stack_t *s, token_t *token)
{
    if (!s)
        return ERR_INTERNAL;

    //zasobnik pro odlozene neterminaly
    struct exp_stack temp_s; 
    int res = NO_ERR;
    if ((res = exp_stack_init(&temp_s)))    
        return res;

    struct exp_stack_item *top = exp_stack_top(s);
    while (top->type != SYM_TOKEN || top->type != SYM_TERM);
    {
        exp_stack_pop(s);
        exp_stack_push_item(&temp_s, top);
        top = exp_stack_top(s);
    }
    // top je ted nejvrchnejsi nalezeny terminal

    if (top->type != SYM_DOLLAR && top->type == SYM_EXPR)
    {
        // vlozeni '<' znaku za top terminal
        exp_stack_push(s, SYM_SHIFT, NULL, NULL, NULL);
        
        while(!exp_stack_isempty(&temp_s)){
            exp_stack_push_item(s, exp_stack_pop(&temp_s));
        }
        exp_stack_destroy(&temp_s);
    }
    else
        return ERR_SEMANT;


    exp_data_t new_item;
    enum exp_stack_symb sym_type = SYM_TERM;

    exp_data_init(&new_item, token);

    // push noveho tokenu ze vstupu
    exp_stack_push(s, sym_type, token, &new_item, NULL);
}

int precedence(token_t **token, enum data_type *data_t)
{
    int res = NO_ERR;
    struct exp_stack s;
    if ((res = exp_stack_init(&s)))    return res;
    if ((res = exp_stack_push(&s, SYM_DOLLAR, NULL, NULL, NULL)))   return res;

    do
    {
        // TODO:
        exp_item_t *top_item = exp_stack_top(&s);
        //ziskani patricne operace shift/redukce/.. z tabulky
        enum prec_type  index = get_index_from_item(*top_item);
        
        if(*table[index] == E)
        {
            // zavorka tokenem
            exp_data_t temp_data;
            exp_data_init(&temp_data, *token);
            exp_stack_push(&s, SYM_TOKEN, *token, &temp_data, NULL);
        }
        else if(*table[index] == S)
            res = exp_stack_shift(&s, *token);
        else if(*table[index] == R)
            res = exp_stack_reduce(&s);
        else 
            return ERR_SYNTAX;
    } while (exp_stack_top(&s)->type != SYM_DOLLAR);

    res = NO_ERR;

    exp_stack_destroy(&s);
    return res;
}

/* expression.c */
