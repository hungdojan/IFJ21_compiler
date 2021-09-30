#include <stdio.h>
#include <stdlib.h>

#include "string.h"


void string_Init(Istring *s)
{
    if(s == NULL)
        return; // ERROR
    s->string = malloc(sizeof(char)*2);
    if(s->string == NULL)
        return; // ERROR

    s->string[0] = '\0';
    s->allocSize = 0;
}

void string_Add_Char(Istring *s, char c)
{
    if(s == NULL)
        return; // ERROR
    s->string = (char*) realloc(s->string, (s->allocSize + 1)  * sizeof(char));
    if(s->string == NULL)
        return; // ERROR

    s->string[s->allocSize] = c;
    s->allocSize++;
}

void string_Free(Istring *s)
{
    if(s == NULL)
        return; // ERROR
    free(s->string);
    s->allocSize = -1;
}



