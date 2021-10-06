//
// Created by xkvasn14 on 03.10.21.
//

#ifndef _TOKEN_H_
#define _TOKEN_H_

#include "istring.h"

enum Token_type {
    type_plus,
    type_minus,
    type_multiply,
    type_divide,
    type_divide_whole,
    type_divide_modulo,
    type_power,
    type_greater_or_eq,
    type_greater,
    type_lesser_or_eq,
    type_lesser,
    type_eq,
    type_not_eq,
    type_assign,
    type_strlen_identifier,
    type_strlen_string,
    type_string_len,
    type_left_parentheses,
    type_right_parentheses,
    type_declare,
    type_concat,
    type_string,
    type_identifier,
    type_integer,
    type_number,
    type_exponent_number,
    type_undefined
} type;


void token_create(Istring *s, enum Token_type type);

#endif //_TOKEN_H_