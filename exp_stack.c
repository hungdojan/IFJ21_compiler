#include <string.h>
#include "exp_stack.h"
#include "error.h"
#include <stdlib.h>

int exp_nterm_init(exp_nterm_t **n)
{
    if (n != NULL)
    {
        *n = (exp_nterm_t *)calloc(1, sizeof(exp_nterm_t));
        if (*n == NULL)     return ERR_INTERNAL;
        // val1 rule a val2 jsou explicitne definovane
        (*n)->parms = NULL;
        (*n)->parm_len = 0;
        (*n)->parm_alloc_size = 0;
        return NO_ERR;
    }
    return ERR_INTERNAL;
}

int exp_data_str_init(exp_data_t *data, char *value, enum data_type type)
{
    if (data != NULL)
    {
        if (type == DATA_STR)
        {
            data->value.string = (char *)calloc(strlen(value) + 1, 1);
            if (data->value.string == NULL)
                return ERR_INTERNAL;
            strcpy(data->value.string, value);
        }
        else if (type == DATA_ID)
        {
            data->value.id = (char *)calloc(strlen(value) + 1, 1);
            if (data->value.id == NULL)
                return ERR_INTERNAL;
            strcpy(data->value.id, value);
        }
        else 
            return ERR_INTERNAL;
    }
    return ERR_INTERNAL;
}

void exp_data_destroy(exp_data_t *data)
{
    if (data != NULL)
    {
        if (data->type == DATA_STR)
        {
            free(data->value.string);
            data->value.string = NULL;
        }
        else if (data->type == DATA_ID)
        {
            free(data->value.id);
            data->value.id = NULL;
        }
        else if (data->type == DATA_SUB_EXP)
        {
            exp_nterm_destroy(&(data->value.sub_expr));
            data->value.sub_expr = NULL;
        }
    }
}

void exp_nterm_destroy(exp_nterm_t **n)
{
    if (n != NULL && *n != NULL)
    {
        // ruseni val1
        if ((*n)->val1.type == DATA_SUB_EXP)
            exp_data_destroy(&(*n)->val1);
        // ruseni val2
        if ((*n)->val2.type == DATA_SUB_EXP)
            exp_data_destroy(&(*n)->val2);
        // ruseni parametru fci
        for (int i = 0; i < (*n)->parm_len; i++)
        {
            if (((*n)->parms)[i].type == DATA_SUB_EXP)
            {
                exp_nterm_destroy(&((*n)->parms)[i].value.sub_expr);
                ((*n)->parms)[i].value.sub_expr = NULL;
            }
        }
        free(*n);
        *n = NULL;
    }
}

// inicializace zasobniku
int exp_stack_init(exp_stack_t *s)
{
    if (s != NULL)
    {
        s->array = (exp_item_t *)calloc(EXP_STACK_SIZE, sizeof(exp_item_t));
        if (s->array == NULL)   return ERR_INTERNAL;
        s->alloc_size = EXP_STACK_SIZE;
        s->len = 0;
    }
    return ERR_INTERNAL;
}

// pridani prvku do zasobniku
int exp_stack_push(exp_stack_t *s, enum exp_stack_symb sym, 
        token_t *token, exp_data_t *data, exp_nterm_t *nterm)
{
    if (s != NULL)
    {
        // zvetseni velikosti zasobniku
        if ((s->len + 1) >= s->alloc_size)
        {
            void *p = realloc(s->array, sizeof(exp_item_t) * s->alloc_size * 2);
            if (p == NULL)  return ERR_INTERNAL;
            s->array = p;
            s->alloc_size *= 2;
        }
        (s->array)[s->len].type = sym;
        switch(sym)
        {
            case SYM_EXPR:  // E
                (s->array)[s->len].data.nterm = nterm;
                break;
            case SYM_TERM:  // operand
                if (data == NULL)   return ERR_INTERNAL;
                (s->array)[s->len].data.term = *data;
                break;
            case SYM_TOKEN: // operator
                if (token == NULL)  return ERR_INTERNAL;
                (s->array)[s->len].data.oper = token->type;
                break;
                // < $
            case SYM_SHIFT: case SYM_DOLLAR:
                break;
        }
        s->len++;
        return NO_ERR;
    }
    return ERR_INTERNAL;
}

// vraci prvni token zpatky
exp_item_t *exp_stack_top(const exp_stack_t *s)
{
    if (s != NULL)
    {
        // vrati prvni neterm
        for (int i = s->len - 1; i >= 0; i--)
        {
            if ((s->array)[i].type != SYM_EXPR)
                return &(s->array)[i];
        }
    }
    return NULL;
}

// vrati prvni vec na zasobniku
exp_item_t *exp_stack_pop(exp_stack_t *s)
{
    if (s != NULL)
        return  &(s->array)[--(s->len)];
    return NULL;
}

int exp_stack_isempty(const exp_stack_t *s)
{
    if (s != NULL)
        return !(s->len) || s->len == -1;
    return 1;
}

// vlozi parametr fce
int exp_nterm_add_parameter(exp_nterm_t *func, exp_data_t parm)
{
    if (func != NULL)
    {
        if (func->parms == NULL)
        {
            func->parms = (exp_data_t *)calloc(EXP_FUNC_PARM_SIZE, sizeof(exp_data_t));
            if (func->parms == NULL)    return ERR_INTERNAL;
            func->parm_len = 0;
            func->parm_alloc_size = EXP_FUNC_PARM_SIZE;
        } 
        else if (func->parm_len + 1 > func->parm_alloc_size)
        {
            void *p = realloc(func->parms, sizeof(exp_data_t) * func->parm_alloc_size * 2);
            if (p == NULL)  return ERR_INTERNAL;
            func->parms = p;
            func->parm_alloc_size *= 2;
        }
        (func->parms)[func->parm_len] = parm;
        func->parm_len++;
    }
    return ERR_INTERNAL;
}

// odstraneni zasobniku a uvolneni alokovane pameti
void exp_stack_destroy(exp_stack_t *s)
{
    if (s != NULL && s->array != NULL)
    {
        for (int i = s->len-1; i >= 0; i--)
        {
            if ((s->array)[i].type == SYM_EXPR)
                exp_nterm_destroy(&(s->array)[i].data.nterm);
            else if ((s->array)[i].type == SYM_EXPR)
                exp_data_destroy(&(s->array)[i].data.term);
            free(s->array);
            s->array = NULL;
            s->len = -1;
            s->alloc_size = 0;
        }
    }
}

/* exp_stack.c */
