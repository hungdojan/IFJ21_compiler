/**
 * @file token.h
 * @brief Deklarace struktur token_t a token_array_t, jejich funkce a vycet typu tokenu
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
    TYPE_UNDEFINED
} type;

/**
 * @brief Vytvori novy token a ulozi ho do pole tokenu
 *
 * @param s Hodnota retezce
 * @param type Typ tokenu
 */
void token_create(Istring *s, enum Token_type type);

#endif //_TOKEN_H_
