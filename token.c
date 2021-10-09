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

#include "token.h"
#include "error.h"


//napr. pro token_t *token;  volat: token_create(&s, type, &token);
int token_create(Istring *s, enum Token_type type, token_t **ref_token){
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
            return print_error(ret, s, token, "nelze ukoncit retezec");


        // vyber korektniho typu hodnoty tokenu
        if(type == TYPE_INTEGER) 
        {
            // konverze hodnoty na konstatni cislo typu int (melo by uz byt ve spravnem tvaru)
            token->value.int_val = strtol(s->value, NULL, 10);
        }
        else if(type == TYPE_NUMBER || type == TYPE_EXPONENT_NUMBER)
        {
            // konverze hodnoty na konstatni cislo typu C double
            token->value.float_val = strtod(s->value, NULL);
        }
        else if(type == TYPE_IDENTIFIER){
            // kontrola, jestli neni identifikator klicovym slovem jazyka
            token->type = check_key_words(s->value);
            
            token->value.str_val = s->value;
        }
        else{
            token->value.str_val = s->value;
        }
    }
    else
    {
        //chyba alokace
        return print_error(ERR_INTERNAL, s, NULL, "nepodarila se alokovat pamet pro token");
    }

    return 0;
}

void token_delete(token_t **token){
    if(token)
    {
        //pokud token neni ukazatel na NULL ukazatel na token 
        if(*token)
        {
            // kdyz ma i neNULLovou hodnotu string, bude dealokovan
            if((*token)->value.str_val)
                free((*token)->value.str_val);

        free(*token);
        }
    }
    *token = NULL;
}


int check_key_words(char *identif)
{
    if(!strcmp(identif, "do")){
        return KW_DO;
    }
    else if(!strcmp(identif, "global")){
        return KW_GLOBAL;
    }
    else if(!strcmp(identif, "number")){
        return KW_NUMBER;
    }
    else if(!strcmp(identif, "else")){
        return KW_ELSE;
    }
    else if(!strcmp(identif, "if")){
        return KW_IF;
    }
    else if(!strcmp(identif, "require")){
        return KW_REQUIRE;
    }
    else if(!strcmp(identif, "end")){
        return KW_END;
    }
    else if(!strcmp(identif, "integer")){
        return KW_INTEGER;
    }
    else if(!strcmp(identif, "return")){
        return KW_RETURN;
    }
    else if(!strcmp(identif, "function")){
        return KW_FUNCTION;
    }
    else if(!strcmp(identif, "local")){
        return KW_LOCAL;
    }
    else if(!strcmp(identif, "string")){
        return KW_STRING;
    }
    else if(!strcmp(identif, "nil")){
        return KW_NIL;
    }
    else if(!strcmp(identif, "then")){
        return KW_THEN;
    }
    else if(!strcmp(identif, "while")){
        return KW_WHILE;
    }
    else
    {
        return TYPE_IDENTIFIER;
    }
}
