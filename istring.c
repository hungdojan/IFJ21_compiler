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
#include "istring.h"

int string_Init(Istring *s)
{
    if(s != NULL)
    {
        s->value = (char *)calloc(STRING_BASE_SIZE, sizeof(char));
        if(s->value == NULL)
            return 99; // TODO: ERROR

        s->value[0] = '\0';
        s->length = 0;
        s->allocSize = STRING_BASE_SIZE;
    }
    return 0;
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
                return 99;

            s->value = temp;
            s->allocSize *= 2;
        } // s->length + 1 >= s->allocSize
        s->value[(s->length)++] = c;
    }
    return 0;
}

void string_Free(Istring *s)
{
    if(s != NULL)
        free(s->value);
}
/* istring.c */
