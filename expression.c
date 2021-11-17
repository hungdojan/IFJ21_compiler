#include "token.h"
#include "expression.h"
#include "error.h"
#include <stdlib.h>

#define PREC_TABLE_SIZE 16

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
        case TYPE_NOT_EQ:            return TI_LOGIC_EQ;
        case TYPE_KW_AND:            return TI_AND;
        case TYPE_KW_OR:             return TI_OR;
        case TYPE_COMMA:             return TI_COMMA;
        case TYPE_IDENTIFIER:        return TI_ID;
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
            switch(item.data.term.type)
            {
                case DATA_ID:   return TI_ID;
                case DATA_STR:
                case DATA_INT:
                case DATA_NUM:
                case DATA_BOOL:
                case DATA_NIL:  return TI_CONST;
                default:        return -1;
            }
            return 0;
        default:
            return -1;
    }
}

static enum prec_type table[PREC_TABLE_SIZE][PREC_TABLE_SIZE] = {
    //   (  )  ^  #  !  *  +  .. >  =  &  |  ,  i  c  $
    {S, E, S, S, S, S, S, S, S, S, S, S, E, S, S, U}, // (
    {U, R, R, U, U, R, R, R, R, R, R, R, R, U, U, U}, // )
    {S, R, S, S, S, R, R, R, R, R, R, R, R, S, S, R}, // ^
    {S, R, S, U, S, R, R, R, R, R, R, R, R, S, S, R}, // #
    {S, R, S, S, S, R, R, R, R, R, R, R, R, S, S, R}, // not
    {S, R, S, S, S, R, R, R, R, R, R, R, R, S, S, R}, // * / // %
    {S, R, S, S, S, S, R, R, R, R, R, R, R, S, S, R}, // + - 
    {S, R, S, S, S, S, S, S, R, R, R, R, R, S, S, R}, // ..
    {S, R, S, S, S, S, S, S, R, R, R, R, R, S, S, R}, // > >= < <=
    {S, R, S, S, S, S, S, S, R, R, R, R, R, S, S, R}, // == ~=
    {S, R, S, S, S, S, S, S, S, S, S, R, S, S, S, R}, // and
    {S, R, S, S, S, S, S, S, S, S, S, R, R, S, S, R}, // or
    {S, E, S, S, S, S, S, S, S, S, S, S, E, S, S, R}, // ,
    {E, R, R, U, U, R, R, R, R, R, R, R, R, U, U, R}, // id
    {U, R, R, U, U, R, R, R, R, R, R, R, R, U, U, R}, // const
    {S, U, S, S, S, S, S, S, S, S, S, S, S, U, U, U}  // $
};

int exp_stack_reduce(exp_stack_t *s)
{
    if (s != NULL)
    {
    }
}

int exp_stack_shift(exp_stack_t *s)
{
}

int precedence(token_t **token)
{
    int res = NO_ERR;
    struct exp_stack s;
    if ((res = exp_stack_init(&s)))    return res;
    if ((res = exp_stack_push(&s, SYM_DOLLAR, NULL, NULL, NULL)))   return res;

    do
    {
        // TODO:
    } while (exp_stack_top(&s)->type != SYM_DOLLAR);
    res = NO_ERR;

    exp_stack_destroy(&s);
    return res;
}

/* expression.c */
