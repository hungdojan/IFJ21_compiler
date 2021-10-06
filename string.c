#include <stdio.h>
#include <stdlib.h>

#include "string.h"


int string_Init(Istring *s)
{
    if(s == NULL)
        return 99;
    s->string = malloc(sizeof(char)*2);
    if(s->string == NULL)
        return 99;

    s->string[0] = '\0';
    s->allocSize = 0;
}

int string_Add_Char(Istring *s, char c)
{
    if(s == NULL)
        return 99;
    s->string = (char*) realloc(s->string, (s->allocSize + 1)  * sizeof(char));
    if(s->string == NULL)
        return 99;

    s->string[s->allocSize] = c;
    s->allocSize++;
}

int string_Free(Istring *s)
{
    if(s == NULL)
        return 99;
    free(s->string);
    s->allocSize = -1;
}



