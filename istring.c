/**
 * @file istring.c
 * @brief Definice retezcovych funkci
 *
 * @authors Hung Do            (xdohun00)
 *          David Kedra        (xkedra00)
 *          Petr Kolarik       (xkolar79)
 *          Jaroslav Kvasnicka (xkvasn14)
 *
 * Reseni projektu IFJ a IAL, FIT VUT 21/22
 */
#include <stdlib.h> // NULL, calloc, free, realloc
#include <string.h>
#include "istring.h"
#include "error.h"

int string_Init(Istring *s)
{
    if(s != NULL)
    {
        s->value = (char *)calloc(STRING_BASE_SIZE, sizeof(char));
        if(s->value == NULL)
            return ERR_INTERNAL;

        s->value[0] = '\0';
        s->length = 0;
        s->allocSize = STRING_BASE_SIZE;
    }
    return NO_ERR;
}

int string_Add_Char(Istring *s, char c)
{
    if(s != NULL)
    {
        // pridany znak presahuje velikost retezcu
        if (s->length + 1 >= s->allocSize)
        {
            void *temp = realloc(s->value, sizeof(char) * s->allocSize * 2);
            if (temp == NULL)
                return ERR_INTERNAL;

            s->value = temp;
            s->allocSize *= 2;
        } // s->length + 1 >= s->allocSize
        s->value[(s->length)++] = c;
    }
    return NO_ERR;
}

int string_compare(Istring *s1, Istring *s2)
{
    if (s1 != NULL && s2 != NULL)
        return strcmp(s1->value, s2->value);
    return ERR_INTERNAL;
}

void string_Free(Istring *s)
{
    if(s != NULL && s->value != NULL)
    {
        free(s->value);

        // potreba nullovat, aby nedoslo k opakovanemu pokusu o free
        s->value = NULL;
    }
}
/* istring.c */
