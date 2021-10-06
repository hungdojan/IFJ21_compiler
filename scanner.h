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

enum State {
    STATE_NEW,
    STATE_INTEGER,
    STATE_STRLEN_STRING,
    STATE_STRLEN_IDENTIFIER,
    STATE_STRING,
    STATE_IDENTIFIER,
    STATE_NUMBER,
    STATE_EXPONENT,
    STATE_EXPONENT_CONTINUE,
    STATE_EXPONENT_FINISH,
    STATE_RETURN_OPERATOR,
    STATE_RETURN_STRING,
    STATE_RETURN_INTEGER,
    STATE_RETURN_NUMBER,
    STATE_RETURN_IDENTIFIER,
    STATE_RETURN_STRLEN_IDENTIFIER,
    STATE_RETURN_STRLEN_STRING
    // STATE_ERROR
    } state;

/**
 * @brief Ziska token ze zdrojoveho souboru
 *
 * @param f Ukazatel na otevreny zdrojovy soubor
 * @return Nulovou hodnotu, pokud nenastala zadna lexikalni chyba
 */
int get_token(FILE *f);

/**
 * @brief Preskakuje komentare ve zdrojovem souboru
 *
 * @param f Ukazatel na otevreny zdrojovy soubor
 * @return Nulovou hodnotu, pokud nenastala zadna lexikalni chyba
 */
int get_rid_of_comments(FILE *f);

#endif // _SCANNER_H_
