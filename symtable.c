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

void item_var_destroy(item_var_t **item)
{
    if (item != NULL && *item != NULL)
    {
        free(*item);
        *item = NULL;
    }
}

void stack_var_init(stack_var_t *stack)
{
    if (stack != NULL)
    {
        stack->top = NULL;
        stack->len = 0;
    }
}

int stack_var_push(stack_var_t *stack, node_ptr var_node)
{
    if (stack != NULL)
    {
        item_var_t *new_item = (item_var_t *)calloc(1, sizeof(item_var_t));
        if (new_item == NULL)
            return ERR_INTERNAL;
        new_item->next = NULL;
        new_item->var_node = var_node;
        item_var_t *temp = stack->top;
        stack->top = new_item;
        new_item->next = temp;
        stack->len += 1;
        return NO_ERR;
    }
    return ERR_INTERNAL;
}

item_var_t *stack_var_pop(stack_var_t *stack)
{
    if (stack != NULL)
    {
        item_var_t *top = stack->top;
        if (stack->top != NULL)
        {
            stack->top = stack->top->next;
            stack->len -= 1;
        }
        return top;
    }
    return NULL;
}

item_var_t *stack_var_top(stack_var_t *stack)
{
    if (stack != NULL)
        return stack->top;
    return NULL;
}

int stack_var_isempty(const stack_var_t stack)
{
    return !stack.len;
}

void stack_var_destroy(stack_var_t *stack)
{
    if (stack != NULL)
    {
        while (stack->top != NULL)
        {
            item_var_t *temp = stack->top->next;
            free(stack->top);
            stack->top = temp;
        }
        stack->top = NULL;
    }
}

int type_control(enum data_type received, enum data_type expected)
{
    // TODO:
    if (received == expected)       return NO_ERR;
    else if (received == DATA_NIL)  return NO_ERR;
    else if (received == DATA_INT && expected == DATA_NUM) return NO_ERR;
    return 1;
}

int tree_init(node_ptr *tree)
{
    if(tree)
    {
        // vytvoreni specialni ROOT node, ktery se po insertu jakekoli stane tim novym uzlem
        *tree = (node_ptr)calloc(1, sizeof(struct tree_node));
        if(!(*tree))
            return ERR_INTERNAL;

        (*tree)->key = NULL;
        (*tree)->left = NULL;
        (*tree)->right = NULL;
        // (*tree)->lof_params = NULL;
        // (*tree)->lof_rets = NULL;
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
        *tree = (node_ptr)calloc(1, sizeof(struct tree_node));
    }
    // jestli ma byt uzel rootem, je node_ptr jiz alokovan z tree_init
    
    if(*tree)
    {
        (*tree)->left = NULL;
        (*tree)->right = NULL;
        (*tree)->type = type;
        (*tree)->is_defined = (*tree)->is_declared = 0;
        // (*tree)->lof_params = (*tree)->lof_rets = NULL;
        (*tree)->var_type = DATA_NIL;
        
        (*tree)->key = (char *)calloc(1, sizeof(char)*(strlen(key)+1));
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
            string_Free(&(*tree)->lof_rets);
            string_Free(&(*tree)->lof_params);
            free(*tree);

            *tree = NULL;
        }
    }
}

node_ptr tree_search(node_ptr tree, char *key)
{
    if(!tree)
        return NULL;
    if (tree->type == ROOT)
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
