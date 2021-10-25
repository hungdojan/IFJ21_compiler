/**
 * @file stack.h
 * @brief Hlavickovy soubor rozsireneho zasobniku
 *
 * @authors Hung Do            (xdohun00)
 *          David Kedra        (xkedra00)
 *          Petr Kolarik       (xkolar79)
 *          Jaroslav Kvasnicka (xkvasn14)
 *
 * Reseni projektu IFJ a IAL, FIT VUT 21/22
 */

#ifndef _STACK_H_
#define _STACK_H_

#include "token.h"
#include <stdlib.h>

enum nonterm { /* TODO: */ undef };
struct stack_item_t;
struct stack_t;

typedef struct stack_item_t * StackItemPtr;

StackItemPtr init_stack_item(const enum Token_type, const enum nonterm);
void destroy_stack_item(StackItemPtr *);

int init_stack(struct stack_t *);
int push_stack(struct stack_t *, const StackItemPtr);
StackItemPtr pop_stack(struct stack_t *);
StackItemPtr top_stack(const struct stack_t *);
int stack_is_empty(const struct stack_t *);
void destroy_stack(struct stack_t *);

#endif // _STACK_H_
