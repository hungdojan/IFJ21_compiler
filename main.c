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

#include <stdio.h>
#include "token.h"
#include "scanner.h"
#include <string.h>

int main() {
    token_t *token;
    FILE *f = fopen("./test/IFJ_good.txt", "r");

    for (int i = 0; i < 12; i++) {
        if (get_token(f, &token) != 0)return 1;
        if (token->type != TYPE_IDENTIFIER)return 1;
        switch (i) {

            case 1:     if (strcmp(token->value.str_val, "_"))return 1; break;
            case 4:     if (strcmp(token->value.str_val, "wo_rd"))return 1; break;
            case 6:     if (strcmp(token->value.str_val, "word42"))return 1; break;
            case 9:     if (strcmp(token->value.str_val, "word_42"))return 1; break;
            case 10:    if (strcmp(token->value.str_val, "_4word2"))return 1; break;
            case 11:    if (strcmp(token->value.str_val, "wo4_rd2"))return 1; break;
        }

        token_delete(&token);
    }

    for (int i = 0; i < 10; i++) {
        if (get_token(f, &token) != 0)return 1;
        if (token->type != TYPE_INTEGER)return 1;

        if (i == 2)     if (token->value.int_val != 123)return 1;
        if (i == 5)     if (token->value.int_val != 12)return 1;
        if (i == 9)     if (token->value.int_val != 10123)return 1;
        token_delete(&token);
    }

    for (int i = 0; i < 15; i++) {
        if (get_token(f, &token) != 0)return 1;

        switch(i) {
            case 0:     if (token->type != TYPE_KW_DO)return 1; break;
            case 1:     if (token->type != TYPE_KW_GLOBAL)return 1; break;
            case 2:     if (token->type != TYPE_KW_NIL)return 1; break;
            case 3:     if (token->type != TYPE_KW_ELSE)return 1; break;
            case 4:     if (token->type != TYPE_KW_END)return 1; break;
            case 5:     if (token->type != TYPE_KW_FUNCTION)return 1; break;
            case 6:     if (token->type != TYPE_KW_IF)return 1; break;
            case 7:     if (token->type != TYPE_KW_INTEGER)return 1; break;
            case 8:     if (token->type != TYPE_KW_LOCAL)return 1; break;
            case 9:     if (token->type != TYPE_KW_NUMBER)return 1; break;
            case 10:    if (token->type != TYPE_KW_REQUIRE)return 1; break;
            case 11:    if (token->type != TYPE_KW_RETURN)return 1; break;
            case 12:    if (token->type != TYPE_KW_STRING)return 1; break;
            case 13:    if (token->type != TYPE_KW_THEN)return 1; break;
            case 14:    if (token->type != TYPE_KW_WHILE)return 1; break;
        }

        token_delete(&token);
    }

    for (int i = 0; i < 9; i++) {
        if (get_token(f, &token) != 0)return 1;

        if (i < 3)  { if (token->type != TYPE_NUMBER)return 1; }
        else        { if (token->type != TYPE_NUMBER)return 1; }
        token_delete(&token);
    }

    for (int i = 0; i < 15; i++) {
        if (get_token(f, &token) != 0)return 1;

        if (token->type != TYPE_STRING)return 1;

        switch (i) {
            case 0:     if (strcmp(token->value.str_val, ""))return 1; break;
            case 1:     if (strcmp(token->value.str_val, "word"))return 1; break;
            case 2:     if (strcmp(token->value.str_val, "double word"))return 1; break;
            case 13:    if (strcmp(token->value.str_val, "Ahoj\n\"Sve'te \\\""))return 1; break;
            case 14:    if (strcmp(token->value.str_val, "{"))return 1; break;
        }
        token_delete(&token);
    }

    for (int i = 0; i < 20; i++) {
        if (get_token(f, &token) != 0)return 1;

        switch(i) {
            case 0:    if (token->type != TYPE_PLUS)return 1; break;
            case 3: case 5:
                       if (token->type != TYPE_STRLEN){return 1;} break;
            case 11:   if (token->type != TYPE_LESSER_OR_EQ)return 1; break;
            case 12:   if (token->type != TYPE_GREATER_OR_EQ)return 1; break;
            case 14:   if (token->type != TYPE_NOT_EQ)return 1; break;
        }
        token_delete(&token);
    }

    for (int i = 0; i < 5; i++)
    {
        if (get_token(f, &token) != 0)return 1;

        switch (i) {
            case 0: case 4:
                if (token->type != TYPE_IDENTIFIER){return 1;} break;
            case 1:     if (token->type != TYPE_ASSIGN)return 1; break;
            case 2:     if (token->type != TYPE_INTEGER)return 1; break;
            case 3:     if (token->type != TYPE_PLUS)return 1; break;
        }
        token_delete(&token);
    }

    for (int i = 0; i < 3; i++) {
        if (get_token(f, &token) != EOF)return 1;
    } // TODO:
    token_delete(&token);
    fclose(f);
    return 0;
}

/* main.c */
