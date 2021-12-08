/**
 * @file main.c
 * @brief Hlavni soubor kompilatoru ifj21
 *
 * @authors Hung Do            (xdohun00)
 *          David Kedra        (xkedra00)
 *          Petr Kolarik       (xkolar79)
 *          Jaroslav Kvasnicka (xkvasn14)
 *
 * Reseni projektu IFJ a IAL, FIT VUT 21/22
 */

#include "parser.h"
#include <stdio.h>
#include "error.h"

extern unsigned file_line;

int main() {
    int res = syntax_analysis(stdin);
    // TODO:
    if (res)
        print_error(res, NULL, NULL, "chyba je mezi radky %u a %u\n", file_line-1, file_line);
    return res;
}

/* main.c */
