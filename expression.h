#ifndef _EXPRESSION_H_
#define _EXPRESSION_H_

#include "token.h"
#include "symtable.h"
#include "exp_stack.h"
#include <stdbool.h>

// reduce akce
int exp_stack_reduce(exp_stack_t *s);

// shift akce
int exp_stack_shift(exp_stack_t *s, token_t *token);

// bude prejmenovano na expression
int precedence(token_t **token);

#endif // _EXPRESSION_H_
