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
    {\
        if (node_type == FUNC)\
            res = tree_insert(&global_tree, id_val, node_type, &node_return); \
        else\
        {\
            node_ptr _node = stack_top(&local_stack);\
            res = tree_insert(&_node, id_val, node_type, &node_return);\
        }\
        if (res != NO_ERR)  return res;     \
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
                if (node_return != NULL)    \
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

extern FILE *global_file;

// typedef struct data_parser
// {
//     token_t **token;
//     node_ptr node;
// } data_parse_t;

int syntax_analysis(FILE *file);

int prg(token_t **token);
int lof_e(token_t **token, node_ptr node, int *index, bool is_parm, queue_t *q);
int lof_e_n(token_t **token, node_ptr node, int *index, bool is_parm, queue_t *q);
int parm(token_t **token, Istring *data);
int parm_n(token_t **token, Istring *data);
int ret(token_t **token, Istring *data, bool gen_code_print);
int ret_n(token_t **token, Istring *data, bool gen_code_print, int index);
int def_parm(token_t **token, Istring *data);
int def_parm_n(token_t **token, Istring *data);
int code(token_t **token, node_ptr *func_node, queue_t *q);
int var_init_assign(token_t **token, enum data_type *data_t, node_ptr *var_node);
int fun_or_exp(token_t **token, enum data_type *data_t, node_ptr *var_node);
int elseif_block(token_t **token, node_ptr *func_node);
int else_block(token_t **token, node_ptr *func_node);
int func_or_assign(token_t **token, node_ptr *node);
int multi_var_n(token_t **token, stack_var_t *lof_vars);
int fun_or_multi_e(token_t **token, stack_var_t *lof_vars);
int multi_e_n(token_t **token, stack_var_t *lof_vars);
int d_type(token_t **token, enum data_type *data_t);

#endif // _PARSER_H_
