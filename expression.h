#ifndef _EXPRESSION_H_
#define _EXPRESSION_H_

#include "error.h"
#include "token.h"
#include "symtable.h"
#include "parser.h"
#include "exp_stack.h"
#include "generator.h"
#include <stdbool.h>

#define PREC_TABLE_SIZE 14

#define PUSH_DATA_TO_EXP_STACK(stack, token, res)\
    do\
    {\
        if ((token)->type == TYPE_IDENTIFIER)\
        {\
            node_ptr __local_node = NULL;       \
            SEARCH_SYMBOL((token)->value.str_val, VAR, __local_node);\
            if (__local_node == NULL || !__local_node->is_defined)\
            {\
                res = ERR_SEM_DEF;\
                break;\
            }\
            exp_data_t __temp_data;                                                 \
            if ((res = exp_data_init(&__temp_data, (token))) != NO_ERR)             \
                break;\
            if ((res = exp_stack_push(&stack, SYM_TERM, NULL, &__temp_data, NULL)) != NO_ERR) \
                break;\
        }\
        else if ((token)->type == TYPE_STRING ||     \
                (token)->type == TYPE_INTEGER || (token)->type == TYPE_NUMBER ||    \
                (token)->type == TYPE_BOOLEAN || (token)->type == TYPE_KW_NIL)                                      \
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
            CLEAR_UP_IN_REDUCE(ERR_SEM_TYPE);                                 \
    } while (0)

#define SEMANTIC_CHECK_INT_AND_NUM(new_nterm, nterm1, nterm2, left_convert, right_convert)  \
    do\
    {\
        if (nterm1->data_t == DATA_NIL || nterm2->data_t == DATA_NIL)       \
            CLEAR_UP_IN_REDUCE(ERR_UNEXP_VAL);                              \
        else if (nterm1->data_t == DATA_INT && nterm2->data_t == DATA_INT)  \
            new_nterm->data_t = DATA_INT;                                   \
        else if ((nterm1->data_t == DATA_NUM && nterm2->data_t == DATA_INT) ||\
                 (nterm1->data_t == DATA_INT && nterm2->data_t == DATA_NUM) ||\
                 (nterm1->data_t == DATA_NUM && nterm2->data_t == DATA_NUM))  \
            new_nterm->data_t = DATA_NUM;                                   \
        else    \
            CLEAR_UP_IN_REDUCE(ERR_SEM_TYPE);\
    } while (0)

#define SEMANTIC_CHECK_BOOL_NON_NIL(new_nterm, nterm1, nterm2) \
    do\
    {\
        if (nterm1->data_t == DATA_NIL || nterm2->data_t == DATA_NIL)  \
            CLEAR_UP_IN_REDUCE(ERR_UNEXP_VAL);  \
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

#define SEMANTIC_GENERAL_TYPE_CHECK(nterm1, nterm2)\
    do\
    {\
        if ((nterm1->data_t == DATA_INT &&  nterm2->data_t == DATA_INT) || \
            (nterm1->data_t == DATA_NUM &&  nterm2->data_t == DATA_INT) ||\
            (nterm1->data_t == DATA_INT &&  nterm2->data_t == DATA_NUM) ||\
            (nterm1->data_t == DATA_NUM &&  nterm2->data_t == DATA_NUM) || \
            (nterm1->data_t == DATA_STR &&  nterm2->data_t == DATA_STR) || \
             nterm1->data_t == DATA_NIL ||  nterm2->data_t == DATA_NIL  || \
            (nterm1->data_t == DATA_BOOL && nterm2->data_t == DATA_BOOL))  \
        { }\
        else\
            CLEAR_UP_IN_REDUCE(ERR_SEM_TYPE);\
    } while(0)

#define CHECK_DIV_ZERO(data)   \
    do\
    {\
        if ((data.type == DATA_INT && data.value.integer == 0)  || \
            (data.type == DATA_NUM && data.value.number == 0.0))    \
            CLEAR_UP_IN_REDUCE(ERR_ZERO_DIV);   \
    } while (0)

#define IS_NOT_NIL(tmp_index, queue)\
    do\
    {\
        CLEAR_OPERAND(OPERAND_FIRST);\
        snprintf(_first, MAX_STR_LEN, "LF@$%s_tmp%d", glob_cnt.func_name, tmp_index);\
        gen_code(queue, INS_POPS, _first, NULL, NULL);\
        \
        CLEAR_OPERAND(OPERAND_DEST);\
        snprintf(_dest, MAX_STR_LEN, "%s&error_nil", glob_cnt.func_name);\
        gen_code(queue, INS_JUMPIFEQ, _dest, _first, "nil@nil");\
    } while (0)

#define IS_NOT_ZERO(tmp_index, queue)\
    do\
    {\
        CLEAR_OPERAND(OPERAND_FIRST);\
        snprintf(_first, MAX_STR_LEN, "LF@$%s_tmp%d", glob_cnt.func_name, tmp_index);\
        \
        CLEAR_OPERAND(OPERAND_DEST);\
        snprintf(_dest, MAX_STR_LEN, "%s&error_zero", glob_cnt.func_name);\
        gen_code(queue, INS_JUMPIFEQ, _dest, _first, "int@0");\
        gen_code(queue, INS_JUMPIFEQ, _dest, _first, "float@0x0p+0");\
    } while (0)

#define FLOAT_IF_NEEDED(op1, op2)\
    do\
    {\
        if (op1 == DATA_NUM && op2 == DATA_INT)\
        {\
            CLEAR_OPERAND(OPERAND_DEST);\
            snprintf(_dest, MAX_STR_LEN, "LF@$%s_tmp1", glob_cnt.func_name);\
            gen_code(q, INS_PUSHS, _dest, NULL, NULL);\
            \
            CLEAR_OPERAND(OPERAND_DEST);\
            snprintf(_dest, MAX_STR_LEN, "%sconvert%d", glob_cnt.func_name, ++glob_cnt.convert_i);\
            CLEAR_OPERAND(OPERAND_FIRST);\
            snprintf(_first, MAX_STR_LEN, "LF@$%s_tmp2", glob_cnt.func_name);\
            gen_code(q, INS_PUSHS, _first, NULL, NULL);\
            gen_code(q, INS_JUMPIFEQ, _dest, _first, "nil@nil");\
            gen_code(q, INS_INT2FLOATS, NULL, NULL, NULL);\
            *data_t = DATA_NUM;\
            \
            CLEAR_OPERAND(OPERAND_DEST);\
            snprintf(_dest, MAX_STR_LEN, "%sconvert%d", glob_cnt.func_name, glob_cnt.convert_i);\
            gen_code(q, INS_LABEL, _dest, NULL, NULL);\
        }\
        else if (op2 == DATA_NUM && op1 == DATA_INT)\
        {\
            CLEAR_OPERAND(OPERAND_DEST);\
            snprintf(_dest, MAX_STR_LEN, "%sconvert%d", glob_cnt.func_name, ++glob_cnt.convert_i);\
            CLEAR_OPERAND(OPERAND_FIRST);\
            snprintf(_first, MAX_STR_LEN, "LF@$%s_tmp1", glob_cnt.func_name);\
            gen_code(q, INS_PUSHS, _first, NULL, NULL);\
            gen_code(q, INS_JUMPIFEQ, _dest, _first, "nil@nil");\
            gen_code(q, INS_INT2FLOATS, NULL, NULL, NULL);\
            *data_t = DATA_NUM;\
            \
            CLEAR_OPERAND(OPERAND_DEST);\
            snprintf(_dest, MAX_STR_LEN, "LF@$%s_tmp2", glob_cnt.func_name);\
            gen_code(q, INS_PUSHS, _dest, NULL, NULL);\
            \
            CLEAR_OPERAND(OPERAND_DEST);\
            snprintf(_dest, MAX_STR_LEN, "%sconvert%d", glob_cnt.func_name, glob_cnt.convert_i);\
            gen_code(q, INS_LABEL, _dest, NULL, NULL);\
        }\
        else\
        {\
            CLEAR_OPERAND(OPERAND_DEST);\
            snprintf(_dest, MAX_STR_LEN, "LF@$%s_tmp1", glob_cnt.func_name);\
            gen_code(q, INS_PUSHS, _dest, NULL, NULL);\
            \
            CLEAR_OPERAND(OPERAND_DEST);\
            snprintf(_dest, MAX_STR_LEN, "LF@$%s_tmp2", glob_cnt.func_name);\
            gen_code(q, INS_PUSHS, _dest, NULL, NULL);\
            *data_t = op1;\
        }\
    } while (0)

#define INT_IF_NEEDED(op1, op2)\
    do\
    {\
        if (op1 == DATA_NUM && op2 == DATA_INT)\
        {\
            CLEAR_OPERAND(OPERAND_DEST);\
            snprintf(_dest, MAX_STR_LEN, "LF@$%s_tmp1", glob_cnt.func_name);\
            gen_code(q, INS_PUSHS, _dest, NULL, NULL);\
            gen_code(q, INS_FLOAT2INTS, NULL, NULL, NULL);\
            \
            CLEAR_OPERAND(OPERAND_DEST);\
            snprintf(_dest, MAX_STR_LEN, "LF@$%s_tmp2", glob_cnt.func_name);\
            gen_code(q, INS_PUSHS, _dest, NULL, NULL);\
            *data_t = DATA_INT;\
        }\
        else if (op2 == DATA_NUM && op1 == DATA_INT)\
        {\
            CLEAR_OPERAND(OPERAND_DEST);\
            snprintf(_dest, MAX_STR_LEN, "LF@$%s_tmp1", glob_cnt.func_name);\
            gen_code(q, INS_PUSHS, _dest, NULL, NULL);\
            \
            CLEAR_OPERAND(OPERAND_DEST);\
            snprintf(_dest, MAX_STR_LEN, "LF@$%s_tmp2", glob_cnt.func_name);\
            gen_code(q, INS_PUSHS, _dest, NULL, NULL);\
            gen_code(q, INS_FLOAT2INTS, NULL, NULL, NULL);\
            *data_t = DATA_INT;\
        }\
        else\
        {\
            CLEAR_OPERAND(OPERAND_DEST);\
            snprintf(_dest, MAX_STR_LEN, "LF@$%s_tmp1", glob_cnt.func_name);\
            gen_code(q, INS_PUSHS, _dest, NULL, NULL);\
            \
            CLEAR_OPERAND(OPERAND_DEST);\
            snprintf(_dest, MAX_STR_LEN, "LF@$%s_tmp2", glob_cnt.func_name);\
            gen_code(q, INS_PUSHS, _dest, NULL, NULL);\
            *data_t = DATA_INT;\
        }\
    } while (0)

extern unsigned reduc_id;

/**
 * @brief 
 *
 * @param s Ukazatel na strukturu exp_stack_t
 * @return Chybovy kod
 */
int exp_stack_reduce(exp_stack_t *s);

/**
 * @brief 
 *
 * @param s Ukazatel na strukturu exp_stack_t
 * @param token
 * @return Chybovy kod
 */
int exp_stack_shift(exp_stack_t *s, token_t *token);

/**
 * @brief 
 *
 * @param token
 * @param data_t
 * @param final_exp
 * @return 
 */
int expression(token_t **token, enum data_type *data_t, exp_nterm_t **final_exp);

/**
 * @brief 
 *
 * @param expr
 * @param q
 * @return 
 */
int generate_code_nterm(exp_nterm_t **expr, queue_t *q, enum data_type *data_t);

#endif // _EXPRESSION_H_
