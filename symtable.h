/**
 * @file symtable.h
 * @brief Hlavickovy soubor tabulky symbolu
 *
 * @authors Hung Do            (xdohun00)
 *          David Kedra        (xkedra00)
 *          Petr Kolarik       (xkolar79)
 *          Jaroslav Kvasnicka (xkvasn14)
 *
 * Reseni projektu IFJ a IAL, FIT VUT 21/22
 */
#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

#include "istring.h"

enum data_type
{
    DATA_NIL=0,
    DATA_STR,
    DATA_INT,
    DATA_NUM,
    DATA_BOOL,
    DATA_SUB_EXP,
    DATA_ID,
};


// vycet moznych typu uzlu symtablu
typedef enum{
    VAR,
    FUNC,
    ROOT,
} item_type;

// node_ptr je ukazatel na 'struct tree_node' uzel, ktery je sam stromem
typedef struct tree_node
{
    char *key;                      /// Nazev identifikatoru
    item_type type;                 /// Typ identifikatoru
    
    int is_declared;                /// Bool, jestli byl deklarovan
    int is_defined;                 /// Bool, jestli byl definovan
    enum data_type var_type;
    int unlim_parms;       /// specialni pro write funkci
    Istring lof_params;         /// datove typy parametru
    Istring lof_rets;           /// navratove hodnoty parametru

    struct tree_node *left;         /// Ukazatel na levy podstrom (klice mensi)
    struct tree_node *right;        /// Ukazatel na pravy podstrom (klice vetsi)

} *node_ptr;

typedef struct item_var
{
    node_ptr var_node;
    struct item_var *next;
} item_var_t;

typedef struct stack_var
{
    item_var_t *top;
    int len;
} stack_var_t;

void item_var_destroy(item_var_t **item);
void stack_var_init(stack_var_t *stack);
int stack_var_push(stack_var_t *stack, node_ptr var_node);
item_var_t *stack_var_pop(stack_var_t *stack);
item_var_t *stack_var_top(stack_var_t *stack);
int stack_var_isempty(const stack_var_t stack);
void stack_var_destroy(stack_var_t *stack);

extern node_ptr global_tree;

/**
 * @brief Porovna typovou kompatibilitu mezi datovymi typy
 *
 * @param received Datovy typ vyrazu, ktery vola funkci
 * @oaram expected Datovy typ, ktery funkce ocekava
 * @return Nenulovou hodnotu pokud jsou nekompatibilni
 */
int type_control(enum data_type received, enum data_type expected);

/**
 * @brief Porovna typovou kompatibilitu mezi vice vyrazy
 *
 * @param received List datovych typu vyrazu, ktere volaji funkci
 * @oaram expected List datovych typu, ktere funkce ocekava
 * @param is_parm  Flag udavajici, zda se jedna o kontrolu parametru nebo navratovych hodnot
 * @return Nenulovou hodnotu pokud jsou nekompatibilni
 */
int lof_type_control(Istring *received, Istring *expected, int is_parm);

/**
 * @brief Inicializace stromu (frame), ktera vytvori specialni ROOT node s nealokovanym klicem NULL. 
 *        Bude-li strom uzivan i po tree_destroy, je potreba nejdrive volat tuto funkci
 *
 * @param tree Ukazatel na strom, ve kterem ma byt vytvoren pocatecni ROOT uzel
 * @return Error kod
*/
int tree_init(node_ptr *tree);

/**
 * @brief Vytvoreni a vlozeni noveho uzlu do stromu
 *
 * @param tree Ukazatel na strom (frame), do ktereho se ulozi novy uzel (identifikator)
 * @param key String klic pridavaneho uzlu
 * @param type Typ noveho uzlu
 * @param out  Vrati pres referenci nove vytvoreny uzel
 * @return Error kod
*/
int tree_insert(node_ptr *tree, char *key, item_type type, node_ptr *out);

/**
 * @brief Dealokace kompletniho stromu a bocnich podstromu
 *
 * @param tree Ukazatel na strom, ktery bude uvolnen a smazan
*/
void tree_destroy(node_ptr *tree);

/**
 * @brief Hledani uzlu/identifikatoru s konkretnim klicem
 *
 * @param tree Strom, ve kterem probiha hledani klice
 * @param key Hledany klic uzlu
 * @return Ukazatel na nalezeny uzel
*/
node_ptr tree_search(node_ptr tree, char *key);

#endif
/* symtable.h */
