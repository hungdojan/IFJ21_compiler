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

#include "token.h"
#include "symtable.h"
#include "error.h"
#include "exp_stack.h"
#include <stdio.h>

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

#define INSERT_SYMBOL(id_val, node_type)   \
    do\
    {\
        if (node_type == FUNC)\
            res = tree_insert(&global_tree, id_val, node_type); \
        else\
            res = tree_insert(stack_top(&local_stack), id_val, node_type);\
        if (res != NO_ERR)  return res;     \
    } while (0)

#define SEARCH_SYMBOL(id_val, node_return, err_type, node_type) \
    do\
    {\
        if (node_type == FUNC)\
            node_return = tree_search(&global_tree, id_val);\
        else\
        {\
            while (stack_isempty(&local_stack))   \
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
        node_ptr __local_tree  = stack_pop_frame(&local_stack);\
        tree_destroy(&__local_tree);\
    } while(0)
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
 * @param lof_data 
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int lof_e(token_t **token, list_t *lof_data);

/**
 * @brief Implementace <lof_e_n>
 *
 * @param token Posledni nacteny token
 * @param lof_data 
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int lof_e_n(token_t **token, list_t *lof_data);

/**
 * @brief Implementace <parm> 
 *
 * @param token Posledni nacteny token
 * @param lof_data 
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int parm(token_t **token, list_t *lof_data);

/**
 * @brief Implementace <parm_n> 
 *
 * @param token Posledni nacteny token
 * @param lof_data 
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int parm_n(token_t **token, list_t *lof_data);

/**
 * @brief Implementace <ret> 
 *
 * @param token Posledni nacteny token
 * @param lof_data 
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int ret(token_t **token, list_t *lof_data);

/**
 * @brief Implementace <def_parm> 
 *
 * @param token Posledni nacteny token
 * @param lof_data 
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int def_parm(token_t **token, list_t *lof_data);

/**
 * @brief Implementace <def_parm_n> 
 *
 * @param token Posledni nacteny token
 * @param lof_data 
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int def_parm_n(token_t **token, list_t *lof_data);

/**
 * @brief Implementace <expression> 
 *
 * @param token Posledni nacteny token
 * @param data_t 
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int expression(token_t **token, enum data_type *data_t);

/**
 * @brief Implementace <code> 
 *
 * @param token Posledni nacteny token
 * @param lof_data 
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int code(token_t **token, list_t *lof_data);

/**
 * @brief Implementace <var_init_assign> 
 *
 * @param token Posledni nacteny token
 * @param data_t
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int var_init_assign(token_t **token, enum data_type *data_t);

/**
 * @brief Implementace <else_block> 
 *
 * @param token Posledni nacteny token
 * @param lof_data 
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int else_block(token_t **token, list_t *lof_data);

/**
 * @brief Implementace <func_or_assign> 
 *
 * @param token Posledni nacteny token
 * @param lof_data 
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int func_or_assign(token_t **token, list_t *lof_data);

/**
 * @brief Implementace <multi_var> 
 *
 * @param token Posledni nacteny token
 * @param lof_data 
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int multi_var(token_t **token, list_t *lof_data);

/**
 * @brief Implementace <multi_var_n> 
 *
 * @param token Posledni nacteny token
 * @param lof_data 
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int multi_var_n(token_t **token, list_t *lof_data);

/**
 * @brief Implementace <multi_e> 
 *
 * @param token Posledni nacteny token
 * @param lof_data 
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int multi_e(token_t **token, list_t *lof_data);

/**
 * @brief Implementace <multi_e_n> 
 *
 * @param token Posledni nacteny token
 * @param lof_data 
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int multi_e_n(token_t **token, list_t *lof_data);

/**
 * @brief Implementace <d_type> 
 *
 * @param token Posledni nacteny token
 * @param data_t
 * @return Nenulove cislo v pripade, ze dojde k chybe nebo nastane konec programu
 */
int d_type(token_t **token, enum data_type *data_t);

#endif // _PARSER_H_
