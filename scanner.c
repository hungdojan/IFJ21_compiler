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
#include "error.h"
#include <stdlib.h>

static void fxxx(Istring *str, int c)
{
    string_Add_Char(str, c / 100 % 10 + '0');
    string_Add_Char(str, c / 10 % 10 + '0');
    string_Add_Char(str, c / 1 % 10 + '0');
}

static void fxx(Istring *s, FILE *f)
{
    char c[3] = {0,};
    c[0] = fgetc(f);
    c[1] = fgetc(f);
    c[2] = 0;
    int number = strtol(c,NULL,16);
    fxxx(s,number);
}


static int esc_seq(Istring *s, FILE *f)
{
    if (f == NULL)
        return 99;
    int c = fgetc(f);
    if (c == 'n')       {c = '\n';fxxx(s,c);}
    else if (c == 't')  {c = '\t';fxxx(s,c);}
    else if (c == 'a')  {c = '\a';fxxx(s,c);}
    else if (c == 'b')  {c = '\b';fxxx(s,c);}
    else if (c == 'f')  {c = '\f';fxxx(s,c);}
    else if (c == 'r')  {c = '\r';fxxx(s,c);}
    else if (c == 'v')  {c = '\v';fxxx(s,c);}
    else if (c == '\\') {c = '\\';fxxx(s,c);}
    else if (c == '\"') {c = '\"';fxxx(s,c);}
    else if (c == '\'') {c = '\'';fxxx(s,c);}
    else if (c == 'x') fxx(s,f);
    else if (isdigit(c))
    {/*
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
            return 1;*/
        string_Add_Char(s,c); // TODO: pokud neni 3x digit tak chyba
        c = fgetc(f);
        string_Add_Char(s,c);
        c = fgetc(f);
        string_Add_Char(s,c);
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
                    state = STATE_END_LOAD;
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
                        state = STATE_END_LOAD;
                    }
                }
                else if (c == '*')
                {
                    string_Add_Char(&str, c);
                    type = TYPE_MULTIPLY;
                    state = STATE_END_LOAD;
                }
                else if (c == '/')
                {
                    string_Add_Char(&str, c);
                    tmp = fgetc(f);
                    if (tmp == '/')
                    {
                        string_Add_Char(&str, tmp);
                        type = TYPE_DIVIDE_WHOLE;
                        state = STATE_END_LOAD;
                    }
                    else
                    {
                        ungetc(tmp, f);
                        type = TYPE_DIVIDE;
                        state = STATE_END_LOAD;
                    }
                }
                else if (c == '%')
                {
                    string_Add_Char(&str, c);
                    type = TYPE_DIVIDE_MODULO;
                    state = STATE_END_LOAD;
                }
                else if (c == '^')
                {
                    string_Add_Char(&str, c);
                    type = TYPE_POWER;
                    state = STATE_END_LOAD;
                }
                else if (c == '>')
                {
                    tmp = fgetc(f);
                    string_Add_Char(&str, c);
                    if (tmp == '=')
                    {
                        string_Add_Char(&str, tmp);
                        type = TYPE_GREATER_OR_EQ;
                        state = STATE_END_LOAD;
                    }
                    else
                    {
                        ungetc(tmp, f);
                        type = TYPE_GREATER;
                        state = STATE_END_LOAD;
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
                        state = STATE_END_LOAD;
                    }
                    else
                    {
                        ungetc(tmp, f);
                        type = TYPE_LESSER;
                        state = STATE_END_LOAD;
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
                        state = STATE_END_LOAD;
                    }
                    else
                    {
                        ungetc(tmp, f);
                        type = TYPE_ASSIGN;
                        state = STATE_END_LOAD;
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
                        state = STATE_END_LOAD;
                    }
                    else
                    {
                        // ERROR - can not do "~x" , x - any symbol
                        //goto error;
                        return print_error(ERR_LEX, &str, NULL, "spatny format operatoru ~");
                    }
                }
                else if (c == ',')
                {
                    string_Add_Char(&str, c);
                    type = TYPE_COMMA;
                    state = STATE_END_LOAD;
                }
                else if (c == '#')
                {
                    // samostatny operator pro zjisteni delky retezce
                    string_Add_Char(&str, c);
                    type = TYPE_STRLEN;
                    state = STATE_END_LOAD;
                }
                else if (c == '(')
                {
                    string_Add_Char(&str, c);
                    type = TYPE_LEFT_PARENTHESES;
                    state = STATE_END_LOAD;
                }
                else if (c == ')')
                {
                    string_Add_Char(&str, c);
                    type = TYPE_RIGHT_PARENTHESES;
                    state = STATE_END_LOAD;
                }
                else if (c == ':')
                {
                    string_Add_Char(&str, c);
                    type = TYPE_DECLARE;
                    state = STATE_END_LOAD;
                }
                else if (c == '.')
                {
                    tmp = fgetc(f);
                    if (tmp == '.')
                    {
                        string_Add_Char(&str, c);
                        string_Add_Char(&str, tmp);
                        type = TYPE_CONCAT;
                        state = STATE_END_LOAD;
                    }
                    else
                    {
                        // ERROR - "." is not a valid definition
                        //goto error;
                        return print_error(ERR_LEX, &str, NULL, "chybejici operand konkatenace");
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
                {
                    type = TYPE_EOF;
                    state = STATE_END_LOAD;
                }
                else if (c == '\n')
                    file_line++;
                else if (isspace(c)) { }
                else
                    return print_error(ERR_LEX, &str, NULL, "neplatny znak \"%c\"", c);
                    //goto error;
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
                    state = STATE_END_LOAD;
                }
                break;
            case STATE_INTEGER:
                if (isdigit(c))
                {
                    string_Add_Char(&str, c);
                }
                else if (c == '.')
                {
                    tmp = fgetc(f);
                    // non-digit character after floating-point sign
                    if (!isdigit(tmp))
                    {
                        ungetc(tmp, f);
                        //goto error;
                        return print_error(ERR_LEX, &str, NULL, "chybny format desetinneho cisla");
                    }
                    string_Add_Char(&str, c);
                    string_Add_Char(&str, tmp);
                    state = STATE_NUMBER;
                }
                else if (c == 'e' || c == 'E')
                {
                    string_Add_Char(&str, c);
                    state = STATE_EXPONENT;
                }
                else
                {
                    ungetc(c, f);
                    type = TYPE_INTEGER;
                    state = STATE_END_LOAD;
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
                    state = STATE_END_LOAD;
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
                    //goto error;
                    return print_error(ERR_LEX, &str, NULL, "neplatna hodnota v exponentu po znaku E");
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
                    state = STATE_END_LOAD;
                }
                break;
            case STATE_STRING:
                if (c == '\"')
                {
                    type = TYPE_STRING;
                    state = STATE_END_LOAD;
                }
                else if (c == '\\')
                {
                    string_Add_Char(&str,'\\');
                    if (esc_seq(&str, f) != 0)
                        return print_error(ERR_LEX, &str, NULL, "chybna escape sekvence stringu");
                        //goto error;
                }
                else
                {
                    if(c <= 32)
                    {
                        string_Add_Char(&str,'\\');
                        string_Add_Char(&str, c / 100 % 10 + '0');
                        string_Add_Char(&str, c / 10 % 10 + '0');
                        string_Add_Char(&str, c / 1 % 10 + '0');
                    }
                    else
                        string_Add_Char(&str,c);
                }
                break;
            default:
                break;
        }

        switch (state)
        {
            case STATE_END_LOAD:
                token_create(&str, type, ref);
                loading = 0;
                break;
            default:
                break;
        }
    } while (loading);
    // if (c == EOF)
    // {
    //     string_Free(&str);
    //     return EOF;
    // }
    return NO_ERR;
/*error:
    string_Free(&str);
    return 1;
*/
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
                    file_line++;
                }
                break;
            case START_MULTI:
                if (c == '[')        state = MULTI_LINE_COMM;
                else    // single line comment
                {
                    if (c != '\n' && c != EOF)
                        while ((c = fgetc(f)) != '\n' && c != EOF)
                        {
                            if (c == '\n')
                                file_line++;
                            ; // preskakuje znak
                        }
                }
                break;
            case MULTI_LINE_COMM:
                if (c == ']')        state = END_MULTI;
                else if (c == '\n')  file_line++;
                break;
            case END_MULTI:
                if (c == ']')        is_comment = 0;
                else if (c == ']')   { file_line++; state = MULTI_LINE_COMM; }
                else                 state = MULTI_LINE_COMM;
                break;
        }
    } while (c != EOF && is_comment);

    // vraci posledni nacteny znak
    return c;

}

/* scanner.c */
