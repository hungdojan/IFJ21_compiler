#ifndef _EXP_STACK_H
#define _EXP_STACK_H

#include "token.h"
#include "symtable.h"
#include <stdbool.h>

#define EXP_STACK_SIZE 8
#define EXP_FUNC_PARM_SIZE 4

// Type symbol
enum exp_stack_symb
{
    SYM_EXPR,       /// E
    SYM_TERM,       /// operand
    SYM_SHIFT,      /// <
    SYM_DOLLAR,     /// $ 
    SYM_TOKEN       /// operator
};

enum exp_rules
{
    RULE_PARENTHESES,       // E -> ( E )
    RULE_STRLEN,            // E -> # E
    RULE_NOT,               // E -> not E
    RULE_POWER,             // E -> E ^ E
    RULE_CONCAT,            // E -> E..E
    RULE_PLUS,              // E -> E + E
    RULE_MINUS,             // E -> E - E
    RULE_MULTIPLY,          // E -> E * E
    RULE_DIVIDE,            // E -> E / E
    RULE_DIVIDE_WHOLE,      // E -> E // E
    RULE_MODULO,            // E -> E % E
    RULE_EQ,                // E -> E == E
    RULE_NE,                // E -> E ~= E
    RULE_GE,                // E -> E >= E
    RULE_GT,                // E -> E > E
    RULE_LE,                // E -> E <= E
    RULE_LT,                // E -> E < E
    RULE_AND,               // E -> E and E
    RULE_OR,                // E -> E or E
    RULE_ID,                // E -> id
    RULE_STR,               // E -> _str_
    RULE_INT,               // E -> _int_
    RULE_NUM,               // E -> _num_
    RULE_NIL,               // E -> _nil_
    RULE_BOOL,              // E -> _bool_
};

enum table_index
{
    TI_LEFT_PARENTHESES,
    TI_RIGHT_PARENTHESES,
    TI_POWER,
    TI_STRLEN,
    TI_NOT,
    TI_MULTIPLY,
    TI_PLUS,
    TI_CONCAT,
    TI_LOGIC,
    TI_AND,
    TI_OR,
    TI_COMMA,
    TI_CONST,
    TI_DOLLAR
};

struct exp_nterm;

// type jednotlivych operandu
typedef struct exp_item_value
{
    enum data_type type;
    union {
        struct exp_nterm *sub_expr;     /// dalsi podvyraz
        char    *id;                    /// ukazatel do TS
        char    *string;                /// hodnota retezce z tokenu
        int     integer;                /// celociselna hodnota z tokenu
        double  number;                 /// desetinna hodnota z tokenu
        bool    boolean;                /// pravdivostni hodnota z tokenu
    } value;
} exp_data_t;


// rozlozeni prikazu
// val1 OPERATION val2
// OPERATION val1
typedef struct exp_nterm
{
    enum data_type  data_t;      /// datovy typ
    enum exp_rules  rule;        /// pouzite pravidlo
    unsigned        id;          /// pomocne id pri generovani kodu
    exp_data_t      val1;        /// hodnota na leve strane
    bool            val1_to_num; /// pretypovani leveho operandu na number
    exp_data_t      val2;        /// hodnota na prave strane
    bool            val2_to_num; /// pretypovani praveho operandu na number
} exp_nterm_t;

// polozka v zasobniku
typedef struct exp_stack_item
{
    enum exp_stack_symb  type;
    union {
        enum Token_type  oper;    /// operator
        exp_nterm_t     *nterm;   /// neterm; slozeny podvyraz
        exp_data_t      term;    /// vyraz
    } data;
} exp_item_t;

// zasobnik samotny
typedef struct exp_stack
{
    exp_item_t *array;
    int alloc_size;
    int len;
} exp_stack_t;

enum prec_type
{
    S, // SHIFT <
    R, // REDUC >
    E, // EQUAL =
    U, // UNDEF _
};

int exp_nterm_init(exp_nterm_t **n);
void exp_nterm_destroy(exp_nterm_t **n);

int exp_data_init(exp_data_t *data, token_t *token);
void exp_data_copy(exp_data_t *dst, exp_data_t *src);
void exp_data_destroy(exp_data_t *data);

// inicializace zasobniku
int exp_stack_init(exp_stack_t *s);

// pridani prvku do zasobniku
int exp_stack_push(exp_stack_t *s, enum exp_stack_symb sym, token_t *token, exp_data_t *data, struct exp_nterm *nterm);

// pridani rovnou itemu do zasobniku
int exp_stack_push_item(exp_stack_t *s, struct exp_stack_item * item);

// vraci prvni polozku zpatky
struct exp_stack_item *exp_stack_top(const exp_stack_t *s);

// vraci prvni term zpatky
struct exp_stack_item *exp_stack_top_term(const exp_stack_t *s);

// vrati prvni vec na zasobniku
struct exp_stack_item *exp_stack_pop(exp_stack_t *s);

int exp_stack_isempty(const exp_stack_t *s);

void exp_stack_destroy_item(exp_item_t *item);

// odstraneni zasobniku a uvolneni alokovane pameti
void exp_stack_destroy(exp_stack_t *s);

#endif // _EXP_STACK_H
