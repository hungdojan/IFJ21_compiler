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

extern unsigned file_line;

int main() {
    int res = syntax_analysis(stdin);
    // TODO:
    printf("%d\n", res);
    if (res)
        printf("chyba na radku: %u\n", file_line);
    return res;
}

/* main.c */
