/**
 * @file error.h
 * @brief Deklarace chybovych vypisu a navratove hodnoty chyb
 *
 * @authors David Kedra        (xkedra00)
 *          Hung Do            (xdohun00)
 *          Petr Kolarik       (xkolar79)
 *          Jaroslav Kvasnicka (xkvasn14)
 *
 * Reseni projektu IFJ a IAL, FIT VUT 21/22
 */
#ifndef _ERROR_H_
#define _ERROR_H_


#include "istring.h"
#include "token.h"

/**
 * @brief Vypis chyboveho stavu a uvolneni prislusnych pameti
 *
 * @param err_code Ciselny kod chyby
 * @param str Ukazatel na Istring k dealokaci nebo NULL
 * @param token Ukazatel na token k dealokaci nebo NULL
 * @param format Volitelna detailnejsi zprava o chybe, NULL, nebo ""
 * @return Chybovy kod err_code
 */
int print_error(int err_code, Istring *str, token_t **token, const char *format, ...);

// vypis moznych chybovych navratovych hodnot
enum err_codes
{
    ERR_LEX = 1,
    ERR_SYNTAX = 2,
    ERR_SEM_DEF = 3,
    ERR_SEM_ASSIGN = 4,
    ERR_SEM_FUNC = 5,
    ERR_SEM_TYPE = 6,
    ERR_SEMANT = 7,
    ERR_UNEXP_VAL = 8,
    ERR_ZERO_DIV = 9,
    ERR_INTERNAL = 99   ///< vnitrni chyba prekladace neovlivnena vstupnim programem (alloc err, ...) 
};

#endif
