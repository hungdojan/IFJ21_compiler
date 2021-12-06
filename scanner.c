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

#define IS_HEX(x)  (isdigit(x) || \
                   ((x) >= 'a' && (x) <= 'f') || \
                   ((x) >= 'A' && (x) <= 'F'))
#define GET_DIGIT(x) (x) - '0'

static int load_ascii(int *c, FILE *f)
{
    if (f == NULL || c == NULL)
        return ERR_INTERNAL;

    int input;      // precteny znak
    // resetovani hodnoty
    *c = 0;
    for (int i = 0; i < 3; i++)
    {
        input = fgetc(f);
        if (!isdigit(input))
            return ERR_INTERNAL;
        *c += GET_DIGIT(input);
        *c *= 10;
    }
    // hodnota v *c je momentalne ctyrmistna
    *c /= 10;

    // kontrola, zda *c spada do intervalu <1, 255>
    if (*c < 1 || *c > 255)
        return ERR_INTERNAL;
    return NO_ERR;
}
static int convert_to_ascii(Istring *str, int c)
{
    int res = NO_ERR;
    if ((res = string_Add_Char(str, c / 100 % 10 + '0')) != NO_ERR) return res;
    if ((res = string_Add_Char(str, c / 10 % 10 + '0')) != NO_ERR) return res;
    if ((res = string_Add_Char(str, c / 1 % 10 + '0')) != NO_ERR) return res;
    return res;
}

static int convert_to_hex(Istring *s, FILE *f)
{
    char c[3] = {0,};
    int input;
    for (int i = 0; i < 2; i++)
    {
        input = fgetc(f);
        if (!IS_HEX(input))
            return ERR_LEX;
        c[i] = input;
    }
    c[2] = 0;
    int number = strtol(c,NULL,16);
    if (number < 1 || number > 255)
        return ERR_INTERNAL;
    return convert_to_ascii(s,number);
}


static int esc_seq(Istring *s, FILE *f)
{
    if (f == NULL)
        return ERR_INTERNAL;
    int res = NO_ERR;
    int c = fgetc(f);

    if (c == 'a')                       { c = '\a'; res = convert_to_ascii(s,c); }
    else if (c == 'b')                  { c = '\b'; res = convert_to_ascii(s,c); }
    else if (c == 'f')                  { c = '\f'; res = convert_to_ascii(s,c); }
    else if (c == 'n')                  { c = '\n'; res = convert_to_ascii(s,c); }
    else if (c == 'r')                  { c = '\r'; res = convert_to_ascii(s,c); }
    else if (c == 't')                  { c = '\t'; res = convert_to_ascii(s,c); }
    else if (c == 'v')                  { c = '\v'; res = convert_to_ascii(s,c); }
    else if (c == '\\')                 { c = '\\'; res = convert_to_ascii(s,c); }
    else if (c == '\"')                 { c = '\"'; res = convert_to_ascii(s,c); }
    else if (c == '\'')                 { c = '\''; res = convert_to_ascii(s,c); }
    else if (c == 'x' || c == 'X')      res = convert_to_hex(s,f);
    else if (isdigit(c))
    {
        ungetc(c, f);
        int res, val;
        if ((res = load_ascii(&val, f)) != NO_ERR)  return res;
        res = convert_to_ascii(s, val);
    }
    else
        return ERR_LEX;
    return res;
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
                    if (esc_seq(&str, f) != NO_ERR)
                        return print_error(ERR_LEX, &str, NULL, "chybna escape sekvence stringu");
                        //goto error;
                }
                else
                {
                    if(c <= 31)
                    {
                        return print_error(ERR_LEX, &str, NULL, "nepovoleny znak v retezcovem literalu");
                    }
                    else if (c == ' ')
                    {
                        string_Add_Char(&str, '\\');
                        int res;
                        if ((res = convert_to_ascii(&str, c)) != NO_ERR)
                            return print_error(res, &str, NULL, "chyba pri nacitani retezcoveho literalu");
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
