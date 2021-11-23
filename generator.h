/**
 * @file generator.h
 * @brief Deklarace struktury Istring a jeji funkce
 *
 * @authors Hung Do            (xdohun00)
 *          David Kedra        (xkedra00)
 *          Petr Kolarik       (xkolar79)
 *          Jaroslav Kvasnicka (xkvasn14)
 *
 * Reseni projektu IFJ a IAL, FIT VUT 21/22
 */


#ifndef _GEN_CODE_H_
#define _GEN_CODE_H_

enum ins_type
{
    INS_MOVE,
    INS_CREATEFRAME,
    INS_PUSHFRAME,
    INS_POPFRAME,
    INS_DEFVAR,
    INS_CALL,
    INS_RETURN,
    INS_PUSHS,
    INS_POPS,
    INS_CLEARS,
    INS_ADD,
    INS_SUB,
    INS_MUL,
    INS_DIV,
    INS_IDIV,
    INS_ADDS,
    INS_SUBS,
    INS_MULS,
    INS_DIVS,
    INS_IDIVS,
    INS_LT,
    INS_GT,
    INS_EQ,
    INS_LTS,
    INS_GTS,
    INS_EQS,
    INS_AND,
    INS_OR,
    INS_NOT,
    INS_ANDS,
    INS_ORS,
    INS_NOTS,
    INS_INT2FLOAT,
    INS_FLOAT2INT,
    INS_INT2CHAR,
    INS_STRI2INT,
    INS_INT2FLOATS,
    INS_FLOAT2INTS,
    INS_INT2CHARS,
    INS_STRI2INTS,
    INS_READ,
    INS_WRITE,
    INS_CONCAT,
    INS_STRLEN,
    INS_GETCHAR,
    INS_SETCHAR,
    INS_TYPE,
    INS_LABEL,
    INS_JUMP,
    INS_JUMPIFEQ,
    INS_JUMPIFNEQ,
    INS_JUMPIFEQS,
    INS_JUMPIFNEQS,
    INS_EXIT,
    INS_BREAK,
    INS_DPRINT
};

/// Instruction structure
typedef struct code
{
    enum ins_type instruction;
    char *dest;
    char *first_op;
    char *second_op;
    struct code *next;
} code_t;

/// Queue of instructions
typedef struct queue
{
    code_t *first;
    code_t *last;
} queue_t;

int gen_code(queue_t *q, enum ins_type type, char* dest, char* first_op, char* second_op);
void queue_flush(queue_t *q);
void queue_init(queue_t *q);
void queue_destroy(queue_t *q);
int queue_add(queue_t *q, code_t *item);

void import_builtin_functions();

#endif // _GEN_CODE_H_