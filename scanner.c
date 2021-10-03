#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "scanner.h"
#include "istring.h"
#include "token.h"

int get_token() {
    char c = '\0';
    char tmp = '\0';
    Istring str;
    string_Init(&str);
    state = STATE_NEW;
    type = type_undefined;

    while (1) {
        c = getchar();

        switch (state) {
            case STATE_NEW:
                if (c == '+') {
                    string_Add_Char(&str, c);
                    type = type_plus;
                    state = STATE_RETURN_OPERATOR;
                } else if (c == '-') {
                    tmp = getchar();
                    if (tmp == '-')
                        get_rid_of_comments();
                    else if (isdigit(tmp)) {
                        string_Add_Char(&str, c);
                        ungetc(tmp, stdin);
                        state = STATE_INTEGER;
                    } else {
                        string_Add_Char(&str, c);
                        ungetc(tmp, stdin);
                        type = type_minus;
                        state = STATE_RETURN_OPERATOR;
                    }
                } else if (c == '*') {
                    string_Add_Char(&str, c);
                    type = type_multiply;
                    state = STATE_RETURN_OPERATOR;
                } else if (c == '/') {
                    tmp = getchar();
                    if (tmp == '/') {
                        string_Add_Char(&str, c);
                        string_Add_Char(&str, tmp);
                        type = type_divide_whole;
                        state = STATE_RETURN_OPERATOR;
                    } else {
                        string_Add_Char(&str, c);
                        ungetc(tmp, stdin);
                        type = type_divide;
                        state = STATE_RETURN_OPERATOR;
                    }
                } else if (c == '%') {
                    string_Add_Char(&str, c);
                    type = type_divide_modulo;
                    state = STATE_RETURN_OPERATOR;
                } else if (c == '^') {
                    string_Add_Char(&str, c);
                    type = type_power;
                    state = STATE_RETURN_OPERATOR;
                } else if (c == '>') {
                    tmp = getchar();
                    if (tmp == '=') {
                        string_Add_Char(&str, c);
                        string_Add_Char(&str, tmp);
                        type = type_greater_or_eq;
                        state = STATE_RETURN_OPERATOR;
                    } else {
                        string_Add_Char(&str, c);
                        ungetc(tmp, stdin);
                        type = type_greater;
                        state = STATE_RETURN_OPERATOR;
                    }
                } else if (c == '<') {
                    tmp = getchar();
                    if (tmp == '=') {
                        string_Add_Char(&str, c);
                        string_Add_Char(&str, tmp);
                        type = type_lesser_or_eq;
                        state = STATE_RETURN_OPERATOR;
                    } else {
                        string_Add_Char(&str, c);
                        ungetc(tmp, stdin);
                        type = type_lesser;
                        state = STATE_RETURN_OPERATOR;
                    }
                } else if (c == '=') {
                    tmp = getchar();
                    if (tmp == '=') {
                        string_Add_Char(&str, c);
                        string_Add_Char(&str, tmp);
                        type = type_eq;
                        state = STATE_RETURN_OPERATOR;
                    } else {
                        string_Add_Char(&str, c);
                        ungetc(tmp, stdin);
                        type = type_assign;
                        state = STATE_RETURN_OPERATOR;
                    }
                } else if (c == '~') {
                    tmp = getchar();
                    if (tmp == '=') {
                        string_Add_Char(&str, c);
                        string_Add_Char(&str, tmp);
                        type = type_not_eq;
                        state = STATE_RETURN_OPERATOR;
                    } else {
                        //string_Add_Char(&str,c);
                        //ungetc(tmp,stdin);
                        // ERROR - can not do "~x" , x - any symbol
                        return 1;
                    }
                } else if (c == '#') {
                    tmp = getchar();
                    if (tmp == '\"') {
                        string_Add_Char(&str, c);
                        string_Add_Char(&str, tmp);
                        state = STATE_STRLEN_STRING;
                    } else if (isalpha(tmp) || '_') {
                        string_Add_Char(&str, c);
                        string_Add_Char(&str, tmp);
                        state = STATE_STRLEN_IDENTIFIER;
                    } else {
                        //string_Add_Char(&str,c);
                        //ungetc(tmp,stdin);
                        // ERROR - #123, #$&^@, ## - not valid definition
                        return 1;
                    }
                } else if (c == '(') {
                    string_Add_Char(&str, c);
                    type = type_left_parentheses;
                    state = STATE_RETURN_OPERATOR;
                } else if (c == ')') {
                    string_Add_Char(&str, c);
                    type = type_right_parentheses;
                    state = STATE_RETURN_OPERATOR;
                } else if (c == ':') {
                    string_Add_Char(&str, c);
                    type = type_declare;
                    state = STATE_RETURN_OPERATOR;
                } else if (c == '.') {
                    tmp = getchar();
                    if (tmp == '.') {
                        string_Add_Char(&str, c);
                        string_Add_Char(&str, tmp);
                        type = type_concat;
                        state = STATE_RETURN_OPERATOR;
                    } else {
                        //string_Add_Char(&str,c);
                        //ungetc(tmp,stdin);
                        // ERROR - "." is not a valid definition
                        return 1;
                    }
                } else if (c == '\"') {
                    string_Add_Char(&str, c);
                    state = STATE_STRING;
                } else if (isalpha(c) || '_') {
                    string_Add_Char(&str, c);
                    state = STATE_IDENTIFIER;
                } else if (isdigit(c)) {
                    string_Add_Char(&str, c);
                    state = STATE_INTEGER;
                } else if (isspace(c)) {}
                else
                    return 1;
                break;
            case STATE_IDENTIFIER:
                if (isalpha(c) || '_' || isdigit(c)) {
                    string_Add_Char(&str, c);
                } else {
                    ungetc(c, stdin);
                    type = type_identifier;
                    state = STATE_RETURN_IDENTIFIER;
                }
                break;
            case STATE_INTEGER:
                if (isdigit(c)) {
                    string_Add_Char(&str, c);
                } else if (c == '.') {
                    string_Add_Char(&str, c);
                    state = STATE_NUMBER;
                } else if (c == 'e' || c == 'E') {
                    string_Add_Char(&str, c);
                    state = STATE_EXPONENT;
                } else {
                    ungetc(c, stdin);
                    type = type_integer;
                    state = STATE_RETURN_INTEGER;
                }
                break;
            case STATE_NUMBER:
                if (isdigit(c)) {
                    string_Add_Char(&str, c);
                } else if (c == 'e' || c == 'E') {
                    string_Add_Char(&str, c);
                    state = STATE_EXPONENT;
                } else {
                    ungetc(c, stdin);
                    type = type_number;
                    state = STATE_RETURN_NUMBER;
                }
                break;
            case STATE_EXPONENT:
                if (isdigit(c)) {
                    string_Add_Char(&str, c);
                    state = STATE_EXPONENT_FINISH;
                } else if (c == '+' || c == '-') {
                    string_Add_Char(&str, c);
                    state = STATE_EXPONENT_CONTINUE;
                } else {
                    // ERROR - after E must come a digit
                    return 1;
                }
                break;
            case STATE_EXPONENT_CONTINUE:
                if (isdigit(c)) {
                    string_Add_Char(&str, c);
                    state = STATE_EXPONENT_FINISH;
                } else {
                    // ERROR - after + or - must come a digit
                    return 1;
                }
                break;
            case STATE_EXPONENT_FINISH:
                if (isdigit(c)) {
                    string_Add_Char(&str, c);
                } else {
                    ungetc(c, stdin);
                    type = type_exponent_number; // or type = type_number;
                    state = STATE_RETURN_NUMBER;
                }
                break;
            case STATE_STRING:
                if (c == '\"') {
                    type = type_string;
                    state = STATE_RETURN_STRING;
                } else if (c == '\\') {
                    // is_escape()
                } else {
                    string_Add_Char(&str, c);
                }
                break;
            case STATE_STRLEN_IDENTIFIER:
                if (isalpha(c) || '_' || isdigit(c)) {
                    string_Add_Char(&str, c);
                } else {
                    ungetc(c, stdin);
                    type = type_strlen_identifier;
                    state = STATE_RETURN_STRLEN_IDENTIFIER;
                }
                break;
            case STATE_STRLEN_STRING:
                if (c == '\"') {
                    type = type_strlen_string;
                    state = STATE_RETURN_STRLEN_STRING;
                } else if (c == '\\') {
                    // is_escape()
                } else {
                    string_Add_Char(&str, c);
                }
                break;
            default:
                break;
        }

        switch (state) {
            case STATE_RETURN_OPERATOR:
                // GENERATE TOKEN WITH TYPE and VALUE + - * / // % ^
                token_create(&str, type);
                string_Init(&str);
                state = STATE_NEW;
                type = type_undefined;
                break;
            case STATE_RETURN_IDENTIFIER:
                // is_key_word()
                // GENERATE TOKEN WITH TYPE identifier and VALUE _498r6wvs
                token_create(&str, type);
                string_Init(&str);
                state = STATE_NEW;
                type = type_undefined;
                break;
            case STATE_RETURN_STRING:
                // GENERATE TOKEN WITH TYPE string and VALUE "xxxxx"
                token_create(&str, type);
                string_Init(&str);
                state = STATE_NEW;
                type = type_undefined;
                break;
            case STATE_RETURN_INTEGER:
                // GENERATE TOKEN WITH TYPE integer and VALUE 8465151
                token_create(&str, type);
                string_Init(&str);
                state = STATE_NEW;
                type = type_undefined;
                break;
            case STATE_RETURN_NUMBER:
                // GENERATE TOKEN WITH TYPE number and VALUE 89465.485, 654.648E6548
                token_create(&str, type);
                string_Init(&str);
                state = STATE_NEW;
                type = type_undefined;
                break;
            case STATE_RETURN_STRLEN_IDENTIFIER:
                // GENERATE TOKEN WITH TYPE STRLEN_IDENTIFIER and VALUE #_s651fe
                token_create(&str, type);
                string_Init(&str);
                state = STATE_NEW;
                type = type_undefined;
                break;
            case STATE_RETURN_STRLEN_STRING:
                // GENERATE TOKEN WITH TYPE STRLEN_STRING and VALUE #"xxxxx"
                token_create(&str, type);
                string_Init(&str);
                state = STATE_NEW;
                type = type_undefined;
                break;
            default:
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
