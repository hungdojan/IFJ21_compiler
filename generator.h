/**
 * @file generator.h
 * @brief Deklarace struktur gen_info, code a jejich funkce
 *
 * @authors Jaroslav Kvasnicka (xkvasn14)
 *          Hung Do            (xdohun00)
 *          David Kedra        (xkedra00)
 *          Petr Kolarik       (xkolar79)
 *
 * Reseni projektu IFJ a IAL, FIT VUT 21/22
 */


#ifndef _GEN_CODE_H_
#define _GEN_CODE_H_
#include "symtable.h"
#include "exp_stack.h"

#define MAX_STR_LEN 200
#define GET_FRAME(x)\
    ((x) == FRAME_GF ? "GF" :\
     (x) == FRAME_LF ? "LF" : "TF")
#define GET_TYPE(x)\
    ((x) == DATA_BOOL ? "bool"   :\
     (x) == DATA_STR  ? "string" :\
     (x) == DATA_NUM  ? "float"  :\
     (x) == DATA_INT  ? "int"    : "nil")
#define CONCAT_TO_OPERAND(ot)\
    do\
    {\
        if (ot == OPERAND_DEST)         strncat(_dest  , _temp, MAX_STR_LEN);\
        else if (ot == OPERAND_FIRST)   strncat(_first , _temp, MAX_STR_LEN);\
        else                            strncat(_second, _temp, MAX_STR_LEN);\
    } while(0)
#define CLEAR_OPERAND(ot)\
    do\
    {\
        if (ot == OPERAND_DEST)         strcpy(_dest  , "");\
        else if (ot == OPERAND_FIRST)   strcpy(_first , "");\
        else                            strcpy(_second, "");\
    } while(0)

/// Cilovy operand; volny retezec pro konkatenaci
extern char _dest[MAX_STR_LEN];
/// Prvni operand; volny retezec pro konkatenaci
extern char _first[MAX_STR_LEN];
/// Druhy operand; volny retezec pro konkatenaci
extern char _second[MAX_STR_LEN];

enum frame_type
{
    FRAME_GF,   /// Globalni ramec
    FRAME_LF,   /// Lokalni ramec
    FRAME_TF,   /// Docasny ramec
};

enum operand_type
{
    OPERAND_DEST,       /// Cilovy operand
    OPERAND_FIRST,      /// Prvni operand
    OPERAND_SECOND,     /// Druhy operand
};

enum label_type
{
    LABEL_IF,           /// Zacatek prvniho vetveni
    LABEL_ENDIF,        /// Navesti, pokud je prvni vetveni nepravdive
    LABEL_ELSEIF,       /// Zacatek prostrednich vetveni
    LABEL_ENDELSEIF,    /// Navesti daneho vetveni, ktere bylo nepravdive
    LABEL_ELSE,         /// Navesti else
    LABEL_WHILE,        /// Zacatek cyklu
    LABEL_ENDWHILE,     /// Konec cyklu
    LABEL_END,          /// Konec celeho vetveni
    LABEL_FUNC          /// Navesti funkce
};

/**
 * @brief Inicializace globalni struktury gen_info_t
 */
void init_gen_info();

/**
 * @brief Vynulovani pocitadla
 *
 * @details Priprava struktury pro dalsi funkci
 */
void reset_gen_info(node_ptr func_node);

/**
 * @brief Vytvoreni retezce operandu promenne
 *
 * @param ft Typ ramce
 * @param ot Typ operandu
 * @param var_node Uzel promenne v TS
 * @return Chybovy kod
 */
int define_variable(enum frame_type ft, enum operand_type ot, node_ptr var_node);

/**
 * @brief Vytvoreni retezce konstanty
 *
 * @param ot Typ operandu
 * @param term Konstantni vyraz
 * @return Chybovy kod
 */
int define_constant(enum operand_type ot, exp_data_t term);

/**
 * @brief Vytvoreni labelu
 *
 * @param ot Typ operandu
 * @param lt Typ popisku
 * @return Chybovy kod
 */
int define_label(enum operand_type ot, enum label_type lt);

/// Globalni struktura pro tvorbu promennych v triadresnem souboru
extern gen_info_t glob_cnt;

// type instrukce
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

/// Struktura instrukce
typedef struct code
{
    enum ins_type instruction;  /// Typ instrukce
    char *dest;                 /// Cilovy operand
    char *first_op;             /// Prvni operand
    char *second_op;            /// Druhy operand
    struct code *next;          /// Ukazatel na dalsi prikaz v pripade zretezeni
} code_t;

/// Fronta instrukci
typedef struct queue
{
    code_t *first;              /// Ukazatel na prvni prvek ve fronte
    code_t *last;               /// Ukazatel na posledni prvek ve fronte
} queue_t;

extern queue_t q_identifier;

/**
 * @brief   Vygeneruje prikaz
 *
 * @param q Ukazatel na strukturu queue_t
 * @param type Type instrukce
 * @param dest Cilovy operand
 * @param first_op Prvni operand
 * @param second_op Druhy operand
 * @return Chybovy kod
 */
int gen_code(queue_t *q, enum ins_type type, char* dest, char* first_op, char* second_op);

/**
 * @brief Vypise vsechny ulozene instrukce z fronty na stdout
 *
 * @param q Ukazatel na strukturu queue_t
 */
void queue_flush(queue_t *q);

/**
 * @brief Inicializace fronty
 *
 * @param q Ukazatel na strukturu queue_t
 */
void queue_init(queue_t *q);

/**
 * @brief Zniceni fronty a dealokace clenu
 *
 * @param q Ukazatel na strukturu queue_t
 */
void queue_destroy(queue_t *q);

/**
 * @brief Pridani clena do fronty
 *
 * @param q Ukazatel na strukturu queue_t
 * @param item Pridavany clen
 */
void queue_add(queue_t *q, code_t *item);

/**
 * @brief Vypis kodu na stdout
 * 
 * @param item Struktura code
 */
void flush_item(code_t *item);

/**
 * @brief Vypis deklaraci promennych na vystup
 * 
 * @param q Ukazatel na strukturu queue_t
 */
void filter_defvar(queue_t *q);

/**
 * @brief Vlozeni vestavenych funkci
 */
void import_builtin_functions();

#endif // _GEN_CODE_H_
