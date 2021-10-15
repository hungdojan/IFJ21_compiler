/**
 * @file error.c
 * @brief Definice funkci pro vypisy chybovych hlasek
 *
 * @authors David Kedra        (xkedra00)
 *          Hung Do            (xdohun00)
 *          Petr Kolarik       (xkolar79)
 *          Jaroslav Kvasnicka (xkvasn14)
 *
 * Reseni projektu IFJ a IAL, FIT VUT 21/22
 */

#include <stdio.h>
#include <stdarg.h>

#include "error.h"

// "Error <err_code>: <format>\n"
// priklad uziti: print_error(INTERNAL_ERR, "Chyba alokace pameti")
int print_error(int err_code, Istring *str, token_t **token, const char *format, ...)
{
    // impicitni zprava o chybe (s kodem chyby) s navazujici unikatne pozadovanou
    switch (err_code)
    {
        case ERR_LEX:
            fprintf(stderr, "Lexeme Structure Error %d: ", err_code);
            break;
        case ERR_SYNTAX:
            fprintf(stderr, "Syntax Error %d: ", err_code);
            break;
        case ERR_SEM_DEF:
            fprintf(stderr, "Semantic Defining Error %d: ", err_code);
            break;
        case ERR_SEM_ASSIGN:
            fprintf(stderr, "Type Incompatibility Error %d: ", err_code);
            break;
        case ERR_SEM_FUNC:
            fprintf(stderr, "Semantic Error %d: ", err_code);
            break;
        case ERR_SEM_TYPE:
            fprintf(stderr, "Expression Error %d: ", err_code);
            break;
        case ERR_SEMANT:
            fprintf(stderr, "Undefined Semantic Error %d: ", err_code);
            break;
        case ERR_UNEXP_VAL:
            fprintf(stderr, "Unexpected NIL Value Error %d: ", err_code);
            break;
        case ERR_ZERO_DIV:
            fprintf(stderr, "Zero Division Error %d: ", err_code);
            break;
        case ERR_INTERNAL:
            fprintf(stderr, "Internal Compiler Error %d: ", err_code);
            break;
        
        default:
            fprintf(stderr, "Unknown Error %d: ", err_code);
            break;
    }

    // je-li volitelna zprava NULL, je vypis preskocen
    if(format != NULL)
    {
        // inicializace struktur pro praci s argumenty
        va_list vargs;
        va_start(vargs, format);

        //vypis volitelne zpravy chyby
        vfprintf(stderr, format, vargs);
        fprintf(stderr, "\n");
        
        va_end(vargs);
    }

    // dealokace pole tokenu  
    if(token)
        token_delete(token);

    // dealokace Istringu, prokud jeste nebyl vytvoren token
    if(str)
        string_Free(str);

    return err_code;

} // print_error()
