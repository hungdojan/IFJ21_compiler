/**
 * @file generator.c
 * @brief Definice funkci ze souboru generator.h
 *
 * @authors Jaroslav Kvasnicka (xkvasn14)
 *          Hung Do            (xdohun00)
 *          David Kedra        (xkedra00)
 *          Petr Kolarik       (xkolar79)
 *
 * Reseni projektu IFJ a IAL, FIT VUT 21/22
 */

#include "generator.h"
#include "error.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char _dest[MAX_STR_LEN] = { 0, };
char _first[MAX_STR_LEN] = { 0, };
char _second[MAX_STR_LEN] = { 0, };

gen_info_t glob_cnt = { 0, };

void init_gen_info()
{
    glob_cnt.if_i = glob_cnt.elseif_i = glob_cnt.else_i = glob_cnt.tmp_var_i = glob_cnt.while_i = 0;
    glob_cnt.func_name = NULL;
}

void reset_gen_info(node_ptr func_node)
{
    glob_cnt.if_i = glob_cnt.elseif_i = glob_cnt.else_i = glob_cnt.tmp_var_i = glob_cnt.while_i = 0;
    if (func_node != NULL)
        glob_cnt.func_name = func_node->key;
    else
        glob_cnt.func_name = NULL;
}

int define_variable(enum frame_type ft, enum operand_type ot, node_ptr var_node)
{
    char _temp[MAX_STR_LEN] = { 0, };
    CLEAR_OPERAND(ot);
    snprintf(_temp, MAX_STR_LEN, "%s@", GET_FRAME(ft));
    // vybrany ramec
    CONCAT_TO_OPERAND(ot);
    if (glob_cnt.func_name != NULL)
    {
        snprintf(_temp, MAX_STR_LEN, "%s_", glob_cnt.func_name);
        CONCAT_TO_OPERAND(ot);
    }
    if (var_node->is_param_var)
    {
        snprintf(_temp, MAX_STR_LEN, "param_");
        CONCAT_TO_OPERAND(ot);
    }
    if (glob_cnt.if_i > 0)
    {
        sprintf(_temp, "if%d_", glob_cnt.if_i);
        CONCAT_TO_OPERAND(ot);
    }
    if (glob_cnt.elseif_i > 0)
    {
        sprintf(_temp, "elseif%d_", glob_cnt.elseif_i);
        CONCAT_TO_OPERAND(ot);
    }
    if (glob_cnt.else_i > 0)
    {
        sprintf(_temp, "else%d_", glob_cnt.else_i);
        CONCAT_TO_OPERAND(ot);
    }
    if (glob_cnt.while_i > 0)
    {
        sprintf(_temp, "while%d_", glob_cnt.while_i);
        CONCAT_TO_OPERAND(ot);
    }
    sprintf(_temp, "%s", var_node->key);
    CONCAT_TO_OPERAND(ot);
    // TODO: kontrola
    return _temp[MAX_STR_LEN-1] != '\0' ? ERR_INTERNAL : NO_ERR;
}

int define_constant(enum operand_type ot, exp_data_t term)
{
    char _temp[MAX_STR_LEN] = { 0, };
    CLEAR_OPERAND(ot);
    snprintf(_temp, MAX_STR_LEN, "%s@", GET_TYPE(term.type));
    CONCAT_TO_OPERAND(ot);

    if (term.type == DATA_STR)
        sprintf(_temp, "%s", term.value.string);
    else if (term.type == DATA_NUM)
        sprintf(_temp, "%a", term.value.number);
    else if (term.type == DATA_INT)
        sprintf(_temp, "%d", term.value.integer);
    else if (term.type == DATA_BOOL)
        sprintf(_temp, "%s", term.value.boolean ? "true" : "false");
    else
        sprintf(_temp, "nil");

    // presun do spravneho operandu
    CONCAT_TO_OPERAND(ot);

    return _temp[MAX_STR_LEN-1] != '\0' ? ERR_INTERNAL : NO_ERR;
}

int define_label(enum operand_type ot, enum label_type lt)
{
    char _temp[MAX_STR_LEN] = { 0, };
    CLEAR_OPERAND(ot);
    sprintf(_temp, "%s_", glob_cnt.func_name);
    CONCAT_TO_OPERAND(ot);
    if (lt != LABEL_FUNC)
    {
        if (lt == LABEL_IF)             sprintf(_temp, "if%d", glob_cnt.if_i); 
        else if (lt == LABEL_ENDIF)     sprintf(_temp, "endif%d", glob_cnt.if_i);
        else if (lt == LABEL_ELSEIF)    sprintf(_temp, "elseif%d", glob_cnt.elseif_i);
        else if (lt == LABEL_ENDELSEIF) sprintf(_temp, "endelsif%d", glob_cnt.elseif_i);
        else if (lt == LABEL_ELSE)      sprintf(_temp, "else%d", glob_cnt.else_i);
        else if (lt == LABEL_WHILE)     sprintf(_temp, "while%d", glob_cnt.while_i);
        else if (lt == LABEL_ENDWHILE)  sprintf(_temp, "endwhile%d", glob_cnt.while_i);
        else if (lt == LABEL_END)       sprintf(_temp, "end%d", glob_cnt.if_i);
        else                            return ERR_INTERNAL;
        CONCAT_TO_OPERAND(ot);
    }
    return NO_ERR;
}

int gen_code(queue_t *q,enum ins_type type, char* dest, char* first_op, char* second_op)
{
    code_t* item = (code_t*)calloc(1, sizeof (struct code));
    if(item == NULL)
        return ERR_INTERNAL;
    item->instruction = type;

    if (dest != NULL)
    {
        item->dest = (char *)calloc(strlen(dest) + 1, 1);
        if (item->dest == NULL)     goto dest_err;
        strcpy(item->dest, dest);
    }

    if (first_op != NULL)
    {
        item->first_op = (char *) calloc(strlen(first_op) + 1, 1);
        if (item->first_op == NULL) goto first_op_err;
        strcpy(item->first_op, first_op);
    }

    if (second_op != NULL)
    {
        item->second_op = (char *) calloc(strlen(second_op) + 1, 1);
        if (item->second_op == NULL) goto second_op_err;
        strcpy(item->second_op, second_op);
    }

    if(q != NULL)
        queue_add(q,item);
    else
    {
        flush_item(item);
        free(item->second_op);
        free(item->first_op);
        free(item->dest);
        free(item);
        item = NULL;
    }
    return 0;
second_op_err:
    free(item->first_op);
first_op_err:
    free(item->dest);
dest_err:
    free(item);
    return ERR_INTERNAL;
}

void queue_flush(queue_t *q)
{
    for(code_t* item = q->first; item != NULL; item = item->next)
    {
        flush_item(item);
    }
    queue_destroy(q);
}

void queue_init(queue_t *q)
{
    q->first = NULL;
    q->last = NULL;
}

void queue_destroy(queue_t *q)
{
    while(q->first != NULL)
    {
        code_t* tmp = q->first;
        q->first = q->first->next;
        free(tmp->dest);
        free(tmp->second_op);
        free(tmp->first_op);
        free(tmp);
        tmp = NULL;
    }
    queue_init(q);
}

void queue_add(queue_t *q, code_t *item)
{
    if(q->first == NULL)
    {
        q->first = item;
        q->last = item;
    }
    else
    {
        q->last->next = item;
        q->last = item;
    }
}

void flush_item(code_t *item)
{
    switch(item->instruction)
    {
        case INS_MOVE:
            fprintf(stdout,"MOVE %s %s\n",item->dest,item->first_op);
            break;
        case INS_CREATEFRAME:
            fprintf(stdout,"CREATEFRAME\n");
            break;
        case INS_PUSHFRAME:
            fprintf(stdout,"PUSHFRAME\n");
            break;
        case INS_POPFRAME:
            fprintf(stdout,"POPFRAME\n");
            break;
        case INS_DEFVAR:
            fprintf(stdout,"DEFVAR %s\n",item->dest);
            break;
        case INS_CALL:
            fprintf(stdout,"CALL %s\n",item->dest);
            break;
        case INS_RETURN:
            fprintf(stdout,"RETURN\n");
            break;
        case INS_PUSHS:
            fprintf(stdout,"PUSHS %s\n",item->dest);
            break;
        case INS_POPS:
            fprintf(stdout,"POPS %s\n",item->dest);
            break;
        case INS_CLEARS:
            fprintf(stdout,"CLEARS\n");
            break;
        case INS_ADD:
            fprintf(stdout,"ADD %s %s %s\n",item->dest,item->first_op,item->second_op);
            break;
        case INS_SUB:
            fprintf(stdout,"SUB %s %s %s\n",item->dest,item->first_op,item->second_op);
            break;
        case INS_MUL:
            fprintf(stdout,"MUL %s %s %s\n",item->dest,item->first_op,item->second_op);
            break;
        case INS_DIV:
            fprintf(stdout,"DIV %s %s %s\n",item->dest,item->first_op,item->second_op);
            break;
        case INS_IDIV:
            fprintf(stdout,"IDIV %s %s %s\n",item->dest,item->first_op,item->second_op);
            break;
        case INS_ADDS:
            fprintf(stdout,"ADDS\n");
            break;
        case INS_SUBS:
            fprintf(stdout,"SUBS\n");
            break;
        case INS_MULS:
            fprintf(stdout,"MULS\n");
            break;
        case INS_DIVS:
            fprintf(stdout,"DIVS\n");
            break;
        case INS_IDIVS:
            fprintf(stdout,"IDIVS\n");
            break;
        case INS_LT:
            fprintf(stdout,"LT %s %s %s\n",item->dest,item->first_op,item->second_op);
            break;
        case INS_GT:
            fprintf(stdout,"GT %s %s %s\n",item->dest,item->first_op,item->second_op);
            break;
        case INS_EQ:
            fprintf(stdout,"EQ %s %s %s\n",item->dest,item->first_op,item->second_op);
            break;
        case INS_LTS:
            fprintf(stdout,"LTS\n");
            break;
        case INS_GTS:
            fprintf(stdout,"GTS\n");
            break;
        case INS_EQS:
            fprintf(stdout,"EQS\n");
            break;
        case INS_AND:
            fprintf(stdout,"AND %s %s %s\n",item->dest,item->first_op,item->second_op);
            break;
        case INS_OR:
            fprintf(stdout,"OR %s %s %s\n",item->dest,item->first_op,item->second_op);
            break;
        case INS_NOT:
            fprintf(stdout,"NOT %s %s\n",item->dest,item->first_op);
            break;
        case INS_ANDS:
            fprintf(stdout,"ANDS\n");
            break;
        case INS_ORS:
            fprintf(stdout,"ORS\n");
            break;
        case INS_NOTS:
            fprintf(stdout,"NOTS\n");
            break;
        case INS_INT2FLOAT:
            fprintf(stdout,"INT2FLOAT %s %s\n",item->dest,item->first_op);
            break;
        case INS_FLOAT2INT:
            fprintf(stdout,"FLOAT2INT %s %s\n",item->dest,item->first_op);
            break;
        case INS_INT2CHAR:
            fprintf(stdout,"INT2CHAR %s %s\n",item->dest,item->first_op);
            break;
        case INS_STRI2INT:
            fprintf(stdout,"STRI2INT %s %s %s\n",item->dest,item->first_op,item->second_op);
            break;
        case INS_INT2FLOATS:
            fprintf(stdout,"INT2FLOATS\n");
            break;
        case INS_FLOAT2INTS:
            fprintf(stdout,"FLOAT2INTS\n");
            break;
        case INS_INT2CHARS:
            fprintf(stdout,"INT2CHARS\n");
            break;
        case INS_STRI2INTS:
            fprintf(stdout,"STRI2INTS\n");
            break;
        case INS_READ:
            fprintf(stdout,"READ %s %s\n",item->dest,item->first_op);
            break;
        case INS_WRITE:
            fprintf(stdout,"WRITE %s\n",item->dest);
            break;
        case INS_CONCAT:
            fprintf(stdout,"CONCAT %s %s %s\n",item->dest,item->first_op,item->second_op);
            break;
        case INS_STRLEN:
            fprintf(stdout,"STRLEN %s %s\n",item->dest,item->first_op);
            break;
        case INS_GETCHAR:
            fprintf(stdout,"GETCHAR %s %s %s\n",item->dest,item->first_op,item->second_op);
            break;
        case INS_SETCHAR:
            fprintf(stdout,"SETCHAR %s %s %s\n",item->dest,item->first_op,item->second_op);
            break;
        case INS_TYPE:
            fprintf(stdout,"TYPE %s %s\n",item->dest,item->first_op);
            break;
        case INS_LABEL:
            fprintf(stdout,"LABEL %s\n",item->dest);
            break;
        case INS_JUMP:
            fprintf(stdout,"JUMP %s\n",item->dest);
            break;
        case INS_JUMPIFEQ:
            fprintf(stdout,"JUMPIFEQ %s %s %s\n",item->dest,item->first_op,item->second_op);
            break;
        case INS_JUMPIFNEQ:
            fprintf(stdout,"JUMPIFNEQ %s %s %s\n",item->dest,item->first_op,item->second_op);
            break;
        case INS_JUMPIFEQS:
            fprintf(stdout,"JUMPIFEQS %s\n",item->dest);
            break;
        case INS_JUMPIFNEQS:
            fprintf(stdout,"JUMPIFNEQS %s\n",item->dest);
            break;
        case INS_EXIT:
            fprintf(stdout,"EXIT %s\n",item->dest);
            break;
        case INS_BREAK:
            fprintf(stdout,"BREAK\n");
            break;
        case INS_DPRINT:
            fprintf(stdout,"DPRINT %s\n",item->dest);
            break;
        default:
            // error?
            // the end of queue?
            break;
    }
}

void filter_defvar(queue_t *q)
{
    if (q != NULL)
    {
        bool is_func_call = false;
        code_t *item = NULL;
        // pokud je to na zacatku instrukce definice
        while (q->first != NULL && q->first->instruction == INS_DEFVAR)
        {
            // vypis instrukce
            flush_item(q->first);

            // posun ve fronte
            item = q->first;
            q->first = item->next;

            // mazani instrukce
            free(item->dest);
            free(item->second_op);
            free(item->first_op);
            free(item);
            item = NULL;
        }
        item = q->first;
        while (item != NULL && item->next != NULL)
        {
            if (item->instruction == INS_CREATEFRAME)
                is_func_call = true;
            else if (item->instruction == INS_CALL)
                is_func_call = false;
            else if (item->next->instruction == INS_DEFVAR && !is_func_call)
            {
                // vypis instrukce 
                flush_item(item->next);

                // posun ve fronte
                code_t *temp = item->next;
                item->next = temp->next;

                // mazani instrukce
                free(temp->dest);
                free(temp->second_op);
                free(temp->first_op);
                free(temp);
                temp = NULL;
            }
            item = item->next;
        }
    }
}

void import_builtin_functions()
{
    /// %%retval - univerzální návratová proměnná - předávaná pomocí TF(ad)retval
    /// %%param1 - univerzální první parametr funkce - předávaná pomocí stacku
    /// %%param2 - univerzální druhý parametr funkce
    /// %%param3 - univerzální třetí parametr funkce
    /// %%cokoli - pomocné proměnné

    fprintf(stdout,".IFJcode21\n\n"
            "DEFVAR GF@%%temp_var1\n"
            "DEFVAR GF@%%temp_var2\n"
            "DEFVAR GF@%%temp_var3\n"
            "JUMP $$main\n\n");


    /// print reads
    fprintf(stdout,"LABEL reads\n"
            "PUSHFRAME\n"
            "DEFVAR LF@%%retvar$1\n"
            "POPS LF@%%retvar$1\n" // empty pop
            "MOVE LF@%%retvar$1 nil@nil\n"
            "READ LF@%%retvar$1 string\n"
            "POPFRAME\n"
            "RETURN\n\n");

    /// print readi
    fprintf(stdout,"LABEL readi\n"
            "PUSHFRAME\n"
            "DEFVAR LF@%%retvar$1\n"
            "POPS LF@%%retvar$1\n" // empty pop
            "MOVE LF@%%retvar$1 nil@nil\n"
            "READ LF@%%retvar$1 int\n"
            "POPFRAME\n"
            "RETURN\n\n");

    /// print readn
    fprintf(stdout,"LABEL readn\n"
            "PUSHFRAME\n"
            "DEFVAR LF@%%retvar$1\n"
            "POPS LF@%%retvar$1\n" // empty pop
            "MOVE LF@%%retvar$1 nil@nil\n"
            "READ LF@%%retvar$1 float\n"
            "POPFRAME\n"
            "RETURN\n\n");

    /// print write
    fprintf(stdout,"LABEL write\n"
            "PUSHFRAME\n"
            "DEFVAR LF@%%param1\n"
            "DEFVAR LF@%%cond\n"
            "DEFVAR LF@%%i\n"
            "POPS LF@%%i\n"
            "SUB LF@%%i LF@%%i int@0\n"
            "LABEL write&cycle\n"
            "LT LF@%%cond LF@%%i int@1\n"
            "JUMPIFEQ write&return LF@%%cond bool@true\n"
            "POPS LF@%%param1\n"
            "WRITE LF@%%param1\n"
            "SUB LF@%%i LF@%%i int@1\n"
            "JUMP write&cycle\n"
            "LABEL write&return\n"
            "POPFRAME\n"
            "RETURN\n\n");

    /// print tointeger
    fprintf(stdout,"LABEL tointeger\n"
            "PUSHFRAME\n"
            "DEFVAR LF@%%retvar$1\n"
            "MOVE LF@%%retvar$1 nil@nil\n"
            "DEFVAR LF@%%cond\n"
            // "DEFVAR LF@%%param1\n"
            // "POPS LF@%%param1\n" // empty pop
            // "POPS LF@%%param1\n"
            "DEFVAR LF@%%param1\n"
            "MOVE LF@%%param1 LF@%%1\n"
            "TYPE LF@%%cond LF@%%param1\n"
            "JUMPIFEQ tointeger&return LF@%%cond string@number\n"
            "FLOAT2INT LF@%%retvar$1 LF@%%param1\n"
            "LABEL tointeger&return\n"
            "POPFRAME\n"
            "RETURN\n\n");

    /// print substr
    fprintf(stdout,"LABEL substr\n"
            "PUSHFRAME\n"
            "DEFVAR LF@%%cond\n"
            "DEFVAR LF@%%param1\n"
            "MOVE LF@%%param1 LF@%%1\n"
            "TYPE LF@%%cond LF@%%param1\n"
            "JUMPIFNEQ substr&exit LF@%%cond string@string\n"
            "DEFVAR LF@%%param2\n"
            "MOVE LF@%%param2 LF@%%2\n"
            "TYPE LF@%%cond LF@%%param2\n"
            "JUMPIFNEQ substr&exit LF@%%cond string@int\n"
            "DEFVAR LF@%%param3\n"
            "MOVE LF@%%param3 LF@%%3\n"
            "TYPE LF@%%cond LF@%%param3\n"
            "JUMPIFNEQ substr&exit LF@%%cond string@int\n"
            // "POPS LF@%%param1\n" // empty pop
            // "POPS LF@%%param1\n"
            // "TYPE LF@%%cond LF@%%param1\n"
            // "JUMPIFNEQ substr&exit LF@%%cond string@string\n"
            // "DEFVAR LF@%%param2\n"
            // "POPS LF@%%param2\n"
            // "TYPE LF@%%cond LF@%%param2\n"
            // "JUMPIFNEQ substr&exit LF@%%cond string@int\n"
            // "DEFVAR LF@%%param3\n"
            // "POPS LF@%%param3\n"
            // "TYPE LF@%%cond LF@%%param3\n"
            // "JUMPIFNEQ substr&exit LF@%%cond string@int\n"
            "DEFVAR LF@%%retvar$1\n"
            "MOVE LF@%%retvar$1 string@\n"
            "DEFVAR LF@%%cond_length\n"
            "STRLEN LF@%%cond_length LF@%%param1\n"
            "LT LF@%%cond LF@%%param2 int@1\n"
            "JUMPIFEQ substr&return LF@%%cond bool@true\n"
            "GT LF@%%cond LF@%%param2 LF@%%cond_length\n"
            "JUMPIFEQ substr&return LF@%%cond bool@true\n"
            "LT LF@%%cond LF@%%param3 int@1\n"
            "JUMPIFEQ substr&return LF@%%cond bool@true\n"
            "GT LF@%%cond LF@%%param3 LF@%%cond_length\n"
            "JUMPIFEQ substr&return LF@%%cond bool@true\n"
            "GT LF@%%cond LF@%%param3 LF@%%param2\n"
            "JUMPIFEQ substr&return LF@%%cond bool@true\n"
            "DEFVAR LF@%%item\n"
            "DEFVAR LF@%%i\n"
            "SUB LF@%%i LF@%%param2 int@1\n"
            "DEFVAR LF@%%j\n"
            "SUB LF@%%j LF@%%param3 int@1\n"
            "LABEL substr&for\n"
            "GT LF@%%cond LF@%%i LF@%%j\n"
            "JUMPIFEQ substr&return LF@%%cond bool@true\n"
            "GETCHAR LF@%%item LF@%%param1 LF@%%i\n"
            "CONCAT LF@%%retvar$1 LF@%%retvar$1 LF@%%item\n"
            "ADD LF@%%i LF@%%i int@1\n"
            "JUMP substr&for\n"
            "LABEL substr&return\n"
            "POPFRAME\n"
            "RETURN\n"
            "LABEL substr&exit\n"
            "EXIT int@8\n\n");

    /// print ord
    fprintf(stdout,"LABEL ord\n"
            "PUSHFRAME\n"
            "DEFVAR LF@%%retvar$1\n"
            "DEFVAR LF@%%param1\n"
            "MOVE LF@%%param1 LF@%%1\n"
            "TYPE LF@%%retvar$1 LF@%%param1\n"
            "JUMPIFNEQ ord&exit LF@%%retvar$1 string@string\n"
            "DEFVAR LF@%%param2\n"
            "MOVE LF@%%param2 LF@%%2\n"
            "TYPE LF@%%retvar$1 LF@%%param2\n"
            "JUMPIFNEQ ord&exit LF@%%retvar$1 string@int\n"
            // "POPS LF@%%param1\n" // empty pop
            // "POPS LF@%%param1\n"
            // "TYPE LF@%%retvar$1 LF@%%param1\n"
            // "JUMPIFNEQ ord&exit LF@%%retvar$1 string@string\n"
            // "DEFVAR LF@%%param2\n"
            // "POPS LF@%%param2\n"
            // "TYPE LF@%%retvar$1 LF@%%param2\n"
            // "JUMPIFNEQ ord&exit LF@%%retvar$1 string@int\n"
            "MOVE LF@%%retvar$1 nil@nil\n"
            "DEFVAR LF@%%cond_length\n"
            "LT LF@%%cond_length LF@%%param2 int@0\n"
            "JUMPIFEQ ord&return LF@%%cond_length bool@true\n"
            "STRLEN LF@%%cond_length LF@%%param1\n"
            "SUB LF@%%cond_length LF@%%cond_length int@1\n"
            "GT LF@%%cond_length LF@%%param2 LF@%%cond_length\n"
            "JUMPIFEQ ord&return LF@%%cond_length bool@true\n"
            "STRI2INT LF@%%retvar$1 LF@%%param1 LF@%%param2\n"
            "LABEL ord&return\n"
            "POPFRAME\n"
            "RETURN\n"
            "LABEL ord&exit\n"
            "EXIT int@8\n\n");

    /// print chr
    fprintf(stdout,"LABEL chr\n"
            "PUSHFRAME\n"
            "DEFVAR LF@%%cond\n"
            "DEFVAR LF@%%param1\n"
            "MOVE LF@%%param1 LF@%%1\n"
            "TYPE LF@%%cond LF@%%param1\n"
            "JUMPIFNEQ chr&exit LF@%%cond string@int\n"
            // "POPS LF@%%param1\n" // empty pop
            // "POPS LF@%%param1\n"
            // "TYPE LF@%%cond LF@%%param1\n"
            // "JUMPIFNEQ chr&exit LF@%%cond string@int\n"
            "DEFVAR LF@%%retvar$1\n"
            "MOVE LF@%%retvar$1 nil@nil\n"
            "LT LF@%%cond LF@%%param1 int@0\n"
            "JUMPIFEQ chr&return LF@%%cond bool@true\n"
            "GT LF@%%cond LF@%%param1 int@255\n"
            "JUMPIFEQ chr&return LF@%%cond bool@true\n"
            "INT2CHAR LF@%%retvar$1 LF@%%param1\n"
            "LABEL chr&return\n"
            "POPFRAME\n"
            "RETURN\n"
            "LABEL char&exit\n"
            "EXIT int@8\n\n");
}

/* generator.c */
