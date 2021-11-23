/**
 * @file generator.c
 * @brief Definice retezcovych funkci
 *
 * @authors Hung Do            (xdohun00) -
 *          David Kedra        (xkedra00)
 *          Petr Kolarik       (xkolar79)
 *          Jaroslav Kvasnicka (xkvasn14) -
 *
 * Reseni projektu IFJ a IAL, FIT VUT 21/22
 */

#include "generator.h"
#include <stdio.h>
#include <stdlib.h>

int gen_code(queue_t *q,enum ins_type type, char* dest, char* first_op, char* second_op)
{
    code_t item = (code_t)malloc(sizeof (struct code));
    if(item == NULL)
        return 60;
    item.instruction = type;
    item.dest = dest;
    item.first_op = first_op;
    item.second_op = second_op;
    queue_add(q,&item);
    return 0;
}
void queue_flush(queue_t *q)
{
    for(code_t item = q->first; item != NULL; item = item.next)
    {
        switch(item.instruction)
        {
            case INS_MOVE:
                fprintf(stdout,"MOVE %s %s\n",item.dest,item.first_op);
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
                fprintf(stdout,"DEFVAR %s\n",item.dest);
                break;
            case INS_CALL:
                fprintf(stdout,"CALL %s\n",item.dest);
                break;
            case INS_RETURN:
                fprintf(stdout,"RETURN\n");
                break;
            case INS_PUSHS:
                fprintf(stdout,"PUSHS %s\n",item.dest);
                break;
            case INS_POPS:
                fprintf(stdout,"POPS %s\n",item.dest);
                break;
            case INS_CLEARS:
                fprintf(stdout,"CLEARS\n");
                break;
            case INS_ADD:
                fprintf(stdout,"ADD %s %s %s\n",item.dest,item.first_op,item.second_op);
                break;
            case INS_SUB:
                fprintf(stdout,"SUB %s %s %s\n",item.dest,item.first_op,item.second_op);
                break;
            case INS_MUL:
                fprintf(stdout,"MUL %s %s %s\n",item.dest,item.first_op,item.second_op);
                break;
            case INS_DIV:
                fprintf(stdout,"DIV %s %s %s\n",item.dest,item.first_op,item.second_op);
                break;
            case INS_IDIV:
                fprintf(stdout,"IDIV %s %s %s\n",item.dest,item.first_op,item.second_op);
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
                fprintf(stdout,"LT %s %s %s\n",item.dest,item.first_op,item.second_op);
                break;
            case INS_GT:
                fprintf(stdout,"GT %s %s %s\n",item.dest,item.first_op,item.second_op);
                break;
            case INS_EQ:
                fprintf(stdout,"EQ %s %s %s\n",item.dest,item.first_op,item.second_op);
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
                fprintf(stdout,"AND %s %s %s\n",item.dest,item.first_op,item.second_op);
                break;
            case INS_OR:
                fprintf(stdout,"OR %s %s %s\n",item.dest,item.first_op,item.second_op);
                break;
            case INS_NOT:
                fprintf(stdout,"NOT %s %s\n",item.dest,item.first_op);
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
                fprintf(stdout,"INT2FLOAT %s %s\n",item.dest,item.first_op);
                break;
            case INS_FLOAT2INT:
                fprintf(stdout,"FLOAT2INT %s %s\n",item.dest,item.first_op);
                break;
            case INS_INT2CHAR:
                fprintf(stdout,"INT2CHAR %s %s\n",item.dest,item.first_op);
                break;
            case INS_STRI2INT:
                fprintf(stdout,"STRI2INT %s %s %s\n",item.dest,item.first_op,item.second_op);
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
                fprintf(stdout,"READ %s %s\n",item.dest,item.first_op);
                break;
            case INS_WRITE:
                fprintf(stdout,"WRITE %s\n",item.dest);
                break;
            case INS_CONCAT:
                fprintf(stdout,"CONCAT %s %s %s\n",item.dest,item.first_op,item.second_op);
                break;
            case INS_STRLEN:
                fprintf(stdout,"STRLEN %s %s\n",item.dest,item.first_op);
                break;
            case INS_GETCHAR:
                fprintf(stdout,"GETCHAR %s %s %s\n",item.dest,item.first_op,item.second_op);
                break;
            case INS_SETCHAR:
                fprintf(stdout,"SETCHAR %s %s %s\n",item.dest,item.first_op,item.second_op);
                break;
            case INS_TYPE:
                fprintf(stdout,"TYPE %s %s\n",item.dest,item.first_op);
                break;
            case INS_LABEL:
                fprintf(stdout,"LABEL %s\n",item.dest);
                break;
            case INS_JUMP:
                fprintf(stdout,"JUMP %s\n",item.dest);
                break;
            case INS_JUMPIFEQ:
                fprintf(stdout,"JUMPIFEQ %s %s %s\n",item.dest,item.first_op,item.second_op);
                break;
            case INS_JUMPIFNEQ:
                fprintf(stdout,"JUMPIFNEQ %s %s %s\n",item.dest,item.first_op,item.second_op);
                break;
            case INS_JUMPIFEQS:
                fprintf(stdout,"JUMPIFEQS %s\n",item.dest);
                break;
            case INS_JUMPIFNEQS:
                fprintf(stdout,"JUMPIFNEQS %s\n",item.dest);
                break;
            case INS_EXIT:
                fprintf(stdout,"EXIT %s\n",item.dest);
                break;
            case INS_BREAK:
                fprintf(stdout,"BREAK\n");
                break;
            case INS_DPRINT:
                fprintf(stdout,"DPRINT %s\n",item.dest);
                break;
            default:
                // chyba
                break;
        }
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
        code_t tmp = q->first;
        q->first = q->first->next;
        free(tmp);
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

void import_builtin_functions()
{
    /// print reads
    fprintf(stdout,"LABEL reads\n"
                   "PUSHFRAME\n"
                   "DEFVAR LF@%%retval\n"
                   "READ LF@%%retval string\n"
                   "POPFRAME\n"
                   "RETURN\n");

    /// print readi
    fprintf(stdout,"LABEL readi\n"
                   "PUSHFRAME\n"
                   "DEFVAR LF@%%retval\n"
                   "READ LF@%%retval int\n"
                   "POPFRAME\n"
                   "RETURN\n");

    /// print readn
    fprintf(stdout,"LABEL readn\n"
                   "PUSHFRAME\n"
                   "DEFVAR LF@%%retval\n"
                   "READ LF@%%retval number\n"
                   "POPFRAME\n"
                   "RETURN\n");

    /// print write
    /// print tointeger
    /// print substr
    /// print ord
    /// print chr

}