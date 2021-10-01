//
// Created by xkvasn14 on 03.10.21.
//
#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include "istring.h"

void token_create(Istring *s, enum Token_type type)
{}


/* create new token has to create token with 2 params
1 param: its type
2 param: its own value that has to be copied from Istring->value, cause the string will be reused later...
 */