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

enum data_type
{
    DATA_SUB_EXP,
    DATA_ID,
    DATA_STR,
    DATA_INT,
    DATA_NUM,
    DATA_BOOL,
    DATA_NIL
};

typedef struct item
{   
    void *data;             /// univerzalni data
    struct item *next;      /// ukazatel na dalsi polozku
} item_t;

typedef struct
{   
    item_t *first;  /// ukazatel na prvni prvek
    item_t *last;   /// ukazatel na posledni prvek
} list_t;

void list_init(list_t *list);
int list_insert(list_t *list, void *data);
item_t *list_get_first(list_t *list);
void list_destroy(list_t *list);

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
    
    int is_defined;                 /// Bool, jestli byl definovan
    int param_num;                  /// Pocet parametru funkce 

    struct tree_node *left;         /// Ukazatel na levy podstrom (klice mensi)
    struct tree_node *right;        /// Ukazatel na pravy podstrom (klice vetsi)

} *node_ptr;

extern node_ptr global_tree;

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
 * @return Error kod
*/
int tree_insert(node_ptr *tree, char *key, item_type type);

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
