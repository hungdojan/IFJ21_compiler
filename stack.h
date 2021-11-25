/**
 * @file stack.h
 * @brief Hlavickovy soubor zasobniku ukladajici tabulky symbolu
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

#include "symtable.h"
#include <stdlib.h>

#define FRAME_STACK_SIZE 8

typedef struct stack_t {
    node_ptr *array;    /// pole ukazatelu na tabulky symbolu
    int last_index;     /// index nejvrchnejsi tabulky symbolu
    int curr_index;     /// index momentalne vyhledavane tabulky symbolu
    int alloc_size;     /// velikost alokavane pameti pro array
} frame_stack;

extern frame_stack local_stack;

/**
 * @breif Inicializace zasobniku
 *
 * @param stack Ukazatel na zasobnik tabulek symbolu
 * @return Vraci nenulovou hodnotu, pokud nastane chyba pri behu funkce
 */
int stack_init(frame_stack *stack);

/**
 * @brief Prida ukazatel na tabulku symbolu
 *
 * @param stack Ukazatel na zasobnik tabulek symbolu
 * @param root Ukazatel na tabulku symbolu momentalniho scopu
 * @return Vraci nenulovou hodnotu, pokud nastane chyba pri behu funkce
 */
int stack_push_frame(frame_stack *stack, const node_ptr root);

/**
 * @brief Vyjmuti nejvrchnejsi tabulky symbolu
 *
 * @detail Vyjme nejvrchnejsi tabulku symbolu ze zasobniku a vrati jeji ukazatel uzivateli
 * @param stack Ukazatel na zasobnik tabulek symbolu
 * @return Ukazatel na nejvrchnejsi tabulku symbolu, NULL pokud je zasobnik prazdny
 */
node_ptr stack_pop(frame_stack *stack);

/**
 * @brief Nejvrchnejsi tabulka zasobniku
 *
 * @detail Vrati uzivateli ukazatel na tabulku symbolu, ktera bylo vlozena jako posledni
 * @param stack Ukazatel na zasobnik tabulek symbolu
 * @return Ukazatel na nejvrchnejsi tabulku symbolu, NULL pokud je zasobnik prazdny
 */
node_ptr stack_top(const frame_stack *stack);

/**
 * @brief Pravdivostni hodnota stavu zasobniku
 * 
 * @param stack Ukazatel na zasobnik tabulek symbolu
 * @return Vraci nenulovou hodnotu, pokud je zasobnik prazdny
 */
int stack_isempty(const frame_stack *stack);

/**
 * @brief Obnova ukazatel na momentalni scope
 *
 * @detail Vraci hodnotu ukazatele momentalniho scope na index nejvrchnejsi tabulky symbolu
 * @param stack Ukazatel na zasobnik tabulek symbolu
 */
void stack_reset_index(frame_stack *stack);

/**
 * @brief Zanoreni v zasobniku
 *
 * @detail Snizuje hodnotu indexu v poli tabulek symbolu o jednu nizsi
 * @param stack Ukazatel na zasobnik tabulek symbolu
 */
void stack_dec_index(frame_stack *stack);

/**
 * @brief Vyprazdnuje zasobnik
 * 
 * @param stack Ukazatel na zasobnik tabulek symbolu
 * @param clear_table_func Ukazatel na funkci cistici tabulku symbolu
 */
void stack_destroy(frame_stack *stack, void (*clear_table_func)(node_ptr *root));

#endif // _STACK_H_
