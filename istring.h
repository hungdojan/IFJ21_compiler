/**
 * @file istring.h
 * @brief Deklarace struktury Istring a jeji funkce
 *
 * @authors Hung Do            (xdohun00)
 *          David Kedra        (xkedra00)
 *          Petr Kolarik       (xkolar79)
 *          Jaroslav Kvasnicka (xkvasn14)
 *
 * Reseni projektu IFJ a IAL, FIT VUT 21/22
 */
#ifndef _ISTRING_H_
#define _ISTRING_H_

#define STRING_BASE_SIZE 4

typedef struct
{
    char *value;        /// Obsah retezce
    int length;         /// Pocet znaku v retezci (vcetne znaku konce retezce)
    int allocSize;      /// Velikost pole
} Istring;

/**
 * @brief Inicializace Istring
 *
 * @param s Ukazatel na strukturu Istring
 * @return Chybovy kod
 */
int string_Init(Istring *s);

/**
 * @brief Pridat znak do retezce
 *
 * @param s Ukazatel na strukturu Istring
 * @param c Pridavany znak
 * @return Chybovy kod
 */
int string_Add_Char(Istring *s, char c);

/**
 * @brief Uvolneni pameti po Istring
 *
 * @param s Ukazatel na strukturu Istring
 */
void string_Free(Istring *s);
#endif // _ISTRING_H_
