#include "token.h"
#include "expression.h"
#include "error.h"
#include "symtable.h"
#include "parser.h"
#include <stdlib.h>

static int get_index_from_token(enum Token_type type)
{
    switch(type)
    {
        case TYPE_LEFT_PARENTHESES:  return TI_LEFT_PARENTHESES;
        case TYPE_RIGHT_PARENTHESES: return TI_RIGHT_PARENTHESES;
        case TYPE_POWER:             return TI_POWER;
        case TYPE_STRLEN:            return TI_STRLEN;
        case TYPE_KW_NOT:            return TI_NOT;
        case TYPE_MULTIPLY:
        case TYPE_DIVIDE:
        case TYPE_DIVIDE_WHOLE:
        case TYPE_DIVIDE_MODULO:     return TI_MULTIPLY;
        case TYPE_PLUS:
        case TYPE_MINUS:             return TI_PLUS;
        case TYPE_CONCAT:            return TI_CONCAT;
        case TYPE_LESSER:
        case TYPE_LESSER_OR_EQ:
        case TYPE_GREATER:
        case TYPE_GREATER_OR_EQ:     return TI_LOGIC;
        case TYPE_EQ:
        case TYPE_KW_AND:            return TI_AND;
        case TYPE_KW_OR:             return TI_OR;
        case TYPE_COMMA:             return TI_COMMA;
        case TYPE_IDENTIFIER:
        case TYPE_NUMBER:
        case TYPE_INTEGER: 
        case TYPE_STRING:  
        case TYPE_BOOLEAN:
        case TYPE_KW_NIL:            return TI_CONST;
        default:                     return TI_DOLLAR;
    }
    return -1;
}

static int get_index_from_item(const exp_item_t item)
{
    switch(item.type)
    {
        case SYM_TOKEN:
            return get_index_from_token(item.data.oper);
        case SYM_DOLLAR:
            return TI_DOLLAR;
        case SYM_TERM:
            return TI_CONST;
        default:
            return -1;
    }
}

static enum prec_type table[PREC_TABLE_SIZE][PREC_TABLE_SIZE] = {
//   (  )  ^  #  !  *  +  .. >  &  |  ,  c  $
    {S, E, S, S, S, S, S, S, S, S, S, E, S, U}, // (
    {U, R, R, U, U, R, R, R, R, R, R, R, U, U}, // )
    {S, R, S, S, S, R, R, R, R, R, R, R, S, R}, // ^
    {S, R, S, U, S, R, R, R, R, R, R, R, S, R}, // #
    {S, R, S, S, S, R, R, R, R, R, R, R, S, R}, // not
    {S, R, S, S, S, R, R, R, R, R, R, R, S, R}, // * / // %
    {S, R, S, S, S, S, R, R, R, R, R, R, S, R}, // + - 
    {S, R, S, S, S, S, S, S, R, R, R, R, S, R}, // ..
    {S, R, S, S, S, S, S, S, R, R, R, R, S, R}, // > >= < <= == ~=
    {S, R, S, S, S, S, S, S, S, S, R, R, S, R}, // and
    {S, R, S, S, S, S, S, S, S, S, R, R, S, R}, // or
    {S, E, S, S, S, S, S, S, S, S, S, E, S, U}, // ,
    {U, R, R, U, U, R, R, R, R, R, R, R, U, R}, // const
    {S, U, S, S, S, S, S, S, S, S, S, U, S, U}  // $
};

int exp_stack_reduce(exp_stack_t *s)
{
    if (!s)
        return ERR_INTERNAL;

    int res = NO_ERR;
    int op_num = 0;         // pocet polozek v zasobniku pred '<' shiftem
    int i = s->len - 1;     // index posledni polozky v zasobniku

    // dokud nenalezneme '<'
    while (i >= 0 && (s->array)[i].type != SYM_SHIFT)
    {
        i--;
        op_num++;
        // nenalezen znak shiftu
        if(i < 0) return ERR_SYNTAX;
    }

    // inicializace noveho prvku
    exp_nterm_t *new_nterm = NULL;
    if ((res = exp_nterm_init(&new_nterm)) != NO_ERR)   return res;

    new_nterm->id = reduc_id++;

    exp_item_t *operand1 = NULL;
    exp_item_t *oper = NULL;
    exp_item_t *operand2 = NULL;

    if(op_num == 1)
    {
        // ziska ten jeden operand
        operand1 = exp_stack_pop(s);
        if (operand1 == NULL)
            CLEAR_UP_IN_REDUCE(ERR_INTERNAL);
        if (operand1->type != SYM_TERM)  
            CLEAR_UP_IN_REDUCE(ERR_SYNTAX);

        // pro termy:
        switch (operand1->data.term.type)
        {
            case DATA_ID:
                {
                    node_ptr node = NULL;
                    new_nterm->rule = RULE_ID;
                    SEARCH_SYMBOL(operand1->data.term.value.id, VAR, node);
                    if (node == NULL)
                        CLEAR_UP_IN_REDUCE(ERR_SEM_DEF);
                    new_nterm->data_t = node->var_type;     // prirazeni podle hodnoty v TS
                }
                break;
            case DATA_STR:
                new_nterm->rule = RULE_STR;
                new_nterm->data_t = DATA_STR;
                break;
            case DATA_INT:
                new_nterm->rule = RULE_INT;
                new_nterm->data_t = DATA_INT;
                break;
            case DATA_NUM:
                new_nterm->rule = RULE_NUM;
                new_nterm->data_t = DATA_NUM;
                break;
            case DATA_BOOL:
                new_nterm->rule = RULE_BOOL;
                new_nterm->data_t = DATA_BOOL;
                break;
            case DATA_NIL: 
                new_nterm->rule = RULE_NIL;
                new_nterm->data_t = DATA_NIL;
                break;
            
            default:
                break;
        }
        exp_data_copy(&new_nterm->val1, &operand1->data.term);

        // uvolni 
        exp_stack_destroy_item(operand1);
    }
    else if(op_num == 2)
    {
        operand1 = exp_stack_pop(s);
        oper = exp_stack_pop(s);
        // kontrola poradi
        if (oper == NULL || operand1 == NULL)
            CLEAR_UP_IN_REDUCE(ERR_INTERNAL);
        if (oper->type != SYM_TOKEN && operand1->type != SYM_EXPR)
            CLEAR_UP_IN_REDUCE(ERR_SYNTAX);

        // semanticka kontrola a prirazeni pravidla
        switch (oper->data.oper)
        {
            case TYPE_STRLEN:   // # 
                // operand musi byt typu string
                if (operand1->data.nterm->data_t == DATA_NIL)
                    CLEAR_UP_IN_REDUCE(ERR_UNEXP_VAL);
                else if (operand1->data.nterm->data_t != DATA_STR)
                    CLEAR_UP_IN_REDUCE(ERR_SEM_TYPE);

                new_nterm->rule = RULE_STRLEN;
                new_nterm->data_t = DATA_INT;
                break;
            case TYPE_KW_NOT:   // not
                new_nterm->rule = RULE_NOT;
                new_nterm->data_t = DATA_BOOL;
                break;
            
            default:
                break;
        }

        new_nterm->val1.type = DATA_SUB_EXP;
        new_nterm->val1.value.sub_expr = operand1->data.nterm;

        // TODO: nejake cisteni?? memory leak??
        exp_stack_destroy_item(oper);
    }
    else if(op_num == 3)
    {
        operand2 = exp_stack_pop(s);
        oper = exp_stack_pop(s);
        operand1 = exp_stack_pop(s);
        if (operand1 == NULL || oper == NULL || operand2 == NULL)
            CLEAR_UP_IN_REDUCE(ERR_INTERNAL);

        // odlisne pravidlo ( E )
        if((operand1->type == SYM_TOKEN && operand2->type == SYM_TOKEN && oper->type == SYM_EXPR) && 
            (operand1->data.oper == TYPE_LEFT_PARENTHESES && operand2->data.oper == TYPE_RIGHT_PARENTHESES))
        {
            free(new_nterm);
            new_nterm = oper->data.nterm;
            exp_stack_destroy_item(operand2);   // odstraneni )
            exp_stack_destroy_item(operand1);   // odstraneni (
            // odstraneni '<'
            exp_stack_pop(s);
            return exp_stack_push(s, SYM_EXPR, NULL, NULL, new_nterm);
        }
        else if(operand1->type != SYM_EXPR || oper->type != SYM_TOKEN || operand2->type != SYM_EXPR)
            CLEAR_UP_IN_REDUCE(ERR_SYNTAX);

        // zbyle pravidla E op E
        switch (oper->data.oper)
        {
            case TYPE_POWER:
                new_nterm->rule = RULE_POWER;
                SEMANTIC_CHECK_INT_AND_NUM(new_nterm, operand1->data.nterm, operand2->data.nterm, 
                        new_nterm->val1_to_num, new_nterm->val2_to_num);
                // TODO: converting to real ??
                // TODO: generating code??
                break;
            case TYPE_CONCAT:
                new_nterm->rule = RULE_CONCAT;
                SEMANTIC_CHECK_STR(new_nterm, operand1->data.nterm, operand2->data.nterm);
                // TODO: converting to real ??
                // TODO: generating code ??
                break;
            case TYPE_PLUS:
                new_nterm->rule = RULE_PLUS;
                SEMANTIC_CHECK_INT_AND_NUM(new_nterm, operand1->data.nterm, operand2->data.nterm, 
                        new_nterm->val1_to_num, new_nterm->val2_to_num);
                // TODO: converting to real ??
                // TODO: generating code??
                break;
            case TYPE_MINUS:
                new_nterm->rule = RULE_MINUS;
                SEMANTIC_CHECK_INT_AND_NUM(new_nterm, operand1->data.nterm, operand2->data.nterm, 
                        new_nterm->val1_to_num, new_nterm->val2_to_num);
                // TODO: converting to real ??
                // TODO: generating code??
                break;
            case TYPE_MULTIPLY:
                new_nterm->rule = RULE_MULTIPLY;
                SEMANTIC_CHECK_INT_AND_NUM(new_nterm, operand1->data.nterm, operand2->data.nterm, 
                        new_nterm->val1_to_num, new_nterm->val2_to_num);
                // TODO: converting to real ??
                // TODO: generating code??
                break;
            case TYPE_DIVIDE:
                new_nterm->rule = RULE_DIVIDE;
                SEMANTIC_CHECK_INT_AND_NUM(new_nterm, operand1->data.nterm, operand2->data.nterm, 
                        new_nterm->val1_to_num, new_nterm->val2_to_num);
                // check for div 0
                if (operand2->data.nterm->rule == RULE_INT)
                    CHECK_DIV_ZERO(operand2->data.nterm->val1);
                // TODO: converting to real ??
                // TODO: generating code??
                break;
            case TYPE_DIVIDE_WHOLE:
                new_nterm->rule = RULE_DIVIDE_WHOLE;
                SEMANTIC_CHECK_INT_AND_NUM(new_nterm, operand1->data.nterm, operand2->data.nterm, 
                        new_nterm->val1_to_num, new_nterm->val2_to_num);
                // check for div 0
                if (operand2->data.nterm->rule == RULE_INT)
                    CHECK_DIV_ZERO(operand2->data.nterm->val1);
                // TODO: converting to real ??
                // TODO: generating code??
                break;
            case TYPE_DIVIDE_MODULO:
                new_nterm->rule = RULE_MODULO;
                SEMANTIC_CHECK_INT_AND_NUM(new_nterm, operand1->data.nterm, operand2->data.nterm, 
                        new_nterm->val1_to_num, new_nterm->val2_to_num);
                // check for div 0
                if (operand2->data.nterm->rule == RULE_INT)
                    CHECK_DIV_ZERO(operand2->data.nterm->val1);
                // TODO: converting to real ??
                // TODO: generating code??
                break;
            case TYPE_EQ:
                new_nterm->rule = RULE_EQ;
                SEMANTIC_CHECK_BOOL_NIL(new_nterm, operand1->data.nterm, operand2->data.nterm);
                // TODO: generating code ??
                break;
            case TYPE_NOT_EQ:
                new_nterm->rule = RULE_NE;
                SEMANTIC_CHECK_BOOL_NIL(new_nterm, operand1->data.nterm, operand2->data.nterm);
                // TODO: generating code ??
                break;
            case TYPE_GREATER_OR_EQ:
                new_nterm->rule = RULE_GE;
                SEMANTIC_CHECK_BOOL_NON_NIL(new_nterm, operand1->data.nterm, operand2->data.nterm);
                // TODO: generating code ??
                break;
            case TYPE_GREATER:
                new_nterm->rule = RULE_GT;
                SEMANTIC_CHECK_BOOL_NON_NIL(new_nterm, operand1->data.nterm, operand2->data.nterm);
                // TODO: generating code ??
                break;
            case TYPE_LESSER_OR_EQ:
                new_nterm->rule = RULE_LE;
                SEMANTIC_CHECK_BOOL_NON_NIL(new_nterm, operand1->data.nterm, operand2->data.nterm);
                // TODO: generating code ??
                break;
            case TYPE_LESSER:
                new_nterm->rule = RULE_LT;
                SEMANTIC_CHECK_BOOL_NON_NIL(new_nterm, operand1->data.nterm, operand2->data.nterm);
                // TODO: generating code ??
                break;
            case TYPE_KW_AND:
                new_nterm->rule = RULE_AND;
                SEMANTIC_CHECK_BOOL_NON_NIL(new_nterm, operand1->data.nterm, operand2->data.nterm);
                // TODO: generating code ??
                break;
            case TYPE_KW_OR:
                new_nterm->rule = RULE_OR;
                SEMANTIC_CHECK_BOOL_NON_NIL(new_nterm, operand1->data.nterm, operand2->data.nterm);
                // TODO: generating code ??
                break;

            default:
                return ERR_SYNTAX;
                break;
        }
        new_nterm->val1.type = DATA_SUB_EXP;
        new_nterm->val2.type = DATA_SUB_EXP;

        new_nterm->val1.value.sub_expr = operand1->data.nterm;
        new_nterm->val2.value.sub_expr = operand2->data.nterm;
        exp_stack_destroy_item(oper);
    }
    else
        return ERR_SYNTAX;


    // 3 operandy je pripadne potreba smazat
    // if (op1->type == SYM_EXPR) exp_nterm_destroy(&op1->data.nterm);
    // else if (op1->type == SYM_TERM) exp_data_destroy(&op1->data.term);
    // if (op2->type == SYM_EXPR) exp_nterm_destroy(&op2->data.nterm);
    // else if (op1->type == SYM_TERM) exp_data_destroy(&op2->data.term);
    // if (op2->type == SYM_EXPR) exp_nterm_destroy(&op2->data.nterm);
    // else if (op1->type == SYM_TERM) exp_data_destroy(&op2->data.term);
    
    // popnuti '<'
    exp_stack_pop(s);
    return exp_stack_push(s, SYM_EXPR, NULL, NULL, new_nterm);
}

int exp_stack_shift(exp_stack_t *s, token_t *token)
{
    if (!s)
        return ERR_INTERNAL;

    //zasobnik pro odlozene neterminaly
    struct exp_stack temp_s; 
    int res = NO_ERR;
    if ((res = exp_stack_init(&temp_s)))     return res;
    // vrchni polozka
    struct exp_stack_item *top = exp_stack_top(s);

    // prohledani prvniho termu v zasobniku
    while (top != NULL && top->type == SYM_EXPR)
    {
        exp_stack_pop(s);
        if ((res = exp_stack_push_item(&temp_s, top)) != NO_ERR)
        {
            exp_stack_destroy(&temp_s);
            return res;
        }
        top = exp_stack_top(s);
    }
    // top je ted nejvrchnejsi nalezeny terminal
    if (top == NULL)    // nenalezl zadny term -> chyba (musi tam byt alespon $)
    {
        exp_stack_destroy(&temp_s);
        return ERR_INTERNAL;
    }

    // vlozeni '<' znaku za top terminal
    if ((res = exp_stack_push(s, SYM_SHIFT, NULL, NULL, NULL)) != NO_ERR)
    {
        exp_stack_destroy(&temp_s);
        return res;
    }

    // navrat vsech odlozenych polozek
    while (!exp_stack_isempty(&temp_s))
    {
        if ((res = exp_stack_push_item(s, exp_stack_pop(&temp_s))) != NO_ERR)
        {
            exp_stack_destroy(&temp_s);
            return res;
        }
    }
    exp_stack_destroy(&temp_s);

    // pridani noveho tokenu ze vstupu
    PUSH_DATA_TO_EXP_STACK((*s), token, res);
    return res;
}

// promenna pro urceni id docasnych promennych
unsigned reduc_id = 0;
int expression(token_t **token, enum data_type *data_t, exp_nterm_t **final_exp)
{
    reduc_id = 0;
    int res = NO_ERR;
    if (data_t == NULL || final_exp == NULL)    return ERR_INTERNAL;
    *data_t = DATA_NIL;
    struct exp_stack s;
    int loading = 1;
    if ((res = exp_stack_init(&s)))    return res;
    if ((res = exp_stack_push(&s, SYM_DOLLAR, NULL, NULL, NULL)))   return res;

    do
    {
        // TODO:
        exp_item_t *top_item = exp_stack_top_term(&s);
        //ziskani patricne operace shift/redukce/.. z tabulky
        // enum prec_type  index = get_index_from_item(*top_item);

        // ziska symbol operace z tabulky
        int row = get_index_from_item(*top_item);
        int col = get_index_from_token((*token)->type);
        switch(table[row][col])
        {
            case E: 
                {
                    PUSH_DATA_TO_EXP_STACK(s, *token, res);
                    if (res != NO_ERR)
                        loading = 0;
                    // {
                    //     exp_stack_destroy(&s);
                    //     *final_exp = NULL;
                    //     return res;
                    // }
                }
                LOAD_TOKEN(token);
                break;
            case S:
                if ((res = exp_stack_shift(&s, *token)) != NO_ERR)
                    loading = 0;
                // {
                //     exp_stack_destroy(&s);
                //     *final_exp = NULL;
                //     return res;
                // }
                LOAD_TOKEN(token);
                break;
            case R:
                if ((res = exp_stack_reduce(&s)) != NO_ERR)
                    loading=0;
                // {
                //     exp_stack_destroy(&s);
                //     *final_exp = NULL;
                //     return res;
                // }
                // LOAD_TOKEN(token);
                break;
            case U:
                // TODO: error handling
                loading = 0;
                break;
        }
    } while (loading);
    if (res == ERR_INTERNAL)
    {
        exp_stack_destroy(&s);
        *final_exp = NULL;
        return res;
    }
    if (exp_stack_top_term(&s)->type != SYM_DOLLAR)
    {
        exp_stack_destroy(&s);
        *final_exp = NULL;
        return ERR_SYNTAX;
    }

    // TODO: dalsi veci
    exp_item_t *final_item = exp_stack_pop(&s);
    if (final_item == NULL)
    {   /* TODO: goto cleaning and error handling */ }
    else        // zisk nejakych dat 
    {
        if (final_item->type != SYM_EXPR)
        {   /* TODO: goto cleaning and error handling */ }

        if (data_t == NULL || final_exp == NULL)
        {   /* TODO: goto cleaning and error handling */ }
        else
        {
            *data_t = final_item->data.nterm->data_t;
            *final_exp = final_item->data.nterm;
        }
    }
    exp_stack_destroy(&s);
    return res;
}

static int push_to_gen_stack(exp_nterm_t *expr)
{
    switch(expr->rule)
    {
        // jeden operand
        case RULE_ID:
            printf("%s", expr->val1.value.id);
            // TODO: GEN_CODE(PUSHS, expr->val1.value.id, NULL, NULL)
            break;
        case RULE_BOOL:
            printf("%s", expr->val1.value.boolean ? "true" : "false");
            // TODO: GEN_CODE(PUSHS, expr->val1.value.boolean, NULL, NULL)
            break;
        case RULE_INT:
            printf("%d", expr->val1.value.integer);
            // TODO: GEN_CODE(PUSHS, expr->val1.value.integer, NULL, NULL)
            break;
        case RULE_NIL:
            printf("nil");
            // TODO: GEN_CODE(PUSHS, nil, NULL, NULL)
            break;
        case RULE_NUM:
            printf("%g", expr->val1.value.number);
            // TODO: GEN_CODE(PUSHS, expr->val1.value.number, NULL, NULL)
            break;
        case RULE_STR:
            printf("%s", expr->val1.value.string);
            // TODO: GEN_CODE(PUSHS, expr->val1.value.string, NULL, NULL)
            break;

            // jeden operand a operator
        case RULE_STRLEN:
            printf("(#");
            push_to_gen_stack(expr->val1.value.sub_expr);
            printf(")");
            // TODO: def vars temp_var1, temp_var2
            // TODO: GEN_CODE(POPS, string@temp_var1, NULL, NULL);
            // TODO: GEN_CODE(STRLEN, string@temp_var1, NULL, int@temp_var2);
            // TODO: GEN_CODE(PUSHS, int@temp_var2, NULL, NULL);
            break;
        case RULE_NOT:
            printf("(not");
            push_to_gen_stack(expr->val1.value.sub_expr);
            printf(")");
            // TODO: GEN_CODE(NOTS, NULL, NULL, NULL);
            break;
        case RULE_CONCAT:
            printf("(");
            push_to_gen_stack(expr->val2.value.sub_expr);
            printf("..");
            push_to_gen_stack(expr->val1.value.sub_expr);
            printf(")");
            // TODO: def vars temp_var1, temp_var2, temp_var3
            // TODO: GEN_CODE(POPS, string@temp_var1, NULL, NULL);
            // TODO: GEN_CODE(POPS, string@temp_var2, NULL, NULL);
            // TODO: GEN_CODE(CONCAT, string@temp_var1, string@temp_var2, string@temp_var3);
            // TODO: GEN_CODE(PUSHS, string@temp_var3, NULL, NULL);
            break;
        case RULE_PLUS:
            printf("(");
            push_to_gen_stack(expr->val2.value.sub_expr);
            printf("+");
            push_to_gen_stack(expr->val1.value.sub_expr);
            printf(")");
            // TODO: GEN_CODE(ADDS, NULL, NULL, NULL);
            break;
        case RULE_MINUS:
            printf("(");
            push_to_gen_stack(expr->val2.value.sub_expr);
            printf("-");
            push_to_gen_stack(expr->val1.value.sub_expr);
            printf(")");
            // TODO: GEN_CODE(SUBS, NULL, NULL, NULL);
            break;
        case RULE_MULTIPLY:
            printf("(");
            push_to_gen_stack(expr->val2.value.sub_expr);
            printf("*");
            push_to_gen_stack(expr->val1.value.sub_expr);
            printf(")");
            // TODO: GEN_CODE(MULS, NULL, NULL, NULL);
            break;
        case RULE_DIVIDE:
            printf("(");
            push_to_gen_stack(expr->val2.value.sub_expr);
            printf("/");
            push_to_gen_stack(expr->val1.value.sub_expr);
            printf(")");
            // TODO: GEN_CODE(DIVS, NULL, NULL, NULL);
            break;
        case RULE_DIVIDE_WHOLE:
            printf("(");
            push_to_gen_stack(expr->val2.value.sub_expr);
            printf("//");
            push_to_gen_stack(expr->val1.value.sub_expr);
            printf(")");
            // TODO: GEN_CODE(IDIVS, NULL, NULL, NULL);
            break;
        case RULE_EQ:
            printf("(");
            push_to_gen_stack(expr->val2.value.sub_expr);
            printf("==");
            push_to_gen_stack(expr->val1.value.sub_expr);
            printf(")");
            // TODO: GEN_CODE(EQS, NULL, NULL, NULL);
            break;
        case RULE_NE:
            printf("(");
            push_to_gen_stack(expr->val2.value.sub_expr);
            printf("~=");
            push_to_gen_stack(expr->val1.value.sub_expr);
            printf(")");
            // TODO: GEN_CODE(EQS, NULL, NULL, NULL);
            // TODO: GEN_CODE(NOTS, NULL, NULL, NULL);
            break;
        case RULE_GT:
            printf("(");
            push_to_gen_stack(expr->val2.value.sub_expr);
            printf(">");
            push_to_gen_stack(expr->val1.value.sub_expr);
            printf(")");
            // TODO: GEN_CODE(GTS, NULL, NULL, NULL);
            break;
        case RULE_GE:
            printf("(");
            push_to_gen_stack(expr->val2.value.sub_expr);
            printf(">=");
            push_to_gen_stack(expr->val1.value.sub_expr);
            printf(")");
            // TODO: dostat nejak z funkci datove typy?? exp->val->type
            //
            // TODO: def vars temp_var1, temp_var2, temp_var3
            // TODO: GEN_CODE(POPS, temp_var1, NULL, NULL);
            // TODO: GEN_CODE(POPS, temp_var2, NULL, NULL);
            //
            // TODO: GEN_CODE(PUSHS, temp_var2, NULL, NULL);
            // TODO: GEN_CODE(PUSHS, temp_var1, NULL, NULL);
            // TODO: GEN_CODE(GTS, NULL, NULL, NULL);
            //
            // TODO: GEN_CODE(PUSHS, temp_var2, NULL, NULL);
            // TODO: GEN_CODE(PUSHS, temp_var1, NULL, NULL);
            // TODO: GEN_CODE(EQS, NULL, NULL, NULL);
            //
            // TODO: GEN_CODE(ORS, NULL, NULL, NULL);
            break;
        case RULE_LT:
            printf("(");
            push_to_gen_stack(expr->val2.value.sub_expr);
            printf("<");
            push_to_gen_stack(expr->val1.value.sub_expr);
            printf(")");
            // TODO: GEN_CODE(LTS, NULL, NULL, NULL);
            break;
        case RULE_LE:
            printf("(");
            push_to_gen_stack(expr->val2.value.sub_expr);
            printf("<=");
            push_to_gen_stack(expr->val1.value.sub_expr);
            printf(")");
            // TODO: dostat nejak z funkci datove typy?? exp->val->type
            //
            // TODO: def vars temp_var1, temp_var2, temp_var3
            // TODO: GEN_CODE(POPS, temp_var1, NULL, NULL);
            // TODO: GEN_CODE(POPS, temp_var2, NULL, NULL);
            //
            // TODO: GEN_CODE(PUSHS, temp_var2, NULL, NULL);
            // TODO: GEN_CODE(PUSHS, temp_var1, NULL, NULL);
            // TODO: GEN_CODE(LTS, NULL, NULL, NULL);
            //
            // TODO: GEN_CODE(PUSHS, temp_var2, NULL, NULL);
            // TODO: GEN_CODE(PUSHS, temp_var1, NULL, NULL);
            // TODO: GEN_CODE(EQS, NULL, NULL, NULL);
            //
            // TODO: GEN_CODE(ORS, NULL, NULL, NULL);
            break;
        case RULE_AND:
            printf("(");
            push_to_gen_stack(expr->val2.value.sub_expr);
            printf("and");
            push_to_gen_stack(expr->val1.value.sub_expr);
            printf(")");
            // TODO: GEN_CODE(ANDS, NULL, NULL, NULL);
            break;
        case RULE_OR:
            printf("(");
            push_to_gen_stack(expr->val2.value.sub_expr);
            printf("or");
            push_to_gen_stack(expr->val1.value.sub_expr);
            printf(")");
            // TODO: GEN_CODE(ORS, NULL, NULL, NULL);
            break;
            // RULE_POWER,             // E -> E ^ E
            // RULE_MODULO,            // E -> E % E
        default:
            break;
    }
    return NO_ERR;
}

int generate_code_nterm(exp_nterm_t **expr)
{
    // TODO: GEN_CODE(CLEARS)
    if (expr != NULL && *expr != NULL)
    {
        printf("(");
        int res = push_to_gen_stack(*expr);
        printf(")\n");
        return res;
    }
    return ERR_INTERNAL;
}

/* expression.c */
