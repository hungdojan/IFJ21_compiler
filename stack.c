/**
 * @file stack.c
 * @brief Definice funkci struktury rozsireneho zasobniku
 *
 * @authors Hung Do            (xdohun00)
 *          David Kedra        (xkedra00)
 *          Petr Kolarik       (xkolar79)
 *          Jaroslav Kvasnicka (xkvasn14)
 *
 * Reseni projektu IFJ a IAL, FIT VUT 21/22
 */

#include "stack.h"
#include "error.h"
#include <stdlib.h>

int stack_init(frame_stack *stack)
{
    if (stack != NULL)
    {
        stack->array = (node_ptr *)calloc(FRAME_STACK_SIZE, sizeof(node_ptr));
        if (stack->array == NULL)
            return ERR_INTERNAL;
        stack->alloc_size = FRAME_STACK_SIZE;
        stack->last_index = stack->curr_index = -1;
    }
    return 0;
}

int stack_push_frame(frame_stack *stack, const node_ptr root)
{
    if (stack != NULL)
    {
        if (stack->last_index + 1 >= stack->alloc_size)
        {
            void *temp = realloc(stack->array, stack->alloc_size * 2);
            if (temp == NULL)
                return ERR_INTERNAL;
            stack->array = temp;
            stack->alloc_size *= 2;
        }
        stack->array[++(stack->last_index)] = root;
        stack->curr_index = stack->last_index;
    }
    return 0;
}

node_ptr stack_pop(frame_stack *stack)
{
    node_ptr node = NULL;
    if (stack != NULL && stack->last_index >= 0)
    {
        node = stack->array[(stack->last_index)--];
        stack->curr_index = stack->last_index;
    }
    return node;
}

node_ptr stack_top(const frame_stack *stack)
{
    node_ptr node = NULL;
    if (stack != NULL && stack->curr_index >= 0)
        node = stack->array[stack->curr_index];
    return node;
}

int stack_isempty(const frame_stack *stack)
{
    if (stack != NULL && stack->curr_index >= 0)
        return 0;
    return 1;
}

void stack_reset_index(frame_stack *stack)
{
    if (stack != NULL)
        stack->curr_index = stack->last_index;
}

void stack_dec_index(frame_stack *stack)
{
    if (stack != NULL)
        (stack->curr_index)--;
}

void stack_destroy(frame_stack *stack, void (*clear_table_func)(node_ptr *root))
{
    if (stack != NULL)
    {
        for (int i = stack->last_index; i >= 0; i--)
            (*clear_table_func)(&stack->array[i]);
        free(stack->array);
    }
}

/* stack.c */
