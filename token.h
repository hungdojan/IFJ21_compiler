/**
 * @file token.h
 * @brief Deklarace struktur, vycet typu tokenu, token_t a jeho funkce
 *
 * @authors David Kedra        (xkedra00)
 *          Jaroslav Kvasnicka (xkvasn14)
 *          Hung Do            (xdohun00)
 *          Petr Kolarik       (xkolar79)
 *
 * Reseni projektu IFJ a IAL, FIT VUT 21/22
 */

#ifndef _TOKEN_H_
#define _TOKEN_H_

#include "istring.h"

enum Token_type {
    TYPE_PLUS,
    TYPE_MINUS,
    TYPE_MULTIPLY,
    TYPE_DIVIDE,
    TYPE_DIVIDE_WHOLE,
    TYPE_DIVIDE_MODULO,
    TYPE_POWER,
    TYPE_GREATER_OR_EQ,
    TYPE_GREATER,
    TYPE_LESSER_OR_EQ,
    TYPE_LESSER,
    TYPE_EQ,
    TYPE_NOT_EQ,
    TYPE_ASSIGN,
    TYPE_STRLEN_IDENTIFIER,
    TYPE_STRLEN_STRING,
    TYPE_LEFT_PARENTHESES,
    TYPE_RIGHT_PARENTHESES,
    TYPE_DECLARE,
    TYPE_CONCAT,
    TYPE_STRING,
    TYPE_IDENTIFIER,
    TYPE_INTEGER,
    TYPE_NUMBER,
    TYPE_EXPONENT_NUMBER,
    TYPE_UNDEFINED,
    TYPE_DO,
    TYPE_GLOBAL,
    TYPE_NUMBER,
    TYPE_ELSE,
    TYPE_IF,
    TYPE_REQUIRE,
    TYPE_END,
    TYPE_INTEGER,
    TYPE_RETURN,
    TYPE_FUNCTION,
    TYPE_LOCAL,
    TYPE_STRING,
    TYPE_NIL,
    TYPE_THEN,
    TYPE_WHILE
} type;

// mozne typy atributu tokenu, vyuzije se jeden z nich 
typedef union
{
    int     int_val;
    double  float_val;
    char *  str_val;
} token_value_t;

// struktura pro token
typedef struct{
    enum Token_type type;
    token_value_t value;
} token_t;


/**
 * @brief Vytvori novy token a ulozi ho do ukazatele z argumentu
 *
 * @param s Struktura lexemu s retezcem
 * @param type Typ tokenu
 * @param ref_token Ukazatel na dynamicky token, kam se ulozi nove vytvoreny
 */
int token_create(Istring *s, enum Token_type type, token_t **ref_token);

/**
 * @brief Otestovani, zda identifikator neni klicovym slovem
 * 
 * @param identif Retezec obsahujici podeyrely identifikator
 * @return Vraci konkretni typ klicoveho slova nebo vzchozi TYPE_IDENTIF 
 */
int check_key_words(char *identif);

/**
 * @brief Vymaze a dealokuje token a jeho atributy
 *
 * @param token Ukazatel na dynamicky token, ktery ma byt uvolnen
 */
void token_delete(token_t **token);

#endif //_TOKEN_H_
