#ifndef _EXPRESSION_H_
#define _EXPRESSION_H_

#include "token.h"
#include "symtable.h"
#include "exp_stack.h"
#include <stdbool.h>

// reduce akce
/**
 * @brief 
 *
 * @param s Ukazatel na strukturu exp_stack_t
 * @return Chybovy kod
 */
int exp_stack_reduce(exp_stack_t *s);

// shift akce
/**
 * @brief 
 *
 * @param s Ukazatel na strukturu exp_stack_t
 * @param token
 * @return Chybovy kod
 */
int exp_stack_shift(exp_stack_t *s, token_t *token);

// bude prejmenovano na expression
/**
 * @brief 
 *
 * @param token
 * @param data_t
 * @return 
 */
int precedence(token_t **token, enum data_type *data_t);

#endif // _EXPRESSION_H_
