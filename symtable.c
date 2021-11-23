/**
 * @file symtable.c
 * @brief Definice funkci tabulky symbolu
 *
 * @authors Hung Do            (xdohun00)
 *          David Kedra        (xkedra00)
 *          Petr Kolarik       (xkolar79)
 *          Jaroslav Kvasnicka (xkvasn14)
 *
 * Reseni projektu IFJ a IAL, FIT VUT 21/22
 */
#include "istring.h"
#include "symtable.h"
#include "error.h"
#include <stdlib.h>
#include <string.h>

int type_control(enum data_type caller, enum data_type callee)
{
    // TODO:
    if (caller == callee)   return NO_ERR;
    return 1;
}

int lof_type_control(Istring *caller, Istring *callee)
{
    if (caller && callee)
    {
        // TODO:
    }
    return ERR_INTERNAL;
}

int tree_init(node_ptr *tree)
{
    if(tree)
    {
        // vytvoreni specialni ROOT node, ktery se po insertu jakekoli stane tim novym uzlem
        *tree = (node_ptr)malloc(sizeof(struct tree_node));
        if(!(*tree))
            return ERR_INTERNAL;

        (*tree)->key = NULL;
        (*tree)->left = NULL;
        (*tree)->right = NULL;
        (*tree)->lof_params = NULL;
        (*tree)->lof_rets = NULL;
        (*tree)->is_defined = (*tree)->is_declared = 0;
        (*tree)->var_type = DATA_NIL;

        (*tree)->type = ROOT; // staci odlisit pres NULLovy key
    }
    return 0;
}

int tree_insert(node_ptr *tree, char *key, item_type type, node_ptr *out)
{
    // ukazatel na root node, ani ona samotna nemuze byt null
    if(!tree)
        return ERR_INTERNAL;

    // pokud nema byt nova node root uzlem, provedeme hledani dal
    if(*tree && (*tree)->key != NULL)
    {
        while(*tree != NULL)
        {
            // hledani prislusne pozice noveho uzlu (tam kde je leaf NULL)
            if(strcmp(key, (*tree)->key) < 0)
                tree = &(*tree)->left;
            else if(strcmp(key, (*tree)->key) > 0)
                tree = &(*tree)->right;
            
            // klic existuje
            else
                return ERR_SEM_DEF;
        }
        // byl nalezen NULLovy leaf:

        // alokace a prirazeni noveho node
        *tree = (node_ptr)malloc(sizeof(struct tree_node));
    }
    // jestli ma byt uzel rootem, je node_ptr jiz alokovan z tree_init
    
    if(*tree)
    {
        (*tree)->left = NULL;
        (*tree)->right = NULL;
        (*tree)->type = type;
        (*tree)->is_defined = (*tree)->is_declared = 0;
        (*tree)->lof_params = (*tree)->lof_rets = NULL;
        (*tree)->var_type = DATA_NIL;
        
        (*tree)->key = (char *)malloc(sizeof(char)*(strlen(key)+1));
        if(!(*tree)->key)
            return ERR_INTERNAL;
        strcpy((*tree)->key, key);
        *out = *tree;
    }
    else    
    {
        *out = NULL;
        return ERR_INTERNAL;
    }
    return 0;
}

void tree_destroy(node_ptr *tree)
{
    if(tree && *tree)
    {
        if(*tree)
        {
            // je potreba uvolnit prave a leve uzly ze stejneho frame
            tree_destroy(&(*tree)->left);
            tree_destroy(&(*tree)->right);
        
            if((*tree)->key)
                free((*tree)->key);
            string_Free((*tree)->lof_rets);
            string_Free((*tree)->lof_params);
            free(*tree);

            *tree = NULL;
        }
    }
}

node_ptr tree_search(node_ptr tree, char *key)
{
    if(!tree)
        return NULL;

    while(tree != NULL)
    {
        // hledani ve frame funkce
        if(strcmp(key, tree->key) < 0)
            tree = tree->left;
        else if(strcmp(key, tree->key) > 0)
            tree = tree->right;
        else
            // klic nalezen
            return tree;
    }

    // identifikator nebyl nalezen v local_frame toho stromu
    return NULL;
}

/* symtable.c */
