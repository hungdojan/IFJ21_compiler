/**
 * @file stack.c
 * @brief Definice funkci struktury specialniho zasobniku
 *
 * @authors Petr Kolarik       (xkolar79)
 *          Hung Do            (xdohun00)
 *          David Kedra        (xkedra00)
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
        /* TODO:
         * Naalokuje prostor pro pole zasobniku (stack->array) na velikost FRAME_STACK_SIZE
         *      - FRAME_STACK_SIZE je definovany v "stack.h"
         *      - pokud nechces resit inicializaci kazdeho prvni, pouzij misto malloc calloc
         *          - viz. manualova stranka - man calloc -> void *calloc(int pocet_prvku, int velikost_prvku)
         *      - v pripade neuspechu ihned vrati ERR_INTERNAL
         * Nastavi oba indexy na -1; alokovanou velikost na FRAME_STACK_SIZE
         * Vrati bezchybnou hodnotu (tedy 0)
         */
    }
    return 0;
}

int stack_push_frame(frame_stack *stack, const node_ptr root)
{
    if (stack != NULL)
    {
        /* TODO:
         * Nejprve zjisti, zda pridanim prvku nepresahne velikost alokovane pameti
         *  -> last_index + 1 >= alloc_size
         *  -> pokud ano, musi dojit k realokaci
         *      Proces realokace:
         *          void *ptr = realloc(ukazatel na pole, nova velikost);
         *          if (ptr != NULL)    // alokace se povedla
         *              ukazatel na pole = ptr;
         *              aloc size = nova velikost
         *          else
         *              vrat ERR_INTERNAL
         * nove realokovane pole musi byt 2x vetsi nez jeho predchudce
         *
         * priradi novy prvek na (root) na pozici last + 1
         * incrementuje last a nastavi curr na last
         * vrati bezchybny stav
         */
    }
    return 0;
}

node_ptr stack_pop(frame_stack *stack)
{
    if (stack != NULL)
    {
        /* TODO:
         * Pokud je zasobnik prazdny -> vrati NULL
         * Jinak vrati ptr na pozici last
         * Dekrementuje last; nastavi curr na last
         */
    }
    return NULL;
}

node_ptr stack_top(const frame_stack *stack)
{
    if (stack != NULL)
    {
        /* TODO:
         * Pokud je zasobnik prazdny -> vrati NULL
         * Jinak vrati ptr, na ktery ukazuje curr
         */
    }
    return NULL;
}

int stack_isempty(const frame_stack *stack)
{
    if (stack != NULL)
    {
        /* TODO:
         * Asi neni moc nad cim diskutovat
         * 1 == TRUE; 0 == FALSE
         */
    }
    return 1;
}

void stack_reset_index(frame_stack *stack)
{
    if (stack != NULL)
    {
        /* TODO:
         * Nastavi hodnotu curr na last
         */
    }
}

void stack_dec_index(frame_stack *stack)
{
    if (stack != NULL)
    {
        /* TODO:
         * Dekrementuje hodnotu stack
         * Pozor na krajni hodnoty
         */
    }
}

void stack_destroy(frame_stack *stack, void (*clear_table_func)(node_ptr *root))
{
    if (stack != NULL)
    {
        /* TODO:
         * Projde vsechny prvky pole a nad kazdym prvkem zavola fci clear_table_func
         * (*clear_table_func)(&stack->array[i])
         * uvolni pamet pro cele pole
         */
    }
}

/* stack.c */
