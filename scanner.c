#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scanner.h"
#include "string.h"

int get_token()
{
    char c = '\0';
    char tmp = '\0';
    Istring str;
    string_Init(&str);

    while(1)
    {
        char c = getchar();
        string_Add_Char(&str,c);

        switch(state)
        {
            case STATE_NEW:
                if(c == '+')
                {
                    // type = type_plus
                    state = STATE_RETURN_OPERATOR;
                }
                else if(c == '-')
                {
                    tmp = getchar();
                    if(tmp == '-')
                    {
                        get_rid_of_comments();
                    }
                    else if(isdigit(tmp))
                    {
                        // NEGATIVE NUMBER
                    }
                    else
                    {
                        ungetc(tmp,stdin);
                        // type = type_minus
                        state = STATE_RETURN_OPERATOR;
                    }
                }
                else if(c == '*')
                {
                    // type = type_plus
                    state = STATE_RETURN_OPERATOR;
                }
                else if(c == '/')
                {
                    // type = type_plus
                    state = STATE_RETURN_OPERATOR;
                }
                else if(c == '%')
                {
                    // type = type_plus
                    state = STATE_RETURN_OPERATOR;
                }
                else if(c == '^')
                {
                    // type = type_plus
                    state = STATE_RETURN_OPERATOR;
                }
                else if(c == '>')
                    return 0;
                else if(c == '<')
                    return 0;
                else if(c == '=')
                    return 0;
                else if(c == '~')
                    return 0;
                else if(c == '#')
                    return 0;
                else if(c == '(')
                    return 0; // STATE RETURN PARANTS
                else if(c == ')')
                    return 0; // STATE RETURN PARANTS
                else if(c == ':')
                    return 0; // STATE RETURN ASSIGN
                else if(c == '\\')
                    return 0;
                else if(c == '\'')
                    return 0;
                else if(c == '\"')
                    return 0;
                else if(c == '\n')
                    return 0;
                else if(isaplha(c) || '_')
                    return 0;
                else if(isdigit(c))
                    return 0;
                else if(isspace(c))
                    return 0;
                else
                    return 1;
                break;





            case STATE_RETURN_OPERATOR:
                // GENERATE TOKEN WITH TYPE and VALUE + - * / // % ^
                break;
        }

    }
}


void get_rid_of_comments()
{

    if(getchar() == '[' && getchar() == '[')
    {
        while(1)
            if(getchar() == ']')
                if(getchar() == ']')
                    return;

    }
    else
    {
        while(1)
            if(getchar() == '\n')
                return;
    }
}