#ifndef _EXPRESSION_H_
#define _EXPRESSION_H_

#include "error.h"
#include "token.h"
#include "symtable.h"
#include "exp_stack.h"
#include <stdbool.h>

#define PREC_TABLE_SIZE 14

#define PUSH_DATA_TO_EXP_STACK(stack, token, res)\
    do\
    {\
        if ((token)->type == TYPE_IDENTIFIER || (token)->type == TYPE_STRING ||     \
                (token)->type == TYPE_INTEGER || (token)->type == TYPE_NUMBER ||    \
                (token)->type == TYPE_BOOLEAN)                                      \
        {                                                                           \
            exp_data_t __temp_data;                                                 \
            if ((res = exp_data_init(&__temp_data, (token))) != NO_ERR)             \
                break;\
            if ((res = exp_stack_push(&stack, SYM_TERM, NULL, &__temp_data, NULL)) != NO_ERR) \
                break;\
        }                                                                           \
        else                                                                        \
        {                                                                           \
            if ((res = exp_stack_push(&stack, SYM_TOKEN, token, NULL, NULL)) != NO_ERR) \
                break;\
        }                                                                           \
    } while (0)

#define CLEAR_UP_IN_REDUCE(err_type)        \
    do                                      \
    {                                       \
        exp_stack_destroy_item(operand1);   \
        exp_stack_destroy_item(oper);       \
        exp_stack_destroy_item(operand2);   \
        exp_nterm_destroy(&new_nterm);      \
        return err_type;                    \
    } while (0)


#define SEMANTIC_CHECK_STR(new_nterm, nterm1, nterm2)  \
    do\
    {\
        if (nterm1->data_t == DATA_NIL || nterm2->data_t == DATA_NIL)       \
            CLEAR_UP_IN_REDUCE(ERR_UNEXP_VAL);                              \
        else if (nterm1->data_t == DATA_STR && nterm2->data_t == DATA_STR)  \
            new_nterm->data_t = DATA_STR;                                   \
        else                                                                \
            CLEAR_UP_IN_REDUCE(ERR_SYNTAX);                                 \
    } while (0)

#define SEMANTIC_CHECK_INT_AND_NUM(new_nterm, nterm1, nterm2, left_convert, right_convert)  \
    do\
    {\
        if (nterm1->data_t == DATA_NIL || nterm2->data_t == DATA_NIL)       \
            CLEAR_UP_IN_REDUCE(ERR_UNEXP_VAL);                              \
        else if (nterm1->data_t == DATA_INT && nterm2->data_t == DATA_INT)  \
            new_nterm->data_t = DATA_INT;                                   \
        else if (nterm1->data_t == DATA_NUM && nterm2->data_t == DATA_INT)  \
        {\
            right_convert = true;                                           \
            new_nterm->data_t = DATA_NUM;                                   \
        }\
        else if (nterm1->data_t == DATA_INT && nterm2->data_t == DATA_NUM)  \
        {\
            left_convert = true;                                            \
            new_nterm->data_t = DATA_NUM;                                   \
        }\
        else if (nterm1->data_t == DATA_NUM && nterm2->data_t == DATA_NUM)  \
            new_nterm->data_t = DATA_NUM;                                   \
        else    \
            CLEAR_UP_IN_REDUCE(ERR_SEM_TYPE);\
    } while (0)

#define SEMANTIC_CHECK_BOOL_NON_NIL(new_nterm, nterm1, nterm2) \
    do\
    {\
        if (nterm1->data_t == nterm2->data_t)     \
            new_nterm->data_t = DATA_BOOL;      \
        else if (nterm1->data_t == DATA_NIL || nterm2->data_t == DATA_NIL)  \
            CLEAR_UP_IN_REDUCE(ERR_UNEXP_VAL);  \
        else    \
            CLEAR_UP_IN_REDUCE(ERR_SEM_TYPE);\
    } while (0)

#define SEMANTIC_CHECK_BOOL_NIL(new_nterm, nterm1, nterm2) \
    do\
    {\
        if (nterm1->data_t == nterm2->data_t ||   \
                nterm1->data_t == DATA_NIL || nterm2->data_t == DATA_NIL) \
            new_nterm->data_t = DATA_BOOL;      \
        else    \
            CLEAR_UP_IN_REDUCE(ERR_SEM_TYPE);\
    } while (0)

#define CHECK_DIV_ZERO(data)   \
    do\
    {\
        if ((data.type == DATA_INT && data.value.integer == 0)  || \
            (data.type == DATA_NUM && data.value.number == 0.0))    \
            CLEAR_UP_IN_REDUCE(ERR_ZERO_DIV);   \
    } while (0)

extern unsigned reduc_id;

// reduce akce
int exp_stack_reduce(exp_stack_t *s);

// shift akce
int exp_stack_shift(exp_stack_t *s, token_t *token);

// bude prejmenovano na expression
int expression(token_t **token, enum data_type *data_t, exp_nterm_t **final_exp);

int generate_code_nterm(exp_nterm_t **expr);

#endif // _EXPRESSION_H_