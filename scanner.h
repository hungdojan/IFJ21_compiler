/**
 * @file scanner.h
 * @brief Deklarace funkci a vycet stavu lexikalni analyzy
 *
 * @authors Jaroslav Kvasnicka (xkvasn14) 
 *          Hung Do            (xdohun00)
 *          David Kedra        (xkedra00)
 *          Petr Kolarik       (xkolar79)
 *          
 *
 * Reseni projektu IFJ a IAL, FIT VUT 21/22
 */
#ifndef _SCANNER_H_
#define _SCANNER_H_

#include "token.h"

enum State {
    STATE_NEW,
    STATE_INTEGER,
    STATE_STRING,
    STATE_IDENTIFIER,
    STATE_NUMBER,
    STATE_EXPONENT,
    STATE_EXPONENT_FINISH,
    STATE_END_LOAD
    // STATE_ERROR
};

/**
 * @brief Ziska token ze zdrojoveho souboru
 *
 * @param f Ukazatel na otevreny zdrojovy soubor
 * @param ref Ukazatel na referencni token, pres ktery se vraci data
 * @return Nulovou hodnotu, pokud nenastala zadna lexikalni chyba
 */
int get_token(FILE *f, token_t **ref);

/**
 * @brief Preskakuje komentare ve zdrojovem souboru
 *
 * @param f Ukazatel na otevreny zdrojovy soubor
 * @return Nulovou hodnotu, pokud nenastala zadna lexikalni chyba
 */
int get_rid_of_comments(FILE *f);

#endif // _SCANNER_H_
