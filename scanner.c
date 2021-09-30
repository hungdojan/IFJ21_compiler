#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "string.h"

void get_token()
{
    char c = '\0';
    char tmp = '\0';
    Istring tmpString;
    string_Init(&tmpString);

    while(1)
    {
        char c = getchar();
        if(c == EOF)
            return;

        switch(c)
        {
            case ':':
                printf("%c\n",c);
                break;
            case '=':
                tmp = getchar();
                if(tmp == '=')
                    printf("ITS A EQ\n");
                else{
                    printf("ITS AN ASSIGN\n");
                    ungetc(tmp,stdin);
                    }
                break;

        }
    }
}

/// params : nejaky token, jmeno/hodnota tokenu
void token_create()
{
    
}