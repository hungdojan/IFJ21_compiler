/**
 * @file parser.h
 * @brief Hlavickovy soubor syntakticke analyzy
 *
 * @authors Hung Do            (xdohun00)
 *          David Kedra        (xkedra00)
 *          Petr Kolarik       (xkolar79)
 *          Jaroslav Kvasnicka (xkvasn14)
 *
 * Reseni projektu IFJ a IAL, FIT VUT 21/22
 */

#ifndef _PARSER_H_
#define _PARSER_H_

#include "generator.h"
#include "scanner.h"
#include "istring.h"
#include "token.h"
#include "symtable.h"
#include "error.h"
#include "exp_stack.h"
#include "stack.h"
#include <stdio.h>
#include <stdbool.h>

/*
 * Pokud neni nacteny token, pak se nove zavola dalsi
 */
#define INIT_TOKEN(token, res)  \
    do                          \
    {                           \
        if (!token) return ERR_INTERNAL;    \
        if (*token == NULL)                 \
        {                       \
            res = get_token(global_file, token);      \
            if (res != NO_ERR)  return res; \
        }                       \
    } while (0)                 \

#define INSERT_SYMBOL(id_val, node_type, node_return)   \
    do\
    {                                                   \
        if (node_type == FUNC)\
            res = tree_insert(&global_tree, id_val, node_type, &node_return); \
        else\
        {\
            stack_reset_index(&local_stack);       \
            node_ptr _node = stack_top(&local_stack);\
            res = tree_insert(&_node, id_val, node_type, &node_return);\
        }\
        if (res != NO_ERR)  return res;     \
        else                node_return->tof_index = glob_cnt;\
    } while (0)

#define SEARCH_SYMBOL(id_val, node_type, node_return) \
    do\
    {\
        if (node_type == FUNC)\
            node_return = tree_search(global_tree, id_val);\
        else\
        {\
            stack_reset_index(&local_stack);\
            while (!stack_isempty(&local_stack))   \
            {\
                node_return = tree_search(stack_top(&local_stack), id_val);\
                if (node_return != NULL && node_return->is_defined)    \
                    break;\
                else\
                    stack_dec_index(&local_stack);\
            }\
        }\
    } while(0)

#define INIT_SCOPE() \
    do\
    {\
        node_ptr __local_tree;\
        if((res = tree_init(&__local_tree)) != NO_ERR) return res;\
        stack_push_frame(&local_stack, __local_tree); \
    } while (0)

#define DESTROY_SCOPE()\
    do\
    {\
        if (!stack_isempty(&local_stack))\
        {\
            node_ptr __local_tree  = stack_pop(&local_stack);\
            tree_destroy(&__local_tree);\
        }\
    } while(0)

#define LOAD_TOKEN(token)                  \
    do                                     \
    {                                      \
        token_delete(token);               \
        if ((res = get_token(global_file, token)) != NO_ERR)\
            return res;     \
    } while (0)

#define LOAD_AND_CHECK(token, my_type)     \
    do                                     \
    {                                      \
        token_delete(token);               \
        if ((res = get_token(global_file, token)) != NO_ERR)\
            return res;     \
        if ((*token)->type != my_type)     \
            return ERR_SYNTAX;             \
    } while (0)

#define CHECK_AND_LOAD(token, my_type)     \
    do                                     \
    {                                      \
        if ((*token)->type == my_type)     \
        {                                  \
            token_delete(token);           \
            if ((res = get_token(global_file, token)) != NO_ERR)\
                return res; \
        }                                  \
        else                               \
            return ERR_SYNTAX;             \
    } while (0)

#define TYPE_CHECK_AND_CONVERT_NTERM(data_t, exp_str, index, final_exp, err)\
    do\
    {\
        if (type_control(data_t, (exp_str->value)[index - 1]))\
        {\
            exp_nterm_destroy(&final_exp);\
            return err;\
        }\
    } while(0)

#define TYPE_CHECK_AND_CONVERT_TERM(data_t, exp_type, err)\
    do\
    {\
        if (type_control(data_t, exp_type))\
            return err;\
    } while(0)

#define CONVERT_TO_FLOAT(data_t, exp_type)\
    do\
    {\
        if (data_t == DATA_INT && exp_type == DATA_NUM)\
            gen_code(q, INS_INT2FLOATS, NULL, NULL, NULL);\
    } while (0)

#define POP_TO_TMP(oper_type, oper, queue)\
    do\
    {\
        CLEAR_OPERAND(oper_type);\
        snprintf(oper, MAX_STR_LEN, "LF@%s_tmp1", glob_cnt.func_name);\
        gen_code(queue, INS_POPS, oper, NULL, NULL);\
    } while (0)

extern FILE *global_file;

/**
 * @brief 
 *
 * @param file
 * @return 
 */
int syntax_analysis(FILE *file);

/**
 * @brief Implementace <prg>
 *
 * @param token Posledni nacteny token
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int prg(token_t **token);

/**
 * @brief Implementace <lof_e>
 *
 * @param token Posledni nacteny token
 * @param node
 * @param index
 * @param is_parm
 * @param q
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int lof_e(token_t **token, node_ptr node, int *index, 
        bool is_parm, queue_t *q);

/**
 * @brief Implementace <lof_e_n>
 *
 * @param token Posledni nacteny token
 * @param node
 * @param index
 * @param is_parm
 * @param q
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int lof_e_n(token_t **token, node_ptr node, int *index, 
        bool is_parm, queue_t *q);

/**
 * @brief Implementace <parm> 
 *
 * @param token Posledni nacteny token
 * @param data 
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int parm(token_t **token, Istring *data);

/**
 * @brief Implementace <parm_n> 
 *
 * @param token Posledni nacteny token
 * @param data
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int parm_n(token_t **token, Istring *data, bool def_retval);

/**
 * @brief Implementace <ret> 
 *
 * @param token Posledni nacteny token
 * @param data
 * @param gen_code_print
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int ret(token_t **token, Istring *data, bool gen_code_print);

/**
 * @brief Implementace <ret_n> 
 *
 * @param token Posledni nacteny token
 * @param data
 * @param gen_code_print
 * @param index
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
// int ret_n(token_t **token, Istring *data, bool gen_code_print, int index);

/**
 * @brief Implementace <def_parm> 
 *
 * @param token Posledni nacteny token
 * @param data 
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int def_parm(token_t **token, Istring *data);

/**
 * @brief Implementace <def_parm> 
 *
 * @param token Posledni nacteny token
 * @param data 
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int def_parm_n(token_t **token, Istring *data);

/**
 * @brief Implementace <code> 
 *
 * @param token Posledni nacteny token
 * @param func_node
 * @param q
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int code(token_t **token, node_ptr *func_node, queue_t *q);

/**
 * @brief Implementace <var_init_assign> 
 *
 * @param token Posledni nacteny token
 * @param data_t
 * @param var_node
 * @param q
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int var_init_assign(token_t **token, node_ptr *var_node, queue_t *q);

/**
 * @brief Implementace <fun_or_exp> 
 *
 * @param token Posledni nacteny token
 * @param data_t
 * @param var_node
 * @param q
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int fun_or_exp(token_t **token, node_ptr *var_node, queue_t *q);

/**
 * @brief Implementace <elseif_block> 
 *
 * @param token Posledni nacteny token
 * @param func_node
 * @param q
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int elseif_block(token_t **token, node_ptr *func_node, queue_t *q, int end_index);

/**
 * @brief Implementace <else_block> 
 *
 * @param token Posledni nacteny token
 * @param func_node
 * @param q
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int else_block(token_t **token, node_ptr *func_node, queue_t *q);

/**
 * @brief Implementace <func_or_assign> 
 *
 * @param token Posledni nacteny token
 * @param node
 * @param q
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int func_or_assign(token_t **token, node_ptr *node, queue_t *q);

/**
 * @brief Implementace <multi_var_n> 
 *
 * @param token Posledni nacteny token
 * @param lof_vars
 * @param q
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int multi_var_n(token_t **token, stack_var_t *lof_vars, queue_t *q);

/**
 * @brief Implementace <fun_or_multi_e> 
 *
 * @param token Posledni nacteny token
 * @param lof_vars
 * @param q
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int fun_or_multi_e(token_t **token, stack_var_t *lof_vars, queue_t *q);

/**
 * @brief Implementace <multi_e_n> 
 *
 * @param token Posledni nacteny token
 * @param lof_vars
 * @param q
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int multi_e_n(token_t **token, stack_var_t *lof_vars, queue_t *q);

/**
 * @brief Implementace <d_type> 
 *
 * @param token Posledni nacteny token
 * @param data_t
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int d_type(token_t **token, enum data_type *data_t);

#endif // _PARSER_H_
