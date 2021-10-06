/**
 * @file scanner.c
 * @brief Definice funkci lexikalni analyzy
 *
 * @authors Jaroslav Kvasnicka (xkvasn14)
 *          Hung Do            (xdohun00)
 *          David Kedra        (xkedra00)
 *          Petr Kolarik       (xkolar79)
 *
 * Reseni projektu IFJ a IAL, FIT VUT 21/22
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "scanner.h"
#include "istring.h"
#include "token.h"

int get_token(FILE *f)
{
    int c = '\0';
    char tmp = '\0';
    Istring str;
    string_Init(&str);
    state = STATE_NEW;
    type = TYPE_UNDEFINED;

    do
    {
        c = fgetc(f);

        switch (state)
        {
            case STATE_NEW:
                if (c == '+')
                {
                    string_Add_Char(&str, c);
                    type = TYPE_PLUS;
                    state = STATE_RETURN_OPERATOR;
                }
                else if (c == '-')
                {
                    tmp = fgetc(f);
                    if (tmp == '-')
                        c = get_rid_of_comments(f);
                    else if (isdigit(tmp))
                    {
                        // FIXME: token_integer_neg
                        string_Add_Char(&str, c);
                        ungetc(tmp, stdin);
                        state = STATE_INTEGER;
                    }
                    else
                    {
                        string_Add_Char(&str, c);
                        ungetc(tmp, stdin);
                        type = TYPE_MINUS;
                        state = STATE_RETURN_OPERATOR;
                    }
                }
                else if (c == '*')
                {
                    string_Add_Char(&str, c);
                    type = TYPE_MULTIPLY;
                    state = STATE_RETURN_OPERATOR;
                }
                else if (c == '/')
                {
                    tmp = fgetc(f);
                    if (tmp == '/')
                    {
                        string_Add_Char(&str, c);
                        string_Add_Char(&str, tmp);
                        type = TYPE_DIVIDE_WHOLE;
                        state = STATE_RETURN_OPERATOR;
                    }
                    else
                    {
                        string_Add_Char(&str, c);
                        ungetc(tmp, stdin);
                        type = TYPE_DIVIDE;
                        state = STATE_RETURN_OPERATOR;
                    }
                }
                else if (c == '%')
                {
                    string_Add_Char(&str, c);
                    type = TYPE_DIVIDE_MODULO;
                    state = STATE_RETURN_OPERATOR;
                }
                else if (c == '^')
                {
                    string_Add_Char(&str, c);
                    type = TYPE_POWER;
                    state = STATE_RETURN_OPERATOR;
                }
                else if (c == '>')
                {
                    tmp = fgetc(f);
                    if (tmp == '=')
                    {
                        string_Add_Char(&str, c);
                        string_Add_Char(&str, tmp);
                        type = TYPE_GREATER_OR_EQ;
                        state = STATE_RETURN_OPERATOR;
                    }
                    else
                    {
                        string_Add_Char(&str, c);
                        ungetc(tmp, stdin);
                        type = TYPE_GREATER;
                        state = STATE_RETURN_OPERATOR;
                    }
                }
                else if (c == '<')
                {
                    tmp = fgetc(f);
                    if (tmp == '=')
                    {
                        string_Add_Char(&str, c);
                        string_Add_Char(&str, tmp);
                        type = TYPE_LESSER_OR_EQ;
                        state = STATE_RETURN_OPERATOR;
                    }
                    else
                    {
                        string_Add_Char(&str, c);
                        ungetc(tmp, stdin);
                        type = TYPE_LESSER;
                        state = STATE_RETURN_OPERATOR;
                    }
                }
                else if (c == '=')
                {
                    tmp = fgetc(f);
                    if (tmp == '=')
                    {
                        string_Add_Char(&str, c);
                        string_Add_Char(&str, tmp);
                        type = TYPE_EQ;
                        state = STATE_RETURN_OPERATOR;
                    }
                    else
                    {
                        string_Add_Char(&str, c);
                        ungetc(tmp, stdin);
                        type = TYPE_ASSIGN;
                        state = STATE_RETURN_OPERATOR;
                    }
                }
                else if (c == '~')
                {
                    tmp = fgetc(f);
                    if (tmp == '=')
                    {
                        string_Add_Char(&str, c);
                        string_Add_Char(&str, tmp);
                        type = TYPE_NOT_EQ;
                        state = STATE_RETURN_OPERATOR;
                    }
                    else
                    {
                        //string_Add_Char(&str,c);
                        //ungetc(tmp,stdin);
                        // ERROR - can not do "~x" , x - any symbol
                        goto error;
                    }
                }
                else if (c == '#')
                {
                    tmp = fgetc(f);
                    if (tmp == '\"')
                    {
                        string_Add_Char(&str, c);
                        string_Add_Char(&str, tmp);
                        state = STATE_STRLEN_STRING;
                    }
                    else if (isalpha(tmp) || '_')
                    {
                        string_Add_Char(&str, c);
                        string_Add_Char(&str, tmp);
                        state = STATE_STRLEN_IDENTIFIER;
                    }
                    else
                    {
                        //string_Add_Char(&str,c);
                        //ungetc(tmp,stdin);
                        // ERROR - #123, #$&^@, ## - not valid definition
                        goto error;
                    }
                }
                else if (c == '(')
                {
                    string_Add_Char(&str, c);
                    type = TYPE_LEFT_PARENTHESES;
                    state = STATE_RETURN_OPERATOR;
                }
                else if (c == ')')
                {
                    string_Add_Char(&str, c);
                    type = TYPE_RIGHT_PARENTHESES;
                    state = STATE_RETURN_OPERATOR;
                }
                else if (c == ':')
                {
                    string_Add_Char(&str, c);
                    type = TYPE_DECLARE;
                    state = STATE_RETURN_OPERATOR;
                }
                else if (c == '.')
                {
                    tmp = fgetc(f);
                    if (tmp == '.')
                    {
                        string_Add_Char(&str, c);
                        string_Add_Char(&str, tmp);
                        type = TYPE_CONCAT;
                        state = STATE_RETURN_OPERATOR;
                    }
                    else
                    {
                        //string_Add_Char(&str,c);
                        //ungetc(tmp,stdin);
                        // ERROR - "." is not a valid definition
                        goto error;
                    }
                }
                else if (c == '\"')
                {
                    string_Add_Char(&str, c);
                    state = STATE_STRING;
                }
                else if (isalpha(c) || '_')
                {
                    string_Add_Char(&str, c);
                    state = STATE_IDENTIFIER;
                }
                else if (isdigit(c))
                {
                    string_Add_Char(&str, c);
                    state = STATE_INTEGER;
                }
                else if (isspace(c))
                {}
                else
                    goto error;
                break;
            case STATE_IDENTIFIER:
                if (isalpha(c) || '_' || isdigit(c))
                {
                    string_Add_Char(&str, c);
                }
                else
                {
                    ungetc(c, stdin);
                    type = TYPE_IDENTIFIER;
                    state = STATE_RETURN_IDENTIFIER;
                }
                break;
            case STATE_INTEGER:
                if (isdigit(c))
                {
                    string_Add_Char(&str, c);
                }
                else if (c == '.')
                {
                    string_Add_Char(&str, c);
                    state = STATE_NUMBER;
                }
                else if (c == 'e' || c == 'E')
                {
                    string_Add_Char(&str, c);
                    state = STATE_EXPONENT;
                }
                else
                {
                    ungetc(c, stdin);
                    type = TYPE_INTEGER;
                    state = STATE_RETURN_INTEGER;
                }
                break;
            case STATE_NUMBER:
                if (isdigit(c))
                {
                    string_Add_Char(&str, c);
                }
                else if (c == 'e' || c == 'E')
                {
                    string_Add_Char(&str, c);
                    state = STATE_EXPONENT;
                }
                else
                {
                    ungetc(c, stdin);
                    type = TYPE_NUMBER;
                    state = STATE_RETURN_NUMBER;
                }
                break;
            case STATE_EXPONENT:
                if (isdigit(c))
                {
                    string_Add_Char(&str, c);
                    state = STATE_EXPONENT_FINISH;
                }
                else if (c == '+' || c == '-')
                {
                    string_Add_Char(&str, c);
                    state = STATE_EXPONENT_CONTINUE;
                }
                else
                {
                    // ERROR - after E must come a digit
                    goto error;
                }
                break;
            case STATE_EXPONENT_CONTINUE:
                if (isdigit(c))
                {
                    string_Add_Char(&str, c);
                    state = STATE_EXPONENT_FINISH;
                }
                else
                {
                    // ERROR - after + or - must come a digit
                    goto error;
                }
                break;
            case STATE_EXPONENT_FINISH:
                if (isdigit(c))
                {
                    string_Add_Char(&str, c);
                }
                else
                {
                    ungetc(c, stdin);
                    type = TYPE_EXPONENT_NUMBER; // OR TYPE = TYPE_NUMBER;
                    state = STATE_RETURN_NUMBER;
                }
                break;
            case STATE_STRING:
                if (c == '\"')
                {
                    type = TYPE_STRING;
                    state = STATE_RETURN_STRING;
                }
                else if (c == '\\')
                {
                    // is_escape()
                }
                else
                {
                    string_Add_Char(&str, c);
                }
                break;
            case STATE_STRLEN_IDENTIFIER:
                if (isalpha(c) || '_' || isdigit(c))
                {
                    string_Add_Char(&str, c);
                }
                else
                {
                    ungetc(c, stdin);
                    type = TYPE_STRLEN_IDENTIFIER;
                    state = STATE_RETURN_STRLEN_IDENTIFIER;
                }
                break;
            case STATE_STRLEN_STRING:
                if (c == '\"')
                {
                    type = TYPE_STRLEN_STRING;
                    state = STATE_RETURN_STRLEN_STRING;
                }
                else if (c == '\\')
                {
                    // is_escape()
                }
                else
                {
                    string_Add_Char(&str, c);
                }
                break;
            default:
                break;
        }

        switch (state)
        {
            case STATE_RETURN_OPERATOR:
                // GENERATE TOKEN WITH TYPE and VALUE + - * / // % ^
                token_create(&str, type);
                string_Init(&str);
                state = STATE_NEW;
                type = TYPE_UNDEFINED;
                break;
            case STATE_RETURN_IDENTIFIER:
                // is_key_word()
                // GENERATE TOKEN WITH TYPE identifier and VALUE _498r6wvs
                token_create(&str, type);
                string_Init(&str);
                state = STATE_NEW;
                type = TYPE_UNDEFINED;
                break;
            case STATE_RETURN_STRING:
                // GENERATE TOKEN WITH TYPE string and VALUE "xxxxx"
                token_create(&str, type);
                string_Init(&str);
                state = STATE_NEW;
                type = TYPE_UNDEFINED;
                break;
            case STATE_RETURN_INTEGER:
                // GENERATE TOKEN WITH TYPE integer and VALUE 8465151
                token_create(&str, type);
                string_Init(&str);
                state = STATE_NEW;
                type = TYPE_UNDEFINED;
                break;
            case STATE_RETURN_NUMBER:
                // GENERATE TOKEN WITH TYPE number and VALUE 89465.485, 654.648E6548
                token_create(&str, type);
                string_Init(&str);
                state = STATE_NEW;
                type = TYPE_UNDEFINED;
                break;
            case STATE_RETURN_STRLEN_IDENTIFIER:
                // GENERATE TOKEN WITH TYPE STRLEN_IDENTIFIER and VALUE #_s651fe
                token_create(&str, type);
                string_Init(&str);
                state = STATE_NEW;
                type = TYPE_UNDEFINED;
                break;
            case STATE_RETURN_STRLEN_STRING:
                // GENERATE TOKEN WITH TYPE STRLEN_STRING and VALUE #"xxxxx"
                token_create(&str, type);
                string_Init(&str);
                state = STATE_NEW;
                type = TYPE_UNDEFINED;
                break;
            default:
                break;
        }
    } while (c != EOF);
    return 0;
error:
    string_Free(&str);
    return 1;
}

#define SINGLE_LINE_COMM 1
#define START_MULTI 2
#define MULTI_LINE_COMM 3
#define END_MULTI 4

int get_rid_of_comments(FILE *f)
{
    int c;                          // nacteny znak
    int is_comment = 1;             // pravdivostni hodnota, ze se stale nacita komentar
    int state = SINGLE_LINE_COMM;
    do
    {
        c = fgetc(f);
        switch (state)
        {
            case SINGLE_LINE_COMM:
                if (c == '[')        state = START_MULTI;
                else    // single line comment
                {
                    if (c != '\n' && c != EOF)
                        while ((c = fgetc(f)) != '\n' && c != EOF)
                            ; // preskakuje znak
                }
                break;
            case START_MULTI:
                if (c == '[')        state = MULTI_LINE_COMM;
                else    // single line comment
                {
                    if (c != '\n' && c != EOF)
                        while ((c = fgetc(f)) != '\n' && c != EOF)
                            ; // preskakuje znak
                }
                break;
            case MULTI_LINE_COMM:
                if (c == ']')        state = END_MULTI;
                break;
            case END_MULTI:
                if (c == ']')        is_comment = 0;
                else                 state = MULTI_LINE_COMM;
                break;
        }
    } while (c != EOF && is_comment);

    // vraci posledni nacteny znak
    return c;
}
/* scanner.c */
