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

static int esc_seq(Istring *s, FILE *f)
{
    if (f == NULL)
        return 99;
    int c = fgetc(f);
    if (c == 'n')       string_Add_Char(s, '\n');
    else if (c == 't')  string_Add_Char(s, '\t');
    else if (c == 'a')  string_Add_Char(s, '\a');
    else if (c == 'b')  string_Add_Char(s, '\b');
    else if (c == 'f')  string_Add_Char(s, '\f');

    else if (c == 'r')  string_Add_Char(s, '\r');
    else if (c == 'v')  string_Add_Char(s, '\v');
    else if (c == '\\') string_Add_Char(s, '\\');
    else if (c == '\"') string_Add_Char(s, '\"');
    else if (c == '\'') string_Add_Char(s, '\'');
    // format \xxx
    else if (isdigit(c))
    {
        int val = (c - '0') * 100;

        c = fgetc(f);
        if (!isdigit(c))  return 1;
        else              val += (c - '0') * 10;

        c = fgetc(f);
        if (!isdigit(c))  return 1;
        else              val += (c - '0');

        if (val >= 0 && val <= 255)     
            string_Add_Char(s, val);
        else
            return 1;
    }
    else                return 1;
    return 0;
}

int get_token(FILE *f, token_t **ref)
{
    int c = '\0';
    char tmp = '\0';
    Istring str;
    string_Init(&str);
    enum State state = STATE_NEW;
    enum Token_type type = TYPE_UNDEFINED;
    int loading = 1;

    if (f == NULL)      return 0;
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
                    /* FIXME
                     * Vyreseno jako operator - a cislo
                     * syntakticky analyzator rozpozna, zda se jedna
                     * o operaci minus nebo zaporne cislo
                    else if (isdigit(tmp))
                    {
                        // FIXME: token_integer_neg
                        string_Add_Char(&str, c);
                        ungetc(tmp, f);
                        state = STATE_INTEGER;
                    }
                    */
                    else
                    {
                        string_Add_Char(&str, c);
                        ungetc(tmp, f);
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
                    string_Add_Char(&str, c);
                    tmp = fgetc(f);
                    if (tmp == '/')
                    {
                        string_Add_Char(&str, tmp);
                        type = TYPE_DIVIDE_WHOLE;
                        state = STATE_RETURN_OPERATOR;
                    }
                    else
                    {
                        ungetc(tmp, f);
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
                    string_Add_Char(&str, c);
                    if (tmp == '=')
                    {
                        string_Add_Char(&str, tmp);
                        type = TYPE_GREATER_OR_EQ;
                        state = STATE_RETURN_OPERATOR;
                    }
                    else
                    {
                        ungetc(tmp, f);
                        type = TYPE_GREATER;
                        state = STATE_RETURN_OPERATOR;
                    }
                }
                else if (c == '<')
                {
                    tmp = fgetc(f);
                    string_Add_Char(&str, c);
                    if (tmp == '=')
                    {
                        string_Add_Char(&str, tmp);
                        type = TYPE_LESSER_OR_EQ;
                        state = STATE_RETURN_OPERATOR;
                    }
                    else
                    {
                        ungetc(tmp, f);
                        type = TYPE_LESSER;
                        state = STATE_RETURN_OPERATOR;
                    }
                }
                else if (c == '=')
                {
                    tmp = fgetc(f);
                    string_Add_Char(&str, c);
                    if (tmp == '=')
                    {
                        string_Add_Char(&str, tmp);
                        type = TYPE_EQ;
                        state = STATE_RETURN_OPERATOR;
                    }
                    else
                    {
                        ungetc(tmp, f);
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
                    if (tmp == '\"' || isalpha(tmp) || c == '_')
                    {
                        ungetc(tmp, f);
                        string_Add_Char(&str,c);
                        type = TYPE_STRLEN;
                        state = STATE_RETURN_OPERATOR;
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
                    state = STATE_STRING;
                }
                else if (isalpha(c) || c == '_')
                {
                    string_Add_Char(&str, c);
                    state = STATE_IDENTIFIER;
                }
                else if (isdigit(c))
                {
                    string_Add_Char(&str, c);
                    state = STATE_INTEGER;
                }
                else if (c == EOF)
                    loading = 0;
                else if (isspace(c)) { }
                else
                    goto error;
                break;
            case STATE_IDENTIFIER:
                if (isalpha(c) || c == '_' || isdigit(c))
                {
                    string_Add_Char(&str, c);
                }
                else
                {
                    ungetc(c, f);
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
                else if (isspace(c))
                {
                    ungetc(c, f);
                    type = TYPE_INTEGER;
                    state = STATE_RETURN_INTEGER;
                }
                else
                {
                    goto error;
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
                    ungetc(c, f);
                    type = TYPE_NUMBER;
                    state = STATE_RETURN_NUMBER;
                }
                break;
            case STATE_EXPONENT:
                if (isdigit(c) || c == '+' || c == '-')
                {
                    string_Add_Char(&str, c);
                    state = STATE_EXPONENT_FINISH;
                }
                else
                {
                    // ERROR - after E must come a digit
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
                    ungetc(c, f);
                    type = TYPE_NUMBER;
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
                    if (esc_seq(&str, f) != 0)
                        goto error;
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
                // GENERATE TOKEN WITH TYPE and VALUE + - * / // % ^ # 
                token_create(&str, type, ref);
                loading = 0;
                break;
            case STATE_RETURN_IDENTIFIER:
                // GENERATE TOKEN WITH TYPE identifier and VALUE _498r6wvs
                token_create(&str, type, ref);
                loading = 0;
                break;
            case STATE_RETURN_STRING:
                // GENERATE TOKEN WITH TYPE string and VALUE "xxxxx"
                token_create(&str, type, ref);
                loading = 0;
                break;
            case STATE_RETURN_INTEGER:
                // GENERATE TOKEN WITH TYPE integer and VALUE 8465151
                token_create(&str, type, ref);
                loading = 0;
                break;
            case STATE_RETURN_NUMBER:
                // GENERATE TOKEN WITH TYPE number and VALUE 89465.485, 654.648E6548
                token_create(&str, type, ref);
                loading = 0;
                break;
            default:
                break;
        }
    } while (c != EOF && loading);
    if (c == EOF)
    {
        string_Free(&str);
        return EOF;
    }
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
                    {
                        while ((c = fgetc(f)) != '\n' && c != EOF)
                            ; // preskakuje znak
                        is_comment = 0;
                    }
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
