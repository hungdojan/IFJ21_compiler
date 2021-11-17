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
#include "error.h"
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


extern FILE *global_file;

int syntax_analysis(FILE *file);

int prg(token_t **token);
int lof_e(token_t **token);
int lof_e_n(token_t **token);
int parm(token_t **token);
int parm_n(token_t **token);
int ret(token_t **token);
int def_parm(token_t **token);
int def_parm_n(token_t **token);
int code(token_t **token);
int var_init_assign(token_t **token);
int else_block(token_t **token);
int func_or_assign(token_t **token);
int multi_var(token_t **token);
int multi_var_n(token_t **token);
int expression(token_t **token);
int d_type(token_t **token);

#endif // _PARSER_H_
