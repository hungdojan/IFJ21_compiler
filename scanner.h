

enum State {
    STATE_NEW,
    STATE_INTEGER,
    STATE_STRLEN_STRING,
    STATE_STRLEN_IDENTIFIER,
    STATE_STRING,
    STATE_IDENTIFIER,
    STATE_NUMBER,
    STATE_EXPONENT,
    STATE_EXPONENT_CONTINUE,
    STATE_EXPONENT_FINISH,
    STATE_RETURN_OPERATOR,
    STATE_RETURN_STRING,
    STATE_RETURN_INTEGER,
    STATE_RETURN_NUMBER,
    STATE_RETURN_IDENTIFIER,
    STATE_RETURN_STRLEN_IDENTIFIER,
    STATE_RETURN_STRLEN_STRING
    // STATE_ERROR
    } state;


int get_token();
void get_rid_of_comments();