/**
 * @file token.c
 * @brief Definice funkci struktur token_t
 *
 * @authors David Kedra        (xkedra00)
 *          Jaroslav Kvasnicka (xkvasn14)
 *          Hung Do            (xdohun00)
 *          Petr Kolarik       (xkolar79)
 *
 * Reseni projektu IFJ a IAL, FIT VUT 21/22
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"
#include "error.h"

int token_create(Istring *s, enum Token_type type, token_t **ref_token)
{
    *ref_token = (token_t *) malloc(sizeof(token_t));
    
    //pro zjednoduseni prirazovani pres (*ref_token)-> na jen token->
    token_t *token = *ref_token;

    if(token != NULL)
    {
        token->type = type;

        // premena pole znaku z Lex na retezec pridanim ukoncovaciho znaku
        int ret = string_Add_Char(s, '\0');
        
        // chyba pridani ukoncovaciho znaku
        if(ret != 0)
            return print_error(ret, s, &token, "nelze ukoncit retezec");


        // vyber korektniho typu hodnoty tokenu

        if(type == TYPE_STRING)
            token->value.str_val = s->value;
        else if(type == TYPE_INTEGER) 
        {
            // konverze hodnoty na konstatni cislo typu int (melo by uz byt ve spravnem tvaru)
            token->value.int_val = strtol(s->value, NULL, 10);

            // v get_token se ztraci ukazatel, pokud neni ulozen ve str_val tokenu
            // je potreba dealokovat puvodni obsah istringu
            string_Free(s);
        }
        else if(type == TYPE_NUMBER)
        {
            // konverze hodnoty na konstatni cislo typu C double
            token->value.float_val = strtod(s->value, NULL);

            //hodnota tokenu neni str_val, musi se dealokovat puvodni retezec
            string_Free(s);
        }
        else if(type == TYPE_IDENTIFIER)
        {
            if (!strcmp(s->value, "false") || !strcmp(s->value, "true"))
            {
                token->type = TYPE_BOOLEAN;
                token->value.bool_val = strcmp(s->value, "false");
            }
            else
            {
                // kontrola, jestli neni identifikator klicovym slovem jazyka
                token->type = check_key_words(s->value);
            }
            // pokud je typem key word, nemusi se uz ukladat do hodnoty
            if(token->type == TYPE_IDENTIFIER)
                token->value.str_val = s->value;
            else
                string_Free(s);
        }
        else
            //ostatni typy primo urcuji operace, relace, apod. Neni potreba ukladat hodnotu
            string_Free(s);

        // !! dulezite: po vytvoreni tokenu istring ztrati ukazatel na value string, protoze by ze strany tokenu
        // nesel vyNULLvat (i obracene) a napr. pri chybe by nastal double free. Nesmi se tu vsak value uvolnit
        s->value = NULL;
    }
    else
    {
        //chyba alokace
        return print_error(ERR_INTERNAL, s, NULL, "nepodarila se alokovat pamet pro token");
    }

    return 0;
}

void token_delete(token_t **token)
{
    // uvolneni, pokud token neni ukazatel na NULLovy ukazatel na token
    if(token && *token)
    {
        // pouze u typu TYPE_STRING a TYPE_IDENTIFIER se ukladaji hodnoty jako retezec
        if((*token)->type == TYPE_STRING || (*token)->type == TYPE_IDENTIFIER)
        {
            // kdyz ma i neNULLovou hodnotu string, bude dealokovan
            if((*token)->value.str_val)
            {
                free((*token)->value.str_val);

                // nulluje ukazatel na string ze strany tokenu. Nedokazal by ale nullovat ukazatel
                // na value ze strany istring, proto se musi na konci token_create
                (*token)->value.str_val = NULL;
            }
        }

        free(*token);
        *token = NULL;
    }
}


int check_key_words(char *identif)
{
    if(!strcmp(identif, "do"))              return TYPE_KW_DO;
    else if(!strcmp(identif, "global"))     return TYPE_KW_GLOBAL;
    else if(!strcmp(identif, "boolean"))    return TYPE_KW_BOOLEAN;
    else if(!strcmp(identif, "number"))     return TYPE_KW_NUMBER;
    else if(!strcmp(identif, "else"))       return TYPE_KW_ELSE;
    else if(!strcmp(identif, "if"))         return TYPE_KW_IF;
    else if(!strcmp(identif, "require"))    return TYPE_KW_REQUIRE;
    else if(!strcmp(identif, "end"))        return TYPE_KW_END;
    else if(!strcmp(identif, "elseif"))     return TYPE_KW_ELSEIF;
    else if(!strcmp(identif, "integer"))    return TYPE_KW_INTEGER;
    else if(!strcmp(identif, "return"))     return TYPE_KW_RETURN;
    else if(!strcmp(identif, "function"))   return TYPE_KW_FUNCTION;
    else if(!strcmp(identif, "local"))      return TYPE_KW_LOCAL;
    else if(!strcmp(identif, "string"))     return TYPE_KW_STRING;
    else if(!strcmp(identif, "nil"))        return TYPE_KW_NIL;
    else if(!strcmp(identif, "then"))       return TYPE_KW_THEN;
    else if(!strcmp(identif, "while"))      return TYPE_KW_WHILE;
    else if(!strcmp(identif, "and"))        return TYPE_KW_AND;
    else if(!strcmp(identif, "or"))         return TYPE_KW_OR;
    else                                    return TYPE_IDENTIFIER;
}

/* token.c */
